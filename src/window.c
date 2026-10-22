/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ ਖਾਲੀ cmd_with_ਗਿਣਤੀ(ਅੱਖਰ *cmd, char_u *bufp, size_t bufsize, ਲੰਮਾ Prenum);
ਸਥਿਰ ਖਾਲੀ win_init(win_T *newp, win_T *oldp, ਪੂਰਨ flags);
ਸਥਿਰ ਖਾਲੀ win_init_some(win_T *newp, win_T *oldp);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_comp_pos(ਚੌਖਟਾ_T *topfrp, ਪੂਰਨ *ਪੰਕਤੀ, ਪੂਰਨ *ਖਾਨਾ);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_setheight(ਚੌਖਟਾ_T *curfrp, ਪੂਰਨ ਉਚਾਈ);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_setwidth(ਚੌਖਟਾ_T *curfrp, ਪੂਰਨ ਚੌੜਾਈ);
ਸਥਿਰ ਖਾਲੀ win_exchange(ਲੰਮਾ);
ਸਥਿਰ ਖਾਲੀ win_rotate(ਪੂਰਨ, ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ win_equal_rec(win_T *ਅਗਲਾ_curwin, ਪੂਰਨ current, ਚੌਖਟਾ_T *topfr, ਪੂਰਨ dir, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ);
ਸਥਿਰ ਖਾਲੀ trigger_winnewpre(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ trigger_winclosed(win_T *win);
ਸਥਿਰ win_T *win_free_mem(win_T *win, ਪੂਰਨ *dirp, tabpage_T *tp);
ਸਥਿਰ ਚੌਖਟਾ_T *win_altframe(win_T *win, tabpage_T *tp);
ਸਥਿਰ tabpage_T *alt_tabpage(ਖਾਲੀ);
ਸਥਿਰ win_T *frame2win(ਚੌਖਟਾ_T *frp);
ਸਥਿਰ ਪੂਰਨ ਚੌਖਟਾ_has_win(ਚੌਖਟਾ_T *frp, win_T *wp);
ਸਥਿਰ ਖਾਲੀ win_fix_scroll(ਪੂਰਨ resize);
ਸਥਿਰ ਖਾਲੀ win_fix_cursor(ਪੂਰਨ ਸਾਧਾਰਨ_4);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_new_ਉਚਾਈ(ਚੌਖਟਾ_T *topfrp, ਪੂਰਨ ਉਚਾਈ, ਪੂਰਨ topfirst, ਪੂਰਨ wfh, ਪੂਰਨ ਸੈੱਟ_ch);
ਸਥਿਰ ਪੂਰਨ ਚੌਖਟਾ_fixed_ਉਚਾਈ(ਚੌਖਟਾ_T *frp);
ਸਥਿਰ ਪੂਰਨ ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(ਚੌਖਟਾ_T *frp);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_ਜੋੜੋ_statusline(ਚੌਖਟਾ_T *frp);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_new_ਚੌੜਾਈ(ਚੌਖਟਾ_T *topfrp, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ leftfirst, ਪੂਰਨ wfw);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_ਸੈੱਟ_vsep(ਚੌਖਟਾ_T *frp, ਪੂਰਨ ਜੋੜੋ);
ਸਥਿਰ ਪੂਰਨ ਚੌਖਟਾ_minwidth(ਚੌਖਟਾ_T *topfrp, win_T *ਅਗਲਾ_curwin);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_fix_ਚੌੜਾਈ(win_T *wp);
ਸਥਿਰ ਪੂਰਨ win_alloc_firstwin(win_T *oldwin);
ਸਥਿਰ ਖਾਲੀ new_ਚੌਖਟਾ(win_T *wp);
ਸਥਿਰ tabpage_T *alloc_tabpage(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ leave_tabpage(buf_T *new_curbuf, ਪੂਰਨ trigger_leave_autocmds);
ਸਥਿਰ ਖਾਲੀ enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, ਪੂਰਨ trigger_enter_autocmds, ਪੂਰਨ trigger_leave_autocmds);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_fix_ਉਚਾਈ(win_T *wp);
ਸਥਿਰ ਪੂਰਨ ਚੌਖਟਾ_minheight(ਚੌਖਟਾ_T *topfrp, win_T *ਅਗਲਾ_curwin);
ਸਥਿਰ ਪੂਰਨ ਚੌਖਟਾ_wincount_ਵਿੱਚ_ਉਚਾਈ(ਚੌਖਟਾ_T *topfrp);
ਸਥਿਰ ਪੂਰਨ may_open_tabpage(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ win_enter_ext(win_T *wp, ਪੂਰਨ flags);
ਸਥਿਰ ਖਾਲੀ win_free(win_T *wp, tabpage_T *tp);
ਸਥਿਰ ਖਾਲੀ win_append(win_T *after, win_T *wp);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_append(ਚੌਖਟਾ_T *after, ਚੌਖਟਾ_T *frp);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_ਪਾਓ(ਚੌਖਟਾ_T *before, ਚੌਖਟਾ_T *frp);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_remove(ਚੌਖਟਾ_T *frp);
ਸਥਿਰ ਖਾਲੀ win_goto_ver(ਪੂਰਨ ਉੱਪਰ, ਲੰਮਾ ਗਿਣਤੀ_2);
ਸਥਿਰ ਖਾਲੀ win_goto_hor(ਪੂਰਨ ਖੱਬਾ, ਲੰਮਾ ਗਿਣਤੀ_2);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_ਜੋੜੋ_ਉਚਾਈ(ਚੌਖਟਾ_T *frp, ਪੂਰਨ n);
ਸਥਿਰ ਖਾਲੀ ਆਖਰੀ_ਸਥਿਤੀ_rec(ਚੌਖਟਾ_T *fr, ਪੂਰਨ statusline);
#ਜੇ defined(FEAT_STL_OPT)
ਸਥਿਰ ਪੂਰਨ ਚੌਖਟਾ_ਲੱਭੋ_global_stlh_rec(ਚੌਖਟਾ_T *frp, ਪੂਰਨ h);
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_change_statusline_ਉਚਾਈ_rec(ਚੌਖਟਾ_T *frp, ਪੂਰਨ global_stlh);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ ਚੌਖਟਾ_flatten(ਚੌਖਟਾ_T *frp);
ਸਥਿਰ ਖਾਲੀ winframe_restore(win_T *wp, ਪੂਰਨ dir, ਚੌਖਟਾ_T *unflat_altfr);

ਸਥਿਰ ਪੂਰਨ make_snapshot_rec(ਚੌਖਟਾ_T *fr, ਚੌਖਟਾ_T **frp);
ਸਥਿਰ ਖਾਲੀ clear_snapshot(tabpage_T *tp, ਪੂਰਨ idx);
ਸਥਿਰ ਖਾਲੀ clear_snapshot_rec(ਚੌਖਟਾ_T *fr);
ਸਥਿਰ ਪੂਰਨ check_snapshot_rec(ਚੌਖਟਾ_T *sn, ਚੌਖਟਾ_T *fr);
ਸਥਿਰ win_T *restore_snapshot_rec(ਚੌਖਟਾ_T *sn, ਚੌਖਟਾ_T *fr);
ਸਥਿਰ win_T *get_snapshot_curwin(ਪੂਰਨ idx);

ਸਥਿਰ ਪੂਰਨ ਚੌਖਟਾ_check_ਉਚਾਈ(ਚੌਖਟਾ_T *topfrp, ਪੂਰਨ ਉਚਾਈ);
ਸਥਿਰ ਪੂਰਨ ਚੌਖਟਾ_check_ਚੌੜਾਈ(ਚੌਖਟਾ_T *topfrp, ਪੂਰਨ ਚੌੜਾਈ);

ਸਥਿਰ win_T *win_alloc(win_T *after, ਪੂਰਨ hidden);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOWIN		((win_T *)-1)	// non-existing window

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਕਤੀਆਂ_AVAIL (ਪੰਕਤੀਆਂ - p_ch - tabline_ਉਚਾਈ())

// flags for win_enter_ext()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WEE_UNDO_SYNC			0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WEE_CURWIN_INVALID		0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WEE_TRIGGER_NEW_AUTOCMDS	0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WEE_ALLOW_PARSE_MESSAGES	0x20

ਸਥਿਰ ਅੱਖਰ *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ਸਥਿਰ ਪੂਰਨ ਵੰਡੋ_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ਸਥਿਰ ਪੂਰਨ close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
ਸਥਿਰ ਪੂਰਨ ਚੌਖਟਾ_locked = 0;

#ਜੇ defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
ਸਥਿਰ ਪੂਰਨ stl_rendered_ਉਚਾਈ = 1;
ਸਥਿਰ ਪੂਰਨ stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
ਸਥਿਰ ਪੂਰਨ stlo_mh = ਸਥਿਤੀ_ਉਚਾਈ_2; // "maxheight" value cache of global 'stlo'
#ਜੇ_ਅੰਤ

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    ਖਾਲੀ
ਖਿੜਕੀ_layout_ਤਾਲਾ(ਖਾਲੀ)
{
    ++ਵੰਡੋ_disallowed;
    ++close_disallowed;
}

    ਖਾਲੀ
ਖਿੜਕੀ_layout_unlock(ਖਾਲੀ)
{
    --ਵੰਡੋ_disallowed;
    --close_disallowed;
}

    ਪੂਰਨ
ਚੌਖਟੇ_locked(ਖਾਲੀ)
{
    ਵਾਪਸ ਚੌਖਟਾ_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    ਪੂਰਨ
ਖਿੜਕੀ_layout_locked(ਗਿਣਤੀ CMD_ਸੂਚਕਾਂਕ cmd)
{
    ਜੇ (ਵੰਡੋ_disallowed > 0 || close_disallowed > 0)
    {
	ਜੇ (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_ਵੰਡੋ_ਖਿੜਕੀ_when_closing_ਬਫਰ));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_not_allowed_ਮੰਜ਼ਿਲ_change_ਖਿੜਕੀ_layout_ਵਿੱਚ_this_autocmd));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

// #define WIN_DEBUG
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    ਸਥਿਰ ਖਾਲੀ
log_ਚੌਖਟਾ_layout(ਚੌਖਟਾ_T *ਚੌਖਟਾ)
{
    ch_log(ਨਿਰਾ, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    ਚੌਖਟਾ->fr_layout == FR_ਪੱਤੀ ? "LEAF"
				  : ਚੌਖਟਾ->fr_layout == FR_ਪੰਕਤੀ ? "ROW" : "COL",
	    ਚੌਖਟਾ->fr_ਚੌੜਾਈ,
	    ਚੌਖਟਾ->fr_ਉਚਾਈ,
	    ਚੌਖਟਾ->fr_win == ਨਿਰਾ ? -1 : ਚੌਖਟਾ->fr_win->w_ਚੌੜਾਈ,
	    ਚੌਖਟਾ->fr_win == ਨਿਰਾ ? -1 : ਚੌਖਟਾ->fr_win->w_ਉਚਾਈ,
	    ਚੌਖਟਾ->fr_win == ਨਿਰਾ ? -1 : ਚੌਖਟਾ->fr_win->w_id);
    ਜੇ (ਚੌਖਟਾ->fr_ਬੱਚਾ != ਨਿਰਾ)
    {
	ch_log(ਨਿਰਾ, "children");
	log_ਚੌਖਟਾ_layout(ਚੌਖਟਾ->fr_ਬੱਚਾ);
	ਜੇ (ਚੌਖਟਾ->fr_ਅਗਲਾ != ਨਿਰਾ)
	    ch_log(ਨਿਰਾ, "END of children");
    }
    ਜੇ (ਚੌਖਟਾ->fr_ਅਗਲਾ != ਨਿਰਾ)
	log_ਚੌਖਟਾ_layout(ਚੌਖਟਾ->fr_ਅਗਲਾ);
}
#ਜੇ_ਅੰਤ

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    ਪੂਰਨ
check_can_ਸੈੱਟ_curbuf_disabled(ਖਾਲੀ)
{
    ਜੇ (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_ਮੰਜ਼ਿਲ_ਬਫਰ));
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    ਪੂਰਨ
check_can_ਸੈੱਟ_curbuf_forceit(ਪੂਰਨ forceit)
{
    ਜੇ (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_ਮੰਜ਼ਿਲ_ਬਫਰ));
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(ਖਾਲੀ)
{
    // In cmdwin, the alternative buffer should be used.
    ਵਾਪਸ is_ਵਿੱਚ_cmdwin() && prevwin != ਨਿਰਾ ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = ਨਿਰਾ;

    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    ਜੇ (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    ਜੇ (wp == ਨਿਰਾ && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    ਵਾਪਸ wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    ਖਾਲੀ
do_ਖਿੜਕੀ(
    ਪੂਰਨ		nchar,
    ਲੰਮਾ	Prenum,
    ਪੂਰਨ		xchar)	    // extra char from ":wincmd gx" or NUL
{
    ਲੰਮਾ	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
    ਪੂਰਨ		type = ਲੱਭੋ_DEFINE;
    ਪੂਰਨ		len;
#ਜੇ_ਅੰਤ
    char_u	cbuf[40];

    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHECK_CMDWIN \
    ਕਰੋ { \
	ਜੇ (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_ਵਿੱਚ_cmdline_ਖਿੜਕੀ)); \
	    ਵਾਪਸ; \
	} \
    } ਜਦੋਂ (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    ਚੋਣ (nchar)
    {
// split current window in two parts, horizontally
    ਹਾਲਤ 'S':
    ਹਾਲਤ Ctrl_S:
    ਹਾਲਤ 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		ਜੇ (bt_quickfix(curbuf))
		    ਜਾਓ newwindow;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		(ਖਾਲੀ)win_ਵੰਡੋ((ਪੂਰਨ)Prenum, 0);
		ਰੋਕੋ;

// split current window in two parts, vertically
    ਹਾਲਤ Ctrl_V:
    ਹਾਲਤ 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		ਜੇ (bt_quickfix(curbuf))
		    ਜਾਓ newwindow;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		(ਖਾਲੀ)win_ਵੰਡੋ((ਪੂਰਨ)Prenum, WSP_VERT);
		ਰੋਕੋ;

// split current window and edit alternate file
    ਹਾਲਤ Ctrl_HAT:
    ਹਾਲਤ '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		ਜੇ (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == ਨਿਰਾ)
		{
		    ਜੇ (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    ਨਹੀਂਤਾਂ
			semsg(_(e_ਬਫਰ_nr_not_ਮਿਲਿਆ), Prenum);
		    ਰੋਕੋ;
		}

		ਜੇ (!curbuf_locked() && win_ਵੰਡੋ(0, 0) == OK)
		    (ਖਾਲੀ)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		ਰੋਕੋ;

// open new window
    ਹਾਲਤ Ctrl_N:
    ਹਾਲਤ 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		ਜੇ (Prenum)
		    // window height
		    vim_snprintf((ਅੱਖਰ *)cbuf, ਆਕਾਰ(cbuf) - 5, "%ld", Prenum);
		ਨਹੀਂਤਾਂ
		    cbuf[0] = NUL;
#ਜੇ defined(FEAT_QUICKFIX)
		ਜੇ (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#ਜੇ_ਅੰਤ
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		ਰੋਕੋ;

// quit current window
    ਹਾਲਤ Ctrl_Q:
    ਹਾਲਤ 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ਗਿਣਤੀ("quit", cbuf, ਆਕਾਰ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ਰੋਕੋ;

// close current window
    ਹਾਲਤ Ctrl_C:
    ਹਾਲਤ 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ਗਿਣਤੀ("close", cbuf, ਆਕਾਰ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ਰੋਕੋ;

#ਜੇ defined(FEAT_QUICKFIX)
// close preview window
    ਹਾਲਤ Ctrl_Z:
    ਹਾਲਤ 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		ਰੋਕੋ;

// cursor to preview window
    ਹਾਲਤ 'P':
		FOR_ALL_WINDOWS(wp)
		    ਜੇ (wp->w_p_pvw)
			ਰੋਕੋ;
		ਜੇ (wp == ਨਿਰਾ)
		    emsg(_(e_there_is_no_preview_ਖਿੜਕੀ));
		ਨਹੀਂਤਾਂ
		    win_goto(wp);
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ

// close all but current window
    ਹਾਲਤ Ctrl_O:
    ਹਾਲਤ 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ਗਿਣਤੀ("only", cbuf, ਆਕਾਰ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ਰੋਕੋ;

// cursor to next window with wrap around
    ਹਾਲਤ Ctrl_W:
    ਹਾਲਤ 'w':
// cursor to previous window with wrap around
    ਹਾਲਤ 'W':
		CHECK_CMDWIN;
		ਜੇ (ONE_ਖਿੜਕੀ && Prenum != 1)	// just one window
		    beep_flush();
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (Prenum)			// go to specified window
		    {
			ਲਈ (wp = firstwin; --Prenum > 0; )
			{
			    ਜੇ (wp->w_ਅਗਲਾ == ਨਿਰਾ)
				ਰੋਕੋ;
			    ਨਹੀਂਤਾਂ
				wp = wp->w_ਅਗਲਾ;
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    ਜੇ (wp == ਨਿਰਾ)
				wp = lastwin;	    // wrap around
			}
			ਨਹੀਂਤਾਂ			    // go to next window
			{
			    wp = curwin->w_ਅਗਲਾ;
			    ਜੇ (wp == ਨਿਰਾ)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		ਰੋਕੋ;

// cursor to window below
    ਹਾਲਤ 'j':
    ਹਾਲਤ K_ਥੱਲੇ:
    ਹਾਲਤ Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		ਰੋਕੋ;

// cursor to window above
    ਹਾਲਤ 'k':
    ਹਾਲਤ K_ਉੱਪਰ:
    ਹਾਲਤ Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		ਰੋਕੋ;

// cursor to left window
    ਹਾਲਤ 'h':
    ਹਾਲਤ K_ਖੱਬਾ:
    ਹਾਲਤ Ctrl_H:
    ਹਾਲਤ K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		ਰੋਕੋ;

// cursor to right window
    ਹਾਲਤ 'l':
    ਹਾਲਤ K_ਸੱਜਾ:
    ਹਾਲਤ Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		ਰੋਕੋ;

// move window to new tab page
    ਹਾਲਤ 'T':
		CHECK_CMDWIN;
		ਜੇ (one_ਖਿੜਕੀ())
		    ਸੁਨੇਹਾ(_(m_onlyone));
		ਨਹੀਂਤਾਂ
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    ਜੇ (win_new_tabpage((ਪੂਰਨ)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			ਜੇ (curwin == wp)
			    win_close(curwin, FALSE);
			ਜੇ (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		ਰੋਕੋ;

// cursor to top-left window
    ਹਾਲਤ 't':
    ਹਾਲਤ Ctrl_T:
		win_goto(firstwin);
		ਰੋਕੋ;

// cursor to bottom-right window
    ਹਾਲਤ 'b':
    ਹਾਲਤ Ctrl_B:
		win_goto(lastwin);
		ਰੋਕੋ;

// cursor to last accessed (previous) window
    ਹਾਲਤ 'p':
    ਹਾਲਤ Ctrl_P:
		ਜੇ (!win_valid(prevwin))
		    beep_flush();
		ਨਹੀਂਤਾਂ
		    win_goto(prevwin);
		ਰੋਕੋ;

// exchange current and next window
    ਹਾਲਤ 'x':
    ਹਾਲਤ Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		ਰੋਕੋ;

// rotate windows downwards
    ਹਾਲਤ Ctrl_R:
    ਹਾਲਤ 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (ਪੂਰਨ)Prenum1);    // downwards
		ਰੋਕੋ;

// rotate windows upwards
    ਹਾਲਤ 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (ਪੂਰਨ)Prenum1);	    // upwards
		ਰੋਕੋ;

// move window to the very top/bottom/left/right
    ਹਾਲਤ 'K':
    ਹਾਲਤ 'J':
    ਹਾਲਤ 'H':
    ਹਾਲਤ 'L':
		CHECK_CMDWIN;
		ਜੇ (ONE_ਖਿੜਕੀ)
		    beep_flush();
		ਨਹੀਂਤਾਂ
		{
		    ਪੂਰਨ dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_ਸਿਖਰ : WSP_BOT);

		    (ਖਾਲੀ)win_splitmove(curwin, (ਪੂਰਨ)Prenum, dir);
		}
		ਰੋਕੋ;

// make all windows the same width and/or height
    ਹਾਲਤ '=':
		{
		    ਪੂਰਨ mod = cmdmod.cmod_ਵੰਡੋ & (WSP_VERT | WSP_HOR);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		    win_equal(ਨਿਰਾ, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		ਰੋਕੋ;

// increase current window height
    ਹਾਲਤ '+':
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		win_setheight(curwin->w_ਉਚਾਈ + (ਪੂਰਨ)Prenum1);
		ਰੋਕੋ;

// decrease current window height
    ਹਾਲਤ '-':
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		win_setheight(curwin->w_ਉਚਾਈ - (ਪੂਰਨ)Prenum1);
		ਰੋਕੋ;

// set current window height
    ਹਾਲਤ Ctrl__:
    ਹਾਲਤ '_':
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		win_setheight(Prenum ? (ਪੂਰਨ)Prenum : 9999);
		ਰੋਕੋ;

// increase current window width
    ਹਾਲਤ '>':
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		win_setwidth(curwin->w_ਚੌੜਾਈ + (ਪੂਰਨ)Prenum1);
		ਰੋਕੋ;

// decrease current window width
    ਹਾਲਤ '<':
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		win_setwidth(curwin->w_ਚੌੜਾਈ - (ਪੂਰਨ)Prenum1);
		ਰੋਕੋ;

// set current window width
    ਹਾਲਤ '|':
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		win_setwidth(Prenum != 0 ? (ਪੂਰਨ)Prenum : 9999);
		ਰੋਕੋ;

// jump to tag and split window if tag exists (in preview window)
#ਜੇ defined(FEAT_QUICKFIX)
    ਹਾਲਤ '}':
		CHECK_CMDWIN;
		ਜੇ (Prenum)
		    g_do_tagpreview = Prenum;
		ਨਹੀਂਤਾਂ
		    g_do_tagpreview = p_pvh;
#ਜੇ_ਅੰਤ
		// FALLTHROUGH
    ਹਾਲਤ ']':
    ਹਾਲਤ Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		ਜੇ (Prenum)
		    postponed_ਵੰਡੋ = Prenum;
		ਨਹੀਂਤਾਂ
		    postponed_ਵੰਡੋ = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		ਜੇ (nchar != '}')
		    g_do_tagpreview = 0;
#ਜੇ_ਅੰਤ

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_ਵੰਡੋ = 0;
		ਰੋਕੋ;

// edit file name under cursor in a new window
    ਹਾਲਤ 'f':
    ਹਾਲਤ 'F':
    ਹਾਲਤ Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		ਜੇ (check_ਪਾਠ_or_curbuf_locked(ਨਿਰਾ))
		    ਰੋਕੋ;

		ptr = grab_file_name(Prenum1, &lnum);
		ਜੇ (ptr != ਨਿਰਾ)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = ਨਿਰਾ;
		    ਜੇ ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    ਜੇ (wp == ਨਿਰਾ && win_ਵੰਡੋ(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			ਜੇ (do_ecmd(0, ptr, ਨਿਰਾ, ਨਿਰਾ, ECMD_LASTL,
						   ECMD_HIDE, ਨਿਰਾ) == FAIL)
			{
			    /*
			     * Note: if FEAT_EVAL is defined and do_ecmd() is aborted resulting
			     * in got_int be true, win_close() unconditionally fails. In such
			     * case, the window split for do_ecmd() is left unclosed, i.e. the
			     * current window is just duplicated. To avoid this, save and load
			     * got_int value before and after closing the window.
			     */
			    sig_atomic_t    old_got_int = got_int;
			    got_int = FALSE;

			    // Failed to open the file, close the window
			    // opened for it.
			    win_close(curwin, FALSE);
			    got_int = got_int || old_got_int;

			    goto_tabpage_win(oldtab, oldwin);
			}
			ਨਹੀਂਤਾਂ
			    wp = curwin;
		    }

		    ਜੇ (wp != ਨਿਰਾ && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    ਹਾਲਤ 'i':			    // Go to any match
    ਹਾਲਤ Ctrl_I:
		type = ਲੱਭੋ_ANY;
		// FALLTHROUGH
    ਹਾਲਤ 'd':			    // Go to definition, using 'define'
    ਹਾਲਤ Ctrl_D:
		CHECK_CMDWIN;
		ਜੇ ((len = ਲੱਭੋ_ident_under_cursor(&ptr, ਲੱਭੋ_IDENT)) == 0)
		    ਰੋਕੋ;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		ਜੇ (ptr == ਨਿਰਾ)
		    ਰੋਕੋ;

		ਲੱਭੋ_ਨਮੂਨਾ_ਵਿੱਚ_ਰਸਤਾ(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_ਵੰਡੋ, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_ਸੈੱਟ_curswant = true;
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ

// Quickfix window only: view the result under the cursor in a new split.
#ਜੇ defined(FEAT_QUICKFIX)
    ਹਾਲਤ K_KENTER:
    ਹਾਲਤ CAR:
		ਜੇ (bt_quickfix(curbuf))
		    qf_view_ਨਤੀਜਾ(TRUE);
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ

// CTRL-W g  extended commands
    ਹਾਲਤ 'g':
    ਹਾਲਤ Ctrl_G:
		CHECK_CMDWIN;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#ਜੇ_ਅੰਤ
		++no_mapping;
		++allow_ਕੁੰਜੀਆਂ;   // no mapping for xchar, but allow key codes
		ਜੇ (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_ਕੁੰਜੀਆਂ;
		(ਖਾਲੀ)ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd(xchar);

		ਚੋਣ (xchar)
		{
#ਜੇ defined(FEAT_QUICKFIX)
		    ਹਾਲਤ '}':
			xchar = Ctrl_RSB;
			ਜੇ (Prenum)
			    g_do_tagpreview = Prenum;
			ਨਹੀਂਤਾਂ
			    g_do_tagpreview = p_pvh;
#ਜੇ_ਅੰਤ
			// FALLTHROUGH
		    ਹਾਲਤ ']':
		    ਹਾਲਤ Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			ਜੇ (Prenum)
			    postponed_ਵੰਡੋ = Prenum;
			ਨਹੀਂਤਾਂ
			    postponed_ਵੰਡੋ = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_ਵੰਡੋ = 0;
			ਰੋਕੋ;

		    ਹਾਲਤ 'f':	    // CTRL-W gf: "gf" in a new tab page
		    ਹਾਲਤ 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_ਸੂਚਕਾਂਕ(curtab) + 1;
			nchar = xchar;
			ਜਾਓ wingotofile;

		    ਹਾਲਤ 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((ਪੂਰਨ)Prenum);
			ਰੋਕੋ;

		    ਹਾਲਤ 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(ਪੂਰਨ)Prenum1);
			ਰੋਕੋ;

		    ਹਾਲਤ TAB:	    // CTRL-W g<Tab>: go to last used tab page
			ਜੇ (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			ਰੋਕੋ;

		    ਮੂਲ:
			beep_flush();
			ਰੋਕੋ;
		}
		ਰੋਕੋ;

    ਮੂਲ:	beep_flush();
		ਰੋਕੋ;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    ਖਾਲੀ
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    ਚੋਣ (*arg)
    {
    ਹਾਲਤ 'S':
    ਹਾਲਤ Ctrl_S:
    ਹਾਲਤ 's':
    ਹਾਲਤ Ctrl_N:
    ਹਾਲਤ 'n':
    ਹਾਲਤ 'j':
    ਹਾਲਤ Ctrl_J:
    ਹਾਲਤ 'k':
    ਹਾਲਤ Ctrl_K:
    ਹਾਲਤ 'T':
    ਹਾਲਤ Ctrl_R:
    ਹਾਲਤ 'r':
    ਹਾਲਤ 'R':
    ਹਾਲਤ 'K':
    ਹਾਲਤ 'J':
    ਹਾਲਤ '+':
    ਹਾਲਤ '-':
    ਹਾਲਤ Ctrl__:
    ਹਾਲਤ '_':
    ਹਾਲਤ '|':
    ਹਾਲਤ ']':
    ਹਾਲਤ Ctrl_RSB:
    ਹਾਲਤ 'g':
    ਹਾਲਤ Ctrl_G:
    ਹਾਲਤ Ctrl_V:
    ਹਾਲਤ 'v':
    ਹਾਲਤ 'h':
    ਹਾਲਤ Ctrl_H:
    ਹਾਲਤ 'l':
    ਹਾਲਤ Ctrl_L:
    ਹਾਲਤ 'H':
    ਹਾਲਤ 'L':
    ਹਾਲਤ '>':
    ਹਾਲਤ '<':
#ਜੇ defined(FEAT_QUICKFIX)
    ਹਾਲਤ '}':
#ਜੇ_ਅੰਤ
    ਹਾਲਤ 'f':
    ਹਾਲਤ 'F':
    ਹਾਲਤ Ctrl_F:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
    ਹਾਲਤ 'i':
    ਹਾਲਤ Ctrl_I:
    ਹਾਲਤ 'd':
    ਹਾਲਤ Ctrl_D:
#ਜੇ_ਅੰਤ
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		ਰੋਕੋ;

    ਹਾਲਤ Ctrl_HAT:
    ਹਾਲਤ '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		ਰੋਕੋ;

    ਹਾਲਤ Ctrl_Q:
    ਹਾਲਤ 'q':
    ਹਾਲਤ Ctrl_C:
    ਹਾਲਤ 'c':
    ਹਾਲਤ Ctrl_O:
    ਹਾਲਤ 'o':
    ਹਾਲਤ Ctrl_W:
    ਹਾਲਤ 'w':
    ਹਾਲਤ 'W':
    ਹਾਲਤ 'x':
    ਹਾਲਤ Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		ਰੋਕੋ;

#ਜੇ defined(FEAT_QUICKFIX)
    ਹਾਲਤ Ctrl_Z:
    ਹਾਲਤ 'z':
    ਹਾਲਤ 'P':
#ਜੇ_ਅੰਤ
    ਹਾਲਤ 't':
    ਹਾਲਤ Ctrl_T:
    ਹਾਲਤ 'b':
    ਹਾਲਤ Ctrl_B:
    ਹਾਲਤ 'p':
    ਹਾਲਤ Ctrl_P:
    ਹਾਲਤ '=':
    ਹਾਲਤ CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		ਰੋਕੋ;
    }
}

    ਸਥਿਰ ਖਾਲੀ
cmd_with_ਗਿਣਤੀ(
    ਅੱਖਰ	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    ਲੰਮਾ	Prenum)
{
    ਜੇ (Prenum > 0)
	vim_snprintf((ਅੱਖਰ *)bufp, bufsize, "%s %ld", cmd, Prenum);
    ਨਹੀਂਤਾਂ
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    ਪੂਰਨ
check_ਵੰਡੋ_disallowed(win_T *wp)
{
    ਜੇ (ਵੰਡੋ_disallowed > 0)
    {
	emsg(_(e_cant_ਵੰਡੋ_ਖਿੜਕੀ_while_closing_another));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (wp->w_ਬਫਰ->b_locked_ਵੰਡੋ)
    {
	emsg(_(e_cannot_ਵੰਡੋ_ਖਿੜਕੀ_when_closing_ਬਫਰ));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * split the current window, implements CTRL-W s and :split
 *
 * "size" is the height or width for the new window, 0 to use half of current
 * height or width.
 *
 * "flags":
 * WSP_ROOM: require enough room for new window
 * WSP_VERT: vertical split.
 * WSP_TOP:  open window at the top-left of the shell (help window).
 * WSP_BOT:  open window at the bottom-right of the shell (quickfix window).
 * WSP_HELP: creating the help window, keep layout snapshot
 *
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
win_ਵੰਡੋ(ਪੂਰਨ ਮਾਪ, ਪੂਰਨ flags)
{
    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ FAIL;

    ਜੇ (check_ਵੰਡੋ_disallowed(curwin) == FAIL)
	ਵਾਪਸ FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    ਜੇ (may_open_tabpage() == OK)
	ਵਾਪਸ OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_ਵੰਡੋ;
    ਜੇ ((flags & WSP_ਸਿਖਰ) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_ਵੰਡੋ_topleft_and_botright_at_the_same_ਸਮਾਂ));
	ਵਾਪਸ FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    ਜੇ (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    ਨਹੀਂਤਾਂ
	clear_snapshot(curtab, SNAP_HELP_IDX);

    ਜੇ (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    ਨਹੀਂਤਾਂ
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    ਵਾਪਸ win_ਵੰਡੋ_ins(ਮਾਪ, flags, ਨਿਰਾ, 0, ਨਿਰਾ);
}

/*
 * When "new_wp" is NULL: split the current window in two.
 * When "new_wp" is not NULL: insert this window at the far
 * top/left/right/bottom.
 * When "to_flatten" is not NULL: flatten this frame before reorganising frames;
 * remains unflattened on failure.
 *
 * On failure, if "new_wp" was not NULL, no changes will have been made to the
 * window layout or sizes.
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
win_ਵੰਡੋ_ins(
    ਪੂਰਨ		ਮਾਪ,
    ਪੂਰਨ		flags,
    win_T	*new_wp,
    ਪੂਰਨ		dir,
    ਚੌਖਟਾ_T	*ਮੰਜ਼ਿਲ_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    ਪੂਰਨ		new_ਮਾਪ = ਮਾਪ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		need_ਸਥਿਤੀ = 0;
    ਪੂਰਨ		do_equal = FALSE;
    ਪੂਰਨ		needed;
    ਪੂਰਨ		ਉਪਲਬਧ;
    ਪੂਰਨ		oldwin_ਉਚਾਈ = 0;
    ਪੂਰਨ		layout;
    ਚੌਖਟਾ_T	*frp, *curfrp, *frp2, *prevfrp;
    ਪੂਰਨ		before;
    ਪੂਰਨ		minheight;
    ਪੂਰਨ		wmh1;
    ਪੂਰਨ		did_ਸੈੱਟ_fraction = FALSE;
    ਪੂਰਨ		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    ਜੇ (new_wp == ਨਿਰਾ)
	trigger_winnewpre();

    ਜੇ (flags & WSP_ਸਿਖਰ)
	oldwin = firstwin;
    ਨਹੀਂਤਾਂ ਜੇ (flags & WSP_BOT)
	oldwin = lastwin;
    ਨਹੀਂਤਾਂ
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    ਜੇ (ONE_ਖਿੜਕੀ && p_ls == 1 && oldwin->w_ਸਥਿਤੀ_ਉਚਾਈ == 0)
    {
	ਜੇ (!(flags & WSP_FORCE_ਗੁੰਜਾਇਸ਼) && VISIBLE_ਉਚਾਈ(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_ਗੁੰਜਾਇਸ਼));
	    ਜਾਓ theend;
	}
	need_ਸਥਿਤੀ = statusline_ਉਚਾਈ(oldwin);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    ਜੇ (gui.ਵਿੱਚ_use)
	ਬਾਹਰਲਾ_flush();
#ਜੇ_ਅੰਤ

    ਜੇ (flags & WSP_VERT)
    {
	ਪੂਰਨ	wmw1;
	ਪੂਰਨ	minwidth;

	layout = FR_ਪੰਕਤੀ;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	ਜੇ (flags & WSP_ਗੁੰਜਾਇਸ਼)
	    needed += p_wiw - wmw1;
	ਜੇ (flags & (WSP_BOT | WSP_ਸਿਖਰ))
	{
	    minwidth = ਚੌਖਟਾ_minwidth(topframe, NOWIN);
	    ਉਪਲਬਧ = topframe->fr_ਚੌੜਾਈ;
	    needed += minwidth;
	}
	ਨਹੀਂਤਾਂ ਜੇ (p_ea)
	{
	    minwidth = ਚੌਖਟਾ_minwidth(oldwin->w_ਚੌਖਟਾ, NOWIN);
	    prevfrp = oldwin->w_ਚੌਖਟਾ;
	    ਲਈ (frp = oldwin->w_ਚੌਖਟਾ->fr_ਮਾਪਾ; frp != ਨਿਰਾ;
							frp = frp->fr_ਮਾਪਾ)
	    {
		ਜੇ (frp->fr_layout == FR_ਪੰਕਤੀ)
		    FOR_ALL_ਚੌਖਟੇ(frp2, frp->fr_ਬੱਚਾ)
			ਜੇ (frp2 != prevfrp)
			    minwidth += ਚੌਖਟਾ_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    ਉਪਲਬਧ = topframe->fr_ਚੌੜਾਈ;
	    needed += minwidth;
	}
	ਨਹੀਂਤਾਂ
	{
	    minwidth = ਚੌਖਟਾ_minwidth(oldwin->w_ਚੌਖਟਾ, NOWIN);
	    ਉਪਲਬਧ = oldwin->w_ਚੌਖਟਾ->fr_ਚੌੜਾਈ;
	    needed += minwidth;
	}
	ਜੇ (!(flags & WSP_FORCE_ਗੁੰਜਾਇਸ਼) && ਉਪਲਬਧ < needed)
	{
	    emsg(_(e_not_enough_ਗੁੰਜਾਇਸ਼));
	    ਜਾਓ theend;
	}
	ਜੇ (new_ਮਾਪ == 0)
	    new_ਮਾਪ = oldwin->w_ਚੌੜਾਈ / 2;
	ਜੇ (new_ਮਾਪ > ਉਪਲਬਧ - minwidth - 1)
	    new_ਮਾਪ = ਉਪਲਬਧ - minwidth - 1;
	ਜੇ (new_ਮਾਪ < wmw1)
	    new_ਮਾਪ = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	ਜੇ (oldwin->w_ਚੌੜਾਈ - new_ਮਾਪ - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	ਜੇ (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_ਚੌੜਾਈ + new_ਮਾਪ + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	ਜੇ (!do_equal && p_ea && ਮਾਪ == 0 && *p_ead != 'v'
	    && oldwin->w_ਚੌਖਟਾ->fr_ਮਾਪਾ != ਨਿਰਾ)
	    do_equal = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	layout = FR_ਖਾਨਾ;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_ਉਚਾਈ(curwin);
	needed = wmh1 + statusline_ਉਚਾਈ(oldwin);
	ਜੇ (flags & WSP_ਗੁੰਜਾਇਸ਼)
	    needed += p_wh - wmh1;
	ਜੇ (flags & (WSP_BOT | WSP_ਸਿਖਰ))
	{
	    minheight = ਚੌਖਟਾ_minheight(topframe, NOWIN) + need_ਸਥਿਤੀ;
	    ਉਪਲਬਧ = topframe->fr_ਉਚਾਈ;
	    needed += minheight;
	}
	ਨਹੀਂਤਾਂ ਜੇ (p_ea)
	{
	    minheight = ਚੌਖਟਾ_minheight(oldwin->w_ਚੌਖਟਾ, NOWIN) + need_ਸਥਿਤੀ;
	    prevfrp = oldwin->w_ਚੌਖਟਾ;
	    ਲਈ (frp = oldwin->w_ਚੌਖਟਾ->fr_ਮਾਪਾ; frp != ਨਿਰਾ;
							frp = frp->fr_ਮਾਪਾ)
	    {
		ਜੇ (frp->fr_layout == FR_ਖਾਨਾ)
		    FOR_ALL_ਚੌਖਟੇ(frp2, frp->fr_ਬੱਚਾ)
			ਜੇ (frp2 != prevfrp)
			    minheight += ਚੌਖਟਾ_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    ਉਪਲਬਧ = topframe->fr_ਉਚਾਈ;
	    needed += minheight;
	}
	ਨਹੀਂਤਾਂ
	{
	    minheight = ਚੌਖਟਾ_minheight(oldwin->w_ਚੌਖਟਾ, NOWIN) + need_ਸਥਿਤੀ;
	    ਉਪਲਬਧ = oldwin->w_ਚੌਖਟਾ->fr_ਉਚਾਈ;
	    needed += minheight;
	}
	ਜੇ (!(flags & WSP_FORCE_ਗੁੰਜਾਇਸ਼) && ਉਪਲਬਧ < needed)
	{
	    emsg(_(e_not_enough_ਗੁੰਜਾਇਸ਼));
	    ਜਾਓ theend;
	}
	oldwin_ਉਚਾਈ = oldwin->w_ਉਚਾਈ;
	ਜੇ (need_ਸਥਿਤੀ)
	{
	    oldwin->w_ਸਥਿਤੀ_ਉਚਾਈ = statusline_ਉਚਾਈ(oldwin);
	    oldwin_ਉਚਾਈ -= oldwin->w_ਸਥਿਤੀ_ਉਚਾਈ;
	}
	// Estimated stl height for the new window.
	ਪੂਰਨ new_win_stlh = need_ਸਥਿਤੀ != 0 ? need_ਸਥਿਤੀ
			: statusline_ਉਚਾਈ(new_wp != ਨਿਰਾ ? new_wp : curwin);
	ਜੇ (new_ਮਾਪ == 0)
	    new_ਮਾਪ = (oldwin_ਉਚਾਈ - new_win_stlh + 1) / 2;
	ਜੇ (new_ਮਾਪ > ਉਪਲਬਧ - minheight - new_win_stlh)
	    new_ਮਾਪ = ਉਪਲਬਧ - minheight - new_win_stlh;
	ਜੇ (new_ਮਾਪ < wmh1)
	    new_ਮਾਪ = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	ਜੇ (oldwin_ਉਚਾਈ - new_ਮਾਪ - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	ਜੇ (oldwin->w_p_wfh)
	{
	    // Set w_fraction now so that the cursor keeps the same relative
	    // vertical position using the old height.
	    ਸੈੱਟ_fraction(oldwin);
	    did_ਸੈੱਟ_fraction = TRUE;

	    win_setheight_win(oldwin->w_ਉਚਾਈ + new_ਮਾਪ
		    + statusline_ਉਚਾਈ(oldwin), oldwin);
	    // w_height now excludes the status line
	    oldwin_ਉਚਾਈ = oldwin->w_ਉਚਾਈ;
	}

	// Make all windows the same height if 'equalalways' is set, 'ead' is
	// not set to 'h', a size was not provided and there is a parent frame.
	ਜੇ (!do_equal && p_ea && ਮਾਪ == 0 && *p_ead != 'h'
	    && oldwin->w_ਚੌਖਟਾ->fr_ਮਾਪਾ != ਨਿਰਾ)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    ਜੇ ((flags & WSP_ਸਿਖਰ) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_ਹੇਠਾਂ)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	ਜੇ (new_wp == ਨਿਰਾ)
	    wp = win_alloc(oldwin, FALSE);
	ਨਹੀਂਤਾਂ
	    win_append(oldwin, wp);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (new_wp == ਨਿਰਾ)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	ਨਹੀਂਤਾਂ
	    win_append(oldwin->w_prev, wp);
    }

    ਜੇ (new_wp == ਨਿਰਾ)
    {
	ਜੇ (wp == ਨਿਰਾ)
	    ਜਾਓ theend;

	new_ਚੌਖਟਾ(wp);
	ਜੇ (wp->w_ਚੌਖਟਾ == ਨਿਰਾ)
	{
	    win_free(wp, ਨਿਰਾ);
	    ਜਾਓ theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    ਜੇ (ਮੰਜ਼ਿਲ_flatten != ਨਿਰਾ)
	ਚੌਖਟਾ_flatten(ਮੰਜ਼ਿਲ_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    ਜੇ (flags & (WSP_ਸਿਖਰ | WSP_BOT))
    {
	ਜੇ ((topframe->fr_layout == FR_ਖਾਨਾ && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ਪੰਕਤੀ && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_ਬੱਚਾ;
	    ਜੇ (flags & WSP_BOT)
		ਜਦੋਂ (curfrp->fr_ਅਗਲਾ != ਨਿਰਾ)
		    curfrp = curfrp->fr_ਅਗਲਾ;
	}
	ਨਹੀਂਤਾਂ
	    curfrp = topframe;
	before = (flags & WSP_ਸਿਖਰ);
    }
    ਨਹੀਂਤਾਂ
    {
	curfrp = oldwin->w_ਚੌਖਟਾ;
	ਜੇ (flags & WSP_ਹੇਠਾਂ)
	    before = FALSE;
	ਨਹੀਂਤਾਂ ਜੇ (flags & WSP_ABOVE)
	    before = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (flags & WSP_VERT)
	    before = !p_spr;
	ਨਹੀਂਤਾਂ
	    before = !p_sb;
    }
    ਜੇ (curfrp->fr_ਮਾਪਾ == ਨਿਰਾ || curfrp->fr_ਮਾਪਾ->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(ਚੌਖਟਾ_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_ਮਾਪਾ = curfrp;
	frp->fr_ਅਗਲਾ = ਨਿਰਾ;
	frp->fr_prev = ਨਿਰਾ;
	curfrp->fr_ਬੱਚਾ = frp;
	curfrp->fr_win = ਨਿਰਾ;
	curfrp = frp;
	ਜੇ (frp->fr_win != ਨਿਰਾ)
	    oldwin->w_ਚੌਖਟਾ = frp;
	ਨਹੀਂਤਾਂ
	    FOR_ALL_ਚੌਖਟੇ(frp, frp->fr_ਬੱਚਾ)
		frp->fr_ਮਾਪਾ = curfrp;
    }

    ਜੇ (new_wp == ਨਿਰਾ)
	frp = wp->w_ਚੌਖਟਾ;
    ਨਹੀਂਤਾਂ
	frp = new_wp->w_ਚੌਖਟਾ;
    frp->fr_ਮਾਪਾ = curfrp->fr_ਮਾਪਾ;

    // Insert the new frame at the right place in the frame list.
    ਜੇ (before)
	ਚੌਖਟਾ_ਪਾਓ(curfrp, frp);
    ਨਹੀਂਤਾਂ
	ਚੌਖਟਾ_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    ਜੇ (!did_ਸੈੱਟ_fraction)
	ਸੈੱਟ_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    ਜੇ (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	ਜੇ (need_ਸਥਿਤੀ)
	{
	    win_new_ਉਚਾਈ(oldwin, oldwin->w_ਉਚਾਈ - need_ਸਥਿਤੀ);
	    oldwin->w_ਸਥਿਤੀ_ਉਚਾਈ = need_ਸਥਿਤੀ;
	}
	ਜੇ (flags & (WSP_ਸਿਖਰ | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    ਪੂਰਨ stl_ਉਚਾਈ = p_ls > 0 ? statusline_ਉਚਾਈ(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_ਉਚਾਈ();
	    win_new_ਉਚਾਈ(wp, curfrp->fr_ਉਚਾਈ - stl_ਉਚਾਈ
							  - WINBAR_ਉਚਾਈ(wp));
	    wp->w_ਸਥਿਤੀ_ਉਚਾਈ = stl_ਉਚਾਈ;
	}
	ਨਹੀਂਤਾਂ
	{
	    // height and row of new window is same as current window
	    wp->w_winrow = oldwin->w_winrow;
	    win_new_ਉਚਾਈ(wp, VISIBLE_ਉਚਾਈ(oldwin));
	    wp->w_ਸਥਿਤੀ_ਉਚਾਈ = oldwin->w_ਸਥਿਤੀ_ਉਚਾਈ;
	}
	frp->fr_ਉਚਾਈ = curfrp->fr_ਉਚਾਈ;

	// "new_size" of the current window goes to the new window, use
	// one column for the vertical separator
	win_new_ਚੌੜਾਈ(wp, new_ਮਾਪ);
	ਜੇ (before)
	    wp->w_vsep_ਚੌੜਾਈ = 1;
	ਨਹੀਂਤਾਂ
	{
	    wp->w_vsep_ਚੌੜਾਈ = oldwin->w_vsep_ਚੌੜਾਈ;
	    oldwin->w_vsep_ਚੌੜਾਈ = 1;
	}
	ਜੇ (flags & (WSP_ਸਿਖਰ | WSP_BOT))
	{
	    ਜੇ (flags & WSP_BOT)
		ਚੌਖਟਾ_ਸੈੱਟ_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    ਚੌਖਟਾ_new_ਚੌੜਾਈ(curfrp, curfrp->fr_ਚੌੜਾਈ
		     - (new_ਮਾਪ + ((flags & WSP_ਸਿਖਰ) != 0)), flags & WSP_ਸਿਖਰ,
								       FALSE);
	}
	ਨਹੀਂਤਾਂ
	    win_new_ਚੌੜਾਈ(oldwin, oldwin->w_ਚੌੜਾਈ - (new_ਮਾਪ + 1));
	ਜੇ (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_ਮਾਪ + 1;
	}
	ਨਹੀਂਤਾਂ		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_ਚੌੜਾਈ + 1;
	ਚੌਖਟਾ_fix_ਚੌੜਾਈ(oldwin);
	ਚੌਖਟਾ_fix_ਚੌੜਾਈ(wp);
    }
    ਨਹੀਂਤਾਂ
    {
	// width and column of new window is same as current window
	ਜੇ (flags & (WSP_ਸਿਖਰ | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_ਚੌੜਾਈ(wp, topframe->fr_ਚੌੜਾਈ);
	    wp->w_vsep_ਚੌੜਾਈ = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_ਚੌੜਾਈ(wp, oldwin->w_ਚੌੜਾਈ);
	    wp->w_vsep_ਚੌੜਾਈ = oldwin->w_vsep_ਚੌੜਾਈ;
	}
	frp->fr_ਚੌੜਾਈ = curfrp->fr_ਚੌੜਾਈ;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_ਉਚਾਈ(wp, new_ਮਾਪ);
	ਪੂਰਨ old_ਸਥਿਤੀ_ਉਚਾਈ = oldwin->w_ਸਥਿਤੀ_ਉਚਾਈ;
	ਜੇ (flags & (WSP_ਸਿਖਰ | WSP_BOT))
	{
	    ਪੂਰਨ new_fr_ਉਚਾਈ = curfrp->fr_ਉਚਾਈ - new_ਮਾਪ
							  + WINBAR_ਉਚਾਈ(wp);

	    ਜੇ (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_ਉਚਾਈ -= statusline_ਉਚਾਈ(curfrp->fr_win);
	    ਜੇ (flags & WSP_BOT)
		ਚੌਖਟਾ_ਜੋੜੋ_statusline(curfrp);
	    ਚੌਖਟਾ_new_ਉਚਾਈ(curfrp, new_fr_ਉਚਾਈ, flags & WSP_ਸਿਖਰ, FALSE,
									FALSE);
	}
	ਨਹੀਂਤਾਂ
	    win_new_ਉਚਾਈ(oldwin, oldwin_ਉਚਾਈ - (new_ਮਾਪ
			+ statusline_ਉਚਾਈ(wp)));
	ਜੇ (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_ਸਥਿਤੀ_ਉਚਾਈ = statusline_ਉਚਾਈ(wp);
	    oldwin->w_winrow += wp->w_ਉਚਾਈ + statusline_ਉਚਾਈ(wp);
	}
	ਨਹੀਂਤਾਂ		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_ਉਚਾਈ(oldwin)
						    + statusline_ਉਚਾਈ(oldwin);
	    wp->w_ਸਥਿਤੀ_ਉਚਾਈ = old_ਸਥਿਤੀ_ਉਚਾਈ;
	    ਜੇ (!(flags & WSP_BOT))
		oldwin->w_ਸਥਿਤੀ_ਉਚਾਈ = statusline_ਉਚਾਈ(oldwin);
	}
	ਚੌਖਟਾ_fix_ਉਚਾਈ(wp);
	ਚੌਖਟਾ_fix_ਉਚਾਈ(oldwin);
    }

    ਜੇ (flags & (WSP_ਸਿਖਰ | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    ਸਥਿਤੀ_redraw_all();

    ਜੇ (need_ਸਥਿਤੀ)
    {
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
	ਸੁਨੇਹਾ_ਖਾਨਾ = sc_ਖਾਨਾ;
	ਸੁਨੇਹਾ_clr_eos_force();	// Old command/ruler may still be there
	comp_ਖਾਨਾ();
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
	ਸੁਨੇਹਾ_ਖਾਨਾ = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    ਜੇ (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    ਨਹੀਂਤਾਂ ਜੇ (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    ਜੇ (flags & WSP_VERT)
    {
	i = p_wiw;
	ਜੇ (ਮਾਪ != 0)
	    p_wiw = ਮਾਪ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	ਜੇ (gui.ਵਿੱਚ_use)
	    gui_init_which_components(ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	i = p_wh;
	ਜੇ (ਮਾਪ != 0)
	    p_wh = ਮਾਪ;
    }

    /*
     * make the new window the current window
     */
    (ਖਾਲੀ)win_enter_ext(wp, (new_wp == ਨਿਰਾ ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    ਜੇ (flags & WSP_VERT)
	p_wiw = i;
    ਨਹੀਂਤਾਂ
	p_wh = i;
    retval = OK;

theend:
    ਜੇ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ਵਾਪਸ retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    ਸਥਿਰ ਖਾਲੀ
win_init(win_T *newp, win_T *oldp, ਪੂਰਨ flags ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		i;

    newp->w_ਬਫਰ = oldp->w_ਬਫਰ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    newp->w_s = &(oldp->w_ਬਫਰ->b_s);
#ਜੇ_ਅੰਤ
    oldp->w_ਬਫਰ->b_nwindows++;
    newp->w_cursor = oldp->w_cursor;
    newp->w_valid = 0;
    newp->w_curswant = oldp->w_curswant;
    newp->w_ਸੈੱਟ_curswant = oldp->w_ਸੈੱਟ_curswant;
    newp->w_topline = oldp->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    newp->w_topfill = oldp->w_topfill;
#ਜੇ_ਅੰਤ
    newp->w_leftcol = oldp->w_leftcol;
    newp->w_pcmark = oldp->w_pcmark;
    newp->w_prev_pcmark = oldp->w_prev_pcmark;
    newp->w_alt_fnum = oldp->w_alt_fnum;
    newp->w_wrow = oldp->w_wrow;
    newp->w_fraction = oldp->w_fraction;
    newp->w_prev_fraction_ਪੰਕਤੀ = oldp->w_prev_fraction_ਪੰਕਤੀ;

    // Not sure if this is needed, but be safe
    remove_highlight_overrides(newp->w_hl);
    VIM_CLEAR(newp->w_hl);

    copy_jumplist(oldp, newp);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    ਜੇ (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = ਨਿਰਾ;
	newp->w_llist_ref = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
	copy_loclist_ਢੇਰ(oldp, newp);
#ਜੇ_ਅੰਤ
    newp->w_localdir = (oldp->w_localdir == ਨਿਰਾ)
				    ? ਨਿਰਾ : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == ਨਿਰਾ)
				    ? ਨਿਰਾ : vim_strsave(oldp->w_prevdir);

    ਜੇ (*p_spk != 'c')
    {
	ਜੇ (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_ਉਚਾਈ = oldp->w_ਉਚਾਈ - WINBAR_ਉਚਾਈ(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_ਉਚਾਈ(oldp);
    }

    // copy tagstack and folds
    ਲਈ (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*ਟੈਗ = &newp->w_tagstack[i];
	*ਟੈਗ = oldp->w_tagstack[i];
	ਜੇ (ਟੈਗ->tagname != ਨਿਰਾ)
	    ਟੈਗ->tagname = vim_strsave(ਟੈਗ->tagname);
	ਜੇ (ਟੈਗ->user_ਅੰਕੜੇ != ਨਿਰਾ)
	    ਟੈਗ->user_ਅੰਕੜੇ = vim_strsave(ਟੈਗ->user_ਅੰਕੜੇ);
    }
    newp->w_tagstackidx = oldp->w_tagstackidx;
    newp->w_tagstacklen = oldp->w_tagstacklen;

    // Keep same changelist position in new window.
    newp->w_changelistidx = oldp->w_changelistidx;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    copyFoldingState(oldp, newp);
#ਜੇ_ਅੰਤ

    win_init_some(newp, oldp);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    {
	// Make sure to also handle highlight overrides copied over from oldp.
	bool pushed = ਧੱਕੋ_highlight_overrides(newp->w_hl, newp->w_hl_len);
	ਜੇ (newp->w_ਬਫਰ->b_ਪਦ != ਨਿਰਾ)
	    ਪਦ_init_default_colors(newp->w_ਬਫਰ->b_ਪਦ);
	ਜੇ (pushed)
	    ਕੱਢੋ_highlight_overrides();
    }
#ਜੇ_ਅੰਤ
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    ਸਥਿਰ ਖਾਲੀ
win_init_some(win_T *newp, win_T *oldp)
{
    // Use the same argument list.
    newp->w_alist = oldp->w_alist;
    ++newp->w_alist->al_refcount;
    newp->w_arg_idx = oldp->w_arg_idx;

    // copy options from existing window
    win_copy_options(oldp, newp);
}

/*
 * Return TRUE if "win" is a global popup or a popup in the current tab page.
 */
    ਪੂਰਨ
win_valid_popup(win_T *win ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	ਜੇ (wp == win)
	    ਵਾਪਸ TRUE;
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	ਜੇ (wp == win)
	    ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    ਪੂਰਨ
win_valid(win_T *win)
{
    win_T	*wp;

    ਜੇ (win == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp == win)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_ਲੱਭੋ_by_id(ਪੂਰਨ id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_id == id)
	    ਵਾਪਸ wp;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	ਜੇ (wp->w_id == id)
	    ਵਾਪਸ wp;
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	ਜੇ (wp->w_id == id)
	    ਵਾਪਸ wp;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    ਪੂਰਨ
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    ਜੇ (win == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp)
	{
	    ਜੇ (wp == win)
		ਵਾਪਸ TRUE;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(tp, wp)
	    ਜੇ (wp == win)
		ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ
    }
    ਵਾਪਸ win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    ਪੂਰਨ
win_ਗਿਣਤੀ(ਖਾਲੀ)
{
    win_T	*wp;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;

    FOR_ALL_WINDOWS(wp)
	++ਗਿਣਤੀ_2;
    ਵਾਪਸ ਗਿਣਤੀ_2;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    ਪੂਰਨ
make_windows(
    ਪੂਰਨ		ਗਿਣਤੀ_2,
    ਪੂਰਨ		vertical)  // split windows vertically if TRUE
{
    ਪੂਰਨ		maxcount;
    ਪੂਰਨ		todo;

    ਜੇ (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_ਚੌੜਾਈ + curwin->w_vsep_ਚੌੜਾਈ
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    ਨਹੀਂਤਾਂ
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_ਉਚਾਈ(curwin) + curwin->w_ਸਥਿਤੀ_ਉਚਾਈ
			- (p_wh - p_wmh)) / (p_wmh + statusline_ਉਚਾਈ(curwin));
    }

    ਜੇ (maxcount < 2)
	maxcount = 2;
    ਜੇ (ਗਿਣਤੀ_2 > maxcount)
	ਗਿਣਤੀ_2 = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    ਜੇ (ਗਿਣਤੀ_2 > 1)
	ਆਖਰੀ_ਸਥਿਤੀ(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    ਖੰਡ_autocmds();

    // todo is number of windows left to create
    ਲਈ (todo = ਗਿਣਤੀ_2 - 1; todo > 0; --todo)
	ਜੇ (vertical)
	{
	    ਜੇ (win_ਵੰਡੋ(curwin->w_ਚੌੜਾਈ - (curwin->w_ਚੌੜਾਈ - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (win_ਵੰਡੋ(curwin->w_ਉਚਾਈ - (curwin->w_ਉਚਾਈ - todo
			    * statusline_ਉਚਾਈ(curwin)) / (todo + 1)
			- statusline_ਉਚਾਈ(curwin), WSP_ABOVE) == FAIL)
		ਰੋਕੋ;
	}

    unblock_autocmds();

    // return actual number of windows
    ਵਾਪਸ (ਗਿਣਤੀ_2 - todo);
}

/*
 * Exchange current and next window
 */
    ਸਥਿਰ ਖਾਲੀ
win_exchange(ਲੰਮਾ Prenum)
{
    ਚੌਖਟਾ_T	*frp;
    ਚੌਖਟਾ_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    ਪੂਰਨ		ਆਰਜ਼ੀ;

    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;
    ਜੇ (ONE_ਖਿੜਕੀ)	    // just one window
    {
	beep_flush();
	ਵਾਪਸ;
    }
    ਜੇ (ਪਾਠ_or_buf_locked())
    {
	beep_flush();
	ਵਾਪਸ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    /*
     * find window to exchange with
     */
    ਜੇ (Prenum)
    {
	frp = curwin->w_ਚੌਖਟਾ->fr_ਮਾਪਾ->fr_ਬੱਚਾ;
	ਜਦੋਂ (frp != ਨਿਰਾ && --Prenum > 0)
	    frp = frp->fr_ਅਗਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_ਚੌਖਟਾ->fr_ਅਗਲਾ != ਨਿਰਾ)	// Swap with next
	frp = curwin->w_ਚੌਖਟਾ->fr_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ    // Swap last window in row/col with previous
	frp = curwin->w_ਚੌਖਟਾ->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    ਜੇ (frp == ਨਿਰਾ || frp->fr_win == ਨਿਰਾ || frp->fr_win == curwin)
	ਵਾਪਸ;
    wp = frp->fr_win;

/*
 * 1. remove curwin from the list. Remember after which window it was in wp2
 * 2. insert curwin before wp in the list
 * if wp != wp2
 *    3. remove wp from the list
 *    4. insert wp after wp2
 * 5. exchange the status line height and vsep width.
 */
    wp2 = curwin->w_prev;
    frp2 = curwin->w_ਚੌਖਟਾ->fr_prev;
    ਜੇ (wp->w_prev != curwin)
    {
	win_remove(curwin, ਨਿਰਾ);
	ਚੌਖਟਾ_remove(curwin->w_ਚੌਖਟਾ);
	win_append(wp->w_prev, curwin);
	ਚੌਖਟਾ_ਪਾਓ(frp, curwin->w_ਚੌਖਟਾ);
    }
    ਜੇ (wp != wp2)
    {
	win_remove(wp, ਨਿਰਾ);
	ਚੌਖਟਾ_remove(wp->w_ਚੌਖਟਾ);
	win_append(wp2, wp);
	ਜੇ (frp2 == ਨਿਰਾ)
	    ਚੌਖਟਾ_ਪਾਓ(wp->w_ਚੌਖਟਾ->fr_ਮਾਪਾ->fr_ਬੱਚਾ, wp->w_ਚੌਖਟਾ);
	ਨਹੀਂਤਾਂ
	    ਚੌਖਟਾ_append(frp2, wp->w_ਚੌਖਟਾ);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    ਆਰਜ਼ੀ = curwin->w_ਸਥਿਤੀ_ਉਚਾਈ;
    ਜੇ (ਆਰਜ਼ੀ != wp->w_ਸਥਿਤੀ_ਉਚਾਈ)
    {
	win_new_ਉਚਾਈ(curwin, curwin->w_ਉਚਾਈ + ਆਰਜ਼ੀ - wp->w_ਸਥਿਤੀ_ਉਚਾਈ);
	win_new_ਉਚਾਈ(wp, wp->w_ਉਚਾਈ + wp->w_ਸਥਿਤੀ_ਉਚਾਈ - ਆਰਜ਼ੀ);
    }
    curwin->w_ਸਥਿਤੀ_ਉਚਾਈ = wp->w_ਸਥਿਤੀ_ਉਚਾਈ;
    wp->w_ਸਥਿਤੀ_ਉਚਾਈ = ਆਰਜ਼ੀ;
    ਆਰਜ਼ੀ = curwin->w_vsep_ਚੌੜਾਈ;
    curwin->w_vsep_ਚੌੜਾਈ = wp->w_vsep_ਚੌੜਾਈ;
    wp->w_vsep_ਚੌੜਾਈ = ਆਰਜ਼ੀ;

    ਚੌਖਟਾ_fix_ਉਚਾਈ(curwin);
    ਚੌਖਟਾ_fix_ਉਚਾਈ(wp);
    ਚੌਖਟਾ_fix_ਚੌੜਾਈ(curwin);
    ਚੌਖਟਾ_fix_ਚੌੜਾਈ(wp);

    win_comp_pos();		// recompute window positions
#ਜੇ defined(FEAT_STL_OPT)
    ਚੌਖਟਾ_change_statusline_ਉਚਾਈ();
#ਜੇ_ਅੰਤ

    ਜੇ (wp->w_ਬਫਰ != curbuf)
	reset_VIsual_and_resel();
    ਨਹੀਂਤਾਂ ਜੇ (VIsual_ਸਰਗਰਮ)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    ਸਥਿਰ ਖਾਲੀ
win_rotate(ਪੂਰਨ upwards, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    win_T	*wp1;
    win_T	*wp2;
    ਚੌਖਟਾ_T	*frp;
    ਪੂਰਨ		n;

    ਜੇ (ONE_ਖਿੜਕੀ)		// nothing to do
    {
	beep_flush();
	ਵਾਪਸ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    // Check if all frames in this row/col have one window.
    FOR_ALL_ਚੌਖਟੇ(frp, curwin->w_ਚੌਖਟਾ->fr_ਮਾਪਾ->fr_ਬੱਚਾ)
	ਜੇ (frp->fr_win == ਨਿਰਾ)
	{
	    emsg(_(e_cannot_rotate_when_another_ਖਿੜਕੀ_is_ਵੰਡੋ));
	    ਵਾਪਸ;
	}

    ਜਦੋਂ (ਗਿਣਤੀ_2--)
    {
	ਜੇ (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_ਚੌਖਟਾ->fr_ਮਾਪਾ->fr_ਬੱਚਾ;
	    wp1 = frp->fr_win;
	    win_remove(wp1, ਨਿਰਾ);
	    ਚੌਖਟਾ_remove(frp);

	    // find last frame and append removed window/frame after it
	    ਲਈ ( ; frp->fr_ਅਗਲਾ != ਨਿਰਾ; frp = frp->fr_ਅਗਲਾ)
		;
	    win_append(frp->fr_win, wp1);
	    ਚੌਖਟਾ_append(frp, wp1->w_ਚੌਖਟਾ);

	    wp2 = frp->fr_win;		// previously last window
	}
	ਨਹੀਂਤਾਂ			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    ਲਈ (frp = curwin->w_ਚੌਖਟਾ; frp->fr_ਅਗਲਾ != ਨਿਰਾ;
							   frp = frp->fr_ਅਗਲਾ)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, ਨਿਰਾ);
	    ਚੌਖਟਾ_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_ਮਾਪਾ->fr_ਬੱਚਾ->fr_win->w_prev, wp1);
	    ਚੌਖਟਾ_ਪਾਓ(frp->fr_ਮਾਪਾ->fr_ਬੱਚਾ, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_ਸਥਿਤੀ_ਉਚਾਈ;
	ਜੇ (n != wp1->w_ਸਥਿਤੀ_ਉਚਾਈ)
	{
	    win_new_ਉਚਾਈ(wp2, wp2->w_ਉਚਾਈ + n - wp1->w_ਸਥਿਤੀ_ਉਚਾਈ);
	    win_new_ਉਚਾਈ(wp1, wp1->w_ਉਚਾਈ + wp1->w_ਸਥਿਤੀ_ਉਚਾਈ - n);
	}
	wp2->w_ਸਥਿਤੀ_ਉਚਾਈ = wp1->w_ਸਥਿਤੀ_ਉਚਾਈ;
	wp1->w_ਸਥਿਤੀ_ਉਚਾਈ = n;
	ਚੌਖਟਾ_fix_ਉਚਾਈ(wp1);
	ਚੌਖਟਾ_fix_ਉਚਾਈ(wp2);
	n = wp2->w_vsep_ਚੌੜਾਈ;
	wp2->w_vsep_ਚੌੜਾਈ = wp1->w_vsep_ਚੌੜਾਈ;
	wp1->w_vsep_ਚੌੜਾਈ = n;
	ਚੌਖਟਾ_fix_ਚੌੜਾਈ(wp1);
	ਚੌਖਟਾ_fix_ਚੌੜਾਈ(wp2);

	// recompute w_winrow and w_wincol for all windows
	win_comp_pos();
    }
#ਜੇ defined(FEAT_STL_OPT)
    ਚੌਖਟਾ_change_statusline_ਉਚਾਈ();
#ਜੇ_ਅੰਤ

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Move "wp" into a new split in a given direction, possibly relative to the
 * current window.
 * "wp" must be valid in the current tabpage.
 * Returns FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
win_splitmove(win_T *wp, ਪੂਰਨ ਮਾਪ, ਪੂਰਨ flags)
{
    ਪੂਰਨ		dir;
    ਪੂਰਨ		ਉਚਾਈ = wp->w_ਉਚਾਈ;
    ਚੌਖਟਾ_T	*unflat_altfr;

    ਜੇ (ONE_ਖਿੜਕੀ)
	ਵਾਪਸ OK;	// nothing to do
    ਜੇ (check_ਵੰਡੋ_disallowed(wp) == FAIL)
	ਵਾਪਸ FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, ਨਿਰਾ, &unflat_altfr);
    win_remove(wp, ਨਿਰਾ);
    ਆਖਰੀ_ਸਥਿਤੀ(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    ਜੇ (win_ਵੰਡੋ_ins(ਮਾਪ, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	ਵਾਪਸ FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    ਜੇ (ਮਾਪ == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(ਉਚਾਈ, wp);
	ਜੇ (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#ਜੇ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#ਜੇ_ਅੰਤ
    ਵਾਪਸ OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    ਖਾਲੀ
win_move_after(win_T *win1, win_T *win2)
{
    ਪੂਰਨ		ਉਚਾਈ;

    // check if the arguments are reasonable
    ਜੇ (win1 == win2)
	ਵਾਪਸ;

    // check if there is something to do
    ਜੇ (win2->w_ਅਗਲਾ != win1)
    {
	ਜੇ (win1->w_ਚੌਖਟਾ->fr_ਮਾਪਾ != win2->w_ਚੌਖਟਾ->fr_ਮਾਪਾ)
	{
	    iemsg("Trying to move a window into another frame");
	    ਵਾਪਸ;
	}

	// may need to move the status line/vertical separator of the last
	//ਖਿੜਕੀ
	ਜੇ (win1 == lastwin)
	{
	    ਉਚਾਈ = win1->w_prev->w_ਸਥਿਤੀ_ਉਚਾਈ;
	    win1->w_prev->w_ਸਥਿਤੀ_ਉਚਾਈ = win1->w_ਸਥਿਤੀ_ਉਚਾਈ;
	    win1->w_ਸਥਿਤੀ_ਉਚਾਈ = ਉਚਾਈ;
	    ਜੇ (win1->w_prev->w_vsep_ਚੌੜਾਈ == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_ਚੌੜਾਈ = 0;
		win1->w_prev->w_ਚੌਖਟਾ->fr_ਚੌੜਾਈ -= 1;
		win1->w_vsep_ਚੌੜਾਈ = 1;
		win1->w_ਚੌਖਟਾ->fr_ਚੌੜਾਈ += 1;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (win2 == lastwin)
	{
	    ਉਚਾਈ = win1->w_ਸਥਿਤੀ_ਉਚਾਈ;
	    win1->w_ਸਥਿਤੀ_ਉਚਾਈ = win2->w_ਸਥਿਤੀ_ਉਚਾਈ;
	    win2->w_ਸਥਿਤੀ_ਉਚਾਈ = ਉਚਾਈ;
	    ਜੇ (win1->w_vsep_ਚੌੜਾਈ == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_ਚੌੜਾਈ = 1;
		win2->w_ਚੌਖਟਾ->fr_ਚੌੜਾਈ += 1;
		win1->w_vsep_ਚੌੜਾਈ = 0;
		win1->w_ਚੌਖਟਾ->fr_ਚੌੜਾਈ -= 1;
	    }
	}
	win_remove(win1, ਨਿਰਾ);
	ਚੌਖਟਾ_remove(win1->w_ਚੌਖਟਾ);
	win_append(win2, win1);
	ਚੌਖਟਾ_append(win2->w_ਚੌਖਟਾ, win1->w_ਚੌਖਟਾ);

	win_comp_pos();	// recompute w_winrow for all windows
	redraw_later(UPD_NOT_VALID);
    }
    win_enter(win1, FALSE);
}

/*
 * Make all windows the same height.
 * 'next_curwin' will soon be the current window, make sure it has enough
 * rows.
 */
    ਖਾਲੀ
win_equal(
    win_T	*ਅਗਲਾ_curwin,	// pointer to current window to be or NULL
    ਪੂਰਨ		current,	// do only frame with current window
    ਪੂਰਨ		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    ਜੇ (dir == 0)
	dir = *p_ead;
    win_equal_rec(ਅਗਲਾ_curwin == ਨਿਰਾ ? curwin : ਅਗਲਾ_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_ਉਚਾਈ(),
		      topframe->fr_ਚੌੜਾਈ, topframe->fr_ਉਚਾਈ);
#ਜੇ defined(FEAT_STL_OPT)
    ਚੌਖਟਾ_change_statusline_ਉਚਾਈ();
#ਜੇ_ਅੰਤ
    ਜੇ (!is_aucmd_win(ਅਗਲਾ_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    ਸਥਿਰ ਖਾਲੀ
win_equal_rec(
    win_T	*ਅਗਲਾ_curwin,	// pointer to current window to be or NULL
    ਪੂਰਨ		current,	// do only frame with current window
    ਚੌਖਟਾ_T	*topfr,		// frame to set size off
    ਪੂਰਨ		dir,		// 'v', 'h' or 'b', see win_equal()
    ਪੂਰਨ		ਖਾਨਾ,		// horizontal position for frame
    ਪੂਰਨ		ਪੰਕਤੀ,		// vertical position for frame
    ਪੂਰਨ		ਚੌੜਾਈ,		// new width of frame
    ਪੂਰਨ		ਉਚਾਈ)		// new height of frame
{
    ਪੂਰਨ		n, m;
    ਪੂਰਨ		extra_sep = 0;
    ਪੂਰਨ		wincount, totwincount = 0;
    ਚੌਖਟਾ_T	*fr;
    ਪੂਰਨ		ਅਗਲਾ_curwin_ਮਾਪ = 0;
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼ = 0;
    ਪੂਰਨ		new_ਮਾਪ;
    ਪੂਰਨ		has_ਅਗਲਾ_curwin = 0;
    ਪੂਰਨ		hnc;

    ਜੇ (topfr->fr_layout == FR_ਪੱਤੀ)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	ਜੇ (topfr->fr_ਉਚਾਈ != ਉਚਾਈ || topfr->fr_win->w_winrow != ਪੰਕਤੀ
		|| topfr->fr_ਚੌੜਾਈ != ਚੌੜਾਈ || topfr->fr_win->w_wincol != ਖਾਨਾ
	   )
	{
	    topfr->fr_win->w_winrow = ਪੰਕਤੀ;
	    ਚੌਖਟਾ_new_ਉਚਾਈ(topfr, ਉਚਾਈ, FALSE, FALSE, FALSE);
	    topfr->fr_win->w_wincol = ਖਾਨਾ;
	    ਚੌਖਟਾ_new_ਚੌੜਾਈ(topfr, ਚੌੜਾਈ, FALSE, FALSE);
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (topfr->fr_layout == FR_ਪੰਕਤੀ)
    {
	topfr->fr_ਚੌੜਾਈ = ਚੌੜਾਈ;
	topfr->fr_ਉਚਾਈ = ਉਚਾਈ;

	ਜੇ (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = ਚੌਖਟਾ_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    ਜੇ (ਖਾਨਾ + ਚੌੜਾਈ >= firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ)
		extra_sep = 1;
	    ਨਹੀਂਤਾਂ
		extra_sep = 0;
	    totwincount = (n + extra_sep) / (p_wmw + 1);
	    has_ਅਗਲਾ_curwin = ਚੌਖਟਾ_has_win(topfr, ਅਗਲਾ_curwin);

	    /*
	     * Compute width for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal width when counting p_wiw for "next_curwin".
	     */
	    m = ਚੌਖਟਾ_minwidth(topfr, ਅਗਲਾ_curwin);
	    ਗੁੰਜਾਇਸ਼ = ਚੌੜਾਈ - m;
	    ਜੇ (ਗੁੰਜਾਇਸ਼ < 0)
	    {
		ਅਗਲਾ_curwin_ਮਾਪ = p_wiw + ਗੁੰਜਾਇਸ਼;
		ਗੁੰਜਾਇਸ਼ = 0;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਅਗਲਾ_curwin_ਮਾਪ = -1;
		FOR_ALL_ਚੌਖਟੇ(fr, topfr->fr_ਬੱਚਾ)
		{
		    ਜੇ (!ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(fr))
			ਜਾਰੀ;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = ਚੌਖਟਾ_minwidth(fr, NOWIN);
		    new_ਮਾਪ = fr->fr_ਚੌੜਾਈ;
		    ਜੇ (ਚੌਖਟਾ_has_win(fr, ਅਗਲਾ_curwin))
		    {
			ਗੁੰਜਾਇਸ਼ += p_wiw - p_wmw;
			ਅਗਲਾ_curwin_ਮਾਪ = 0;
			ਜੇ (new_ਮਾਪ < p_wiw)
			    new_ਮਾਪ = p_wiw;
		    }
		    ਨਹੀਂਤਾਂ
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_ਅਗਲਾ == ਨਿਰਾ
					? extra_sep : 0)) / (p_wmw + 1);
		    ਗੁੰਜਾਇਸ਼ -= new_ਮਾਪ - n;
		    ਜੇ (ਗੁੰਜਾਇਸ਼ < 0)
		    {
			new_ਮਾਪ += ਗੁੰਜਾਇਸ਼;
			ਗੁੰਜਾਇਸ਼ = 0;
		    }
		    fr->fr_newwidth = new_ਮਾਪ;
		}
		ਜੇ (ਅਗਲਾ_curwin_ਮਾਪ == -1)
		{
		    ਜੇ (!has_ਅਗਲਾ_curwin)
			ਅਗਲਾ_curwin_ਮਾਪ = 0;
		    ਨਹੀਂਤਾਂ ਜੇ (totwincount > 1
			    && (ਗੁੰਜਾਇਸ਼ + (totwincount - 2))
						  / (totwincount - 1) > p_wiw)
		    {
			// Can make all windows wider than 'winwidth', spread
			// the room equally.
			ਅਗਲਾ_curwin_ਮਾਪ = (ਗੁੰਜਾਇਸ਼ + p_wiw
					    + (totwincount - 1) * p_wmw
					    + (totwincount - 1)) / totwincount;
			ਗੁੰਜਾਇਸ਼ -= ਅਗਲਾ_curwin_ਮਾਪ - p_wiw;
		    }
		    ਨਹੀਂਤਾਂ
			ਅਗਲਾ_curwin_ਮਾਪ = p_wiw;
		}
	    }

	    ਜੇ (has_ਅਗਲਾ_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_ਚੌਖਟੇ(fr, topfr->fr_ਬੱਚਾ)
	{
	    wincount = 1;
	    ਜੇ (fr->fr_ਅਗਲਾ == ਨਿਰਾ)
		// last frame gets all that remains (avoid roundoff error)
		new_ਮਾਪ = ਚੌੜਾਈ;
	    ਨਹੀਂਤਾਂ ਜੇ (dir == 'v')
		new_ਮਾਪ = fr->fr_ਚੌੜਾਈ;
	    ਨਹੀਂਤਾਂ ਜੇ (ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(fr))
	    {
		new_ਮਾਪ = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = ਚੌਖਟਾ_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_ਅਗਲਾ == ਨਿਰਾ ? extra_sep : 0))
								/ (p_wmw + 1);
		m = ਚੌਖਟਾ_minwidth(fr, ਅਗਲਾ_curwin);
		ਜੇ (has_ਅਗਲਾ_curwin)
		    hnc = ਚੌਖਟਾ_has_win(fr, ਅਗਲਾ_curwin);
		ਨਹੀਂਤਾਂ
		    hnc = FALSE;
		ਜੇ (hnc)	    // don't count next_curwin
		    --wincount;
		ਜੇ (totwincount == 0)
		    new_ਮਾਪ = ਗੁੰਜਾਇਸ਼;
		ਨਹੀਂਤਾਂ
		    new_ਮਾਪ = (wincount * ਗੁੰਜਾਇਸ਼ + ((ਬਿਨਾਨਿਸ਼ਾਨੀ)totwincount >> 1))
								/ totwincount;
		ਜੇ (hnc)	    // add next_curwin size
		{
		    ਅਗਲਾ_curwin_ਮਾਪ -= p_wiw - (m - n);
		    ਜੇ (ਅਗਲਾ_curwin_ਮਾਪ < 0)
			ਅਗਲਾ_curwin_ਮਾਪ = 0;
		    new_ਮਾਪ += ਅਗਲਾ_curwin_ਮਾਪ;
		    ਗੁੰਜਾਇਸ਼ -= new_ਮਾਪ - ਅਗਲਾ_curwin_ਮਾਪ;
		}
		ਨਹੀਂਤਾਂ
		    ਗੁੰਜਾਇਸ਼ -= new_ਮਾਪ;
		new_ਮਾਪ += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    ਜੇ (!current || dir != 'v' || topfr->fr_ਮਾਪਾ != ਨਿਰਾ
		    || (new_ਮਾਪ != fr->fr_ਚੌੜਾਈ)
		    || ਚੌਖਟਾ_has_win(fr, ਅਗਲਾ_curwin))
		win_equal_rec(ਅਗਲਾ_curwin, current, fr, dir, ਖਾਨਾ, ਪੰਕਤੀ,
							    new_ਮਾਪ, ਉਚਾਈ);
	    ਖਾਨਾ += new_ਮਾਪ;
	    ਚੌੜਾਈ -= new_ਮਾਪ;
	    totwincount -= wincount;
	}
    }
    ਨਹੀਂਤਾਂ // topfr->fr_layout == FR_COL
    {
	topfr->fr_ਚੌੜਾਈ = ਚੌੜਾਈ;
	topfr->fr_ਉਚਾਈ = ਉਚਾਈ;

	ਜੇ (dir != 'h')			// equalize frame heights
	{
	    totwincount = ਚੌਖਟਾ_wincount_ਵਿੱਚ_ਉਚਾਈ(topfr);
	    has_ਅਗਲਾ_curwin = ਚੌਖਟਾ_has_win(topfr, ਅਗਲਾ_curwin);

	    /*
	     * Compute height for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal height when counting p_wh for "next_curwin".
	     */
	    m = ਚੌਖਟਾ_minheight(topfr, ਅਗਲਾ_curwin);
	    ਗੁੰਜਾਇਸ਼ = ਉਚਾਈ - m;
	    ਜੇ (ਗੁੰਜਾਇਸ਼ < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		ਅਗਲਾ_curwin_ਮਾਪ = p_wh + ਗੁੰਜਾਇਸ਼;
		ਗੁੰਜਾਇਸ਼ = 0;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਅਗਲਾ_curwin_ਮਾਪ = -1;
		FOR_ALL_ਚੌਖਟੇ(fr, topfr->fr_ਬੱਚਾ)
		{
		    ਜੇ (!ਚੌਖਟਾ_fixed_ਉਚਾਈ(fr))
			ਜਾਰੀ;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = ਚੌਖਟਾ_minheight(fr, NOWIN);
		    new_ਮਾਪ = fr->fr_ਉਚਾਈ;
		    ਜੇ (ਚੌਖਟਾ_has_win(fr, ਅਗਲਾ_curwin))
		    {
			ਗੁੰਜਾਇਸ਼ += p_wh - p_wmh;
			ਅਗਲਾ_curwin_ਮਾਪ = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			ਜੇ (new_ਮਾਪ < n + (p_wh - p_wmh))
			    new_ਮਾਪ = n + (p_wh - p_wmh);
		    }
		    ਨਹੀਂਤਾਂ
			// These windows don't use up room.
			totwincount -= ਚੌਖਟਾ_wincount_ਵਿੱਚ_ਉਚਾਈ(fr);
		    ਗੁੰਜਾਇਸ਼ -= new_ਮਾਪ - n;
		    ਜੇ (ਗੁੰਜਾਇਸ਼ < 0)
		    {
			new_ਮਾਪ += ਗੁੰਜਾਇਸ਼;
			ਗੁੰਜਾਇਸ਼ = 0;
		    }
		    fr->fr_newheight = new_ਮਾਪ;
		}
		ਜੇ (ਅਗਲਾ_curwin_ਮਾਪ == -1)
		{
		    ਜੇ (!has_ਅਗਲਾ_curwin)
			ਅਗਲਾ_curwin_ਮਾਪ = 0;
		    ਨਹੀਂਤਾਂ ਜੇ (totwincount > 1
			    && (ਗੁੰਜਾਇਸ਼ + (totwincount - 2))
						   / (totwincount - 1) > p_wh)
		    {
			// can make all windows higher than 'winheight',
			// spread the room equally.
			ਅਗਲਾ_curwin_ਮਾਪ = (ਗੁੰਜਾਇਸ਼ + p_wh
					   + (totwincount - 1) * p_wmh
					   + (totwincount - 1)) / totwincount;
			ਗੁੰਜਾਇਸ਼ -= ਅਗਲਾ_curwin_ਮਾਪ - p_wh;
		    }
		    ਨਹੀਂਤਾਂ
			ਅਗਲਾ_curwin_ਮਾਪ = p_wh;
		}
	    }

	    ਜੇ (has_ਅਗਲਾ_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_ਚੌਖਟੇ(fr, topfr->fr_ਬੱਚਾ)
	{
	    wincount = 1;
	    ਜੇ (fr->fr_ਅਗਲਾ == ਨਿਰਾ)
		// last frame gets all that remains (avoid roundoff error)
		new_ਮਾਪ = ਉਚਾਈ;
	    ਨਹੀਂਤਾਂ ਜੇ (dir == 'h')
		new_ਮਾਪ = fr->fr_ਉਚਾਈ;
	    ਨਹੀਂਤਾਂ ਜੇ (ਚੌਖਟਾ_fixed_ਉਚਾਈ(fr))
	    {
		new_ਮਾਪ = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = ਚੌਖਟਾ_minheight(fr, NOWIN);
		wincount = ਚੌਖਟਾ_wincount_ਵਿੱਚ_ਉਚਾਈ(fr);
		m = ਚੌਖਟਾ_minheight(fr, ਅਗਲਾ_curwin);
		ਜੇ (has_ਅਗਲਾ_curwin)
		    hnc = ਚੌਖਟਾ_has_win(fr, ਅਗਲਾ_curwin);
		ਨਹੀਂਤਾਂ
		    hnc = FALSE;
		ਜੇ (hnc)	    // don't count next_curwin
		    --wincount;
		ਜੇ (totwincount == 0)
		    new_ਮਾਪ = ਗੁੰਜਾਇਸ਼;
		ਨਹੀਂਤਾਂ
		    new_ਮਾਪ = (wincount * ਗੁੰਜਾਇਸ਼ + ((ਬਿਨਾਨਿਸ਼ਾਨੀ)totwincount >> 1))
								/ totwincount;
		ਜੇ (hnc)	    // add next_curwin size
		{
		    ਅਗਲਾ_curwin_ਮਾਪ -= p_wh - (m - n);
		    new_ਮਾਪ += ਅਗਲਾ_curwin_ਮਾਪ;
		    ਗੁੰਜਾਇਸ਼ -= new_ਮਾਪ - ਅਗਲਾ_curwin_ਮਾਪ;
		}
		ਨਹੀਂਤਾਂ
		    ਗੁੰਜਾਇਸ਼ -= new_ਮਾਪ;
		new_ਮਾਪ += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    ਜੇ (!current || dir != 'h' || topfr->fr_ਮਾਪਾ != ਨਿਰਾ
		    || (new_ਮਾਪ != fr->fr_ਉਚਾਈ)
		    || ਚੌਖਟਾ_has_win(fr, ਅਗਲਾ_curwin))
		win_equal_rec(ਅਗਲਾ_curwin, current, fr, dir, ਖਾਨਾ, ਪੰਕਤੀ,
							     ਚੌੜਾਈ, new_ਮਾਪ);
	    ਪੰਕਤੀ += new_ਮਾਪ;
	    ਉਚਾਈ -= new_ਮਾਪ;
	    totwincount -= wincount;
	}
    }
}

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
    ਖਾਲੀ
leaving_ਖਿੜਕੀ(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    ਜੇ (!bt_prompt(win->w_ਬਫਰ) || is_aucmd_win(win))
	ਵਾਪਸ;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_ਬਫਰ->b_prompt_ਪਾਓ = restart_edit;
    ਜੇ (restart_edit != 0 && ਬਹੁਲਕ_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) && !stop_ਪਾਓ_ਬਹੁਲਕ)
    {
	stop_ਪਾਓ_ਬਹੁਲਕ = TRUE;
	ਜੇ (win->w_ਬਫਰ->b_prompt_ਪਾਓ == NUL)
	    win->w_ਬਫਰ->b_prompt_ਪਾਓ = 'A';
    }
}

    ਖਾਲੀ
entering_ਖਿੜਕੀ(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    ਜੇ (!bt_prompt(win->w_ਬਫਰ) || is_aucmd_win(win))
	ਵਾਪਸ;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    ਜੇ (win->w_ਬਫਰ->b_prompt_ਪਾਓ != NUL)
	stop_ਪਾਓ_ਬਹੁਲਕ = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) == 0)
	restart_edit = win->w_ਬਫਰ->b_prompt_ਪਾਓ;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
win_init_ਖਾਲੀ_ਥਾਵਾਂ(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_ਸਤਰਾਂ_valid = 0;
    wp->w_cursor.lnum = 1;
    wp->w_curswant = wp->w_cursor.ਖਾਨਾ = 0;
    wp->w_cursor.coladd = 0;
    wp->w_pcmark.lnum = 1;	// pcmark not cleared but set to line 1
    wp->w_pcmark.ਖਾਨਾ = 0;
    wp->w_prev_pcmark.lnum = 0;
    wp->w_prev_pcmark.ਖਾਨਾ = 0;
    wp->w_topline = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    wp->w_topfill = 0;
#ਜੇ_ਅੰਤ
    wp->w_botline = 2;
    wp->w_valid = 0;
#ਜੇ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    wp->w_s = &wp->w_ਬਫਰ->b_s;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਪਦ_reset_hlfwin(wp);
#ਜੇ_ਅੰਤ
}

/*
 * Init the current window "curwin".
 * Called when a new file is being edited.
 */
    ਖਾਲੀ
curwin_init(ਖਾਲੀ)
{
    win_init_ਖਾਲੀ_ਥਾਵਾਂ(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    ਖਾਲੀ
close_windows(
    buf_T	*buf,
    ਪੂਰਨ		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = tabpage_ਸੂਚਕਾਂਕ(ਨਿਰਾ);

    ++RedrawingDisabled;

    ਲਈ (wp = firstwin; wp != ਨਿਰਾ && !ONE_ਖਿੜਕੀ; )
    {
	ਜੇ (wp->w_ਬਫਰ == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_ਬਫਰ->b_locked > 0))
	{
	    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_ਮਾਪ))
		ਜਾਓ theend;  // Only give one error message.
	    ਜੇ (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		ਰੋਕੋ;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	ਨਹੀਂਤਾਂ
	    wp = wp->w_ਅਗਲਾ;
    }

    // Also check windows in other tab pages.
    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ; tp = nexttp)
    {
	nexttp = tp->tp_ਅਗਲਾ;
	ਜੇ (tp != curtab)
	    FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp)
		ਜੇ (wp->w_ਬਫਰ == buf
		    && !(win_locked(wp) || wp->w_ਬਫਰ->b_locked > 0))
		{
		    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_ਮਾਪ))
			ਜਾਓ theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = ਪਹਿਲਾ_tabpage;
		    ਰੋਕੋ;
		}
    }

theend:
    ਜੇ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    ਜੇ (ਗਿਣਤੀ_2 != tabpage_ਸੂਚਕਾਂਕ(ਨਿਰਾ))
	apply_autocmds(EVENT_TABCLOSED, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    ਪੂਰਨ
ਆਖਰੀ_ਖਿੜਕੀ(ਖਾਲੀ)
{
    ਵਾਪਸ (one_ਖਿੜਕੀ() && ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ == ਨਿਰਾ);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    ਪੂਰਨ
one_ਖਿੜਕੀ(ਖਾਲੀ)
{
    win_T	*wp;
    ਪੂਰਨ		ਵੇਖਿਆ_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ (!is_aucmd_win(wp))
	{
	    ਜੇ (ਵੇਖਿਆ_one)
		ਵਾਪਸ FALSE;
	    ਵੇਖਿਆ_one = TRUE;
	}
    }
    ਵਾਪਸ TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
close_ਆਖਰੀ_ਖਿੜਕੀ_tabpage(
    win_T	*win,
    ਪੂਰਨ		free_buf,
    tabpage_T   *prev_curtab)
{
    ਜੇ (!ONE_ਖਿੜਕੀ)
	ਵਾਪਸ FALSE;

    buf_T	*old_curbuf = curbuf;
    tabpage_T	*save_lastused = lastused_tabpage;

    /*
     * Closing the last window in a tab page.  First go to another tab
     * page and then close the window and the tab page.  This avoids that
     * curwin and curtab are invalid while we are freeing memory, they may
     * be used in GUI events.
     * Don't trigger *Enter autocommands yet, they may use wrong values, so do
     * that below.
     * Do trigger *Leave autocommands, unless win->w_buffer is NULL, in which
     * case they have already been triggered.
     */
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_ਬਫਰ != ਨਿਰਾ);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    ਜੇ (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    ਜੇ (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    entering_ਖਿੜਕੀ(curwin);
#ਜੇ_ਅੰਤ
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
#ਜੇ defined(FEAT_TABPANEL)
    ਜੇ (p_stpl > 0)
	shell_new_columns();
#ਜੇ_ਅੰਤ
    apply_autocmds(EVENT_WINENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    ਜੇ (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    ਵਾਪਸ TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    ਸਥਿਰ ਖਾਲੀ
win_close_ਬਫਰ(win_T *win, ਪੂਰਨ action, ਪੂਰਨ abort_if_ਆਖਰੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    ਜੇ (win->w_ਬਫਰ != ਨਿਰਾ)
	reset_synblock(win);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    ਜੇ (win->w_ਬਫਰ != ਨਿਰਾ && bt_quickfix(win->w_ਬਫਰ)
					&& win->w_ਬਫਰ->b_nwindows == 1)
	win->w_ਬਫਰ->b_p_bl = FALSE;
#ਜੇ_ਅੰਤ

    // Close the link to the buffer.
    ਜੇ (win->w_ਬਫਰ != ਨਿਰਾ)
    {
	bufref_T    bufref;

	ਸੈੱਟ_bufref(&bufref, curbuf);
	++win->w_locked;
	close_ਬਫਰ(win, win->w_ਬਫਰ, action, abort_if_ਆਖਰੀ, TRUE, TRUE);
	ਜੇ (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	ਜੇ (!bufref_valid(&bufref))
	    curbuf = firstbuf;
    }
}

/*
 * Close window "win".  Only works for the current tab page.
 * If "free_buf" is TRUE related buffer may be unloaded.
 *
 * Called by :quit, :close, :xit, :wq and findtag().
 * Returns FAIL when the window was not closed.
 */
    ਪੂਰਨ
win_close(win_T *win, ਪੂਰਨ free_buf)
{
    win_T	*wp;
    ਪੂਰਨ		other_ਬਫਰ = FALSE;
    ਪੂਰਨ		close_curwin = FALSE;
    ਪੂਰਨ		dir;
    ਪੂਰਨ		help_ਖਿੜਕੀ = FALSE;
    ਪੂਰਨ		quickfix_ਖਿੜਕੀ = FALSE;
    tabpage_T   *prev_curtab = curtab;
    ਚੌਖਟਾ_T	*win_ਚੌਖਟਾ = win->w_ਚੌਖਟਾ->fr_ਮਾਪਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		had_diffmode = win->w_p_diff;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
    ਪੂਰਨ		did_decrement = FALSE;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    ਜੇ (may_close_ਪਦ_popup() == OK)
	ਵਾਪਸ OK;
#ਜੇ_ਅੰਤ
    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ FAIL;

    ਜੇ (ਆਖਰੀ_ਖਿੜਕੀ())
    {
	emsg(_(e_cannot_close_ਆਖਰੀ_ਖਿੜਕੀ));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_close))
	ਵਾਪਸ FAIL;

    ਜੇ (win_locked(win) || (win->w_ਬਫਰ != ਨਿਰਾ
					       && win->w_ਬਫਰ->b_locked > 0))
	ਵਾਪਸ FAIL; // window is already being closed
    ਜੇ (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_ਖਿੜਕੀ));
	ਵਾਪਸ FAIL;
    }
    ਜੇ ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_ਖਿੜਕੀ())
    {
	emsg(_(e_cannot_close_ਖਿੜਕੀ_only_autocmd_ਖਿੜਕੀ_would_remain));
	ਵਾਪਸ FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    ਜੇ (close_ਆਖਰੀ_ਖਿੜਕੀ_tabpage(win, free_buf, prev_curtab))
	ਵਾਪਸ FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    ਜੇ (bt_help(win->w_ਬਫਰ))
	help_ਖਿੜਕੀ = TRUE;
    ਨਹੀਂਤਾਂ
	clear_snapshot(curtab, SNAP_HELP_IDX);

    ਜੇ (bt_quickfix(win->w_ਬਫਰ))
	quickfix_ਖਿੜਕੀ = TRUE;
    ਨਹੀਂਤਾਂ
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    ਜੇ (win == curwin)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	leaving_ਖਿੜਕੀ(curwin);
#ਜੇ_ਅੰਤ
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, ਨਿਰਾ));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	ਜੇ (wp->w_ਬਫਰ != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_ਬਫਰ = TRUE;
	    ਜੇ (!win_valid(win))
		ਵਾਪਸ FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	    ਜੇ (!win_valid(win))
		ਵਾਪਸ FAIL;
	    --win->w_locked;
	    ਜੇ (ਆਖਰੀ_ਖਿੜਕੀ())
		ਵਾਪਸ FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	ਜੇ (!win_valid(win))
	    ਵਾਪਸ FAIL;
	--win->w_locked;
	ਜੇ (ਆਖਰੀ_ਖਿੜਕੀ())
	    ਵਾਪਸ FAIL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// autocmds may abort script processing
	ਜੇ (aborting())
	    ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    ਜੇ (gui.ਵਿੱਚ_use)
	ਬਾਹਰਲਾ_flush();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (popup_win_closed(win) && !win_valid(win))
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    ਜੇ (!win_valid_any_tab(win))
	ਵਾਪਸ OK;

    win_close_ਬਫਰ(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    ਜੇ (win_valid(win) && win->w_ਬਫਰ == ਨਿਰਾ
#ਜੇ defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#ਜੇ_ਅੰਤ
	    && ਆਖਰੀ_ਖਿੜਕੀ())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	ਜੇ (curwin->w_ਬਫਰ == ਨਿਰਾ)
	    curwin->w_ਬਫਰ = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    ਜੇ (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_ਬਫਰ == ਨਿਰਾ)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	ਵਾਪਸ FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    ਜੇ (!win_valid(win) || ਆਖਰੀ_ਖਿੜਕੀ()
	    || close_ਆਖਰੀ_ਖਿੜਕੀ_tabpage(win, free_buf, prev_curtab))
	ਵਾਪਸ FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++ਵੰਡੋ_disallowed;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
    ++dont_parse_messages;
#ਜੇ_ਅੰਤ

    ਜੇ (win->w_ਬਫਰ != ਨਿਰਾ)
	--win->w_ਬਫਰ->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, ਨਿਰਾ);

    ਜੇ (help_ਖਿੜਕੀ || quickfix_ਖਿੜਕੀ)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_ਖਿੜਕੀ ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	ਜੇ (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    ਜੇ (win == curwin)
    {
	curwin = wp;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	ਜੇ (wp->w_p_pvw || bt_quickfix(wp->w_ਬਫਰ))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    ਲਈ (;;)
	    {
		ਜੇ (wp->w_ਅਗਲਾ == ਨਿਰਾ)
		    wp = firstwin;
		ਨਹੀਂਤਾਂ
		    wp = wp->w_ਅਗਲਾ;
		ਜੇ (wp == curwin)
		    ਰੋਕੋ;
		ਜੇ (!wp->w_p_pvw && !bt_quickfix(wp->w_ਬਫਰ))
		{
		    curwin = wp;
		    ਰੋਕੋ;
		}
	    }
	}
#ਜੇ_ਅੰਤ
	curbuf = curwin->w_ਬਫਰ;
	close_curwin = TRUE;

	// The cursor position may be invalid if the buffer changed after last
	// using the window.
	check_cursor();
    }

    /*
     * If last window has a status line now and we don't want one, remove the
     * status line.  Do this before win_equal(), because it may change the
     * height of a window
     */
    ਆਖਰੀ_ਸਥਿਤੀ(FALSE);

    ਜੇ (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_ਚੌਖਟਾ->fr_ਮਾਪਾ == win_ਚੌਖਟਾ, dir);
    ਨਹੀਂਤਾਂ
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    ਜੇ (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
	did_decrement =
#ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)
#ਜੇ_ਅੰਤ
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	ਜੇ (other_ਬਫਰ)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    }

    ਜੇ (ONE_ਖਿੜਕੀ && curwin->w_locked && curbuf->b_locked_ਵੰਡੋ
	    && ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);

    --ਵੰਡੋ_disallowed;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
    ਜੇ (!did_decrement)
	--dont_parse_messages;
#ਜੇ_ਅੰਤ

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    ਜੇ (help_ਖਿੜਕੀ || quickfix_ਖਿੜਕੀ)
	restore_snapshot(help_ਖਿੜਕੀ ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    ਜੇ (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	ਪੂਰਨ	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    ਜੇ (dwin->w_p_diff)
		++diffcount;
	ਜੇ (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    ਜੇ (gui.ਵਿੱਚ_use && !win_hasvertsplit())
	gui_init_which_components(ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    redraw_all_later(UPD_NOT_VALID);
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਖਾਲੀ
trigger_winnewpre(ਖਾਲੀ)
{
    ਖਿੜਕੀ_layout_ਤਾਲਾ();
    apply_autocmds(EVENT_WINNEWPRE, ਨਿਰਾ, ਨਿਰਾ, FALSE, ਨਿਰਾ);
    ਖਿੜਕੀ_layout_unlock();
}

    ਸਥਿਰ ਖਾਲੀ
trigger_winclosed(win_T *win)
{
    ਸਥਿਰ ਪੂਰਨ	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    ਜੇ (recursive)
	ਵਾਪਸ;
    recursive = TRUE;
    vim_snprintf((ਅੱਖਰ *)winid, ਆਕਾਰ(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_ਬਫਰ);
    recursive = FALSE;
}

    ਖਾਲੀ
trigger_tabclosedpre(tabpage_T *tp)
{
    ਸਥਿਰ ਪੂਰਨ	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    ਜੇ (!has_tabclosedpre() || recursive)
	ਵਾਪਸ;

    ਜੇ (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    ਖਿੜਕੀ_layout_ਤਾਲਾ();
    apply_autocmds(EVENT_TABCLOSEDPRE, ਨਿਰਾ, ਨਿਰਾ, FALSE, ਨਿਰਾ);
    ਖਿੜਕੀ_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    ਜੇ (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    ਨਹੀਂਤਾਂ
	// fall back to the first tabpage
	goto_tabpage_tp(ਪਹਿਲਾ_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    ਖਾਲੀ
snapshot_windows_scroll_ਮਾਪ(ਖਾਲੀ)
{
    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	wp->w_ਆਖਰੀ_topline = wp->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	wp->w_ਆਖਰੀ_topfill = wp->w_topfill;
#ਜੇ_ਅੰਤ
	wp->w_ਆਖਰੀ_leftcol = wp->w_leftcol;
	wp->w_ਆਖਰੀ_skipcol = wp->w_skipcol;
	wp->w_ਆਖਰੀ_ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ;
	wp->w_ਆਖਰੀ_ਉਚਾਈ = wp->w_ਉਚਾਈ;
    }
}

ਸਥਿਰ ਪੂਰਨ did_initial_scroll_ਮਾਪ_snapshot = FALSE;

    ਖਾਲੀ
may_make_initial_scroll_ਮਾਪ_snapshot(ਖਾਲੀ)
{
    ਜੇ (!did_initial_scroll_ਮਾਪ_snapshot)
    {
	did_initial_scroll_ਮਾਪ_snapshot = TRUE;
	snapshot_windows_scroll_ਮਾਪ();
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Create a dictionary with information about size and scroll changes in a
 * window.
 * Returns the dictionary with refcount set to one.
 * Returns NULL when out of memory.
 */
    ਸਥਿਰ dict_T *
make_win_info_dict(
	ਪੂਰਨ ਚੌੜਾਈ,
	ਪੂਰਨ ਉਚਾਈ,
	ਪੂਰਨ topline,
# ifdef FEAT_DIFF
	ਪੂਰਨ topfill,
# endif
	ਪੂਰਨ leftcol,
	ਪੂਰਨ skipcol)
{
    dict_T *d = dict_alloc();
    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    ਜਦੋਂ (1)
    {
	typval_T tv;
	tv.v_ਤਾਲਾ = 0;
	tv.v_type = VAR_ਸੰਖਿਆ;

	tv.vval.v_ਸੰਖਿਆ = ਚੌੜਾਈ;
	ਜੇ (dict_ਜੋੜੋ_tv(d, "width", &tv) == FAIL)
	    ਰੋਕੋ;
	tv.vval.v_ਸੰਖਿਆ = ਉਚਾਈ;
	ਜੇ (dict_ਜੋੜੋ_tv(d, "height", &tv) == FAIL)
	    ਰੋਕੋ;
	tv.vval.v_ਸੰਖਿਆ = topline;
	ਜੇ (dict_ਜੋੜੋ_tv(d, "topline", &tv) == FAIL)
	    ਰੋਕੋ;
# ifdef FEAT_DIFF
	tv.vval.v_ਸੰਖਿਆ = topfill;
# ਨਹੀਂਤਾਂ
	tv.vval.v_ਸੰਖਿਆ = 0;
# endif
	ਜੇ (dict_ਜੋੜੋ_tv(d, "topfill", &tv) == FAIL)
	    ਰੋਕੋ;
	tv.vval.v_ਸੰਖਿਆ = leftcol;
	ਜੇ (dict_ਜੋੜੋ_tv(d, "leftcol", &tv) == FAIL)
	    ਰੋਕੋ;
	tv.vval.v_ਸੰਖਿਆ = skipcol;
	ਜੇ (dict_ਜੋੜੋ_tv(d, "skipcol", &tv) == FAIL)
	    ਰੋਕੋ;
	ਵਾਪਸ d;
    }
    dict_unref(d);
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

// Return values of check_window_scroll_resize():
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CWSR_SCROLLED	1  // at least one window scrolled
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CWSR_RESIZED	2  // at least one window size changed

/*
 * This function is used for three purposes:
 * 1. Goes over all windows in the current tab page and sets:
 *    "size_count" to the nr of windows with size changes.
 *    "first_scroll_win" to the first window with any relevant changes.
 *    "first_size_win" to the first window with size changes.
 *
 * 2. When the first three arguments are NULL and "winlist" is not NULL,
 *    "winlist" is set to the list of window IDs with size changes.
 *
 * 3. When the first three arguments are NULL and "v_event" is not NULL,
 *    information about changed windows is added to "v_event".
 */
    ਸਥਿਰ ਖਾਲੀ
check_ਖਿੜਕੀ_scroll_resize(
	ਪੂਰਨ	*ਮਾਪ_ਗਿਣਤੀ,
	win_T	**ਪਹਿਲਾ_scroll_win,
	win_T	**ਪਹਿਲਾ_ਮਾਪ_win,
	list_T	*winlist ਅਣਵਰਤਿਆ,
	dict_T	*v_event ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ listidx = 0;
    ਪੂਰਨ tot_ਚੌੜਾਈ = 0;
    ਪੂਰਨ tot_ਉਚਾਈ = 0;
    ਪੂਰਨ tot_topline = 0;
# ifdef FEAT_DIFF
    ਪੂਰਨ tot_topfill = 0;
# endif
    ਪੂਰਨ tot_leftcol = 0;
    ਪੂਰਨ tot_skipcol = 0;
#ਜੇ_ਅੰਤ

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	ਪੂਰਨ ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	ਪੂਰਨ ਮਾਪ_ਬਦਲਿਆ = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_ਆਖਰੀ_ਚੌੜਾਈ != wp->w_ਚੌੜਾਈ
			    || wp->w_ਆਖਰੀ_ਉਚਾਈ != wp->w_ਉਚਾਈ);
	ਜੇ (ਮਾਪ_ਬਦਲਿਆ)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (winlist != ਨਿਰਾ)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_ਤਾਲਾ = 0;
		tv.v_type = VAR_ਸੰਖਿਆ;
		tv.vval.v_ਸੰਖਿਆ = wp->w_id;
		list_ਸੈੱਟ_ਵਸਤੂ(winlist, listidx++, &tv);
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਜੇ (ਮਾਪ_ਗਿਣਤੀ != ਨਿਰਾ)
	    {
		++*ਮਾਪ_ਗਿਣਤੀ;
		ਜੇ (*ਪਹਿਲਾ_ਮਾਪ_win == ਨਿਰਾ)
		    *ਪਹਿਲਾ_ਮਾਪ_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		ਜੇ (*ਪਹਿਲਾ_scroll_win == ਨਿਰਾ && !ignore_scroll)
		    *ਪਹਿਲਾ_scroll_win = wp;
	    }
	}

	ਪੂਰਨ scroll_ਬਦਲਿਆ = !ignore_scroll
				&& (wp->w_ਆਖਰੀ_topline != wp->w_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
				|| wp->w_ਆਖਰੀ_topfill != wp->w_topfill
#ਜੇ_ਅੰਤ
				|| wp->w_ਆਖਰੀ_leftcol != wp->w_leftcol
				|| wp->w_ਆਖਰੀ_skipcol != wp->w_skipcol);
	ਜੇ (scroll_ਬਦਲਿਆ
	    && ਪਹਿਲਾ_scroll_win != ਨਿਰਾ && *ਪਹਿਲਾ_scroll_win == ਨਿਰਾ)
	    *ਪਹਿਲਾ_scroll_win = wp;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ ((ਮਾਪ_ਬਦਲਿਆ || scroll_ਬਦਲਿਆ) && v_event != ਨਿਰਾ)
	{
	    // Add info about this window to the v:event dictionary.
	    ਪੂਰਨ ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ - wp->w_ਆਖਰੀ_ਚੌੜਾਈ;
	    ਪੂਰਨ ਉਚਾਈ = wp->w_ਉਚਾਈ - wp->w_ਆਖਰੀ_ਉਚਾਈ;
	    ਪੂਰਨ topline = wp->w_topline - wp->w_ਆਖਰੀ_topline;
# ifdef FEAT_DIFF
	    ਪੂਰਨ topfill = wp->w_topfill - wp->w_ਆਖਰੀ_topfill;
# endif
	    ਪੂਰਨ leftcol = wp->w_leftcol - wp->w_ਆਖਰੀ_leftcol;
	    ਪੂਰਨ skipcol = wp->w_skipcol - wp->w_ਆਖਰੀ_skipcol;
	    dict_T *d = make_win_info_dict(ਚੌੜਾਈ, ਉਚਾਈ, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    ਜੇ (d == ਨਿਰਾ)
		ਰੋਕੋ;
	    ਅੱਖਰ winid[NUMBUFLEN];
	    vim_snprintf(winid, ਆਕਾਰ(winid), "%d", wp->w_id);
	    ਜੇ (dict_ਜੋੜੋ_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		ਰੋਕੋ;
	    }
	    --d->dv_refcount;

	    tot_ਚੌੜਾਈ += ਨਿਰਪੇਖ(ਚੌੜਾਈ);
	    tot_ਉਚਾਈ += ਨਿਰਪੇਖ(ਉਚਾਈ);
	    tot_topline += ਨਿਰਪੇਖ(topline);
# ifdef FEAT_DIFF
	    tot_topfill += ਨਿਰਪੇਖ(topfill);
# endif
	    tot_leftcol += ਨਿਰਪੇਖ(leftcol);
	    tot_skipcol += ਨਿਰਪੇਖ(skipcol);
	}
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (v_event != ਨਿਰਾ)
    {
	dict_T *alldict = make_win_info_dict(tot_ਚੌੜਾਈ, tot_ਉਚਾਈ, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	ਜੇ (alldict != ਨਿਰਾ)
	{
	    ਜੇ (dict_ਜੋੜੋ_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    ਨਹੀਂਤਾਂ
		--alldict->dv_refcount;
	}
    }
#ਜੇ_ਅੰਤ
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    ਖਾਲੀ
may_trigger_win_scrolled_resized(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ	    recursive = FALSE;
    ਪੂਰਨ		    do_resize = has_winresized();
    ਪੂਰਨ		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    ਜੇ (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_ਮਾਪ_snapshot)
	ਵਾਪਸ;

    ਪੂਰਨ ਮਾਪ_ਗਿਣਤੀ = 0;
    win_T *ਪਹਿਲਾ_scroll_win = ਨਿਰਾ, *ਪਹਿਲਾ_ਮਾਪ_win = ਨਿਰਾ;
    check_ਖਿੜਕੀ_scroll_resize(&ਮਾਪ_ਗਿਣਤੀ, &ਪਹਿਲਾ_scroll_win, &ਪਹਿਲਾ_ਮਾਪ_win,
								    ਨਿਰਾ, ਨਿਰਾ);
    ਪੂਰਨ trigger_resize = do_resize && ਮਾਪ_ਗਿਣਤੀ > 0;
    ਪੂਰਨ trigger_scroll = do_scroll && ਪਹਿਲਾ_scroll_win != ਨਿਰਾ;
    ਜੇ (!trigger_resize && !trigger_scroll)
	ਵਾਪਸ;  // no relevant changes
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    list_T *windows_list = ਨਿਰਾ;
    ਜੇ (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_ਚੀਜ਼ਾਂ(ਮਾਪ_ਗਿਣਤੀ);
	ਜੇ (windows_list != ਨਿਰਾ)
	    check_ਖਿੜਕੀ_scroll_resize(ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, windows_list, ਨਿਰਾ);
    }

    dict_T *scroll_dict = ਨਿਰਾ;
    ਜੇ (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	ਜੇ (scroll_dict != ਨਿਰਾ)
	{
	    scroll_dict->dv_refcount = 1;
	    check_ਖਿੜਕੀ_scroll_resize(ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, scroll_dict);
	}
    }
#ਜੇ_ਅੰਤ

    // WinScrolled/WinResized are triggered only once, even when multiple
    // windows scrolled or changed size.  Store the current values before
    // triggering the event, if a scroll or resize happens as a side effect
    // then WinScrolled/WinResized is triggered for that later.
    snapshot_windows_scroll_ਮਾਪ();

    // "curwin" may be different from the actual current window, make
    // sure it can be restored.
    ਖਿੜਕੀ_layout_ਤਾਲਾ();
    recursive = TRUE;

    // If both are to be triggered do WinResized first.
    ਜੇ (trigger_resize
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    && windows_list != ਨਿਰਾ
#ਜੇ_ਅੰਤ
	    )
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	ਜੇ (dict_ਜੋੜੋ_list(v_event, "windows", windows_list) == OK)
	{
	    dict_ਸੈੱਟ_ਚੀਜ਼ਾਂ_ro(v_event);
#ਜੇ_ਅੰਤ
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((ਅੱਖਰ *)winid, ਆਕਾਰ(winid), "%d",
							 ਪਹਿਲਾ_ਮਾਪ_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     ਪਹਿਲਾ_ਮਾਪ_win->w_ਬਫਰ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#ਜੇ_ਅੰਤ
    }

    ਜੇ (trigger_scroll
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    && scroll_dict != ਨਿਰਾ
#ਜੇ_ਅੰਤ
	    )
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", ਨਿਰਾ);
	dict_ਸੈੱਟ_ਚੀਜ਼ਾਂ_ro(v_event);
	dict_unref(scroll_dict);
#ਜੇ_ਅੰਤ
	char_u winid[NUMBUFLEN];
	vim_snprintf((ਅੱਖਰ *)winid, ਆਕਾਰ(winid), "%d",
						       ਪਹਿਲਾ_scroll_win->w_id);
	apply_autocmds(EVENT_WINSCROLLED, winid, winid, FALSE,
						   ਪਹਿਲਾ_scroll_win->w_ਬਫਰ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	restore_v_event(v_event, &save_v_event);
#ਜੇ_ਅੰਤ
    }

    recursive = FALSE;
    ਖਿੜਕੀ_layout_unlock();
}

/*
 * Close window "win" in tab page "tp", which is not the current tab page.
 * This may be the last window in that tab page and result in closing the tab,
 * thus "tp" may become invalid!
 * Caller must check if buffer is hidden and whether the tabline needs to be
 * updated.
 */
    ਖਾਲੀ
win_close_othertab(win_T *win, ਪੂਰਨ free_buf, tabpage_T *tp)
{
    win_T	*wp;
    ਪੂਰਨ		dir;
    tabpage_T   *ptp = ਨਿਰਾ;
    ਪੂਰਨ		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_ਮਾਪ))
	ਵਾਪਸ;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    ਜੇ (win_locked(win) || (win->w_ਬਫਰ != ਨਿਰਾ
					       && win->w_ਬਫਰ->b_locked > 0))
	ਵਾਪਸ; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    ਜੇ (win->w_ਬਫਰ != ਨਿਰਾ)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	ਜੇ (!win_valid_any_tab(win))
	    ਵਾਪਸ;
    }

    ਜੇ (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	ਜੇ (!win_valid_any_tab(win))
	    ਵਾਪਸ;
    }

    ਜੇ (win->w_ਬਫਰ != ਨਿਰਾ)
	// Close the link to the buffer.
	close_ਬਫਰ(win, win->w_ਬਫਰ, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    ਲਈ (ptp = ਪਹਿਲਾ_tabpage; ptp != ਨਿਰਾ && ptp != tp; ptp = ptp->tp_ਅਗਲਾ)
	;
    ਜੇ (ptp == ਨਿਰਾ || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	ਜੇ (win_valid_any_tab(win) && win->w_ਬਫਰ == ਨਿਰਾ)
	{
	    win->w_ਬਫਰ = firstbuf;
	    ++firstbuf->b_nwindows;
	    ਜੇ (win == curwin)
		curbuf = curwin->w_ਬਫਰ;
	    win_init_ਖਾਲੀ_ਥਾਵਾਂ(win);
	}
	ਵਾਪਸ;
    }

    // Autocommands may have closed the window already.
    ਲਈ (wp = tp->tp_firstwin; wp != ਨਿਰਾ && wp != win; wp = wp->w_ਅਗਲਾ)
	;
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    // When closing the last window in a tab page remove the tab page.
    ਜੇ (tp->tp_firstwin == tp->tp_lastwin)
    {
	ਪੂਰਨ	h = tabline_ਉਚਾਈ();

	ਜੇ (tp == ਪਹਿਲਾ_tabpage)
	    ਪਹਿਲਾ_tabpage = tp->tp_ਅਗਲਾ;
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (ptp = ਪਹਿਲਾ_tabpage; ptp != ਨਿਰਾ && ptp->tp_ਅਗਲਾ != tp;
							   ptp = ptp->tp_ਅਗਲਾ)
		;
	    ਜੇ (ptp == ਨਿਰਾ)
	    {
		ਅੰਦਰੂਨੀ_error("win_close_othertab()");
		ਵਾਪਸ;
	    }
	    ptp->tp_ਅਗਲਾ = tp->tp_ਅਗਲਾ;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	ਜੇ (h != tabline_ਉਚਾਈ())
	    shell_new_ਪੰਕਤੀਆਂ();
    }

    ਜੇ (win->w_ਬਫਰ != ਨਿਰਾ)
	--win->w_ਬਫਰ->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    ਜੇ (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    ਸਥਿਰ win_T *
win_free_mem(
    win_T	*win,
    ਪੂਰਨ		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ਚੌਖਟਾ_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == ਨਿਰਾ ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_ਚੌਖਟਾ;
    wp = winframe_remove(win, dirp, tp, ਨਿਰਾ);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    ਜੇ (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    ਵਾਪਸ wp;
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
win_free_all(ਖਾਲੀ)
{
    ਪੂਰਨ		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = ਨਿਰਾ;
    cmdwin_win = ਨਿਰਾ;

    ਜਦੋਂ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ)
	tabpage_close(TRUE);

    ਲਈ (ਪੂਰਨ i = 0; i < AUCMD_WIN_ਗਿਣਤੀ; ++i)
	ਜੇ (aucmd_win[i].auc_win != ਨਿਰਾ)
	{
	    (ਖਾਲੀ)win_free_mem(aucmd_win[i].auc_win, &dummy, ਨਿਰਾ);
	    aucmd_win[i].auc_win = ਨਿਰਾ;
	}

    ਜਦੋਂ (firstwin != ਨਿਰਾ)
	(ਖਾਲੀ)win_free_mem(firstwin, &dummy, ਨਿਰਾ);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    ਪੂਰਨ		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    ਚੌਖਟਾ_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    ਚੌਖਟਾ_T	*frp, *frp2, *frp3;
    ਚੌਖਟਾ_T	*frp_close = win->w_ਚੌਖਟਾ;
    win_T	*wp;
    ਪੂਰਨ		ਪੰਕਤੀ, ਖਾਨਾ;

    /*
     * If there is only one window there is nothing to remove.
     */
    ਜੇ (tp == ਨਿਰਾ ? ONE_ਖਿੜਕੀ : tp->tp_firstwin == tp->tp_lastwin)
	ਵਾਪਸ ਨਿਰਾ;

    ਚੌਖਟਾ_locked++;

    // Save the position of the containing frame (which will also contain the
    // altframe) before we remove anything, to recompute window positions later.
    wp = frame2win(frp_close->fr_ਮਾਪਾ);
    ਪੰਕਤੀ = wp->w_winrow;
    ਖਾਨਾ = wp->w_wincol;

    /*
     * Remove the window from its frame.
     */
    frp2 = win_altframe(win, tp);
    wp = frame2win(frp2);

    // If this is a rightmost window, remove vertical separators to the left.
    ਜੇ (win->w_vsep_ਚੌੜਾਈ == 0 && frp_close->fr_ਮਾਪਾ->fr_layout == FR_ਪੰਕਤੀ &&
	    frp_close->fr_prev != ਨਿਰਾ)
	ਚੌਖਟਾ_ਸੈੱਟ_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    ਚੌਖਟਾ_remove(frp_close);

    ਜੇ (frp_close->fr_ਮਾਪਾ->fr_layout == FR_ਖਾਨਾ)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	ਜੇ (frp2->fr_win != ਨਿਰਾ && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_ਅਗਲਾ;
	    ਜਦੋਂ (frp != ਨਿਰਾ || frp3 != ਨਿਰਾ)
	    {
		ਜੇ (frp != ਨਿਰਾ)
		{
		    ਜੇ (!ਚੌਖਟਾ_fixed_ਉਚਾਈ(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			ਰੋਕੋ;
		    }
		    frp = frp->fr_prev;
		}
		ਜੇ (frp3 != ਨਿਰਾ)
		{
		    ਜੇ (frp3->fr_win != ਨਿਰਾ && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			ਰੋਕੋ;
		    }
		    frp3 = frp3->fr_ਅਗਲਾ;
		}
	    }
	}
	ਚੌਖਟਾ_new_ਉਚਾਈ(frp2, frp2->fr_ਉਚਾਈ + frp_close->fr_ਉਚਾਈ,
			    frp2 == frp_close->fr_ਅਗਲਾ, !ਚੌਖਟਾ_fixed_ਉਚਾਈ(frp2), FALSE);
	*dirp = 'v';
    }
    ਨਹੀਂਤਾਂ
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	ਜੇ (frp2->fr_win != ਨਿਰਾ && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_ਅਗਲਾ;
	    ਜਦੋਂ (frp != ਨਿਰਾ || frp3 != ਨਿਰਾ)
	    {
		ਜੇ (frp != ਨਿਰਾ)
		{
		    ਜੇ (!ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			ਰੋਕੋ;
		    }
		    frp = frp->fr_prev;
		}
		ਜੇ (frp3 != ਨਿਰਾ)
		{
		    ਜੇ (frp3->fr_win != ਨਿਰਾ && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			ਰੋਕੋ;
		    }
		    frp3 = frp3->fr_ਅਗਲਾ;
		}
	    }
	}
	ਚੌਖਟਾ_new_ਚੌੜਾਈ(frp2, frp2->fr_ਚੌੜਾਈ + frp_close->fr_ਚੌੜਾਈ,
			    frp2 == frp_close->fr_ਅਗਲਾ, !ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(frp2));
	*dirp = 'h';
    }

    // If the altframe wasn't adjacent and left/above, resizing it will have
    // changed window positions within the parent frame.  Recompute them.
    ਜੇ (frp2 != frp_close->fr_prev)
	ਚੌਖਟਾ_comp_pos(frp_close->fr_ਮਾਪਾ, &ਪੰਕਤੀ, &ਖਾਨਾ);

    ਜੇ (unflat_altfr == ਨਿਰਾ)
	ਚੌਖਟਾ_flatten(frp2);
    ਨਹੀਂਤਾਂ
	*unflat_altfr = frp2;

    ਚੌਖਟਾ_locked--;

    ਵਾਪਸ wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_flatten(ਚੌਖਟਾ_T *frp)
{
    ਚੌਖਟਾ_T	*frp2, *frp3;

    ਜੇ (frp->fr_ਅਗਲਾ != ਨਿਰਾ || frp->fr_prev != ਨਿਰਾ)
	ਵਾਪਸ;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_ਮਾਪਾ->fr_layout = frp->fr_layout;
    frp->fr_ਮਾਪਾ->fr_ਬੱਚਾ = frp->fr_ਬੱਚਾ;
    FOR_ALL_ਚੌਖਟੇ(frp2, frp->fr_ਬੱਚਾ)
	frp2->fr_ਮਾਪਾ = frp->fr_ਮਾਪਾ;
    frp->fr_ਮਾਪਾ->fr_win = frp->fr_win;
    ਜੇ (frp->fr_win != ਨਿਰਾ)
	frp->fr_win->w_ਚੌਖਟਾ = frp->fr_ਮਾਪਾ;
    frp2 = frp->fr_ਮਾਪਾ;
    ਜੇ (topframe->fr_ਬੱਚਾ == frp)
	topframe->fr_ਬੱਚਾ = frp2;
    vim_free(frp);

    frp = frp2->fr_ਮਾਪਾ;
    ਜੇ (frp != ਨਿਰਾ && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	ਜੇ (frp->fr_ਬੱਚਾ == frp2)
	    frp->fr_ਬੱਚਾ = frp2->fr_ਬੱਚਾ;
	frp2->fr_ਬੱਚਾ->fr_prev = frp2->fr_prev;
	ਜੇ (frp2->fr_prev != ਨਿਰਾ)
	    frp2->fr_prev->fr_ਅਗਲਾ = frp2->fr_ਬੱਚਾ;
	ਲਈ (frp3 = frp2->fr_ਬੱਚਾ; ; frp3 = frp3->fr_ਅਗਲਾ)
	{
	    frp3->fr_ਮਾਪਾ = frp;
	    ਜੇ (frp3->fr_ਅਗਲਾ == ਨਿਰਾ)
	    {
		frp3->fr_ਅਗਲਾ = frp2->fr_ਅਗਲਾ;
		ਜੇ (frp2->fr_ਅਗਲਾ != ਨਿਰਾ)
		    frp2->fr_ਅਗਲਾ->fr_prev = frp3;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (topframe->fr_ਬੱਚਾ == frp2)
	    topframe->fr_ਬੱਚਾ = frp;
	vim_free(frp2);
    }
}

/*
 * Undo changes from a prior call to winframe_remove, also restoring lost
 * vertical separators and statuslines, and changed window positions for
 * windows within "unflat_altfr".
 * Caller must ensure no other changes were made to the layout or window sizes!
 */
    ਸਥਿਰ ਖਾਲੀ
winframe_restore(win_T *wp, ਪੂਰਨ dir, ਚੌਖਟਾ_T *unflat_altfr)
{
    ਚੌਖਟਾ_T	*frp = wp->w_ਚੌਖਟਾ;

    // Put "wp"'s frame back where it was.
    ਜੇ (frp->fr_prev != ਨਿਰਾ)
	ਚੌਖਟਾ_append(frp->fr_prev, frp);
    ਨਹੀਂਤਾਂ
	ਚੌਖਟਾ_ਪਾਓ(frp->fr_ਅਗਲਾ, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    ਜੇ (wp->w_vsep_ਚੌੜਾਈ == 0
	    && frp->fr_ਮਾਪਾ->fr_layout == FR_ਪੰਕਤੀ && frp->fr_prev != ਨਿਰਾ)
	ਚੌਖਟਾ_ਸੈੱਟ_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ == 0
	    && frp->fr_ਮਾਪਾ->fr_layout == FR_ਖਾਨਾ && frp->fr_prev != ਨਿਰਾ)
	ਚੌਖਟਾ_ਜੋੜੋ_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    ਜੇ (dir == 'v')
    {
	ਚੌਖਟਾ_new_ਉਚਾਈ(unflat_altfr, unflat_altfr->fr_ਉਚਾਈ - frp->fr_ਉਚਾਈ,
		unflat_altfr == frp->fr_ਅਗਲਾ, FALSE, FALSE);
    }
    ਨਹੀਂਤਾਂ ਜੇ (dir == 'h')
    {
	ਚੌਖਟਾ_new_ਚੌੜਾਈ(unflat_altfr, unflat_altfr->fr_ਚੌੜਾਈ - frp->fr_ਚੌੜਾਈ,
		unflat_altfr == frp->fr_ਅਗਲਾ, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    ਜੇ (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_ਮਾਪਾ);
	ਪੂਰਨ	ਪੰਕਤੀ = topleft->w_winrow;
	ਪੂਰਨ	ਖਾਨਾ = topleft->w_wincol;

	ਚੌਖਟਾ_comp_pos(frp->fr_ਮਾਪਾ, &ਪੰਕਤੀ, &ਖਾਨਾ);
    }
}

/*
 * Return a pointer to the frame that will receive the empty screen space that
 * is left over after "win" is closed.
 *
 * If 'splitbelow' or 'splitright' is set, the space goes above or to the left
 * by default.  Otherwise, the free space goes below or to the right.  The
 * result is that opening a window and then immediately closing it will
 * preserve the initial window layout.  The 'wfh' and 'wfw' settings are
 * respected when possible.
 */
    ਸਥਿਰ ਚੌਖਟਾ_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ਚੌਖਟਾ_T	*frp;
    ਚੌਖਟਾ_T	*other_fr, *ਨਿਸ਼ਾਨਾ_fr;

    ਜੇ (tp == ਨਿਰਾ ? ONE_ਖਿੜਕੀ : tp->tp_firstwin == tp->tp_lastwin)
	ਵਾਪਸ alt_tabpage()->tp_curwin->w_ਚੌਖਟਾ;

    frp = win->w_ਚੌਖਟਾ;

    ਜੇ (frp->fr_prev == ਨਿਰਾ)
	ਵਾਪਸ frp->fr_ਅਗਲਾ;
    ਜੇ (frp->fr_ਅਗਲਾ == ਨਿਰਾ)
	ਵਾਪਸ frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    //ਖਿੜਕੀ
    ਨਿਸ਼ਾਨਾ_fr = frp->fr_ਅਗਲਾ;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    ਜੇ (frp->fr_ਮਾਪਾ != ਨਿਰਾ && frp->fr_ਮਾਪਾ->fr_layout == FR_ਖਾਨਾ && p_sb)
    {
	ਨਿਸ਼ਾਨਾ_fr = frp->fr_prev;
	other_fr  = frp->fr_ਅਗਲਾ;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    ਜੇ (frp->fr_ਮਾਪਾ != ਨਿਰਾ && frp->fr_ਮਾਪਾ->fr_layout == FR_ਪੰਕਤੀ && p_spr)
    {
	ਨਿਸ਼ਾਨਾ_fr = frp->fr_prev;
	other_fr  = frp->fr_ਅਗਲਾ;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    ਜੇ (frp->fr_ਮਾਪਾ != ਨਿਰਾ && frp->fr_ਮਾਪਾ->fr_layout == FR_ਪੰਕਤੀ)
    {
	ਜੇ (ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(ਨਿਸ਼ਾਨਾ_fr) && !ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(other_fr))
	    ਨਿਸ਼ਾਨਾ_fr = other_fr;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਚੌਖਟਾ_fixed_ਉਚਾਈ(ਨਿਸ਼ਾਨਾ_fr) && !ਚੌਖਟਾ_fixed_ਉਚਾਈ(other_fr))
	    ਨਿਸ਼ਾਨਾ_fr = other_fr;
    }

    ਵਾਪਸ ਨਿਸ਼ਾਨਾ_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    ਸਥਿਰ tabpage_T *
alt_tabpage(ਖਾਲੀ)
{
    tabpage_T	*tp = ਨਿਰਾ;
    ਪੂਰਨ		forward;

    // Use the last accessed tab page, if possible.
    ਜੇ ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	ਵਾਪਸ lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_ਅਗਲਾ != ਨਿਰਾ &&
	    ((tcl_flags & TCL_ਖੱਬਾ) == 0 || curtab == ਪਹਿਲਾ_tabpage);

    ਜੇ (forward)
	tp = curtab->tp_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	// Use the previous tab page.
	ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp->tp_ਅਗਲਾ != curtab; tp = tp->tp_ਅਗਲਾ)
	    ;

    ਵਾਪਸ tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    ਸਥਿਰ win_T *
frame2win(ਚੌਖਟਾ_T *frp)
{
    ਜਦੋਂ (frp->fr_win == ਨਿਰਾ)
	frp = frp->fr_ਬੱਚਾ;
    ਵਾਪਸ frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    ਸਥਿਰ ਪੂਰਨ
ਚੌਖਟਾ_has_win(ਚੌਖਟਾ_T *frp, win_T *wp)
{
    ਚੌਖਟਾ_T	*p;

    ਜੇ (frp->fr_layout == FR_ਪੱਤੀ)
	ਵਾਪਸ frp->fr_win == wp;

    FOR_ALL_ਚੌਖਟੇ(p, frp->fr_ਬੱਚਾ)
	ਜੇ (ਚੌਖਟਾ_has_win(p, wp))
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
ਸਥਿਰ ਪੂਰਨ min_ਸੈੱਟ_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_new_ਉਚਾਈ(
    ਚੌਖਟਾ_T	*topfrp,
    ਪੂਰਨ		ਉਚਾਈ,
    ਪੂਰਨ		topfirst,	// resize topmost contained frame first
    ਪੂਰਨ		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    ਪੂਰਨ		ਸੈੱਟ_ch)		// set 'cmdheight' to resize topframe
{
    ਚੌਖਟਾ_T	*frp;
    ਪੂਰਨ		extra_ਸਤਰਾਂ;
    ਪੂਰਨ		h;

    ਜੇ (topfrp->fr_ਮਾਪਾ == ਨਿਰਾ && ਸੈੱਟ_ch)
    {
	// topframe: update the command line height, with side effects.
	ਪੂਰਨ new_ch = MAX(min_ਸੈੱਟ_ch, p_ch + topfrp->fr_ਉਚਾਈ - ਉਚਾਈ);
	ਪੂਰਨ save_ch = min_ਸੈੱਟ_ch;
	ਜੇ (new_ch != p_ch)
	    ਸੈੱਟ_option_ਮੁੱਲ((char_u *)"cmdheight", new_ch, ਨਿਰਾ, 0);
	min_ਸੈੱਟ_ch = save_ch;
	ਉਚਾਈ = MIN(ਉਚਾਈ, ਪੰਕਤੀਆਂ_AVAIL);
    }
    ਜੇ (topfrp->fr_win != ਨਿਰਾ)
    {
	// Simple case: just one window.
	win_new_ਉਚਾਈ(topfrp->fr_win,
				    ਉਚਾਈ - topfrp->fr_win->w_ਸਥਿਤੀ_ਉਚਾਈ
					      - WINBAR_ਉਚਾਈ(topfrp->fr_win));
    }
    ਨਹੀਂਤਾਂ ਜੇ (topfrp->fr_layout == FR_ਪੰਕਤੀ)
    {
	ਕਰੋ
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_ਚੌਖਟੇ(frp, topfrp->fr_ਬੱਚਾ)
	    {
		ਚੌਖਟਾ_new_ਉਚਾਈ(frp, ਉਚਾਈ, topfirst, wfh, ਸੈੱਟ_ch);
		ਜੇ (frp->fr_ਉਚਾਈ > ਉਚਾਈ)
		{
		    // Could not fit the windows, make the whole row higher.
		    ਉਚਾਈ = frp->fr_ਉਚਾਈ;
		    ਰੋਕੋ;
		}
	    }
	}
	ਜਦੋਂ (frp != ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_ਬੱਚਾ;
	ਜੇ (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    ਜਦੋਂ (ਚੌਖਟਾ_fixed_ਉਚਾਈ(frp))
	    {
		frp = frp->fr_ਅਗਲਾ;
		ਜੇ (frp == ਨਿਰਾ)
		    ਵਾਪਸ;	    // no frame without 'wfh', give up
	    }
	ਜੇ (!topfirst)
	{
	    // Find the bottom frame of this column
	    ਜਦੋਂ (frp->fr_ਅਗਲਾ != ਨਿਰਾ)
		frp = frp->fr_ਅਗਲਾ;
	    ਜੇ (wfh)
		// Advance back for frames with one window with 'wfh' set.
		ਜਦੋਂ (ਚੌਖਟਾ_fixed_ਉਚਾਈ(frp))
		    frp = frp->fr_prev;
	}

	extra_ਸਤਰਾਂ = ਉਚਾਈ - topfrp->fr_ਉਚਾਈ;
	ਜੇ (extra_ਸਤਰਾਂ < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    ਜਦੋਂ (frp != ਨਿਰਾ)
	    {
		h = ਚੌਖਟਾ_minheight(frp, ਨਿਰਾ);
		ਜੇ (frp->fr_ਉਚਾਈ + extra_ਸਤਰਾਂ < h)
		{
		    extra_ਸਤਰਾਂ += frp->fr_ਉਚਾਈ - h;
		    ਚੌਖਟਾ_new_ਉਚਾਈ(frp, h, topfirst, wfh, ਸੈੱਟ_ch);
		}
		ਨਹੀਂਤਾਂ
		{
		    ਚੌਖਟਾ_new_ਉਚਾਈ(frp, frp->fr_ਉਚਾਈ + extra_ਸਤਰਾਂ,
							topfirst, wfh, ਸੈੱਟ_ch);
		    ਰੋਕੋ;
		}
		ਜੇ (topfirst)
		{
		    ਕਰੋ
			frp = frp->fr_ਅਗਲਾ;
		    ਜਦੋਂ (wfh && frp != ਨਿਰਾ && ਚੌਖਟਾ_fixed_ਉਚਾਈ(frp));
		}
		ਨਹੀਂਤਾਂ
		{
		    ਕਰੋ
			frp = frp->fr_prev;
		    ਜਦੋਂ (wfh && frp != ਨਿਰਾ && ਚੌਖਟਾ_fixed_ਉਚਾਈ(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		ਜੇ (frp == ਨਿਰਾ)
		    ਉਚਾਈ -= extra_ਸਤਰਾਂ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (extra_ਸਤਰਾਂ > 0)
	{
	    // increase height of bottom or top frame
	    ਚੌਖਟਾ_new_ਉਚਾਈ(frp, frp->fr_ਉਚਾਈ + extra_ਸਤਰਾਂ, topfirst, wfh,
									ਸੈੱਟ_ch);
	}
    }
    topfrp->fr_ਉਚਾਈ = ਉਚਾਈ;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    ਸਥਿਰ ਪੂਰਨ
ਚੌਖਟਾ_fixed_ਉਚਾਈ(ਚੌਖਟਾ_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    ਜੇ (frp->fr_win != ਨਿਰਾ)
	ਵਾਪਸ frp->fr_win->w_p_wfh;

    ਜੇ (frp->fr_layout == FR_ਪੰਕਤੀ)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_ਚੌਖਟੇ(frp, frp->fr_ਬੱਚਾ)
	    ਜੇ (ਚੌਖਟਾ_fixed_ਉਚਾਈ(frp))
		ਵਾਪਸ TRUE;
	ਵਾਪਸ FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_ਚੌਖਟੇ(frp, frp->fr_ਬੱਚਾ)
	ਜੇ (!ਚੌਖਟਾ_fixed_ਉਚਾਈ(frp))
	    ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    ਸਥਿਰ ਪੂਰਨ
ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(ਚੌਖਟਾ_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    ਜੇ (frp->fr_win != ਨਿਰਾ)
	ਵਾਪਸ frp->fr_win->w_p_wfw;

    ਜੇ (frp->fr_layout == FR_ਖਾਨਾ)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_ਚੌਖਟੇ(frp, frp->fr_ਬੱਚਾ)
	    ਜੇ (ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(frp))
		ਵਾਪਸ TRUE;
	ਵਾਪਸ FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_ਚੌਖਟੇ(frp, frp->fr_ਬੱਚਾ)
	ਜੇ (!ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(frp))
	    ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_ਜੋੜੋ_statusline(ਚੌਖਟਾ_T *frp)
{
    win_T	*wp;

    ਜੇ (frp->fr_layout == FR_ਪੱਤੀ)
    {
	wp = frp->fr_win;
	wp->w_ਸਥਿਤੀ_ਉਚਾਈ = statusline_ਉਚਾਈ(wp);
    }
    ਨਹੀਂਤਾਂ ਜੇ (frp->fr_layout == FR_ਪੰਕਤੀ)
    {
	// Handle all the frames in the row.
	FOR_ALL_ਚੌਖਟੇ(frp, frp->fr_ਬੱਚਾ)
	    ਚੌਖਟਾ_ਜੋੜੋ_statusline(frp);
    }
    ਨਹੀਂਤਾਂ // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	ਲਈ (frp = frp->fr_ਬੱਚਾ; frp->fr_ਅਗਲਾ != ਨਿਰਾ; frp = frp->fr_ਅਗਲਾ)
	    ;
	ਚੌਖਟਾ_ਜੋੜੋ_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_new_ਚੌੜਾਈ(
    ਚੌਖਟਾ_T	*topfrp,
    ਪੂਰਨ		ਚੌੜਾਈ,
    ਪੂਰਨ		leftfirst,	// resize leftmost contained frame first
    ਪੂਰਨ		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    ਚੌਖਟਾ_T	*frp;
    ਪੂਰਨ		extra_ਖਾਨੇ_ਗਿਣਤੀ;
    ਪੂਰਨ		w;
    win_T	*wp;

    ਜੇ (topfrp->fr_layout == FR_ਪੱਤੀ)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	ਲਈ (frp = topfrp; frp->fr_ਮਾਪਾ != ਨਿਰਾ; frp = frp->fr_ਮਾਪਾ)
	    ਜੇ (frp->fr_ਮਾਪਾ->fr_layout == FR_ਪੰਕਤੀ && frp->fr_ਅਗਲਾ != ਨਿਰਾ)
		ਰੋਕੋ;
	ਜੇ (frp->fr_ਮਾਪਾ == ਨਿਰਾ)
	    wp->w_vsep_ਚੌੜਾਈ = 0;
	win_new_ਚੌੜਾਈ(wp, ਚੌੜਾਈ - wp->w_vsep_ਚੌੜਾਈ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (topfrp->fr_layout == FR_ਖਾਨਾ)
    {
	ਕਰੋ
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_ਚੌਖਟੇ(frp, topfrp->fr_ਬੱਚਾ)
	    {
		ਚੌਖਟਾ_new_ਚੌੜਾਈ(frp, ਚੌੜਾਈ, leftfirst, wfw);
		ਜੇ (frp->fr_ਚੌੜਾਈ > ਚੌੜਾਈ)
		{
		    // Could not fit the windows, make whole column wider.
		    ਚੌੜਾਈ = frp->fr_ਚੌੜਾਈ;
		    ਰੋਕੋ;
		}
	    }
	} ਜਦੋਂ (frp != ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_ਬੱਚਾ;
	ਜੇ (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    ਜਦੋਂ (ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(frp))
	    {
		frp = frp->fr_ਅਗਲਾ;
		ਜੇ (frp == ਨਿਰਾ)
		    ਵਾਪਸ;	    // no frame without 'wfw', give up
	    }
	ਜੇ (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    ਜਦੋਂ (frp->fr_ਅਗਲਾ != ਨਿਰਾ)
		frp = frp->fr_ਅਗਲਾ;
	    ਜੇ (wfw)
		// Advance back for frames with one window with 'wfw' set.
		ਜਦੋਂ (ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(frp))
		    frp = frp->fr_prev;
	}

	extra_ਖਾਨੇ_ਗਿਣਤੀ = ਚੌੜਾਈ - topfrp->fr_ਚੌੜਾਈ;
	ਜੇ (extra_ਖਾਨੇ_ਗਿਣਤੀ < 0)
	{
	    // reduce frame width, rightmost frame first
	    ਜਦੋਂ (frp != ਨਿਰਾ)
	    {
		w = ਚੌਖਟਾ_minwidth(frp, ਨਿਰਾ);
		ਜੇ (frp->fr_ਚੌੜਾਈ + extra_ਖਾਨੇ_ਗਿਣਤੀ < w)
		{
		    extra_ਖਾਨੇ_ਗਿਣਤੀ += frp->fr_ਚੌੜਾਈ - w;
		    ਚੌਖਟਾ_new_ਚੌੜਾਈ(frp, w, leftfirst, wfw);
		}
		ਨਹੀਂਤਾਂ
		{
		    ਚੌਖਟਾ_new_ਚੌੜਾਈ(frp, frp->fr_ਚੌੜਾਈ + extra_ਖਾਨੇ_ਗਿਣਤੀ,
							      leftfirst, wfw);
		    ਰੋਕੋ;
		}
		ਜੇ (leftfirst)
		{
		    ਕਰੋ
			frp = frp->fr_ਅਗਲਾ;
		    ਜਦੋਂ (wfw && frp != ਨਿਰਾ && ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(frp));
		}
		ਨਹੀਂਤਾਂ
		{
		    ਕਰੋ
			frp = frp->fr_prev;
		    ਜਦੋਂ (wfw && frp != ਨਿਰਾ && ਚੌਖਟਾ_fixed_ਚੌੜਾਈ(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		ਜੇ (frp == ਨਿਰਾ)
		    ਚੌੜਾਈ -= extra_ਖਾਨੇ_ਗਿਣਤੀ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (extra_ਖਾਨੇ_ਗਿਣਤੀ > 0)
	{
	    // increase width of rightmost frame
	    ਚੌਖਟਾ_new_ਚੌੜਾਈ(frp, frp->fr_ਚੌੜਾਈ + extra_ਖਾਨੇ_ਗਿਣਤੀ, leftfirst, wfw);
	}
    }
    topfrp->fr_ਚੌੜਾਈ = ਚੌੜਾਈ;
}

/*
 * Add or remove the vertical separator of windows to the right side of "frp".
 * Note: Does not check if there is room!
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_ਸੈੱਟ_vsep(ਚੌਖਟਾ_T *frp, ਪੂਰਨ ਜੋੜੋ)
{
    win_T	*wp;

    ਜੇ (frp->fr_layout == FR_ਪੱਤੀ)
    {
	wp = frp->fr_win;
	ਜੇ (ਜੋੜੋ && wp->w_vsep_ਚੌੜਾਈ == 0)
	{
	    ਜੇ (wp->w_ਚੌੜਾਈ > 0)	// don't make it negative
		win_new_ਚੌੜਾਈ(wp, wp->w_ਚੌੜਾਈ - 1);
	    wp->w_vsep_ਚੌੜਾਈ = 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!ਜੋੜੋ && wp->w_vsep_ਚੌੜਾਈ == 1)
	{
	    win_new_ਚੌੜਾਈ(wp, wp->w_ਚੌੜਾਈ + 1);
	    wp->w_vsep_ਚੌੜਾਈ = 0;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (frp->fr_layout == FR_ਖਾਨਾ)
    {
	// Handle all the frames in the column.
	FOR_ALL_ਚੌਖਟੇ(frp, frp->fr_ਬੱਚਾ)
	    ਚੌਖਟਾ_ਸੈੱਟ_vsep(frp, ਜੋੜੋ);
    }
    ਨਹੀਂਤਾਂ // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_ਬੱਚਾ;
	ਜਦੋਂ (frp->fr_ਅਗਲਾ != ਨਿਰਾ)
	    frp = frp->fr_ਅਗਲਾ;
	ਚੌਖਟਾ_ਸੈੱਟ_vsep(frp, ਜੋੜੋ);
    }
}

/*
 * Set frame width from the window it contains.
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_fix_ਚੌੜਾਈ(win_T *wp)
{
    wp->w_ਚੌਖਟਾ->fr_ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ + wp->w_vsep_ਚੌੜਾਈ;
}

/*
 * Set frame height from the window it contains.
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_fix_ਉਚਾਈ(win_T *wp)
{
    wp->w_ਚੌਖਟਾ->fr_ਉਚਾਈ = VISIBLE_ਉਚਾਈ(wp) + wp->w_ਸਥਿਤੀ_ਉਚਾਈ;
}

/*
 * Compute the minimal height for frame "topfrp".
 * Uses the 'winminheight' option.
 * When "next_curwin" isn't NULL, use p_wh for this window.
 * When "next_curwin" is NOWIN, don't use at least one line for the current
 * window.
 */
    ਸਥਿਰ ਪੂਰਨ
ਚੌਖਟਾ_minheight(ਚੌਖਟਾ_T *topfrp, win_T *ਅਗਲਾ_curwin)
{
    ਚੌਖਟਾ_T	*frp;
    ਪੂਰਨ		m;
    ਪੂਰਨ		n;

    ਜੇ (topfrp->fr_win != ਨਿਰਾ)
    {
	ਜੇ (topfrp->fr_win == ਅਗਲਾ_curwin)
	    m = p_wh + topfrp->fr_win->w_ਸਥਿਤੀ_ਉਚਾਈ;
	ਨਹੀਂਤਾਂ
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_ਸਥਿਤੀ_ਉਚਾਈ;
	    ਜੇ (topfrp->fr_win == curwin && ਅਗਲਾ_curwin == ਨਿਰਾ)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		ਜੇ (p_wmh == 0)
		    ++m;
		m += WINBAR_ਉਚਾਈ(curwin);
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (topfrp->fr_layout == FR_ਪੰਕਤੀ)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_ਚੌਖਟੇ(frp, topfrp->fr_ਬੱਚਾ)
	{
	    n = ਚੌਖਟਾ_minheight(frp, ਅਗਲਾ_curwin);
	    ਜੇ (n > m)
		m = n;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_ਚੌਖਟੇ(frp, topfrp->fr_ਬੱਚਾ)
	    m += ਚੌਖਟਾ_minheight(frp, ਅਗਲਾ_curwin);
    }

    ਵਾਪਸ m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
ਸਥਿਰ ਪੂਰਨ
ਚੌਖਟਾ_wincount_ਵਿੱਚ_ਉਚਾਈ(ਚੌਖਟਾ_T *topfrp)
{
    ਚੌਖਟਾ_T	*frp;
    ਪੂਰਨ		c = 0;
    ਪੂਰਨ		n;

    ਜੇ (topfrp->fr_win != ਨਿਰਾ)
	c = 1;
    ਨਹੀਂਤਾਂ ਜੇ (topfrp->fr_layout == FR_ਪੰਕਤੀ)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_ਚੌਖਟੇ(frp, topfrp->fr_ਬੱਚਾ)
	{
	    n = ਚੌਖਟਾ_wincount_ਵਿੱਚ_ਉਚਾਈ(frp);
	    ਜੇ (n > c)
		c = n;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_ਚੌਖਟੇ(frp, topfrp->fr_ਬੱਚਾ)
	    c += ਚੌਖਟਾ_wincount_ਵਿੱਚ_ਉਚਾਈ(frp);
    }

    ਵਾਪਸ c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    ਸਥਿਰ ਪੂਰਨ
ਚੌਖਟਾ_minwidth(
    ਚੌਖਟਾ_T	*topfrp,
    win_T	*ਅਗਲਾ_curwin)	// use p_wh and p_wiw for next_curwin
{
    ਚੌਖਟਾ_T	*frp;
    ਪੂਰਨ		m, n;

    ਜੇ (topfrp->fr_win != ਨਿਰਾ)
    {
	ਜੇ (topfrp->fr_win == ਅਗਲਾ_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_ਚੌੜਾਈ;
	ਨਹੀਂਤਾਂ
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_ਚੌੜਾਈ;
	    // Current window is minimal one column wide
	    ਜੇ (p_wmw == 0 && topfrp->fr_win == curwin && ਅਗਲਾ_curwin == ਨਿਰਾ)
		++m;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (topfrp->fr_layout == FR_ਖਾਨਾ)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_ਚੌਖਟੇ(frp, topfrp->fr_ਬੱਚਾ)
	{
	    n = ਚੌਖਟਾ_minwidth(frp, ਅਗਲਾ_curwin);
	    ਜੇ (n > m)
		m = n;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_ਚੌਖਟੇ(frp, topfrp->fr_ਬੱਚਾ)
	    m += ਚੌਖਟਾ_minwidth(frp, ਅਗਲਾ_curwin);
    }

    ਵਾਪਸ m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    ਖਾਲੀ
close_others(
    ਪੂਰਨ		ਸੁਨੇਹਾ_2,
    ਪੂਰਨ		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    ਪੂਰਨ		r;

    ਜੇ (one_ਖਿੜਕੀ())
    {
	ਜੇ (ਸੁਨੇਹਾ_2 && !autocmd_busy)
	    ਸੁਨੇਹਾ(_(m_onlyone));
	ਵਾਪਸ;
    }

    // Be very careful here: autocommands may change the window layout.
    ਲਈ (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_ਅਗਲਾ;

	// autocommands messed this one up
	ਜੇ (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_ਬਫਰ;
	}

	ਜੇ (wp == curwin)		// don't close current window
	    ਜਾਰੀ;

	// autoccommands messed this one up
	ਜੇ (!buf_valid(wp->w_ਬਫਰ) && win_valid(wp))
	{
	    wp->w_ਬਫਰ = ਨਿਰਾ;
	    win_close(wp, 0);
	    ਜਾਰੀ;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_ਬਫਰ, forceit);
	ਜੇ (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    ਜਾਰੀ;
	}
	ਜੇ (!r)
	{
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ਜੇ (ਸੁਨੇਹਾ_2 && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_ਬਦਲਿਆ(wp->w_ਬਫਰ, FALSE);
		ਜੇ (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    ਜਾਰੀ;
		}
	    }
	    ਜੇ (buf_Is_ਬਦਲਿਆ(wp->w_ਬਫਰ))
#ਜੇ_ਅੰਤ
		ਜਾਰੀ;
	}
	win_close(wp, !buf_hide(wp->w_ਬਫਰ) && !buf_Is_ਬਦਲਿਆ(wp->w_ਬਫਰ));
    }

    ਜੇ (ਸੁਨੇਹਾ_2 && !ONE_ਖਿੜਕੀ)
	emsg(_(e_other_ਖਿੜਕੀ_ਸ਼ਾਮਲ_ਹੈ_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    ਖਾਲੀ
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
ਸਥਿਰ ਪੂਰਨ command_ਚੌਖਟਾ_ਉਚਾਈ = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    ਖਾਲੀ
use_tabpage(tabpage_T *tp)
{
    curtab = tp;
    topframe = curtab->tp_topframe;
    firstwin = curtab->tp_firstwin;
    lastwin = curtab->tp_lastwin;
    curwin = curtab->tp_curwin;
}

/*
 * Allocate the first window and put an empty buffer in it.
 * Called from main().
 * Return FAIL when something goes wrong (out of memory).
 */
    ਪੂਰਨ
win_alloc_ਪਹਿਲਾ(ਖਾਲੀ)
{
    ਜੇ (win_alloc_firstwin(ਨਿਰਾ) == FAIL)
	ਵਾਪਸ FAIL;

    ਪਹਿਲਾ_tabpage = alloc_tabpage();
    ਜੇ (ਪਹਿਲਾ_tabpage == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    curtab = ਪਹਿਲਾ_tabpage;
    unuse_tabpage(ਪਹਿਲਾ_tabpage);

    ਵਾਪਸ OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(ਖਾਲੀ)
{
    win_T *wp;

    wp = win_alloc(ਨਿਰਾ, TRUE);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_ਚੌਖਟਾ(wp);
    ਵਾਪਸ wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    ਖਾਲੀ
win_init_popup_win(win_T *wp, buf_T *buf)
{
    wp->w_ਬਫਰ = buf;
    ++buf->b_nwindows;
    win_init_ਖਾਲੀ_ਥਾਵਾਂ(wp); // set cursor and topline to safe values

    // Make sure w_localdir is NULL to avoid a chdir() in win_enter_ext().
    VIM_CLEAR(wp->w_localdir);
}

/*
 * Allocate the first window or the first window in a new tab page.
 * When "oldwin" is NULL create an empty buffer for it.
 * When "oldwin" is not NULL copy info from it to the new window.
 * Return FAIL when something goes wrong (out of memory).
 */
    ਸਥਿਰ ਪੂਰਨ
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(ਨਿਰਾ, FALSE);
    ਜੇ (curwin == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (oldwin == ਨਿਰਾ)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(ਨਿਰਾ, ਨਿਰਾ, 1L, BLN_LISTED);
	ਜੇ (curwin == ਨਿਰਾ || curbuf == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	curwin->w_ਬਫਰ = curbuf;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#ਜੇ_ਅੰਤ
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    ਨਹੀਂਤਾਂ
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_ਚੌਖਟਾ(curwin);
    ਜੇ (curwin->w_ਚੌਖਟਾ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    topframe = curwin->w_ਚੌਖਟਾ;
    topframe->fr_ਚੌੜਾਈ = COLUMNS_WITHOUT_TPL();
    topframe->fr_ਉਚਾਈ = ਪੰਕਤੀਆਂ - p_ch;

    ਵਾਪਸ OK;
}

/*
 * Create a frame for window "wp".
 */
    ਸਥਿਰ ਖਾਲੀ
new_ਚੌਖਟਾ(win_T *wp)
{
    ਚੌਖਟਾ_T *frp = ALLOC_CLEAR_ONE(ਚੌਖਟਾ_T);

    wp->w_ਚੌਖਟਾ = frp;
    ਜੇ (frp == ਨਿਰਾ)
	ਵਾਪਸ;
    frp->fr_layout = FR_ਪੱਤੀ;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    ਖਾਲੀ
win_init_ਮਾਪ(ਖਾਲੀ)
{
    firstwin->w_ਉਚਾਈ = ਪੰਕਤੀਆਂ_AVAIL;
    firstwin->w_prev_ਉਚਾਈ = ਪੰਕਤੀਆਂ_AVAIL;
    topframe->fr_ਉਚਾਈ = ਪੰਕਤੀਆਂ_AVAIL;
    firstwin->w_ਚੌੜਾਈ = topframe->fr_ਚੌੜਾਈ;
}

/*
 * Allocate a new tabpage_T and init the values.
 * Returns NULL when out of memory.
 */
    ਸਥਿਰ tabpage_T *
alloc_tabpage(ਖਾਲੀ)
{
    tabpage_T	*tp;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਪੂਰਨ		i;
#ਜੇ_ਅੰਤ


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    ਜੇ (tp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    ਜੇ (tp->tp_vars == ਨਿਰਾ)
    {
	vim_free(tp);
	ਵਾਪਸ ਨਿਰਾ;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਲਈ (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#ਜੇ_ਅੰਤ
    tp->tp_ch_ਵਰਤਿਆ = p_ch;

    ਵਾਪਸ tp;
}

    ਖਾਲੀ
free_tabpage(tabpage_T *tp)
{
    ਪੂਰਨ idx;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    diff_clear(tp);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜਦੋਂ (tp->tp_ਪਹਿਲਾ_popupwin != ਨਿਰਾ)
	popup_close_tabpage(tp, tp->tp_ਪਹਿਲਾ_popupwin->w_id, TRUE);
#ਜੇ_ਅੰਤ
    ਲਈ (idx = 0; idx < SNAP_ਗਿਣਤੀ; ++idx)
	clear_snapshot(tp, idx);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#ਜੇ_ਅੰਤ

    ਜੇ (tp == lastused_tabpage)
	lastused_tabpage = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TABPANEL
    tabpanel_forget_tabpage(tp);
#ਜੇ_ਅੰਤ

    vim_free(tp->tp_localdir);
    vim_free(tp->tp_prevdir);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
    python_tabpage_free(tp);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
    python3_tabpage_free(tp);
#ਜੇ_ਅੰਤ

    vim_free(tp);
}

/*
 * Create a new Tab page with one window.
 * It will edit the current buffer, like after ":split".
 * When "after" is 0 put it just after the current Tab page.
 * Otherwise put it just before tab page "after".
 *
 * Does not trigger WinNewPre, since the window structures
 * are not completely setup yet and could cause dereferencing
 * NULL pointers
 *
 * Return FAIL or OK.
 */
    ਪੂਰਨ
win_new_tabpage(ਪੂਰਨ after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    ਪੂਰਨ		n;
#ਜੇ defined(FEAT_TABPANEL)
    ਪੂਰਨ prev_columns = COLUMNS_WITHOUT_TPL();
#ਜੇ_ਅੰਤ

    ਜੇ (cmdwin_type != 0)
    {
	emsg(_(e_invalid_ਵਿੱਚ_cmdline_ਖਿੜਕੀ));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_tabnew))
	ਵਾਪਸ FAIL;

    newtp = alloc_tabpage();
    ਜੇ (newtp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // Remember the current windows in this Tab page.
    ਜੇ (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	ਵਾਪਸ FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == ਨਿਰਾ)
				    ? ਨਿਰਾ : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    ਜੇ (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	ਜੇ (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_ਅਗਲਾ = ਪਹਿਲਾ_tabpage;
	    ਪਹਿਲਾ_tabpage = newtp;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp->tp_ਅਗਲਾ != ਨਿਰਾ
					       && n < after; tp = tp->tp_ਅਗਲਾ)
		    ++n;
	    }
	    newtp->tp_ਅਗਲਾ = tp->tp_ਅਗਲਾ;
	    tp->tp_ਅਗਲਾ = newtp;
	}
	newtp->tp_firstwin = newtp->tp_lastwin = newtp->tp_curwin = curwin;

	win_init_ਮਾਪ();
	firstwin->w_winrow = tabline_ਉਚਾਈ();
	firstwin->w_prev_winrow = firstwin->w_winrow;
	win_comp_scroll(curwin);

	newtp->tp_topframe = topframe;
	ਆਖਰੀ_ਸਥਿਤੀ(FALSE);

	lastused_tabpage = prev_tp;

#ਜੇ defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	entering_ਖਿੜਕੀ(curwin);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_TABPANEL)
	ਜੇ (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    ਪੂਰਨ		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		ਜੇ (tp2 != curtab)
		{
		    ਚੌਖਟਾ_new_ਚੌੜਾਈ(tp2->tp_topframe, w, FALSE, TRUE);
		    ਜੇ (!ਚੌਖਟਾ_check_ਚੌੜਾਈ(tp2->tp_topframe, w))
			ਚੌਖਟਾ_new_ਚੌੜਾਈ(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#ਜੇ_ਅੰਤ
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	ਵਾਪਸ OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    ਵਾਪਸ FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
may_open_tabpage(ਖਾਲੀ)
{
    ਪੂਰਨ		n = (cmdmod.cmod_tab == 0)
				       ? postponed_ਵੰਡੋ_tab : cmdmod.cmod_tab;

    ਜੇ (n == 0)
	ਵਾਪਸ FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_ਵੰਡੋ_tab = 0;
    ਵਾਪਸ win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    ਪੂਰਨ
make_tabpages(ਪੂਰਨ maxcount)
{
    ਪੂਰਨ		ਗਿਣਤੀ_2 = maxcount;
    ਪੂਰਨ		todo;

    // Limit to 'tabpagemax' tabs.
    ਜੇ (ਗਿਣਤੀ_2 > p_tpm)
	ਗਿਣਤੀ_2 = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    ਖੰਡ_autocmds();

    ਲਈ (todo = ਗਿਣਤੀ_2 - 1; todo > 0; --todo)
	ਜੇ (win_new_tabpage(0) == FAIL)
	    ਰੋਕੋ;

    unblock_autocmds();

    // return actual number of tab pages
    ਵਾਪਸ (ਗਿਣਤੀ_2 - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    ਪੂਰਨ
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	ਜੇ (tp == tpc)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    ਪੂਰਨ
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	ਜੇ (tp == tpc)
	{
	    FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp)
	    {
		ਜੇ (win_valid_any_tab(wp))
		    ਵਾਪਸ TRUE;
	    }
	    ਵਾਪਸ FALSE;
	}
    }
    // shouldn't happen
    ਵਾਪਸ FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    ਖਾਲੀ
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    ਜੇ (tab == ਪਹਿਲਾ_tabpage)
    {
	ਪਹਿਲਾ_tabpage = tab->tp_ਅਗਲਾ;
	ptp = ਪਹਿਲਾ_tabpage;
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (ptp = ਪਹਿਲਾ_tabpage; ptp != ਨਿਰਾ && ptp->tp_ਅਗਲਾ != tab;
							    ptp = ptp->tp_ਅਗਲਾ)
	    ;
	ਪ੍ਰਤਿਜ੍ਞਾ(ptp != ਨਿਰਾ);
	ptp->tp_ਅਗਲਾ = tab->tp_ਅਗਲਾ;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
ਲੱਭੋ_tabpage(ਪੂਰਨ n)
{
    tabpage_T	*tp;
    ਪੂਰਨ		i = 1;

    ਜੇ (n == 0)
	ਵਾਪਸ curtab;

    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ && i != n; tp = tp->tp_ਅਗਲਾ)
	++i;
    ਵਾਪਸ tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    ਪੂਰਨ
tabpage_ਸੂਚਕਾਂਕ(tabpage_T *ftp)
{
    ਪੂਰਨ		i = 1;
    tabpage_T	*tp;

    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ && tp != ftp; tp = tp->tp_ਅਗਲਾ)
	++i;
    ਵਾਪਸ i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    ਸਥਿਰ ਪੂਰਨ
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    ਪੂਰਨ		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    leaving_ਖਿੜਕੀ(curwin);
#ਜੇ_ਅੰਤ
    reset_VIsual_and_resel();	// stop Visual mode
    ਜੇ (trigger_leave_autocmds)
    {
	ਜੇ (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	    ਜੇ (curtab != tp)
		ਵਾਪਸ FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	ਜੇ (curtab != tp)
	    ਵਾਪਸ FAIL;
	apply_autocmds(EVENT_TABLEAVE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	ਜੇ (curtab != tp)
	    ਵਾਪਸ FAIL;
    }

    reset_dragwin();
#ਜੇ defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_remove_scrollbars();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
    // Clear all image placements, they will be redrawn again if needed.
    clear_all_image_placements();
#ਜੇ_ਅੰਤ
    tp->tp_curwin = curwin;
    tp->tp_prevwin = prevwin;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_old_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ_AVAIL;
    ਜੇ (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_ਚੌੜਾਈ;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = ਨਿਰਾ;
    lastwin = ਨਿਰਾ;
    ਵਾਪਸ OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    ਸਥਿਰ ਖਾਲੀ
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    ਪੂਰਨ		trigger_enter_autocmds,
    ਪੂਰਨ		trigger_leave_autocmds)
{
    ਪੂਰਨ		old_off = tp->tp_firstwin->w_winrow;
    win_T	*ਅਗਲਾ_prevwin = tp->tp_prevwin;
    tabpage_T	*ਆਖਰੀ_tab = curtab;

    use_tabpage(tp);

    ਜੇ (p_ch != curtab->tp_ch_ਵਰਤਿਆ)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	ਪੂਰਨ new_ch = curtab->tp_ch_ਵਰਤਿਆ;
	curtab->tp_ch_ਵਰਤਿਆ = p_ch;
	command_ਚੌਖਟਾ_ਉਚਾਈ = FALSE;
	ਸੈੱਟ_option_ਮੁੱਲ((char_u *)"cmdheight", new_ch, ਨਿਰਾ, 0);
	command_ਚੌਖਟਾ_ਉਚਾਈ = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (ਖਾਲੀ)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
		  | (trigger_enter_autocmds ? WEE_TRIGGER_ENTER_AUTOCMDS : 0)
		  | (trigger_leave_autocmds ? WEE_TRIGGER_LEAVE_AUTOCMDS : 0));
    prevwin = ਅਗਲਾ_prevwin;

    ਆਖਰੀ_ਸਥਿਤੀ(FALSE);		// status line may appear or disappear
    win_comp_pos();		// recompute w_winrow for all windows
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    diff_need_scrollbind = TRUE;
#ਜੇ_ਅੰਤ

    // If there was a click in a window, it won't be usable for a following
    // drag.
    reset_dragwin();

    // The tabpage line may have appeared or disappeared, may need to resize
    // the frames for that.  When the Vim window was resized need to update
    // frame sizes too.
    ਜੇ (curtab->tp_old_ਪੰਕਤੀਆਂ != ਪੰਕਤੀਆਂ_AVAIL || (old_off != firstwin->w_winrow
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#ਜੇ_ਅੰਤ
		))
	shell_new_ਪੰਕਤੀਆਂ();
    ਜੇ (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	ਜੇ (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_ਚੌੜਾਈ;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	ਨਹੀਂਤਾਂ
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = ਆਖਰੀ_tab;

#ਜੇ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#ਜੇ_ਅੰਤ

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    ਜੇ (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	ਜੇ (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    ਖਾਲੀ
goto_tabpage(ਪੂਰਨ n)
{
    tabpage_T	*tp = ਨਿਰਾ;  // shut up compiler
    tabpage_T	*ttp;
    ਪੂਰਨ		i;

    ਜੇ (ਪਾਠ_locked())
    {
	// Not allowed when editing the command line.
	ਪਾਠ_locked_ਸੁਨੇਹਾ();
	ਵਾਪਸ;
    }

    // If there is only one it can't work.
    ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ == ਨਿਰਾ)
    {
	ਜੇ (n > 1)
	    beep_flush();
	ਵਾਪਸ;
    }

    ਜੇ (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	ਜੇ (curtab->tp_ਅਗਲਾ == ਨਿਰਾ)
	    tp = ਪਹਿਲਾ_tabpage;
	ਨਹੀਂਤਾਂ
	    tp = curtab->tp_ਅਗਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	ਲਈ (i = n; i < 0; ++i)
	{
	    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp->tp_ਅਗਲਾ != ttp && tp->tp_ਅਗਲਾ != ਨਿਰਾ;
		    tp = tp->tp_ਅਗਲਾ)
		;
	    ttp = tp;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (n == 9999)
    {
	// Go to last tab page.
	ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp->tp_ਅਗਲਾ != ਨਿਰਾ; tp = tp->tp_ਅਗਲਾ)
	    ;
    }
    ਨਹੀਂਤਾਂ
    {
	// Go to tab page "n".
	tp = ਲੱਭੋ_tabpage(n);
	ਜੇ (tp == ਨਿਰਾ)
	{
	    beep_flush();
	    ਵਾਪਸ;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    ਜੇ (gui_use_tabline())
	gui_mch_ਸੈੱਟ_curtab(tabpage_ਸੂਚਕਾਂਕ(curtab));
#ਜੇ_ਅੰਤ
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    ਖਾਲੀ
goto_tabpage_tp(
    tabpage_T	*tp,
    ਪੂਰਨ		trigger_enter_autocmds,
    ਪੂਰਨ		trigger_leave_autocmds)
{
    ਜੇ (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    ਸੈੱਟ_keep_ਸੁਨੇਹਾ(ਨਿਰਾ, 0);

    skip_win_fix_scroll = TRUE;
    ਜੇ (tp != curtab && leave_tabpage(tp->tp_curwin->w_ਬਫਰ,
					trigger_leave_autocmds) == OK)
    {
	ਜੇ (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	ਨਹੀਂਤਾਂ
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    ਪੂਰਨ
goto_tabpage_lastused(ਖਾਲੀ)
{
    ਜੇ (!valid_tabpage(lastused_tabpage))
	ਵਾਪਸ FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    ਵਾਪਸ OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    ਖਾਲੀ
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    ਜੇ (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
	ਜੇ (gui_use_tabline())
	    gui_mch_ਸੈੱਟ_curtab(tabpage_ਸੂਚਕਾਂਕ(curtab));
#ਜੇ_ਅੰਤ
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    ਖਾਲੀ
tabpage_move(ਪੂਰਨ nr)
{
    ਪੂਰਨ		n = 1;
    tabpage_T	*tp, *tp_dst;

    ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (tabpage_move_disallowed)
	ਵਾਪਸ;

    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp->tp_ਅਗਲਾ != ਨਿਰਾ && n < nr; tp = tp->tp_ਅਗਲਾ)
	++n;

    ਜੇ (tp == curtab || (nr > 0 && tp->tp_ਅਗਲਾ != ਨਿਰਾ
						    && tp->tp_ਅਗਲਾ == curtab))
	ਵਾਪਸ;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    ਜੇ (curtab == ਪਹਿਲਾ_tabpage)
	ਪਹਿਲਾ_tabpage = curtab->tp_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
    {
	FOR_ALL_TABPAGES(tp)
	    ਜੇ (tp->tp_ਅਗਲਾ == curtab)
		ਰੋਕੋ;
	ਜੇ (tp == ਨਿਰਾ)	// "cannot happen"
	    ਵਾਪਸ;
	tp->tp_ਅਗਲਾ = curtab->tp_ਅਗਲਾ;
    }

    // Re-insert it at the specified position.
    ਜੇ (nr <= 0)
    {
	curtab->tp_ਅਗਲਾ = ਪਹਿਲਾ_tabpage;
	ਪਹਿਲਾ_tabpage = curtab;
    }
    ਨਹੀਂਤਾਂ
    {
	curtab->tp_ਅਗਲਾ = tp_dst->tp_ਅਗਲਾ;
	tp_dst->tp_ਅਗਲਾ = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
}


/*
 * Go to another window.
 * When jumping to another buffer, stop Visual mode.  Do this before
 * changing windows so we can yank the selection into the '*' register.
 * (note: this may trigger ModeChanged autocommand!)
 * When jumping to another window on the same buffer, adjust its cursor
 * position to keep the same Visual area.
 */
    ਖਾਲੀ
win_goto(win_T *wp)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    win_T	*owp = curwin;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;
    ਜੇ (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_ਮੰਜ਼ਿਲ_enter_popup_ਖਿੜਕੀ));
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (ਪਾਠ_or_buf_locked())
    {
	beep_flush();
	ਵਾਪਸ;
    }

    ਜੇ (wp->w_ਬਫਰ != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    ਨਹੀਂਤਾਂ ਜੇ (VIsual_ਸਰਗਰਮ)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    ਜੇ (!win_valid(wp))
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
    win_enter(wp, TRUE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    ਜੇ (win_valid(owp) && owp->w_p_cole > 0 && !ਸੁਨੇਹਾ_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    ਜੇ (curwin->w_p_cole > 0 && !ਸੁਨੇਹਾ_scrolled)
	need_cursor_ਸਤਰ_redraw = TRUE;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_ਲੱਭੋ_nr(ਪੂਰਨ winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (--winnr == 0)
	    ਰੋਕੋ;
    ਵਾਪਸ wp;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_ਲੱਭੋ_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    ਜੇ (wp == win)
		ਵਾਪਸ tp;
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, ਪੂਰਨ ਉੱਪਰ, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    ਚੌਖਟਾ_T	*fr;
    ਚੌਖਟਾ_T	*nfr;
    ਚੌਖਟਾ_T	*foundfr;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (popup_is_popup(wp))
	// popups don't have neighbors.
	ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    foundfr = wp->w_ਚੌਖਟਾ;
    ਜਦੋਂ (ਗਿਣਤੀ_2--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	ਲਈ (;;)
	{
	    ਜੇ (fr == tp->tp_topframe)
		ਜਾਓ ਸਿਰਾ;
	    ਜੇ (ਉੱਪਰ)
		nfr = fr->fr_prev;
	    ਨਹੀਂਤਾਂ
		nfr = fr->fr_ਅਗਲਾ;
	    ਜੇ (fr->fr_ਮਾਪਾ->fr_layout == FR_ਖਾਨਾ && nfr != ਨਿਰਾ)
		ਰੋਕੋ;
	    fr = fr->fr_ਮਾਪਾ;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	ਲਈ (;;)
	{
	    ਜੇ (nfr->fr_layout == FR_ਪੱਤੀ)
	    {
		foundfr = nfr;
		ਰੋਕੋ;
	    }
	    fr = nfr->fr_ਬੱਚਾ;
	    ਜੇ (nfr->fr_layout == FR_ਪੰਕਤੀ)
	    {
		// Find the frame at the cursor row.
		ਜਦੋਂ (fr->fr_ਅਗਲਾ != ਨਿਰਾ
			&& frame2win(fr)->w_wincol + fr->fr_ਚੌੜਾਈ
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_ਅਗਲਾ;
	    }
	    ਜੇ (nfr->fr_layout == FR_ਖਾਨਾ && ਉੱਪਰ)
		ਜਦੋਂ (fr->fr_ਅਗਲਾ != ਨਿਰਾ)
		    fr = fr->fr_ਅਗਲਾ;
	    nfr = fr;
	}
    }
ਸਿਰਾ:
    ਵਾਪਸ foundfr != ਨਿਰਾ ? foundfr->fr_win : ਨਿਰਾ;
}

/*
 * Move to window above or below "count" times.
 */
    ਸਥਿਰ ਖਾਲੀ
win_goto_ver(
    ਪੂਰਨ		ਉੱਪਰ,		// TRUE to go to win above
    ਲੰਮਾ	ਗਿਣਤੀ_2)
{
    win_T	*win;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (ERROR_IF_ਪਦ_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    win = win_vert_neighbor(curtab, curwin, ਉੱਪਰ, ਗਿਣਤੀ_2);
    ਜੇ (win != ਨਿਰਾ)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, ਪੂਰਨ ਖੱਬਾ, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    ਚੌਖਟਾ_T	*fr;
    ਚੌਖਟਾ_T	*nfr;
    ਚੌਖਟਾ_T	*foundfr;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (popup_is_popup(wp))
	// popups don't have neighbors.
	ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    foundfr = wp->w_ਚੌਖਟਾ;
    ਜਦੋਂ (ਗਿਣਤੀ_2--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	ਲਈ (;;)
	{
	    ਜੇ (fr == tp->tp_topframe)
		ਜਾਓ ਸਿਰਾ;
	    ਜੇ (ਖੱਬਾ)
		nfr = fr->fr_prev;
	    ਨਹੀਂਤਾਂ
		nfr = fr->fr_ਅਗਲਾ;
	    ਜੇ (fr->fr_ਮਾਪਾ->fr_layout == FR_ਪੰਕਤੀ && nfr != ਨਿਰਾ)
		ਰੋਕੋ;
	    fr = fr->fr_ਮਾਪਾ;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	ਲਈ (;;)
	{
	    ਜੇ (nfr->fr_layout == FR_ਪੱਤੀ)
	    {
		foundfr = nfr;
		ਰੋਕੋ;
	    }
	    fr = nfr->fr_ਬੱਚਾ;
	    ਜੇ (nfr->fr_layout == FR_ਖਾਨਾ)
	    {
		// Find the frame at the cursor row.
		ਜਦੋਂ (fr->fr_ਅਗਲਾ != ਨਿਰਾ
			&& frame2win(fr)->w_winrow + fr->fr_ਉਚਾਈ
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_ਅਗਲਾ;
	    }
	    ਜੇ (nfr->fr_layout == FR_ਪੰਕਤੀ && ਖੱਬਾ)
		ਜਦੋਂ (fr->fr_ਅਗਲਾ != ਨਿਰਾ)
		    fr = fr->fr_ਅਗਲਾ;
	    nfr = fr;
	}
    }
ਸਿਰਾ:
    ਵਾਪਸ foundfr != ਨਿਰਾ ? foundfr->fr_win : ਨਿਰਾ;
}

/*
 * Move to left or right window.
 */
    ਸਥਿਰ ਖਾਲੀ
win_goto_hor(
    ਪੂਰਨ		ਖੱਬਾ,		// TRUE to go to left win
    ਲੰਮਾ	ਗਿਣਤੀ_2)
{
    win_T	*win;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (ERROR_IF_ਪਦ_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    win = win_horz_neighbor(curtab, curwin, ਖੱਬਾ, ਗਿਣਤੀ_2);
    ਜੇ (win != ਨਿਰਾ)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    ਖਾਲੀ
win_enter(win_T *wp, ਪੂਰਨ undo_sync)
{
    (ਖਾਲੀ)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    ਖਾਲੀ
win_fix_current_dir(ਖਾਲੀ)
{
    ਜੇ (curwin->w_localdir != ਨਿਰਾ || curtab->tp_localdir != ਨਿਰਾ)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	ਜੇ (globaldir == ਨਿਰਾ)
	{
	    char_u	cwd[MAXPATHL];

	    ਜੇ (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	ਜੇ (curwin->w_localdir != ਨਿਰਾ)
	    dirname = curwin->w_localdir;
	ਨਹੀਂਤਾਂ
	    dirname = curtab->tp_localdir;

	ਜੇ (mch_chdir((ਅੱਖਰ *)dirname) == 0)
	{
	    ਆਖਰੀ_chdir_reason = ਨਿਰਾ;
	    shorten_fnames(TRUE);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (globaldir != ਨਿਰਾ)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((ਅੱਖਰ *)globaldir);
	VIM_CLEAR(globaldir);
	ਆਖਰੀ_chdir_reason = ਨਿਰਾ;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    ਸਥਿਰ ਪੂਰਨ
win_enter_ext(win_T *wp, ਪੂਰਨ flags)
{
    ਪੂਰਨ		other_ਬਫਰ = FALSE;
    ਪੂਰਨ		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    ਪੂਰਨ		did_decrement = FALSE;

    ਜੇ (wp == curwin && curwin_invalid == 0)	// nothing to do
	ਵਾਪਸ FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਜੇ (curwin_invalid == 0)
	leaving_ਖਿੜਕੀ(curwin);
#ਜੇ_ਅੰਤ

    ਜੇ (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	ਜੇ (wp->w_ਬਫਰ != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	    other_ਬਫਰ = TRUE;
	    ਜੇ (!win_valid(wp))
		ਵਾਪਸ FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	ਜੇ (!win_valid(wp))
	    ਵਾਪਸ FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// autocmds may abort script processing
	ਜੇ (aborting())
	    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
    }

    // sync undo before leaving the current buffer
    ਜੇ ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_ਬਫਰ)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    ਜੇ (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    ਜੇ (wp->w_ਬਫਰ != curbuf)
	buf_copy_options(wp->w_ਬਫਰ, BCO_ENTER | BCO_NOHELP);
    ਜੇ (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_ਸਥਿਤੀ = true;
    }
    curwin = wp;
    curbuf = wp->w_ਬਫਰ;
    check_cursor();
    ਜੇ (!virtual_ਸਰਗਰਮ())
	curwin->w_cursor.coladd = 0;
    ਜੇ (*p_spk == 'c')		// assume cursor position needs updating
	ਬਦਲਿਆ_ਸਤਰ_abv_curs();
    ਨਹੀਂਤਾਂ
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (ਬਹੁਲਕ_ਸਾਧਾਰਨ|ਬਹੁਲਕ_CMDLINE|ਬਹੁਲਕ_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
    ਜੇ (flags & WEE_ALLOW_PARSE_MESSAGES)
    {
	--dont_parse_messages;
	did_decrement = TRUE;
    }
#ਜੇ_ਅੰਤ

    win_fix_current_dir();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    entering_ਖਿੜਕੀ(curwin);
#ਜੇ_ਅੰਤ
    // Careful: autocommands may close the window and make "wp" invalid
    ਜੇ (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    ਜੇ (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	ਜੇ (other_ਬਫਰ)
	    apply_autocmds(EVENT_BUFENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_ਸਥਿਤੀ = true;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (bt_terminal(curwin->w_ਬਫਰ))
	// terminal is likely in another mode
	redraw_ਬਹੁਲਕ = TRUE;
#ਜੇ_ਅੰਤ
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    ਜੇ (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    ਜੇ (curwin->w_ਉਚਾਈ < p_wh && !curwin->w_p_wfh
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#ਜੇ_ਅੰਤ
	    )
	win_setheight((ਪੂਰਨ)p_wh);
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_ਉਚਾਈ == 0)
	win_setheight(1);

    // set window width to desired minimal value
    ਜੇ (curwin->w_ਚੌੜਾਈ < p_wiw && !curwin->w_p_wfw)
	win_setwidth((ਪੂਰਨ)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    ਵਾਪਸ did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = ਨਿਰਾ;

    ਜੇ (curwin->w_ਬਫਰ == buf)
	wp = curwin;
    ਨਹੀਂਤਾਂ
	FOR_ALL_WINDOWS(wp)
	    ਜੇ (wp->w_ਬਫਰ == buf)
		ਰੋਕੋ;
    ਜੇ (wp != ਨਿਰਾ)
	win_enter(wp, FALSE);
    ਵਾਪਸ wp;
}

/*
 * Jump to the first open window in any tab page that contains buffer "buf",
 * if one exists. First search in the windows present in the current tab page.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_tab(buf_T *buf)
{
    win_T	*wp = buf_jump_open_win(buf);
    tabpage_T	*tp;

    ਜੇ (wp != ਨਿਰਾ)
	ਵਾਪਸ wp;

    FOR_ALL_TABPAGES(tp)
	ਜੇ (tp != curtab)
	{
	    FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp)
		ਜੇ (wp->w_ਬਫਰ == buf)
		    ਰੋਕੋ;
	    ਜੇ (wp != ਨਿਰਾ)
	    {
		goto_tabpage_win(tp, wp);
		ਜੇ (curwin != wp)
		    wp = ਨਿਰਾ;	// something went wrong
		ਰੋਕੋ;
	    }
	}
    ਵਾਪਸ wp;
}

ਸਥਿਰ ਪੂਰਨ ਆਖਰੀ_win_id = ਸਭ_ਤੋਂ_ਨੀਵਾਂ_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    ਸਥਿਰ win_T *
win_alloc(win_T *after, ਪੂਰਨ hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    ਜੇ (new_wp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (win_alloc_ਸਤਰਾਂ(new_wp) == FAIL)
    {
	vim_free(new_wp);
	ਵਾਪਸ ਨਿਰਾ;
    }

    new_wp->w_id = ++ਆਖਰੀ_win_id;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    ਜੇ (new_wp->w_vars == ਨਿਰਾ)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	ਵਾਪਸ ਨਿਰਾ;
    }
    init_var_dict(new_wp->w_vars, &new_wp->w_winvar, VAR_SCOPE);
#ਜੇ_ਅੰਤ

    // Don't execute autocommands while the window is not properly
    // initialized yet.  gui_create_scrollbar() may trigger a FocusGained
    // event.
    ਖੰਡ_autocmds();

    /*
     * link the window in the window list
     */
    ਜੇ (!hidden)
	win_append(after, new_wp);
    new_wp->w_wincol = TPL_LCOL();
    new_wp->w_ਚੌੜਾਈ = COLUMNS_WITHOUT_TPL();

    // position the display and the cursor at the top of the file.
    new_wp->w_topline = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    new_wp->w_topfill = 0;
#ਜੇ_ਅੰਤ
    new_wp->w_botline = 2;
    new_wp->w_cursor.lnum = 1;
    new_wp->w_scbind_pos = 1;

    // use global option value for global-local options
    new_wp->w_allbuf_opt.wo_so = new_wp->w_p_so = -1;
    new_wp->w_allbuf_opt.wo_sop = new_wp->w_p_sop = -1;
    new_wp->w_allbuf_opt.wo_siso = new_wp->w_p_siso = -1;

    // We won't calculate w_fraction until resizing the window
    new_wp->w_fraction = 0;
    new_wp->w_prev_fraction_ਪੰਕਤੀ = -1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ਖੱਬਾ],
		SBAR_ਖੱਬਾ, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ਸੱਜਾ],
		SBAR_ਸੱਜਾ, new_wp);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    foldInitWin(new_wp);
#ਜੇ_ਅੰਤ
    unblock_autocmds();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    new_wp->w_ਅਗਲਾ_ਮੇਲ_id = 1000;  // up to 1000 can be picked by the user
#ਜੇ_ਅੰਤ
    ਵਾਪਸ new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    ਸਥਿਰ ਖਾਲੀ
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ਪੂਰਨ		i;
    buf_T	*buf;
    wininfo_T	*wip;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    clearFolding(wp);
#ਜੇ_ਅੰਤ

    // reduce the reference count to the argument list.
    alist_unlink(wp->w_alist);

    // Don't execute autocommands while the window is halfway being deleted.
    // gui_mch_destroy_scrollbar() may trigger a FocusGained event.
    ਖੰਡ_autocmds();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LUA
    lua_ਖਿੜਕੀ_free(wp);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
    mzscheme_ਖਿੜਕੀ_free(wp);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERL
    perl_win_free(wp);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
    python_ਖਿੜਕੀ_free(wp);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
    python3_ਖਿੜਕੀ_free(wp);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TCL
    tcl_ਖਿੜਕੀ_free(wp);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RUBY
    ruby_ਖਿੜਕੀ_free(wp);
#ਜੇ_ਅੰਤ

    remove_highlight_overrides(wp->w_hl);
    vim_free(wp->w_hl);

    // Free statusline click regions.
    ਜੇ (wp->w_stl_click != ਨਿਰਾ)
    {
	ਲਈ (i = 0; i < wp->w_stl_click_ਗਿਣਤੀ; i++)
	    vim_free(wp->w_stl_click[i].funcname);
	vim_free(wp->w_stl_click);
    }

    clear_winopt(&wp->w_onebuf_opt);
    clear_winopt(&wp->w_allbuf_opt);

    vim_free(wp->w_lcs_chars.multispace);
    vim_free(wp->w_lcs_chars.leadmultispace);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    vars_clear(&wp->w_vars->dv_hashtab);	// free all w: variables
    hash_init(&wp->w_vars->dv_hashtab);
    unref_var_dict(wp->w_vars);
#ਜੇ_ਅੰਤ

    {
	tabpage_T	*ttp;

	ਜੇ (prevwin == wp)
	    prevwin = ਨਿਰਾ;
	FOR_ALL_TABPAGES(ttp)
	    ਜੇ (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = ਨਿਰਾ;
    }
    win_free_lsize(wp);

    ਲਈ (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_ਇੰਦਰਾਜ(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    ਜੇ (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    ਜੇ (wip2 != wip && wip2->wi_win == ਨਿਰਾ)
		    {
			ਜੇ (wip2->wi_ਅਗਲਾ != ਨਿਰਾ)
			    wip2->wi_ਅਗਲਾ->wi_prev = wip2->wi_prev;
			ਜੇ (wip2->wi_prev == ਨਿਰਾ)
			    buf->b_wininfo = wip2->wi_ਅਗਲਾ;
			ਨਹੀਂਤਾਂ
			    wip2->wi_prev->wi_ਅਗਲਾ = wip2->wi_ਅਗਲਾ;
			free_wininfo(wip2);
			ਰੋਕੋ;
		    }

		wip->wi_win = ਨਿਰਾ;
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    clear_matches(wp);
#ਜੇ_ਅੰਤ

    free_jumplist(wp);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    qf_free_all(wp);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ਖੱਬਾ]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ਸੱਜਾ]);
    }
#ਜੇ_ਅੰਤ // FEAT_GUI

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    remove_winbar(wp);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    ਲਈ (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    vim_free(wp->w_scrollbar_highlight);
    vim_free(wp->w_thumb_highlight);
    vim_free(wp->w_popup_title);
    list_unref(wp->w_popup_ਨਕਾਬ);
    vim_free(wp->w_popup_ਨਕਾਬ_cells);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    vim_free(wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ

    ਜੇ (win_valid_any_tab(wp))
	win_remove(wp, tp);
    ਜੇ (autocmd_busy)
    {
	wp->w_ਅਗਲਾ = au_pending_free_win;
	au_pending_free_win = wp;
    }
    ਨਹੀਂਤਾਂ
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    ਪੂਰਨ
win_unlisted(win_T *wp)
{
    ਵਾਪਸ is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#ਜੇ defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    ਖਾਲੀ
win_free_popup(win_T *win)
{
    ਜੇ (win->w_ਬਫਰ != ਨਿਰਾ)
    {
	ਜੇ (bt_popup(win->w_ਬਫਰ))
	    win_close_ਬਫਰ(win, DOBUF_WIPE_REUSE, FALSE);
	ਨਹੀਂਤਾਂ
	    close_ਬਫਰ(win, win->w_ਬਫਰ, 0, FALSE, FALSE, TRUE);

	ਜੇ (win->w_ਬਫਰ != ਨਿਰਾ)
	    --win->w_ਬਫਰ->b_nwindows;
    }
# ਜੇ defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    ਜੇ (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_ਚੌਖਟਾ);
    win_free(win, ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

/*
 * Append window "wp" in the window list after window "after".
 */
    ਸਥਿਰ ਖਾਲੀ
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    ਜੇ (after == ਨਿਰਾ)	    // after NULL is in front of the first
	before = firstwin;
    ਨਹੀਂਤਾਂ
	before = after->w_ਅਗਲਾ;

    wp->w_ਅਗਲਾ = before;
    wp->w_prev = after;
    ਜੇ (after == ਨਿਰਾ)
	firstwin = wp;
    ਨਹੀਂਤਾਂ
	after->w_ਅਗਲਾ = wp;
    ਜੇ (before == ਨਿਰਾ)
	lastwin = wp;
    ਨਹੀਂਤਾਂ
	before->w_prev = wp;
}

/*
 * Remove a window from the window list.
 */
    ਖਾਲੀ
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ਜੇ (wp->w_prev != ਨਿਰਾ)
	wp->w_prev->w_ਅਗਲਾ = wp->w_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ ਜੇ (tp == ਨਿਰਾ)
	firstwin = curtab->tp_firstwin = wp->w_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	tp->tp_firstwin = wp->w_ਅਗਲਾ;

    ਜੇ (wp->w_ਅਗਲਾ != ਨਿਰਾ)
	wp->w_ਅਗਲਾ->w_prev = wp->w_prev;
    ਨਹੀਂਤਾਂ ਜੇ (tp == ਨਿਰਾ)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    ਨਹੀਂਤਾਂ
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_append(ਚੌਖਟਾ_T *after, ਚੌਖਟਾ_T *frp)
{
    frp->fr_ਅਗਲਾ = after->fr_ਅਗਲਾ;
    after->fr_ਅਗਲਾ = frp;
    ਜੇ (frp->fr_ਅਗਲਾ != ਨਿਰਾ)
	frp->fr_ਅਗਲਾ->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_ਪਾਓ(ਚੌਖਟਾ_T *before, ਚੌਖਟਾ_T *frp)
{
    frp->fr_ਅਗਲਾ = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    ਜੇ (frp->fr_prev != ਨਿਰਾ)
	frp->fr_prev->fr_ਅਗਲਾ = frp;
    ਨਹੀਂਤਾਂ
	frp->fr_ਮਾਪਾ->fr_ਬੱਚਾ = frp;
}

/*
 * Remove a frame from a frame list.
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_remove(ਚੌਖਟਾ_T *frp)
{
    ਜੇ (frp->fr_prev != ਨਿਰਾ)
	frp->fr_prev->fr_ਅਗਲਾ = frp->fr_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	frp->fr_ਮਾਪਾ->fr_ਬੱਚਾ = frp->fr_ਅਗਲਾ;
    ਜੇ (frp->fr_ਅਗਲਾ != ਨਿਰਾ)
	frp->fr_ਅਗਲਾ->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    ਪੂਰਨ
win_alloc_ਸਤਰਾਂ(win_T *wp)
{
    wp->w_ਸਤਰਾਂ_valid = 0;
    wp->w_ਸਤਰਾਂ = ALLOC_CLEAR_MULT(wline_T, ਪੰਕਤੀਆਂ);
    ਜੇ (wp->w_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * free lsize arrays for a window
 */
    ਖਾਲੀ
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    ਜੇ (wp != ਨਿਰਾ)
	VIM_CLEAR(wp->w_ਸਤਰਾਂ);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    ਖਾਲੀ
shell_new_ਪੰਕਤੀਆਂ(ਖਾਲੀ)
{
    ਪੂਰਨ		h = (ਪੂਰਨ)ਪੰਕਤੀਆਂ_AVAIL;

    ਜੇ (firstwin == ਨਿਰਾ)	// not initialized yet
	ਵਾਪਸ;
    ਜੇ (h < ਚੌਖਟਾ_minheight(topframe, ਨਿਰਾ))
	h = ਚੌਖਟਾ_minheight(topframe, ਨਿਰਾ);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    ਚੌਖਟਾ_new_ਉਚਾਈ(topframe, h, FALSE, TRUE, FALSE);
    ਜੇ (!ਚੌਖਟਾ_check_ਉਚਾਈ(topframe, h))
	ਚੌਖਟਾ_new_ਉਚਾਈ(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_ਵਰਤਿਆ = p_ch;

    ਜੇ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ 0
    // Disabled: don't want making the screen smaller make a window larger.
    ਜੇ (p_ea)
	win_equal(curwin, FALSE, 'v');
#ਜੇ_ਅੰਤ
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    ਖਾਲੀ
shell_new_columns(ਖਾਲੀ)
{
    ਜੇ (firstwin == ਨਿਰਾ)	// not initialized yet
	ਵਾਪਸ;

#ਜੇ defined(FEAT_TABPANEL)
    ਸਥਿਰ ਪੂਰਨ prev_tp_ਚੌੜਾਈ;
    ਸਥਿਰ ਪੂਰਨ prev_wincol;
    ਸਥਿਰ ਪੂਰਨ prev_fr_ਚੌੜਾਈ;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    ਚੌਖਟਾ_new_ਚੌੜਾਈ(topframe, w, FALSE, TRUE);
    ਜੇ (!ਚੌਖਟਾ_check_ਚੌੜਾਈ(topframe, w))
	ਚੌਖਟਾ_new_ਚੌੜਾਈ(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#ਜੇ defined(FEAT_TABPANEL)
    ਪੂਰਨ tp_ਚੌੜਾਈ = tabpanel_ਚੌੜਾਈ();
    ਪੂਰਨ tp_ਚੌੜਾਈ_ਬਦਲਿਆ = tp_ਚੌੜਾਈ != prev_tp_ਚੌੜਾਈ;
    // tabpanel width changed
    ਜੇ (tp_ਚੌੜਾਈ_ਬਦਲਿਆ && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    ਜੇ (tp_ਚੌੜਾਈ_ਬਦਲਿਆ
	    || (tp_ਚੌੜਾਈ > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_ਚੌੜਾਈ != prev_fr_ਚੌੜਾਈ)))
    {
	screen_fill(cmdline_ਪੰਕਤੀ, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, 0, (ਪੂਰਨ)Columns, ' ', ' ', 0);
    }
    prev_tp_ਚੌੜਾਈ = tabpanel_ਚੌੜਾਈ();
    prev_wincol = firstwin->w_wincol;
    prev_fr_ਚੌੜਾਈ = topframe->fr_ਚੌੜਾਈ;
#ਜੇ_ਅੰਤ
    // Adjust offset for command line start column
    cmdline_ਖਾਨਾ_off = firstwin->w_wincol;
    cmdline_ਚੌੜਾਈ = topframe->fr_ਚੌੜਾਈ;
    comp_ਖਾਨਾ();
    ਜੇ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ 0
    // Disabled: don't want making the screen smaller make a window larger.
    ਜੇ (p_ea)
	win_equal(curwin, FALSE, 'h');
#ਜੇ_ਅੰਤ
}

/*
 * Save the size of all windows in "gap".
 */
    ਖਾਲੀ
win_ਮਾਪ_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, ਆਕਾਰ(ਪੂਰਨ), 1);
    ਜੇ (ga_grow(gap, win_ਗਿਣਤੀ() * 2 + 1) == FAIL)
	ਵਾਪਸ;

    // first entry is the total lines available for windows
    ((ਪੂਰਨ *)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = ਪੰਕਤੀਆਂ_AVAIL - ਆਖਰੀ_stl_ਉਚਾਈ(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((ਪੂਰਨ *)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] =
	    wp->w_ਚੌੜਾਈ + wp->w_vsep_ਚੌੜਾਈ;
	((ਪੂਰਨ *)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = wp->w_ਉਚਾਈ;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    ਖਾਲੀ
win_ਮਾਪ_restore(garray_T *gap)
{
    win_T	*wp;
    ਪੂਰਨ		i, j;

    ਜੇ (win_ਗਿਣਤੀ() * 2 + 1 == gap->ga_len
	    && ((ਪੂਰਨ *)gap->ga_ਅੰਕੜੇ)[0] == ਪੰਕਤੀਆਂ_AVAIL - ਆਖਰੀ_stl_ਉਚਾਈ(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	ਲਈ (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		ਚੌਖਟਾ_setwidth(wp->w_ਚੌਖਟਾ, ((ਪੂਰਨ *)gap->ga_ਅੰਕੜੇ)[i++]);
		win_setheight_win(((ਪੂਰਨ *)gap->ga_ਅੰਕੜੇ)[i++], wp);
	    }
	}
	// recompute the window positions
	win_comp_pos();
    }
}

/*
 * Update the position for all windows, using the width and height of the
 * frames.
 * Returns the row just after the last window.
 */
    ਖਾਲੀ
win_comp_pos(ਖਾਲੀ)
{
    ਪੂਰਨ		ਪੰਕਤੀ = tabline_ਉਚਾਈ();
    ਪੂਰਨ		ਖਾਨਾ = TPL_LCOL();

    ਚੌਖਟਾ_comp_pos(topframe, &ਪੰਕਤੀ, &ਖਾਨਾ);
#ਜੇ defined(FEAT_STL_OPT)
    ਚੌਖਟਾ_change_statusline_ਉਚਾਈ();
#ਜੇ_ਅੰਤ
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_comp_pos(ਚੌਖਟਾ_T *topfrp, ਪੂਰਨ *ਪੰਕਤੀ, ਪੂਰਨ *ਖਾਨਾ)
{
    win_T	*wp;
    ਚੌਖਟਾ_T	*frp;
    ਪੂਰਨ		startcol;
    ਪੂਰਨ		startrow;
    ਪੂਰਨ		h;

    wp = topfrp->fr_win;
    ਜੇ (wp != ਨਿਰਾ)
    {
	ਜੇ (wp->w_winrow != *ਪੰਕਤੀ || wp->w_wincol != *ਖਾਨਾ)
	{
	    // position changed, redraw
	    wp->w_winrow = *ਪੰਕਤੀ;
	    wp->w_wincol = *ਖਾਨਾ;
	    redraw_win_later(wp, UPD_NOT_VALID);
	    wp->w_redr_ਸਥਿਤੀ = true;
	}
	// WinBar will not show if the window height is zero
	h = VISIBLE_ਉਚਾਈ(wp) + wp->w_ਸਥਿਤੀ_ਉਚਾਈ;
	*ਪੰਕਤੀ += h > topfrp->fr_ਉਚਾਈ ? topfrp->fr_ਉਚਾਈ : h;
	*ਖਾਨਾ += wp->w_ਚੌੜਾਈ + wp->w_vsep_ਚੌੜਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	startrow = *ਪੰਕਤੀ;
	startcol = *ਖਾਨਾ;
	FOR_ALL_ਚੌਖਟੇ(frp, topfrp->fr_ਬੱਚਾ)
	{
	    ਜੇ (topfrp->fr_layout == FR_ਪੰਕਤੀ)
		*ਪੰਕਤੀ = startrow;	// all frames are at the same row
	    ਨਹੀਂਤਾਂ
		*ਖਾਨਾ = startcol;	// all frames are at the same col
	    ਚੌਖਟਾ_comp_pos(frp, ਪੰਕਤੀ, ਖਾਨਾ);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    ਖਾਲੀ
win_ensure_ਮਾਪ(ਖਾਲੀ)
{
    ਜੇ (curwin->w_ਉਚਾਈ == 0)
	win_setheight(1);
    ਜੇ (curwin->w_ਚੌੜਾਈ == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    ਖਾਲੀ
win_setheight(ਪੂਰਨ ਉਚਾਈ)
{
    win_setheight_win(ਉਚਾਈ, curwin);
#ਜੇ defined(FEAT_STL_OPT)
    ਚੌਖਟਾ_change_statusline_ਉਚਾਈ();
#ਜੇ_ਅੰਤ
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    ਖਾਲੀ
win_setheight_win(ਪੂਰਨ ਉਚਾਈ, win_T *win)
{
    ਜੇ (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	ਜੇ (ਉਚਾਈ < p_wmh)
	    ਉਚਾਈ = p_wmh;
	ਜੇ (ਉਚਾਈ == 0)
	    ਉਚਾਈ = 1;
	ਉਚਾਈ += WINBAR_ਉਚਾਈ(curwin);
    }

    ਚੌਖਟਾ_setheight(win->w_ਚੌਖਟਾ, ਉਚਾਈ + win->w_ਸਥਿਤੀ_ਉਚਾਈ);

    // recompute the window positions
    win_comp_pos();
    win_fix_scroll(TRUE);

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Set the height of a frame to "height" and take care that all frames and
 * windows inside it are resized.  Also resize frames on the left and right if
 * the are in the same FR_ROW frame.
 *
 * Strategy:
 * If the frame is part of a FR_COL frame, try fitting the frame in that
 * frame.  If that doesn't work (the FR_COL frame is too small), recursively
 * go to containing frames to resize them and make room.
 * If the frame is part of a FR_ROW frame, all frames must be resized as well.
 * Check for the minimal height of the FR_ROW frame.
 * At the top level we can also use change the command line height.
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_setheight(ਚੌਖਟਾ_T *curfrp, ਪੂਰਨ ਉਚਾਈ)
{
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼;		// total number of lines available
    ਪੂਰਨ		take;		// number of lines taken from other windows
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼_cmdline;	// lines available from cmdline
    ਪੂਰਨ		ਲੜੀਵਾਰ;
    ਚੌਖਟਾ_T	*frp;
    ਪੂਰਨ		h;
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼_reserved;

    // If the height already is the desired value, nothing to do.
    ਜੇ (curfrp->fr_ਉਚਾਈ == ਉਚਾਈ)
	ਵਾਪਸ;

    ਜੇ (curfrp->fr_ਮਾਪਾ == ਨਿਰਾ)
    {
	// topframe: can only change the command line height
	ਜੇ (ਉਚਾਈ > 0)
	    ਚੌਖਟਾ_new_ਉਚਾਈ(curfrp, ਉਚਾਈ, FALSE, FALSE, TRUE);
    }
    ਨਹੀਂਤਾਂ ਜੇ (curfrp->fr_ਮਾਪਾ->fr_layout == FR_ਪੰਕਤੀ)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = ਚੌਖਟਾ_minheight(curfrp->fr_ਮਾਪਾ, ਨਿਰਾ);
	ਜੇ (ਉਚਾਈ < h)
	    ਉਚਾਈ = h;
	ਚੌਖਟਾ_setheight(curfrp->fr_ਮਾਪਾ, ਉਚਾਈ);
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Column of frames: try to change only frames in this column.
	 */
	/*
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the height to it.
	 * Try not to reduce the height of a window with 'winfixheight' set.
	 */
	ਲਈ (ਲੜੀਵਾਰ = 1; ਲੜੀਵਾਰ <= 2; ++ਲੜੀਵਾਰ)
	{
	    ਗੁੰਜਾਇਸ਼ = 0;
	    ਗੁੰਜਾਇਸ਼_reserved = 0;
	    FOR_ALL_ਚੌਖਟੇ(frp, curfrp->fr_ਮਾਪਾ->fr_ਬੱਚਾ)
	    {
		ਜੇ (frp != curfrp
			&& frp->fr_win != ਨਿਰਾ
			&& frp->fr_win->w_p_wfh)
		    ਗੁੰਜਾਇਸ਼_reserved += frp->fr_ਉਚਾਈ;
		ਗੁੰਜਾਇਸ਼ += frp->fr_ਉਚਾਈ;
		ਜੇ (frp != curfrp)
		    ਗੁੰਜਾਇਸ਼ -= ਚੌਖਟਾ_minheight(frp, ਨਿਰਾ);
	    }
	    ਜੇ (curfrp->fr_ਚੌੜਾਈ != topframe->fr_ਚੌੜਾਈ)
		ਗੁੰਜਾਇਸ਼_cmdline = 0;
	    ਨਹੀਂਤਾਂ
	    {
		ਗੁੰਜਾਇਸ਼_cmdline = ਪੰਕਤੀਆਂ - p_ch - (lastwin->w_winrow
						+ VISIBLE_ਉਚਾਈ(lastwin)
						+ lastwin->w_ਸਥਿਤੀ_ਉਚਾਈ);
		ਜੇ (ਗੁੰਜਾਇਸ਼_cmdline < 0)
		    ਗੁੰਜਾਇਸ਼_cmdline = 0;
	    }

	    ਜੇ (ਉਚਾਈ <= ਗੁੰਜਾਇਸ਼ + ਗੁੰਜਾਇਸ਼_cmdline)
		ਰੋਕੋ;
	    ਜੇ (ਲੜੀਵਾਰ == 2 || curfrp->fr_ਚੌੜਾਈ == topframe->fr_ਚੌੜਾਈ)
	    {
		ਉਚਾਈ = ਗੁੰਜਾਇਸ਼ + ਗੁੰਜਾਇਸ਼_cmdline;
		ਰੋਕੋ;
	    }
	    ਚੌਖਟਾ_setheight(curfrp->fr_ਮਾਪਾ, ਉਚਾਈ
		+ ਚੌਖਟਾ_minheight(curfrp->fr_ਮਾਪਾ, NOWIN) - (ਪੂਰਨ)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = ਉਚਾਈ - curfrp->fr_ਉਚਾਈ;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	ਜੇ (ਉਚਾਈ > ਗੁੰਜਾਇਸ਼ + ਗੁੰਜਾਇਸ਼_cmdline - ਗੁੰਜਾਇਸ਼_reserved)
	    ਗੁੰਜਾਇਸ਼_reserved = ਗੁੰਜਾਇਸ਼ + ਗੁੰਜਾਇਸ਼_cmdline - ਉਚਾਈ;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	ਜੇ (take < 0 && ਗੁੰਜਾਇਸ਼ - curfrp->fr_ਉਚਾਈ <= ਗੁੰਜਾਇਸ਼_reserved)
	    ਗੁੰਜਾਇਸ਼_reserved = 0;

	ਜੇ (take > 0 && ਗੁੰਜਾਇਸ਼_cmdline > 0)
	{
	    // use lines from cmdline first
	    ਜੇ (take < ਗੁੰਜਾਇਸ਼_cmdline)
		ਗੁੰਜਾਇਸ਼_cmdline = take;
	    take -= ਗੁੰਜਾਇਸ਼_cmdline;
	    topframe->fr_ਉਚਾਈ += ਗੁੰਜਾਇਸ਼_cmdline;
	}

	/*
	 * set the current frame to the new height
	 */
	ਚੌਖਟਾ_new_ਉਚਾਈ(curfrp, ਉਚਾਈ, FALSE, FALSE, TRUE);

	/*
	 * First take lines from the frames after the current frame.  If
	 * that is not enough, takes lines from frames above the current
	 * frame.
	 */
	ਲਈ (ਲੜੀਵਾਰ = 0; ਲੜੀਵਾਰ < 2; ++ਲੜੀਵਾਰ)
	{
	    ਜੇ (ਲੜੀਵਾਰ == 0)
		frp = curfrp->fr_ਅਗਲਾ;	// 1st run: start with next window
	    ਨਹੀਂਤਾਂ
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    ਜਦੋਂ (frp != ਨਿਰਾ && take != 0)
	    {
		h = ਚੌਖਟਾ_minheight(frp, ਨਿਰਾ);
		ਜੇ (ਗੁੰਜਾਇਸ਼_reserved > 0
			&& frp->fr_win != ਨਿਰਾ
			&& frp->fr_win->w_p_wfh)
		{
		    ਜੇ (ਗੁੰਜਾਇਸ਼_reserved >= frp->fr_ਉਚਾਈ)
			ਗੁੰਜਾਇਸ਼_reserved -= frp->fr_ਉਚਾਈ;
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (frp->fr_ਉਚਾਈ - ਗੁੰਜਾਇਸ਼_reserved > take)
			    ਗੁੰਜਾਇਸ਼_reserved = frp->fr_ਉਚਾਈ - take;
			take -= frp->fr_ਉਚਾਈ - ਗੁੰਜਾਇਸ਼_reserved;
			ਚੌਖਟਾ_new_ਉਚਾਈ(frp, ਗੁੰਜਾਇਸ਼_reserved, FALSE, FALSE,
									TRUE);
			ਗੁੰਜਾਇਸ਼_reserved = 0;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (frp->fr_ਉਚਾਈ - take < h)
		    {
			take -= frp->fr_ਉਚਾਈ - h;
			ਚੌਖਟਾ_new_ਉਚਾਈ(frp, h, FALSE, FALSE, TRUE);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਚੌਖਟਾ_new_ਉਚਾਈ(frp, frp->fr_ਉਚਾਈ - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		ਜੇ (ਲੜੀਵਾਰ == 0)
		    frp = frp->fr_ਅਗਲਾ;
		ਨਹੀਂਤਾਂ
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    ਖਾਲੀ
win_setwidth(ਪੂਰਨ ਚੌੜਾਈ)
{
    win_setwidth_win(ਚੌੜਾਈ, curwin);
}

    ਖਾਲੀ
win_setwidth_win(ਪੂਰਨ ਚੌੜਾਈ, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    ਜੇ (wp == curwin)
    {
	ਜੇ (ਚੌੜਾਈ < p_wmw)
	    ਚੌੜਾਈ = p_wmw;
	ਜੇ (ਚੌੜਾਈ == 0)
	    ਚੌੜਾਈ = 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਚੌੜਾਈ < 0)
	ਚੌੜਾਈ = 0;

    ਚੌਖਟਾ_setwidth(wp->w_ਚੌਖਟਾ, ਚੌੜਾਈ + wp->w_vsep_ਚੌੜਾਈ);

    // recompute the window positions
    win_comp_pos();

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Set the width of a frame to "width" and take care that all frames and
 * windows inside it are resized.  Also resize frames above and below if the
 * are in the same FR_ROW frame.
 *
 * Strategy is similar to frame_setheight().
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_setwidth(ਚੌਖਟਾ_T *curfrp, ਪੂਰਨ ਚੌੜਾਈ)
{
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼;		// total number of lines available
    ਪੂਰਨ		take;		// number of lines taken from other windows
    ਪੂਰਨ		ਲੜੀਵਾਰ;
    ਚੌਖਟਾ_T	*frp;
    ਪੂਰਨ		w;
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼_reserved;

    // If the width already is the desired value, nothing to do.
    ਜੇ (curfrp->fr_ਚੌੜਾਈ == ਚੌੜਾਈ)
	ਵਾਪਸ;

    ਜੇ (curfrp->fr_ਮਾਪਾ == ਨਿਰਾ)
	// topframe: can't change width
	ਵਾਪਸ;

    ਜੇ (curfrp->fr_ਮਾਪਾ->fr_layout == FR_ਖਾਨਾ)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = ਚੌਖਟਾ_minwidth(curfrp->fr_ਮਾਪਾ, ਨਿਰਾ);
	ਜੇ (ਚੌੜਾਈ < w)
	    ਚੌੜਾਈ = w;
	ਚੌਖਟਾ_setwidth(curfrp->fr_ਮਾਪਾ, ਚੌੜਾਈ);
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	ਲਈ (ਲੜੀਵਾਰ = 1; ਲੜੀਵਾਰ <= 2; ++ਲੜੀਵਾਰ)
	{
	    ਗੁੰਜਾਇਸ਼ = 0;
	    ਗੁੰਜਾਇਸ਼_reserved = 0;
	    FOR_ALL_ਚੌਖਟੇ(frp, curfrp->fr_ਮਾਪਾ->fr_ਬੱਚਾ)
	    {
		ਜੇ (frp != curfrp
			&& frp->fr_win != ਨਿਰਾ
			&& frp->fr_win->w_p_wfw)
		    ਗੁੰਜਾਇਸ਼_reserved += frp->fr_ਚੌੜਾਈ;
		ਗੁੰਜਾਇਸ਼ += frp->fr_ਚੌੜਾਈ;
		ਜੇ (frp != curfrp)
		    ਗੁੰਜਾਇਸ਼ -= ਚੌਖਟਾ_minwidth(frp, ਨਿਰਾ);
	    }

	    ਜੇ (ਚੌੜਾਈ <= ਗੁੰਜਾਇਸ਼)
		ਰੋਕੋ;
	    ਜੇ (ਲੜੀਵਾਰ == 2 || curfrp->fr_ਉਚਾਈ >= ਪੰਕਤੀਆਂ_AVAIL)
	    {
		ਚੌੜਾਈ = ਗੁੰਜਾਇਸ਼;
		ਰੋਕੋ;
	    }
	    ਚੌਖਟਾ_setwidth(curfrp->fr_ਮਾਪਾ, ਚੌੜਾਈ
		 + ਚੌਖਟਾ_minwidth(curfrp->fr_ਮਾਪਾ, NOWIN) - (ਪੂਰਨ)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = ਚੌੜਾਈ - curfrp->fr_ਚੌੜਾਈ;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	ਜੇ (ਚੌੜਾਈ > ਗੁੰਜਾਇਸ਼ - ਗੁੰਜਾਇਸ਼_reserved)
	    ਗੁੰਜਾਇਸ਼_reserved = ਗੁੰਜਾਇਸ਼ - ਚੌੜਾਈ;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	ਜੇ (take < 0 && ਗੁੰਜਾਇਸ਼ - curfrp->fr_ਚੌੜਾਈ < ਗੁੰਜਾਇਸ਼_reserved)
	    ਗੁੰਜਾਇਸ਼_reserved = 0;

	/*
	 * set the current frame to the new width
	 */
	ਚੌਖਟਾ_new_ਚੌੜਾਈ(curfrp, ਚੌੜਾਈ, FALSE, FALSE);

	/*
	 * First take lines from the frames right of the current frame.  If
	 * that is not enough, takes lines from frames left of the current
	 * frame.
	 */
	ਲਈ (ਲੜੀਵਾਰ = 0; ਲੜੀਵਾਰ < 2; ++ਲੜੀਵਾਰ)
	{
	    ਜੇ (ਲੜੀਵਾਰ == 0)
		frp = curfrp->fr_ਅਗਲਾ;	// 1st run: start with next window
	    ਨਹੀਂਤਾਂ
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    ਜਦੋਂ (frp != ਨਿਰਾ && take != 0)
	    {
		w = ਚੌਖਟਾ_minwidth(frp, ਨਿਰਾ);
		ਜੇ (ਗੁੰਜਾਇਸ਼_reserved > 0
			&& frp->fr_win != ਨਿਰਾ
			&& frp->fr_win->w_p_wfw)
		{
		    ਜੇ (ਗੁੰਜਾਇਸ਼_reserved >= frp->fr_ਚੌੜਾਈ)
			ਗੁੰਜਾਇਸ਼_reserved -= frp->fr_ਚੌੜਾਈ;
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (frp->fr_ਚੌੜਾਈ - ਗੁੰਜਾਇਸ਼_reserved > take)
			    ਗੁੰਜਾਇਸ਼_reserved = frp->fr_ਚੌੜਾਈ - take;
			take -= frp->fr_ਚੌੜਾਈ - ਗੁੰਜਾਇਸ਼_reserved;
			ਚੌਖਟਾ_new_ਚੌੜਾਈ(frp, ਗੁੰਜਾਇਸ਼_reserved, FALSE, FALSE);
			ਗੁੰਜਾਇਸ਼_reserved = 0;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (frp->fr_ਚੌੜਾਈ - take < w)
		    {
			take -= frp->fr_ਚੌੜਾਈ - w;
			ਚੌਖਟਾ_new_ਚੌੜਾਈ(frp, w, FALSE, FALSE);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਚੌਖਟਾ_new_ਚੌੜਾਈ(frp, frp->fr_ਚੌੜਾਈ - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		ਜੇ (ਲੜੀਵਾਰ == 0)
		    frp = frp->fr_ਅਗਲਾ;
		ਨਹੀਂਤਾਂ
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    ਖਾਲੀ
win_setminheight(ਖਾਲੀ)
{
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼;
    ਪੂਰਨ		needed;
    ਪੂਰਨ		ਪਹਿਲਾ = TRUE;

    // loop until there is a 'winminheight' that is possible
    ਜਦੋਂ (p_wmh > 0)
    {
	ਗੁੰਜਾਇਸ਼ = ਪੰਕਤੀਆਂ - p_ch;
	needed = min_ਪੰਕਤੀਆਂ_for_all_tabpages() - 1;  // 1 was added for the cmdline
	ਜੇ (ਗੁੰਜਾਇਸ਼ >= needed)
	    ਰੋਕੋ;
	--p_wmh;
	ਜੇ (ਪਹਿਲਾ)
	{
	    emsg(_(e_not_enough_ਗੁੰਜਾਇਸ਼));
	    ਪਹਿਲਾ = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    ਖਾਲੀ
win_setminwidth(ਖਾਲੀ)
{
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼;
    ਪੂਰਨ		needed;
    ਪੂਰਨ		ਪਹਿਲਾ = TRUE;

    // loop until there is a 'winminheight' that is possible
    ਜਦੋਂ (p_wmw > 0)
    {
	ਗੁੰਜਾਇਸ਼ = topframe->fr_ਚੌੜਾਈ;
	needed = ਚੌਖਟਾ_minwidth(topframe, ਨਿਰਾ);
	ਜੇ (ਗੁੰਜਾਇਸ਼ >= needed)
	    ਰੋਕੋ;
	--p_wmw;
	ਜੇ (ਪਹਿਲਾ)
	{
	    emsg(_(e_not_enough_ਗੁੰਜਾਇਸ਼));
	    ਪਹਿਲਾ = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    ਖਾਲੀ
win_drag_ਸਥਿਤੀ_ਸਤਰ(win_T *dragwin, ਪੂਰਨ ਵਿੱਥ)
{
    ਚੌਖਟਾ_T	*curfr;
    ਚੌਖਟਾ_T	*fr;
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼;
    ਪੂਰਨ		ਉੱਪਰ;	// if TRUE, drag status line up, otherwise down
    ਪੂਰਨ		n;

    fr = dragwin->w_ਚੌਖਟਾ;
    curfr = fr;
    ਜੇ (fr != topframe)		// more than one window
    {
	fr = fr->fr_ਮਾਪਾ;
	// When the parent frame is not a column of frames, its parent should
	// be.
	ਜੇ (fr->fr_layout != FR_ਖਾਨਾ)
	{
	    curfr = fr;
	    ਜੇ (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_ਮਾਪਾ;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    ਜਦੋਂ (curfr != topframe && curfr->fr_ਅਗਲਾ == ਨਿਰਾ)
    {
	ਜੇ (fr != topframe)
	    fr = fr->fr_ਮਾਪਾ;
	curfr = fr;
	ਜੇ (fr != topframe)
	    fr = fr->fr_ਮਾਪਾ;
    }

    ਜੇ (ਵਿੱਥ < 0) // drag up
    {
	ਉੱਪਰ = TRUE;
	ਵਿੱਥ = -ਵਿੱਥ;
	// sum up the room of the current frame and above it
	ਜੇ (fr == curfr)
	{
	    // only one window
	    ਗੁੰਜਾਇਸ਼ = fr->fr_ਉਚਾਈ - ਚੌਖਟਾ_minheight(fr, ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਗੁੰਜਾਇਸ਼ = 0;
	    ਲਈ (fr = fr->fr_ਬੱਚਾ; ; fr = fr->fr_ਅਗਲਾ)
	    {
		ਗੁੰਜਾਇਸ਼ += fr->fr_ਉਚਾਈ - ਚੌਖਟਾ_minheight(fr, ਨਿਰਾ);
		ਜੇ (fr == curfr)
		    ਰੋਕੋ;
	    }
	}
	fr = curfr->fr_ਅਗਲਾ;		// put fr at frame that grows
    }
    ਨਹੀਂਤਾਂ    // drag down
    {
	ਉੱਪਰ = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	ਗੁੰਜਾਇਸ਼ = ਪੰਕਤੀਆਂ - cmdline_ਪੰਕਤੀ;
	ਜੇ (curfr->fr_ਅਗਲਾ == ਨਿਰਾ)
	    --ਗੁੰਜਾਇਸ਼;
	ਨਹੀਂਤਾਂ
	    ਗੁੰਜਾਇਸ਼ -= p_ch;
	ਜੇ (ਗੁੰਜਾਇਸ਼ < 0)
	    ਗੁੰਜਾਇਸ਼ = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_ਚੌਖਟੇ(fr, curfr->fr_ਅਗਲਾ)
	    ਗੁੰਜਾਇਸ਼ += fr->fr_ਉਚਾਈ - ਚੌਖਟਾ_minheight(fr, ਨਿਰਾ);
	fr = curfr;			// put fr at window that grows
    }

    ਜੇ (ਗੁੰਜਾਇਸ਼ < ਵਿੱਥ)		// Not enough room
	ਵਿੱਥ = ਗੁੰਜਾਇਸ਼;		// Move as far as we can
    ਜੇ (ਵਿੱਥ <= 0)
	ਵਾਪਸ;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    ਜੇ (fr != ਨਿਰਾ)
	ਚੌਖਟਾ_new_ਉਚਾਈ(fr, fr->fr_ਉਚਾਈ + ਵਿੱਥ, ਉੱਪਰ, FALSE, TRUE);

    ਜੇ (ਉੱਪਰ)
	fr = curfr;		// current frame gets smaller
    ਨਹੀਂਤਾਂ
	fr = curfr->fr_ਅਗਲਾ;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    ਜਦੋਂ (fr != ਨਿਰਾ && ਵਿੱਥ > 0)
    {
	n = ਚੌਖਟਾ_minheight(fr, ਨਿਰਾ);
	ਜੇ (fr->fr_ਉਚਾਈ - ਵਿੱਥ <= n)
	{
	    ਵਿੱਥ -= fr->fr_ਉਚਾਈ - n;
	    ਚੌਖਟਾ_new_ਉਚਾਈ(fr, n, !ਉੱਪਰ, FALSE, TRUE);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਚੌਖਟਾ_new_ਉਚਾਈ(fr, fr->fr_ਉਚਾਈ - ਵਿੱਥ, !ਉੱਪਰ, FALSE, TRUE);
	    ਰੋਕੋ;
	}
	ਜੇ (ਉੱਪਰ)
	    fr = fr->fr_prev;
	ਨਹੀਂਤਾਂ
	    fr = fr->fr_ਅਗਲਾ;
    }
    win_comp_pos();
    win_fix_scroll(TRUE);

    redraw_all_later(UPD_SOME_VALID);
    showmode();
}

/*
 * Separator line of dragwin is dragged "offset" lines right (negative is left).
 */
    ਖਾਲੀ
win_drag_vsep_ਸਤਰ(win_T *dragwin, ਪੂਰਨ ਵਿੱਥ)
{
    ਚੌਖਟਾ_T	*curfr;
    ਚੌਖਟਾ_T	*fr;
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼;
    ਪੂਰਨ		ਖੱਬਾ;	// if TRUE, drag separator line left, otherwise right
    ਪੂਰਨ		n;

    fr = dragwin->w_ਚੌਖਟਾ;
    ਜੇ (fr == topframe)		// only one window (cannot happen?)
	ਵਾਪਸ;
    curfr = fr;
    fr = fr->fr_ਮਾਪਾ;
    // When the parent frame is not a row of frames, its parent should be.
    ਜੇ (fr->fr_layout != FR_ਪੰਕਤੀ)
    {
	ਜੇ (fr == topframe)	// only a column of windows (cannot happen?)
	    ਵਾਪਸ;
	curfr = fr;
	fr = fr->fr_ਮਾਪਾ;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    ਜਦੋਂ (curfr->fr_ਅਗਲਾ == ਨਿਰਾ)
    {
	ਜੇ (fr == topframe)
	    ਰੋਕੋ;
	curfr = fr;
	fr = fr->fr_ਮਾਪਾ;
	ਜੇ (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_ਮਾਪਾ;
	}
    }

    ਜੇ (ਵਿੱਥ < 0) // drag left
    {
	ਖੱਬਾ = TRUE;
	ਵਿੱਥ = -ਵਿੱਥ;
	// sum up the room of the current frame and left of it
	ਗੁੰਜਾਇਸ਼ = 0;
	ਲਈ (fr = fr->fr_ਬੱਚਾ; ; fr = fr->fr_ਅਗਲਾ)
	{
	    ਗੁੰਜਾਇਸ਼ += fr->fr_ਚੌੜਾਈ - ਚੌਖਟਾ_minwidth(fr, ਨਿਰਾ);
	    ਜੇ (fr == curfr)
		ਰੋਕੋ;
	}
	fr = curfr->fr_ਅਗਲਾ;		// put fr at frame that grows
    }
    ਨਹੀਂਤਾਂ    // drag right
    {
	ਖੱਬਾ = FALSE;
	// sum up the room of frames right of the current one
	ਗੁੰਜਾਇਸ਼ = 0;
	FOR_ALL_ਚੌਖਟੇ(fr, curfr->fr_ਅਗਲਾ)
	    ਗੁੰਜਾਇਸ਼ += fr->fr_ਚੌੜਾਈ - ਚੌਖਟਾ_minwidth(fr, ਨਿਰਾ);
	fr = curfr;			// put fr at window that grows
    }

    ਜੇ (ਗੁੰਜਾਇਸ਼ < ਵਿੱਥ)		// Not enough room
	ਵਿੱਥ = ਗੁੰਜਾਇਸ਼;		// Move as far as we can
    ਜੇ (ਵਿੱਥ <= 0)		// No room at all, quit.
	ਵਾਪਸ;
    ਜੇ (fr == ਨਿਰਾ)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	ਵਾਪਸ;

    // grow frame fr by offset lines
    ਚੌਖਟਾ_new_ਚੌੜਾਈ(fr, fr->fr_ਚੌੜਾਈ + ਵਿੱਥ, ਖੱਬਾ, FALSE);

    // shrink other frames: current and at the left or at the right
    ਜੇ (ਖੱਬਾ)
	fr = curfr;		// current frame gets smaller
    ਨਹੀਂਤਾਂ
	fr = curfr->fr_ਅਗਲਾ;	// next frame gets smaller

    ਜਦੋਂ (fr != ਨਿਰਾ && ਵਿੱਥ > 0)
    {
	n = ਚੌਖਟਾ_minwidth(fr, ਨਿਰਾ);
	ਜੇ (fr->fr_ਚੌੜਾਈ - ਵਿੱਥ <= n)
	{
	    ਵਿੱਥ -= fr->fr_ਚੌੜਾਈ - n;
	    ਚੌਖਟਾ_new_ਚੌੜਾਈ(fr, n, !ਖੱਬਾ, FALSE);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਚੌਖਟਾ_new_ਚੌੜਾਈ(fr, fr->fr_ਚੌੜਾਈ - ਵਿੱਥ, !ਖੱਬਾ, FALSE);
	    ਰੋਕੋ;
	}
	ਜੇ (ਖੱਬਾ)
	    fr = fr->fr_prev;
	ਨਹੀਂਤਾਂ
	    fr = fr->fr_ਅਗਲਾ;
    }
    win_comp_pos();
    redraw_all_later(UPD_NOT_VALID);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FRACTION_MULT	16384L

/*
 * Set wp->w_fraction for the current w_wrow and w_height.
 * Has no effect when the window is less than two lines.
 */
    ਖਾਲੀ
ਸੈੱਟ_fraction(win_T *wp)
{
    ਜੇ (wp->w_ਉਚਾਈ > 1)
	// When cursor is in the first line the percentage is computed as if
	// it's halfway that line.  Thus with two lines it is 25%, with three
	// lines 17%, etc.  Similarly for the last line: 75%, 83%, etc.
	wp->w_fraction = ((ਲੰਮਾ)wp->w_wrow * FRACTION_MULT
				     + FRACTION_MULT / 2) / (ਲੰਮਾ)wp->w_ਉਚਾਈ;
}

/*
 * Handle scroll position, depending on 'splitkeep'.  Replaces the
 * scroll_to_fraction() call from win_new_height() if 'splitkeep' is "screen"
 * or "topline".  Instead we iterate over all windows in a tabpage and
 * calculate the new scroll position.
 * TODO: Ensure this also works with wrapped lines.
 * Requires a not fully visible cursor line to be allowed at the bottom of
 * a window ("zb"), probably only when 'smoothscroll' is also set.
 */
    ਸਥਿਰ ਖਾਲੀ
win_fix_scroll(ਪੂਰਨ resize)
{
    ਜੇ (*p_spk == 'c')
	ਵਾਪਸ;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	ਜੇ (wp->w_ਉਚਾਈ != wp->w_prev_ਉਚਾਈ)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    ਜੇ (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    {
		ਪੂਰਨ diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_ਉਚਾਈ - wp->w_prev_ਉਚਾਈ);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		ਜੇ (diff > 0)
		    cursor_ਥੱਲੇ_inner(wp, diff);
		ਨਹੀਂਤਾਂ
		    cursor_ਉੱਪਰ_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_ਮੰਜ਼ਿਲ_fraction(wp, wp->w_prev_ਉਚਾਈ);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		ਜੇ (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_ਉਚਾਈ = wp->w_ਉਚਾਈ;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    ਜੇ (!(get_real_state() & (ਬਹੁਲਕ_ਸਾਧਾਰਨ|ਬਹੁਲਕ_CMDLINE|ਬਹੁਲਕ_TERMINAL)))
	win_fix_cursor(FALSE);
    ਨਹੀਂਤਾਂ ਜੇ (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    ਸਥਿਰ ਖਾਲੀ
win_fix_cursor(ਪੂਰਨ ਸਾਧਾਰਨ_4)
{
    win_T	*wp = curwin;

    ਜੇ (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ < wp->w_ਉਚਾਈ)
	ਵਾਪਸ;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    ਲੰਮਾ so = MIN(wp->w_ਉਚਾਈ / 2, get_scrolloff_ਮੁੱਲ());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_ਥੱਲੇ_inner(wp, so);
    linenr_T ਸਿਖਰ = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_ਉੱਪਰ_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    ਜੇ (lnum > bot && (wp->w_botline - wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ) != 1)
	nlnum = bot;
    ਨਹੀਂਤਾਂ ਜੇ (lnum < ਸਿਖਰ && wp->w_topline != 1)
	nlnum = (so == wp->w_ਉਚਾਈ / 2) ? bot : ਸਿਖਰ;

    ਜੇ (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	ਜੇ (ਸਾਧਾਰਨ_4)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	ਨਹੀਂਤਾਂ  // Scroll instead when not in normal mode.
	{
	    wp->w_fraction = (nlnum == bot) ? FRACTION_MULT : 0;
	    scroll_ਮੰਜ਼ਿਲ_fraction(wp, wp->w_prev_ਉਚਾਈ);
	    validate_botline();
	}
    }
}

/*
 * Set the height of a window.
 * "height" excludes any window toolbar.
 * This takes care of the things inside the window, not what happens to the
 * window position, the frame or to other windows.
 */
    ਖਾਲੀ
win_new_ਉਚਾਈ(win_T *wp, ਪੂਰਨ ਉਚਾਈ)
{
    ਪੂਰਨ		prev_ਉਚਾਈ = wp->w_ਉਚਾਈ;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    ਜੇ (ਉਚਾਈ < 0)
	ਉਚਾਈ = 0;
    ਜੇ (wp->w_ਉਚਾਈ == ਉਚਾਈ)
	ਵਾਪਸ;	    // nothing to do

    ਜੇ (wp->w_ਉਚਾਈ > 0)
    {
	ਜੇ (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	ਜੇ (wp->w_ਉਚਾਈ != prev_ਉਚਾਈ)
	    ਵਾਪਸ;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	ਜੇ (wp->w_wrow != wp->w_prev_fraction_ਪੰਕਤੀ)
	    ਸੈੱਟ_fraction(wp);
    }

    wp->w_ਉਚਾਈ = ਉਚਾਈ;
    wp->w_redr_ਸਥਿਤੀ = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    ਜੇ (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	ਜੇ (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_ਮੰਜ਼ਿਲ_fraction(wp, prev_ਉਚਾਈ);
    }
}

    ਖਾਲੀ
scroll_ਮੰਜ਼ਿਲ_fraction(win_T *wp, ਪੂਰਨ prev_ਉਚਾਈ)
{
    linenr_T	lnum;
    ਪੂਰਨ		sline, ਸਤਰ_ਮਾਪ;
    ਪੂਰਨ		ਉਚਾਈ = wp->w_ਉਚਾਈ;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    ਜੇ (ਉਚਾਈ > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (ਉਚਾਈ < wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	ਜੇ (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((ਲੰਮਾ)wp->w_fraction * (ਲੰਮਾ)ਉਚਾਈ - 1L)
							       / FRACTION_MULT;
	ਸਤਰ_ਮਾਪ = plines_win_ਖਾਨਾ(wp, lnum, (ਲੰਮਾ)(wp->w_cursor.ਖਾਨਾ)) - 1;
	sline = wp->w_wrow - ਸਤਰ_ਮਾਪ;

	ਜੇ (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    ਪੂਰਨ ਪੰਕਤੀਆਂ_2 = plines_win(wp, lnum, FALSE);

	    ਜੇ (sline > wp->w_ਉਚਾਈ - ਪੰਕਤੀਆਂ_2)
	    {
		sline = wp->w_ਉਚਾਈ - ਪੰਕਤੀਆਂ_2;
		wp->w_wrow -= ਪੰਕਤੀਆਂ_2 - ਸਤਰ_ਮਾਪ;
	    }
	}

	ਜੇ (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    ਪੂਰਨ		want_ਪੰਕਤੀ = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = ਸਤਰ_ਮਾਪ;
	    ਜੇ (wp->w_wrow >= wp->w_ਉਚਾਈ
				       && (wp->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(wp);

		--wp->w_wrow;
		ਜਦੋਂ (wp->w_wrow > want_ਪੰਕਤੀ)
		{
		    skipcol += wp->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(wp) + win_ਖਾਨਾ_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (sline > 0)
	{
	    ਜਦੋਂ (sline > 0 && lnum > 1)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, ਨਿਰਾ, TRUE, ਨਿਰਾ);
		ਜੇ (lnum == 1)
		{
		    // first line in buffer is folded
		    ਸਤਰ_ਮਾਪ = 1;
		    --sline;
		    ਰੋਕੋ;
		}
#ਜੇ_ਅੰਤ
		--lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		ਜੇ (lnum == wp->w_topline)
		    ਸਤਰ_ਮਾਪ = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    ਸਤਰ_ਮਾਪ = plines_win(wp, lnum, TRUE);
		sline -= ਸਤਰ_ਮਾਪ;
	    }

	    ਜੇ (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		hasFoldingWin(wp, lnum, ਨਿਰਾ, &lnum, TRUE, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
		lnum++;
		wp->w_wrow -= ਸਤਰ_ਮਾਪ + sline;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	ਸੈੱਟ_topline(wp, lnum);
    }

    ਜੇ (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    ਜੇ (prev_ਉਚਾਈ > 0)
	wp->w_prev_fraction_ਪੰਕਤੀ = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    ਖਾਲੀ
win_new_ਚੌੜਾਈ(win_T *wp, ਪੂਰਨ ਚੌੜਾਈ)
{
    // Should we give an error if width < 0?
    wp->w_ਚੌੜਾਈ = ਚੌੜਾਈ < 0 ? 0 : ਚੌੜਾਈ;
    wp->w_ਸਤਰਾਂ_valid = 0;
    ਬਦਲਿਆ_ਸਤਰ_abv_curs_win(wp);
    invalidate_botline_win(wp);

    ਜੇ (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_ਸਥਿਤੀ = true;
}

    ਖਾਲੀ
win_comp_scroll(win_T *wp)
{
#ਜੇ defined(FEAT_EVAL)
    ਪੂਰਨ old_w_p_scr = wp->w_p_scr;
#ਜੇ_ਅੰਤ

    wp->w_p_scr = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)wp->w_ਉਚਾਈ >> 1);
    ਜੇ (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#ਜੇ defined(FEAT_EVAL)
    ਜੇ (wp->w_p_scr != old_w_p_scr)
    {
	// Used by "verbose set scroll".
	wp->w_p_script_ctx[WV_SCROLL].sc_sid = SID_WINLAYOUT;
	wp->w_p_script_ctx[WV_SCROLL].sc_lnum = 0;
    }
#ਜੇ_ਅੰਤ
}

/*
 * Command_height: called whenever p_ch has been changed.
 */
    ਖਾਲੀ
command_ਉਚਾਈ(ਖਾਲੀ)
{
    ਪੂਰਨ		old_p_ch = curtab->tp_ch_ਵਰਤਿਆ;

    // Find bottom frame with width of screen.
    ਚੌਖਟਾ_T *frp = lastwin->w_ਚੌਖਟਾ;
    ਜਦੋਂ (frp->fr_ਚੌੜਾਈ != topframe->fr_ਚੌੜਾਈ && frp->fr_ਮਾਪਾ != ਨਿਰਾ)
	frp = frp->fr_ਮਾਪਾ;

    // Avoid changing the height of a window with 'winfixheight' set.
    ਜਦੋਂ (frp->fr_prev != ਨਿਰਾ && frp->fr_layout == FR_ਪੱਤੀ
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    ਜਦੋਂ (p_ch > old_p_ch && command_ਚੌਖਟਾ_ਉਚਾਈ)
    {
	ਜੇ (frp == ਨਿਰਾ)
	{
	    emsg(_(e_not_enough_ਗੁੰਜਾਇਸ਼));
	    p_ch = old_p_ch;
	    ਰੋਕੋ;
	}
	ਪੂਰਨ h = MIN(p_ch - old_p_ch,
			frp->fr_ਉਚਾਈ - ਚੌਖਟਾ_minheight(frp, ਨਿਰਾ));
	ਚੌਖਟਾ_ਜੋੜੋ_ਉਚਾਈ(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    ਜੇ (p_ch < old_p_ch && command_ਚੌਖਟਾ_ਉਚਾਈ && frp != ਨਿਰਾ)
	ਚੌਖਟਾ_ਜੋੜੋ_ਉਚਾਈ(frp, (ਪੂਰਨ)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    ਜੇ (ਸੁਨੇਹਾ_scrolled == 0 && ਭਰਿਆ_screen)
    {
	screen_fill(cmdline_ਪੰਕਤੀ, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, 0, (ਪੂਰਨ)Columns, ' ', ' ', 0);
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ;
    }

    // Use the value of p_ch that we remembered.  This is needed for when the
    // GUI starts up, we can't be sure in what order things happen.  And when
    // p_ch was changed in another tab page.
    curtab->tp_ch_ਵਰਤਿਆ = p_ch;
    min_ਸੈੱਟ_ch = p_ch;
}

/*
 * Resize frame "frp" to be "n" lines higher (negative for less high).
 * Also resize the frames it is contained in.
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_ਜੋੜੋ_ਉਚਾਈ(ਚੌਖਟਾ_T *frp, ਪੂਰਨ n)
{
    ਚੌਖਟਾ_new_ਉਚਾਈ(frp, frp->fr_ਉਚਾਈ + n, FALSE, FALSE, FALSE);
    ਲਈ (;;)
    {
	frp = frp->fr_ਮਾਪਾ;
	ਜੇ (frp == ਨਿਰਾ)
	    ਰੋਕੋ;
	frp->fr_ਉਚਾਈ += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    ਖਾਲੀ
ਆਖਰੀ_ਸਥਿਤੀ(
    ਪੂਰਨ		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    ਆਖਰੀ_ਸਥਿਤੀ_rec(topframe, ਆਖਰੀ_stl_ਉਚਾਈ(morewin) > 0);
#ਜੇ defined(FEAT_STL_OPT)
    ਚੌਖਟਾ_change_statusline_ਉਚਾਈ();
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
ਆਖਰੀ_ਸਥਿਤੀ_rec(ਚੌਖਟਾ_T *fr, ਪੂਰਨ statusline)
{
    ਚੌਖਟਾ_T	*fp;
    win_T	*wp;

    ਜੇ (fr->fr_layout == FR_ਪੱਤੀ)
    {
	wp = fr->fr_win;
	ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ != 0 && !statusline)
	{
	    // remove status line
	    win_new_ਉਚਾਈ(wp, wp->w_ਉਚਾਈ + wp->w_ਸਥਿਤੀ_ਉਚਾਈ);
	    wp->w_ਸਥਿਤੀ_ਉਚਾਈ = 0;
	    comp_ਖਾਨਾ();
	}
	ਨਹੀਂਤਾਂ ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    ਜਦੋਂ (fp->fr_ਉਚਾਈ <= ਚੌਖਟਾ_minheight(fp, ਨਿਰਾ))
	    {
		ਜੇ (fp == topframe)
		{
		    emsg(_(e_not_enough_ਗੁੰਜਾਇਸ਼));
		    ਵਾਪਸ;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		ਜੇ (fp->fr_ਮਾਪਾ->fr_layout == FR_ਖਾਨਾ && fp->fr_prev != ਨਿਰਾ)
		    fp = fp->fr_prev;
		ਨਹੀਂਤਾਂ
		    fp = fp->fr_ਮਾਪਾ;
	    }
	    wp->w_ਸਥਿਤੀ_ਉਚਾਈ = statusline_ਉਚਾਈ(wp);
	    ਜੇ (fp != fr)
	    {
		ਚੌਖਟਾ_new_ਉਚਾਈ(fp, fp->fr_ਉਚਾਈ - wp->w_ਸਥਿਤੀ_ਉਚਾਈ,
			FALSE, FALSE, FALSE);
		ਚੌਖਟਾ_fix_ਉਚਾਈ(wp);
		win_comp_pos();
	    }
	    ਨਹੀਂਤਾਂ
		win_new_ਉਚਾਈ(wp, wp->w_ਉਚਾਈ - wp->w_ਸਥਿਤੀ_ਉਚਾਈ);
	    comp_ਖਾਨਾ();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	ਜੇ (ਨਿਰਪੇਖ(wp->w_ਉਚਾਈ - wp->w_prev_ਉਚਾਈ) == 1)
	    wp->w_prev_ਉਚਾਈ = wp->w_ਉਚਾਈ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (fr->fr_layout == FR_ਪੰਕਤੀ)
    {
	// vertically split windows, set status line for each one
	ਜੇ (!statusline)
	{
	    FOR_ALL_ਚੌਖਟੇ(fp, fr->fr_ਬੱਚਾ)
		ਆਖਰੀ_ਸਥਿਤੀ_rec(fp, statusline);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਚੌਖਟਾ_T	*fp2;
	    ਪੂਰਨ		max_stlh = 0;
	    ਪੂਰਨ		new_ਪੰਕਤੀ_ਉਚਾਈ;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_ਚੌਖਟੇ(fp, fr->fr_ਬੱਚਾ)
		ਜੇ (fp->fr_win != ਨਿਰਾ && fp->fr_win->w_ਸਥਿਤੀ_ਉਚਾਈ == 0)
		{
		    ਪੂਰਨ h = statusline_ਉਚਾਈ(fp->fr_win);
		    ਜੇ (h > max_stlh)
			max_stlh = h;
		}
	    ਜੇ (max_stlh == 0)
		ਵਾਪਸ;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    ਜਦੋਂ (fp2->fr_ਉਚਾਈ - ਚੌਖਟਾ_minheight(fp2, ਨਿਰਾ) < max_stlh)
	    {
		ਜੇ (fp2 == topframe)
		{
		    emsg(_(e_not_enough_ਗੁੰਜਾਇਸ਼));
		    ਵਾਪਸ;
		}
		ਜੇ (fp2->fr_ਮਾਪਾ->fr_layout == FR_ਖਾਨਾ
						&& fp2->fr_prev != ਨਿਰਾ)
		    fp2 = fp2->fr_prev;
		ਨਹੀਂਤਾਂ
		    fp2 = fp2->fr_ਮਾਪਾ;
	    }

	    ਜੇ (fp2 != fr)
	    {
		ਚੌਖਟਾ_new_ਉਚਾਈ(fp2, fp2->fr_ਉਚਾਈ - max_stlh,
							FALSE, FALSE, FALSE);
		new_ਪੰਕਤੀ_ਉਚਾਈ = fr->fr_ਉਚਾਈ + max_stlh;
	    }
	    ਨਹੀਂਤਾਂ
		new_ਪੰਕਤੀ_ਉਚਾਈ = fr->fr_ਉਚਾਈ;

	    // Set status and content heights for all leaves.
	    FOR_ALL_ਚੌਖਟੇ(fp, fr->fr_ਬੱਚਾ)
	    {
		ਜੇ (fp->fr_win != ਨਿਰਾ)
		{
		    wp = fp->fr_win;
		    ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ == 0)
		    {
			wp->w_ਸਥਿਤੀ_ਉਚਾਈ = statusline_ਉਚਾਈ(wp);
			win_new_ਉਚਾਈ(wp,
				  new_ਪੰਕਤੀ_ਉਚਾਈ - wp->w_ਸਥਿਤੀ_ਉਚਾਈ);
			ਚੌਖਟਾ_fix_ਉਚਾਈ(wp);
			comp_ਖਾਨਾ();
			redraw_all_later(UPD_SOME_VALID);
			ਜੇ (ਨਿਰਪੇਖ(wp->w_ਉਚਾਈ - wp->w_prev_ਉਚਾਈ) == 1)
			    wp->w_prev_ਉਚਾਈ = wp->w_ਉਚਾਈ;
		    }
		}
		ਨਹੀਂਤਾਂ
		    ਆਖਰੀ_ਸਥਿਤੀ_rec(fp, statusline); // nested frames
	    }

	    fr->fr_ਉਚਾਈ = new_ਪੰਕਤੀ_ਉਚਾਈ;
	    win_comp_pos();
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// horizontally split window, set status line for last one
	ਲਈ (fp = fr->fr_ਬੱਚਾ; fp->fr_ਅਗਲਾ != ਨਿਰਾ; fp = fp->fr_ਅਗਲਾ)
	    ;
	ਆਖਰੀ_ਸਥਿਤੀ_rec(fp, statusline);
    }
}

#ਜੇ defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    ਖਾਲੀ
update_stl_rendered_ਉਚਾਈ(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == ਨਿਰਾ) ? p_stl : wp->w_p_stl;
    ਪੂਰਨ		opt_flags = (wp == ਨਿਰਾ) ? 0 : OPT_LOCAL;
    ਪੂਰਨ		*ਉਚਾਈ = (wp == ਨਿਰਾ) ? &stl_rendered_ਉਚਾਈ
						: &wp->w_stl_rendered_ਉਚਾਈ;

    *ਉਚਾਈ = get_stl_rendered_ਉਚਾਈ(wp == ਨਿਰਾ ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    ਸਥਿਰ ਪੂਰਨ
ਚੌਖਟਾ_ਲੱਭੋ_global_stlh_rec(ਚੌਖਟਾ_T *frp, ਪੂਰਨ h)
{
    ਜੇ (frp->fr_layout == FR_ਪੱਤੀ)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	ਜੇ (wp->w_ਉਚਾਈ > 0 && wp->w_ਸਥਿਤੀ_ਉਚਾਈ > 0
		&& *wp->w_p_stlo == NUL)
	{
	    ਪੂਰਨ win_free_ਉਚਾਈ = frp->fr_ਉਚਾਈ - WINBAR_ਉਚਾਈ(wp);

	    ਜੇ (win_free_ਉਚਾਈ - p_wmh < h)
		h = win_free_ਉਚਾਈ - p_wmh;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (frp = frp->fr_ਬੱਚਾ; frp != ਨਿਰਾ; frp = frp->fr_ਅਗਲਾ)
	    h = ਚੌਖਟਾ_ਲੱਭੋ_global_stlh_rec(frp, h);
    }
    ਵਾਪਸ h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    ਸਥਿਰ ਖਾਲੀ
ਚੌਖਟਾ_change_statusline_ਉਚਾਈ_rec(ਚੌਖਟਾ_T *frp, ਪੂਰਨ global_stlh)
{
    ਜੇ (frp->fr_layout == FR_ਪੱਤੀ)
    {
	win_T  *wp = frp->fr_win;

	ਜੇ (wp->w_ਉਚਾਈ > 0 && wp->w_ਸਥਿਤੀ_ਉਚਾਈ > 0)
	{
	    ਪੂਰਨ win_free_ਉਚਾਈ = frp->fr_ਉਚਾਈ - WINBAR_ਉਚਾਈ(wp);

	    wp->w_ਸਥਿਤੀ_ਉਚਾਈ = statusline_ਉਚਾਈ(wp);
	    ਜੇ (wp->w_p_stlo == ਨਿਰਾ || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ > global_stlh)
		    wp->w_ਸਥਿਤੀ_ਉਚਾਈ = global_stlh;
	    }
	    ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ > win_free_ਉਚਾਈ - p_wmh)
		wp->w_ਸਥਿਤੀ_ਉਚਾਈ = win_free_ਉਚਾਈ - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    ਪੂਰਨ	     saved_topfill = wp->w_topfill;
# endif
	    win_new_ਉਚਾਈ(wp, win_free_ਉਚਾਈ - wp->w_ਸਥਿਤੀ_ਉਚਾਈ);
	    wp->w_topline = saved_topline;
# ifdef FEAT_DIFF
	    wp->w_topfill = saved_topfill;
# endif
	    wp->w_valid &= ~(VALID_WROW | VALID_CROW
			     | VALID_BOTLINE | VALID_BOTLINE_AP);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (frp = frp->fr_ਬੱਚਾ; frp != ਨਿਰਾ; frp = frp->fr_ਅਗਲਾ)
	    ਚੌਖਟਾ_change_statusline_ਉਚਾਈ_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    ਪੂਰਨ
ਚੌਖਟਾ_change_statusline_ਉਚਾਈ(ਖਾਲੀ)
{
    tabpage_T	*tp;
    ਪੂਰਨ		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = ਚੌਖਟਾ_ਲੱਭੋ_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	ਚੌਖਟਾ_change_statusline_ਉਚਾਈ_rec(tp->tp_topframe, global_stlh);
    comp_ਖਾਨਾ();
    redraw_all_later(UPD_SOME_VALID);
    ਵਾਪਸ global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    ਖਾਲੀ
ਸੈੱਟ_stlo_mh(ਪੂਰਨ mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
statuslineopt_ਬਦਲਿਆ(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    ਪੂਰਨ		l_stlo_fh = FALSE;
    ਪੂਰਨ		l_stlo_mh = 1;

    ਜੇ (stlopt != ਨਿਰਾ)
	p = stlopt;
    ਨਹੀਂਤਾਂ
	p = wp->w_p_stlo;

    ਜਦੋਂ (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	ਜੇ (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    ਜੇ (l_stlo_mh < 1)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	ਜੇ (*p != ',' && *p != NUL)
	    ਵਾਪਸ FAIL;
	ਜੇ (*p == ',')
	    ++p;
    }

    ਜੇ (wp == ਨਿਰਾ)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    ਨਹੀਂਤਾਂ
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    ਪੂਰਨ
statusline_ਉਚਾਈ(win_T *wp ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ stl_ਉਚਾਈ = 1;
#ਜੇ defined(FEAT_STL_OPT)
    bool fixed;
    ਪੂਰਨ l_stl_rendered_ਉਚਾਈ;

    ਜੇ (wp != ਨਿਰਾ && *wp->w_p_stlo != NUL)
    {
	stl_ਉਚਾਈ = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_ਉਚਾਈ = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_ਉਚਾਈ
				    : stl_rendered_ਉਚਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	stl_ਉਚਾਈ = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_ਉਚਾਈ = (wp != ਨਿਰਾ && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_ਉਚਾਈ
				    : stl_rendered_ਉਚਾਈ;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    ਜੇ (!fixed)
    {
	ਪੂਰਨ has_custom_stl = (*p_stl != NUL)
	    || (wp != ਨਿਰਾ && *wp->w_p_stl != NUL);
	ਜੇ (has_custom_stl)
	    stl_ਉਚਾਈ = MAX(1, MIN(l_stl_rendered_ਉਚਾਈ, stl_ਉਚਾਈ));
	ਨਹੀਂਤਾਂ
	    stl_ਉਚਾਈ = 1;
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ stl_ਉਚਾਈ;
}

/*
 * Return the number of lines used by the tab page line.
 */
    ਪੂਰਨ
tabline_ਉਚਾਈ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    ਜੇ (gui_use_tabline())
	ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
    ਚੋਣ (p_stal)
    {
	ਹਾਲਤ 0: ਵਾਪਸ 0;
	ਹਾਲਤ 1: ਵਾਪਸ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ == ਨਿਰਾ) ? 0 : 1;
    }
    ਵਾਪਸ 1;
}

/*
 * Return the height of the last window's statusline.
 */
    ਪੂਰਨ
ਆਖਰੀ_stl_ਉਚਾਈ(
    ਪੂਰਨ		morewin)	// pretend there are two or more windows
{
    ਵਾਪਸ (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_ਖਿੜਕੀ)))
		? ਸਥਿਤੀ_ਉਚਾਈ_2 : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    ਪੂਰਨ
min_ਪੰਕਤੀਆਂ(ਖਾਲੀ)
{
    ਜੇ (firstwin == ਨਿਰਾ)	// not initialized yet
	ਵਾਪਸ MIN_ਸਤਰਾਂ;

    ਵਾਪਸ ਚੌਖਟਾ_minheight(curtab->tp_topframe, ਨਿਰਾ) + tabline_ਉਚਾਈ()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    ਪੂਰਨ
min_ਪੰਕਤੀਆਂ_for_all_tabpages(ਖਾਲੀ)
{
    ਪੂਰਨ		ਕੁੱਲ;
    tabpage_T	*tp;
    ਪੂਰਨ		n;

    ਜੇ (firstwin == ਨਿਰਾ)	// not initialized yet
	ਵਾਪਸ MIN_ਸਤਰਾਂ;

    ਕੁੱਲ = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = ਚੌਖਟਾ_minheight(tp->tp_topframe, ਨਿਰਾ);
	ਜੇ (ਕੁੱਲ < n)
	    ਕੁੱਲ = n;
    }
    ਕੁੱਲ += tabline_ਉਚਾਈ();
    ਕੁੱਲ += MIN_CMDHEIGHT;
    ਵਾਪਸ ਕੁੱਲ;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    ਪੂਰਨ
only_one_ਖਿੜਕੀ(ਖਾਲੀ)
{
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    win_T	*wp;

#ਜੇ defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    ਜੇ (popup_is_popup(curwin))
	ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ

    // If there is another tab page there always is another window.
    ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_ਬਫਰ != ਨਿਰਾ
		&& (!((bt_help(wp->w_ਬਫਰ) && !bt_help(curbuf))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		    || wp->w_p_pvw
#ਜੇ_ਅੰਤ
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++ਗਿਣਤੀ_2;
    ਵਾਪਸ (ਗਿਣਤੀ_2 <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    ਸਥਿਰ ਖਾਲੀ
check_lnums_both(ਪੂਰਨ do_curwin, ਪੂਰਨ nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ਜੇ ((do_curwin || wp != curwin) && wp->w_ਬਫਰ == curbuf)
	{
	    ਪੂਰਨ need_adjust;

	    ਜੇ (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਜੇ (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਜੇ (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਜੇ (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਜੇ (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    ਖਾਲੀ
check_lnums(ਪੂਰਨ do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    ਖਾਲੀ
check_lnums_nested(ਪੂਰਨ do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    ਖਾਲੀ
reset_lnums(ਖਾਲੀ)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ਜੇ (wp->w_ਬਫਰ == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    ਜੇ (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    ਜੇ (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    ਜੇ (wp->w_save_cursor.w_topline_save > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		wp->w_valid &= ~VALID_TOPLINE;
	}
}

/*
 * A snapshot of the window sizes, to restore them after closing the help
 * or other window.
 * Only these fields are used:
 * fr_layout
 * fr_width
 * fr_height
 * fr_next
 * fr_child
 * fr_win (only valid for the old curwin, NULL otherwise)
 */

/*
 * Create a snapshot of the current frame sizes.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 * Return FAIL if out of memory, OK otherwise.
 */
    ਪੂਰਨ
make_snapshot(ਪੂਰਨ idx)
{
    clear_snapshot(curtab, idx);
    ਜੇ (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
make_snapshot_rec(ਚੌਖਟਾ_T *fr, ਚੌਖਟਾ_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(ਚੌਖਟਾ_T);
    ਜੇ (*frp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_ਚੌੜਾਈ = fr->fr_ਚੌੜਾਈ;
    (*frp)->fr_ਉਚਾਈ = fr->fr_ਉਚਾਈ;
    ਜੇ (fr->fr_ਅਗਲਾ != ਨਿਰਾ)
    {
	ਜੇ (make_snapshot_rec(fr->fr_ਅਗਲਾ, &((*frp)->fr_ਅਗਲਾ)) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਜੇ (fr->fr_ਬੱਚਾ != ਨਿਰਾ)
    {
	ਜੇ (make_snapshot_rec(fr->fr_ਬੱਚਾ, &((*frp)->fr_ਬੱਚਾ)) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਜੇ (fr->fr_layout == FR_ਪੱਤੀ && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    ਵਾਪਸ OK;
}

/*
 * Remove any existing snapshot.
 */
    ਸਥਿਰ ਖਾਲੀ
clear_snapshot(tabpage_T *tp, ਪੂਰਨ idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
clear_snapshot_rec(ਚੌਖਟਾ_T *fr)
{
    ਜੇ (fr == ਨਿਰਾ)
	ਵਾਪਸ;
    clear_snapshot_rec(fr->fr_ਅਗਲਾ);
    clear_snapshot_rec(fr->fr_ਬੱਚਾ);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    ਸਥਿਰ win_T *
get_snapshot_curwin_rec(ਚੌਖਟਾ_T *ft)
{
    win_T	*wp;

    ਜੇ (ft->fr_ਅਗਲਾ != ਨਿਰਾ)
    {
	ਜੇ ((wp = get_snapshot_curwin_rec(ft->fr_ਅਗਲਾ)) != ਨਿਰਾ)
	    ਵਾਪਸ wp;
    }
    ਜੇ (ft->fr_ਬੱਚਾ != ਨਿਰਾ)
    {
	ਜੇ ((wp = get_snapshot_curwin_rec(ft->fr_ਬੱਚਾ)) != ਨਿਰਾ)
	    ਵਾਪਸ wp;
    }

    ਵਾਪਸ ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    ਸਥਿਰ win_T *
get_snapshot_curwin(ਪੂਰਨ idx)
{
    ਜੇ (curtab->tp_snapshot[idx] == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    ਖਾਲੀ
restore_snapshot(
    ਪੂਰਨ		idx,
    ਪੂਰਨ		close_curwin)	    // closing current window
{
    win_T	*wp;

    ਜੇ (curtab->tp_snapshot[idx] != ਨਿਰਾ
	    && curtab->tp_snapshot[idx]->fr_ਚੌੜਾਈ == topframe->fr_ਚੌੜਾਈ
	    && curtab->tp_snapshot[idx]->fr_ਉਚਾਈ == topframe->fr_ਉਚਾਈ
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	ਜੇ (wp != ਨਿਰਾ && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    ਸਥਿਰ ਪੂਰਨ
check_snapshot_rec(ਚੌਖਟਾ_T *sn, ਚੌਖਟਾ_T *fr)
{
    ਜੇ (sn->fr_layout != fr->fr_layout
	    || (sn->fr_ਅਗਲਾ == ਨਿਰਾ) != (fr->fr_ਅਗਲਾ == ਨਿਰਾ)
	    || (sn->fr_ਬੱਚਾ == ਨਿਰਾ) != (fr->fr_ਬੱਚਾ == ਨਿਰਾ)
	    || (sn->fr_ਅਗਲਾ != ਨਿਰਾ
		&& check_snapshot_rec(sn->fr_ਅਗਲਾ, fr->fr_ਅਗਲਾ) == FAIL)
	    || (sn->fr_ਬੱਚਾ != ਨਿਰਾ
		&& check_snapshot_rec(sn->fr_ਬੱਚਾ, fr->fr_ਬੱਚਾ) == FAIL)
	    || (sn->fr_win != ਨਿਰਾ && !win_valid(sn->fr_win)))
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    ਸਥਿਰ win_T *
restore_snapshot_rec(ਚੌਖਟਾ_T *sn, ਚੌਖਟਾ_T *fr)
{
    win_T	*wp = ਨਿਰਾ;
    win_T	*wp2;

    fr->fr_ਉਚਾਈ = sn->fr_ਉਚਾਈ;
    fr->fr_ਚੌੜਾਈ = sn->fr_ਚੌੜਾਈ;
    ਜੇ (fr->fr_layout == FR_ਪੱਤੀ)
    {
	ਚੌਖਟਾ_new_ਉਚਾਈ(fr, fr->fr_ਉਚਾਈ, FALSE, FALSE, FALSE);
	ਚੌਖਟਾ_new_ਚੌੜਾਈ(fr, fr->fr_ਚੌੜਾਈ, FALSE, FALSE);
	wp = sn->fr_win;
    }
    ਜੇ (sn->fr_ਅਗਲਾ != ਨਿਰਾ)
    {
	wp2 = restore_snapshot_rec(sn->fr_ਅਗਲਾ, fr->fr_ਅਗਲਾ);
	ਜੇ (wp2 != ਨਿਰਾ)
	    wp = wp2;
    }
    ਜੇ (sn->fr_ਬੱਚਾ != ਨਿਰਾ)
    {
	wp2 = restore_snapshot_rec(sn->fr_ਬੱਚਾ, fr->fr_ਬੱਚਾ);
	ਜੇ (wp2 != ਨਿਰਾ)
	    wp = wp2;
    }
    ਵਾਪਸ wp;
}

#ਜੇ defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    ਪੂਰਨ
win_hasvertsplit(ਖਾਲੀ)
{
    ਚੌਖਟਾ_T	*fr;

    ਜੇ (topframe->fr_layout == FR_ਪੰਕਤੀ)
	ਵਾਪਸ TRUE;

    ਜੇ (topframe->fr_layout == FR_ਖਾਨਾ)
	FOR_ALL_ਚੌਖਟੇ(fr, topframe->fr_ਬੱਚਾ)
	    ਜੇ (fr->fr_layout == FR_ਪੰਕਤੀ)
		ਵਾਪਸ TRUE;

    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    ਪੂਰਨ
get_win_ਸੰਖਿਆ(win_T *wp, win_T *ਪਹਿਲਾ_win)
{
    ਪੂਰਨ		i = 1;
    win_T	*w;

    ਲਈ (w = ਪਹਿਲਾ_win; w != ਨਿਰਾ && w != wp; w = W_ਅਗਲਾ(w))
	++i;

    ਜੇ (w == ਨਿਰਾ)
	ਵਾਪਸ 0;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ i;
}

    ਪੂਰਨ
get_tab_ਸੰਖਿਆ(tabpage_T *tp)
{
    ਪੂਰਨ		i = 1;
    tabpage_T	*t;

    ਲਈ (t = ਪਹਿਲਾ_tabpage; t != ਨਿਰਾ && t != tp; t = t->tp_ਅਗਲਾ)
	++i;

    ਜੇ (t == ਨਿਰਾ)
	ਵਾਪਸ 0;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ i;
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    ਸਥਿਰ ਪੂਰਨ
ਚੌਖਟਾ_check_ਉਚਾਈ(ਚੌਖਟਾ_T *topfrp, ਪੂਰਨ ਉਚਾਈ)
{
    ਚੌਖਟਾ_T *frp;

    ਜੇ (topfrp->fr_ਉਚਾਈ != ਉਚਾਈ)
	ਵਾਪਸ FALSE;

    ਜੇ (topfrp->fr_layout == FR_ਪੰਕਤੀ)
	FOR_ALL_ਚੌਖਟੇ(frp, topfrp->fr_ਬੱਚਾ)
	    ਜੇ (frp->fr_ਉਚਾਈ != ਉਚਾਈ)
		ਵਾਪਸ FALSE;

    ਵਾਪਸ TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    ਸਥਿਰ ਪੂਰਨ
ਚੌਖਟਾ_check_ਚੌੜਾਈ(ਚੌਖਟਾ_T *topfrp, ਪੂਰਨ ਚੌੜਾਈ)
{
    ਚੌਖਟਾ_T *frp;

    ਜੇ (topfrp->fr_ਚੌੜਾਈ != ਚੌੜਾਈ)
	ਵਾਪਸ FALSE;

    ਜੇ (topfrp->fr_layout == FR_ਖਾਨਾ)
	FOR_ALL_ਚੌਖਟੇ(frp, topfrp->fr_ਬੱਚਾ)
	    ਜੇ (frp->fr_ਚੌੜਾਈ != ਚੌੜਾਈ)
		ਵਾਪਸ FALSE;

    ਵਾਪਸ TRUE;
}

#ਜੇ defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    ਸਥਿਰ ਪੂਰਨ
int_cmp(ਅਚਲ ਖਾਲੀ *pa, ਅਚਲ ਖਾਲੀ *pb)
{
    ਅਚਲ ਪੂਰਨ a = *(ਅਚਲ ਪੂਰਨ *)pa;
    ਅਚਲ ਪੂਰਨ b = *(ਅਚਲ ਪੂਰਨ *)pb;
    ਜੇ (a > b)
	ਵਾਪਸ 1;
    ਜੇ (a < b)
	ਵਾਪਸ -1;
    ਵਾਪਸ 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    ਅੱਖਰ *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = ਖਾਲੀ_ਥਾਵਾਂ_option;
    ਪੂਰਨ		tw;
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    ਪੂਰਨ		color_ਖਾਨੇ_ਗਿਣਤੀ[256];
    ਪੂਰਨ		i;
    ਪੂਰਨ		j = 0;

    ਜੇ (wp != ਨਿਰਾ && wp->w_ਬਫਰ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;  // buffer was closed

    ਜੇ (cc != ਨਿਰਾ)
	s = cc;
    ਨਹੀਂਤਾਂ ਜੇ (wp != ਨਿਰਾ)
	s = wp->w_p_cc;

    ਜੇ (wp != ਨਿਰਾ)
	tw = wp->w_ਬਫਰ->b_p_tw;
    ਨਹੀਂਤਾਂ
	// buffer-local value not set, assume zero
	tw = 0;

    ਜਦੋਂ (*s != NUL && ਗਿਣਤੀ_2 < 255)
    {
	ਜੇ (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    ਖਾਨਾ = (*s == '-') ? -1 : 1;
	    ++s;
	    ਜੇ (!VIM_ISDIGIT(*s))
		ਵਾਪਸ e_invalid_ਦਲੀਲ;
	    ਖਾਨਾ = ਖਾਨਾ * getdigits(&s);
	    ਜੇ (tw == 0)
		ਜਾਓ skip;  // 'textwidth' not set, skip this item
	    ਖਾਨਾ += tw;
	    ਜੇ (ਖਾਨਾ < 0)
		ਜਾਓ skip;
	}
	ਨਹੀਂਤਾਂ ਜੇ (VIM_ISDIGIT(*s))
	    ਖਾਨਾ = getdigits(&s);
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
	color_ਖਾਨੇ_ਗਿਣਤੀ[ਗਿਣਤੀ_2++] = ਖਾਨਾ - 1;  // 1-based to 0-based
skip:
	ਜੇ (*s == NUL)
	    ਰੋਕੋ;
	ਜੇ (*s != ',')
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
	ਜੇ (*++s == NUL)
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;  // illegal trailing comma as in "set cc=80,"
    }

    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;  // only parse "cc"

    vim_free(wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ);
    ਜੇ (ਗਿਣਤੀ_2 == 0)
	wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ = ALLOC_MULT(ਪੂਰਨ, ਗਿਣਤੀ_2 + 1);
	ਜੇ (wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ != ਨਿਰਾ)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_ਖਾਨੇ_ਗਿਣਤੀ, ਗਿਣਤੀ_2, ਆਕਾਰ(ਪੂਰਨ), int_cmp);

	    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
		// skip duplicates
		ਜੇ (j == 0 || wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ[j - 1] != color_ਖਾਨੇ_ਗਿਣਤੀ[i])
		    wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ[j++] = color_ਖਾਨੇ_ਗਿਣਤੀ[i];
	    wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ[j] = -1;  // end marker
	}
    }

    ਵਾਪਸ ਨਿਰਾ;  // no error
}
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
get_ਆਖਰੀ_winid(ਖਾਲੀ)
{
    ਵਾਪਸ ਆਖਰੀ_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   ਪੂਰਨ
win_locked(win_T *wp)
{
    ਵਾਪਸ wp->w_locked;
}
