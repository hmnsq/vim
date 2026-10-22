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
स्थिर पूर्णांक		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
स्थिर पूर्णांक		map_locked = 0;

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
get_maphash_list(पूर्णांक state, पूर्णांक c)
{
    परत maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(पूर्णांक state, पूर्णांक c)
{
    परत curbuf->b_maphash[MAP_HASH(state, c)];
}

    पूर्णांक
is_maphash_valid(शून्य)
{
    परत maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    स्थिर शून्य
validate_maphash(शून्य)
{
    जर (maphash_valid)
	परत;

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
    vim_free(mp->m_किल्ल्या);
    जर (mp->m_alt != रिक्त)
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
map_mode_to_chars(पूर्णांक mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    जर ((mode & (MODE_घाला | MODE_CMDLINE)) == (MODE_घाला | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    अन्यथा जर (mode & MODE_घाला)
	ga_append(&mapmode, 'i');			// :imap
    अन्यथा जर (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    अन्यथा जर (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    अन्यथा जर ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    अन्यथा
    {
	जर (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	जर (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	जर (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	जर ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	अन्यथा
	{
	    जर (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    जर (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    परत (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    स्थिर शून्य
showmap(
    mapblock_T	*mp,
    पूर्णांक		local)	    // TRUE for buffer-local map
{
    पूर्णांक		len = 1;
    char_u	*mapchars;

    जर (message_filtered(mp->m_किल्ल्या) && message_filtered(mp->m_str))
	परत;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    जर (संदेश_didout || संदेश_silent != 0)
    {
	संदेश_putchar('\n');
	जर (got_int)	    // 'q' typed at MORE prompt
	    जा theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    जर (mapchars != रिक्त)
    {
	संदेश_puts((अक्षर *)mapchars);
	len = (पूर्णांक)STRLEN(mapchars);
	vim_free(mapchars);
    }

    पर्यंत (++len <= 3)
	संदेश_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = संदेश_outtrans_special(mp->m_किल्ल्या, TRUE, 0);
    करा
    {
	संदेश_putchar(' ');		// pad with blanks
	++len;
    } पर्यंत (len < 12);

    जर (mp->m_noremap == REMAP_NONE)
	संदेश_puts_attr("*", HL_ATTR(HLF_8));
    अन्यथा जर (mp->m_noremap == REMAP_SCRIPT)
	संदेश_puts_attr("&", HL_ATTR(HLF_8));
    अन्यथा
	संदेश_putchar(' ');

    जर (local)
	संदेश_putchar('@');
    अन्यथा
	संदेश_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    जर (*mp->m_str == NUL)
	संदेश_puts_attr("<Nop>", HL_ATTR(HLF_8));
    अन्यथा
	संदेश_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    जर (p_verbose > 0)
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
	char_u	    *किल्ल्या,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	पूर्णांक	    noremap,
	पूर्णांक	    nowait,
	पूर्णांक	    silent,
	पूर्णांक	    mode,
	पूर्णांक	    is_abbr,
#ifdef FEAT_EVAL
	पूर्णांक	    अभिव्यक्ती,
	scid_T	    sid,	    // 0 to use current_sctx
	पूर्णांक	    scriptversion,
	linenr_T    lnum,
#endif
	पूर्णांक	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    जर (mp == रिक्त)
	परत रिक्त;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    जर (*किल्ल्या == Ctrl_C)
    {
	जर (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	अन्यथा
	    mapped_ctrl_c |= mode;
    }

    mp->m_किल्ल्या = vim_strsave(किल्ल्या);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    जर (mp->m_किल्ल्या == रिक्त || mp->m_str == रिक्त)
    {
	vim_free(mp->m_किल्ल्या);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	परत रिक्त;
    }
    mp->m_keylen = (पूर्णांक)STRLEN(mp->m_किल्ल्या);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_अभिव्यक्ती = अभिव्यक्ती;
    जर (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    अन्यथा
    {
	mp->m_script_ctx = current_sctx;
	जर (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    जर (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    अन्यथा
    {
	पूर्णांक n = MAP_HASH(mp->m_mode, mp->m_किल्ल्या[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    परत mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    स्थिर शून्य
list_mappings(
	पूर्णांक	keyround,
	पूर्णांक	abbrev,
	पूर्णांक	haskey,
	char_u	*किल्ल्या,
	पूर्णांक	किल्ल्या_len,
	पूर्णांक	mode,
	पूर्णांक	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    जर (p_verbose > 0 && keyround == 1)
    {
	जर (seen_Modify_Other_किल्ल्या)
	    संदेश_puts(_("Seen modifyOtherKeys: true\n"));

	जर (modify_otherkeys_state != MOKS_INITIAL)
	{
	    अक्षर *name = _("Unknown");
	    निवड (modify_otherkeys_state)
	    {
		स्थिती MOKS_INITIAL: थांबा;
		स्थिती MOKS_OFF: name = _("Off"); थांबा;
		स्थिती MOKS_ENABLED: name = _("On"); थांबा;
		स्थिती MOKS_DISABLED: name = _("Disabled"); थांबा;
		स्थिती MOKS_AFTER_T_TE: name = _("Cleared"); थांबा;
	    }

	    अक्षर buf[200];
	    vim_snprintf(buf, आकार(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    संदेश_puts(buf);
	}

	जर (kitty_protocol_state != KKPS_INITIAL)
	{
	    अक्षर *name = _("Unknown");
	    निवड (kitty_protocol_state)
	    {
		स्थिती KKPS_INITIAL: थांबा;
		स्थिती KKPS_OFF: name = _("Off"); थांबा;
		स्थिती KKPS_ENABLED: name = _("On"); थांबा;
		स्थिती KKPS_DISABLED: name = _("Disabled"); थांबा;
		स्थिती KKPS_AFTER_T_TE: name = _("Cleared"); थांबा;
	    }

	    अक्षर buf[200];
	    vim_snprintf(buf, आकार(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    संदेश_puts(buf);
	}
    }

    // need to loop over all global hash lists
    साठी (पूर्णांक hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	जर (abbrev)
	{
	    जर (hash != 0)	// there is only one abbreviation list
		थांबा;
	    mp = curbuf->b_first_abbr;
	}
	अन्यथा
	    mp = curbuf->b_maphash[hash];
	साठी ( ; mp != रिक्त && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    जर (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		जर (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		अन्यथा
		{
		    पूर्णांक n = mp->m_keylen;
		    जर (STRNCMP(mp->m_किल्ल्या, किल्ल्या,
				   (माप_प्रकार)(n < किल्ल्या_len ? n : किल्ल्या_len)) == 0)
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
    पूर्णांक
do_map(
    पूर्णांक		maptype,
    char_u	*arg,
    पूर्णांक		mode,
    पूर्णांक		abbrev)		// not a mapping but an abbreviation
{
    char_u	*किल्ल्या;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {रिक्त, रिक्त};
    char_u	*rhs;
    char_u	*p;
    पूर्णांक		n;
    पूर्णांक		len = 0;	// init for GCC
    पूर्णांक		hasarg;
    पूर्णांक		haskey;
    पूर्णांक		do_print;
    पूर्णांक		keyround;
    char_u	*किल्ल्या_buf = रिक्त;
    char_u	*alt_किल्ल्या_buf = रिक्त;
    char_u	*arg_buf = रिक्त;
    पूर्णांक		retval = 0;
    पूर्णांक		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    पूर्णांक		unique = FALSE;
    पूर्णांक		nowait = FALSE;
    पूर्णांक		silent = FALSE;
    पूर्णांक		special = FALSE;
#ifdef FEAT_EVAL
    पूर्णांक		अभिव्यक्ती = FALSE;
#endif
    पूर्णांक		did_simplify = FALSE;
    पूर्णांक		unmap_lhs_only = FALSE;
    पूर्णांक		noremap;
    char_u      *orig_rhs;

    किल्ल्या = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    जर (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    जर (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    अन्यथा
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    साठी (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	जर (STRNCMP(किल्ल्या, "<buffer>", 8) == 0)
	{
	    किल्ल्या = skipwhite(किल्ल्या + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    सुरूठेवा;
	}

	// Check for "<nowait>": don't wait for more characters.
	जर (STRNCMP(किल्ल्या, "<nowait>", 8) == 0)
	{
	    किल्ल्या = skipwhite(किल्ल्या + 8);
	    nowait = TRUE;
	    सुरूठेवा;
	}

	// Check for "<silent>": don't echo commands.
	जर (STRNCMP(किल्ल्या, "<silent>", 8) == 0)
	{
	    किल्ल्या = skipwhite(किल्ल्या + 8);
	    silent = TRUE;
	    सुरूठेवा;
	}

	// Check for "<special>": accept special keys in <>
	जर (STRNCMP(किल्ल्या, "<special>", 9) == 0)
	{
	    किल्ल्या = skipwhite(किल्ल्या + 9);
	    special = TRUE;
	    सुरूठेवा;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	जर (STRNCMP(किल्ल्या, "<script>", 8) == 0)
	{
	    किल्ल्या = skipwhite(किल्ल्या + 8);
	    noremap = REMAP_SCRIPT;
	    सुरूठेवा;
	}

	// Check for "<expr>": {rhs} is an expression.
	जर (STRNCMP(किल्ल्या, "<expr>", 6) == 0)
	{
	    किल्ल्या = skipwhite(किल्ल्या + 6);
	    अभिव्यक्ती = TRUE;
	    सुरूठेवा;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	जर (STRNCMP(किल्ल्या, "<unique>", 8) == 0)
	{
	    किल्ल्या = skipwhite(किल्ल्या + 8);
	    unique = TRUE;
	    सुरूठेवा;
	}
	थांबा;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = किल्ल्या;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == रिक्त);
    पर्यंत (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	जर ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    जर (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*किल्ल्या != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    जर (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	जा theend;
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
    जर (haskey)
    {
	char_u	*new_किल्ल्या;
	पूर्णांक	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	जर (special)
	    flags |= REPTERM_SPECIAL;
	new_किल्ल्या = replace_termcodes(किल्ल्या, &किल्ल्या_buf, 0, flags, &did_simplify);
	जर (did_simplify)
	    (शून्य)replace_termcodes(किल्ल्या, &alt_किल्ल्या_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, रिक्त);
	किल्ल्या = new_किल्ल्या;
    }
    orig_rhs = rhs;
    जर (hasarg)
    {
	जर (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	अन्यथा
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), रिक्त);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    साठी (keyround = 1; keyround <= 2; ++keyround)
    {
	पूर्णांक	did_it = FALSE;
	पूर्णांक	did_local = FALSE;
	पूर्णांक	keyround1_simplified = keyround == 1 && did_simplify;
	पूर्णांक	round;
	पूर्णांक	अंक_rounds;

	जर (keyround == 2)
	{
	    जर (alt_किल्ल्या_buf == रिक्त)
		थांबा;
	    किल्ल्या = alt_किल्ल्या_buf;
	}
	अन्यथा जर (alt_किल्ल्या_buf != रिक्त && do_print)
	    // when printing always use the not-simplified map
	    किल्ल्या = alt_किल्ल्या_buf;

	// check arguments and translate function keys
	जर (haskey)
	{
	    len = (पूर्णांक)STRLEN(किल्ल्या);
	    जर (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		जा theend;
	    }

	    जर (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		जर (has_mbyte)
		{
		    पूर्णांक		first, last;
		    पूर्णांक		same = -1;
		    char_u	किल्ल्या_unescaped[MAXMAPLEN + 1];
		    माप_प्रकार	किल्ल्या_unescaped_len;

		    mch_memmove(किल्ल्या_unescaped, किल्ल्या, (माप_प्रकार)(len + 1));
		    किल्ल्या_unescaped_len = vim_unescape_csi(किल्ल्या_unescaped);
		    p = किल्ल्या_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    पर्यंत (p < किल्ल्या_unescaped + किल्ल्या_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			जर (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    जर (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			जा theend;
		    }
		}
		अन्यथा जर (vim_iswordc(किल्ल्या[len - 1]))
		    // ends in keyword char
		    साठी (n = 0; n < len - 2; ++n)
			जर (vim_iswordc(किल्ल्या[n]) != vim_iswordc(किल्ल्या[len - 2]))
			{
			    retval = 1;
			    जा theend;
			}
		// An abbreviation cannot contain white space.
		साठी (n = 0; n < len; ++n)
		    जर (VIM_ISWHITE(किल्ल्या[n]))
		    {
			retval = 1;
			जा theend;
		    }
	    }
	}

	जर (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	जर (do_print)
	    संदेश_start();

	// Check if a new local mapping wasn't already defined globally.
	जर (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    साठी (पूर्णांक hash = 0; hash < 256 && !got_int; ++hash)
	    {
		जर (abbrev)
		{
		    जर (hash != 0)	// there is only one abbreviation list
			थांबा;
		    mp = first_abbr;
		}
		अन्यथा
		    mp = maphash[hash];
		साठी ( ; mp != रिक्त && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    जर ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_किल्ल्या, किल्ल्या, (माप_प्रकार)len) == 0)
		    {
			जर (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_किल्ल्या);
			अन्यथा
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_किल्ल्या);
			retval = 5;
			जा theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	जर (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, किल्ल्या, len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	अंक_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	साठी (round = 0; round < अंक_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    साठी (पूर्णांक hash = 0; hash < 256 && !got_int; ++hash)
	    {
		जर (abbrev)
		{
		    जर (hash > 0)	// there is only one abbreviation list
			थांबा;
		    mpp = abbr_table;
		}
		अन्यथा
		    mpp = &(map_table[hash]);
		साठी (mp = *mpp; mp != रिक्त && !got_int; mp = *mpp)
		{

		    जर ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			सुरूठेवा;
		    }
		    जर (!haskey)	// show all entries
		    {
			जर (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    अन्यथा	// do we have a match?
		    {
			जर (round)	// second round: Try unmap "rhs" string
			{
			    n = (पूर्णांक)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			अन्यथा
			{
			    n = mp->m_keylen;
			    p = mp->m_किल्ल्या;
			}
			जर (STRNCMP(p, किल्ल्या, (माप_प्रकार)(n < len ? n : len)) == 0)
			{
			    जर (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				जर (n != len && (!abbrev || round || n > len
					       || *skipwhite(किल्ल्या + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    सुरूठेवा;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				जर (keyround1_simplified && !mp->m_simplified)
				    थांबा;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    अन्यथा जर (!hasarg)	// show matching entry
			    {
				जर (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    अन्यथा जर (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				सुरूठेवा;
			    }
			    अन्यथा जर (unique)
			    {
				जर (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				अन्यथा
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				जा theend;
			    }
			    अन्यथा
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				जर (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    जर (newstr == रिक्त)
				    {
					retval = 4;		// no mem
					जा theend;
				    }
				    जर (mp->m_alt != रिक्त)
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
				    mp->m_अभिव्यक्ती = अभिव्यक्ती;
				    mp->m_script_ctx = current_sctx;
				    जर (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    जर (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				सुरूठेवा;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    पूर्णांक new_hash = MAP_HASH(mp->m_mode, mp->m_किल्ल्या[0]);
			    जर (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_next;
				mp->m_next = map_table[new_hash];
				map_table[new_hash] = mp;

				सुरूठेवा;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_next);
		}
	    }
	}

	जर (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    जर (!did_it)
	    {
		जर (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    अन्यथा जर (*किल्ल्या == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		जर (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		अन्यथा
		    mapped_ctrl_c &= ~mode;
	    }
	    सुरूठेवा;
	}

	जर (!haskey || !hasarg)
	{
	    // print entries
	    जर (!did_it && !did_local)
	    {
		जर (abbrev)
		    संदेश(_("No abbreviation found"));
		अन्यथा
		    संदेश(_("No mapping found"));
	    }
	    जा theend;    // listing finished
	}

	जर (did_it)
	    सुरूठेवा;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, किल्ल्या,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    अभिव्यक्ती, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	जर (mp_result[keyround - 1] == रिक्त)
	{
	    retval = 4;	    // no mem
	    जा theend;
	}
    }

    जर (mp_result[0] != रिक्त && mp_result[1] != रिक्त)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(किल्ल्या_buf);
    vim_free(alt_किल्ल्या_buf);
    vim_free(arg_buf);
    परत retval;
}

/*
 * Get the mapping mode from the command name.
 */
    स्थिर पूर्णांक
get_map_mode(char_u **cmdp, पूर्णांक forceit)
{
    char_u	*p;
    पूर्णांक		modec;
    पूर्णांक		mode;

    p = *cmdp;
    modec = *p++;
    जर (modec == 'i')
	mode = MODE_घाला;				// :imap
    अन्यथा जर (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    अन्यथा जर (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    अन्यथा जर (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    अन्यथा जर (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    अन्यथा जर (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    अन्यथा जर (modec == 's')
	mode = MODE_SELECT;			// :smap
    अन्यथा जर (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    अन्यथा जर (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    अन्यथा
    {
	--p;
	जर (forceit)
	    mode = MODE_घाला | MODE_CMDLINE;		// :map !
	अन्यथा
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    परत mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    स्थिर शून्य
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    पूर्णांक		forceit,
    पूर्णांक		abbr)
{
    पूर्णांक		mode;
    पूर्णांक		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    जर (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	परत;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    स्थिर पूर्णांक
is_map_locked(शून्य)
{
    जर (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	परत TRUE;
    }
    परत FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    शून्य
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    पूर्णांक		mode,		// mode in which to delete
    पूर्णांक		local,		// TRUE for buffer-local mappings
    पूर्णांक		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    पूर्णांक		hash;
    पूर्णांक		new_hash;

    जर (is_map_locked())
	परत;

    validate_maphash();

    साठी (hash = 0; hash < 256; ++hash)
    {
	जर (abbr)
	{
	    जर (hash > 0)	// there is only one abbrlist
		थांबा;
	    जर (local)
		mpp = &buf->b_first_abbr;
	    अन्यथा
		mpp = &first_abbr;
	}
	अन्यथा
	{
	    जर (local)
		mpp = &buf->b_maphash[hash];
	    अन्यथा
		mpp = &maphash[hash];
	}
	पर्यंत (*mpp != रिक्त)
	{
	    mp = *mpp;
	    जर (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		जर (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    सुरूठेवा;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_किल्ल्या[0]);
		जर (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    जर (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    अन्यथा
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    सुरूठेवा;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

#जर defined(FEAT_EVAL)
    पूर्णांक
mode_str2flags(char_u *modechars)
{
    पूर्णांक		mode = 0;

    जर (vim_strchr(modechars, 'n') != रिक्त)
	mode |= MODE_NORMAL;
    जर (vim_strchr(modechars, 'v') != रिक्त)
	mode |= MODE_VISUAL | MODE_SELECT;
    जर (vim_strchr(modechars, 'x') != रिक्त)
	mode |= MODE_VISUAL;
    जर (vim_strchr(modechars, 's') != रिक्त)
	mode |= MODE_SELECT;
    जर (vim_strchr(modechars, 'o') != रिक्त)
	mode |= MODE_OP_PENDING;
    जर (vim_strchr(modechars, 'i') != रिक्त)
	mode |= MODE_घाला;
    जर (vim_strchr(modechars, 'l') != रिक्त)
	mode |= MODE_LANGMAP;
    जर (vim_strchr(modechars, 'c') != रिक्त)
	mode |= MODE_CMDLINE;

    परत mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    पूर्णांक
map_to_exists(char_u *str, char_u *modechars, पूर्णांक abbr)
{
    char_u	*rhs;
    char_u	*buf;
    पूर्णांक		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, रिक्त);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    परत retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    पूर्णांक
map_to_exists_mode(char_u *rhs, पूर्णांक mode, पूर्णांक abbr)
{
    mapblock_T	*mp;
    पूर्णांक		hash;
    पूर्णांक		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    साठी (;;)
    {
	साठी (hash = 0; hash < 256; ++hash)
	{
	    जर (abbr)
	    {
		जर (hash > 0)		// there is only one abbr list
		    थांबा;
		जर (exp_buffer)
		    mp = curbuf->b_first_abbr;
		अन्यथा
		    mp = first_abbr;
	    }
	    अन्यथा जर (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    अन्यथा
		mp = maphash[hash];
	    साठी (; mp; mp = mp->m_next)
	    {
		जर ((mp->m_mode & mode)
			&& म_माला((अक्षर *)mp->m_str, (अक्षर *)rhs) != रिक्त)
		    परत TRUE;
	    }
	}
	जर (exp_buffer)
	    थांबा;
	exp_buffer = TRUE;
    }

    परत FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
स्थिर पूर्णांक	expand_mapmodes = 0;
स्थिर पूर्णांक	expand_isabbrev = 0;
स्थिर पूर्णांक	expand_buffer = FALSE;

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
    पूर्णांक		c;
    पूर्णांक		modifiers;
    पूर्णांक		cpo_bslash;
    पूर्णांक		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != रिक्त);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != रिक्त);

    साठी (; *str; ++str)
    {
	c = *str;
	जर (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    जर (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    जर (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		जर (cpo_special)
		{
		    ga_clear(&ga);
		    परत रिक्त;
		}
		c = TO_SPECIAL(str[1], str[2]);
		जर (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    जर (IS_SPECIAL(c) || modifiers)	// special key
	    {
		जर (cpo_special)
		{
		    ga_clear(&ga);
		    परत रिक्त;
		}
		ga_concat(&ga, get_special_किल्ली_name(c, modifiers));
		सुरूठेवा; // for (str)
	    }
	}
	जर (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	जर (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    परत (char_u *)(ga.ga_data);
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
    पूर्णांक		forceit,	// TRUE if '!' given
    पूर्णांक		isabbrev,	// TRUE if abbreviation
    पूर्णांक		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    जर (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    अन्यथा
    {
	जर (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	अन्यथा
	{
	    expand_mapmodes = MODE_घाला | MODE_CMDLINE;
	    जर (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	साठी (;;)
	{
	    जर (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		सुरूठेवा;
	    }
	    जर (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		सुरूठेवा;
	    }
	    जर (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		सुरूठेवा;
	    }
	    जर (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		सुरूठेवा;
	    }
	    जर (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		सुरूठेवा;
	    }
#ifdef FEAT_EVAL
	    जर (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		सुरूठेवा;
	    }
	    जर (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		सुरूठेवा;
	    }
#endif
	    थांबा;
	}
	xp->xp_pattern = arg;
    }

    परत रिक्त;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    पूर्णांक
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    पूर्णांक		*अंक_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    पूर्णांक		hash;
    पूर्णांक		मोजणी;
    char_u	*p;
    पूर्णांक		i;
    पूर्णांक		fuzzy;
    पूर्णांक		match;
    पूर्णांक		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *अंक_Matches = 0;		    // return values in case of FAIL
    *matches = रिक्त;

    जर (!fuzzy)
	ga_init2(&ga, आकार(अक्षर *), 3);
    अन्यथा
	ga_init2(&ga, आकार(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    साठी (i = 0; i < 7; ++i)
    {
	जर (i == 0)
	    p = (char_u *)"<silent>";
	अन्यथा जर (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	अन्यथा जर (i == 2)
	    p = (char_u *)"<script>";
	अन्यथा जर (i == 3)
	    p = (char_u *)"<expr>";
#endif
	अन्यथा जर (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	अन्यथा जर (i == 5)
	    p = (char_u *)"<nowait>";
	अन्यथा जर (i == 6)
	    p = (char_u *)"<special>";
	अन्यथा
	    सुरूठेवा;

	जर (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	अन्यथा
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	जर (!match)
	    सुरूठेवा;

	जर (ga_grow(&ga, 1) == FAIL)
	    थांबा;

	जर (fuzzy)
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

    साठी (hash = 0; hash < 256; ++hash)
    {
	जर (expand_isabbrev)
	{
	    जर (hash > 0)	// only one abbrev list
		थांबा; // for (hash)
	    mp = first_abbr;
	}
	अन्यथा जर (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	अन्यथा
	    mp = maphash[hash];
	साठी (; mp; mp = mp->m_next)
	{
	    जर (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		सुरूठेवा;

	    p = translate_mapping(mp->m_किल्ल्या);
	    जर (p == रिक्त)
		सुरूठेवा;

	    जर (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    अन्यथा
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    जर (!match)
	    {
		vim_free(p);
		सुरूठेवा;
	    }

	    जर (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		थांबा;
	    }

	    जर (fuzzy)
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

    जर (ga.ga_len == 0)
	परत FAIL;

    जर (!fuzzy)
    {
	*matches = ga.ga_data;
	*अंक_Matches = ga.ga_len;
    }
    अन्यथा
    {
	जर (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    परत FAIL;
	*अंक_Matches = ga.ga_len;
    }

    मोजणी = *अंक_Matches;
    जर (मोजणी > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	जर (!fuzzy)
	    sort_strings(*matches, मोजणी);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + मोजणी;

	पर्यंत (ptr2 < ptr3)
	{
	    जर (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    अन्यथा
	    {
		vim_free(*ptr2++);
		मोजणी--;
	    }
	}
    }

    *अंक_Matches = मोजणी;
    परत (मोजणी == 0 ? FAIL : OK);
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
    पूर्णांक
check_abbr(
    पूर्णांक		c,
    char_u	*ptr,
    पूर्णांक		col,
    पूर्णांक		mincol)
{
    पूर्णांक		len;
    पूर्णांक		scol;		// starting column of the abbr.
    पूर्णांक		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    पूर्णांक		clen = 0;	// length in characters
    पूर्णांक		is_id = TRUE;
    पूर्णांक		vim_abbr;

    जर (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	परत FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    जर (noremap_किल्ल्या() && c != Ctrl_RSB)
	परत FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    जर (col == 0)				// cannot be an abbr.
	परत FALSE;

    जर (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	जर (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	अन्यथा
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    जर (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	पर्यंत (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    जर (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		थांबा;
	    }
	    ++clen;
	}
	scol = (पूर्णांक)(p - ptr);
    }
    अन्यथा
    {
	जर (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	अन्यथा
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    जर (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	साठी (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    जर (scol < mincol)
	scol = mincol;
    जर (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	जर (mp == रिक्त)
	{
	    mp = mp2;
	    mp2 = रिक्त;
	}
	साठी ( ; mp; mp->m_next == रिक्त
				  ? (mp = mp2, mp2 = रिक्त) : (mp = mp->m_next))
	{
	    पूर्णांक		qlen = mp->m_keylen;
	    char_u	*q = mp->m_किल्ल्या;
	    पूर्णांक		match;

	    जर (vim_strbyte(mp->m_किल्ल्या, K_SPECIAL) != रिक्त)
	    {
		char_u *qe = vim_strsave(mp->m_किल्ल्या);

		// might have CSI escaped mp->m_keys
		जर (qe != रिक्त)
		{
		    q = qe;
		    qlen = (पूर्णांक)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (माप_प्रकार)len) == 0;
	    जर (q != mp->m_किल्ल्या)
		vim_free(q);
	    जर (match)
		थांबा;
	}
	जर (mp != रिक्त)
	{
	    पूर्णांक	noremap;
	    पूर्णांक silent;
#ifdef FEAT_EVAL
	    पूर्णांक अभिव्यक्ती;
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
	    जर (c != Ctrl_RSB)
	    {
					// special key code, split up
		जर (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		अन्यथा
		{
		    जर (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    जर (has_mbyte)
		    {
			पूर्णांक	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			जर (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			जर (escaped != रिक्त)
			{
			    newlen = (पूर्णांक)STRLEN(escaped);
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
	    अभिव्यक्ती = mp->m_अभिव्यक्ती;

	    जर (अभिव्यक्ती)
		s = eval_map_अभिव्यक्ती(mp, c);
	    अन्यथा
#endif
		s = mp->m_str;
	    जर (s != रिक्त)
	    {
					// insert the to string
		(शून्य)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (पूर्णांक)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		जर (अभिव्यक्ती)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    जर (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    पर्यंत (len-- > 0)		// delete the from string
		(शून्य)ins_typebuf(tb, 1, 0, TRUE, silent);
	    परत TRUE;
	}
    }
    परत FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_अभिव्यक्ती(
    mapblock_T	*mp,
    पूर्णांक		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*अभिव्यक्ती;
    pos_T	save_cursor;
    पूर्णांक		save_संदेश_col;
    पूर्णांक		save_संदेश_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    पूर्णांक		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    अभिव्यक्ती = vim_strsave(mp->m_str);
    जर (अभिव्यक्ती == रिक्त)
	परत रिक्त;
    vim_unescape_csi(अभिव्यक्ती);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_संदेश_col = संदेश_col;
    save_संदेश_row = संदेश_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    जर (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(अभिव्यक्ती, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    संदेश_col = save_संदेश_col;
    संदेश_row = save_संदेश_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(अभिव्यक्ती);

    जर (p == रिक्त)
	परत रिक्त;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    परत res;
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
    जर (res == रिक्त)
	परत रिक्त;

    d = res;
    साठी (s = p; *s != NUL; )
    {
	जर ((s[0] == K_SPECIAL
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

    परत res;
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

    पर्यंत (*s != NUL)
    {
	जर (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	अन्यथा जर ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (पूर्णांक)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	अन्यथा
	    *d++ = *s++;
    }
    *d = NUL;
    परत (माप_प्रकार)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    पूर्णांक
makemap(
    खाता	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    अक्षर	*cmd;
    पूर्णांक		abbr;
    पूर्णांक		hash;
    पूर्णांक		did_cpo = FALSE;
    पूर्णांक		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    साठी (abbr = 0; abbr < 2; ++abbr)
	साठी (hash = 0; hash < 256; ++hash)
	{
	    जर (abbr)
	    {
		जर (hash > 0)		// there is only one abbr list
		    थांबा;
		जर (buf != रिक्त)
		    mp = buf->b_first_abbr;
		अन्यथा
		    mp = first_abbr;
	    }
	    अन्यथा
	    {
		जर (buf != रिक्त)
		    mp = buf->b_maphash[hash];
		अन्यथा
		    mp = maphash[hash];
	    }

	    साठी ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		जर (mp->m_simplified)
		    सुरूठेवा;

		// skip script-local mappings
		जर (mp->m_noremap == REMAP_SCRIPT)
		    सुरूठेवा;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		साठी (p = mp->m_str; *p != NUL; ++p)
		    जर (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (पूर्णांक)KE_SNR)
			थांबा;
		जर (*p != NUL)
		    सुरूठेवा;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		जर (abbr)
		    cmd = "abbr";
		अन्यथा
		    cmd = "map";
		निवड (mp->m_mode)
		{
		    स्थिती MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			थांबा;
		    स्थिती MODE_NORMAL:
			c1 = 'n';
			थांबा;
		    स्थिती MODE_VISUAL:
			c1 = 'x';
			थांबा;
		    स्थिती MODE_SELECT:
			c1 = 's';
			थांबा;
		    स्थिती MODE_OP_PENDING:
			c1 = 'o';
			थांबा;
		    स्थिती MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			थांबा;
		    स्थिती MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			थांबा;
		    स्थिती MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			थांबा;
		    स्थिती MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			थांबा;
		    स्थिती MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			थांबा;
		    स्थिती MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			थांबा;
		    स्थिती MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			थांबा;
		    स्थिती MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			थांबा;
		    स्थिती MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			थांबा;
		    स्थिती MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			थांबा;
		    स्थिती MODE_CMDLINE | MODE_घाला:
			जर (!abbr)
			    cmd = "map!";
			थांबा;
		    स्थिती MODE_CMDLINE:
			c1 = 'c';
			थांबा;
		    स्थिती MODE_घाला:
			c1 = 'i';
			थांबा;
		    स्थिती MODE_LANGMAP:
			c1 = 'l';
			थांबा;
		    स्थिती MODE_TERMINAL:
			c1 = 't';
			थांबा;
		    पूर्वनिर्धारित:
			iemsg(e_makemap_illegal_mode);
			परत FAIL;
		}
		करा	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    जर (!did_cpo)
		    {
			जर (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			अन्यथा
			    साठी (i = 0; i < 2; ++i)
				साठी (p = (i ? mp->m_str : mp->m_किल्ल्या); *p; ++p)
				    जर (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			जर (did_cpo)
			{
			    जर (ख_लिखो(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || ख_लिखो(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				परत FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    जर (mp->m_अभिव्यक्ती && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && ख_माला_दो("legacy ", fd) < 0)
			परत FAIL;
#endif
		    जर (c1 && अ_दो(c1, fd) < 0)
			परत FAIL;
		    जर (mp->m_noremap != REMAP_YES && ख_लिखो(fd, "nore") < 0)
			परत FAIL;
		    जर (ख_माला_दो(cmd, fd) < 0)
			परत FAIL;
		    जर (buf != रिक्त && ख_माला_दो(" <buffer>", fd) < 0)
			परत FAIL;
		    जर (mp->m_nowait && ख_माला_दो(" <nowait>", fd) < 0)
			परत FAIL;
		    जर (mp->m_silent && ख_माला_दो(" <silent>", fd) < 0)
			परत FAIL;
#ifdef FEAT_EVAL
		    जर (mp->m_noremap == REMAP_SCRIPT
						 && ख_माला_दो("<script>", fd) < 0)
			परत FAIL;
		    जर (mp->m_अभिव्यक्ती && ख_माला_दो(" <expr>", fd) < 0)
			परत FAIL;
#endif

		    जर (       अ_दो(' ', fd) < 0
			    || put_escstr(fd, mp->m_किल्ल्या, 0) == FAIL
			    || अ_दो(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			परत FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} पर्यंत (c1 != NUL);
	    }
	}

    जर (did_cpo)
	जर (ख_लिखो(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    परत FAIL;
    परत OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णांक
put_escstr(खाता *fd, char_u *strstart, पूर्णांक what)
{
    char_u	*str = strstart;
    पूर्णांक		c;
    पूर्णांक		modifiers;

    // :map xx <Nop>
    जर (*str == NUL && what == 1)
    {
	जर (ख_लिखो(fd, "<Nop>") < 0)
	    परत FAIL;
	परत OK;
    }

    साठी ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	जर (p != रिक्त)
	{
	    पर्यंत (*p != NUL)
		जर (ख_अक्षर_दो(*p++, fd) < 0)
		    परत FAIL;
	    --str;
	    सुरूठेवा;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	जर (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    जर (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		जर (p == रिक्त)
		    c = *str;
		अन्यथा
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    जर (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    जर (IS_SPECIAL(c) || modifiers)	// special key
	    {
		जर (ख_माला_दो((अक्षर *)get_special_किल्ली_name(c, modifiers), fd) < 0)
		    परत FAIL;
		सुरूठेवा;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	जर (c == NL)
	{
	    जर (what == 2)
	    {
		जर (ख_लिखो(fd, "\\\026\n") < 0)
		    परत FAIL;
	    }
	    अन्यथा
	    {
		जर (ख_लिखो(fd, "<NL>") < 0)
		    परत FAIL;
	    }
	    सुरूठेवा;
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
	जर (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    जर (अ_दो('\\', fd) < 0)
		परत FAIL;
	}
	अन्यथा जर (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    जर (अ_दो(Ctrl_V, fd) < 0)
		परत FAIL;
	}
	जर (अ_दो(c, fd) < 0)
	    परत FAIL;
    }
    परत OK;
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
    पूर्णांक		i;
    char_u	buf[3];
    पूर्णांक		abbr;
    पूर्णांक		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_ढकला(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    साठी (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	साठी (abbr = 0; abbr <= 1; ++abbr)
	    साठी (hash = 0; hash < 256; ++hash)
	    {
		जर (abbr)
		{
		    जर (hash)	    // there is only one abbr list
			थांबा;
		    जर (bp != रिक्त)
			mp = bp->b_first_abbr;
		    अन्यथा
			mp = first_abbr;
		}
		अन्यथा
		{
		    जर (bp != रिक्त)
			mp = bp->b_maphash[hash];
		    अन्यथा
			mp = maphash[hash];
		}
		साठी ( ; mp != रिक्त; mp = mp->m_next)
		{
		    साठी (i = 0; i <= 1; ++i)	// do this twice
		    {
			जर (i == 0)
			    p = mp->m_किल्ल्या;	// once for the "from" part
			अन्यथा
			    p = mp->m_str;	// and once for the "to" part
			पर्यंत (*p)
			{
			    जर (*p == K_SPECIAL)
			    {
				++p;
				जर (*p < 128)   // for "normal" tcap entries
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
	जर (bp == रिक्त)
	    थांबा;
    }
    ESTACK_CHECK_NOW;
    estack_काढा();
}

#जर defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*किल्ल्या,
    पूर्णांक		mode,
    पूर्णांक		exact,		// require exact match
    पूर्णांक		ign_mod,	// ignore preceding modifier
    पूर्णांक		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    पूर्णांक		*local_ptr)	// return: buffer-local mapping or NULL
{
    पूर्णांक		hash;
    पूर्णांक		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    पूर्णांक		local;

    validate_maphash();

    len = (पूर्णांक)STRLEN(किल्ल्या);
    साठी (local = 1; local >= 0; --local)
	// loop over all hash lists
	साठी (hash = 0; hash < 256; ++hash)
	{
	    जर (abbr)
	    {
		जर (hash > 0)		// there is only one list.
		    थांबा;
		जर (local)
		    mp = curbuf->b_first_abbr;
		अन्यथा
		    mp = first_abbr;
	    }
	    अन्यथा जर (local)
		mp = curbuf->b_maphash[hash];
	    अन्यथा
		mp = maphash[hash];
	    साठी ( ; mp != रिक्त; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		जर ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    जर (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    अन्यथा
			minlen = len;
		    s = mp->m_किल्ल्या;
		    जर (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			जर (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    जर (STRNCMP(s, किल्ल्या, minlen) == 0)
		    {
			जर (mp_ptr != रिक्त)
			    *mp_ptr = mp;
			जर (local_ptr != रिक्त)
			    *local_ptr = local;
			परत mp->m_str;
		    }
		}
	    }
	}

    परत रिक्त;
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
    पूर्णांक		abbr = FALSE;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	परत;

    name = tv_get_string(&argvars[0]);
    जर (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    अन्यथा
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	जर (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (पूर्णांक)tv_get_bool(&argvars[2]);
    }

    जर (map_to_exists(name, mode, abbr))
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
	पूर्णांक	    buffer_local,   // false if not buffer local mapping
	पूर्णांक	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_किल्ल्या, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_किल्ल्या);
    जर (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_संख्या(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_संख्या(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_संख्या(dict, "expr", mp->m_अभिव्यक्ती ? 1L : 0L);
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
get_maparg(typval_T *argvars, typval_T *rettv, पूर्णांक exact)
{
    char_u	*किल्ल्या;
    char_u	*किल्ल्या_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*किल्ल्या_buf = रिक्त;
    char_u	*alt_किल्ल्या_buf = रिक्त;
    पूर्णांक		did_simplify = FALSE;
    char_u	*rhs;
    पूर्णांक		mode;
    पूर्णांक		abbr = FALSE;
    पूर्णांक		get_dict = FALSE;
    mapblock_T	*mp = रिक्त;
    पूर्णांक		buffer_local;
    पूर्णांक		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    किल्ल्या = tv_get_string(&argvars[0]);
    जर (*किल्ल्या == NUL)
	परत;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	जर (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (पूर्णांक)tv_get_bool(&argvars[2]);
	    जर (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (पूर्णांक)tv_get_bool(&argvars[3]);
	}
    }
    अन्यथा
	which = (char_u *)"";
    जर (which == रिक्त)
	परत;

    mode = get_map_mode(&which, 0);

    किल्ल्या_simplified = replace_termcodes(किल्ल्या, &किल्ल्या_buf, 0, flags,
								&did_simplify);
    rhs = check_map(किल्ल्या_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    जर (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(शून्य)replace_termcodes(किल्ल्या, &alt_किल्ल्या_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, रिक्त);
	rhs = check_map(alt_किल्ल्या_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    जर (!get_dict)
    {
	// Return a string.
	जर (rhs != रिक्त)
	{
	    जर (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    अन्यथा
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    अन्यथा जर (rettv_dict_alloc(rettv) == OK && rhs != रिक्त)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? किल्ल्या_simplified : रिक्त,
			  buffer_local, abbr);

    vim_free(किल्ल्या_buf);
    vim_free(alt_किल्ल्या_buf);
}

/*
 * "maplist()" function
 */
    शून्य
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    पूर्णांक		buffer_local;
    char_u	*किल्ल्या_buf;
    पूर्णांक		did_simplify;
    पूर्णांक		hash;
    char_u	*lhs;
    स्थिरांक पूर्णांक	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    पूर्णांक		abbr = FALSE;

    जर (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	परत;
    जर (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    साठी (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	साठी (hash = 0; hash < 256; ++hash)
	{
	    जर (abbr)
	    {
		जर (hash > 0)		// there is only one abbr list
		    थांबा;
		जर (buffer_local)
		    mp = curbuf->b_first_abbr;
		अन्यथा
		    mp = first_abbr;
	    }
	    अन्यथा जर (buffer_local)
		mp = curbuf->b_maphash[hash];
	    अन्यथा
		mp = maphash[hash];
	    साठी (; mp; mp = mp->m_next)
	    {
		जर (mp->m_simplified)
		    सुरूठेवा;
		जर ((d = dict_alloc()) == रिक्त)
		    परत;
		जर (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    परत;
		}

		किल्ल्या_buf = रिक्त;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_किल्ल्या, TRUE, FALSE);
		(शून्य)replace_termcodes(lhs, &किल्ल्या_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? किल्ल्या_buf : रिक्त,
				 buffer_local, abbr);
		vim_free(किल्ल्या_buf);
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
    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		परत;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    शून्य
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	परत;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    स्थिर पूर्णांक
get_map_mode_string(char_u *mode_string, पूर्णांक abbr)
{
    char_u	*p = mode_string;
    पूर्णांक		mode = 0;
    पूर्णांक		tmode;
    पूर्णांक		modec;
    स्थिरांक पूर्णांक	MASK_V = MODE_VISUAL | MODE_SELECT;
    स्थिरांक पूर्णांक	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    स्थिरांक पूर्णांक	MASK_BANG = MODE_घाला | MODE_CMDLINE;

    जर (*p == NUL)
	p = (char_u *)" ";	// compatibility
    पर्यंत ((modec = *p++))
    {
	निवड (modec)
	{
	    स्थिती 'i': tmode = MODE_घाला;	थांबा;
	    स्थिती 'l': tmode = MODE_LANGMAP;	थांबा;
	    स्थिती 'c': tmode = MODE_CMDLINE;	थांबा;
	    स्थिती 'n': tmode = MODE_NORMAL;	थांबा;
	    स्थिती 'x': tmode = MODE_VISUAL;	थांबा;
	    स्थिती 's': tmode = MODE_SELECT;	थांबा;
	    स्थिती 'o': tmode = MODE_OP_PENDING;	थांबा;
	    स्थिती 't': tmode = MODE_TERMINAL;	थांबा;
	    स्थिती 'v': tmode = MASK_V;		थांबा;
	    स्थिती '!': tmode = MASK_BANG;	थांबा;
	    स्थिती ' ': tmode = MASK_MAP;		थांबा;
	    पूर्वनिर्धारित:
		      परत 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    जर ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	परत 0;

    परत mode;
}

/*
 * "mapset()" function
 */
    शून्य
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    पूर्णांक		mode;
    char_u	buf[NUMBUFLEN];
    पूर्णांक		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = रिक्त;
    पूर्णांक		noremap;
    पूर्णांक		अभिव्यक्ती;
    पूर्णांक		silent;
    पूर्णांक		buffer;
    scid_T	sid;
    पूर्णांक		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    पूर्णांक		nowait;
    char_u	*arg;
    पूर्णांक		dict_only;
    mapblock_T	*mp_result[2] = {रिक्त, रिक्त};

    जर (check_secure())
	परत;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    जर (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	परत;

    जर (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	जर (which == रिक्त || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    परत;
	}
    }
    अन्यथा
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	जर (which == रिक्त)
	    परत;
	is_abbr = (पूर्णांक)tv_get_bool(&argvars[1]);

	जर (check_for_dict_arg(argvars, 2) == FAIL)
	    परत;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    जर (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	परत;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    जर (lhs == रिक्त || lhsraw == रिक्त || rhs == रिक्त)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	परत;
    }
    orig_rhs = rhs;

    noremap = dict_get_संख्या(d, "noremap") ? REMAP_NONE: 0;
    जर (dict_get_संख्या(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    अभिव्यक्ती = dict_get_संख्या(d, "expr") != 0;
    silent = dict_get_संख्या(d, "silent") != 0;
    sid = dict_get_संख्या(d, "sid");
    scriptversion = dict_get_संख्या(d, "scriptversion");
    lnum = dict_get_संख्या(d, "lnum");
    buffer = dict_get_संख्या(d, "buffer");
    nowait = dict_get_संख्या(d, "nowait") != 0;
    // mode from the dict is not used

    जर (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    अन्यथा
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, रिक्त);

    जर (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    जर (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	जर (arg == रिक्त)
	    परत;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    अन्यथा
    {
	arg = vim_strsave(lhs);
	जर (arg == रिक्त)
	    परत;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, अभिव्यक्ती, sid,
			    scriptversion, lnum, 0);
    जर (lhsrawalt != रिक्त)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, अभिव्यक्ती, sid,
			    scriptversion, lnum, 1);

    जर (mp_result[0] != रिक्त && mp_result[1] != रिक्त)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#जर defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
संरचना initmap
{
    char_u	*arg;
    पूर्णांक		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
स्थिर संरचना initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_घाला | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# जर defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
स्थिर संरचना initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_घाला | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_घाला | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_घाला},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  अन्यथा
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_घाला}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# जर defined(MACOS_X)
स्थिर संरचना initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_घाला | MODE_CMDLINE},
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
#जर defined(MSWIN) || defined(MACOS_X)
    पूर्णांक		i;

# जर defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    जर (!gui.starting)
#  endif
    {
	साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# जर defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(initmappings); ++i)
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
add_map(char_u *map, पूर्णांक mode, पूर्णांक nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    जर (s != रिक्त)
    {
	(शून्य)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#जर defined(FEAT_LANGMAP)
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
प्रकारव्याख्या संरचना
{
    पूर्णांक	    from;
    पूर्णांक     to;
} langmap_entry_T;

स्थिर garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    स्थिर शून्य
langmap_set_entry(पूर्णांक from, पूर्णांक to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    पूर्णांक		    a = 0;
    पूर्णांक		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    पर्यंत (a != b)
    {
	पूर्णांक i = (a + b) / 2;
	पूर्णांक d = entries[i].from - from;

	जर (d == 0)
	{
	    entries[i].to = to;
	    परत;
	}
	जर (d < 0)
	    a = i + 1;
	अन्यथा
	    b = i;
    }

    जर (ga_grow(&langmap_mapga, 1) == FAIL)
	परत;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * आकार(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    पूर्णांक
langmap_adjust_mb(पूर्णांक c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    पूर्णांक a = 0;
    पूर्णांक b = langmap_mapga.ga_len;

    पर्यंत (a != b)
    {
	पूर्णांक i = (a + b) / 2;
	पूर्णांक d = entries[i].from - c;

	जर (d == 0)
	    परत entries[i].to;  // found matching entry
	जर (d < 0)
	    a = i + 1;
	अन्यथा
	    b = i;
    }
    परत c;  // no entry found, return "c" unmodified
}

    शून्य
langmap_init(शून्य)
{
    पूर्णांक i;

    साठी (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, आकार(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    अक्षर *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    पूर्णांक	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    साठी (p = p_langmap; p[0] != NUL; )
    {
	साठी (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    जर (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	जर (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	अन्यथा
	    p2 = रिक्त;	    // aAbBcCdD form, p2 is NULL
	पर्यंत (p[0])
	{
	    जर (p[0] == ',')
	    {
		++p;
		थांबा;
	    }
	    जर (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    जर (p2 == रिक्त)
	    {
		MB_PTR_ADV(p);
		जर (p[0] != ',')
		{
		    जर (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    अन्यथा
	    {
		जर (p2[0] != ',')
		{
		    जर (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    जर (to == NUL)
	    {
		प्र_लिखो(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		परत args->os_errbuf;
	    }

	    जर (from >= 256)
		langmap_set_entry(from, to);
	    अन्यथा
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    जर (p2 != रिक्त)
	    {
		MB_PTR_ADV(p2);
		जर (*p == ';')
		{
		    p = p2;
		    जर (p[0] != NUL)
		    {
			जर (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    परत args->os_errbuf;
			}
			++p;
		    }
		    थांबा;
		}
	    }
	}
    }

    परत रिक्त;
}
#endif

    स्थिर शून्य
do_exmap(exarg_T *eap, पूर्णांक isabbrev)
{
    पूर्णांक	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    निवड (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	स्थिती 1: emsg(_(e_invalid_argument));
		थांबा;
	स्थिती 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		थांबा;
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
    जर (secure)
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
