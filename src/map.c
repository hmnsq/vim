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

#ಅನ್ತರ್ಭಾವ "vim.h"

/*
 * List used for abbreviations.
 */
ಸ್ಥಿರ mapblock_T	*first_abbr = ರಿಕ್ತ; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
ಸ್ಥಿರ mapblock_T	*(maphash[256]);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		map_locked = 0;

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
get_maphash_list(ಪೂರ್ಣಾಙ್ಕ state, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(ಪೂರ್ಣಾಙ್ಕ state, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ curbuf->b_maphash[MAP_HASH(state, c)];
}

    ಪೂರ್ಣಾಙ್ಕ
is_maphash_valid(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
validate_maphash(ಶೂನ್ಯ)
{
    ಯದಿ (maphash_valid)
	ಪ್ರತ್ಯಾಗಮ;

    CLEAR_FIELD(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_ಕೀಲಿಗಳು);
    ಯದಿ (mp->m_alt != ರಿಕ್ತ)
	mp->m_alt->m_alt = ರಿಕ್ತ;
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
    ಸ್ಥಿರ char_u *
map_mode_to_chars(ಪೂರ್ಣಾಙ್ಕ mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    ಯದಿ ((mode & (MODE_ಸೇರಿಸು | MODE_CMDLINE)) == (MODE_ಸೇರಿಸು | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    ಅನ್ಯಥಾ ಯದಿ (mode & MODE_ಸೇರಿಸು)
	ga_append(&mapmode, 'i');			// :imap
    ಅನ್ಯಥಾ ಯದಿ (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    ಅನ್ಯಥಾ ಯದಿ (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    ಅನ್ಯಥಾ ಯದಿ ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    ಅನ್ಯಥಾ
    {
	ಯದಿ (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	ಯದಿ (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	ಯದಿ (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	ಯದಿ ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    ಯದಿ (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    ಪ್ರತ್ಯಾಗಮ (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
showmap(
    mapblock_T	*mp,
    ಪೂರ್ಣಾಙ್ಕ		local)	    // TRUE for buffer-local map
{
    ಪೂರ್ಣಾಙ್ಕ		len = 1;
    char_u	*mapchars;

    ಯದಿ (message_filtered(mp->m_ಕೀಲಿಗಳು) && message_filtered(mp->m_str))
	ಪ್ರತ್ಯಾಗಮ;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    ಯದಿ (ಸಂದೇಶ_didout || ಸಂದೇಶ_silent != 0)
    {
	ಸಂದೇಶ_putchar('\n');
	ಯದಿ (got_int)	    // 'q' typed at MORE prompt
	    ಗಚ್ಛ theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    ಯದಿ (mapchars != ರಿಕ್ತ)
    {
	ಸಂದೇಶ_puts((ವರ್ಣ *)mapchars);
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(mapchars);
	vim_free(mapchars);
    }

    ಯಾವತ್ (++len <= 3)
	ಸಂದೇಶ_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = ಸಂದೇಶ_outtrans_special(mp->m_ಕೀಲಿಗಳು, TRUE, 0);
    ಕುರು
    {
	ಸಂದೇಶ_putchar(' ');		// pad with blanks
	++len;
    } ಯಾವತ್ (len < 12);

    ಯದಿ (mp->m_noremap == REMAP_NONE)
	ಸಂದೇಶ_puts_attr("*", HL_ATTR(HLF_8));
    ಅನ್ಯಥಾ ಯದಿ (mp->m_noremap == REMAP_SCRIPT)
	ಸಂದೇಶ_puts_attr("&", HL_ATTR(HLF_8));
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_putchar(' ');

    ಯದಿ (local)
	ಸಂದೇಶ_putchar('@');
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    ಯದಿ (*mp->m_str == NUL)
	ಸಂದೇಶ_puts_attr("<Nop>", HL_ATTR(HLF_8));
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    ಯದಿ (p_verbose > 0)
	last_set_ಸಂದೇಶ(mp->m_script_ctx);
#endif
    ಸಂದೇಶ_clr_eos();
    out_flush();			// show one line at a time

theend:
    --map_locked;
}

    ಸ್ಥಿರ mapblock_T *
map_add(
	mapblock_T  **map_table,
	mapblock_T  **abbr_table,
	char_u	    *ಕೀಲಿಗಳು,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	ಪೂರ್ಣಾಙ್ಕ	    noremap,
	ಪೂರ್ಣಾಙ್ಕ	    nowait,
	ಪೂರ್ಣಾಙ್ಕ	    silent,
	ಪೂರ್ಣಾಙ್ಕ	    mode,
	ಪೂರ್ಣಾಙ್ಕ	    is_abbr,
#ifdef FEAT_EVAL
	ಪೂರ್ಣಾಙ್ಕ	    ಅಭಿವ್ಯಕ್ತಿ,
	scid_T	    sid,	    // 0 to use current_sctx
	ಪೂರ್ಣಾಙ್ಕ	    scriptversion,
	linenr_T    lnum,
#endif
	ಪೂರ್ಣಾಙ್ಕ	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    ಯದಿ (mp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    ಯದಿ (*ಕೀಲಿಗಳು == Ctrl_C)
    {
	ಯದಿ (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	ಅನ್ಯಥಾ
	    mapped_ctrl_c |= mode;
    }

    mp->m_ಕೀಲಿಗಳು = vim_strsave(ಕೀಲಿಗಳು);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    ಯದಿ (mp->m_ಕೀಲಿಗಳು == ರಿಕ್ತ || mp->m_str == ರಿಕ್ತ)
    {
	vim_free(mp->m_ಕೀಲಿಗಳು);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    mp->m_keylen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(mp->m_ಕೀಲಿಗಳು);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_ಅಭಿವ್ಯಕ್ತಿ = ಅಭಿವ್ಯಕ್ತಿ;
    ಯದಿ (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    ಅನ್ಯಥಾ
    {
	mp->m_script_ctx = current_sctx;
	ಯದಿ (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    ಯದಿ (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ n = MAP_HASH(mp->m_mode, mp->m_ಕೀಲಿಗಳು[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    ಪ್ರತ್ಯಾಗಮ mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_mappings(
	ಪೂರ್ಣಾಙ್ಕ	keyround,
	ಪೂರ್ಣಾಙ್ಕ	abbrev,
	ಪೂರ್ಣಾಙ್ಕ	haskey,
	char_u	*ಕೀಲಿಗಳು,
	ಪೂರ್ಣಾಙ್ಕ	ಕೀಲಿಗಳು_len,
	ಪೂರ್ಣಾಙ್ಕ	mode,
	ಪೂರ್ಣಾಙ್ಕ	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    ಯದಿ (p_verbose > 0 && keyround == 1)
    {
	ಯದಿ (seen_Modify_Other_ಕೀಲಿಗಳು)
	    ಸಂದೇಶ_puts(_("Seen modifyOtherKeys: true\n"));

	ಯದಿ (modify_otherkeys_state != MOKS_INITIAL)
	{
	    ವರ್ಣ *name = _("Unknown");
	    ವಿಕಲ್ಪ (modify_otherkeys_state)
	    {
		ಪಕ್ಷ MOKS_INITIAL: ಭಙ್ಗ;
		ಪಕ್ಷ MOKS_OFF: name = _("Off"); ಭಙ್ಗ;
		ಪಕ್ಷ MOKS_ENABLED: name = _("On"); ಭಙ್ಗ;
		ಪಕ್ಷ MOKS_DISABLED: name = _("Disabled"); ಭಙ್ಗ;
		ಪಕ್ಷ MOKS_AFTER_T_TE: name = _("Cleared"); ಭಙ್ಗ;
	    }

	    ವರ್ಣ buf[200];
	    vim_snprintf(buf, ಪರಿಮಾಣ(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    ಸಂದೇಶ_puts(buf);
	}

	ಯದಿ (kitty_protocol_state != KKPS_INITIAL)
	{
	    ವರ್ಣ *name = _("Unknown");
	    ವಿಕಲ್ಪ (kitty_protocol_state)
	    {
		ಪಕ್ಷ KKPS_INITIAL: ಭಙ್ಗ;
		ಪಕ್ಷ KKPS_OFF: name = _("Off"); ಭಙ್ಗ;
		ಪಕ್ಷ KKPS_ENABLED: name = _("On"); ಭಙ್ಗ;
		ಪಕ್ಷ KKPS_DISABLED: name = _("Disabled"); ಭಙ್ಗ;
		ಪಕ್ಷ KKPS_AFTER_T_TE: name = _("Cleared"); ಭಙ್ಗ;
	    }

	    ವರ್ಣ buf[200];
	    vim_snprintf(buf, ಪರಿಮಾಣ(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    ಸಂದೇಶ_puts(buf);
	}
    }

    // need to loop over all global hash lists
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	ಯದಿ (abbrev)
	{
	    ಯದಿ (hash != 0)	// there is only one abbreviation list
		ಭಙ್ಗ;
	    mp = curbuf->b_first_abbr;
	}
	ಅನ್ಯಥಾ
	    mp = curbuf->b_maphash[hash];
	ಪ್ರತಿ ( ; mp != ರಿಕ್ತ && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    ಯದಿ (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		ಯದಿ (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		ಅನ್ಯಥಾ
		{
		    ಪೂರ್ಣಾಙ್ಕ n = mp->m_keylen;
		    ಯದಿ (STRNCMP(mp->m_ಕೀಲಿಗಳು, ಕೀಲಿಗಳು,
				   (size_t)(n < ಕೀಲಿಗಳು_len ? n : ಕೀಲಿಗಳು_len)) == 0)
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
    ಪೂರ್ಣಾಙ್ಕ
do_map(
    ಪೂರ್ಣಾಙ್ಕ		maptype,
    char_u	*arg,
    ಪೂರ್ಣಾಙ್ಕ		mode,
    ಪೂರ್ಣಾಙ್ಕ		abbrev)		// not a mapping but an abbreviation
{
    char_u	*ಕೀಲಿಗಳು;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {ರಿಕ್ತ, ರಿಕ್ತ};
    char_u	*rhs;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;	// init for GCC
    ಪೂರ್ಣಾಙ್ಕ		hasarg;
    ಪೂರ್ಣಾಙ್ಕ		haskey;
    ಪೂರ್ಣಾಙ್ಕ		do_print;
    ಪೂರ್ಣಾಙ್ಕ		keyround;
    char_u	*ಕೀಲಿಗಳು_buf = ರಿಕ್ತ;
    char_u	*alt_ಕೀಲಿಗಳು_buf = ರಿಕ್ತ;
    char_u	*arg_buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		retval = 0;
    ಪೂರ್ಣಾಙ್ಕ		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    ಪೂರ್ಣಾಙ್ಕ		unique = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		nowait = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		silent = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		special = FALSE;
#ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ		ಅಭಿವ್ಯಕ್ತಿ = FALSE;
#endif
    ಪೂರ್ಣಾಙ್ಕ		did_simplify = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		unmap_lhs_only = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		noremap;
    char_u      *orig_rhs;

    ಕೀಲಿಗಳು = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    ಯದಿ (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    ಯದಿ (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    ಅನ್ಯಥಾ
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    ಪ್ರತಿ (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	ಯದಿ (STRNCMP(ಕೀಲಿಗಳು, "<buffer>", 8) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    ಅನುವರ್ತ;
	}

	// Check for "<nowait>": don't wait for more characters.
	ಯದಿ (STRNCMP(ಕೀಲಿಗಳು, "<nowait>", 8) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 8);
	    nowait = TRUE;
	    ಅನುವರ್ತ;
	}

	// Check for "<silent>": don't echo commands.
	ಯದಿ (STRNCMP(ಕೀಲಿಗಳು, "<silent>", 8) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 8);
	    silent = TRUE;
	    ಅನುವರ್ತ;
	}

	// Check for "<special>": accept special keys in <>
	ಯದಿ (STRNCMP(ಕೀಲಿಗಳು, "<special>", 9) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 9);
	    special = TRUE;
	    ಅನುವರ್ತ;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	ಯದಿ (STRNCMP(ಕೀಲಿಗಳು, "<script>", 8) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 8);
	    noremap = REMAP_SCRIPT;
	    ಅನುವರ್ತ;
	}

	// Check for "<expr>": {rhs} is an expression.
	ಯದಿ (STRNCMP(ಕೀಲಿಗಳು, "<expr>", 6) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 6);
	    ಅಭಿವ್ಯಕ್ತಿ = TRUE;
	    ಅನುವರ್ತ;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	ಯದಿ (STRNCMP(ಕೀಲಿಗಳು, "<unique>", 8) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 8);
	    unique = TRUE;
	    ಅನುವರ್ತ;
	}
	ಭಙ್ಗ;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = ಕೀಲಿಗಳು;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ರಿಕ್ತ);
    ಯಾವತ್ (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	ಯದಿ ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    ಯದಿ (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*ಕೀಲಿಗಳು != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    ಯದಿ (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	ಗಚ್ಛ theend;
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
    ಯದಿ (haskey)
    {
	char_u	*new_ಕೀಲಿಗಳು;
	ಪೂರ್ಣಾಙ್ಕ	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	ಯದಿ (special)
	    flags |= REPTERM_SPECIAL;
	new_ಕೀಲಿಗಳು = replace_termcodes(ಕೀಲಿಗಳು, &ಕೀಲಿಗಳು_buf, 0, flags, &did_simplify);
	ಯದಿ (did_simplify)
	    (ಶೂನ್ಯ)replace_termcodes(ಕೀಲಿಗಳು, &alt_ಕೀಲಿಗಳು_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, ರಿಕ್ತ);
	ಕೀಲಿಗಳು = new_ಕೀಲಿಗಳು;
    }
    orig_rhs = rhs;
    ಯದಿ (hasarg)
    {
	ಯದಿ (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	ಅನ್ಯಥಾ
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ರಿಕ್ತ);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    ಪ್ರತಿ (keyround = 1; keyround <= 2; ++keyround)
    {
	ಪೂರ್ಣಾಙ್ಕ	did_it = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	did_local = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	keyround1_simplified = keyround == 1 && did_simplify;
	ಪೂರ್ಣಾಙ್ಕ	round;
	ಪೂರ್ಣಾಙ್ಕ	ಅಂಕೆ_rounds;

	ಯದಿ (keyround == 2)
	{
	    ಯದಿ (alt_ಕೀಲಿಗಳು_buf == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    ಕೀಲಿಗಳು = alt_ಕೀಲಿಗಳು_buf;
	}
	ಅನ್ಯಥಾ ಯದಿ (alt_ಕೀಲಿಗಳು_buf != ರಿಕ್ತ && do_print)
	    // when printing always use the not-simplified map
	    ಕೀಲಿಗಳು = alt_ಕೀಲಿಗಳು_buf;

	// check arguments and translate function keys
	ಯದಿ (haskey)
	{
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಕೀಲಿಗಳು);
	    ಯದಿ (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		ಗಚ್ಛ theend;
	    }

	    ಯದಿ (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		ಯದಿ (has_mbyte)
		{
		    ಪೂರ್ಣಾಙ್ಕ		first, last;
		    ಪೂರ್ಣಾಙ್ಕ		same = -1;
		    char_u	ಕೀಲಿಗಳು_unescaped[MAXMAPLEN + 1];
		    size_t	ಕೀಲಿಗಳು_unescaped_len;

		    mch_memmove(ಕೀಲಿಗಳು_unescaped, ಕೀಲಿಗಳು, (size_t)(len + 1));
		    ಕೀಲಿಗಳು_unescaped_len = vim_unescape_csi(ಕೀಲಿಗಳು_unescaped);
		    p = ಕೀಲಿಗಳು_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    ಯಾವತ್ (p < ಕೀಲಿಗಳು_unescaped + ಕೀಲಿಗಳು_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			ಯದಿ (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    ಯದಿ (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			ಗಚ್ಛ theend;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (vim_iswordc(ಕೀಲಿಗಳು[len - 1]))
		    // ends in keyword char
		    ಪ್ರತಿ (n = 0; n < len - 2; ++n)
			ಯದಿ (vim_iswordc(ಕೀಲಿಗಳು[n]) != vim_iswordc(ಕೀಲಿಗಳು[len - 2]))
			{
			    retval = 1;
			    ಗಚ್ಛ theend;
			}
		// An abbreviation cannot contain white space.
		ಪ್ರತಿ (n = 0; n < len; ++n)
		    ಯದಿ (VIM_ISWHITE(ಕೀಲಿಗಳು[n]))
		    {
			retval = 1;
			ಗಚ್ಛ theend;
		    }
	    }
	}

	ಯದಿ (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	ಯದಿ (do_print)
	    ಸಂದೇಶ_start();

	// Check if a new local mapping wasn't already defined globally.
	ಯದಿ (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ಯದಿ (abbrev)
		{
		    ಯದಿ (hash != 0)	// there is only one abbreviation list
			ಭಙ್ಗ;
		    mp = first_abbr;
		}
		ಅನ್ಯಥಾ
		    mp = maphash[hash];
		ಪ್ರತಿ ( ; mp != ರಿಕ್ತ && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    ಯದಿ ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_ಕೀಲಿಗಳು, ಕೀಲಿಗಳು, (size_t)len) == 0)
		    {
			ಯದಿ (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_ಕೀಲಿಗಳು);
			ಅನ್ಯಥಾ
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_ಕೀಲಿಗಳು);
			retval = 5;
			ಗಚ್ಛ theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	ಯದಿ (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, ಕೀಲಿಗಳು, len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	ಅಂಕೆ_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	ಪ್ರತಿ (round = 0; round < ಅಂಕೆ_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ಯದಿ (abbrev)
		{
		    ಯದಿ (hash > 0)	// there is only one abbreviation list
			ಭಙ್ಗ;
		    mpp = abbr_table;
		}
		ಅನ್ಯಥಾ
		    mpp = &(map_table[hash]);
		ಪ್ರತಿ (mp = *mpp; mp != ರಿಕ್ತ && !got_int; mp = *mpp)
		{

		    ಯದಿ ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			ಅನುವರ್ತ;
		    }
		    ಯದಿ (!haskey)	// show all entries
		    {
			ಯದಿ (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    ಅನ್ಯಥಾ	// do we have a match?
		    {
			ಯದಿ (round)	// second round: Try unmap "rhs" string
			{
			    n = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			ಅನ್ಯಥಾ
			{
			    n = mp->m_keylen;
			    p = mp->m_ಕೀಲಿಗಳು;
			}
			ಯದಿ (STRNCMP(p, ಕೀಲಿಗಳು, (size_t)(n < len ? n : len)) == 0)
			{
			    ಯದಿ (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				ಯದಿ (n != len && (!abbrev || round || n > len
					       || *skipwhite(ಕೀಲಿಗಳು + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    ಅನುವರ್ತ;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				ಯದಿ (keyround1_simplified && !mp->m_simplified)
				    ಭಙ್ಗ;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    ಅನ್ಯಥಾ ಯದಿ (!hasarg)	// show matching entry
			    {
				ಯದಿ (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    ಅನ್ಯಥಾ ಯದಿ (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				ಅನುವರ್ತ;
			    }
			    ಅನ್ಯಥಾ ಯದಿ (unique)
			    {
				ಯದಿ (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				ಅನ್ಯಥಾ
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				ಗಚ್ಛ theend;
			    }
			    ಅನ್ಯಥಾ
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				ಯದಿ (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    ಯದಿ (newstr == ರಿಕ್ತ)
				    {
					retval = 4;		// no mem
					ಗಚ್ಛ theend;
				    }
				    ಯದಿ (mp->m_alt != ರಿಕ್ತ)
					mp->m_alt = mp->m_alt->m_alt = ರಿಕ್ತ;
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
				    mp->m_ಅಭಿವ್ಯಕ್ತಿ = ಅಭಿವ್ಯಕ್ತಿ;
				    mp->m_script_ctx = current_sctx;
				    ಯದಿ (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    ಯದಿ (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				ಅನುವರ್ತ;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    ಪೂರ್ಣಾಙ್ಕ new_hash = MAP_HASH(mp->m_mode, mp->m_ಕೀಲಿಗಳು[0]);
			    ಯದಿ (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_next;
				mp->m_next = map_table[new_hash];
				map_table[new_hash] = mp;

				ಅನುವರ್ತ;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_next);
		}
	    }
	}

	ಯದಿ (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    ಯದಿ (!did_it)
	    {
		ಯದಿ (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*ಕೀಲಿಗಳು == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		ಯದಿ (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		ಅನ್ಯಥಾ
		    mapped_ctrl_c &= ~mode;
	    }
	    ಅನುವರ್ತ;
	}

	ಯದಿ (!haskey || !hasarg)
	{
	    // print entries
	    ಯದಿ (!did_it && !did_local)
	    {
		ಯದಿ (abbrev)
		    ಸಂದೇಶ(_("No abbreviation found"));
		ಅನ್ಯಥಾ
		    ಸಂದೇಶ(_("No mapping found"));
	    }
	    ಗಚ್ಛ theend;    // listing finished
	}

	ಯದಿ (did_it)
	    ಅನುವರ್ತ;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, ಕೀಲಿಗಳು,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    ಅಭಿವ್ಯಕ್ತಿ, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	ಯದಿ (mp_result[keyround - 1] == ರಿಕ್ತ)
	{
	    retval = 4;	    // no mem
	    ಗಚ್ಛ theend;
	}
    }

    ಯದಿ (mp_result[0] != ರಿಕ್ತ && mp_result[1] != ರಿಕ್ತ)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(ಕೀಲಿಗಳು_buf);
    vim_free(alt_ಕೀಲಿಗಳು_buf);
    vim_free(arg_buf);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Get the mapping mode from the command name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_map_mode(char_u **cmdp, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		modec;
    ಪೂರ್ಣಾಙ್ಕ		mode;

    p = *cmdp;
    modec = *p++;
    ಯದಿ (modec == 'i')
	mode = MODE_ಸೇರಿಸು;				// :imap
    ಅನ್ಯಥಾ ಯದಿ (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    ಅನ್ಯಥಾ ಯದಿ (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    ಅನ್ಯಥಾ ಯದಿ (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    ಅನ್ಯಥಾ ಯದಿ (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    ಅನ್ಯಥಾ ಯದಿ (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    ಅನ್ಯಥಾ ಯದಿ (modec == 's')
	mode = MODE_SELECT;			// :smap
    ಅನ್ಯಥಾ ಯದಿ (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    ಅನ್ಯಥಾ ಯದಿ (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    ಅನ್ಯಥಾ
    {
	--p;
	ಯದಿ (forceit)
	    mode = MODE_ಸೇರಿಸು | MODE_CMDLINE;		// :map !
	ಅನ್ಯಥಾ
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    ಪ್ರತ್ಯಾಗಮ mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    ಪೂರ್ಣಾಙ್ಕ		forceit,
    ಪೂರ್ಣಾಙ್ಕ		abbr)
{
    ಪೂರ್ಣಾಙ್ಕ		mode;
    ಪೂರ್ಣಾಙ್ಕ		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    ಯದಿ (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	ಪ್ರತ್ಯಾಗಮ;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_map_locked(ಶೂನ್ಯ)
{
    ಯದಿ (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    ಶೂನ್ಯ
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    ಪೂರ್ಣಾಙ್ಕ		mode,		// mode in which to delete
    ಪೂರ್ಣಾಙ್ಕ		local,		// TRUE for buffer-local mappings
    ಪೂರ್ಣಾಙ್ಕ		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    ಪೂರ್ಣಾಙ್ಕ		hash;
    ಪೂರ್ಣಾಙ್ಕ		new_hash;

    ಯದಿ (is_map_locked())
	ಪ್ರತ್ಯಾಗಮ;

    validate_maphash();

    ಪ್ರತಿ (hash = 0; hash < 256; ++hash)
    {
	ಯದಿ (abbr)
	{
	    ಯದಿ (hash > 0)	// there is only one abbrlist
		ಭಙ್ಗ;
	    ಯದಿ (local)
		mpp = &buf->b_first_abbr;
	    ಅನ್ಯಥಾ
		mpp = &first_abbr;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (local)
		mpp = &buf->b_maphash[hash];
	    ಅನ್ಯಥಾ
		mpp = &maphash[hash];
	}
	ಯಾವತ್ (*mpp != ರಿಕ್ತ)
	{
	    mp = *mpp;
	    ಯದಿ (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		ಯದಿ (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    ಅನುವರ್ತ;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_ಕೀಲಿಗಳು[0]);
		ಯದಿ (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    ಯದಿ (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    ಅನ್ಯಥಾ
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    ಅನುವರ್ತ;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

#ಯದಿ defined(FEAT_EVAL)
    ಪೂರ್ಣಾಙ್ಕ
mode_str2flags(char_u *modechars)
{
    ಪೂರ್ಣಾಙ್ಕ		mode = 0;

    ಯದಿ (vim_strchr(modechars, 'n') != ರಿಕ್ತ)
	mode |= MODE_NORMAL;
    ಯದಿ (vim_strchr(modechars, 'v') != ರಿಕ್ತ)
	mode |= MODE_VISUAL | MODE_SELECT;
    ಯದಿ (vim_strchr(modechars, 'x') != ರಿಕ್ತ)
	mode |= MODE_VISUAL;
    ಯದಿ (vim_strchr(modechars, 's') != ರಿಕ್ತ)
	mode |= MODE_SELECT;
    ಯದಿ (vim_strchr(modechars, 'o') != ರಿಕ್ತ)
	mode |= MODE_OP_PENDING;
    ಯದಿ (vim_strchr(modechars, 'i') != ರಿಕ್ತ)
	mode |= MODE_ಸೇರಿಸು;
    ಯದಿ (vim_strchr(modechars, 'l') != ರಿಕ್ತ)
	mode |= MODE_LANGMAP;
    ಯದಿ (vim_strchr(modechars, 'c') != ರಿಕ್ತ)
	mode |= MODE_CMDLINE;

    ಪ್ರತ್ಯಾಗಮ mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    ಪೂರ್ಣಾಙ್ಕ
map_to_exists(char_u *str, char_u *modechars, ಪೂರ್ಣಾಙ್ಕ abbr)
{
    char_u	*rhs;
    char_u	*buf;
    ಪೂರ್ಣಾಙ್ಕ		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, ರಿಕ್ತ);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    ಪ್ರತ್ಯಾಗಮ retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    ಪೂರ್ಣಾಙ್ಕ
map_to_exists_mode(char_u *rhs, ಪೂರ್ಣಾಙ್ಕ mode, ಪೂರ್ಣಾಙ್ಕ abbr)
{
    mapblock_T	*mp;
    ಪೂರ್ಣಾಙ್ಕ		hash;
    ಪೂರ್ಣಾಙ್ಕ		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    ಪ್ರತಿ (;;)
    {
	ಪ್ರತಿ (hash = 0; hash < 256; ++hash)
	{
	    ಯದಿ (abbr)
	    {
		ಯದಿ (hash > 0)		// there is only one abbr list
		    ಭಙ್ಗ;
		ಯದಿ (exp_buffer)
		    mp = curbuf->b_first_abbr;
		ಅನ್ಯಥಾ
		    mp = first_abbr;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    ಅನ್ಯಥಾ
		mp = maphash[hash];
	    ಪ್ರತಿ (; mp; mp = mp->m_next)
	    {
		ಯದಿ ((mp->m_mode & mode)
			&& strstr((ವರ್ಣ *)mp->m_str, (ವರ್ಣ *)rhs) != ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ TRUE;
	    }
	}
	ಯದಿ (exp_buffer)
	    ಭಙ್ಗ;
	exp_buffer = TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	expand_mapmodes = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	expand_isabbrev = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	expand_buffer = FALSE;

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
    ಸ್ಥಿರ char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		modifiers;
    ಪೂರ್ಣಾಙ್ಕ		cpo_bslash;
    ಪೂರ್ಣಾಙ್ಕ		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != ರಿಕ್ತ);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != ರಿಕ್ತ);

    ಪ್ರತಿ (; *str; ++str)
    {
	c = *str;
	ಯದಿ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    ಯದಿ (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    ಯದಿ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		ಯದಿ (cpo_special)
		{
		    ga_clear(&ga);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		c = TO_SPECIAL(str[1], str[2]);
		ಯದಿ (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    ಯದಿ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ಯದಿ (cpo_special)
		{
		    ga_clear(&ga);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ga_concat(&ga, get_special_ಕೀಲಿ_name(c, modifiers));
		ಅನುವರ್ತ; // for (str)
	    }
	}
	ಯದಿ (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	ಯದಿ (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    ಪ್ರತ್ಯಾಗಮ (char_u *)(ga.ga_data);
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
    ಪೂರ್ಣಾಙ್ಕ		forceit,	// TRUE if '!' given
    ಪೂರ್ಣಾಙ್ಕ		isabbrev,	// TRUE if abbreviation
    ಪೂರ್ಣಾಙ್ಕ		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    ಯದಿ (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	ಅನ್ಯಥಾ
	{
	    expand_mapmodes = MODE_ಸೇರಿಸು | MODE_CMDLINE;
	    ಯದಿ (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		ಅನುವರ್ತ;
	    }
#ifdef FEAT_EVAL
	    ಯದಿ (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		ಅನುವರ್ತ;
	    }
#endif
	    ಭಙ್ಗ;
	}
	xp->xp_pattern = arg;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		hash;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		fuzzy;
    ಪೂರ್ಣಾಙ್ಕ		match;
    ಪೂರ್ಣಾಙ್ಕ		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *ಅಂಕೆ_Matches = 0;		    // return values in case of FAIL
    *matches = ರಿಕ್ತ;

    ಯದಿ (!fuzzy)
	ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ *), 3);
    ಅನ್ಯಥಾ
	ga_init2(&ga, ಪರಿಮಾಣ(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    ಪ್ರತಿ (i = 0; i < 7; ++i)
    {
	ಯದಿ (i == 0)
	    p = (char_u *)"<silent>";
	ಅನ್ಯಥಾ ಯದಿ (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	ಅನ್ಯಥಾ ಯದಿ (i == 2)
	    p = (char_u *)"<script>";
	ಅನ್ಯಥಾ ಯದಿ (i == 3)
	    p = (char_u *)"<expr>";
#endif
	ಅನ್ಯಥಾ ಯದಿ (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	ಅನ್ಯಥಾ ಯದಿ (i == 5)
	    p = (char_u *)"<nowait>";
	ಅನ್ಯಥಾ ಯದಿ (i == 6)
	    p = (char_u *)"<special>";
	ಅನ್ಯಥಾ
	    ಅನುವರ್ತ;

	ಯದಿ (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	ಅನ್ಯಥಾ
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	ಯದಿ (!match)
	    ಅನುವರ್ತ;

	ಯದಿ (ga_grow(&ga, 1) == FAIL)
	    ಭಙ್ಗ;

	ಯದಿ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	ಅನ್ಯಥಾ
	    ((char_u **)ga.ga_data)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    ಪ್ರತಿ (hash = 0; hash < 256; ++hash)
    {
	ಯದಿ (expand_isabbrev)
	{
	    ಯದಿ (hash > 0)	// only one abbrev list
		ಭಙ್ಗ; // for (hash)
	    mp = first_abbr;
	}
	ಅನ್ಯಥಾ ಯದಿ (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	ಅನ್ಯಥಾ
	    mp = maphash[hash];
	ಪ್ರತಿ (; mp; mp = mp->m_next)
	{
	    ಯದಿ (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		ಅನುವರ್ತ;

	    p = translate_mapping(mp->m_ಕೀಲಿಗಳು);
	    ಯದಿ (p == ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    ಯದಿ (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    ಅನ್ಯಥಾ
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    ಯದಿ (!match)
	    {
		vim_free(p);
		ಅನುವರ್ತ;
	    }

	    ಯದಿ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		ಭಙ್ಗ;
	    }

	    ಯದಿ (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ಅನ್ಯಥಾ
		((char_u **)ga.ga_data)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    ಯದಿ (ga.ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (!fuzzy)
    {
	*matches = ga.ga_data;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }

    ಎಣಿಕೆ = *ಅಂಕೆ_Matches;
    ಯದಿ (ಎಣಿಕೆ > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	ಯದಿ (!fuzzy)
	    sort_strings(*matches, ಎಣಿಕೆ);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + ಎಣಿಕೆ;

	ಯಾವತ್ (ptr2 < ptr3)
	{
	    ಯದಿ (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    ಅನ್ಯಥಾ
	    {
		vim_free(*ptr2++);
		ಎಣಿಕೆ--;
	    }
	}
    }

    *ಅಂಕೆ_Matches = ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ (ಎಣಿಕೆ == 0 ? FAIL : OK);
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
    ಪೂರ್ಣಾಙ್ಕ
check_abbr(
    ಪೂರ್ಣಾಙ್ಕ		c,
    char_u	*ptr,
    ಪೂರ್ಣಾಙ್ಕ		col,
    ಪೂರ್ಣಾಙ್ಕ		mincol)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		scol;		// starting column of the abbr.
    ಪೂರ್ಣಾಙ್ಕ		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    ಪೂರ್ಣಾಙ್ಕ		clen = 0;	// length in characters
    ಪೂರ್ಣಾಙ್ಕ		is_id = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		vim_abbr;

    ಯದಿ (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    ಯದಿ (noremap_ಕೀಲಿಗಳು() && c != Ctrl_RSB)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    ಯದಿ (col == 0)				// cannot be an abbr.
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	ಯದಿ (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	ಅನ್ಯಥಾ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ಯದಿ (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	ಯಾವತ್ (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    ಯದಿ (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		ಭಙ್ಗ;
	    }
	    ++clen;
	}
	scol = (ಪೂರ್ಣಾಙ್ಕ)(p - ptr);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	ಅನ್ಯಥಾ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ಯದಿ (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	ಪ್ರತಿ (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    ಯದಿ (scol < mincol)
	scol = mincol;
    ಯದಿ (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	ಯದಿ (mp == ರಿಕ್ತ)
	{
	    mp = mp2;
	    mp2 = ರಿಕ್ತ;
	}
	ಪ್ರತಿ ( ; mp; mp->m_next == ರಿಕ್ತ
				  ? (mp = mp2, mp2 = ರಿಕ್ತ) : (mp = mp->m_next))
	{
	    ಪೂರ್ಣಾಙ್ಕ		qlen = mp->m_keylen;
	    char_u	*q = mp->m_ಕೀಲಿಗಳು;
	    ಪೂರ್ಣಾಙ್ಕ		match;

	    ಯದಿ (vim_strbyte(mp->m_ಕೀಲಿಗಳು, K_SPECIAL) != ರಿಕ್ತ)
	    {
		char_u *qe = vim_strsave(mp->m_ಕೀಲಿಗಳು);

		// might have CSI escaped mp->m_keys
		ಯದಿ (qe != ರಿಕ್ತ)
		{
		    q = qe;
		    qlen = (ಪೂರ್ಣಾಙ್ಕ)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    ಯದಿ (q != mp->m_ಕೀಲಿಗಳು)
		vim_free(q);
	    ಯದಿ (match)
		ಭಙ್ಗ;
	}
	ಯದಿ (mp != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ	noremap;
	    ಪೂರ್ಣಾಙ್ಕ silent;
#ifdef FEAT_EVAL
	    ಪೂರ್ಣಾಙ್ಕ ಅಭಿವ್ಯಕ್ತಿ;
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
	    ಯದಿ (c != Ctrl_RSB)
	    {
					// special key code, split up
		ಯದಿ (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    ಯದಿ (has_mbyte)
		    {
			ಪೂರ್ಣಾಙ್ಕ	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			ಯದಿ (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			ಯದಿ (escaped != ರಿಕ್ತ)
			{
			    newlen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    ಅನ್ಯಥಾ
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(ಶೂನ್ಯ)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ifdef FEAT_EVAL
	    ಅಭಿವ್ಯಕ್ತಿ = mp->m_ಅಭಿವ್ಯಕ್ತಿ;

	    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ)
		s = eval_map_ಅಭಿವ್ಯಕ್ತಿ(mp, c);
	    ಅನ್ಯಥಾ
#endif
		s = mp->m_str;
	    ಯದಿ (s != ರಿಕ್ತ)
	    {
					// insert the to string
		(ಶೂನ್ಯ)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    ಯದಿ (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    ಯಾವತ್ (len-- > 0)		// delete the from string
		(ಶೂನ್ಯ)ins_typebuf(tb, 1, 0, TRUE, silent);
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_ಅಭಿವ್ಯಕ್ತಿ(
    mapblock_T	*mp,
    ಪೂರ್ಣಾಙ್ಕ		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ;
    pos_T	save_cursor;
    ಪೂರ್ಣಾಙ್ಕ		save_ಸಂದೇಶ_col;
    ಪೂರ್ಣಾಙ್ಕ		save_ಸಂದೇಶ_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    ಪೂರ್ಣಾಙ್ಕ		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    ಅಭಿವ್ಯಕ್ತಿ = vim_strsave(mp->m_str);
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    vim_unescape_csi(ಅಭಿವ್ಯಕ್ತಿ);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_ಸಂದೇಶ_col = ಸಂದೇಶ_col;
    save_ಸಂದೇಶ_row = ಸಂದೇಶ_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    ಯದಿ (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(ಅಭಿವ್ಯಕ್ತಿ, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    ಸಂದೇಶ_col = save_ಸಂದೇಶ_col;
    ಸಂದೇಶ_row = save_ಸಂದೇಶ_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(ಅಭಿವ್ಯಕ್ತಿ);

    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    ಪ್ರತ್ಯಾಗಮ res;
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
    ಯದಿ (res == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    d = res;
    ಪ್ರತಿ (s = p; *s != NUL; )
    {
	ಯದಿ ((s[0] == K_SPECIAL
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
	ಅನ್ಯಥಾ
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    ಪ್ರತ್ಯಾಗಮ res;
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

    ಯಾವತ್ (*s != NUL)
    {
	ಯದಿ (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	ಅನ್ಯಥಾ ಯದಿ ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (ಪೂರ್ಣಾಙ್ಕ)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	ಅನ್ಯಥಾ
	    *d++ = *s++;
    }
    *d = NUL;
    ಪ್ರತ್ಯಾಗಮ (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    ವರ್ಣ	*cmd;
    ಪೂರ್ಣಾಙ್ಕ		abbr;
    ಪೂರ್ಣಾಙ್ಕ		hash;
    ಪೂರ್ಣಾಙ್ಕ		did_cpo = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    ಪ್ರತಿ (abbr = 0; abbr < 2; ++abbr)
	ಪ್ರತಿ (hash = 0; hash < 256; ++hash)
	{
	    ಯದಿ (abbr)
	    {
		ಯದಿ (hash > 0)		// there is only one abbr list
		    ಭಙ್ಗ;
		ಯದಿ (buf != ರಿಕ್ತ)
		    mp = buf->b_first_abbr;
		ಅನ್ಯಥಾ
		    mp = first_abbr;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (buf != ರಿಕ್ತ)
		    mp = buf->b_maphash[hash];
		ಅನ್ಯಥಾ
		    mp = maphash[hash];
	    }

	    ಪ್ರತಿ ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		ಯದಿ (mp->m_simplified)
		    ಅನುವರ್ತ;

		// skip script-local mappings
		ಯದಿ (mp->m_noremap == REMAP_SCRIPT)
		    ಅನುವರ್ತ;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		ಪ್ರತಿ (p = mp->m_str; *p != NUL; ++p)
		    ಯದಿ (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (ಪೂರ್ಣಾಙ್ಕ)KE_SNR)
			ಭಙ್ಗ;
		ಯದಿ (*p != NUL)
		    ಅನುವರ್ತ;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		ಯದಿ (abbr)
		    cmd = "abbr";
		ಅನ್ಯಥಾ
		    cmd = "map";
		ವಿಕಲ್ಪ (mp->m_mode)
		{
		    ಪಕ್ಷ MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_NORMAL:
			c1 = 'n';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_VISUAL:
			c1 = 'x';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_SELECT:
			c1 = 's';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_OP_PENDING:
			c1 = 'o';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_CMDLINE | MODE_ಸೇರಿಸು:
			ಯದಿ (!abbr)
			    cmd = "map!";
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_CMDLINE:
			c1 = 'c';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_ಸೇರಿಸು:
			c1 = 'i';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_LANGMAP:
			c1 = 'l';
			ಭಙ್ಗ;
		    ಪಕ್ಷ MODE_TERMINAL:
			c1 = 't';
			ಭಙ್ಗ;
		    ಸ್ವಭಾವ:
			iemsg(e_makemap_illegal_mode);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಕುರು	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    ಯದಿ (!did_cpo)
		    {
			ಯದಿ (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			ಅನ್ಯಥಾ
			    ಪ್ರತಿ (i = 0; i < 2; ++i)
				ಪ್ರತಿ (p = (i ? mp->m_str : mp->m_ಕೀಲಿಗಳು); *p; ++p)
				    ಯದಿ (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			ಯದಿ (did_cpo)
			{
			    ಯದಿ (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				ಪ್ರತ್ಯಾಗಮ FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    ಯದಿ (mp->m_ಅಭಿವ್ಯಕ್ತಿ && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && fputs("legacy ", fd) < 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
#endif
		    ಯದಿ (c1 && putc(c1, fd) < 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಯದಿ (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಯದಿ (fputs(cmd, fd) < 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಯದಿ (buf != ರಿಕ್ತ && fputs(" <buffer>", fd) < 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಯದಿ (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಯದಿ (mp->m_silent && fputs(" <silent>", fd) < 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
#ifdef FEAT_EVAL
		    ಯದಿ (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಯದಿ (mp->m_ಅಭಿವ್ಯಕ್ತಿ && fputs(" <expr>", fd) < 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
#endif

		    ಯದಿ (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_ಕೀಲಿಗಳು, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} ಯಾವತ್ (c1 != NUL);
	    }
	}

    ಯದಿ (did_cpo)
	ಯದಿ (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಙ್ಕ
put_escstr(FILE *fd, char_u *strstart, ಪೂರ್ಣಾಙ್ಕ what)
{
    char_u	*str = strstart;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		modifiers;

    // :map xx <Nop>
    ಯದಿ (*str == NUL && what == 1)
    {
	ಯದಿ (fprintf(fd, "<Nop>") < 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಪ್ರತಿ ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ಯಾವತ್ (*p != NUL)
		ಯದಿ (fputc(*p++, fd) < 0)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    --str;
	    ಅನುವರ್ತ;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	ಯದಿ (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    ಯದಿ (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		ಯದಿ (p == ರಿಕ್ತ)
		    c = *str;
		ಅನ್ಯಥಾ
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    ಯದಿ (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    ಯದಿ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ಯದಿ (fputs((ವರ್ಣ *)get_special_ಕೀಲಿ_name(c, modifiers), fd) < 0)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಅನುವರ್ತ;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	ಯದಿ (c == NL)
	{
	    ಯದಿ (what == 2)
	    {
		ಯದಿ (fprintf(fd, "\\\026\n") < 0)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (fprintf(fd, "<NL>") < 0)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನುವರ್ತ;
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
	ಯದಿ (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    ಯದಿ (putc('\\', fd) < 0)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    ಯದಿ (putc(Ctrl_V, fd) < 0)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (putc(c, fd) < 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    ಶೂನ್ಯ
check_map_keycodes(ಶೂನ್ಯ)
{
    mapblock_T	*mp;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	buf[3];
    ಪೂರ್ಣಾಙ್ಕ		abbr;
    ಪೂರ್ಣಾಙ್ಕ		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_ತಳ್ಳು(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    ಪ್ರತಿ (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	ಪ್ರತಿ (abbr = 0; abbr <= 1; ++abbr)
	    ಪ್ರತಿ (hash = 0; hash < 256; ++hash)
	    {
		ಯದಿ (abbr)
		{
		    ಯದಿ (hash)	    // there is only one abbr list
			ಭಙ್ಗ;
		    ಯದಿ (bp != ರಿಕ್ತ)
			mp = bp->b_first_abbr;
		    ಅನ್ಯಥಾ
			mp = first_abbr;
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (bp != ರಿಕ್ತ)
			mp = bp->b_maphash[hash];
		    ಅನ್ಯಥಾ
			mp = maphash[hash];
		}
		ಪ್ರತಿ ( ; mp != ರಿಕ್ತ; mp = mp->m_next)
		{
		    ಪ್ರತಿ (i = 0; i <= 1; ++i)	// do this twice
		    {
			ಯದಿ (i == 0)
			    p = mp->m_ಕೀಲಿಗಳು;	// once for the "from" part
			ಅನ್ಯಥಾ
			    p = mp->m_str;	// and once for the "to" part
			ಯಾವತ್ (*p)
			{
			    ಯದಿ (*p == K_SPECIAL)
			    {
				++p;
				ಯದಿ (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (ಶೂನ್ಯ)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	ಯದಿ (bp == ರಿಕ್ತ)
	    ಭಙ್ಗ;
    }
    ESTACK_CHECK_NOW;
    estack_ತೆಗೆ();
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*ಕೀಲಿಗಳು,
    ಪೂರ್ಣಾಙ್ಕ		mode,
    ಪೂರ್ಣಾಙ್ಕ		exact,		// require exact match
    ಪೂರ್ಣಾಙ್ಕ		ign_mod,	// ignore preceding modifier
    ಪೂರ್ಣಾಙ್ಕ		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    ಪೂರ್ಣಾಙ್ಕ		*local_ptr)	// return: buffer-local mapping or NULL
{
    ಪೂರ್ಣಾಙ್ಕ		hash;
    ಪೂರ್ಣಾಙ್ಕ		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		local;

    validate_maphash();

    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಕೀಲಿಗಳು);
    ಪ್ರತಿ (local = 1; local >= 0; --local)
	// loop over all hash lists
	ಪ್ರತಿ (hash = 0; hash < 256; ++hash)
	{
	    ಯದಿ (abbr)
	    {
		ಯದಿ (hash > 0)		// there is only one list.
		    ಭಙ್ಗ;
		ಯದಿ (local)
		    mp = curbuf->b_first_abbr;
		ಅನ್ಯಥಾ
		    mp = first_abbr;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (local)
		mp = curbuf->b_maphash[hash];
	    ಅನ್ಯಥಾ
		mp = maphash[hash];
	    ಪ್ರತಿ ( ; mp != ರಿಕ್ತ; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		ಯದಿ ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    ಯದಿ (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    ಅನ್ಯಥಾ
			minlen = len;
		    s = mp->m_ಕೀಲಿಗಳು;
		    ಯದಿ (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			ಯದಿ (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    ಯದಿ (STRNCMP(s, ಕೀಲಿಗಳು, minlen) == 0)
		    {
			ಯದಿ (mp_ptr != ರಿಕ್ತ)
			    *mp_ptr = mp;
			ಯದಿ (local_ptr != ರಿಕ್ತ)
			    *local_ptr = local;
			ಪ್ರತ್ಯಾಗಮ mp->m_str;
		    }
		}
	    }
	}

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * "hasmapto()" function
 */
    ಶೂನ್ಯ
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*mode;
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		abbr = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string(&argvars[0]);
    ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    ಅನ್ಯಥಾ
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool(&argvars[2]);
    }

    ಯದಿ (map_to_exists(name, mode, abbr))
	rettv->vval.v_ಸಂಖ್ಯೆ = TRUE;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	ಪೂರ್ಣಾಙ್ಕ	    buffer_local,   // false if not buffer local mapping
	ಪೂರ್ಣಾಙ್ಕ	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_ಕೀಲಿಗಳು, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_ಕೀಲಿಗಳು);
    ಯದಿ (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_ಸಂಖ್ಯೆ(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_ಸಂಖ್ಯೆ(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_ಸಂಖ್ಯೆ(dict, "expr", mp->m_ಅಭಿವ್ಯಕ್ತಿ ? 1L : 0L);
    dict_add_ಸಂಖ್ಯೆ(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_add_ಸಂಖ್ಯೆ(dict, "sid", (long)mp->m_script_ctx.sc_sid);
    dict_add_ಸಂಖ್ಯೆ(dict, "scriptversion",
		    (long)mp->m_script_ctx.sc_version);
    dict_add_ಸಂಖ್ಯೆ(dict, "lnum", (long)mp->m_script_ctx.sc_lnum);
    dict_add_ಸಂಖ್ಯೆ(dict, "buffer", (long)buffer_local);
    dict_add_ಸಂಖ್ಯೆ(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_add_string(dict, "mode", mapmode);
    dict_add_ಸಂಖ್ಯೆ(dict, "abbr", abbr ? 1L : 0L);
    dict_add_ಸಂಖ್ಯೆ(dict, "mode_bits", mp->m_mode);

    vim_free(mapmode);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
get_maparg(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ exact)
{
    char_u	*ಕೀಲಿಗಳು;
    char_u	*ಕೀಲಿಗಳು_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*ಕೀಲಿಗಳು_buf = ರಿಕ್ತ;
    char_u	*alt_ಕೀಲಿಗಳು_buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		did_simplify = FALSE;
    char_u	*rhs;
    ಪೂರ್ಣಾಙ್ಕ		mode;
    ಪೂರ್ಣಾಙ್ಕ		abbr = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		get_dict = FALSE;
    mapblock_T	*mp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		buffer_local;
    ಪೂರ್ಣಾಙ್ಕ		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಕೀಲಿಗಳು = tv_get_string(&argvars[0]);
    ಯದಿ (*ಕೀಲಿಗಳು == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool(&argvars[2]);
	    ಯದಿ (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool(&argvars[3]);
	}
    }
    ಅನ್ಯಥಾ
	which = (char_u *)"";
    ಯದಿ (which == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    mode = get_map_mode(&which, 0);

    ಕೀಲಿಗಳು_simplified = replace_termcodes(ಕೀಲಿಗಳು, &ಕೀಲಿಗಳು_buf, 0, flags,
								&did_simplify);
    rhs = check_map(ಕೀಲಿಗಳು_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    ಯದಿ (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(ಶೂನ್ಯ)replace_termcodes(ಕೀಲಿಗಳು, &alt_ಕೀಲಿಗಳು_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, ರಿಕ್ತ);
	rhs = check_map(alt_ಕೀಲಿಗಳು_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    ಯದಿ (!get_dict)
    {
	// Return a string.
	ಯದಿ (rhs != ರಿಕ್ತ)
	{
	    ಯದಿ (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    ಅನ್ಯಥಾ
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    ಅನ್ಯಥಾ ಯದಿ (rettv_dict_alloc(rettv) == OK && rhs != ರಿಕ್ತ)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? ಕೀಲಿಗಳು_simplified : ರಿಕ್ತ,
			  buffer_local, abbr);

    vim_free(ಕೀಲಿಗಳು_buf);
    vim_free(alt_ಕೀಲಿಗಳು_buf);
}

/*
 * "maplist()" function
 */
    ಶೂನ್ಯ
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    ಪೂರ್ಣಾಙ್ಕ		buffer_local;
    char_u	*ಕೀಲಿಗಳು_buf;
    ಪೂರ್ಣಾಙ್ಕ		did_simplify;
    ಪೂರ್ಣಾಙ್ಕ		hash;
    char_u	*lhs;
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    ಪೂರ್ಣಾಙ್ಕ		abbr = FALSE;

    ಯದಿ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    ಪ್ರತಿ (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	ಪ್ರತಿ (hash = 0; hash < 256; ++hash)
	{
	    ಯದಿ (abbr)
	    {
		ಯದಿ (hash > 0)		// there is only one abbr list
		    ಭಙ್ಗ;
		ಯದಿ (buffer_local)
		    mp = curbuf->b_first_abbr;
		ಅನ್ಯಥಾ
		    mp = first_abbr;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (buffer_local)
		mp = curbuf->b_maphash[hash];
	    ಅನ್ಯಥಾ
		mp = maphash[hash];
	    ಪ್ರತಿ (; mp; mp = mp->m_next)
	    {
		ಯದಿ (mp->m_simplified)
		    ಅನುವರ್ತ;
		ಯದಿ ((d = dict_alloc()) == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ;
		ಯದಿ (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    ಪ್ರತ್ಯಾಗಮ;
		}

		ಕೀಲಿಗಳು_buf = ರಿಕ್ತ;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_ಕೀಲಿಗಳು, TRUE, FALSE);
		(ಶೂನ್ಯ)replace_termcodes(lhs, &ಕೀಲಿಗಳು_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? ಕೀಲಿಗಳು_buf : ರಿಕ್ತ,
				 buffer_local, abbr);
		vim_free(ಕೀಲಿಗಳು_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    ಶೂನ್ಯ
f_maparg(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		ಪ್ರತ್ಯಾಗಮ;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    ಶೂನ್ಯ
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_map_mode_string(char_u *mode_string, ಪೂರ್ಣಾಙ್ಕ abbr)
{
    char_u	*p = mode_string;
    ಪೂರ್ಣಾಙ್ಕ		mode = 0;
    ಪೂರ್ಣಾಙ್ಕ		tmode;
    ಪೂರ್ಣಾಙ್ಕ		modec;
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	MASK_V = MODE_VISUAL | MODE_SELECT;
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	MASK_BANG = MODE_ಸೇರಿಸು | MODE_CMDLINE;

    ಯದಿ (*p == NUL)
	p = (char_u *)" ";	// compatibility
    ಯಾವತ್ ((modec = *p++))
    {
	ವಿಕಲ್ಪ (modec)
	{
	    ಪಕ್ಷ 'i': tmode = MODE_ಸೇರಿಸು;	ಭಙ್ಗ;
	    ಪಕ್ಷ 'l': tmode = MODE_LANGMAP;	ಭಙ್ಗ;
	    ಪಕ್ಷ 'c': tmode = MODE_CMDLINE;	ಭಙ್ಗ;
	    ಪಕ್ಷ 'n': tmode = MODE_NORMAL;	ಭಙ್ಗ;
	    ಪಕ್ಷ 'x': tmode = MODE_VISUAL;	ಭಙ್ಗ;
	    ಪಕ್ಷ 's': tmode = MODE_SELECT;	ಭಙ್ಗ;
	    ಪಕ್ಷ 'o': tmode = MODE_OP_PENDING;	ಭಙ್ಗ;
	    ಪಕ್ಷ 't': tmode = MODE_TERMINAL;	ಭಙ್ಗ;
	    ಪಕ್ಷ 'v': tmode = MASK_V;		ಭಙ್ಗ;
	    ಪಕ್ಷ '!': tmode = MASK_BANG;	ಭಙ್ಗ;
	    ಪಕ್ಷ ' ': tmode = MASK_MAP;		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		      ಪ್ರತ್ಯಾಗಮ 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    ಯದಿ ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತ್ಯಾಗಮ mode;
}

/*
 * "mapset()" function
 */
    ಶೂನ್ಯ
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    ಪೂರ್ಣಾಙ್ಕ		mode;
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		noremap;
    ಪೂರ್ಣಾಙ್ಕ		ಅಭಿವ್ಯಕ್ತಿ;
    ಪೂರ್ಣಾಙ್ಕ		silent;
    ಪೂರ್ಣಾಙ್ಕ		buffer;
    scid_T	sid;
    ಪೂರ್ಣಾಙ್ಕ		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    ಪೂರ್ಣಾಙ್ಕ		nowait;
    char_u	*arg;
    ಪೂರ್ಣಾಙ್ಕ		dict_only;
    mapblock_T	*mp_result[2] = {ರಿಕ್ತ, ರಿಕ್ತ};

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    ಯದಿ (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	ಯದಿ (which == ರಿಕ್ತ || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	ಯದಿ (which == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	is_abbr = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool(&argvars[1]);

	ಯದಿ (check_for_dict_arg(argvars, 2) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    ಯದಿ (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	ಪ್ರತ್ಯಾಗಮ;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    ಯದಿ (lhs == ರಿಕ್ತ || lhsraw == ರಿಕ್ತ || rhs == ರಿಕ್ತ)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	ಪ್ರತ್ಯಾಗಮ;
    }
    orig_rhs = rhs;

    noremap = dict_get_ಸಂಖ್ಯೆ(d, "noremap") ? REMAP_NONE: 0;
    ಯದಿ (dict_get_ಸಂಖ್ಯೆ(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    ಅಭಿವ್ಯಕ್ತಿ = dict_get_ಸಂಖ್ಯೆ(d, "expr") != 0;
    silent = dict_get_ಸಂಖ್ಯೆ(d, "silent") != 0;
    sid = dict_get_ಸಂಖ್ಯೆ(d, "sid");
    scriptversion = dict_get_ಸಂಖ್ಯೆ(d, "scriptversion");
    lnum = dict_get_ಸಂಖ್ಯೆ(d, "lnum");
    buffer = dict_get_ಸಂಖ್ಯೆ(d, "buffer");
    nowait = dict_get_ಸಂಖ್ಯೆ(d, "nowait") != 0;
    // mode from the dict is not used

    ಯದಿ (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    ಅನ್ಯಥಾ
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, ರಿಕ್ತ);

    ಯದಿ (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    ಯದಿ (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	ಯದಿ (arg == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    ಅನ್ಯಥಾ
    {
	arg = vim_strsave(lhs);
	ಯದಿ (arg == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, ಅಭಿವ್ಯಕ್ತಿ, sid,
			    scriptversion, lnum, 0);
    ಯದಿ (lhsrawalt != ರಿಕ್ತ)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, ಅಭಿವ್ಯಕ್ತಿ, sid,
			    scriptversion, lnum, 1);

    ಯದಿ (mp_result[0] != ರಿಕ್ತ && mp_result[1] != ರಿಕ್ತ)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#ಯದಿ defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
ಸಂರಚನಾ initmap
{
    char_u	*arg;
    ಪೂರ್ಣಾಙ್ಕ		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
ಸ್ಥಿರ ಸಂರಚನಾ initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_ಸೇರಿಸು | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
ಸ್ಥಿರ ಸಂರಚನಾ initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_ಸೇರಿಸು | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_ಸೇರಿಸು | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_ಸೇರಿಸು},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  ಅನ್ಯಥಾ
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_ಸೇರಿಸು}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# ಯದಿ defined(MACOS_X)
ಸ್ಥಿರ ಸಂರಚನಾ initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_ಸೇರಿಸು | MODE_CMDLINE},
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
    ಶೂನ್ಯ
init_mappings(ಶೂನ್ಯ)
{
#ಯದಿ defined(MSWIN) || defined(MACOS_X)
    ಪೂರ್ಣಾಙ್ಕ		i;

# ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ಯದಿ (!gui.starting)
#  endif
    {
	ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# ಯದಿ defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(initmappings); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode, FALSE);
# endif
#endif
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    ಶೂನ್ಯ
add_map(char_u *map, ಪೂರ್ಣಾಙ್ಕ mode, ಪೂರ್ಣಾಙ್ಕ nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    ಯದಿ (s != ರಿಕ್ತ)
    {
	(ಶೂನ್ಯ)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#ಯದಿ defined(FEAT_LANGMAP)
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ	    from;
    ಪೂರ್ಣಾಙ್ಕ     to;
} langmap_entry_T;

ಸ್ಥಿರ garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
langmap_set_entry(ಪೂರ್ಣಾಙ್ಕ from, ಪೂರ್ಣಾಙ್ಕ to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    ಪೂರ್ಣಾಙ್ಕ		    a = 0;
    ಪೂರ್ಣಾಙ್ಕ		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    ಯಾವತ್ (a != b)
    {
	ಪೂರ್ಣಾಙ್ಕ i = (a + b) / 2;
	ಪೂರ್ಣಾಙ್ಕ d = entries[i].from - from;

	ಯದಿ (d == 0)
	{
	    entries[i].to = to;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (d < 0)
	    a = i + 1;
	ಅನ್ಯಥಾ
	    b = i;
    }

    ಯದಿ (ga_grow(&langmap_mapga, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * ಪರಿಮಾಣ(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    ಪೂರ್ಣಾಙ್ಕ
langmap_adjust_mb(ಪೂರ್ಣಾಙ್ಕ c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    ಪೂರ್ಣಾಙ್ಕ a = 0;
    ಪೂರ್ಣಾಙ್ಕ b = langmap_mapga.ga_len;

    ಯಾವತ್ (a != b)
    {
	ಪೂರ್ಣಾಙ್ಕ i = (a + b) / 2;
	ಪೂರ್ಣಾಙ್ಕ d = entries[i].from - c;

	ಯದಿ (d == 0)
	    ಪ್ರತ್ಯಾಗಮ entries[i].to;  // found matching entry
	ಯದಿ (d < 0)
	    a = i + 1;
	ಅನ್ಯಥಾ
	    b = i;
    }
    ಪ್ರತ್ಯಾಗಮ c;  // no entry found, return "c" unmodified
}

    ಶೂನ್ಯ
langmap_init(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, ಪರಿಮಾಣ(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    ವರ್ಣ *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    ಪೂರ್ಣಾಙ್ಕ	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    ಪ್ರತಿ (p = p_langmap; p[0] != NUL; )
    {
	ಪ್ರತಿ (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    ಯದಿ (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	ಯದಿ (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	ಅನ್ಯಥಾ
	    p2 = ರಿಕ್ತ;	    // aAbBcCdD form, p2 is NULL
	ಯಾವತ್ (p[0])
	{
	    ಯದಿ (p[0] == ',')
	    {
		++p;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    ಯದಿ (p2 == ರಿಕ್ತ)
	    {
		MB_PTR_ADV(p);
		ಯದಿ (p[0] != ',')
		{
		    ಯದಿ (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (p2[0] != ',')
		{
		    ಯದಿ (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    ಯದಿ (to == NUL)
	    {
		sprintf(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		ಪ್ರತ್ಯಾಗಮ args->os_errbuf;
	    }

	    ಯದಿ (from >= 256)
		langmap_set_entry(from, to);
	    ಅನ್ಯಥಾ
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    ಯದಿ (p2 != ರಿಕ್ತ)
	    {
		MB_PTR_ADV(p2);
		ಯದಿ (*p == ';')
		{
		    p = p2;
		    ಯದಿ (p[0] != NUL)
		    {
			ಯದಿ (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    ಪ್ರತ್ಯಾಗಮ args->os_errbuf;
			}
			++p;
		    }
		    ಭಙ್ಗ;
		}
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
do_exmap(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ isabbrev)
{
    ಪೂರ್ಣಾಙ್ಕ	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    ವಿಕಲ್ಪ (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	ಪಕ್ಷ 1: emsg(_(e_invalid_argument));
		ಭಙ್ಗ;
	ಪಕ್ಷ 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		ಭಙ್ಗ;
    }
}

/*
 * ":abbreviate" and friends.
 */
    ಶೂನ್ಯ
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    ಶೂನ್ಯ
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    ಯದಿ (secure)
    {
	secure = 2;
	ಸಂದೇಶ_outtrans(eap->cmd);
	ಸಂದೇಶ_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    ಶೂನ್ಯ
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    ಶೂನ್ಯ
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    ಶೂನ್ಯ
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
