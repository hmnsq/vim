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
ಸ್ಥಿರ mapblock_T	*ಮೊದಲ_abbr = ಖಾಲಿ; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
ಸ್ಥಿರ mapblock_T	*(maphash[256]);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		map_locked = 0;

/*
 * Make a hash value for a mapping.
 * "mode" is the lower 4 bits of the State for the mapping.
 * "c1" is the first character of the "lhs".
 * Returns a value between 0 and 255, index in maphash.
 * Put Normal/Visual mode mappings mostly separately from Insert/Cmdline mode.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MAP_HASH(ಬಹುಲಕ, c1) (((ಬಹುಲಕ) & (ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT | ಬಹುಲಕ_OP_PENDING | ಬಹುಲಕ_TERMINAL)) ? (c1) : ((c1) ^ 0x80))

/*
 * Get the start of the hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_maphash_list(ಪೂರ್ಣಾಂಕ state, ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(ಪೂರ್ಣಾಂಕ state, ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ curbuf->b_maphash[MAP_HASH(state, c)];
}

    ಪೂರ್ಣಾಂಕ
is_maphash_valid(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
validate_maphash(ಶೂನ್ಯ)
{
    ಆದರೆ (maphash_valid)
	ಹಿಂತಿರುಗಿ;

    CLEAR_ಕ್ಷೇತ್ರ(maphash);
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
    ಆದರೆ (mp->m_alt != ಖಾಲಿ)
	mp->m_alt->m_alt = ಖಾಲಿ;
    vim_free(mp->m_str);
    vim_free(mp->m_orig_str);
    *mpp = mp->m_ಮುಂದಿನ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    reset_ಕೊನೆಯ_ಬಳಸಿದ_map(mp);
#ಆದರೆ_ಅಂತ್ಯ
    vim_free(mp);
}

/*
 * Return characters to represent the map mode in an allocated string.
 * Returns NULL when out of memory.
 */
    ಸ್ಥಿರ char_u *
map_ಬಹುಲಕ_ಗಮ್ಯ_chars(ಪೂರ್ಣಾಂಕ ಬಹುಲಕ)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    ಆದರೆ ((ಬಹುಲಕ & (ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE)) == (ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ & ಬಹುಲಕ_ಸೇರಿಸು)
	ga_append(&mapmode, 'i');			// :imap
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ & ಬಹುಲಕ_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ & ಬಹುಲಕ_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಬಹುಲಕ
		 & (ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT | ಬಹುಲಕ_OP_PENDING))
		== (ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT | ಬಹುಲಕ_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಬಹುಲಕ & ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
	    ga_append(&mapmode, 'n');			// :nmap
	ಆದರೆ (ಬಹುಲಕ & ಬಹುಲಕ_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	ಆದರೆ (ಬಹುಲಕ & ಬಹುಲಕ_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	ಆದರೆ ((ಬಹುಲಕ & (ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT)) == (ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ಬಹುಲಕ & ಬಹುಲಕ_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    ಆದರೆ (ಬಹುಲಕ & ಬಹುಲಕ_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    ಹಿಂತಿರುಗಿ (char_u *)mapmode.ga_ದತ್ತಾಂಶ;
}

/*
 * Output a line for one mapping.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
showmap(
    mapblock_T	*mp,
    ಪೂರ್ಣಾಂಕ		local)	    // TRUE for buffer-local map
{
    ಪೂರ್ಣಾಂಕ		len = 1;
    char_u	*mapchars;

    ಆದರೆ (ಸಂದೇಶ_filtered(mp->m_ಕೀಲಿಗಳು) && ಸಂದೇಶ_filtered(mp->m_str))
	ಹಿಂತಿರುಗಿ;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    ಆದರೆ (ಸಂದೇಶ_didout || ಸಂದೇಶ_silent != 0)
    {
	ಸಂದೇಶ_putchar('\n');
	ಆದರೆ (got_int)	    // 'q' typed at MORE prompt
	    ಹೋಗು theend;
    }

    mapchars = map_ಬಹುಲಕ_ಗಮ್ಯ_chars(mp->m_ಬಹುಲಕ);
    ಆದರೆ (mapchars != ಖಾಲಿ)
    {
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)mapchars);
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(mapchars);
	vim_free(mapchars);
    }

    ಆಗ (++len <= 3)
	ಸಂದೇಶ_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = ಸಂದೇಶ_outtrans_special(mp->m_ಕೀಲಿಗಳು, TRUE, 0);
    ಮಾಡು
    {
	ಸಂದೇಶ_putchar(' ');		// pad with blanks
	++len;
    } ಆಗ (len < 12);

    ಆದರೆ (mp->m_noremap == REMAP_NONE)
	ಸಂದೇಶ_puts_attr("*", HL_ATTR(HLF_8));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mp->m_noremap == REMAP_SCRIPT)
	ಸಂದೇಶ_puts_attr("&", HL_ATTR(HLF_8));
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_putchar(' ');

    ಆದರೆ (local)
	ಸಂದೇಶ_putchar('@');
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    ಆದರೆ (*mp->m_str == NUL)
	ಸಂದೇಶ_puts_attr("<Nop>", HL_ATTR(HLF_8));
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_outtrans_special(mp->m_str, FALSE, 0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (p_verbose > 0)
	ಕೊನೆಯ_ಗಣ_ಸಂದೇಶ(mp->m_script_ctx);
#ಆದರೆ_ಅಂತ್ಯ
    ಸಂದೇಶ_clr_eos();
    ಹೊರಹೋಗು_flush();			// show one line at a time

theend:
    --map_locked;
}

    ಸ್ಥಿರ mapblock_T *
map_ಕೂಡಿಸು(
	mapblock_T  **map_ಕೋಷ್ಟಕ,
	mapblock_T  **abbr_ಕೋಷ್ಟಕ,
	char_u	    *ಕೀಲಿಗಳು,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	ಪೂರ್ಣಾಂಕ	    noremap,
	ಪೂರ್ಣಾಂಕ	    nowait,
	ಪೂರ್ಣಾಂಕ	    silent,
	ಪೂರ್ಣಾಂಕ	    ಬಹುಲಕ,
	ಪೂರ್ಣಾಂಕ	    is_abbr,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಪೂರ್ಣಾಂಕ	    ಅಭಿವ್ಯಕ್ತಿ,
	scid_T	    sid,	    // 0 to use current_sctx
	ಪೂರ್ಣಾಂಕ	    scriptversion,
	linenr_T    lnum,
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ಣಾಂಕ	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    ಆದರೆ (mp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    ಆದರೆ (*ಕೀಲಿಗಳು == Ctrl_C)
    {
	ಆದರೆ (map_ಕೋಷ್ಟಕ == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= ಬಹುಲಕ;
	ಇಲ್ಲದಿದ್ದರೆ
	    mapped_ctrl_c |= ಬಹುಲಕ;
    }

    mp->m_ಕೀಲಿಗಳು = vim_strsave(ಕೀಲಿಗಳು);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    ಆದರೆ (mp->m_ಕೀಲಿಗಳು == ಖಾಲಿ || mp->m_str == ಖಾಲಿ)
    {
	vim_free(mp->m_ಕೀಲಿಗಳು);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    mp->m_keylen = (ಪೂರ್ಣಾಂಕ)STRLEN(mp->m_ಕೀಲಿಗಳು);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_ಬಹುಲಕ = ಬಹುಲಕ;
    mp->m_simplified = simplified;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    mp->m_ಅಭಿವ್ಯಕ್ತಿ = ಅಭಿವ್ಯಕ್ತಿ;
    ಆದರೆ (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_ಆವೃತ್ತಿ = scriptversion;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	mp->m_script_ctx = current_sctx;
	ಆದರೆ (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_ಆವೃತ್ತಿ = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // add the new entry in front of the abbrlist or maphash[] list
    ಆದರೆ (is_abbr)
    {
	mp->m_ಮುಂದಿನ = *abbr_ಕೋಷ್ಟಕ;
	*abbr_ಕೋಷ್ಟಕ = mp;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ n = MAP_HASH(mp->m_ಬಹುಲಕ, mp->m_ಕೀಲಿಗಳು[0]);

	mp->m_ಮುಂದಿನ = map_ಕೋಷ್ಟಕ[n];
	map_ಕೋಷ್ಟಕ[n] = mp;
    }
    ಹಿಂತಿರುಗಿ mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_mappings(
	ಪೂರ್ಣಾಂಕ	keyround,
	ಪೂರ್ಣಾಂಕ	abbrev,
	ಪೂರ್ಣಾಂಕ	haskey,
	char_u	*ಕೀಲಿಗಳು,
	ಪೂರ್ಣಾಂಕ	ಕೀಲಿಗಳು_len,
	ಪೂರ್ಣಾಂಕ	ಬಹುಲಕ,
	ಪೂರ್ಣಾಂಕ	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    ಆದರೆ (p_verbose > 0 && keyround == 1)
    {
	ಆದರೆ (ನೋಡಿದ_Modify_Other_ಕೀಲಿಗಳು)
	    ಸಂದೇಶ_puts(_("Seen modifyOtherKeys: true\n"));

	ಆದರೆ (modify_otherkeys_state != MOKS_INITIAL)
	{
	    ಅಕ್ಷರ *name = _("Unknown");
	    ಆಯ್ಕೆ (modify_otherkeys_state)
	    {
		ಸ್ಥಿತಿ MOKS_INITIAL: ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ MOKS_OFF: name = _("Off"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ MOKS_ENABLED: name = _("On"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ MOKS_DISABLED: name = _("Disabled"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ MOKS_AFTER_T_TE: name = _("Cleared"); ನಿಲ್ಲಿಸು;
	    }

	    ಅಕ್ಷರ buf[200];
	    vim_snprintf(buf, ಗಾತ್ರ(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    ಸಂದೇಶ_puts(buf);
	}

	ಆದರೆ (kitty_protocol_state != KKPS_INITIAL)
	{
	    ಅಕ್ಷರ *name = _("Unknown");
	    ಆಯ್ಕೆ (kitty_protocol_state)
	    {
		ಸ್ಥಿತಿ KKPS_INITIAL: ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ KKPS_OFF: name = _("Off"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ KKPS_ENABLED: name = _("On"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ KKPS_DISABLED: name = _("Disabled"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ KKPS_AFTER_T_TE: name = _("Cleared"); ನಿಲ್ಲಿಸು;
	    }

	    ಅಕ್ಷರ buf[200];
	    vim_snprintf(buf, ಗಾತ್ರ(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    ಸಂದೇಶ_puts(buf);
	}
    }

    // need to loop over all global hash lists
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	ಆದರೆ (abbrev)
	{
	    ಆದರೆ (hash != 0)	// there is only one abbreviation list
		ನಿಲ್ಲಿಸು;
	    mp = curbuf->b_ಮೊದಲ_abbr;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    mp = curbuf->b_maphash[hash];
	ಗಾಗಿ ( ; mp != ಖಾಲಿ && !got_int; mp = mp->m_ಮುಂದಿನ)
	{
	    // check entries with the same mode
	    ಆದರೆ (!mp->m_simplified && (mp->m_ಬಹುಲಕ & ಬಹುಲಕ) != 0)
	    {
		ಆದರೆ (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪೂರ್ಣಾಂಕ n = mp->m_keylen;
		    ಆದರೆ (STRNCMP(mp->m_ಕೀಲಿಗಳು, ಕೀಲಿಗಳು,
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
    ಪೂರ್ಣಾಂಕ
do_map(
    ಪೂರ್ಣಾಂಕ		maptype,
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ,
    ಪೂರ್ಣಾಂಕ		abbrev)		// not a mapping but an abbreviation
{
    char_u	*ಕೀಲಿಗಳು;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_ಫಲಿತಾಂಶ[2] = {ಖಾಲಿ, ಖಾಲಿ};
    char_u	*rhs;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		len = 0;	// init for GCC
    ಪೂರ್ಣಾಂಕ		hasarg;
    ಪೂರ್ಣಾಂಕ		haskey;
    ಪೂರ್ಣಾಂಕ		do_print;
    ಪೂರ್ಣಾಂಕ		keyround;
    char_u	*ಕೀಲಿಗಳು_buf = ಖಾಲಿ;
    char_u	*alt_ಕೀಲಿಗಳು_buf = ಖಾಲಿ;
    char_u	*arg_buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		retval = 0;
    ಪೂರ್ಣಾಂಕ		do_backslash;
    mapblock_T	**abbr_ಕೋಷ್ಟಕ;
    mapblock_T	**map_ಕೋಷ್ಟಕ;
    ಪೂರ್ಣಾಂಕ		unique = FALSE;
    ಪೂರ್ಣಾಂಕ		nowait = FALSE;
    ಪೂರ್ಣಾಂಕ		silent = FALSE;
    ಪೂರ್ಣಾಂಕ		special = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		ಅಭಿವ್ಯಕ್ತಿ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		did_simplify = FALSE;
    ಪೂರ್ಣಾಂಕ		unmap_lhs_only = FALSE;
    ಪೂರ್ಣಾಂಕ		noremap;
    char_u      *orig_rhs;

    ಕೀಲಿಗಳು = arg;
    map_ಕೋಷ್ಟಕ = maphash;
    abbr_ಕೋಷ್ಟಕ = &ಮೊದಲ_abbr;

    ಆದರೆ (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    ಆದರೆ (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    ಇಲ್ಲದಿದ್ದರೆ
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    ಗಾಗಿ (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	ಆದರೆ (STRNCMP(ಕೀಲಿಗಳು, "<buffer>", 8) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 8);
	    map_ಕೋಷ್ಟಕ = curbuf->b_maphash;
	    abbr_ಕೋಷ್ಟಕ = &curbuf->b_ಮೊದಲ_abbr;
	    ಮುಂದುವರಿ;
	}

	// Check for "<nowait>": don't wait for more characters.
	ಆದರೆ (STRNCMP(ಕೀಲಿಗಳು, "<nowait>", 8) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 8);
	    nowait = TRUE;
	    ಮುಂದುವರಿ;
	}

	// Check for "<silent>": don't echo commands.
	ಆದರೆ (STRNCMP(ಕೀಲಿಗಳು, "<silent>", 8) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 8);
	    silent = TRUE;
	    ಮುಂದುವರಿ;
	}

	// Check for "<special>": accept special keys in <>
	ಆದರೆ (STRNCMP(ಕೀಲಿಗಳು, "<special>", 9) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 9);
	    special = TRUE;
	    ಮುಂದುವರಿ;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	ಆದರೆ (STRNCMP(ಕೀಲಿಗಳು, "<script>", 8) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 8);
	    noremap = REMAP_SCRIPT;
	    ಮುಂದುವರಿ;
	}

	// Check for "<expr>": {rhs} is an expression.
	ಆದರೆ (STRNCMP(ಕೀಲಿಗಳು, "<expr>", 6) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 6);
	    ಅಭಿವ್ಯಕ್ತಿ = TRUE;
	    ಮುಂದುವರಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	// Check for "<unique>": don't overwrite an existing mapping.
	ಆದರೆ (STRNCMP(ಕೀಲಿಗಳು, "<unique>", 8) == 0)
	{
	    ಕೀಲಿಗಳು = skipwhite(ಕೀಲಿಗಳು + 8);
	    unique = TRUE;
	    ಮುಂದುವರಿ;
	}
	ನಿಲ್ಲಿಸು;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = ಕೀಲಿಗಳು;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ಖಾಲಿ);
    ಆಗ (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	ಆದರೆ ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    ಆದರೆ (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*ಕೀಲಿಗಳು != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    ಆದರೆ (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	ಹೋಗು theend;
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
    ಆದರೆ (haskey)
    {
	char_u	*new_ಕೀಲಿಗಳು;
	ಪೂರ್ಣಾಂಕ	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	ಆದರೆ (special)
	    flags |= REPTERM_SPECIAL;
	new_ಕೀಲಿಗಳು = replace_termcodes(ಕೀಲಿಗಳು, &ಕೀಲಿಗಳು_buf, 0, flags, &did_simplify);
	ಆದರೆ (did_simplify)
	    (ಶೂನ್ಯ)replace_termcodes(ಕೀಲಿಗಳು, &alt_ಕೀಲಿಗಳು_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, ಖಾಲಿ);
	ಕೀಲಿಗಳು = new_ಕೀಲಿಗಳು;
    }
    orig_rhs = rhs;
    ಆದರೆ (hasarg)
    {
	ಆದರೆ (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	ಇಲ್ಲದಿದ್ದರೆ
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ಖಾಲಿ);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    ಗಾಗಿ (keyround = 1; keyround <= 2; ++keyround)
    {
	ಪೂರ್ಣಾಂಕ	did_it = FALSE;
	ಪೂರ್ಣಾಂಕ	did_local = FALSE;
	ಪೂರ್ಣಾಂಕ	keyround1_simplified = keyround == 1 && did_simplify;
	ಪೂರ್ಣಾಂಕ	ಸುತ್ತು;
	ಪೂರ್ಣಾಂಕ	ಅಂಕೆ_ಸುತ್ತುಗಳು;

	ಆದರೆ (keyround == 2)
	{
	    ಆದರೆ (alt_ಕೀಲಿಗಳು_buf == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    ಕೀಲಿಗಳು = alt_ಕೀಲಿಗಳು_buf;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (alt_ಕೀಲಿಗಳು_buf != ಖಾಲಿ && do_print)
	    // when printing always use the not-simplified map
	    ಕೀಲಿಗಳು = alt_ಕೀಲಿಗಳು_buf;

	// check arguments and translate function keys
	ಆದರೆ (haskey)
	{
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಕೀಲಿಗಳು);
	    ಆದರೆ (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		ಹೋಗು theend;
	    }

	    ಆದರೆ (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		ಆದರೆ (has_mbyte)
		{
		    ಪೂರ್ಣಾಂಕ		ಮೊದಲ, ಕೊನೆಯ;
		    ಪೂರ್ಣಾಂಕ		same = -1;
		    char_u	ಕೀಲಿಗಳು_unescaped[MAXMAPLEN + 1];
		    size_t	ಕೀಲಿಗಳು_unescaped_len;

		    mch_memmove(ಕೀಲಿಗಳು_unescaped, ಕೀಲಿಗಳು, (size_t)(len + 1));
		    ಕೀಲಿಗಳು_unescaped_len = vim_unescape_csi(ಕೀಲಿಗಳು_unescaped);
		    p = ಕೀಲಿಗಳು_unescaped;

		    ಮೊದಲ = vim_iswordp(p);
		    ಕೊನೆಯ = ಮೊದಲ;
		    MB_PTR_ADV(p);
		    n = 1;
		    ಆಗ (p < ಕೀಲಿಗಳು_unescaped + ಕೀಲಿಗಳು_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			ಕೊನೆಯ = vim_iswordp(p);	// type of last char
			ಆದರೆ (same == -1 && ಕೊನೆಯ != ಮೊದಲ)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    ಆದರೆ (ಕೊನೆಯ && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			ಹೋಗು theend;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_iswordc(ಕೀಲಿಗಳು[len - 1]))
		    // ends in keyword char
		    ಗಾಗಿ (n = 0; n < len - 2; ++n)
			ಆದರೆ (vim_iswordc(ಕೀಲಿಗಳು[n]) != vim_iswordc(ಕೀಲಿಗಳು[len - 2]))
			{
			    retval = 1;
			    ಹೋಗು theend;
			}
		// An abbreviation cannot contain white space.
		ಗಾಗಿ (n = 0; n < len; ++n)
		    ಆದರೆ (VIM_ISWHITE(ಕೀಲಿಗಳು[n]))
		    {
			retval = 1;
			ಹೋಗು theend;
		    }
	    }
	}

	ಆದರೆ (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	ಆದರೆ (do_print)
	    ಸಂದೇಶ_ಆರಂಭ();

	// Check if a new local mapping wasn't already defined globally.
	ಆದರೆ (unique && map_ಕೋಷ್ಟಕ == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ಆದರೆ (abbrev)
		{
		    ಆದರೆ (hash != 0)	// there is only one abbreviation list
			ನಿಲ್ಲಿಸು;
		    mp = ಮೊದಲ_abbr;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    mp = maphash[hash];
		ಗಾಗಿ ( ; mp != ಖಾಲಿ && !got_int; mp = mp->m_ಮುಂದಿನ)
		{
		    // check entries with the same mode
		    ಆದರೆ ((mp->m_ಬಹುಲಕ & ಬಹುಲಕ) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_ಕೀಲಿಗಳು, ಕೀಲಿಗಳು, (size_t)len) == 0)
		    {
			ಆದರೆ (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_ಕೀಲಿಗಳು);
			ಇಲ್ಲದಿದ್ದರೆ
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_ಕೀಲಿಗಳು);
			retval = 5;
			ಹೋಗು theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	ಆದರೆ (map_ಕೋಷ್ಟಕ != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, ಕೀಲಿಗಳು, len,
							     ಬಹುಲಕ, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	ಅಂಕೆ_ಸುತ್ತುಗಳು = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	ಗಾಗಿ (ಸುತ್ತು = 0; ಸುತ್ತು < ಅಂಕೆ_ಸುತ್ತುಗಳು && !did_it && !got_int; ++ಸುತ್ತು)
	{
	    // need to loop over all hash lists
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ಆದರೆ (abbrev)
		{
		    ಆದರೆ (hash > 0)	// there is only one abbreviation list
			ನಿಲ್ಲಿಸು;
		    mpp = abbr_ಕೋಷ್ಟಕ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    mpp = &(map_ಕೋಷ್ಟಕ[hash]);
		ಗಾಗಿ (mp = *mpp; mp != ಖಾಲಿ && !got_int; mp = *mpp)
		{

		    ಆದರೆ ((mp->m_ಬಹುಲಕ & ಬಹುಲಕ) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_ಮುಂದಿನ);
			ಮುಂದುವರಿ;
		    }
		    ಆದರೆ (!haskey)	// show all entries
		    {
			ಆದರೆ (!mp->m_simplified)
			{
			    showmap(mp, map_ಕೋಷ್ಟಕ != maphash);
			    did_it = TRUE;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ	// do we have a match?
		    {
			ಆದರೆ (ಸುತ್ತು)	// second round: Try unmap "rhs" string
			{
			    n = (ಪೂರ್ಣಾಂಕ)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    n = mp->m_keylen;
			    p = mp->m_ಕೀಲಿಗಳು;
			}
			ಆದರೆ (STRNCMP(p, ಕೀಲಿಗಳು, (size_t)(n < len ? n : len)) == 0)
			{
			    ಆದರೆ (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				ಆದರೆ (n != len && (!abbrev || ಸುತ್ತು || n > len
					       || *skipwhite(ಕೀಲಿಗಳು + n) != NUL))
				{
				    mpp = &(mp->m_ಮುಂದಿನ);
				    ಮುಂದುವರಿ;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				ಆದರೆ (keyround1_simplified && !mp->m_simplified)
				    ನಿಲ್ಲಿಸು;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_ಬಹುಲಕ &= ~ಬಹುಲಕ;
				did_it = TRUE;	// remember we did something
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!hasarg)	// show matching entry
			    {
				ಆದರೆ (!mp->m_simplified)
				{
				    showmap(mp, map_ಕೋಷ್ಟಕ != maphash);
				    did_it = TRUE;
				}
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_ಮುಂದಿನ);
				ಮುಂದುವರಿ;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (unique)
			    {
				ಆದರೆ (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				ಇಲ್ಲದಿದ್ದರೆ
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				ಹೋಗು theend;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				// new rhs for existing entry
				mp->m_ಬಹುಲಕ &= ~ಬಹುಲಕ;	// remove mode bits
				ಆದರೆ (mp->m_ಬಹುಲಕ == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    ಆದರೆ (newstr == ಖಾಲಿ)
				    {
					retval = 4;		// no mem
					ಹೋಗು theend;
				    }
				    ಆದರೆ (mp->m_alt != ಖಾಲಿ)
					mp->m_alt = mp->m_alt->m_alt = ಖಾಲಿ;
				    vim_free(mp->m_str);
				    mp->m_str = newstr;
				    vim_free(mp->m_orig_str);
				    mp->m_orig_str = vim_strsave(orig_rhs);
				    mp->m_noremap = noremap;
				    mp->m_nowait = nowait;
				    mp->m_silent = silent;
				    mp->m_ಬಹುಲಕ = ಬಹುಲಕ;
				    mp->m_simplified = keyround1_simplified;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
				    mp->m_ಅಭಿವ್ಯಕ್ತಿ = ಅಭಿವ್ಯಕ್ತಿ;
				    mp->m_script_ctx = current_sctx;
				    ಆದರೆ (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_ಆವೃತ್ತಿ = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#ಆದರೆ_ಅಂತ್ಯ
				    mp_ಫಲಿತಾಂಶ[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    ಆದರೆ (mp->m_ಬಹುಲಕ == 0)  // entry can be deleted
			    {
				map_free(mpp);
				ಮುಂದುವರಿ;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    ಪೂರ್ಣಾಂಕ new_hash = MAP_HASH(mp->m_ಬಹುಲಕ, mp->m_ಕೀಲಿಗಳು[0]);
			    ಆದರೆ (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_ಮುಂದಿನ;
				mp->m_ಮುಂದಿನ = map_ಕೋಷ್ಟಕ[new_hash];
				map_ಕೋಷ್ಟಕ[new_hash] = mp;

				ಮುಂದುವರಿ;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_ಮುಂದಿನ);
		}
	    }
	}

	ಆದರೆ (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    ಆದರೆ (!did_it)
	    {
		ಆದರೆ (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಕೀಲಿಗಳು == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		ಆದರೆ (map_ಕೋಷ್ಟಕ == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~ಬಹುಲಕ;
		ಇಲ್ಲದಿದ್ದರೆ
		    mapped_ctrl_c &= ~ಬಹುಲಕ;
	    }
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (!haskey || !hasarg)
	{
	    // print entries
	    ಆದರೆ (!did_it && !did_local)
	    {
		ಆದರೆ (abbrev)
		    ಸಂದೇಶ(_("No abbreviation found"));
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ(_("No mapping found"));
	    }
	    ಹೋಗು theend;    // listing finished
	}

	ಆದರೆ (did_it)
	    ಮುಂದುವರಿ;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_ಫಲಿತಾಂಶ[keyround - 1] = map_ಕೂಡಿಸು(map_ಕೋಷ್ಟಕ, abbr_ಕೋಷ್ಟಕ, ಕೀಲಿಗಳು,
		    rhs, orig_rhs, noremap, nowait, silent, ಬಹುಲಕ, abbrev,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    ಅಭಿವ್ಯಕ್ತಿ, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#ಆದರೆ_ಅಂತ್ಯ
		    keyround1_simplified);
	ಆದರೆ (mp_ಫಲಿತಾಂಶ[keyround - 1] == ಖಾಲಿ)
	{
	    retval = 4;	    // no mem
	    ಹೋಗು theend;
	}
    }

    ಆದರೆ (mp_ಫಲಿತಾಂಶ[0] != ಖಾಲಿ && mp_ಫಲಿತಾಂಶ[1] != ಖಾಲಿ)
    {
	mp_ಫಲಿತಾಂಶ[0]->m_alt = mp_ಫಲಿತಾಂಶ[1];
	mp_ಫಲಿತಾಂಶ[1]->m_alt = mp_ಫಲಿತಾಂಶ[0];
    }

theend:
    vim_free(ಕೀಲಿಗಳು_buf);
    vim_free(alt_ಕೀಲಿಗಳು_buf);
    vim_free(arg_buf);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Get the mapping mode from the command name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_map_ಬಹುಲಕ(char_u **cmdp, ಪೂರ್ಣಾಂಕ forceit)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		modec;
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ;

    p = *cmdp;
    modec = *p++;
    ಆದರೆ (modec == 'i')
	ಬಹುಲಕ = ಬಹುಲಕ_ಸೇರಿಸು;				// :imap
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (modec == 'l')
	ಬಹುಲಕ = ಬಹುಲಕ_LANGMAP;				// :lmap
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (modec == 'c')
	ಬಹುಲಕ = ಬಹುಲಕ_CMDLINE;				// :cmap
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (modec == 'n' && *p != 'o')		    // avoid :noremap
	ಬಹುಲಕ = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;				// :nmap
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (modec == 'v')
	ಬಹುಲಕ = ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT;		// :vmap
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (modec == 'x')
	ಬಹುಲಕ = ಬಹುಲಕ_VISUAL;				// :xmap
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (modec == 's')
	ಬಹುಲಕ = ಬಹುಲಕ_SELECT;			// :smap
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (modec == 'o')
	ಬಹುಲಕ = ಬಹುಲಕ_OP_PENDING;			// :omap
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (modec == 't')
	ಬಹುಲಕ = ಬಹುಲಕ_TERMINAL;			// :tmap
    ಇಲ್ಲದಿದ್ದರೆ
    {
	--p;
	ಆದರೆ (forceit)
	    ಬಹುಲಕ = ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE;		// :map !
	ಇಲ್ಲದಿದ್ದರೆ
	    ಬಹುಲಕ = ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT | ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_OP_PENDING;
							// :map
    }

    *cmdp = p;
    ಹಿಂತಿರುಗಿ ಬಹುಲಕ;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		forceit,
    ಪೂರ್ಣಾಂಕ		abbr)
{
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ;
    ಪೂರ್ಣಾಂಕ		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    ಆದರೆ (!local && *arg != NUL)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ;
    }

    ಬಹುಲಕ = get_map_ಬಹುಲಕ(&cmdp, forceit);
    map_clear_ಬಹುಲಕ(curbuf, ಬಹುಲಕ, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_map_locked(ಶೂನ್ಯ)
{
    ಆದರೆ (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    ಶೂನ್ಯ
map_clear_ಬಹುಲಕ(
    buf_T	*buf,		// buffer for local mappings
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ,		// mode in which to delete
    ಪೂರ್ಣಾಂಕ		local,		// TRUE for buffer-local mappings
    ಪೂರ್ಣಾಂಕ		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    ಪೂರ್ಣಾಂಕ		hash;
    ಪೂರ್ಣಾಂಕ		new_hash;

    ಆದರೆ (is_map_locked())
	ಹಿಂತಿರುಗಿ;

    validate_maphash();

    ಗಾಗಿ (hash = 0; hash < 256; ++hash)
    {
	ಆದರೆ (abbr)
	{
	    ಆದರೆ (hash > 0)	// there is only one abbrlist
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (local)
		mpp = &buf->b_ಮೊದಲ_abbr;
	    ಇಲ್ಲದಿದ್ದರೆ
		mpp = &ಮೊದಲ_abbr;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (local)
		mpp = &buf->b_maphash[hash];
	    ಇಲ್ಲದಿದ್ದರೆ
		mpp = &maphash[hash];
	}
	ಆಗ (*mpp != ಖಾಲಿ)
	{
	    mp = *mpp;
	    ಆದರೆ (mp->m_ಬಹುಲಕ & ಬಹುಲಕ)
	    {
		mp->m_ಬಹುಲಕ &= ~ಬಹುಲಕ;
		ಆದರೆ (mp->m_ಬಹುಲಕ == 0) // entry can be deleted
		{
		    map_free(mpp);
		    ಮುಂದುವರಿ;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_ಬಹುಲಕ, mp->m_ಕೀಲಿಗಳು[0]);
		ಆದರೆ (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_ಮುಂದಿನ;
		    ಆದರೆ (local)
		    {
			mp->m_ಮುಂದಿನ = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			mp->m_ಮುಂದಿನ = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    ಮುಂದುವರಿ;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_ಮುಂದಿನ);
	}
    }
}

#ಆದರೆ defined(FEAT_EVAL)
    ಪೂರ್ಣಾಂಕ
ಬಹುಲಕ_str2flags(char_u *modechars)
{
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ = 0;

    ಆದರೆ (vim_strchr(modechars, 'n') != ಖಾಲಿ)
	ಬಹುಲಕ |= ಬಹುಲಕ_ಸಾಮಾನ್ಯ;
    ಆದರೆ (vim_strchr(modechars, 'v') != ಖಾಲಿ)
	ಬಹುಲಕ |= ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT;
    ಆದರೆ (vim_strchr(modechars, 'x') != ಖಾಲಿ)
	ಬಹುಲಕ |= ಬಹುಲಕ_VISUAL;
    ಆದರೆ (vim_strchr(modechars, 's') != ಖಾಲಿ)
	ಬಹುಲಕ |= ಬಹುಲಕ_SELECT;
    ಆದರೆ (vim_strchr(modechars, 'o') != ಖಾಲಿ)
	ಬಹುಲಕ |= ಬಹುಲಕ_OP_PENDING;
    ಆದರೆ (vim_strchr(modechars, 'i') != ಖಾಲಿ)
	ಬಹುಲಕ |= ಬಹುಲಕ_ಸೇರಿಸು;
    ಆದರೆ (vim_strchr(modechars, 'l') != ಖಾಲಿ)
	ಬಹುಲಕ |= ಬಹುಲಕ_LANGMAP;
    ಆದರೆ (vim_strchr(modechars, 'c') != ಖಾಲಿ)
	ಬಹುಲಕ |= ಬಹುಲಕ_CMDLINE;

    ಹಿಂತಿರುಗಿ ಬಹುಲಕ;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    ಪೂರ್ಣಾಂಕ
map_ಗಮ್ಯ_exists(char_u *str, char_u *modechars, ಪೂರ್ಣಾಂಕ abbr)
{
    char_u	*rhs;
    char_u	*buf;
    ಪೂರ್ಣಾಂಕ		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, ಖಾಲಿ);

    retval = map_ಗಮ್ಯ_exists_ಬಹುಲಕ(rhs, ಬಹುಲಕ_str2flags(modechars), abbr);
    vim_free(buf);

    ಹಿಂತಿರುಗಿ retval;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    ಪೂರ್ಣಾಂಕ
map_ಗಮ್ಯ_exists_ಬಹುಲಕ(char_u *rhs, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ, ಪೂರ್ಣಾಂಕ abbr)
{
    mapblock_T	*mp;
    ಪೂರ್ಣಾಂಕ		hash;
    ಪೂರ್ಣಾಂಕ		exp_ಬಫರ್ = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    ಗಾಗಿ (;;)
    {
	ಗಾಗಿ (hash = 0; hash < 256; ++hash)
	{
	    ಆದರೆ (abbr)
	    {
		ಆದರೆ (hash > 0)		// there is only one abbr list
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (exp_ಬಫರ್)
		    mp = curbuf->b_ಮೊದಲ_abbr;
		ಇಲ್ಲದಿದ್ದರೆ
		    mp = ಮೊದಲ_abbr;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (exp_ಬಫರ್)
		mp = curbuf->b_maphash[hash];
	    ಇಲ್ಲದಿದ್ದರೆ
		mp = maphash[hash];
	    ಗಾಗಿ (; mp; mp = mp->m_ಮುಂದಿನ)
	    {
		ಆದರೆ ((mp->m_ಬಹುಲಕ & ಬಹುಲಕ)
			&& ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)mp->m_str, (ಅಕ್ಷರ *)rhs) != ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ TRUE;
	    }
	}
	ಆದರೆ (exp_ಬಫರ್)
	    ನಿಲ್ಲಿಸು;
	exp_ಬಫರ್ = TRUE;
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	expand_mapmodes = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	expand_isabbrev = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	expand_ಬಫರ್ = FALSE;

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
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		modifiers;
    ಪೂರ್ಣಾಂಕ		cpo_bslash;
    ಪೂರ್ಣಾಂಕ		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != ಖಾಲಿ);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != ಖಾಲಿ);

    ಗಾಗಿ (; *str; ++str)
    {
	c = *str;
	ಆದರೆ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    ಆದರೆ (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    ಆದರೆ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		ಆದರೆ (cpo_special)
		{
		    ga_clear(&ga);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		c = ಗಮ್ಯ_SPECIAL(str[1], str[2]);
		ಆದರೆ (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    ಆದರೆ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ಆದರೆ (cpo_special)
		{
		    ga_clear(&ga);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ga_concat(&ga, get_special_ಕೀಲಿ_name(c, modifiers));
		ಮುಂದುವರಿ; // for (str)
	    }
	}
	ಆದರೆ (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	ಆದರೆ (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    ಹಿಂತಿರುಗಿ (char_u *)(ga.ga_ದತ್ತಾಂಶ);
}

/*
 * Work out what to complete when doing command line completion of mapping
 * or abbreviation names.
 */
    char_u *
ಗಣ_context_ಒಳ_map_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		forceit,	// TRUE if '!' given
    ಪೂರ್ಣಾಂಕ		isabbrev,	// TRUE if abbreviation
    ಪೂರ್ಣಾಂಕ		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    ಆದರೆ (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (isunmap)
	    expand_mapmodes = get_map_ಬಹುಲಕ(&cmd, forceit || isabbrev);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    expand_mapmodes = ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE;
	    ಆದರೆ (!isabbrev)
		expand_mapmodes += ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT | ಬಹುಲಕ_ಸಾಮಾನ್ಯ
							     | ಬಹುಲಕ_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_ಬಫರ್ = FALSE;
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_ಬಫರ್ = TRUE;
		arg = skipwhite(arg + 8);
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		ಮುಂದುವರಿ;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		ಮುಂದುವರಿ;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	}
	xp->xp_ನಮೂನೆ = arg;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		hash;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		fuzzy;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *ಅಂಕೆ_Matches = 0;		    // return values in case of FAIL
    *matches = ಖಾಲಿ;

    ಆದರೆ (!fuzzy)
	ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 3);
    ಇಲ್ಲದಿದ್ದರೆ
	ga_init2(&ga, ಗಾತ್ರ(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    ಗಾಗಿ (i = 0; i < 7; ++i)
    {
	ಆದರೆ (i == 0)
	    p = (char_u *)"<silent>";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 1)
	    p = (char_u *)"<unique>";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 2)
	    p = (char_u *)"<script>";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 3)
	    p = (char_u *)"<expr>";
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 4 && !expand_ಬಫರ್)
	    p = (char_u *)"<buffer>";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 5)
	    p = (char_u *)"<nowait>";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 6)
	    p = (char_u *)"<special>";
	ಇಲ್ಲದಿದ್ದರೆ
	    ಮುಂದುವರಿ;

	ಆದರೆ (!fuzzy)
	    ಹೊಂದಿಕೆ = vim_regexec(regmatch, p, (colnr_T)0);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    score = fuzzy_ಹೊಂದಿಕೆ_str(p, pat);
	    ಹೊಂದಿಕೆ = (score != FUZZY_SCORE_NONE);
	}

	ಆದರೆ (!ಹೊಂದಿಕೆ)
	    ಮುಂದುವರಿ;

	ಆದರೆ (ga_grow(&ga, 1) == FAIL)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ((char_u **)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    ಗಾಗಿ (hash = 0; hash < 256; ++hash)
    {
	ಆದರೆ (expand_isabbrev)
	{
	    ಆದರೆ (hash > 0)	// only one abbrev list
		ನಿಲ್ಲಿಸು; // for (hash)
	    mp = ಮೊದಲ_abbr;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (expand_ಬಫರ್)
	    mp = curbuf->b_maphash[hash];
	ಇಲ್ಲದಿದ್ದರೆ
	    mp = maphash[hash];
	ಗಾಗಿ (; mp; mp = mp->m_ಮುಂದಿನ)
	{
	    ಆದರೆ (mp->m_simplified || !(mp->m_ಬಹುಲಕ & expand_mapmodes))
		ಮುಂದುವರಿ;

	    p = translate_mapping(mp->m_ಕೀಲಿಗಳು);
	    ಆದರೆ (p == ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    ಆದರೆ (!fuzzy)
		ಹೊಂದಿಕೆ = vim_regexec(regmatch, p, (colnr_T)0);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		score = fuzzy_ಹೊಂದಿಕೆ_str(p, pat);
		ಹೊಂದಿಕೆ = (score != FUZZY_SCORE_NONE);
	    }

	    ಆದರೆ (!ಹೊಂದಿಕೆ)
	    {
		vim_free(p);
		ಮುಂದುವರಿ;
	    }

	    ಆದರೆ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		((char_u **)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    ಆದರೆ (ga.ga_len == 0)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (!fuzzy)
    {
	*matches = ga.ga_ದತ್ತಾಂಶ;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (fuzzymatches_ಗಮ್ಯ_strmatches(ga.ga_ದತ್ತಾಂಶ, matches, ga.ga_len,
							FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }

    ಎಣಿಕೆ_2 = *ಅಂಕೆ_Matches;
    ಆದರೆ (ಎಣಿಕೆ_2 > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	ಆದರೆ (!fuzzy)
	    ವಿಂಗಡಿಸು_ಅಕ್ಷರಸರಣಿಗಳು(*matches, ಎಣಿಕೆ_2);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + ಎಣಿಕೆ_2;

	ಆಗ (ptr2 < ptr3)
	{
	    ಆದರೆ (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		vim_free(*ptr2++);
		ಎಣಿಕೆ_2--;
	    }
	}
    }

    *ಅಂಕೆ_Matches = ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ (ಎಣಿಕೆ_2 == 0 ? FAIL : OK);
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
    ಪೂರ್ಣಾಂಕ
check_abbr(
    ಪೂರ್ಣಾಂಕ		c,
    char_u	*ptr,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		mincol)
{
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		scol;		// starting column of the abbr.
    ಪೂರ್ಣಾಂಕ		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    ಪೂರ್ಣಾಂಕ		clen = 0;	// length in characters
    ಪೂರ್ಣಾಂಕ		is_id = TRUE;
    ಪೂರ್ಣಾಂಕ		vim_abbr;

    ಆದರೆ (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	ಹಿಂತಿರುಗಿ FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    ಆದರೆ (noremap_ಕೀಲಿಗಳು() && c != Ctrl_RSB)
	ಹಿಂತಿರುಗಿ FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    ಆದರೆ (ಲಂಬಸಾಲು == 0)				// cannot be an abbr.
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + ಲಂಬಸಾಲು);
	ಆದರೆ (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ಆದರೆ (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	ಆಗ (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    ಆದರೆ (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		ನಿಲ್ಲಿಸು;
	    }
	    ++clen;
	}
	scol = (ಪೂರ್ಣಾಂಕ)(p - ptr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!vim_iswordc(ptr[ಲಂಬಸಾಲು - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ಆದರೆ (ಲಂಬಸಾಲು > 1)
		is_id = vim_iswordc(ptr[ಲಂಬಸಾಲು - 2]);
	}
	ಗಾಗಿ (scol = ಲಂಬಸಾಲು - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    ಆದರೆ (scol < mincol)
	scol = mincol;
    ಆದರೆ (scol < ಲಂಬಸಾಲು)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = ಲಂಬಸಾಲು - scol;
	mp = curbuf->b_ಮೊದಲ_abbr;
	mp2 = ಮೊದಲ_abbr;
	ಆದರೆ (mp == ಖಾಲಿ)
	{
	    mp = mp2;
	    mp2 = ಖಾಲಿ;
	}
	ಗಾಗಿ ( ; mp; mp->m_ಮುಂದಿನ == ಖಾಲಿ
				  ? (mp = mp2, mp2 = ಖಾಲಿ) : (mp = mp->m_ಮುಂದಿನ))
	{
	    ಪೂರ್ಣಾಂಕ		qlen = mp->m_keylen;
	    char_u	*q = mp->m_ಕೀಲಿಗಳು;
	    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ;

	    ಆದರೆ (vim_strbyte(mp->m_ಕೀಲಿಗಳು, K_SPECIAL) != ಖಾಲಿ)
	    {
		char_u *qe = vim_strsave(mp->m_ಕೀಲಿಗಳು);

		// might have CSI escaped mp->m_keys
		ಆದರೆ (qe != ಖಾಲಿ)
		{
		    q = qe;
		    qlen = (ಪೂರ್ಣಾಂಕ)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    ಹೊಂದಿಕೆ =    (mp->m_ಬಹುಲಕ & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    ಆದರೆ (q != mp->m_ಕೀಲಿಗಳು)
		vim_free(q);
	    ಆದರೆ (ಹೊಂದಿಕೆ)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (mp != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ	noremap;
	    ಪೂರ್ಣಾಂಕ silent;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಪೂರ್ಣಾಂಕ ಅಭಿವ್ಯಕ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ

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
	    ಆದರೆ (c != Ctrl_RSB)
	    {
					// special key code, split up
		ಆದರೆ (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_ಎರಡನೇ(c);
		    tb[j++] = K_ಮೂರನೇ(c);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    ಆದರೆ (has_mbyte)
		    {
			ಪೂರ್ಣಾಂಕ	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			ಆದರೆ (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			ಆದರೆ (escaped != ಖಾಲಿ)
			{
			    newlen = (ಪೂರ್ಣಾಂಕ)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(ಶೂನ್ಯ)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಅಭಿವ್ಯಕ್ತಿ = mp->m_ಅಭಿವ್ಯಕ್ತಿ;

	    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ)
		s = eval_map_ಅಭಿವ್ಯಕ್ತಿ(mp, c);
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		s = mp->m_str;
	    ಆದರೆ (s != ಖಾಲಿ)
	    {
					// insert the to string
		(ಶೂನ್ಯ)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (ಪೂರ್ಣಾಂಕ)STRLEN(s) + j + 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ)
		    vim_free(s);
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    ಆದರೆ (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    ಆಗ (len-- > 0)		// delete the from string
		(ಶೂನ್ಯ)ins_typebuf(tb, 1, 0, TRUE, silent);
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_ಅಭಿವ್ಯಕ್ತಿ(
    mapblock_T	*mp,
    ಪೂರ್ಣಾಂಕ		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ;
    pos_T	save_cursor;
    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    ಪೂರ್ಣಾಂಕ		save_sctx_ಆವೃತ್ತಿ = current_sctx.sc_ಆವೃತ್ತಿ;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    ಅಭಿವ್ಯಕ್ತಿ = vim_strsave(mp->m_str);
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    vim_unescape_csi(ಅಭಿವ್ಯಕ್ತಿ);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_ಸಾಮಾನ್ಯ_ಬೀಗ;
    ಗಣ_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_ಸಂದೇಶ_ಲಂಬಸಾಲು = ಸಂದೇಶ_ಲಂಬಸಾಲು;
    save_ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;

    current_sctx.sc_ಆವೃತ್ತಿ = mp->m_script_ctx.sc_ಆವೃತ್ತಿ;
    ಆದರೆ (mp->m_script_ctx.sc_ಆವೃತ್ತಿ == SCRIPT_ಆವೃತ್ತಿ_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_ಗಮ್ಯ_string(ಅಭಿವ್ಯಕ್ತಿ, FALSE, FALSE);

    --textlock;
    --ex_ಸಾಮಾನ್ಯ_ಬೀಗ;
    curwin->w_cursor = save_cursor;
    ಸಂದೇಶ_ಲಂಬಸಾಲು = save_ಸಂದೇಶ_ಲಂಬಸಾಲು;
    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = save_ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_ಆವೃತ್ತಿ = save_sctx_ಆವೃತ್ತಿ;

    vim_free(ಅಭಿವ್ಯಕ್ತಿ);

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    ಹಿಂತಿರುಗಿ res;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಆದರೆ (res == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    d = res;
    ಗಾಗಿ (s = p; *s != NUL; )
    {
	ಆದರೆ ((s[0] == K_SPECIAL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    || (gui.ಒಳ_use && s[0] == CSI)
#ಆದರೆ_ಅಂತ್ಯ
	    ) && s[1] != NUL && s[2] != NUL)
	{
	    // Copy special key unmodified.
	    *d++ = *s++;
	    *d++ = *s++;
	    *d++ = *s++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = ಕೂಡಿಸು_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    ಹಿಂತಿರುಗಿ res;
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

    ಆಗ (*s != NUL)
    {
	ಆದರೆ (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (ಪೂರ್ಣಾಂಕ)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *d++ = *s++;
    }
    *d = NUL;
    ಹಿಂತಿರುಗಿ (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    ಅಕ್ಷರ	*cmd;
    ಪೂರ್ಣಾಂಕ		abbr;
    ಪೂರ್ಣಾಂಕ		hash;
    ಪೂರ್ಣಾಂಕ		did_cpo = FALSE;
    ಪೂರ್ಣಾಂಕ		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    ಗಾಗಿ (abbr = 0; abbr < 2; ++abbr)
	ಗಾಗಿ (hash = 0; hash < 256; ++hash)
	{
	    ಆದರೆ (abbr)
	    {
		ಆದರೆ (hash > 0)		// there is only one abbr list
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (buf != ಖಾಲಿ)
		    mp = buf->b_ಮೊದಲ_abbr;
		ಇಲ್ಲದಿದ್ದರೆ
		    mp = ಮೊದಲ_abbr;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (buf != ಖಾಲಿ)
		    mp = buf->b_maphash[hash];
		ಇಲ್ಲದಿದ್ದರೆ
		    mp = maphash[hash];
	    }

	    ಗಾಗಿ ( ; mp; mp = mp->m_ಮುಂದಿನ)
	    {
		// simplified map blocks are not created explicitly
		ಆದರೆ (mp->m_simplified)
		    ಮುಂದುವರಿ;

		// skip script-local mappings
		ಆದರೆ (mp->m_noremap == REMAP_SCRIPT)
		    ಮುಂದುವರಿ;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		ಗಾಗಿ (p = mp->m_str; *p != NUL; ++p)
		    ಆದರೆ (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (ಪೂರ್ಣಾಂಕ)KE_SNR)
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (*p != NUL)
		    ಮುಂದುವರಿ;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		ಆದರೆ (abbr)
		    cmd = "abbr";
		ಇಲ್ಲದಿದ್ದರೆ
		    cmd = "map";
		ಆಯ್ಕೆ (mp->m_ಬಹುಲಕ)
		{
		    ಸ್ಥಿತಿ ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT
							     | ಬಹುಲಕ_OP_PENDING:
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_ಸಾಮಾನ್ಯ:
			c1 = 'n';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_VISUAL:
			c1 = 'x';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_SELECT:
			c1 = 's';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_OP_PENDING:
			c1 = 'o';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_VISUAL:
			c1 = 'n';
			c2 = 'x';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_SELECT:
			c1 = 'n';
			c2 = 's';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT:
			c1 = 'v';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_SELECT | ಬಹುಲಕ_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT:
			c1 = 'n';
			c2 = 'v';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_SELECT | ಬಹುಲಕ_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT | ಬಹುಲಕ_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_CMDLINE | ಬಹುಲಕ_ಸೇರಿಸು:
			ಆದರೆ (!abbr)
			    cmd = "map!";
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_CMDLINE:
			c1 = 'c';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_ಸೇರಿಸು:
			c1 = 'i';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_LANGMAP:
			c1 = 'l';
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ಬಹುಲಕ_TERMINAL:
			c1 = 't';
			ನಿಲ್ಲಿಸು;
		    ಪೂರ್ವನಿಯೋಜಿತ:
			iemsg(e_makemap_illegal_ಬಹುಲಕ);
			ಹಿಂತಿರುಗಿ FAIL;
		}
		ಮಾಡು	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    ಆದರೆ (!did_cpo)
		    {
			ಆದರೆ (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			ಇಲ್ಲದಿದ್ದರೆ
			    ಗಾಗಿ (i = 0; i < 2; ++i)
				ಗಾಗಿ (p = (i ? mp->m_str : mp->m_ಕೀಲಿಗಳು); *p; ++p)
				    ಆದರೆ (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			ಆದರೆ (did_cpo)
			{
			    ಆದರೆ (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				ಹಿಂತಿರುಗಿ FAIL;
			}
		    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    // If it is not vim9 use legacy
		    ಆದರೆ (mp->m_ಅಭಿವ್ಯಕ್ತಿ && mp->m_script_ctx.sc_ಆವೃತ್ತಿ < SCRIPT_ಆವೃತ್ತಿ_VIM9
			    && fputs("legacy ", fd) < 0)
			ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (c1 && putc(c1, fd) < 0)
			ಹಿಂತಿರುಗಿ FAIL;
		    ಆದರೆ (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			ಹಿಂತಿರುಗಿ FAIL;
		    ಆದರೆ (fputs(cmd, fd) < 0)
			ಹಿಂತಿರುಗಿ FAIL;
		    ಆದರೆ (buf != ಖಾಲಿ && fputs(" <buffer>", fd) < 0)
			ಹಿಂತಿರುಗಿ FAIL;
		    ಆದರೆ (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			ಹಿಂತಿರುಗಿ FAIL;
		    ಆದರೆ (mp->m_silent && fputs(" <silent>", fd) < 0)
			ಹಿಂತಿರುಗಿ FAIL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    ಆದರೆ (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			ಹಿಂತಿರುಗಿ FAIL;
		    ಆದರೆ (mp->m_ಅಭಿವ್ಯಕ್ತಿ && fputs(" <expr>", fd) < 0)
			ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ

		    ಆದರೆ (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_ಕೀಲಿಗಳು, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			ಹಿಂತಿರುಗಿ FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} ಆಗ (c1 != NUL);
	    }
	}

    ಆದರೆ (did_cpo)
	ಆದರೆ (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
put_escstr(FILE *fd, char_u *strstart, ಪೂರ್ಣಾಂಕ what)
{
    char_u	*str = strstart;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		modifiers;

    // :map xx <Nop>
    ಆದರೆ (*str == NUL && what == 1)
    {
	ಆದರೆ (fprintf(fd, "<Nop>") < 0)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಹಿಂತಿರುಗಿ OK;
    }

    ಗಾಗಿ ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಆಗ (*p != NUL)
		ಆದರೆ (fputc(*p++, fd) < 0)
		    ಹಿಂತಿರುಗಿ FAIL;
	    --str;
	    ಮುಂದುವರಿ;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	ಆದರೆ (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    ಆದರೆ (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		ಆದರೆ (p == ಖಾಲಿ)
		    c = *str;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    ಆದರೆ (c == K_SPECIAL)
	    {
		c = ಗಮ್ಯ_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    ಆದರೆ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ಆದರೆ (fputs((ಅಕ್ಷರ *)get_special_ಕೀಲಿ_name(c, modifiers), fd) < 0)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಮುಂದುವರಿ;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	ಆದರೆ (c == NL)
	{
	    ಆದರೆ (what == 2)
	    {
		ಆದರೆ (fprintf(fd, "\\\026\n") < 0)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (fprintf(fd, "<NL>") < 0)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಮುಂದುವರಿ;
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
	ಆದರೆ (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    ಆದರೆ (putc('\\', fd) < 0)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    ಆದರೆ (putc(Ctrl_V, fd) < 0)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (putc(c, fd) < 0)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ		i;
    char_u	buf[3];
    ಪೂರ್ಣಾಂಕ		abbr;
    ಪೂರ್ಣಾಂಕ		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_ತಳ್ಳು(ETYPE_ಆಂತರಿಕ, (char_u *)"mappings", 0);
    ESTACK_CHECK_ಸಿದ್ಧತೆ;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    ಗಾಗಿ (bp = firstbuf; ; bp = bp->b_ಮುಂದಿನ)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	ಗಾಗಿ (abbr = 0; abbr <= 1; ++abbr)
	    ಗಾಗಿ (hash = 0; hash < 256; ++hash)
	    {
		ಆದರೆ (abbr)
		{
		    ಆದರೆ (hash)	    // there is only one abbr list
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (bp != ಖಾಲಿ)
			mp = bp->b_ಮೊದಲ_abbr;
		    ಇಲ್ಲದಿದ್ದರೆ
			mp = ಮೊದಲ_abbr;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (bp != ಖಾಲಿ)
			mp = bp->b_maphash[hash];
		    ಇಲ್ಲದಿದ್ದರೆ
			mp = maphash[hash];
		}
		ಗಾಗಿ ( ; mp != ಖಾಲಿ; mp = mp->m_ಮುಂದಿನ)
		{
		    ಗಾಗಿ (i = 0; i <= 1; ++i)	// do this twice
		    {
			ಆದರೆ (i == 0)
			    p = mp->m_ಕೀಲಿಗಳು;	// once for the "from" part
			ಇಲ್ಲದಿದ್ದರೆ
			    p = mp->m_str;	// and once for the "to" part
			ಆಗ (*p)
			{
			    ಆದರೆ (*p == K_SPECIAL)
			    {
				++p;
				ಆದರೆ (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (ಶೂನ್ಯ)ಕೂಡಿಸು_termcap_ನಮೂದು(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	ಆದರೆ (bp == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    }
    ESTACK_CHECK_NOW;
    estack_ತೆಗೆ();
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*ಕೀಲಿಗಳು,
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ,
    ಪೂರ್ಣಾಂಕ		exact,		// require exact match
    ಪೂರ್ಣಾಂಕ		ign_mod,	// ignore preceding modifier
    ಪೂರ್ಣಾಂಕ		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    ಪೂರ್ಣಾಂಕ		*local_ptr)	// return: buffer-local mapping or NULL
{
    ಪೂರ್ಣಾಂಕ		hash;
    ಪೂರ್ಣಾಂಕ		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		local;

    validate_maphash();

    len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಕೀಲಿಗಳು);
    ಗಾಗಿ (local = 1; local >= 0; --local)
	// loop over all hash lists
	ಗಾಗಿ (hash = 0; hash < 256; ++hash)
	{
	    ಆದರೆ (abbr)
	    {
		ಆದರೆ (hash > 0)		// there is only one list.
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (local)
		    mp = curbuf->b_ಮೊದಲ_abbr;
		ಇಲ್ಲದಿದ್ದರೆ
		    mp = ಮೊದಲ_abbr;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (local)
		mp = curbuf->b_maphash[hash];
	    ಇಲ್ಲದಿದ್ದರೆ
		mp = maphash[hash];
	    ಗಾಗಿ ( ; mp != ಖಾಲಿ; mp = mp->m_ಮುಂದಿನ)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		ಆದರೆ ((mp->m_ಬಹುಲಕ & ಬಹುಲಕ) && (!exact || mp->m_keylen == len))
		{
		    ಆದರೆ (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    ಇಲ್ಲದಿದ್ದರೆ
			minlen = len;
		    s = mp->m_ಕೀಲಿಗಳು;
		    ಆದರೆ (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			ಆದರೆ (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    ಆದರೆ (STRNCMP(s, ಕೀಲಿಗಳು, minlen) == 0)
		    {
			ಆದರೆ (mp_ptr != ಖಾಲಿ)
			    *mp_ptr = mp;
			ಆದರೆ (local_ptr != ಖಾಲಿ)
			    *local_ptr = local;
			ಹಿಂತಿರುಗಿ mp->m_str;
		    }
		}
	    }
	}

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * "hasmapto()" function
 */
    ಶೂನ್ಯ
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*ಬಹುಲಕ;
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		abbr = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string(&argvars[0]);
    ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
	ಬಹುಲಕ = (char_u *)"nvo";
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಬಹುಲಕ = tv_get_string_buf(&argvars[1], buf);
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (ಪೂರ್ಣಾಂಕ)tv_get_bool(&argvars[2]);
    }

    ಆದರೆ (map_ಗಮ್ಯ_exists(name, ಬಹುಲಕ, abbr))
	rettv->vval.v_ಸಂಖ್ಯೆ = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
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
	ಪೂರ್ಣಾಂಕ	    ಬಫರ್_local,   // false if not buffer local mapping
	ಪೂರ್ಣಾಂಕ	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_ಕೀಲಿಗಳು, TRUE, FALSE);
    char_u	    *mapmode = map_ಬಹುಲಕ_ಗಮ್ಯ_chars(mp->m_ಬಹುಲಕ);

    dict_ಕೂಡಿಸು_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_ಕೂಡಿಸು_string(dict, "lhsraw", mp->m_ಕೀಲಿಗಳು);
    ಆದರೆ (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_ಕೂಡಿಸು_string(dict, "lhsrawalt", lhsrawalt);
    dict_ಕೂಡಿಸು_string(dict, "rhs", mp->m_orig_str);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "expr", mp->m_ಅಭಿವ್ಯಕ್ತಿ ? 1L : 0L);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "sid", (ದೀರ್ಘ)mp->m_script_ctx.sc_sid);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "scriptversion",
		    (ದೀರ್ಘ)mp->m_script_ctx.sc_ಆವೃತ್ತಿ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "lnum", (ದೀರ್ಘ)mp->m_script_ctx.sc_lnum);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "buffer", (ದೀರ್ಘ)ಬಫರ್_local);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_ಕೂಡಿಸು_string(dict, "mode", mapmode);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "abbr", abbr ? 1L : 0L);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "mode_bits", mp->m_ಬಹುಲಕ);

    vim_free(mapmode);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
get_maparg(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ exact)
{
    char_u	*ಕೀಲಿಗಳು;
    char_u	*ಕೀಲಿಗಳು_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*ಕೀಲಿಗಳು_buf = ಖಾಲಿ;
    char_u	*alt_ಕೀಲಿಗಳು_buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		did_simplify = FALSE;
    char_u	*rhs;
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ;
    ಪೂರ್ಣಾಂಕ		abbr = FALSE;
    ಪೂರ್ಣಾಂಕ		get_dict = FALSE;
    mapblock_T	*mp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಬಫರ್_local;
    ಪೂರ್ಣಾಂಕ		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಕೀಲಿಗಳು = tv_get_string(&argvars[0]);
    ಆದರೆ (*ಕೀಲಿಗಳು == NUL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (ಪೂರ್ಣಾಂಕ)tv_get_bool(&argvars[2]);
	    ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (ಪೂರ್ಣಾಂಕ)tv_get_bool(&argvars[3]);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	which = (char_u *)"";
    ಆದರೆ (which == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಬಹುಲಕ = get_map_ಬಹುಲಕ(&which, 0);

    ಕೀಲಿಗಳು_simplified = replace_termcodes(ಕೀಲಿಗಳು, &ಕೀಲಿಗಳು_buf, 0, flags,
								&did_simplify);
    rhs = check_map(ಕೀಲಿಗಳು_simplified, ಬಹುಲಕ, exact, FALSE, abbr,
							   &mp, &ಬಫರ್_local);
    ಆದರೆ (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(ಶೂನ್ಯ)replace_termcodes(ಕೀಲಿಗಳು, &alt_ಕೀಲಿಗಳು_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, ಖಾಲಿ);
	rhs = check_map(alt_ಕೀಲಿಗಳು_buf, ಬಹುಲಕ, exact, FALSE, abbr, &mp,
								&ಬಫರ್_local);
    }

    ಆದರೆ (!get_dict)
    {
	// Return a string.
	ಆದರೆ (rhs != ಖಾಲಿ)
	{
	    ಆದರೆ (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    ಇಲ್ಲದಿದ್ದರೆ
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rettv_dict_alloc(rettv) == OK && rhs != ಖಾಲಿ)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? ಕೀಲಿಗಳು_simplified : ಖಾಲಿ,
			  ಬಫರ್_local, abbr);

    vim_free(ಕೀಲಿಗಳು_buf);
    vim_free(alt_ಕೀಲಿಗಳು_buf);
}

/*
 * "maplist()" function
 */
    ಶೂನ್ಯ
f_maplist(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    ಪೂರ್ಣಾಂಕ		ಬಫರ್_local;
    char_u	*ಕೀಲಿಗಳು_buf;
    ಪೂರ್ಣಾಂಕ		did_simplify;
    ಪೂರ್ಣಾಂಕ		hash;
    char_u	*lhs;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    ಪೂರ್ಣಾಂಕ		abbr = FALSE;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    ಗಾಗಿ (ಬಫರ್_local = 0; ಬಫರ್_local <= 1; ++ಬಫರ್_local)
    {
	ಗಾಗಿ (hash = 0; hash < 256; ++hash)
	{
	    ಆದರೆ (abbr)
	    {
		ಆದರೆ (hash > 0)		// there is only one abbr list
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (ಬಫರ್_local)
		    mp = curbuf->b_ಮೊದಲ_abbr;
		ಇಲ್ಲದಿದ್ದರೆ
		    mp = ಮೊದಲ_abbr;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಫರ್_local)
		mp = curbuf->b_maphash[hash];
	    ಇಲ್ಲದಿದ್ದರೆ
		mp = maphash[hash];
	    ಗಾಗಿ (; mp; mp = mp->m_ಮುಂದಿನ)
	    {
		ಆದರೆ (mp->m_simplified)
		    ಮುಂದುವರಿ;
		ಆದರೆ ((d = dict_alloc()) == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ;
		ಆದರೆ (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    ಹಿಂತಿರುಗಿ;
		}

		ಕೀಲಿಗಳು_buf = ಖಾಲಿ;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_ಕೀಲಿಗಳು, TRUE, FALSE);
		(ಶೂನ್ಯ)replace_termcodes(lhs, &ಕೀಲಿಗಳು_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? ಕೀಲಿಗಳು_buf : ಖಾಲಿ,
				 ಬಫರ್_local, abbr);
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
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		ಹಿಂತಿರುಗಿ;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    ಶೂನ್ಯ
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_map_ಬಹುಲಕ_string(char_u *ಬಹುಲಕ_string, ಪೂರ್ಣಾಂಕ abbr)
{
    char_u	*p = ಬಹುಲಕ_string;
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ = 0;
    ಪೂರ್ಣಾಂಕ		tmode;
    ಪೂರ್ಣಾಂಕ		modec;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	ಮುಖವಾಡ_V = ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	ಮುಖವಾಡ_MAP = ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT | ಬಹುಲಕ_ಸಾಮಾನ್ಯ
							     | ಬಹುಲಕ_OP_PENDING;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	ಮುಖವಾಡ_BANG = ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE;

    ಆದರೆ (*p == NUL)
	p = (char_u *)" ";	// compatibility
    ಆಗ ((modec = *p++))
    {
	ಆಯ್ಕೆ (modec)
	{
	    ಸ್ಥಿತಿ 'i': tmode = ಬಹುಲಕ_ಸೇರಿಸು;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'l': tmode = ಬಹುಲಕ_LANGMAP;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'c': tmode = ಬಹುಲಕ_CMDLINE;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'n': tmode = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'x': tmode = ಬಹುಲಕ_VISUAL;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 's': tmode = ಬಹುಲಕ_SELECT;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'o': tmode = ಬಹುಲಕ_OP_PENDING;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 't': tmode = ಬಹುಲಕ_TERMINAL;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'v': tmode = ಮುಖವಾಡ_V;		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '!': tmode = ಮುಖವಾಡ_BANG;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ' ': tmode = ಮುಖವಾಡ_MAP;		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		      ಹಿಂತಿರುಗಿ 0; // error, unknown mode character
	}
	ಬಹುಲಕ |= tmode;
    }
    ಆದರೆ ((abbr && (ಬಹುಲಕ & ~ಮುಖವಾಡ_BANG) != 0)
	|| (!abbr && (ಬಹುಲಕ & (ಬಹುಲಕ-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((ಬಹುಲಕ & ಮುಖವಾಡ_BANG) != 0 && (ಬಹುಲಕ & ~ಮುಖವಾಡ_BANG) == 0)
		    || ((ಬಹುಲಕ & ಮುಖವಾಡ_MAP) != 0 && (ಬಹುಲಕ & ~ಮುಖವಾಡ_MAP) == 0)))))
	ಹಿಂತಿರುಗಿ 0;

    ಹಿಂತಿರುಗಿ ಬಹುಲಕ;
}

/*
 * "mapset()" function
 */
    ಶೂನ್ಯ
f_mapset(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    char_u	*which;
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ;
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		noremap;
    ಪೂರ್ಣಾಂಕ		ಅಭಿವ್ಯಕ್ತಿ;
    ಪೂರ್ಣಾಂಕ		silent;
    ಪೂರ್ಣಾಂಕ		ಬಫರ್;
    scid_T	sid;
    ಪೂರ್ಣಾಂಕ		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_ಕೋಷ್ಟಕ = maphash;
    mapblock_T  **abbr_ಕೋಷ್ಟಕ = &ಮೊದಲ_abbr;
    ಪೂರ್ಣಾಂಕ		nowait;
    char_u	*arg;
    ಪೂರ್ಣಾಂಕ		dict_only;
    mapblock_T	*mp_ಫಲಿತಾಂಶ[2] = {ಖಾಲಿ, ಖಾಲಿ};

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	ಆದರೆ (which == ಖಾಲಿ || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_ಒಳ_mapset_dict_ವಾದ));
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	ಆದರೆ (which == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	is_abbr = (ಪೂರ್ಣಾಂಕ)tv_get_bool(&argvars[1]);

	ಆದರೆ (check_for_dict_arg(argvars, 2) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	d = argvars[2].vval.v_dict;
    }
    ಬಹುಲಕ = get_map_ಬಹುಲಕ_string(which, is_abbr);
    ಆದರೆ (ಬಹುಲಕ == 0)
    {
	semsg(_(e_illegal_map_ಬಹುಲಕ_string_str), which);
	ಹಿಂತಿರುಗಿ;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    ಆದರೆ (lhs == ಖಾಲಿ || lhsraw == ಖಾಲಿ || rhs == ಖಾಲಿ)
    {
	emsg(_(e_entries_missing_ಒಳ_mapset_dict_ವಾದ));
	ಹಿಂತಿರುಗಿ;
    }
    orig_rhs = rhs;

    noremap = dict_get_ಸಂಖ್ಯೆ(d, "noremap") ? REMAP_NONE: 0;
    ಆದರೆ (dict_get_ಸಂಖ್ಯೆ(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    ಅಭಿವ್ಯಕ್ತಿ = dict_get_ಸಂಖ್ಯೆ(d, "expr") != 0;
    silent = dict_get_ಸಂಖ್ಯೆ(d, "silent") != 0;
    sid = dict_get_ಸಂಖ್ಯೆ(d, "sid");
    scriptversion = dict_get_ಸಂಖ್ಯೆ(d, "scriptversion");
    lnum = dict_get_ಸಂಖ್ಯೆ(d, "lnum");
    ಬಫರ್ = dict_get_ಸಂಖ್ಯೆ(d, "buffer");
    nowait = dict_get_ಸಂಖ್ಯೆ(d, "nowait") != 0;
    // mode from the dict is not used

    ಆದರೆ (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    ಇಲ್ಲದಿದ್ದರೆ
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, ಖಾಲಿ);

    ಆದರೆ (ಬಫರ್)
    {
	map_ಕೋಷ್ಟಕ = curbuf->b_maphash;
	abbr_ಕೋಷ್ಟಕ = &curbuf->b_ಮೊದಲ_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    ಆದರೆ (ಬಫರ್)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	ಆದರೆ (arg == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	arg = vim_strsave(lhs);
	ಆದರೆ (arg == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, ಬಹುಲಕ, is_abbr);
    vim_free(arg);

    mp_ಫಲಿತಾಂಶ[0] = map_ಕೂಡಿಸು(map_ಕೋಷ್ಟಕ, abbr_ಕೋಷ್ಟಕ, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, ಬಹುಲಕ, is_abbr, ಅಭಿವ್ಯಕ್ತಿ, sid,
			    scriptversion, lnum, 0);
    ಆದರೆ (lhsrawalt != ಖಾಲಿ)
	mp_ಫಲಿತಾಂಶ[1] = map_ಕೂಡಿಸು(map_ಕೋಷ್ಟಕ, abbr_ಕೋಷ್ಟಕ, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, ಬಹುಲಕ, is_abbr, ಅಭಿವ್ಯಕ್ತಿ, sid,
			    scriptversion, lnum, 1);

    ಆದರೆ (mp_ಫಲಿತಾಂಶ[0] != ಖಾಲಿ && mp_ಫಲಿತಾಂಶ[1] != ಖಾಲಿ)
    {
	mp_ಫಲಿತಾಂಶ[0]->m_alt = mp_ಫಲಿತಾಂಶ[1];
	mp_ಫಲಿತಾಂಶ[1]->m_alt = mp_ಫಲಿತಾಂಶ[0];
    }

    vim_free(arg_buf);
}
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(ಬಹುಲಕ_VISUAL | ಬಹುಲಕ_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
ರಚನೆ initmap
{
    char_u	*arg;
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
ಸ್ಥಿರ ರಚನೆ initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", ಬಹುಲಕ_ಸಾಮಾನ್ಯ},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
ಸ್ಥಿರ ರಚನೆ initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", ಬಹುಲಕ_ಸಾಮಾನ್ಯ | VIS_SEL},
	{(char_u *)"\316w <C-Home>", ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE},
	{(char_u *)"\316u <C-End>", ಬಹುಲಕ_ಸಾಮಾನ್ಯ | VIS_SEL},
	{(char_u *)"\316u <C-End>", ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", ಬಹುಲಕ_ಸಾಮಾನ್ಯ},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", ಬಹುಲಕ_ಸೇರಿಸು},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  ಇಲ್ಲದಿದ್ದರೆ
	{(char_u *)"\316\324 P", ಬಹುಲಕ_ಸಾಮಾನ್ಯ},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", ಬಹುಲಕ_ಸೇರಿಸು}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# ಆದರೆ defined(MACOS_X)
ಸ್ಥಿರ ರಚನೆ initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", ಬಹುಲಕ_ಸಾಮಾನ್ಯ},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE},
	{(char_u *)"<D-c> \"*y", VIS_SEL},
	{(char_u *)"<D-x> \"*d", VIS_SEL},
	{(char_u *)"<Backspace> \"-d", VIS_SEL},
};
# endif

# undef VIS_SEL
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set up default mappings.
 */
    ಶೂನ್ಯ
init_mappings(ಶೂನ್ಯ)
{
#ಆದರೆ defined(MSWIN) || defined(MACOS_X)
    ಪೂರ್ಣಾಂಕ		i;

# ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ಆದರೆ (!gui.starting)
#  endif
    {
	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(cinitmappings); ++i)
	    ಕೂಡಿಸು_map(cinitmappings[i].arg, cinitmappings[i].ಬಹುಲಕ, FALSE);
    }
# endif
# ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(initmappings); ++i)
	ಕೂಡಿಸು_map(initmappings[i].arg, initmappings[i].ಬಹುಲಕ, FALSE);
# endif
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    ಶೂನ್ಯ
ಕೂಡಿಸು_map(char_u *map, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ, ಪೂರ್ಣಾಂಕ nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = ಖಾಲಿಗಳು_option;	// Allow <> notation
    s = vim_strsave(map);
    ಆದರೆ (s != ಖಾಲಿ)
    {
	(ಶೂನ್ಯ)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, ಬಹುಲಕ, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#ಆದರೆ defined(FEAT_LANGMAP)
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	    from;
    ಪೂರ್ಣಾಂಕ     ಗಮ್ಯ;
} langmap_ನಮೂದು_T;

ಸ್ಥಿರ garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
langmap_ಗಣ_ನಮೂದು(ಪೂರ್ಣಾಂಕ from, ಪೂರ್ಣಾಂಕ ಗಮ್ಯ)
{
    langmap_ನಮೂದು_T *entries = (langmap_ನಮೂದು_T *)(langmap_mapga.ga_ದತ್ತಾಂಶ);
    ಪೂರ್ಣಾಂಕ		    a = 0;
    ಪೂರ್ಣಾಂಕ		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    ಆಗ (a != b)
    {
	ಪೂರ್ಣಾಂಕ i = (a + b) / 2;
	ಪೂರ್ಣಾಂಕ d = entries[i].from - from;

	ಆದರೆ (d == 0)
	{
	    entries[i].ಗಮ್ಯ = ಗಮ್ಯ;
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (d < 0)
	    a = i + 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    b = i;
    }

    ಆದರೆ (ga_grow(&langmap_mapga, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_ನಮೂದು_T *)(langmap_mapga.ga_ದತ್ತಾಂಶ) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * ಗಾತ್ರ(langmap_ನಮೂದು_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].ಗಮ್ಯ = ಗಮ್ಯ;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    ಪೂರ್ಣಾಂಕ
langmap_adjust_mb(ಪೂರ್ಣಾಂಕ c)
{
    langmap_ನಮೂದು_T *entries = (langmap_ನಮೂದು_T *)(langmap_mapga.ga_ದತ್ತಾಂಶ);
    ಪೂರ್ಣಾಂಕ a = 0;
    ಪೂರ್ಣಾಂಕ b = langmap_mapga.ga_len;

    ಆಗ (a != b)
    {
	ಪೂರ್ಣಾಂಕ i = (a + b) / 2;
	ಪೂರ್ಣಾಂಕ d = entries[i].from - c;

	ಆದರೆ (d == 0)
	    ಹಿಂತಿರುಗಿ entries[i].ಗಮ್ಯ;  // found matching entry
	ಆದರೆ (d < 0)
	    a = i + 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    b = i;
    }
    ಹಿಂತಿರುಗಿ c;  // no entry found, return "c" unmodified
}

    ಶೂನ್ಯ
langmap_init(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, ಗಾತ್ರ(langmap_ನಮೂದು_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    ಅಕ್ಷರ *
did_ಗಣ_langmap(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    char_u  *p;
    char_u  *p2;
    ಪೂರ್ಣಾಂಕ	    from, ಗಮ್ಯ;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    ಗಾಗಿ (p = p_langmap; p[0] != NUL; )
    {
	ಗಾಗಿ (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    ಆದರೆ (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	ಆದರೆ (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	ಇಲ್ಲದಿದ್ದರೆ
	    p2 = ಖಾಲಿ;	    // aAbBcCdD form, p2 is NULL
	ಆಗ (p[0])
	{
	    ಆದರೆ (p[0] == ',')
	    {
		++p;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    ಗಮ್ಯ = NUL;
	    ಆದರೆ (p2 == ಖಾಲಿ)
	    {
		MB_PTR_ADV(p);
		ಆದರೆ (p[0] != ',')
		{
		    ಆದರೆ (p[0] == '\\')
			++p;
		    ಗಮ್ಯ = (*mb_ptr2char)(p);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (p2[0] != ',')
		{
		    ಆದರೆ (p2[0] == '\\')
			++p2;
		    ಗಮ್ಯ = (*mb_ptr2char)(p2);
		}
	    }
	    ಆದರೆ (ಗಮ್ಯ == NUL)
	    {
		sprintf(ವಾದಗಳು->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		ಹಿಂತಿರುಗಿ ವಾದಗಳು->os_errbuf;
	    }

	    ಆದರೆ (from >= 256)
		langmap_ಗಣ_ನಮೂದು(from, ಗಮ್ಯ);
	    ಇಲ್ಲದಿದ್ದರೆ
		langmap_mapchar[from & 255] = ಗಮ್ಯ;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    ಆದರೆ (p2 != ಖಾಲಿ)
	    {
		MB_PTR_ADV(p2);
		ಆದರೆ (*p == ';')
		{
		    p = p2;
		    ಆದರೆ (p[0] != NUL)
		    {
			ಆದರೆ (p[0] != ',')
			{
			    vim_snprintf(ವಾದಗಳು->os_errbuf, ವಾದಗಳು->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    ಹಿಂತಿರುಗಿ ವಾದಗಳು->os_errbuf;
			}
			++p;
		    }
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
do_exmap(exarg_T *eap, ಪೂರ್ಣಾಂಕ isabbrev)
{
    ಪೂರ್ಣಾಂಕ	    ಬಹುಲಕ;
    char_u  *cmdp;

    cmdp = eap->cmd;
    ಬಹುಲಕ = get_map_ಬಹುಲಕ(&cmdp, eap->forceit || isabbrev);

    ಆಯ್ಕೆ (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, ಬಹುಲಕ, isabbrev))
    {
	ಸ್ಥಿತಿ 1: emsg(_(e_invalid_ವಾದ));
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		ನಿಲ್ಲಿಸು;
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
    ಆದರೆ (secure)
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
