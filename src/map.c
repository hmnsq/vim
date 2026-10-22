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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/*
 * List used for abbreviations.
 */
ਸਥਿਰ mapblock_T	*ਪਹਿਲਾ_abbr = ਨਿਰਾ; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
ਸਥਿਰ mapblock_T	*(maphash[256]);
ਸਥਿਰ ਪੂਰਨ		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
ਸਥਿਰ ਪੂਰਨ		map_locked = 0;

/*
 * Make a hash value for a mapping.
 * "mode" is the lower 4 bits of the State for the mapping.
 * "c1" is the first character of the "lhs".
 * Returns a value between 0 and 255, index in maphash.
 * Put Normal/Visual mode mappings mostly separately from Insert/Cmdline mode.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAP_HASH(ਬਹੁਲਕ, c1) (((ਬਹੁਲਕ) & (ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT | ਬਹੁਲਕ_OP_PENDING | ਬਹੁਲਕ_TERMINAL)) ? (c1) : ((c1) ^ 0x80))

/*
 * Get the start of the hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_maphash_list(ਪੂਰਨ state, ਪੂਰਨ c)
{
    ਵਾਪਸ maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(ਪੂਰਨ state, ਪੂਰਨ c)
{
    ਵਾਪਸ curbuf->b_maphash[MAP_HASH(state, c)];
}

    ਪੂਰਨ
is_maphash_valid(ਖਾਲੀ)
{
    ਵਾਪਸ maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    ਸਥਿਰ ਖਾਲੀ
validate_maphash(ਖਾਲੀ)
{
    ਜੇ (maphash_valid)
	ਵਾਪਸ;

    CLEAR_ਖੇਤਰ(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    ਸਥਿਰ ਖਾਲੀ
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_ਕੁੰਜੀਆਂ);
    ਜੇ (mp->m_alt != ਨਿਰਾ)
	mp->m_alt->m_alt = ਨਿਰਾ;
    vim_free(mp->m_str);
    vim_free(mp->m_orig_str);
    *mpp = mp->m_ਅਗਲਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    reset_ਆਖਰੀ_ਵਰਤਿਆ_map(mp);
#ਜੇ_ਅੰਤ
    vim_free(mp);
}

/*
 * Return characters to represent the map mode in an allocated string.
 * Returns NULL when out of memory.
 */
    ਸਥਿਰ char_u *
map_ਬਹੁਲਕ_ਮੰਜ਼ਿਲ_chars(ਪੂਰਨ ਬਹੁਲਕ)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    ਜੇ ((ਬਹੁਲਕ & (ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE)) == (ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ & ਬਹੁਲਕ_ਪਾਓ)
	ga_append(&mapmode, 'i');			// :imap
    ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ & ਬਹੁਲਕ_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ & ਬਹੁਲਕ_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    ਨਹੀਂਤਾਂ ਜੇ ((ਬਹੁਲਕ
		 & (ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT | ਬਹੁਲਕ_OP_PENDING))
		== (ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT | ਬਹੁਲਕ_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਬਹੁਲਕ & ਬਹੁਲਕ_ਸਾਧਾਰਨ)
	    ga_append(&mapmode, 'n');			// :nmap
	ਜੇ (ਬਹੁਲਕ & ਬਹੁਲਕ_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	ਜੇ (ਬਹੁਲਕ & ਬਹੁਲਕ_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	ਜੇ ((ਬਹੁਲਕ & (ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT)) == (ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ਬਹੁਲਕ & ਬਹੁਲਕ_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    ਜੇ (ਬਹੁਲਕ & ਬਹੁਲਕ_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    ਵਾਪਸ (char_u *)mapmode.ga_ਅੰਕੜੇ;
}

/*
 * Output a line for one mapping.
 */
    ਸਥਿਰ ਖਾਲੀ
showmap(
    mapblock_T	*mp,
    ਪੂਰਨ		local)	    // TRUE for buffer-local map
{
    ਪੂਰਨ		len = 1;
    char_u	*mapchars;

    ਜੇ (ਸੁਨੇਹਾ_filtered(mp->m_ਕੁੰਜੀਆਂ) && ਸੁਨੇਹਾ_filtered(mp->m_str))
	ਵਾਪਸ;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    ਜੇ (ਸੁਨੇਹਾ_didout || ਸੁਨੇਹਾ_silent != 0)
    {
	ਸੁਨੇਹਾ_putchar('\n');
	ਜੇ (got_int)	    // 'q' typed at MORE prompt
	    ਜਾਓ theend;
    }

    mapchars = map_ਬਹੁਲਕ_ਮੰਜ਼ਿਲ_chars(mp->m_ਬਹੁਲਕ);
    ਜੇ (mapchars != ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)mapchars);
	len = (ਪੂਰਨ)STRLEN(mapchars);
	vim_free(mapchars);
    }

    ਜਦੋਂ (++len <= 3)
	ਸੁਨੇਹਾ_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = ਸੁਨੇਹਾ_outtrans_special(mp->m_ਕੁੰਜੀਆਂ, TRUE, 0);
    ਕਰੋ
    {
	ਸੁਨੇਹਾ_putchar(' ');		// pad with blanks
	++len;
    } ਜਦੋਂ (len < 12);

    ਜੇ (mp->m_noremap == REMAP_NONE)
	ਸੁਨੇਹਾ_puts_attr("*", HL_ATTR(HLF_8));
    ਨਹੀਂਤਾਂ ਜੇ (mp->m_noremap == REMAP_SCRIPT)
	ਸੁਨੇਹਾ_puts_attr("&", HL_ATTR(HLF_8));
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_putchar(' ');

    ਜੇ (local)
	ਸੁਨੇਹਾ_putchar('@');
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    ਜੇ (*mp->m_str == NUL)
	ਸੁਨੇਹਾ_puts_attr("<Nop>", HL_ATTR(HLF_8));
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_outtrans_special(mp->m_str, FALSE, 0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (p_verbose > 0)
	ਆਖਰੀ_ਸੈੱਟ_ਸੁਨੇਹਾ(mp->m_script_ctx);
#ਜੇ_ਅੰਤ
    ਸੁਨੇਹਾ_clr_eos();
    ਬਾਹਰਲਾ_flush();			// show one line at a time

theend:
    --map_locked;
}

    ਸਥਿਰ mapblock_T *
map_ਜੋੜੋ(
	mapblock_T  **map_ਸਾਰਣੀ,
	mapblock_T  **abbr_ਸਾਰਣੀ,
	char_u	    *ਕੁੰਜੀਆਂ,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	ਪੂਰਨ	    noremap,
	ਪੂਰਨ	    nowait,
	ਪੂਰਨ	    silent,
	ਪੂਰਨ	    ਬਹੁਲਕ,
	ਪੂਰਨ	    is_abbr,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਪੂਰਨ	    ਵਿਅੰਜਕ,
	scid_T	    sid,	    // 0 to use current_sctx
	ਪੂਰਨ	    scriptversion,
	linenr_T    lnum,
#ਜੇ_ਅੰਤ
	ਪੂਰਨ	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    ਜੇ (mp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    ਜੇ (*ਕੁੰਜੀਆਂ == Ctrl_C)
    {
	ਜੇ (map_ਸਾਰਣੀ == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= ਬਹੁਲਕ;
	ਨਹੀਂਤਾਂ
	    mapped_ctrl_c |= ਬਹੁਲਕ;
    }

    mp->m_ਕੁੰਜੀਆਂ = vim_strsave(ਕੁੰਜੀਆਂ);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    ਜੇ (mp->m_ਕੁੰਜੀਆਂ == ਨਿਰਾ || mp->m_str == ਨਿਰਾ)
    {
	vim_free(mp->m_ਕੁੰਜੀਆਂ);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	ਵਾਪਸ ਨਿਰਾ;
    }
    mp->m_keylen = (ਪੂਰਨ)STRLEN(mp->m_ਕੁੰਜੀਆਂ);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_ਬਹੁਲਕ = ਬਹੁਲਕ;
    mp->m_simplified = simplified;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    mp->m_ਵਿਅੰਜਕ = ਵਿਅੰਜਕ;
    ਜੇ (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_ਸੰਸਕਰਣ = scriptversion;
    }
    ਨਹੀਂਤਾਂ
    {
	mp->m_script_ctx = current_sctx;
	ਜੇ (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_ਸੰਸਕਰਣ = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#ਜੇ_ਅੰਤ

    // add the new entry in front of the abbrlist or maphash[] list
    ਜੇ (is_abbr)
    {
	mp->m_ਅਗਲਾ = *abbr_ਸਾਰਣੀ;
	*abbr_ਸਾਰਣੀ = mp;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ n = MAP_HASH(mp->m_ਬਹੁਲਕ, mp->m_ਕੁੰਜੀਆਂ[0]);

	mp->m_ਅਗਲਾ = map_ਸਾਰਣੀ[n];
	map_ਸਾਰਣੀ[n] = mp;
    }
    ਵਾਪਸ mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    ਸਥਿਰ ਖਾਲੀ
list_mappings(
	ਪੂਰਨ	keyround,
	ਪੂਰਨ	abbrev,
	ਪੂਰਨ	haskey,
	char_u	*ਕੁੰਜੀਆਂ,
	ਪੂਰਨ	ਕੁੰਜੀਆਂ_len,
	ਪੂਰਨ	ਬਹੁਲਕ,
	ਪੂਰਨ	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    ਜੇ (p_verbose > 0 && keyround == 1)
    {
	ਜੇ (ਵੇਖਿਆ_Modify_Other_ਕੁੰਜੀਆਂ)
	    ਸੁਨੇਹਾ_puts(_("Seen modifyOtherKeys: true\n"));

	ਜੇ (modify_otherkeys_state != MOKS_INITIAL)
	{
	    ਅੱਖਰ *name = _("Unknown");
	    ਚੋਣ (modify_otherkeys_state)
	    {
		ਹਾਲਤ MOKS_INITIAL: ਰੋਕੋ;
		ਹਾਲਤ MOKS_OFF: name = _("Off"); ਰੋਕੋ;
		ਹਾਲਤ MOKS_ENABLED: name = _("On"); ਰੋਕੋ;
		ਹਾਲਤ MOKS_DISABLED: name = _("Disabled"); ਰੋਕੋ;
		ਹਾਲਤ MOKS_AFTER_T_TE: name = _("Cleared"); ਰੋਕੋ;
	    }

	    ਅੱਖਰ buf[200];
	    vim_snprintf(buf, ਆਕਾਰ(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    ਸੁਨੇਹਾ_puts(buf);
	}

	ਜੇ (kitty_protocol_state != KKPS_INITIAL)
	{
	    ਅੱਖਰ *name = _("Unknown");
	    ਚੋਣ (kitty_protocol_state)
	    {
		ਹਾਲਤ KKPS_INITIAL: ਰੋਕੋ;
		ਹਾਲਤ KKPS_OFF: name = _("Off"); ਰੋਕੋ;
		ਹਾਲਤ KKPS_ENABLED: name = _("On"); ਰੋਕੋ;
		ਹਾਲਤ KKPS_DISABLED: name = _("Disabled"); ਰੋਕੋ;
		ਹਾਲਤ KKPS_AFTER_T_TE: name = _("Cleared"); ਰੋਕੋ;
	    }

	    ਅੱਖਰ buf[200];
	    vim_snprintf(buf, ਆਕਾਰ(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    ਸੁਨੇਹਾ_puts(buf);
	}
    }

    // need to loop over all global hash lists
    ਲਈ (ਪੂਰਨ hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	ਜੇ (abbrev)
	{
	    ਜੇ (hash != 0)	// there is only one abbreviation list
		ਰੋਕੋ;
	    mp = curbuf->b_ਪਹਿਲਾ_abbr;
	}
	ਨਹੀਂਤਾਂ
	    mp = curbuf->b_maphash[hash];
	ਲਈ ( ; mp != ਨਿਰਾ && !got_int; mp = mp->m_ਅਗਲਾ)
	{
	    // check entries with the same mode
	    ਜੇ (!mp->m_simplified && (mp->m_ਬਹੁਲਕ & ਬਹੁਲਕ) != 0)
	    {
		ਜੇ (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਪੂਰਨ n = mp->m_keylen;
		    ਜੇ (STRNCMP(mp->m_ਕੁੰਜੀਆਂ, ਕੁੰਜੀਆਂ,
				   (size_t)(n < ਕੁੰਜੀਆਂ_len ? n : ਕੁੰਜੀਆਂ_len)) == 0)
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
    ਪੂਰਨ
do_map(
    ਪੂਰਨ		maptype,
    char_u	*arg,
    ਪੂਰਨ		ਬਹੁਲਕ,
    ਪੂਰਨ		abbrev)		// not a mapping but an abbreviation
{
    char_u	*ਕੁੰਜੀਆਂ;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_ਨਤੀਜਾ[2] = {ਨਿਰਾ, ਨਿਰਾ};
    char_u	*rhs;
    char_u	*p;
    ਪੂਰਨ		n;
    ਪੂਰਨ		len = 0;	// init for GCC
    ਪੂਰਨ		hasarg;
    ਪੂਰਨ		haskey;
    ਪੂਰਨ		do_print;
    ਪੂਰਨ		keyround;
    char_u	*ਕੁੰਜੀਆਂ_buf = ਨਿਰਾ;
    char_u	*alt_ਕੁੰਜੀਆਂ_buf = ਨਿਰਾ;
    char_u	*arg_buf = ਨਿਰਾ;
    ਪੂਰਨ		retval = 0;
    ਪੂਰਨ		do_backslash;
    mapblock_T	**abbr_ਸਾਰਣੀ;
    mapblock_T	**map_ਸਾਰਣੀ;
    ਪੂਰਨ		unique = FALSE;
    ਪੂਰਨ		nowait = FALSE;
    ਪੂਰਨ		silent = FALSE;
    ਪੂਰਨ		special = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		ਵਿਅੰਜਕ = FALSE;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		did_simplify = FALSE;
    ਪੂਰਨ		unmap_lhs_only = FALSE;
    ਪੂਰਨ		noremap;
    char_u      *orig_rhs;

    ਕੁੰਜੀਆਂ = arg;
    map_ਸਾਰਣੀ = maphash;
    abbr_ਸਾਰਣੀ = &ਪਹਿਲਾ_abbr;

    ਜੇ (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    ਜੇ (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    ਨਹੀਂਤਾਂ
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    ਲਈ (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	ਜੇ (STRNCMP(ਕੁੰਜੀਆਂ, "<buffer>", 8) == 0)
	{
	    ਕੁੰਜੀਆਂ = skipwhite(ਕੁੰਜੀਆਂ + 8);
	    map_ਸਾਰਣੀ = curbuf->b_maphash;
	    abbr_ਸਾਰਣੀ = &curbuf->b_ਪਹਿਲਾ_abbr;
	    ਜਾਰੀ;
	}

	// Check for "<nowait>": don't wait for more characters.
	ਜੇ (STRNCMP(ਕੁੰਜੀਆਂ, "<nowait>", 8) == 0)
	{
	    ਕੁੰਜੀਆਂ = skipwhite(ਕੁੰਜੀਆਂ + 8);
	    nowait = TRUE;
	    ਜਾਰੀ;
	}

	// Check for "<silent>": don't echo commands.
	ਜੇ (STRNCMP(ਕੁੰਜੀਆਂ, "<silent>", 8) == 0)
	{
	    ਕੁੰਜੀਆਂ = skipwhite(ਕੁੰਜੀਆਂ + 8);
	    silent = TRUE;
	    ਜਾਰੀ;
	}

	// Check for "<special>": accept special keys in <>
	ਜੇ (STRNCMP(ਕੁੰਜੀਆਂ, "<special>", 9) == 0)
	{
	    ਕੁੰਜੀਆਂ = skipwhite(ਕੁੰਜੀਆਂ + 9);
	    special = TRUE;
	    ਜਾਰੀ;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	ਜੇ (STRNCMP(ਕੁੰਜੀਆਂ, "<script>", 8) == 0)
	{
	    ਕੁੰਜੀਆਂ = skipwhite(ਕੁੰਜੀਆਂ + 8);
	    noremap = REMAP_SCRIPT;
	    ਜਾਰੀ;
	}

	// Check for "<expr>": {rhs} is an expression.
	ਜੇ (STRNCMP(ਕੁੰਜੀਆਂ, "<expr>", 6) == 0)
	{
	    ਕੁੰਜੀਆਂ = skipwhite(ਕੁੰਜੀਆਂ + 6);
	    ਵਿਅੰਜਕ = TRUE;
	    ਜਾਰੀ;
	}
#ਜੇ_ਅੰਤ
	// Check for "<unique>": don't overwrite an existing mapping.
	ਜੇ (STRNCMP(ਕੁੰਜੀਆਂ, "<unique>", 8) == 0)
	{
	    ਕੁੰਜੀਆਂ = skipwhite(ਕੁੰਜੀਆਂ + 8);
	    unique = TRUE;
	    ਜਾਰੀ;
	}
	ਰੋਕੋ;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = ਕੁੰਜੀਆਂ;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ਨਿਰਾ);
    ਜਦੋਂ (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	ਜੇ ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    ਜੇ (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*ਕੁੰਜੀਆਂ != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    ਜੇ (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	ਜਾਓ theend;
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
    ਜੇ (haskey)
    {
	char_u	*new_ਕੁੰਜੀਆਂ;
	ਪੂਰਨ	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	ਜੇ (special)
	    flags |= REPTERM_SPECIAL;
	new_ਕੁੰਜੀਆਂ = replace_termcodes(ਕੁੰਜੀਆਂ, &ਕੁੰਜੀਆਂ_buf, 0, flags, &did_simplify);
	ਜੇ (did_simplify)
	    (ਖਾਲੀ)replace_termcodes(ਕੁੰਜੀਆਂ, &alt_ਕੁੰਜੀਆਂ_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, ਨਿਰਾ);
	ਕੁੰਜੀਆਂ = new_ਕੁੰਜੀਆਂ;
    }
    orig_rhs = rhs;
    ਜੇ (hasarg)
    {
	ਜੇ (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	ਨਹੀਂਤਾਂ
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ਨਿਰਾ);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    ਲਈ (keyround = 1; keyround <= 2; ++keyround)
    {
	ਪੂਰਨ	did_it = FALSE;
	ਪੂਰਨ	did_local = FALSE;
	ਪੂਰਨ	keyround1_simplified = keyround == 1 && did_simplify;
	ਪੂਰਨ	ਗੇੜਾ;
	ਪੂਰਨ	ਅੰਕ_ਗੇੜ;

	ਜੇ (keyround == 2)
	{
	    ਜੇ (alt_ਕੁੰਜੀਆਂ_buf == ਨਿਰਾ)
		ਰੋਕੋ;
	    ਕੁੰਜੀਆਂ = alt_ਕੁੰਜੀਆਂ_buf;
	}
	ਨਹੀਂਤਾਂ ਜੇ (alt_ਕੁੰਜੀਆਂ_buf != ਨਿਰਾ && do_print)
	    // when printing always use the not-simplified map
	    ਕੁੰਜੀਆਂ = alt_ਕੁੰਜੀਆਂ_buf;

	// check arguments and translate function keys
	ਜੇ (haskey)
	{
	    len = (ਪੂਰਨ)STRLEN(ਕੁੰਜੀਆਂ);
	    ਜੇ (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		ਜਾਓ theend;
	    }

	    ਜੇ (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		ਜੇ (has_mbyte)
		{
		    ਪੂਰਨ		ਪਹਿਲਾ, ਆਖਰੀ;
		    ਪੂਰਨ		same = -1;
		    char_u	ਕੁੰਜੀਆਂ_unescaped[MAXMAPLEN + 1];
		    size_t	ਕੁੰਜੀਆਂ_unescaped_len;

		    mch_memmove(ਕੁੰਜੀਆਂ_unescaped, ਕੁੰਜੀਆਂ, (size_t)(len + 1));
		    ਕੁੰਜੀਆਂ_unescaped_len = vim_unescape_csi(ਕੁੰਜੀਆਂ_unescaped);
		    p = ਕੁੰਜੀਆਂ_unescaped;

		    ਪਹਿਲਾ = vim_iswordp(p);
		    ਆਖਰੀ = ਪਹਿਲਾ;
		    MB_PTR_ADV(p);
		    n = 1;
		    ਜਦੋਂ (p < ਕੁੰਜੀਆਂ_unescaped + ਕੁੰਜੀਆਂ_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			ਆਖਰੀ = vim_iswordp(p);	// type of last char
			ਜੇ (same == -1 && ਆਖਰੀ != ਪਹਿਲਾ)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    ਜੇ (ਆਖਰੀ && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			ਜਾਓ theend;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (vim_iswordc(ਕੁੰਜੀਆਂ[len - 1]))
		    // ends in keyword char
		    ਲਈ (n = 0; n < len - 2; ++n)
			ਜੇ (vim_iswordc(ਕੁੰਜੀਆਂ[n]) != vim_iswordc(ਕੁੰਜੀਆਂ[len - 2]))
			{
			    retval = 1;
			    ਜਾਓ theend;
			}
		// An abbreviation cannot contain white space.
		ਲਈ (n = 0; n < len; ++n)
		    ਜੇ (VIM_ISWHITE(ਕੁੰਜੀਆਂ[n]))
		    {
			retval = 1;
			ਜਾਓ theend;
		    }
	    }
	}

	ਜੇ (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	ਜੇ (do_print)
	    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();

	// Check if a new local mapping wasn't already defined globally.
	ਜੇ (unique && map_ਸਾਰਣੀ == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    ਲਈ (ਪੂਰਨ hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ਜੇ (abbrev)
		{
		    ਜੇ (hash != 0)	// there is only one abbreviation list
			ਰੋਕੋ;
		    mp = ਪਹਿਲਾ_abbr;
		}
		ਨਹੀਂਤਾਂ
		    mp = maphash[hash];
		ਲਈ ( ; mp != ਨਿਰਾ && !got_int; mp = mp->m_ਅਗਲਾ)
		{
		    // check entries with the same mode
		    ਜੇ ((mp->m_ਬਹੁਲਕ & ਬਹੁਲਕ) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_ਕੁੰਜੀਆਂ, ਕੁੰਜੀਆਂ, (size_t)len) == 0)
		    {
			ਜੇ (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_ਕੁੰਜੀਆਂ);
			ਨਹੀਂਤਾਂ
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_ਕੁੰਜੀਆਂ);
			retval = 5;
			ਜਾਓ theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	ਜੇ (map_ਸਾਰਣੀ != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, ਕੁੰਜੀਆਂ, len,
							     ਬਹੁਲਕ, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	ਅੰਕ_ਗੇੜ = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	ਲਈ (ਗੇੜਾ = 0; ਗੇੜਾ < ਅੰਕ_ਗੇੜ && !did_it && !got_int; ++ਗੇੜਾ)
	{
	    // need to loop over all hash lists
	    ਲਈ (ਪੂਰਨ hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ਜੇ (abbrev)
		{
		    ਜੇ (hash > 0)	// there is only one abbreviation list
			ਰੋਕੋ;
		    mpp = abbr_ਸਾਰਣੀ;
		}
		ਨਹੀਂਤਾਂ
		    mpp = &(map_ਸਾਰਣੀ[hash]);
		ਲਈ (mp = *mpp; mp != ਨਿਰਾ && !got_int; mp = *mpp)
		{

		    ਜੇ ((mp->m_ਬਹੁਲਕ & ਬਹੁਲਕ) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_ਅਗਲਾ);
			ਜਾਰੀ;
		    }
		    ਜੇ (!haskey)	// show all entries
		    {
			ਜੇ (!mp->m_simplified)
			{
			    showmap(mp, map_ਸਾਰਣੀ != maphash);
			    did_it = TRUE;
			}
		    }
		    ਨਹੀਂਤਾਂ	// do we have a match?
		    {
			ਜੇ (ਗੇੜਾ)	// second round: Try unmap "rhs" string
			{
			    n = (ਪੂਰਨ)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			ਨਹੀਂਤਾਂ
			{
			    n = mp->m_keylen;
			    p = mp->m_ਕੁੰਜੀਆਂ;
			}
			ਜੇ (STRNCMP(p, ਕੁੰਜੀਆਂ, (size_t)(n < len ? n : len)) == 0)
			{
			    ਜੇ (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				ਜੇ (n != len && (!abbrev || ਗੇੜਾ || n > len
					       || *skipwhite(ਕੁੰਜੀਆਂ + n) != NUL))
				{
				    mpp = &(mp->m_ਅਗਲਾ);
				    ਜਾਰੀ;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				ਜੇ (keyround1_simplified && !mp->m_simplified)
				    ਰੋਕੋ;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_ਬਹੁਲਕ &= ~ਬਹੁਲਕ;
				did_it = TRUE;	// remember we did something
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (!hasarg)	// show matching entry
			    {
				ਜੇ (!mp->m_simplified)
				{
				    showmap(mp, map_ਸਾਰਣੀ != maphash);
				    did_it = TRUE;
				}
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_ਅਗਲਾ);
				ਜਾਰੀ;
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (unique)
			    {
				ਜੇ (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				ਨਹੀਂਤਾਂ
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				ਜਾਓ theend;
			    }
			    ਨਹੀਂਤਾਂ
			    {
				// new rhs for existing entry
				mp->m_ਬਹੁਲਕ &= ~ਬਹੁਲਕ;	// remove mode bits
				ਜੇ (mp->m_ਬਹੁਲਕ == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    ਜੇ (newstr == ਨਿਰਾ)
				    {
					retval = 4;		// no mem
					ਜਾਓ theend;
				    }
				    ਜੇ (mp->m_alt != ਨਿਰਾ)
					mp->m_alt = mp->m_alt->m_alt = ਨਿਰਾ;
				    vim_free(mp->m_str);
				    mp->m_str = newstr;
				    vim_free(mp->m_orig_str);
				    mp->m_orig_str = vim_strsave(orig_rhs);
				    mp->m_noremap = noremap;
				    mp->m_nowait = nowait;
				    mp->m_silent = silent;
				    mp->m_ਬਹੁਲਕ = ਬਹੁਲਕ;
				    mp->m_simplified = keyround1_simplified;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
				    mp->m_ਵਿਅੰਜਕ = ਵਿਅੰਜਕ;
				    mp->m_script_ctx = current_sctx;
				    ਜੇ (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_ਸੰਸਕਰਣ = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#ਜੇ_ਅੰਤ
				    mp_ਨਤੀਜਾ[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    ਜੇ (mp->m_ਬਹੁਲਕ == 0)  // entry can be deleted
			    {
				map_free(mpp);
				ਜਾਰੀ;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    ਪੂਰਨ new_hash = MAP_HASH(mp->m_ਬਹੁਲਕ, mp->m_ਕੁੰਜੀਆਂ[0]);
			    ਜੇ (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_ਅਗਲਾ;
				mp->m_ਅਗਲਾ = map_ਸਾਰਣੀ[new_hash];
				map_ਸਾਰਣੀ[new_hash] = mp;

				ਜਾਰੀ;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_ਅਗਲਾ);
		}
	    }
	}

	ਜੇ (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    ਜੇ (!did_it)
	    {
		ਜੇ (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*ਕੁੰਜੀਆਂ == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		ਜੇ (map_ਸਾਰਣੀ == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~ਬਹੁਲਕ;
		ਨਹੀਂਤਾਂ
		    mapped_ctrl_c &= ~ਬਹੁਲਕ;
	    }
	    ਜਾਰੀ;
	}

	ਜੇ (!haskey || !hasarg)
	{
	    // print entries
	    ਜੇ (!did_it && !did_local)
	    {
		ਜੇ (abbrev)
		    ਸੁਨੇਹਾ(_("No abbreviation found"));
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ(_("No mapping found"));
	    }
	    ਜਾਓ theend;    // listing finished
	}

	ਜੇ (did_it)
	    ਜਾਰੀ;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_ਨਤੀਜਾ[keyround - 1] = map_ਜੋੜੋ(map_ਸਾਰਣੀ, abbr_ਸਾਰਣੀ, ਕੁੰਜੀਆਂ,
		    rhs, orig_rhs, noremap, nowait, silent, ਬਹੁਲਕ, abbrev,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    ਵਿਅੰਜਕ, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#ਜੇ_ਅੰਤ
		    keyround1_simplified);
	ਜੇ (mp_ਨਤੀਜਾ[keyround - 1] == ਨਿਰਾ)
	{
	    retval = 4;	    // no mem
	    ਜਾਓ theend;
	}
    }

    ਜੇ (mp_ਨਤੀਜਾ[0] != ਨਿਰਾ && mp_ਨਤੀਜਾ[1] != ਨਿਰਾ)
    {
	mp_ਨਤੀਜਾ[0]->m_alt = mp_ਨਤੀਜਾ[1];
	mp_ਨਤੀਜਾ[1]->m_alt = mp_ਨਤੀਜਾ[0];
    }

theend:
    vim_free(ਕੁੰਜੀਆਂ_buf);
    vim_free(alt_ਕੁੰਜੀਆਂ_buf);
    vim_free(arg_buf);
    ਵਾਪਸ retval;
}

/*
 * Get the mapping mode from the command name.
 */
    ਸਥਿਰ ਪੂਰਨ
get_map_ਬਹੁਲਕ(char_u **cmdp, ਪੂਰਨ forceit)
{
    char_u	*p;
    ਪੂਰਨ		modec;
    ਪੂਰਨ		ਬਹੁਲਕ;

    p = *cmdp;
    modec = *p++;
    ਜੇ (modec == 'i')
	ਬਹੁਲਕ = ਬਹੁਲਕ_ਪਾਓ;				// :imap
    ਨਹੀਂਤਾਂ ਜੇ (modec == 'l')
	ਬਹੁਲਕ = ਬਹੁਲਕ_LANGMAP;				// :lmap
    ਨਹੀਂਤਾਂ ਜੇ (modec == 'c')
	ਬਹੁਲਕ = ਬਹੁਲਕ_CMDLINE;				// :cmap
    ਨਹੀਂਤਾਂ ਜੇ (modec == 'n' && *p != 'o')		    // avoid :noremap
	ਬਹੁਲਕ = ਬਹੁਲਕ_ਸਾਧਾਰਨ;				// :nmap
    ਨਹੀਂਤਾਂ ਜੇ (modec == 'v')
	ਬਹੁਲਕ = ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT;		// :vmap
    ਨਹੀਂਤਾਂ ਜੇ (modec == 'x')
	ਬਹੁਲਕ = ਬਹੁਲਕ_VISUAL;				// :xmap
    ਨਹੀਂਤਾਂ ਜੇ (modec == 's')
	ਬਹੁਲਕ = ਬਹੁਲਕ_SELECT;			// :smap
    ਨਹੀਂਤਾਂ ਜੇ (modec == 'o')
	ਬਹੁਲਕ = ਬਹੁਲਕ_OP_PENDING;			// :omap
    ਨਹੀਂਤਾਂ ਜੇ (modec == 't')
	ਬਹੁਲਕ = ਬਹੁਲਕ_TERMINAL;			// :tmap
    ਨਹੀਂਤਾਂ
    {
	--p;
	ਜੇ (forceit)
	    ਬਹੁਲਕ = ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE;		// :map !
	ਨਹੀਂਤਾਂ
	    ਬਹੁਲਕ = ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT | ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_OP_PENDING;
							// :map
    }

    *cmdp = p;
    ਵਾਪਸ ਬਹੁਲਕ;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    ਸਥਿਰ ਖਾਲੀ
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    ਪੂਰਨ		forceit,
    ਪੂਰਨ		abbr)
{
    ਪੂਰਨ		ਬਹੁਲਕ;
    ਪੂਰਨ		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    ਜੇ (!local && *arg != NUL)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ;
    }

    ਬਹੁਲਕ = get_map_ਬਹੁਲਕ(&cmdp, forceit);
    map_clear_ਬਹੁਲਕ(curbuf, ਬਹੁਲਕ, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ਸਥਿਰ ਪੂਰਨ
is_map_locked(ਖਾਲੀ)
{
    ਜੇ (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    ਖਾਲੀ
map_clear_ਬਹੁਲਕ(
    buf_T	*buf,		// buffer for local mappings
    ਪੂਰਨ		ਬਹੁਲਕ,		// mode in which to delete
    ਪੂਰਨ		local,		// TRUE for buffer-local mappings
    ਪੂਰਨ		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    ਪੂਰਨ		hash;
    ਪੂਰਨ		new_hash;

    ਜੇ (is_map_locked())
	ਵਾਪਸ;

    validate_maphash();

    ਲਈ (hash = 0; hash < 256; ++hash)
    {
	ਜੇ (abbr)
	{
	    ਜੇ (hash > 0)	// there is only one abbrlist
		ਰੋਕੋ;
	    ਜੇ (local)
		mpp = &buf->b_ਪਹਿਲਾ_abbr;
	    ਨਹੀਂਤਾਂ
		mpp = &ਪਹਿਲਾ_abbr;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (local)
		mpp = &buf->b_maphash[hash];
	    ਨਹੀਂਤਾਂ
		mpp = &maphash[hash];
	}
	ਜਦੋਂ (*mpp != ਨਿਰਾ)
	{
	    mp = *mpp;
	    ਜੇ (mp->m_ਬਹੁਲਕ & ਬਹੁਲਕ)
	    {
		mp->m_ਬਹੁਲਕ &= ~ਬਹੁਲਕ;
		ਜੇ (mp->m_ਬਹੁਲਕ == 0) // entry can be deleted
		{
		    map_free(mpp);
		    ਜਾਰੀ;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_ਬਹੁਲਕ, mp->m_ਕੁੰਜੀਆਂ[0]);
		ਜੇ (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_ਅਗਲਾ;
		    ਜੇ (local)
		    {
			mp->m_ਅਗਲਾ = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			mp->m_ਅਗਲਾ = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    ਜਾਰੀ;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_ਅਗਲਾ);
	}
    }
}

#ਜੇ defined(FEAT_EVAL)
    ਪੂਰਨ
ਬਹੁਲਕ_str2flags(char_u *modechars)
{
    ਪੂਰਨ		ਬਹੁਲਕ = 0;

    ਜੇ (vim_strchr(modechars, 'n') != ਨਿਰਾ)
	ਬਹੁਲਕ |= ਬਹੁਲਕ_ਸਾਧਾਰਨ;
    ਜੇ (vim_strchr(modechars, 'v') != ਨਿਰਾ)
	ਬਹੁਲਕ |= ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT;
    ਜੇ (vim_strchr(modechars, 'x') != ਨਿਰਾ)
	ਬਹੁਲਕ |= ਬਹੁਲਕ_VISUAL;
    ਜੇ (vim_strchr(modechars, 's') != ਨਿਰਾ)
	ਬਹੁਲਕ |= ਬਹੁਲਕ_SELECT;
    ਜੇ (vim_strchr(modechars, 'o') != ਨਿਰਾ)
	ਬਹੁਲਕ |= ਬਹੁਲਕ_OP_PENDING;
    ਜੇ (vim_strchr(modechars, 'i') != ਨਿਰਾ)
	ਬਹੁਲਕ |= ਬਹੁਲਕ_ਪਾਓ;
    ਜੇ (vim_strchr(modechars, 'l') != ਨਿਰਾ)
	ਬਹੁਲਕ |= ਬਹੁਲਕ_LANGMAP;
    ਜੇ (vim_strchr(modechars, 'c') != ਨਿਰਾ)
	ਬਹੁਲਕ |= ਬਹੁਲਕ_CMDLINE;

    ਵਾਪਸ ਬਹੁਲਕ;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    ਪੂਰਨ
map_ਮੰਜ਼ਿਲ_exists(char_u *str, char_u *modechars, ਪੂਰਨ abbr)
{
    char_u	*rhs;
    char_u	*buf;
    ਪੂਰਨ		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, ਨਿਰਾ);

    retval = map_ਮੰਜ਼ਿਲ_exists_ਬਹੁਲਕ(rhs, ਬਹੁਲਕ_str2flags(modechars), abbr);
    vim_free(buf);

    ਵਾਪਸ retval;
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    ਪੂਰਨ
map_ਮੰਜ਼ਿਲ_exists_ਬਹੁਲਕ(char_u *rhs, ਪੂਰਨ ਬਹੁਲਕ, ਪੂਰਨ abbr)
{
    mapblock_T	*mp;
    ਪੂਰਨ		hash;
    ਪੂਰਨ		exp_ਬਫਰ = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    ਲਈ (;;)
    {
	ਲਈ (hash = 0; hash < 256; ++hash)
	{
	    ਜੇ (abbr)
	    {
		ਜੇ (hash > 0)		// there is only one abbr list
		    ਰੋਕੋ;
		ਜੇ (exp_ਬਫਰ)
		    mp = curbuf->b_ਪਹਿਲਾ_abbr;
		ਨਹੀਂਤਾਂ
		    mp = ਪਹਿਲਾ_abbr;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (exp_ਬਫਰ)
		mp = curbuf->b_maphash[hash];
	    ਨਹੀਂਤਾਂ
		mp = maphash[hash];
	    ਲਈ (; mp; mp = mp->m_ਅਗਲਾ)
	    {
		ਜੇ ((mp->m_ਬਹੁਲਕ & ਬਹੁਲਕ)
			&& ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)mp->m_str, (ਅੱਖਰ *)rhs) != ਨਿਰਾ)
		    ਵਾਪਸ TRUE;
	    }
	}
	ਜੇ (exp_ਬਫਰ)
	    ਰੋਕੋ;
	exp_ਬਫਰ = TRUE;
    }

    ਵਾਪਸ FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
ਸਥਿਰ ਪੂਰਨ	expand_mapmodes = 0;
ਸਥਿਰ ਪੂਰਨ	expand_isabbrev = 0;
ਸਥਿਰ ਪੂਰਨ	expand_ਬਫਰ = FALSE;

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
    ਸਥਿਰ char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    ਪੂਰਨ		c;
    ਪੂਰਨ		modifiers;
    ਪੂਰਨ		cpo_bslash;
    ਪੂਰਨ		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != ਨਿਰਾ);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != ਨਿਰਾ);

    ਲਈ (; *str; ++str)
    {
	c = *str;
	ਜੇ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    ਜੇ (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    ਜੇ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		ਜੇ (cpo_special)
		{
		    ga_clear(&ga);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		c = ਮੰਜ਼ਿਲ_SPECIAL(str[1], str[2]);
		ਜੇ (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    ਜੇ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ਜੇ (cpo_special)
		{
		    ga_clear(&ga);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		ga_concat(&ga, get_special_ਕੁੰਜੀ_name(c, modifiers));
		ਜਾਰੀ; // for (str)
	    }
	}
	ਜੇ (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	ਜੇ (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    ਵਾਪਸ (char_u *)(ga.ga_ਅੰਕੜੇ);
}

/*
 * Work out what to complete when doing command line completion of mapping
 * or abbreviation names.
 */
    char_u *
ਸੈੱਟ_context_ਵਿੱਚ_map_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    ਪੂਰਨ		forceit,	// TRUE if '!' given
    ਪੂਰਨ		isabbrev,	// TRUE if abbreviation
    ਪੂਰਨ		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    ਜੇ (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (isunmap)
	    expand_mapmodes = get_map_ਬਹੁਲਕ(&cmd, forceit || isabbrev);
	ਨਹੀਂਤਾਂ
	{
	    expand_mapmodes = ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE;
	    ਜੇ (!isabbrev)
		expand_mapmodes += ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT | ਬਹੁਲਕ_ਸਾਧਾਰਨ
							     | ਬਹੁਲਕ_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_ਬਫਰ = FALSE;
	ਲਈ (;;)
	{
	    ਜੇ (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_ਬਫਰ = TRUE;
		arg = skipwhite(arg + 8);
		ਜਾਰੀ;
	    }
	    ਜੇ (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ਜਾਰੀ;
	    }
	    ਜੇ (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ਜਾਰੀ;
	    }
	    ਜੇ (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ਜਾਰੀ;
	    }
	    ਜੇ (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		ਜਾਰੀ;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ਜਾਰੀ;
	    }
	    ਜੇ (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		ਜਾਰੀ;
	    }
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	}
	xp->xp_ਨਮੂਨਾ = arg;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    ਪੂਰਨ
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    ਪੂਰਨ		*ਅੰਕ_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    ਪੂਰਨ		hash;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    char_u	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		fuzzy;
    ਪੂਰਨ		ਮੇਲ;
    ਪੂਰਨ		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *ਅੰਕ_Matches = 0;		    // return values in case of FAIL
    *matches = ਨਿਰਾ;

    ਜੇ (!fuzzy)
	ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ *), 3);
    ਨਹੀਂਤਾਂ
	ga_init2(&ga, ਆਕਾਰ(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    ਲਈ (i = 0; i < 7; ++i)
    {
	ਜੇ (i == 0)
	    p = (char_u *)"<silent>";
	ਨਹੀਂਤਾਂ ਜੇ (i == 1)
	    p = (char_u *)"<unique>";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਨਹੀਂਤਾਂ ਜੇ (i == 2)
	    p = (char_u *)"<script>";
	ਨਹੀਂਤਾਂ ਜੇ (i == 3)
	    p = (char_u *)"<expr>";
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ ਜੇ (i == 4 && !expand_ਬਫਰ)
	    p = (char_u *)"<buffer>";
	ਨਹੀਂਤਾਂ ਜੇ (i == 5)
	    p = (char_u *)"<nowait>";
	ਨਹੀਂਤਾਂ ਜੇ (i == 6)
	    p = (char_u *)"<special>";
	ਨਹੀਂਤਾਂ
	    ਜਾਰੀ;

	ਜੇ (!fuzzy)
	    ਮੇਲ = vim_regexec(regmatch, p, (colnr_T)0);
	ਨਹੀਂਤਾਂ
	{
	    score = fuzzy_ਮੇਲ_str(p, pat);
	    ਮੇਲ = (score != FUZZY_SCORE_NONE);
	}

	ਜੇ (!ਮੇਲ)
	    ਜਾਰੀ;

	ਜੇ (ga_grow(&ga, 1) == FAIL)
	    ਰੋਕੋ;

	ਜੇ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_ਅੰਕੜੇ)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	ਨਹੀਂਤਾਂ
	    ((char_u **)ga.ga_ਅੰਕੜੇ)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    ਲਈ (hash = 0; hash < 256; ++hash)
    {
	ਜੇ (expand_isabbrev)
	{
	    ਜੇ (hash > 0)	// only one abbrev list
		ਰੋਕੋ; // for (hash)
	    mp = ਪਹਿਲਾ_abbr;
	}
	ਨਹੀਂਤਾਂ ਜੇ (expand_ਬਫਰ)
	    mp = curbuf->b_maphash[hash];
	ਨਹੀਂਤਾਂ
	    mp = maphash[hash];
	ਲਈ (; mp; mp = mp->m_ਅਗਲਾ)
	{
	    ਜੇ (mp->m_simplified || !(mp->m_ਬਹੁਲਕ & expand_mapmodes))
		ਜਾਰੀ;

	    p = translate_mapping(mp->m_ਕੁੰਜੀਆਂ);
	    ਜੇ (p == ਨਿਰਾ)
		ਜਾਰੀ;

	    ਜੇ (!fuzzy)
		ਮੇਲ = vim_regexec(regmatch, p, (colnr_T)0);
	    ਨਹੀਂਤਾਂ
	    {
		score = fuzzy_ਮੇਲ_str(p, pat);
		ਮੇਲ = (score != FUZZY_SCORE_NONE);
	    }

	    ਜੇ (!ਮੇਲ)
	    {
		vim_free(p);
		ਜਾਰੀ;
	    }

	    ਜੇ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		ਰੋਕੋ;
	    }

	    ਜੇ (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_ਅੰਕੜੇ)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ਨਹੀਂਤਾਂ
		((char_u **)ga.ga_ਅੰਕੜੇ)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    ਜੇ (ga.ga_len == 0)
	ਵਾਪਸ FAIL;

    ਜੇ (!fuzzy)
    {
	*matches = ga.ga_ਅੰਕੜੇ;
	*ਅੰਕ_Matches = ga.ga_len;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (fuzzymatches_ਮੰਜ਼ਿਲ_strmatches(ga.ga_ਅੰਕੜੇ, matches, ga.ga_len,
							FALSE) == FAIL)
	    ਵਾਪਸ FAIL;
	*ਅੰਕ_Matches = ga.ga_len;
    }

    ਗਿਣਤੀ_2 = *ਅੰਕ_Matches;
    ਜੇ (ਗਿਣਤੀ_2 > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	ਜੇ (!fuzzy)
	    ਛਾਂਟੋ_ਲੜੀਆਂ(*matches, ਗਿਣਤੀ_2);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + ਗਿਣਤੀ_2;

	ਜਦੋਂ (ptr2 < ptr3)
	{
	    ਜੇ (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    ਨਹੀਂਤਾਂ
	    {
		vim_free(*ptr2++);
		ਗਿਣਤੀ_2--;
	    }
	}
    }

    *ਅੰਕ_Matches = ਗਿਣਤੀ_2;
    ਵਾਪਸ (ਗਿਣਤੀ_2 == 0 ? FAIL : OK);
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
    ਪੂਰਨ
check_abbr(
    ਪੂਰਨ		c,
    char_u	*ptr,
    ਪੂਰਨ		ਖਾਨਾ,
    ਪੂਰਨ		mincol)
{
    ਪੂਰਨ		len;
    ਪੂਰਨ		scol;		// starting column of the abbr.
    ਪੂਰਨ		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    ਪੂਰਨ		clen = 0;	// length in characters
    ਪੂਰਨ		is_id = TRUE;
    ਪੂਰਨ		vim_abbr;

    ਜੇ (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	ਵਾਪਸ FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    ਜੇ (noremap_ਕੁੰਜੀਆਂ() && c != Ctrl_RSB)
	ਵਾਪਸ FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    ਜੇ (ਖਾਨਾ == 0)				// cannot be an abbr.
	ਵਾਪਸ FALSE;

    ਜੇ (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + ਖਾਨਾ);
	ਜੇ (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	ਨਹੀਂਤਾਂ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ਜੇ (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	ਜਦੋਂ (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    ਜੇ (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		ਰੋਕੋ;
	    }
	    ++clen;
	}
	scol = (ਪੂਰਨ)(p - ptr);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!vim_iswordc(ptr[ਖਾਨਾ - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	ਨਹੀਂਤਾਂ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ਜੇ (ਖਾਨਾ > 1)
		is_id = vim_iswordc(ptr[ਖਾਨਾ - 2]);
	}
	ਲਈ (scol = ਖਾਨਾ - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    ਜੇ (scol < mincol)
	scol = mincol;
    ਜੇ (scol < ਖਾਨਾ)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = ਖਾਨਾ - scol;
	mp = curbuf->b_ਪਹਿਲਾ_abbr;
	mp2 = ਪਹਿਲਾ_abbr;
	ਜੇ (mp == ਨਿਰਾ)
	{
	    mp = mp2;
	    mp2 = ਨਿਰਾ;
	}
	ਲਈ ( ; mp; mp->m_ਅਗਲਾ == ਨਿਰਾ
				  ? (mp = mp2, mp2 = ਨਿਰਾ) : (mp = mp->m_ਅਗਲਾ))
	{
	    ਪੂਰਨ		qlen = mp->m_keylen;
	    char_u	*q = mp->m_ਕੁੰਜੀਆਂ;
	    ਪੂਰਨ		ਮੇਲ;

	    ਜੇ (vim_strbyte(mp->m_ਕੁੰਜੀਆਂ, K_SPECIAL) != ਨਿਰਾ)
	    {
		char_u *qe = vim_strsave(mp->m_ਕੁੰਜੀਆਂ);

		// might have CSI escaped mp->m_keys
		ਜੇ (qe != ਨਿਰਾ)
		{
		    q = qe;
		    qlen = (ਪੂਰਨ)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    ਮੇਲ =    (mp->m_ਬਹੁਲਕ & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    ਜੇ (q != mp->m_ਕੁੰਜੀਆਂ)
		vim_free(q);
	    ਜੇ (ਮੇਲ)
		ਰੋਕੋ;
	}
	ਜੇ (mp != ਨਿਰਾ)
	{
	    ਪੂਰਨ	noremap;
	    ਪੂਰਨ silent;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਪੂਰਨ ਵਿਅੰਜਕ;
#ਜੇ_ਅੰਤ

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
	    ਜੇ (c != Ctrl_RSB)
	    {
					// special key code, split up
		ਜੇ (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_ਦੂਜਾ(c);
		    tb[j++] = K_ਤੀਜਾ(c);
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    ਜੇ (has_mbyte)
		    {
			ਪੂਰਨ	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			ਜੇ (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			ਜੇ (escaped != ਨਿਰਾ)
			{
			    newlen = (ਪੂਰਨ)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    ਨਹੀਂਤਾਂ
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(ਖਾਲੀ)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਵਿਅੰਜਕ = mp->m_ਵਿਅੰਜਕ;

	    ਜੇ (ਵਿਅੰਜਕ)
		s = eval_map_ਵਿਅੰਜਕ(mp, c);
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		s = mp->m_str;
	    ਜੇ (s != ਨਿਰਾ)
	    {
					// insert the to string
		(ਖਾਲੀ)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (ਪੂਰਨ)STRLEN(s) + j + 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਜੇ (ਵਿਅੰਜਕ)
		    vim_free(s);
#ਜੇ_ਅੰਤ
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    ਜੇ (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    ਜਦੋਂ (len-- > 0)		// delete the from string
		(ਖਾਲੀ)ins_typebuf(tb, 1, 0, TRUE, silent);
	    ਵਾਪਸ TRUE;
	}
    }
    ਵਾਪਸ FALSE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_ਵਿਅੰਜਕ(
    mapblock_T	*mp,
    ਪੂਰਨ		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*ਵਿਅੰਜਕ;
    pos_T	save_cursor;
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_ਖਾਨਾ;
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_ਪੰਕਤੀ;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    ਪੂਰਨ		save_sctx_ਸੰਸਕਰਣ = current_sctx.sc_ਸੰਸਕਰਣ;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    ਵਿਅੰਜਕ = vim_strsave(mp->m_str);
    ਜੇ (ਵਿਅੰਜਕ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    vim_unescape_csi(ਵਿਅੰਜਕ);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_ਸਾਧਾਰਨ_ਤਾਲਾ;
    ਸੈੱਟ_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_ਸੁਨੇਹਾ_ਖਾਨਾ = ਸੁਨੇਹਾ_ਖਾਨਾ;
    save_ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;

    current_sctx.sc_ਸੰਸਕਰਣ = mp->m_script_ctx.sc_ਸੰਸਕਰਣ;
    ਜੇ (mp->m_script_ctx.sc_ਸੰਸਕਰਣ == SCRIPT_ਸੰਸਕਰਣ_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_ਮੰਜ਼ਿਲ_string(ਵਿਅੰਜਕ, FALSE, FALSE);

    --textlock;
    --ex_ਸਾਧਾਰਨ_ਤਾਲਾ;
    curwin->w_cursor = save_cursor;
    ਸੁਨੇਹਾ_ਖਾਨਾ = save_ਸੁਨੇਹਾ_ਖਾਨਾ;
    ਸੁਨੇਹਾ_ਪੰਕਤੀ = save_ਸੁਨੇਹਾ_ਪੰਕਤੀ;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_ਸੰਸਕਰਣ = save_sctx_ਸੰਸਕਰਣ;

    vim_free(ਵਿਅੰਜਕ);

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    ਵਾਪਸ res;
}
#ਜੇ_ਅੰਤ

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
    ਜੇ (res == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    d = res;
    ਲਈ (s = p; *s != NUL; )
    {
	ਜੇ ((s[0] == K_SPECIAL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    || (gui.ਵਿੱਚ_use && s[0] == CSI)
#ਜੇ_ਅੰਤ
	    ) && s[1] != NUL && s[2] != NUL)
	{
	    // Copy special key unmodified.
	    *d++ = *s++;
	    *d++ = *s++;
	    *d++ = *s++;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = ਜੋੜੋ_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    ਵਾਪਸ res;
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

    ਜਦੋਂ (*s != NUL)
    {
	ਜੇ (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	ਨਹੀਂਤਾਂ ਜੇ ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (ਪੂਰਨ)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	ਨਹੀਂਤਾਂ
	    *d++ = *s++;
    }
    *d = NUL;
    ਵਾਪਸ (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ਪੂਰਨ
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    ਅੱਖਰ	*cmd;
    ਪੂਰਨ		abbr;
    ਪੂਰਨ		hash;
    ਪੂਰਨ		did_cpo = FALSE;
    ਪੂਰਨ		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    ਲਈ (abbr = 0; abbr < 2; ++abbr)
	ਲਈ (hash = 0; hash < 256; ++hash)
	{
	    ਜੇ (abbr)
	    {
		ਜੇ (hash > 0)		// there is only one abbr list
		    ਰੋਕੋ;
		ਜੇ (buf != ਨਿਰਾ)
		    mp = buf->b_ਪਹਿਲਾ_abbr;
		ਨਹੀਂਤਾਂ
		    mp = ਪਹਿਲਾ_abbr;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (buf != ਨਿਰਾ)
		    mp = buf->b_maphash[hash];
		ਨਹੀਂਤਾਂ
		    mp = maphash[hash];
	    }

	    ਲਈ ( ; mp; mp = mp->m_ਅਗਲਾ)
	    {
		// simplified map blocks are not created explicitly
		ਜੇ (mp->m_simplified)
		    ਜਾਰੀ;

		// skip script-local mappings
		ਜੇ (mp->m_noremap == REMAP_SCRIPT)
		    ਜਾਰੀ;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		ਲਈ (p = mp->m_str; *p != NUL; ++p)
		    ਜੇ (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (ਪੂਰਨ)KE_SNR)
			ਰੋਕੋ;
		ਜੇ (*p != NUL)
		    ਜਾਰੀ;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		ਜੇ (abbr)
		    cmd = "abbr";
		ਨਹੀਂਤਾਂ
		    cmd = "map";
		ਚੋਣ (mp->m_ਬਹੁਲਕ)
		{
		    ਹਾਲਤ ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT
							     | ਬਹੁਲਕ_OP_PENDING:
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_ਸਾਧਾਰਨ:
			c1 = 'n';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_VISUAL:
			c1 = 'x';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_SELECT:
			c1 = 's';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_OP_PENDING:
			c1 = 'o';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_VISUAL:
			c1 = 'n';
			c2 = 'x';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_SELECT:
			c1 = 'n';
			c2 = 's';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT:
			c1 = 'v';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_SELECT | ਬਹੁਲਕ_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT:
			c1 = 'n';
			c2 = 'v';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_SELECT | ਬਹੁਲਕ_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT | ਬਹੁਲਕ_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_CMDLINE | ਬਹੁਲਕ_ਪਾਓ:
			ਜੇ (!abbr)
			    cmd = "map!";
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_CMDLINE:
			c1 = 'c';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_ਪਾਓ:
			c1 = 'i';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_LANGMAP:
			c1 = 'l';
			ਰੋਕੋ;
		    ਹਾਲਤ ਬਹੁਲਕ_TERMINAL:
			c1 = 't';
			ਰੋਕੋ;
		    ਮੂਲ:
			iemsg(e_makemap_illegal_ਬਹੁਲਕ);
			ਵਾਪਸ FAIL;
		}
		ਕਰੋ	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    ਜੇ (!did_cpo)
		    {
			ਜੇ (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			ਨਹੀਂਤਾਂ
			    ਲਈ (i = 0; i < 2; ++i)
				ਲਈ (p = (i ? mp->m_str : mp->m_ਕੁੰਜੀਆਂ); *p; ++p)
				    ਜੇ (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			ਜੇ (did_cpo)
			{
			    ਜੇ (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				ਵਾਪਸ FAIL;
			}
		    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    // If it is not vim9 use legacy
		    ਜੇ (mp->m_ਵਿਅੰਜਕ && mp->m_script_ctx.sc_ਸੰਸਕਰਣ < SCRIPT_ਸੰਸਕਰਣ_VIM9
			    && fputs("legacy ", fd) < 0)
			ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
		    ਜੇ (c1 && putc(c1, fd) < 0)
			ਵਾਪਸ FAIL;
		    ਜੇ (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			ਵਾਪਸ FAIL;
		    ਜੇ (fputs(cmd, fd) < 0)
			ਵਾਪਸ FAIL;
		    ਜੇ (buf != ਨਿਰਾ && fputs(" <buffer>", fd) < 0)
			ਵਾਪਸ FAIL;
		    ਜੇ (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			ਵਾਪਸ FAIL;
		    ਜੇ (mp->m_silent && fputs(" <silent>", fd) < 0)
			ਵਾਪਸ FAIL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    ਜੇ (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			ਵਾਪਸ FAIL;
		    ਜੇ (mp->m_ਵਿਅੰਜਕ && fputs(" <expr>", fd) < 0)
			ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ

		    ਜੇ (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_ਕੁੰਜੀਆਂ, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			ਵਾਪਸ FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} ਜਦੋਂ (c1 != NUL);
	    }
	}

    ਜੇ (did_cpo)
	ਜੇ (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
put_escstr(FILE *fd, char_u *strstart, ਪੂਰਨ what)
{
    char_u	*str = strstart;
    ਪੂਰਨ		c;
    ਪੂਰਨ		modifiers;

    // :map xx <Nop>
    ਜੇ (*str == NUL && what == 1)
    {
	ਜੇ (fprintf(fd, "<Nop>") < 0)
	    ਵਾਪਸ FAIL;
	ਵਾਪਸ OK;
    }

    ਲਈ ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਜਦੋਂ (*p != NUL)
		ਜੇ (fputc(*p++, fd) < 0)
		    ਵਾਪਸ FAIL;
	    --str;
	    ਜਾਰੀ;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	ਜੇ (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    ਜੇ (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		ਜੇ (p == ਨਿਰਾ)
		    c = *str;
		ਨਹੀਂਤਾਂ
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    ਜੇ (c == K_SPECIAL)
	    {
		c = ਮੰਜ਼ਿਲ_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    ਜੇ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ਜੇ (fputs((ਅੱਖਰ *)get_special_ਕੁੰਜੀ_name(c, modifiers), fd) < 0)
		    ਵਾਪਸ FAIL;
		ਜਾਰੀ;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	ਜੇ (c == NL)
	{
	    ਜੇ (what == 2)
	    {
		ਜੇ (fprintf(fd, "\\\026\n") < 0)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (fprintf(fd, "<NL>") < 0)
		    ਵਾਪਸ FAIL;
	    }
	    ਜਾਰੀ;
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
	ਜੇ (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    ਜੇ (putc('\\', fd) < 0)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    ਜੇ (putc(Ctrl_V, fd) < 0)
		ਵਾਪਸ FAIL;
	}
	ਜੇ (putc(c, fd) < 0)
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    ਖਾਲੀ
check_map_keycodes(ਖਾਲੀ)
{
    mapblock_T	*mp;
    char_u	*p;
    ਪੂਰਨ		i;
    char_u	buf[3];
    ਪੂਰਨ		abbr;
    ਪੂਰਨ		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_ਧੱਕੋ(ETYPE_ਅੰਦਰੂਨੀ, (char_u *)"mappings", 0);
    ESTACK_CHECK_ਤਿਆਰੀ;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    ਲਈ (bp = firstbuf; ; bp = bp->b_ਅਗਲਾ)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	ਲਈ (abbr = 0; abbr <= 1; ++abbr)
	    ਲਈ (hash = 0; hash < 256; ++hash)
	    {
		ਜੇ (abbr)
		{
		    ਜੇ (hash)	    // there is only one abbr list
			ਰੋਕੋ;
		    ਜੇ (bp != ਨਿਰਾ)
			mp = bp->b_ਪਹਿਲਾ_abbr;
		    ਨਹੀਂਤਾਂ
			mp = ਪਹਿਲਾ_abbr;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (bp != ਨਿਰਾ)
			mp = bp->b_maphash[hash];
		    ਨਹੀਂਤਾਂ
			mp = maphash[hash];
		}
		ਲਈ ( ; mp != ਨਿਰਾ; mp = mp->m_ਅਗਲਾ)
		{
		    ਲਈ (i = 0; i <= 1; ++i)	// do this twice
		    {
			ਜੇ (i == 0)
			    p = mp->m_ਕੁੰਜੀਆਂ;	// once for the "from" part
			ਨਹੀਂਤਾਂ
			    p = mp->m_str;	// and once for the "to" part
			ਜਦੋਂ (*p)
			{
			    ਜੇ (*p == K_SPECIAL)
			    {
				++p;
				ਜੇ (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (ਖਾਲੀ)ਜੋੜੋ_termcap_ਇੰਦਰਾਜ(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	ਜੇ (bp == ਨਿਰਾ)
	    ਰੋਕੋ;
    }
    ESTACK_CHECK_NOW;
    estack_ਕੱਢੋ();
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*ਕੁੰਜੀਆਂ,
    ਪੂਰਨ		ਬਹੁਲਕ,
    ਪੂਰਨ		exact,		// require exact match
    ਪੂਰਨ		ign_mod,	// ignore preceding modifier
    ਪੂਰਨ		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    ਪੂਰਨ		*local_ptr)	// return: buffer-local mapping or NULL
{
    ਪੂਰਨ		hash;
    ਪੂਰਨ		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    ਪੂਰਨ		local;

    validate_maphash();

    len = (ਪੂਰਨ)STRLEN(ਕੁੰਜੀਆਂ);
    ਲਈ (local = 1; local >= 0; --local)
	// loop over all hash lists
	ਲਈ (hash = 0; hash < 256; ++hash)
	{
	    ਜੇ (abbr)
	    {
		ਜੇ (hash > 0)		// there is only one list.
		    ਰੋਕੋ;
		ਜੇ (local)
		    mp = curbuf->b_ਪਹਿਲਾ_abbr;
		ਨਹੀਂਤਾਂ
		    mp = ਪਹਿਲਾ_abbr;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (local)
		mp = curbuf->b_maphash[hash];
	    ਨਹੀਂਤਾਂ
		mp = maphash[hash];
	    ਲਈ ( ; mp != ਨਿਰਾ; mp = mp->m_ਅਗਲਾ)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		ਜੇ ((mp->m_ਬਹੁਲਕ & ਬਹੁਲਕ) && (!exact || mp->m_keylen == len))
		{
		    ਜੇ (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    ਨਹੀਂਤਾਂ
			minlen = len;
		    s = mp->m_ਕੁੰਜੀਆਂ;
		    ਜੇ (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			ਜੇ (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    ਜੇ (STRNCMP(s, ਕੁੰਜੀਆਂ, minlen) == 0)
		    {
			ਜੇ (mp_ptr != ਨਿਰਾ)
			    *mp_ptr = mp;
			ਜੇ (local_ptr != ਨਿਰਾ)
			    *local_ptr = local;
			ਵਾਪਸ mp->m_str;
		    }
		}
	    }
	}

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * "hasmapto()" function
 */
    ਖਾਲੀ
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*ਬਹੁਲਕ;
    char_u	buf[NUMBUFLEN];
    ਪੂਰਨ		abbr = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    name = tv_get_string(&argvars[0]);
    ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
	ਬਹੁਲਕ = (char_u *)"nvo";
    ਨਹੀਂਤਾਂ
    {
	ਬਹੁਲਕ = tv_get_string_buf(&argvars[1], buf);
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (ਪੂਰਨ)tv_get_bool(&argvars[2]);
    }

    ਜੇ (map_ਮੰਜ਼ਿਲ_exists(name, ਬਹੁਲਕ, abbr))
	rettv->vval.v_ਸੰਖਿਆ = TRUE;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    ਸਥਿਰ ਖਾਲੀ
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	ਪੂਰਨ	    ਬਫਰ_local,   // false if not buffer local mapping
	ਪੂਰਨ	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_ਕੁੰਜੀਆਂ, TRUE, FALSE);
    char_u	    *mapmode = map_ਬਹੁਲਕ_ਮੰਜ਼ਿਲ_chars(mp->m_ਬਹੁਲਕ);

    dict_ਜੋੜੋ_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_ਜੋੜੋ_string(dict, "lhsraw", mp->m_ਕੁੰਜੀਆਂ);
    ਜੇ (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_ਜੋੜੋ_string(dict, "lhsrawalt", lhsrawalt);
    dict_ਜੋੜੋ_string(dict, "rhs", mp->m_orig_str);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "expr", mp->m_ਵਿਅੰਜਕ ? 1L : 0L);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "sid", (ਲੰਮਾ)mp->m_script_ctx.sc_sid);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "scriptversion",
		    (ਲੰਮਾ)mp->m_script_ctx.sc_ਸੰਸਕਰਣ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "lnum", (ਲੰਮਾ)mp->m_script_ctx.sc_lnum);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "buffer", (ਲੰਮਾ)ਬਫਰ_local);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_ਜੋੜੋ_string(dict, "mode", mapmode);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "abbr", abbr ? 1L : 0L);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "mode_bits", mp->m_ਬਹੁਲਕ);

    vim_free(mapmode);
}

    ਸਥਿਰ ਖਾਲੀ
get_maparg(typval_T *argvars, typval_T *rettv, ਪੂਰਨ exact)
{
    char_u	*ਕੁੰਜੀਆਂ;
    char_u	*ਕੁੰਜੀਆਂ_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*ਕੁੰਜੀਆਂ_buf = ਨਿਰਾ;
    char_u	*alt_ਕੁੰਜੀਆਂ_buf = ਨਿਰਾ;
    ਪੂਰਨ		did_simplify = FALSE;
    char_u	*rhs;
    ਪੂਰਨ		ਬਹੁਲਕ;
    ਪੂਰਨ		abbr = FALSE;
    ਪੂਰਨ		get_dict = FALSE;
    mapblock_T	*mp = ਨਿਰਾ;
    ਪੂਰਨ		ਬਫਰ_local;
    ਪੂਰਨ		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਕੁੰਜੀਆਂ = tv_get_string(&argvars[0]);
    ਜੇ (*ਕੁੰਜੀਆਂ == NUL)
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (ਪੂਰਨ)tv_get_bool(&argvars[2]);
	    ਜੇ (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (ਪੂਰਨ)tv_get_bool(&argvars[3]);
	}
    }
    ਨਹੀਂਤਾਂ
	which = (char_u *)"";
    ਜੇ (which == ਨਿਰਾ)
	ਵਾਪਸ;

    ਬਹੁਲਕ = get_map_ਬਹੁਲਕ(&which, 0);

    ਕੁੰਜੀਆਂ_simplified = replace_termcodes(ਕੁੰਜੀਆਂ, &ਕੁੰਜੀਆਂ_buf, 0, flags,
								&did_simplify);
    rhs = check_map(ਕੁੰਜੀਆਂ_simplified, ਬਹੁਲਕ, exact, FALSE, abbr,
							   &mp, &ਬਫਰ_local);
    ਜੇ (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(ਖਾਲੀ)replace_termcodes(ਕੁੰਜੀਆਂ, &alt_ਕੁੰਜੀਆਂ_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, ਨਿਰਾ);
	rhs = check_map(alt_ਕੁੰਜੀਆਂ_buf, ਬਹੁਲਕ, exact, FALSE, abbr, &mp,
								&ਬਫਰ_local);
    }

    ਜੇ (!get_dict)
    {
	// Return a string.
	ਜੇ (rhs != ਨਿਰਾ)
	{
	    ਜੇ (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    ਨਹੀਂਤਾਂ
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    ਨਹੀਂਤਾਂ ਜੇ (rettv_dict_alloc(rettv) == OK && rhs != ਨਿਰਾ)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? ਕੁੰਜੀਆਂ_simplified : ਨਿਰਾ,
			  ਬਫਰ_local, abbr);

    vim_free(ਕੁੰਜੀਆਂ_buf);
    vim_free(alt_ਕੁੰਜੀਆਂ_buf);
}

/*
 * "maplist()" function
 */
    ਖਾਲੀ
f_maplist(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    ਪੂਰਨ		ਬਫਰ_local;
    char_u	*ਕੁੰਜੀਆਂ_buf;
    ਪੂਰਨ		did_simplify;
    ਪੂਰਨ		hash;
    char_u	*lhs;
    ਅਚਲ ਪੂਰਨ	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    ਪੂਰਨ		abbr = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;
    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    ਲਈ (ਬਫਰ_local = 0; ਬਫਰ_local <= 1; ++ਬਫਰ_local)
    {
	ਲਈ (hash = 0; hash < 256; ++hash)
	{
	    ਜੇ (abbr)
	    {
		ਜੇ (hash > 0)		// there is only one abbr list
		    ਰੋਕੋ;
		ਜੇ (ਬਫਰ_local)
		    mp = curbuf->b_ਪਹਿਲਾ_abbr;
		ਨਹੀਂਤਾਂ
		    mp = ਪਹਿਲਾ_abbr;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਬਫਰ_local)
		mp = curbuf->b_maphash[hash];
	    ਨਹੀਂਤਾਂ
		mp = maphash[hash];
	    ਲਈ (; mp; mp = mp->m_ਅਗਲਾ)
	    {
		ਜੇ (mp->m_simplified)
		    ਜਾਰੀ;
		ਜੇ ((d = dict_alloc()) == ਨਿਰਾ)
		    ਵਾਪਸ;
		ਜੇ (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    ਵਾਪਸ;
		}

		ਕੁੰਜੀਆਂ_buf = ਨਿਰਾ;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_ਕੁੰਜੀਆਂ, TRUE, FALSE);
		(ਖਾਲੀ)replace_termcodes(lhs, &ਕੁੰਜੀਆਂ_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? ਕੁੰਜੀਆਂ_buf : ਨਿਰਾ,
				 ਬਫਰ_local, abbr);
		vim_free(ਕੁੰਜੀਆਂ_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    ਖਾਲੀ
f_maparg(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		ਵਾਪਸ;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    ਖਾਲੀ
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    ਸਥਿਰ ਪੂਰਨ
get_map_ਬਹੁਲਕ_string(char_u *ਬਹੁਲਕ_string, ਪੂਰਨ abbr)
{
    char_u	*p = ਬਹੁਲਕ_string;
    ਪੂਰਨ		ਬਹੁਲਕ = 0;
    ਪੂਰਨ		tmode;
    ਪੂਰਨ		modec;
    ਅਚਲ ਪੂਰਨ	ਨਕਾਬ_V = ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT;
    ਅਚਲ ਪੂਰਨ	ਨਕਾਬ_MAP = ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT | ਬਹੁਲਕ_ਸਾਧਾਰਨ
							     | ਬਹੁਲਕ_OP_PENDING;
    ਅਚਲ ਪੂਰਨ	ਨਕਾਬ_BANG = ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE;

    ਜੇ (*p == NUL)
	p = (char_u *)" ";	// compatibility
    ਜਦੋਂ ((modec = *p++))
    {
	ਚੋਣ (modec)
	{
	    ਹਾਲਤ 'i': tmode = ਬਹੁਲਕ_ਪਾਓ;	ਰੋਕੋ;
	    ਹਾਲਤ 'l': tmode = ਬਹੁਲਕ_LANGMAP;	ਰੋਕੋ;
	    ਹਾਲਤ 'c': tmode = ਬਹੁਲਕ_CMDLINE;	ਰੋਕੋ;
	    ਹਾਲਤ 'n': tmode = ਬਹੁਲਕ_ਸਾਧਾਰਨ;	ਰੋਕੋ;
	    ਹਾਲਤ 'x': tmode = ਬਹੁਲਕ_VISUAL;	ਰੋਕੋ;
	    ਹਾਲਤ 's': tmode = ਬਹੁਲਕ_SELECT;	ਰੋਕੋ;
	    ਹਾਲਤ 'o': tmode = ਬਹੁਲਕ_OP_PENDING;	ਰੋਕੋ;
	    ਹਾਲਤ 't': tmode = ਬਹੁਲਕ_TERMINAL;	ਰੋਕੋ;
	    ਹਾਲਤ 'v': tmode = ਨਕਾਬ_V;		ਰੋਕੋ;
	    ਹਾਲਤ '!': tmode = ਨਕਾਬ_BANG;	ਰੋਕੋ;
	    ਹਾਲਤ ' ': tmode = ਨਕਾਬ_MAP;		ਰੋਕੋ;
	    ਮੂਲ:
		      ਵਾਪਸ 0; // error, unknown mode character
	}
	ਬਹੁਲਕ |= tmode;
    }
    ਜੇ ((abbr && (ਬਹੁਲਕ & ~ਨਕਾਬ_BANG) != 0)
	|| (!abbr && (ਬਹੁਲਕ & (ਬਹੁਲਕ-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((ਬਹੁਲਕ & ਨਕਾਬ_BANG) != 0 && (ਬਹੁਲਕ & ~ਨਕਾਬ_BANG) == 0)
		    || ((ਬਹੁਲਕ & ਨਕਾਬ_MAP) != 0 && (ਬਹੁਲਕ & ~ਨਕਾਬ_MAP) == 0)))))
	ਵਾਪਸ 0;

    ਵਾਪਸ ਬਹੁਲਕ;
}

/*
 * "mapset()" function
 */
    ਖਾਲੀ
f_mapset(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    char_u	*which;
    ਪੂਰਨ		ਬਹੁਲਕ;
    char_u	buf[NUMBUFLEN];
    ਪੂਰਨ		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = ਨਿਰਾ;
    ਪੂਰਨ		noremap;
    ਪੂਰਨ		ਵਿਅੰਜਕ;
    ਪੂਰਨ		silent;
    ਪੂਰਨ		ਬਫਰ;
    scid_T	sid;
    ਪੂਰਨ		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_ਸਾਰਣੀ = maphash;
    mapblock_T  **abbr_ਸਾਰਣੀ = &ਪਹਿਲਾ_abbr;
    ਪੂਰਨ		nowait;
    char_u	*arg;
    ਪੂਰਨ		dict_only;
    mapblock_T	*mp_ਨਤੀਜਾ[2] = {ਨਿਰਾ, ਨਿਰਾ};

    ਜੇ (check_secure())
	ਵਾਪਸ;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	ਵਾਪਸ;

    ਜੇ (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	ਜੇ (which == ਨਿਰਾ || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_ਵਿੱਚ_mapset_dict_ਦਲੀਲ));
	    ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	ਜੇ (which == ਨਿਰਾ)
	    ਵਾਪਸ;
	is_abbr = (ਪੂਰਨ)tv_get_bool(&argvars[1]);

	ਜੇ (check_for_dict_arg(argvars, 2) == FAIL)
	    ਵਾਪਸ;
	d = argvars[2].vval.v_dict;
    }
    ਬਹੁਲਕ = get_map_ਬਹੁਲਕ_string(which, is_abbr);
    ਜੇ (ਬਹੁਲਕ == 0)
    {
	semsg(_(e_illegal_map_ਬਹੁਲਕ_string_str), which);
	ਵਾਪਸ;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    ਜੇ (lhs == ਨਿਰਾ || lhsraw == ਨਿਰਾ || rhs == ਨਿਰਾ)
    {
	emsg(_(e_entries_missing_ਵਿੱਚ_mapset_dict_ਦਲੀਲ));
	ਵਾਪਸ;
    }
    orig_rhs = rhs;

    noremap = dict_get_ਸੰਖਿਆ(d, "noremap") ? REMAP_NONE: 0;
    ਜੇ (dict_get_ਸੰਖਿਆ(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    ਵਿਅੰਜਕ = dict_get_ਸੰਖਿਆ(d, "expr") != 0;
    silent = dict_get_ਸੰਖਿਆ(d, "silent") != 0;
    sid = dict_get_ਸੰਖਿਆ(d, "sid");
    scriptversion = dict_get_ਸੰਖਿਆ(d, "scriptversion");
    lnum = dict_get_ਸੰਖਿਆ(d, "lnum");
    ਬਫਰ = dict_get_ਸੰਖਿਆ(d, "buffer");
    nowait = dict_get_ਸੰਖਿਆ(d, "nowait") != 0;
    // mode from the dict is not used

    ਜੇ (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    ਨਹੀਂਤਾਂ
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, ਨਿਰਾ);

    ਜੇ (ਬਫਰ)
    {
	map_ਸਾਰਣੀ = curbuf->b_maphash;
	abbr_ਸਾਰਣੀ = &curbuf->b_ਪਹਿਲਾ_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    ਜੇ (ਬਫਰ)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	ਜੇ (arg == ਨਿਰਾ)
	    ਵਾਪਸ;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    ਨਹੀਂਤਾਂ
    {
	arg = vim_strsave(lhs);
	ਜੇ (arg == ਨਿਰਾ)
	    ਵਾਪਸ;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, ਬਹੁਲਕ, is_abbr);
    vim_free(arg);

    mp_ਨਤੀਜਾ[0] = map_ਜੋੜੋ(map_ਸਾਰਣੀ, abbr_ਸਾਰਣੀ, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, ਬਹੁਲਕ, is_abbr, ਵਿਅੰਜਕ, sid,
			    scriptversion, lnum, 0);
    ਜੇ (lhsrawalt != ਨਿਰਾ)
	mp_ਨਤੀਜਾ[1] = map_ਜੋੜੋ(map_ਸਾਰਣੀ, abbr_ਸਾਰਣੀ, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, ਬਹੁਲਕ, is_abbr, ਵਿਅੰਜਕ, sid,
			    scriptversion, lnum, 1);

    ਜੇ (mp_ਨਤੀਜਾ[0] != ਨਿਰਾ && mp_ਨਤੀਜਾ[1] != ਨਿਰਾ)
    {
	mp_ਨਤੀਜਾ[0]->m_alt = mp_ਨਤੀਜਾ[1];
	mp_ਨਤੀਜਾ[1]->m_alt = mp_ਨਤੀਜਾ[0];
    }

    vim_free(arg_buf);
}
#ਜੇ_ਅੰਤ


#ਜੇ defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(ਬਹੁਲਕ_VISUAL | ਬਹੁਲਕ_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
ਬਣਤਰ initmap
{
    char_u	*arg;
    ਪੂਰਨ		ਬਹੁਲਕ;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
ਸਥਿਰ ਬਣਤਰ initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", ਬਹੁਲਕ_ਸਾਧਾਰਨ},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# ਜੇ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
ਸਥਿਰ ਬਣਤਰ initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", ਬਹੁਲਕ_ਸਾਧਾਰਨ | VIS_SEL},
	{(char_u *)"\316w <C-Home>", ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE},
	{(char_u *)"\316u <C-End>", ਬਹੁਲਕ_ਸਾਧਾਰਨ | VIS_SEL},
	{(char_u *)"\316u <C-End>", ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", ਬਹੁਲਕ_ਸਾਧਾਰਨ},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", ਬਹੁਲਕ_ਪਾਓ},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  ਨਹੀਂਤਾਂ
	{(char_u *)"\316\324 P", ਬਹੁਲਕ_ਸਾਧਾਰਨ},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", ਬਹੁਲਕ_ਪਾਓ}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# ਜੇ defined(MACOS_X)
ਸਥਿਰ ਬਣਤਰ initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", ਬਹੁਲਕ_ਸਾਧਾਰਨ},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE},
	{(char_u *)"<D-c> \"*y", VIS_SEL},
	{(char_u *)"<D-x> \"*d", VIS_SEL},
	{(char_u *)"<Backspace> \"-d", VIS_SEL},
};
# endif

# undef VIS_SEL
#ਜੇ_ਅੰਤ

/*
 * Set up default mappings.
 */
    ਖਾਲੀ
init_mappings(ਖਾਲੀ)
{
#ਜੇ defined(MSWIN) || defined(MACOS_X)
    ਪੂਰਨ		i;

# ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ਜੇ (!gui.starting)
#  endif
    {
	ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(cinitmappings); ++i)
	    ਜੋੜੋ_map(cinitmappings[i].arg, cinitmappings[i].ਬਹੁਲਕ, FALSE);
    }
# endif
# ਜੇ defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(initmappings); ++i)
	ਜੋੜੋ_map(initmappings[i].arg, initmappings[i].ਬਹੁਲਕ, FALSE);
# endif
#ਜੇ_ਅੰਤ
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    ਖਾਲੀ
ਜੋੜੋ_map(char_u *map, ਪੂਰਨ ਬਹੁਲਕ, ਪੂਰਨ nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;	// Allow <> notation
    s = vim_strsave(map);
    ਜੇ (s != ਨਿਰਾ)
    {
	(ਖਾਲੀ)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, ਬਹੁਲਕ, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#ਜੇ defined(FEAT_LANGMAP)
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	    from;
    ਪੂਰਨ     ਮੰਜ਼ਿਲ;
} langmap_ਇੰਦਰਾਜ_T;

ਸਥਿਰ garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    ਸਥਿਰ ਖਾਲੀ
langmap_ਸੈੱਟ_ਇੰਦਰਾਜ(ਪੂਰਨ from, ਪੂਰਨ ਮੰਜ਼ਿਲ)
{
    langmap_ਇੰਦਰਾਜ_T *entries = (langmap_ਇੰਦਰਾਜ_T *)(langmap_mapga.ga_ਅੰਕੜੇ);
    ਪੂਰਨ		    a = 0;
    ਪੂਰਨ		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    ਜਦੋਂ (a != b)
    {
	ਪੂਰਨ i = (a + b) / 2;
	ਪੂਰਨ d = entries[i].from - from;

	ਜੇ (d == 0)
	{
	    entries[i].ਮੰਜ਼ਿਲ = ਮੰਜ਼ਿਲ;
	    ਵਾਪਸ;
	}
	ਜੇ (d < 0)
	    a = i + 1;
	ਨਹੀਂਤਾਂ
	    b = i;
    }

    ਜੇ (ga_grow(&langmap_mapga, 1) == FAIL)
	ਵਾਪਸ;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_ਇੰਦਰਾਜ_T *)(langmap_mapga.ga_ਅੰਕੜੇ) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * ਆਕਾਰ(langmap_ਇੰਦਰਾਜ_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].ਮੰਜ਼ਿਲ = ਮੰਜ਼ਿਲ;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    ਪੂਰਨ
langmap_adjust_mb(ਪੂਰਨ c)
{
    langmap_ਇੰਦਰਾਜ_T *entries = (langmap_ਇੰਦਰਾਜ_T *)(langmap_mapga.ga_ਅੰਕੜੇ);
    ਪੂਰਨ a = 0;
    ਪੂਰਨ b = langmap_mapga.ga_len;

    ਜਦੋਂ (a != b)
    {
	ਪੂਰਨ i = (a + b) / 2;
	ਪੂਰਨ d = entries[i].from - c;

	ਜੇ (d == 0)
	    ਵਾਪਸ entries[i].ਮੰਜ਼ਿਲ;  // found matching entry
	ਜੇ (d < 0)
	    a = i + 1;
	ਨਹੀਂਤਾਂ
	    b = i;
    }
    ਵਾਪਸ c;  // no entry found, return "c" unmodified
}

    ਖਾਲੀ
langmap_init(ਖਾਲੀ)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, ਆਕਾਰ(langmap_ਇੰਦਰਾਜ_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_langmap(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    char_u  *p;
    char_u  *p2;
    ਪੂਰਨ	    from, ਮੰਜ਼ਿਲ;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    ਲਈ (p = p_langmap; p[0] != NUL; )
    {
	ਲਈ (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    ਜੇ (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	ਜੇ (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	ਨਹੀਂਤਾਂ
	    p2 = ਨਿਰਾ;	    // aAbBcCdD form, p2 is NULL
	ਜਦੋਂ (p[0])
	{
	    ਜੇ (p[0] == ',')
	    {
		++p;
		ਰੋਕੋ;
	    }
	    ਜੇ (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    ਮੰਜ਼ਿਲ = NUL;
	    ਜੇ (p2 == ਨਿਰਾ)
	    {
		MB_PTR_ADV(p);
		ਜੇ (p[0] != ',')
		{
		    ਜੇ (p[0] == '\\')
			++p;
		    ਮੰਜ਼ਿਲ = (*mb_ptr2char)(p);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (p2[0] != ',')
		{
		    ਜੇ (p2[0] == '\\')
			++p2;
		    ਮੰਜ਼ਿਲ = (*mb_ptr2char)(p2);
		}
	    }
	    ਜੇ (ਮੰਜ਼ਿਲ == NUL)
	    {
		sprintf(ਦਲੀਲਾਂ->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		ਵਾਪਸ ਦਲੀਲਾਂ->os_errbuf;
	    }

	    ਜੇ (from >= 256)
		langmap_ਸੈੱਟ_ਇੰਦਰਾਜ(from, ਮੰਜ਼ਿਲ);
	    ਨਹੀਂਤਾਂ
		langmap_mapchar[from & 255] = ਮੰਜ਼ਿਲ;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    ਜੇ (p2 != ਨਿਰਾ)
	    {
		MB_PTR_ADV(p2);
		ਜੇ (*p == ';')
		{
		    p = p2;
		    ਜੇ (p[0] != NUL)
		    {
			ਜੇ (p[0] != ',')
			{
			    vim_snprintf(ਦਲੀਲਾਂ->os_errbuf, ਦਲੀਲਾਂ->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    ਵਾਪਸ ਦਲੀਲਾਂ->os_errbuf;
			}
			++p;
		    }
		    ਰੋਕੋ;
		}
	    }
	}
    }

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
do_exmap(exarg_T *eap, ਪੂਰਨ isabbrev)
{
    ਪੂਰਨ	    ਬਹੁਲਕ;
    char_u  *cmdp;

    cmdp = eap->cmd;
    ਬਹੁਲਕ = get_map_ਬਹੁਲਕ(&cmdp, eap->forceit || isabbrev);

    ਚੋਣ (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, ਬਹੁਲਕ, isabbrev))
    {
	ਹਾਲਤ 1: emsg(_(e_invalid_ਦਲੀਲ));
		ਰੋਕੋ;
	ਹਾਲਤ 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		ਰੋਕੋ;
    }
}

/*
 * ":abbreviate" and friends.
 */
    ਖਾਲੀ
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    ਖਾਲੀ
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    ਜੇ (secure)
    {
	secure = 2;
	ਸੁਨੇਹਾ_outtrans(eap->cmd);
	ਸੁਨੇਹਾ_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    ਖਾਲੀ
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    ਖਾਲੀ
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    ਖਾਲੀ
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
