/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ ಶೂನ್ಯ cmd_with_ಎಣಿಕೆ(ಅಕ್ಷರ *cmd, char_u *bufp, size_t bufsize, ದೀರ್ಘ Prenum);
ಸ್ಥಿರ ಶೂನ್ಯ win_init(win_T *newp, win_T *oldp, ಪೂರ್ಣಾಂಕ flags);
ಸ್ಥಿರ ಶೂನ್ಯ win_init_some(win_T *newp, win_T *oldp);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_comp_pos(ಚೌಕಟ್ಟು_T *topfrp, ಪೂರ್ಣಾಂಕ *ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ *ಲಂಬಸಾಲು);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_setheight(ಚೌಕಟ್ಟು_T *curfrp, ಪೂರ್ಣಾಂಕ ಎತ್ತರ);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_setwidth(ಚೌಕಟ್ಟು_T *curfrp, ಪೂರ್ಣಾಂಕ ಅಗಲ);
ಸ್ಥಿರ ಶೂನ್ಯ win_exchange(ದೀರ್ಘ);
ಸ್ಥಿರ ಶೂನ್ಯ win_rotate(ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ win_equal_rec(win_T *ಮುಂದಿನ_curwin, ಪೂರ್ಣಾಂಕ current, ಚೌಕಟ್ಟು_T *topfr, ಪೂರ್ಣಾಂಕ dir, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ);
ಸ್ಥಿರ ಶೂನ್ಯ trigger_winnewpre(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ trigger_winclosed(win_T *win);
ಸ್ಥಿರ win_T *win_free_mem(win_T *win, ಪೂರ್ಣಾಂಕ *dirp, tabpage_T *tp);
ಸ್ಥಿರ ಚೌಕಟ್ಟು_T *win_altframe(win_T *win, tabpage_T *tp);
ಸ್ಥಿರ tabpage_T *alt_tabpage(ಶೂನ್ಯ);
ಸ್ಥಿರ win_T *frame2win(ಚೌಕಟ್ಟು_T *frp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_has_win(ಚೌಕಟ್ಟು_T *frp, win_T *wp);
ಸ್ಥಿರ ಶೂನ್ಯ win_fix_scroll(ಪೂರ್ಣಾಂಕ resize);
ಸ್ಥಿರ ಶೂನ್ಯ win_fix_cursor(ಪೂರ್ಣಾಂಕ ಸಾಮಾನ್ಯ_4);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_new_ಎತ್ತರ(ಚೌಕಟ್ಟು_T *topfrp, ಪೂರ್ಣಾಂಕ ಎತ್ತರ, ಪೂರ್ಣಾಂಕ topfirst, ಪೂರ್ಣಾಂಕ wfh, ಪೂರ್ಣಾಂಕ ಗಣ_ch);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(ಚೌಕಟ್ಟು_T *frp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_fixed_ಅಗಲ(ಚೌಕಟ್ಟು_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_ಕೂಡಿಸು_statusline(ಚೌಕಟ್ಟು_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_new_ಅಗಲ(ಚೌಕಟ್ಟು_T *topfrp, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ leftfirst, ಪೂರ್ಣಾಂಕ wfw);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_ಗಣ_vsep(ಚೌಕಟ್ಟು_T *frp, ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_minwidth(ಚೌಕಟ್ಟು_T *topfrp, win_T *ಮುಂದಿನ_curwin);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_fix_ಅಗಲ(win_T *wp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ win_alloc_firstwin(win_T *oldwin);
ಸ್ಥಿರ ಶೂನ್ಯ new_ಚೌಕಟ್ಟು(win_T *wp);
ಸ್ಥಿರ tabpage_T *alloc_tabpage(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ leave_tabpage(buf_T *new_curbuf, ಪೂರ್ಣಾಂಕ trigger_leave_autocmds);
ಸ್ಥಿರ ಶೂನ್ಯ enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, ಪೂರ್ಣಾಂಕ trigger_enter_autocmds, ಪೂರ್ಣಾಂಕ trigger_leave_autocmds);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_fix_ಎತ್ತರ(win_T *wp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_minheight(ಚೌಕಟ್ಟು_T *topfrp, win_T *ಮುಂದಿನ_curwin);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_wincount_ಒಳ_ಎತ್ತರ(ಚೌಕಟ್ಟು_T *topfrp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ may_open_tabpage(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ win_enter_ext(win_T *wp, ಪೂರ್ಣಾಂಕ flags);
ಸ್ಥಿರ ಶೂನ್ಯ win_free(win_T *wp, tabpage_T *tp);
ಸ್ಥಿರ ಶೂನ್ಯ win_append(win_T *after, win_T *wp);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_append(ಚೌಕಟ್ಟು_T *after, ಚೌಕಟ್ಟು_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_ಸೇರಿಸು(ಚೌಕಟ್ಟು_T *before, ಚೌಕಟ್ಟು_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_remove(ಚೌಕಟ್ಟು_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ win_goto_ver(ಪೂರ್ಣಾಂಕ ಮೇಲೆ, ದೀರ್ಘ ಎಣಿಕೆ_2);
ಸ್ಥಿರ ಶೂನ್ಯ win_goto_hor(ಪೂರ್ಣಾಂಕ ಎಡ_2, ದೀರ್ಘ ಎಣಿಕೆ_2);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_ಕೂಡಿಸು_ಎತ್ತರ(ಚೌಕಟ್ಟು_T *frp, ಪೂರ್ಣಾಂಕ n);
ಸ್ಥಿರ ಶೂನ್ಯ ಕೊನೆಯ_ಸ್ಥಿತಿ_rec(ಚೌಕಟ್ಟು_T *fr, ಪೂರ್ಣಾಂಕ statusline);
#ಆದರೆ defined(FEAT_STL_OPT)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_ಕಂಡುಹಿಡಿ_global_stlh_rec(ಚೌಕಟ್ಟು_T *frp, ಪೂರ್ಣಾಂಕ h);
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ_rec(ಚೌಕಟ್ಟು_T *frp, ಪೂರ್ಣಾಂಕ global_stlh);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ ಚೌಕಟ್ಟು_flatten(ಚೌಕಟ್ಟು_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ winframe_restore(win_T *wp, ಪೂರ್ಣಾಂಕ dir, ಚೌಕಟ್ಟು_T *unflat_altfr);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ make_snapshot_rec(ಚೌಕಟ್ಟು_T *fr, ಚೌಕಟ್ಟು_T **frp);
ಸ್ಥಿರ ಶೂನ್ಯ clear_snapshot(tabpage_T *tp, ಪೂರ್ಣಾಂಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ clear_snapshot_rec(ಚೌಕಟ್ಟು_T *fr);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_snapshot_rec(ಚೌಕಟ್ಟು_T *sn, ಚೌಕಟ್ಟು_T *fr);
ಸ್ಥಿರ win_T *restore_snapshot_rec(ಚೌಕಟ್ಟು_T *sn, ಚೌಕಟ್ಟು_T *fr);
ಸ್ಥಿರ win_T *get_snapshot_curwin(ಪೂರ್ಣಾಂಕ idx);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_check_ಎತ್ತರ(ಚೌಕಟ್ಟು_T *topfrp, ಪೂರ್ಣಾಂಕ ಎತ್ತರ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_check_ಅಗಲ(ಚೌಕಟ್ಟು_T *topfrp, ಪೂರ್ಣಾಂಕ ಅಗಲ);

ಸ್ಥಿರ win_T *win_alloc(win_T *after, ಪೂರ್ಣಾಂಕ hidden);

#ವ್ಯಾಖ್ಯಾನಿಸು NOWIN		((win_T *)-1)	// non-existing window

#ವ್ಯಾಖ್ಯಾನಿಸು ಅಡ್ಡಸಾಲುಗಳು_AVAIL (ಅಡ್ಡಸಾಲುಗಳು - p_ch - tabline_ಎತ್ತರ())

// flags for win_enter_ext()
#ವ್ಯಾಖ್ಯಾನಿಸು WEE_UNDO_SYNC			0x01
#ವ್ಯಾಖ್ಯಾನಿಸು WEE_CURWIN_INVALID		0x02
#ವ್ಯಾಖ್ಯಾನಿಸು WEE_TRIGGER_NEW_AUTOCMDS	0x04
#ವ್ಯಾಖ್ಯಾನಿಸು WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#ವ್ಯಾಖ್ಯಾನಿಸು WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#ವ್ಯಾಖ್ಯಾನಿಸು WEE_ALLOW_PARSE_MESSAGES	0x20

ಸ್ಥಿರ ಅಕ್ಷರ *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ವಿಭಜಿಸು_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಚೌಕಟ್ಟು_locked = 0;

#ಆದರೆ defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ stl_rendered_ಎತ್ತರ = 1;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ stlo_mh = ಸ್ಥಿತಿ_ಎತ್ತರ_2; // "maxheight" value cache of global 'stlo'
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    ಶೂನ್ಯ
ಕಿಟಕಿ_layout_ಬೀಗ(ಶೂನ್ಯ)
{
    ++ವಿಭಜಿಸು_disallowed;
    ++close_disallowed;
}

    ಶೂನ್ಯ
ಕಿಟಕಿ_layout_unlock(ಶೂನ್ಯ)
{
    --ವಿಭಜಿಸು_disallowed;
    --close_disallowed;
}

    ಪೂರ್ಣಾಂಕ
ಚೌಕಟ್ಟುಗಳು_locked(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ಚೌಕಟ್ಟು_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    ಪೂರ್ಣಾಂಕ
ಕಿಟಕಿ_layout_locked(ಎಣಿಕೆ CMD_ಸೂಚ್ಯಂಕ cmd)
{
    ಆದರೆ (ವಿಭಜಿಸು_disallowed > 0 || close_disallowed > 0)
    {
	ಆದರೆ (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_ವಿಭಜಿಸು_ಕಿಟಕಿ_when_closing_ಬಫರ್));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_not_allowed_ಗಮ್ಯ_change_ಕಿಟಕಿ_layout_ಒಳ_this_autocmd));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

// #define WIN_DEBUG
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
log_ಚೌಕಟ್ಟು_layout(ಚೌಕಟ್ಟು_T *ಚೌಕಟ್ಟು)
{
    ch_log(ಖಾಲಿ, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    ಚೌಕಟ್ಟು->fr_layout == FR_ಎಲೆ ? "LEAF"
				  : ಚೌಕಟ್ಟು->fr_layout == FR_ಅಡ್ಡಸಾಲು ? "ROW" : "COL",
	    ಚೌಕಟ್ಟು->fr_ಅಗಲ,
	    ಚೌಕಟ್ಟು->fr_ಎತ್ತರ,
	    ಚೌಕಟ್ಟು->fr_win == ಖಾಲಿ ? -1 : ಚೌಕಟ್ಟು->fr_win->w_ಅಗಲ,
	    ಚೌಕಟ್ಟು->fr_win == ಖಾಲಿ ? -1 : ಚೌಕಟ್ಟು->fr_win->w_ಎತ್ತರ,
	    ಚೌಕಟ್ಟು->fr_win == ಖಾಲಿ ? -1 : ಚೌಕಟ್ಟು->fr_win->w_id);
    ಆದರೆ (ಚೌಕಟ್ಟು->fr_ಮಗು != ಖಾಲಿ)
    {
	ch_log(ಖಾಲಿ, "children");
	log_ಚೌಕಟ್ಟು_layout(ಚೌಕಟ್ಟು->fr_ಮಗು);
	ಆದರೆ (ಚೌಕಟ್ಟು->fr_ಮುಂದಿನ != ಖಾಲಿ)
	    ch_log(ಖಾಲಿ, "END of children");
    }
    ಆದರೆ (ಚೌಕಟ್ಟು->fr_ಮುಂದಿನ != ಖಾಲಿ)
	log_ಚೌಕಟ್ಟು_layout(ಚೌಕಟ್ಟು->fr_ಮುಂದಿನ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    ಪೂರ್ಣಾಂಕ
check_can_ಗಣ_curbuf_disabled(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_ಗಮ್ಯ_ಬಫರ್));
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    ಪೂರ್ಣಾಂಕ
check_can_ಗಣ_curbuf_forceit(ಪೂರ್ಣಾಂಕ forceit)
{
    ಆದರೆ (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_ಗಮ್ಯ_ಬಫರ್));
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(ಶೂನ್ಯ)
{
    // In cmdwin, the alternative buffer should be used.
    ಹಿಂತಿರುಗಿ is_ಒಳ_cmdwin() && prevwin != ಖಾಲಿ ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = ಖಾಲಿ;

    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    ಆದರೆ (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    ಆದರೆ (wp == ಖಾಲಿ && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    ಹಿಂತಿರುಗಿ wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    ಶೂನ್ಯ
do_ಕಿಟಕಿ(
    ಪೂರ್ಣಾಂಕ		nchar,
    ದೀರ್ಘ	Prenum,
    ಪೂರ್ಣಾಂಕ		xchar)	    // extra char from ":wincmd gx" or NUL
{
    ದೀರ್ಘ	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
    ಪೂರ್ಣಾಂಕ		type = ಕಂಡುಹಿಡಿ_DEFINE;
    ಪೂರ್ಣಾಂಕ		len;
#ಆದರೆ_ಅಂತ್ಯ
    char_u	cbuf[40];

    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿಸು CHECK_CMDWIN \
    ಮಾಡು { \
	ಆದರೆ (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_ಒಳ_cmdline_ಕಿಟಕಿ)); \
	    ಹಿಂತಿರುಗಿ; \
	} \
    } ಆಗ (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    ಆಯ್ಕೆ (nchar)
    {
// split current window in two parts, horizontally
    ಸ್ಥಿತಿ 'S':
    ಸ್ಥಿತಿ Ctrl_S:
    ಸ್ಥಿತಿ 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		ಆದರೆ (bt_quickfix(curbuf))
		    ಹೋಗು newwindow;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		(ಶೂನ್ಯ)win_ವಿಭಜಿಸು((ಪೂರ್ಣಾಂಕ)Prenum, 0);
		ನಿಲ್ಲಿಸು;

// split current window in two parts, vertically
    ಸ್ಥಿತಿ Ctrl_V:
    ಸ್ಥಿತಿ 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		ಆದರೆ (bt_quickfix(curbuf))
		    ಹೋಗು newwindow;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		(ಶೂನ್ಯ)win_ವಿಭಜಿಸು((ಪೂರ್ಣಾಂಕ)Prenum, WSP_VERT);
		ನಿಲ್ಲಿಸು;

// split current window and edit alternate file
    ಸ್ಥಿತಿ Ctrl_HAT:
    ಸ್ಥಿತಿ '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		ಆದರೆ (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == ಖಾಲಿ)
		{
		    ಆದರೆ (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    ಇಲ್ಲದಿದ್ದರೆ
			semsg(_(e_ಬಫರ್_nr_not_ಸಿಕ್ಕಿತು), Prenum);
		    ನಿಲ್ಲಿಸು;
		}

		ಆದರೆ (!curbuf_locked() && win_ವಿಭಜಿಸು(0, 0) == OK)
		    (ಶೂನ್ಯ)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		ನಿಲ್ಲಿಸು;

// open new window
    ಸ್ಥಿತಿ Ctrl_N:
    ಸ್ಥಿತಿ 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		ಆದರೆ (Prenum)
		    // window height
		    vim_snprintf((ಅಕ್ಷರ *)cbuf, ಗಾತ್ರ(cbuf) - 5, "%ld", Prenum);
		ಇಲ್ಲದಿದ್ದರೆ
		    cbuf[0] = NUL;
#ಆದರೆ defined(FEAT_QUICKFIX)
		ಆದರೆ (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#ಆದರೆ_ಅಂತ್ಯ
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		ನಿಲ್ಲಿಸು;

// quit current window
    ಸ್ಥಿತಿ Ctrl_Q:
    ಸ್ಥಿತಿ 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ಎಣಿಕೆ("quit", cbuf, ಗಾತ್ರ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ನಿಲ್ಲಿಸು;

// close current window
    ಸ್ಥಿತಿ Ctrl_C:
    ಸ್ಥಿತಿ 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ಎಣಿಕೆ("close", cbuf, ಗಾತ್ರ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ನಿಲ್ಲಿಸು;

#ಆದರೆ defined(FEAT_QUICKFIX)
// close preview window
    ಸ್ಥಿತಿ Ctrl_Z:
    ಸ್ಥಿತಿ 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		ನಿಲ್ಲಿಸು;

// cursor to preview window
    ಸ್ಥಿತಿ 'P':
		FOR_ALL_WINDOWS(wp)
		    ಆದರೆ (wp->w_p_pvw)
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (wp == ಖಾಲಿ)
		    emsg(_(e_there_is_no_preview_ಕಿಟಕಿ));
		ಇಲ್ಲದಿದ್ದರೆ
		    win_goto(wp);
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

// close all but current window
    ಸ್ಥಿತಿ Ctrl_O:
    ಸ್ಥಿತಿ 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ಎಣಿಕೆ("only", cbuf, ಗಾತ್ರ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ನಿಲ್ಲಿಸು;

// cursor to next window with wrap around
    ಸ್ಥಿತಿ Ctrl_W:
    ಸ್ಥಿತಿ 'w':
// cursor to previous window with wrap around
    ಸ್ಥಿತಿ 'W':
		CHECK_CMDWIN;
		ಆದರೆ (ONE_ಕಿಟಕಿ && Prenum != 1)	// just one window
		    beep_flush();
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (Prenum)			// go to specified window
		    {
			ಗಾಗಿ (wp = firstwin; --Prenum > 0; )
			{
			    ಆದರೆ (wp->w_ಮುಂದಿನ == ಖಾಲಿ)
				ನಿಲ್ಲಿಸು;
			    ಇಲ್ಲದಿದ್ದರೆ
				wp = wp->w_ಮುಂದಿನ;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    ಆದರೆ (wp == ಖಾಲಿ)
				wp = lastwin;	    // wrap around
			}
			ಇಲ್ಲದಿದ್ದರೆ			    // go to next window
			{
			    wp = curwin->w_ಮುಂದಿನ;
			    ಆದರೆ (wp == ಖಾಲಿ)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		ನಿಲ್ಲಿಸು;

// cursor to window below
    ಸ್ಥಿತಿ 'j':
    ಸ್ಥಿತಿ K_ಕೆಳಕ್ಕೆ:
    ಸ್ಥಿತಿ Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		ನಿಲ್ಲಿಸು;

// cursor to window above
    ಸ್ಥಿತಿ 'k':
    ಸ್ಥಿತಿ K_ಮೇಲೆ:
    ಸ್ಥಿತಿ Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		ನಿಲ್ಲಿಸು;

// cursor to left window
    ಸ್ಥಿತಿ 'h':
    ಸ್ಥಿತಿ K_ಎಡ:
    ಸ್ಥಿತಿ Ctrl_H:
    ಸ್ಥಿತಿ K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		ನಿಲ್ಲಿಸು;

// cursor to right window
    ಸ್ಥಿತಿ 'l':
    ಸ್ಥಿತಿ K_ಬಲ:
    ಸ್ಥಿತಿ Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		ನಿಲ್ಲಿಸು;

// move window to new tab page
    ಸ್ಥಿತಿ 'T':
		CHECK_CMDWIN;
		ಆದರೆ (one_ಕಿಟಕಿ())
		    ಸಂದೇಶ(_(m_onlyone));
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    ಆದರೆ (win_new_tabpage((ಪೂರ್ಣಾಂಕ)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			ಆದರೆ (curwin == wp)
			    win_close(curwin, FALSE);
			ಆದರೆ (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		ನಿಲ್ಲಿಸು;

// cursor to top-left window
    ಸ್ಥಿತಿ 't':
    ಸ್ಥಿತಿ Ctrl_T:
		win_goto(firstwin);
		ನಿಲ್ಲಿಸು;

// cursor to bottom-right window
    ಸ್ಥಿತಿ 'b':
    ಸ್ಥಿತಿ Ctrl_B:
		win_goto(lastwin);
		ನಿಲ್ಲಿಸು;

// cursor to last accessed (previous) window
    ಸ್ಥಿತಿ 'p':
    ಸ್ಥಿತಿ Ctrl_P:
		ಆದರೆ (!win_valid(prevwin))
		    beep_flush();
		ಇಲ್ಲದಿದ್ದರೆ
		    win_goto(prevwin);
		ನಿಲ್ಲಿಸು;

// exchange current and next window
    ಸ್ಥಿತಿ 'x':
    ಸ್ಥಿತಿ Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		ನಿಲ್ಲಿಸು;

// rotate windows downwards
    ಸ್ಥಿತಿ Ctrl_R:
    ಸ್ಥಿತಿ 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (ಪೂರ್ಣಾಂಕ)Prenum1);    // downwards
		ನಿಲ್ಲಿಸು;

// rotate windows upwards
    ಸ್ಥಿತಿ 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (ಪೂರ್ಣಾಂಕ)Prenum1);	    // upwards
		ನಿಲ್ಲಿಸು;

// move window to the very top/bottom/left/right
    ಸ್ಥಿತಿ 'K':
    ಸ್ಥಿತಿ 'J':
    ಸ್ಥಿತಿ 'H':
    ಸ್ಥಿತಿ 'L':
		CHECK_CMDWIN;
		ಆದರೆ (ONE_ಕಿಟಕಿ)
		    beep_flush();
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪೂರ್ಣಾಂಕ dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_ತುದಿ : WSP_BOT);

		    (ಶೂನ್ಯ)win_splitmove(curwin, (ಪೂರ್ಣಾಂಕ)Prenum, dir);
		}
		ನಿಲ್ಲಿಸು;

// make all windows the same width and/or height
    ಸ್ಥಿತಿ '=':
		{
		    ಪೂರ್ಣಾಂಕ mod = cmdmod.cmod_ವಿಭಜಿಸು & (WSP_VERT | WSP_HOR);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		    win_equal(ಖಾಲಿ, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		ನಿಲ್ಲಿಸು;

// increase current window height
    ಸ್ಥಿತಿ '+':
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		win_setheight(curwin->w_ಎತ್ತರ + (ಪೂರ್ಣಾಂಕ)Prenum1);
		ನಿಲ್ಲಿಸು;

// decrease current window height
    ಸ್ಥಿತಿ '-':
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		win_setheight(curwin->w_ಎತ್ತರ - (ಪೂರ್ಣಾಂಕ)Prenum1);
		ನಿಲ್ಲಿಸು;

// set current window height
    ಸ್ಥಿತಿ Ctrl__:
    ಸ್ಥಿತಿ '_':
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		win_setheight(Prenum ? (ಪೂರ್ಣಾಂಕ)Prenum : 9999);
		ನಿಲ್ಲಿಸು;

// increase current window width
    ಸ್ಥಿತಿ '>':
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		win_setwidth(curwin->w_ಅಗಲ + (ಪೂರ್ಣಾಂಕ)Prenum1);
		ನಿಲ್ಲಿಸು;

// decrease current window width
    ಸ್ಥಿತಿ '<':
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		win_setwidth(curwin->w_ಅಗಲ - (ಪೂರ್ಣಾಂಕ)Prenum1);
		ನಿಲ್ಲಿಸು;

// set current window width
    ಸ್ಥಿತಿ '|':
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		win_setwidth(Prenum != 0 ? (ಪೂರ್ಣಾಂಕ)Prenum : 9999);
		ನಿಲ್ಲಿಸು;

// jump to tag and split window if tag exists (in preview window)
#ಆದರೆ defined(FEAT_QUICKFIX)
    ಸ್ಥಿತಿ '}':
		CHECK_CMDWIN;
		ಆದರೆ (Prenum)
		    g_do_tagpreview = Prenum;
		ಇಲ್ಲದಿದ್ದರೆ
		    g_do_tagpreview = p_pvh;
#ಆದರೆ_ಅಂತ್ಯ
		// FALLTHROUGH
    ಸ್ಥಿತಿ ']':
    ಸ್ಥಿತಿ Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		ಆದರೆ (Prenum)
		    postponed_ವಿಭಜಿಸು = Prenum;
		ಇಲ್ಲದಿದ್ದರೆ
		    postponed_ವಿಭಜಿಸು = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		ಆದರೆ (nchar != '}')
		    g_do_tagpreview = 0;
#ಆದರೆ_ಅಂತ್ಯ

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_ವಿಭಜಿಸು = 0;
		ನಿಲ್ಲಿಸು;

// edit file name under cursor in a new window
    ಸ್ಥಿತಿ 'f':
    ಸ್ಥಿತಿ 'F':
    ಸ್ಥಿತಿ Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		ಆದರೆ (check_ಪಠ್ಯ_or_curbuf_locked(ಖಾಲಿ))
		    ನಿಲ್ಲಿಸು;

		ptr = grab_file_name(Prenum1, &lnum);
		ಆದರೆ (ptr != ಖಾಲಿ)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = ಖಾಲಿ;
		    ಆದರೆ ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    ಆದರೆ (wp == ಖಾಲಿ && win_ವಿಭಜಿಸು(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			ಆದರೆ (do_ecmd(0, ptr, ಖಾಲಿ, ಖಾಲಿ, ECMD_LASTL,
						   ECMD_HIDE, ಖಾಲಿ) == FAIL)
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
			ಇಲ್ಲದಿದ್ದರೆ
			    wp = curwin;
		    }

		    ಆದರೆ (wp != ಖಾಲಿ && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    ಸ್ಥಿತಿ 'i':			    // Go to any match
    ಸ್ಥಿತಿ Ctrl_I:
		type = ಕಂಡುಹಿಡಿ_ANY;
		// FALLTHROUGH
    ಸ್ಥಿತಿ 'd':			    // Go to definition, using 'define'
    ಸ್ಥಿತಿ Ctrl_D:
		CHECK_CMDWIN;
		ಆದರೆ ((len = ಕಂಡುಹಿಡಿ_ident_under_cursor(&ptr, ಕಂಡುಹಿಡಿ_IDENT)) == 0)
		    ನಿಲ್ಲಿಸು;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		ಆದರೆ (ptr == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;

		ಕಂಡುಹಿಡಿ_ನಮೂನೆ_ಒಳ_ಮಾರ್ಗ(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_ವಿಭಜಿಸು, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_ಗಣ_curswant = true;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

// Quickfix window only: view the result under the cursor in a new split.
#ಆದರೆ defined(FEAT_QUICKFIX)
    ಸ್ಥಿತಿ K_KENTER:
    ಸ್ಥಿತಿ CAR:
		ಆದರೆ (bt_quickfix(curbuf))
		    qf_view_ಫಲಿತಾಂಶ(TRUE);
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

// CTRL-W g  extended commands
    ಸ್ಥಿತಿ 'g':
    ಸ್ಥಿತಿ Ctrl_G:
		CHECK_CMDWIN;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#ಆದರೆ_ಅಂತ್ಯ
		++no_mapping;
		++allow_ಕೀಲಿಗಳು;   // no mapping for xchar, but allow key codes
		ಆದರೆ (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_ಕೀಲಿಗಳು;
		(ಶೂನ್ಯ)ಕೂಡಿಸು_ಗಮ್ಯ_showcmd(xchar);

		ಆಯ್ಕೆ (xchar)
		{
#ಆದರೆ defined(FEAT_QUICKFIX)
		    ಸ್ಥಿತಿ '}':
			xchar = Ctrl_RSB;
			ಆದರೆ (Prenum)
			    g_do_tagpreview = Prenum;
			ಇಲ್ಲದಿದ್ದರೆ
			    g_do_tagpreview = p_pvh;
#ಆದರೆ_ಅಂತ್ಯ
			// FALLTHROUGH
		    ಸ್ಥಿತಿ ']':
		    ಸ್ಥಿತಿ Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			ಆದರೆ (Prenum)
			    postponed_ವಿಭಜಿಸು = Prenum;
			ಇಲ್ಲದಿದ್ದರೆ
			    postponed_ವಿಭಜಿಸು = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_ವಿಭಜಿಸು = 0;
			ನಿಲ್ಲಿಸು;

		    ಸ್ಥಿತಿ 'f':	    // CTRL-W gf: "gf" in a new tab page
		    ಸ್ಥಿತಿ 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_ಸೂಚ್ಯಂಕ(curtab) + 1;
			nchar = xchar;
			ಹೋಗು wingotofile;

		    ಸ್ಥಿತಿ 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((ಪೂರ್ಣಾಂಕ)Prenum);
			ನಿಲ್ಲಿಸು;

		    ಸ್ಥಿತಿ 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(ಪೂರ್ಣಾಂಕ)Prenum1);
			ನಿಲ್ಲಿಸು;

		    ಸ್ಥಿತಿ TAB:	    // CTRL-W g<Tab>: go to last used tab page
			ಆದರೆ (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			ನಿಲ್ಲಿಸು;

		    ಪೂರ್ವನಿಯೋಜಿತ:
			beep_flush();
			ನಿಲ್ಲಿಸು;
		}
		ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:	beep_flush();
		ನಿಲ್ಲಿಸು;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    ಶೂನ್ಯ
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    ಆಯ್ಕೆ (*arg)
    {
    ಸ್ಥಿತಿ 'S':
    ಸ್ಥಿತಿ Ctrl_S:
    ಸ್ಥಿತಿ 's':
    ಸ್ಥಿತಿ Ctrl_N:
    ಸ್ಥಿತಿ 'n':
    ಸ್ಥಿತಿ 'j':
    ಸ್ಥಿತಿ Ctrl_J:
    ಸ್ಥಿತಿ 'k':
    ಸ್ಥಿತಿ Ctrl_K:
    ಸ್ಥಿತಿ 'T':
    ಸ್ಥಿತಿ Ctrl_R:
    ಸ್ಥಿತಿ 'r':
    ಸ್ಥಿತಿ 'R':
    ಸ್ಥಿತಿ 'K':
    ಸ್ಥಿತಿ 'J':
    ಸ್ಥಿತಿ '+':
    ಸ್ಥಿತಿ '-':
    ಸ್ಥಿತಿ Ctrl__:
    ಸ್ಥಿತಿ '_':
    ಸ್ಥಿತಿ '|':
    ಸ್ಥಿತಿ ']':
    ಸ್ಥಿತಿ Ctrl_RSB:
    ಸ್ಥಿತಿ 'g':
    ಸ್ಥಿತಿ Ctrl_G:
    ಸ್ಥಿತಿ Ctrl_V:
    ಸ್ಥಿತಿ 'v':
    ಸ್ಥಿತಿ 'h':
    ಸ್ಥಿತಿ Ctrl_H:
    ಸ್ಥಿತಿ 'l':
    ಸ್ಥಿತಿ Ctrl_L:
    ಸ್ಥಿತಿ 'H':
    ಸ್ಥಿತಿ 'L':
    ಸ್ಥಿತಿ '>':
    ಸ್ಥಿತಿ '<':
#ಆದರೆ defined(FEAT_QUICKFIX)
    ಸ್ಥಿತಿ '}':
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ 'f':
    ಸ್ಥಿತಿ 'F':
    ಸ್ಥಿತಿ Ctrl_F:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
    ಸ್ಥಿತಿ 'i':
    ಸ್ಥಿತಿ Ctrl_I:
    ಸ್ಥಿತಿ 'd':
    ಸ್ಥಿತಿ Ctrl_D:
#ಆದರೆ_ಅಂತ್ಯ
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ Ctrl_HAT:
    ಸ್ಥಿತಿ '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ Ctrl_Q:
    ಸ್ಥಿತಿ 'q':
    ಸ್ಥಿತಿ Ctrl_C:
    ಸ್ಥಿತಿ 'c':
    ಸ್ಥಿತಿ Ctrl_O:
    ಸ್ಥಿತಿ 'o':
    ಸ್ಥಿತಿ Ctrl_W:
    ಸ್ಥಿತಿ 'w':
    ಸ್ಥಿತಿ 'W':
    ಸ್ಥಿತಿ 'x':
    ಸ್ಥಿತಿ Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		ನಿಲ್ಲಿಸು;

#ಆದರೆ defined(FEAT_QUICKFIX)
    ಸ್ಥಿತಿ Ctrl_Z:
    ಸ್ಥಿತಿ 'z':
    ಸ್ಥಿತಿ 'P':
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ 't':
    ಸ್ಥಿತಿ Ctrl_T:
    ಸ್ಥಿತಿ 'b':
    ಸ್ಥಿತಿ Ctrl_B:
    ಸ್ಥಿತಿ 'p':
    ಸ್ಥಿತಿ Ctrl_P:
    ಸ್ಥಿತಿ '=':
    ಸ್ಥಿತಿ CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		ನಿಲ್ಲಿಸು;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cmd_with_ಎಣಿಕೆ(
    ಅಕ್ಷರ	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    ದೀರ್ಘ	Prenum)
{
    ಆದರೆ (Prenum > 0)
	vim_snprintf((ಅಕ್ಷರ *)bufp, bufsize, "%s %ld", cmd, Prenum);
    ಇಲ್ಲದಿದ್ದರೆ
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    ಪೂರ್ಣಾಂಕ
check_ವಿಭಜಿಸು_disallowed(win_T *wp)
{
    ಆದರೆ (ವಿಭಜಿಸು_disallowed > 0)
    {
	emsg(_(e_cant_ವಿಭಜಿಸು_ಕಿಟಕಿ_while_closing_another));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (wp->w_ಬಫರ್->b_locked_ವಿಭಜಿಸು)
    {
	emsg(_(e_cannot_ವಿಭಜಿಸು_ಕಿಟಕಿ_when_closing_ಬಫರ್));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ
win_ವಿಭಜಿಸು(ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2, ಪೂರ್ಣಾಂಕ flags)
{
    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (check_ವಿಭಜಿಸು_disallowed(curwin) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    ಆದರೆ (may_open_tabpage() == OK)
	ಹಿಂತಿರುಗಿ OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_ವಿಭಜಿಸು;
    ಆದರೆ ((flags & WSP_ತುದಿ) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_ವಿಭಜಿಸು_topleft_and_botright_at_the_same_ಸಮಯ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    ಆದರೆ (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    ಇಲ್ಲದಿದ್ದರೆ
	clear_snapshot(curtab, SNAP_HELP_IDX);

    ಆದರೆ (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    ಇಲ್ಲದಿದ್ದರೆ
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    ಹಿಂತಿರುಗಿ win_ವಿಭಜಿಸು_ins(ಗಾತ್ರ_2, flags, ಖಾಲಿ, 0, ಖಾಲಿ);
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
    ಪೂರ್ಣಾಂಕ
win_ವಿಭಜಿಸು_ins(
    ಪೂರ್ಣಾಂಕ		ಗಾತ್ರ_2,
    ಪೂರ್ಣಾಂಕ		flags,
    win_T	*new_wp,
    ಪೂರ್ಣಾಂಕ		dir,
    ಚೌಕಟ್ಟು_T	*ಗಮ್ಯ_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    ಪೂರ್ಣಾಂಕ		new_ಗಾತ್ರ = ಗಾತ್ರ_2;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		need_ಸ್ಥಿತಿ = 0;
    ಪೂರ್ಣಾಂಕ		do_equal = FALSE;
    ಪೂರ್ಣಾಂಕ		needed;
    ಪೂರ್ಣಾಂಕ		ಲಭ್ಯ;
    ಪೂರ್ಣಾಂಕ		oldwin_ಎತ್ತರ = 0;
    ಪೂರ್ಣಾಂಕ		layout;
    ಚೌಕಟ್ಟು_T	*frp, *curfrp, *frp2, *prevfrp;
    ಪೂರ್ಣಾಂಕ		before;
    ಪೂರ್ಣಾಂಕ		minheight;
    ಪೂರ್ಣಾಂಕ		wmh1;
    ಪೂರ್ಣಾಂಕ		did_ಗಣ_fraction = FALSE;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    ಆದರೆ (new_wp == ಖಾಲಿ)
	trigger_winnewpre();

    ಆದರೆ (flags & WSP_ತುದಿ)
	oldwin = firstwin;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & WSP_BOT)
	oldwin = lastwin;
    ಇಲ್ಲದಿದ್ದರೆ
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    ಆದರೆ (ONE_ಕಿಟಕಿ && p_ls == 1 && oldwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0)
    {
	ಆದರೆ (!(flags & WSP_FORCE_ಸ್ಥಳ) && VISIBLE_ಎತ್ತರ(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_ಸ್ಥಳ));
	    ಹೋಗು theend;
	}
	need_ಸ್ಥಿತಿ = statusline_ಎತ್ತರ(oldwin);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    ಆದರೆ (gui.ಒಳ_use)
	ಹೊರಹೋಗು_flush();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (flags & WSP_VERT)
    {
	ಪೂರ್ಣಾಂಕ	wmw1;
	ಪೂರ್ಣಾಂಕ	minwidth;

	layout = FR_ಅಡ್ಡಸಾಲು;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	ಆದರೆ (flags & WSP_ಸ್ಥಳ)
	    needed += p_wiw - wmw1;
	ಆದರೆ (flags & (WSP_BOT | WSP_ತುದಿ))
	{
	    minwidth = ಚೌಕಟ್ಟು_minwidth(topframe, NOWIN);
	    ಲಭ್ಯ = topframe->fr_ಅಗಲ;
	    needed += minwidth;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_ea)
	{
	    minwidth = ಚೌಕಟ್ಟು_minwidth(oldwin->w_ಚೌಕಟ್ಟು, NOWIN);
	    prevfrp = oldwin->w_ಚೌಕಟ್ಟು;
	    ಗಾಗಿ (frp = oldwin->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ; frp != ಖಾಲಿ;
							frp = frp->fr_ಪೋಷಕ)
	    {
		ಆದರೆ (frp->fr_layout == FR_ಅಡ್ಡಸಾಲು)
		    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp2, frp->fr_ಮಗು)
			ಆದರೆ (frp2 != prevfrp)
			    minwidth += ಚೌಕಟ್ಟು_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    ಲಭ್ಯ = topframe->fr_ಅಗಲ;
	    needed += minwidth;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    minwidth = ಚೌಕಟ್ಟು_minwidth(oldwin->w_ಚೌಕಟ್ಟು, NOWIN);
	    ಲಭ್ಯ = oldwin->w_ಚೌಕಟ್ಟು->fr_ಅಗಲ;
	    needed += minwidth;
	}
	ಆದರೆ (!(flags & WSP_FORCE_ಸ್ಥಳ) && ಲಭ್ಯ < needed)
	{
	    emsg(_(e_not_enough_ಸ್ಥಳ));
	    ಹೋಗು theend;
	}
	ಆದರೆ (new_ಗಾತ್ರ == 0)
	    new_ಗಾತ್ರ = oldwin->w_ಅಗಲ / 2;
	ಆದರೆ (new_ಗಾತ್ರ > ಲಭ್ಯ - minwidth - 1)
	    new_ಗಾತ್ರ = ಲಭ್ಯ - minwidth - 1;
	ಆದರೆ (new_ಗಾತ್ರ < wmw1)
	    new_ಗಾತ್ರ = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	ಆದರೆ (oldwin->w_ಅಗಲ - new_ಗಾತ್ರ - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	ಆದರೆ (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_ಅಗಲ + new_ಗಾತ್ರ + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	ಆದರೆ (!do_equal && p_ea && ಗಾತ್ರ_2 == 0 && *p_ead != 'v'
	    && oldwin->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ != ಖಾಲಿ)
	    do_equal = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	layout = FR_ಲಂಬಸಾಲು;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_ಎತ್ತರ(curwin);
	needed = wmh1 + statusline_ಎತ್ತರ(oldwin);
	ಆದರೆ (flags & WSP_ಸ್ಥಳ)
	    needed += p_wh - wmh1;
	ಆದರೆ (flags & (WSP_BOT | WSP_ತುದಿ))
	{
	    minheight = ಚೌಕಟ್ಟು_minheight(topframe, NOWIN) + need_ಸ್ಥಿತಿ;
	    ಲಭ್ಯ = topframe->fr_ಎತ್ತರ;
	    needed += minheight;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_ea)
	{
	    minheight = ಚೌಕಟ್ಟು_minheight(oldwin->w_ಚೌಕಟ್ಟು, NOWIN) + need_ಸ್ಥಿತಿ;
	    prevfrp = oldwin->w_ಚೌಕಟ್ಟು;
	    ಗಾಗಿ (frp = oldwin->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ; frp != ಖಾಲಿ;
							frp = frp->fr_ಪೋಷಕ)
	    {
		ಆದರೆ (frp->fr_layout == FR_ಲಂಬಸಾಲು)
		    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp2, frp->fr_ಮಗು)
			ಆದರೆ (frp2 != prevfrp)
			    minheight += ಚೌಕಟ್ಟು_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    ಲಭ್ಯ = topframe->fr_ಎತ್ತರ;
	    needed += minheight;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    minheight = ಚೌಕಟ್ಟು_minheight(oldwin->w_ಚೌಕಟ್ಟು, NOWIN) + need_ಸ್ಥಿತಿ;
	    ಲಭ್ಯ = oldwin->w_ಚೌಕಟ್ಟು->fr_ಎತ್ತರ;
	    needed += minheight;
	}
	ಆದರೆ (!(flags & WSP_FORCE_ಸ್ಥಳ) && ಲಭ್ಯ < needed)
	{
	    emsg(_(e_not_enough_ಸ್ಥಳ));
	    ಹೋಗು theend;
	}
	oldwin_ಎತ್ತರ = oldwin->w_ಎತ್ತರ;
	ಆದರೆ (need_ಸ್ಥಿತಿ)
	{
	    oldwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ = statusline_ಎತ್ತರ(oldwin);
	    oldwin_ಎತ್ತರ -= oldwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	}
	// Estimated stl height for the new window.
	ಪೂರ್ಣಾಂಕ new_win_stlh = need_ಸ್ಥಿತಿ != 0 ? need_ಸ್ಥಿತಿ
			: statusline_ಎತ್ತರ(new_wp != ಖಾಲಿ ? new_wp : curwin);
	ಆದರೆ (new_ಗಾತ್ರ == 0)
	    new_ಗಾತ್ರ = (oldwin_ಎತ್ತರ - new_win_stlh + 1) / 2;
	ಆದರೆ (new_ಗಾತ್ರ > ಲಭ್ಯ - minheight - new_win_stlh)
	    new_ಗಾತ್ರ = ಲಭ್ಯ - minheight - new_win_stlh;
	ಆದರೆ (new_ಗಾತ್ರ < wmh1)
	    new_ಗಾತ್ರ = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	ಆದರೆ (oldwin_ಎತ್ತರ - new_ಗಾತ್ರ - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	ಆದರೆ (oldwin->w_p_wfh)
	{
	    // Set w_fraction now so that the cursor keeps the same relative
	    // vertical position using the old height.
	    ಗಣ_fraction(oldwin);
	    did_ಗಣ_fraction = TRUE;

	    win_setheight_win(oldwin->w_ಎತ್ತರ + new_ಗಾತ್ರ
		    + statusline_ಎತ್ತರ(oldwin), oldwin);
	    // w_height now excludes the status line
	    oldwin_ಎತ್ತರ = oldwin->w_ಎತ್ತರ;
	}

	// Make all windows the same height if 'equalalways' is set, 'ead' is
	// not set to 'h', a size was not provided and there is a parent frame.
	ಆದರೆ (!do_equal && p_ea && ಗಾತ್ರ_2 == 0 && *p_ead != 'h'
	    && oldwin->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ != ಖಾಲಿ)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    ಆದರೆ ((flags & WSP_ತುದಿ) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_ಕೆಳಗೆ)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	ಆದರೆ (new_wp == ಖಾಲಿ)
	    wp = win_alloc(oldwin, FALSE);
	ಇಲ್ಲದಿದ್ದರೆ
	    win_append(oldwin, wp);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (new_wp == ಖಾಲಿ)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	ಇಲ್ಲದಿದ್ದರೆ
	    win_append(oldwin->w_prev, wp);
    }

    ಆದರೆ (new_wp == ಖಾಲಿ)
    {
	ಆದರೆ (wp == ಖಾಲಿ)
	    ಹೋಗು theend;

	new_ಚೌಕಟ್ಟು(wp);
	ಆದರೆ (wp->w_ಚೌಕಟ್ಟು == ಖಾಲಿ)
	{
	    win_free(wp, ಖಾಲಿ);
	    ಹೋಗು theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    ಆದರೆ (ಗಮ್ಯ_flatten != ಖಾಲಿ)
	ಚೌಕಟ್ಟು_flatten(ಗಮ್ಯ_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    ಆದರೆ (flags & (WSP_ತುದಿ | WSP_BOT))
    {
	ಆದರೆ ((topframe->fr_layout == FR_ಲಂಬಸಾಲು && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ಅಡ್ಡಸಾಲು && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_ಮಗು;
	    ಆದರೆ (flags & WSP_BOT)
		ಆಗ (curfrp->fr_ಮುಂದಿನ != ಖಾಲಿ)
		    curfrp = curfrp->fr_ಮುಂದಿನ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    curfrp = topframe;
	before = (flags & WSP_ತುದಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	curfrp = oldwin->w_ಚೌಕಟ್ಟು;
	ಆದರೆ (flags & WSP_ಕೆಳಗೆ)
	    before = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & WSP_ABOVE)
	    before = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & WSP_VERT)
	    before = !p_spr;
	ಇಲ್ಲದಿದ್ದರೆ
	    before = !p_sb;
    }
    ಆದರೆ (curfrp->fr_ಪೋಷಕ == ಖಾಲಿ || curfrp->fr_ಪೋಷಕ->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(ಚೌಕಟ್ಟು_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_ಪೋಷಕ = curfrp;
	frp->fr_ಮುಂದಿನ = ಖಾಲಿ;
	frp->fr_prev = ಖಾಲಿ;
	curfrp->fr_ಮಗು = frp;
	curfrp->fr_win = ಖಾಲಿ;
	curfrp = frp;
	ಆದರೆ (frp->fr_win != ಖಾಲಿ)
	    oldwin->w_ಚೌಕಟ್ಟು = frp;
	ಇಲ್ಲದಿದ್ದರೆ
	    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, frp->fr_ಮಗು)
		frp->fr_ಪೋಷಕ = curfrp;
    }

    ಆದರೆ (new_wp == ಖಾಲಿ)
	frp = wp->w_ಚೌಕಟ್ಟು;
    ಇಲ್ಲದಿದ್ದರೆ
	frp = new_wp->w_ಚೌಕಟ್ಟು;
    frp->fr_ಪೋಷಕ = curfrp->fr_ಪೋಷಕ;

    // Insert the new frame at the right place in the frame list.
    ಆದರೆ (before)
	ಚೌಕಟ್ಟು_ಸೇರಿಸು(curfrp, frp);
    ಇಲ್ಲದಿದ್ದರೆ
	ಚೌಕಟ್ಟು_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    ಆದರೆ (!did_ಗಣ_fraction)
	ಗಣ_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    ಆದರೆ (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	ಆದರೆ (need_ಸ್ಥಿತಿ)
	{
	    win_new_ಎತ್ತರ(oldwin, oldwin->w_ಎತ್ತರ - need_ಸ್ಥಿತಿ);
	    oldwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ = need_ಸ್ಥಿತಿ;
	}
	ಆದರೆ (flags & (WSP_ತುದಿ | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    ಪೂರ್ಣಾಂಕ stl_ಎತ್ತರ = p_ls > 0 ? statusline_ಎತ್ತರ(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_ಎತ್ತರ();
	    win_new_ಎತ್ತರ(wp, curfrp->fr_ಎತ್ತರ - stl_ಎತ್ತರ
							  - WINBAR_ಎತ್ತರ(wp));
	    wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = stl_ಎತ್ತರ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // height and row of new window is same as current window
	    wp->w_winrow = oldwin->w_winrow;
	    win_new_ಎತ್ತರ(wp, VISIBLE_ಎತ್ತರ(oldwin));
	    wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = oldwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	}
	frp->fr_ಎತ್ತರ = curfrp->fr_ಎತ್ತರ;

	// "new_size" of the current window goes to the new window, use
	// one column for the vertical separator
	win_new_ಅಗಲ(wp, new_ಗಾತ್ರ);
	ಆದರೆ (before)
	    wp->w_vsep_ಅಗಲ = 1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wp->w_vsep_ಅಗಲ = oldwin->w_vsep_ಅಗಲ;
	    oldwin->w_vsep_ಅಗಲ = 1;
	}
	ಆದರೆ (flags & (WSP_ತುದಿ | WSP_BOT))
	{
	    ಆದರೆ (flags & WSP_BOT)
		ಚೌಕಟ್ಟು_ಗಣ_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    ಚೌಕಟ್ಟು_new_ಅಗಲ(curfrp, curfrp->fr_ಅಗಲ
		     - (new_ಗಾತ್ರ + ((flags & WSP_ತುದಿ) != 0)), flags & WSP_ತುದಿ,
								       FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    win_new_ಅಗಲ(oldwin, oldwin->w_ಅಗಲ - (new_ಗಾತ್ರ + 1));
	ಆದರೆ (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_ಗಾತ್ರ + 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_ಅಗಲ + 1;
	ಚೌಕಟ್ಟು_fix_ಅಗಲ(oldwin);
	ಚೌಕಟ್ಟು_fix_ಅಗಲ(wp);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// width and column of new window is same as current window
	ಆದರೆ (flags & (WSP_ತುದಿ | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_ಅಗಲ(wp, topframe->fr_ಅಗಲ);
	    wp->w_vsep_ಅಗಲ = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_ಅಗಲ(wp, oldwin->w_ಅಗಲ);
	    wp->w_vsep_ಅಗಲ = oldwin->w_vsep_ಅಗಲ;
	}
	frp->fr_ಅಗಲ = curfrp->fr_ಅಗಲ;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_ಎತ್ತರ(wp, new_ಗಾತ್ರ);
	ಪೂರ್ಣಾಂಕ old_ಸ್ಥಿತಿ_ಎತ್ತರ = oldwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	ಆದರೆ (flags & (WSP_ತುದಿ | WSP_BOT))
	{
	    ಪೂರ್ಣಾಂಕ new_fr_ಎತ್ತರ = curfrp->fr_ಎತ್ತರ - new_ಗಾತ್ರ
							  + WINBAR_ಎತ್ತರ(wp);

	    ಆದರೆ (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_ಎತ್ತರ -= statusline_ಎತ್ತರ(curfrp->fr_win);
	    ಆದರೆ (flags & WSP_BOT)
		ಚೌಕಟ್ಟು_ಕೂಡಿಸು_statusline(curfrp);
	    ಚೌಕಟ್ಟು_new_ಎತ್ತರ(curfrp, new_fr_ಎತ್ತರ, flags & WSP_ತುದಿ, FALSE,
									FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    win_new_ಎತ್ತರ(oldwin, oldwin_ಎತ್ತರ - (new_ಗಾತ್ರ
			+ statusline_ಎತ್ತರ(wp)));
	ಆದರೆ (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = statusline_ಎತ್ತರ(wp);
	    oldwin->w_winrow += wp->w_ಎತ್ತರ + statusline_ಎತ್ತರ(wp);
	}
	ಇಲ್ಲದಿದ್ದರೆ		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_ಎತ್ತರ(oldwin)
						    + statusline_ಎತ್ತರ(oldwin);
	    wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = old_ಸ್ಥಿತಿ_ಎತ್ತರ;
	    ಆದರೆ (!(flags & WSP_BOT))
		oldwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ = statusline_ಎತ್ತರ(oldwin);
	}
	ಚೌಕಟ್ಟು_fix_ಎತ್ತರ(wp);
	ಚೌಕಟ್ಟು_fix_ಎತ್ತರ(oldwin);
    }

    ಆದರೆ (flags & (WSP_ತುದಿ | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    ಸ್ಥಿತಿ_redraw_all();

    ಆದರೆ (need_ಸ್ಥಿತಿ)
    {
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
	ಸಂದೇಶ_ಲಂಬಸಾಲು = sc_ಲಂಬಸಾಲು;
	ಸಂದೇಶ_clr_eos_force();	// Old command/ruler may still be there
	comp_ಲಂಬಸಾಲು();
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
	ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    ಆದರೆ (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    ಆದರೆ (flags & WSP_VERT)
    {
	i = p_wiw;
	ಆದರೆ (ಗಾತ್ರ_2 != 0)
	    p_wiw = ಗಾತ್ರ_2;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	ಆದರೆ (gui.ಒಳ_use)
	    gui_init_which_components(ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	i = p_wh;
	ಆದರೆ (ಗಾತ್ರ_2 != 0)
	    p_wh = ಗಾತ್ರ_2;
    }

    /*
     * make the new window the current window
     */
    (ಶೂನ್ಯ)win_enter_ext(wp, (new_wp == ಖಾಲಿ ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    ಆದರೆ (flags & WSP_VERT)
	p_wiw = i;
    ಇಲ್ಲದಿದ್ದರೆ
	p_wh = i;
    retval = OK;

theend:
    ಆದರೆ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ಹಿಂತಿರುಗಿ retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_init(win_T *newp, win_T *oldp, ಪೂರ್ಣಾಂಕ flags ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		i;

    newp->w_ಬಫರ್ = oldp->w_ಬಫರ್;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    newp->w_s = &(oldp->w_ಬಫರ್->b_s);
#ಆದರೆ_ಅಂತ್ಯ
    oldp->w_ಬಫರ್->b_nwindows++;
    newp->w_cursor = oldp->w_cursor;
    newp->w_valid = 0;
    newp->w_curswant = oldp->w_curswant;
    newp->w_ಗಣ_curswant = oldp->w_ಗಣ_curswant;
    newp->w_topline = oldp->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    newp->w_topfill = oldp->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ
    newp->w_leftcol = oldp->w_leftcol;
    newp->w_pcmark = oldp->w_pcmark;
    newp->w_prev_pcmark = oldp->w_prev_pcmark;
    newp->w_alt_fnum = oldp->w_alt_fnum;
    newp->w_wrow = oldp->w_wrow;
    newp->w_fraction = oldp->w_fraction;
    newp->w_prev_fraction_ಅಡ್ಡಸಾಲು = oldp->w_prev_fraction_ಅಡ್ಡಸಾಲು;

    // Not sure if this is needed, but be safe
    remove_highlight_overrides(newp->w_hl);
    VIM_CLEAR(newp->w_hl);

    copy_jumplist(oldp, newp);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    ಆದರೆ (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = ಖಾಲಿ;
	newp->w_llist_ref = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	copy_loclist_ರಾಶಿ(oldp, newp);
#ಆದರೆ_ಅಂತ್ಯ
    newp->w_localdir = (oldp->w_localdir == ಖಾಲಿ)
				    ? ಖಾಲಿ : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == ಖಾಲಿ)
				    ? ಖಾಲಿ : vim_strsave(oldp->w_prevdir);

    ಆದರೆ (*p_spk != 'c')
    {
	ಆದರೆ (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_ಎತ್ತರ = oldp->w_ಎತ್ತರ - WINBAR_ಎತ್ತರ(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_ಎತ್ತರ(oldp);
    }

    // copy tagstack and folds
    ಗಾಗಿ (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*ಟ್ಯಾಗ್ = &newp->w_tagstack[i];
	*ಟ್ಯಾಗ್ = oldp->w_tagstack[i];
	ಆದರೆ (ಟ್ಯಾಗ್->tagname != ಖಾಲಿ)
	    ಟ್ಯಾಗ್->tagname = vim_strsave(ಟ್ಯಾಗ್->tagname);
	ಆದರೆ (ಟ್ಯಾಗ್->user_ದತ್ತಾಂಶ != ಖಾಲಿ)
	    ಟ್ಯಾಗ್->user_ದತ್ತಾಂಶ = vim_strsave(ಟ್ಯಾಗ್->user_ದತ್ತಾಂಶ);
    }
    newp->w_tagstackidx = oldp->w_tagstackidx;
    newp->w_tagstacklen = oldp->w_tagstacklen;

    // Keep same changelist position in new window.
    newp->w_changelistidx = oldp->w_changelistidx;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    copyFoldingState(oldp, newp);
#ಆದರೆ_ಅಂತ್ಯ

    win_init_some(newp, oldp);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    {
	// Make sure to also handle highlight overrides copied over from oldp.
	bool pushed = ತಳ್ಳು_highlight_overrides(newp->w_hl, newp->w_hl_len);
	ಆದರೆ (newp->w_ಬಫರ್->b_ಪದ != ಖಾಲಿ)
	    ಪದ_init_default_colors(newp->w_ಬಫರ್->b_ಪದ);
	ಆದರೆ (pushed)
	    ತೆಗೆ_highlight_overrides();
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಪೂರ್ಣಾಂಕ
win_valid_popup(win_T *win ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	ಆದರೆ (wp == win)
	    ಹಿಂತಿರುಗಿ TRUE;
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	ಆದರೆ (wp == win)
	    ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    ಪೂರ್ಣಾಂಕ
win_valid(win_T *win)
{
    win_T	*wp;

    ಆದರೆ (win == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp == win)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_ಕಂಡುಹಿಡಿ_by_id(ಪೂರ್ಣಾಂಕ id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_id == id)
	    ಹಿಂತಿರುಗಿ wp;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	ಆದರೆ (wp->w_id == id)
	    ಹಿಂತಿರುಗಿ wp;
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	ಆದರೆ (wp->w_id == id)
	    ಹಿಂತಿರುಗಿ wp;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    ಪೂರ್ಣಾಂಕ
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    ಆದರೆ (win == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp)
	{
	    ಆದರೆ (wp == win)
		ಹಿಂತಿರುಗಿ TRUE;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_ಒಳ_TAB(tp, wp)
	    ಆದರೆ (wp == win)
		ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಹಿಂತಿರುಗಿ win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    ಪೂರ್ಣಾಂಕ
win_ಎಣಿಕೆ(ಶೂನ್ಯ)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;

    FOR_ALL_WINDOWS(wp)
	++ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    ಪೂರ್ಣಾಂಕ
make_windows(
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		vertical)  // split windows vertically if TRUE
{
    ಪೂರ್ಣಾಂಕ		maxcount;
    ಪೂರ್ಣಾಂಕ		todo;

    ಆದರೆ (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_ಅಗಲ + curwin->w_vsep_ಅಗಲ
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_ಎತ್ತರ(curwin) + curwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ
			- (p_wh - p_wmh)) / (p_wmh + statusline_ಎತ್ತರ(curwin));
    }

    ಆದರೆ (maxcount < 2)
	maxcount = 2;
    ಆದರೆ (ಎಣಿಕೆ_2 > maxcount)
	ಎಣಿಕೆ_2 = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    ಆದರೆ (ಎಣಿಕೆ_2 > 1)
	ಕೊನೆಯ_ಸ್ಥಿತಿ(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    ಖಂಡ_autocmds();

    // todo is number of windows left to create
    ಗಾಗಿ (todo = ಎಣಿಕೆ_2 - 1; todo > 0; --todo)
	ಆದರೆ (vertical)
	{
	    ಆದರೆ (win_ವಿಭಜಿಸು(curwin->w_ಅಗಲ - (curwin->w_ಅಗಲ - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (win_ವಿಭಜಿಸು(curwin->w_ಎತ್ತರ - (curwin->w_ಎತ್ತರ - todo
			    * statusline_ಎತ್ತರ(curwin)) / (todo + 1)
			- statusline_ಎತ್ತರ(curwin), WSP_ABOVE) == FAIL)
		ನಿಲ್ಲಿಸು;
	}

    unblock_autocmds();

    // return actual number of windows
    ಹಿಂತಿರುಗಿ (ಎಣಿಕೆ_2 - todo);
}

/*
 * Exchange current and next window
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_exchange(ದೀರ್ಘ Prenum)
{
    ಚೌಕಟ್ಟು_T	*frp;
    ಚೌಕಟ್ಟು_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    ಪೂರ್ಣಾಂಕ		ತಾತ್ಕಾಲಿಕ;

    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ONE_ಕಿಟಕಿ)	    // just one window
    {
	beep_flush();
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಪಠ್ಯ_or_buf_locked())
    {
	beep_flush();
	ಹಿಂತಿರುಗಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * find window to exchange with
     */
    ಆದರೆ (Prenum)
    {
	frp = curwin->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ->fr_ಮಗು;
	ಆಗ (frp != ಖಾಲಿ && --Prenum > 0)
	    frp = frp->fr_ಮುಂದಿನ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_ಚೌಕಟ್ಟು->fr_ಮುಂದಿನ != ಖಾಲಿ)	// Swap with next
	frp = curwin->w_ಚೌಕಟ್ಟು->fr_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ    // Swap last window in row/col with previous
	frp = curwin->w_ಚೌಕಟ್ಟು->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    ಆದರೆ (frp == ಖಾಲಿ || frp->fr_win == ಖಾಲಿ || frp->fr_win == curwin)
	ಹಿಂತಿರುಗಿ;
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
    frp2 = curwin->w_ಚೌಕಟ್ಟು->fr_prev;
    ಆದರೆ (wp->w_prev != curwin)
    {
	win_remove(curwin, ಖಾಲಿ);
	ಚೌಕಟ್ಟು_remove(curwin->w_ಚೌಕಟ್ಟು);
	win_append(wp->w_prev, curwin);
	ಚೌಕಟ್ಟು_ಸೇರಿಸು(frp, curwin->w_ಚೌಕಟ್ಟು);
    }
    ಆದರೆ (wp != wp2)
    {
	win_remove(wp, ಖಾಲಿ);
	ಚೌಕಟ್ಟು_remove(wp->w_ಚೌಕಟ್ಟು);
	win_append(wp2, wp);
	ಆದರೆ (frp2 == ಖಾಲಿ)
	    ಚೌಕಟ್ಟು_ಸೇರಿಸು(wp->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ->fr_ಮಗು, wp->w_ಚೌಕಟ್ಟು);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಚೌಕಟ್ಟು_append(frp2, wp->w_ಚೌಕಟ್ಟು);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    ತಾತ್ಕಾಲಿಕ = curwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
    ಆದರೆ (ತಾತ್ಕಾಲಿಕ != wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ)
    {
	win_new_ಎತ್ತರ(curwin, curwin->w_ಎತ್ತರ + ತಾತ್ಕಾಲಿಕ - wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ);
	win_new_ಎತ್ತರ(wp, wp->w_ಎತ್ತರ + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ - ತಾತ್ಕಾಲಿಕ);
    }
    curwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ = wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
    wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = ತಾತ್ಕಾಲಿಕ;
    ತಾತ್ಕಾಲಿಕ = curwin->w_vsep_ಅಗಲ;
    curwin->w_vsep_ಅಗಲ = wp->w_vsep_ಅಗಲ;
    wp->w_vsep_ಅಗಲ = ತಾತ್ಕಾಲಿಕ;

    ಚೌಕಟ್ಟು_fix_ಎತ್ತರ(curwin);
    ಚೌಕಟ್ಟು_fix_ಎತ್ತರ(wp);
    ಚೌಕಟ್ಟು_fix_ಅಗಲ(curwin);
    ಚೌಕಟ್ಟು_fix_ಅಗಲ(wp);

    win_comp_pos();		// recompute window positions
#ಆದರೆ defined(FEAT_STL_OPT)
    ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (wp->w_ಬಫರ್ != curbuf)
	reset_VIsual_and_resel();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_rotate(ಪೂರ್ಣಾಂಕ upwards, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    win_T	*wp1;
    win_T	*wp2;
    ಚೌಕಟ್ಟು_T	*frp;
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (ONE_ಕಿಟಕಿ)		// nothing to do
    {
	beep_flush();
	ಹಿಂತಿರುಗಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    // Check if all frames in this row/col have one window.
    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, curwin->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ->fr_ಮಗು)
	ಆದರೆ (frp->fr_win == ಖಾಲಿ)
	{
	    emsg(_(e_cannot_rotate_when_another_ಕಿಟಕಿ_is_ವಿಭಜಿಸು));
	    ಹಿಂತಿರುಗಿ;
	}

    ಆಗ (ಎಣಿಕೆ_2--)
    {
	ಆದರೆ (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ->fr_ಮಗು;
	    wp1 = frp->fr_win;
	    win_remove(wp1, ಖಾಲಿ);
	    ಚೌಕಟ್ಟು_remove(frp);

	    // find last frame and append removed window/frame after it
	    ಗಾಗಿ ( ; frp->fr_ಮುಂದಿನ != ಖಾಲಿ; frp = frp->fr_ಮುಂದಿನ)
		;
	    win_append(frp->fr_win, wp1);
	    ಚೌಕಟ್ಟು_append(frp, wp1->w_ಚೌಕಟ್ಟು);

	    wp2 = frp->fr_win;		// previously last window
	}
	ಇಲ್ಲದಿದ್ದರೆ			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    ಗಾಗಿ (frp = curwin->w_ಚೌಕಟ್ಟು; frp->fr_ಮುಂದಿನ != ಖಾಲಿ;
							   frp = frp->fr_ಮುಂದಿನ)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, ಖಾಲಿ);
	    ಚೌಕಟ್ಟು_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_ಪೋಷಕ->fr_ಮಗು->fr_win->w_prev, wp1);
	    ಚೌಕಟ್ಟು_ಸೇರಿಸು(frp->fr_ಪೋಷಕ->fr_ಮಗು, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	ಆದರೆ (n != wp1->w_ಸ್ಥಿತಿ_ಎತ್ತರ)
	{
	    win_new_ಎತ್ತರ(wp2, wp2->w_ಎತ್ತರ + n - wp1->w_ಸ್ಥಿತಿ_ಎತ್ತರ);
	    win_new_ಎತ್ತರ(wp1, wp1->w_ಎತ್ತರ + wp1->w_ಸ್ಥಿತಿ_ಎತ್ತರ - n);
	}
	wp2->w_ಸ್ಥಿತಿ_ಎತ್ತರ = wp1->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	wp1->w_ಸ್ಥಿತಿ_ಎತ್ತರ = n;
	ಚೌಕಟ್ಟು_fix_ಎತ್ತರ(wp1);
	ಚೌಕಟ್ಟು_fix_ಎತ್ತರ(wp2);
	n = wp2->w_vsep_ಅಗಲ;
	wp2->w_vsep_ಅಗಲ = wp1->w_vsep_ಅಗಲ;
	wp1->w_vsep_ಅಗಲ = n;
	ಚೌಕಟ್ಟು_fix_ಅಗಲ(wp1);
	ಚೌಕಟ್ಟು_fix_ಅಗಲ(wp2);

	// recompute w_winrow and w_wincol for all windows
	win_comp_pos();
    }
#ಆದರೆ defined(FEAT_STL_OPT)
    ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ();
#ಆದರೆ_ಅಂತ್ಯ

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Move "wp" into a new split in a given direction, possibly relative to the
 * current window.
 * "wp" must be valid in the current tabpage.
 * Returns FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
win_splitmove(win_T *wp, ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2, ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ		dir;
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ = wp->w_ಎತ್ತರ;
    ಚೌಕಟ್ಟು_T	*unflat_altfr;

    ಆದರೆ (ONE_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ OK;	// nothing to do
    ಆದರೆ (check_ವಿಭಜಿಸು_disallowed(wp) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, ಖಾಲಿ, &unflat_altfr);
    win_remove(wp, ಖಾಲಿ);
    ಕೊನೆಯ_ಸ್ಥಿತಿ(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    ಆದರೆ (win_ವಿಭಜಿಸು_ins(ಗಾತ್ರ_2, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    ಆದರೆ (ಗಾತ್ರ_2 == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(ಎತ್ತರ, wp);
	ಆದರೆ (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#ಆದರೆ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    ಶೂನ್ಯ
win_move_after(win_T *win1, win_T *win2)
{
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ;

    // check if the arguments are reasonable
    ಆದರೆ (win1 == win2)
	ಹಿಂತಿರುಗಿ;

    // check if there is something to do
    ಆದರೆ (win2->w_ಮುಂದಿನ != win1)
    {
	ಆದರೆ (win1->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ != win2->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ)
	{
	    iemsg("Trying to move a window into another frame");
	    ಹಿಂತಿರುಗಿ;
	}

	// may need to move the status line/vertical separator of the last
	//ಕಿಟಕಿ
	ಆದರೆ (win1 == lastwin)
	{
	    ಎತ್ತರ = win1->w_prev->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	    win1->w_prev->w_ಸ್ಥಿತಿ_ಎತ್ತರ = win1->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	    win1->w_ಸ್ಥಿತಿ_ಎತ್ತರ = ಎತ್ತರ;
	    ಆದರೆ (win1->w_prev->w_vsep_ಅಗಲ == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_ಅಗಲ = 0;
		win1->w_prev->w_ಚೌಕಟ್ಟು->fr_ಅಗಲ -= 1;
		win1->w_vsep_ಅಗಲ = 1;
		win1->w_ಚೌಕಟ್ಟು->fr_ಅಗಲ += 1;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (win2 == lastwin)
	{
	    ಎತ್ತರ = win1->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	    win1->w_ಸ್ಥಿತಿ_ಎತ್ತರ = win2->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	    win2->w_ಸ್ಥಿತಿ_ಎತ್ತರ = ಎತ್ತರ;
	    ಆದರೆ (win1->w_vsep_ಅಗಲ == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_ಅಗಲ = 1;
		win2->w_ಚೌಕಟ್ಟು->fr_ಅಗಲ += 1;
		win1->w_vsep_ಅಗಲ = 0;
		win1->w_ಚೌಕಟ್ಟು->fr_ಅಗಲ -= 1;
	    }
	}
	win_remove(win1, ಖಾಲಿ);
	ಚೌಕಟ್ಟು_remove(win1->w_ಚೌಕಟ್ಟು);
	win_append(win2, win1);
	ಚೌಕಟ್ಟು_append(win2->w_ಚೌಕಟ್ಟು, win1->w_ಚೌಕಟ್ಟು);

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
    ಶೂನ್ಯ
win_equal(
    win_T	*ಮುಂದಿನ_curwin,	// pointer to current window to be or NULL
    ಪೂರ್ಣಾಂಕ		current,	// do only frame with current window
    ಪೂರ್ಣಾಂಕ		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    ಆದರೆ (dir == 0)
	dir = *p_ead;
    win_equal_rec(ಮುಂದಿನ_curwin == ಖಾಲಿ ? curwin : ಮುಂದಿನ_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_ಎತ್ತರ(),
		      topframe->fr_ಅಗಲ, topframe->fr_ಎತ್ತರ);
#ಆದರೆ defined(FEAT_STL_OPT)
    ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ();
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!is_aucmd_win(ಮುಂದಿನ_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_equal_rec(
    win_T	*ಮುಂದಿನ_curwin,	// pointer to current window to be or NULL
    ಪೂರ್ಣಾಂಕ		current,	// do only frame with current window
    ಚೌಕಟ್ಟು_T	*topfr,		// frame to set size off
    ಪೂರ್ಣಾಂಕ		dir,		// 'v', 'h' or 'b', see win_equal()
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,		// horizontal position for frame
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,		// vertical position for frame
    ಪೂರ್ಣಾಂಕ		ಅಗಲ,		// new width of frame
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ)		// new height of frame
{
    ಪೂರ್ಣಾಂಕ		n, m;
    ಪೂರ್ಣಾಂಕ		extra_sep = 0;
    ಪೂರ್ಣಾಂಕ		wincount, totwincount = 0;
    ಚೌಕಟ್ಟು_T	*fr;
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_curwin_ಗಾತ್ರ = 0;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ = 0;
    ಪೂರ್ಣಾಂಕ		new_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ		has_ಮುಂದಿನ_curwin = 0;
    ಪೂರ್ಣಾಂಕ		hnc;

    ಆದರೆ (topfr->fr_layout == FR_ಎಲೆ)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	ಆದರೆ (topfr->fr_ಎತ್ತರ != ಎತ್ತರ || topfr->fr_win->w_winrow != ಅಡ್ಡಸಾಲು
		|| topfr->fr_ಅಗಲ != ಅಗಲ || topfr->fr_win->w_wincol != ಲಂಬಸಾಲು
	   )
	{
	    topfr->fr_win->w_winrow = ಅಡ್ಡಸಾಲು;
	    ಚೌಕಟ್ಟು_new_ಎತ್ತರ(topfr, ಎತ್ತರ, FALSE, FALSE, FALSE);
	    topfr->fr_win->w_wincol = ಲಂಬಸಾಲು;
	    ಚೌಕಟ್ಟು_new_ಅಗಲ(topfr, ಅಗಲ, FALSE, FALSE);
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (topfr->fr_layout == FR_ಅಡ್ಡಸಾಲು)
    {
	topfr->fr_ಅಗಲ = ಅಗಲ;
	topfr->fr_ಎತ್ತರ = ಎತ್ತರ;

	ಆದರೆ (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = ಚೌಕಟ್ಟು_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    ಆದರೆ (ಲಂಬಸಾಲು + ಅಗಲ >= firstwin->w_wincol + topframe->fr_ಅಗಲ)
		extra_sep = 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		extra_sep = 0;
	    totwincount = (n + extra_sep) / (p_wmw + 1);
	    has_ಮುಂದಿನ_curwin = ಚೌಕಟ್ಟು_has_win(topfr, ಮುಂದಿನ_curwin);

	    /*
	     * Compute width for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal width when counting p_wiw for "next_curwin".
	     */
	    m = ಚೌಕಟ್ಟು_minwidth(topfr, ಮುಂದಿನ_curwin);
	    ಸ್ಥಳ = ಅಗಲ - m;
	    ಆದರೆ (ಸ್ಥಳ < 0)
	    {
		ಮುಂದಿನ_curwin_ಗಾತ್ರ = p_wiw + ಸ್ಥಳ;
		ಸ್ಥಳ = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಮುಂದಿನ_curwin_ಗಾತ್ರ = -1;
		FOR_ALL_ಚೌಕಟ್ಟುಗಳು(fr, topfr->fr_ಮಗು)
		{
		    ಆದರೆ (!ಚೌಕಟ್ಟು_fixed_ಅಗಲ(fr))
			ಮುಂದುವರಿ;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = ಚೌಕಟ್ಟು_minwidth(fr, NOWIN);
		    new_ಗಾತ್ರ = fr->fr_ಅಗಲ;
		    ಆದರೆ (ಚೌಕಟ್ಟು_has_win(fr, ಮುಂದಿನ_curwin))
		    {
			ಸ್ಥಳ += p_wiw - p_wmw;
			ಮುಂದಿನ_curwin_ಗಾತ್ರ = 0;
			ಆದರೆ (new_ಗಾತ್ರ < p_wiw)
			    new_ಗಾತ್ರ = p_wiw;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_ಮುಂದಿನ == ಖಾಲಿ
					? extra_sep : 0)) / (p_wmw + 1);
		    ಸ್ಥಳ -= new_ಗಾತ್ರ - n;
		    ಆದರೆ (ಸ್ಥಳ < 0)
		    {
			new_ಗಾತ್ರ += ಸ್ಥಳ;
			ಸ್ಥಳ = 0;
		    }
		    fr->fr_newwidth = new_ಗಾತ್ರ;
		}
		ಆದರೆ (ಮುಂದಿನ_curwin_ಗಾತ್ರ == -1)
		{
		    ಆದರೆ (!has_ಮುಂದಿನ_curwin)
			ಮುಂದಿನ_curwin_ಗಾತ್ರ = 0;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (totwincount > 1
			    && (ಸ್ಥಳ + (totwincount - 2))
						  / (totwincount - 1) > p_wiw)
		    {
			// Can make all windows wider than 'winwidth', spread
			// the room equally.
			ಮುಂದಿನ_curwin_ಗಾತ್ರ = (ಸ್ಥಳ + p_wiw
					    + (totwincount - 1) * p_wmw
					    + (totwincount - 1)) / totwincount;
			ಸ್ಥಳ -= ಮುಂದಿನ_curwin_ಗಾತ್ರ - p_wiw;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ಮುಂದಿನ_curwin_ಗಾತ್ರ = p_wiw;
		}
	    }

	    ಆದರೆ (has_ಮುಂದಿನ_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(fr, topfr->fr_ಮಗು)
	{
	    wincount = 1;
	    ಆದರೆ (fr->fr_ಮುಂದಿನ == ಖಾಲಿ)
		// last frame gets all that remains (avoid roundoff error)
		new_ಗಾತ್ರ = ಅಗಲ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir == 'v')
		new_ಗಾತ್ರ = fr->fr_ಅಗಲ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಚೌಕಟ್ಟು_fixed_ಅಗಲ(fr))
	    {
		new_ಗಾತ್ರ = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = ಚೌಕಟ್ಟು_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_ಮುಂದಿನ == ಖಾಲಿ ? extra_sep : 0))
								/ (p_wmw + 1);
		m = ಚೌಕಟ್ಟು_minwidth(fr, ಮುಂದಿನ_curwin);
		ಆದರೆ (has_ಮುಂದಿನ_curwin)
		    hnc = ಚೌಕಟ್ಟು_has_win(fr, ಮುಂದಿನ_curwin);
		ಇಲ್ಲದಿದ್ದರೆ
		    hnc = FALSE;
		ಆದರೆ (hnc)	    // don't count next_curwin
		    --wincount;
		ಆದರೆ (totwincount == 0)
		    new_ಗಾತ್ರ = ಸ್ಥಳ;
		ಇಲ್ಲದಿದ್ದರೆ
		    new_ಗಾತ್ರ = (wincount * ಸ್ಥಳ + ((ಚಿಹ್ನೆರಹಿತ)totwincount >> 1))
								/ totwincount;
		ಆದರೆ (hnc)	    // add next_curwin size
		{
		    ಮುಂದಿನ_curwin_ಗಾತ್ರ -= p_wiw - (m - n);
		    ಆದರೆ (ಮುಂದಿನ_curwin_ಗಾತ್ರ < 0)
			ಮುಂದಿನ_curwin_ಗಾತ್ರ = 0;
		    new_ಗಾತ್ರ += ಮುಂದಿನ_curwin_ಗಾತ್ರ;
		    ಸ್ಥಳ -= new_ಗಾತ್ರ - ಮುಂದಿನ_curwin_ಗಾತ್ರ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸ್ಥಳ -= new_ಗಾತ್ರ;
		new_ಗಾತ್ರ += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    ಆದರೆ (!current || dir != 'v' || topfr->fr_ಪೋಷಕ != ಖಾಲಿ
		    || (new_ಗಾತ್ರ != fr->fr_ಅಗಲ)
		    || ಚೌಕಟ್ಟು_has_win(fr, ಮುಂದಿನ_curwin))
		win_equal_rec(ಮುಂದಿನ_curwin, current, fr, dir, ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು,
							    new_ಗಾತ್ರ, ಎತ್ತರ);
	    ಲಂಬಸಾಲು += new_ಗಾತ್ರ;
	    ಅಗಲ -= new_ಗಾತ್ರ;
	    totwincount -= wincount;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ // topfr->fr_layout == FR_COL
    {
	topfr->fr_ಅಗಲ = ಅಗಲ;
	topfr->fr_ಎತ್ತರ = ಎತ್ತರ;

	ಆದರೆ (dir != 'h')			// equalize frame heights
	{
	    totwincount = ಚೌಕಟ್ಟು_wincount_ಒಳ_ಎತ್ತರ(topfr);
	    has_ಮುಂದಿನ_curwin = ಚೌಕಟ್ಟು_has_win(topfr, ಮುಂದಿನ_curwin);

	    /*
	     * Compute height for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal height when counting p_wh for "next_curwin".
	     */
	    m = ಚೌಕಟ್ಟು_minheight(topfr, ಮುಂದಿನ_curwin);
	    ಸ್ಥಳ = ಎತ್ತರ - m;
	    ಆದರೆ (ಸ್ಥಳ < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		ಮುಂದಿನ_curwin_ಗಾತ್ರ = p_wh + ಸ್ಥಳ;
		ಸ್ಥಳ = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಮುಂದಿನ_curwin_ಗಾತ್ರ = -1;
		FOR_ALL_ಚೌಕಟ್ಟುಗಳು(fr, topfr->fr_ಮಗು)
		{
		    ಆದರೆ (!ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(fr))
			ಮುಂದುವರಿ;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = ಚೌಕಟ್ಟು_minheight(fr, NOWIN);
		    new_ಗಾತ್ರ = fr->fr_ಎತ್ತರ;
		    ಆದರೆ (ಚೌಕಟ್ಟು_has_win(fr, ಮುಂದಿನ_curwin))
		    {
			ಸ್ಥಳ += p_wh - p_wmh;
			ಮುಂದಿನ_curwin_ಗಾತ್ರ = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			ಆದರೆ (new_ಗಾತ್ರ < n + (p_wh - p_wmh))
			    new_ಗಾತ್ರ = n + (p_wh - p_wmh);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			// These windows don't use up room.
			totwincount -= ಚೌಕಟ್ಟು_wincount_ಒಳ_ಎತ್ತರ(fr);
		    ಸ್ಥಳ -= new_ಗಾತ್ರ - n;
		    ಆದರೆ (ಸ್ಥಳ < 0)
		    {
			new_ಗಾತ್ರ += ಸ್ಥಳ;
			ಸ್ಥಳ = 0;
		    }
		    fr->fr_newheight = new_ಗಾತ್ರ;
		}
		ಆದರೆ (ಮುಂದಿನ_curwin_ಗಾತ್ರ == -1)
		{
		    ಆದರೆ (!has_ಮುಂದಿನ_curwin)
			ಮುಂದಿನ_curwin_ಗಾತ್ರ = 0;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (totwincount > 1
			    && (ಸ್ಥಳ + (totwincount - 2))
						   / (totwincount - 1) > p_wh)
		    {
			// can make all windows higher than 'winheight',
			// spread the room equally.
			ಮುಂದಿನ_curwin_ಗಾತ್ರ = (ಸ್ಥಳ + p_wh
					   + (totwincount - 1) * p_wmh
					   + (totwincount - 1)) / totwincount;
			ಸ್ಥಳ -= ಮುಂದಿನ_curwin_ಗಾತ್ರ - p_wh;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ಮುಂದಿನ_curwin_ಗಾತ್ರ = p_wh;
		}
	    }

	    ಆದರೆ (has_ಮುಂದಿನ_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(fr, topfr->fr_ಮಗು)
	{
	    wincount = 1;
	    ಆದರೆ (fr->fr_ಮುಂದಿನ == ಖಾಲಿ)
		// last frame gets all that remains (avoid roundoff error)
		new_ಗಾತ್ರ = ಎತ್ತರ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir == 'h')
		new_ಗಾತ್ರ = fr->fr_ಎತ್ತರ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(fr))
	    {
		new_ಗಾತ್ರ = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = ಚೌಕಟ್ಟು_minheight(fr, NOWIN);
		wincount = ಚೌಕಟ್ಟು_wincount_ಒಳ_ಎತ್ತರ(fr);
		m = ಚೌಕಟ್ಟು_minheight(fr, ಮುಂದಿನ_curwin);
		ಆದರೆ (has_ಮುಂದಿನ_curwin)
		    hnc = ಚೌಕಟ್ಟು_has_win(fr, ಮುಂದಿನ_curwin);
		ಇಲ್ಲದಿದ್ದರೆ
		    hnc = FALSE;
		ಆದರೆ (hnc)	    // don't count next_curwin
		    --wincount;
		ಆದರೆ (totwincount == 0)
		    new_ಗಾತ್ರ = ಸ್ಥಳ;
		ಇಲ್ಲದಿದ್ದರೆ
		    new_ಗಾತ್ರ = (wincount * ಸ್ಥಳ + ((ಚಿಹ್ನೆರಹಿತ)totwincount >> 1))
								/ totwincount;
		ಆದರೆ (hnc)	    // add next_curwin size
		{
		    ಮುಂದಿನ_curwin_ಗಾತ್ರ -= p_wh - (m - n);
		    new_ಗಾತ್ರ += ಮುಂದಿನ_curwin_ಗಾತ್ರ;
		    ಸ್ಥಳ -= new_ಗಾತ್ರ - ಮುಂದಿನ_curwin_ಗಾತ್ರ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸ್ಥಳ -= new_ಗಾತ್ರ;
		new_ಗಾತ್ರ += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    ಆದರೆ (!current || dir != 'h' || topfr->fr_ಪೋಷಕ != ಖಾಲಿ
		    || (new_ಗಾತ್ರ != fr->fr_ಎತ್ತರ)
		    || ಚೌಕಟ್ಟು_has_win(fr, ಮುಂದಿನ_curwin))
		win_equal_rec(ಮುಂದಿನ_curwin, current, fr, dir, ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು,
							     ಅಗಲ, new_ಗಾತ್ರ);
	    ಅಡ್ಡಸಾಲು += new_ಗಾತ್ರ;
	    ಎತ್ತರ -= new_ಗಾತ್ರ;
	    totwincount -= wincount;
	}
    }
}

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
    ಶೂನ್ಯ
leaving_ಕಿಟಕಿ(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    ಆದರೆ (!bt_prompt(win->w_ಬಫರ್) || is_aucmd_win(win))
	ಹಿಂತಿರುಗಿ;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_ಬಫರ್->b_prompt_ಸೇರಿಸು = restart_edit;
    ಆದರೆ (restart_edit != 0 && ಬಹುಲಕ_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) && !stop_ಸೇರಿಸು_ಬಹುಲಕ)
    {
	stop_ಸೇರಿಸು_ಬಹುಲಕ = TRUE;
	ಆದರೆ (win->w_ಬಫರ್->b_prompt_ಸೇರಿಸು == NUL)
	    win->w_ಬಫರ್->b_prompt_ಸೇರಿಸು = 'A';
    }
}

    ಶೂನ್ಯ
entering_ಕಿಟಕಿ(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    ಆದರೆ (!bt_prompt(win->w_ಬಫರ್) || is_aucmd_win(win))
	ಹಿಂತಿರುಗಿ;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    ಆದರೆ (win->w_ಬಫರ್->b_prompt_ಸೇರಿಸು != NUL)
	stop_ಸೇರಿಸು_ಬಹುಲಕ = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) == 0)
	restart_edit = win->w_ಬಫರ್->b_prompt_ಸೇರಿಸು;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
win_init_ಖಾಲಿಗಳು(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_ಸಾಲುಗಳು_valid = 0;
    wp->w_cursor.lnum = 1;
    wp->w_curswant = wp->w_cursor.ಲಂಬಸಾಲು = 0;
    wp->w_cursor.coladd = 0;
    wp->w_pcmark.lnum = 1;	// pcmark not cleared but set to line 1
    wp->w_pcmark.ಲಂಬಸಾಲು = 0;
    wp->w_prev_pcmark.lnum = 0;
    wp->w_prev_pcmark.ಲಂಬಸಾಲು = 0;
    wp->w_topline = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    wp->w_topfill = 0;
#ಆದರೆ_ಅಂತ್ಯ
    wp->w_botline = 2;
    wp->w_valid = 0;
#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    wp->w_s = &wp->w_ಬಫರ್->b_s;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಪದ_reset_hlfwin(wp);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Init the current window "curwin".
 * Called when a new file is being edited.
 */
    ಶೂನ್ಯ
curwin_init(ಶೂನ್ಯ)
{
    win_init_ಖಾಲಿಗಳು(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    ಶೂನ್ಯ
close_windows(
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = tabpage_ಸೂಚ್ಯಂಕ(ಖಾಲಿ);

    ++RedrawingDisabled;

    ಗಾಗಿ (wp = firstwin; wp != ಖಾಲಿ && !ONE_ಕಿಟಕಿ; )
    {
	ಆದರೆ (wp->w_ಬಫರ್ == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_ಬಫರ್->b_locked > 0))
	{
	    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_ಗಾತ್ರ))
		ಹೋಗು theend;  // Only give one error message.
	    ಆದರೆ (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		ನಿಲ್ಲಿಸು;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    wp = wp->w_ಮುಂದಿನ;
    }

    // Also check windows in other tab pages.
    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ; tp = nexttp)
    {
	nexttp = tp->tp_ಮುಂದಿನ;
	ಆದರೆ (tp != curtab)
	    FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp)
		ಆದರೆ (wp->w_ಬಫರ್ == buf
		    && !(win_locked(wp) || wp->w_ಬಫರ್->b_locked > 0))
		{
		    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_ಗಾತ್ರ))
			ಹೋಗು theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = ಮೊದಲ_tabpage;
		    ನಿಲ್ಲಿಸು;
		}
    }

theend:
    ಆದರೆ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    ಆದರೆ (ಎಣಿಕೆ_2 != tabpage_ಸೂಚ್ಯಂಕ(ಖಾಲಿ))
	apply_autocmds(EVENT_TABCLOSED, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    ಪೂರ್ಣಾಂಕ
ಕೊನೆಯ_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (one_ಕಿಟಕಿ() && ಮೊದಲ_tabpage->tp_ಮುಂದಿನ == ಖಾಲಿ);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    ಪೂರ್ಣಾಂಕ
one_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		ನೋಡಿದ_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ (!is_aucmd_win(wp))
	{
	    ಆದರೆ (ನೋಡಿದ_one)
		ಹಿಂತಿರುಗಿ FALSE;
	    ನೋಡಿದ_one = TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
close_ಕೊನೆಯ_ಕಿಟಕಿ_tabpage(
    win_T	*win,
    ಪೂರ್ಣಾಂಕ		free_buf,
    tabpage_T   *prev_curtab)
{
    ಆದರೆ (!ONE_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_ಬಫರ್ != ಖಾಲಿ);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    ಆದರೆ (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    ಆದರೆ (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    entering_ಕಿಟಕಿ(curwin);
#ಆದರೆ_ಅಂತ್ಯ
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
#ಆದರೆ defined(FEAT_TABPANEL)
    ಆದರೆ (p_stpl > 0)
	shell_new_columns();
#ಆದರೆ_ಅಂತ್ಯ
    apply_autocmds(EVENT_WINENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    ಆದರೆ (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_close_ಬಫರ್(win_T *win, ಪೂರ್ಣಾಂಕ action, ಪೂರ್ಣಾಂಕ abort_if_ಕೊನೆಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    ಆದರೆ (win->w_ಬಫರ್ != ಖಾಲಿ)
	reset_synblock(win);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    ಆದರೆ (win->w_ಬಫರ್ != ಖಾಲಿ && bt_quickfix(win->w_ಬಫರ್)
					&& win->w_ಬಫರ್->b_nwindows == 1)
	win->w_ಬಫರ್->b_p_bl = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // Close the link to the buffer.
    ಆದರೆ (win->w_ಬಫರ್ != ಖಾಲಿ)
    {
	bufref_T    bufref;

	ಗಣ_bufref(&bufref, curbuf);
	++win->w_locked;
	close_ಬಫರ್(win, win->w_ಬಫರ್, action, abort_if_ಕೊನೆಯ, TRUE, TRUE);
	ಆದರೆ (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	ಆದರೆ (!bufref_valid(&bufref))
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
    ಪೂರ್ಣಾಂಕ
win_close(win_T *win, ಪೂರ್ಣಾಂಕ free_buf)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		other_ಬಫರ್ = FALSE;
    ಪೂರ್ಣಾಂಕ		close_curwin = FALSE;
    ಪೂರ್ಣಾಂಕ		dir;
    ಪೂರ್ಣಾಂಕ		help_ಕಿಟಕಿ = FALSE;
    ಪೂರ್ಣಾಂಕ		quickfix_ಕಿಟಕಿ = FALSE;
    tabpage_T   *prev_curtab = curtab;
    ಚೌಕಟ್ಟು_T	*win_ಚೌಕಟ್ಟು = win->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		had_diffmode = win->w_p_diff;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
    ಪೂರ್ಣಾಂಕ		did_decrement = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    ಆದರೆ (may_close_ಪದ_popup() == OK)
	ಹಿಂತಿರುಗಿ OK;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (ಕೊನೆಯ_ಕಿಟಕಿ())
    {
	emsg(_(e_cannot_close_ಕೊನೆಯ_ಕಿಟಕಿ));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_close))
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (win_locked(win) || (win->w_ಬಫರ್ != ಖಾಲಿ
					       && win->w_ಬಫರ್->b_locked > 0))
	ಹಿಂತಿರುಗಿ FAIL; // window is already being closed
    ಆದರೆ (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_ಕಿಟಕಿ));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_ಕಿಟಕಿ())
    {
	emsg(_(e_cannot_close_ಕಿಟಕಿ_only_autocmd_ಕಿಟಕಿ_would_remain));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    ಆದರೆ (close_ಕೊನೆಯ_ಕಿಟಕಿ_tabpage(win, free_buf, prev_curtab))
	ಹಿಂತಿರುಗಿ FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    ಆದರೆ (bt_help(win->w_ಬಫರ್))
	help_ಕಿಟಕಿ = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	clear_snapshot(curtab, SNAP_HELP_IDX);

    ಆದರೆ (bt_quickfix(win->w_ಬಫರ್))
	quickfix_ಕಿಟಕಿ = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    ಆದರೆ (win == curwin)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	leaving_ಕಿಟಕಿ(curwin);
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, ಖಾಲಿ));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	ಆದರೆ (wp->w_ಬಫರ್ != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_ಬಫರ್ = TRUE;
	    ಆದರೆ (!win_valid(win))
		ಹಿಂತಿರುಗಿ FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	    ಆದರೆ (!win_valid(win))
		ಹಿಂತಿರುಗಿ FAIL;
	    --win->w_locked;
	    ಆದರೆ (ಕೊನೆಯ_ಕಿಟಕಿ())
		ಹಿಂತಿರುಗಿ FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	ಆದರೆ (!win_valid(win))
	    ಹಿಂತಿರುಗಿ FAIL;
	--win->w_locked;
	ಆದರೆ (ಕೊನೆಯ_ಕಿಟಕಿ())
	    ಹಿಂತಿರುಗಿ FAIL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// autocmds may abort script processing
	ಆದರೆ (aborting())
	    ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    ಆದರೆ (gui.ಒಳ_use)
	ಹೊರಹೋಗು_flush();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (popup_win_closed(win) && !win_valid(win))
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    ಆದರೆ (!win_valid_any_tab(win))
	ಹಿಂತಿರುಗಿ OK;

    win_close_ಬಫರ್(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    ಆದರೆ (win_valid(win) && win->w_ಬಫರ್ == ಖಾಲಿ
#ಆದರೆ defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#ಆದರೆ_ಅಂತ್ಯ
	    && ಕೊನೆಯ_ಕಿಟಕಿ())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	ಆದರೆ (curwin->w_ಬಫರ್ == ಖಾಲಿ)
	    curwin->w_ಬಫರ್ = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    ಆದರೆ (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_ಬಫರ್ == ಖಾಲಿ)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    ಆದರೆ (!win_valid(win) || ಕೊನೆಯ_ಕಿಟಕಿ()
	    || close_ಕೊನೆಯ_ಕಿಟಕಿ_tabpage(win, free_buf, prev_curtab))
	ಹಿಂತಿರುಗಿ FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++ವಿಭಜಿಸು_disallowed;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
    ++dont_parse_messages;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (win->w_ಬಫರ್ != ಖಾಲಿ)
	--win->w_ಬಫರ್->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, ಖಾಲಿ);

    ಆದರೆ (help_ಕಿಟಕಿ || quickfix_ಕಿಟಕಿ)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_ಕಿಟಕಿ ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	ಆದರೆ (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    ಆದರೆ (win == curwin)
    {
	curwin = wp;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	ಆದರೆ (wp->w_p_pvw || bt_quickfix(wp->w_ಬಫರ್))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    ಗಾಗಿ (;;)
	    {
		ಆದರೆ (wp->w_ಮುಂದಿನ == ಖಾಲಿ)
		    wp = firstwin;
		ಇಲ್ಲದಿದ್ದರೆ
		    wp = wp->w_ಮುಂದಿನ;
		ಆದರೆ (wp == curwin)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (!wp->w_p_pvw && !bt_quickfix(wp->w_ಬಫರ್))
		{
		    curwin = wp;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	curbuf = curwin->w_ಬಫರ್;
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
    ಕೊನೆಯ_ಸ್ಥಿತಿ(FALSE);

    ಆದರೆ (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ == win_ಚೌಕಟ್ಟು, dir);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    ಆದರೆ (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
	did_decrement =
#ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)
#ಆದರೆ_ಅಂತ್ಯ
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	ಆದರೆ (other_ಬಫರ್)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    }

    ಆದರೆ (ONE_ಕಿಟಕಿ && curwin->w_locked && curbuf->b_locked_ವಿಭಜಿಸು
	    && ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);

    --ವಿಭಜಿಸು_disallowed;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
    ಆದರೆ (!did_decrement)
	--dont_parse_messages;
#ಆದರೆ_ಅಂತ್ಯ

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    ಆದರೆ (help_ಕಿಟಕಿ || quickfix_ಕಿಟಕಿ)
	restore_snapshot(help_ಕಿಟಕಿ ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    ಆದರೆ (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	ಪೂರ್ಣಾಂಕ	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    ಆದರೆ (dwin->w_p_diff)
		++diffcount;
	ಆದರೆ (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    ಆದರೆ (gui.ಒಳ_use && !win_hasvertsplit())
	gui_init_which_components(ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    redraw_all_later(UPD_NOT_VALID);
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
trigger_winnewpre(ಶೂನ್ಯ)
{
    ಕಿಟಕಿ_layout_ಬೀಗ();
    apply_autocmds(EVENT_WINNEWPRE, ಖಾಲಿ, ಖಾಲಿ, FALSE, ಖಾಲಿ);
    ಕಿಟಕಿ_layout_unlock();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
trigger_winclosed(win_T *win)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ;
    recursive = TRUE;
    vim_snprintf((ಅಕ್ಷರ *)winid, ಗಾತ್ರ(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_ಬಫರ್);
    recursive = FALSE;
}

    ಶೂನ್ಯ
trigger_tabclosedpre(tabpage_T *tp)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    ಆದರೆ (!has_tabclosedpre() || recursive)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    ಕಿಟಕಿ_layout_ಬೀಗ();
    apply_autocmds(EVENT_TABCLOSEDPRE, ಖಾಲಿ, ಖಾಲಿ, FALSE, ಖಾಲಿ);
    ಕಿಟಕಿ_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    ಆದರೆ (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
	// fall back to the first tabpage
	goto_tabpage_tp(ಮೊದಲ_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    ಶೂನ್ಯ
snapshot_windows_scroll_ಗಾತ್ರ(ಶೂನ್ಯ)
{
    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	wp->w_ಕೊನೆಯ_topline = wp->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	wp->w_ಕೊನೆಯ_topfill = wp->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ
	wp->w_ಕೊನೆಯ_leftcol = wp->w_leftcol;
	wp->w_ಕೊನೆಯ_skipcol = wp->w_skipcol;
	wp->w_ಕೊನೆಯ_ಅಗಲ = wp->w_ಅಗಲ;
	wp->w_ಕೊನೆಯ_ಎತ್ತರ = wp->w_ಎತ್ತರ;
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ did_initial_scroll_ಗಾತ್ರ_snapshot = FALSE;

    ಶೂನ್ಯ
may_make_initial_scroll_ಗಾತ್ರ_snapshot(ಶೂನ್ಯ)
{
    ಆದರೆ (!did_initial_scroll_ಗಾತ್ರ_snapshot)
    {
	did_initial_scroll_ಗಾತ್ರ_snapshot = TRUE;
	snapshot_windows_scroll_ಗಾತ್ರ();
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Create a dictionary with information about size and scroll changes in a
 * window.
 * Returns the dictionary with refcount set to one.
 * Returns NULL when out of memory.
 */
    ಸ್ಥಿರ dict_T *
make_win_info_dict(
	ಪೂರ್ಣಾಂಕ ಅಗಲ,
	ಪೂರ್ಣಾಂಕ ಎತ್ತರ,
	ಪೂರ್ಣಾಂಕ topline,
# ifdef FEAT_DIFF
	ಪೂರ್ಣಾಂಕ topfill,
# endif
	ಪೂರ್ಣಾಂಕ leftcol,
	ಪೂರ್ಣಾಂಕ skipcol)
{
    dict_T *d = dict_alloc();
    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    ಆಗ (1)
    {
	typval_T tv;
	tv.v_ಬೀಗ = 0;
	tv.v_type = VAR_ಸಂಖ್ಯೆ;

	tv.vval.v_ಸಂಖ್ಯೆ = ಅಗಲ;
	ಆದರೆ (dict_ಕೂಡಿಸು_tv(d, "width", &tv) == FAIL)
	    ನಿಲ್ಲಿಸು;
	tv.vval.v_ಸಂಖ್ಯೆ = ಎತ್ತರ;
	ಆದರೆ (dict_ಕೂಡಿಸು_tv(d, "height", &tv) == FAIL)
	    ನಿಲ್ಲಿಸು;
	tv.vval.v_ಸಂಖ್ಯೆ = topline;
	ಆದರೆ (dict_ಕೂಡಿಸು_tv(d, "topline", &tv) == FAIL)
	    ನಿಲ್ಲಿಸು;
# ifdef FEAT_DIFF
	tv.vval.v_ಸಂಖ್ಯೆ = topfill;
# ಇಲ್ಲದಿದ್ದರೆ
	tv.vval.v_ಸಂಖ್ಯೆ = 0;
# endif
	ಆದರೆ (dict_ಕೂಡಿಸು_tv(d, "topfill", &tv) == FAIL)
	    ನಿಲ್ಲಿಸು;
	tv.vval.v_ಸಂಖ್ಯೆ = leftcol;
	ಆದರೆ (dict_ಕೂಡಿಸು_tv(d, "leftcol", &tv) == FAIL)
	    ನಿಲ್ಲಿಸು;
	tv.vval.v_ಸಂಖ್ಯೆ = skipcol;
	ಆದರೆ (dict_ಕೂಡಿಸು_tv(d, "skipcol", &tv) == FAIL)
	    ನಿಲ್ಲಿಸು;
	ಹಿಂತಿರುಗಿ d;
    }
    dict_unref(d);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

// Return values of check_window_scroll_resize():
#ವ್ಯಾಖ್ಯಾನಿಸು CWSR_SCROLLED	1  // at least one window scrolled
#ವ್ಯಾಖ್ಯಾನಿಸು CWSR_RESIZED	2  // at least one window size changed

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
    ಸ್ಥಿರ ಶೂನ್ಯ
check_ಕಿಟಕಿ_scroll_resize(
	ಪೂರ್ಣಾಂಕ	*ಗಾತ್ರ_ಎಣಿಕೆ,
	win_T	**ಮೊದಲ_scroll_win,
	win_T	**ಮೊದಲ_ಗಾತ್ರ_win,
	list_T	*winlist ಬಳಸದ,
	dict_T	*v_event ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ listidx = 0;
    ಪೂರ್ಣಾಂಕ tot_ಅಗಲ = 0;
    ಪೂರ್ಣಾಂಕ tot_ಎತ್ತರ = 0;
    ಪೂರ್ಣಾಂಕ tot_topline = 0;
# ifdef FEAT_DIFF
    ಪೂರ್ಣಾಂಕ tot_topfill = 0;
# endif
    ಪೂರ್ಣಾಂಕ tot_leftcol = 0;
    ಪೂರ್ಣಾಂಕ tot_skipcol = 0;
#ಆದರೆ_ಅಂತ್ಯ

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	ಪೂರ್ಣಾಂಕ ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_ಬದಲಾಯಿತು = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_ಕೊನೆಯ_ಅಗಲ != wp->w_ಅಗಲ
			    || wp->w_ಕೊನೆಯ_ಎತ್ತರ != wp->w_ಎತ್ತರ);
	ಆದರೆ (ಗಾತ್ರ_ಬದಲಾಯಿತು)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (winlist != ಖಾಲಿ)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_ಬೀಗ = 0;
		tv.v_type = VAR_ಸಂಖ್ಯೆ;
		tv.vval.v_ಸಂಖ್ಯೆ = wp->w_id;
		list_ಗಣ_ಅಂಶ(winlist, listidx++, &tv);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (ಗಾತ್ರ_ಎಣಿಕೆ != ಖಾಲಿ)
	    {
		++*ಗಾತ್ರ_ಎಣಿಕೆ;
		ಆದರೆ (*ಮೊದಲ_ಗಾತ್ರ_win == ಖಾಲಿ)
		    *ಮೊದಲ_ಗಾತ್ರ_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		ಆದರೆ (*ಮೊದಲ_scroll_win == ಖಾಲಿ && !ignore_scroll)
		    *ಮೊದಲ_scroll_win = wp;
	    }
	}

	ಪೂರ್ಣಾಂಕ scroll_ಬದಲಾಯಿತು = !ignore_scroll
				&& (wp->w_ಕೊನೆಯ_topline != wp->w_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
				|| wp->w_ಕೊನೆಯ_topfill != wp->w_topfill
#ಆದರೆ_ಅಂತ್ಯ
				|| wp->w_ಕೊನೆಯ_leftcol != wp->w_leftcol
				|| wp->w_ಕೊನೆಯ_skipcol != wp->w_skipcol);
	ಆದರೆ (scroll_ಬದಲಾಯಿತು
	    && ಮೊದಲ_scroll_win != ಖಾಲಿ && *ಮೊದಲ_scroll_win == ಖಾಲಿ)
	    *ಮೊದಲ_scroll_win = wp;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ ((ಗಾತ್ರ_ಬದಲಾಯಿತು || scroll_ಬದಲಾಯಿತು) && v_event != ಖಾಲಿ)
	{
	    // Add info about this window to the v:event dictionary.
	    ಪೂರ್ಣಾಂಕ ಅಗಲ = wp->w_ಅಗಲ - wp->w_ಕೊನೆಯ_ಅಗಲ;
	    ಪೂರ್ಣಾಂಕ ಎತ್ತರ = wp->w_ಎತ್ತರ - wp->w_ಕೊನೆಯ_ಎತ್ತರ;
	    ಪೂರ್ಣಾಂಕ topline = wp->w_topline - wp->w_ಕೊನೆಯ_topline;
# ifdef FEAT_DIFF
	    ಪೂರ್ಣಾಂಕ topfill = wp->w_topfill - wp->w_ಕೊನೆಯ_topfill;
# endif
	    ಪೂರ್ಣಾಂಕ leftcol = wp->w_leftcol - wp->w_ಕೊನೆಯ_leftcol;
	    ಪೂರ್ಣಾಂಕ skipcol = wp->w_skipcol - wp->w_ಕೊನೆಯ_skipcol;
	    dict_T *d = make_win_info_dict(ಅಗಲ, ಎತ್ತರ, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    ಆದರೆ (d == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    ಅಕ್ಷರ winid[NUMBUFLEN];
	    vim_snprintf(winid, ಗಾತ್ರ(winid), "%d", wp->w_id);
	    ಆದರೆ (dict_ಕೂಡಿಸು_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		ನಿಲ್ಲಿಸು;
	    }
	    --d->dv_refcount;

	    tot_ಅಗಲ += ನಿರಪೇಕ್ಷ(ಅಗಲ);
	    tot_ಎತ್ತರ += ನಿರಪೇಕ್ಷ(ಎತ್ತರ);
	    tot_topline += ನಿರಪೇಕ್ಷ(topline);
# ifdef FEAT_DIFF
	    tot_topfill += ನಿರಪೇಕ್ಷ(topfill);
# endif
	    tot_leftcol += ನಿರಪೇಕ್ಷ(leftcol);
	    tot_skipcol += ನಿರಪೇಕ್ಷ(skipcol);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (v_event != ಖಾಲಿ)
    {
	dict_T *alldict = make_win_info_dict(tot_ಅಗಲ, tot_ಎತ್ತರ, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	ಆದರೆ (alldict != ಖಾಲಿ)
	{
	    ಆದರೆ (dict_ಕೂಡಿಸು_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    ಇಲ್ಲದಿದ್ದರೆ
		--alldict->dv_refcount;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    ಶೂನ್ಯ
may_trigger_win_scrolled_resized(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    recursive = FALSE;
    ಪೂರ್ಣಾಂಕ		    do_resize = has_winresized();
    ಪೂರ್ಣಾಂಕ		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    ಆದರೆ (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_ಗಾತ್ರ_snapshot)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_ಎಣಿಕೆ = 0;
    win_T *ಮೊದಲ_scroll_win = ಖಾಲಿ, *ಮೊದಲ_ಗಾತ್ರ_win = ಖಾಲಿ;
    check_ಕಿಟಕಿ_scroll_resize(&ಗಾತ್ರ_ಎಣಿಕೆ, &ಮೊದಲ_scroll_win, &ಮೊದಲ_ಗಾತ್ರ_win,
								    ಖಾಲಿ, ಖಾಲಿ);
    ಪೂರ್ಣಾಂಕ trigger_resize = do_resize && ಗಾತ್ರ_ಎಣಿಕೆ > 0;
    ಪೂರ್ಣಾಂಕ trigger_scroll = do_scroll && ಮೊದಲ_scroll_win != ಖಾಲಿ;
    ಆದರೆ (!trigger_resize && !trigger_scroll)
	ಹಿಂತಿರುಗಿ;  // no relevant changes
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    list_T *windows_list = ಖಾಲಿ;
    ಆದರೆ (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_ವಸ್ತುಗಳು(ಗಾತ್ರ_ಎಣಿಕೆ);
	ಆದರೆ (windows_list != ಖಾಲಿ)
	    check_ಕಿಟಕಿ_scroll_resize(ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, windows_list, ಖಾಲಿ);
    }

    dict_T *scroll_dict = ಖಾಲಿ;
    ಆದರೆ (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	ಆದರೆ (scroll_dict != ಖಾಲಿ)
	{
	    scroll_dict->dv_refcount = 1;
	    check_ಕಿಟಕಿ_scroll_resize(ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, scroll_dict);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    // WinScrolled/WinResized are triggered only once, even when multiple
    // windows scrolled or changed size.  Store the current values before
    // triggering the event, if a scroll or resize happens as a side effect
    // then WinScrolled/WinResized is triggered for that later.
    snapshot_windows_scroll_ಗಾತ್ರ();

    // "curwin" may be different from the actual current window, make
    // sure it can be restored.
    ಕಿಟಕಿ_layout_ಬೀಗ();
    recursive = TRUE;

    // If both are to be triggered do WinResized first.
    ಆದರೆ (trigger_resize
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    && windows_list != ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	ಆದರೆ (dict_ಕೂಡಿಸು_list(v_event, "windows", windows_list) == OK)
	{
	    dict_ಗಣ_ವಸ್ತುಗಳು_ro(v_event);
#ಆದರೆ_ಅಂತ್ಯ
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((ಅಕ್ಷರ *)winid, ಗಾತ್ರ(winid), "%d",
							 ಮೊದಲ_ಗಾತ್ರ_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     ಮೊದಲ_ಗಾತ್ರ_win->w_ಬಫರ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (trigger_scroll
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    && scroll_dict != ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", ಖಾಲಿ);
	dict_ಗಣ_ವಸ್ತುಗಳು_ro(v_event);
	dict_unref(scroll_dict);
#ಆದರೆ_ಅಂತ್ಯ
	char_u winid[NUMBUFLEN];
	vim_snprintf((ಅಕ್ಷರ *)winid, ಗಾತ್ರ(winid), "%d",
						       ಮೊದಲ_scroll_win->w_id);
	apply_autocmds(EVENT_WINSCROLLED, winid, winid, FALSE,
						   ಮೊದಲ_scroll_win->w_ಬಫರ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	restore_v_event(v_event, &save_v_event);
#ಆದರೆ_ಅಂತ್ಯ
    }

    recursive = FALSE;
    ಕಿಟಕಿ_layout_unlock();
}

/*
 * Close window "win" in tab page "tp", which is not the current tab page.
 * This may be the last window in that tab page and result in closing the tab,
 * thus "tp" may become invalid!
 * Caller must check if buffer is hidden and whether the tabline needs to be
 * updated.
 */
    ಶೂನ್ಯ
win_close_othertab(win_T *win, ಪೂರ್ಣಾಂಕ free_buf, tabpage_T *tp)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		dir;
    tabpage_T   *ptp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_ಗಾತ್ರ))
	ಹಿಂತಿರುಗಿ;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    ಆದರೆ (win_locked(win) || (win->w_ಬಫರ್ != ಖಾಲಿ
					       && win->w_ಬಫರ್->b_locked > 0))
	ಹಿಂತಿರುಗಿ; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    ಆದರೆ (win->w_ಬಫರ್ != ಖಾಲಿ)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	ಆದರೆ (!win_valid_any_tab(win))
	    ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	ಆದರೆ (!win_valid_any_tab(win))
	    ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (win->w_ಬಫರ್ != ಖಾಲಿ)
	// Close the link to the buffer.
	close_ಬಫರ್(win, win->w_ಬಫರ್, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    ಗಾಗಿ (ptp = ಮೊದಲ_tabpage; ptp != ಖಾಲಿ && ptp != tp; ptp = ptp->tp_ಮುಂದಿನ)
	;
    ಆದರೆ (ptp == ಖಾಲಿ || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	ಆದರೆ (win_valid_any_tab(win) && win->w_ಬಫರ್ == ಖಾಲಿ)
	{
	    win->w_ಬಫರ್ = firstbuf;
	    ++firstbuf->b_nwindows;
	    ಆದರೆ (win == curwin)
		curbuf = curwin->w_ಬಫರ್;
	    win_init_ಖಾಲಿಗಳು(win);
	}
	ಹಿಂತಿರುಗಿ;
    }

    // Autocommands may have closed the window already.
    ಗಾಗಿ (wp = tp->tp_firstwin; wp != ಖಾಲಿ && wp != win; wp = wp->w_ಮುಂದಿನ)
	;
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // When closing the last window in a tab page remove the tab page.
    ಆದರೆ (tp->tp_firstwin == tp->tp_lastwin)
    {
	ಪೂರ್ಣಾಂಕ	h = tabline_ಎತ್ತರ();

	ಆದರೆ (tp == ಮೊದಲ_tabpage)
	    ಮೊದಲ_tabpage = tp->tp_ಮುಂದಿನ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (ptp = ಮೊದಲ_tabpage; ptp != ಖಾಲಿ && ptp->tp_ಮುಂದಿನ != tp;
							   ptp = ptp->tp_ಮುಂದಿನ)
		;
	    ಆದರೆ (ptp == ಖಾಲಿ)
	    {
		ಆಂತರಿಕ_error("win_close_othertab()");
		ಹಿಂತಿರುಗಿ;
	    }
	    ptp->tp_ಮುಂದಿನ = tp->tp_ಮುಂದಿನ;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	ಆದರೆ (h != tabline_ಎತ್ತರ())
	    shell_new_ಅಡ್ಡಸಾಲುಗಳು();
    }

    ಆದರೆ (win->w_ಬಫರ್ != ಖಾಲಿ)
	--win->w_ಬಫರ್->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    ಆದರೆ (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    ಸ್ಥಿರ win_T *
win_free_mem(
    win_T	*win,
    ಪೂರ್ಣಾಂಕ		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ಚೌಕಟ್ಟು_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == ಖಾಲಿ ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_ಚೌಕಟ್ಟು;
    wp = winframe_remove(win, dirp, tp, ಖಾಲಿ);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    ಆದರೆ (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    ಹಿಂತಿರುಗಿ wp;
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
win_free_all(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = ಖಾಲಿ;
    cmdwin_win = ಖಾಲಿ;

    ಆಗ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ)
	tabpage_close(TRUE);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < AUCMD_WIN_ಎಣಿಕೆ; ++i)
	ಆದರೆ (aucmd_win[i].auc_win != ಖಾಲಿ)
	{
	    (ಶೂನ್ಯ)win_free_mem(aucmd_win[i].auc_win, &dummy, ಖಾಲಿ);
	    aucmd_win[i].auc_win = ಖಾಲಿ;
	}

    ಆಗ (firstwin != ಖಾಲಿ)
	(ಶೂನ್ಯ)win_free_mem(firstwin, &dummy, ಖಾಲಿ);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    ಪೂರ್ಣಾಂಕ		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    ಚೌಕಟ್ಟು_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    ಚೌಕಟ್ಟು_T	*frp, *frp2, *frp3;
    ಚೌಕಟ್ಟು_T	*frp_close = win->w_ಚೌಕಟ್ಟು;
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು;

    /*
     * If there is only one window there is nothing to remove.
     */
    ಆದರೆ (tp == ಖಾಲಿ ? ONE_ಕಿಟಕಿ : tp->tp_firstwin == tp->tp_lastwin)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಚೌಕಟ್ಟು_locked++;

    // Save the position of the containing frame (which will also contain the
    // altframe) before we remove anything, to recompute window positions later.
    wp = frame2win(frp_close->fr_ಪೋಷಕ);
    ಅಡ್ಡಸಾಲು = wp->w_winrow;
    ಲಂಬಸಾಲು = wp->w_wincol;

    /*
     * Remove the window from its frame.
     */
    frp2 = win_altframe(win, tp);
    wp = frame2win(frp2);

    // If this is a rightmost window, remove vertical separators to the left.
    ಆದರೆ (win->w_vsep_ಅಗಲ == 0 && frp_close->fr_ಪೋಷಕ->fr_layout == FR_ಅಡ್ಡಸಾಲು &&
	    frp_close->fr_prev != ಖಾಲಿ)
	ಚೌಕಟ್ಟು_ಗಣ_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    ಚೌಕಟ್ಟು_remove(frp_close);

    ಆದರೆ (frp_close->fr_ಪೋಷಕ->fr_layout == FR_ಲಂಬಸಾಲು)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	ಆದರೆ (frp2->fr_win != ಖಾಲಿ && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_ಮುಂದಿನ;
	    ಆಗ (frp != ಖಾಲಿ || frp3 != ಖಾಲಿ)
	    {
		ಆದರೆ (frp != ಖಾಲಿ)
		{
		    ಆದರೆ (!ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			ನಿಲ್ಲಿಸು;
		    }
		    frp = frp->fr_prev;
		}
		ಆದರೆ (frp3 != ಖಾಲಿ)
		{
		    ಆದರೆ (frp3->fr_win != ಖಾಲಿ && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			ನಿಲ್ಲಿಸು;
		    }
		    frp3 = frp3->fr_ಮುಂದಿನ;
		}
	    }
	}
	ಚೌಕಟ್ಟು_new_ಎತ್ತರ(frp2, frp2->fr_ಎತ್ತರ + frp_close->fr_ಎತ್ತರ,
			    frp2 == frp_close->fr_ಮುಂದಿನ, !ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(frp2), FALSE);
	*dirp = 'v';
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	ಆದರೆ (frp2->fr_win != ಖಾಲಿ && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_ಮುಂದಿನ;
	    ಆಗ (frp != ಖಾಲಿ || frp3 != ಖಾಲಿ)
	    {
		ಆದರೆ (frp != ಖಾಲಿ)
		{
		    ಆದರೆ (!ಚೌಕಟ್ಟು_fixed_ಅಗಲ(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			ನಿಲ್ಲಿಸು;
		    }
		    frp = frp->fr_prev;
		}
		ಆದರೆ (frp3 != ಖಾಲಿ)
		{
		    ಆದರೆ (frp3->fr_win != ಖಾಲಿ && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			ನಿಲ್ಲಿಸು;
		    }
		    frp3 = frp3->fr_ಮುಂದಿನ;
		}
	    }
	}
	ಚೌಕಟ್ಟು_new_ಅಗಲ(frp2, frp2->fr_ಅಗಲ + frp_close->fr_ಅಗಲ,
			    frp2 == frp_close->fr_ಮುಂದಿನ, !ಚೌಕಟ್ಟು_fixed_ಅಗಲ(frp2));
	*dirp = 'h';
    }

    // If the altframe wasn't adjacent and left/above, resizing it will have
    // changed window positions within the parent frame.  Recompute them.
    ಆದರೆ (frp2 != frp_close->fr_prev)
	ಚೌಕಟ್ಟು_comp_pos(frp_close->fr_ಪೋಷಕ, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು);

    ಆದರೆ (unflat_altfr == ಖಾಲಿ)
	ಚೌಕಟ್ಟು_flatten(frp2);
    ಇಲ್ಲದಿದ್ದರೆ
	*unflat_altfr = frp2;

    ಚೌಕಟ್ಟು_locked--;

    ಹಿಂತಿರುಗಿ wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_flatten(ಚೌಕಟ್ಟು_T *frp)
{
    ಚೌಕಟ್ಟು_T	*frp2, *frp3;

    ಆದರೆ (frp->fr_ಮುಂದಿನ != ಖಾಲಿ || frp->fr_prev != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_ಪೋಷಕ->fr_layout = frp->fr_layout;
    frp->fr_ಪೋಷಕ->fr_ಮಗು = frp->fr_ಮಗು;
    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp2, frp->fr_ಮಗು)
	frp2->fr_ಪೋಷಕ = frp->fr_ಪೋಷಕ;
    frp->fr_ಪೋಷಕ->fr_win = frp->fr_win;
    ಆದರೆ (frp->fr_win != ಖಾಲಿ)
	frp->fr_win->w_ಚೌಕಟ್ಟು = frp->fr_ಪೋಷಕ;
    frp2 = frp->fr_ಪೋಷಕ;
    ಆದರೆ (topframe->fr_ಮಗು == frp)
	topframe->fr_ಮಗು = frp2;
    vim_free(frp);

    frp = frp2->fr_ಪೋಷಕ;
    ಆದರೆ (frp != ಖಾಲಿ && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	ಆದರೆ (frp->fr_ಮಗು == frp2)
	    frp->fr_ಮಗು = frp2->fr_ಮಗು;
	frp2->fr_ಮಗು->fr_prev = frp2->fr_prev;
	ಆದರೆ (frp2->fr_prev != ಖಾಲಿ)
	    frp2->fr_prev->fr_ಮುಂದಿನ = frp2->fr_ಮಗು;
	ಗಾಗಿ (frp3 = frp2->fr_ಮಗು; ; frp3 = frp3->fr_ಮುಂದಿನ)
	{
	    frp3->fr_ಪೋಷಕ = frp;
	    ಆದರೆ (frp3->fr_ಮುಂದಿನ == ಖಾಲಿ)
	    {
		frp3->fr_ಮುಂದಿನ = frp2->fr_ಮುಂದಿನ;
		ಆದರೆ (frp2->fr_ಮುಂದಿನ != ಖಾಲಿ)
		    frp2->fr_ಮುಂದಿನ->fr_prev = frp3;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (topframe->fr_ಮಗು == frp2)
	    topframe->fr_ಮಗು = frp;
	vim_free(frp2);
    }
}

/*
 * Undo changes from a prior call to winframe_remove, also restoring lost
 * vertical separators and statuslines, and changed window positions for
 * windows within "unflat_altfr".
 * Caller must ensure no other changes were made to the layout or window sizes!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
winframe_restore(win_T *wp, ಪೂರ್ಣಾಂಕ dir, ಚೌಕಟ್ಟು_T *unflat_altfr)
{
    ಚೌಕಟ್ಟು_T	*frp = wp->w_ಚೌಕಟ್ಟು;

    // Put "wp"'s frame back where it was.
    ಆದರೆ (frp->fr_prev != ಖಾಲಿ)
	ಚೌಕಟ್ಟು_append(frp->fr_prev, frp);
    ಇಲ್ಲದಿದ್ದರೆ
	ಚೌಕಟ್ಟು_ಸೇರಿಸು(frp->fr_ಮುಂದಿನ, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    ಆದರೆ (wp->w_vsep_ಅಗಲ == 0
	    && frp->fr_ಪೋಷಕ->fr_layout == FR_ಅಡ್ಡಸಾಲು && frp->fr_prev != ಖಾಲಿ)
	ಚೌಕಟ್ಟು_ಗಣ_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0
	    && frp->fr_ಪೋಷಕ->fr_layout == FR_ಲಂಬಸಾಲು && frp->fr_prev != ಖಾಲಿ)
	ಚೌಕಟ್ಟು_ಕೂಡಿಸು_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    ಆದರೆ (dir == 'v')
    {
	ಚೌಕಟ್ಟು_new_ಎತ್ತರ(unflat_altfr, unflat_altfr->fr_ಎತ್ತರ - frp->fr_ಎತ್ತರ,
		unflat_altfr == frp->fr_ಮುಂದಿನ, FALSE, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir == 'h')
    {
	ಚೌಕಟ್ಟು_new_ಅಗಲ(unflat_altfr, unflat_altfr->fr_ಅಗಲ - frp->fr_ಅಗಲ,
		unflat_altfr == frp->fr_ಮುಂದಿನ, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    ಆದರೆ (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_ಪೋಷಕ);
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು = topleft->w_winrow;
	ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು = topleft->w_wincol;

	ಚೌಕಟ್ಟು_comp_pos(frp->fr_ಪೋಷಕ, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು);
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
    ಸ್ಥಿರ ಚೌಕಟ್ಟು_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ಚೌಕಟ್ಟು_T	*frp;
    ಚೌಕಟ್ಟು_T	*other_fr, *ಗುರಿ_fr;

    ಆದರೆ (tp == ಖಾಲಿ ? ONE_ಕಿಟಕಿ : tp->tp_firstwin == tp->tp_lastwin)
	ಹಿಂತಿರುಗಿ alt_tabpage()->tp_curwin->w_ಚೌಕಟ್ಟು;

    frp = win->w_ಚೌಕಟ್ಟು;

    ಆದರೆ (frp->fr_prev == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ frp->fr_ಮುಂದಿನ;
    ಆದರೆ (frp->fr_ಮುಂದಿನ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    //ಕಿಟಕಿ
    ಗುರಿ_fr = frp->fr_ಮುಂದಿನ;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    ಆದರೆ (frp->fr_ಪೋಷಕ != ಖಾಲಿ && frp->fr_ಪೋಷಕ->fr_layout == FR_ಲಂಬಸಾಲು && p_sb)
    {
	ಗುರಿ_fr = frp->fr_prev;
	other_fr  = frp->fr_ಮುಂದಿನ;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    ಆದರೆ (frp->fr_ಪೋಷಕ != ಖಾಲಿ && frp->fr_ಪೋಷಕ->fr_layout == FR_ಅಡ್ಡಸಾಲು && p_spr)
    {
	ಗುರಿ_fr = frp->fr_prev;
	other_fr  = frp->fr_ಮುಂದಿನ;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    ಆದರೆ (frp->fr_ಪೋಷಕ != ಖಾಲಿ && frp->fr_ಪೋಷಕ->fr_layout == FR_ಅಡ್ಡಸಾಲು)
    {
	ಆದರೆ (ಚೌಕಟ್ಟು_fixed_ಅಗಲ(ಗುರಿ_fr) && !ಚೌಕಟ್ಟು_fixed_ಅಗಲ(other_fr))
	    ಗುರಿ_fr = other_fr;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(ಗುರಿ_fr) && !ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(other_fr))
	    ಗುರಿ_fr = other_fr;
    }

    ಹಿಂತಿರುಗಿ ಗುರಿ_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    ಸ್ಥಿರ tabpage_T *
alt_tabpage(ಶೂನ್ಯ)
{
    tabpage_T	*tp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		forward;

    // Use the last accessed tab page, if possible.
    ಆದರೆ ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	ಹಿಂತಿರುಗಿ lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_ಮುಂದಿನ != ಖಾಲಿ &&
	    ((tcl_flags & TCL_ಎಡ) == 0 || curtab == ಮೊದಲ_tabpage);

    ಆದರೆ (forward)
	tp = curtab->tp_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	// Use the previous tab page.
	ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp->tp_ಮುಂದಿನ != curtab; tp = tp->tp_ಮುಂದಿನ)
	    ;

    ಹಿಂತಿರುಗಿ tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    ಸ್ಥಿರ win_T *
frame2win(ಚೌಕಟ್ಟು_T *frp)
{
    ಆಗ (frp->fr_win == ಖಾಲಿ)
	frp = frp->fr_ಮಗು;
    ಹಿಂತಿರುಗಿ frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಚೌಕಟ್ಟು_has_win(ಚೌಕಟ್ಟು_T *frp, win_T *wp)
{
    ಚೌಕಟ್ಟು_T	*p;

    ಆದರೆ (frp->fr_layout == FR_ಎಲೆ)
	ಹಿಂತಿರುಗಿ frp->fr_win == wp;

    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(p, frp->fr_ಮಗು)
	ಆದರೆ (ಚೌಕಟ್ಟು_has_win(p, wp))
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ min_ಗಣ_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_new_ಎತ್ತರ(
    ಚೌಕಟ್ಟು_T	*topfrp,
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ,
    ಪೂರ್ಣಾಂಕ		topfirst,	// resize topmost contained frame first
    ಪೂರ್ಣಾಂಕ		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    ಪೂರ್ಣಾಂಕ		ಗಣ_ch)		// set 'cmdheight' to resize topframe
{
    ಚೌಕಟ್ಟು_T	*frp;
    ಪೂರ್ಣಾಂಕ		extra_ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ		h;

    ಆದರೆ (topfrp->fr_ಪೋಷಕ == ಖಾಲಿ && ಗಣ_ch)
    {
	// topframe: update the command line height, with side effects.
	ಪೂರ್ಣಾಂಕ new_ch = MAX(min_ಗಣ_ch, p_ch + topfrp->fr_ಎತ್ತರ - ಎತ್ತರ);
	ಪೂರ್ಣಾಂಕ save_ch = min_ಗಣ_ch;
	ಆದರೆ (new_ch != p_ch)
	    ಗಣ_option_ಮೌಲ್ಯ((char_u *)"cmdheight", new_ch, ಖಾಲಿ, 0);
	min_ಗಣ_ch = save_ch;
	ಎತ್ತರ = MIN(ಎತ್ತರ, ಅಡ್ಡಸಾಲುಗಳು_AVAIL);
    }
    ಆದರೆ (topfrp->fr_win != ಖಾಲಿ)
    {
	// Simple case: just one window.
	win_new_ಎತ್ತರ(topfrp->fr_win,
				    ಎತ್ತರ - topfrp->fr_win->w_ಸ್ಥಿತಿ_ಎತ್ತರ
					      - WINBAR_ಎತ್ತರ(topfrp->fr_win));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (topfrp->fr_layout == FR_ಅಡ್ಡಸಾಲು)
    {
	ಮಾಡು
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, topfrp->fr_ಮಗು)
	    {
		ಚೌಕಟ್ಟು_new_ಎತ್ತರ(frp, ಎತ್ತರ, topfirst, wfh, ಗಣ_ch);
		ಆದರೆ (frp->fr_ಎತ್ತರ > ಎತ್ತರ)
		{
		    // Could not fit the windows, make the whole row higher.
		    ಎತ್ತರ = frp->fr_ಎತ್ತರ;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
	ಆಗ (frp != ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_ಮಗು;
	ಆದರೆ (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    ಆಗ (ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(frp))
	    {
		frp = frp->fr_ಮುಂದಿನ;
		ಆದರೆ (frp == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ;	    // no frame without 'wfh', give up
	    }
	ಆದರೆ (!topfirst)
	{
	    // Find the bottom frame of this column
	    ಆಗ (frp->fr_ಮುಂದಿನ != ಖಾಲಿ)
		frp = frp->fr_ಮುಂದಿನ;
	    ಆದರೆ (wfh)
		// Advance back for frames with one window with 'wfh' set.
		ಆಗ (ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(frp))
		    frp = frp->fr_prev;
	}

	extra_ಸಾಲುಗಳು = ಎತ್ತರ - topfrp->fr_ಎತ್ತರ;
	ಆದರೆ (extra_ಸಾಲುಗಳು < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    ಆಗ (frp != ಖಾಲಿ)
	    {
		h = ಚೌಕಟ್ಟು_minheight(frp, ಖಾಲಿ);
		ಆದರೆ (frp->fr_ಎತ್ತರ + extra_ಸಾಲುಗಳು < h)
		{
		    extra_ಸಾಲುಗಳು += frp->fr_ಎತ್ತರ - h;
		    ಚೌಕಟ್ಟು_new_ಎತ್ತರ(frp, h, topfirst, wfh, ಗಣ_ch);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಚೌಕಟ್ಟು_new_ಎತ್ತರ(frp, frp->fr_ಎತ್ತರ + extra_ಸಾಲುಗಳು,
							topfirst, wfh, ಗಣ_ch);
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (topfirst)
		{
		    ಮಾಡು
			frp = frp->fr_ಮುಂದಿನ;
		    ಆಗ (wfh && frp != ಖಾಲಿ && ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(frp));
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಮಾಡು
			frp = frp->fr_prev;
		    ಆಗ (wfh && frp != ಖಾಲಿ && ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		ಆದರೆ (frp == ಖಾಲಿ)
		    ಎತ್ತರ -= extra_ಸಾಲುಗಳು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (extra_ಸಾಲುಗಳು > 0)
	{
	    // increase height of bottom or top frame
	    ಚೌಕಟ್ಟು_new_ಎತ್ತರ(frp, frp->fr_ಎತ್ತರ + extra_ಸಾಲುಗಳು, topfirst, wfh,
									ಗಣ_ch);
	}
    }
    topfrp->fr_ಎತ್ತರ = ಎತ್ತರ;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(ಚೌಕಟ್ಟು_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    ಆದರೆ (frp->fr_win != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ frp->fr_win->w_p_wfh;

    ಆದರೆ (frp->fr_layout == FR_ಅಡ್ಡಸಾಲು)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, frp->fr_ಮಗು)
	    ಆದರೆ (ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(frp))
		ಹಿಂತಿರುಗಿ TRUE;
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, frp->fr_ಮಗು)
	ಆದರೆ (!ಚೌಕಟ್ಟು_fixed_ಎತ್ತರ(frp))
	    ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಚೌಕಟ್ಟು_fixed_ಅಗಲ(ಚೌಕಟ್ಟು_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    ಆದರೆ (frp->fr_win != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ frp->fr_win->w_p_wfw;

    ಆದರೆ (frp->fr_layout == FR_ಲಂಬಸಾಲು)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, frp->fr_ಮಗು)
	    ಆದರೆ (ಚೌಕಟ್ಟು_fixed_ಅಗಲ(frp))
		ಹಿಂತಿರುಗಿ TRUE;
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, frp->fr_ಮಗು)
	ಆದರೆ (!ಚೌಕಟ್ಟು_fixed_ಅಗಲ(frp))
	    ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_ಕೂಡಿಸು_statusline(ಚೌಕಟ್ಟು_T *frp)
{
    win_T	*wp;

    ಆದರೆ (frp->fr_layout == FR_ಎಲೆ)
    {
	wp = frp->fr_win;
	wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = statusline_ಎತ್ತರ(wp);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (frp->fr_layout == FR_ಅಡ್ಡಸಾಲು)
    {
	// Handle all the frames in the row.
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, frp->fr_ಮಗು)
	    ಚೌಕಟ್ಟು_ಕೂಡಿಸು_statusline(frp);
    }
    ಇಲ್ಲದಿದ್ದರೆ // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	ಗಾಗಿ (frp = frp->fr_ಮಗು; frp->fr_ಮುಂದಿನ != ಖಾಲಿ; frp = frp->fr_ಮುಂದಿನ)
	    ;
	ಚೌಕಟ್ಟು_ಕೂಡಿಸು_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_new_ಅಗಲ(
    ಚೌಕಟ್ಟು_T	*topfrp,
    ಪೂರ್ಣಾಂಕ		ಅಗಲ,
    ಪೂರ್ಣಾಂಕ		leftfirst,	// resize leftmost contained frame first
    ಪೂರ್ಣಾಂಕ		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    ಚೌಕಟ್ಟು_T	*frp;
    ಪೂರ್ಣಾಂಕ		extra_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಪೂರ್ಣಾಂಕ		w;
    win_T	*wp;

    ಆದರೆ (topfrp->fr_layout == FR_ಎಲೆ)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	ಗಾಗಿ (frp = topfrp; frp->fr_ಪೋಷಕ != ಖಾಲಿ; frp = frp->fr_ಪೋಷಕ)
	    ಆದರೆ (frp->fr_ಪೋಷಕ->fr_layout == FR_ಅಡ್ಡಸಾಲು && frp->fr_ಮುಂದಿನ != ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (frp->fr_ಪೋಷಕ == ಖಾಲಿ)
	    wp->w_vsep_ಅಗಲ = 0;
	win_new_ಅಗಲ(wp, ಅಗಲ - wp->w_vsep_ಅಗಲ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (topfrp->fr_layout == FR_ಲಂಬಸಾಲು)
    {
	ಮಾಡು
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, topfrp->fr_ಮಗು)
	    {
		ಚೌಕಟ್ಟು_new_ಅಗಲ(frp, ಅಗಲ, leftfirst, wfw);
		ಆದರೆ (frp->fr_ಅಗಲ > ಅಗಲ)
		{
		    // Could not fit the windows, make whole column wider.
		    ಅಗಲ = frp->fr_ಅಗಲ;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	} ಆಗ (frp != ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_ಮಗು;
	ಆದರೆ (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    ಆಗ (ಚೌಕಟ್ಟು_fixed_ಅಗಲ(frp))
	    {
		frp = frp->fr_ಮುಂದಿನ;
		ಆದರೆ (frp == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ;	    // no frame without 'wfw', give up
	    }
	ಆದರೆ (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    ಆಗ (frp->fr_ಮುಂದಿನ != ಖಾಲಿ)
		frp = frp->fr_ಮುಂದಿನ;
	    ಆದರೆ (wfw)
		// Advance back for frames with one window with 'wfw' set.
		ಆಗ (ಚೌಕಟ್ಟು_fixed_ಅಗಲ(frp))
		    frp = frp->fr_prev;
	}

	extra_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಅಗಲ - topfrp->fr_ಅಗಲ;
	ಆದರೆ (extra_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < 0)
	{
	    // reduce frame width, rightmost frame first
	    ಆಗ (frp != ಖಾಲಿ)
	    {
		w = ಚೌಕಟ್ಟು_minwidth(frp, ಖಾಲಿ);
		ಆದರೆ (frp->fr_ಅಗಲ + extra_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < w)
		{
		    extra_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ += frp->fr_ಅಗಲ - w;
		    ಚೌಕಟ್ಟು_new_ಅಗಲ(frp, w, leftfirst, wfw);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಚೌಕಟ್ಟು_new_ಅಗಲ(frp, frp->fr_ಅಗಲ + extra_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
							      leftfirst, wfw);
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (leftfirst)
		{
		    ಮಾಡು
			frp = frp->fr_ಮುಂದಿನ;
		    ಆಗ (wfw && frp != ಖಾಲಿ && ಚೌಕಟ್ಟು_fixed_ಅಗಲ(frp));
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಮಾಡು
			frp = frp->fr_prev;
		    ಆಗ (wfw && frp != ಖಾಲಿ && ಚೌಕಟ್ಟು_fixed_ಅಗಲ(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		ಆದರೆ (frp == ಖಾಲಿ)
		    ಅಗಲ -= extra_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (extra_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > 0)
	{
	    // increase width of rightmost frame
	    ಚೌಕಟ್ಟು_new_ಅಗಲ(frp, frp->fr_ಅಗಲ + extra_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, leftfirst, wfw);
	}
    }
    topfrp->fr_ಅಗಲ = ಅಗಲ;
}

/*
 * Add or remove the vertical separator of windows to the right side of "frp".
 * Note: Does not check if there is room!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_ಗಣ_vsep(ಚೌಕಟ್ಟು_T *frp, ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು)
{
    win_T	*wp;

    ಆದರೆ (frp->fr_layout == FR_ಎಲೆ)
    {
	wp = frp->fr_win;
	ಆದರೆ (ಕೂಡಿಸು && wp->w_vsep_ಅಗಲ == 0)
	{
	    ಆದರೆ (wp->w_ಅಗಲ > 0)	// don't make it negative
		win_new_ಅಗಲ(wp, wp->w_ಅಗಲ - 1);
	    wp->w_vsep_ಅಗಲ = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಕೂಡಿಸು && wp->w_vsep_ಅಗಲ == 1)
	{
	    win_new_ಅಗಲ(wp, wp->w_ಅಗಲ + 1);
	    wp->w_vsep_ಅಗಲ = 0;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (frp->fr_layout == FR_ಲಂಬಸಾಲು)
    {
	// Handle all the frames in the column.
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, frp->fr_ಮಗು)
	    ಚೌಕಟ್ಟು_ಗಣ_vsep(frp, ಕೂಡಿಸು);
    }
    ಇಲ್ಲದಿದ್ದರೆ // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_ಮಗು;
	ಆಗ (frp->fr_ಮುಂದಿನ != ಖಾಲಿ)
	    frp = frp->fr_ಮುಂದಿನ;
	ಚೌಕಟ್ಟು_ಗಣ_vsep(frp, ಕೂಡಿಸು);
    }
}

/*
 * Set frame width from the window it contains.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_fix_ಅಗಲ(win_T *wp)
{
    wp->w_ಚೌಕಟ್ಟು->fr_ಅಗಲ = wp->w_ಅಗಲ + wp->w_vsep_ಅಗಲ;
}

/*
 * Set frame height from the window it contains.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_fix_ಎತ್ತರ(win_T *wp)
{
    wp->w_ಚೌಕಟ್ಟು->fr_ಎತ್ತರ = VISIBLE_ಎತ್ತರ(wp) + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
}

/*
 * Compute the minimal height for frame "topfrp".
 * Uses the 'winminheight' option.
 * When "next_curwin" isn't NULL, use p_wh for this window.
 * When "next_curwin" is NOWIN, don't use at least one line for the current
 * window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಚೌಕಟ್ಟು_minheight(ಚೌಕಟ್ಟು_T *topfrp, win_T *ಮುಂದಿನ_curwin)
{
    ಚೌಕಟ್ಟು_T	*frp;
    ಪೂರ್ಣಾಂಕ		m;
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (topfrp->fr_win != ಖಾಲಿ)
    {
	ಆದರೆ (topfrp->fr_win == ಮುಂದಿನ_curwin)
	    m = p_wh + topfrp->fr_win->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	    ಆದರೆ (topfrp->fr_win == curwin && ಮುಂದಿನ_curwin == ಖಾಲಿ)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		ಆದರೆ (p_wmh == 0)
		    ++m;
		m += WINBAR_ಎತ್ತರ(curwin);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (topfrp->fr_layout == FR_ಅಡ್ಡಸಾಲು)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, topfrp->fr_ಮಗು)
	{
	    n = ಚೌಕಟ್ಟು_minheight(frp, ಮುಂದಿನ_curwin);
	    ಆದರೆ (n > m)
		m = n;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, topfrp->fr_ಮಗು)
	    m += ಚೌಕಟ್ಟು_minheight(frp, ಮುಂದಿನ_curwin);
    }

    ಹಿಂತಿರುಗಿ m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಚೌಕಟ್ಟು_wincount_ಒಳ_ಎತ್ತರ(ಚೌಕಟ್ಟು_T *topfrp)
{
    ಚೌಕಟ್ಟು_T	*frp;
    ಪೂರ್ಣಾಂಕ		c = 0;
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (topfrp->fr_win != ಖಾಲಿ)
	c = 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (topfrp->fr_layout == FR_ಅಡ್ಡಸಾಲು)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, topfrp->fr_ಮಗು)
	{
	    n = ಚೌಕಟ್ಟು_wincount_ಒಳ_ಎತ್ತರ(frp);
	    ಆದರೆ (n > c)
		c = n;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, topfrp->fr_ಮಗು)
	    c += ಚೌಕಟ್ಟು_wincount_ಒಳ_ಎತ್ತರ(frp);
    }

    ಹಿಂತಿರುಗಿ c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಚೌಕಟ್ಟು_minwidth(
    ಚೌಕಟ್ಟು_T	*topfrp,
    win_T	*ಮುಂದಿನ_curwin)	// use p_wh and p_wiw for next_curwin
{
    ಚೌಕಟ್ಟು_T	*frp;
    ಪೂರ್ಣಾಂಕ		m, n;

    ಆದರೆ (topfrp->fr_win != ಖಾಲಿ)
    {
	ಆದರೆ (topfrp->fr_win == ಮುಂದಿನ_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_ಅಗಲ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_ಅಗಲ;
	    // Current window is minimal one column wide
	    ಆದರೆ (p_wmw == 0 && topfrp->fr_win == curwin && ಮುಂದಿನ_curwin == ಖಾಲಿ)
		++m;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (topfrp->fr_layout == FR_ಲಂಬಸಾಲು)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, topfrp->fr_ಮಗು)
	{
	    n = ಚೌಕಟ್ಟು_minwidth(frp, ಮುಂದಿನ_curwin);
	    ಆದರೆ (n > m)
		m = n;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, topfrp->fr_ಮಗು)
	    m += ಚೌಕಟ್ಟು_minwidth(frp, ಮುಂದಿನ_curwin);
    }

    ಹಿಂತಿರುಗಿ m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    ಶೂನ್ಯ
close_others(
    ಪೂರ್ಣಾಂಕ		ಸಂದೇಶ_2,
    ಪೂರ್ಣಾಂಕ		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    ಪೂರ್ಣಾಂಕ		r;

    ಆದರೆ (one_ಕಿಟಕಿ())
    {
	ಆದರೆ (ಸಂದೇಶ_2 && !autocmd_busy)
	    ಸಂದೇಶ(_(m_onlyone));
	ಹಿಂತಿರುಗಿ;
    }

    // Be very careful here: autocommands may change the window layout.
    ಗಾಗಿ (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_ಮುಂದಿನ;

	// autocommands messed this one up
	ಆದರೆ (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_ಬಫರ್;
	}

	ಆದರೆ (wp == curwin)		// don't close current window
	    ಮುಂದುವರಿ;

	// autoccommands messed this one up
	ಆದರೆ (!buf_valid(wp->w_ಬಫರ್) && win_valid(wp))
	{
	    wp->w_ಬಫರ್ = ಖಾಲಿ;
	    win_close(wp, 0);
	    ಮುಂದುವರಿ;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_ಬಫರ್, forceit);
	ಆದರೆ (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (!r)
	{
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ಆದರೆ (ಸಂದೇಶ_2 && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_ಬದಲಾಯಿತು(wp->w_ಬಫರ್, FALSE);
		ಆದರೆ (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    ಮುಂದುವರಿ;
		}
	    }
	    ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(wp->w_ಬಫರ್))
#ಆದರೆ_ಅಂತ್ಯ
		ಮುಂದುವರಿ;
	}
	win_close(wp, !buf_hide(wp->w_ಬಫರ್) && !buf_Is_ಬದಲಾಯಿತು(wp->w_ಬಫರ್));
    }

    ಆದರೆ (ಸಂದೇಶ_2 && !ONE_ಕಿಟಕಿ)
	emsg(_(e_other_ಕಿಟಕಿ_ಒಳಗೊಂಡಿದೆ_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    ಶೂನ್ಯ
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ command_ಚೌಕಟ್ಟು_ಎತ್ತರ = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    ಶೂನ್ಯ
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
    ಪೂರ್ಣಾಂಕ
win_alloc_ಮೊದಲ(ಶೂನ್ಯ)
{
    ಆದರೆ (win_alloc_firstwin(ಖಾಲಿ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಮೊದಲ_tabpage = alloc_tabpage();
    ಆದರೆ (ಮೊದಲ_tabpage == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    curtab = ಮೊದಲ_tabpage;
    unuse_tabpage(ಮೊದಲ_tabpage);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(ಶೂನ್ಯ)
{
    win_T *wp;

    wp = win_alloc(ಖಾಲಿ, TRUE);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_ಚೌಕಟ್ಟು(wp);
    ಹಿಂತಿರುಗಿ wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    ಶೂನ್ಯ
win_init_popup_win(win_T *wp, buf_T *buf)
{
    wp->w_ಬಫರ್ = buf;
    ++buf->b_nwindows;
    win_init_ಖಾಲಿಗಳು(wp); // set cursor and topline to safe values

    // Make sure w_localdir is NULL to avoid a chdir() in win_enter_ext().
    VIM_CLEAR(wp->w_localdir);
}

/*
 * Allocate the first window or the first window in a new tab page.
 * When "oldwin" is NULL create an empty buffer for it.
 * When "oldwin" is not NULL copy info from it to the new window.
 * Return FAIL when something goes wrong (out of memory).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(ಖಾಲಿ, FALSE);
    ಆದರೆ (curwin == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (oldwin == ಖಾಲಿ)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(ಖಾಲಿ, ಖಾಲಿ, 1L, BLN_LISTED);
	ಆದರೆ (curwin == ಖಾಲಿ || curbuf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	curwin->w_ಬಫರ್ = curbuf;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#ಆದರೆ_ಅಂತ್ಯ
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_ಚೌಕಟ್ಟು(curwin);
    ಆದರೆ (curwin->w_ಚೌಕಟ್ಟು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    topframe = curwin->w_ಚೌಕಟ್ಟು;
    topframe->fr_ಅಗಲ = COLUMNS_WITHOUT_TPL();
    topframe->fr_ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು - p_ch;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Create a frame for window "wp".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
new_ಚೌಕಟ್ಟು(win_T *wp)
{
    ಚೌಕಟ್ಟು_T *frp = ALLOC_CLEAR_ONE(ಚೌಕಟ್ಟು_T);

    wp->w_ಚೌಕಟ್ಟು = frp;
    ಆದರೆ (frp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    frp->fr_layout = FR_ಎಲೆ;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    ಶೂನ್ಯ
win_init_ಗಾತ್ರ(ಶೂನ್ಯ)
{
    firstwin->w_ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು_AVAIL;
    firstwin->w_prev_ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು_AVAIL;
    topframe->fr_ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು_AVAIL;
    firstwin->w_ಅಗಲ = topframe->fr_ಅಗಲ;
}

/*
 * Allocate a new tabpage_T and init the values.
 * Returns NULL when out of memory.
 */
    ಸ್ಥಿರ tabpage_T *
alloc_tabpage(ಶೂನ್ಯ)
{
    tabpage_T	*tp;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಪೂರ್ಣಾಂಕ		i;
#ಆದರೆ_ಅಂತ್ಯ


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    ಆದರೆ (tp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    ಆದರೆ (tp->tp_vars == ಖಾಲಿ)
    {
	vim_free(tp);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಗಾಗಿ (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    tp->tp_ch_ಬಳಸಿದ = p_ch;

    ಹಿಂತಿರುಗಿ tp;
}

    ಶೂನ್ಯ
free_tabpage(tabpage_T *tp)
{
    ಪೂರ್ಣಾಂಕ idx;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    diff_clear(tp);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆಗ (tp->tp_ಮೊದಲ_popupwin != ಖಾಲಿ)
	popup_close_tabpage(tp, tp->tp_ಮೊದಲ_popupwin->w_id, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಾಗಿ (idx = 0; idx < SNAP_ಎಣಿಕೆ; ++idx)
	clear_snapshot(tp, idx);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (tp == lastused_tabpage)
	lastused_tabpage = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TABPANEL
    tabpanel_forget_tabpage(tp);
#ಆದರೆ_ಅಂತ್ಯ

    vim_free(tp->tp_localdir);
    vim_free(tp->tp_prevdir);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
    python_tabpage_free(tp);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
    python3_tabpage_free(tp);
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
win_new_tabpage(ಪೂರ್ಣಾಂಕ after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    ಪೂರ್ಣಾಂಕ		n;
#ಆದರೆ defined(FEAT_TABPANEL)
    ಪೂರ್ಣಾಂಕ prev_columns = COLUMNS_WITHOUT_TPL();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (cmdwin_type != 0)
    {
	emsg(_(e_invalid_ಒಳ_cmdline_ಕಿಟಕಿ));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_tabnew))
	ಹಿಂತಿರುಗಿ FAIL;

    newtp = alloc_tabpage();
    ಆದರೆ (newtp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // Remember the current windows in this Tab page.
    ಆದರೆ (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == ಖಾಲಿ)
				    ? ಖಾಲಿ : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    ಆದರೆ (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	ಆದರೆ (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_ಮುಂದಿನ = ಮೊದಲ_tabpage;
	    ಮೊದಲ_tabpage = newtp;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp->tp_ಮುಂದಿನ != ಖಾಲಿ
					       && n < after; tp = tp->tp_ಮುಂದಿನ)
		    ++n;
	    }
	    newtp->tp_ಮುಂದಿನ = tp->tp_ಮುಂದಿನ;
	    tp->tp_ಮುಂದಿನ = newtp;
	}
	newtp->tp_firstwin = newtp->tp_lastwin = newtp->tp_curwin = curwin;

	win_init_ಗಾತ್ರ();
	firstwin->w_winrow = tabline_ಎತ್ತರ();
	firstwin->w_prev_winrow = firstwin->w_winrow;
	win_comp_scroll(curwin);

	newtp->tp_topframe = topframe;
	ಕೊನೆಯ_ಸ್ಥಿತಿ(FALSE);

	lastused_tabpage = prev_tp;

#ಆದರೆ defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	entering_ಕಿಟಕಿ(curwin);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TABPANEL)
	ಆದರೆ (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    ಪೂರ್ಣಾಂಕ		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		ಆದರೆ (tp2 != curtab)
		{
		    ಚೌಕಟ್ಟು_new_ಅಗಲ(tp2->tp_topframe, w, FALSE, TRUE);
		    ಆದರೆ (!ಚೌಕಟ್ಟು_check_ಅಗಲ(tp2->tp_topframe, w))
			ಚೌಕಟ್ಟು_new_ಅಗಲ(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#ಆದರೆ_ಅಂತ್ಯ
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	ಹಿಂತಿರುಗಿ OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
may_open_tabpage(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		n = (cmdmod.cmod_tab == 0)
				       ? postponed_ವಿಭಜಿಸು_tab : cmdmod.cmod_tab;

    ಆದರೆ (n == 0)
	ಹಿಂತಿರುಗಿ FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_ವಿಭಜಿಸು_tab = 0;
    ಹಿಂತಿರುಗಿ win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    ಪೂರ್ಣಾಂಕ
make_tabpages(ಪೂರ್ಣಾಂಕ maxcount)
{
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = maxcount;
    ಪೂರ್ಣಾಂಕ		todo;

    // Limit to 'tabpagemax' tabs.
    ಆದರೆ (ಎಣಿಕೆ_2 > p_tpm)
	ಎಣಿಕೆ_2 = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    ಖಂಡ_autocmds();

    ಗಾಗಿ (todo = ಎಣಿಕೆ_2 - 1; todo > 0; --todo)
	ಆದರೆ (win_new_tabpage(0) == FAIL)
	    ನಿಲ್ಲಿಸು;

    unblock_autocmds();

    // return actual number of tab pages
    ಹಿಂತಿರುಗಿ (ಎಣಿಕೆ_2 - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    ಪೂರ್ಣಾಂಕ
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	ಆದರೆ (tp == tpc)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    ಪೂರ್ಣಾಂಕ
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	ಆದರೆ (tp == tpc)
	{
	    FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp)
	    {
		ಆದರೆ (win_valid_any_tab(wp))
		    ಹಿಂತಿರುಗಿ TRUE;
	    }
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }
    // shouldn't happen
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    ಶೂನ್ಯ
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    ಆದರೆ (tab == ಮೊದಲ_tabpage)
    {
	ಮೊದಲ_tabpage = tab->tp_ಮುಂದಿನ;
	ptp = ಮೊದಲ_tabpage;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (ptp = ಮೊದಲ_tabpage; ptp != ಖಾಲಿ && ptp->tp_ಮುಂದಿನ != tab;
							    ptp = ptp->tp_ಮುಂದಿನ)
	    ;
	ಪ್ರತಿಜ್ಞಾ(ptp != ಖಾಲಿ);
	ptp->tp_ಮುಂದಿನ = tab->tp_ಮುಂದಿನ;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
ಕಂಡುಹಿಡಿ_tabpage(ಪೂರ್ಣಾಂಕ n)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ		i = 1;

    ಆದರೆ (n == 0)
	ಹಿಂತಿರುಗಿ curtab;

    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ && i != n; tp = tp->tp_ಮುಂದಿನ)
	++i;
    ಹಿಂತಿರುಗಿ tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    ಪೂರ್ಣಾಂಕ
tabpage_ಸೂಚ್ಯಂಕ(tabpage_T *ftp)
{
    ಪೂರ್ಣಾಂಕ		i = 1;
    tabpage_T	*tp;

    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ && tp != ftp; tp = tp->tp_ಮುಂದಿನ)
	++i;
    ಹಿಂತಿರುಗಿ i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    ಪೂರ್ಣಾಂಕ		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    leaving_ಕಿಟಕಿ(curwin);
#ಆದರೆ_ಅಂತ್ಯ
    reset_VIsual_and_resel();	// stop Visual mode
    ಆದರೆ (trigger_leave_autocmds)
    {
	ಆದರೆ (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	    ಆದರೆ (curtab != tp)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	ಆದರೆ (curtab != tp)
	    ಹಿಂತಿರುಗಿ FAIL;
	apply_autocmds(EVENT_TABLEAVE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	ಆದರೆ (curtab != tp)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    reset_dragwin();
#ಆದರೆ defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    ಆದರೆ (gui.ಒಳ_use)
	gui_remove_scrollbars();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
    // Clear all image placements, they will be redrawn again if needed.
    clear_all_image_placements();
#ಆದರೆ_ಅಂತ್ಯ
    tp->tp_curwin = curwin;
    tp->tp_prevwin = prevwin;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_old_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು_AVAIL;
    ಆದರೆ (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_ಅಗಲ;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = ಖಾಲಿ;
    lastwin = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    ಪೂರ್ಣಾಂಕ		trigger_enter_autocmds,
    ಪೂರ್ಣಾಂಕ		trigger_leave_autocmds)
{
    ಪೂರ್ಣಾಂಕ		old_off = tp->tp_firstwin->w_winrow;
    win_T	*ಮುಂದಿನ_prevwin = tp->tp_prevwin;
    tabpage_T	*ಕೊನೆಯ_tab = curtab;

    use_tabpage(tp);

    ಆದರೆ (p_ch != curtab->tp_ch_ಬಳಸಿದ)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	ಪೂರ್ಣಾಂಕ new_ch = curtab->tp_ch_ಬಳಸಿದ;
	curtab->tp_ch_ಬಳಸಿದ = p_ch;
	command_ಚೌಕಟ್ಟು_ಎತ್ತರ = FALSE;
	ಗಣ_option_ಮೌಲ್ಯ((char_u *)"cmdheight", new_ch, ಖಾಲಿ, 0);
	command_ಚೌಕಟ್ಟು_ಎತ್ತರ = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (ಶೂನ್ಯ)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
		  | (trigger_enter_autocmds ? WEE_TRIGGER_ENTER_AUTOCMDS : 0)
		  | (trigger_leave_autocmds ? WEE_TRIGGER_LEAVE_AUTOCMDS : 0));
    prevwin = ಮುಂದಿನ_prevwin;

    ಕೊನೆಯ_ಸ್ಥಿತಿ(FALSE);		// status line may appear or disappear
    win_comp_pos();		// recompute w_winrow for all windows
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    diff_need_scrollbind = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    // If there was a click in a window, it won't be usable for a following
    // drag.
    reset_dragwin();

    // The tabpage line may have appeared or disappeared, may need to resize
    // the frames for that.  When the Vim window was resized need to update
    // frame sizes too.
    ಆದರೆ (curtab->tp_old_ಅಡ್ಡಸಾಲುಗಳು != ಅಡ್ಡಸಾಲುಗಳು_AVAIL || (old_off != firstwin->w_winrow
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#ಆದರೆ_ಅಂತ್ಯ
		))
	shell_new_ಅಡ್ಡಸಾಲುಗಳು();
    ಆದರೆ (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	ಆದರೆ (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_ಅಗಲ;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = ಕೊನೆಯ_tab;

#ಆದರೆ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#ಆದರೆ_ಅಂತ್ಯ

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    ಆದರೆ (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	ಆದರೆ (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    ಶೂನ್ಯ
goto_tabpage(ಪೂರ್ಣಾಂಕ n)
{
    tabpage_T	*tp = ಖಾಲಿ;  // shut up compiler
    tabpage_T	*ttp;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (ಪಠ್ಯ_locked())
    {
	// Not allowed when editing the command line.
	ಪಠ್ಯ_locked_ಸಂದೇಶ();
	ಹಿಂತಿರುಗಿ;
    }

    // If there is only one it can't work.
    ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ == ಖಾಲಿ)
    {
	ಆದರೆ (n > 1)
	    beep_flush();
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	ಆದರೆ (curtab->tp_ಮುಂದಿನ == ಖಾಲಿ)
	    tp = ಮೊದಲ_tabpage;
	ಇಲ್ಲದಿದ್ದರೆ
	    tp = curtab->tp_ಮುಂದಿನ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	ಗಾಗಿ (i = n; i < 0; ++i)
	{
	    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp->tp_ಮುಂದಿನ != ttp && tp->tp_ಮುಂದಿನ != ಖಾಲಿ;
		    tp = tp->tp_ಮುಂದಿನ)
		;
	    ttp = tp;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n == 9999)
    {
	// Go to last tab page.
	ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp->tp_ಮುಂದಿನ != ಖಾಲಿ; tp = tp->tp_ಮುಂದಿನ)
	    ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Go to tab page "n".
	tp = ಕಂಡುಹಿಡಿ_tabpage(n);
	ಆದರೆ (tp == ಖಾಲಿ)
	{
	    beep_flush();
	    ಹಿಂತಿರುಗಿ;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    ಆದರೆ (gui_use_tabline())
	gui_mch_ಗಣ_curtab(tabpage_ಸೂಚ್ಯಂಕ(curtab));
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    ಶೂನ್ಯ
goto_tabpage_tp(
    tabpage_T	*tp,
    ಪೂರ್ಣಾಂಕ		trigger_enter_autocmds,
    ಪೂರ್ಣಾಂಕ		trigger_leave_autocmds)
{
    ಆದರೆ (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    ಗಣ_keep_ಸಂದೇಶ(ಖಾಲಿ, 0);

    skip_win_fix_scroll = TRUE;
    ಆದರೆ (tp != curtab && leave_tabpage(tp->tp_curwin->w_ಬಫರ್,
					trigger_leave_autocmds) == OK)
    {
	ಆದರೆ (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	ಇಲ್ಲದಿದ್ದರೆ
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    ಪೂರ್ಣಾಂಕ
goto_tabpage_lastused(ಶೂನ್ಯ)
{
    ಆದರೆ (!valid_tabpage(lastused_tabpage))
	ಹಿಂತಿರುಗಿ FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    ಶೂನ್ಯ
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    ಆದರೆ (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
	ಆದರೆ (gui_use_tabline())
	    gui_mch_ಗಣ_curtab(tabpage_ಸೂಚ್ಯಂಕ(curtab));
#ಆದರೆ_ಅಂತ್ಯ
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    ಶೂನ್ಯ
tabpage_move(ಪೂರ್ಣಾಂಕ nr)
{
    ಪೂರ್ಣಾಂಕ		n = 1;
    tabpage_T	*tp, *tp_dst;

    ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (tabpage_move_disallowed)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp->tp_ಮುಂದಿನ != ಖಾಲಿ && n < nr; tp = tp->tp_ಮುಂದಿನ)
	++n;

    ಆದರೆ (tp == curtab || (nr > 0 && tp->tp_ಮುಂದಿನ != ಖಾಲಿ
						    && tp->tp_ಮುಂದಿನ == curtab))
	ಹಿಂತಿರುಗಿ;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    ಆದರೆ (curtab == ಮೊದಲ_tabpage)
	ಮೊದಲ_tabpage = curtab->tp_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	FOR_ALL_TABPAGES(tp)
	    ಆದರೆ (tp->tp_ಮುಂದಿನ == curtab)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (tp == ಖಾಲಿ)	// "cannot happen"
	    ಹಿಂತಿರುಗಿ;
	tp->tp_ಮುಂದಿನ = curtab->tp_ಮುಂದಿನ;
    }

    // Re-insert it at the specified position.
    ಆದರೆ (nr <= 0)
    {
	curtab->tp_ಮುಂದಿನ = ಮೊದಲ_tabpage;
	ಮೊದಲ_tabpage = curtab;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	curtab->tp_ಮುಂದಿನ = tp_dst->tp_ಮುಂದಿನ;
	tp_dst->tp_ಮುಂದಿನ = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
}


/*
 * Go to another window.
 * When jumping to another buffer, stop Visual mode.  Do this before
 * changing windows so we can yank the selection into the '*' register.
 * (note: this may trigger ModeChanged autocommand!)
 * When jumping to another window on the same buffer, adjust its cursor
 * position to keep the same Visual area.
 */
    ಶೂನ್ಯ
win_goto(win_T *wp)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    win_T	*owp = curwin;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_ಗಮ್ಯ_enter_popup_ಕಿಟಕಿ));
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಪಠ್ಯ_or_buf_locked())
    {
	beep_flush();
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (wp->w_ಬಫರ್ != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    ಆದರೆ (!win_valid(wp))
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    win_enter(wp, TRUE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    ಆದರೆ (win_valid(owp) && owp->w_p_cole > 0 && !ಸಂದೇಶ_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    ಆದರೆ (curwin->w_p_cole > 0 && !ಸಂದೇಶ_scrolled)
	need_cursor_ಸಾಲು_redraw = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_ಕಂಡುಹಿಡಿ_nr(ಪೂರ್ಣಾಂಕ winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (--winnr == 0)
	    ನಿಲ್ಲಿಸು;
    ಹಿಂತಿರುಗಿ wp;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_ಕಂಡುಹಿಡಿ_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಆದರೆ (wp == win)
		ಹಿಂತಿರುಗಿ tp;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, ಪೂರ್ಣಾಂಕ ಮೇಲೆ, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    ಚೌಕಟ್ಟು_T	*fr;
    ಚೌಕಟ್ಟು_T	*nfr;
    ಚೌಕಟ್ಟು_T	*foundfr;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (popup_is_popup(wp))
	// popups don't have neighbors.
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    foundfr = wp->w_ಚೌಕಟ್ಟು;
    ಆಗ (ಎಣಿಕೆ_2--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (fr == tp->tp_topframe)
		ಹೋಗು ಕೊನೆ;
	    ಆದರೆ (ಮೇಲೆ)
		nfr = fr->fr_prev;
	    ಇಲ್ಲದಿದ್ದರೆ
		nfr = fr->fr_ಮುಂದಿನ;
	    ಆದರೆ (fr->fr_ಪೋಷಕ->fr_layout == FR_ಲಂಬಸಾಲು && nfr != ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    fr = fr->fr_ಪೋಷಕ;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (nfr->fr_layout == FR_ಎಲೆ)
	    {
		foundfr = nfr;
		ನಿಲ್ಲಿಸು;
	    }
	    fr = nfr->fr_ಮಗು;
	    ಆದರೆ (nfr->fr_layout == FR_ಅಡ್ಡಸಾಲು)
	    {
		// Find the frame at the cursor row.
		ಆಗ (fr->fr_ಮುಂದಿನ != ಖಾಲಿ
			&& frame2win(fr)->w_wincol + fr->fr_ಅಗಲ
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_ಮುಂದಿನ;
	    }
	    ಆದರೆ (nfr->fr_layout == FR_ಲಂಬಸಾಲು && ಮೇಲೆ)
		ಆಗ (fr->fr_ಮುಂದಿನ != ಖಾಲಿ)
		    fr = fr->fr_ಮುಂದಿನ;
	    nfr = fr;
	}
    }
ಕೊನೆ:
    ಹಿಂತಿರುಗಿ foundfr != ಖಾಲಿ ? foundfr->fr_win : ಖಾಲಿ;
}

/*
 * Move to window above or below "count" times.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_goto_ver(
    ಪೂರ್ಣಾಂಕ		ಮೇಲೆ,		// TRUE to go to win above
    ದೀರ್ಘ	ಎಣಿಕೆ_2)
{
    win_T	*win;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (ERROR_IF_ಪದ_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    win = win_vert_neighbor(curtab, curwin, ಮೇಲೆ, ಎಣಿಕೆ_2);
    ಆದರೆ (win != ಖಾಲಿ)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, ಪೂರ್ಣಾಂಕ ಎಡ_2, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    ಚೌಕಟ್ಟು_T	*fr;
    ಚೌಕಟ್ಟು_T	*nfr;
    ಚೌಕಟ್ಟು_T	*foundfr;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (popup_is_popup(wp))
	// popups don't have neighbors.
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    foundfr = wp->w_ಚೌಕಟ್ಟು;
    ಆಗ (ಎಣಿಕೆ_2--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (fr == tp->tp_topframe)
		ಹೋಗು ಕೊನೆ;
	    ಆದರೆ (ಎಡ_2)
		nfr = fr->fr_prev;
	    ಇಲ್ಲದಿದ್ದರೆ
		nfr = fr->fr_ಮುಂದಿನ;
	    ಆದರೆ (fr->fr_ಪೋಷಕ->fr_layout == FR_ಅಡ್ಡಸಾಲು && nfr != ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    fr = fr->fr_ಪೋಷಕ;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (nfr->fr_layout == FR_ಎಲೆ)
	    {
		foundfr = nfr;
		ನಿಲ್ಲಿಸು;
	    }
	    fr = nfr->fr_ಮಗು;
	    ಆದರೆ (nfr->fr_layout == FR_ಲಂಬಸಾಲು)
	    {
		// Find the frame at the cursor row.
		ಆಗ (fr->fr_ಮುಂದಿನ != ಖಾಲಿ
			&& frame2win(fr)->w_winrow + fr->fr_ಎತ್ತರ
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_ಮುಂದಿನ;
	    }
	    ಆದರೆ (nfr->fr_layout == FR_ಅಡ್ಡಸಾಲು && ಎಡ_2)
		ಆಗ (fr->fr_ಮುಂದಿನ != ಖಾಲಿ)
		    fr = fr->fr_ಮುಂದಿನ;
	    nfr = fr;
	}
    }
ಕೊನೆ:
    ಹಿಂತಿರುಗಿ foundfr != ಖಾಲಿ ? foundfr->fr_win : ಖಾಲಿ;
}

/*
 * Move to left or right window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_goto_hor(
    ಪೂರ್ಣಾಂಕ		ಎಡ_2,		// TRUE to go to left win
    ದೀರ್ಘ	ಎಣಿಕೆ_2)
{
    win_T	*win;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (ERROR_IF_ಪದ_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    win = win_horz_neighbor(curtab, curwin, ಎಡ_2, ಎಣಿಕೆ_2);
    ಆದರೆ (win != ಖಾಲಿ)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    ಶೂನ್ಯ
win_enter(win_T *wp, ಪೂರ್ಣಾಂಕ undo_sync)
{
    (ಶೂನ್ಯ)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    ಶೂನ್ಯ
win_fix_current_dir(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_localdir != ಖಾಲಿ || curtab->tp_localdir != ಖಾಲಿ)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	ಆದರೆ (globaldir == ಖಾಲಿ)
	{
	    char_u	cwd[MAXPATHL];

	    ಆದರೆ (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	ಆದರೆ (curwin->w_localdir != ಖಾಲಿ)
	    dirname = curwin->w_localdir;
	ಇಲ್ಲದಿದ್ದರೆ
	    dirname = curtab->tp_localdir;

	ಆದರೆ (mch_chdir((ಅಕ್ಷರ *)dirname) == 0)
	{
	    ಕೊನೆಯ_chdir_reason = ಖಾಲಿ;
	    shorten_fnames(TRUE);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (globaldir != ಖಾಲಿ)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((ಅಕ್ಷರ *)globaldir);
	VIM_CLEAR(globaldir);
	ಕೊನೆಯ_chdir_reason = ಖಾಲಿ;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
win_enter_ext(win_T *wp, ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ		other_ಬಫರ್ = FALSE;
    ಪೂರ್ಣಾಂಕ		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    ಪೂರ್ಣಾಂಕ		did_decrement = FALSE;

    ಆದರೆ (wp == curwin && curwin_invalid == 0)	// nothing to do
	ಹಿಂತಿರುಗಿ FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (curwin_invalid == 0)
	leaving_ಕಿಟಕಿ(curwin);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	ಆದರೆ (wp->w_ಬಫರ್ != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	    other_ಬಫರ್ = TRUE;
	    ಆದರೆ (!win_valid(wp))
		ಹಿಂತಿರುಗಿ FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	ಆದರೆ (!win_valid(wp))
	    ಹಿಂತಿರುಗಿ FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// autocmds may abort script processing
	ಆದರೆ (aborting())
	    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    }

    // sync undo before leaving the current buffer
    ಆದರೆ ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_ಬಫರ್)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    ಆದರೆ (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    ಆದರೆ (wp->w_ಬಫರ್ != curbuf)
	buf_copy_options(wp->w_ಬಫರ್, BCO_ENTER | BCO_NOHELP);
    ಆದರೆ (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_ಸ್ಥಿತಿ = true;
    }
    curwin = wp;
    curbuf = wp->w_ಬಫರ್;
    check_cursor();
    ಆದರೆ (!virtual_ಸಕ್ರಿಯ())
	curwin->w_cursor.coladd = 0;
    ಆದರೆ (*p_spk == 'c')		// assume cursor position needs updating
	ಬದಲಾಯಿತು_ಸಾಲು_abv_curs();
    ಇಲ್ಲದಿದ್ದರೆ
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (ಬಹುಲಕ_ಸಾಮಾನ್ಯ|ಬಹುಲಕ_CMDLINE|ಬಹುಲಕ_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
    ಆದರೆ (flags & WEE_ALLOW_PARSE_MESSAGES)
    {
	--dont_parse_messages;
	did_decrement = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    win_fix_current_dir();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    entering_ಕಿಟಕಿ(curwin);
#ಆದರೆ_ಅಂತ್ಯ
    // Careful: autocommands may close the window and make "wp" invalid
    ಆದರೆ (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    ಆದರೆ (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	ಆದರೆ (other_ಬಫರ್)
	    apply_autocmds(EVENT_BUFENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_ಸ್ಥಿತಿ = true;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (bt_terminal(curwin->w_ಬಫರ್))
	// terminal is likely in another mode
	redraw_ಬಹುಲಕ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    ಆದರೆ (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    ಆದರೆ (curwin->w_ಎತ್ತರ < p_wh && !curwin->w_p_wfh
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#ಆದರೆ_ಅಂತ್ಯ
	    )
	win_setheight((ಪೂರ್ಣಾಂಕ)p_wh);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_ಎತ್ತರ == 0)
	win_setheight(1);

    // set window width to desired minimal value
    ಆದರೆ (curwin->w_ಅಗಲ < p_wiw && !curwin->w_p_wfw)
	win_setwidth((ಪೂರ್ಣಾಂಕ)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    ಹಿಂತಿರುಗಿ did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = ಖಾಲಿ;

    ಆದರೆ (curwin->w_ಬಫರ್ == buf)
	wp = curwin;
    ಇಲ್ಲದಿದ್ದರೆ
	FOR_ALL_WINDOWS(wp)
	    ಆದರೆ (wp->w_ಬಫರ್ == buf)
		ನಿಲ್ಲಿಸು;
    ಆದರೆ (wp != ಖಾಲಿ)
	win_enter(wp, FALSE);
    ಹಿಂತಿರುಗಿ wp;
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

    ಆದರೆ (wp != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ wp;

    FOR_ALL_TABPAGES(tp)
	ಆದರೆ (tp != curtab)
	{
	    FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp)
		ಆದರೆ (wp->w_ಬಫರ್ == buf)
		    ನಿಲ್ಲಿಸು;
	    ಆದರೆ (wp != ಖಾಲಿ)
	    {
		goto_tabpage_win(tp, wp);
		ಆದರೆ (curwin != wp)
		    wp = ಖಾಲಿ;	// something went wrong
		ನಿಲ್ಲಿಸು;
	    }
	}
    ಹಿಂತಿರುಗಿ wp;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_win_id = ಅತಿಕಡಿಮೆ_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    ಸ್ಥಿರ win_T *
win_alloc(win_T *after, ಪೂರ್ಣಾಂಕ hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    ಆದರೆ (new_wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (win_alloc_ಸಾಲುಗಳು(new_wp) == FAIL)
    {
	vim_free(new_wp);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    new_wp->w_id = ++ಕೊನೆಯ_win_id;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    ಆದರೆ (new_wp->w_vars == ಖಾಲಿ)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    init_var_dict(new_wp->w_vars, &new_wp->w_winvar, VAR_SCOPE);
#ಆದರೆ_ಅಂತ್ಯ

    // Don't execute autocommands while the window is not properly
    // initialized yet.  gui_create_scrollbar() may trigger a FocusGained
    // event.
    ಖಂಡ_autocmds();

    /*
     * link the window in the window list
     */
    ಆದರೆ (!hidden)
	win_append(after, new_wp);
    new_wp->w_wincol = TPL_LCOL();
    new_wp->w_ಅಗಲ = COLUMNS_WITHOUT_TPL();

    // position the display and the cursor at the top of the file.
    new_wp->w_topline = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    new_wp->w_topfill = 0;
#ಆದರೆ_ಅಂತ್ಯ
    new_wp->w_botline = 2;
    new_wp->w_cursor.lnum = 1;
    new_wp->w_scbind_pos = 1;

    // use global option value for global-local options
    new_wp->w_allbuf_opt.wo_so = new_wp->w_p_so = -1;
    new_wp->w_allbuf_opt.wo_sop = new_wp->w_p_sop = -1;
    new_wp->w_allbuf_opt.wo_siso = new_wp->w_p_siso = -1;

    // We won't calculate w_fraction until resizing the window
    new_wp->w_fraction = 0;
    new_wp->w_prev_fraction_ಅಡ್ಡಸಾಲು = -1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ಎಡ],
		SBAR_ಎಡ, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ಬಲ],
		SBAR_ಬಲ, new_wp);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    foldInitWin(new_wp);
#ಆದರೆ_ಅಂತ್ಯ
    unblock_autocmds();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    new_wp->w_ಮುಂದಿನ_ಹೊಂದಿಕೆ_id = 1000;  // up to 1000 can be picked by the user
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ಪೂರ್ಣಾಂಕ		i;
    buf_T	*buf;
    wininfo_T	*wip;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    clearFolding(wp);
#ಆದರೆ_ಅಂತ್ಯ

    // reduce the reference count to the argument list.
    alist_unlink(wp->w_alist);

    // Don't execute autocommands while the window is halfway being deleted.
    // gui_mch_destroy_scrollbar() may trigger a FocusGained event.
    ಖಂಡ_autocmds();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LUA
    lua_ಕಿಟಕಿ_free(wp);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
    mzscheme_ಕಿಟಕಿ_free(wp);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERL
    perl_win_free(wp);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
    python_ಕಿಟಕಿ_free(wp);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
    python3_ಕಿಟಕಿ_free(wp);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TCL
    tcl_ಕಿಟಕಿ_free(wp);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RUBY
    ruby_ಕಿಟಕಿ_free(wp);
#ಆದರೆ_ಅಂತ್ಯ

    remove_highlight_overrides(wp->w_hl);
    vim_free(wp->w_hl);

    // Free statusline click regions.
    ಆದರೆ (wp->w_stl_click != ಖಾಲಿ)
    {
	ಗಾಗಿ (i = 0; i < wp->w_stl_click_ಎಣಿಕೆ; i++)
	    vim_free(wp->w_stl_click[i].funcname);
	vim_free(wp->w_stl_click);
    }

    clear_winopt(&wp->w_onebuf_opt);
    clear_winopt(&wp->w_allbuf_opt);

    vim_free(wp->w_lcs_chars.multispace);
    vim_free(wp->w_lcs_chars.leadmultispace);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    vars_clear(&wp->w_vars->dv_hashtab);	// free all w: variables
    hash_init(&wp->w_vars->dv_hashtab);
    unref_var_dict(wp->w_vars);
#ಆದರೆ_ಅಂತ್ಯ

    {
	tabpage_T	*ttp;

	ಆದರೆ (prevwin == wp)
	    prevwin = ಖಾಲಿ;
	FOR_ALL_TABPAGES(ttp)
	    ಆದರೆ (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = ಖಾಲಿ;
    }
    win_free_lsize(wp);

    ಗಾಗಿ (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_ನಮೂದು(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    ಆದರೆ (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    ಆದರೆ (wip2 != wip && wip2->wi_win == ಖಾಲಿ)
		    {
			ಆದರೆ (wip2->wi_ಮುಂದಿನ != ಖಾಲಿ)
			    wip2->wi_ಮುಂದಿನ->wi_prev = wip2->wi_prev;
			ಆದರೆ (wip2->wi_prev == ಖಾಲಿ)
			    buf->b_wininfo = wip2->wi_ಮುಂದಿನ;
			ಇಲ್ಲದಿದ್ದರೆ
			    wip2->wi_prev->wi_ಮುಂದಿನ = wip2->wi_ಮುಂದಿನ;
			free_wininfo(wip2);
			ನಿಲ್ಲಿಸು;
		    }

		wip->wi_win = ಖಾಲಿ;
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    clear_matches(wp);
#ಆದರೆ_ಅಂತ್ಯ

    free_jumplist(wp);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    qf_free_all(wp);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ಎಡ]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ಬಲ]);
    }
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    remove_winbar(wp);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    ಗಾಗಿ (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    vim_free(wp->w_scrollbar_highlight);
    vim_free(wp->w_thumb_highlight);
    vim_free(wp->w_popup_title);
    list_unref(wp->w_popup_ಮುಖವಾಡ);
    vim_free(wp->w_popup_ಮುಖವಾಡ_cells);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    vim_free(wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (win_valid_any_tab(wp))
	win_remove(wp, tp);
    ಆದರೆ (autocmd_busy)
    {
	wp->w_ಮುಂದಿನ = au_pending_free_win;
	au_pending_free_win = wp;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    ಪೂರ್ಣಾಂಕ
win_unlisted(win_T *wp)
{
    ಹಿಂತಿರುಗಿ is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#ಆದರೆ defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    ಶೂನ್ಯ
win_free_popup(win_T *win)
{
    ಆದರೆ (win->w_ಬಫರ್ != ಖಾಲಿ)
    {
	ಆದರೆ (bt_popup(win->w_ಬಫರ್))
	    win_close_ಬಫರ್(win, DOBUF_WIPE_REUSE, FALSE);
	ಇಲ್ಲದಿದ್ದರೆ
	    close_ಬಫರ್(win, win->w_ಬಫರ್, 0, FALSE, FALSE, TRUE);

	ಆದರೆ (win->w_ಬಫರ್ != ಖಾಲಿ)
	    --win->w_ಬಫರ್->b_nwindows;
    }
# ಆದರೆ defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    ಆದರೆ (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_ಚೌಕಟ್ಟು);
    win_free(win, ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Append window "wp" in the window list after window "after".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    ಆದರೆ (after == ಖಾಲಿ)	    // after NULL is in front of the first
	before = firstwin;
    ಇಲ್ಲದಿದ್ದರೆ
	before = after->w_ಮುಂದಿನ;

    wp->w_ಮುಂದಿನ = before;
    wp->w_prev = after;
    ಆದರೆ (after == ಖಾಲಿ)
	firstwin = wp;
    ಇಲ್ಲದಿದ್ದರೆ
	after->w_ಮುಂದಿನ = wp;
    ಆದರೆ (before == ಖಾಲಿ)
	lastwin = wp;
    ಇಲ್ಲದಿದ್ದರೆ
	before->w_prev = wp;
}

/*
 * Remove a window from the window list.
 */
    ಶೂನ್ಯ
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ಆದರೆ (wp->w_prev != ಖಾಲಿ)
	wp->w_prev->w_ಮುಂದಿನ = wp->w_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tp == ಖಾಲಿ)
	firstwin = curtab->tp_firstwin = wp->w_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	tp->tp_firstwin = wp->w_ಮುಂದಿನ;

    ಆದರೆ (wp->w_ಮುಂದಿನ != ಖಾಲಿ)
	wp->w_ಮುಂದಿನ->w_prev = wp->w_prev;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tp == ಖಾಲಿ)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    ಇಲ್ಲದಿದ್ದರೆ
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_append(ಚೌಕಟ್ಟು_T *after, ಚೌಕಟ್ಟು_T *frp)
{
    frp->fr_ಮುಂದಿನ = after->fr_ಮುಂದಿನ;
    after->fr_ಮುಂದಿನ = frp;
    ಆದರೆ (frp->fr_ಮುಂದಿನ != ಖಾಲಿ)
	frp->fr_ಮುಂದಿನ->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_ಸೇರಿಸು(ಚೌಕಟ್ಟು_T *before, ಚೌಕಟ್ಟು_T *frp)
{
    frp->fr_ಮುಂದಿನ = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    ಆದರೆ (frp->fr_prev != ಖಾಲಿ)
	frp->fr_prev->fr_ಮುಂದಿನ = frp;
    ಇಲ್ಲದಿದ್ದರೆ
	frp->fr_ಪೋಷಕ->fr_ಮಗು = frp;
}

/*
 * Remove a frame from a frame list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_remove(ಚೌಕಟ್ಟು_T *frp)
{
    ಆದರೆ (frp->fr_prev != ಖಾಲಿ)
	frp->fr_prev->fr_ಮುಂದಿನ = frp->fr_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	frp->fr_ಪೋಷಕ->fr_ಮಗು = frp->fr_ಮುಂದಿನ;
    ಆದರೆ (frp->fr_ಮುಂದಿನ != ಖಾಲಿ)
	frp->fr_ಮುಂದಿನ->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    ಪೂರ್ಣಾಂಕ
win_alloc_ಸಾಲುಗಳು(win_T *wp)
{
    wp->w_ಸಾಲುಗಳು_valid = 0;
    wp->w_ಸಾಲುಗಳು = ALLOC_CLEAR_MULT(wline_T, ಅಡ್ಡಸಾಲುಗಳು);
    ಆದರೆ (wp->w_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * free lsize arrays for a window
 */
    ಶೂನ್ಯ
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    ಆದರೆ (wp != ಖಾಲಿ)
	VIM_CLEAR(wp->w_ಸಾಲುಗಳು);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    ಶೂನ್ಯ
shell_new_ಅಡ್ಡಸಾಲುಗಳು(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		h = (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು_AVAIL;

    ಆದರೆ (firstwin == ಖಾಲಿ)	// not initialized yet
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (h < ಚೌಕಟ್ಟು_minheight(topframe, ಖಾಲಿ))
	h = ಚೌಕಟ್ಟು_minheight(topframe, ಖಾಲಿ);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    ಚೌಕಟ್ಟು_new_ಎತ್ತರ(topframe, h, FALSE, TRUE, FALSE);
    ಆದರೆ (!ಚೌಕಟ್ಟು_check_ಎತ್ತರ(topframe, h))
	ಚೌಕಟ್ಟು_new_ಎತ್ತರ(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_ಬಳಸಿದ = p_ch;

    ಆದರೆ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ 0
    // Disabled: don't want making the screen smaller make a window larger.
    ಆದರೆ (p_ea)
	win_equal(curwin, FALSE, 'v');
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    ಶೂನ್ಯ
shell_new_columns(ಶೂನ್ಯ)
{
    ಆದರೆ (firstwin == ಖಾಲಿ)	// not initialized yet
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ defined(FEAT_TABPANEL)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prev_tp_ಅಗಲ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prev_wincol;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prev_fr_ಅಗಲ;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    ಚೌಕಟ್ಟು_new_ಅಗಲ(topframe, w, FALSE, TRUE);
    ಆದರೆ (!ಚೌಕಟ್ಟು_check_ಅಗಲ(topframe, w))
	ಚೌಕಟ್ಟು_new_ಅಗಲ(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#ಆದರೆ defined(FEAT_TABPANEL)
    ಪೂರ್ಣಾಂಕ tp_ಅಗಲ = tabpanel_ಅಗಲ();
    ಪೂರ್ಣಾಂಕ tp_ಅಗಲ_ಬದಲಾಯಿತು = tp_ಅಗಲ != prev_tp_ಅಗಲ;
    // tabpanel width changed
    ಆದರೆ (tp_ಅಗಲ_ಬದಲಾಯಿತು && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    ಆದರೆ (tp_ಅಗಲ_ಬದಲಾಯಿತು
	    || (tp_ಅಗಲ > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_ಅಗಲ != prev_fr_ಅಗಲ)))
    {
	screen_fill(cmdline_ಅಡ್ಡಸಾಲು, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, 0, (ಪೂರ್ಣಾಂಕ)Columns, ' ', ' ', 0);
    }
    prev_tp_ಅಗಲ = tabpanel_ಅಗಲ();
    prev_wincol = firstwin->w_wincol;
    prev_fr_ಅಗಲ = topframe->fr_ಅಗಲ;
#ಆದರೆ_ಅಂತ್ಯ
    // Adjust offset for command line start column
    cmdline_ಲಂಬಸಾಲು_off = firstwin->w_wincol;
    cmdline_ಅಗಲ = topframe->fr_ಅಗಲ;
    comp_ಲಂಬಸಾಲು();
    ಆದರೆ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ 0
    // Disabled: don't want making the screen smaller make a window larger.
    ಆದರೆ (p_ea)
	win_equal(curwin, FALSE, 'h');
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Save the size of all windows in "gap".
 */
    ಶೂನ್ಯ
win_ಗಾತ್ರ_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ), 1);
    ಆದರೆ (ga_grow(gap, win_ಎಣಿಕೆ() * 2 + 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // first entry is the total lines available for windows
    ((ಪೂರ್ಣಾಂಕ *)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] = ಅಡ್ಡಸಾಲುಗಳು_AVAIL - ಕೊನೆಯ_stl_ಎತ್ತರ(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((ಪೂರ್ಣಾಂಕ *)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] =
	    wp->w_ಅಗಲ + wp->w_vsep_ಅಗಲ;
	((ಪೂರ್ಣಾಂಕ *)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] = wp->w_ಎತ್ತರ;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    ಶೂನ್ಯ
win_ಗಾತ್ರ_restore(garray_T *gap)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		i, j;

    ಆದರೆ (win_ಎಣಿಕೆ() * 2 + 1 == gap->ga_len
	    && ((ಪೂರ್ಣಾಂಕ *)gap->ga_ದತ್ತಾಂಶ)[0] == ಅಡ್ಡಸಾಲುಗಳು_AVAIL - ಕೊನೆಯ_stl_ಎತ್ತರ(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	ಗಾಗಿ (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		ಚೌಕಟ್ಟು_setwidth(wp->w_ಚೌಕಟ್ಟು, ((ಪೂರ್ಣಾಂಕ *)gap->ga_ದತ್ತಾಂಶ)[i++]);
		win_setheight_win(((ಪೂರ್ಣಾಂಕ *)gap->ga_ದತ್ತಾಂಶ)[i++], wp);
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
    ಶೂನ್ಯ
win_comp_pos(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = tabline_ಎತ್ತರ();
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = TPL_LCOL();

    ಚೌಕಟ್ಟು_comp_pos(topframe, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು);
#ಆದರೆ defined(FEAT_STL_OPT)
    ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_comp_pos(ಚೌಕಟ್ಟು_T *topfrp, ಪೂರ್ಣಾಂಕ *ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ *ಲಂಬಸಾಲು)
{
    win_T	*wp;
    ಚೌಕಟ್ಟು_T	*frp;
    ಪೂರ್ಣಾಂಕ		startcol;
    ಪೂರ್ಣಾಂಕ		startrow;
    ಪೂರ್ಣಾಂಕ		h;

    wp = topfrp->fr_win;
    ಆದರೆ (wp != ಖಾಲಿ)
    {
	ಆದರೆ (wp->w_winrow != *ಅಡ್ಡಸಾಲು || wp->w_wincol != *ಲಂಬಸಾಲು)
	{
	    // position changed, redraw
	    wp->w_winrow = *ಅಡ್ಡಸಾಲು;
	    wp->w_wincol = *ಲಂಬಸಾಲು;
	    redraw_win_later(wp, UPD_NOT_VALID);
	    wp->w_redr_ಸ್ಥಿತಿ = true;
	}
	// WinBar will not show if the window height is zero
	h = VISIBLE_ಎತ್ತರ(wp) + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	*ಅಡ್ಡಸಾಲು += h > topfrp->fr_ಎತ್ತರ ? topfrp->fr_ಎತ್ತರ : h;
	*ಲಂಬಸಾಲು += wp->w_ಅಗಲ + wp->w_vsep_ಅಗಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	startrow = *ಅಡ್ಡಸಾಲು;
	startcol = *ಲಂಬಸಾಲು;
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, topfrp->fr_ಮಗು)
	{
	    ಆದರೆ (topfrp->fr_layout == FR_ಅಡ್ಡಸಾಲು)
		*ಅಡ್ಡಸಾಲು = startrow;	// all frames are at the same row
	    ಇಲ್ಲದಿದ್ದರೆ
		*ಲಂಬಸಾಲು = startcol;	// all frames are at the same col
	    ಚೌಕಟ್ಟು_comp_pos(frp, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    ಶೂನ್ಯ
win_ensure_ಗಾತ್ರ(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_ಎತ್ತರ == 0)
	win_setheight(1);
    ಆದರೆ (curwin->w_ಅಗಲ == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    ಶೂನ್ಯ
win_setheight(ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    win_setheight_win(ಎತ್ತರ, curwin);
#ಆದರೆ defined(FEAT_STL_OPT)
    ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    ಶೂನ್ಯ
win_setheight_win(ಪೂರ್ಣಾಂಕ ಎತ್ತರ, win_T *win)
{
    ಆದರೆ (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	ಆದರೆ (ಎತ್ತರ < p_wmh)
	    ಎತ್ತರ = p_wmh;
	ಆದರೆ (ಎತ್ತರ == 0)
	    ಎತ್ತರ = 1;
	ಎತ್ತರ += WINBAR_ಎತ್ತರ(curwin);
    }

    ಚೌಕಟ್ಟು_setheight(win->w_ಚೌಕಟ್ಟು, ಎತ್ತರ + win->w_ಸ್ಥಿತಿ_ಎತ್ತರ);

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
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_setheight(ಚೌಕಟ್ಟು_T *curfrp, ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ;		// total number of lines available
    ಪೂರ್ಣಾಂಕ		take;		// number of lines taken from other windows
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ_cmdline;	// lines available from cmdline
    ಪೂರ್ಣಾಂಕ		ಸತತ;
    ಚೌಕಟ್ಟು_T	*frp;
    ಪೂರ್ಣಾಂಕ		h;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ_reserved;

    // If the height already is the desired value, nothing to do.
    ಆದರೆ (curfrp->fr_ಎತ್ತರ == ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (curfrp->fr_ಪೋಷಕ == ಖಾಲಿ)
    {
	// topframe: can only change the command line height
	ಆದರೆ (ಎತ್ತರ > 0)
	    ಚೌಕಟ್ಟು_new_ಎತ್ತರ(curfrp, ಎತ್ತರ, FALSE, FALSE, TRUE);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curfrp->fr_ಪೋಷಕ->fr_layout == FR_ಅಡ್ಡಸಾಲು)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = ಚೌಕಟ್ಟು_minheight(curfrp->fr_ಪೋಷಕ, ಖಾಲಿ);
	ಆದರೆ (ಎತ್ತರ < h)
	    ಎತ್ತರ = h;
	ಚೌಕಟ್ಟು_setheight(curfrp->fr_ಪೋಷಕ, ಎತ್ತರ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
	ಗಾಗಿ (ಸತತ = 1; ಸತತ <= 2; ++ಸತತ)
	{
	    ಸ್ಥಳ = 0;
	    ಸ್ಥಳ_reserved = 0;
	    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, curfrp->fr_ಪೋಷಕ->fr_ಮಗು)
	    {
		ಆದರೆ (frp != curfrp
			&& frp->fr_win != ಖಾಲಿ
			&& frp->fr_win->w_p_wfh)
		    ಸ್ಥಳ_reserved += frp->fr_ಎತ್ತರ;
		ಸ್ಥಳ += frp->fr_ಎತ್ತರ;
		ಆದರೆ (frp != curfrp)
		    ಸ್ಥಳ -= ಚೌಕಟ್ಟು_minheight(frp, ಖಾಲಿ);
	    }
	    ಆದರೆ (curfrp->fr_ಅಗಲ != topframe->fr_ಅಗಲ)
		ಸ್ಥಳ_cmdline = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಸ್ಥಳ_cmdline = ಅಡ್ಡಸಾಲುಗಳು - p_ch - (lastwin->w_winrow
						+ VISIBLE_ಎತ್ತರ(lastwin)
						+ lastwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ);
		ಆದರೆ (ಸ್ಥಳ_cmdline < 0)
		    ಸ್ಥಳ_cmdline = 0;
	    }

	    ಆದರೆ (ಎತ್ತರ <= ಸ್ಥಳ + ಸ್ಥಳ_cmdline)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (ಸತತ == 2 || curfrp->fr_ಅಗಲ == topframe->fr_ಅಗಲ)
	    {
		ಎತ್ತರ = ಸ್ಥಳ + ಸ್ಥಳ_cmdline;
		ನಿಲ್ಲಿಸು;
	    }
	    ಚೌಕಟ್ಟು_setheight(curfrp->fr_ಪೋಷಕ, ಎತ್ತರ
		+ ಚೌಕಟ್ಟು_minheight(curfrp->fr_ಪೋಷಕ, NOWIN) - (ಪೂರ್ಣಾಂಕ)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = ಎತ್ತರ - curfrp->fr_ಎತ್ತರ;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	ಆದರೆ (ಎತ್ತರ > ಸ್ಥಳ + ಸ್ಥಳ_cmdline - ಸ್ಥಳ_reserved)
	    ಸ್ಥಳ_reserved = ಸ್ಥಳ + ಸ್ಥಳ_cmdline - ಎತ್ತರ;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	ಆದರೆ (take < 0 && ಸ್ಥಳ - curfrp->fr_ಎತ್ತರ <= ಸ್ಥಳ_reserved)
	    ಸ್ಥಳ_reserved = 0;

	ಆದರೆ (take > 0 && ಸ್ಥಳ_cmdline > 0)
	{
	    // use lines from cmdline first
	    ಆದರೆ (take < ಸ್ಥಳ_cmdline)
		ಸ್ಥಳ_cmdline = take;
	    take -= ಸ್ಥಳ_cmdline;
	    topframe->fr_ಎತ್ತರ += ಸ್ಥಳ_cmdline;
	}

	/*
	 * set the current frame to the new height
	 */
	ಚೌಕಟ್ಟು_new_ಎತ್ತರ(curfrp, ಎತ್ತರ, FALSE, FALSE, TRUE);

	/*
	 * First take lines from the frames after the current frame.  If
	 * that is not enough, takes lines from frames above the current
	 * frame.
	 */
	ಗಾಗಿ (ಸತತ = 0; ಸತತ < 2; ++ಸತತ)
	{
	    ಆದರೆ (ಸತತ == 0)
		frp = curfrp->fr_ಮುಂದಿನ;	// 1st run: start with next window
	    ಇಲ್ಲದಿದ್ದರೆ
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    ಆಗ (frp != ಖಾಲಿ && take != 0)
	    {
		h = ಚೌಕಟ್ಟು_minheight(frp, ಖಾಲಿ);
		ಆದರೆ (ಸ್ಥಳ_reserved > 0
			&& frp->fr_win != ಖಾಲಿ
			&& frp->fr_win->w_p_wfh)
		{
		    ಆದರೆ (ಸ್ಥಳ_reserved >= frp->fr_ಎತ್ತರ)
			ಸ್ಥಳ_reserved -= frp->fr_ಎತ್ತರ;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (frp->fr_ಎತ್ತರ - ಸ್ಥಳ_reserved > take)
			    ಸ್ಥಳ_reserved = frp->fr_ಎತ್ತರ - take;
			take -= frp->fr_ಎತ್ತರ - ಸ್ಥಳ_reserved;
			ಚೌಕಟ್ಟು_new_ಎತ್ತರ(frp, ಸ್ಥಳ_reserved, FALSE, FALSE,
									TRUE);
			ಸ್ಥಳ_reserved = 0;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (frp->fr_ಎತ್ತರ - take < h)
		    {
			take -= frp->fr_ಎತ್ತರ - h;
			ಚೌಕಟ್ಟು_new_ಎತ್ತರ(frp, h, FALSE, FALSE, TRUE);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಚೌಕಟ್ಟು_new_ಎತ್ತರ(frp, frp->fr_ಎತ್ತರ - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		ಆದರೆ (ಸತತ == 0)
		    frp = frp->fr_ಮುಂದಿನ;
		ಇಲ್ಲದಿದ್ದರೆ
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    ಶೂನ್ಯ
win_setwidth(ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
    win_setwidth_win(ಅಗಲ, curwin);
}

    ಶೂನ್ಯ
win_setwidth_win(ಪೂರ್ಣಾಂಕ ಅಗಲ, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    ಆದರೆ (wp == curwin)
    {
	ಆದರೆ (ಅಗಲ < p_wmw)
	    ಅಗಲ = p_wmw;
	ಆದರೆ (ಅಗಲ == 0)
	    ಅಗಲ = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಗಲ < 0)
	ಅಗಲ = 0;

    ಚೌಕಟ್ಟು_setwidth(wp->w_ಚೌಕಟ್ಟು, ಅಗಲ + wp->w_vsep_ಅಗಲ);

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
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_setwidth(ಚೌಕಟ್ಟು_T *curfrp, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ;		// total number of lines available
    ಪೂರ್ಣಾಂಕ		take;		// number of lines taken from other windows
    ಪೂರ್ಣಾಂಕ		ಸತತ;
    ಚೌಕಟ್ಟು_T	*frp;
    ಪೂರ್ಣಾಂಕ		w;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ_reserved;

    // If the width already is the desired value, nothing to do.
    ಆದರೆ (curfrp->fr_ಅಗಲ == ಅಗಲ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (curfrp->fr_ಪೋಷಕ == ಖಾಲಿ)
	// topframe: can't change width
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (curfrp->fr_ಪೋಷಕ->fr_layout == FR_ಲಂಬಸಾಲು)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = ಚೌಕಟ್ಟು_minwidth(curfrp->fr_ಪೋಷಕ, ಖಾಲಿ);
	ಆದರೆ (ಅಗಲ < w)
	    ಅಗಲ = w;
	ಚೌಕಟ್ಟು_setwidth(curfrp->fr_ಪೋಷಕ, ಅಗಲ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	ಗಾಗಿ (ಸತತ = 1; ಸತತ <= 2; ++ಸತತ)
	{
	    ಸ್ಥಳ = 0;
	    ಸ್ಥಳ_reserved = 0;
	    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, curfrp->fr_ಪೋಷಕ->fr_ಮಗು)
	    {
		ಆದರೆ (frp != curfrp
			&& frp->fr_win != ಖಾಲಿ
			&& frp->fr_win->w_p_wfw)
		    ಸ್ಥಳ_reserved += frp->fr_ಅಗಲ;
		ಸ್ಥಳ += frp->fr_ಅಗಲ;
		ಆದರೆ (frp != curfrp)
		    ಸ್ಥಳ -= ಚೌಕಟ್ಟು_minwidth(frp, ಖಾಲಿ);
	    }

	    ಆದರೆ (ಅಗಲ <= ಸ್ಥಳ)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (ಸತತ == 2 || curfrp->fr_ಎತ್ತರ >= ಅಡ್ಡಸಾಲುಗಳು_AVAIL)
	    {
		ಅಗಲ = ಸ್ಥಳ;
		ನಿಲ್ಲಿಸು;
	    }
	    ಚೌಕಟ್ಟು_setwidth(curfrp->fr_ಪೋಷಕ, ಅಗಲ
		 + ಚೌಕಟ್ಟು_minwidth(curfrp->fr_ಪೋಷಕ, NOWIN) - (ಪೂರ್ಣಾಂಕ)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = ಅಗಲ - curfrp->fr_ಅಗಲ;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	ಆದರೆ (ಅಗಲ > ಸ್ಥಳ - ಸ್ಥಳ_reserved)
	    ಸ್ಥಳ_reserved = ಸ್ಥಳ - ಅಗಲ;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	ಆದರೆ (take < 0 && ಸ್ಥಳ - curfrp->fr_ಅಗಲ < ಸ್ಥಳ_reserved)
	    ಸ್ಥಳ_reserved = 0;

	/*
	 * set the current frame to the new width
	 */
	ಚೌಕಟ್ಟು_new_ಅಗಲ(curfrp, ಅಗಲ, FALSE, FALSE);

	/*
	 * First take lines from the frames right of the current frame.  If
	 * that is not enough, takes lines from frames left of the current
	 * frame.
	 */
	ಗಾಗಿ (ಸತತ = 0; ಸತತ < 2; ++ಸತತ)
	{
	    ಆದರೆ (ಸತತ == 0)
		frp = curfrp->fr_ಮುಂದಿನ;	// 1st run: start with next window
	    ಇಲ್ಲದಿದ್ದರೆ
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    ಆಗ (frp != ಖಾಲಿ && take != 0)
	    {
		w = ಚೌಕಟ್ಟು_minwidth(frp, ಖಾಲಿ);
		ಆದರೆ (ಸ್ಥಳ_reserved > 0
			&& frp->fr_win != ಖಾಲಿ
			&& frp->fr_win->w_p_wfw)
		{
		    ಆದರೆ (ಸ್ಥಳ_reserved >= frp->fr_ಅಗಲ)
			ಸ್ಥಳ_reserved -= frp->fr_ಅಗಲ;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (frp->fr_ಅಗಲ - ಸ್ಥಳ_reserved > take)
			    ಸ್ಥಳ_reserved = frp->fr_ಅಗಲ - take;
			take -= frp->fr_ಅಗಲ - ಸ್ಥಳ_reserved;
			ಚೌಕಟ್ಟು_new_ಅಗಲ(frp, ಸ್ಥಳ_reserved, FALSE, FALSE);
			ಸ್ಥಳ_reserved = 0;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (frp->fr_ಅಗಲ - take < w)
		    {
			take -= frp->fr_ಅಗಲ - w;
			ಚೌಕಟ್ಟು_new_ಅಗಲ(frp, w, FALSE, FALSE);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಚೌಕಟ್ಟು_new_ಅಗಲ(frp, frp->fr_ಅಗಲ - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		ಆದರೆ (ಸತತ == 0)
		    frp = frp->fr_ಮುಂದಿನ;
		ಇಲ್ಲದಿದ್ದರೆ
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    ಶೂನ್ಯ
win_setminheight(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ;
    ಪೂರ್ಣಾಂಕ		needed;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = TRUE;

    // loop until there is a 'winminheight' that is possible
    ಆಗ (p_wmh > 0)
    {
	ಸ್ಥಳ = ಅಡ್ಡಸಾಲುಗಳು - p_ch;
	needed = min_ಅಡ್ಡಸಾಲುಗಳು_for_all_tabpages() - 1;  // 1 was added for the cmdline
	ಆದರೆ (ಸ್ಥಳ >= needed)
	    ನಿಲ್ಲಿಸು;
	--p_wmh;
	ಆದರೆ (ಮೊದಲ)
	{
	    emsg(_(e_not_enough_ಸ್ಥಳ));
	    ಮೊದಲ = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    ಶೂನ್ಯ
win_setminwidth(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ;
    ಪೂರ್ಣಾಂಕ		needed;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = TRUE;

    // loop until there is a 'winminheight' that is possible
    ಆಗ (p_wmw > 0)
    {
	ಸ್ಥಳ = topframe->fr_ಅಗಲ;
	needed = ಚೌಕಟ್ಟು_minwidth(topframe, ಖಾಲಿ);
	ಆದರೆ (ಸ್ಥಳ >= needed)
	    ನಿಲ್ಲಿಸು;
	--p_wmw;
	ಆದರೆ (ಮೊದಲ)
	{
	    emsg(_(e_not_enough_ಸ್ಥಳ));
	    ಮೊದಲ = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    ಶೂನ್ಯ
win_drag_ಸ್ಥಿತಿ_ಸಾಲು(win_T *dragwin, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    ಚೌಕಟ್ಟು_T	*curfr;
    ಚೌಕಟ್ಟು_T	*fr;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ;
    ಪೂರ್ಣಾಂಕ		ಮೇಲೆ;	// if TRUE, drag status line up, otherwise down
    ಪೂರ್ಣಾಂಕ		n;

    fr = dragwin->w_ಚೌಕಟ್ಟು;
    curfr = fr;
    ಆದರೆ (fr != topframe)		// more than one window
    {
	fr = fr->fr_ಪೋಷಕ;
	// When the parent frame is not a column of frames, its parent should
	// be.
	ಆದರೆ (fr->fr_layout != FR_ಲಂಬಸಾಲು)
	{
	    curfr = fr;
	    ಆದರೆ (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_ಪೋಷಕ;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    ಆಗ (curfr != topframe && curfr->fr_ಮುಂದಿನ == ಖಾಲಿ)
    {
	ಆದರೆ (fr != topframe)
	    fr = fr->fr_ಪೋಷಕ;
	curfr = fr;
	ಆದರೆ (fr != topframe)
	    fr = fr->fr_ಪೋಷಕ;
    }

    ಆದರೆ (ಸ್ಥಾನಾಂತರ < 0) // drag up
    {
	ಮೇಲೆ = TRUE;
	ಸ್ಥಾನಾಂತರ = -ಸ್ಥಾನಾಂತರ;
	// sum up the room of the current frame and above it
	ಆದರೆ (fr == curfr)
	{
	    // only one window
	    ಸ್ಥಳ = fr->fr_ಎತ್ತರ - ಚೌಕಟ್ಟು_minheight(fr, ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಸ್ಥಳ = 0;
	    ಗಾಗಿ (fr = fr->fr_ಮಗು; ; fr = fr->fr_ಮುಂದಿನ)
	    {
		ಸ್ಥಳ += fr->fr_ಎತ್ತರ - ಚೌಕಟ್ಟು_minheight(fr, ಖಾಲಿ);
		ಆದರೆ (fr == curfr)
		    ನಿಲ್ಲಿಸು;
	    }
	}
	fr = curfr->fr_ಮುಂದಿನ;		// put fr at frame that grows
    }
    ಇಲ್ಲದಿದ್ದರೆ    // drag down
    {
	ಮೇಲೆ = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	ಸ್ಥಳ = ಅಡ್ಡಸಾಲುಗಳು - cmdline_ಅಡ್ಡಸಾಲು;
	ಆದರೆ (curfr->fr_ಮುಂದಿನ == ಖಾಲಿ)
	    --ಸ್ಥಳ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸ್ಥಳ -= p_ch;
	ಆದರೆ (ಸ್ಥಳ < 0)
	    ಸ್ಥಳ = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(fr, curfr->fr_ಮುಂದಿನ)
	    ಸ್ಥಳ += fr->fr_ಎತ್ತರ - ಚೌಕಟ್ಟು_minheight(fr, ಖಾಲಿ);
	fr = curfr;			// put fr at window that grows
    }

    ಆದರೆ (ಸ್ಥಳ < ಸ್ಥಾನಾಂತರ)		// Not enough room
	ಸ್ಥಾನಾಂತರ = ಸ್ಥಳ;		// Move as far as we can
    ಆದರೆ (ಸ್ಥಾನಾಂತರ <= 0)
	ಹಿಂತಿರುಗಿ;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    ಆದರೆ (fr != ಖಾಲಿ)
	ಚೌಕಟ್ಟು_new_ಎತ್ತರ(fr, fr->fr_ಎತ್ತರ + ಸ್ಥಾನಾಂತರ, ಮೇಲೆ, FALSE, TRUE);

    ಆದರೆ (ಮೇಲೆ)
	fr = curfr;		// current frame gets smaller
    ಇಲ್ಲದಿದ್ದರೆ
	fr = curfr->fr_ಮುಂದಿನ;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    ಆಗ (fr != ಖಾಲಿ && ಸ್ಥಾನಾಂತರ > 0)
    {
	n = ಚೌಕಟ್ಟು_minheight(fr, ಖಾಲಿ);
	ಆದರೆ (fr->fr_ಎತ್ತರ - ಸ್ಥಾನಾಂತರ <= n)
	{
	    ಸ್ಥಾನಾಂತರ -= fr->fr_ಎತ್ತರ - n;
	    ಚೌಕಟ್ಟು_new_ಎತ್ತರ(fr, n, !ಮೇಲೆ, FALSE, TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಚೌಕಟ್ಟು_new_ಎತ್ತರ(fr, fr->fr_ಎತ್ತರ - ಸ್ಥಾನಾಂತರ, !ಮೇಲೆ, FALSE, TRUE);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (ಮೇಲೆ)
	    fr = fr->fr_prev;
	ಇಲ್ಲದಿದ್ದರೆ
	    fr = fr->fr_ಮುಂದಿನ;
    }
    win_comp_pos();
    win_fix_scroll(TRUE);

    redraw_all_later(UPD_SOME_VALID);
    showmode();
}

/*
 * Separator line of dragwin is dragged "offset" lines right (negative is left).
 */
    ಶೂನ್ಯ
win_drag_vsep_ಸಾಲು(win_T *dragwin, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    ಚೌಕಟ್ಟು_T	*curfr;
    ಚೌಕಟ್ಟು_T	*fr;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ;
    ಪೂರ್ಣಾಂಕ		ಎಡ_2;	// if TRUE, drag separator line left, otherwise right
    ಪೂರ್ಣಾಂಕ		n;

    fr = dragwin->w_ಚೌಕಟ್ಟು;
    ಆದರೆ (fr == topframe)		// only one window (cannot happen?)
	ಹಿಂತಿರುಗಿ;
    curfr = fr;
    fr = fr->fr_ಪೋಷಕ;
    // When the parent frame is not a row of frames, its parent should be.
    ಆದರೆ (fr->fr_layout != FR_ಅಡ್ಡಸಾಲು)
    {
	ಆದರೆ (fr == topframe)	// only a column of windows (cannot happen?)
	    ಹಿಂತಿರುಗಿ;
	curfr = fr;
	fr = fr->fr_ಪೋಷಕ;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    ಆಗ (curfr->fr_ಮುಂದಿನ == ಖಾಲಿ)
    {
	ಆದರೆ (fr == topframe)
	    ನಿಲ್ಲಿಸು;
	curfr = fr;
	fr = fr->fr_ಪೋಷಕ;
	ಆದರೆ (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_ಪೋಷಕ;
	}
    }

    ಆದರೆ (ಸ್ಥಾನಾಂತರ < 0) // drag left
    {
	ಎಡ_2 = TRUE;
	ಸ್ಥಾನಾಂತರ = -ಸ್ಥಾನಾಂತರ;
	// sum up the room of the current frame and left of it
	ಸ್ಥಳ = 0;
	ಗಾಗಿ (fr = fr->fr_ಮಗು; ; fr = fr->fr_ಮುಂದಿನ)
	{
	    ಸ್ಥಳ += fr->fr_ಅಗಲ - ಚೌಕಟ್ಟು_minwidth(fr, ಖಾಲಿ);
	    ಆದರೆ (fr == curfr)
		ನಿಲ್ಲಿಸು;
	}
	fr = curfr->fr_ಮುಂದಿನ;		// put fr at frame that grows
    }
    ಇಲ್ಲದಿದ್ದರೆ    // drag right
    {
	ಎಡ_2 = FALSE;
	// sum up the room of frames right of the current one
	ಸ್ಥಳ = 0;
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(fr, curfr->fr_ಮುಂದಿನ)
	    ಸ್ಥಳ += fr->fr_ಅಗಲ - ಚೌಕಟ್ಟು_minwidth(fr, ಖಾಲಿ);
	fr = curfr;			// put fr at window that grows
    }

    ಆದರೆ (ಸ್ಥಳ < ಸ್ಥಾನಾಂತರ)		// Not enough room
	ಸ್ಥಾನಾಂತರ = ಸ್ಥಳ;		// Move as far as we can
    ಆದರೆ (ಸ್ಥಾನಾಂತರ <= 0)		// No room at all, quit.
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (fr == ಖಾಲಿ)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	ಹಿಂತಿರುಗಿ;

    // grow frame fr by offset lines
    ಚೌಕಟ್ಟು_new_ಅಗಲ(fr, fr->fr_ಅಗಲ + ಸ್ಥಾನಾಂತರ, ಎಡ_2, FALSE);

    // shrink other frames: current and at the left or at the right
    ಆದರೆ (ಎಡ_2)
	fr = curfr;		// current frame gets smaller
    ಇಲ್ಲದಿದ್ದರೆ
	fr = curfr->fr_ಮುಂದಿನ;	// next frame gets smaller

    ಆಗ (fr != ಖಾಲಿ && ಸ್ಥಾನಾಂತರ > 0)
    {
	n = ಚೌಕಟ್ಟು_minwidth(fr, ಖಾಲಿ);
	ಆದರೆ (fr->fr_ಅಗಲ - ಸ್ಥಾನಾಂತರ <= n)
	{
	    ಸ್ಥಾನಾಂತರ -= fr->fr_ಅಗಲ - n;
	    ಚೌಕಟ್ಟು_new_ಅಗಲ(fr, n, !ಎಡ_2, FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಚೌಕಟ್ಟು_new_ಅಗಲ(fr, fr->fr_ಅಗಲ - ಸ್ಥಾನಾಂತರ, !ಎಡ_2, FALSE);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (ಎಡ_2)
	    fr = fr->fr_prev;
	ಇಲ್ಲದಿದ್ದರೆ
	    fr = fr->fr_ಮುಂದಿನ;
    }
    win_comp_pos();
    redraw_all_later(UPD_NOT_VALID);
}

#ವ್ಯಾಖ್ಯಾನಿಸು FRACTION_MULT	16384L

/*
 * Set wp->w_fraction for the current w_wrow and w_height.
 * Has no effect when the window is less than two lines.
 */
    ಶೂನ್ಯ
ಗಣ_fraction(win_T *wp)
{
    ಆದರೆ (wp->w_ಎತ್ತರ > 1)
	// When cursor is in the first line the percentage is computed as if
	// it's halfway that line.  Thus with two lines it is 25%, with three
	// lines 17%, etc.  Similarly for the last line: 75%, 83%, etc.
	wp->w_fraction = ((ದೀರ್ಘ)wp->w_wrow * FRACTION_MULT
				     + FRACTION_MULT / 2) / (ದೀರ್ಘ)wp->w_ಎತ್ತರ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
win_fix_scroll(ಪೂರ್ಣಾಂಕ resize)
{
    ಆದರೆ (*p_spk == 'c')
	ಹಿಂತಿರುಗಿ;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	ಆದರೆ (wp->w_ಎತ್ತರ != wp->w_prev_ಎತ್ತರ)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    ಆದರೆ (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		ಪೂರ್ಣಾಂಕ diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_ಎತ್ತರ - wp->w_prev_ಎತ್ತರ);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		ಆದರೆ (diff > 0)
		    cursor_ಕೆಳಕ್ಕೆ_inner(wp, diff);
		ಇಲ್ಲದಿದ್ದರೆ
		    cursor_ಮೇಲೆ_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_ಗಮ್ಯ_fraction(wp, wp->w_prev_ಎತ್ತರ);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		ಆದರೆ (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_ಎತ್ತರ = wp->w_ಎತ್ತರ;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    ಆದರೆ (!(get_real_state() & (ಬಹುಲಕ_ಸಾಮಾನ್ಯ|ಬಹುಲಕ_CMDLINE|ಬಹುಲಕ_TERMINAL)))
	win_fix_cursor(FALSE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_fix_cursor(ಪೂರ್ಣಾಂಕ ಸಾಮಾನ್ಯ_4)
{
    win_T	*wp = curwin;

    ಆದರೆ (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ < wp->w_ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    ದೀರ್ಘ so = MIN(wp->w_ಎತ್ತರ / 2, get_scrolloff_ಮೌಲ್ಯ());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_ಕೆಳಕ್ಕೆ_inner(wp, so);
    linenr_T ತುದಿ = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_ಮೇಲೆ_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    ಆದರೆ (lnum > bot && (wp->w_botline - wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ) != 1)
	nlnum = bot;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum < ತುದಿ && wp->w_topline != 1)
	nlnum = (so == wp->w_ಎತ್ತರ / 2) ? bot : ತುದಿ;

    ಆದರೆ (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	ಆದರೆ (ಸಾಮಾನ್ಯ_4)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	ಇಲ್ಲದಿದ್ದರೆ  // Scroll instead when not in normal mode.
	{
	    wp->w_fraction = (nlnum == bot) ? FRACTION_MULT : 0;
	    scroll_ಗಮ್ಯ_fraction(wp, wp->w_prev_ಎತ್ತರ);
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
    ಶೂನ್ಯ
win_new_ಎತ್ತರ(win_T *wp, ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    ಪೂರ್ಣಾಂಕ		prev_ಎತ್ತರ = wp->w_ಎತ್ತರ;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    ಆದರೆ (ಎತ್ತರ < 0)
	ಎತ್ತರ = 0;
    ಆದರೆ (wp->w_ಎತ್ತರ == ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ;	    // nothing to do

    ಆದರೆ (wp->w_ಎತ್ತರ > 0)
    {
	ಆದರೆ (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	ಆದರೆ (wp->w_ಎತ್ತರ != prev_ಎತ್ತರ)
	    ಹಿಂತಿರುಗಿ;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	ಆದರೆ (wp->w_wrow != wp->w_prev_fraction_ಅಡ್ಡಸಾಲು)
	    ಗಣ_fraction(wp);
    }

    wp->w_ಎತ್ತರ = ಎತ್ತರ;
    wp->w_redr_ಸ್ಥಿತಿ = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    ಆದರೆ (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	ಆದರೆ (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_ಗಮ್ಯ_fraction(wp, prev_ಎತ್ತರ);
    }
}

    ಶೂನ್ಯ
scroll_ಗಮ್ಯ_fraction(win_T *wp, ಪೂರ್ಣಾಂಕ prev_ಎತ್ತರ)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		sline, ಸಾಲು_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ = wp->w_ಎತ್ತರ;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    ಆದರೆ (ಎತ್ತರ > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (ಎತ್ತರ < wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	ಆದರೆ (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((ದೀರ್ಘ)wp->w_fraction * (ದೀರ್ಘ)ಎತ್ತರ - 1L)
							       / FRACTION_MULT;
	ಸಾಲು_ಗಾತ್ರ = plines_win_ಲಂಬಸಾಲು(wp, lnum, (ದೀರ್ಘ)(wp->w_cursor.ಲಂಬಸಾಲು)) - 1;
	sline = wp->w_wrow - ಸಾಲು_ಗಾತ್ರ;

	ಆದರೆ (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2 = plines_win(wp, lnum, FALSE);

	    ಆದರೆ (sline > wp->w_ಎತ್ತರ - ಅಡ್ಡಸಾಲುಗಳು_2)
	    {
		sline = wp->w_ಎತ್ತರ - ಅಡ್ಡಸಾಲುಗಳು_2;
		wp->w_wrow -= ಅಡ್ಡಸಾಲುಗಳು_2 - ಸಾಲು_ಗಾತ್ರ;
	    }
	}

	ಆದರೆ (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    ಪೂರ್ಣಾಂಕ		want_ಅಡ್ಡಸಾಲು = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = ಸಾಲು_ಗಾತ್ರ;
	    ಆದರೆ (wp->w_wrow >= wp->w_ಎತ್ತರ
				       && (wp->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(wp);

		--wp->w_wrow;
		ಆಗ (wp->w_wrow > want_ಅಡ್ಡಸಾಲು)
		{
		    skipcol += wp->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(wp) + win_ಲಂಬಸಾಲು_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sline > 0)
	{
	    ಆಗ (sline > 0 && lnum > 1)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, ಖಾಲಿ, TRUE, ಖಾಲಿ);
		ಆದರೆ (lnum == 1)
		{
		    // first line in buffer is folded
		    ಸಾಲು_ಗಾತ್ರ = 1;
		    --sline;
		    ನಿಲ್ಲಿಸು;
		}
#ಆದರೆ_ಅಂತ್ಯ
		--lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		ಆದರೆ (lnum == wp->w_topline)
		    ಸಾಲು_ಗಾತ್ರ = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    ಸಾಲು_ಗಾತ್ರ = plines_win(wp, lnum, TRUE);
		sline -= ಸಾಲು_ಗಾತ್ರ;
	    }

	    ಆದರೆ (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		hasFoldingWin(wp, lnum, ಖಾಲಿ, &lnum, TRUE, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
		lnum++;
		wp->w_wrow -= ಸಾಲು_ಗಾತ್ರ + sline;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	ಗಣ_topline(wp, lnum);
    }

    ಆದರೆ (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    ಆದರೆ (prev_ಎತ್ತರ > 0)
	wp->w_prev_fraction_ಅಡ್ಡಸಾಲು = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    ಶೂನ್ಯ
win_new_ಅಗಲ(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
    // Should we give an error if width < 0?
    wp->w_ಅಗಲ = ಅಗಲ < 0 ? 0 : ಅಗಲ;
    wp->w_ಸಾಲುಗಳು_valid = 0;
    ಬದಲಾಯಿತು_ಸಾಲು_abv_curs_win(wp);
    invalidate_botline_win(wp);

    ಆದರೆ (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_ಸ್ಥಿತಿ = true;
}

    ಶೂನ್ಯ
win_comp_scroll(win_T *wp)
{
#ಆದರೆ defined(FEAT_EVAL)
    ಪೂರ್ಣಾಂಕ old_w_p_scr = wp->w_p_scr;
#ಆದರೆ_ಅಂತ್ಯ

    wp->w_p_scr = ((ಚಿಹ್ನೆರಹಿತ)wp->w_ಎತ್ತರ >> 1);
    ಆದರೆ (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (wp->w_p_scr != old_w_p_scr)
    {
	// Used by "verbose set scroll".
	wp->w_p_script_ctx[WV_SCROLL].sc_sid = SID_WINLAYOUT;
	wp->w_p_script_ctx[WV_SCROLL].sc_lnum = 0;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Command_height: called whenever p_ch has been changed.
 */
    ಶೂನ್ಯ
command_ಎತ್ತರ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		old_p_ch = curtab->tp_ch_ಬಳಸಿದ;

    // Find bottom frame with width of screen.
    ಚೌಕಟ್ಟು_T *frp = lastwin->w_ಚೌಕಟ್ಟು;
    ಆಗ (frp->fr_ಅಗಲ != topframe->fr_ಅಗಲ && frp->fr_ಪೋಷಕ != ಖಾಲಿ)
	frp = frp->fr_ಪೋಷಕ;

    // Avoid changing the height of a window with 'winfixheight' set.
    ಆಗ (frp->fr_prev != ಖಾಲಿ && frp->fr_layout == FR_ಎಲೆ
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    ಆಗ (p_ch > old_p_ch && command_ಚೌಕಟ್ಟು_ಎತ್ತರ)
    {
	ಆದರೆ (frp == ಖಾಲಿ)
	{
	    emsg(_(e_not_enough_ಸ್ಥಳ));
	    p_ch = old_p_ch;
	    ನಿಲ್ಲಿಸು;
	}
	ಪೂರ್ಣಾಂಕ h = MIN(p_ch - old_p_ch,
			frp->fr_ಎತ್ತರ - ಚೌಕಟ್ಟು_minheight(frp, ಖಾಲಿ));
	ಚೌಕಟ್ಟು_ಕೂಡಿಸು_ಎತ್ತರ(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    ಆದರೆ (p_ch < old_p_ch && command_ಚೌಕಟ್ಟು_ಎತ್ತರ && frp != ಖಾಲಿ)
	ಚೌಕಟ್ಟು_ಕೂಡಿಸು_ಎತ್ತರ(frp, (ಪೂರ್ಣಾಂಕ)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    ಆದರೆ (ಸಂದೇಶ_scrolled == 0 && ತುಂಬಿದ_screen)
    {
	screen_fill(cmdline_ಅಡ್ಡಸಾಲು, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, 0, (ಪೂರ್ಣಾಂಕ)Columns, ' ', ' ', 0);
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು;
    }

    // Use the value of p_ch that we remembered.  This is needed for when the
    // GUI starts up, we can't be sure in what order things happen.  And when
    // p_ch was changed in another tab page.
    curtab->tp_ch_ಬಳಸಿದ = p_ch;
    min_ಗಣ_ch = p_ch;
}

/*
 * Resize frame "frp" to be "n" lines higher (negative for less high).
 * Also resize the frames it is contained in.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_ಕೂಡಿಸು_ಎತ್ತರ(ಚೌಕಟ್ಟು_T *frp, ಪೂರ್ಣಾಂಕ n)
{
    ಚೌಕಟ್ಟು_new_ಎತ್ತರ(frp, frp->fr_ಎತ್ತರ + n, FALSE, FALSE, FALSE);
    ಗಾಗಿ (;;)
    {
	frp = frp->fr_ಪೋಷಕ;
	ಆದರೆ (frp == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	frp->fr_ಎತ್ತರ += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    ಶೂನ್ಯ
ಕೊನೆಯ_ಸ್ಥಿತಿ(
    ಪೂರ್ಣಾಂಕ		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    ಕೊನೆಯ_ಸ್ಥಿತಿ_rec(topframe, ಕೊನೆಯ_stl_ಎತ್ತರ(morewin) > 0);
#ಆದರೆ defined(FEAT_STL_OPT)
    ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೊನೆಯ_ಸ್ಥಿತಿ_rec(ಚೌಕಟ್ಟು_T *fr, ಪೂರ್ಣಾಂಕ statusline)
{
    ಚೌಕಟ್ಟು_T	*fp;
    win_T	*wp;

    ಆದರೆ (fr->fr_layout == FR_ಎಲೆ)
    {
	wp = fr->fr_win;
	ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ != 0 && !statusline)
	{
	    // remove status line
	    win_new_ಎತ್ತರ(wp, wp->w_ಎತ್ತರ + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ);
	    wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = 0;
	    comp_ಲಂಬಸಾಲು();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    ಆಗ (fp->fr_ಎತ್ತರ <= ಚೌಕಟ್ಟು_minheight(fp, ಖಾಲಿ))
	    {
		ಆದರೆ (fp == topframe)
		{
		    emsg(_(e_not_enough_ಸ್ಥಳ));
		    ಹಿಂತಿರುಗಿ;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		ಆದರೆ (fp->fr_ಪೋಷಕ->fr_layout == FR_ಲಂಬಸಾಲು && fp->fr_prev != ಖಾಲಿ)
		    fp = fp->fr_prev;
		ಇಲ್ಲದಿದ್ದರೆ
		    fp = fp->fr_ಪೋಷಕ;
	    }
	    wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = statusline_ಎತ್ತರ(wp);
	    ಆದರೆ (fp != fr)
	    {
		ಚೌಕಟ್ಟು_new_ಎತ್ತರ(fp, fp->fr_ಎತ್ತರ - wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ,
			FALSE, FALSE, FALSE);
		ಚೌಕಟ್ಟು_fix_ಎತ್ತರ(wp);
		win_comp_pos();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		win_new_ಎತ್ತರ(wp, wp->w_ಎತ್ತರ - wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ);
	    comp_ಲಂಬಸಾಲು();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	ಆದರೆ (ನಿರಪೇಕ್ಷ(wp->w_ಎತ್ತರ - wp->w_prev_ಎತ್ತರ) == 1)
	    wp->w_prev_ಎತ್ತರ = wp->w_ಎತ್ತರ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fr->fr_layout == FR_ಅಡ್ಡಸಾಲು)
    {
	// vertically split windows, set status line for each one
	ಆದರೆ (!statusline)
	{
	    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(fp, fr->fr_ಮಗು)
		ಕೊನೆಯ_ಸ್ಥಿತಿ_rec(fp, statusline);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಚೌಕಟ್ಟು_T	*fp2;
	    ಪೂರ್ಣಾಂಕ		max_stlh = 0;
	    ಪೂರ್ಣಾಂಕ		new_ಅಡ್ಡಸಾಲು_ಎತ್ತರ;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(fp, fr->fr_ಮಗು)
		ಆದರೆ (fp->fr_win != ಖಾಲಿ && fp->fr_win->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0)
		{
		    ಪೂರ್ಣಾಂಕ h = statusline_ಎತ್ತರ(fp->fr_win);
		    ಆದರೆ (h > max_stlh)
			max_stlh = h;
		}
	    ಆದರೆ (max_stlh == 0)
		ಹಿಂತಿರುಗಿ;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    ಆಗ (fp2->fr_ಎತ್ತರ - ಚೌಕಟ್ಟು_minheight(fp2, ಖಾಲಿ) < max_stlh)
	    {
		ಆದರೆ (fp2 == topframe)
		{
		    emsg(_(e_not_enough_ಸ್ಥಳ));
		    ಹಿಂತಿರುಗಿ;
		}
		ಆದರೆ (fp2->fr_ಪೋಷಕ->fr_layout == FR_ಲಂಬಸಾಲು
						&& fp2->fr_prev != ಖಾಲಿ)
		    fp2 = fp2->fr_prev;
		ಇಲ್ಲದಿದ್ದರೆ
		    fp2 = fp2->fr_ಪೋಷಕ;
	    }

	    ಆದರೆ (fp2 != fr)
	    {
		ಚೌಕಟ್ಟು_new_ಎತ್ತರ(fp2, fp2->fr_ಎತ್ತರ - max_stlh,
							FALSE, FALSE, FALSE);
		new_ಅಡ್ಡಸಾಲು_ಎತ್ತರ = fr->fr_ಎತ್ತರ + max_stlh;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		new_ಅಡ್ಡಸಾಲು_ಎತ್ತರ = fr->fr_ಎತ್ತರ;

	    // Set status and content heights for all leaves.
	    FOR_ALL_ಚೌಕಟ್ಟುಗಳು(fp, fr->fr_ಮಗು)
	    {
		ಆದರೆ (fp->fr_win != ಖಾಲಿ)
		{
		    wp = fp->fr_win;
		    ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0)
		    {
			wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = statusline_ಎತ್ತರ(wp);
			win_new_ಎತ್ತರ(wp,
				  new_ಅಡ್ಡಸಾಲು_ಎತ್ತರ - wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ);
			ಚೌಕಟ್ಟು_fix_ಎತ್ತರ(wp);
			comp_ಲಂಬಸಾಲು();
			redraw_all_later(UPD_SOME_VALID);
			ಆದರೆ (ನಿರಪೇಕ್ಷ(wp->w_ಎತ್ತರ - wp->w_prev_ಎತ್ತರ) == 1)
			    wp->w_prev_ಎತ್ತರ = wp->w_ಎತ್ತರ;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಕೊನೆಯ_ಸ್ಥಿತಿ_rec(fp, statusline); // nested frames
	    }

	    fr->fr_ಎತ್ತರ = new_ಅಡ್ಡಸಾಲು_ಎತ್ತರ;
	    win_comp_pos();
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// horizontally split window, set status line for last one
	ಗಾಗಿ (fp = fr->fr_ಮಗು; fp->fr_ಮುಂದಿನ != ಖಾಲಿ; fp = fp->fr_ಮುಂದಿನ)
	    ;
	ಕೊನೆಯ_ಸ್ಥಿತಿ_rec(fp, statusline);
    }
}

#ಆದರೆ defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    ಶೂನ್ಯ
update_stl_rendered_ಎತ್ತರ(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == ಖಾಲಿ) ? p_stl : wp->w_p_stl;
    ಪೂರ್ಣಾಂಕ		opt_flags = (wp == ಖಾಲಿ) ? 0 : OPT_LOCAL;
    ಪೂರ್ಣಾಂಕ		*ಎತ್ತರ = (wp == ಖಾಲಿ) ? &stl_rendered_ಎತ್ತರ
						: &wp->w_stl_rendered_ಎತ್ತರ;

    *ಎತ್ತರ = get_stl_rendered_ಎತ್ತರ(wp == ಖಾಲಿ ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಚೌಕಟ್ಟು_ಕಂಡುಹಿಡಿ_global_stlh_rec(ಚೌಕಟ್ಟು_T *frp, ಪೂರ್ಣಾಂಕ h)
{
    ಆದರೆ (frp->fr_layout == FR_ಎಲೆ)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	ಆದರೆ (wp->w_ಎತ್ತರ > 0 && wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ > 0
		&& *wp->w_p_stlo == NUL)
	{
	    ಪೂರ್ಣಾಂಕ win_free_ಎತ್ತರ = frp->fr_ಎತ್ತರ - WINBAR_ಎತ್ತರ(wp);

	    ಆದರೆ (win_free_ಎತ್ತರ - p_wmh < h)
		h = win_free_ಎತ್ತರ - p_wmh;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (frp = frp->fr_ಮಗು; frp != ಖಾಲಿ; frp = frp->fr_ಮುಂದಿನ)
	    h = ಚೌಕಟ್ಟು_ಕಂಡುಹಿಡಿ_global_stlh_rec(frp, h);
    }
    ಹಿಂತಿರುಗಿ h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ_rec(ಚೌಕಟ್ಟು_T *frp, ಪೂರ್ಣಾಂಕ global_stlh)
{
    ಆದರೆ (frp->fr_layout == FR_ಎಲೆ)
    {
	win_T  *wp = frp->fr_win;

	ಆದರೆ (wp->w_ಎತ್ತರ > 0 && wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ > 0)
	{
	    ಪೂರ್ಣಾಂಕ win_free_ಎತ್ತರ = frp->fr_ಎತ್ತರ - WINBAR_ಎತ್ತರ(wp);

	    wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = statusline_ಎತ್ತರ(wp);
	    ಆದರೆ (wp->w_p_stlo == ಖಾಲಿ || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ > global_stlh)
		    wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = global_stlh;
	    }
	    ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ > win_free_ಎತ್ತರ - p_wmh)
		wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ = win_free_ಎತ್ತರ - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    ಪೂರ್ಣಾಂಕ	     saved_topfill = wp->w_topfill;
# endif
	    win_new_ಎತ್ತರ(wp, win_free_ಎತ್ತರ - wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ);
	    wp->w_topline = saved_topline;
# ifdef FEAT_DIFF
	    wp->w_topfill = saved_topfill;
# endif
	    wp->w_valid &= ~(VALID_WROW | VALID_CROW
			     | VALID_BOTLINE | VALID_BOTLINE_AP);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (frp = frp->fr_ಮಗು; frp != ಖಾಲಿ; frp = frp->fr_ಮುಂದಿನ)
	    ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    ಪೂರ್ಣಾಂಕ
ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ(ಶೂನ್ಯ)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = ಚೌಕಟ್ಟು_ಕಂಡುಹಿಡಿ_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ_rec(tp->tp_topframe, global_stlh);
    comp_ಲಂಬಸಾಲು();
    redraw_all_later(UPD_SOME_VALID);
    ಹಿಂತಿರುಗಿ global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    ಶೂನ್ಯ
ಗಣ_stlo_mh(ಪೂರ್ಣಾಂಕ mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
statuslineopt_ಬದಲಾಯಿತು(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		l_stlo_fh = FALSE;
    ಪೂರ್ಣಾಂಕ		l_stlo_mh = 1;

    ಆದರೆ (stlopt != ಖಾಲಿ)
	p = stlopt;
    ಇಲ್ಲದಿದ್ದರೆ
	p = wp->w_p_stlo;

    ಆಗ (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	ಆದರೆ (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    ಆದರೆ (l_stlo_mh < 1)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	ಆದರೆ (*p != ',' && *p != NUL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (*p == ',')
	    ++p;
    }

    ಆದರೆ (wp == ಖಾಲಿ)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    ಪೂರ್ಣಾಂಕ
statusline_ಎತ್ತರ(win_T *wp ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ stl_ಎತ್ತರ = 1;
#ಆದರೆ defined(FEAT_STL_OPT)
    bool fixed;
    ಪೂರ್ಣಾಂಕ l_stl_rendered_ಎತ್ತರ;

    ಆದರೆ (wp != ಖಾಲಿ && *wp->w_p_stlo != NUL)
    {
	stl_ಎತ್ತರ = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_ಎತ್ತರ = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_ಎತ್ತರ
				    : stl_rendered_ಎತ್ತರ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	stl_ಎತ್ತರ = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_ಎತ್ತರ = (wp != ಖಾಲಿ && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_ಎತ್ತರ
				    : stl_rendered_ಎತ್ತರ;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    ಆದರೆ (!fixed)
    {
	ಪೂರ್ಣಾಂಕ has_custom_stl = (*p_stl != NUL)
	    || (wp != ಖಾಲಿ && *wp->w_p_stl != NUL);
	ಆದರೆ (has_custom_stl)
	    stl_ಎತ್ತರ = MAX(1, MIN(l_stl_rendered_ಎತ್ತರ, stl_ಎತ್ತರ));
	ಇಲ್ಲದಿದ್ದರೆ
	    stl_ಎತ್ತರ = 1;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ stl_ಎತ್ತರ;
}

/*
 * Return the number of lines used by the tab page line.
 */
    ಪೂರ್ಣಾಂಕ
tabline_ಎತ್ತರ(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    ಆದರೆ (gui_use_tabline())
	ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಆಯ್ಕೆ (p_stal)
    {
	ಸ್ಥಿತಿ 0: ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ 1: ಹಿಂತಿರುಗಿ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ == ಖಾಲಿ) ? 0 : 1;
    }
    ಹಿಂತಿರುಗಿ 1;
}

/*
 * Return the height of the last window's statusline.
 */
    ಪೂರ್ಣಾಂಕ
ಕೊನೆಯ_stl_ಎತ್ತರ(
    ಪೂರ್ಣಾಂಕ		morewin)	// pretend there are two or more windows
{
    ಹಿಂತಿರುಗಿ (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_ಕಿಟಕಿ)))
		? ಸ್ಥಿತಿ_ಎತ್ತರ_2 : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    ಪೂರ್ಣಾಂಕ
min_ಅಡ್ಡಸಾಲುಗಳು(ಶೂನ್ಯ)
{
    ಆದರೆ (firstwin == ಖಾಲಿ)	// not initialized yet
	ಹಿಂತಿರುಗಿ MIN_ಸಾಲುಗಳು;

    ಹಿಂತಿರುಗಿ ಚೌಕಟ್ಟು_minheight(curtab->tp_topframe, ಖಾಲಿ) + tabline_ಎತ್ತರ()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    ಪೂರ್ಣಾಂಕ
min_ಅಡ್ಡಸಾಲುಗಳು_for_all_tabpages(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ಒಟ್ಟು;
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (firstwin == ಖಾಲಿ)	// not initialized yet
	ಹಿಂತಿರುಗಿ MIN_ಸಾಲುಗಳು;

    ಒಟ್ಟು = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = ಚೌಕಟ್ಟು_minheight(tp->tp_topframe, ಖಾಲಿ);
	ಆದರೆ (ಒಟ್ಟು < n)
	    ಒಟ್ಟು = n;
    }
    ಒಟ್ಟು += tabline_ಎತ್ತರ();
    ಒಟ್ಟು += MIN_CMDHEIGHT;
    ಹಿಂತಿರುಗಿ ಒಟ್ಟು;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    ಪೂರ್ಣಾಂಕ
only_one_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    win_T	*wp;

#ಆದರೆ defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    ಆದರೆ (popup_is_popup(curwin))
	ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // If there is another tab page there always is another window.
    ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_ಬಫರ್ != ಖಾಲಿ
		&& (!((bt_help(wp->w_ಬಫರ್) && !bt_help(curbuf))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		    || wp->w_p_pvw
#ಆದರೆ_ಅಂತ್ಯ
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ (ಎಣಿಕೆ_2 <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_lnums_both(ಪೂರ್ಣಾಂಕ do_curwin, ಪೂರ್ಣಾಂಕ nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಆದರೆ ((do_curwin || wp != curwin) && wp->w_ಬಫರ್ == curbuf)
	{
	    ಪೂರ್ಣಾಂಕ need_adjust;

	    ಆದರೆ (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಆದರೆ (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಆದರೆ (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಆದರೆ (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಆದರೆ (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    ಶೂನ್ಯ
check_lnums(ಪೂರ್ಣಾಂಕ do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    ಶೂನ್ಯ
check_lnums_nested(ಪೂರ್ಣಾಂಕ do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    ಶೂನ್ಯ
reset_lnums(ಶೂನ್ಯ)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಆದರೆ (wp->w_ಬಫರ್ == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    ಆದರೆ (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    ಆದರೆ (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    ಆದರೆ (wp->w_save_cursor.w_topline_save > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
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
    ಪೂರ್ಣಾಂಕ
make_snapshot(ಪೂರ್ಣಾಂಕ idx)
{
    clear_snapshot(curtab, idx);
    ಆದರೆ (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
make_snapshot_rec(ಚೌಕಟ್ಟು_T *fr, ಚೌಕಟ್ಟು_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(ಚೌಕಟ್ಟು_T);
    ಆದರೆ (*frp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_ಅಗಲ = fr->fr_ಅಗಲ;
    (*frp)->fr_ಎತ್ತರ = fr->fr_ಎತ್ತರ;
    ಆದರೆ (fr->fr_ಮುಂದಿನ != ಖಾಲಿ)
    {
	ಆದರೆ (make_snapshot_rec(fr->fr_ಮುಂದಿನ, &((*frp)->fr_ಮುಂದಿನ)) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (fr->fr_ಮಗು != ಖಾಲಿ)
    {
	ಆದರೆ (make_snapshot_rec(fr->fr_ಮಗು, &((*frp)->fr_ಮಗು)) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (fr->fr_layout == FR_ಎಲೆ && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Remove any existing snapshot.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_snapshot(tabpage_T *tp, ಪೂರ್ಣಾಂಕ idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_snapshot_rec(ಚೌಕಟ್ಟು_T *fr)
{
    ಆದರೆ (fr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    clear_snapshot_rec(fr->fr_ಮುಂದಿನ);
    clear_snapshot_rec(fr->fr_ಮಗು);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    ಸ್ಥಿರ win_T *
get_snapshot_curwin_rec(ಚೌಕಟ್ಟು_T *ft)
{
    win_T	*wp;

    ಆದರೆ (ft->fr_ಮುಂದಿನ != ಖಾಲಿ)
    {
	ಆದರೆ ((wp = get_snapshot_curwin_rec(ft->fr_ಮುಂದಿನ)) != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ wp;
    }
    ಆದರೆ (ft->fr_ಮಗು != ಖಾಲಿ)
    {
	ಆದರೆ ((wp = get_snapshot_curwin_rec(ft->fr_ಮಗು)) != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ wp;
    }

    ಹಿಂತಿರುಗಿ ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    ಸ್ಥಿರ win_T *
get_snapshot_curwin(ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (curtab->tp_snapshot[idx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    ಶೂನ್ಯ
restore_snapshot(
    ಪೂರ್ಣಾಂಕ		idx,
    ಪೂರ್ಣಾಂಕ		close_curwin)	    // closing current window
{
    win_T	*wp;

    ಆದರೆ (curtab->tp_snapshot[idx] != ಖಾಲಿ
	    && curtab->tp_snapshot[idx]->fr_ಅಗಲ == topframe->fr_ಅಗಲ
	    && curtab->tp_snapshot[idx]->fr_ಎತ್ತರ == topframe->fr_ಎತ್ತರ
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	ಆದರೆ (wp != ಖಾಲಿ && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_snapshot_rec(ಚೌಕಟ್ಟು_T *sn, ಚೌಕಟ್ಟು_T *fr)
{
    ಆದರೆ (sn->fr_layout != fr->fr_layout
	    || (sn->fr_ಮುಂದಿನ == ಖಾಲಿ) != (fr->fr_ಮುಂದಿನ == ಖಾಲಿ)
	    || (sn->fr_ಮಗು == ಖಾಲಿ) != (fr->fr_ಮಗು == ಖಾಲಿ)
	    || (sn->fr_ಮುಂದಿನ != ಖಾಲಿ
		&& check_snapshot_rec(sn->fr_ಮುಂದಿನ, fr->fr_ಮುಂದಿನ) == FAIL)
	    || (sn->fr_ಮಗು != ಖಾಲಿ
		&& check_snapshot_rec(sn->fr_ಮಗು, fr->fr_ಮಗು) == FAIL)
	    || (sn->fr_win != ಖಾಲಿ && !win_valid(sn->fr_win)))
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    ಸ್ಥಿರ win_T *
restore_snapshot_rec(ಚೌಕಟ್ಟು_T *sn, ಚೌಕಟ್ಟು_T *fr)
{
    win_T	*wp = ಖಾಲಿ;
    win_T	*wp2;

    fr->fr_ಎತ್ತರ = sn->fr_ಎತ್ತರ;
    fr->fr_ಅಗಲ = sn->fr_ಅಗಲ;
    ಆದರೆ (fr->fr_layout == FR_ಎಲೆ)
    {
	ಚೌಕಟ್ಟು_new_ಎತ್ತರ(fr, fr->fr_ಎತ್ತರ, FALSE, FALSE, FALSE);
	ಚೌಕಟ್ಟು_new_ಅಗಲ(fr, fr->fr_ಅಗಲ, FALSE, FALSE);
	wp = sn->fr_win;
    }
    ಆದರೆ (sn->fr_ಮುಂದಿನ != ಖಾಲಿ)
    {
	wp2 = restore_snapshot_rec(sn->fr_ಮುಂದಿನ, fr->fr_ಮುಂದಿನ);
	ಆದರೆ (wp2 != ಖಾಲಿ)
	    wp = wp2;
    }
    ಆದರೆ (sn->fr_ಮಗು != ಖಾಲಿ)
    {
	wp2 = restore_snapshot_rec(sn->fr_ಮಗು, fr->fr_ಮಗು);
	ಆದರೆ (wp2 != ಖಾಲಿ)
	    wp = wp2;
    }
    ಹಿಂತಿರುಗಿ wp;
}

#ಆದರೆ defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    ಪೂರ್ಣಾಂಕ
win_hasvertsplit(ಶೂನ್ಯ)
{
    ಚೌಕಟ್ಟು_T	*fr;

    ಆದರೆ (topframe->fr_layout == FR_ಅಡ್ಡಸಾಲು)
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (topframe->fr_layout == FR_ಲಂಬಸಾಲು)
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(fr, topframe->fr_ಮಗು)
	    ಆದರೆ (fr->fr_layout == FR_ಅಡ್ಡಸಾಲು)
		ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    ಪೂರ್ಣಾಂಕ
get_win_ಸಂಖ್ಯೆ(win_T *wp, win_T *ಮೊದಲ_win)
{
    ಪೂರ್ಣಾಂಕ		i = 1;
    win_T	*w;

    ಗಾಗಿ (w = ಮೊದಲ_win; w != ಖಾಲಿ && w != wp; w = W_ಮುಂದಿನ(w))
	++i;

    ಆದರೆ (w == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ i;
}

    ಪೂರ್ಣಾಂಕ
get_tab_ಸಂಖ್ಯೆ(tabpage_T *tp)
{
    ಪೂರ್ಣಾಂಕ		i = 1;
    tabpage_T	*t;

    ಗಾಗಿ (t = ಮೊದಲ_tabpage; t != ಖಾಲಿ && t != tp; t = t->tp_ಮುಂದಿನ)
	++i;

    ಆದರೆ (t == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ i;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಚೌಕಟ್ಟು_check_ಎತ್ತರ(ಚೌಕಟ್ಟು_T *topfrp, ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    ಚೌಕಟ್ಟು_T *frp;

    ಆದರೆ (topfrp->fr_ಎತ್ತರ != ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (topfrp->fr_layout == FR_ಅಡ್ಡಸಾಲು)
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, topfrp->fr_ಮಗು)
	    ಆದರೆ (frp->fr_ಎತ್ತರ != ಎತ್ತರ)
		ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಚೌಕಟ್ಟು_check_ಅಗಲ(ಚೌಕಟ್ಟು_T *topfrp, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
    ಚೌಕಟ್ಟು_T *frp;

    ಆದರೆ (topfrp->fr_ಅಗಲ != ಅಗಲ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (topfrp->fr_layout == FR_ಲಂಬಸಾಲು)
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, topfrp->fr_ಮಗು)
	    ಆದರೆ (frp->fr_ಅಗಲ != ಅಗಲ)
		ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
}

#ಆದರೆ defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
int_cmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *pa, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *pb)
{
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ a = *(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *)pa;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ b = *(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *)pb;
    ಆದರೆ (a > b)
	ಹಿಂತಿರುಗಿ 1;
    ಆದರೆ (a < b)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    ಅಕ್ಷರ *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = ಖಾಲಿಗಳು_option;
    ಪೂರ್ಣಾಂಕ		tw;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    ಪೂರ್ಣಾಂಕ		color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[256];
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		j = 0;

    ಆದರೆ (wp != ಖಾಲಿ && wp->w_ಬಫರ್ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;  // buffer was closed

    ಆದರೆ (cc != ಖಾಲಿ)
	s = cc;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp != ಖಾಲಿ)
	s = wp->w_p_cc;

    ಆದರೆ (wp != ಖಾಲಿ)
	tw = wp->w_ಬಫರ್->b_p_tw;
    ಇಲ್ಲದಿದ್ದರೆ
	// buffer-local value not set, assume zero
	tw = 0;

    ಆಗ (*s != NUL && ಎಣಿಕೆ_2 < 255)
    {
	ಆದರೆ (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    ಲಂಬಸಾಲು = (*s == '-') ? -1 : 1;
	    ++s;
	    ಆದರೆ (!VIM_ISDIGIT(*s))
		ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	    ಲಂಬಸಾಲು = ಲಂಬಸಾಲು * getdigits(&s);
	    ಆದರೆ (tw == 0)
		ಹೋಗು skip;  // 'textwidth' not set, skip this item
	    ಲಂಬಸಾಲು += tw;
	    ಆದರೆ (ಲಂಬಸಾಲು < 0)
		ಹೋಗು skip;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISDIGIT(*s))
	    ಲಂಬಸಾಲು = getdigits(&s);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[ಎಣಿಕೆ_2++] = ಲಂಬಸಾಲು - 1;  // 1-based to 0-based
skip:
	ಆದರೆ (*s == NUL)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (*s != ',')
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	ಆದರೆ (*++s == NUL)
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;  // illegal trailing comma as in "set cc=80,"
    }

    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;  // only parse "cc"

    vim_free(wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, ಎಣಿಕೆ_2 + 1);
	ಆದರೆ (wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != ಖಾಲಿ)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಎಣಿಕೆ_2, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ), int_cmp);

	    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
		// skip duplicates
		ಆದರೆ (j == 0 || wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[j - 1] != color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[i])
		    wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[j++] = color_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[i];
	    wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[j] = -1;  // end marker
	}
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;  // no error
}
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
get_ಕೊನೆಯ_winid(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ಕೊನೆಯ_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   ಪೂರ್ಣಾಂಕ
win_locked(win_T *wp)
{
    ಹಿಂತಿರುಗಿ wp->w_locked;
}
