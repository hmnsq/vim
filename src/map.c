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

#include "vim.h"

/*
 * List used for abbreviations.
 */
static mapblock_T	*first_abbr = NULL; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
static mapblock_T	*(maphash[256]);
static 정수		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
static 정수		map_locked = 0;

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
get_maphash_list(정수 state, 정수 c)
{
    반환 maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(정수 state, 정수 c)
{
    반환 curbuf->b_maphash[MAP_HASH(state, c)];
}

    정수
is_maphash_valid(void)
{
    반환 maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    static void
validate_maphash(void)
{
    만약 (maphash_valid)
	반환;

    CLEAR_FIELD(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    static void
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_keys);
    만약 (mp->m_alt != NULL)
	mp->m_alt->m_alt = NULL;
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
    static char_u *
map_mode_to_chars(정수 mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    만약 ((mode & (MODE_INSERT | MODE_CMDLINE)) == (MODE_INSERT | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    아니면 만약 (mode & MODE_INSERT)
	ga_append(&mapmode, 'i');			// :imap
    아니면 만약 (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    아니면 만약 (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    아니면 만약 ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    아니면
    {
	만약 (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	만약 (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	만약 (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	만약 ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	아니면
	{
	    만약 (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    만약 (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    반환 (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    static void
showmap(
    mapblock_T	*mp,
    정수		local)	    // TRUE for buffer-local map
{
    정수		len = 1;
    char_u	*mapchars;

    만약 (message_filtered(mp->m_keys) && message_filtered(mp->m_str))
	반환;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    만약 (msg_didout || msg_silent != 0)
    {
	msg_putchar('\n');
	만약 (got_int)	    // 'q' typed at MORE prompt
	    goto theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    만약 (mapchars != NULL)
    {
	msg_puts((char *)mapchars);
	len = (정수)STRLEN(mapchars);
	vim_free(mapchars);
    }

    동안 (++len <= 3)
	msg_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = msg_outtrans_special(mp->m_keys, TRUE, 0);
    do
    {
	msg_putchar(' ');		// pad with blanks
	++len;
    } 동안 (len < 12);

    만약 (mp->m_noremap == REMAP_NONE)
	msg_puts_attr("*", HL_ATTR(HLF_8));
    아니면 만약 (mp->m_noremap == REMAP_SCRIPT)
	msg_puts_attr("&", HL_ATTR(HLF_8));
    아니면
	msg_putchar(' ');

    만약 (local)
	msg_putchar('@');
    아니면
	msg_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    만약 (*mp->m_str == NUL)
	msg_puts_attr("<Nop>", HL_ATTR(HLF_8));
    아니면
	msg_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    만약 (p_verbose > 0)
	last_set_msg(mp->m_script_ctx);
#endif
    msg_clr_eos();
    out_flush();			// show one line at a time

theend:
    --map_locked;
}

    static mapblock_T *
map_add(
	mapblock_T  **map_table,
	mapblock_T  **abbr_table,
	char_u	    *keys,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	정수	    noremap,
	정수	    nowait,
	정수	    silent,
	정수	    mode,
	정수	    is_abbr,
#ifdef FEAT_EVAL
	정수	    expr,
	scid_T	    sid,	    // 0 to use current_sctx
	정수	    scriptversion,
	linenr_T    lnum,
#endif
	정수	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    만약 (mp == NULL)
	반환 NULL;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    만약 (*keys == Ctrl_C)
    {
	만약 (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	아니면
	    mapped_ctrl_c |= mode;
    }

    mp->m_keys = vim_strsave(keys);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    만약 (mp->m_keys == NULL || mp->m_str == NULL)
    {
	vim_free(mp->m_keys);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	반환 NULL;
    }
    mp->m_keylen = (정수)STRLEN(mp->m_keys);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_expr = expr;
    만약 (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    아니면
    {
	mp->m_script_ctx = current_sctx;
	만약 (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    만약 (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    아니면
    {
	정수 n = MAP_HASH(mp->m_mode, mp->m_keys[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    반환 mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    static void
list_mappings(
	정수	keyround,
	정수	abbrev,
	정수	haskey,
	char_u	*keys,
	정수	keys_len,
	정수	mode,
	정수	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    만약 (p_verbose > 0 && keyround == 1)
    {
	만약 (seenModifyOtherKeys)
	    msg_puts(_("Seen modifyOtherKeys: true\n"));

	만약 (modify_otherkeys_state != MOKS_INITIAL)
	{
	    char *name = _("Unknown");
	    switch (modify_otherkeys_state)
	    {
		case MOKS_INITIAL: break;
		case MOKS_OFF: name = _("Off"); break;
		case MOKS_ENABLED: name = _("On"); break;
		case MOKS_DISABLED: name = _("Disabled"); break;
		case MOKS_AFTER_T_TE: name = _("Cleared"); break;
	    }

	    char buf[200];
	    vim_snprintf(buf, sizeof(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    msg_puts(buf);
	}

	만약 (kitty_protocol_state != KKPS_INITIAL)
	{
	    char *name = _("Unknown");
	    switch (kitty_protocol_state)
	    {
		case KKPS_INITIAL: break;
		case KKPS_OFF: name = _("Off"); break;
		case KKPS_ENABLED: name = _("On"); break;
		case KKPS_DISABLED: name = _("Disabled"); break;
		case KKPS_AFTER_T_TE: name = _("Cleared"); break;
	    }

	    char buf[200];
	    vim_snprintf(buf, sizeof(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    msg_puts(buf);
	}
    }

    // need to loop over all global hash lists
    위해 (정수 hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	만약 (abbrev)
	{
	    만약 (hash != 0)	// there is only one abbreviation list
		break;
	    mp = curbuf->b_first_abbr;
	}
	아니면
	    mp = curbuf->b_maphash[hash];
	위해 ( ; mp != NULL && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    만약 (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		만약 (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		아니면
		{
		    정수 n = mp->m_keylen;
		    만약 (STRNCMP(mp->m_keys, keys,
				   (size_t)(n < keys_len ? n : keys_len)) == 0)
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
    정수
do_map(
    정수		maptype,
    char_u	*arg,
    정수		mode,
    정수		abbrev)		// not a mapping but an abbreviation
{
    char_u	*keys;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {NULL, NULL};
    char_u	*rhs;
    char_u	*p;
    정수		n;
    정수		len = 0;	// init for GCC
    정수		hasarg;
    정수		haskey;
    정수		do_print;
    정수		keyround;
    char_u	*keys_buf = NULL;
    char_u	*alt_keys_buf = NULL;
    char_u	*arg_buf = NULL;
    정수		retval = 0;
    정수		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    정수		unique = FALSE;
    정수		nowait = FALSE;
    정수		silent = FALSE;
    정수		special = FALSE;
#ifdef FEAT_EVAL
    정수		expr = FALSE;
#endif
    정수		did_simplify = FALSE;
    정수		unmap_lhs_only = FALSE;
    정수		noremap;
    char_u      *orig_rhs;

    keys = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    만약 (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    만약 (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    아니면
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    위해 (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	만약 (STRNCMP(keys, "<buffer>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    continue;
	}

	// Check for "<nowait>": don't wait for more characters.
	만약 (STRNCMP(keys, "<nowait>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    nowait = TRUE;
	    continue;
	}

	// Check for "<silent>": don't echo commands.
	만약 (STRNCMP(keys, "<silent>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    silent = TRUE;
	    continue;
	}

	// Check for "<special>": accept special keys in <>
	만약 (STRNCMP(keys, "<special>", 9) == 0)
	{
	    keys = skipwhite(keys + 9);
	    special = TRUE;
	    continue;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	만약 (STRNCMP(keys, "<script>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    noremap = REMAP_SCRIPT;
	    continue;
	}

	// Check for "<expr>": {rhs} is an expression.
	만약 (STRNCMP(keys, "<expr>", 6) == 0)
	{
	    keys = skipwhite(keys + 6);
	    expr = TRUE;
	    continue;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	만약 (STRNCMP(keys, "<unique>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    unique = TRUE;
	    continue;
	}
	break;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = keys;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == NULL);
    동안 (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	만약 ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    만약 (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*keys != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    만약 (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	goto theend;
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
    만약 (haskey)
    {
	char_u	*new_keys;
	정수	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	만약 (special)
	    flags |= REPTERM_SPECIAL;
	new_keys = replace_termcodes(keys, &keys_buf, 0, flags, &did_simplify);
	만약 (did_simplify)
	    (void)replace_termcodes(keys, &alt_keys_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, NULL);
	keys = new_keys;
    }
    orig_rhs = rhs;
    만약 (hasarg)
    {
	만약 (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	아니면
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), NULL);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    위해 (keyround = 1; keyround <= 2; ++keyround)
    {
	정수	did_it = FALSE;
	정수	did_local = FALSE;
	정수	keyround1_simplified = keyround == 1 && did_simplify;
	정수	round;
	정수	num_rounds;

	만약 (keyround == 2)
	{
	    만약 (alt_keys_buf == NULL)
		break;
	    keys = alt_keys_buf;
	}
	아니면 만약 (alt_keys_buf != NULL && do_print)
	    // when printing always use the not-simplified map
	    keys = alt_keys_buf;

	// check arguments and translate function keys
	만약 (haskey)
	{
	    len = (정수)STRLEN(keys);
	    만약 (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		goto theend;
	    }

	    만약 (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		만약 (has_mbyte)
		{
		    정수		first, last;
		    정수		same = -1;
		    char_u	keys_unescaped[MAXMAPLEN + 1];
		    size_t	keys_unescaped_len;

		    mch_memmove(keys_unescaped, keys, (size_t)(len + 1));
		    keys_unescaped_len = vim_unescape_csi(keys_unescaped);
		    p = keys_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    동안 (p < keys_unescaped + keys_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			만약 (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    만약 (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			goto theend;
		    }
		}
		아니면 만약 (vim_iswordc(keys[len - 1]))
		    // ends in keyword char
		    위해 (n = 0; n < len - 2; ++n)
			만약 (vim_iswordc(keys[n]) != vim_iswordc(keys[len - 2]))
			{
			    retval = 1;
			    goto theend;
			}
		// An abbreviation cannot contain white space.
		위해 (n = 0; n < len; ++n)
		    만약 (VIM_ISWHITE(keys[n]))
		    {
			retval = 1;
			goto theend;
		    }
	    }
	}

	만약 (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	만약 (do_print)
	    msg_start();

	// Check if a new local mapping wasn't already defined globally.
	만약 (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    위해 (정수 hash = 0; hash < 256 && !got_int; ++hash)
	    {
		만약 (abbrev)
		{
		    만약 (hash != 0)	// there is only one abbreviation list
			break;
		    mp = first_abbr;
		}
		아니면
		    mp = maphash[hash];
		위해 ( ; mp != NULL && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    만약 ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_keys, keys, (size_t)len) == 0)
		    {
			만약 (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_keys);
			아니면
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_keys);
			retval = 5;
			goto theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	만약 (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, keys, len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	num_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	위해 (round = 0; round < num_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    위해 (정수 hash = 0; hash < 256 && !got_int; ++hash)
	    {
		만약 (abbrev)
		{
		    만약 (hash > 0)	// there is only one abbreviation list
			break;
		    mpp = abbr_table;
		}
		아니면
		    mpp = &(map_table[hash]);
		위해 (mp = *mpp; mp != NULL && !got_int; mp = *mpp)
		{

		    만약 ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			continue;
		    }
		    만약 (!haskey)	// show all entries
		    {
			만약 (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    아니면	// do we have a match?
		    {
			만약 (round)	// second round: Try unmap "rhs" string
			{
			    n = (정수)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			아니면
			{
			    n = mp->m_keylen;
			    p = mp->m_keys;
			}
			만약 (STRNCMP(p, keys, (size_t)(n < len ? n : len)) == 0)
			{
			    만약 (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				만약 (n != len && (!abbrev || round || n > len
					       || *skipwhite(keys + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    continue;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				만약 (keyround1_simplified && !mp->m_simplified)
				    break;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    아니면 만약 (!hasarg)	// show matching entry
			    {
				만약 (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    아니면 만약 (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				continue;
			    }
			    아니면 만약 (unique)
			    {
				만약 (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				아니면
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				goto theend;
			    }
			    아니면
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				만약 (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    만약 (newstr == NULL)
				    {
					retval = 4;		// no mem
					goto theend;
				    }
				    만약 (mp->m_alt != NULL)
					mp->m_alt = mp->m_alt->m_alt = NULL;
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
				    mp->m_expr = expr;
				    mp->m_script_ctx = current_sctx;
				    만약 (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    만약 (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				continue;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    정수 new_hash = MAP_HASH(mp->m_mode, mp->m_keys[0]);
			    만약 (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_next;
				mp->m_next = map_table[new_hash];
				map_table[new_hash] = mp;

				continue;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_next);
		}
	    }
	}

	만약 (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    만약 (!did_it)
	    {
		만약 (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    아니면 만약 (*keys == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		만약 (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		아니면
		    mapped_ctrl_c &= ~mode;
	    }
	    continue;
	}

	만약 (!haskey || !hasarg)
	{
	    // print entries
	    만약 (!did_it && !did_local)
	    {
		만약 (abbrev)
		    msg(_("No abbreviation found"));
		아니면
		    msg(_("No mapping found"));
	    }
	    goto theend;    // listing finished
	}

	만약 (did_it)
	    continue;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, keys,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    expr, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	만약 (mp_result[keyround - 1] == NULL)
	{
	    retval = 4;	    // no mem
	    goto theend;
	}
    }

    만약 (mp_result[0] != NULL && mp_result[1] != NULL)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(keys_buf);
    vim_free(alt_keys_buf);
    vim_free(arg_buf);
    반환 retval;
}

/*
 * Get the mapping mode from the command name.
 */
    static 정수
get_map_mode(char_u **cmdp, 정수 forceit)
{
    char_u	*p;
    정수		modec;
    정수		mode;

    p = *cmdp;
    modec = *p++;
    만약 (modec == 'i')
	mode = MODE_INSERT;				// :imap
    아니면 만약 (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    아니면 만약 (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    아니면 만약 (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    아니면 만약 (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    아니면 만약 (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    아니면 만약 (modec == 's')
	mode = MODE_SELECT;			// :smap
    아니면 만약 (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    아니면 만약 (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    아니면
    {
	--p;
	만약 (forceit)
	    mode = MODE_INSERT | MODE_CMDLINE;		// :map !
	아니면
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    반환 mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    static void
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    정수		forceit,
    정수		abbr)
{
    정수		mode;
    정수		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    만약 (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	반환;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    static 정수
is_map_locked(void)
{
    만약 (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	반환 TRUE;
    }
    반환 FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    void
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    정수		mode,		// mode in which to delete
    정수		local,		// TRUE for buffer-local mappings
    정수		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    정수		hash;
    정수		new_hash;

    만약 (is_map_locked())
	반환;

    validate_maphash();

    위해 (hash = 0; hash < 256; ++hash)
    {
	만약 (abbr)
	{
	    만약 (hash > 0)	// there is only one abbrlist
		break;
	    만약 (local)
		mpp = &buf->b_first_abbr;
	    아니면
		mpp = &first_abbr;
	}
	아니면
	{
	    만약 (local)
		mpp = &buf->b_maphash[hash];
	    아니면
		mpp = &maphash[hash];
	}
	동안 (*mpp != NULL)
	{
	    mp = *mpp;
	    만약 (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		만약 (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    continue;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_keys[0]);
		만약 (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    만약 (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    아니면
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    continue;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

#만약 defined(FEAT_EVAL)
    정수
mode_str2flags(char_u *modechars)
{
    정수		mode = 0;

    만약 (vim_strchr(modechars, 'n') != NULL)
	mode |= MODE_NORMAL;
    만약 (vim_strchr(modechars, 'v') != NULL)
	mode |= MODE_VISUAL | MODE_SELECT;
    만약 (vim_strchr(modechars, 'x') != NULL)
	mode |= MODE_VISUAL;
    만약 (vim_strchr(modechars, 's') != NULL)
	mode |= MODE_SELECT;
    만약 (vim_strchr(modechars, 'o') != NULL)
	mode |= MODE_OP_PENDING;
    만약 (vim_strchr(modechars, 'i') != NULL)
	mode |= MODE_INSERT;
    만약 (vim_strchr(modechars, 'l') != NULL)
	mode |= MODE_LANGMAP;
    만약 (vim_strchr(modechars, 'c') != NULL)
	mode |= MODE_CMDLINE;

    반환 mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    정수
map_to_exists(char_u *str, char_u *modechars, 정수 abbr)
{
    char_u	*rhs;
    char_u	*buf;
    정수		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, NULL);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    반환 retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    정수
map_to_exists_mode(char_u *rhs, 정수 mode, 정수 abbr)
{
    mapblock_T	*mp;
    정수		hash;
    정수		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    위해 (;;)
    {
	위해 (hash = 0; hash < 256; ++hash)
	{
	    만약 (abbr)
	    {
		만약 (hash > 0)		// there is only one abbr list
		    break;
		만약 (exp_buffer)
		    mp = curbuf->b_first_abbr;
		아니면
		    mp = first_abbr;
	    }
	    아니면 만약 (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    아니면
		mp = maphash[hash];
	    위해 (; mp; mp = mp->m_next)
	    {
		만약 ((mp->m_mode & mode)
			&& strstr((char *)mp->m_str, (char *)rhs) != NULL)
		    반환 TRUE;
	    }
	}
	만약 (exp_buffer)
	    break;
	exp_buffer = TRUE;
    }

    반환 FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
static 정수	expand_mapmodes = 0;
static 정수	expand_isabbrev = 0;
static 정수	expand_buffer = FALSE;

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
    static char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    정수		c;
    정수		modifiers;
    정수		cpo_bslash;
    정수		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != NULL);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != NULL);

    위해 (; *str; ++str)
    {
	c = *str;
	만약 (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    만약 (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    만약 (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		만약 (cpo_special)
		{
		    ga_clear(&ga);
		    반환 NULL;
		}
		c = TO_SPECIAL(str[1], str[2]);
		만약 (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    만약 (IS_SPECIAL(c) || modifiers)	// special key
	    {
		만약 (cpo_special)
		{
		    ga_clear(&ga);
		    반환 NULL;
		}
		ga_concat(&ga, get_special_key_name(c, modifiers));
		continue; // for (str)
	    }
	}
	만약 (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	만약 (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    반환 (char_u *)(ga.ga_data);
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
    정수		forceit,	// TRUE if '!' given
    정수		isabbrev,	// TRUE if abbreviation
    정수		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    만약 (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    아니면
    {
	만약 (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	아니면
	{
	    expand_mapmodes = MODE_INSERT | MODE_CMDLINE;
	    만약 (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	위해 (;;)
	{
	    만약 (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		continue;
	    }
	    만약 (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    만약 (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    만약 (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    만약 (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		continue;
	    }
#ifdef FEAT_EVAL
	    만약 (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    만약 (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		continue;
	    }
#endif
	    break;
	}
	xp->xp_pattern = arg;
    }

    반환 NULL;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    정수
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    정수		*numMatches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    정수		hash;
    정수		count;
    char_u	*p;
    정수		i;
    정수		fuzzy;
    정수		match;
    정수		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *numMatches = 0;		    // return values in case of FAIL
    *matches = NULL;

    만약 (!fuzzy)
	ga_init2(&ga, sizeof(char *), 3);
    아니면
	ga_init2(&ga, sizeof(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    위해 (i = 0; i < 7; ++i)
    {
	만약 (i == 0)
	    p = (char_u *)"<silent>";
	아니면 만약 (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	아니면 만약 (i == 2)
	    p = (char_u *)"<script>";
	아니면 만약 (i == 3)
	    p = (char_u *)"<expr>";
#endif
	아니면 만약 (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	아니면 만약 (i == 5)
	    p = (char_u *)"<nowait>";
	아니면 만약 (i == 6)
	    p = (char_u *)"<special>";
	아니면
	    continue;

	만약 (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	아니면
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	만약 (!match)
	    continue;

	만약 (ga_grow(&ga, 1) == FAIL)
	    break;

	만약 (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	아니면
	    ((char_u **)ga.ga_data)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    위해 (hash = 0; hash < 256; ++hash)
    {
	만약 (expand_isabbrev)
	{
	    만약 (hash > 0)	// only one abbrev list
		break; // for (hash)
	    mp = first_abbr;
	}
	아니면 만약 (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	아니면
	    mp = maphash[hash];
	위해 (; mp; mp = mp->m_next)
	{
	    만약 (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		continue;

	    p = translate_mapping(mp->m_keys);
	    만약 (p == NULL)
		continue;

	    만약 (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    아니면
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    만약 (!match)
	    {
		vim_free(p);
		continue;
	    }

	    만약 (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		break;
	    }

	    만약 (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    아니면
		((char_u **)ga.ga_data)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    만약 (ga.ga_len == 0)
	반환 FAIL;

    만약 (!fuzzy)
    {
	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
    }
    아니면
    {
	만약 (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    반환 FAIL;
	*numMatches = ga.ga_len;
    }

    count = *numMatches;
    만약 (count > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	만약 (!fuzzy)
	    sort_strings(*matches, count);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + count;

	동안 (ptr2 < ptr3)
	{
	    만약 (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    아니면
	    {
		vim_free(*ptr2++);
		count--;
	    }
	}
    }

    *numMatches = count;
    반환 (count == 0 ? FAIL : OK);
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
    정수
check_abbr(
    정수		c,
    char_u	*ptr,
    정수		col,
    정수		mincol)
{
    정수		len;
    정수		scol;		// starting column of the abbr.
    정수		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    정수		clen = 0;	// length in characters
    정수		is_id = TRUE;
    정수		vim_abbr;

    만약 (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	반환 FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    만약 (noremap_keys() && c != Ctrl_RSB)
	반환 FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    만약 (col == 0)				// cannot be an abbr.
	반환 FALSE;

    만약 (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	만약 (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	아니면
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    만약 (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	동안 (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    만약 (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		break;
	    }
	    ++clen;
	}
	scol = (정수)(p - ptr);
    }
    아니면
    {
	만약 (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	아니면
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    만약 (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	위해 (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    만약 (scol < mincol)
	scol = mincol;
    만약 (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	만약 (mp == NULL)
	{
	    mp = mp2;
	    mp2 = NULL;
	}
	위해 ( ; mp; mp->m_next == NULL
				  ? (mp = mp2, mp2 = NULL) : (mp = mp->m_next))
	{
	    정수		qlen = mp->m_keylen;
	    char_u	*q = mp->m_keys;
	    정수		match;

	    만약 (vim_strbyte(mp->m_keys, K_SPECIAL) != NULL)
	    {
		char_u *qe = vim_strsave(mp->m_keys);

		// might have CSI escaped mp->m_keys
		만약 (qe != NULL)
		{
		    q = qe;
		    qlen = (정수)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    만약 (q != mp->m_keys)
		vim_free(q);
	    만약 (match)
		break;
	}
	만약 (mp != NULL)
	{
	    정수	noremap;
	    정수 silent;
#ifdef FEAT_EVAL
	    정수 expr;
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
	    만약 (c != Ctrl_RSB)
	    {
					// special key code, split up
		만약 (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		아니면
		{
		    만약 (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    만약 (has_mbyte)
		    {
			정수	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			만약 (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			만약 (escaped != NULL)
			{
			    newlen = (정수)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    아니면
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(void)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ifdef FEAT_EVAL
	    expr = mp->m_expr;

	    만약 (expr)
		s = eval_map_expr(mp, c);
	    아니면
#endif
		s = mp->m_str;
	    만약 (s != NULL)
	    {
					// insert the to string
		(void)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (정수)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		만약 (expr)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    만약 (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    동안 (len-- > 0)		// delete the from string
		(void)ins_typebuf(tb, 1, 0, TRUE, silent);
	    반환 TRUE;
	}
    }
    반환 FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_expr(
    mapblock_T	*mp,
    정수		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*expr;
    pos_T	save_cursor;
    정수		save_msg_col;
    정수		save_msg_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    정수		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    expr = vim_strsave(mp->m_str);
    만약 (expr == NULL)
	반환 NULL;
    vim_unescape_csi(expr);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_msg_col = msg_col;
    save_msg_row = msg_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    만약 (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(expr, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    msg_col = save_msg_col;
    msg_row = save_msg_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(expr);

    만약 (p == NULL)
	반환 NULL;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    반환 res;
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
    만약 (res == NULL)
	반환 NULL;

    d = res;
    위해 (s = p; *s != NUL; )
    {
	만약 ((s[0] == K_SPECIAL
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
	아니면
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    반환 res;
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

    동안 (*s != NUL)
    {
	만약 (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	아니면 만약 ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (정수)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	아니면
	    *d++ = *s++;
    }
    *d = NUL;
    반환 (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    정수
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    char	*cmd;
    정수		abbr;
    정수		hash;
    정수		did_cpo = FALSE;
    정수		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    위해 (abbr = 0; abbr < 2; ++abbr)
	위해 (hash = 0; hash < 256; ++hash)
	{
	    만약 (abbr)
	    {
		만약 (hash > 0)		// there is only one abbr list
		    break;
		만약 (buf != NULL)
		    mp = buf->b_first_abbr;
		아니면
		    mp = first_abbr;
	    }
	    아니면
	    {
		만약 (buf != NULL)
		    mp = buf->b_maphash[hash];
		아니면
		    mp = maphash[hash];
	    }

	    위해 ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		만약 (mp->m_simplified)
		    continue;

		// skip script-local mappings
		만약 (mp->m_noremap == REMAP_SCRIPT)
		    continue;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		위해 (p = mp->m_str; *p != NUL; ++p)
		    만약 (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (정수)KE_SNR)
			break;
		만약 (*p != NUL)
		    continue;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		만약 (abbr)
		    cmd = "abbr";
		아니면
		    cmd = "map";
		switch (mp->m_mode)
		{
		    case MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			break;
		    case MODE_NORMAL:
			c1 = 'n';
			break;
		    case MODE_VISUAL:
			c1 = 'x';
			break;
		    case MODE_SELECT:
			c1 = 's';
			break;
		    case MODE_OP_PENDING:
			c1 = 'o';
			break;
		    case MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			break;
		    case MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			break;
		    case MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			break;
		    case MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			break;
		    case MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			break;
		    case MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			break;
		    case MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			break;
		    case MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			break;
		    case MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			break;
		    case MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			break;
		    case MODE_CMDLINE | MODE_INSERT:
			만약 (!abbr)
			    cmd = "map!";
			break;
		    case MODE_CMDLINE:
			c1 = 'c';
			break;
		    case MODE_INSERT:
			c1 = 'i';
			break;
		    case MODE_LANGMAP:
			c1 = 'l';
			break;
		    case MODE_TERMINAL:
			c1 = 't';
			break;
		    default:
			iemsg(e_makemap_illegal_mode);
			반환 FAIL;
		}
		do	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    만약 (!did_cpo)
		    {
			만약 (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			아니면
			    위해 (i = 0; i < 2; ++i)
				위해 (p = (i ? mp->m_str : mp->m_keys); *p; ++p)
				    만약 (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			만약 (did_cpo)
			{
			    만약 (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				반환 FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    만약 (mp->m_expr && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && fputs("legacy ", fd) < 0)
			반환 FAIL;
#endif
		    만약 (c1 && putc(c1, fd) < 0)
			반환 FAIL;
		    만약 (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			반환 FAIL;
		    만약 (fputs(cmd, fd) < 0)
			반환 FAIL;
		    만약 (buf != NULL && fputs(" <buffer>", fd) < 0)
			반환 FAIL;
		    만약 (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			반환 FAIL;
		    만약 (mp->m_silent && fputs(" <silent>", fd) < 0)
			반환 FAIL;
#ifdef FEAT_EVAL
		    만약 (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			반환 FAIL;
		    만약 (mp->m_expr && fputs(" <expr>", fd) < 0)
			반환 FAIL;
#endif

		    만약 (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_keys, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			반환 FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} 동안 (c1 != NUL);
	    }
	}

    만약 (did_cpo)
	만약 (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    반환 FAIL;
    반환 OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    정수
put_escstr(FILE *fd, char_u *strstart, 정수 what)
{
    char_u	*str = strstart;
    정수		c;
    정수		modifiers;

    // :map xx <Nop>
    만약 (*str == NUL && what == 1)
    {
	만약 (fprintf(fd, "<Nop>") < 0)
	    반환 FAIL;
	반환 OK;
    }

    위해 ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	만약 (p != NULL)
	{
	    동안 (*p != NUL)
		만약 (fputc(*p++, fd) < 0)
		    반환 FAIL;
	    --str;
	    continue;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	만약 (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    만약 (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		만약 (p == NULL)
		    c = *str;
		아니면
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    만약 (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    만약 (IS_SPECIAL(c) || modifiers)	// special key
	    {
		만약 (fputs((char *)get_special_key_name(c, modifiers), fd) < 0)
		    반환 FAIL;
		continue;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	만약 (c == NL)
	{
	    만약 (what == 2)
	    {
		만약 (fprintf(fd, "\\\026\n") < 0)
		    반환 FAIL;
	    }
	    아니면
	    {
		만약 (fprintf(fd, "<NL>") < 0)
		    반환 FAIL;
	    }
	    continue;
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
	만약 (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    만약 (putc('\\', fd) < 0)
		반환 FAIL;
	}
	아니면 만약 (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    만약 (putc(Ctrl_V, fd) < 0)
		반환 FAIL;
	}
	만약 (putc(c, fd) < 0)
	    반환 FAIL;
    }
    반환 OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    void
check_map_keycodes(void)
{
    mapblock_T	*mp;
    char_u	*p;
    정수		i;
    char_u	buf[3];
    정수		abbr;
    정수		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_push(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    위해 (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	위해 (abbr = 0; abbr <= 1; ++abbr)
	    위해 (hash = 0; hash < 256; ++hash)
	    {
		만약 (abbr)
		{
		    만약 (hash)	    // there is only one abbr list
			break;
		    만약 (bp != NULL)
			mp = bp->b_first_abbr;
		    아니면
			mp = first_abbr;
		}
		아니면
		{
		    만약 (bp != NULL)
			mp = bp->b_maphash[hash];
		    아니면
			mp = maphash[hash];
		}
		위해 ( ; mp != NULL; mp = mp->m_next)
		{
		    위해 (i = 0; i <= 1; ++i)	// do this twice
		    {
			만약 (i == 0)
			    p = mp->m_keys;	// once for the "from" part
			아니면
			    p = mp->m_str;	// and once for the "to" part
			동안 (*p)
			{
			    만약 (*p == K_SPECIAL)
			    {
				++p;
				만약 (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (void)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	만약 (bp == NULL)
	    break;
    }
    ESTACK_CHECK_NOW;
    estack_pop();
}

#만약 defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*keys,
    정수		mode,
    정수		exact,		// require exact match
    정수		ign_mod,	// ignore preceding modifier
    정수		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    정수		*local_ptr)	// return: buffer-local mapping or NULL
{
    정수		hash;
    정수		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    정수		local;

    validate_maphash();

    len = (정수)STRLEN(keys);
    위해 (local = 1; local >= 0; --local)
	// loop over all hash lists
	위해 (hash = 0; hash < 256; ++hash)
	{
	    만약 (abbr)
	    {
		만약 (hash > 0)		// there is only one list.
		    break;
		만약 (local)
		    mp = curbuf->b_first_abbr;
		아니면
		    mp = first_abbr;
	    }
	    아니면 만약 (local)
		mp = curbuf->b_maphash[hash];
	    아니면
		mp = maphash[hash];
	    위해 ( ; mp != NULL; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		만약 ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    만약 (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    아니면
			minlen = len;
		    s = mp->m_keys;
		    만약 (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			만약 (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    만약 (STRNCMP(s, keys, minlen) == 0)
		    {
			만약 (mp_ptr != NULL)
			    *mp_ptr = mp;
			만약 (local_ptr != NULL)
			    *local_ptr = local;
			반환 mp->m_str;
		    }
		}
	    }
	}

    반환 NULL;
}

/*
 * "hasmapto()" function
 */
    void
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*mode;
    char_u	buf[NUMBUFLEN];
    정수		abbr = FALSE;

    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	반환;

    name = tv_get_string(&argvars[0]);
    만약 (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    아니면
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	만약 (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (정수)tv_get_bool(&argvars[2]);
    }

    만약 (map_to_exists(name, mode, abbr))
	rettv->vval.v_number = TRUE;
    아니면
	rettv->vval.v_number = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    static void
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	정수	    buffer_local,   // false if not buffer local mapping
	정수	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_keys, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_keys);
    만약 (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_number(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_number(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_number(dict, "expr", mp->m_expr ? 1L : 0L);
    dict_add_number(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_add_number(dict, "sid", (long)mp->m_script_ctx.sc_sid);
    dict_add_number(dict, "scriptversion",
		    (long)mp->m_script_ctx.sc_version);
    dict_add_number(dict, "lnum", (long)mp->m_script_ctx.sc_lnum);
    dict_add_number(dict, "buffer", (long)buffer_local);
    dict_add_number(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_add_string(dict, "mode", mapmode);
    dict_add_number(dict, "abbr", abbr ? 1L : 0L);
    dict_add_number(dict, "mode_bits", mp->m_mode);

    vim_free(mapmode);
}

    static void
get_maparg(typval_T *argvars, typval_T *rettv, 정수 exact)
{
    char_u	*keys;
    char_u	*keys_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*keys_buf = NULL;
    char_u	*alt_keys_buf = NULL;
    정수		did_simplify = FALSE;
    char_u	*rhs;
    정수		mode;
    정수		abbr = FALSE;
    정수		get_dict = FALSE;
    mapblock_T	*mp = NULL;
    정수		buffer_local;
    정수		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    keys = tv_get_string(&argvars[0]);
    만약 (*keys == NUL)
	반환;

    만약 (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	만약 (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (정수)tv_get_bool(&argvars[2]);
	    만약 (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (정수)tv_get_bool(&argvars[3]);
	}
    }
    아니면
	which = (char_u *)"";
    만약 (which == NULL)
	반환;

    mode = get_map_mode(&which, 0);

    keys_simplified = replace_termcodes(keys, &keys_buf, 0, flags,
								&did_simplify);
    rhs = check_map(keys_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    만약 (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(void)replace_termcodes(keys, &alt_keys_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, NULL);
	rhs = check_map(alt_keys_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    만약 (!get_dict)
    {
	// Return a string.
	만약 (rhs != NULL)
	{
	    만약 (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    아니면
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    아니면 만약 (rettv_dict_alloc(rettv) == OK && rhs != NULL)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? keys_simplified : NULL,
			  buffer_local, abbr);

    vim_free(keys_buf);
    vim_free(alt_keys_buf);
}

/*
 * "maplist()" function
 */
    void
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    정수		buffer_local;
    char_u	*keys_buf;
    정수		did_simplify;
    정수		hash;
    char_u	*lhs;
    const 정수	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    정수		abbr = FALSE;

    만약 (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	반환;
    만약 (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    만약 (rettv_list_alloc(rettv) == FAIL)
	반환;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    위해 (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	위해 (hash = 0; hash < 256; ++hash)
	{
	    만약 (abbr)
	    {
		만약 (hash > 0)		// there is only one abbr list
		    break;
		만약 (buffer_local)
		    mp = curbuf->b_first_abbr;
		아니면
		    mp = first_abbr;
	    }
	    아니면 만약 (buffer_local)
		mp = curbuf->b_maphash[hash];
	    아니면
		mp = maphash[hash];
	    위해 (; mp; mp = mp->m_next)
	    {
		만약 (mp->m_simplified)
		    continue;
		만약 ((d = dict_alloc()) == NULL)
		    반환;
		만약 (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    반환;
		}

		keys_buf = NULL;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_keys, TRUE, FALSE);
		(void)replace_termcodes(lhs, &keys_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? keys_buf : NULL,
				 buffer_local, abbr);
		vim_free(keys_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    void
f_maparg(typval_T *argvars, typval_T *rettv)
{
    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		반환;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    void
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	반환;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    static 정수
get_map_mode_string(char_u *mode_string, 정수 abbr)
{
    char_u	*p = mode_string;
    정수		mode = 0;
    정수		tmode;
    정수		modec;
    const 정수	MASK_V = MODE_VISUAL | MODE_SELECT;
    const 정수	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    const 정수	MASK_BANG = MODE_INSERT | MODE_CMDLINE;

    만약 (*p == NUL)
	p = (char_u *)" ";	// compatibility
    동안 ((modec = *p++))
    {
	switch (modec)
	{
	    case 'i': tmode = MODE_INSERT;	break;
	    case 'l': tmode = MODE_LANGMAP;	break;
	    case 'c': tmode = MODE_CMDLINE;	break;
	    case 'n': tmode = MODE_NORMAL;	break;
	    case 'x': tmode = MODE_VISUAL;	break;
	    case 's': tmode = MODE_SELECT;	break;
	    case 'o': tmode = MODE_OP_PENDING;	break;
	    case 't': tmode = MODE_TERMINAL;	break;
	    case 'v': tmode = MASK_V;		break;
	    case '!': tmode = MASK_BANG;	break;
	    case ' ': tmode = MASK_MAP;		break;
	    default:
		      반환 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    만약 ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	반환 0;

    반환 mode;
}

/*
 * "mapset()" function
 */
    void
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    정수		mode;
    char_u	buf[NUMBUFLEN];
    정수		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = NULL;
    정수		noremap;
    정수		expr;
    정수		silent;
    정수		buffer;
    scid_T	sid;
    정수		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    정수		nowait;
    char_u	*arg;
    정수		dict_only;
    mapblock_T	*mp_result[2] = {NULL, NULL};

    만약 (check_secure())
	반환;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    만약 (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	반환;

    만약 (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	만약 (which == NULL || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    반환;
	}
    }
    아니면
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	만약 (which == NULL)
	    반환;
	is_abbr = (정수)tv_get_bool(&argvars[1]);

	만약 (check_for_dict_arg(argvars, 2) == FAIL)
	    반환;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    만약 (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	반환;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    만약 (lhs == NULL || lhsraw == NULL || rhs == NULL)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	반환;
    }
    orig_rhs = rhs;

    noremap = dict_get_number(d, "noremap") ? REMAP_NONE: 0;
    만약 (dict_get_number(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    expr = dict_get_number(d, "expr") != 0;
    silent = dict_get_number(d, "silent") != 0;
    sid = dict_get_number(d, "sid");
    scriptversion = dict_get_number(d, "scriptversion");
    lnum = dict_get_number(d, "lnum");
    buffer = dict_get_number(d, "buffer");
    nowait = dict_get_number(d, "nowait") != 0;
    // mode from the dict is not used

    만약 (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    아니면
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, NULL);

    만약 (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    만약 (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	만약 (arg == NULL)
	    반환;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    아니면
    {
	arg = vim_strsave(lhs);
	만약 (arg == NULL)
	    반환;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, expr, sid,
			    scriptversion, lnum, 0);
    만약 (lhsrawalt != NULL)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, expr, sid,
			    scriptversion, lnum, 1);

    만약 (mp_result[0] != NULL && mp_result[1] != NULL)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#만약 defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
struct initmap
{
    char_u	*arg;
    정수		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
static struct initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_INSERT | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# 만약 defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
static struct initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_INSERT | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_INSERT | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_INSERT},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  아니면
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_INSERT}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# 만약 defined(MACOS_X)
static struct initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_INSERT | MODE_CMDLINE},
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
    void
init_mappings(void)
{
#만약 defined(MSWIN) || defined(MACOS_X)
    정수		i;

# 만약 defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    만약 (!gui.starting)
#  endif
    {
	위해 (i = 0; i < (정수)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# 만약 defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    위해 (i = 0; i < (정수)ARRAY_LENGTH(initmappings); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode, FALSE);
# endif
#endif
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    void
add_map(char_u *map, 정수 mode, 정수 nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    만약 (s != NULL)
    {
	(void)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#만약 defined(FEAT_LANGMAP)
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
typedef struct
{
    정수	    from;
    정수     to;
} langmap_entry_T;

static garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    static void
langmap_set_entry(정수 from, 정수 to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    정수		    a = 0;
    정수		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    동안 (a != b)
    {
	정수 i = (a + b) / 2;
	정수 d = entries[i].from - from;

	만약 (d == 0)
	{
	    entries[i].to = to;
	    반환;
	}
	만약 (d < 0)
	    a = i + 1;
	아니면
	    b = i;
    }

    만약 (ga_grow(&langmap_mapga, 1) == FAIL)
	반환;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * sizeof(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    정수
langmap_adjust_mb(정수 c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    정수 a = 0;
    정수 b = langmap_mapga.ga_len;

    동안 (a != b)
    {
	정수 i = (a + b) / 2;
	정수 d = entries[i].from - c;

	만약 (d == 0)
	    반환 entries[i].to;  // found matching entry
	만약 (d < 0)
	    a = i + 1;
	아니면
	    b = i;
    }
    반환 c;  // no entry found, return "c" unmodified
}

    void
langmap_init(void)
{
    정수 i;

    위해 (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, sizeof(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    char *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    정수	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    위해 (p = p_langmap; p[0] != NUL; )
    {
	위해 (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    만약 (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	만약 (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	아니면
	    p2 = NULL;	    // aAbBcCdD form, p2 is NULL
	동안 (p[0])
	{
	    만약 (p[0] == ',')
	    {
		++p;
		break;
	    }
	    만약 (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    만약 (p2 == NULL)
	    {
		MB_PTR_ADV(p);
		만약 (p[0] != ',')
		{
		    만약 (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    아니면
	    {
		만약 (p2[0] != ',')
		{
		    만약 (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    만약 (to == NUL)
	    {
		sprintf(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		반환 args->os_errbuf;
	    }

	    만약 (from >= 256)
		langmap_set_entry(from, to);
	    아니면
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    만약 (p2 != NULL)
	    {
		MB_PTR_ADV(p2);
		만약 (*p == ';')
		{
		    p = p2;
		    만약 (p[0] != NUL)
		    {
			만약 (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    반환 args->os_errbuf;
			}
			++p;
		    }
		    break;
		}
	    }
	}
    }

    반환 NULL;
}
#endif

    static void
do_exmap(exarg_T *eap, 정수 isabbrev)
{
    정수	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    switch (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	case 1: emsg(_(e_invalid_argument));
		break;
	case 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		break;
    }
}

/*
 * ":abbreviate" and friends.
 */
    void
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    void
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    만약 (secure)
    {
	secure = 2;
	msg_outtrans(eap->cmd);
	msg_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    void
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    void
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    void
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
