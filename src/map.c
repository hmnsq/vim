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

#شامل "vim.h"

/*
 * List used for abbreviations.
 */
ایستا mapblock_T	*first_abbr = پوچ; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
ایستا mapblock_T	*(maphash[256]);
ایستا صحیح		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
ایستا صحیح		map_locked = 0;

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
get_maphash_list(صحیح state, صحیح c)
{
    بازگشت maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(صحیح state, صحیح c)
{
    بازگشت curbuf->b_maphash[MAP_HASH(state, c)];
}

    صحیح
is_maphash_valid(تهی)
{
    بازگشت maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    ایستا تهی
validate_maphash(تهی)
{
    اگر (maphash_valid)
	بازگشت;

    CLEAR_FIELD(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    ایستا تهی
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_کلیدها);
    اگر (mp->m_alt != پوچ)
	mp->m_alt->m_alt = پوچ;
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
    ایستا char_u *
map_mode_to_chars(صحیح mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    اگر ((mode & (MODE_درج | MODE_CMDLINE)) == (MODE_درج | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    وگرنه اگر (mode & MODE_درج)
	ga_append(&mapmode, 'i');			// :imap
    وگرنه اگر (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    وگرنه اگر (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    وگرنه اگر ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    وگرنه
    {
	اگر (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	اگر (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	اگر (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	اگر ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	وگرنه
	{
	    اگر (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    اگر (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    بازگشت (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    ایستا تهی
showmap(
    mapblock_T	*mp,
    صحیح		local)	    // TRUE for buffer-local map
{
    صحیح		len = 1;
    char_u	*mapchars;

    اگر (message_filtered(mp->m_کلیدها) && message_filtered(mp->m_str))
	بازگشت;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    اگر (پیام_didout || پیام_silent != 0)
    {
	پیام_putchar('\n');
	اگر (got_int)	    // 'q' typed at MORE prompt
	    برو theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    اگر (mapchars != پوچ)
    {
	پیام_puts((نویسه *)mapchars);
	len = (صحیح)STRLEN(mapchars);
	vim_free(mapchars);
    }

    وقتی (++len <= 3)
	پیام_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = پیام_outtrans_special(mp->m_کلیدها, TRUE, 0);
    انجامبده
    {
	پیام_putchar(' ');		// pad with blanks
	++len;
    } وقتی (len < 12);

    اگر (mp->m_noremap == REMAP_NONE)
	پیام_puts_attr("*", HL_ATTR(HLF_8));
    وگرنه اگر (mp->m_noremap == REMAP_SCRIPT)
	پیام_puts_attr("&", HL_ATTR(HLF_8));
    وگرنه
	پیام_putchar(' ');

    اگر (local)
	پیام_putchar('@');
    وگرنه
	پیام_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    اگر (*mp->m_str == NUL)
	پیام_puts_attr("<Nop>", HL_ATTR(HLF_8));
    وگرنه
	پیام_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    اگر (p_verbose > 0)
	last_set_پیام(mp->m_script_ctx);
#endif
    پیام_clr_eos();
    out_flush();			// show one line at a time

theend:
    --map_locked;
}

    ایستا mapblock_T *
map_add(
	mapblock_T  **map_table,
	mapblock_T  **abbr_table,
	char_u	    *کلیدها,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	صحیح	    noremap,
	صحیح	    nowait,
	صحیح	    silent,
	صحیح	    mode,
	صحیح	    is_abbr,
#ifdef FEAT_EVAL
	صحیح	    عبارت,
	scid_T	    sid,	    // 0 to use current_sctx
	صحیح	    scriptversion,
	linenr_T    lnum,
#endif
	صحیح	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    اگر (mp == پوچ)
	بازگشت پوچ;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    اگر (*کلیدها == Ctrl_C)
    {
	اگر (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	وگرنه
	    mapped_ctrl_c |= mode;
    }

    mp->m_کلیدها = vim_strsave(کلیدها);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    اگر (mp->m_کلیدها == پوچ || mp->m_str == پوچ)
    {
	vim_free(mp->m_کلیدها);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	بازگشت پوچ;
    }
    mp->m_keylen = (صحیح)STRLEN(mp->m_کلیدها);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_عبارت = عبارت;
    اگر (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    وگرنه
    {
	mp->m_script_ctx = current_sctx;
	اگر (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    اگر (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    وگرنه
    {
	صحیح n = MAP_HASH(mp->m_mode, mp->m_کلیدها[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    بازگشت mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    ایستا تهی
list_mappings(
	صحیح	keyround,
	صحیح	abbrev,
	صحیح	haskey,
	char_u	*کلیدها,
	صحیح	کلیدها_len,
	صحیح	mode,
	صحیح	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    اگر (p_verbose > 0 && keyround == 1)
    {
	اگر (seen_Modify_Other_کلیدها)
	    پیام_puts(_("Seen modifyOtherKeys: true\n"));

	اگر (modify_otherkeys_state != MOKS_INITIAL)
	{
	    نویسه *name = _("Unknown");
	    انتخاب (modify_otherkeys_state)
	    {
		حالت MOKS_INITIAL: بشکن;
		حالت MOKS_OFF: name = _("Off"); بشکن;
		حالت MOKS_ENABLED: name = _("On"); بشکن;
		حالت MOKS_DISABLED: name = _("Disabled"); بشکن;
		حالت MOKS_AFTER_T_TE: name = _("Cleared"); بشکن;
	    }

	    نویسه buf[200];
	    vim_snprintf(buf, اندازه(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    پیام_puts(buf);
	}

	اگر (kitty_protocol_state != KKPS_INITIAL)
	{
	    نویسه *name = _("Unknown");
	    انتخاب (kitty_protocol_state)
	    {
		حالت KKPS_INITIAL: بشکن;
		حالت KKPS_OFF: name = _("Off"); بشکن;
		حالت KKPS_ENABLED: name = _("On"); بشکن;
		حالت KKPS_DISABLED: name = _("Disabled"); بشکن;
		حالت KKPS_AFTER_T_TE: name = _("Cleared"); بشکن;
	    }

	    نویسه buf[200];
	    vim_snprintf(buf, اندازه(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    پیام_puts(buf);
	}
    }

    // need to loop over all global hash lists
    برای (صحیح hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	اگر (abbrev)
	{
	    اگر (hash != 0)	// there is only one abbreviation list
		بشکن;
	    mp = curbuf->b_first_abbr;
	}
	وگرنه
	    mp = curbuf->b_maphash[hash];
	برای ( ; mp != پوچ && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    اگر (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		اگر (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		وگرنه
		{
		    صحیح n = mp->m_keylen;
		    اگر (STRNCMP(mp->m_کلیدها, کلیدها,
				   (size_t)(n < کلیدها_len ? n : کلیدها_len)) == 0)
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
    صحیح
do_map(
    صحیح		maptype,
    char_u	*arg,
    صحیح		mode,
    صحیح		abbrev)		// not a mapping but an abbreviation
{
    char_u	*کلیدها;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {پوچ, پوچ};
    char_u	*rhs;
    char_u	*p;
    صحیح		n;
    صحیح		len = 0;	// init for GCC
    صحیح		hasarg;
    صحیح		haskey;
    صحیح		do_print;
    صحیح		keyround;
    char_u	*کلیدها_buf = پوچ;
    char_u	*alt_کلیدها_buf = پوچ;
    char_u	*arg_buf = پوچ;
    صحیح		retval = 0;
    صحیح		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    صحیح		unique = FALSE;
    صحیح		nowait = FALSE;
    صحیح		silent = FALSE;
    صحیح		special = FALSE;
#ifdef FEAT_EVAL
    صحیح		عبارت = FALSE;
#endif
    صحیح		did_simplify = FALSE;
    صحیح		unmap_lhs_only = FALSE;
    صحیح		noremap;
    char_u      *orig_rhs;

    کلیدها = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    اگر (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    اگر (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    وگرنه
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    برای (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	اگر (STRNCMP(کلیدها, "<buffer>", 8) == 0)
	{
	    کلیدها = skipwhite(کلیدها + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    ادامه;
	}

	// Check for "<nowait>": don't wait for more characters.
	اگر (STRNCMP(کلیدها, "<nowait>", 8) == 0)
	{
	    کلیدها = skipwhite(کلیدها + 8);
	    nowait = TRUE;
	    ادامه;
	}

	// Check for "<silent>": don't echo commands.
	اگر (STRNCMP(کلیدها, "<silent>", 8) == 0)
	{
	    کلیدها = skipwhite(کلیدها + 8);
	    silent = TRUE;
	    ادامه;
	}

	// Check for "<special>": accept special keys in <>
	اگر (STRNCMP(کلیدها, "<special>", 9) == 0)
	{
	    کلیدها = skipwhite(کلیدها + 9);
	    special = TRUE;
	    ادامه;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	اگر (STRNCMP(کلیدها, "<script>", 8) == 0)
	{
	    کلیدها = skipwhite(کلیدها + 8);
	    noremap = REMAP_SCRIPT;
	    ادامه;
	}

	// Check for "<expr>": {rhs} is an expression.
	اگر (STRNCMP(کلیدها, "<expr>", 6) == 0)
	{
	    کلیدها = skipwhite(کلیدها + 6);
	    عبارت = TRUE;
	    ادامه;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	اگر (STRNCMP(کلیدها, "<unique>", 8) == 0)
	{
	    کلیدها = skipwhite(کلیدها + 8);
	    unique = TRUE;
	    ادامه;
	}
	بشکن;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = کلیدها;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == پوچ);
    وقتی (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	اگر ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    اگر (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*کلیدها != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    اگر (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	برو theend;
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
    اگر (haskey)
    {
	char_u	*new_کلیدها;
	صحیح	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	اگر (special)
	    flags |= REPTERM_SPECIAL;
	new_کلیدها = replace_termcodes(کلیدها, &کلیدها_buf, 0, flags, &did_simplify);
	اگر (did_simplify)
	    (تهی)replace_termcodes(کلیدها, &alt_کلیدها_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, پوچ);
	کلیدها = new_کلیدها;
    }
    orig_rhs = rhs;
    اگر (hasarg)
    {
	اگر (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	وگرنه
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), پوچ);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    برای (keyround = 1; keyround <= 2; ++keyround)
    {
	صحیح	did_it = FALSE;
	صحیح	did_local = FALSE;
	صحیح	keyround1_simplified = keyround == 1 && did_simplify;
	صحیح	round;
	صحیح	رقم_rounds;

	اگر (keyround == 2)
	{
	    اگر (alt_کلیدها_buf == پوچ)
		بشکن;
	    کلیدها = alt_کلیدها_buf;
	}
	وگرنه اگر (alt_کلیدها_buf != پوچ && do_print)
	    // when printing always use the not-simplified map
	    کلیدها = alt_کلیدها_buf;

	// check arguments and translate function keys
	اگر (haskey)
	{
	    len = (صحیح)STRLEN(کلیدها);
	    اگر (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		برو theend;
	    }

	    اگر (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		اگر (has_mbyte)
		{
		    صحیح		first, last;
		    صحیح		same = -1;
		    char_u	کلیدها_unescaped[MAXMAPLEN + 1];
		    size_t	کلیدها_unescaped_len;

		    mch_memmove(کلیدها_unescaped, کلیدها, (size_t)(len + 1));
		    کلیدها_unescaped_len = vim_unescape_csi(کلیدها_unescaped);
		    p = کلیدها_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    وقتی (p < کلیدها_unescaped + کلیدها_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			اگر (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    اگر (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			برو theend;
		    }
		}
		وگرنه اگر (vim_iswordc(کلیدها[len - 1]))
		    // ends in keyword char
		    برای (n = 0; n < len - 2; ++n)
			اگر (vim_iswordc(کلیدها[n]) != vim_iswordc(کلیدها[len - 2]))
			{
			    retval = 1;
			    برو theend;
			}
		// An abbreviation cannot contain white space.
		برای (n = 0; n < len; ++n)
		    اگر (VIM_ISWHITE(کلیدها[n]))
		    {
			retval = 1;
			برو theend;
		    }
	    }
	}

	اگر (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	اگر (do_print)
	    پیام_start();

	// Check if a new local mapping wasn't already defined globally.
	اگر (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    برای (صحیح hash = 0; hash < 256 && !got_int; ++hash)
	    {
		اگر (abbrev)
		{
		    اگر (hash != 0)	// there is only one abbreviation list
			بشکن;
		    mp = first_abbr;
		}
		وگرنه
		    mp = maphash[hash];
		برای ( ; mp != پوچ && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    اگر ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_کلیدها, کلیدها, (size_t)len) == 0)
		    {
			اگر (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_کلیدها);
			وگرنه
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_کلیدها);
			retval = 5;
			برو theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	اگر (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, کلیدها, len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	رقم_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	برای (round = 0; round < رقم_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    برای (صحیح hash = 0; hash < 256 && !got_int; ++hash)
	    {
		اگر (abbrev)
		{
		    اگر (hash > 0)	// there is only one abbreviation list
			بشکن;
		    mpp = abbr_table;
		}
		وگرنه
		    mpp = &(map_table[hash]);
		برای (mp = *mpp; mp != پوچ && !got_int; mp = *mpp)
		{

		    اگر ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			ادامه;
		    }
		    اگر (!haskey)	// show all entries
		    {
			اگر (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    وگرنه	// do we have a match?
		    {
			اگر (round)	// second round: Try unmap "rhs" string
			{
			    n = (صحیح)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			وگرنه
			{
			    n = mp->m_keylen;
			    p = mp->m_کلیدها;
			}
			اگر (STRNCMP(p, کلیدها, (size_t)(n < len ? n : len)) == 0)
			{
			    اگر (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				اگر (n != len && (!abbrev || round || n > len
					       || *skipwhite(کلیدها + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    ادامه;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				اگر (keyround1_simplified && !mp->m_simplified)
				    بشکن;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    وگرنه اگر (!hasarg)	// show matching entry
			    {
				اگر (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    وگرنه اگر (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				ادامه;
			    }
			    وگرنه اگر (unique)
			    {
				اگر (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				وگرنه
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				برو theend;
			    }
			    وگرنه
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				اگر (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    اگر (newstr == پوچ)
				    {
					retval = 4;		// no mem
					برو theend;
				    }
				    اگر (mp->m_alt != پوچ)
					mp->m_alt = mp->m_alt->m_alt = پوچ;
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
				    mp->m_عبارت = عبارت;
				    mp->m_script_ctx = current_sctx;
				    اگر (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    اگر (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				ادامه;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    صحیح new_hash = MAP_HASH(mp->m_mode, mp->m_کلیدها[0]);
			    اگر (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_next;
				mp->m_next = map_table[new_hash];
				map_table[new_hash] = mp;

				ادامه;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_next);
		}
	    }
	}

	اگر (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    اگر (!did_it)
	    {
		اگر (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    وگرنه اگر (*کلیدها == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		اگر (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		وگرنه
		    mapped_ctrl_c &= ~mode;
	    }
	    ادامه;
	}

	اگر (!haskey || !hasarg)
	{
	    // print entries
	    اگر (!did_it && !did_local)
	    {
		اگر (abbrev)
		    پیام(_("No abbreviation found"));
		وگرنه
		    پیام(_("No mapping found"));
	    }
	    برو theend;    // listing finished
	}

	اگر (did_it)
	    ادامه;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, کلیدها,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    عبارت, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	اگر (mp_result[keyround - 1] == پوچ)
	{
	    retval = 4;	    // no mem
	    برو theend;
	}
    }

    اگر (mp_result[0] != پوچ && mp_result[1] != پوچ)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(کلیدها_buf);
    vim_free(alt_کلیدها_buf);
    vim_free(arg_buf);
    بازگشت retval;
}

/*
 * Get the mapping mode from the command name.
 */
    ایستا صحیح
get_map_mode(char_u **cmdp, صحیح forceit)
{
    char_u	*p;
    صحیح		modec;
    صحیح		mode;

    p = *cmdp;
    modec = *p++;
    اگر (modec == 'i')
	mode = MODE_درج;				// :imap
    وگرنه اگر (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    وگرنه اگر (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    وگرنه اگر (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    وگرنه اگر (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    وگرنه اگر (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    وگرنه اگر (modec == 's')
	mode = MODE_SELECT;			// :smap
    وگرنه اگر (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    وگرنه اگر (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    وگرنه
    {
	--p;
	اگر (forceit)
	    mode = MODE_درج | MODE_CMDLINE;		// :map !
	وگرنه
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    بازگشت mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    ایستا تهی
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    صحیح		forceit,
    صحیح		abbr)
{
    صحیح		mode;
    صحیح		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    اگر (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	بازگشت;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ایستا صحیح
is_map_locked(تهی)
{
    اگر (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    تهی
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    صحیح		mode,		// mode in which to delete
    صحیح		local,		// TRUE for buffer-local mappings
    صحیح		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    صحیح		hash;
    صحیح		new_hash;

    اگر (is_map_locked())
	بازگشت;

    validate_maphash();

    برای (hash = 0; hash < 256; ++hash)
    {
	اگر (abbr)
	{
	    اگر (hash > 0)	// there is only one abbrlist
		بشکن;
	    اگر (local)
		mpp = &buf->b_first_abbr;
	    وگرنه
		mpp = &first_abbr;
	}
	وگرنه
	{
	    اگر (local)
		mpp = &buf->b_maphash[hash];
	    وگرنه
		mpp = &maphash[hash];
	}
	وقتی (*mpp != پوچ)
	{
	    mp = *mpp;
	    اگر (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		اگر (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    ادامه;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_کلیدها[0]);
		اگر (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    اگر (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    وگرنه
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    ادامه;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

#اگر defined(FEAT_EVAL)
    صحیح
mode_str2flags(char_u *modechars)
{
    صحیح		mode = 0;

    اگر (vim_strchr(modechars, 'n') != پوچ)
	mode |= MODE_NORMAL;
    اگر (vim_strchr(modechars, 'v') != پوچ)
	mode |= MODE_VISUAL | MODE_SELECT;
    اگر (vim_strchr(modechars, 'x') != پوچ)
	mode |= MODE_VISUAL;
    اگر (vim_strchr(modechars, 's') != پوچ)
	mode |= MODE_SELECT;
    اگر (vim_strchr(modechars, 'o') != پوچ)
	mode |= MODE_OP_PENDING;
    اگر (vim_strchr(modechars, 'i') != پوچ)
	mode |= MODE_درج;
    اگر (vim_strchr(modechars, 'l') != پوچ)
	mode |= MODE_LANGMAP;
    اگر (vim_strchr(modechars, 'c') != پوچ)
	mode |= MODE_CMDLINE;

    بازگشت mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    صحیح
map_to_exists(char_u *str, char_u *modechars, صحیح abbr)
{
    char_u	*rhs;
    char_u	*buf;
    صحیح		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, پوچ);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    بازگشت retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    صحیح
map_to_exists_mode(char_u *rhs, صحیح mode, صحیح abbr)
{
    mapblock_T	*mp;
    صحیح		hash;
    صحیح		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    برای (;;)
    {
	برای (hash = 0; hash < 256; ++hash)
	{
	    اگر (abbr)
	    {
		اگر (hash > 0)		// there is only one abbr list
		    بشکن;
		اگر (exp_buffer)
		    mp = curbuf->b_first_abbr;
		وگرنه
		    mp = first_abbr;
	    }
	    وگرنه اگر (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    وگرنه
		mp = maphash[hash];
	    برای (; mp; mp = mp->m_next)
	    {
		اگر ((mp->m_mode & mode)
			&& strstr((نویسه *)mp->m_str, (نویسه *)rhs) != پوچ)
		    بازگشت TRUE;
	    }
	}
	اگر (exp_buffer)
	    بشکن;
	exp_buffer = TRUE;
    }

    بازگشت FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
ایستا صحیح	expand_mapmodes = 0;
ایستا صحیح	expand_isabbrev = 0;
ایستا صحیح	expand_buffer = FALSE;

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
    ایستا char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    صحیح		c;
    صحیح		modifiers;
    صحیح		cpo_bslash;
    صحیح		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != پوچ);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != پوچ);

    برای (; *str; ++str)
    {
	c = *str;
	اگر (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    اگر (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    اگر (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		اگر (cpo_special)
		{
		    ga_clear(&ga);
		    بازگشت پوچ;
		}
		c = TO_SPECIAL(str[1], str[2]);
		اگر (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    اگر (IS_SPECIAL(c) || modifiers)	// special key
	    {
		اگر (cpo_special)
		{
		    ga_clear(&ga);
		    بازگشت پوچ;
		}
		ga_concat(&ga, get_special_کلید_name(c, modifiers));
		ادامه; // for (str)
	    }
	}
	اگر (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	اگر (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    بازگشت (char_u *)(ga.ga_data);
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
    صحیح		forceit,	// TRUE if '!' given
    صحیح		isabbrev,	// TRUE if abbreviation
    صحیح		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    اگر (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    وگرنه
    {
	اگر (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	وگرنه
	{
	    expand_mapmodes = MODE_درج | MODE_CMDLINE;
	    اگر (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	برای (;;)
	{
	    اگر (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		ادامه;
	    }
	    اگر (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ادامه;
	    }
	    اگر (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ادامه;
	    }
	    اگر (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ادامه;
	    }
	    اگر (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		ادامه;
	    }
#ifdef FEAT_EVAL
	    اگر (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ادامه;
	    }
	    اگر (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		ادامه;
	    }
#endif
	    بشکن;
	}
	xp->xp_pattern = arg;
    }

    بازگشت پوچ;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    صحیح
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    صحیح		*رقم_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    صحیح		hash;
    صحیح		شمار;
    char_u	*p;
    صحیح		i;
    صحیح		fuzzy;
    صحیح		match;
    صحیح		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *رقم_Matches = 0;		    // return values in case of FAIL
    *matches = پوچ;

    اگر (!fuzzy)
	ga_init2(&ga, اندازه(نویسه *), 3);
    وگرنه
	ga_init2(&ga, اندازه(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    برای (i = 0; i < 7; ++i)
    {
	اگر (i == 0)
	    p = (char_u *)"<silent>";
	وگرنه اگر (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	وگرنه اگر (i == 2)
	    p = (char_u *)"<script>";
	وگرنه اگر (i == 3)
	    p = (char_u *)"<expr>";
#endif
	وگرنه اگر (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	وگرنه اگر (i == 5)
	    p = (char_u *)"<nowait>";
	وگرنه اگر (i == 6)
	    p = (char_u *)"<special>";
	وگرنه
	    ادامه;

	اگر (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	وگرنه
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	اگر (!match)
	    ادامه;

	اگر (ga_grow(&ga, 1) == FAIL)
	    بشکن;

	اگر (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	وگرنه
	    ((char_u **)ga.ga_data)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    برای (hash = 0; hash < 256; ++hash)
    {
	اگر (expand_isabbrev)
	{
	    اگر (hash > 0)	// only one abbrev list
		بشکن; // for (hash)
	    mp = first_abbr;
	}
	وگرنه اگر (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	وگرنه
	    mp = maphash[hash];
	برای (; mp; mp = mp->m_next)
	{
	    اگر (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		ادامه;

	    p = translate_mapping(mp->m_کلیدها);
	    اگر (p == پوچ)
		ادامه;

	    اگر (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    وگرنه
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    اگر (!match)
	    {
		vim_free(p);
		ادامه;
	    }

	    اگر (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		بشکن;
	    }

	    اگر (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    وگرنه
		((char_u **)ga.ga_data)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    اگر (ga.ga_len == 0)
	بازگشت FAIL;

    اگر (!fuzzy)
    {
	*matches = ga.ga_data;
	*رقم_Matches = ga.ga_len;
    }
    وگرنه
    {
	اگر (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    بازگشت FAIL;
	*رقم_Matches = ga.ga_len;
    }

    شمار = *رقم_Matches;
    اگر (شمار > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	اگر (!fuzzy)
	    sort_strings(*matches, شمار);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + شمار;

	وقتی (ptr2 < ptr3)
	{
	    اگر (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    وگرنه
	    {
		vim_free(*ptr2++);
		شمار--;
	    }
	}
    }

    *رقم_Matches = شمار;
    بازگشت (شمار == 0 ? FAIL : OK);
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
    صحیح
check_abbr(
    صحیح		c,
    char_u	*ptr,
    صحیح		col,
    صحیح		mincol)
{
    صحیح		len;
    صحیح		scol;		// starting column of the abbr.
    صحیح		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    صحیح		clen = 0;	// length in characters
    صحیح		is_id = TRUE;
    صحیح		vim_abbr;

    اگر (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	بازگشت FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    اگر (noremap_کلیدها() && c != Ctrl_RSB)
	بازگشت FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    اگر (col == 0)				// cannot be an abbr.
	بازگشت FALSE;

    اگر (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	اگر (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	وگرنه
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    اگر (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	وقتی (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    اگر (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		بشکن;
	    }
	    ++clen;
	}
	scol = (صحیح)(p - ptr);
    }
    وگرنه
    {
	اگر (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	وگرنه
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    اگر (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	برای (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    اگر (scol < mincol)
	scol = mincol;
    اگر (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	اگر (mp == پوچ)
	{
	    mp = mp2;
	    mp2 = پوچ;
	}
	برای ( ; mp; mp->m_next == پوچ
				  ? (mp = mp2, mp2 = پوچ) : (mp = mp->m_next))
	{
	    صحیح		qlen = mp->m_keylen;
	    char_u	*q = mp->m_کلیدها;
	    صحیح		match;

	    اگر (vim_strbyte(mp->m_کلیدها, K_SPECIAL) != پوچ)
	    {
		char_u *qe = vim_strsave(mp->m_کلیدها);

		// might have CSI escaped mp->m_keys
		اگر (qe != پوچ)
		{
		    q = qe;
		    qlen = (صحیح)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    اگر (q != mp->m_کلیدها)
		vim_free(q);
	    اگر (match)
		بشکن;
	}
	اگر (mp != پوچ)
	{
	    صحیح	noremap;
	    صحیح silent;
#ifdef FEAT_EVAL
	    صحیح عبارت;
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
	    اگر (c != Ctrl_RSB)
	    {
					// special key code, split up
		اگر (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		وگرنه
		{
		    اگر (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    اگر (has_mbyte)
		    {
			صحیح	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			اگر (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			اگر (escaped != پوچ)
			{
			    newlen = (صحیح)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    وگرنه
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(تهی)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ifdef FEAT_EVAL
	    عبارت = mp->m_عبارت;

	    اگر (عبارت)
		s = eval_map_عبارت(mp, c);
	    وگرنه
#endif
		s = mp->m_str;
	    اگر (s != پوچ)
	    {
					// insert the to string
		(تهی)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (صحیح)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		اگر (عبارت)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    اگر (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    وقتی (len-- > 0)		// delete the from string
		(تهی)ins_typebuf(tb, 1, 0, TRUE, silent);
	    بازگشت TRUE;
	}
    }
    بازگشت FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_عبارت(
    mapblock_T	*mp,
    صحیح		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*عبارت;
    pos_T	save_cursor;
    صحیح		save_پیام_col;
    صحیح		save_پیام_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    صحیح		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    عبارت = vim_strsave(mp->m_str);
    اگر (عبارت == پوچ)
	بازگشت پوچ;
    vim_unescape_csi(عبارت);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_پیام_col = پیام_col;
    save_پیام_row = پیام_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    اگر (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(عبارت, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    پیام_col = save_پیام_col;
    پیام_row = save_پیام_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(عبارت);

    اگر (p == پوچ)
	بازگشت پوچ;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    بازگشت res;
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
    اگر (res == پوچ)
	بازگشت پوچ;

    d = res;
    برای (s = p; *s != NUL; )
    {
	اگر ((s[0] == K_SPECIAL
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
	وگرنه
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    بازگشت res;
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

    وقتی (*s != NUL)
    {
	اگر (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	وگرنه اگر ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (صحیح)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	وگرنه
	    *d++ = *s++;
    }
    *d = NUL;
    بازگشت (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    صحیح
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    نویسه	*cmd;
    صحیح		abbr;
    صحیح		hash;
    صحیح		did_cpo = FALSE;
    صحیح		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    برای (abbr = 0; abbr < 2; ++abbr)
	برای (hash = 0; hash < 256; ++hash)
	{
	    اگر (abbr)
	    {
		اگر (hash > 0)		// there is only one abbr list
		    بشکن;
		اگر (buf != پوچ)
		    mp = buf->b_first_abbr;
		وگرنه
		    mp = first_abbr;
	    }
	    وگرنه
	    {
		اگر (buf != پوچ)
		    mp = buf->b_maphash[hash];
		وگرنه
		    mp = maphash[hash];
	    }

	    برای ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		اگر (mp->m_simplified)
		    ادامه;

		// skip script-local mappings
		اگر (mp->m_noremap == REMAP_SCRIPT)
		    ادامه;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		برای (p = mp->m_str; *p != NUL; ++p)
		    اگر (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (صحیح)KE_SNR)
			بشکن;
		اگر (*p != NUL)
		    ادامه;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		اگر (abbr)
		    cmd = "abbr";
		وگرنه
		    cmd = "map";
		انتخاب (mp->m_mode)
		{
		    حالت MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			بشکن;
		    حالت MODE_NORMAL:
			c1 = 'n';
			بشکن;
		    حالت MODE_VISUAL:
			c1 = 'x';
			بشکن;
		    حالت MODE_SELECT:
			c1 = 's';
			بشکن;
		    حالت MODE_OP_PENDING:
			c1 = 'o';
			بشکن;
		    حالت MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			بشکن;
		    حالت MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			بشکن;
		    حالت MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			بشکن;
		    حالت MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			بشکن;
		    حالت MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			بشکن;
		    حالت MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			بشکن;
		    حالت MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			بشکن;
		    حالت MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			بشکن;
		    حالت MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			بشکن;
		    حالت MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			بشکن;
		    حالت MODE_CMDLINE | MODE_درج:
			اگر (!abbr)
			    cmd = "map!";
			بشکن;
		    حالت MODE_CMDLINE:
			c1 = 'c';
			بشکن;
		    حالت MODE_درج:
			c1 = 'i';
			بشکن;
		    حالت MODE_LANGMAP:
			c1 = 'l';
			بشکن;
		    حالت MODE_TERMINAL:
			c1 = 't';
			بشکن;
		    پیشفرض:
			iemsg(e_makemap_illegal_mode);
			بازگشت FAIL;
		}
		انجامبده	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    اگر (!did_cpo)
		    {
			اگر (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			وگرنه
			    برای (i = 0; i < 2; ++i)
				برای (p = (i ? mp->m_str : mp->m_کلیدها); *p; ++p)
				    اگر (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			اگر (did_cpo)
			{
			    اگر (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				بازگشت FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    اگر (mp->m_عبارت && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && fputs("legacy ", fd) < 0)
			بازگشت FAIL;
#endif
		    اگر (c1 && putc(c1, fd) < 0)
			بازگشت FAIL;
		    اگر (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			بازگشت FAIL;
		    اگر (fputs(cmd, fd) < 0)
			بازگشت FAIL;
		    اگر (buf != پوچ && fputs(" <buffer>", fd) < 0)
			بازگشت FAIL;
		    اگر (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			بازگشت FAIL;
		    اگر (mp->m_silent && fputs(" <silent>", fd) < 0)
			بازگشت FAIL;
#ifdef FEAT_EVAL
		    اگر (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			بازگشت FAIL;
		    اگر (mp->m_عبارت && fputs(" <expr>", fd) < 0)
			بازگشت FAIL;
#endif

		    اگر (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_کلیدها, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			بازگشت FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} وقتی (c1 != NUL);
	    }
	}

    اگر (did_cpo)
	اگر (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    بازگشت FAIL;
    بازگشت OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
put_escstr(FILE *fd, char_u *strstart, صحیح what)
{
    char_u	*str = strstart;
    صحیح		c;
    صحیح		modifiers;

    // :map xx <Nop>
    اگر (*str == NUL && what == 1)
    {
	اگر (fprintf(fd, "<Nop>") < 0)
	    بازگشت FAIL;
	بازگشت OK;
    }

    برای ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	اگر (p != پوچ)
	{
	    وقتی (*p != NUL)
		اگر (fputc(*p++, fd) < 0)
		    بازگشت FAIL;
	    --str;
	    ادامه;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	اگر (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    اگر (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		اگر (p == پوچ)
		    c = *str;
		وگرنه
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    اگر (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    اگر (IS_SPECIAL(c) || modifiers)	// special key
	    {
		اگر (fputs((نویسه *)get_special_کلید_name(c, modifiers), fd) < 0)
		    بازگشت FAIL;
		ادامه;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	اگر (c == NL)
	{
	    اگر (what == 2)
	    {
		اگر (fprintf(fd, "\\\026\n") < 0)
		    بازگشت FAIL;
	    }
	    وگرنه
	    {
		اگر (fprintf(fd, "<NL>") < 0)
		    بازگشت FAIL;
	    }
	    ادامه;
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
	اگر (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    اگر (putc('\\', fd) < 0)
		بازگشت FAIL;
	}
	وگرنه اگر (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    اگر (putc(Ctrl_V, fd) < 0)
		بازگشت FAIL;
	}
	اگر (putc(c, fd) < 0)
	    بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    تهی
check_map_keycodes(تهی)
{
    mapblock_T	*mp;
    char_u	*p;
    صحیح		i;
    char_u	buf[3];
    صحیح		abbr;
    صحیح		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_افزودن(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    برای (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	برای (abbr = 0; abbr <= 1; ++abbr)
	    برای (hash = 0; hash < 256; ++hash)
	    {
		اگر (abbr)
		{
		    اگر (hash)	    // there is only one abbr list
			بشکن;
		    اگر (bp != پوچ)
			mp = bp->b_first_abbr;
		    وگرنه
			mp = first_abbr;
		}
		وگرنه
		{
		    اگر (bp != پوچ)
			mp = bp->b_maphash[hash];
		    وگرنه
			mp = maphash[hash];
		}
		برای ( ; mp != پوچ; mp = mp->m_next)
		{
		    برای (i = 0; i <= 1; ++i)	// do this twice
		    {
			اگر (i == 0)
			    p = mp->m_کلیدها;	// once for the "from" part
			وگرنه
			    p = mp->m_str;	// and once for the "to" part
			وقتی (*p)
			{
			    اگر (*p == K_SPECIAL)
			    {
				++p;
				اگر (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (تهی)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	اگر (bp == پوچ)
	    بشکن;
    }
    ESTACK_CHECK_NOW;
    estack_برداشت();
}

#اگر defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*کلیدها,
    صحیح		mode,
    صحیح		exact,		// require exact match
    صحیح		ign_mod,	// ignore preceding modifier
    صحیح		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    صحیح		*local_ptr)	// return: buffer-local mapping or NULL
{
    صحیح		hash;
    صحیح		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    صحیح		local;

    validate_maphash();

    len = (صحیح)STRLEN(کلیدها);
    برای (local = 1; local >= 0; --local)
	// loop over all hash lists
	برای (hash = 0; hash < 256; ++hash)
	{
	    اگر (abbr)
	    {
		اگر (hash > 0)		// there is only one list.
		    بشکن;
		اگر (local)
		    mp = curbuf->b_first_abbr;
		وگرنه
		    mp = first_abbr;
	    }
	    وگرنه اگر (local)
		mp = curbuf->b_maphash[hash];
	    وگرنه
		mp = maphash[hash];
	    برای ( ; mp != پوچ; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		اگر ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    اگر (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    وگرنه
			minlen = len;
		    s = mp->m_کلیدها;
		    اگر (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			اگر (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    اگر (STRNCMP(s, کلیدها, minlen) == 0)
		    {
			اگر (mp_ptr != پوچ)
			    *mp_ptr = mp;
			اگر (local_ptr != پوچ)
			    *local_ptr = local;
			بازگشت mp->m_str;
		    }
		}
	    }
	}

    بازگشت پوچ;
}

/*
 * "hasmapto()" function
 */
    تهی
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*mode;
    char_u	buf[NUMBUFLEN];
    صحیح		abbr = FALSE;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	بازگشت;

    name = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    وگرنه
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (صحیح)tv_get_bool(&argvars[2]);
    }

    اگر (map_to_exists(name, mode, abbr))
	rettv->vval.v_عدد = TRUE;
    وگرنه
	rettv->vval.v_عدد = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    ایستا تهی
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	صحیح	    buffer_local,   // false if not buffer local mapping
	صحیح	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_کلیدها, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_کلیدها);
    اگر (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_عدد(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_عدد(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_عدد(dict, "expr", mp->m_عبارت ? 1L : 0L);
    dict_add_عدد(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_add_عدد(dict, "sid", (long)mp->m_script_ctx.sc_sid);
    dict_add_عدد(dict, "scriptversion",
		    (long)mp->m_script_ctx.sc_version);
    dict_add_عدد(dict, "lnum", (long)mp->m_script_ctx.sc_lnum);
    dict_add_عدد(dict, "buffer", (long)buffer_local);
    dict_add_عدد(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_add_string(dict, "mode", mapmode);
    dict_add_عدد(dict, "abbr", abbr ? 1L : 0L);
    dict_add_عدد(dict, "mode_bits", mp->m_mode);

    vim_free(mapmode);
}

    ایستا تهی
get_maparg(typval_T *argvars, typval_T *rettv, صحیح exact)
{
    char_u	*کلیدها;
    char_u	*کلیدها_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*کلیدها_buf = پوچ;
    char_u	*alt_کلیدها_buf = پوچ;
    صحیح		did_simplify = FALSE;
    char_u	*rhs;
    صحیح		mode;
    صحیح		abbr = FALSE;
    صحیح		get_dict = FALSE;
    mapblock_T	*mp = پوچ;
    صحیح		buffer_local;
    صحیح		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    کلیدها = tv_get_string(&argvars[0]);
    اگر (*کلیدها == NUL)
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (صحیح)tv_get_bool(&argvars[2]);
	    اگر (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (صحیح)tv_get_bool(&argvars[3]);
	}
    }
    وگرنه
	which = (char_u *)"";
    اگر (which == پوچ)
	بازگشت;

    mode = get_map_mode(&which, 0);

    کلیدها_simplified = replace_termcodes(کلیدها, &کلیدها_buf, 0, flags,
								&did_simplify);
    rhs = check_map(کلیدها_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    اگر (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(تهی)replace_termcodes(کلیدها, &alt_کلیدها_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, پوچ);
	rhs = check_map(alt_کلیدها_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    اگر (!get_dict)
    {
	// Return a string.
	اگر (rhs != پوچ)
	{
	    اگر (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    وگرنه
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    وگرنه اگر (rettv_dict_alloc(rettv) == OK && rhs != پوچ)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? کلیدها_simplified : پوچ,
			  buffer_local, abbr);

    vim_free(کلیدها_buf);
    vim_free(alt_کلیدها_buf);
}

/*
 * "maplist()" function
 */
    تهی
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    صحیح		buffer_local;
    char_u	*کلیدها_buf;
    صحیح		did_simplify;
    صحیح		hash;
    char_u	*lhs;
    ثابت صحیح	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    صحیح		abbr = FALSE;

    اگر (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	بازگشت;
    اگر (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    برای (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	برای (hash = 0; hash < 256; ++hash)
	{
	    اگر (abbr)
	    {
		اگر (hash > 0)		// there is only one abbr list
		    بشکن;
		اگر (buffer_local)
		    mp = curbuf->b_first_abbr;
		وگرنه
		    mp = first_abbr;
	    }
	    وگرنه اگر (buffer_local)
		mp = curbuf->b_maphash[hash];
	    وگرنه
		mp = maphash[hash];
	    برای (; mp; mp = mp->m_next)
	    {
		اگر (mp->m_simplified)
		    ادامه;
		اگر ((d = dict_alloc()) == پوچ)
		    بازگشت;
		اگر (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    بازگشت;
		}

		کلیدها_buf = پوچ;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_کلیدها, TRUE, FALSE);
		(تهی)replace_termcodes(lhs, &کلیدها_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? کلیدها_buf : پوچ,
				 buffer_local, abbr);
		vim_free(کلیدها_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    تهی
f_maparg(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		بازگشت;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    تهی
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	بازگشت;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    ایستا صحیح
get_map_mode_string(char_u *mode_string, صحیح abbr)
{
    char_u	*p = mode_string;
    صحیح		mode = 0;
    صحیح		tmode;
    صحیح		modec;
    ثابت صحیح	MASK_V = MODE_VISUAL | MODE_SELECT;
    ثابت صحیح	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    ثابت صحیح	MASK_BANG = MODE_درج | MODE_CMDLINE;

    اگر (*p == NUL)
	p = (char_u *)" ";	// compatibility
    وقتی ((modec = *p++))
    {
	انتخاب (modec)
	{
	    حالت 'i': tmode = MODE_درج;	بشکن;
	    حالت 'l': tmode = MODE_LANGMAP;	بشکن;
	    حالت 'c': tmode = MODE_CMDLINE;	بشکن;
	    حالت 'n': tmode = MODE_NORMAL;	بشکن;
	    حالت 'x': tmode = MODE_VISUAL;	بشکن;
	    حالت 's': tmode = MODE_SELECT;	بشکن;
	    حالت 'o': tmode = MODE_OP_PENDING;	بشکن;
	    حالت 't': tmode = MODE_TERMINAL;	بشکن;
	    حالت 'v': tmode = MASK_V;		بشکن;
	    حالت '!': tmode = MASK_BANG;	بشکن;
	    حالت ' ': tmode = MASK_MAP;		بشکن;
	    پیشفرض:
		      بازگشت 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    اگر ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	بازگشت 0;

    بازگشت mode;
}

/*
 * "mapset()" function
 */
    تهی
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    صحیح		mode;
    char_u	buf[NUMBUFLEN];
    صحیح		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = پوچ;
    صحیح		noremap;
    صحیح		عبارت;
    صحیح		silent;
    صحیح		buffer;
    scid_T	sid;
    صحیح		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    صحیح		nowait;
    char_u	*arg;
    صحیح		dict_only;
    mapblock_T	*mp_result[2] = {پوچ, پوچ};

    اگر (check_secure())
	بازگشت;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    اگر (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	بازگشت;

    اگر (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	اگر (which == پوچ || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    بازگشت;
	}
    }
    وگرنه
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	اگر (which == پوچ)
	    بازگشت;
	is_abbr = (صحیح)tv_get_bool(&argvars[1]);

	اگر (check_for_dict_arg(argvars, 2) == FAIL)
	    بازگشت;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    اگر (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	بازگشت;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    اگر (lhs == پوچ || lhsraw == پوچ || rhs == پوچ)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	بازگشت;
    }
    orig_rhs = rhs;

    noremap = dict_get_عدد(d, "noremap") ? REMAP_NONE: 0;
    اگر (dict_get_عدد(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    عبارت = dict_get_عدد(d, "expr") != 0;
    silent = dict_get_عدد(d, "silent") != 0;
    sid = dict_get_عدد(d, "sid");
    scriptversion = dict_get_عدد(d, "scriptversion");
    lnum = dict_get_عدد(d, "lnum");
    buffer = dict_get_عدد(d, "buffer");
    nowait = dict_get_عدد(d, "nowait") != 0;
    // mode from the dict is not used

    اگر (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    وگرنه
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, پوچ);

    اگر (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    اگر (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	اگر (arg == پوچ)
	    بازگشت;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    وگرنه
    {
	arg = vim_strsave(lhs);
	اگر (arg == پوچ)
	    بازگشت;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, عبارت, sid,
			    scriptversion, lnum, 0);
    اگر (lhsrawalt != پوچ)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, عبارت, sid,
			    scriptversion, lnum, 1);

    اگر (mp_result[0] != پوچ && mp_result[1] != پوچ)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#اگر defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
ساختار initmap
{
    char_u	*arg;
    صحیح		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
ایستا ساختار initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_درج | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
ایستا ساختار initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_درج | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_درج | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_درج},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  وگرنه
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_درج}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# اگر defined(MACOS_X)
ایستا ساختار initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_درج | MODE_CMDLINE},
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
    تهی
init_mappings(تهی)
{
#اگر defined(MSWIN) || defined(MACOS_X)
    صحیح		i;

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    اگر (!gui.starting)
#  endif
    {
	برای (i = 0; i < (صحیح)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# اگر defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    برای (i = 0; i < (صحیح)ARRAY_LENGTH(initmappings); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode, FALSE);
# endif
#endif
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    تهی
add_map(char_u *map, صحیح mode, صحیح nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    اگر (s != پوچ)
    {
	(تهی)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#اگر defined(FEAT_LANGMAP)
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
تعریفنوع ساختار
{
    صحیح	    from;
    صحیح     to;
} langmap_entry_T;

ایستا garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    ایستا تهی
langmap_set_entry(صحیح from, صحیح to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    صحیح		    a = 0;
    صحیح		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    وقتی (a != b)
    {
	صحیح i = (a + b) / 2;
	صحیح d = entries[i].from - from;

	اگر (d == 0)
	{
	    entries[i].to = to;
	    بازگشت;
	}
	اگر (d < 0)
	    a = i + 1;
	وگرنه
	    b = i;
    }

    اگر (ga_grow(&langmap_mapga, 1) == FAIL)
	بازگشت;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * اندازه(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    صحیح
langmap_adjust_mb(صحیح c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    صحیح a = 0;
    صحیح b = langmap_mapga.ga_len;

    وقتی (a != b)
    {
	صحیح i = (a + b) / 2;
	صحیح d = entries[i].from - c;

	اگر (d == 0)
	    بازگشت entries[i].to;  // found matching entry
	اگر (d < 0)
	    a = i + 1;
	وگرنه
	    b = i;
    }
    بازگشت c;  // no entry found, return "c" unmodified
}

    تهی
langmap_init(تهی)
{
    صحیح i;

    برای (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, اندازه(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    نویسه *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    صحیح	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    برای (p = p_langmap; p[0] != NUL; )
    {
	برای (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    اگر (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	اگر (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	وگرنه
	    p2 = پوچ;	    // aAbBcCdD form, p2 is NULL
	وقتی (p[0])
	{
	    اگر (p[0] == ',')
	    {
		++p;
		بشکن;
	    }
	    اگر (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    اگر (p2 == پوچ)
	    {
		MB_PTR_ADV(p);
		اگر (p[0] != ',')
		{
		    اگر (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    وگرنه
	    {
		اگر (p2[0] != ',')
		{
		    اگر (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    اگر (to == NUL)
	    {
		sprintf(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		بازگشت args->os_errbuf;
	    }

	    اگر (from >= 256)
		langmap_set_entry(from, to);
	    وگرنه
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    اگر (p2 != پوچ)
	    {
		MB_PTR_ADV(p2);
		اگر (*p == ';')
		{
		    p = p2;
		    اگر (p[0] != NUL)
		    {
			اگر (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    بازگشت args->os_errbuf;
			}
			++p;
		    }
		    بشکن;
		}
	    }
	}
    }

    بازگشت پوچ;
}
#endif

    ایستا تهی
do_exmap(exarg_T *eap, صحیح isabbrev)
{
    صحیح	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    انتخاب (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	حالت 1: emsg(_(e_invalid_argument));
		بشکن;
	حالت 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		بشکن;
    }
}

/*
 * ":abbreviate" and friends.
 */
    تهی
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    تهی
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    اگر (secure)
    {
	secure = 2;
	پیام_outtrans(eap->cmd);
	پیام_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    تهی
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    تهی
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    تهی
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
