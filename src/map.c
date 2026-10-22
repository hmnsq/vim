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

#अन्तर्भाव "vim.h"

/*
 * List used for abbreviations.
 */
स्थिर mapblock_T	*first_abbr = रिक्त; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
स्थिर mapblock_T	*(maphash[256]);
स्थिर पूर्णाङ्क		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
स्थिर पूर्णाङ्क		map_locked = 0;

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
get_maphash_list(पूर्णाङ्क state, पूर्णाङ्क c)
{
    प्रत्यागम maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(पूर्णाङ्क state, पूर्णाङ्क c)
{
    प्रत्यागम curbuf->b_maphash[MAP_HASH(state, c)];
}

    पूर्णाङ्क
is_maphash_valid(शून्य)
{
    प्रत्यागम maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    स्थिर शून्य
validate_maphash(शून्य)
{
    यदि (maphash_valid)
	प्रत्यागम;

    CLEAR_FIELD(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    स्थिर शून्य
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_कुंजियाँ);
    यदि (mp->m_alt != रिक्त)
	mp->m_alt->m_alt = रिक्त;
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
    स्थिर char_u *
map_mode_to_chars(पूर्णाङ्क mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    यदि ((mode & (MODE_जोड़ो | MODE_CMDLINE)) == (MODE_जोड़ो | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    अन्यथा यदि (mode & MODE_जोड़ो)
	ga_append(&mapmode, 'i');			// :imap
    अन्यथा यदि (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    अन्यथा यदि (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    अन्यथा यदि ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    अन्यथा
    {
	यदि (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	यदि (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	यदि (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	यदि ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	अन्यथा
	{
	    यदि (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    यदि (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    प्रत्यागम (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    स्थिर शून्य
showmap(
    mapblock_T	*mp,
    पूर्णाङ्क		local)	    // TRUE for buffer-local map
{
    पूर्णाङ्क		len = 1;
    char_u	*mapchars;

    यदि (message_filtered(mp->m_कुंजियाँ) && message_filtered(mp->m_str))
	प्रत्यागम;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    यदि (संदेश_didout || संदेश_silent != 0)
    {
	संदेश_putchar('\n');
	यदि (got_int)	    // 'q' typed at MORE prompt
	    गच्छ theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    यदि (mapchars != रिक्त)
    {
	संदेश_puts((वर्ण *)mapchars);
	len = (पूर्णाङ्क)STRLEN(mapchars);
	vim_free(mapchars);
    }

    यावत् (++len <= 3)
	संदेश_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = संदेश_outtrans_special(mp->m_कुंजियाँ, TRUE, 0);
    कुरु
    {
	संदेश_putchar(' ');		// pad with blanks
	++len;
    } यावत् (len < 12);

    यदि (mp->m_noremap == REMAP_NONE)
	संदेश_puts_attr("*", HL_ATTR(HLF_8));
    अन्यथा यदि (mp->m_noremap == REMAP_SCRIPT)
	संदेश_puts_attr("&", HL_ATTR(HLF_8));
    अन्यथा
	संदेश_putchar(' ');

    यदि (local)
	संदेश_putchar('@');
    अन्यथा
	संदेश_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    यदि (*mp->m_str == NUL)
	संदेश_puts_attr("<Nop>", HL_ATTR(HLF_8));
    अन्यथा
	संदेश_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    यदि (p_verbose > 0)
	last_set_संदेश(mp->m_script_ctx);
#endif
    संदेश_clr_eos();
    out_flush();			// show one line at a time

theend:
    --map_locked;
}

    स्थिर mapblock_T *
map_add(
	mapblock_T  **map_table,
	mapblock_T  **abbr_table,
	char_u	    *कुंजियाँ,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	पूर्णाङ्क	    noremap,
	पूर्णाङ्क	    nowait,
	पूर्णाङ्क	    silent,
	पूर्णाङ्क	    mode,
	पूर्णाङ्क	    is_abbr,
#ifdef FEAT_EVAL
	पूर्णाङ्क	    व्यंजक,
	scid_T	    sid,	    // 0 to use current_sctx
	पूर्णाङ्क	    scriptversion,
	linenr_T    lnum,
#endif
	पूर्णाङ्क	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    यदि (mp == रिक्त)
	प्रत्यागम रिक्त;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    यदि (*कुंजियाँ == Ctrl_C)
    {
	यदि (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	अन्यथा
	    mapped_ctrl_c |= mode;
    }

    mp->m_कुंजियाँ = vim_strsave(कुंजियाँ);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    यदि (mp->m_कुंजियाँ == रिक्त || mp->m_str == रिक्त)
    {
	vim_free(mp->m_कुंजियाँ);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	प्रत्यागम रिक्त;
    }
    mp->m_keylen = (पूर्णाङ्क)STRLEN(mp->m_कुंजियाँ);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_व्यंजक = व्यंजक;
    यदि (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    अन्यथा
    {
	mp->m_script_ctx = current_sctx;
	यदि (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    यदि (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    अन्यथा
    {
	पूर्णाङ्क n = MAP_HASH(mp->m_mode, mp->m_कुंजियाँ[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    प्रत्यागम mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    स्थिर शून्य
list_mappings(
	पूर्णाङ्क	keyround,
	पूर्णाङ्क	abbrev,
	पूर्णाङ्क	haskey,
	char_u	*कुंजियाँ,
	पूर्णाङ्क	कुंजियाँ_len,
	पूर्णाङ्क	mode,
	पूर्णाङ्क	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    यदि (p_verbose > 0 && keyround == 1)
    {
	यदि (seen_Modify_Other_कुंजियाँ)
	    संदेश_puts(_("Seen modifyOtherKeys: true\n"));

	यदि (modify_otherkeys_state != MOKS_INITIAL)
	{
	    वर्ण *name = _("Unknown");
	    विकल्प (modify_otherkeys_state)
	    {
		पक्ष MOKS_INITIAL: भङ्ग;
		पक्ष MOKS_OFF: name = _("Off"); भङ्ग;
		पक्ष MOKS_ENABLED: name = _("On"); भङ्ग;
		पक्ष MOKS_DISABLED: name = _("Disabled"); भङ्ग;
		पक्ष MOKS_AFTER_T_TE: name = _("Cleared"); भङ्ग;
	    }

	    वर्ण buf[200];
	    vim_snprintf(buf, परिमाण(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    संदेश_puts(buf);
	}

	यदि (kitty_protocol_state != KKPS_INITIAL)
	{
	    वर्ण *name = _("Unknown");
	    विकल्प (kitty_protocol_state)
	    {
		पक्ष KKPS_INITIAL: भङ्ग;
		पक्ष KKPS_OFF: name = _("Off"); भङ्ग;
		पक्ष KKPS_ENABLED: name = _("On"); भङ्ग;
		पक्ष KKPS_DISABLED: name = _("Disabled"); भङ्ग;
		पक्ष KKPS_AFTER_T_TE: name = _("Cleared"); भङ्ग;
	    }

	    वर्ण buf[200];
	    vim_snprintf(buf, परिमाण(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    संदेश_puts(buf);
	}
    }

    // need to loop over all global hash lists
    प्रति (पूर्णाङ्क hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	यदि (abbrev)
	{
	    यदि (hash != 0)	// there is only one abbreviation list
		भङ्ग;
	    mp = curbuf->b_first_abbr;
	}
	अन्यथा
	    mp = curbuf->b_maphash[hash];
	प्रति ( ; mp != रिक्त && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    यदि (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		यदि (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		अन्यथा
		{
		    पूर्णाङ्क n = mp->m_keylen;
		    यदि (STRNCMP(mp->m_कुंजियाँ, कुंजियाँ,
				   (माप_प्रकार)(n < कुंजियाँ_len ? n : कुंजियाँ_len)) == 0)
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
    पूर्णाङ्क
do_map(
    पूर्णाङ्क		maptype,
    char_u	*arg,
    पूर्णाङ्क		mode,
    पूर्णाङ्क		abbrev)		// not a mapping but an abbreviation
{
    char_u	*कुंजियाँ;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {रिक्त, रिक्त};
    char_u	*rhs;
    char_u	*p;
    पूर्णाङ्क		n;
    पूर्णाङ्क		len = 0;	// init for GCC
    पूर्णाङ्क		hasarg;
    पूर्णाङ्क		haskey;
    पूर्णाङ्क		do_print;
    पूर्णाङ्क		keyround;
    char_u	*कुंजियाँ_buf = रिक्त;
    char_u	*alt_कुंजियाँ_buf = रिक्त;
    char_u	*arg_buf = रिक्त;
    पूर्णाङ्क		retval = 0;
    पूर्णाङ्क		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    पूर्णाङ्क		unique = FALSE;
    पूर्णाङ्क		nowait = FALSE;
    पूर्णाङ्क		silent = FALSE;
    पूर्णाङ्क		special = FALSE;
#ifdef FEAT_EVAL
    पूर्णाङ्क		व्यंजक = FALSE;
#endif
    पूर्णाङ्क		did_simplify = FALSE;
    पूर्णाङ्क		unmap_lhs_only = FALSE;
    पूर्णाङ्क		noremap;
    char_u      *orig_rhs;

    कुंजियाँ = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    यदि (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    यदि (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    अन्यथा
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    प्रति (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	यदि (STRNCMP(कुंजियाँ, "<buffer>", 8) == 0)
	{
	    कुंजियाँ = skipwhite(कुंजियाँ + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    अनुवर्त;
	}

	// Check for "<nowait>": don't wait for more characters.
	यदि (STRNCMP(कुंजियाँ, "<nowait>", 8) == 0)
	{
	    कुंजियाँ = skipwhite(कुंजियाँ + 8);
	    nowait = TRUE;
	    अनुवर्त;
	}

	// Check for "<silent>": don't echo commands.
	यदि (STRNCMP(कुंजियाँ, "<silent>", 8) == 0)
	{
	    कुंजियाँ = skipwhite(कुंजियाँ + 8);
	    silent = TRUE;
	    अनुवर्त;
	}

	// Check for "<special>": accept special keys in <>
	यदि (STRNCMP(कुंजियाँ, "<special>", 9) == 0)
	{
	    कुंजियाँ = skipwhite(कुंजियाँ + 9);
	    special = TRUE;
	    अनुवर्त;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	यदि (STRNCMP(कुंजियाँ, "<script>", 8) == 0)
	{
	    कुंजियाँ = skipwhite(कुंजियाँ + 8);
	    noremap = REMAP_SCRIPT;
	    अनुवर्त;
	}

	// Check for "<expr>": {rhs} is an expression.
	यदि (STRNCMP(कुंजियाँ, "<expr>", 6) == 0)
	{
	    कुंजियाँ = skipwhite(कुंजियाँ + 6);
	    व्यंजक = TRUE;
	    अनुवर्त;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	यदि (STRNCMP(कुंजियाँ, "<unique>", 8) == 0)
	{
	    कुंजियाँ = skipwhite(कुंजियाँ + 8);
	    unique = TRUE;
	    अनुवर्त;
	}
	भङ्ग;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = कुंजियाँ;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == रिक्त);
    यावत् (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	यदि ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    यदि (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*कुंजियाँ != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    यदि (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	गच्छ theend;
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
    यदि (haskey)
    {
	char_u	*new_कुंजियाँ;
	पूर्णाङ्क	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	यदि (special)
	    flags |= REPTERM_SPECIAL;
	new_कुंजियाँ = replace_termcodes(कुंजियाँ, &कुंजियाँ_buf, 0, flags, &did_simplify);
	यदि (did_simplify)
	    (शून्य)replace_termcodes(कुंजियाँ, &alt_कुंजियाँ_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, रिक्त);
	कुंजियाँ = new_कुंजियाँ;
    }
    orig_rhs = rhs;
    यदि (hasarg)
    {
	यदि (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	अन्यथा
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), रिक्त);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    प्रति (keyround = 1; keyround <= 2; ++keyround)
    {
	पूर्णाङ्क	did_it = FALSE;
	पूर्णाङ्क	did_local = FALSE;
	पूर्णाङ्क	keyround1_simplified = keyround == 1 && did_simplify;
	पूर्णाङ्क	round;
	पूर्णाङ्क	अंक_rounds;

	यदि (keyround == 2)
	{
	    यदि (alt_कुंजियाँ_buf == रिक्त)
		भङ्ग;
	    कुंजियाँ = alt_कुंजियाँ_buf;
	}
	अन्यथा यदि (alt_कुंजियाँ_buf != रिक्त && do_print)
	    // when printing always use the not-simplified map
	    कुंजियाँ = alt_कुंजियाँ_buf;

	// check arguments and translate function keys
	यदि (haskey)
	{
	    len = (पूर्णाङ्क)STRLEN(कुंजियाँ);
	    यदि (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		गच्छ theend;
	    }

	    यदि (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		यदि (has_mbyte)
		{
		    पूर्णाङ्क		first, last;
		    पूर्णाङ्क		same = -1;
		    char_u	कुंजियाँ_unescaped[MAXMAPLEN + 1];
		    माप_प्रकार	कुंजियाँ_unescaped_len;

		    mch_memmove(कुंजियाँ_unescaped, कुंजियाँ, (माप_प्रकार)(len + 1));
		    कुंजियाँ_unescaped_len = vim_unescape_csi(कुंजियाँ_unescaped);
		    p = कुंजियाँ_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    यावत् (p < कुंजियाँ_unescaped + कुंजियाँ_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			यदि (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    यदि (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			गच्छ theend;
		    }
		}
		अन्यथा यदि (vim_iswordc(कुंजियाँ[len - 1]))
		    // ends in keyword char
		    प्रति (n = 0; n < len - 2; ++n)
			यदि (vim_iswordc(कुंजियाँ[n]) != vim_iswordc(कुंजियाँ[len - 2]))
			{
			    retval = 1;
			    गच्छ theend;
			}
		// An abbreviation cannot contain white space.
		प्रति (n = 0; n < len; ++n)
		    यदि (VIM_ISWHITE(कुंजियाँ[n]))
		    {
			retval = 1;
			गच्छ theend;
		    }
	    }
	}

	यदि (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	यदि (do_print)
	    संदेश_start();

	// Check if a new local mapping wasn't already defined globally.
	यदि (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    प्रति (पूर्णाङ्क hash = 0; hash < 256 && !got_int; ++hash)
	    {
		यदि (abbrev)
		{
		    यदि (hash != 0)	// there is only one abbreviation list
			भङ्ग;
		    mp = first_abbr;
		}
		अन्यथा
		    mp = maphash[hash];
		प्रति ( ; mp != रिक्त && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    यदि ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_कुंजियाँ, कुंजियाँ, (माप_प्रकार)len) == 0)
		    {
			यदि (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_कुंजियाँ);
			अन्यथा
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_कुंजियाँ);
			retval = 5;
			गच्छ theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	यदि (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, कुंजियाँ, len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	अंक_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	प्रति (round = 0; round < अंक_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    प्रति (पूर्णाङ्क hash = 0; hash < 256 && !got_int; ++hash)
	    {
		यदि (abbrev)
		{
		    यदि (hash > 0)	// there is only one abbreviation list
			भङ्ग;
		    mpp = abbr_table;
		}
		अन्यथा
		    mpp = &(map_table[hash]);
		प्रति (mp = *mpp; mp != रिक्त && !got_int; mp = *mpp)
		{

		    यदि ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			अनुवर्त;
		    }
		    यदि (!haskey)	// show all entries
		    {
			यदि (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    अन्यथा	// do we have a match?
		    {
			यदि (round)	// second round: Try unmap "rhs" string
			{
			    n = (पूर्णाङ्क)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			अन्यथा
			{
			    n = mp->m_keylen;
			    p = mp->m_कुंजियाँ;
			}
			यदि (STRNCMP(p, कुंजियाँ, (माप_प्रकार)(n < len ? n : len)) == 0)
			{
			    यदि (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				यदि (n != len && (!abbrev || round || n > len
					       || *skipwhite(कुंजियाँ + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    अनुवर्त;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				यदि (keyround1_simplified && !mp->m_simplified)
				    भङ्ग;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    अन्यथा यदि (!hasarg)	// show matching entry
			    {
				यदि (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    अन्यथा यदि (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				अनुवर्त;
			    }
			    अन्यथा यदि (unique)
			    {
				यदि (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				अन्यथा
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				गच्छ theend;
			    }
			    अन्यथा
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				यदि (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    यदि (newstr == रिक्त)
				    {
					retval = 4;		// no mem
					गच्छ theend;
				    }
				    यदि (mp->m_alt != रिक्त)
					mp->m_alt = mp->m_alt->m_alt = रिक्त;
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
				    mp->m_व्यंजक = व्यंजक;
				    mp->m_script_ctx = current_sctx;
				    यदि (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    यदि (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				अनुवर्त;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    पूर्णाङ्क new_hash = MAP_HASH(mp->m_mode, mp->m_कुंजियाँ[0]);
			    यदि (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_next;
				mp->m_next = map_table[new_hash];
				map_table[new_hash] = mp;

				अनुवर्त;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_next);
		}
	    }
	}

	यदि (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    यदि (!did_it)
	    {
		यदि (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    अन्यथा यदि (*कुंजियाँ == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		यदि (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		अन्यथा
		    mapped_ctrl_c &= ~mode;
	    }
	    अनुवर्त;
	}

	यदि (!haskey || !hasarg)
	{
	    // print entries
	    यदि (!did_it && !did_local)
	    {
		यदि (abbrev)
		    संदेश(_("No abbreviation found"));
		अन्यथा
		    संदेश(_("No mapping found"));
	    }
	    गच्छ theend;    // listing finished
	}

	यदि (did_it)
	    अनुवर्त;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, कुंजियाँ,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    व्यंजक, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	यदि (mp_result[keyround - 1] == रिक्त)
	{
	    retval = 4;	    // no mem
	    गच्छ theend;
	}
    }

    यदि (mp_result[0] != रिक्त && mp_result[1] != रिक्त)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(कुंजियाँ_buf);
    vim_free(alt_कुंजियाँ_buf);
    vim_free(arg_buf);
    प्रत्यागम retval;
}

/*
 * Get the mapping mode from the command name.
 */
    स्थिर पूर्णाङ्क
get_map_mode(char_u **cmdp, पूर्णाङ्क forceit)
{
    char_u	*p;
    पूर्णाङ्क		modec;
    पूर्णाङ्क		mode;

    p = *cmdp;
    modec = *p++;
    यदि (modec == 'i')
	mode = MODE_जोड़ो;				// :imap
    अन्यथा यदि (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    अन्यथा यदि (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    अन्यथा यदि (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    अन्यथा यदि (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    अन्यथा यदि (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    अन्यथा यदि (modec == 's')
	mode = MODE_SELECT;			// :smap
    अन्यथा यदि (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    अन्यथा यदि (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    अन्यथा
    {
	--p;
	यदि (forceit)
	    mode = MODE_जोड़ो | MODE_CMDLINE;		// :map !
	अन्यथा
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    प्रत्यागम mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    स्थिर शून्य
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    पूर्णाङ्क		forceit,
    पूर्णाङ्क		abbr)
{
    पूर्णाङ्क		mode;
    पूर्णाङ्क		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    यदि (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	प्रत्यागम;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    स्थिर पूर्णाङ्क
is_map_locked(शून्य)
{
    यदि (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    शून्य
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    पूर्णाङ्क		mode,		// mode in which to delete
    पूर्णाङ्क		local,		// TRUE for buffer-local mappings
    पूर्णाङ्क		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    पूर्णाङ्क		hash;
    पूर्णाङ्क		new_hash;

    यदि (is_map_locked())
	प्रत्यागम;

    validate_maphash();

    प्रति (hash = 0; hash < 256; ++hash)
    {
	यदि (abbr)
	{
	    यदि (hash > 0)	// there is only one abbrlist
		भङ्ग;
	    यदि (local)
		mpp = &buf->b_first_abbr;
	    अन्यथा
		mpp = &first_abbr;
	}
	अन्यथा
	{
	    यदि (local)
		mpp = &buf->b_maphash[hash];
	    अन्यथा
		mpp = &maphash[hash];
	}
	यावत् (*mpp != रिक्त)
	{
	    mp = *mpp;
	    यदि (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		यदि (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    अनुवर्त;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_कुंजियाँ[0]);
		यदि (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    यदि (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    अन्यथा
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    अनुवर्त;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

#यदि defined(FEAT_EVAL)
    पूर्णाङ्क
mode_str2flags(char_u *modechars)
{
    पूर्णाङ्क		mode = 0;

    यदि (vim_strchr(modechars, 'n') != रिक्त)
	mode |= MODE_NORMAL;
    यदि (vim_strchr(modechars, 'v') != रिक्त)
	mode |= MODE_VISUAL | MODE_SELECT;
    यदि (vim_strchr(modechars, 'x') != रिक्त)
	mode |= MODE_VISUAL;
    यदि (vim_strchr(modechars, 's') != रिक्त)
	mode |= MODE_SELECT;
    यदि (vim_strchr(modechars, 'o') != रिक्त)
	mode |= MODE_OP_PENDING;
    यदि (vim_strchr(modechars, 'i') != रिक्त)
	mode |= MODE_जोड़ो;
    यदि (vim_strchr(modechars, 'l') != रिक्त)
	mode |= MODE_LANGMAP;
    यदि (vim_strchr(modechars, 'c') != रिक्त)
	mode |= MODE_CMDLINE;

    प्रत्यागम mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    पूर्णाङ्क
map_to_exists(char_u *str, char_u *modechars, पूर्णाङ्क abbr)
{
    char_u	*rhs;
    char_u	*buf;
    पूर्णाङ्क		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, रिक्त);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    प्रत्यागम retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    पूर्णाङ्क
map_to_exists_mode(char_u *rhs, पूर्णाङ्क mode, पूर्णाङ्क abbr)
{
    mapblock_T	*mp;
    पूर्णाङ्क		hash;
    पूर्णाङ्क		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    प्रति (;;)
    {
	प्रति (hash = 0; hash < 256; ++hash)
	{
	    यदि (abbr)
	    {
		यदि (hash > 0)		// there is only one abbr list
		    भङ्ग;
		यदि (exp_buffer)
		    mp = curbuf->b_first_abbr;
		अन्यथा
		    mp = first_abbr;
	    }
	    अन्यथा यदि (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    अन्यथा
		mp = maphash[hash];
	    प्रति (; mp; mp = mp->m_next)
	    {
		यदि ((mp->m_mode & mode)
			&& म_माला((वर्ण *)mp->m_str, (वर्ण *)rhs) != रिक्त)
		    प्रत्यागम TRUE;
	    }
	}
	यदि (exp_buffer)
	    भङ्ग;
	exp_buffer = TRUE;
    }

    प्रत्यागम FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
स्थिर पूर्णाङ्क	expand_mapmodes = 0;
स्थिर पूर्णाङ्क	expand_isabbrev = 0;
स्थिर पूर्णाङ्क	expand_buffer = FALSE;

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
    स्थिर char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    पूर्णाङ्क		c;
    पूर्णाङ्क		modifiers;
    पूर्णाङ्क		cpo_bslash;
    पूर्णाङ्क		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != रिक्त);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != रिक्त);

    प्रति (; *str; ++str)
    {
	c = *str;
	यदि (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    यदि (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    यदि (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		यदि (cpo_special)
		{
		    ga_clear(&ga);
		    प्रत्यागम रिक्त;
		}
		c = TO_SPECIAL(str[1], str[2]);
		यदि (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    यदि (IS_SPECIAL(c) || modifiers)	// special key
	    {
		यदि (cpo_special)
		{
		    ga_clear(&ga);
		    प्रत्यागम रिक्त;
		}
		ga_concat(&ga, get_special_कुंजी_name(c, modifiers));
		अनुवर्त; // for (str)
	    }
	}
	यदि (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	यदि (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    प्रत्यागम (char_u *)(ga.ga_data);
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
    पूर्णाङ्क		forceit,	// TRUE if '!' given
    पूर्णाङ्क		isabbrev,	// TRUE if abbreviation
    पूर्णाङ्क		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    यदि (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    अन्यथा
    {
	यदि (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	अन्यथा
	{
	    expand_mapmodes = MODE_जोड़ो | MODE_CMDLINE;
	    यदि (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	प्रति (;;)
	{
	    यदि (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		अनुवर्त;
	    }
	    यदि (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		अनुवर्त;
	    }
	    यदि (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		अनुवर्त;
	    }
	    यदि (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		अनुवर्त;
	    }
	    यदि (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		अनुवर्त;
	    }
#ifdef FEAT_EVAL
	    यदि (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		अनुवर्त;
	    }
	    यदि (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		अनुवर्त;
	    }
#endif
	    भङ्ग;
	}
	xp->xp_pattern = arg;
    }

    प्रत्यागम रिक्त;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    पूर्णाङ्क
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    पूर्णाङ्क		*अंक_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    पूर्णाङ्क		hash;
    पूर्णाङ्क		गिनती;
    char_u	*p;
    पूर्णाङ्क		i;
    पूर्णाङ्क		fuzzy;
    पूर्णाङ्क		match;
    पूर्णाङ्क		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *अंक_Matches = 0;		    // return values in case of FAIL
    *matches = रिक्त;

    यदि (!fuzzy)
	ga_init2(&ga, परिमाण(वर्ण *), 3);
    अन्यथा
	ga_init2(&ga, परिमाण(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    प्रति (i = 0; i < 7; ++i)
    {
	यदि (i == 0)
	    p = (char_u *)"<silent>";
	अन्यथा यदि (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	अन्यथा यदि (i == 2)
	    p = (char_u *)"<script>";
	अन्यथा यदि (i == 3)
	    p = (char_u *)"<expr>";
#endif
	अन्यथा यदि (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	अन्यथा यदि (i == 5)
	    p = (char_u *)"<nowait>";
	अन्यथा यदि (i == 6)
	    p = (char_u *)"<special>";
	अन्यथा
	    अनुवर्त;

	यदि (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	अन्यथा
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	यदि (!match)
	    अनुवर्त;

	यदि (ga_grow(&ga, 1) == FAIL)
	    भङ्ग;

	यदि (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	अन्यथा
	    ((char_u **)ga.ga_data)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    प्रति (hash = 0; hash < 256; ++hash)
    {
	यदि (expand_isabbrev)
	{
	    यदि (hash > 0)	// only one abbrev list
		भङ्ग; // for (hash)
	    mp = first_abbr;
	}
	अन्यथा यदि (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	अन्यथा
	    mp = maphash[hash];
	प्रति (; mp; mp = mp->m_next)
	{
	    यदि (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		अनुवर्त;

	    p = translate_mapping(mp->m_कुंजियाँ);
	    यदि (p == रिक्त)
		अनुवर्त;

	    यदि (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    अन्यथा
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    यदि (!match)
	    {
		vim_free(p);
		अनुवर्त;
	    }

	    यदि (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		भङ्ग;
	    }

	    यदि (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    अन्यथा
		((char_u **)ga.ga_data)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    यदि (ga.ga_len == 0)
	प्रत्यागम FAIL;

    यदि (!fuzzy)
    {
	*matches = ga.ga_data;
	*अंक_Matches = ga.ga_len;
    }
    अन्यथा
    {
	यदि (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    प्रत्यागम FAIL;
	*अंक_Matches = ga.ga_len;
    }

    गिनती = *अंक_Matches;
    यदि (गिनती > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	यदि (!fuzzy)
	    sort_strings(*matches, गिनती);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + गिनती;

	यावत् (ptr2 < ptr3)
	{
	    यदि (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    अन्यथा
	    {
		vim_free(*ptr2++);
		गिनती--;
	    }
	}
    }

    *अंक_Matches = गिनती;
    प्रत्यागम (गिनती == 0 ? FAIL : OK);
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
    पूर्णाङ्क
check_abbr(
    पूर्णाङ्क		c,
    char_u	*ptr,
    पूर्णाङ्क		col,
    पूर्णाङ्क		mincol)
{
    पूर्णाङ्क		len;
    पूर्णाङ्क		scol;		// starting column of the abbr.
    पूर्णाङ्क		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    पूर्णाङ्क		clen = 0;	// length in characters
    पूर्णाङ्क		is_id = TRUE;
    पूर्णाङ्क		vim_abbr;

    यदि (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	प्रत्यागम FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    यदि (noremap_कुंजियाँ() && c != Ctrl_RSB)
	प्रत्यागम FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    यदि (col == 0)				// cannot be an abbr.
	प्रत्यागम FALSE;

    यदि (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	यदि (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	अन्यथा
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    यदि (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	यावत् (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    यदि (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		भङ्ग;
	    }
	    ++clen;
	}
	scol = (पूर्णाङ्क)(p - ptr);
    }
    अन्यथा
    {
	यदि (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	अन्यथा
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    यदि (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	प्रति (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    यदि (scol < mincol)
	scol = mincol;
    यदि (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	यदि (mp == रिक्त)
	{
	    mp = mp2;
	    mp2 = रिक्त;
	}
	प्रति ( ; mp; mp->m_next == रिक्त
				  ? (mp = mp2, mp2 = रिक्त) : (mp = mp->m_next))
	{
	    पूर्णाङ्क		qlen = mp->m_keylen;
	    char_u	*q = mp->m_कुंजियाँ;
	    पूर्णाङ्क		match;

	    यदि (vim_strbyte(mp->m_कुंजियाँ, K_SPECIAL) != रिक्त)
	    {
		char_u *qe = vim_strsave(mp->m_कुंजियाँ);

		// might have CSI escaped mp->m_keys
		यदि (qe != रिक्त)
		{
		    q = qe;
		    qlen = (पूर्णाङ्क)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (माप_प्रकार)len) == 0;
	    यदि (q != mp->m_कुंजियाँ)
		vim_free(q);
	    यदि (match)
		भङ्ग;
	}
	यदि (mp != रिक्त)
	{
	    पूर्णाङ्क	noremap;
	    पूर्णाङ्क silent;
#ifdef FEAT_EVAL
	    पूर्णाङ्क व्यंजक;
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
	    यदि (c != Ctrl_RSB)
	    {
					// special key code, split up
		यदि (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		अन्यथा
		{
		    यदि (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    यदि (has_mbyte)
		    {
			पूर्णाङ्क	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			यदि (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			यदि (escaped != रिक्त)
			{
			    newlen = (पूर्णाङ्क)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    अन्यथा
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(शून्य)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ifdef FEAT_EVAL
	    व्यंजक = mp->m_व्यंजक;

	    यदि (व्यंजक)
		s = eval_map_व्यंजक(mp, c);
	    अन्यथा
#endif
		s = mp->m_str;
	    यदि (s != रिक्त)
	    {
					// insert the to string
		(शून्य)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (पूर्णाङ्क)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		यदि (व्यंजक)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    यदि (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    यावत् (len-- > 0)		// delete the from string
		(शून्य)ins_typebuf(tb, 1, 0, TRUE, silent);
	    प्रत्यागम TRUE;
	}
    }
    प्रत्यागम FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_व्यंजक(
    mapblock_T	*mp,
    पूर्णाङ्क		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*व्यंजक;
    pos_T	save_cursor;
    पूर्णाङ्क		save_संदेश_col;
    पूर्णाङ्क		save_संदेश_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    पूर्णाङ्क		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    व्यंजक = vim_strsave(mp->m_str);
    यदि (व्यंजक == रिक्त)
	प्रत्यागम रिक्त;
    vim_unescape_csi(व्यंजक);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_संदेश_col = संदेश_col;
    save_संदेश_row = संदेश_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    यदि (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(व्यंजक, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    संदेश_col = save_संदेश_col;
    संदेश_row = save_संदेश_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(व्यंजक);

    यदि (p == रिक्त)
	प्रत्यागम रिक्त;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    प्रत्यागम res;
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
    यदि (res == रिक्त)
	प्रत्यागम रिक्त;

    d = res;
    प्रति (s = p; *s != NUL; )
    {
	यदि ((s[0] == K_SPECIAL
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
	अन्यथा
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    प्रत्यागम res;
}

/*
 * Remove escaping from CSI and K_SPECIAL characters.  Reverse of
 * vim_strsave_escape_csi().  Works in-place.
 * Returns the number of bytes in the unescaped string.
 */
    माप_प्रकार
vim_unescape_csi(char_u *p)
{
    char_u	*s = p, *d = p;

    यावत् (*s != NUL)
    {
	यदि (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	अन्यथा यदि ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (पूर्णाङ्क)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	अन्यथा
	    *d++ = *s++;
    }
    *d = NUL;
    प्रत्यागम (माप_प्रकार)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    पूर्णाङ्क
makemap(
    खाता	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    वर्ण	*cmd;
    पूर्णाङ्क		abbr;
    पूर्णाङ्क		hash;
    पूर्णाङ्क		did_cpo = FALSE;
    पूर्णाङ्क		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    प्रति (abbr = 0; abbr < 2; ++abbr)
	प्रति (hash = 0; hash < 256; ++hash)
	{
	    यदि (abbr)
	    {
		यदि (hash > 0)		// there is only one abbr list
		    भङ्ग;
		यदि (buf != रिक्त)
		    mp = buf->b_first_abbr;
		अन्यथा
		    mp = first_abbr;
	    }
	    अन्यथा
	    {
		यदि (buf != रिक्त)
		    mp = buf->b_maphash[hash];
		अन्यथा
		    mp = maphash[hash];
	    }

	    प्रति ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		यदि (mp->m_simplified)
		    अनुवर्त;

		// skip script-local mappings
		यदि (mp->m_noremap == REMAP_SCRIPT)
		    अनुवर्त;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		प्रति (p = mp->m_str; *p != NUL; ++p)
		    यदि (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (पूर्णाङ्क)KE_SNR)
			भङ्ग;
		यदि (*p != NUL)
		    अनुवर्त;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		यदि (abbr)
		    cmd = "abbr";
		अन्यथा
		    cmd = "map";
		विकल्प (mp->m_mode)
		{
		    पक्ष MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			भङ्ग;
		    पक्ष MODE_NORMAL:
			c1 = 'n';
			भङ्ग;
		    पक्ष MODE_VISUAL:
			c1 = 'x';
			भङ्ग;
		    पक्ष MODE_SELECT:
			c1 = 's';
			भङ्ग;
		    पक्ष MODE_OP_PENDING:
			c1 = 'o';
			भङ्ग;
		    पक्ष MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			भङ्ग;
		    पक्ष MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			भङ्ग;
		    पक्ष MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			भङ्ग;
		    पक्ष MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			भङ्ग;
		    पक्ष MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			भङ्ग;
		    पक्ष MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			भङ्ग;
		    पक्ष MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			भङ्ग;
		    पक्ष MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			भङ्ग;
		    पक्ष MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			भङ्ग;
		    पक्ष MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			भङ्ग;
		    पक्ष MODE_CMDLINE | MODE_जोड़ो:
			यदि (!abbr)
			    cmd = "map!";
			भङ्ग;
		    पक्ष MODE_CMDLINE:
			c1 = 'c';
			भङ्ग;
		    पक्ष MODE_जोड़ो:
			c1 = 'i';
			भङ्ग;
		    पक्ष MODE_LANGMAP:
			c1 = 'l';
			भङ्ग;
		    पक्ष MODE_TERMINAL:
			c1 = 't';
			भङ्ग;
		    स्वभाव:
			iemsg(e_makemap_illegal_mode);
			प्रत्यागम FAIL;
		}
		कुरु	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    यदि (!did_cpo)
		    {
			यदि (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			अन्यथा
			    प्रति (i = 0; i < 2; ++i)
				प्रति (p = (i ? mp->m_str : mp->m_कुंजियाँ); *p; ++p)
				    यदि (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			यदि (did_cpo)
			{
			    यदि (ख_लिखो(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || ख_लिखो(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				प्रत्यागम FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    यदि (mp->m_व्यंजक && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && ख_माला_दो("legacy ", fd) < 0)
			प्रत्यागम FAIL;
#endif
		    यदि (c1 && अ_दो(c1, fd) < 0)
			प्रत्यागम FAIL;
		    यदि (mp->m_noremap != REMAP_YES && ख_लिखो(fd, "nore") < 0)
			प्रत्यागम FAIL;
		    यदि (ख_माला_दो(cmd, fd) < 0)
			प्रत्यागम FAIL;
		    यदि (buf != रिक्त && ख_माला_दो(" <buffer>", fd) < 0)
			प्रत्यागम FAIL;
		    यदि (mp->m_nowait && ख_माला_दो(" <nowait>", fd) < 0)
			प्रत्यागम FAIL;
		    यदि (mp->m_silent && ख_माला_दो(" <silent>", fd) < 0)
			प्रत्यागम FAIL;
#ifdef FEAT_EVAL
		    यदि (mp->m_noremap == REMAP_SCRIPT
						 && ख_माला_दो("<script>", fd) < 0)
			प्रत्यागम FAIL;
		    यदि (mp->m_व्यंजक && ख_माला_दो(" <expr>", fd) < 0)
			प्रत्यागम FAIL;
#endif

		    यदि (       अ_दो(' ', fd) < 0
			    || put_escstr(fd, mp->m_कुंजियाँ, 0) == FAIL
			    || अ_दो(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			प्रत्यागम FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} यावत् (c1 != NUL);
	    }
	}

    यदि (did_cpo)
	यदि (ख_लिखो(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णाङ्क
put_escstr(खाता *fd, char_u *strstart, पूर्णाङ्क what)
{
    char_u	*str = strstart;
    पूर्णाङ्क		c;
    पूर्णाङ्क		modifiers;

    // :map xx <Nop>
    यदि (*str == NUL && what == 1)
    {
	यदि (ख_लिखो(fd, "<Nop>") < 0)
	    प्रत्यागम FAIL;
	प्रत्यागम OK;
    }

    प्रति ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	यदि (p != रिक्त)
	{
	    यावत् (*p != NUL)
		यदि (ख_अक्षर_दो(*p++, fd) < 0)
		    प्रत्यागम FAIL;
	    --str;
	    अनुवर्त;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	यदि (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    यदि (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		यदि (p == रिक्त)
		    c = *str;
		अन्यथा
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    यदि (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    यदि (IS_SPECIAL(c) || modifiers)	// special key
	    {
		यदि (ख_माला_दो((वर्ण *)get_special_कुंजी_name(c, modifiers), fd) < 0)
		    प्रत्यागम FAIL;
		अनुवर्त;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	यदि (c == NL)
	{
	    यदि (what == 2)
	    {
		यदि (ख_लिखो(fd, "\\\026\n") < 0)
		    प्रत्यागम FAIL;
	    }
	    अन्यथा
	    {
		यदि (ख_लिखो(fd, "<NL>") < 0)
		    प्रत्यागम FAIL;
	    }
	    अनुवर्त;
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
	यदि (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    यदि (अ_दो('\\', fd) < 0)
		प्रत्यागम FAIL;
	}
	अन्यथा यदि (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    यदि (अ_दो(Ctrl_V, fd) < 0)
		प्रत्यागम FAIL;
	}
	यदि (अ_दो(c, fd) < 0)
	    प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    शून्य
check_map_keycodes(शून्य)
{
    mapblock_T	*mp;
    char_u	*p;
    पूर्णाङ्क		i;
    char_u	buf[3];
    पूर्णाङ्क		abbr;
    पूर्णाङ्क		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_धकेलो(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    प्रति (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	प्रति (abbr = 0; abbr <= 1; ++abbr)
	    प्रति (hash = 0; hash < 256; ++hash)
	    {
		यदि (abbr)
		{
		    यदि (hash)	    // there is only one abbr list
			भङ्ग;
		    यदि (bp != रिक्त)
			mp = bp->b_first_abbr;
		    अन्यथा
			mp = first_abbr;
		}
		अन्यथा
		{
		    यदि (bp != रिक्त)
			mp = bp->b_maphash[hash];
		    अन्यथा
			mp = maphash[hash];
		}
		प्रति ( ; mp != रिक्त; mp = mp->m_next)
		{
		    प्रति (i = 0; i <= 1; ++i)	// do this twice
		    {
			यदि (i == 0)
			    p = mp->m_कुंजियाँ;	// once for the "from" part
			अन्यथा
			    p = mp->m_str;	// and once for the "to" part
			यावत् (*p)
			{
			    यदि (*p == K_SPECIAL)
			    {
				++p;
				यदि (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (शून्य)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	यदि (bp == रिक्त)
	    भङ्ग;
    }
    ESTACK_CHECK_NOW;
    estack_निकालो();
}

#यदि defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*कुंजियाँ,
    पूर्णाङ्क		mode,
    पूर्णाङ्क		exact,		// require exact match
    पूर्णाङ्क		ign_mod,	// ignore preceding modifier
    पूर्णाङ्क		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    पूर्णाङ्क		*local_ptr)	// return: buffer-local mapping or NULL
{
    पूर्णाङ्क		hash;
    पूर्णाङ्क		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    पूर्णाङ्क		local;

    validate_maphash();

    len = (पूर्णाङ्क)STRLEN(कुंजियाँ);
    प्रति (local = 1; local >= 0; --local)
	// loop over all hash lists
	प्रति (hash = 0; hash < 256; ++hash)
	{
	    यदि (abbr)
	    {
		यदि (hash > 0)		// there is only one list.
		    भङ्ग;
		यदि (local)
		    mp = curbuf->b_first_abbr;
		अन्यथा
		    mp = first_abbr;
	    }
	    अन्यथा यदि (local)
		mp = curbuf->b_maphash[hash];
	    अन्यथा
		mp = maphash[hash];
	    प्रति ( ; mp != रिक्त; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		यदि ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    यदि (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    अन्यथा
			minlen = len;
		    s = mp->m_कुंजियाँ;
		    यदि (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			यदि (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    यदि (STRNCMP(s, कुंजियाँ, minlen) == 0)
		    {
			यदि (mp_ptr != रिक्त)
			    *mp_ptr = mp;
			यदि (local_ptr != रिक्त)
			    *local_ptr = local;
			प्रत्यागम mp->m_str;
		    }
		}
	    }
	}

    प्रत्यागम रिक्त;
}

/*
 * "hasmapto()" function
 */
    शून्य
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*mode;
    char_u	buf[NUMBUFLEN];
    पूर्णाङ्क		abbr = FALSE;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

    name = tv_get_string(&argvars[0]);
    यदि (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    अन्यथा
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	यदि (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (पूर्णाङ्क)tv_get_bool(&argvars[2]);
    }

    यदि (map_to_exists(name, mode, abbr))
	rettv->vval.v_संख्या = TRUE;
    अन्यथा
	rettv->vval.v_संख्या = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    स्थिर शून्य
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	पूर्णाङ्क	    buffer_local,   // false if not buffer local mapping
	पूर्णाङ्क	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_कुंजियाँ, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_कुंजियाँ);
    यदि (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_संख्या(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_संख्या(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_संख्या(dict, "expr", mp->m_व्यंजक ? 1L : 0L);
    dict_add_संख्या(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_add_संख्या(dict, "sid", (दीर्घ)mp->m_script_ctx.sc_sid);
    dict_add_संख्या(dict, "scriptversion",
		    (दीर्घ)mp->m_script_ctx.sc_version);
    dict_add_संख्या(dict, "lnum", (दीर्घ)mp->m_script_ctx.sc_lnum);
    dict_add_संख्या(dict, "buffer", (दीर्घ)buffer_local);
    dict_add_संख्या(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_add_string(dict, "mode", mapmode);
    dict_add_संख्या(dict, "abbr", abbr ? 1L : 0L);
    dict_add_संख्या(dict, "mode_bits", mp->m_mode);

    vim_free(mapmode);
}

    स्थिर शून्य
get_maparg(typval_T *argvars, typval_T *rettv, पूर्णाङ्क exact)
{
    char_u	*कुंजियाँ;
    char_u	*कुंजियाँ_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*कुंजियाँ_buf = रिक्त;
    char_u	*alt_कुंजियाँ_buf = रिक्त;
    पूर्णाङ्क		did_simplify = FALSE;
    char_u	*rhs;
    पूर्णाङ्क		mode;
    पूर्णाङ्क		abbr = FALSE;
    पूर्णाङ्क		get_dict = FALSE;
    mapblock_T	*mp = रिक्त;
    पूर्णाङ्क		buffer_local;
    पूर्णाङ्क		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    कुंजियाँ = tv_get_string(&argvars[0]);
    यदि (*कुंजियाँ == NUL)
	प्रत्यागम;

    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	यदि (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (पूर्णाङ्क)tv_get_bool(&argvars[2]);
	    यदि (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (पूर्णाङ्क)tv_get_bool(&argvars[3]);
	}
    }
    अन्यथा
	which = (char_u *)"";
    यदि (which == रिक्त)
	प्रत्यागम;

    mode = get_map_mode(&which, 0);

    कुंजियाँ_simplified = replace_termcodes(कुंजियाँ, &कुंजियाँ_buf, 0, flags,
								&did_simplify);
    rhs = check_map(कुंजियाँ_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    यदि (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(शून्य)replace_termcodes(कुंजियाँ, &alt_कुंजियाँ_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, रिक्त);
	rhs = check_map(alt_कुंजियाँ_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    यदि (!get_dict)
    {
	// Return a string.
	यदि (rhs != रिक्त)
	{
	    यदि (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    अन्यथा
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    अन्यथा यदि (rettv_dict_alloc(rettv) == OK && rhs != रिक्त)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? कुंजियाँ_simplified : रिक्त,
			  buffer_local, abbr);

    vim_free(कुंजियाँ_buf);
    vim_free(alt_कुंजियाँ_buf);
}

/*
 * "maplist()" function
 */
    शून्य
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    पूर्णाङ्क		buffer_local;
    char_u	*कुंजियाँ_buf;
    पूर्णाङ्क		did_simplify;
    पूर्णाङ्क		hash;
    char_u	*lhs;
    ध्रुव पूर्णाङ्क	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    पूर्णाङ्क		abbr = FALSE;

    यदि (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	प्रत्यागम;
    यदि (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    प्रति (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	प्रति (hash = 0; hash < 256; ++hash)
	{
	    यदि (abbr)
	    {
		यदि (hash > 0)		// there is only one abbr list
		    भङ्ग;
		यदि (buffer_local)
		    mp = curbuf->b_first_abbr;
		अन्यथा
		    mp = first_abbr;
	    }
	    अन्यथा यदि (buffer_local)
		mp = curbuf->b_maphash[hash];
	    अन्यथा
		mp = maphash[hash];
	    प्रति (; mp; mp = mp->m_next)
	    {
		यदि (mp->m_simplified)
		    अनुवर्त;
		यदि ((d = dict_alloc()) == रिक्त)
		    प्रत्यागम;
		यदि (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    प्रत्यागम;
		}

		कुंजियाँ_buf = रिक्त;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_कुंजियाँ, TRUE, FALSE);
		(शून्य)replace_termcodes(lhs, &कुंजियाँ_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? कुंजियाँ_buf : रिक्त,
				 buffer_local, abbr);
		vim_free(कुंजियाँ_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    शून्य
f_maparg(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		प्रत्यागम;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    शून्य
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    स्थिर पूर्णाङ्क
get_map_mode_string(char_u *mode_string, पूर्णाङ्क abbr)
{
    char_u	*p = mode_string;
    पूर्णाङ्क		mode = 0;
    पूर्णाङ्क		tmode;
    पूर्णाङ्क		modec;
    ध्रुव पूर्णाङ्क	MASK_V = MODE_VISUAL | MODE_SELECT;
    ध्रुव पूर्णाङ्क	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    ध्रुव पूर्णाङ्क	MASK_BANG = MODE_जोड़ो | MODE_CMDLINE;

    यदि (*p == NUL)
	p = (char_u *)" ";	// compatibility
    यावत् ((modec = *p++))
    {
	विकल्प (modec)
	{
	    पक्ष 'i': tmode = MODE_जोड़ो;	भङ्ग;
	    पक्ष 'l': tmode = MODE_LANGMAP;	भङ्ग;
	    पक्ष 'c': tmode = MODE_CMDLINE;	भङ्ग;
	    पक्ष 'n': tmode = MODE_NORMAL;	भङ्ग;
	    पक्ष 'x': tmode = MODE_VISUAL;	भङ्ग;
	    पक्ष 's': tmode = MODE_SELECT;	भङ्ग;
	    पक्ष 'o': tmode = MODE_OP_PENDING;	भङ्ग;
	    पक्ष 't': tmode = MODE_TERMINAL;	भङ्ग;
	    पक्ष 'v': tmode = MASK_V;		भङ्ग;
	    पक्ष '!': tmode = MASK_BANG;	भङ्ग;
	    पक्ष ' ': tmode = MASK_MAP;		भङ्ग;
	    स्वभाव:
		      प्रत्यागम 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    यदि ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	प्रत्यागम 0;

    प्रत्यागम mode;
}

/*
 * "mapset()" function
 */
    शून्य
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    पूर्णाङ्क		mode;
    char_u	buf[NUMBUFLEN];
    पूर्णाङ्क		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = रिक्त;
    पूर्णाङ्क		noremap;
    पूर्णाङ्क		व्यंजक;
    पूर्णाङ्क		silent;
    पूर्णाङ्क		buffer;
    scid_T	sid;
    पूर्णाङ्क		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    पूर्णाङ्क		nowait;
    char_u	*arg;
    पूर्णाङ्क		dict_only;
    mapblock_T	*mp_result[2] = {रिक्त, रिक्त};

    यदि (check_secure())
	प्रत्यागम;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    यदि (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	प्रत्यागम;

    यदि (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	यदि (which == रिक्त || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    प्रत्यागम;
	}
    }
    अन्यथा
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	यदि (which == रिक्त)
	    प्रत्यागम;
	is_abbr = (पूर्णाङ्क)tv_get_bool(&argvars[1]);

	यदि (check_for_dict_arg(argvars, 2) == FAIL)
	    प्रत्यागम;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    यदि (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	प्रत्यागम;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    यदि (lhs == रिक्त || lhsraw == रिक्त || rhs == रिक्त)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	प्रत्यागम;
    }
    orig_rhs = rhs;

    noremap = dict_get_संख्या(d, "noremap") ? REMAP_NONE: 0;
    यदि (dict_get_संख्या(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    व्यंजक = dict_get_संख्या(d, "expr") != 0;
    silent = dict_get_संख्या(d, "silent") != 0;
    sid = dict_get_संख्या(d, "sid");
    scriptversion = dict_get_संख्या(d, "scriptversion");
    lnum = dict_get_संख्या(d, "lnum");
    buffer = dict_get_संख्या(d, "buffer");
    nowait = dict_get_संख्या(d, "nowait") != 0;
    // mode from the dict is not used

    यदि (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    अन्यथा
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, रिक्त);

    यदि (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    यदि (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	यदि (arg == रिक्त)
	    प्रत्यागम;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    अन्यथा
    {
	arg = vim_strsave(lhs);
	यदि (arg == रिक्त)
	    प्रत्यागम;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, व्यंजक, sid,
			    scriptversion, lnum, 0);
    यदि (lhsrawalt != रिक्त)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, व्यंजक, sid,
			    scriptversion, lnum, 1);

    यदि (mp_result[0] != रिक्त && mp_result[1] != रिक्त)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#यदि defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
संरचना initmap
{
    char_u	*arg;
    पूर्णाङ्क		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
स्थिर संरचना initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_जोड़ो | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# यदि defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
स्थिर संरचना initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_जोड़ो | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_जोड़ो | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_जोड़ो},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  अन्यथा
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_जोड़ो}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# यदि defined(MACOS_X)
स्थिर संरचना initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_जोड़ो | MODE_CMDLINE},
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
    शून्य
init_mappings(शून्य)
{
#यदि defined(MSWIN) || defined(MACOS_X)
    पूर्णाङ्क		i;

# यदि defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    यदि (!gui.starting)
#  endif
    {
	प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# यदि defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(initmappings); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode, FALSE);
# endif
#endif
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    शून्य
add_map(char_u *map, पूर्णाङ्क mode, पूर्णाङ्क nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    यदि (s != रिक्त)
    {
	(शून्य)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#यदि defined(FEAT_LANGMAP)
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
प्रकारनिर्देश संरचना
{
    पूर्णाङ्क	    from;
    पूर्णाङ्क     to;
} langmap_entry_T;

स्थिर garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    स्थिर शून्य
langmap_set_entry(पूर्णाङ्क from, पूर्णाङ्क to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    पूर्णाङ्क		    a = 0;
    पूर्णाङ्क		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    यावत् (a != b)
    {
	पूर्णाङ्क i = (a + b) / 2;
	पूर्णाङ्क d = entries[i].from - from;

	यदि (d == 0)
	{
	    entries[i].to = to;
	    प्रत्यागम;
	}
	यदि (d < 0)
	    a = i + 1;
	अन्यथा
	    b = i;
    }

    यदि (ga_grow(&langmap_mapga, 1) == FAIL)
	प्रत्यागम;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * परिमाण(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    पूर्णाङ्क
langmap_adjust_mb(पूर्णाङ्क c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    पूर्णाङ्क a = 0;
    पूर्णाङ्क b = langmap_mapga.ga_len;

    यावत् (a != b)
    {
	पूर्णाङ्क i = (a + b) / 2;
	पूर्णाङ्क d = entries[i].from - c;

	यदि (d == 0)
	    प्रत्यागम entries[i].to;  // found matching entry
	यदि (d < 0)
	    a = i + 1;
	अन्यथा
	    b = i;
    }
    प्रत्यागम c;  // no entry found, return "c" unmodified
}

    शून्य
langmap_init(शून्य)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, परिमाण(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    वर्ण *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    पूर्णाङ्क	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    प्रति (p = p_langmap; p[0] != NUL; )
    {
	प्रति (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    यदि (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	यदि (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	अन्यथा
	    p2 = रिक्त;	    // aAbBcCdD form, p2 is NULL
	यावत् (p[0])
	{
	    यदि (p[0] == ',')
	    {
		++p;
		भङ्ग;
	    }
	    यदि (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    यदि (p2 == रिक्त)
	    {
		MB_PTR_ADV(p);
		यदि (p[0] != ',')
		{
		    यदि (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    अन्यथा
	    {
		यदि (p2[0] != ',')
		{
		    यदि (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    यदि (to == NUL)
	    {
		प्र_लिखो(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		प्रत्यागम args->os_errbuf;
	    }

	    यदि (from >= 256)
		langmap_set_entry(from, to);
	    अन्यथा
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    यदि (p2 != रिक्त)
	    {
		MB_PTR_ADV(p2);
		यदि (*p == ';')
		{
		    p = p2;
		    यदि (p[0] != NUL)
		    {
			यदि (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    प्रत्यागम args->os_errbuf;
			}
			++p;
		    }
		    भङ्ग;
		}
	    }
	}
    }

    प्रत्यागम रिक्त;
}
#endif

    स्थिर शून्य
do_exmap(exarg_T *eap, पूर्णाङ्क isabbrev)
{
    पूर्णाङ्क	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    विकल्प (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	पक्ष 1: emsg(_(e_invalid_argument));
		भङ्ग;
	पक्ष 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		भङ्ग;
    }
}

/*
 * ":abbreviate" and friends.
 */
    शून्य
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    शून्य
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    यदि (secure)
    {
	secure = 2;
	संदेश_outtrans(eap->cmd);
	संदेश_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    शून्य
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    शून्य
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    शून्य
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
