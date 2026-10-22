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

#అన్తర్భావ "vim.h"

/*
 * List used for abbreviations.
 */
స్థిర mapblock_T	*మొదటి_abbr = ఖాళీ; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
స్థిర mapblock_T	*(maphash[256]);
స్థిర పూర్ణాంకం		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
స్థిర పూర్ణాంకం		map_locked = 0;

/*
 * Make a hash value for a mapping.
 * "mode" is the lower 4 bits of the State for the mapping.
 * "c1" is the first character of the "lhs".
 * Returns a value between 0 and 255, index in maphash.
 * Put Normal/Visual mode mappings mostly separately from Insert/Cmdline mode.
 */
#నిర్వచించు MAP_HASH(బహుళకం, c1) (((బహుళకం) & (బహుళకం_సాధారణ | బహుళకం_VISUAL | బహుళకం_SELECT | బహుళకం_OP_PENDING | బహుళకం_TERMINAL)) ? (c1) : ((c1) ^ 0x80))

/*
 * Get the start of the hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_maphash_list(పూర్ణాంకం state, పూర్ణాంకం c)
{
    తిరిగి maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(పూర్ణాంకం state, పూర్ణాంకం c)
{
    తిరిగి curbuf->b_maphash[MAP_HASH(state, c)];
}

    పూర్ణాంకం
is_maphash_valid(శూన్య)
{
    తిరిగి maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    స్థిర శూన్య
validate_maphash(శూన్య)
{
    ఒకవేళ (maphash_valid)
	తిరిగి;

    CLEAR_క్షేత్రం(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    స్థిర శూన్య
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_కీలకాలు);
    ఒకవేళ (mp->m_alt != ఖాళీ)
	mp->m_alt->m_alt = ఖాళీ;
    vim_free(mp->m_str);
    vim_free(mp->m_orig_str);
    *mpp = mp->m_తదుపరి;
#నిర్వచితమైతే FEAT_EVAL
    reset_చివరి_వాడిన_map(mp);
#అయితే_ముగింపు
    vim_free(mp);
}

/*
 * Return characters to represent the map mode in an allocated string.
 * Returns NULL when out of memory.
 */
    స్థిర char_u *
map_బహుళకం_గమ్యం_chars(పూర్ణాంకం బహుళకం)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    ఒకవేళ ((బహుళకం & (బహుళకం_చొప్పించు | బహుళకం_CMDLINE)) == (బహుళకం_చొప్పించు | బహుళకం_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    లేకపోతే ఒకవేళ (బహుళకం & బహుళకం_చొప్పించు)
	ga_append(&mapmode, 'i');			// :imap
    లేకపోతే ఒకవేళ (బహుళకం & బహుళకం_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    లేకపోతే ఒకవేళ (బహుళకం & బహుళకం_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    లేకపోతే ఒకవేళ ((బహుళకం
		 & (బహుళకం_సాధారణ | బహుళకం_VISUAL | బహుళకం_SELECT | బహుళకం_OP_PENDING))
		== (బహుళకం_సాధారణ | బహుళకం_VISUAL | బహుళకం_SELECT | బహుళకం_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    లేకపోతే
    {
	ఒకవేళ (బహుళకం & బహుళకం_సాధారణ)
	    ga_append(&mapmode, 'n');			// :nmap
	ఒకవేళ (బహుళకం & బహుళకం_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	ఒకవేళ (బహుళకం & బహుళకం_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	ఒకవేళ ((బహుళకం & (బహుళకం_VISUAL | బహుళకం_SELECT)) == (బహుళకం_VISUAL | బహుళకం_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	లేకపోతే
	{
	    ఒకవేళ (బహుళకం & బహుళకం_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    ఒకవేళ (బహుళకం & బహుళకం_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    తిరిగి (char_u *)mapmode.ga_దత్తాంశం;
}

/*
 * Output a line for one mapping.
 */
    స్థిర శూన్య
showmap(
    mapblock_T	*mp,
    పూర్ణాంకం		local)	    // TRUE for buffer-local map
{
    పూర్ణాంకం		len = 1;
    char_u	*mapchars;

    ఒకవేళ (సందేశం_filtered(mp->m_కీలకాలు) && సందేశం_filtered(mp->m_str))
	తిరిగి;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    ఒకవేళ (సందేశం_didout || సందేశం_silent != 0)
    {
	సందేశం_putchar('\n');
	ఒకవేళ (got_int)	    // 'q' typed at MORE prompt
	    వెళ్ళు theend;
    }

    mapchars = map_బహుళకం_గమ్యం_chars(mp->m_బహుళకం);
    ఒకవేళ (mapchars != ఖాళీ)
    {
	సందేశం_puts((అక్షరం *)mapchars);
	len = (పూర్ణాంకం)STRLEN(mapchars);
	vim_free(mapchars);
    }

    అయితే (++len <= 3)
	సందేశం_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = సందేశం_outtrans_special(mp->m_కీలకాలు, TRUE, 0);
    చేయి
    {
	సందేశం_putchar(' ');		// pad with blanks
	++len;
    } అయితే (len < 12);

    ఒకవేళ (mp->m_noremap == REMAP_NONE)
	సందేశం_puts_attr("*", HL_ATTR(HLF_8));
    లేకపోతే ఒకవేళ (mp->m_noremap == REMAP_SCRIPT)
	సందేశం_puts_attr("&", HL_ATTR(HLF_8));
    లేకపోతే
	సందేశం_putchar(' ');

    ఒకవేళ (local)
	సందేశం_putchar('@');
    లేకపోతే
	సందేశం_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    ఒకవేళ (*mp->m_str == NUL)
	సందేశం_puts_attr("<Nop>", HL_ATTR(HLF_8));
    లేకపోతే
	సందేశం_outtrans_special(mp->m_str, FALSE, 0);
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (p_verbose > 0)
	చివరి_సమితి_సందేశం(mp->m_script_ctx);
#అయితే_ముగింపు
    సందేశం_clr_eos();
    బహిర్గతం_flush();			// show one line at a time

theend:
    --map_locked;
}

    స్థిర mapblock_T *
map_కూడు(
	mapblock_T  **map_పట్టిక,
	mapblock_T  **abbr_పట్టిక,
	char_u	    *కీలకాలు,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	పూర్ణాంకం	    noremap,
	పూర్ణాంకం	    nowait,
	పూర్ణాంకం	    silent,
	పూర్ణాంకం	    బహుళకం,
	పూర్ణాంకం	    is_abbr,
#నిర్వచితమైతే FEAT_EVAL
	పూర్ణాంకం	    సమాసం,
	scid_T	    sid,	    // 0 to use current_sctx
	పూర్ణాంకం	    scriptversion,
	linenr_T    lnum,
#అయితే_ముగింపు
	పూర్ణాంకం	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    ఒకవేళ (mp == ఖాళీ)
	తిరిగి ఖాళీ;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    ఒకవేళ (*కీలకాలు == Ctrl_C)
    {
	ఒకవేళ (map_పట్టిక == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= బహుళకం;
	లేకపోతే
	    mapped_ctrl_c |= బహుళకం;
    }

    mp->m_కీలకాలు = vim_strsave(కీలకాలు);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    ఒకవేళ (mp->m_కీలకాలు == ఖాళీ || mp->m_str == ఖాళీ)
    {
	vim_free(mp->m_కీలకాలు);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	తిరిగి ఖాళీ;
    }
    mp->m_keylen = (పూర్ణాంకం)STRLEN(mp->m_కీలకాలు);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_బహుళకం = బహుళకం;
    mp->m_simplified = simplified;
#నిర్వచితమైతే FEAT_EVAL
    mp->m_సమాసం = సమాసం;
    ఒకవేళ (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_సంస్కరణ = scriptversion;
    }
    లేకపోతే
    {
	mp->m_script_ctx = current_sctx;
	ఒకవేళ (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_సంస్కరణ = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#అయితే_ముగింపు

    // add the new entry in front of the abbrlist or maphash[] list
    ఒకవేళ (is_abbr)
    {
	mp->m_తదుపరి = *abbr_పట్టిక;
	*abbr_పట్టిక = mp;
    }
    లేకపోతే
    {
	పూర్ణాంకం n = MAP_HASH(mp->m_బహుళకం, mp->m_కీలకాలు[0]);

	mp->m_తదుపరి = map_పట్టిక[n];
	map_పట్టిక[n] = mp;
    }
    తిరిగి mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    స్థిర శూన్య
list_mappings(
	పూర్ణాంకం	keyround,
	పూర్ణాంకం	abbrev,
	పూర్ణాంకం	haskey,
	char_u	*కీలకాలు,
	పూర్ణాంకం	కీలకాలు_len,
	పూర్ణాంకం	బహుళకం,
	పూర్ణాంకం	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    ఒకవేళ (p_verbose > 0 && keyround == 1)
    {
	ఒకవేళ (చూసిన_Modify_Other_కీలకాలు)
	    సందేశం_puts(_("Seen modifyOtherKeys: true\n"));

	ఒకవేళ (modify_otherkeys_state != MOKS_INITIAL)
	{
	    అక్షరం *name = _("Unknown");
	    ఎంపిక (modify_otherkeys_state)
	    {
		స్థితి MOKS_INITIAL: ఆపు;
		స్థితి MOKS_OFF: name = _("Off"); ఆపు;
		స్థితి MOKS_ENABLED: name = _("On"); ఆపు;
		స్థితి MOKS_DISABLED: name = _("Disabled"); ఆపు;
		స్థితి MOKS_AFTER_T_TE: name = _("Cleared"); ఆపు;
	    }

	    అక్షరం buf[200];
	    vim_snprintf(buf, పరిమాణం(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    సందేశం_puts(buf);
	}

	ఒకవేళ (kitty_protocol_state != KKPS_INITIAL)
	{
	    అక్షరం *name = _("Unknown");
	    ఎంపిక (kitty_protocol_state)
	    {
		స్థితి KKPS_INITIAL: ఆపు;
		స్థితి KKPS_OFF: name = _("Off"); ఆపు;
		స్థితి KKPS_ENABLED: name = _("On"); ఆపు;
		స్థితి KKPS_DISABLED: name = _("Disabled"); ఆపు;
		స్థితి KKPS_AFTER_T_TE: name = _("Cleared"); ఆపు;
	    }

	    అక్షరం buf[200];
	    vim_snprintf(buf, పరిమాణం(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    సందేశం_puts(buf);
	}
    }

    // need to loop over all global hash lists
    కొరకు (పూర్ణాంకం hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	ఒకవేళ (abbrev)
	{
	    ఒకవేళ (hash != 0)	// there is only one abbreviation list
		ఆపు;
	    mp = curbuf->b_మొదటి_abbr;
	}
	లేకపోతే
	    mp = curbuf->b_maphash[hash];
	కొరకు ( ; mp != ఖాళీ && !got_int; mp = mp->m_తదుపరి)
	{
	    // check entries with the same mode
	    ఒకవేళ (!mp->m_simplified && (mp->m_బహుళకం & బహుళకం) != 0)
	    {
		ఒకవేళ (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		లేకపోతే
		{
		    పూర్ణాంకం n = mp->m_keylen;
		    ఒకవేళ (STRNCMP(mp->m_కీలకాలు, కీలకాలు,
				   (size_t)(n < కీలకాలు_len ? n : కీలకాలు_len)) == 0)
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
    పూర్ణాంకం
do_map(
    పూర్ణాంకం		maptype,
    char_u	*arg,
    పూర్ణాంకం		బహుళకం,
    పూర్ణాంకం		abbrev)		// not a mapping but an abbreviation
{
    char_u	*కీలకాలు;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_ఫలితం[2] = {ఖాళీ, ఖాళీ};
    char_u	*rhs;
    char_u	*p;
    పూర్ణాంకం		n;
    పూర్ణాంకం		len = 0;	// init for GCC
    పూర్ణాంకం		hasarg;
    పూర్ణాంకం		haskey;
    పూర్ణాంకం		do_print;
    పూర్ణాంకం		keyround;
    char_u	*కీలకాలు_buf = ఖాళీ;
    char_u	*alt_కీలకాలు_buf = ఖాళీ;
    char_u	*arg_buf = ఖాళీ;
    పూర్ణాంకం		retval = 0;
    పూర్ణాంకం		do_backslash;
    mapblock_T	**abbr_పట్టిక;
    mapblock_T	**map_పట్టిక;
    పూర్ణాంకం		unique = FALSE;
    పూర్ణాంకం		nowait = FALSE;
    పూర్ణాంకం		silent = FALSE;
    పూర్ణాంకం		special = FALSE;
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం		సమాసం = FALSE;
#అయితే_ముగింపు
    పూర్ణాంకం		did_simplify = FALSE;
    పూర్ణాంకం		unmap_lhs_only = FALSE;
    పూర్ణాంకం		noremap;
    char_u      *orig_rhs;

    కీలకాలు = arg;
    map_పట్టిక = maphash;
    abbr_పట్టిక = &మొదటి_abbr;

    ఒకవేళ (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    ఒకవేళ (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    లేకపోతే
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    కొరకు (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	ఒకవేళ (STRNCMP(కీలకాలు, "<buffer>", 8) == 0)
	{
	    కీలకాలు = skipwhite(కీలకాలు + 8);
	    map_పట్టిక = curbuf->b_maphash;
	    abbr_పట్టిక = &curbuf->b_మొదటి_abbr;
	    కొనసాగు;
	}

	// Check for "<nowait>": don't wait for more characters.
	ఒకవేళ (STRNCMP(కీలకాలు, "<nowait>", 8) == 0)
	{
	    కీలకాలు = skipwhite(కీలకాలు + 8);
	    nowait = TRUE;
	    కొనసాగు;
	}

	// Check for "<silent>": don't echo commands.
	ఒకవేళ (STRNCMP(కీలకాలు, "<silent>", 8) == 0)
	{
	    కీలకాలు = skipwhite(కీలకాలు + 8);
	    silent = TRUE;
	    కొనసాగు;
	}

	// Check for "<special>": accept special keys in <>
	ఒకవేళ (STRNCMP(కీలకాలు, "<special>", 9) == 0)
	{
	    కీలకాలు = skipwhite(కీలకాలు + 9);
	    special = TRUE;
	    కొనసాగు;
	}

#నిర్వచితమైతే FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	ఒకవేళ (STRNCMP(కీలకాలు, "<script>", 8) == 0)
	{
	    కీలకాలు = skipwhite(కీలకాలు + 8);
	    noremap = REMAP_SCRIPT;
	    కొనసాగు;
	}

	// Check for "<expr>": {rhs} is an expression.
	ఒకవేళ (STRNCMP(కీలకాలు, "<expr>", 6) == 0)
	{
	    కీలకాలు = skipwhite(కీలకాలు + 6);
	    సమాసం = TRUE;
	    కొనసాగు;
	}
#అయితే_ముగింపు
	// Check for "<unique>": don't overwrite an existing mapping.
	ఒకవేళ (STRNCMP(కీలకాలు, "<unique>", 8) == 0)
	{
	    కీలకాలు = skipwhite(కీలకాలు + 8);
	    unique = TRUE;
	    కొనసాగు;
	}
	ఆపు;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = కీలకాలు;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ఖాళీ);
    అయితే (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	ఒకవేళ ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    ఒకవేళ (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*కీలకాలు != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    ఒకవేళ (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	వెళ్ళు theend;
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
    ఒకవేళ (haskey)
    {
	char_u	*new_కీలకాలు;
	పూర్ణాంకం	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	ఒకవేళ (special)
	    flags |= REPTERM_SPECIAL;
	new_కీలకాలు = replace_termcodes(కీలకాలు, &కీలకాలు_buf, 0, flags, &did_simplify);
	ఒకవేళ (did_simplify)
	    (శూన్య)replace_termcodes(కీలకాలు, &alt_కీలకాలు_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, ఖాళీ);
	కీలకాలు = new_కీలకాలు;
    }
    orig_rhs = rhs;
    ఒకవేళ (hasarg)
    {
	ఒకవేళ (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	లేకపోతే
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ఖాళీ);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    కొరకు (keyround = 1; keyround <= 2; ++keyround)
    {
	పూర్ణాంకం	did_it = FALSE;
	పూర్ణాంకం	did_local = FALSE;
	పూర్ణాంకం	keyround1_simplified = keyround == 1 && did_simplify;
	పూర్ణాంకం	విడత;
	పూర్ణాంకం	అంకె_విడతలు;

	ఒకవేళ (keyround == 2)
	{
	    ఒకవేళ (alt_కీలకాలు_buf == ఖాళీ)
		ఆపు;
	    కీలకాలు = alt_కీలకాలు_buf;
	}
	లేకపోతే ఒకవేళ (alt_కీలకాలు_buf != ఖాళీ && do_print)
	    // when printing always use the not-simplified map
	    కీలకాలు = alt_కీలకాలు_buf;

	// check arguments and translate function keys
	ఒకవేళ (haskey)
	{
	    len = (పూర్ణాంకం)STRLEN(కీలకాలు);
	    ఒకవేళ (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		వెళ్ళు theend;
	    }

	    ఒకవేళ (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		ఒకవేళ (has_mbyte)
		{
		    పూర్ణాంకం		మొదటి, చివరి;
		    పూర్ణాంకం		same = -1;
		    char_u	కీలకాలు_unescaped[MAXMAPLEN + 1];
		    size_t	కీలకాలు_unescaped_len;

		    mch_memmove(కీలకాలు_unescaped, కీలకాలు, (size_t)(len + 1));
		    కీలకాలు_unescaped_len = vim_unescape_csi(కీలకాలు_unescaped);
		    p = కీలకాలు_unescaped;

		    మొదటి = vim_iswordp(p);
		    చివరి = మొదటి;
		    MB_PTR_ADV(p);
		    n = 1;
		    అయితే (p < కీలకాలు_unescaped + కీలకాలు_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			చివరి = vim_iswordp(p);	// type of last char
			ఒకవేళ (same == -1 && చివరి != మొదటి)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    ఒకవేళ (చివరి && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			వెళ్ళు theend;
		    }
		}
		లేకపోతే ఒకవేళ (vim_iswordc(కీలకాలు[len - 1]))
		    // ends in keyword char
		    కొరకు (n = 0; n < len - 2; ++n)
			ఒకవేళ (vim_iswordc(కీలకాలు[n]) != vim_iswordc(కీలకాలు[len - 2]))
			{
			    retval = 1;
			    వెళ్ళు theend;
			}
		// An abbreviation cannot contain white space.
		కొరకు (n = 0; n < len; ++n)
		    ఒకవేళ (VIM_ISWHITE(కీలకాలు[n]))
		    {
			retval = 1;
			వెళ్ళు theend;
		    }
	    }
	}

	ఒకవేళ (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	ఒకవేళ (do_print)
	    సందేశం_ప్రారంభం();

	// Check if a new local mapping wasn't already defined globally.
	ఒకవేళ (unique && map_పట్టిక == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    కొరకు (పూర్ణాంకం hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ఒకవేళ (abbrev)
		{
		    ఒకవేళ (hash != 0)	// there is only one abbreviation list
			ఆపు;
		    mp = మొదటి_abbr;
		}
		లేకపోతే
		    mp = maphash[hash];
		కొరకు ( ; mp != ఖాళీ && !got_int; mp = mp->m_తదుపరి)
		{
		    // check entries with the same mode
		    ఒకవేళ ((mp->m_బహుళకం & బహుళకం) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_కీలకాలు, కీలకాలు, (size_t)len) == 0)
		    {
			ఒకవేళ (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_కీలకాలు);
			లేకపోతే
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_కీలకాలు);
			retval = 5;
			వెళ్ళు theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	ఒకవేళ (map_పట్టిక != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, కీలకాలు, len,
							     బహుళకం, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	అంకె_విడతలు = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	కొరకు (విడత = 0; విడత < అంకె_విడతలు && !did_it && !got_int; ++విడత)
	{
	    // need to loop over all hash lists
	    కొరకు (పూర్ణాంకం hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ఒకవేళ (abbrev)
		{
		    ఒకవేళ (hash > 0)	// there is only one abbreviation list
			ఆపు;
		    mpp = abbr_పట్టిక;
		}
		లేకపోతే
		    mpp = &(map_పట్టిక[hash]);
		కొరకు (mp = *mpp; mp != ఖాళీ && !got_int; mp = *mpp)
		{

		    ఒకవేళ ((mp->m_బహుళకం & బహుళకం) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_తదుపరి);
			కొనసాగు;
		    }
		    ఒకవేళ (!haskey)	// show all entries
		    {
			ఒకవేళ (!mp->m_simplified)
			{
			    showmap(mp, map_పట్టిక != maphash);
			    did_it = TRUE;
			}
		    }
		    లేకపోతే	// do we have a match?
		    {
			ఒకవేళ (విడత)	// second round: Try unmap "rhs" string
			{
			    n = (పూర్ణాంకం)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			లేకపోతే
			{
			    n = mp->m_keylen;
			    p = mp->m_కీలకాలు;
			}
			ఒకవేళ (STRNCMP(p, కీలకాలు, (size_t)(n < len ? n : len)) == 0)
			{
			    ఒకవేళ (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				ఒకవేళ (n != len && (!abbrev || విడత || n > len
					       || *skipwhite(కీలకాలు + n) != NUL))
				{
				    mpp = &(mp->m_తదుపరి);
				    కొనసాగు;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				ఒకవేళ (keyround1_simplified && !mp->m_simplified)
				    ఆపు;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_బహుళకం &= ~బహుళకం;
				did_it = TRUE;	// remember we did something
			    }
			    లేకపోతే ఒకవేళ (!hasarg)	// show matching entry
			    {
				ఒకవేళ (!mp->m_simplified)
				{
				    showmap(mp, map_పట్టిక != maphash);
				    did_it = TRUE;
				}
			    }
			    లేకపోతే ఒకవేళ (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_తదుపరి);
				కొనసాగు;
			    }
			    లేకపోతే ఒకవేళ (unique)
			    {
				ఒకవేళ (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				లేకపోతే
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				వెళ్ళు theend;
			    }
			    లేకపోతే
			    {
				// new rhs for existing entry
				mp->m_బహుళకం &= ~బహుళకం;	// remove mode bits
				ఒకవేళ (mp->m_బహుళకం == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    ఒకవేళ (newstr == ఖాళీ)
				    {
					retval = 4;		// no mem
					వెళ్ళు theend;
				    }
				    ఒకవేళ (mp->m_alt != ఖాళీ)
					mp->m_alt = mp->m_alt->m_alt = ఖాళీ;
				    vim_free(mp->m_str);
				    mp->m_str = newstr;
				    vim_free(mp->m_orig_str);
				    mp->m_orig_str = vim_strsave(orig_rhs);
				    mp->m_noremap = noremap;
				    mp->m_nowait = nowait;
				    mp->m_silent = silent;
				    mp->m_బహుళకం = బహుళకం;
				    mp->m_simplified = keyround1_simplified;
#నిర్వచితమైతే FEAT_EVAL
				    mp->m_సమాసం = సమాసం;
				    mp->m_script_ctx = current_sctx;
				    ఒకవేళ (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_సంస్కరణ = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#అయితే_ముగింపు
				    mp_ఫలితం[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    ఒకవేళ (mp->m_బహుళకం == 0)  // entry can be deleted
			    {
				map_free(mpp);
				కొనసాగు;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    పూర్ణాంకం new_hash = MAP_HASH(mp->m_బహుళకం, mp->m_కీలకాలు[0]);
			    ఒకవేళ (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_తదుపరి;
				mp->m_తదుపరి = map_పట్టిక[new_hash];
				map_పట్టిక[new_hash] = mp;

				కొనసాగు;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_తదుపరి);
		}
	    }
	}

	ఒకవేళ (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    ఒకవేళ (!did_it)
	    {
		ఒకవేళ (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    లేకపోతే ఒకవేళ (*కీలకాలు == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		ఒకవేళ (map_పట్టిక == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~బహుళకం;
		లేకపోతే
		    mapped_ctrl_c &= ~బహుళకం;
	    }
	    కొనసాగు;
	}

	ఒకవేళ (!haskey || !hasarg)
	{
	    // print entries
	    ఒకవేళ (!did_it && !did_local)
	    {
		ఒకవేళ (abbrev)
		    సందేశం(_("No abbreviation found"));
		లేకపోతే
		    సందేశం(_("No mapping found"));
	    }
	    వెళ్ళు theend;    // listing finished
	}

	ఒకవేళ (did_it)
	    కొనసాగు;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_ఫలితం[keyround - 1] = map_కూడు(map_పట్టిక, abbr_పట్టిక, కీలకాలు,
		    rhs, orig_rhs, noremap, nowait, silent, బహుళకం, abbrev,
#నిర్వచితమైతే FEAT_EVAL
		    సమాసం, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#అయితే_ముగింపు
		    keyround1_simplified);
	ఒకవేళ (mp_ఫలితం[keyround - 1] == ఖాళీ)
	{
	    retval = 4;	    // no mem
	    వెళ్ళు theend;
	}
    }

    ఒకవేళ (mp_ఫలితం[0] != ఖాళీ && mp_ఫలితం[1] != ఖాళీ)
    {
	mp_ఫలితం[0]->m_alt = mp_ఫలితం[1];
	mp_ఫలితం[1]->m_alt = mp_ఫలితం[0];
    }

theend:
    vim_free(కీలకాలు_buf);
    vim_free(alt_కీలకాలు_buf);
    vim_free(arg_buf);
    తిరిగి retval;
}

/*
 * Get the mapping mode from the command name.
 */
    స్థిర పూర్ణాంకం
get_map_బహుళకం(char_u **cmdp, పూర్ణాంకం forceit)
{
    char_u	*p;
    పూర్ణాంకం		modec;
    పూర్ణాంకం		బహుళకం;

    p = *cmdp;
    modec = *p++;
    ఒకవేళ (modec == 'i')
	బహుళకం = బహుళకం_చొప్పించు;				// :imap
    లేకపోతే ఒకవేళ (modec == 'l')
	బహుళకం = బహుళకం_LANGMAP;				// :lmap
    లేకపోతే ఒకవేళ (modec == 'c')
	బహుళకం = బహుళకం_CMDLINE;				// :cmap
    లేకపోతే ఒకవేళ (modec == 'n' && *p != 'o')		    // avoid :noremap
	బహుళకం = బహుళకం_సాధారణ;				// :nmap
    లేకపోతే ఒకవేళ (modec == 'v')
	బహుళకం = బహుళకం_VISUAL | బహుళకం_SELECT;		// :vmap
    లేకపోతే ఒకవేళ (modec == 'x')
	బహుళకం = బహుళకం_VISUAL;				// :xmap
    లేకపోతే ఒకవేళ (modec == 's')
	బహుళకం = బహుళకం_SELECT;			// :smap
    లేకపోతే ఒకవేళ (modec == 'o')
	బహుళకం = బహుళకం_OP_PENDING;			// :omap
    లేకపోతే ఒకవేళ (modec == 't')
	బహుళకం = బహుళకం_TERMINAL;			// :tmap
    లేకపోతే
    {
	--p;
	ఒకవేళ (forceit)
	    బహుళకం = బహుళకం_చొప్పించు | బహుళకం_CMDLINE;		// :map !
	లేకపోతే
	    బహుళకం = బహుళకం_VISUAL | బహుళకం_SELECT | బహుళకం_సాధారణ | బహుళకం_OP_PENDING;
							// :map
    }

    *cmdp = p;
    తిరిగి బహుళకం;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    స్థిర శూన్య
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    పూర్ణాంకం		forceit,
    పూర్ణాంకం		abbr)
{
    పూర్ణాంకం		బహుళకం;
    పూర్ణాంకం		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    ఒకవేళ (!local && *arg != NUL)
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి;
    }

    బహుళకం = get_map_బహుళకం(&cmdp, forceit);
    map_clear_బహుళకం(curbuf, బహుళకం, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    స్థిర పూర్ణాంకం
is_map_locked(శూన్య)
{
    ఒకవేళ (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    శూన్య
map_clear_బహుళకం(
    buf_T	*buf,		// buffer for local mappings
    పూర్ణాంకం		బహుళకం,		// mode in which to delete
    పూర్ణాంకం		local,		// TRUE for buffer-local mappings
    పూర్ణాంకం		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    పూర్ణాంకం		hash;
    పూర్ణాంకం		new_hash;

    ఒకవేళ (is_map_locked())
	తిరిగి;

    validate_maphash();

    కొరకు (hash = 0; hash < 256; ++hash)
    {
	ఒకవేళ (abbr)
	{
	    ఒకవేళ (hash > 0)	// there is only one abbrlist
		ఆపు;
	    ఒకవేళ (local)
		mpp = &buf->b_మొదటి_abbr;
	    లేకపోతే
		mpp = &మొదటి_abbr;
	}
	లేకపోతే
	{
	    ఒకవేళ (local)
		mpp = &buf->b_maphash[hash];
	    లేకపోతే
		mpp = &maphash[hash];
	}
	అయితే (*mpp != ఖాళీ)
	{
	    mp = *mpp;
	    ఒకవేళ (mp->m_బహుళకం & బహుళకం)
	    {
		mp->m_బహుళకం &= ~బహుళకం;
		ఒకవేళ (mp->m_బహుళకం == 0) // entry can be deleted
		{
		    map_free(mpp);
		    కొనసాగు;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_బహుళకం, mp->m_కీలకాలు[0]);
		ఒకవేళ (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_తదుపరి;
		    ఒకవేళ (local)
		    {
			mp->m_తదుపరి = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    లేకపోతే
		    {
			mp->m_తదుపరి = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    కొనసాగు;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_తదుపరి);
	}
    }
}

#ఒకవేళ defined(FEAT_EVAL)
    పూర్ణాంకం
బహుళకం_str2flags(char_u *modechars)
{
    పూర్ణాంకం		బహుళకం = 0;

    ఒకవేళ (vim_strchr(modechars, 'n') != ఖాళీ)
	బహుళకం |= బహుళకం_సాధారణ;
    ఒకవేళ (vim_strchr(modechars, 'v') != ఖాళీ)
	బహుళకం |= బహుళకం_VISUAL | బహుళకం_SELECT;
    ఒకవేళ (vim_strchr(modechars, 'x') != ఖాళీ)
	బహుళకం |= బహుళకం_VISUAL;
    ఒకవేళ (vim_strchr(modechars, 's') != ఖాళీ)
	బహుళకం |= బహుళకం_SELECT;
    ఒకవేళ (vim_strchr(modechars, 'o') != ఖాళీ)
	బహుళకం |= బహుళకం_OP_PENDING;
    ఒకవేళ (vim_strchr(modechars, 'i') != ఖాళీ)
	బహుళకం |= బహుళకం_చొప్పించు;
    ఒకవేళ (vim_strchr(modechars, 'l') != ఖాళీ)
	బహుళకం |= బహుళకం_LANGMAP;
    ఒకవేళ (vim_strchr(modechars, 'c') != ఖాళీ)
	బహుళకం |= బహుళకం_CMDLINE;

    తిరిగి బహుళకం;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    పూర్ణాంకం
map_గమ్యం_exists(char_u *str, char_u *modechars, పూర్ణాంకం abbr)
{
    char_u	*rhs;
    char_u	*buf;
    పూర్ణాంకం		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, ఖాళీ);

    retval = map_గమ్యం_exists_బహుళకం(rhs, బహుళకం_str2flags(modechars), abbr);
    vim_free(buf);

    తిరిగి retval;
}
#అయితే_ముగింపు

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    పూర్ణాంకం
map_గమ్యం_exists_బహుళకం(char_u *rhs, పూర్ణాంకం బహుళకం, పూర్ణాంకం abbr)
{
    mapblock_T	*mp;
    పూర్ణాంకం		hash;
    పూర్ణాంకం		exp_బఫరు = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    కొరకు (;;)
    {
	కొరకు (hash = 0; hash < 256; ++hash)
	{
	    ఒకవేళ (abbr)
	    {
		ఒకవేళ (hash > 0)		// there is only one abbr list
		    ఆపు;
		ఒకవేళ (exp_బఫరు)
		    mp = curbuf->b_మొదటి_abbr;
		లేకపోతే
		    mp = మొదటి_abbr;
	    }
	    లేకపోతే ఒకవేళ (exp_బఫరు)
		mp = curbuf->b_maphash[hash];
	    లేకపోతే
		mp = maphash[hash];
	    కొరకు (; mp; mp = mp->m_తదుపరి)
	    {
		ఒకవేళ ((mp->m_బహుళకం & బహుళకం)
			&& అక్షరశ్రేణి_శోధన((అక్షరం *)mp->m_str, (అక్షరం *)rhs) != ఖాళీ)
		    తిరిగి TRUE;
	    }
	}
	ఒకవేళ (exp_బఫరు)
	    ఆపు;
	exp_బఫరు = TRUE;
    }

    తిరిగి FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
స్థిర పూర్ణాంకం	expand_mapmodes = 0;
స్థిర పూర్ణాంకం	expand_isabbrev = 0;
స్థిర పూర్ణాంకం	expand_బఫరు = FALSE;

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
    స్థిర char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    పూర్ణాంకం		c;
    పూర్ణాంకం		modifiers;
    పూర్ణాంకం		cpo_bslash;
    పూర్ణాంకం		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != ఖాళీ);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != ఖాళీ);

    కొరకు (; *str; ++str)
    {
	c = *str;
	ఒకవేళ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    ఒకవేళ (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    ఒకవేళ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		ఒకవేళ (cpo_special)
		{
		    ga_clear(&ga);
		    తిరిగి ఖాళీ;
		}
		c = గమ్యం_SPECIAL(str[1], str[2]);
		ఒకవేళ (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    ఒకవేళ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ఒకవేళ (cpo_special)
		{
		    ga_clear(&ga);
		    తిరిగి ఖాళీ;
		}
		ga_concat(&ga, get_special_కీలకం_name(c, modifiers));
		కొనసాగు; // for (str)
	    }
	}
	ఒకవేళ (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	ఒకవేళ (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    తిరిగి (char_u *)(ga.ga_దత్తాంశం);
}

/*
 * Work out what to complete when doing command line completion of mapping
 * or abbreviation names.
 */
    char_u *
సమితి_context_లోపల_map_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    పూర్ణాంకం		forceit,	// TRUE if '!' given
    పూర్ణాంకం		isabbrev,	// TRUE if abbreviation
    పూర్ణాంకం		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    ఒకవేళ (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    లేకపోతే
    {
	ఒకవేళ (isunmap)
	    expand_mapmodes = get_map_బహుళకం(&cmd, forceit || isabbrev);
	లేకపోతే
	{
	    expand_mapmodes = బహుళకం_చొప్పించు | బహుళకం_CMDLINE;
	    ఒకవేళ (!isabbrev)
		expand_mapmodes += బహుళకం_VISUAL | బహుళకం_SELECT | బహుళకం_సాధారణ
							     | బహుళకం_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_బఫరు = FALSE;
	కొరకు (;;)
	{
	    ఒకవేళ (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_బఫరు = TRUE;
		arg = skipwhite(arg + 8);
		కొనసాగు;
	    }
	    ఒకవేళ (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		కొనసాగు;
	    }
	    ఒకవేళ (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		కొనసాగు;
	    }
	    ఒకవేళ (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		కొనసాగు;
	    }
	    ఒకవేళ (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		కొనసాగు;
	    }
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		కొనసాగు;
	    }
	    ఒకవేళ (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		కొనసాగు;
	    }
#అయితే_ముగింపు
	    ఆపు;
	}
	xp->xp_నమూనా = arg;
    }

    తిరిగి ఖాళీ;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    పూర్ణాంకం
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    పూర్ణాంకం		*అంకె_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    పూర్ణాంకం		hash;
    పూర్ణాంకం		లెక్క;
    char_u	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		fuzzy;
    పూర్ణాంకం		సరిపోలిక;
    పూర్ణాంకం		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *అంకె_Matches = 0;		    // return values in case of FAIL
    *matches = ఖాళీ;

    ఒకవేళ (!fuzzy)
	ga_init2(&ga, పరిమాణం(అక్షరం *), 3);
    లేకపోతే
	ga_init2(&ga, పరిమాణం(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    కొరకు (i = 0; i < 7; ++i)
    {
	ఒకవేళ (i == 0)
	    p = (char_u *)"<silent>";
	లేకపోతే ఒకవేళ (i == 1)
	    p = (char_u *)"<unique>";
#నిర్వచితమైతే FEAT_EVAL
	లేకపోతే ఒకవేళ (i == 2)
	    p = (char_u *)"<script>";
	లేకపోతే ఒకవేళ (i == 3)
	    p = (char_u *)"<expr>";
#అయితే_ముగింపు
	లేకపోతే ఒకవేళ (i == 4 && !expand_బఫరు)
	    p = (char_u *)"<buffer>";
	లేకపోతే ఒకవేళ (i == 5)
	    p = (char_u *)"<nowait>";
	లేకపోతే ఒకవేళ (i == 6)
	    p = (char_u *)"<special>";
	లేకపోతే
	    కొనసాగు;

	ఒకవేళ (!fuzzy)
	    సరిపోలిక = vim_regexec(regmatch, p, (colnr_T)0);
	లేకపోతే
	{
	    score = fuzzy_సరిపోలిక_str(p, pat);
	    సరిపోలిక = (score != FUZZY_SCORE_NONE);
	}

	ఒకవేళ (!సరిపోలిక)
	    కొనసాగు;

	ఒకవేళ (ga_grow(&ga, 1) == FAIL)
	    ఆపు;

	ఒకవేళ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_దత్తాంశం)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	లేకపోతే
	    ((char_u **)ga.ga_దత్తాంశం)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    కొరకు (hash = 0; hash < 256; ++hash)
    {
	ఒకవేళ (expand_isabbrev)
	{
	    ఒకవేళ (hash > 0)	// only one abbrev list
		ఆపు; // for (hash)
	    mp = మొదటి_abbr;
	}
	లేకపోతే ఒకవేళ (expand_బఫరు)
	    mp = curbuf->b_maphash[hash];
	లేకపోతే
	    mp = maphash[hash];
	కొరకు (; mp; mp = mp->m_తదుపరి)
	{
	    ఒకవేళ (mp->m_simplified || !(mp->m_బహుళకం & expand_mapmodes))
		కొనసాగు;

	    p = translate_mapping(mp->m_కీలకాలు);
	    ఒకవేళ (p == ఖాళీ)
		కొనసాగు;

	    ఒకవేళ (!fuzzy)
		సరిపోలిక = vim_regexec(regmatch, p, (colnr_T)0);
	    లేకపోతే
	    {
		score = fuzzy_సరిపోలిక_str(p, pat);
		సరిపోలిక = (score != FUZZY_SCORE_NONE);
	    }

	    ఒకవేళ (!సరిపోలిక)
	    {
		vim_free(p);
		కొనసాగు;
	    }

	    ఒకవేళ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		ఆపు;
	    }

	    ఒకవేళ (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_దత్తాంశం)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    లేకపోతే
		((char_u **)ga.ga_దత్తాంశం)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    ఒకవేళ (ga.ga_len == 0)
	తిరిగి FAIL;

    ఒకవేళ (!fuzzy)
    {
	*matches = ga.ga_దత్తాంశం;
	*అంకె_Matches = ga.ga_len;
    }
    లేకపోతే
    {
	ఒకవేళ (fuzzymatches_గమ్యం_strmatches(ga.ga_దత్తాంశం, matches, ga.ga_len,
							FALSE) == FAIL)
	    తిరిగి FAIL;
	*అంకె_Matches = ga.ga_len;
    }

    లెక్క = *అంకె_Matches;
    ఒకవేళ (లెక్క > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	ఒకవేళ (!fuzzy)
	    క్రమీకరించు_అక్షరశ్రేణులు(*matches, లెక్క);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + లెక్క;

	అయితే (ptr2 < ptr3)
	{
	    ఒకవేళ (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    లేకపోతే
	    {
		vim_free(*ptr2++);
		లెక్క--;
	    }
	}
    }

    *అంకె_Matches = లెక్క;
    తిరిగి (లెక్క == 0 ? FAIL : OK);
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
    పూర్ణాంకం
check_abbr(
    పూర్ణాంకం		c,
    char_u	*ptr,
    పూర్ణాంకం		నిలువు,
    పూర్ణాంకం		mincol)
{
    పూర్ణాంకం		len;
    పూర్ణాంకం		scol;		// starting column of the abbr.
    పూర్ణాంకం		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    పూర్ణాంకం		clen = 0;	// length in characters
    పూర్ణాంకం		is_id = TRUE;
    పూర్ణాంకం		vim_abbr;

    ఒకవేళ (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	తిరిగి FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    ఒకవేళ (noremap_కీలకాలు() && c != Ctrl_RSB)
	తిరిగి FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    ఒకవేళ (నిలువు == 0)				// cannot be an abbr.
	తిరిగి FALSE;

    ఒకవేళ (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + నిలువు);
	ఒకవేళ (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	లేకపోతే
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ఒకవేళ (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	అయితే (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    ఒకవేళ (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		ఆపు;
	    }
	    ++clen;
	}
	scol = (పూర్ణాంకం)(p - ptr);
    }
    లేకపోతే
    {
	ఒకవేళ (!vim_iswordc(ptr[నిలువు - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	లేకపోతే
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ఒకవేళ (నిలువు > 1)
		is_id = vim_iswordc(ptr[నిలువు - 2]);
	}
	కొరకు (scol = నిలువు - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    ఒకవేళ (scol < mincol)
	scol = mincol;
    ఒకవేళ (scol < నిలువు)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = నిలువు - scol;
	mp = curbuf->b_మొదటి_abbr;
	mp2 = మొదటి_abbr;
	ఒకవేళ (mp == ఖాళీ)
	{
	    mp = mp2;
	    mp2 = ఖాళీ;
	}
	కొరకు ( ; mp; mp->m_తదుపరి == ఖాళీ
				  ? (mp = mp2, mp2 = ఖాళీ) : (mp = mp->m_తదుపరి))
	{
	    పూర్ణాంకం		qlen = mp->m_keylen;
	    char_u	*q = mp->m_కీలకాలు;
	    పూర్ణాంకం		సరిపోలిక;

	    ఒకవేళ (vim_strbyte(mp->m_కీలకాలు, K_SPECIAL) != ఖాళీ)
	    {
		char_u *qe = vim_strsave(mp->m_కీలకాలు);

		// might have CSI escaped mp->m_keys
		ఒకవేళ (qe != ఖాళీ)
		{
		    q = qe;
		    qlen = (పూర్ణాంకం)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    సరిపోలిక =    (mp->m_బహుళకం & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    ఒకవేళ (q != mp->m_కీలకాలు)
		vim_free(q);
	    ఒకవేళ (సరిపోలిక)
		ఆపు;
	}
	ఒకవేళ (mp != ఖాళీ)
	{
	    పూర్ణాంకం	noremap;
	    పూర్ణాంకం silent;
#నిర్వచితమైతే FEAT_EVAL
	    పూర్ణాంకం సమాసం;
#అయితే_ముగింపు

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
	    ఒకవేళ (c != Ctrl_RSB)
	    {
					// special key code, split up
		ఒకవేళ (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_రెండవ(c);
		    tb[j++] = K_మూడవ(c);
		}
		లేకపోతే
		{
		    ఒకవేళ (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    ఒకవేళ (has_mbyte)
		    {
			పూర్ణాంకం	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			ఒకవేళ (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			ఒకవేళ (escaped != ఖాళీ)
			{
			    newlen = (పూర్ణాంకం)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    లేకపోతే
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(శూన్య)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#నిర్వచితమైతే FEAT_EVAL
	    సమాసం = mp->m_సమాసం;

	    ఒకవేళ (సమాసం)
		s = eval_map_సమాసం(mp, c);
	    లేకపోతే
#అయితే_ముగింపు
		s = mp->m_str;
	    ఒకవేళ (s != ఖాళీ)
	    {
					// insert the to string
		(శూన్య)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (పూర్ణాంకం)STRLEN(s) + j + 1;
#నిర్వచితమైతే FEAT_EVAL
		ఒకవేళ (సమాసం)
		    vim_free(s);
#అయితే_ముగింపు
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    ఒకవేళ (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    అయితే (len-- > 0)		// delete the from string
		(శూన్య)ins_typebuf(tb, 1, 0, TRUE, silent);
	    తిరిగి TRUE;
	}
    }
    తిరిగి FALSE;
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_సమాసం(
    mapblock_T	*mp,
    పూర్ణాంకం		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*సమాసం;
    pos_T	save_cursor;
    పూర్ణాంకం		save_సందేశం_నిలువు;
    పూర్ణాంకం		save_సందేశం_అడ్డువరుస;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    పూర్ణాంకం		save_sctx_సంస్కరణ = current_sctx.sc_సంస్కరణ;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    సమాసం = vim_strsave(mp->m_str);
    ఒకవేళ (సమాసం == ఖాళీ)
	తిరిగి ఖాళీ;
    vim_unescape_csi(సమాసం);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_సాధారణ_తాళం;
    సమితి_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_సందేశం_నిలువు = సందేశం_నిలువు;
    save_సందేశం_అడ్డువరుస = సందేశం_అడ్డువరుస;

    current_sctx.sc_సంస్కరణ = mp->m_script_ctx.sc_సంస్కరణ;
    ఒకవేళ (mp->m_script_ctx.sc_సంస్కరణ == SCRIPT_సంస్కరణ_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_గమ్యం_string(సమాసం, FALSE, FALSE);

    --textlock;
    --ex_సాధారణ_తాళం;
    curwin->w_cursor = save_cursor;
    సందేశం_నిలువు = save_సందేశం_నిలువు;
    సందేశం_అడ్డువరుస = save_సందేశం_అడ్డువరుస;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_సంస్కరణ = save_sctx_సంస్కరణ;

    vim_free(సమాసం);

    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    తిరిగి res;
}
#అయితే_ముగింపు

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
    ఒకవేళ (res == ఖాళీ)
	తిరిగి ఖాళీ;

    d = res;
    కొరకు (s = p; *s != NUL; )
    {
	ఒకవేళ ((s[0] == K_SPECIAL
#నిర్వచితమైతే FEAT_GUI
		    || (gui.లోపల_use && s[0] == CSI)
#అయితే_ముగింపు
	    ) && s[1] != NUL && s[2] != NUL)
	{
	    // Copy special key unmodified.
	    *d++ = *s++;
	    *d++ = *s++;
	    *d++ = *s++;
	}
	లేకపోతే
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = కూడు_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    తిరిగి res;
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

    అయితే (*s != NUL)
    {
	ఒకవేళ (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	లేకపోతే ఒకవేళ ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (పూర్ణాంకం)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	లేకపోతే
	    *d++ = *s++;
    }
    *d = NUL;
    తిరిగి (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    పూర్ణాంకం
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    అక్షరం	*cmd;
    పూర్ణాంకం		abbr;
    పూర్ణాంకం		hash;
    పూర్ణాంకం		did_cpo = FALSE;
    పూర్ణాంకం		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    కొరకు (abbr = 0; abbr < 2; ++abbr)
	కొరకు (hash = 0; hash < 256; ++hash)
	{
	    ఒకవేళ (abbr)
	    {
		ఒకవేళ (hash > 0)		// there is only one abbr list
		    ఆపు;
		ఒకవేళ (buf != ఖాళీ)
		    mp = buf->b_మొదటి_abbr;
		లేకపోతే
		    mp = మొదటి_abbr;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (buf != ఖాళీ)
		    mp = buf->b_maphash[hash];
		లేకపోతే
		    mp = maphash[hash];
	    }

	    కొరకు ( ; mp; mp = mp->m_తదుపరి)
	    {
		// simplified map blocks are not created explicitly
		ఒకవేళ (mp->m_simplified)
		    కొనసాగు;

		// skip script-local mappings
		ఒకవేళ (mp->m_noremap == REMAP_SCRIPT)
		    కొనసాగు;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		కొరకు (p = mp->m_str; *p != NUL; ++p)
		    ఒకవేళ (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (పూర్ణాంకం)KE_SNR)
			ఆపు;
		ఒకవేళ (*p != NUL)
		    కొనసాగు;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		ఒకవేళ (abbr)
		    cmd = "abbr";
		లేకపోతే
		    cmd = "map";
		ఎంపిక (mp->m_బహుళకం)
		{
		    స్థితి బహుళకం_సాధారణ | బహుళకం_VISUAL | బహుళకం_SELECT
							     | బహుళకం_OP_PENDING:
			ఆపు;
		    స్థితి బహుళకం_సాధారణ:
			c1 = 'n';
			ఆపు;
		    స్థితి బహుళకం_VISUAL:
			c1 = 'x';
			ఆపు;
		    స్థితి బహుళకం_SELECT:
			c1 = 's';
			ఆపు;
		    స్థితి బహుళకం_OP_PENDING:
			c1 = 'o';
			ఆపు;
		    స్థితి బహుళకం_సాధారణ | బహుళకం_VISUAL:
			c1 = 'n';
			c2 = 'x';
			ఆపు;
		    స్థితి బహుళకం_సాధారణ | బహుళకం_SELECT:
			c1 = 'n';
			c2 = 's';
			ఆపు;
		    స్థితి బహుళకం_సాధారణ | బహుళకం_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			ఆపు;
		    స్థితి బహుళకం_VISUAL | బహుళకం_SELECT:
			c1 = 'v';
			ఆపు;
		    స్థితి బహుళకం_VISUAL | బహుళకం_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			ఆపు;
		    స్థితి బహుళకం_SELECT | బహుళకం_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			ఆపు;
		    స్థితి బహుళకం_సాధారణ | బహుళకం_VISUAL | బహుళకం_SELECT:
			c1 = 'n';
			c2 = 'v';
			ఆపు;
		    స్థితి బహుళకం_సాధారణ | బహుళకం_VISUAL | బహుళకం_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			ఆపు;
		    స్థితి బహుళకం_సాధారణ | బహుళకం_SELECT | బహుళకం_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			ఆపు;
		    స్థితి బహుళకం_VISUAL | బహుళకం_SELECT | బహుళకం_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			ఆపు;
		    స్థితి బహుళకం_CMDLINE | బహుళకం_చొప్పించు:
			ఒకవేళ (!abbr)
			    cmd = "map!";
			ఆపు;
		    స్థితి బహుళకం_CMDLINE:
			c1 = 'c';
			ఆపు;
		    స్థితి బహుళకం_చొప్పించు:
			c1 = 'i';
			ఆపు;
		    స్థితి బహుళకం_LANGMAP:
			c1 = 'l';
			ఆపు;
		    స్థితి బహుళకం_TERMINAL:
			c1 = 't';
			ఆపు;
		    అప్రమేయ:
			iemsg(e_makemap_illegal_బహుళకం);
			తిరిగి FAIL;
		}
		చేయి	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    ఒకవేళ (!did_cpo)
		    {
			ఒకవేళ (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			లేకపోతే
			    కొరకు (i = 0; i < 2; ++i)
				కొరకు (p = (i ? mp->m_str : mp->m_కీలకాలు); *p; ++p)
				    ఒకవేళ (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			ఒకవేళ (did_cpo)
			{
			    ఒకవేళ (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				తిరిగి FAIL;
			}
		    }
#నిర్వచితమైతే FEAT_EVAL
		    // If it is not vim9 use legacy
		    ఒకవేళ (mp->m_సమాసం && mp->m_script_ctx.sc_సంస్కరణ < SCRIPT_సంస్కరణ_VIM9
			    && fputs("legacy ", fd) < 0)
			తిరిగి FAIL;
#అయితే_ముగింపు
		    ఒకవేళ (c1 && putc(c1, fd) < 0)
			తిరిగి FAIL;
		    ఒకవేళ (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			తిరిగి FAIL;
		    ఒకవేళ (fputs(cmd, fd) < 0)
			తిరిగి FAIL;
		    ఒకవేళ (buf != ఖాళీ && fputs(" <buffer>", fd) < 0)
			తిరిగి FAIL;
		    ఒకవేళ (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			తిరిగి FAIL;
		    ఒకవేళ (mp->m_silent && fputs(" <silent>", fd) < 0)
			తిరిగి FAIL;
#నిర్వచితమైతే FEAT_EVAL
		    ఒకవేళ (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			తిరిగి FAIL;
		    ఒకవేళ (mp->m_సమాసం && fputs(" <expr>", fd) < 0)
			తిరిగి FAIL;
#అయితే_ముగింపు

		    ఒకవేళ (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_కీలకాలు, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			తిరిగి FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} అయితే (c1 != NUL);
	    }
	}

    ఒకవేళ (did_cpo)
	ఒకవేళ (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
put_escstr(FILE *fd, char_u *strstart, పూర్ణాంకం what)
{
    char_u	*str = strstart;
    పూర్ణాంకం		c;
    పూర్ణాంకం		modifiers;

    // :map xx <Nop>
    ఒకవేళ (*str == NUL && what == 1)
    {
	ఒకవేళ (fprintf(fd, "<Nop>") < 0)
	    తిరిగి FAIL;
	తిరిగి OK;
    }

    కొరకు ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	ఒకవేళ (p != ఖాళీ)
	{
	    అయితే (*p != NUL)
		ఒకవేళ (fputc(*p++, fd) < 0)
		    తిరిగి FAIL;
	    --str;
	    కొనసాగు;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	ఒకవేళ (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    ఒకవేళ (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		ఒకవేళ (p == ఖాళీ)
		    c = *str;
		లేకపోతే
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    ఒకవేళ (c == K_SPECIAL)
	    {
		c = గమ్యం_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    ఒకవేళ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ఒకవేళ (fputs((అక్షరం *)get_special_కీలకం_name(c, modifiers), fd) < 0)
		    తిరిగి FAIL;
		కొనసాగు;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	ఒకవేళ (c == NL)
	{
	    ఒకవేళ (what == 2)
	    {
		ఒకవేళ (fprintf(fd, "\\\026\n") < 0)
		    తిరిగి FAIL;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (fprintf(fd, "<NL>") < 0)
		    తిరిగి FAIL;
	    }
	    కొనసాగు;
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
	ఒకవేళ (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    ఒకవేళ (putc('\\', fd) < 0)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    ఒకవేళ (putc(Ctrl_V, fd) < 0)
		తిరిగి FAIL;
	}
	ఒకవేళ (putc(c, fd) < 0)
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    శూన్య
check_map_keycodes(శూన్య)
{
    mapblock_T	*mp;
    char_u	*p;
    పూర్ణాంకం		i;
    char_u	buf[3];
    పూర్ణాంకం		abbr;
    పూర్ణాంకం		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_నెట్టు(ETYPE_అంతర్గత, (char_u *)"mappings", 0);
    ESTACK_CHECK_అమరిక;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    కొరకు (bp = firstbuf; ; bp = bp->b_తదుపరి)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	కొరకు (abbr = 0; abbr <= 1; ++abbr)
	    కొరకు (hash = 0; hash < 256; ++hash)
	    {
		ఒకవేళ (abbr)
		{
		    ఒకవేళ (hash)	    // there is only one abbr list
			ఆపు;
		    ఒకవేళ (bp != ఖాళీ)
			mp = bp->b_మొదటి_abbr;
		    లేకపోతే
			mp = మొదటి_abbr;
		}
		లేకపోతే
		{
		    ఒకవేళ (bp != ఖాళీ)
			mp = bp->b_maphash[hash];
		    లేకపోతే
			mp = maphash[hash];
		}
		కొరకు ( ; mp != ఖాళీ; mp = mp->m_తదుపరి)
		{
		    కొరకు (i = 0; i <= 1; ++i)	// do this twice
		    {
			ఒకవేళ (i == 0)
			    p = mp->m_కీలకాలు;	// once for the "from" part
			లేకపోతే
			    p = mp->m_str;	// and once for the "to" part
			అయితే (*p)
			{
			    ఒకవేళ (*p == K_SPECIAL)
			    {
				++p;
				ఒకవేళ (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (శూన్య)కూడు_termcap_నమోదు(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	ఒకవేళ (bp == ఖాళీ)
	    ఆపు;
    }
    ESTACK_CHECK_NOW;
    estack_తీయి();
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*కీలకాలు,
    పూర్ణాంకం		బహుళకం,
    పూర్ణాంకం		exact,		// require exact match
    పూర్ణాంకం		ign_mod,	// ignore preceding modifier
    పూర్ణాంకం		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    పూర్ణాంకం		*local_ptr)	// return: buffer-local mapping or NULL
{
    పూర్ణాంకం		hash;
    పూర్ణాంకం		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    పూర్ణాంకం		local;

    validate_maphash();

    len = (పూర్ణాంకం)STRLEN(కీలకాలు);
    కొరకు (local = 1; local >= 0; --local)
	// loop over all hash lists
	కొరకు (hash = 0; hash < 256; ++hash)
	{
	    ఒకవేళ (abbr)
	    {
		ఒకవేళ (hash > 0)		// there is only one list.
		    ఆపు;
		ఒకవేళ (local)
		    mp = curbuf->b_మొదటి_abbr;
		లేకపోతే
		    mp = మొదటి_abbr;
	    }
	    లేకపోతే ఒకవేళ (local)
		mp = curbuf->b_maphash[hash];
	    లేకపోతే
		mp = maphash[hash];
	    కొరకు ( ; mp != ఖాళీ; mp = mp->m_తదుపరి)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		ఒకవేళ ((mp->m_బహుళకం & బహుళకం) && (!exact || mp->m_keylen == len))
		{
		    ఒకవేళ (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    లేకపోతే
			minlen = len;
		    s = mp->m_కీలకాలు;
		    ఒకవేళ (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			ఒకవేళ (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    ఒకవేళ (STRNCMP(s, కీలకాలు, minlen) == 0)
		    {
			ఒకవేళ (mp_ptr != ఖాళీ)
			    *mp_ptr = mp;
			ఒకవేళ (local_ptr != ఖాళీ)
			    *local_ptr = local;
			తిరిగి mp->m_str;
		    }
		}
	    }
	}

    తిరిగి ఖాళీ;
}

/*
 * "hasmapto()" function
 */
    శూన్య
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*బహుళకం;
    char_u	buf[NUMBUFLEN];
    పూర్ణాంకం		abbr = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	తిరిగి;

    name = tv_get_string(&argvars[0]);
    ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
	బహుళకం = (char_u *)"nvo";
    లేకపోతే
    {
	బహుళకం = tv_get_string_buf(&argvars[1], buf);
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (పూర్ణాంకం)tv_get_bool(&argvars[2]);
    }

    ఒకవేళ (map_గమ్యం_exists(name, బహుళకం, abbr))
	rettv->vval.v_సంఖ్య = TRUE;
    లేకపోతే
	rettv->vval.v_సంఖ్య = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    స్థిర శూన్య
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	పూర్ణాంకం	    బఫరు_local,   // false if not buffer local mapping
	పూర్ణాంకం	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_కీలకాలు, TRUE, FALSE);
    char_u	    *mapmode = map_బహుళకం_గమ్యం_chars(mp->m_బహుళకం);

    dict_కూడు_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_కూడు_string(dict, "lhsraw", mp->m_కీలకాలు);
    ఒకవేళ (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_కూడు_string(dict, "lhsrawalt", lhsrawalt);
    dict_కూడు_string(dict, "rhs", mp->m_orig_str);
    dict_కూడు_సంఖ్య(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_కూడు_సంఖ్య(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_కూడు_సంఖ్య(dict, "expr", mp->m_సమాసం ? 1L : 0L);
    dict_కూడు_సంఖ్య(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_కూడు_సంఖ్య(dict, "sid", (దీర్ఘ)mp->m_script_ctx.sc_sid);
    dict_కూడు_సంఖ్య(dict, "scriptversion",
		    (దీర్ఘ)mp->m_script_ctx.sc_సంస్కరణ);
    dict_కూడు_సంఖ్య(dict, "lnum", (దీర్ఘ)mp->m_script_ctx.sc_lnum);
    dict_కూడు_సంఖ్య(dict, "buffer", (దీర్ఘ)బఫరు_local);
    dict_కూడు_సంఖ్య(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_కూడు_string(dict, "mode", mapmode);
    dict_కూడు_సంఖ్య(dict, "abbr", abbr ? 1L : 0L);
    dict_కూడు_సంఖ్య(dict, "mode_bits", mp->m_బహుళకం);

    vim_free(mapmode);
}

    స్థిర శూన్య
get_maparg(typval_T *argvars, typval_T *rettv, పూర్ణాంకం exact)
{
    char_u	*కీలకాలు;
    char_u	*కీలకాలు_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*కీలకాలు_buf = ఖాళీ;
    char_u	*alt_కీలకాలు_buf = ఖాళీ;
    పూర్ణాంకం		did_simplify = FALSE;
    char_u	*rhs;
    పూర్ణాంకం		బహుళకం;
    పూర్ణాంకం		abbr = FALSE;
    పూర్ణాంకం		get_dict = FALSE;
    mapblock_T	*mp = ఖాళీ;
    పూర్ణాంకం		బఫరు_local;
    పూర్ణాంకం		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    కీలకాలు = tv_get_string(&argvars[0]);
    ఒకవేళ (*కీలకాలు == NUL)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (పూర్ణాంకం)tv_get_bool(&argvars[2]);
	    ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (పూర్ణాంకం)tv_get_bool(&argvars[3]);
	}
    }
    లేకపోతే
	which = (char_u *)"";
    ఒకవేళ (which == ఖాళీ)
	తిరిగి;

    బహుళకం = get_map_బహుళకం(&which, 0);

    కీలకాలు_simplified = replace_termcodes(కీలకాలు, &కీలకాలు_buf, 0, flags,
								&did_simplify);
    rhs = check_map(కీలకాలు_simplified, బహుళకం, exact, FALSE, abbr,
							   &mp, &బఫరు_local);
    ఒకవేళ (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(శూన్య)replace_termcodes(కీలకాలు, &alt_కీలకాలు_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, ఖాళీ);
	rhs = check_map(alt_కీలకాలు_buf, బహుళకం, exact, FALSE, abbr, &mp,
								&బఫరు_local);
    }

    ఒకవేళ (!get_dict)
    {
	// Return a string.
	ఒకవేళ (rhs != ఖాళీ)
	{
	    ఒకవేళ (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    లేకపోతే
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    లేకపోతే ఒకవేళ (rettv_dict_alloc(rettv) == OK && rhs != ఖాళీ)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? కీలకాలు_simplified : ఖాళీ,
			  బఫరు_local, abbr);

    vim_free(కీలకాలు_buf);
    vim_free(alt_కీలకాలు_buf);
}

/*
 * "maplist()" function
 */
    శూన్య
f_maplist(typval_T *argvars వాడని, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    పూర్ణాంకం		బఫరు_local;
    char_u	*కీలకాలు_buf;
    పూర్ణాంకం		did_simplify;
    పూర్ణాంకం		hash;
    char_u	*lhs;
    స్థిరరాశి పూర్ణాంకం	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    పూర్ణాంకం		abbr = FALSE;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	తిరిగి;
    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    కొరకు (బఫరు_local = 0; బఫరు_local <= 1; ++బఫరు_local)
    {
	కొరకు (hash = 0; hash < 256; ++hash)
	{
	    ఒకవేళ (abbr)
	    {
		ఒకవేళ (hash > 0)		// there is only one abbr list
		    ఆపు;
		ఒకవేళ (బఫరు_local)
		    mp = curbuf->b_మొదటి_abbr;
		లేకపోతే
		    mp = మొదటి_abbr;
	    }
	    లేకపోతే ఒకవేళ (బఫరు_local)
		mp = curbuf->b_maphash[hash];
	    లేకపోతే
		mp = maphash[hash];
	    కొరకు (; mp; mp = mp->m_తదుపరి)
	    {
		ఒకవేళ (mp->m_simplified)
		    కొనసాగు;
		ఒకవేళ ((d = dict_alloc()) == ఖాళీ)
		    తిరిగి;
		ఒకవేళ (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    తిరిగి;
		}

		కీలకాలు_buf = ఖాళీ;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_కీలకాలు, TRUE, FALSE);
		(శూన్య)replace_termcodes(lhs, &కీలకాలు_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? కీలకాలు_buf : ఖాళీ,
				 బఫరు_local, abbr);
		vim_free(కీలకాలు_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    శూన్య
f_maparg(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		తిరిగి;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    శూన్య
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	తిరిగి;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    స్థిర పూర్ణాంకం
get_map_బహుళకం_string(char_u *బహుళకం_string, పూర్ణాంకం abbr)
{
    char_u	*p = బహుళకం_string;
    పూర్ణాంకం		బహుళకం = 0;
    పూర్ణాంకం		tmode;
    పూర్ణాంకం		modec;
    స్థిరరాశి పూర్ణాంకం	ముసుగు_V = బహుళకం_VISUAL | బహుళకం_SELECT;
    స్థిరరాశి పూర్ణాంకం	ముసుగు_MAP = బహుళకం_VISUAL | బహుళకం_SELECT | బహుళకం_సాధారణ
							     | బహుళకం_OP_PENDING;
    స్థిరరాశి పూర్ణాంకం	ముసుగు_BANG = బహుళకం_చొప్పించు | బహుళకం_CMDLINE;

    ఒకవేళ (*p == NUL)
	p = (char_u *)" ";	// compatibility
    అయితే ((modec = *p++))
    {
	ఎంపిక (modec)
	{
	    స్థితి 'i': tmode = బహుళకం_చొప్పించు;	ఆపు;
	    స్థితి 'l': tmode = బహుళకం_LANGMAP;	ఆపు;
	    స్థితి 'c': tmode = బహుళకం_CMDLINE;	ఆపు;
	    స్థితి 'n': tmode = బహుళకం_సాధారణ;	ఆపు;
	    స్థితి 'x': tmode = బహుళకం_VISUAL;	ఆపు;
	    స్థితి 's': tmode = బహుళకం_SELECT;	ఆపు;
	    స్థితి 'o': tmode = బహుళకం_OP_PENDING;	ఆపు;
	    స్థితి 't': tmode = బహుళకం_TERMINAL;	ఆపు;
	    స్థితి 'v': tmode = ముసుగు_V;		ఆపు;
	    స్థితి '!': tmode = ముసుగు_BANG;	ఆపు;
	    స్థితి ' ': tmode = ముసుగు_MAP;		ఆపు;
	    అప్రమేయ:
		      తిరిగి 0; // error, unknown mode character
	}
	బహుళకం |= tmode;
    }
    ఒకవేళ ((abbr && (బహుళకం & ~ముసుగు_BANG) != 0)
	|| (!abbr && (బహుళకం & (బహుళకం-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((బహుళకం & ముసుగు_BANG) != 0 && (బహుళకం & ~ముసుగు_BANG) == 0)
		    || ((బహుళకం & ముసుగు_MAP) != 0 && (బహుళకం & ~ముసుగు_MAP) == 0)))))
	తిరిగి 0;

    తిరిగి బహుళకం;
}

/*
 * "mapset()" function
 */
    శూన్య
f_mapset(typval_T *argvars, typval_T *rettv వాడని)
{
    char_u	*which;
    పూర్ణాంకం		బహుళకం;
    char_u	buf[NUMBUFLEN];
    పూర్ణాంకం		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = ఖాళీ;
    పూర్ణాంకం		noremap;
    పూర్ణాంకం		సమాసం;
    పూర్ణాంకం		silent;
    పూర్ణాంకం		బఫరు;
    scid_T	sid;
    పూర్ణాంకం		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_పట్టిక = maphash;
    mapblock_T  **abbr_పట్టిక = &మొదటి_abbr;
    పూర్ణాంకం		nowait;
    char_u	*arg;
    పూర్ణాంకం		dict_only;
    mapblock_T	*mp_ఫలితం[2] = {ఖాళీ, ఖాళీ};

    ఒకవేళ (check_secure())
	తిరిగి;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	తిరిగి;

    ఒకవేళ (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	ఒకవేళ (which == ఖాళీ || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_లోపల_mapset_dict_వాదం));
	    తిరిగి;
	}
    }
    లేకపోతే
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	ఒకవేళ (which == ఖాళీ)
	    తిరిగి;
	is_abbr = (పూర్ణాంకం)tv_get_bool(&argvars[1]);

	ఒకవేళ (check_for_dict_arg(argvars, 2) == FAIL)
	    తిరిగి;
	d = argvars[2].vval.v_dict;
    }
    బహుళకం = get_map_బహుళకం_string(which, is_abbr);
    ఒకవేళ (బహుళకం == 0)
    {
	semsg(_(e_illegal_map_బహుళకం_string_str), which);
	తిరిగి;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    ఒకవేళ (lhs == ఖాళీ || lhsraw == ఖాళీ || rhs == ఖాళీ)
    {
	emsg(_(e_entries_missing_లోపల_mapset_dict_వాదం));
	తిరిగి;
    }
    orig_rhs = rhs;

    noremap = dict_get_సంఖ్య(d, "noremap") ? REMAP_NONE: 0;
    ఒకవేళ (dict_get_సంఖ్య(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    సమాసం = dict_get_సంఖ్య(d, "expr") != 0;
    silent = dict_get_సంఖ్య(d, "silent") != 0;
    sid = dict_get_సంఖ్య(d, "sid");
    scriptversion = dict_get_సంఖ్య(d, "scriptversion");
    lnum = dict_get_సంఖ్య(d, "lnum");
    బఫరు = dict_get_సంఖ్య(d, "buffer");
    nowait = dict_get_సంఖ్య(d, "nowait") != 0;
    // mode from the dict is not used

    ఒకవేళ (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    లేకపోతే
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, ఖాళీ);

    ఒకవేళ (బఫరు)
    {
	map_పట్టిక = curbuf->b_maphash;
	abbr_పట్టిక = &curbuf->b_మొదటి_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    ఒకవేళ (బఫరు)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	ఒకవేళ (arg == ఖాళీ)
	    తిరిగి;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    లేకపోతే
    {
	arg = vim_strsave(lhs);
	ఒకవేళ (arg == ఖాళీ)
	    తిరిగి;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, బహుళకం, is_abbr);
    vim_free(arg);

    mp_ఫలితం[0] = map_కూడు(map_పట్టిక, abbr_పట్టిక, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, బహుళకం, is_abbr, సమాసం, sid,
			    scriptversion, lnum, 0);
    ఒకవేళ (lhsrawalt != ఖాళీ)
	mp_ఫలితం[1] = map_కూడు(map_పట్టిక, abbr_పట్టిక, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, బహుళకం, is_abbr, సమాసం, sid,
			    scriptversion, lnum, 1);

    ఒకవేళ (mp_ఫలితం[0] != ఖాళీ && mp_ఫలితం[1] != ఖాళీ)
    {
	mp_ఫలితం[0]->m_alt = mp_ఫలితం[1];
	mp_ఫలితం[1]->m_alt = mp_ఫలితం[0];
    }

    vim_free(arg_buf);
}
#అయితే_ముగింపు


#ఒకవేళ defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(బహుళకం_VISUAL | బహుళకం_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
నిర్మాణం initmap
{
    char_u	*arg;
    పూర్ణాంకం		బహుళకం;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
స్థిర నిర్మాణం initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", బహుళకం_సాధారణ},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", బహుళకం_చొప్పించు | బహుళకం_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
స్థిర నిర్మాణం initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", బహుళకం_సాధారణ | VIS_SEL},
	{(char_u *)"\316w <C-Home>", బహుళకం_చొప్పించు | బహుళకం_CMDLINE},
	{(char_u *)"\316u <C-End>", బహుళకం_సాధారణ | VIS_SEL},
	{(char_u *)"\316u <C-End>", బహుళకం_చొప్పించు | బహుళకం_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", బహుళకం_సాధారణ},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", బహుళకం_చొప్పించు},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  లేకపోతే
	{(char_u *)"\316\324 P", బహుళకం_సాధారణ},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", బహుళకం_చొప్పించు}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# ఒకవేళ defined(MACOS_X)
స్థిర నిర్మాణం initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", బహుళకం_సాధారణ},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", బహుళకం_చొప్పించు | బహుళకం_CMDLINE},
	{(char_u *)"<D-c> \"*y", VIS_SEL},
	{(char_u *)"<D-x> \"*d", VIS_SEL},
	{(char_u *)"<Backspace> \"-d", VIS_SEL},
};
# endif

# undef VIS_SEL
#అయితే_ముగింపు

/*
 * Set up default mappings.
 */
    శూన్య
init_mappings(శూన్య)
{
#ఒకవేళ defined(MSWIN) || defined(MACOS_X)
    పూర్ణాంకం		i;

# ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ఒకవేళ (!gui.starting)
#  endif
    {
	కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(cinitmappings); ++i)
	    కూడు_map(cinitmappings[i].arg, cinitmappings[i].బహుళకం, FALSE);
    }
# endif
# ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(initmappings); ++i)
	కూడు_map(initmappings[i].arg, initmappings[i].బహుళకం, FALSE);
# endif
#అయితే_ముగింపు
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    శూన్య
కూడు_map(char_u *map, పూర్ణాంకం బహుళకం, పూర్ణాంకం nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = ఖాళీలు_option;	// Allow <> notation
    s = vim_strsave(map);
    ఒకవేళ (s != ఖాళీ)
    {
	(శూన్య)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, బహుళకం, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#ఒకవేళ defined(FEAT_LANGMAP)
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
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	    from;
    పూర్ణాంకం     గమ్యం;
} langmap_నమోదు_T;

స్థిర garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    స్థిర శూన్య
langmap_సమితి_నమోదు(పూర్ణాంకం from, పూర్ణాంకం గమ్యం)
{
    langmap_నమోదు_T *entries = (langmap_నమోదు_T *)(langmap_mapga.ga_దత్తాంశం);
    పూర్ణాంకం		    a = 0;
    పూర్ణాంకం		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    అయితే (a != b)
    {
	పూర్ణాంకం i = (a + b) / 2;
	పూర్ణాంకం d = entries[i].from - from;

	ఒకవేళ (d == 0)
	{
	    entries[i].గమ్యం = గమ్యం;
	    తిరిగి;
	}
	ఒకవేళ (d < 0)
	    a = i + 1;
	లేకపోతే
	    b = i;
    }

    ఒకవేళ (ga_grow(&langmap_mapga, 1) == FAIL)
	తిరిగి;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_నమోదు_T *)(langmap_mapga.ga_దత్తాంశం) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * పరిమాణం(langmap_నమోదు_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].గమ్యం = గమ్యం;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    పూర్ణాంకం
langmap_adjust_mb(పూర్ణాంకం c)
{
    langmap_నమోదు_T *entries = (langmap_నమోదు_T *)(langmap_mapga.ga_దత్తాంశం);
    పూర్ణాంకం a = 0;
    పూర్ణాంకం b = langmap_mapga.ga_len;

    అయితే (a != b)
    {
	పూర్ణాంకం i = (a + b) / 2;
	పూర్ణాంకం d = entries[i].from - c;

	ఒకవేళ (d == 0)
	    తిరిగి entries[i].గమ్యం;  // found matching entry
	ఒకవేళ (d < 0)
	    a = i + 1;
	లేకపోతే
	    b = i;
    }
    తిరిగి c;  // no entry found, return "c" unmodified
}

    శూన్య
langmap_init(శూన్య)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, పరిమాణం(langmap_నమోదు_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    అక్షరం *
did_సమితి_langmap(optset_T *వాదనలు వాడని)
{
    char_u  *p;
    char_u  *p2;
    పూర్ణాంకం	    from, గమ్యం;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    కొరకు (p = p_langmap; p[0] != NUL; )
    {
	కొరకు (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    ఒకవేళ (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	ఒకవేళ (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	లేకపోతే
	    p2 = ఖాళీ;	    // aAbBcCdD form, p2 is NULL
	అయితే (p[0])
	{
	    ఒకవేళ (p[0] == ',')
	    {
		++p;
		ఆపు;
	    }
	    ఒకవేళ (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    గమ్యం = NUL;
	    ఒకవేళ (p2 == ఖాళీ)
	    {
		MB_PTR_ADV(p);
		ఒకవేళ (p[0] != ',')
		{
		    ఒకవేళ (p[0] == '\\')
			++p;
		    గమ్యం = (*mb_ptr2char)(p);
		}
	    }
	    లేకపోతే
	    {
		ఒకవేళ (p2[0] != ',')
		{
		    ఒకవేళ (p2[0] == '\\')
			++p2;
		    గమ్యం = (*mb_ptr2char)(p2);
		}
	    }
	    ఒకవేళ (గమ్యం == NUL)
	    {
		sprintf(వాదనలు->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		తిరిగి వాదనలు->os_errbuf;
	    }

	    ఒకవేళ (from >= 256)
		langmap_సమితి_నమోదు(from, గమ్యం);
	    లేకపోతే
		langmap_mapchar[from & 255] = గమ్యం;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    ఒకవేళ (p2 != ఖాళీ)
	    {
		MB_PTR_ADV(p2);
		ఒకవేళ (*p == ';')
		{
		    p = p2;
		    ఒకవేళ (p[0] != NUL)
		    {
			ఒకవేళ (p[0] != ',')
			{
			    vim_snprintf(వాదనలు->os_errbuf, వాదనలు->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    తిరిగి వాదనలు->os_errbuf;
			}
			++p;
		    }
		    ఆపు;
		}
	    }
	}
    }

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

    స్థిర శూన్య
do_exmap(exarg_T *eap, పూర్ణాంకం isabbrev)
{
    పూర్ణాంకం	    బహుళకం;
    char_u  *cmdp;

    cmdp = eap->cmd;
    బహుళకం = get_map_బహుళకం(&cmdp, eap->forceit || isabbrev);

    ఎంపిక (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, బహుళకం, isabbrev))
    {
	స్థితి 1: emsg(_(e_invalid_వాదం));
		ఆపు;
	స్థితి 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		ఆపు;
    }
}

/*
 * ":abbreviate" and friends.
 */
    శూన్య
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    శూన్య
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    ఒకవేళ (secure)
    {
	secure = 2;
	సందేశం_outtrans(eap->cmd);
	సందేశం_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    శూన్య
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    శూన్య
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    శూన్య
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
