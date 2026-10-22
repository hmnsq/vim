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

#ଅନ୍ତର୍ଭାଵ "vim.h"

/*
 * List used for abbreviations.
 */
ସ୍ଥିର mapblock_T	*first_abbr = ଖାଲି; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
ସ୍ଥିର mapblock_T	*(maphash[256]);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		map_locked = 0;

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
get_maphash_list(ପୂର୍ଣ୍ଣସଂଖ୍ୟା state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(ପୂର୍ଣ୍ଣସଂଖ୍ୟା state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ curbuf->b_maphash[MAP_HASH(state, c)];
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_maphash_valid(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
validate_maphash(ଶୂନ୍ୟ)
{
    ଯଦି (maphash_valid)
	ଫେରନ୍ତୁ;

    CLEAR_FIELD(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    ସ୍ଥିର ଶୂନ୍ୟ
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_ଚାବିଗୁଡ଼ିକ);
    ଯଦି (mp->m_alt != ଖାଲି)
	mp->m_alt->m_alt = ଖାଲି;
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
    ସ୍ଥିର char_u *
map_mode_to_chars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    ଯଦି ((mode & (MODE_ଭର୍ତ୍ତି | MODE_CMDLINE)) == (MODE_ଭର୍ତ୍ତି | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    ଅନ୍ୟଥା ଯଦି (mode & MODE_ଭର୍ତ୍ତି)
	ga_append(&mapmode, 'i');			// :imap
    ଅନ୍ୟଥା ଯଦି (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    ଅନ୍ୟଥା ଯଦି (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    ଅନ୍ୟଥା ଯଦି ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    ଅନ୍ୟଥା
    {
	ଯଦି (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	ଯଦି (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	ଯଦି (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	ଯଦି ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	ଅନ୍ୟଥା
	{
	    ଯଦି (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    ଯଦି (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    ଫେରନ୍ତୁ (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
showmap(
    mapblock_T	*mp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		local)	    // TRUE for buffer-local map
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 1;
    char_u	*mapchars;

    ଯଦି (message_filtered(mp->m_ଚାବିଗୁଡ଼ିକ) && message_filtered(mp->m_str))
	ଫେରନ୍ତୁ;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    ଯଦି (ସନ୍ଦେଶ_didout || ସନ୍ଦେଶ_silent != 0)
    {
	ସନ୍ଦେଶ_putchar('\n');
	ଯଦି (got_int)	    // 'q' typed at MORE prompt
	    ଯାଅ theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    ଯଦି (mapchars != ଖାଲି)
    {
	ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)mapchars);
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(mapchars);
	vim_free(mapchars);
    }

    ଯେତେବେଳେ (++len <= 3)
	ସନ୍ଦେଶ_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = ସନ୍ଦେଶ_outtrans_special(mp->m_ଚାବିଗୁଡ଼ିକ, TRUE, 0);
    କର
    {
	ସନ୍ଦେଶ_putchar(' ');		// pad with blanks
	++len;
    } ଯେତେବେଳେ (len < 12);

    ଯଦି (mp->m_noremap == REMAP_NONE)
	ସନ୍ଦେଶ_puts_attr("*", HL_ATTR(HLF_8));
    ଅନ୍ୟଥା ଯଦି (mp->m_noremap == REMAP_SCRIPT)
	ସନ୍ଦେଶ_puts_attr("&", HL_ATTR(HLF_8));
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ_putchar(' ');

    ଯଦି (local)
	ସନ୍ଦେଶ_putchar('@');
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    ଯଦି (*mp->m_str == NUL)
	ସନ୍ଦେଶ_puts_attr("<Nop>", HL_ATTR(HLF_8));
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    ଯଦି (p_verbose > 0)
	last_set_ସନ୍ଦେଶ(mp->m_script_ctx);
#endif
    ସନ୍ଦେଶ_clr_eos();
    out_flush();			// show one line at a time

theend:
    --map_locked;
}

    ସ୍ଥିର mapblock_T *
map_add(
	mapblock_T  **map_table,
	mapblock_T  **abbr_table,
	char_u	    *ଚାବିଗୁଡ଼ିକ,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    noremap,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nowait,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    silent,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mode,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_abbr,
#ifdef FEAT_EVAL
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ପଦସମଷ୍ଟି,
	scid_T	    sid,	    // 0 to use current_sctx
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    scriptversion,
	linenr_T    lnum,
#endif
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    ଯଦି (mp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    ଯଦି (*ଚାବିଗୁଡ଼ିକ == Ctrl_C)
    {
	ଯଦି (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	ଅନ୍ୟଥା
	    mapped_ctrl_c |= mode;
    }

    mp->m_ଚାବିଗୁଡ଼ିକ = vim_strsave(ଚାବିଗୁଡ଼ିକ);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    ଯଦି (mp->m_ଚାବିଗୁଡ଼ିକ == ଖାଲି || mp->m_str == ଖାଲି)
    {
	vim_free(mp->m_ଚାବିଗୁଡ଼ିକ);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    mp->m_keylen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(mp->m_ଚାବିଗୁଡ଼ିକ);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_ପଦସମଷ୍ଟି = ପଦସମଷ୍ଟି;
    ଯଦି (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    ଅନ୍ୟଥା
    {
	mp->m_script_ctx = current_sctx;
	ଯଦି (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    ଯଦି (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = MAP_HASH(mp->m_mode, mp->m_ଚାବିଗୁଡ଼ିକ[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    ଫେରନ୍ତୁ mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_mappings(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	keyround,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	abbrev,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	haskey,
	char_u	*ଚାବିଗୁଡ଼ିକ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଚାବିଗୁଡ଼ିକ_len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	mode,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    ଯଦି (p_verbose > 0 && keyround == 1)
    {
	ଯଦି (seen_Modify_Other_ଚାବିଗୁଡ଼ିକ)
	    ସନ୍ଦେଶ_puts(_("Seen modifyOtherKeys: true\n"));

	ଯଦି (modify_otherkeys_state != MOKS_INITIAL)
	{
	    ଅକ୍ଷର *name = _("Unknown");
	    ବାଛ (modify_otherkeys_state)
	    {
		ସ୍ଥିତି MOKS_INITIAL: ଭାଙ୍ଗ;
		ସ୍ଥିତି MOKS_OFF: name = _("Off"); ଭାଙ୍ଗ;
		ସ୍ଥିତି MOKS_ENABLED: name = _("On"); ଭାଙ୍ଗ;
		ସ୍ଥିତି MOKS_DISABLED: name = _("Disabled"); ଭାଙ୍ଗ;
		ସ୍ଥିତି MOKS_AFTER_T_TE: name = _("Cleared"); ଭାଙ୍ଗ;
	    }

	    ଅକ୍ଷର buf[200];
	    vim_snprintf(buf, ଆକାର(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    ସନ୍ଦେଶ_puts(buf);
	}

	ଯଦି (kitty_protocol_state != KKPS_INITIAL)
	{
	    ଅକ୍ଷର *name = _("Unknown");
	    ବାଛ (kitty_protocol_state)
	    {
		ସ୍ଥିତି KKPS_INITIAL: ଭାଙ୍ଗ;
		ସ୍ଥିତି KKPS_OFF: name = _("Off"); ଭାଙ୍ଗ;
		ସ୍ଥିତି KKPS_ENABLED: name = _("On"); ଭାଙ୍ଗ;
		ସ୍ଥିତି KKPS_DISABLED: name = _("Disabled"); ଭାଙ୍ଗ;
		ସ୍ଥିତି KKPS_AFTER_T_TE: name = _("Cleared"); ଭାଙ୍ଗ;
	    }

	    ଅକ୍ଷର buf[200];
	    vim_snprintf(buf, ଆକାର(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    ସନ୍ଦେଶ_puts(buf);
	}
    }

    // need to loop over all global hash lists
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	ଯଦି (abbrev)
	{
	    ଯଦି (hash != 0)	// there is only one abbreviation list
		ଭାଙ୍ଗ;
	    mp = curbuf->b_first_abbr;
	}
	ଅନ୍ୟଥା
	    mp = curbuf->b_maphash[hash];
	ପାଇଁ ( ; mp != ଖାଲି && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    ଯଦି (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		ଯଦି (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		ଅନ୍ୟଥା
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = mp->m_keylen;
		    ଯଦି (STRNCMP(mp->m_ଚାବିଗୁଡ଼ିକ, ଚାବିଗୁଡ଼ିକ,
				   (size_t)(n < ଚାବିଗୁଡ଼ିକ_len ? n : ଚାବିଗୁଡ଼ିକ_len)) == 0)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_map(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maptype,
    char_u	*arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abbrev)		// not a mapping but an abbreviation
{
    char_u	*ଚାବିଗୁଡ଼ିକ;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {ଖାଲି, ଖାଲି};
    char_u	*rhs;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;	// init for GCC
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hasarg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		haskey;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_print;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keyround;
    char_u	*ଚାବିଗୁଡ଼ିକ_buf = ଖାଲି;
    char_u	*alt_ଚାବିଗୁଡ଼ିକ_buf = ଖାଲି;
    char_u	*arg_buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unique = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nowait = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		special = FALSE;
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ପଦସମଷ୍ଟି = FALSE;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_simplify = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unmap_lhs_only = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noremap;
    char_u      *orig_rhs;

    ଚାବିଗୁଡ଼ିକ = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    ଯଦି (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    ଯଦି (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    ଅନ୍ୟଥା
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    ପାଇଁ (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	ଯଦି (STRNCMP(ଚାବିଗୁଡ଼ିକ, "<buffer>", 8) == 0)
	{
	    ଚାବିଗୁଡ଼ିକ = skipwhite(ଚାବିଗୁଡ଼ିକ + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    ଜାରି;
	}

	// Check for "<nowait>": don't wait for more characters.
	ଯଦି (STRNCMP(ଚାବିଗୁଡ଼ିକ, "<nowait>", 8) == 0)
	{
	    ଚାବିଗୁଡ଼ିକ = skipwhite(ଚାବିଗୁଡ଼ିକ + 8);
	    nowait = TRUE;
	    ଜାରି;
	}

	// Check for "<silent>": don't echo commands.
	ଯଦି (STRNCMP(ଚାବିଗୁଡ଼ିକ, "<silent>", 8) == 0)
	{
	    ଚାବିଗୁଡ଼ିକ = skipwhite(ଚାବିଗୁଡ଼ିକ + 8);
	    silent = TRUE;
	    ଜାରି;
	}

	// Check for "<special>": accept special keys in <>
	ଯଦି (STRNCMP(ଚାବିଗୁଡ଼ିକ, "<special>", 9) == 0)
	{
	    ଚାବିଗୁଡ଼ିକ = skipwhite(ଚାବିଗୁଡ଼ିକ + 9);
	    special = TRUE;
	    ଜାରି;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	ଯଦି (STRNCMP(ଚାବିଗୁଡ଼ିକ, "<script>", 8) == 0)
	{
	    ଚାବିଗୁଡ଼ିକ = skipwhite(ଚାବିଗୁଡ଼ିକ + 8);
	    noremap = REMAP_SCRIPT;
	    ଜାରି;
	}

	// Check for "<expr>": {rhs} is an expression.
	ଯଦି (STRNCMP(ଚାବିଗୁଡ଼ିକ, "<expr>", 6) == 0)
	{
	    ଚାବିଗୁଡ଼ିକ = skipwhite(ଚାବିଗୁଡ଼ିକ + 6);
	    ପଦସମଷ୍ଟି = TRUE;
	    ଜାରି;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	ଯଦି (STRNCMP(ଚାବିଗୁଡ଼ିକ, "<unique>", 8) == 0)
	{
	    ଚାବିଗୁଡ଼ିକ = skipwhite(ଚାବିଗୁଡ଼ିକ + 8);
	    unique = TRUE;
	    ଜାରି;
	}
	ଭାଙ୍ଗ;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = ଚାବିଗୁଡ଼ିକ;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ଖାଲି);
    ଯେତେବେଳେ (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	ଯଦି ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    ଯଦି (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*ଚାବିଗୁଡ଼ିକ != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    ଯଦି (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	ଯାଅ theend;
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
    ଯଦି (haskey)
    {
	char_u	*new_ଚାବିଗୁଡ଼ିକ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	ଯଦି (special)
	    flags |= REPTERM_SPECIAL;
	new_ଚାବିଗୁଡ଼ିକ = replace_termcodes(ଚାବିଗୁଡ଼ିକ, &ଚାବିଗୁଡ଼ିକ_buf, 0, flags, &did_simplify);
	ଯଦି (did_simplify)
	    (ଶୂନ୍ୟ)replace_termcodes(ଚାବିଗୁଡ଼ିକ, &alt_ଚାବିଗୁଡ଼ିକ_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, ଖାଲି);
	ଚାବିଗୁଡ଼ିକ = new_ଚାବିଗୁଡ଼ିକ;
    }
    orig_rhs = rhs;
    ଯଦି (hasarg)
    {
	ଯଦି (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	ଅନ୍ୟଥା
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ଖାଲି);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    ପାଇଁ (keyround = 1; keyround <= 2; ++keyround)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_it = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_local = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	keyround1_simplified = keyround == 1 && did_simplify;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	round;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଅଙ୍କ_rounds;

	ଯଦି (keyround == 2)
	{
	    ଯଦି (alt_ଚାବିଗୁଡ଼ିକ_buf == ଖାଲି)
		ଭାଙ୍ଗ;
	    ଚାବିଗୁଡ଼ିକ = alt_ଚାବିଗୁଡ଼ିକ_buf;
	}
	ଅନ୍ୟଥା ଯଦି (alt_ଚାବିଗୁଡ଼ିକ_buf != ଖାଲି && do_print)
	    // when printing always use the not-simplified map
	    ଚାବିଗୁଡ଼ିକ = alt_ଚାବିଗୁଡ଼ିକ_buf;

	// check arguments and translate function keys
	ଯଦି (haskey)
	{
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ଚାବିଗୁଡ଼ିକ);
	    ଯଦି (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		ଯାଅ theend;
	    }

	    ଯଦି (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		ଯଦି (has_mbyte)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first, last;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		same = -1;
		    char_u	ଚାବିଗୁଡ଼ିକ_unescaped[MAXMAPLEN + 1];
		    size_t	ଚାବିଗୁଡ଼ିକ_unescaped_len;

		    mch_memmove(ଚାବିଗୁଡ଼ିକ_unescaped, ଚାବିଗୁଡ଼ିକ, (size_t)(len + 1));
		    ଚାବିଗୁଡ଼ିକ_unescaped_len = vim_unescape_csi(ଚାବିଗୁଡ଼ିକ_unescaped);
		    p = ଚାବିଗୁଡ଼ିକ_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    ଯେତେବେଳେ (p < ଚାବିଗୁଡ଼ିକ_unescaped + ଚାବିଗୁଡ଼ିକ_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			ଯଦି (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    ଯଦି (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			ଯାଅ theend;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (vim_iswordc(ଚାବିଗୁଡ଼ିକ[len - 1]))
		    // ends in keyword char
		    ପାଇଁ (n = 0; n < len - 2; ++n)
			ଯଦି (vim_iswordc(ଚାବିଗୁଡ଼ିକ[n]) != vim_iswordc(ଚାବିଗୁଡ଼ିକ[len - 2]))
			{
			    retval = 1;
			    ଯାଅ theend;
			}
		// An abbreviation cannot contain white space.
		ପାଇଁ (n = 0; n < len; ++n)
		    ଯଦି (VIM_ISWHITE(ଚାବିଗୁଡ଼ିକ[n]))
		    {
			retval = 1;
			ଯାଅ theend;
		    }
	    }
	}

	ଯଦି (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	ଯଦି (do_print)
	    ସନ୍ଦେଶ_start();

	// Check if a new local mapping wasn't already defined globally.
	ଯଦି (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ଯଦି (abbrev)
		{
		    ଯଦି (hash != 0)	// there is only one abbreviation list
			ଭାଙ୍ଗ;
		    mp = first_abbr;
		}
		ଅନ୍ୟଥା
		    mp = maphash[hash];
		ପାଇଁ ( ; mp != ଖାଲି && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    ଯଦି ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_ଚାବିଗୁଡ଼ିକ, ଚାବିଗୁଡ଼ିକ, (size_t)len) == 0)
		    {
			ଯଦି (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_ଚାବିଗୁଡ଼ିକ);
			ଅନ୍ୟଥା
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_ଚାବିଗୁଡ଼ିକ);
			retval = 5;
			ଯାଅ theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	ଯଦି (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, ଚାବିଗୁଡ଼ିକ, len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	ଅଙ୍କ_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	ପାଇଁ (round = 0; round < ଅଙ୍କ_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ଯଦି (abbrev)
		{
		    ଯଦି (hash > 0)	// there is only one abbreviation list
			ଭାଙ୍ଗ;
		    mpp = abbr_table;
		}
		ଅନ୍ୟଥା
		    mpp = &(map_table[hash]);
		ପାଇଁ (mp = *mpp; mp != ଖାଲି && !got_int; mp = *mpp)
		{

		    ଯଦି ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			ଜାରି;
		    }
		    ଯଦି (!haskey)	// show all entries
		    {
			ଯଦି (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    ଅନ୍ୟଥା	// do we have a match?
		    {
			ଯଦି (round)	// second round: Try unmap "rhs" string
			{
			    n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			ଅନ୍ୟଥା
			{
			    n = mp->m_keylen;
			    p = mp->m_ଚାବିଗୁଡ଼ିକ;
			}
			ଯଦି (STRNCMP(p, ଚାବିଗୁଡ଼ିକ, (size_t)(n < len ? n : len)) == 0)
			{
			    ଯଦି (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				ଯଦି (n != len && (!abbrev || round || n > len
					       || *skipwhite(ଚାବିଗୁଡ଼ିକ + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    ଜାରି;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				ଯଦି (keyround1_simplified && !mp->m_simplified)
				    ଭାଙ୍ଗ;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    ଅନ୍ୟଥା ଯଦି (!hasarg)	// show matching entry
			    {
				ଯଦି (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    ଅନ୍ୟଥା ଯଦି (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				ଜାରି;
			    }
			    ଅନ୍ୟଥା ଯଦି (unique)
			    {
				ଯଦି (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				ଅନ୍ୟଥା
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				ଯାଅ theend;
			    }
			    ଅନ୍ୟଥା
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				ଯଦି (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    ଯଦି (newstr == ଖାଲି)
				    {
					retval = 4;		// no mem
					ଯାଅ theend;
				    }
				    ଯଦି (mp->m_alt != ଖାଲି)
					mp->m_alt = mp->m_alt->m_alt = ଖାଲି;
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
				    mp->m_ପଦସମଷ୍ଟି = ପଦସମଷ୍ଟି;
				    mp->m_script_ctx = current_sctx;
				    ଯଦି (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    ଯଦି (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				ଜାରି;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_hash = MAP_HASH(mp->m_mode, mp->m_ଚାବିଗୁଡ଼ିକ[0]);
			    ଯଦି (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_next;
				mp->m_next = map_table[new_hash];
				map_table[new_hash] = mp;

				ଜାରି;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_next);
		}
	    }
	}

	ଯଦି (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    ଯଦି (!did_it)
	    {
		ଯଦି (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    ଅନ୍ୟଥା ଯଦି (*ଚାବିଗୁଡ଼ିକ == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		ଯଦି (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		ଅନ୍ୟଥା
		    mapped_ctrl_c &= ~mode;
	    }
	    ଜାରି;
	}

	ଯଦି (!haskey || !hasarg)
	{
	    // print entries
	    ଯଦି (!did_it && !did_local)
	    {
		ଯଦି (abbrev)
		    ସନ୍ଦେଶ(_("No abbreviation found"));
		ଅନ୍ୟଥା
		    ସନ୍ଦେଶ(_("No mapping found"));
	    }
	    ଯାଅ theend;    // listing finished
	}

	ଯଦି (did_it)
	    ଜାରି;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, ଚାବିଗୁଡ଼ିକ,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    ପଦସମଷ୍ଟି, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	ଯଦି (mp_result[keyround - 1] == ଖାଲି)
	{
	    retval = 4;	    // no mem
	    ଯାଅ theend;
	}
    }

    ଯଦି (mp_result[0] != ଖାଲି && mp_result[1] != ଖାଲି)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(ଚାବିଗୁଡ଼ିକ_buf);
    vim_free(alt_ଚାବିଗୁଡ଼ିକ_buf);
    vim_free(arg_buf);
    ଫେରନ୍ତୁ retval;
}

/*
 * Get the mapping mode from the command name.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_map_mode(char_u **cmdp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modec;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode;

    p = *cmdp;
    modec = *p++;
    ଯଦି (modec == 'i')
	mode = MODE_ଭର୍ତ୍ତି;				// :imap
    ଅନ୍ୟଥା ଯଦି (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    ଅନ୍ୟଥା ଯଦି (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    ଅନ୍ୟଥା ଯଦି (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    ଅନ୍ୟଥା ଯଦି (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    ଅନ୍ୟଥା ଯଦି (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    ଅନ୍ୟଥା ଯଦି (modec == 's')
	mode = MODE_SELECT;			// :smap
    ଅନ୍ୟଥା ଯଦି (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    ଅନ୍ୟଥା ଯଦି (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    ଅନ୍ୟଥା
    {
	--p;
	ଯଦି (forceit)
	    mode = MODE_ଭର୍ତ୍ତି | MODE_CMDLINE;		// :map !
	ଅନ୍ୟଥା
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    ଫେରନ୍ତୁ mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abbr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    ଯଦି (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	ଫେରନ୍ତୁ;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_map_locked(ଶୂନ୍ୟ)
{
    ଯଦି (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    ଶୂନ୍ୟ
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode,		// mode in which to delete
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		local,		// TRUE for buffer-local mappings
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hash;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_hash;

    ଯଦି (is_map_locked())
	ଫେରନ୍ତୁ;

    validate_maphash();

    ପାଇଁ (hash = 0; hash < 256; ++hash)
    {
	ଯଦି (abbr)
	{
	    ଯଦି (hash > 0)	// there is only one abbrlist
		ଭାଙ୍ଗ;
	    ଯଦି (local)
		mpp = &buf->b_first_abbr;
	    ଅନ୍ୟଥା
		mpp = &first_abbr;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (local)
		mpp = &buf->b_maphash[hash];
	    ଅନ୍ୟଥା
		mpp = &maphash[hash];
	}
	ଯେତେବେଳେ (*mpp != ଖାଲି)
	{
	    mp = *mpp;
	    ଯଦି (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		ଯଦି (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    ଜାରି;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_ଚାବିଗୁଡ଼ିକ[0]);
		ଯଦି (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    ଯଦି (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    ଅନ୍ୟଥା
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    ଜାରି;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

#ଯଦି defined(FEAT_EVAL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mode_str2flags(char_u *modechars)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode = 0;

    ଯଦି (vim_strchr(modechars, 'n') != ଖାଲି)
	mode |= MODE_NORMAL;
    ଯଦି (vim_strchr(modechars, 'v') != ଖାଲି)
	mode |= MODE_VISUAL | MODE_SELECT;
    ଯଦି (vim_strchr(modechars, 'x') != ଖାଲି)
	mode |= MODE_VISUAL;
    ଯଦି (vim_strchr(modechars, 's') != ଖାଲି)
	mode |= MODE_SELECT;
    ଯଦି (vim_strchr(modechars, 'o') != ଖାଲି)
	mode |= MODE_OP_PENDING;
    ଯଦି (vim_strchr(modechars, 'i') != ଖାଲି)
	mode |= MODE_ଭର୍ତ୍ତି;
    ଯଦି (vim_strchr(modechars, 'l') != ଖାଲି)
	mode |= MODE_LANGMAP;
    ଯଦି (vim_strchr(modechars, 'c') != ଖାଲି)
	mode |= MODE_CMDLINE;

    ଫେରନ୍ତୁ mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
map_to_exists(char_u *str, char_u *modechars, ପୂର୍ଣ୍ଣସଂଖ୍ୟା abbr)
{
    char_u	*rhs;
    char_u	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, ଖାଲି);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    ଫେରନ୍ତୁ retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
map_to_exists_mode(char_u *rhs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା abbr)
{
    mapblock_T	*mp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hash;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    ପାଇଁ (;;)
    {
	ପାଇଁ (hash = 0; hash < 256; ++hash)
	{
	    ଯଦି (abbr)
	    {
		ଯଦି (hash > 0)		// there is only one abbr list
		    ଭାଙ୍ଗ;
		ଯଦି (exp_buffer)
		    mp = curbuf->b_first_abbr;
		ଅନ୍ୟଥା
		    mp = first_abbr;
	    }
	    ଅନ୍ୟଥା ଯଦି (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    ଅନ୍ୟଥା
		mp = maphash[hash];
	    ପାଇଁ (; mp; mp = mp->m_next)
	    {
		ଯଦି ((mp->m_mode & mode)
			&& strstr((ଅକ୍ଷର *)mp->m_str, (ଅକ୍ଷର *)rhs) != ଖାଲି)
		    ଫେରନ୍ତୁ TRUE;
	    }
	}
	ଯଦି (exp_buffer)
	    ଭାଙ୍ଗ;
	exp_buffer = TRUE;
    }

    ଫେରନ୍ତୁ FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	expand_mapmodes = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	expand_isabbrev = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	expand_buffer = FALSE;

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
    ସ୍ଥିର char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cpo_bslash;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != ଖାଲି);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != ଖାଲି);

    ପାଇଁ (; *str; ++str)
    {
	c = *str;
	ଯଦି (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    ଯଦି (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    ଯଦି (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		ଯଦି (cpo_special)
		{
		    ga_clear(&ga);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		c = TO_SPECIAL(str[1], str[2]);
		ଯଦି (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    ଯଦି (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ଯଦି (cpo_special)
		{
		    ga_clear(&ga);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		ga_concat(&ga, get_special_ଚାବି_name(c, modifiers));
		ଜାରି; // for (str)
	    }
	}
	ଯଦି (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	ଯଦି (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    ଫେରନ୍ତୁ (char_u *)(ga.ga_data);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,	// TRUE if '!' given
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		isabbrev,	// TRUE if abbreviation
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    ଯଦି (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    ଅନ୍ୟଥା
    {
	ଯଦି (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	ଅନ୍ୟଥା
	{
	    expand_mapmodes = MODE_ଭର୍ତ୍ତି | MODE_CMDLINE;
	    ଯଦି (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	ପାଇଁ (;;)
	{
	    ଯଦି (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		ଜାରି;
	    }
	    ଯଦି (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ଜାରି;
	    }
	    ଯଦି (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ଜାରି;
	    }
	    ଯଦି (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ଜାରି;
	    }
	    ଯଦି (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		ଜାରି;
	    }
#ifdef FEAT_EVAL
	    ଯଦି (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ଜାରି;
	    }
	    ଯଦି (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		ଜାରି;
	    }
#endif
	    ଭାଙ୍ଗ;
	}
	xp->xp_pattern = arg;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hash;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fuzzy;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *ଅଙ୍କ_Matches = 0;		    // return values in case of FAIL
    *matches = ଖାଲି;

    ଯଦି (!fuzzy)
	ga_init2(&ga, ଆକାର(ଅକ୍ଷର *), 3);
    ଅନ୍ୟଥା
	ga_init2(&ga, ଆକାର(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    ପାଇଁ (i = 0; i < 7; ++i)
    {
	ଯଦି (i == 0)
	    p = (char_u *)"<silent>";
	ଅନ୍ୟଥା ଯଦି (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	ଅନ୍ୟଥା ଯଦି (i == 2)
	    p = (char_u *)"<script>";
	ଅନ୍ୟଥା ଯଦି (i == 3)
	    p = (char_u *)"<expr>";
#endif
	ଅନ୍ୟଥା ଯଦି (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	ଅନ୍ୟଥା ଯଦି (i == 5)
	    p = (char_u *)"<nowait>";
	ଅନ୍ୟଥା ଯଦି (i == 6)
	    p = (char_u *)"<special>";
	ଅନ୍ୟଥା
	    ଜାରି;

	ଯଦି (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	ଅନ୍ୟଥା
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	ଯଦି (!match)
	    ଜାରି;

	ଯଦି (ga_grow(&ga, 1) == FAIL)
	    ଭାଙ୍ଗ;

	ଯଦି (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	ଅନ୍ୟଥା
	    ((char_u **)ga.ga_data)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    ପାଇଁ (hash = 0; hash < 256; ++hash)
    {
	ଯଦି (expand_isabbrev)
	{
	    ଯଦି (hash > 0)	// only one abbrev list
		ଭାଙ୍ଗ; // for (hash)
	    mp = first_abbr;
	}
	ଅନ୍ୟଥା ଯଦି (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	ଅନ୍ୟଥା
	    mp = maphash[hash];
	ପାଇଁ (; mp; mp = mp->m_next)
	{
	    ଯଦି (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		ଜାରି;

	    p = translate_mapping(mp->m_ଚାବିଗୁଡ଼ିକ);
	    ଯଦି (p == ଖାଲି)
		ଜାରି;

	    ଯଦି (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    ଅନ୍ୟଥା
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    ଯଦି (!match)
	    {
		vim_free(p);
		ଜାରି;
	    }

	    ଯଦି (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ଅନ୍ୟଥା
		((char_u **)ga.ga_data)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    ଯଦି (ga.ga_len == 0)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (!fuzzy)
    {
	*matches = ga.ga_data;
	*ଅଙ୍କ_Matches = ga.ga_len;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	*ଅଙ୍କ_Matches = ga.ga_len;
    }

    ଗଣନା_2 = *ଅଙ୍କ_Matches;
    ଯଦି (ଗଣନା_2 > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	ଯଦି (!fuzzy)
	    sort_strings(*matches, ଗଣନା_2);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + ଗଣନା_2;

	ଯେତେବେଳେ (ptr2 < ptr3)
	{
	    ଯଦି (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    ଅନ୍ୟଥା
	    {
		vim_free(*ptr2++);
		ଗଣନା_2--;
	    }
	}
    }

    *ଅଙ୍କ_Matches = ଗଣନା_2;
    ଫେରନ୍ତୁ (ଗଣନା_2 == 0 ? FAIL : OK);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_abbr(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c,
    char_u	*ptr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mincol)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scol;		// starting column of the abbr.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		clen = 0;	// length in characters
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_id = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim_abbr;

    ଯଦି (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	ଫେରନ୍ତୁ FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    ଯଦି (noremap_ଚାବିଗୁଡ଼ିକ() && c != Ctrl_RSB)
	ଫେରନ୍ତୁ FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    ଯଦି (col == 0)				// cannot be an abbr.
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	ଯଦି (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	ଅନ୍ୟଥା
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ଯଦି (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	ଯେତେବେଳେ (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    ଯଦି (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		ଭାଙ୍ଗ;
	    }
	    ++clen;
	}
	scol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ptr);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	ଅନ୍ୟଥା
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ଯଦି (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	ପାଇଁ (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    ଯଦି (scol < mincol)
	scol = mincol;
    ଯଦି (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	ଯଦି (mp == ଖାଲି)
	{
	    mp = mp2;
	    mp2 = ଖାଲି;
	}
	ପାଇଁ ( ; mp; mp->m_next == ଖାଲି
				  ? (mp = mp2, mp2 = ଖାଲି) : (mp = mp->m_next))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qlen = mp->m_keylen;
	    char_u	*q = mp->m_ଚାବିଗୁଡ଼ିକ;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match;

	    ଯଦି (vim_strbyte(mp->m_ଚାବିଗୁଡ଼ିକ, K_SPECIAL) != ଖାଲି)
	    {
		char_u *qe = vim_strsave(mp->m_ଚାବିଗୁଡ଼ିକ);

		// might have CSI escaped mp->m_keys
		ଯଦି (qe != ଖାଲି)
		{
		    q = qe;
		    qlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    ଯଦି (q != mp->m_ଚାବିଗୁଡ଼ିକ)
		vim_free(q);
	    ଯଦି (match)
		ଭାଙ୍ଗ;
	}
	ଯଦି (mp != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	noremap;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା silent;
#ifdef FEAT_EVAL
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ପଦସମଷ୍ଟି;
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
	    ଯଦି (c != Ctrl_RSB)
	    {
					// special key code, split up
		ଯଦି (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    ଯଦି (has_mbyte)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			ଯଦି (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			ଯଦି (escaped != ଖାଲି)
			{
			    newlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    ଅନ୍ୟଥା
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(ଶୂନ୍ୟ)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ifdef FEAT_EVAL
	    ପଦସମଷ୍ଟି = mp->m_ପଦସମଷ୍ଟି;

	    ଯଦି (ପଦସମଷ୍ଟି)
		s = eval_map_ପଦସମଷ୍ଟି(mp, c);
	    ଅନ୍ୟଥା
#endif
		s = mp->m_str;
	    ଯଦି (s != ଖାଲି)
	    {
					// insert the to string
		(ଶୂନ୍ୟ)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		ଯଦି (ପଦସମଷ୍ଟି)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    ଯଦି (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    ଯେତେବେଳେ (len-- > 0)		// delete the from string
		(ଶୂନ୍ୟ)ins_typebuf(tb, 1, 0, TRUE, silent);
	    ଫେରନ୍ତୁ TRUE;
	}
    }
    ଫେରନ୍ତୁ FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_ପଦସମଷ୍ଟି(
    mapblock_T	*mp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*ପଦସମଷ୍ଟି;
    pos_T	save_cursor;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ସନ୍ଦେଶ_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ସନ୍ଦେଶ_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    ପଦସମଷ୍ଟି = vim_strsave(mp->m_str);
    ଯଦି (ପଦସମଷ୍ଟି == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    vim_unescape_csi(ପଦସମଷ୍ଟି);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_ସନ୍ଦେଶ_col = ସନ୍ଦେଶ_col;
    save_ସନ୍ଦେଶ_row = ସନ୍ଦେଶ_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    ଯଦି (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(ପଦସମଷ୍ଟି, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    ସନ୍ଦେଶ_col = save_ସନ୍ଦେଶ_col;
    ସନ୍ଦେଶ_row = save_ସନ୍ଦେଶ_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(ପଦସମଷ୍ଟି);

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    ଫେରନ୍ତୁ res;
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
    ଯଦି (res == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    d = res;
    ପାଇଁ (s = p; *s != NUL; )
    {
	ଯଦି ((s[0] == K_SPECIAL
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
	ଅନ୍ୟଥା
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    ଫେରନ୍ତୁ res;
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

    ଯେତେବେଳେ (*s != NUL)
    {
	ଯଦି (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	ଅନ୍ୟଥା ଯଦି ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	ଅନ୍ୟଥା
	    *d++ = *s++;
    }
    *d = NUL;
    ଫେରନ୍ତୁ (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    ଅକ୍ଷର	*cmd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abbr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hash;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_cpo = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    ପାଇଁ (abbr = 0; abbr < 2; ++abbr)
	ପାଇଁ (hash = 0; hash < 256; ++hash)
	{
	    ଯଦି (abbr)
	    {
		ଯଦି (hash > 0)		// there is only one abbr list
		    ଭାଙ୍ଗ;
		ଯଦି (buf != ଖାଲି)
		    mp = buf->b_first_abbr;
		ଅନ୍ୟଥା
		    mp = first_abbr;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (buf != ଖାଲି)
		    mp = buf->b_maphash[hash];
		ଅନ୍ୟଥା
		    mp = maphash[hash];
	    }

	    ପାଇଁ ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		ଯଦି (mp->m_simplified)
		    ଜାରି;

		// skip script-local mappings
		ଯଦି (mp->m_noremap == REMAP_SCRIPT)
		    ଜାରି;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		ପାଇଁ (p = mp->m_str; *p != NUL; ++p)
		    ଯଦି (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR)
			ଭାଙ୍ଗ;
		ଯଦି (*p != NUL)
		    ଜାରି;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		ଯଦି (abbr)
		    cmd = "abbr";
		ଅନ୍ୟଥା
		    cmd = "map";
		ବାଛ (mp->m_mode)
		{
		    ସ୍ଥିତି MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_NORMAL:
			c1 = 'n';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_VISUAL:
			c1 = 'x';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_SELECT:
			c1 = 's';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_OP_PENDING:
			c1 = 'o';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_CMDLINE | MODE_ଭର୍ତ୍ତି:
			ଯଦି (!abbr)
			    cmd = "map!";
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_CMDLINE:
			c1 = 'c';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_ଭର୍ତ୍ତି:
			c1 = 'i';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_LANGMAP:
			c1 = 'l';
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି MODE_TERMINAL:
			c1 = 't';
			ଭାଙ୍ଗ;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			iemsg(e_makemap_illegal_mode);
			ଫେରନ୍ତୁ FAIL;
		}
		କର	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    ଯଦି (!did_cpo)
		    {
			ଯଦି (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			ଅନ୍ୟଥା
			    ପାଇଁ (i = 0; i < 2; ++i)
				ପାଇଁ (p = (i ? mp->m_str : mp->m_ଚାବିଗୁଡ଼ିକ); *p; ++p)
				    ଯଦି (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			ଯଦି (did_cpo)
			{
			    ଯଦି (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				ଫେରନ୍ତୁ FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    ଯଦି (mp->m_ପଦସମଷ୍ଟି && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && fputs("legacy ", fd) < 0)
			ଫେରନ୍ତୁ FAIL;
#endif
		    ଯଦି (c1 && putc(c1, fd) < 0)
			ଫେରନ୍ତୁ FAIL;
		    ଯଦି (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			ଫେରନ୍ତୁ FAIL;
		    ଯଦି (fputs(cmd, fd) < 0)
			ଫେରନ୍ତୁ FAIL;
		    ଯଦି (buf != ଖାଲି && fputs(" <buffer>", fd) < 0)
			ଫେରନ୍ତୁ FAIL;
		    ଯଦି (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			ଫେରନ୍ତୁ FAIL;
		    ଯଦି (mp->m_silent && fputs(" <silent>", fd) < 0)
			ଫେରନ୍ତୁ FAIL;
#ifdef FEAT_EVAL
		    ଯଦି (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			ଫେରନ୍ତୁ FAIL;
		    ଯଦି (mp->m_ପଦସମଷ୍ଟି && fputs(" <expr>", fd) < 0)
			ଫେରନ୍ତୁ FAIL;
#endif

		    ଯଦି (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_ଚାବିଗୁଡ଼ିକ, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			ଫେରନ୍ତୁ FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} ଯେତେବେଳେ (c1 != NUL);
	    }
	}

    ଯଦି (did_cpo)
	ଯଦି (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_escstr(FILE *fd, char_u *strstart, ପୂର୍ଣ୍ଣସଂଖ୍ୟା what)
{
    char_u	*str = strstart;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers;

    // :map xx <Nop>
    ଯଦି (*str == NUL && what == 1)
    {
	ଯଦି (fprintf(fd, "<Nop>") < 0)
	    ଫେରନ୍ତୁ FAIL;
	ଫେରନ୍ତୁ OK;
    }

    ପାଇଁ ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	ଯଦି (p != ଖାଲି)
	{
	    ଯେତେବେଳେ (*p != NUL)
		ଯଦି (fputc(*p++, fd) < 0)
		    ଫେରନ୍ତୁ FAIL;
	    --str;
	    ଜାରି;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	ଯଦି (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    ଯଦି (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		ଯଦି (p == ଖାଲି)
		    c = *str;
		ଅନ୍ୟଥା
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    ଯଦି (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    ଯଦି (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ଯଦି (fputs((ଅକ୍ଷର *)get_special_ଚାବି_name(c, modifiers), fd) < 0)
		    ଫେରନ୍ତୁ FAIL;
		ଜାରି;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	ଯଦି (c == NL)
	{
	    ଯଦି (what == 2)
	    {
		ଯଦି (fprintf(fd, "\\\026\n") < 0)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (fprintf(fd, "<NL>") < 0)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଜାରି;
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
	ଯଦି (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    ଯଦି (putc('\\', fd) < 0)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    ଯଦି (putc(Ctrl_V, fd) < 0)
		ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (putc(c, fd) < 0)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    ଶୂନ୍ୟ
check_map_keycodes(ଶୂନ୍ୟ)
{
    mapblock_T	*mp;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	buf[3];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abbr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_ଠେଲ(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    ପାଇଁ (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	ପାଇଁ (abbr = 0; abbr <= 1; ++abbr)
	    ପାଇଁ (hash = 0; hash < 256; ++hash)
	    {
		ଯଦି (abbr)
		{
		    ଯଦି (hash)	    // there is only one abbr list
			ଭାଙ୍ଗ;
		    ଯଦି (bp != ଖାଲି)
			mp = bp->b_first_abbr;
		    ଅନ୍ୟଥା
			mp = first_abbr;
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (bp != ଖାଲି)
			mp = bp->b_maphash[hash];
		    ଅନ୍ୟଥା
			mp = maphash[hash];
		}
		ପାଇଁ ( ; mp != ଖାଲି; mp = mp->m_next)
		{
		    ପାଇଁ (i = 0; i <= 1; ++i)	// do this twice
		    {
			ଯଦି (i == 0)
			    p = mp->m_ଚାବିଗୁଡ଼ିକ;	// once for the "from" part
			ଅନ୍ୟଥା
			    p = mp->m_str;	// and once for the "to" part
			ଯେତେବେଳେ (*p)
			{
			    ଯଦି (*p == K_SPECIAL)
			    {
				++p;
				ଯଦି (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (ଶୂନ୍ୟ)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	ଯଦି (bp == ଖାଲି)
	    ଭାଙ୍ଗ;
    }
    ESTACK_CHECK_NOW;
    estack_କାଢ଼();
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*ଚାବିଗୁଡ଼ିକ,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		exact,		// require exact match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ign_mod,	// ignore preceding modifier
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*local_ptr)	// return: buffer-local mapping or NULL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hash;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		local;

    validate_maphash();

    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ଚାବିଗୁଡ଼ିକ);
    ପାଇଁ (local = 1; local >= 0; --local)
	// loop over all hash lists
	ପାଇଁ (hash = 0; hash < 256; ++hash)
	{
	    ଯଦି (abbr)
	    {
		ଯଦି (hash > 0)		// there is only one list.
		    ଭାଙ୍ଗ;
		ଯଦି (local)
		    mp = curbuf->b_first_abbr;
		ଅନ୍ୟଥା
		    mp = first_abbr;
	    }
	    ଅନ୍ୟଥା ଯଦି (local)
		mp = curbuf->b_maphash[hash];
	    ଅନ୍ୟଥା
		mp = maphash[hash];
	    ପାଇଁ ( ; mp != ଖାଲି; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		ଯଦି ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    ଯଦି (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    ଅନ୍ୟଥା
			minlen = len;
		    s = mp->m_ଚାବିଗୁଡ଼ିକ;
		    ଯଦି (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			ଯଦି (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    ଯଦି (STRNCMP(s, ଚାବିଗୁଡ଼ିକ, minlen) == 0)
		    {
			ଯଦି (mp_ptr != ଖାଲି)
			    *mp_ptr = mp;
			ଯଦି (local_ptr != ଖାଲି)
			    *local_ptr = local;
			ଫେରନ୍ତୁ mp->m_str;
		    }
		}
	    }
	}

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * "hasmapto()" function
 */
    ଶୂନ୍ୟ
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*mode;
    char_u	buf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abbr = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    name = tv_get_string(&argvars[0]);
    ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    ଅନ୍ୟଥା
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool(&argvars[2]);
    }

    ଯଦି (map_to_exists(name, mode, abbr))
	rettv->vval.v_ସଂଖ୍ୟା = TRUE;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    buffer_local,   // false if not buffer local mapping
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_ଚାବିଗୁଡ଼ିକ, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_ଚାବିଗୁଡ଼ିକ);
    ଯଦି (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_ସଂଖ୍ୟା(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_ସଂଖ୍ୟା(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_ସଂଖ୍ୟା(dict, "expr", mp->m_ପଦସମଷ୍ଟି ? 1L : 0L);
    dict_add_ସଂଖ୍ୟା(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_add_ସଂଖ୍ୟା(dict, "sid", (long)mp->m_script_ctx.sc_sid);
    dict_add_ସଂଖ୍ୟା(dict, "scriptversion",
		    (long)mp->m_script_ctx.sc_version);
    dict_add_ସଂଖ୍ୟା(dict, "lnum", (long)mp->m_script_ctx.sc_lnum);
    dict_add_ସଂଖ୍ୟା(dict, "buffer", (long)buffer_local);
    dict_add_ସଂଖ୍ୟା(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_add_string(dict, "mode", mapmode);
    dict_add_ସଂଖ୍ୟା(dict, "abbr", abbr ? 1L : 0L);
    dict_add_ସଂଖ୍ୟା(dict, "mode_bits", mp->m_mode);

    vim_free(mapmode);
}

    ସ୍ଥିର ଶୂନ୍ୟ
get_maparg(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exact)
{
    char_u	*ଚାବିଗୁଡ଼ିକ;
    char_u	*ଚାବିଗୁଡ଼ିକ_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*ଚାବିଗୁଡ଼ିକ_buf = ଖାଲି;
    char_u	*alt_ଚାବିଗୁଡ଼ିକ_buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_simplify = FALSE;
    char_u	*rhs;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abbr = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		get_dict = FALSE;
    mapblock_T	*mp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buffer_local;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଚାବିଗୁଡ଼ିକ = tv_get_string(&argvars[0]);
    ଯଦି (*ଚାବିଗୁଡ଼ିକ == NUL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool(&argvars[2]);
	    ଯଦି (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool(&argvars[3]);
	}
    }
    ଅନ୍ୟଥା
	which = (char_u *)"";
    ଯଦି (which == ଖାଲି)
	ଫେରନ୍ତୁ;

    mode = get_map_mode(&which, 0);

    ଚାବିଗୁଡ଼ିକ_simplified = replace_termcodes(ଚାବିଗୁଡ଼ିକ, &ଚାବିଗୁଡ଼ିକ_buf, 0, flags,
								&did_simplify);
    rhs = check_map(ଚାବିଗୁଡ଼ିକ_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    ଯଦି (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(ଶୂନ୍ୟ)replace_termcodes(ଚାବିଗୁଡ଼ିକ, &alt_ଚାବିଗୁଡ଼ିକ_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, ଖାଲି);
	rhs = check_map(alt_ଚାବିଗୁଡ଼ିକ_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    ଯଦି (!get_dict)
    {
	// Return a string.
	ଯଦି (rhs != ଖାଲି)
	{
	    ଯଦି (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    ଅନ୍ୟଥା
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    ଅନ୍ୟଥା ଯଦି (rettv_dict_alloc(rettv) == OK && rhs != ଖାଲି)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? ଚାବିଗୁଡ଼ିକ_simplified : ଖାଲି,
			  buffer_local, abbr);

    vim_free(ଚାବିଗୁଡ଼ିକ_buf);
    vim_free(alt_ଚାବିଗୁଡ଼ିକ_buf);
}

/*
 * "maplist()" function
 */
    ଶୂନ୍ୟ
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buffer_local;
    char_u	*ଚାବିଗୁଡ଼ିକ_buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_simplify;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hash;
    char_u	*lhs;
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abbr = FALSE;

    ଯଦି (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    ପାଇଁ (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	ପାଇଁ (hash = 0; hash < 256; ++hash)
	{
	    ଯଦି (abbr)
	    {
		ଯଦି (hash > 0)		// there is only one abbr list
		    ଭାଙ୍ଗ;
		ଯଦି (buffer_local)
		    mp = curbuf->b_first_abbr;
		ଅନ୍ୟଥା
		    mp = first_abbr;
	    }
	    ଅନ୍ୟଥା ଯଦି (buffer_local)
		mp = curbuf->b_maphash[hash];
	    ଅନ୍ୟଥା
		mp = maphash[hash];
	    ପାଇଁ (; mp; mp = mp->m_next)
	    {
		ଯଦି (mp->m_simplified)
		    ଜାରି;
		ଯଦି ((d = dict_alloc()) == ଖାଲି)
		    ଫେରନ୍ତୁ;
		ଯଦି (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    ଫେରନ୍ତୁ;
		}

		ଚାବିଗୁଡ଼ିକ_buf = ଖାଲି;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_ଚାବିଗୁଡ଼ିକ, TRUE, FALSE);
		(ଶୂନ୍ୟ)replace_termcodes(lhs, &ଚାବିଗୁଡ଼ିକ_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? ଚାବିଗୁଡ଼ିକ_buf : ଖାଲି,
				 buffer_local, abbr);
		vim_free(ଚାବିଗୁଡ଼ିକ_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    ଶୂନ୍ୟ
f_maparg(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		ଫେରନ୍ତୁ;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    ଶୂନ୍ୟ
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_map_mode_string(char_u *mode_string, ପୂର୍ଣ୍ଣସଂଖ୍ୟା abbr)
{
    char_u	*p = mode_string;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tmode;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modec;
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	MASK_V = MODE_VISUAL | MODE_SELECT;
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	MASK_BANG = MODE_ଭର୍ତ୍ତି | MODE_CMDLINE;

    ଯଦି (*p == NUL)
	p = (char_u *)" ";	// compatibility
    ଯେତେବେଳେ ((modec = *p++))
    {
	ବାଛ (modec)
	{
	    ସ୍ଥିତି 'i': tmode = MODE_ଭର୍ତ୍ତି;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'l': tmode = MODE_LANGMAP;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'c': tmode = MODE_CMDLINE;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'n': tmode = MODE_NORMAL;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'x': tmode = MODE_VISUAL;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 's': tmode = MODE_SELECT;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'o': tmode = MODE_OP_PENDING;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 't': tmode = MODE_TERMINAL;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'v': tmode = MASK_V;		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '!': tmode = MASK_BANG;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି ' ': tmode = MASK_MAP;		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		      ଫେରନ୍ତୁ 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    ଯଦି ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	ଫେରନ୍ତୁ 0;

    ଫେରନ୍ତୁ mode;
}

/*
 * "mapset()" function
 */
    ଶୂନ୍ୟ
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode;
    char_u	buf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noremap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ପଦସମଷ୍ଟି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buffer;
    scid_T	sid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nowait;
    char_u	*arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dict_only;
    mapblock_T	*mp_result[2] = {ଖାଲି, ଖାଲି};

    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    ଯଦି (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	ଫେରନ୍ତୁ;

    ଯଦି (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	ଯଦି (which == ଖାଲି || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    ଫେରନ୍ତୁ;
	}
    }
    ଅନ୍ୟଥା
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	ଯଦି (which == ଖାଲି)
	    ଫେରନ୍ତୁ;
	is_abbr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool(&argvars[1]);

	ଯଦି (check_for_dict_arg(argvars, 2) == FAIL)
	    ଫେରନ୍ତୁ;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    ଯଦି (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	ଫେରନ୍ତୁ;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    ଯଦି (lhs == ଖାଲି || lhsraw == ଖାଲି || rhs == ଖାଲି)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	ଫେରନ୍ତୁ;
    }
    orig_rhs = rhs;

    noremap = dict_get_ସଂଖ୍ୟା(d, "noremap") ? REMAP_NONE: 0;
    ଯଦି (dict_get_ସଂଖ୍ୟା(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    ପଦସମଷ୍ଟି = dict_get_ସଂଖ୍ୟା(d, "expr") != 0;
    silent = dict_get_ସଂଖ୍ୟା(d, "silent") != 0;
    sid = dict_get_ସଂଖ୍ୟା(d, "sid");
    scriptversion = dict_get_ସଂଖ୍ୟା(d, "scriptversion");
    lnum = dict_get_ସଂଖ୍ୟା(d, "lnum");
    buffer = dict_get_ସଂଖ୍ୟା(d, "buffer");
    nowait = dict_get_ସଂଖ୍ୟା(d, "nowait") != 0;
    // mode from the dict is not used

    ଯଦି (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    ଅନ୍ୟଥା
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, ଖାଲି);

    ଯଦି (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    ଯଦି (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	ଯଦି (arg == ଖାଲି)
	    ଫେରନ୍ତୁ;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    ଅନ୍ୟଥା
    {
	arg = vim_strsave(lhs);
	ଯଦି (arg == ଖାଲି)
	    ଫେରନ୍ତୁ;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, ପଦସମଷ୍ଟି, sid,
			    scriptversion, lnum, 0);
    ଯଦି (lhsrawalt != ଖାଲି)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, ପଦସମଷ୍ଟି, sid,
			    scriptversion, lnum, 1);

    ଯଦି (mp_result[0] != ଖାଲି && mp_result[1] != ଖାଲି)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#ଯଦି defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
ସଂରଚନା initmap
{
    char_u	*arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
ସ୍ଥିର ସଂରଚନା initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_ଭର୍ତ୍ତି | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# ଯଦି defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
ସ୍ଥିର ସଂରଚନା initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_ଭର୍ତ୍ତି | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_ଭର୍ତ୍ତି | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_ଭର୍ତ୍ତି},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  ଅନ୍ୟଥା
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_ଭର୍ତ୍ତି}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# ଯଦି defined(MACOS_X)
ସ୍ଥିର ସଂରଚନା initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_ଭର୍ତ୍ତି | MODE_CMDLINE},
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
    ଶୂନ୍ୟ
init_mappings(ଶୂନ୍ୟ)
{
#ଯଦି defined(MSWIN) || defined(MACOS_X)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

# ଯଦି defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ଯଦି (!gui.starting)
#  endif
    {
	ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# ଯଦି defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(initmappings); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode, FALSE);
# endif
#endif
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    ଶୂନ୍ୟ
add_map(char_u *map, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    ଯଦି (s != ଖାଲି)
    {
	(ଶୂନ୍ୟ)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#ଯଦି defined(FEAT_LANGMAP)
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    from;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     to;
} langmap_entry_T;

ସ୍ଥିର garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
langmap_set_entry(ପୂର୍ଣ୍ଣସଂଖ୍ୟା from, ପୂର୍ଣ୍ଣସଂଖ୍ୟା to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    a = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    ଯେତେବେଳେ (a != b)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = (a + b) / 2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା d = entries[i].from - from;

	ଯଦି (d == 0)
	{
	    entries[i].to = to;
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (d < 0)
	    a = i + 1;
	ଅନ୍ୟଥା
	    b = i;
    }

    ଯଦି (ga_grow(&langmap_mapga, 1) == FAIL)
	ଫେରନ୍ତୁ;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * ଆକାର(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
langmap_adjust_mb(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା a = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = langmap_mapga.ga_len;

    ଯେତେବେଳେ (a != b)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = (a + b) / 2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା d = entries[i].from - c;

	ଯଦି (d == 0)
	    ଫେରନ୍ତୁ entries[i].to;  // found matching entry
	ଯଦି (d < 0)
	    a = i + 1;
	ଅନ୍ୟଥା
	    b = i;
    }
    ଫେରନ୍ତୁ c;  // no entry found, return "c" unmodified
}

    ଶୂନ୍ୟ
langmap_init(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, ଆକାର(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    ଅକ୍ଷର *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    ପାଇଁ (p = p_langmap; p[0] != NUL; )
    {
	ପାଇଁ (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    ଯଦି (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	ଯଦି (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	ଅନ୍ୟଥା
	    p2 = ଖାଲି;	    // aAbBcCdD form, p2 is NULL
	ଯେତେବେଳେ (p[0])
	{
	    ଯଦି (p[0] == ',')
	    {
		++p;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    ଯଦି (p2 == ଖାଲି)
	    {
		MB_PTR_ADV(p);
		ଯଦି (p[0] != ',')
		{
		    ଯଦି (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (p2[0] != ',')
		{
		    ଯଦି (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    ଯଦି (to == NUL)
	    {
		sprintf(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		ଫେରନ୍ତୁ args->os_errbuf;
	    }

	    ଯଦି (from >= 256)
		langmap_set_entry(from, to);
	    ଅନ୍ୟଥା
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    ଯଦି (p2 != ଖାଲି)
	    {
		MB_PTR_ADV(p2);
		ଯଦି (*p == ';')
		{
		    p = p2;
		    ଯଦି (p[0] != NUL)
		    {
			ଯଦି (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    ଫେରନ୍ତୁ args->os_errbuf;
			}
			++p;
		    }
		    ଭାଙ୍ଗ;
		}
	    }
	}
    }

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
do_exmap(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା isabbrev)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    ବାଛ (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	ସ୍ଥିତି 1: emsg(_(e_invalid_argument));
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		ଭାଙ୍ଗ;
    }
}

/*
 * ":abbreviate" and friends.
 */
    ଶୂନ୍ୟ
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    ଶୂନ୍ୟ
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    ଯଦି (secure)
    {
	secure = 2;
	ସନ୍ଦେଶ_outtrans(eap->cmd);
	ସନ୍ଦେଶ_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    ଶୂନ୍ୟ
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    ଶୂନ୍ୟ
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    ଶୂନ୍ୟ
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
