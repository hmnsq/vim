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
ساکن mapblock_T	*پہلا_abbr = تہی; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
ساکن mapblock_T	*(maphash[256]);
ساکن صحیح		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
ساکن صحیح		map_locked = 0;

/*
 * Make a hash value for a mapping.
 * "mode" is the lower 4 bits of the State for the mapping.
 * "c1" is the first character of the "lhs".
 * Returns a value between 0 and 255, index in maphash.
 * Put Normal/Visual mode mappings mostly separately from Insert/Cmdline mode.
 */
#تعریف_کرو MAP_HASH(منوال, c1) (((منوال) & (منوال_معمول | منوال_VISUAL | منوال_SELECT | منوال_OP_PENDING | منوال_TERMINAL)) ? (c1) : ((c1) ^ 0x80))

/*
 * Get the start of the hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_maphash_list(صحیح state, صحیح c)
{
    واپس maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(صحیح state, صحیح c)
{
    واپس curbuf->b_maphash[MAP_HASH(state, c)];
}

    صحیح
is_maphash_valid(خالی)
{
    واپس maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    ساکن خالی
validate_maphash(خالی)
{
    اگر (maphash_valid)
	واپس;

    CLEAR_میدان(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    ساکن خالی
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_کلیدیں);
    اگر (mp->m_alt != تہی)
	mp->m_alt->m_alt = تہی;
    vim_free(mp->m_str);
    vim_free(mp->m_orig_str);
    *mpp = mp->m_اگلا;
#اگر_تعریف_شدہ FEAT_EVAL
    reset_آخری_مستعمل_map(mp);
#اگر_ختم
    vim_free(mp);
}

/*
 * Return characters to represent the map mode in an allocated string.
 * Returns NULL when out of memory.
 */
    ساکن char_u *
map_منوال_مقصد_chars(صحیح منوال)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    اگر ((منوال & (منوال_داخل_کرو | منوال_CMDLINE)) == (منوال_داخل_کرو | منوال_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    ورنہ اگر (منوال & منوال_داخل_کرو)
	ga_append(&mapmode, 'i');			// :imap
    ورنہ اگر (منوال & منوال_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    ورنہ اگر (منوال & منوال_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    ورنہ اگر ((منوال
		 & (منوال_معمول | منوال_VISUAL | منوال_SELECT | منوال_OP_PENDING))
		== (منوال_معمول | منوال_VISUAL | منوال_SELECT | منوال_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    ورنہ
    {
	اگر (منوال & منوال_معمول)
	    ga_append(&mapmode, 'n');			// :nmap
	اگر (منوال & منوال_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	اگر (منوال & منوال_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	اگر ((منوال & (منوال_VISUAL | منوال_SELECT)) == (منوال_VISUAL | منوال_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	ورنہ
	{
	    اگر (منوال & منوال_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    اگر (منوال & منوال_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    واپس (char_u *)mapmode.ga_ڈیٹا;
}

/*
 * Output a line for one mapping.
 */
    ساکن خالی
showmap(
    mapblock_T	*mp,
    صحیح		local)	    // TRUE for buffer-local map
{
    صحیح		len = 1;
    char_u	*mapchars;

    اگر (پیغام_filtered(mp->m_کلیدیں) && پیغام_filtered(mp->m_str))
	واپس;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    اگر (پیغام_didout || پیغام_silent != 0)
    {
	پیغام_putchar('\n');
	اگر (got_int)	    // 'q' typed at MORE prompt
	    جاؤ theend;
    }

    mapchars = map_منوال_مقصد_chars(mp->m_منوال);
    اگر (mapchars != تہی)
    {
	پیغام_puts((حرف *)mapchars);
	len = (صحیح)STRLEN(mapchars);
	vim_free(mapchars);
    }

    جب (++len <= 3)
	پیغام_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = پیغام_outtrans_special(mp->m_کلیدیں, TRUE, 0);
    کرو
    {
	پیغام_putchar(' ');		// pad with blanks
	++len;
    } جب (len < 12);

    اگر (mp->m_noremap == REMAP_NONE)
	پیغام_puts_attr("*", HL_ATTR(HLF_8));
    ورنہ اگر (mp->m_noremap == REMAP_SCRIPT)
	پیغام_puts_attr("&", HL_ATTR(HLF_8));
    ورنہ
	پیغام_putchar(' ');

    اگر (local)
	پیغام_putchar('@');
    ورنہ
	پیغام_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    اگر (*mp->m_str == NUL)
	پیغام_puts_attr("<Nop>", HL_ATTR(HLF_8));
    ورنہ
	پیغام_outtrans_special(mp->m_str, FALSE, 0);
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (p_verbose > 0)
	آخری_سیٹ_پیغام(mp->m_script_ctx);
#اگر_ختم
    پیغام_clr_eos();
    برآمد_flush();			// show one line at a time

theend:
    --map_locked;
}

    ساکن mapblock_T *
map_جوڑو(
	mapblock_T  **map_جدول,
	mapblock_T  **abbr_جدول,
	char_u	    *کلیدیں,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	صحیح	    noremap,
	صحیح	    nowait,
	صحیح	    silent,
	صحیح	    منوال,
	صحیح	    is_abbr,
#اگر_تعریف_شدہ FEAT_EVAL
	صحیح	    اظہاریہ,
	scid_T	    sid,	    // 0 to use current_sctx
	صحیح	    scriptversion,
	linenr_T    lnum,
#اگر_ختم
	صحیح	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    اگر (mp == تہی)
	واپس تہی;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    اگر (*کلیدیں == Ctrl_C)
    {
	اگر (map_جدول == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= منوال;
	ورنہ
	    mapped_ctrl_c |= منوال;
    }

    mp->m_کلیدیں = vim_strsave(کلیدیں);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    اگر (mp->m_کلیدیں == تہی || mp->m_str == تہی)
    {
	vim_free(mp->m_کلیدیں);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	واپس تہی;
    }
    mp->m_keylen = (صحیح)STRLEN(mp->m_کلیدیں);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_منوال = منوال;
    mp->m_simplified = simplified;
#اگر_تعریف_شدہ FEAT_EVAL
    mp->m_اظہاریہ = اظہاریہ;
    اگر (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_نسخہ = scriptversion;
    }
    ورنہ
    {
	mp->m_script_ctx = current_sctx;
	اگر (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_نسخہ = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#اگر_ختم

    // add the new entry in front of the abbrlist or maphash[] list
    اگر (is_abbr)
    {
	mp->m_اگلا = *abbr_جدول;
	*abbr_جدول = mp;
    }
    ورنہ
    {
	صحیح n = MAP_HASH(mp->m_منوال, mp->m_کلیدیں[0]);

	mp->m_اگلا = map_جدول[n];
	map_جدول[n] = mp;
    }
    واپس mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    ساکن خالی
list_mappings(
	صحیح	keyround,
	صحیح	abbrev,
	صحیح	haskey,
	char_u	*کلیدیں,
	صحیح	کلیدیں_len,
	صحیح	منوال,
	صحیح	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    اگر (p_verbose > 0 && keyround == 1)
    {
	اگر (نظر_آیا_Modify_Other_کلیدیں)
	    پیغام_puts(_("Seen modifyOtherKeys: true\n"));

	اگر (modify_otherkeys_state != MOKS_INITIAL)
	{
	    حرف *name = _("Unknown");
	    انتخاب (modify_otherkeys_state)
	    {
		حالت MOKS_INITIAL: توڑو;
		حالت MOKS_OFF: name = _("Off"); توڑو;
		حالت MOKS_ENABLED: name = _("On"); توڑو;
		حالت MOKS_DISABLED: name = _("Disabled"); توڑو;
		حالت MOKS_AFTER_T_TE: name = _("Cleared"); توڑو;
	    }

	    حرف buf[200];
	    vim_snprintf(buf, حجم(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    پیغام_puts(buf);
	}

	اگر (kitty_protocol_state != KKPS_INITIAL)
	{
	    حرف *name = _("Unknown");
	    انتخاب (kitty_protocol_state)
	    {
		حالت KKPS_INITIAL: توڑو;
		حالت KKPS_OFF: name = _("Off"); توڑو;
		حالت KKPS_ENABLED: name = _("On"); توڑو;
		حالت KKPS_DISABLED: name = _("Disabled"); توڑو;
		حالت KKPS_AFTER_T_TE: name = _("Cleared"); توڑو;
	    }

	    حرف buf[200];
	    vim_snprintf(buf, حجم(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    پیغام_puts(buf);
	}
    }

    // need to loop over all global hash lists
    برائے (صحیح hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	اگر (abbrev)
	{
	    اگر (hash != 0)	// there is only one abbreviation list
		توڑو;
	    mp = curbuf->b_پہلا_abbr;
	}
	ورنہ
	    mp = curbuf->b_maphash[hash];
	برائے ( ; mp != تہی && !got_int; mp = mp->m_اگلا)
	{
	    // check entries with the same mode
	    اگر (!mp->m_simplified && (mp->m_منوال & منوال) != 0)
	    {
		اگر (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		ورنہ
		{
		    صحیح n = mp->m_keylen;
		    اگر (STRNCMP(mp->m_کلیدیں, کلیدیں,
				   (size_t)(n < کلیدیں_len ? n : کلیدیں_len)) == 0)
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
    صحیح		منوال,
    صحیح		abbrev)		// not a mapping but an abbreviation
{
    char_u	*کلیدیں;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_نتیجہ[2] = {تہی, تہی};
    char_u	*rhs;
    char_u	*p;
    صحیح		n;
    صحیح		len = 0;	// init for GCC
    صحیح		hasarg;
    صحیح		haskey;
    صحیح		do_print;
    صحیح		keyround;
    char_u	*کلیدیں_buf = تہی;
    char_u	*alt_کلیدیں_buf = تہی;
    char_u	*arg_buf = تہی;
    صحیح		retval = 0;
    صحیح		do_backslash;
    mapblock_T	**abbr_جدول;
    mapblock_T	**map_جدول;
    صحیح		unique = FALSE;
    صحیح		nowait = FALSE;
    صحیح		silent = FALSE;
    صحیح		special = FALSE;
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح		اظہاریہ = FALSE;
#اگر_ختم
    صحیح		did_simplify = FALSE;
    صحیح		unmap_lhs_only = FALSE;
    صحیح		noremap;
    char_u      *orig_rhs;

    کلیدیں = arg;
    map_جدول = maphash;
    abbr_جدول = &پہلا_abbr;

    اگر (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    اگر (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    ورنہ
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    برائے (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	اگر (STRNCMP(کلیدیں, "<buffer>", 8) == 0)
	{
	    کلیدیں = skipwhite(کلیدیں + 8);
	    map_جدول = curbuf->b_maphash;
	    abbr_جدول = &curbuf->b_پہلا_abbr;
	    جاری;
	}

	// Check for "<nowait>": don't wait for more characters.
	اگر (STRNCMP(کلیدیں, "<nowait>", 8) == 0)
	{
	    کلیدیں = skipwhite(کلیدیں + 8);
	    nowait = TRUE;
	    جاری;
	}

	// Check for "<silent>": don't echo commands.
	اگر (STRNCMP(کلیدیں, "<silent>", 8) == 0)
	{
	    کلیدیں = skipwhite(کلیدیں + 8);
	    silent = TRUE;
	    جاری;
	}

	// Check for "<special>": accept special keys in <>
	اگر (STRNCMP(کلیدیں, "<special>", 9) == 0)
	{
	    کلیدیں = skipwhite(کلیدیں + 9);
	    special = TRUE;
	    جاری;
	}

#اگر_تعریف_شدہ FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	اگر (STRNCMP(کلیدیں, "<script>", 8) == 0)
	{
	    کلیدیں = skipwhite(کلیدیں + 8);
	    noremap = REMAP_SCRIPT;
	    جاری;
	}

	// Check for "<expr>": {rhs} is an expression.
	اگر (STRNCMP(کلیدیں, "<expr>", 6) == 0)
	{
	    کلیدیں = skipwhite(کلیدیں + 6);
	    اظہاریہ = TRUE;
	    جاری;
	}
#اگر_ختم
	// Check for "<unique>": don't overwrite an existing mapping.
	اگر (STRNCMP(کلیدیں, "<unique>", 8) == 0)
	{
	    کلیدیں = skipwhite(کلیدیں + 8);
	    unique = TRUE;
	    جاری;
	}
	توڑو;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = کلیدیں;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == تہی);
    جب (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
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
    haskey = (*کلیدیں != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    اگر (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	جاؤ theend;
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
	char_u	*new_کلیدیں;
	صحیح	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	اگر (special)
	    flags |= REPTERM_SPECIAL;
	new_کلیدیں = replace_termcodes(کلیدیں, &کلیدیں_buf, 0, flags, &did_simplify);
	اگر (did_simplify)
	    (خالی)replace_termcodes(کلیدیں, &alt_کلیدیں_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, تہی);
	کلیدیں = new_کلیدیں;
    }
    orig_rhs = rhs;
    اگر (hasarg)
    {
	اگر (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	ورنہ
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), تہی);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    برائے (keyround = 1; keyround <= 2; ++keyround)
    {
	صحیح	did_it = FALSE;
	صحیح	did_local = FALSE;
	صحیح	keyround1_simplified = keyround == 1 && did_simplify;
	صحیح	باری;
	صحیح	عدد_دور;

	اگر (keyround == 2)
	{
	    اگر (alt_کلیدیں_buf == تہی)
		توڑو;
	    کلیدیں = alt_کلیدیں_buf;
	}
	ورنہ اگر (alt_کلیدیں_buf != تہی && do_print)
	    // when printing always use the not-simplified map
	    کلیدیں = alt_کلیدیں_buf;

	// check arguments and translate function keys
	اگر (haskey)
	{
	    len = (صحیح)STRLEN(کلیدیں);
	    اگر (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		جاؤ theend;
	    }

	    اگر (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		اگر (has_mbyte)
		{
		    صحیح		پہلا, آخری;
		    صحیح		same = -1;
		    char_u	کلیدیں_unescaped[MAXMAPLEN + 1];
		    size_t	کلیدیں_unescaped_len;

		    mch_memmove(کلیدیں_unescaped, کلیدیں, (size_t)(len + 1));
		    کلیدیں_unescaped_len = vim_unescape_csi(کلیدیں_unescaped);
		    p = کلیدیں_unescaped;

		    پہلا = vim_iswordp(p);
		    آخری = پہلا;
		    MB_PTR_ADV(p);
		    n = 1;
		    جب (p < کلیدیں_unescaped + کلیدیں_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			آخری = vim_iswordp(p);	// type of last char
			اگر (same == -1 && آخری != پہلا)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    اگر (آخری && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			جاؤ theend;
		    }
		}
		ورنہ اگر (vim_iswordc(کلیدیں[len - 1]))
		    // ends in keyword char
		    برائے (n = 0; n < len - 2; ++n)
			اگر (vim_iswordc(کلیدیں[n]) != vim_iswordc(کلیدیں[len - 2]))
			{
			    retval = 1;
			    جاؤ theend;
			}
		// An abbreviation cannot contain white space.
		برائے (n = 0; n < len; ++n)
		    اگر (VIM_ISWHITE(کلیدیں[n]))
		    {
			retval = 1;
			جاؤ theend;
		    }
	    }
	}

	اگر (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	اگر (do_print)
	    پیغام_آغاز();

	// Check if a new local mapping wasn't already defined globally.
	اگر (unique && map_جدول == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    برائے (صحیح hash = 0; hash < 256 && !got_int; ++hash)
	    {
		اگر (abbrev)
		{
		    اگر (hash != 0)	// there is only one abbreviation list
			توڑو;
		    mp = پہلا_abbr;
		}
		ورنہ
		    mp = maphash[hash];
		برائے ( ; mp != تہی && !got_int; mp = mp->m_اگلا)
		{
		    // check entries with the same mode
		    اگر ((mp->m_منوال & منوال) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_کلیدیں, کلیدیں, (size_t)len) == 0)
		    {
			اگر (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_کلیدیں);
			ورنہ
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_کلیدیں);
			retval = 5;
			جاؤ theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	اگر (map_جدول != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, کلیدیں, len,
							     منوال, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	عدد_دور = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	برائے (باری = 0; باری < عدد_دور && !did_it && !got_int; ++باری)
	{
	    // need to loop over all hash lists
	    برائے (صحیح hash = 0; hash < 256 && !got_int; ++hash)
	    {
		اگر (abbrev)
		{
		    اگر (hash > 0)	// there is only one abbreviation list
			توڑو;
		    mpp = abbr_جدول;
		}
		ورنہ
		    mpp = &(map_جدول[hash]);
		برائے (mp = *mpp; mp != تہی && !got_int; mp = *mpp)
		{

		    اگر ((mp->m_منوال & منوال) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_اگلا);
			جاری;
		    }
		    اگر (!haskey)	// show all entries
		    {
			اگر (!mp->m_simplified)
			{
			    showmap(mp, map_جدول != maphash);
			    did_it = TRUE;
			}
		    }
		    ورنہ	// do we have a match?
		    {
			اگر (باری)	// second round: Try unmap "rhs" string
			{
			    n = (صحیح)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			ورنہ
			{
			    n = mp->m_keylen;
			    p = mp->m_کلیدیں;
			}
			اگر (STRNCMP(p, کلیدیں, (size_t)(n < len ? n : len)) == 0)
			{
			    اگر (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				اگر (n != len && (!abbrev || باری || n > len
					       || *skipwhite(کلیدیں + n) != NUL))
				{
				    mpp = &(mp->m_اگلا);
				    جاری;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				اگر (keyround1_simplified && !mp->m_simplified)
				    توڑو;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_منوال &= ~منوال;
				did_it = TRUE;	// remember we did something
			    }
			    ورنہ اگر (!hasarg)	// show matching entry
			    {
				اگر (!mp->m_simplified)
				{
				    showmap(mp, map_جدول != maphash);
				    did_it = TRUE;
				}
			    }
			    ورنہ اگر (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_اگلا);
				جاری;
			    }
			    ورنہ اگر (unique)
			    {
				اگر (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				ورنہ
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				جاؤ theend;
			    }
			    ورنہ
			    {
				// new rhs for existing entry
				mp->m_منوال &= ~منوال;	// remove mode bits
				اگر (mp->m_منوال == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    اگر (newstr == تہی)
				    {
					retval = 4;		// no mem
					جاؤ theend;
				    }
				    اگر (mp->m_alt != تہی)
					mp->m_alt = mp->m_alt->m_alt = تہی;
				    vim_free(mp->m_str);
				    mp->m_str = newstr;
				    vim_free(mp->m_orig_str);
				    mp->m_orig_str = vim_strsave(orig_rhs);
				    mp->m_noremap = noremap;
				    mp->m_nowait = nowait;
				    mp->m_silent = silent;
				    mp->m_منوال = منوال;
				    mp->m_simplified = keyround1_simplified;
#اگر_تعریف_شدہ FEAT_EVAL
				    mp->m_اظہاریہ = اظہاریہ;
				    mp->m_script_ctx = current_sctx;
				    اگر (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_نسخہ = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#اگر_ختم
				    mp_نتیجہ[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    اگر (mp->m_منوال == 0)  // entry can be deleted
			    {
				map_free(mpp);
				جاری;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    صحیح new_hash = MAP_HASH(mp->m_منوال, mp->m_کلیدیں[0]);
			    اگر (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_اگلا;
				mp->m_اگلا = map_جدول[new_hash];
				map_جدول[new_hash] = mp;

				جاری;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_اگلا);
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
	    ورنہ اگر (*کلیدیں == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		اگر (map_جدول == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~منوال;
		ورنہ
		    mapped_ctrl_c &= ~منوال;
	    }
	    جاری;
	}

	اگر (!haskey || !hasarg)
	{
	    // print entries
	    اگر (!did_it && !did_local)
	    {
		اگر (abbrev)
		    پیغام(_("No abbreviation found"));
		ورنہ
		    پیغام(_("No mapping found"));
	    }
	    جاؤ theend;    // listing finished
	}

	اگر (did_it)
	    جاری;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_نتیجہ[keyround - 1] = map_جوڑو(map_جدول, abbr_جدول, کلیدیں,
		    rhs, orig_rhs, noremap, nowait, silent, منوال, abbrev,
#اگر_تعریف_شدہ FEAT_EVAL
		    اظہاریہ, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#اگر_ختم
		    keyround1_simplified);
	اگر (mp_نتیجہ[keyround - 1] == تہی)
	{
	    retval = 4;	    // no mem
	    جاؤ theend;
	}
    }

    اگر (mp_نتیجہ[0] != تہی && mp_نتیجہ[1] != تہی)
    {
	mp_نتیجہ[0]->m_alt = mp_نتیجہ[1];
	mp_نتیجہ[1]->m_alt = mp_نتیجہ[0];
    }

theend:
    vim_free(کلیدیں_buf);
    vim_free(alt_کلیدیں_buf);
    vim_free(arg_buf);
    واپس retval;
}

/*
 * Get the mapping mode from the command name.
 */
    ساکن صحیح
get_map_منوال(char_u **cmdp, صحیح forceit)
{
    char_u	*p;
    صحیح		modec;
    صحیح		منوال;

    p = *cmdp;
    modec = *p++;
    اگر (modec == 'i')
	منوال = منوال_داخل_کرو;				// :imap
    ورنہ اگر (modec == 'l')
	منوال = منوال_LANGMAP;				// :lmap
    ورنہ اگر (modec == 'c')
	منوال = منوال_CMDLINE;				// :cmap
    ورنہ اگر (modec == 'n' && *p != 'o')		    // avoid :noremap
	منوال = منوال_معمول;				// :nmap
    ورنہ اگر (modec == 'v')
	منوال = منوال_VISUAL | منوال_SELECT;		// :vmap
    ورنہ اگر (modec == 'x')
	منوال = منوال_VISUAL;				// :xmap
    ورنہ اگر (modec == 's')
	منوال = منوال_SELECT;			// :smap
    ورنہ اگر (modec == 'o')
	منوال = منوال_OP_PENDING;			// :omap
    ورنہ اگر (modec == 't')
	منوال = منوال_TERMINAL;			// :tmap
    ورنہ
    {
	--p;
	اگر (forceit)
	    منوال = منوال_داخل_کرو | منوال_CMDLINE;		// :map !
	ورنہ
	    منوال = منوال_VISUAL | منوال_SELECT | منوال_معمول | منوال_OP_PENDING;
							// :map
    }

    *cmdp = p;
    واپس منوال;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    ساکن خالی
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    صحیح		forceit,
    صحیح		abbr)
{
    صحیح		منوال;
    صحیح		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    اگر (!local && *arg != NUL)
    {
	emsg(_(e_invalid_دلیل));
	واپس;
    }

    منوال = get_map_منوال(&cmdp, forceit);
    map_clear_منوال(curbuf, منوال, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ساکن صحیح
is_map_locked(خالی)
{
    اگر (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    خالی
map_clear_منوال(
    buf_T	*buf,		// buffer for local mappings
    صحیح		منوال,		// mode in which to delete
    صحیح		local,		// TRUE for buffer-local mappings
    صحیح		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    صحیح		hash;
    صحیح		new_hash;

    اگر (is_map_locked())
	واپس;

    validate_maphash();

    برائے (hash = 0; hash < 256; ++hash)
    {
	اگر (abbr)
	{
	    اگر (hash > 0)	// there is only one abbrlist
		توڑو;
	    اگر (local)
		mpp = &buf->b_پہلا_abbr;
	    ورنہ
		mpp = &پہلا_abbr;
	}
	ورنہ
	{
	    اگر (local)
		mpp = &buf->b_maphash[hash];
	    ورنہ
		mpp = &maphash[hash];
	}
	جب (*mpp != تہی)
	{
	    mp = *mpp;
	    اگر (mp->m_منوال & منوال)
	    {
		mp->m_منوال &= ~منوال;
		اگر (mp->m_منوال == 0) // entry can be deleted
		{
		    map_free(mpp);
		    جاری;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_منوال, mp->m_کلیدیں[0]);
		اگر (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_اگلا;
		    اگر (local)
		    {
			mp->m_اگلا = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    ورنہ
		    {
			mp->m_اگلا = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    جاری;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_اگلا);
	}
    }
}

#اگر defined(FEAT_EVAL)
    صحیح
منوال_str2flags(char_u *modechars)
{
    صحیح		منوال = 0;

    اگر (vim_strchr(modechars, 'n') != تہی)
	منوال |= منوال_معمول;
    اگر (vim_strchr(modechars, 'v') != تہی)
	منوال |= منوال_VISUAL | منوال_SELECT;
    اگر (vim_strchr(modechars, 'x') != تہی)
	منوال |= منوال_VISUAL;
    اگر (vim_strchr(modechars, 's') != تہی)
	منوال |= منوال_SELECT;
    اگر (vim_strchr(modechars, 'o') != تہی)
	منوال |= منوال_OP_PENDING;
    اگر (vim_strchr(modechars, 'i') != تہی)
	منوال |= منوال_داخل_کرو;
    اگر (vim_strchr(modechars, 'l') != تہی)
	منوال |= منوال_LANGMAP;
    اگر (vim_strchr(modechars, 'c') != تہی)
	منوال |= منوال_CMDLINE;

    واپس منوال;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    صحیح
map_مقصد_exists(char_u *str, char_u *modechars, صحیح abbr)
{
    char_u	*rhs;
    char_u	*buf;
    صحیح		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, تہی);

    retval = map_مقصد_exists_منوال(rhs, منوال_str2flags(modechars), abbr);
    vim_free(buf);

    واپس retval;
}
#اگر_ختم

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    صحیح
map_مقصد_exists_منوال(char_u *rhs, صحیح منوال, صحیح abbr)
{
    mapblock_T	*mp;
    صحیح		hash;
    صحیح		exp_بفر = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    برائے (;;)
    {
	برائے (hash = 0; hash < 256; ++hash)
	{
	    اگر (abbr)
	    {
		اگر (hash > 0)		// there is only one abbr list
		    توڑو;
		اگر (exp_بفر)
		    mp = curbuf->b_پہلا_abbr;
		ورنہ
		    mp = پہلا_abbr;
	    }
	    ورنہ اگر (exp_بفر)
		mp = curbuf->b_maphash[hash];
	    ورنہ
		mp = maphash[hash];
	    برائے (; mp; mp = mp->m_اگلا)
	    {
		اگر ((mp->m_منوال & منوال)
			&& لڑی_تلاش((حرف *)mp->m_str, (حرف *)rhs) != تہی)
		    واپس TRUE;
	    }
	}
	اگر (exp_بفر)
	    توڑو;
	exp_بفر = TRUE;
    }

    واپس FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
ساکن صحیح	expand_mapmodes = 0;
ساکن صحیح	expand_isabbrev = 0;
ساکن صحیح	expand_بفر = FALSE;

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
    ساکن char_u *
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

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != تہی);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != تہی);

    برائے (; *str; ++str)
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
		    واپس تہی;
		}
		c = مقصد_SPECIAL(str[1], str[2]);
		اگر (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    اگر (IS_SPECIAL(c) || modifiers)	// special key
	    {
		اگر (cpo_special)
		{
		    ga_clear(&ga);
		    واپس تہی;
		}
		ga_concat(&ga, get_special_کلید_name(c, modifiers));
		جاری; // for (str)
	    }
	}
	اگر (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	اگر (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    واپس (char_u *)(ga.ga_ڈیٹا);
}

/*
 * Work out what to complete when doing command line completion of mapping
 * or abbreviation names.
 */
    char_u *
سیٹ_context_اندر_map_cmd(
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
    ورنہ
    {
	اگر (isunmap)
	    expand_mapmodes = get_map_منوال(&cmd, forceit || isabbrev);
	ورنہ
	{
	    expand_mapmodes = منوال_داخل_کرو | منوال_CMDLINE;
	    اگر (!isabbrev)
		expand_mapmodes += منوال_VISUAL | منوال_SELECT | منوال_معمول
							     | منوال_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_بفر = FALSE;
	برائے (;;)
	{
	    اگر (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_بفر = TRUE;
		arg = skipwhite(arg + 8);
		جاری;
	    }
	    اگر (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		جاری;
	    }
	    اگر (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		جاری;
	    }
	    اگر (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		جاری;
	    }
	    اگر (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		جاری;
	    }
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		جاری;
	    }
	    اگر (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		جاری;
	    }
#اگر_ختم
	    توڑو;
	}
	xp->xp_نمونہ = arg;
    }

    واپس تہی;
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
    صحیح		*عدد_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    صحیح		hash;
    صحیح		گنتی;
    char_u	*p;
    صحیح		i;
    صحیح		fuzzy;
    صحیح		مطابقت;
    صحیح		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *عدد_Matches = 0;		    // return values in case of FAIL
    *matches = تہی;

    اگر (!fuzzy)
	ga_init2(&ga, حجم(حرف *), 3);
    ورنہ
	ga_init2(&ga, حجم(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    برائے (i = 0; i < 7; ++i)
    {
	اگر (i == 0)
	    p = (char_u *)"<silent>";
	ورنہ اگر (i == 1)
	    p = (char_u *)"<unique>";
#اگر_تعریف_شدہ FEAT_EVAL
	ورنہ اگر (i == 2)
	    p = (char_u *)"<script>";
	ورنہ اگر (i == 3)
	    p = (char_u *)"<expr>";
#اگر_ختم
	ورنہ اگر (i == 4 && !expand_بفر)
	    p = (char_u *)"<buffer>";
	ورنہ اگر (i == 5)
	    p = (char_u *)"<nowait>";
	ورنہ اگر (i == 6)
	    p = (char_u *)"<special>";
	ورنہ
	    جاری;

	اگر (!fuzzy)
	    مطابقت = vim_regexec(regmatch, p, (colnr_T)0);
	ورنہ
	{
	    score = fuzzy_مطابقت_str(p, pat);
	    مطابقت = (score != FUZZY_SCORE_NONE);
	}

	اگر (!مطابقت)
	    جاری;

	اگر (ga_grow(&ga, 1) == FAIL)
	    توڑو;

	اگر (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_ڈیٹا)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	ورنہ
	    ((char_u **)ga.ga_ڈیٹا)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    برائے (hash = 0; hash < 256; ++hash)
    {
	اگر (expand_isabbrev)
	{
	    اگر (hash > 0)	// only one abbrev list
		توڑو; // for (hash)
	    mp = پہلا_abbr;
	}
	ورنہ اگر (expand_بفر)
	    mp = curbuf->b_maphash[hash];
	ورنہ
	    mp = maphash[hash];
	برائے (; mp; mp = mp->m_اگلا)
	{
	    اگر (mp->m_simplified || !(mp->m_منوال & expand_mapmodes))
		جاری;

	    p = translate_mapping(mp->m_کلیدیں);
	    اگر (p == تہی)
		جاری;

	    اگر (!fuzzy)
		مطابقت = vim_regexec(regmatch, p, (colnr_T)0);
	    ورنہ
	    {
		score = fuzzy_مطابقت_str(p, pat);
		مطابقت = (score != FUZZY_SCORE_NONE);
	    }

	    اگر (!مطابقت)
	    {
		vim_free(p);
		جاری;
	    }

	    اگر (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		توڑو;
	    }

	    اگر (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_ڈیٹا)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ورنہ
		((char_u **)ga.ga_ڈیٹا)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    اگر (ga.ga_len == 0)
	واپس FAIL;

    اگر (!fuzzy)
    {
	*matches = ga.ga_ڈیٹا;
	*عدد_Matches = ga.ga_len;
    }
    ورنہ
    {
	اگر (fuzzymatches_مقصد_strmatches(ga.ga_ڈیٹا, matches, ga.ga_len,
							FALSE) == FAIL)
	    واپس FAIL;
	*عدد_Matches = ga.ga_len;
    }

    گنتی = *عدد_Matches;
    اگر (گنتی > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	اگر (!fuzzy)
	    ترتیب_دو_لڑیاں(*matches, گنتی);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + گنتی;

	جب (ptr2 < ptr3)
	{
	    اگر (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    ورنہ
	    {
		vim_free(*ptr2++);
		گنتی--;
	    }
	}
    }

    *عدد_Matches = گنتی;
    واپس (گنتی == 0 ? FAIL : OK);
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
    صحیح		ستون,
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
	واپس FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    اگر (noremap_کلیدیں() && c != Ctrl_RSB)
	واپس FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    اگر (ستون == 0)				// cannot be an abbr.
	واپس FALSE;

    اگر (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + ستون);
	اگر (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	ورنہ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    اگر (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	جب (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    اگر (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		توڑو;
	    }
	    ++clen;
	}
	scol = (صحیح)(p - ptr);
    }
    ورنہ
    {
	اگر (!vim_iswordc(ptr[ستون - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	ورنہ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    اگر (ستون > 1)
		is_id = vim_iswordc(ptr[ستون - 2]);
	}
	برائے (scol = ستون - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    اگر (scol < mincol)
	scol = mincol;
    اگر (scol < ستون)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = ستون - scol;
	mp = curbuf->b_پہلا_abbr;
	mp2 = پہلا_abbr;
	اگر (mp == تہی)
	{
	    mp = mp2;
	    mp2 = تہی;
	}
	برائے ( ; mp; mp->m_اگلا == تہی
				  ? (mp = mp2, mp2 = تہی) : (mp = mp->m_اگلا))
	{
	    صحیح		qlen = mp->m_keylen;
	    char_u	*q = mp->m_کلیدیں;
	    صحیح		مطابقت;

	    اگر (vim_strbyte(mp->m_کلیدیں, K_SPECIAL) != تہی)
	    {
		char_u *qe = vim_strsave(mp->m_کلیدیں);

		// might have CSI escaped mp->m_keys
		اگر (qe != تہی)
		{
		    q = qe;
		    qlen = (صحیح)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    مطابقت =    (mp->m_منوال & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    اگر (q != mp->m_کلیدیں)
		vim_free(q);
	    اگر (مطابقت)
		توڑو;
	}
	اگر (mp != تہی)
	{
	    صحیح	noremap;
	    صحیح silent;
#اگر_تعریف_شدہ FEAT_EVAL
	    صحیح اظہاریہ;
#اگر_ختم

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
		    tb[j++] = K_دوسرا(c);
		    tb[j++] = K_تیسرا(c);
		}
		ورنہ
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
			اگر (escaped != تہی)
			{
			    newlen = (صحیح)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    ورنہ
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(خالی)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#اگر_تعریف_شدہ FEAT_EVAL
	    اظہاریہ = mp->m_اظہاریہ;

	    اگر (اظہاریہ)
		s = eval_map_اظہاریہ(mp, c);
	    ورنہ
#اگر_ختم
		s = mp->m_str;
	    اگر (s != تہی)
	    {
					// insert the to string
		(خالی)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (صحیح)STRLEN(s) + j + 1;
#اگر_تعریف_شدہ FEAT_EVAL
		اگر (اظہاریہ)
		    vim_free(s);
#اگر_ختم
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    اگر (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    جب (len-- > 0)		// delete the from string
		(خالی)ins_typebuf(tb, 1, 0, TRUE, silent);
	    واپس TRUE;
	}
    }
    واپس FALSE;
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_اظہاریہ(
    mapblock_T	*mp,
    صحیح		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*اظہاریہ;
    pos_T	save_cursor;
    صحیح		save_پیغام_ستون;
    صحیح		save_پیغام_صف;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    صحیح		save_sctx_نسخہ = current_sctx.sc_نسخہ;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    اظہاریہ = vim_strsave(mp->m_str);
    اگر (اظہاریہ == تہی)
	واپس تہی;
    vim_unescape_csi(اظہاریہ);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_معمول_تالا;
    سیٹ_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_پیغام_ستون = پیغام_ستون;
    save_پیغام_صف = پیغام_صف;

    current_sctx.sc_نسخہ = mp->m_script_ctx.sc_نسخہ;
    اگر (mp->m_script_ctx.sc_نسخہ == SCRIPT_نسخہ_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_مقصد_string(اظہاریہ, FALSE, FALSE);

    --textlock;
    --ex_معمول_تالا;
    curwin->w_cursor = save_cursor;
    پیغام_ستون = save_پیغام_ستون;
    پیغام_صف = save_پیغام_صف;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_نسخہ = save_sctx_نسخہ;

    vim_free(اظہاریہ);

    اگر (p == تہی)
	واپس تہی;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    واپس res;
}
#اگر_ختم

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
    اگر (res == تہی)
	واپس تہی;

    d = res;
    برائے (s = p; *s != NUL; )
    {
	اگر ((s[0] == K_SPECIAL
#اگر_تعریف_شدہ FEAT_GUI
		    || (gui.اندر_use && s[0] == CSI)
#اگر_ختم
	    ) && s[1] != NUL && s[2] != NUL)
	{
	    // Copy special key unmodified.
	    *d++ = *s++;
	    *d++ = *s++;
	    *d++ = *s++;
	}
	ورنہ
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = جوڑو_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    واپس res;
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

    جب (*s != NUL)
    {
	اگر (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	ورنہ اگر ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (صحیح)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	ورنہ
	    *d++ = *s++;
    }
    *d = NUL;
    واپس (size_t)(d - p);
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
    حرف	*cmd;
    صحیح		abbr;
    صحیح		hash;
    صحیح		did_cpo = FALSE;
    صحیح		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    برائے (abbr = 0; abbr < 2; ++abbr)
	برائے (hash = 0; hash < 256; ++hash)
	{
	    اگر (abbr)
	    {
		اگر (hash > 0)		// there is only one abbr list
		    توڑو;
		اگر (buf != تہی)
		    mp = buf->b_پہلا_abbr;
		ورنہ
		    mp = پہلا_abbr;
	    }
	    ورنہ
	    {
		اگر (buf != تہی)
		    mp = buf->b_maphash[hash];
		ورنہ
		    mp = maphash[hash];
	    }

	    برائے ( ; mp; mp = mp->m_اگلا)
	    {
		// simplified map blocks are not created explicitly
		اگر (mp->m_simplified)
		    جاری;

		// skip script-local mappings
		اگر (mp->m_noremap == REMAP_SCRIPT)
		    جاری;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		برائے (p = mp->m_str; *p != NUL; ++p)
		    اگر (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (صحیح)KE_SNR)
			توڑو;
		اگر (*p != NUL)
		    جاری;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		اگر (abbr)
		    cmd = "abbr";
		ورنہ
		    cmd = "map";
		انتخاب (mp->m_منوال)
		{
		    حالت منوال_معمول | منوال_VISUAL | منوال_SELECT
							     | منوال_OP_PENDING:
			توڑو;
		    حالت منوال_معمول:
			c1 = 'n';
			توڑو;
		    حالت منوال_VISUAL:
			c1 = 'x';
			توڑو;
		    حالت منوال_SELECT:
			c1 = 's';
			توڑو;
		    حالت منوال_OP_PENDING:
			c1 = 'o';
			توڑو;
		    حالت منوال_معمول | منوال_VISUAL:
			c1 = 'n';
			c2 = 'x';
			توڑو;
		    حالت منوال_معمول | منوال_SELECT:
			c1 = 'n';
			c2 = 's';
			توڑو;
		    حالت منوال_معمول | منوال_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			توڑو;
		    حالت منوال_VISUAL | منوال_SELECT:
			c1 = 'v';
			توڑو;
		    حالت منوال_VISUAL | منوال_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			توڑو;
		    حالت منوال_SELECT | منوال_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			توڑو;
		    حالت منوال_معمول | منوال_VISUAL | منوال_SELECT:
			c1 = 'n';
			c2 = 'v';
			توڑو;
		    حالت منوال_معمول | منوال_VISUAL | منوال_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			توڑو;
		    حالت منوال_معمول | منوال_SELECT | منوال_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			توڑو;
		    حالت منوال_VISUAL | منوال_SELECT | منوال_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			توڑو;
		    حالت منوال_CMDLINE | منوال_داخل_کرو:
			اگر (!abbr)
			    cmd = "map!";
			توڑو;
		    حالت منوال_CMDLINE:
			c1 = 'c';
			توڑو;
		    حالت منوال_داخل_کرو:
			c1 = 'i';
			توڑو;
		    حالت منوال_LANGMAP:
			c1 = 'l';
			توڑو;
		    حالت منوال_TERMINAL:
			c1 = 't';
			توڑو;
		    طےشدہ:
			iemsg(e_makemap_illegal_منوال);
			واپس FAIL;
		}
		کرو	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    اگر (!did_cpo)
		    {
			اگر (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			ورنہ
			    برائے (i = 0; i < 2; ++i)
				برائے (p = (i ? mp->m_str : mp->m_کلیدیں); *p; ++p)
				    اگر (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			اگر (did_cpo)
			{
			    اگر (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				واپس FAIL;
			}
		    }
#اگر_تعریف_شدہ FEAT_EVAL
		    // If it is not vim9 use legacy
		    اگر (mp->m_اظہاریہ && mp->m_script_ctx.sc_نسخہ < SCRIPT_نسخہ_VIM9
			    && fputs("legacy ", fd) < 0)
			واپس FAIL;
#اگر_ختم
		    اگر (c1 && putc(c1, fd) < 0)
			واپس FAIL;
		    اگر (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			واپس FAIL;
		    اگر (fputs(cmd, fd) < 0)
			واپس FAIL;
		    اگر (buf != تہی && fputs(" <buffer>", fd) < 0)
			واپس FAIL;
		    اگر (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			واپس FAIL;
		    اگر (mp->m_silent && fputs(" <silent>", fd) < 0)
			واپس FAIL;
#اگر_تعریف_شدہ FEAT_EVAL
		    اگر (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			واپس FAIL;
		    اگر (mp->m_اظہاریہ && fputs(" <expr>", fd) < 0)
			واپس FAIL;
#اگر_ختم

		    اگر (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_کلیدیں, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			واپس FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} جب (c1 != NUL);
	    }
	}

    اگر (did_cpo)
	اگر (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    واپس FAIL;
    واپس OK;
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
	    واپس FAIL;
	واپس OK;
    }

    برائے ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	اگر (p != تہی)
	{
	    جب (*p != NUL)
		اگر (fputc(*p++, fd) < 0)
		    واپس FAIL;
	    --str;
	    جاری;
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

		اگر (p == تہی)
		    c = *str;
		ورنہ
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    اگر (c == K_SPECIAL)
	    {
		c = مقصد_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    اگر (IS_SPECIAL(c) || modifiers)	// special key
	    {
		اگر (fputs((حرف *)get_special_کلید_name(c, modifiers), fd) < 0)
		    واپس FAIL;
		جاری;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	اگر (c == NL)
	{
	    اگر (what == 2)
	    {
		اگر (fprintf(fd, "\\\026\n") < 0)
		    واپس FAIL;
	    }
	    ورنہ
	    {
		اگر (fprintf(fd, "<NL>") < 0)
		    واپس FAIL;
	    }
	    جاری;
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
		واپس FAIL;
	}
	ورنہ اگر (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    اگر (putc(Ctrl_V, fd) < 0)
		واپس FAIL;
	}
	اگر (putc(c, fd) < 0)
	    واپس FAIL;
    }
    واپس OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    خالی
check_map_keycodes(خالی)
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
    estack_ڈالو(ETYPE_اندرونی, (char_u *)"mappings", 0);
    ESTACK_CHECK_تیاری;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    برائے (bp = firstbuf; ; bp = bp->b_اگلا)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	برائے (abbr = 0; abbr <= 1; ++abbr)
	    برائے (hash = 0; hash < 256; ++hash)
	    {
		اگر (abbr)
		{
		    اگر (hash)	    // there is only one abbr list
			توڑو;
		    اگر (bp != تہی)
			mp = bp->b_پہلا_abbr;
		    ورنہ
			mp = پہلا_abbr;
		}
		ورنہ
		{
		    اگر (bp != تہی)
			mp = bp->b_maphash[hash];
		    ورنہ
			mp = maphash[hash];
		}
		برائے ( ; mp != تہی; mp = mp->m_اگلا)
		{
		    برائے (i = 0; i <= 1; ++i)	// do this twice
		    {
			اگر (i == 0)
			    p = mp->m_کلیدیں;	// once for the "from" part
			ورنہ
			    p = mp->m_str;	// and once for the "to" part
			جب (*p)
			{
			    اگر (*p == K_SPECIAL)
			    {
				++p;
				اگر (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (خالی)جوڑو_termcap_اندراج(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	اگر (bp == تہی)
	    توڑو;
    }
    ESTACK_CHECK_NOW;
    estack_نکالو();
}

#اگر defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*کلیدیں,
    صحیح		منوال,
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

    len = (صحیح)STRLEN(کلیدیں);
    برائے (local = 1; local >= 0; --local)
	// loop over all hash lists
	برائے (hash = 0; hash < 256; ++hash)
	{
	    اگر (abbr)
	    {
		اگر (hash > 0)		// there is only one list.
		    توڑو;
		اگر (local)
		    mp = curbuf->b_پہلا_abbr;
		ورنہ
		    mp = پہلا_abbr;
	    }
	    ورنہ اگر (local)
		mp = curbuf->b_maphash[hash];
	    ورنہ
		mp = maphash[hash];
	    برائے ( ; mp != تہی; mp = mp->m_اگلا)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		اگر ((mp->m_منوال & منوال) && (!exact || mp->m_keylen == len))
		{
		    اگر (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    ورنہ
			minlen = len;
		    s = mp->m_کلیدیں;
		    اگر (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			اگر (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    اگر (STRNCMP(s, کلیدیں, minlen) == 0)
		    {
			اگر (mp_ptr != تہی)
			    *mp_ptr = mp;
			اگر (local_ptr != تہی)
			    *local_ptr = local;
			واپس mp->m_str;
		    }
		}
	    }
	}

    واپس تہی;
}

/*
 * "hasmapto()" function
 */
    خالی
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*منوال;
    char_u	buf[NUMBUFLEN];
    صحیح		abbr = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	واپس;

    name = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type == VAR_UNKNOWN)
	منوال = (char_u *)"nvo";
    ورنہ
    {
	منوال = tv_get_string_buf(&argvars[1], buf);
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (صحیح)tv_get_bool(&argvars[2]);
    }

    اگر (map_مقصد_exists(name, منوال, abbr))
	rettv->vval.v_نمبر = TRUE;
    ورنہ
	rettv->vval.v_نمبر = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    ساکن خالی
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	صحیح	    بفر_local,   // false if not buffer local mapping
	صحیح	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_کلیدیں, TRUE, FALSE);
    char_u	    *mapmode = map_منوال_مقصد_chars(mp->m_منوال);

    dict_جوڑو_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_جوڑو_string(dict, "lhsraw", mp->m_کلیدیں);
    اگر (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_جوڑو_string(dict, "lhsrawalt", lhsrawalt);
    dict_جوڑو_string(dict, "rhs", mp->m_orig_str);
    dict_جوڑو_نمبر(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_جوڑو_نمبر(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_جوڑو_نمبر(dict, "expr", mp->m_اظہاریہ ? 1L : 0L);
    dict_جوڑو_نمبر(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_جوڑو_نمبر(dict, "sid", (طویل)mp->m_script_ctx.sc_sid);
    dict_جوڑو_نمبر(dict, "scriptversion",
		    (طویل)mp->m_script_ctx.sc_نسخہ);
    dict_جوڑو_نمبر(dict, "lnum", (طویل)mp->m_script_ctx.sc_lnum);
    dict_جوڑو_نمبر(dict, "buffer", (طویل)بفر_local);
    dict_جوڑو_نمبر(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_جوڑو_string(dict, "mode", mapmode);
    dict_جوڑو_نمبر(dict, "abbr", abbr ? 1L : 0L);
    dict_جوڑو_نمبر(dict, "mode_bits", mp->m_منوال);

    vim_free(mapmode);
}

    ساکن خالی
get_maparg(typval_T *argvars, typval_T *rettv, صحیح exact)
{
    char_u	*کلیدیں;
    char_u	*کلیدیں_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*کلیدیں_buf = تہی;
    char_u	*alt_کلیدیں_buf = تہی;
    صحیح		did_simplify = FALSE;
    char_u	*rhs;
    صحیح		منوال;
    صحیح		abbr = FALSE;
    صحیح		get_dict = FALSE;
    mapblock_T	*mp = تہی;
    صحیح		بفر_local;
    صحیح		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    کلیدیں = tv_get_string(&argvars[0]);
    اگر (*کلیدیں == NUL)
	واپس;

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
    ورنہ
	which = (char_u *)"";
    اگر (which == تہی)
	واپس;

    منوال = get_map_منوال(&which, 0);

    کلیدیں_simplified = replace_termcodes(کلیدیں, &کلیدیں_buf, 0, flags,
								&did_simplify);
    rhs = check_map(کلیدیں_simplified, منوال, exact, FALSE, abbr,
							   &mp, &بفر_local);
    اگر (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(خالی)replace_termcodes(کلیدیں, &alt_کلیدیں_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, تہی);
	rhs = check_map(alt_کلیدیں_buf, منوال, exact, FALSE, abbr, &mp,
								&بفر_local);
    }

    اگر (!get_dict)
    {
	// Return a string.
	اگر (rhs != تہی)
	{
	    اگر (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    ورنہ
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    ورنہ اگر (rettv_dict_alloc(rettv) == OK && rhs != تہی)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? کلیدیں_simplified : تہی,
			  بفر_local, abbr);

    vim_free(کلیدیں_buf);
    vim_free(alt_کلیدیں_buf);
}

/*
 * "maplist()" function
 */
    خالی
f_maplist(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    صحیح		بفر_local;
    char_u	*کلیدیں_buf;
    صحیح		did_simplify;
    صحیح		hash;
    char_u	*lhs;
    ثابت صحیح	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    صحیح		abbr = FALSE;

    اگر (اندر_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	واپس;
    اگر (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    برائے (بفر_local = 0; بفر_local <= 1; ++بفر_local)
    {
	برائے (hash = 0; hash < 256; ++hash)
	{
	    اگر (abbr)
	    {
		اگر (hash > 0)		// there is only one abbr list
		    توڑو;
		اگر (بفر_local)
		    mp = curbuf->b_پہلا_abbr;
		ورنہ
		    mp = پہلا_abbr;
	    }
	    ورنہ اگر (بفر_local)
		mp = curbuf->b_maphash[hash];
	    ورنہ
		mp = maphash[hash];
	    برائے (; mp; mp = mp->m_اگلا)
	    {
		اگر (mp->m_simplified)
		    جاری;
		اگر ((d = dict_alloc()) == تہی)
		    واپس;
		اگر (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    واپس;
		}

		کلیدیں_buf = تہی;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_کلیدیں, TRUE, FALSE);
		(خالی)replace_termcodes(lhs, &کلیدیں_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? کلیدیں_buf : تہی,
				 بفر_local, abbr);
		vim_free(کلیدیں_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    خالی
f_maparg(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		واپس;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    خالی
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	واپس;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    ساکن صحیح
get_map_منوال_string(char_u *منوال_string, صحیح abbr)
{
    char_u	*p = منوال_string;
    صحیح		منوال = 0;
    صحیح		tmode;
    صحیح		modec;
    ثابت صحیح	نقاب_V = منوال_VISUAL | منوال_SELECT;
    ثابت صحیح	نقاب_MAP = منوال_VISUAL | منوال_SELECT | منوال_معمول
							     | منوال_OP_PENDING;
    ثابت صحیح	نقاب_BANG = منوال_داخل_کرو | منوال_CMDLINE;

    اگر (*p == NUL)
	p = (char_u *)" ";	// compatibility
    جب ((modec = *p++))
    {
	انتخاب (modec)
	{
	    حالت 'i': tmode = منوال_داخل_کرو;	توڑو;
	    حالت 'l': tmode = منوال_LANGMAP;	توڑو;
	    حالت 'c': tmode = منوال_CMDLINE;	توڑو;
	    حالت 'n': tmode = منوال_معمول;	توڑو;
	    حالت 'x': tmode = منوال_VISUAL;	توڑو;
	    حالت 's': tmode = منوال_SELECT;	توڑو;
	    حالت 'o': tmode = منوال_OP_PENDING;	توڑو;
	    حالت 't': tmode = منوال_TERMINAL;	توڑو;
	    حالت 'v': tmode = نقاب_V;		توڑو;
	    حالت '!': tmode = نقاب_BANG;	توڑو;
	    حالت ' ': tmode = نقاب_MAP;		توڑو;
	    طےشدہ:
		      واپس 0; // error, unknown mode character
	}
	منوال |= tmode;
    }
    اگر ((abbr && (منوال & ~نقاب_BANG) != 0)
	|| (!abbr && (منوال & (منوال-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((منوال & نقاب_BANG) != 0 && (منوال & ~نقاب_BANG) == 0)
		    || ((منوال & نقاب_MAP) != 0 && (منوال & ~نقاب_MAP) == 0)))))
	واپس 0;

    واپس منوال;
}

/*
 * "mapset()" function
 */
    خالی
f_mapset(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    char_u	*which;
    صحیح		منوال;
    char_u	buf[NUMBUFLEN];
    صحیح		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = تہی;
    صحیح		noremap;
    صحیح		اظہاریہ;
    صحیح		silent;
    صحیح		بفر;
    scid_T	sid;
    صحیح		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_جدول = maphash;
    mapblock_T  **abbr_جدول = &پہلا_abbr;
    صحیح		nowait;
    char_u	*arg;
    صحیح		dict_only;
    mapblock_T	*mp_نتیجہ[2] = {تہی, تہی};

    اگر (check_secure())
	واپس;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    اگر (اندر_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	واپس;

    اگر (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	اگر (which == تہی || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_اندر_mapset_dict_دلیل));
	    واپس;
	}
    }
    ورنہ
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	اگر (which == تہی)
	    واپس;
	is_abbr = (صحیح)tv_get_bool(&argvars[1]);

	اگر (check_for_dict_arg(argvars, 2) == FAIL)
	    واپس;
	d = argvars[2].vval.v_dict;
    }
    منوال = get_map_منوال_string(which, is_abbr);
    اگر (منوال == 0)
    {
	semsg(_(e_illegal_map_منوال_string_str), which);
	واپس;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    اگر (lhs == تہی || lhsraw == تہی || rhs == تہی)
    {
	emsg(_(e_entries_missing_اندر_mapset_dict_دلیل));
	واپس;
    }
    orig_rhs = rhs;

    noremap = dict_get_نمبر(d, "noremap") ? REMAP_NONE: 0;
    اگر (dict_get_نمبر(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    اظہاریہ = dict_get_نمبر(d, "expr") != 0;
    silent = dict_get_نمبر(d, "silent") != 0;
    sid = dict_get_نمبر(d, "sid");
    scriptversion = dict_get_نمبر(d, "scriptversion");
    lnum = dict_get_نمبر(d, "lnum");
    بفر = dict_get_نمبر(d, "buffer");
    nowait = dict_get_نمبر(d, "nowait") != 0;
    // mode from the dict is not used

    اگر (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    ورنہ
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, تہی);

    اگر (بفر)
    {
	map_جدول = curbuf->b_maphash;
	abbr_جدول = &curbuf->b_پہلا_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    اگر (بفر)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	اگر (arg == تہی)
	    واپس;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    ورنہ
    {
	arg = vim_strsave(lhs);
	اگر (arg == تہی)
	    واپس;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, منوال, is_abbr);
    vim_free(arg);

    mp_نتیجہ[0] = map_جوڑو(map_جدول, abbr_جدول, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, منوال, is_abbr, اظہاریہ, sid,
			    scriptversion, lnum, 0);
    اگر (lhsrawalt != تہی)
	mp_نتیجہ[1] = map_جوڑو(map_جدول, abbr_جدول, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, منوال, is_abbr, اظہاریہ, sid,
			    scriptversion, lnum, 1);

    اگر (mp_نتیجہ[0] != تہی && mp_نتیجہ[1] != تہی)
    {
	mp_نتیجہ[0]->m_alt = mp_نتیجہ[1];
	mp_نتیجہ[1]->m_alt = mp_نتیجہ[0];
    }

    vim_free(arg_buf);
}
#اگر_ختم


#اگر defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(منوال_VISUAL | منوال_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
ساخت initmap
{
    char_u	*arg;
    صحیح		منوال;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
ساکن ساخت initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", منوال_معمول},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", منوال_داخل_کرو | منوال_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
ساکن ساخت initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", منوال_معمول | VIS_SEL},
	{(char_u *)"\316w <C-Home>", منوال_داخل_کرو | منوال_CMDLINE},
	{(char_u *)"\316u <C-End>", منوال_معمول | VIS_SEL},
	{(char_u *)"\316u <C-End>", منوال_داخل_کرو | منوال_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", منوال_معمول},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", منوال_داخل_کرو},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  ورنہ
	{(char_u *)"\316\324 P", منوال_معمول},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", منوال_داخل_کرو}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# اگر defined(MACOS_X)
ساکن ساخت initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", منوال_معمول},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", منوال_داخل_کرو | منوال_CMDLINE},
	{(char_u *)"<D-c> \"*y", VIS_SEL},
	{(char_u *)"<D-x> \"*d", VIS_SEL},
	{(char_u *)"<Backspace> \"-d", VIS_SEL},
};
# endif

# undef VIS_SEL
#اگر_ختم

/*
 * Set up default mappings.
 */
    خالی
init_mappings(خالی)
{
#اگر defined(MSWIN) || defined(MACOS_X)
    صحیح		i;

# اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    اگر (!gui.starting)
#  endif
    {
	برائے (i = 0; i < (صحیح)ARRAY_لمبائی(cinitmappings); ++i)
	    جوڑو_map(cinitmappings[i].arg, cinitmappings[i].منوال, FALSE);
    }
# endif
# اگر defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(initmappings); ++i)
	جوڑو_map(initmappings[i].arg, initmappings[i].منوال, FALSE);
# endif
#اگر_ختم
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    خالی
جوڑو_map(char_u *map, صحیح منوال, صحیح nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = خالی_خانے_option;	// Allow <> notation
    s = vim_strsave(map);
    اگر (s != تہی)
    {
	(خالی)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, منوال, FALSE);
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
قسمتعریف ساخت
{
    صحیح	    from;
    صحیح     مقصد;
} langmap_اندراج_T;

ساکن garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    ساکن خالی
langmap_سیٹ_اندراج(صحیح from, صحیح مقصد)
{
    langmap_اندراج_T *entries = (langmap_اندراج_T *)(langmap_mapga.ga_ڈیٹا);
    صحیح		    a = 0;
    صحیح		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    جب (a != b)
    {
	صحیح i = (a + b) / 2;
	صحیح d = entries[i].from - from;

	اگر (d == 0)
	{
	    entries[i].مقصد = مقصد;
	    واپس;
	}
	اگر (d < 0)
	    a = i + 1;
	ورنہ
	    b = i;
    }

    اگر (ga_grow(&langmap_mapga, 1) == FAIL)
	واپس;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_اندراج_T *)(langmap_mapga.ga_ڈیٹا) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * حجم(langmap_اندراج_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].مقصد = مقصد;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    صحیح
langmap_adjust_mb(صحیح c)
{
    langmap_اندراج_T *entries = (langmap_اندراج_T *)(langmap_mapga.ga_ڈیٹا);
    صحیح a = 0;
    صحیح b = langmap_mapga.ga_len;

    جب (a != b)
    {
	صحیح i = (a + b) / 2;
	صحیح d = entries[i].from - c;

	اگر (d == 0)
	    واپس entries[i].مقصد;  // found matching entry
	اگر (d < 0)
	    a = i + 1;
	ورنہ
	    b = i;
    }
    واپس c;  // no entry found, return "c" unmodified
}

    خالی
langmap_init(خالی)
{
    صحیح i;

    برائے (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, حجم(langmap_اندراج_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    حرف *
did_سیٹ_langmap(optset_T *دلائل غیر_مستعمل)
{
    char_u  *p;
    char_u  *p2;
    صحیح	    from, مقصد;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    برائے (p = p_langmap; p[0] != NUL; )
    {
	برائے (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    اگر (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	اگر (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	ورنہ
	    p2 = تہی;	    // aAbBcCdD form, p2 is NULL
	جب (p[0])
	{
	    اگر (p[0] == ',')
	    {
		++p;
		توڑو;
	    }
	    اگر (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    مقصد = NUL;
	    اگر (p2 == تہی)
	    {
		MB_PTR_ADV(p);
		اگر (p[0] != ',')
		{
		    اگر (p[0] == '\\')
			++p;
		    مقصد = (*mb_ptr2char)(p);
		}
	    }
	    ورنہ
	    {
		اگر (p2[0] != ',')
		{
		    اگر (p2[0] == '\\')
			++p2;
		    مقصد = (*mb_ptr2char)(p2);
		}
	    }
	    اگر (مقصد == NUL)
	    {
		sprintf(دلائل->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		واپس دلائل->os_errbuf;
	    }

	    اگر (from >= 256)
		langmap_سیٹ_اندراج(from, مقصد);
	    ورنہ
		langmap_mapchar[from & 255] = مقصد;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    اگر (p2 != تہی)
	    {
		MB_PTR_ADV(p2);
		اگر (*p == ';')
		{
		    p = p2;
		    اگر (p[0] != NUL)
		    {
			اگر (p[0] != ',')
			{
			    vim_snprintf(دلائل->os_errbuf, دلائل->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    واپس دلائل->os_errbuf;
			}
			++p;
		    }
		    توڑو;
		}
	    }
	}
    }

    واپس تہی;
}
#اگر_ختم

    ساکن خالی
do_exmap(exarg_T *eap, صحیح isabbrev)
{
    صحیح	    منوال;
    char_u  *cmdp;

    cmdp = eap->cmd;
    منوال = get_map_منوال(&cmdp, eap->forceit || isabbrev);

    انتخاب (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, منوال, isabbrev))
    {
	حالت 1: emsg(_(e_invalid_دلیل));
		توڑو;
	حالت 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		توڑو;
    }
}

/*
 * ":abbreviate" and friends.
 */
    خالی
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    خالی
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    اگر (secure)
    {
	secure = 2;
	پیغام_outtrans(eap->cmd);
	پیغام_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    خالی
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    خالی
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    خالی
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
