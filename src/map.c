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

#ضمن "vim.h"

/*
 * List used for abbreviations.
 */
ساكن mapblock_T	*first_abbr = عدم; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
ساكن mapblock_T	*(maphash[256]);
ساكن صحيح		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
ساكن صحيح		map_locked = 0;

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
get_maphash_list(صحيح state, صحيح c)
{
    أرجع maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(صحيح state, صحيح c)
{
    أرجع curbuf->b_maphash[MAP_HASH(state, c)];
}

    صحيح
is_maphash_valid(فراغ)
{
    أرجع maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    ساكن فراغ
validate_maphash(فراغ)
{
    إذا (maphash_valid)
	أرجع;

    CLEAR_FIELD(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    ساكن فراغ
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_کلیلەکان);
    إذا (mp->m_alt != عدم)
	mp->m_alt->m_alt = عدم;
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
    ساكن char_u *
map_mode_to_chars(صحيح mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    إذا ((mode & (MODE_زیادکردن | MODE_CMDLINE)) == (MODE_زیادکردن | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    وإلا إذا (mode & MODE_زیادکردن)
	ga_append(&mapmode, 'i');			// :imap
    وإلا إذا (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    وإلا إذا (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    وإلا إذا ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    وإلا
    {
	إذا (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	إذا (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	إذا (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	إذا ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	وإلا
	{
	    إذا (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    إذا (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    أرجع (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    ساكن فراغ
showmap(
    mapblock_T	*mp,
    صحيح		local)	    // TRUE for buffer-local map
{
    صحيح		len = 1;
    char_u	*mapchars;

    إذا (message_filtered(mp->m_کلیلەکان) && message_filtered(mp->m_str))
	أرجع;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    إذا (پەیام_didout || پەیام_silent != 0)
    {
	پەیام_putchar('\n');
	إذا (got_int)	    // 'q' typed at MORE prompt
	    اذهب theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    إذا (mapchars != عدم)
    {
	پەیام_puts((حرف *)mapchars);
	len = (صحيح)STRLEN(mapchars);
	vim_free(mapchars);
    }

    طالما (++len <= 3)
	پەیام_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = پەیام_outtrans_special(mp->m_کلیلەکان, TRUE, 0);
    افعل
    {
	پەیام_putchar(' ');		// pad with blanks
	++len;
    } طالما (len < 12);

    إذا (mp->m_noremap == REMAP_NONE)
	پەیام_puts_attr("*", HL_ATTR(HLF_8));
    وإلا إذا (mp->m_noremap == REMAP_SCRIPT)
	پەیام_puts_attr("&", HL_ATTR(HLF_8));
    وإلا
	پەیام_putchar(' ');

    إذا (local)
	پەیام_putchar('@');
    وإلا
	پەیام_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    إذا (*mp->m_str == NUL)
	پەیام_puts_attr("<Nop>", HL_ATTR(HLF_8));
    وإلا
	پەیام_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    إذا (p_verbose > 0)
	last_set_پەیام(mp->m_script_ctx);
#endif
    پەیام_clr_eos();
    out_flush();			// show one line at a time

theend:
    --map_locked;
}

    ساكن mapblock_T *
map_add(
	mapblock_T  **map_table,
	mapblock_T  **abbr_table,
	char_u	    *کلیلەکان,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	صحيح	    noremap,
	صحيح	    nowait,
	صحيح	    silent,
	صحيح	    mode,
	صحيح	    is_abbr,
#ifdef FEAT_EVAL
	صحيح	    دەربڕین,
	scid_T	    sid,	    // 0 to use current_sctx
	صحيح	    scriptversion,
	linenr_T    lnum,
#endif
	صحيح	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    إذا (mp == عدم)
	أرجع عدم;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    إذا (*کلیلەکان == Ctrl_C)
    {
	إذا (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	وإلا
	    mapped_ctrl_c |= mode;
    }

    mp->m_کلیلەکان = vim_strsave(کلیلەکان);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    إذا (mp->m_کلیلەکان == عدم || mp->m_str == عدم)
    {
	vim_free(mp->m_کلیلەکان);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	أرجع عدم;
    }
    mp->m_keylen = (صحيح)STRLEN(mp->m_کلیلەکان);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_دەربڕین = دەربڕین;
    إذا (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    وإلا
    {
	mp->m_script_ctx = current_sctx;
	إذا (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    إذا (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    وإلا
    {
	صحيح n = MAP_HASH(mp->m_mode, mp->m_کلیلەکان[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    أرجع mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    ساكن فراغ
list_mappings(
	صحيح	keyround,
	صحيح	abbrev,
	صحيح	haskey,
	char_u	*کلیلەکان,
	صحيح	کلیلەکان_len,
	صحيح	mode,
	صحيح	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    إذا (p_verbose > 0 && keyround == 1)
    {
	إذا (seen_Modify_Other_کلیلەکان)
	    پەیام_puts(_("Seen modifyOtherKeys: true\n"));

	إذا (modify_otherkeys_state != MOKS_INITIAL)
	{
	    حرف *name = _("Unknown");
	    بدل (modify_otherkeys_state)
	    {
		حالة MOKS_INITIAL: اكسر;
		حالة MOKS_OFF: name = _("Off"); اكسر;
		حالة MOKS_ENABLED: name = _("On"); اكسر;
		حالة MOKS_DISABLED: name = _("Disabled"); اكسر;
		حالة MOKS_AFTER_T_TE: name = _("Cleared"); اكسر;
	    }

	    حرف buf[200];
	    vim_snprintf(buf, حجم(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    پەیام_puts(buf);
	}

	إذا (kitty_protocol_state != KKPS_INITIAL)
	{
	    حرف *name = _("Unknown");
	    بدل (kitty_protocol_state)
	    {
		حالة KKPS_INITIAL: اكسر;
		حالة KKPS_OFF: name = _("Off"); اكسر;
		حالة KKPS_ENABLED: name = _("On"); اكسر;
		حالة KKPS_DISABLED: name = _("Disabled"); اكسر;
		حالة KKPS_AFTER_T_TE: name = _("Cleared"); اكسر;
	    }

	    حرف buf[200];
	    vim_snprintf(buf, حجم(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    پەیام_puts(buf);
	}
    }

    // need to loop over all global hash lists
    لكل (صحيح hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	إذا (abbrev)
	{
	    إذا (hash != 0)	// there is only one abbreviation list
		اكسر;
	    mp = curbuf->b_first_abbr;
	}
	وإلا
	    mp = curbuf->b_maphash[hash];
	لكل ( ; mp != عدم && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    إذا (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		إذا (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		وإلا
		{
		    صحيح n = mp->m_keylen;
		    إذا (STRNCMP(mp->m_کلیلەکان, کلیلەکان,
				   (size_t)(n < کلیلەکان_len ? n : کلیلەکان_len)) == 0)
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
    صحيح
do_map(
    صحيح		maptype,
    char_u	*arg,
    صحيح		mode,
    صحيح		abbrev)		// not a mapping but an abbreviation
{
    char_u	*کلیلەکان;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {عدم, عدم};
    char_u	*rhs;
    char_u	*p;
    صحيح		n;
    صحيح		len = 0;	// init for GCC
    صحيح		hasarg;
    صحيح		haskey;
    صحيح		do_print;
    صحيح		keyround;
    char_u	*کلیلەکان_buf = عدم;
    char_u	*alt_کلیلەکان_buf = عدم;
    char_u	*arg_buf = عدم;
    صحيح		retval = 0;
    صحيح		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    صحيح		unique = FALSE;
    صحيح		nowait = FALSE;
    صحيح		silent = FALSE;
    صحيح		special = FALSE;
#ifdef FEAT_EVAL
    صحيح		دەربڕین = FALSE;
#endif
    صحيح		did_simplify = FALSE;
    صحيح		unmap_lhs_only = FALSE;
    صحيح		noremap;
    char_u      *orig_rhs;

    کلیلەکان = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    إذا (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    إذا (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    وإلا
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    لكل (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	إذا (STRNCMP(کلیلەکان, "<buffer>", 8) == 0)
	{
	    کلیلەکان = skipwhite(کلیلەکان + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    استمر;
	}

	// Check for "<nowait>": don't wait for more characters.
	إذا (STRNCMP(کلیلەکان, "<nowait>", 8) == 0)
	{
	    کلیلەکان = skipwhite(کلیلەکان + 8);
	    nowait = TRUE;
	    استمر;
	}

	// Check for "<silent>": don't echo commands.
	إذا (STRNCMP(کلیلەکان, "<silent>", 8) == 0)
	{
	    کلیلەکان = skipwhite(کلیلەکان + 8);
	    silent = TRUE;
	    استمر;
	}

	// Check for "<special>": accept special keys in <>
	إذا (STRNCMP(کلیلەکان, "<special>", 9) == 0)
	{
	    کلیلەکان = skipwhite(کلیلەکان + 9);
	    special = TRUE;
	    استمر;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	إذا (STRNCMP(کلیلەکان, "<script>", 8) == 0)
	{
	    کلیلەکان = skipwhite(کلیلەکان + 8);
	    noremap = REMAP_SCRIPT;
	    استمر;
	}

	// Check for "<expr>": {rhs} is an expression.
	إذا (STRNCMP(کلیلەکان, "<expr>", 6) == 0)
	{
	    کلیلەکان = skipwhite(کلیلەکان + 6);
	    دەربڕین = TRUE;
	    استمر;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	إذا (STRNCMP(کلیلەکان, "<unique>", 8) == 0)
	{
	    کلیلەکان = skipwhite(کلیلەکان + 8);
	    unique = TRUE;
	    استمر;
	}
	اكسر;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = کلیلەکان;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == عدم);
    طالما (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	إذا ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    إذا (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*کلیلەکان != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    إذا (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	اذهب theend;
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
    إذا (haskey)
    {
	char_u	*new_کلیلەکان;
	صحيح	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	إذا (special)
	    flags |= REPTERM_SPECIAL;
	new_کلیلەکان = replace_termcodes(کلیلەکان, &کلیلەکان_buf, 0, flags, &did_simplify);
	إذا (did_simplify)
	    (فراغ)replace_termcodes(کلیلەکان, &alt_کلیلەکان_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, عدم);
	کلیلەکان = new_کلیلەکان;
    }
    orig_rhs = rhs;
    إذا (hasarg)
    {
	إذا (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	وإلا
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), عدم);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    لكل (keyround = 1; keyround <= 2; ++keyround)
    {
	صحيح	did_it = FALSE;
	صحيح	did_local = FALSE;
	صحيح	keyround1_simplified = keyround == 1 && did_simplify;
	صحيح	round;
	صحيح	ژمارە_rounds;

	إذا (keyround == 2)
	{
	    إذا (alt_کلیلەکان_buf == عدم)
		اكسر;
	    کلیلەکان = alt_کلیلەکان_buf;
	}
	وإلا إذا (alt_کلیلەکان_buf != عدم && do_print)
	    // when printing always use the not-simplified map
	    کلیلەکان = alt_کلیلەکان_buf;

	// check arguments and translate function keys
	إذا (haskey)
	{
	    len = (صحيح)STRLEN(کلیلەکان);
	    إذا (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		اذهب theend;
	    }

	    إذا (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		إذا (has_mbyte)
		{
		    صحيح		first, last;
		    صحيح		same = -1;
		    char_u	کلیلەکان_unescaped[MAXMAPLEN + 1];
		    size_t	کلیلەکان_unescaped_len;

		    mch_memmove(کلیلەکان_unescaped, کلیلەکان, (size_t)(len + 1));
		    کلیلەکان_unescaped_len = vim_unescape_csi(کلیلەکان_unescaped);
		    p = کلیلەکان_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    طالما (p < کلیلەکان_unescaped + کلیلەکان_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			إذا (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    إذا (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			اذهب theend;
		    }
		}
		وإلا إذا (vim_iswordc(کلیلەکان[len - 1]))
		    // ends in keyword char
		    لكل (n = 0; n < len - 2; ++n)
			إذا (vim_iswordc(کلیلەکان[n]) != vim_iswordc(کلیلەکان[len - 2]))
			{
			    retval = 1;
			    اذهب theend;
			}
		// An abbreviation cannot contain white space.
		لكل (n = 0; n < len; ++n)
		    إذا (VIM_ISWHITE(کلیلەکان[n]))
		    {
			retval = 1;
			اذهب theend;
		    }
	    }
	}

	إذا (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	إذا (do_print)
	    پەیام_start();

	// Check if a new local mapping wasn't already defined globally.
	إذا (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    لكل (صحيح hash = 0; hash < 256 && !got_int; ++hash)
	    {
		إذا (abbrev)
		{
		    إذا (hash != 0)	// there is only one abbreviation list
			اكسر;
		    mp = first_abbr;
		}
		وإلا
		    mp = maphash[hash];
		لكل ( ; mp != عدم && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    إذا ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_کلیلەکان, کلیلەکان, (size_t)len) == 0)
		    {
			إذا (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_کلیلەکان);
			وإلا
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_کلیلەکان);
			retval = 5;
			اذهب theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	إذا (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, کلیلەکان, len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	ژمارە_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	لكل (round = 0; round < ژمارە_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    لكل (صحيح hash = 0; hash < 256 && !got_int; ++hash)
	    {
		إذا (abbrev)
		{
		    إذا (hash > 0)	// there is only one abbreviation list
			اكسر;
		    mpp = abbr_table;
		}
		وإلا
		    mpp = &(map_table[hash]);
		لكل (mp = *mpp; mp != عدم && !got_int; mp = *mpp)
		{

		    إذا ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			استمر;
		    }
		    إذا (!haskey)	// show all entries
		    {
			إذا (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    وإلا	// do we have a match?
		    {
			إذا (round)	// second round: Try unmap "rhs" string
			{
			    n = (صحيح)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			وإلا
			{
			    n = mp->m_keylen;
			    p = mp->m_کلیلەکان;
			}
			إذا (STRNCMP(p, کلیلەکان, (size_t)(n < len ? n : len)) == 0)
			{
			    إذا (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				إذا (n != len && (!abbrev || round || n > len
					       || *skipwhite(کلیلەکان + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    استمر;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				إذا (keyround1_simplified && !mp->m_simplified)
				    اكسر;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    وإلا إذا (!hasarg)	// show matching entry
			    {
				إذا (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    وإلا إذا (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				استمر;
			    }
			    وإلا إذا (unique)
			    {
				إذا (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				وإلا
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				اذهب theend;
			    }
			    وإلا
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				إذا (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    إذا (newstr == عدم)
				    {
					retval = 4;		// no mem
					اذهب theend;
				    }
				    إذا (mp->m_alt != عدم)
					mp->m_alt = mp->m_alt->m_alt = عدم;
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
				    mp->m_دەربڕین = دەربڕین;
				    mp->m_script_ctx = current_sctx;
				    إذا (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    إذا (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				استمر;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    صحيح new_hash = MAP_HASH(mp->m_mode, mp->m_کلیلەکان[0]);
			    إذا (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_next;
				mp->m_next = map_table[new_hash];
				map_table[new_hash] = mp;

				استمر;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_next);
		}
	    }
	}

	إذا (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    إذا (!did_it)
	    {
		إذا (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    وإلا إذا (*کلیلەکان == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		إذا (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		وإلا
		    mapped_ctrl_c &= ~mode;
	    }
	    استمر;
	}

	إذا (!haskey || !hasarg)
	{
	    // print entries
	    إذا (!did_it && !did_local)
	    {
		إذا (abbrev)
		    پەیام(_("No abbreviation found"));
		وإلا
		    پەیام(_("No mapping found"));
	    }
	    اذهب theend;    // listing finished
	}

	إذا (did_it)
	    استمر;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, کلیلەکان,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    دەربڕین, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	إذا (mp_result[keyround - 1] == عدم)
	{
	    retval = 4;	    // no mem
	    اذهب theend;
	}
    }

    إذا (mp_result[0] != عدم && mp_result[1] != عدم)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(کلیلەکان_buf);
    vim_free(alt_کلیلەکان_buf);
    vim_free(arg_buf);
    أرجع retval;
}

/*
 * Get the mapping mode from the command name.
 */
    ساكن صحيح
get_map_mode(char_u **cmdp, صحيح forceit)
{
    char_u	*p;
    صحيح		modec;
    صحيح		mode;

    p = *cmdp;
    modec = *p++;
    إذا (modec == 'i')
	mode = MODE_زیادکردن;				// :imap
    وإلا إذا (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    وإلا إذا (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    وإلا إذا (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    وإلا إذا (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    وإلا إذا (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    وإلا إذا (modec == 's')
	mode = MODE_SELECT;			// :smap
    وإلا إذا (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    وإلا إذا (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    وإلا
    {
	--p;
	إذا (forceit)
	    mode = MODE_زیادکردن | MODE_CMDLINE;		// :map !
	وإلا
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    أرجع mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    ساكن فراغ
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    صحيح		forceit,
    صحيح		abbr)
{
    صحيح		mode;
    صحيح		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    إذا (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	أرجع;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ساكن صحيح
is_map_locked(فراغ)
{
    إذا (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    فراغ
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    صحيح		mode,		// mode in which to delete
    صحيح		local,		// TRUE for buffer-local mappings
    صحيح		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    صحيح		hash;
    صحيح		new_hash;

    إذا (is_map_locked())
	أرجع;

    validate_maphash();

    لكل (hash = 0; hash < 256; ++hash)
    {
	إذا (abbr)
	{
	    إذا (hash > 0)	// there is only one abbrlist
		اكسر;
	    إذا (local)
		mpp = &buf->b_first_abbr;
	    وإلا
		mpp = &first_abbr;
	}
	وإلا
	{
	    إذا (local)
		mpp = &buf->b_maphash[hash];
	    وإلا
		mpp = &maphash[hash];
	}
	طالما (*mpp != عدم)
	{
	    mp = *mpp;
	    إذا (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		إذا (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    استمر;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_کلیلەکان[0]);
		إذا (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    إذا (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    وإلا
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    استمر;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

#إذا defined(FEAT_EVAL)
    صحيح
mode_str2flags(char_u *modechars)
{
    صحيح		mode = 0;

    إذا (vim_strchr(modechars, 'n') != عدم)
	mode |= MODE_NORMAL;
    إذا (vim_strchr(modechars, 'v') != عدم)
	mode |= MODE_VISUAL | MODE_SELECT;
    إذا (vim_strchr(modechars, 'x') != عدم)
	mode |= MODE_VISUAL;
    إذا (vim_strchr(modechars, 's') != عدم)
	mode |= MODE_SELECT;
    إذا (vim_strchr(modechars, 'o') != عدم)
	mode |= MODE_OP_PENDING;
    إذا (vim_strchr(modechars, 'i') != عدم)
	mode |= MODE_زیادکردن;
    إذا (vim_strchr(modechars, 'l') != عدم)
	mode |= MODE_LANGMAP;
    إذا (vim_strchr(modechars, 'c') != عدم)
	mode |= MODE_CMDLINE;

    أرجع mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    صحيح
map_to_exists(char_u *str, char_u *modechars, صحيح abbr)
{
    char_u	*rhs;
    char_u	*buf;
    صحيح		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, عدم);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    أرجع retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    صحيح
map_to_exists_mode(char_u *rhs, صحيح mode, صحيح abbr)
{
    mapblock_T	*mp;
    صحيح		hash;
    صحيح		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    لكل (;;)
    {
	لكل (hash = 0; hash < 256; ++hash)
	{
	    إذا (abbr)
	    {
		إذا (hash > 0)		// there is only one abbr list
		    اكسر;
		إذا (exp_buffer)
		    mp = curbuf->b_first_abbr;
		وإلا
		    mp = first_abbr;
	    }
	    وإلا إذا (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    وإلا
		mp = maphash[hash];
	    لكل (; mp; mp = mp->m_next)
	    {
		إذا ((mp->m_mode & mode)
			&& strstr((حرف *)mp->m_str, (حرف *)rhs) != عدم)
		    أرجع TRUE;
	    }
	}
	إذا (exp_buffer)
	    اكسر;
	exp_buffer = TRUE;
    }

    أرجع FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
ساكن صحيح	expand_mapmodes = 0;
ساكن صحيح	expand_isabbrev = 0;
ساكن صحيح	expand_buffer = FALSE;

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
    ساكن char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    صحيح		c;
    صحيح		modifiers;
    صحيح		cpo_bslash;
    صحيح		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != عدم);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != عدم);

    لكل (; *str; ++str)
    {
	c = *str;
	إذا (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    إذا (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    إذا (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		إذا (cpo_special)
		{
		    ga_clear(&ga);
		    أرجع عدم;
		}
		c = TO_SPECIAL(str[1], str[2]);
		إذا (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    إذا (IS_SPECIAL(c) || modifiers)	// special key
	    {
		إذا (cpo_special)
		{
		    ga_clear(&ga);
		    أرجع عدم;
		}
		ga_concat(&ga, get_special_کلیل_name(c, modifiers));
		استمر; // for (str)
	    }
	}
	إذا (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	إذا (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    أرجع (char_u *)(ga.ga_data);
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
    صحيح		forceit,	// TRUE if '!' given
    صحيح		isabbrev,	// TRUE if abbreviation
    صحيح		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    إذا (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    وإلا
    {
	إذا (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	وإلا
	{
	    expand_mapmodes = MODE_زیادکردن | MODE_CMDLINE;
	    إذا (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	لكل (;;)
	{
	    إذا (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		استمر;
	    }
	    إذا (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		استمر;
	    }
	    إذا (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		استمر;
	    }
	    إذا (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		استمر;
	    }
	    إذا (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		استمر;
	    }
#ifdef FEAT_EVAL
	    إذا (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		استمر;
	    }
	    إذا (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		استمر;
	    }
#endif
	    اكسر;
	}
	xp->xp_pattern = arg;
    }

    أرجع عدم;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    صحيح
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    صحيح		*ژمارە_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    صحيح		hash;
    صحيح		ژماردن;
    char_u	*p;
    صحيح		i;
    صحيح		fuzzy;
    صحيح		match;
    صحيح		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *ژمارە_Matches = 0;		    // return values in case of FAIL
    *matches = عدم;

    إذا (!fuzzy)
	ga_init2(&ga, حجم(حرف *), 3);
    وإلا
	ga_init2(&ga, حجم(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    لكل (i = 0; i < 7; ++i)
    {
	إذا (i == 0)
	    p = (char_u *)"<silent>";
	وإلا إذا (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	وإلا إذا (i == 2)
	    p = (char_u *)"<script>";
	وإلا إذا (i == 3)
	    p = (char_u *)"<expr>";
#endif
	وإلا إذا (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	وإلا إذا (i == 5)
	    p = (char_u *)"<nowait>";
	وإلا إذا (i == 6)
	    p = (char_u *)"<special>";
	وإلا
	    استمر;

	إذا (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	وإلا
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	إذا (!match)
	    استمر;

	إذا (ga_grow(&ga, 1) == FAIL)
	    اكسر;

	إذا (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	وإلا
	    ((char_u **)ga.ga_data)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    لكل (hash = 0; hash < 256; ++hash)
    {
	إذا (expand_isabbrev)
	{
	    إذا (hash > 0)	// only one abbrev list
		اكسر; // for (hash)
	    mp = first_abbr;
	}
	وإلا إذا (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	وإلا
	    mp = maphash[hash];
	لكل (; mp; mp = mp->m_next)
	{
	    إذا (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		استمر;

	    p = translate_mapping(mp->m_کلیلەکان);
	    إذا (p == عدم)
		استمر;

	    إذا (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    وإلا
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    إذا (!match)
	    {
		vim_free(p);
		استمر;
	    }

	    إذا (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		اكسر;
	    }

	    إذا (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    وإلا
		((char_u **)ga.ga_data)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    إذا (ga.ga_len == 0)
	أرجع FAIL;

    إذا (!fuzzy)
    {
	*matches = ga.ga_data;
	*ژمارە_Matches = ga.ga_len;
    }
    وإلا
    {
	إذا (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    أرجع FAIL;
	*ژمارە_Matches = ga.ga_len;
    }

    ژماردن = *ژمارە_Matches;
    إذا (ژماردن > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	إذا (!fuzzy)
	    sort_strings(*matches, ژماردن);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + ژماردن;

	طالما (ptr2 < ptr3)
	{
	    إذا (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    وإلا
	    {
		vim_free(*ptr2++);
		ژماردن--;
	    }
	}
    }

    *ژمارە_Matches = ژماردن;
    أرجع (ژماردن == 0 ? FAIL : OK);
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
    صحيح
check_abbr(
    صحيح		c,
    char_u	*ptr,
    صحيح		col,
    صحيح		mincol)
{
    صحيح		len;
    صحيح		scol;		// starting column of the abbr.
    صحيح		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    صحيح		clen = 0;	// length in characters
    صحيح		is_id = TRUE;
    صحيح		vim_abbr;

    إذا (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	أرجع FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    إذا (noremap_کلیلەکان() && c != Ctrl_RSB)
	أرجع FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    إذا (col == 0)				// cannot be an abbr.
	أرجع FALSE;

    إذا (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	إذا (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	وإلا
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    إذا (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	طالما (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    إذا (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		اكسر;
	    }
	    ++clen;
	}
	scol = (صحيح)(p - ptr);
    }
    وإلا
    {
	إذا (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	وإلا
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    إذا (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	لكل (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    إذا (scol < mincol)
	scol = mincol;
    إذا (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	إذا (mp == عدم)
	{
	    mp = mp2;
	    mp2 = عدم;
	}
	لكل ( ; mp; mp->m_next == عدم
				  ? (mp = mp2, mp2 = عدم) : (mp = mp->m_next))
	{
	    صحيح		qlen = mp->m_keylen;
	    char_u	*q = mp->m_کلیلەکان;
	    صحيح		match;

	    إذا (vim_strbyte(mp->m_کلیلەکان, K_SPECIAL) != عدم)
	    {
		char_u *qe = vim_strsave(mp->m_کلیلەکان);

		// might have CSI escaped mp->m_keys
		إذا (qe != عدم)
		{
		    q = qe;
		    qlen = (صحيح)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    إذا (q != mp->m_کلیلەکان)
		vim_free(q);
	    إذا (match)
		اكسر;
	}
	إذا (mp != عدم)
	{
	    صحيح	noremap;
	    صحيح silent;
#ifdef FEAT_EVAL
	    صحيح دەربڕین;
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
	    إذا (c != Ctrl_RSB)
	    {
					// special key code, split up
		إذا (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		وإلا
		{
		    إذا (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    إذا (has_mbyte)
		    {
			صحيح	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			إذا (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			إذا (escaped != عدم)
			{
			    newlen = (صحيح)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    وإلا
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(فراغ)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ifdef FEAT_EVAL
	    دەربڕین = mp->m_دەربڕین;

	    إذا (دەربڕین)
		s = eval_map_دەربڕین(mp, c);
	    وإلا
#endif
		s = mp->m_str;
	    إذا (s != عدم)
	    {
					// insert the to string
		(فراغ)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (صحيح)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		إذا (دەربڕین)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    إذا (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    طالما (len-- > 0)		// delete the from string
		(فراغ)ins_typebuf(tb, 1, 0, TRUE, silent);
	    أرجع TRUE;
	}
    }
    أرجع FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_دەربڕین(
    mapblock_T	*mp,
    صحيح		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*دەربڕین;
    pos_T	save_cursor;
    صحيح		save_پەیام_col;
    صحيح		save_پەیام_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    صحيح		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    دەربڕین = vim_strsave(mp->m_str);
    إذا (دەربڕین == عدم)
	أرجع عدم;
    vim_unescape_csi(دەربڕین);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_پەیام_col = پەیام_col;
    save_پەیام_row = پەیام_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    إذا (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(دەربڕین, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    پەیام_col = save_پەیام_col;
    پەیام_row = save_پەیام_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(دەربڕین);

    إذا (p == عدم)
	أرجع عدم;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    أرجع res;
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
    إذا (res == عدم)
	أرجع عدم;

    d = res;
    لكل (s = p; *s != NUL; )
    {
	إذا ((s[0] == K_SPECIAL
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
	وإلا
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    أرجع res;
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

    طالما (*s != NUL)
    {
	إذا (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	وإلا إذا ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (صحيح)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	وإلا
	    *d++ = *s++;
    }
    *d = NUL;
    أرجع (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    صحيح
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    حرف	*cmd;
    صحيح		abbr;
    صحيح		hash;
    صحيح		did_cpo = FALSE;
    صحيح		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    لكل (abbr = 0; abbr < 2; ++abbr)
	لكل (hash = 0; hash < 256; ++hash)
	{
	    إذا (abbr)
	    {
		إذا (hash > 0)		// there is only one abbr list
		    اكسر;
		إذا (buf != عدم)
		    mp = buf->b_first_abbr;
		وإلا
		    mp = first_abbr;
	    }
	    وإلا
	    {
		إذا (buf != عدم)
		    mp = buf->b_maphash[hash];
		وإلا
		    mp = maphash[hash];
	    }

	    لكل ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		إذا (mp->m_simplified)
		    استمر;

		// skip script-local mappings
		إذا (mp->m_noremap == REMAP_SCRIPT)
		    استمر;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		لكل (p = mp->m_str; *p != NUL; ++p)
		    إذا (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (صحيح)KE_SNR)
			اكسر;
		إذا (*p != NUL)
		    استمر;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		إذا (abbr)
		    cmd = "abbr";
		وإلا
		    cmd = "map";
		بدل (mp->m_mode)
		{
		    حالة MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			اكسر;
		    حالة MODE_NORMAL:
			c1 = 'n';
			اكسر;
		    حالة MODE_VISUAL:
			c1 = 'x';
			اكسر;
		    حالة MODE_SELECT:
			c1 = 's';
			اكسر;
		    حالة MODE_OP_PENDING:
			c1 = 'o';
			اكسر;
		    حالة MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			اكسر;
		    حالة MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			اكسر;
		    حالة MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			اكسر;
		    حالة MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			اكسر;
		    حالة MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			اكسر;
		    حالة MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			اكسر;
		    حالة MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			اكسر;
		    حالة MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			اكسر;
		    حالة MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			اكسر;
		    حالة MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			اكسر;
		    حالة MODE_CMDLINE | MODE_زیادکردن:
			إذا (!abbr)
			    cmd = "map!";
			اكسر;
		    حالة MODE_CMDLINE:
			c1 = 'c';
			اكسر;
		    حالة MODE_زیادکردن:
			c1 = 'i';
			اكسر;
		    حالة MODE_LANGMAP:
			c1 = 'l';
			اكسر;
		    حالة MODE_TERMINAL:
			c1 = 't';
			اكسر;
		    افتراضي:
			iemsg(e_makemap_illegal_mode);
			أرجع FAIL;
		}
		افعل	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    إذا (!did_cpo)
		    {
			إذا (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			وإلا
			    لكل (i = 0; i < 2; ++i)
				لكل (p = (i ? mp->m_str : mp->m_کلیلەکان); *p; ++p)
				    إذا (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			إذا (did_cpo)
			{
			    إذا (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				أرجع FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    إذا (mp->m_دەربڕین && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && fputs("legacy ", fd) < 0)
			أرجع FAIL;
#endif
		    إذا (c1 && putc(c1, fd) < 0)
			أرجع FAIL;
		    إذا (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			أرجع FAIL;
		    إذا (fputs(cmd, fd) < 0)
			أرجع FAIL;
		    إذا (buf != عدم && fputs(" <buffer>", fd) < 0)
			أرجع FAIL;
		    إذا (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			أرجع FAIL;
		    إذا (mp->m_silent && fputs(" <silent>", fd) < 0)
			أرجع FAIL;
#ifdef FEAT_EVAL
		    إذا (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			أرجع FAIL;
		    إذا (mp->m_دەربڕین && fputs(" <expr>", fd) < 0)
			أرجع FAIL;
#endif

		    إذا (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_کلیلەکان, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			أرجع FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} طالما (c1 != NUL);
	    }
	}

    إذا (did_cpo)
	إذا (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    أرجع FAIL;
    أرجع OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    صحيح
put_escstr(FILE *fd, char_u *strstart, صحيح what)
{
    char_u	*str = strstart;
    صحيح		c;
    صحيح		modifiers;

    // :map xx <Nop>
    إذا (*str == NUL && what == 1)
    {
	إذا (fprintf(fd, "<Nop>") < 0)
	    أرجع FAIL;
	أرجع OK;
    }

    لكل ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	إذا (p != عدم)
	{
	    طالما (*p != NUL)
		إذا (fputc(*p++, fd) < 0)
		    أرجع FAIL;
	    --str;
	    استمر;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	إذا (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    إذا (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		إذا (p == عدم)
		    c = *str;
		وإلا
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    إذا (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    إذا (IS_SPECIAL(c) || modifiers)	// special key
	    {
		إذا (fputs((حرف *)get_special_کلیل_name(c, modifiers), fd) < 0)
		    أرجع FAIL;
		استمر;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	إذا (c == NL)
	{
	    إذا (what == 2)
	    {
		إذا (fprintf(fd, "\\\026\n") < 0)
		    أرجع FAIL;
	    }
	    وإلا
	    {
		إذا (fprintf(fd, "<NL>") < 0)
		    أرجع FAIL;
	    }
	    استمر;
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
	إذا (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    إذا (putc('\\', fd) < 0)
		أرجع FAIL;
	}
	وإلا إذا (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    إذا (putc(Ctrl_V, fd) < 0)
		أرجع FAIL;
	}
	إذا (putc(c, fd) < 0)
	    أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    فراغ
check_map_keycodes(فراغ)
{
    mapblock_T	*mp;
    char_u	*p;
    صحيح		i;
    char_u	buf[3];
    صحيح		abbr;
    صحيح		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_پاڵنان(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    لكل (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	لكل (abbr = 0; abbr <= 1; ++abbr)
	    لكل (hash = 0; hash < 256; ++hash)
	    {
		إذا (abbr)
		{
		    إذا (hash)	    // there is only one abbr list
			اكسر;
		    إذا (bp != عدم)
			mp = bp->b_first_abbr;
		    وإلا
			mp = first_abbr;
		}
		وإلا
		{
		    إذا (bp != عدم)
			mp = bp->b_maphash[hash];
		    وإلا
			mp = maphash[hash];
		}
		لكل ( ; mp != عدم; mp = mp->m_next)
		{
		    لكل (i = 0; i <= 1; ++i)	// do this twice
		    {
			إذا (i == 0)
			    p = mp->m_کلیلەکان;	// once for the "from" part
			وإلا
			    p = mp->m_str;	// and once for the "to" part
			طالما (*p)
			{
			    إذا (*p == K_SPECIAL)
			    {
				++p;
				إذا (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (فراغ)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	إذا (bp == عدم)
	    اكسر;
    }
    ESTACK_CHECK_NOW;
    estack_دەرهێنان();
}

#إذا defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*کلیلەکان,
    صحيح		mode,
    صحيح		exact,		// require exact match
    صحيح		ign_mod,	// ignore preceding modifier
    صحيح		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    صحيح		*local_ptr)	// return: buffer-local mapping or NULL
{
    صحيح		hash;
    صحيح		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    صحيح		local;

    validate_maphash();

    len = (صحيح)STRLEN(کلیلەکان);
    لكل (local = 1; local >= 0; --local)
	// loop over all hash lists
	لكل (hash = 0; hash < 256; ++hash)
	{
	    إذا (abbr)
	    {
		إذا (hash > 0)		// there is only one list.
		    اكسر;
		إذا (local)
		    mp = curbuf->b_first_abbr;
		وإلا
		    mp = first_abbr;
	    }
	    وإلا إذا (local)
		mp = curbuf->b_maphash[hash];
	    وإلا
		mp = maphash[hash];
	    لكل ( ; mp != عدم; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		إذا ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    إذا (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    وإلا
			minlen = len;
		    s = mp->m_کلیلەکان;
		    إذا (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			إذا (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    إذا (STRNCMP(s, کلیلەکان, minlen) == 0)
		    {
			إذا (mp_ptr != عدم)
			    *mp_ptr = mp;
			إذا (local_ptr != عدم)
			    *local_ptr = local;
			أرجع mp->m_str;
		    }
		}
	    }
	}

    أرجع عدم;
}

/*
 * "hasmapto()" function
 */
    فراغ
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*mode;
    char_u	buf[NUMBUFLEN];
    صحيح		abbr = FALSE;

    إذا (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	أرجع;

    name = tv_get_string(&argvars[0]);
    إذا (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    وإلا
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (صحيح)tv_get_bool(&argvars[2]);
    }

    إذا (map_to_exists(name, mode, abbr))
	rettv->vval.v_ژمار = TRUE;
    وإلا
	rettv->vval.v_ژمار = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    ساكن فراغ
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	صحيح	    buffer_local,   // false if not buffer local mapping
	صحيح	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_کلیلەکان, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_کلیلەکان);
    إذا (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_ژمار(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_ژمار(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_ژمار(dict, "expr", mp->m_دەربڕین ? 1L : 0L);
    dict_add_ژمار(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_add_ژمار(dict, "sid", (long)mp->m_script_ctx.sc_sid);
    dict_add_ژمار(dict, "scriptversion",
		    (long)mp->m_script_ctx.sc_version);
    dict_add_ژمار(dict, "lnum", (long)mp->m_script_ctx.sc_lnum);
    dict_add_ژمار(dict, "buffer", (long)buffer_local);
    dict_add_ژمار(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_add_string(dict, "mode", mapmode);
    dict_add_ژمار(dict, "abbr", abbr ? 1L : 0L);
    dict_add_ژمار(dict, "mode_bits", mp->m_mode);

    vim_free(mapmode);
}

    ساكن فراغ
get_maparg(typval_T *argvars, typval_T *rettv, صحيح exact)
{
    char_u	*کلیلەکان;
    char_u	*کلیلەکان_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*کلیلەکان_buf = عدم;
    char_u	*alt_کلیلەکان_buf = عدم;
    صحيح		did_simplify = FALSE;
    char_u	*rhs;
    صحيح		mode;
    صحيح		abbr = FALSE;
    صحيح		get_dict = FALSE;
    mapblock_T	*mp = عدم;
    صحيح		buffer_local;
    صحيح		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    کلیلەکان = tv_get_string(&argvars[0]);
    إذا (*کلیلەکان == NUL)
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (صحيح)tv_get_bool(&argvars[2]);
	    إذا (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (صحيح)tv_get_bool(&argvars[3]);
	}
    }
    وإلا
	which = (char_u *)"";
    إذا (which == عدم)
	أرجع;

    mode = get_map_mode(&which, 0);

    کلیلەکان_simplified = replace_termcodes(کلیلەکان, &کلیلەکان_buf, 0, flags,
								&did_simplify);
    rhs = check_map(کلیلەکان_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    إذا (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(فراغ)replace_termcodes(کلیلەکان, &alt_کلیلەکان_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, عدم);
	rhs = check_map(alt_کلیلەکان_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    إذا (!get_dict)
    {
	// Return a string.
	إذا (rhs != عدم)
	{
	    إذا (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    وإلا
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    وإلا إذا (rettv_dict_alloc(rettv) == OK && rhs != عدم)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? کلیلەکان_simplified : عدم,
			  buffer_local, abbr);

    vim_free(کلیلەکان_buf);
    vim_free(alt_کلیلەکان_buf);
}

/*
 * "maplist()" function
 */
    فراغ
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    صحيح		buffer_local;
    char_u	*کلیلەکان_buf;
    صحيح		did_simplify;
    صحيح		hash;
    char_u	*lhs;
    ثابت صحيح	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    صحيح		abbr = FALSE;

    إذا (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	أرجع;
    إذا (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    لكل (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	لكل (hash = 0; hash < 256; ++hash)
	{
	    إذا (abbr)
	    {
		إذا (hash > 0)		// there is only one abbr list
		    اكسر;
		إذا (buffer_local)
		    mp = curbuf->b_first_abbr;
		وإلا
		    mp = first_abbr;
	    }
	    وإلا إذا (buffer_local)
		mp = curbuf->b_maphash[hash];
	    وإلا
		mp = maphash[hash];
	    لكل (; mp; mp = mp->m_next)
	    {
		إذا (mp->m_simplified)
		    استمر;
		إذا ((d = dict_alloc()) == عدم)
		    أرجع;
		إذا (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    أرجع;
		}

		کلیلەکان_buf = عدم;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_کلیلەکان, TRUE, FALSE);
		(فراغ)replace_termcodes(lhs, &کلیلەکان_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? کلیلەکان_buf : عدم,
				 buffer_local, abbr);
		vim_free(کلیلەکان_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    فراغ
f_maparg(typval_T *argvars, typval_T *rettv)
{
    إذا (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		أرجع;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    فراغ
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    إذا (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	أرجع;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    ساكن صحيح
get_map_mode_string(char_u *mode_string, صحيح abbr)
{
    char_u	*p = mode_string;
    صحيح		mode = 0;
    صحيح		tmode;
    صحيح		modec;
    ثابت صحيح	MASK_V = MODE_VISUAL | MODE_SELECT;
    ثابت صحيح	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    ثابت صحيح	MASK_BANG = MODE_زیادکردن | MODE_CMDLINE;

    إذا (*p == NUL)
	p = (char_u *)" ";	// compatibility
    طالما ((modec = *p++))
    {
	بدل (modec)
	{
	    حالة 'i': tmode = MODE_زیادکردن;	اكسر;
	    حالة 'l': tmode = MODE_LANGMAP;	اكسر;
	    حالة 'c': tmode = MODE_CMDLINE;	اكسر;
	    حالة 'n': tmode = MODE_NORMAL;	اكسر;
	    حالة 'x': tmode = MODE_VISUAL;	اكسر;
	    حالة 's': tmode = MODE_SELECT;	اكسر;
	    حالة 'o': tmode = MODE_OP_PENDING;	اكسر;
	    حالة 't': tmode = MODE_TERMINAL;	اكسر;
	    حالة 'v': tmode = MASK_V;		اكسر;
	    حالة '!': tmode = MASK_BANG;	اكسر;
	    حالة ' ': tmode = MASK_MAP;		اكسر;
	    افتراضي:
		      أرجع 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    إذا ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	أرجع 0;

    أرجع mode;
}

/*
 * "mapset()" function
 */
    فراغ
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    صحيح		mode;
    char_u	buf[NUMBUFLEN];
    صحيح		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = عدم;
    صحيح		noremap;
    صحيح		دەربڕین;
    صحيح		silent;
    صحيح		buffer;
    scid_T	sid;
    صحيح		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    صحيح		nowait;
    char_u	*arg;
    صحيح		dict_only;
    mapblock_T	*mp_result[2] = {عدم, عدم};

    إذا (check_secure())
	أرجع;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    إذا (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	أرجع;

    إذا (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	إذا (which == عدم || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    أرجع;
	}
    }
    وإلا
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	إذا (which == عدم)
	    أرجع;
	is_abbr = (صحيح)tv_get_bool(&argvars[1]);

	إذا (check_for_dict_arg(argvars, 2) == FAIL)
	    أرجع;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    إذا (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	أرجع;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    إذا (lhs == عدم || lhsraw == عدم || rhs == عدم)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	أرجع;
    }
    orig_rhs = rhs;

    noremap = dict_get_ژمار(d, "noremap") ? REMAP_NONE: 0;
    إذا (dict_get_ژمار(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    دەربڕین = dict_get_ژمار(d, "expr") != 0;
    silent = dict_get_ژمار(d, "silent") != 0;
    sid = dict_get_ژمار(d, "sid");
    scriptversion = dict_get_ژمار(d, "scriptversion");
    lnum = dict_get_ژمار(d, "lnum");
    buffer = dict_get_ژمار(d, "buffer");
    nowait = dict_get_ژمار(d, "nowait") != 0;
    // mode from the dict is not used

    إذا (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    وإلا
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, عدم);

    إذا (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    إذا (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	إذا (arg == عدم)
	    أرجع;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    وإلا
    {
	arg = vim_strsave(lhs);
	إذا (arg == عدم)
	    أرجع;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, دەربڕین, sid,
			    scriptversion, lnum, 0);
    إذا (lhsrawalt != عدم)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, دەربڕین, sid,
			    scriptversion, lnum, 1);

    إذا (mp_result[0] != عدم && mp_result[1] != عدم)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#إذا defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
بنية initmap
{
    char_u	*arg;
    صحيح		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
ساكن بنية initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_زیادکردن | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# إذا defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
ساكن بنية initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_زیادکردن | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_زیادکردن | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_زیادکردن},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  وإلا
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_زیادکردن}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# إذا defined(MACOS_X)
ساكن بنية initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_زیادکردن | MODE_CMDLINE},
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
    فراغ
init_mappings(فراغ)
{
#إذا defined(MSWIN) || defined(MACOS_X)
    صحيح		i;

# إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    إذا (!gui.starting)
#  endif
    {
	لكل (i = 0; i < (صحيح)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# إذا defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    لكل (i = 0; i < (صحيح)ARRAY_LENGTH(initmappings); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode, FALSE);
# endif
#endif
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    فراغ
add_map(char_u *map, صحيح mode, صحيح nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    إذا (s != عدم)
    {
	(فراغ)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#إذا defined(FEAT_LANGMAP)
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
تعريفنوع بنية
{
    صحيح	    from;
    صحيح     to;
} langmap_entry_T;

ساكن garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    ساكن فراغ
langmap_set_entry(صحيح from, صحيح to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    صحيح		    a = 0;
    صحيح		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    طالما (a != b)
    {
	صحيح i = (a + b) / 2;
	صحيح d = entries[i].from - from;

	إذا (d == 0)
	{
	    entries[i].to = to;
	    أرجع;
	}
	إذا (d < 0)
	    a = i + 1;
	وإلا
	    b = i;
    }

    إذا (ga_grow(&langmap_mapga, 1) == FAIL)
	أرجع;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * حجم(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    صحيح
langmap_adjust_mb(صحيح c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    صحيح a = 0;
    صحيح b = langmap_mapga.ga_len;

    طالما (a != b)
    {
	صحيح i = (a + b) / 2;
	صحيح d = entries[i].from - c;

	إذا (d == 0)
	    أرجع entries[i].to;  // found matching entry
	إذا (d < 0)
	    a = i + 1;
	وإلا
	    b = i;
    }
    أرجع c;  // no entry found, return "c" unmodified
}

    فراغ
langmap_init(فراغ)
{
    صحيح i;

    لكل (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, حجم(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    حرف *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    صحيح	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    لكل (p = p_langmap; p[0] != NUL; )
    {
	لكل (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    إذا (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	إذا (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	وإلا
	    p2 = عدم;	    // aAbBcCdD form, p2 is NULL
	طالما (p[0])
	{
	    إذا (p[0] == ',')
	    {
		++p;
		اكسر;
	    }
	    إذا (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    إذا (p2 == عدم)
	    {
		MB_PTR_ADV(p);
		إذا (p[0] != ',')
		{
		    إذا (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    وإلا
	    {
		إذا (p2[0] != ',')
		{
		    إذا (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    إذا (to == NUL)
	    {
		sprintf(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		أرجع args->os_errbuf;
	    }

	    إذا (from >= 256)
		langmap_set_entry(from, to);
	    وإلا
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    إذا (p2 != عدم)
	    {
		MB_PTR_ADV(p2);
		إذا (*p == ';')
		{
		    p = p2;
		    إذا (p[0] != NUL)
		    {
			إذا (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    أرجع args->os_errbuf;
			}
			++p;
		    }
		    اكسر;
		}
	    }
	}
    }

    أرجع عدم;
}
#endif

    ساكن فراغ
do_exmap(exarg_T *eap, صحيح isabbrev)
{
    صحيح	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    بدل (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	حالة 1: emsg(_(e_invalid_argument));
		اكسر;
	حالة 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		اكسر;
    }
}

/*
 * ":abbreviate" and friends.
 */
    فراغ
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    فراغ
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    إذا (secure)
    {
	secure = 2;
	پەیام_outtrans(eap->cmd);
	پەیام_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    فراغ
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    فراغ
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    فراغ
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
