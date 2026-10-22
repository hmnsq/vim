/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ ಶೂನ್ಯ cmd_with_ಎಣಿಕೆ(ವರ್ಣ *cmd, char_u *bufp, size_t bufsize, long Prenum);
ಸ್ಥಿರ ಶೂನ್ಯ win_init(win_T *newp, win_T *oldp, ಪೂರ್ಣಾಙ್ಕ flags);
ಸ್ಥಿರ ಶೂನ್ಯ win_init_some(win_T *newp, win_T *oldp);
ಸ್ಥಿರ ಶೂನ್ಯ frame_comp_pos(frame_T *topfrp, ಪೂರ್ಣಾಙ್ಕ *row, ಪೂರ್ಣಾಙ್ಕ *col);
ಸ್ಥಿರ ಶೂನ್ಯ frame_setheight(frame_T *curfrp, ಪೂರ್ಣಾಙ್ಕ height);
ಸ್ಥಿರ ಶೂನ್ಯ frame_setwidth(frame_T *curfrp, ಪೂರ್ಣಾಙ್ಕ width);
ಸ್ಥಿರ ಶೂನ್ಯ win_exchange(long);
ಸ್ಥಿರ ಶೂನ್ಯ win_rotate(ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಶೂನ್ಯ win_equal_rec(win_T *next_curwin, ಪೂರ್ಣಾಙ್ಕ current, frame_T *topfr, ಪೂರ್ಣಾಙ್ಕ dir, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height);
ಸ್ಥಿರ ಶೂನ್ಯ trigger_winnewpre(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ trigger_winclosed(win_T *win);
ಸ್ಥಿರ win_T *win_free_mem(win_T *win, ಪೂರ್ಣಾಙ್ಕ *dirp, tabpage_T *tp);
ಸ್ಥಿರ frame_T *win_altframe(win_T *win, tabpage_T *tp);
ಸ್ಥಿರ tabpage_T *alt_tabpage(ಶೂನ್ಯ);
ಸ್ಥಿರ win_T *frame2win(frame_T *frp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ frame_has_win(frame_T *frp, win_T *wp);
ಸ್ಥಿರ ಶೂನ್ಯ win_fix_scroll(ಪೂರ್ಣಾಙ್ಕ resize);
ಸ್ಥಿರ ಶೂನ್ಯ win_fix_cursor(ಪೂರ್ಣಾಙ್ಕ normal);
ಸ್ಥಿರ ಶೂನ್ಯ frame_new_height(frame_T *topfrp, ಪೂರ್ಣಾಙ್ಕ height, ಪೂರ್ಣಾಙ್ಕ topfirst, ಪೂರ್ಣಾಙ್ಕ wfh, ಪೂರ್ಣಾಙ್ಕ set_ch);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ frame_fixed_height(frame_T *frp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ frame_fixed_width(frame_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ frame_add_statusline(frame_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ frame_new_width(frame_T *topfrp, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ leftfirst, ಪೂರ್ಣಾಙ್ಕ wfw);
ಸ್ಥಿರ ಶೂನ್ಯ frame_set_vsep(frame_T *frp, ಪೂರ್ಣಾಙ್ಕ add);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ frame_minwidth(frame_T *topfrp, win_T *next_curwin);
ಸ್ಥಿರ ಶೂನ್ಯ frame_fix_width(win_T *wp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ win_alloc_firstwin(win_T *oldwin);
ಸ್ಥಿರ ಶೂನ್ಯ new_frame(win_T *wp);
ಸ್ಥಿರ tabpage_T *alloc_tabpage(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ leave_tabpage(buf_T *new_curbuf, ಪೂರ್ಣಾಙ್ಕ trigger_leave_autocmds);
ಸ್ಥಿರ ಶೂನ್ಯ enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, ಪೂರ್ಣಾಙ್ಕ trigger_enter_autocmds, ಪೂರ್ಣಾಙ್ಕ trigger_leave_autocmds);
ಸ್ಥಿರ ಶೂನ್ಯ frame_fix_height(win_T *wp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ frame_minheight(frame_T *topfrp, win_T *next_curwin);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ frame_wincount_in_height(frame_T *topfrp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ may_open_tabpage(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ win_enter_ext(win_T *wp, ಪೂರ್ಣಾಙ್ಕ flags);
ಸ್ಥಿರ ಶೂನ್ಯ win_free(win_T *wp, tabpage_T *tp);
ಸ್ಥಿರ ಶೂನ್ಯ win_append(win_T *after, win_T *wp);
ಸ್ಥಿರ ಶೂನ್ಯ frame_append(frame_T *after, frame_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ frame_ಸೇರಿಸು(frame_T *before, frame_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ frame_remove(frame_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ win_goto_ver(ಪೂರ್ಣಾಙ್ಕ up, long ಎಣಿಕೆ);
ಸ್ಥಿರ ಶೂನ್ಯ win_goto_hor(ಪೂರ್ಣಾಙ್ಕ ಎಡ, long ಎಣಿಕೆ);
ಸ್ಥಿರ ಶೂನ್ಯ frame_add_height(frame_T *frp, ಪೂರ್ಣಾಙ್ಕ n);
ಸ್ಥಿರ ಶೂನ್ಯ last_status_rec(frame_T *fr, ಪೂರ್ಣಾಙ್ಕ statusline);
#ಯದಿ defined(FEAT_STL_OPT)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ frame_find_global_stlh_rec(frame_T *frp, ಪೂರ್ಣಾಙ್ಕ h);
ಸ್ಥಿರ ಶೂನ್ಯ frame_change_statusline_height_rec(frame_T *frp, ಪೂರ್ಣಾಙ್ಕ global_stlh);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ frame_flatten(frame_T *frp);
ಸ್ಥಿರ ಶೂನ್ಯ winframe_restore(win_T *wp, ಪೂರ್ಣಾಙ್ಕ dir, frame_T *unflat_altfr);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ make_snapshot_rec(frame_T *fr, frame_T **frp);
ಸ್ಥಿರ ಶೂನ್ಯ clear_snapshot(tabpage_T *tp, ಪೂರ್ಣಾಙ್ಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ clear_snapshot_rec(frame_T *fr);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_snapshot_rec(frame_T *sn, frame_T *fr);
ಸ್ಥಿರ win_T *restore_snapshot_rec(frame_T *sn, frame_T *fr);
ಸ್ಥಿರ win_T *get_snapshot_curwin(ಪೂರ್ಣಾಙ್ಕ idx);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ frame_check_height(frame_T *topfrp, ಪೂರ್ಣಾಙ್ಕ height);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ frame_check_width(frame_T *topfrp, ಪೂರ್ಣಾಙ್ಕ width);

ಸ್ಥಿರ win_T *win_alloc(win_T *after, ಪೂರ್ಣಾಙ್ಕ hidden);

#define NOWIN		((win_T *)-1)	// non-existing window

#define ROWS_AVAIL (Rows - p_ch - tabline_height())

// flags for win_enter_ext()
#define WEE_UNDO_SYNC			0x01
#define WEE_CURWIN_INVALID		0x02
#define WEE_TRIGGER_NEW_AUTOCMDS	0x04
#define WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#define WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#define WEE_ALLOW_PARSE_MESSAGES	0x20

ಸ್ಥಿರ ವರ್ಣ *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ split_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ frame_locked = 0;

#ಯದಿ defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ stl_rendered_height = 1;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ stlo_mh = STATUS_HEIGHT; // "maxheight" value cache of global 'stlo'
#endif

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    ಶೂನ್ಯ
window_layout_lock(ಶೂನ್ಯ)
{
    ++split_disallowed;
    ++close_disallowed;
}

    ಶೂನ್ಯ
window_layout_unlock(ಶೂನ್ಯ)
{
    --split_disallowed;
    --close_disallowed;
}

    ಪೂರ್ಣಾಙ್ಕ
frames_locked(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ frame_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    ಪೂರ್ಣಾಙ್ಕ
window_layout_locked(ಗಣನಾ CMD_index cmd)
{
    ಯದಿ (split_disallowed > 0 || close_disallowed > 0)
    {
	ಯದಿ (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_split_window_when_closing_buffer));
	ಅನ್ಯಥಾ
	    emsg(_(e_not_allowed_to_change_window_layout_in_this_autocmd));
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

// #define WIN_DEBUG
#ifdef WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
log_frame_layout(frame_T *frame)
{
    ch_log(ರಿಕ್ತ, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    frame->fr_layout == FR_LEAF ? "LEAF"
				  : frame->fr_layout == FR_ROW ? "ROW" : "COL",
	    frame->fr_width,
	    frame->fr_height,
	    frame->fr_win == ರಿಕ್ತ ? -1 : frame->fr_win->w_width,
	    frame->fr_win == ರಿಕ್ತ ? -1 : frame->fr_win->w_height,
	    frame->fr_win == ರಿಕ್ತ ? -1 : frame->fr_win->w_id);
    ಯದಿ (frame->fr_child != ರಿಕ್ತ)
    {
	ch_log(ರಿಕ್ತ, "children");
	log_frame_layout(frame->fr_child);
	ಯದಿ (frame->fr_next != ರಿಕ್ತ)
	    ch_log(ರಿಕ್ತ, "END of children");
    }
    ಯದಿ (frame->fr_next != ರಿಕ್ತ)
	log_frame_layout(frame->fr_next);
}
#endif

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_can_set_curbuf_disabled(ಶೂನ್ಯ)
{
    ಯದಿ (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_can_set_curbuf_forceit(ಪೂರ್ಣಾಙ್ಕ forceit)
{
    ಯದಿ (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(ಶೂನ್ಯ)
{
    // In cmdwin, the alternative buffer should be used.
    ಪ್ರತ್ಯಾಗಮ is_in_cmdwin() && prevwin != ರಿಕ್ತ ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = ರಿಕ್ತ;

    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    ಯದಿ (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    ಯದಿ (wp == ರಿಕ್ತ && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    ಪ್ರತ್ಯಾಗಮ wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    ಶೂನ್ಯ
do_window(
    ಪೂರ್ಣಾಙ್ಕ		nchar,
    long	Prenum,
    ಪೂರ್ಣಾಙ್ಕ		xchar)	    // extra char from ":wincmd gx" or NUL
{
    long	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ifdef FEAT_FIND_ID
    ಪೂರ್ಣಾಙ್ಕ		type = FIND_DEFINE;
    ಪೂರ್ಣಾಙ್ಕ		len;
#endif
    char_u	cbuf[40];

    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;

#define CHECK_CMDWIN \
    ಕುರು { \
	ಯದಿ (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_in_cmdline_window)); \
	    ಪ್ರತ್ಯಾಗಮ; \
	} \
    } ಯಾವತ್ (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    ವಿಕಲ್ಪ (nchar)
    {
// split current window in two parts, horizontally
    ಪಕ್ಷ 'S':
    ಪಕ್ಷ Ctrl_S:
    ಪಕ್ಷ 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		ಯದಿ (bt_quickfix(curbuf))
		    ಗಚ್ಛ newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(ಶೂನ್ಯ)win_split((ಪೂರ್ಣಾಙ್ಕ)Prenum, 0);
		ಭಙ್ಗ;

// split current window in two parts, vertically
    ಪಕ್ಷ Ctrl_V:
    ಪಕ್ಷ 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		ಯದಿ (bt_quickfix(curbuf))
		    ಗಚ್ಛ newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(ಶೂನ್ಯ)win_split((ಪೂರ್ಣಾಙ್ಕ)Prenum, WSP_VERT);
		ಭಙ್ಗ;

// split current window and edit alternate file
    ಪಕ್ಷ Ctrl_HAT:
    ಪಕ್ಷ '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		ಯದಿ (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == ರಿಕ್ತ)
		{
		    ಯದಿ (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    ಅನ್ಯಥಾ
			semsg(_(e_buffer_nr_not_found), Prenum);
		    ಭಙ್ಗ;
		}

		ಯದಿ (!curbuf_locked() && win_split(0, 0) == OK)
		    (ಶೂನ್ಯ)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		ಭಙ್ಗ;

// open new window
    ಪಕ್ಷ Ctrl_N:
    ಪಕ್ಷ 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		ಯದಿ (Prenum)
		    // window height
		    vim_snprintf((ವರ್ಣ *)cbuf, ಪರಿಮಾಣ(cbuf) - 5, "%ld", Prenum);
		ಅನ್ಯಥಾ
		    cbuf[0] = NUL;
#ಯದಿ defined(FEAT_QUICKFIX)
		ಯದಿ (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#endif
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		ಭಙ್ಗ;

// quit current window
    ಪಕ್ಷ Ctrl_Q:
    ಪಕ್ಷ 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ಎಣಿಕೆ("quit", cbuf, ಪರಿಮಾಣ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ಭಙ್ಗ;

// close current window
    ಪಕ್ಷ Ctrl_C:
    ಪಕ್ಷ 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ಎಣಿಕೆ("close", cbuf, ಪರಿಮಾಣ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ಭಙ್ಗ;

#ಯದಿ defined(FEAT_QUICKFIX)
// close preview window
    ಪಕ್ಷ Ctrl_Z:
    ಪಕ್ಷ 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		ಭಙ್ಗ;

// cursor to preview window
    ಪಕ್ಷ 'P':
		FOR_ALL_WINDOWS(wp)
		    ಯದಿ (wp->w_p_pvw)
			ಭಙ್ಗ;
		ಯದಿ (wp == ರಿಕ್ತ)
		    emsg(_(e_there_is_no_preview_window));
		ಅನ್ಯಥಾ
		    win_goto(wp);
		ಭಙ್ಗ;
#endif

// close all but current window
    ಪಕ್ಷ Ctrl_O:
    ಪಕ್ಷ 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ಎಣಿಕೆ("only", cbuf, ಪರಿಮಾಣ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ಭಙ್ಗ;

// cursor to next window with wrap around
    ಪಕ್ಷ Ctrl_W:
    ಪಕ್ಷ 'w':
// cursor to previous window with wrap around
    ಪಕ್ಷ 'W':
		CHECK_CMDWIN;
		ಯದಿ (ONE_WINDOW && Prenum != 1)	// just one window
		    beep_flush();
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (Prenum)			// go to specified window
		    {
			ಪ್ರತಿ (wp = firstwin; --Prenum > 0; )
			{
			    ಯದಿ (wp->w_next == ರಿಕ್ತ)
				ಭಙ್ಗ;
			    ಅನ್ಯಥಾ
				wp = wp->w_next;
			}
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    ಯದಿ (wp == ರಿಕ್ತ)
				wp = lastwin;	    // wrap around
			}
			ಅನ್ಯಥಾ			    // go to next window
			{
			    wp = curwin->w_next;
			    ಯದಿ (wp == ರಿಕ್ತ)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		ಭಙ್ಗ;

// cursor to window below
    ಪಕ್ಷ 'j':
    ಪಕ್ಷ K_DOWN:
    ಪಕ್ಷ Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		ಭಙ್ಗ;

// cursor to window above
    ಪಕ್ಷ 'k':
    ಪಕ್ಷ K_UP:
    ಪಕ್ಷ Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		ಭಙ್ಗ;

// cursor to left window
    ಪಕ್ಷ 'h':
    ಪಕ್ಷ K_ಎಡ:
    ಪಕ್ಷ Ctrl_H:
    ಪಕ್ಷ K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		ಭಙ್ಗ;

// cursor to right window
    ಪಕ್ಷ 'l':
    ಪಕ್ಷ K_ಬಲ:
    ಪಕ್ಷ Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		ಭಙ್ಗ;

// move window to new tab page
    ಪಕ್ಷ 'T':
		CHECK_CMDWIN;
		ಯದಿ (one_window())
		    ಸಂದೇಶ(_(m_onlyone));
		ಅನ್ಯಥಾ
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    ಯದಿ (win_new_tabpage((ಪೂರ್ಣಾಙ್ಕ)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			ಯದಿ (curwin == wp)
			    win_close(curwin, FALSE);
			ಯದಿ (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		ಭಙ್ಗ;

// cursor to top-left window
    ಪಕ್ಷ 't':
    ಪಕ್ಷ Ctrl_T:
		win_goto(firstwin);
		ಭಙ್ಗ;

// cursor to bottom-right window
    ಪಕ್ಷ 'b':
    ಪಕ್ಷ Ctrl_B:
		win_goto(lastwin);
		ಭಙ್ಗ;

// cursor to last accessed (previous) window
    ಪಕ್ಷ 'p':
    ಪಕ್ಷ Ctrl_P:
		ಯದಿ (!win_valid(prevwin))
		    beep_flush();
		ಅನ್ಯಥಾ
		    win_goto(prevwin);
		ಭಙ್ಗ;

// exchange current and next window
    ಪಕ್ಷ 'x':
    ಪಕ್ಷ Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		ಭಙ್ಗ;

// rotate windows downwards
    ಪಕ್ಷ Ctrl_R:
    ಪಕ್ಷ 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (ಪೂರ್ಣಾಙ್ಕ)Prenum1);    // downwards
		ಭಙ್ಗ;

// rotate windows upwards
    ಪಕ್ಷ 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (ಪೂರ್ಣಾಙ್ಕ)Prenum1);	    // upwards
		ಭಙ್ಗ;

// move window to the very top/bottom/left/right
    ಪಕ್ಷ 'K':
    ಪಕ್ಷ 'J':
    ಪಕ್ಷ 'H':
    ಪಕ್ಷ 'L':
		CHECK_CMDWIN;
		ಯದಿ (ONE_WINDOW)
		    beep_flush();
		ಅನ್ಯಥಾ
		{
		    ಪೂರ್ಣಾಙ್ಕ dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_ತುದಿ : WSP_BOT);

		    (ಶೂನ್ಯ)win_splitmove(curwin, (ಪೂರ್ಣಾಙ್ಕ)Prenum, dir);
		}
		ಭಙ್ಗ;

// make all windows the same width and/or height
    ಪಕ್ಷ '=':
		{
		    ಪೂರ್ಣಾಙ್ಕ mod = cmdmod.cmod_split & (WSP_VERT | WSP_HOR);
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_equal(ರಿಕ್ತ, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		ಭಙ್ಗ;

// increase current window height
    ಪಕ್ಷ '+':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height + (ಪೂರ್ಣಾಙ್ಕ)Prenum1);
		ಭಙ್ಗ;

// decrease current window height
    ಪಕ್ಷ '-':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height - (ಪೂರ್ಣಾಙ್ಕ)Prenum1);
		ಭಙ್ಗ;

// set current window height
    ಪಕ್ಷ Ctrl__:
    ಪಕ್ಷ '_':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(Prenum ? (ಪೂರ್ಣಾಙ್ಕ)Prenum : 9999);
		ಭಙ್ಗ;

// increase current window width
    ಪಕ್ಷ '>':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width + (ಪೂರ್ಣಾಙ್ಕ)Prenum1);
		ಭಙ್ಗ;

// decrease current window width
    ಪಕ್ಷ '<':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width - (ಪೂರ್ಣಾಙ್ಕ)Prenum1);
		ಭಙ್ಗ;

// set current window width
    ಪಕ್ಷ '|':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(Prenum != 0 ? (ಪೂರ್ಣಾಙ್ಕ)Prenum : 9999);
		ಭಙ್ಗ;

// jump to tag and split window if tag exists (in preview window)
#ಯದಿ defined(FEAT_QUICKFIX)
    ಪಕ್ಷ '}':
		CHECK_CMDWIN;
		ಯದಿ (Prenum)
		    g_do_tagpreview = Prenum;
		ಅನ್ಯಥಾ
		    g_do_tagpreview = p_pvh;
#endif
		// FALLTHROUGH
    ಪಕ್ಷ ']':
    ಪಕ್ಷ Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		ಯದಿ (Prenum)
		    postponed_split = Prenum;
		ಅನ್ಯಥಾ
		    postponed_split = -1;
#ifdef FEAT_QUICKFIX
		ಯದಿ (nchar != '}')
		    g_do_tagpreview = 0;
#endif

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_split = 0;
		ಭಙ್ಗ;

// edit file name under cursor in a new window
    ಪಕ್ಷ 'f':
    ಪಕ್ಷ 'F':
    ಪಕ್ಷ Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		ಯದಿ (check_text_or_curbuf_locked(ರಿಕ್ತ))
		    ಭಙ್ಗ;

		ptr = grab_file_name(Prenum1, &lnum);
		ಯದಿ (ptr != ರಿಕ್ತ)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = ರಿಕ್ತ;
		    ಯದಿ ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    ಯದಿ (wp == ರಿಕ್ತ && win_split(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			ಯದಿ (do_ecmd(0, ptr, ರಿಕ್ತ, ರಿಕ್ತ, ECMD_LASTL,
						   ECMD_HIDE, ರಿಕ್ತ) == FAIL)
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
			ಅನ್ಯಥಾ
			    wp = curwin;
		    }

		    ಯದಿ (wp != ರಿಕ್ತ && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		ಭಙ್ಗ;

#ifdef FEAT_FIND_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    ಪಕ್ಷ 'i':			    // Go to any match
    ಪಕ್ಷ Ctrl_I:
		type = FIND_ANY;
		// FALLTHROUGH
    ಪಕ್ಷ 'd':			    // Go to definition, using 'define'
    ಪಕ್ಷ Ctrl_D:
		CHECK_CMDWIN;
		ಯದಿ ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
		    ಭಙ್ಗ;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		ಯದಿ (ptr == ರಿಕ್ತ)
		    ಭಙ್ಗ;

		find_pattern_in_path(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_SPLIT, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_set_curswant = true;
		ಭಙ್ಗ;
#endif

// Quickfix window only: view the result under the cursor in a new split.
#ಯದಿ defined(FEAT_QUICKFIX)
    ಪಕ್ಷ K_KENTER:
    ಪಕ್ಷ CAR:
		ಯದಿ (bt_quickfix(curbuf))
		    qf_view_result(TRUE);
		ಭಙ್ಗ;
#endif

// CTRL-W g  extended commands
    ಪಕ್ಷ 'g':
    ಪಕ್ಷ Ctrl_G:
		CHECK_CMDWIN;
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#endif
		++no_mapping;
		++allow_ಕೀಲಿಗಳು;   // no mapping for xchar, but allow key codes
		ಯದಿ (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_ಕೀಲಿಗಳು;
		(ಶೂನ್ಯ)add_to_showcmd(xchar);

		ವಿಕಲ್ಪ (xchar)
		{
#ಯದಿ defined(FEAT_QUICKFIX)
		    ಪಕ್ಷ '}':
			xchar = Ctrl_RSB;
			ಯದಿ (Prenum)
			    g_do_tagpreview = Prenum;
			ಅನ್ಯಥಾ
			    g_do_tagpreview = p_pvh;
#endif
			// FALLTHROUGH
		    ಪಕ್ಷ ']':
		    ಪಕ್ಷ Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			ಯದಿ (Prenum)
			    postponed_split = Prenum;
			ಅನ್ಯಥಾ
			    postponed_split = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_split = 0;
			ಭಙ್ಗ;

		    ಪಕ್ಷ 'f':	    // CTRL-W gf: "gf" in a new tab page
		    ಪಕ್ಷ 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_index(curtab) + 1;
			nchar = xchar;
			ಗಚ್ಛ wingotofile;

		    ಪಕ್ಷ 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((ಪೂರ್ಣಾಙ್ಕ)Prenum);
			ಭಙ್ಗ;

		    ಪಕ್ಷ 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(ಪೂರ್ಣಾಙ್ಕ)Prenum1);
			ಭಙ್ಗ;

		    ಪಕ್ಷ TAB:	    // CTRL-W g<Tab>: go to last used tab page
			ಯದಿ (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			ಭಙ್ಗ;

		    ಸ್ವಭಾವ:
			beep_flush();
			ಭಙ್ಗ;
		}
		ಭಙ್ಗ;

    ಸ್ವಭಾವ:	beep_flush();
		ಭಙ್ಗ;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    ಶೂನ್ಯ
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    ವಿಕಲ್ಪ (*arg)
    {
    ಪಕ್ಷ 'S':
    ಪಕ್ಷ Ctrl_S:
    ಪಕ್ಷ 's':
    ಪಕ್ಷ Ctrl_N:
    ಪಕ್ಷ 'n':
    ಪಕ್ಷ 'j':
    ಪಕ್ಷ Ctrl_J:
    ಪಕ್ಷ 'k':
    ಪಕ್ಷ Ctrl_K:
    ಪಕ್ಷ 'T':
    ಪಕ್ಷ Ctrl_R:
    ಪಕ್ಷ 'r':
    ಪಕ್ಷ 'R':
    ಪಕ್ಷ 'K':
    ಪಕ್ಷ 'J':
    ಪಕ್ಷ '+':
    ಪಕ್ಷ '-':
    ಪಕ್ಷ Ctrl__:
    ಪಕ್ಷ '_':
    ಪಕ್ಷ '|':
    ಪಕ್ಷ ']':
    ಪಕ್ಷ Ctrl_RSB:
    ಪಕ್ಷ 'g':
    ಪಕ್ಷ Ctrl_G:
    ಪಕ್ಷ Ctrl_V:
    ಪಕ್ಷ 'v':
    ಪಕ್ಷ 'h':
    ಪಕ್ಷ Ctrl_H:
    ಪಕ್ಷ 'l':
    ಪಕ್ಷ Ctrl_L:
    ಪಕ್ಷ 'H':
    ಪಕ್ಷ 'L':
    ಪಕ್ಷ '>':
    ಪಕ್ಷ '<':
#ಯದಿ defined(FEAT_QUICKFIX)
    ಪಕ್ಷ '}':
#endif
    ಪಕ್ಷ 'f':
    ಪಕ್ಷ 'F':
    ಪಕ್ಷ Ctrl_F:
#ifdef FEAT_FIND_ID
    ಪಕ್ಷ 'i':
    ಪಕ್ಷ Ctrl_I:
    ಪಕ್ಷ 'd':
    ಪಕ್ಷ Ctrl_D:
#endif
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		ಭಙ್ಗ;

    ಪಕ್ಷ Ctrl_HAT:
    ಪಕ್ಷ '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		ಭಙ್ಗ;

    ಪಕ್ಷ Ctrl_Q:
    ಪಕ್ಷ 'q':
    ಪಕ್ಷ Ctrl_C:
    ಪಕ್ಷ 'c':
    ಪಕ್ಷ Ctrl_O:
    ಪಕ್ಷ 'o':
    ಪಕ್ಷ Ctrl_W:
    ಪಕ್ಷ 'w':
    ಪಕ್ಷ 'W':
    ಪಕ್ಷ 'x':
    ಪಕ್ಷ Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		ಭಙ್ಗ;

#ಯದಿ defined(FEAT_QUICKFIX)
    ಪಕ್ಷ Ctrl_Z:
    ಪಕ್ಷ 'z':
    ಪಕ್ಷ 'P':
#endif
    ಪಕ್ಷ 't':
    ಪಕ್ಷ Ctrl_T:
    ಪಕ್ಷ 'b':
    ಪಕ್ಷ Ctrl_B:
    ಪಕ್ಷ 'p':
    ಪಕ್ಷ Ctrl_P:
    ಪಕ್ಷ '=':
    ಪಕ್ಷ CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		ಭಙ್ಗ;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cmd_with_ಎಣಿಕೆ(
    ವರ್ಣ	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    long	Prenum)
{
    ಯದಿ (Prenum > 0)
	vim_snprintf((ವರ್ಣ *)bufp, bufsize, "%s %ld", cmd, Prenum);
    ಅನ್ಯಥಾ
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_split_disallowed(win_T *wp)
{
    ಯದಿ (split_disallowed > 0)
    {
	emsg(_(e_cant_split_window_while_closing_another));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (wp->w_buffer->b_locked_split)
    {
	emsg(_(e_cannot_split_window_when_closing_buffer));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ
win_split(ಪೂರ್ಣಾಙ್ಕ size, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (check_split_disallowed(curwin) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    ಯದಿ (may_open_tabpage() == OK)
	ಪ್ರತ್ಯಾಗಮ OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_split;
    ಯದಿ ((flags & WSP_ತುದಿ) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_split_topleft_and_botright_at_the_same_time));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    ಯದಿ (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    ಅನ್ಯಥಾ
	clear_snapshot(curtab, SNAP_HELP_IDX);

    ಯದಿ (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    ಅನ್ಯಥಾ
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    ಪ್ರತ್ಯಾಗಮ win_split_ins(size, flags, ರಿಕ್ತ, 0, ರಿಕ್ತ);
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
    ಪೂರ್ಣಾಙ್ಕ
win_split_ins(
    ಪೂರ್ಣಾಙ್ಕ		size,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    win_T	*new_wp,
    ಪೂರ್ಣಾಙ್ಕ		dir,
    frame_T	*to_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    ಪೂರ್ಣಾಙ್ಕ		new_size = size;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		need_status = 0;
    ಪೂರ್ಣಾಙ್ಕ		do_equal = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		needed;
    ಪೂರ್ಣಾಙ್ಕ		available;
    ಪೂರ್ಣಾಙ್ಕ		oldwin_height = 0;
    ಪೂರ್ಣಾಙ್ಕ		layout;
    frame_T	*frp, *curfrp, *frp2, *prevfrp;
    ಪೂರ್ಣಾಙ್ಕ		before;
    ಪೂರ್ಣಾಙ್ಕ		minheight;
    ಪೂರ್ಣಾಙ್ಕ		wmh1;
    ಪೂರ್ಣಾಙ್ಕ		did_set_fraction = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    ಯದಿ (new_wp == ರಿಕ್ತ)
	trigger_winnewpre();

    ಯದಿ (flags & WSP_ತುದಿ)
	oldwin = firstwin;
    ಅನ್ಯಥಾ ಯದಿ (flags & WSP_BOT)
	oldwin = lastwin;
    ಅನ್ಯಥಾ
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    ಯದಿ (ONE_WINDOW && p_ls == 1 && oldwin->w_status_height == 0)
    {
	ಯದಿ (!(flags & WSP_FORCE_ROOM) && VISIBLE_HEIGHT(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_room));
	    ಗಚ್ಛ theend;
	}
	need_status = statusline_height(oldwin);
    }

#ifdef FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    ಯದಿ (gui.in_use)
	out_flush();
#endif

    ಯದಿ (flags & WSP_VERT)
    {
	ಪೂರ್ಣಾಙ್ಕ	wmw1;
	ಪೂರ್ಣಾಙ್ಕ	minwidth;

	layout = FR_ROW;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	ಯದಿ (flags & WSP_ROOM)
	    needed += p_wiw - wmw1;
	ಯದಿ (flags & (WSP_BOT | WSP_ತುದಿ))
	{
	    minwidth = frame_minwidth(topframe, NOWIN);
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	ಅನ್ಯಥಾ ಯದಿ (p_ea)
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    prevfrp = oldwin->w_frame;
	    ಪ್ರತಿ (frp = oldwin->w_frame->fr_parent; frp != ರಿಕ್ತ;
							frp = frp->fr_parent)
	    {
		ಯದಿ (frp->fr_layout == FR_ROW)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			ಯದಿ (frp2 != prevfrp)
			    minwidth += frame_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	ಅನ್ಯಥಾ
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    available = oldwin->w_frame->fr_width;
	    needed += minwidth;
	}
	ಯದಿ (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    ಗಚ್ಛ theend;
	}
	ಯದಿ (new_size == 0)
	    new_size = oldwin->w_width / 2;
	ಯದಿ (new_size > available - minwidth - 1)
	    new_size = available - minwidth - 1;
	ಯದಿ (new_size < wmw1)
	    new_size = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	ಯದಿ (oldwin->w_width - new_size - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	ಯದಿ (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_width + new_size + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	ಯದಿ (!do_equal && p_ea && size == 0 && *p_ead != 'v'
	    && oldwin->w_frame->fr_parent != ರಿಕ್ತ)
	    do_equal = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	layout = FR_COL;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_HEIGHT(curwin);
	needed = wmh1 + statusline_height(oldwin);
	ಯದಿ (flags & WSP_ROOM)
	    needed += p_wh - wmh1;
	ಯದಿ (flags & (WSP_BOT | WSP_ತುದಿ))
	{
	    minheight = frame_minheight(topframe, NOWIN) + need_status;
	    available = topframe->fr_height;
	    needed += minheight;
	}
	ಅನ್ಯಥಾ ಯದಿ (p_ea)
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    prevfrp = oldwin->w_frame;
	    ಪ್ರತಿ (frp = oldwin->w_frame->fr_parent; frp != ರಿಕ್ತ;
							frp = frp->fr_parent)
	    {
		ಯದಿ (frp->fr_layout == FR_COL)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			ಯದಿ (frp2 != prevfrp)
			    minheight += frame_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_height;
	    needed += minheight;
	}
	ಅನ್ಯಥಾ
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    available = oldwin->w_frame->fr_height;
	    needed += minheight;
	}
	ಯದಿ (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    ಗಚ್ಛ theend;
	}
	oldwin_height = oldwin->w_height;
	ಯದಿ (need_status)
	{
	    oldwin->w_status_height = statusline_height(oldwin);
	    oldwin_height -= oldwin->w_status_height;
	}
	// Estimated stl height for the new window.
	ಪೂರ್ಣಾಙ್ಕ new_win_stlh = need_status != 0 ? need_status
			: statusline_height(new_wp != ರಿಕ್ತ ? new_wp : curwin);
	ಯದಿ (new_size == 0)
	    new_size = (oldwin_height - new_win_stlh + 1) / 2;
	ಯದಿ (new_size > available - minheight - new_win_stlh)
	    new_size = available - minheight - new_win_stlh;
	ಯದಿ (new_size < wmh1)
	    new_size = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	ಯದಿ (oldwin_height - new_size - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	ಯದಿ (oldwin->w_p_wfh)
	{
	    // Set w_fraction now so that the cursor keeps the same relative
	    // vertical position using the old height.
	    set_fraction(oldwin);
	    did_set_fraction = TRUE;

	    win_setheight_win(oldwin->w_height + new_size
		    + statusline_height(oldwin), oldwin);
	    // w_height now excludes the status line
	    oldwin_height = oldwin->w_height;
	}

	// Make all windows the same height if 'equalalways' is set, 'ead' is
	// not set to 'h', a size was not provided and there is a parent frame.
	ಯದಿ (!do_equal && p_ea && size == 0 && *p_ead != 'h'
	    && oldwin->w_frame->fr_parent != ರಿಕ್ತ)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    ಯದಿ ((flags & WSP_ತುದಿ) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_BELOW)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	ಯದಿ (new_wp == ರಿಕ್ತ)
	    wp = win_alloc(oldwin, FALSE);
	ಅನ್ಯಥಾ
	    win_append(oldwin, wp);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (new_wp == ರಿಕ್ತ)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	ಅನ್ಯಥಾ
	    win_append(oldwin->w_prev, wp);
    }

    ಯದಿ (new_wp == ರಿಕ್ತ)
    {
	ಯದಿ (wp == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;

	new_frame(wp);
	ಯದಿ (wp->w_frame == ರಿಕ್ತ)
	{
	    win_free(wp, ರಿಕ್ತ);
	    ಗಚ್ಛ theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    ಯದಿ (to_flatten != ರಿಕ್ತ)
	frame_flatten(to_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    ಯದಿ (flags & (WSP_ತುದಿ | WSP_BOT))
    {
	ಯದಿ ((topframe->fr_layout == FR_COL && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ROW && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_child;
	    ಯದಿ (flags & WSP_BOT)
		ಯಾವತ್ (curfrp->fr_next != ರಿಕ್ತ)
		    curfrp = curfrp->fr_next;
	}
	ಅನ್ಯಥಾ
	    curfrp = topframe;
	before = (flags & WSP_ತುದಿ);
    }
    ಅನ್ಯಥಾ
    {
	curfrp = oldwin->w_frame;
	ಯದಿ (flags & WSP_BELOW)
	    before = FALSE;
	ಅನ್ಯಥಾ ಯದಿ (flags & WSP_ABOVE)
	    before = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (flags & WSP_VERT)
	    before = !p_spr;
	ಅನ್ಯಥಾ
	    before = !p_sb;
    }
    ಯದಿ (curfrp->fr_parent == ರಿಕ್ತ || curfrp->fr_parent->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(frame_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_parent = curfrp;
	frp->fr_next = ರಿಕ್ತ;
	frp->fr_prev = ರಿಕ್ತ;
	curfrp->fr_child = frp;
	curfrp->fr_win = ರಿಕ್ತ;
	curfrp = frp;
	ಯದಿ (frp->fr_win != ರಿಕ್ತ)
	    oldwin->w_frame = frp;
	ಅನ್ಯಥಾ
	    FOR_ALL_FRAMES(frp, frp->fr_child)
		frp->fr_parent = curfrp;
    }

    ಯದಿ (new_wp == ರಿಕ್ತ)
	frp = wp->w_frame;
    ಅನ್ಯಥಾ
	frp = new_wp->w_frame;
    frp->fr_parent = curfrp->fr_parent;

    // Insert the new frame at the right place in the frame list.
    ಯದಿ (before)
	frame_ಸೇರಿಸು(curfrp, frp);
    ಅನ್ಯಥಾ
	frame_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    ಯದಿ (!did_set_fraction)
	set_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    ಯದಿ (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	ಯದಿ (need_status)
	{
	    win_new_height(oldwin, oldwin->w_height - need_status);
	    oldwin->w_status_height = need_status;
	}
	ಯದಿ (flags & (WSP_ತುದಿ | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    ಪೂರ್ಣಾಙ್ಕ stl_height = p_ls > 0 ? statusline_height(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_height();
	    win_new_height(wp, curfrp->fr_height - stl_height
							  - WINBAR_HEIGHT(wp));
	    wp->w_status_height = stl_height;
	}
	ಅನ್ಯಥಾ
	{
	    // height and row of new window is same as current window
	    wp->w_winrow = oldwin->w_winrow;
	    win_new_height(wp, VISIBLE_HEIGHT(oldwin));
	    wp->w_status_height = oldwin->w_status_height;
	}
	frp->fr_height = curfrp->fr_height;

	// "new_size" of the current window goes to the new window, use
	// one column for the vertical separator
	win_new_width(wp, new_size);
	ಯದಿ (before)
	    wp->w_vsep_width = 1;
	ಅನ್ಯಥಾ
	{
	    wp->w_vsep_width = oldwin->w_vsep_width;
	    oldwin->w_vsep_width = 1;
	}
	ಯದಿ (flags & (WSP_ತುದಿ | WSP_BOT))
	{
	    ಯದಿ (flags & WSP_BOT)
		frame_set_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    frame_new_width(curfrp, curfrp->fr_width
		     - (new_size + ((flags & WSP_ತುದಿ) != 0)), flags & WSP_ತುದಿ,
								       FALSE);
	}
	ಅನ್ಯಥಾ
	    win_new_width(oldwin, oldwin->w_width - (new_size + 1));
	ಯದಿ (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_size + 1;
	}
	ಅನ್ಯಥಾ		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_width + 1;
	frame_fix_width(oldwin);
	frame_fix_width(wp);
    }
    ಅನ್ಯಥಾ
    {
	// width and column of new window is same as current window
	ಯದಿ (flags & (WSP_ತುದಿ | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_width(wp, topframe->fr_width);
	    wp->w_vsep_width = 0;
	}
	ಅನ್ಯಥಾ
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_width(wp, oldwin->w_width);
	    wp->w_vsep_width = oldwin->w_vsep_width;
	}
	frp->fr_width = curfrp->fr_width;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_height(wp, new_size);
	ಪೂರ್ಣಾಙ್ಕ old_status_height = oldwin->w_status_height;
	ಯದಿ (flags & (WSP_ತುದಿ | WSP_BOT))
	{
	    ಪೂರ್ಣಾಙ್ಕ new_fr_height = curfrp->fr_height - new_size
							  + WINBAR_HEIGHT(wp);

	    ಯದಿ (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_height -= statusline_height(curfrp->fr_win);
	    ಯದಿ (flags & WSP_BOT)
		frame_add_statusline(curfrp);
	    frame_new_height(curfrp, new_fr_height, flags & WSP_ತುದಿ, FALSE,
									FALSE);
	}
	ಅನ್ಯಥಾ
	    win_new_height(oldwin, oldwin_height - (new_size
			+ statusline_height(wp)));
	ಯದಿ (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_status_height = statusline_height(wp);
	    oldwin->w_winrow += wp->w_height + statusline_height(wp);
	}
	ಅನ್ಯಥಾ		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_HEIGHT(oldwin)
						    + statusline_height(oldwin);
	    wp->w_status_height = old_status_height;
	    ಯದಿ (!(flags & WSP_BOT))
		oldwin->w_status_height = statusline_height(oldwin);
	}
	frame_fix_height(wp);
	frame_fix_height(oldwin);
    }

    ಯದಿ (flags & (WSP_ತುದಿ | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    status_redraw_all();

    ಯದಿ (need_status)
    {
	ಸಂದೇಶ_row = Rows - 1;
	ಸಂದೇಶ_col = sc_col;
	ಸಂದೇಶ_clr_eos_force();	// Old command/ruler may still be there
	comp_col();
	ಸಂದೇಶ_row = Rows - 1;
	ಸಂದೇಶ_col = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    ಯದಿ (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    ಅನ್ಯಥಾ ಯದಿ (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    ಯದಿ (flags & WSP_VERT)
    {
	i = p_wiw;
	ಯದಿ (size != 0)
	    p_wiw = size;

#ifdef FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	ಯದಿ (gui.in_use)
	    gui_init_which_components(ರಿಕ್ತ);
#endif
    }
    ಅನ್ಯಥಾ
    {
	i = p_wh;
	ಯದಿ (size != 0)
	    p_wh = size;
    }

    /*
     * make the new window the current window
     */
    (ಶೂನ್ಯ)win_enter_ext(wp, (new_wp == ರಿಕ್ತ ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    ಯದಿ (flags & WSP_VERT)
	p_wiw = i;
    ಅನ್ಯಥಾ
	p_wh = i;
    retval = OK;

theend:
    ಯದಿ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ಪ್ರತ್ಯಾಗಮ retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_init(win_T *newp, win_T *oldp, ಪೂರ್ಣಾಙ್ಕ flags UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    newp->w_buffer = oldp->w_buffer;
#ifdef FEAT_SYN_HL
    newp->w_s = &(oldp->w_buffer->b_s);
#endif
    oldp->w_buffer->b_nwindows++;
    newp->w_cursor = oldp->w_cursor;
    newp->w_valid = 0;
    newp->w_curswant = oldp->w_curswant;
    newp->w_set_curswant = oldp->w_set_curswant;
    newp->w_topline = oldp->w_topline;
#ifdef FEAT_DIFF
    newp->w_topfill = oldp->w_topfill;
#endif
    newp->w_leftcol = oldp->w_leftcol;
    newp->w_pcmark = oldp->w_pcmark;
    newp->w_prev_pcmark = oldp->w_prev_pcmark;
    newp->w_alt_fnum = oldp->w_alt_fnum;
    newp->w_wrow = oldp->w_wrow;
    newp->w_fraction = oldp->w_fraction;
    newp->w_prev_fraction_row = oldp->w_prev_fraction_row;

    // Not sure if this is needed, but be safe
    remove_highlight_overrides(newp->w_hl);
    VIM_CLEAR(newp->w_hl);

    copy_jumplist(oldp, newp);
#ifdef FEAT_QUICKFIX
    ಯದಿ (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = ರಿಕ್ತ;
	newp->w_llist_ref = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
	copy_loclist_ರಾಶಿ(oldp, newp);
#endif
    newp->w_localdir = (oldp->w_localdir == ರಿಕ್ತ)
				    ? ರಿಕ್ತ : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == ರಿಕ್ತ)
				    ? ರಿಕ್ತ : vim_strsave(oldp->w_prevdir);

    ಯದಿ (*p_spk != 'c')
    {
	ಯದಿ (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_height = oldp->w_height - WINBAR_HEIGHT(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_HEIGHT(oldp);
    }

    // copy tagstack and folds
    ಪ್ರತಿ (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*tag = &newp->w_tagstack[i];
	*tag = oldp->w_tagstack[i];
	ಯದಿ (tag->tagname != ರಿಕ್ತ)
	    tag->tagname = vim_strsave(tag->tagname);
	ಯದಿ (tag->user_data != ರಿಕ್ತ)
	    tag->user_data = vim_strsave(tag->user_data);
    }
    newp->w_tagstackidx = oldp->w_tagstackidx;
    newp->w_tagstacklen = oldp->w_tagstacklen;

    // Keep same changelist position in new window.
    newp->w_changelistidx = oldp->w_changelistidx;

#ifdef FEAT_FOLDING
    copyFoldingState(oldp, newp);
#endif

    win_init_some(newp, oldp);

#ifdef FEAT_TERMINAL
    {
	// Make sure to also handle highlight overrides copied over from oldp.
	bool pushed = ತಳ್ಳು_highlight_overrides(newp->w_hl, newp->w_hl_len);
	ಯದಿ (newp->w_buffer->b_term != ರಿಕ್ತ)
	    term_init_default_colors(newp->w_buffer->b_term);
	ಯದಿ (pushed)
	    ತೆಗೆ_highlight_overrides();
    }
#endif
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
    ಪೂರ್ಣಾಙ್ಕ
win_valid_popup(win_T *win UNUSED)
{
#ifdef FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	ಯದಿ (wp == win)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ಯದಿ (wp == win)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
#endif
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    ಪೂರ್ಣಾಙ್ಕ
win_valid(win_T *win)
{
    win_T	*wp;

    ಯದಿ (win == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp == win)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_find_by_id(ಪೂರ್ಣಾಙ್ಕ id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_id == id)
	    ಪ್ರತ್ಯಾಗಮ wp;
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	ಯದಿ (wp->w_id == id)
	    ಪ್ರತ್ಯಾಗಮ wp;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ಯದಿ (wp->w_id == id)
	    ಪ್ರತ್ಯಾಗಮ wp;
#endif
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    ಪೂರ್ಣಾಙ್ಕ
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    ಯದಿ (win == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ಯದಿ (wp == win)
		ಪ್ರತ್ಯಾಗಮ TRUE;
	}
#ifdef FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    ಯದಿ (wp == win)
		ಪ್ರತ್ಯಾಗಮ TRUE;
#endif
    }
    ಪ್ರತ್ಯಾಗಮ win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    ಪೂರ್ಣಾಙ್ಕ
win_ಎಣಿಕೆ(ಶೂನ್ಯ)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;

    FOR_ALL_WINDOWS(wp)
	++ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    ಪೂರ್ಣಾಙ್ಕ
make_windows(
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		vertical)  // split windows vertically if TRUE
{
    ಪೂರ್ಣಾಙ್ಕ		maxcount;
    ಪೂರ್ಣಾಙ್ಕ		todo;

    ಯದಿ (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_width + curwin->w_vsep_width
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    ಅನ್ಯಥಾ
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_HEIGHT(curwin) + curwin->w_status_height
			- (p_wh - p_wmh)) / (p_wmh + statusline_height(curwin));
    }

    ಯದಿ (maxcount < 2)
	maxcount = 2;
    ಯದಿ (ಎಣಿಕೆ > maxcount)
	ಎಣಿಕೆ = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    ಯದಿ (ಎಣಿಕೆ > 1)
	last_status(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    // todo is number of windows left to create
    ಪ್ರತಿ (todo = ಎಣಿಕೆ - 1; todo > 0; --todo)
	ಯದಿ (vertical)
	{
	    ಯದಿ (win_split(curwin->w_width - (curwin->w_width - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (win_split(curwin->w_height - (curwin->w_height - todo
			    * statusline_height(curwin)) / (todo + 1)
			- statusline_height(curwin), WSP_ABOVE) == FAIL)
		ಭಙ್ಗ;
	}

    unblock_autocmds();

    // return actual number of windows
    ಪ್ರತ್ಯಾಗಮ (ಎಣಿಕೆ - todo);
}

/*
 * Exchange current and next window
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_exchange(long Prenum)
{
    frame_T	*frp;
    frame_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    ಪೂರ್ಣಾಙ್ಕ		temp;

    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (ONE_WINDOW)	    // just one window
    {
	beep_flush();
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (text_or_buf_locked())
    {
	beep_flush();
	ಪ್ರತ್ಯಾಗಮ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * find window to exchange with
     */
    ಯದಿ (Prenum)
    {
	frp = curwin->w_frame->fr_parent->fr_child;
	ಯಾವತ್ (frp != ರಿಕ್ತ && --Prenum > 0)
	    frp = frp->fr_next;
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_frame->fr_next != ರಿಕ್ತ)	// Swap with next
	frp = curwin->w_frame->fr_next;
    ಅನ್ಯಥಾ    // Swap last window in row/col with previous
	frp = curwin->w_frame->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    ಯದಿ (frp == ರಿಕ್ತ || frp->fr_win == ರಿಕ್ತ || frp->fr_win == curwin)
	ಪ್ರತ್ಯಾಗಮ;
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
    frp2 = curwin->w_frame->fr_prev;
    ಯದಿ (wp->w_prev != curwin)
    {
	win_remove(curwin, ರಿಕ್ತ);
	frame_remove(curwin->w_frame);
	win_append(wp->w_prev, curwin);
	frame_ಸೇರಿಸು(frp, curwin->w_frame);
    }
    ಯದಿ (wp != wp2)
    {
	win_remove(wp, ರಿಕ್ತ);
	frame_remove(wp->w_frame);
	win_append(wp2, wp);
	ಯದಿ (frp2 == ರಿಕ್ತ)
	    frame_ಸೇರಿಸು(wp->w_frame->fr_parent->fr_child, wp->w_frame);
	ಅನ್ಯಥಾ
	    frame_append(frp2, wp->w_frame);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    temp = curwin->w_status_height;
    ಯದಿ (temp != wp->w_status_height)
    {
	win_new_height(curwin, curwin->w_height + temp - wp->w_status_height);
	win_new_height(wp, wp->w_height + wp->w_status_height - temp);
    }
    curwin->w_status_height = wp->w_status_height;
    wp->w_status_height = temp;
    temp = curwin->w_vsep_width;
    curwin->w_vsep_width = wp->w_vsep_width;
    wp->w_vsep_width = temp;

    frame_fix_height(curwin);
    frame_fix_height(wp);
    frame_fix_width(curwin);
    frame_fix_width(wp);

    win_comp_pos();		// recompute window positions
#ಯದಿ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    ಯದಿ (wp->w_buffer != curbuf)
	reset_VIsual_and_resel();
    ಅನ್ಯಥಾ ಯದಿ (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_rotate(ಪೂರ್ಣಾಙ್ಕ upwards, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    win_T	*wp1;
    win_T	*wp2;
    frame_T	*frp;
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯದಿ (ONE_WINDOW)		// nothing to do
    {
	beep_flush();
	ಪ್ರತ್ಯಾಗಮ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Check if all frames in this row/col have one window.
    FOR_ALL_FRAMES(frp, curwin->w_frame->fr_parent->fr_child)
	ಯದಿ (frp->fr_win == ರಿಕ್ತ)
	{
	    emsg(_(e_cannot_rotate_when_another_window_is_split));
	    ಪ್ರತ್ಯಾಗಮ;
	}

    ಯಾವತ್ (ಎಣಿಕೆ--)
    {
	ಯದಿ (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_frame->fr_parent->fr_child;
	    wp1 = frp->fr_win;
	    win_remove(wp1, ರಿಕ್ತ);
	    frame_remove(frp);

	    // find last frame and append removed window/frame after it
	    ಪ್ರತಿ ( ; frp->fr_next != ರಿಕ್ತ; frp = frp->fr_next)
		;
	    win_append(frp->fr_win, wp1);
	    frame_append(frp, wp1->w_frame);

	    wp2 = frp->fr_win;		// previously last window
	}
	ಅನ್ಯಥಾ			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    ಪ್ರತಿ (frp = curwin->w_frame; frp->fr_next != ರಿಕ್ತ;
							   frp = frp->fr_next)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, ರಿಕ್ತ);
	    frame_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_parent->fr_child->fr_win->w_prev, wp1);
	    frame_ಸೇರಿಸು(frp->fr_parent->fr_child, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_status_height;
	ಯದಿ (n != wp1->w_status_height)
	{
	    win_new_height(wp2, wp2->w_height + n - wp1->w_status_height);
	    win_new_height(wp1, wp1->w_height + wp1->w_status_height - n);
	}
	wp2->w_status_height = wp1->w_status_height;
	wp1->w_status_height = n;
	frame_fix_height(wp1);
	frame_fix_height(wp2);
	n = wp2->w_vsep_width;
	wp2->w_vsep_width = wp1->w_vsep_width;
	wp1->w_vsep_width = n;
	frame_fix_width(wp1);
	frame_fix_width(wp2);

	// recompute w_winrow and w_wincol for all windows
	win_comp_pos();
    }
#ಯದಿ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Move "wp" into a new split in a given direction, possibly relative to the
 * current window.
 * "wp" must be valid in the current tabpage.
 * Returns FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
win_splitmove(win_T *wp, ಪೂರ್ಣಾಙ್ಕ size, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಪೂರ್ಣಾಙ್ಕ		dir;
    ಪೂರ್ಣಾಙ್ಕ		height = wp->w_height;
    frame_T	*unflat_altfr;

    ಯದಿ (ONE_WINDOW)
	ಪ್ರತ್ಯಾಗಮ OK;	// nothing to do
    ಯದಿ (check_split_disallowed(wp) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, ರಿಕ್ತ, &unflat_altfr);
    win_remove(wp, ರಿಕ್ತ);
    last_status(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    ಯದಿ (win_split_ins(size, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    ಯದಿ (size == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(height, wp);
	ಯದಿ (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#ಯದಿ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    ಶೂನ್ಯ
win_move_after(win_T *win1, win_T *win2)
{
    ಪೂರ್ಣಾಙ್ಕ		height;

    // check if the arguments are reasonable
    ಯದಿ (win1 == win2)
	ಪ್ರತ್ಯಾಗಮ;

    // check if there is something to do
    ಯದಿ (win2->w_next != win1)
    {
	ಯದಿ (win1->w_frame->fr_parent != win2->w_frame->fr_parent)
	{
	    iemsg("Trying to move a window into another frame");
	    ಪ್ರತ್ಯಾಗಮ;
	}

	// may need to move the status line/vertical separator of the last
	// window
	ಯದಿ (win1 == lastwin)
	{
	    height = win1->w_prev->w_status_height;
	    win1->w_prev->w_status_height = win1->w_status_height;
	    win1->w_status_height = height;
	    ಯದಿ (win1->w_prev->w_vsep_width == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_width = 0;
		win1->w_prev->w_frame->fr_width -= 1;
		win1->w_vsep_width = 1;
		win1->w_frame->fr_width += 1;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (win2 == lastwin)
	{
	    height = win1->w_status_height;
	    win1->w_status_height = win2->w_status_height;
	    win2->w_status_height = height;
	    ಯದಿ (win1->w_vsep_width == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_width = 1;
		win2->w_frame->fr_width += 1;
		win1->w_vsep_width = 0;
		win1->w_frame->fr_width -= 1;
	    }
	}
	win_remove(win1, ರಿಕ್ತ);
	frame_remove(win1->w_frame);
	win_append(win2, win1);
	frame_append(win2->w_frame, win1->w_frame);

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
    win_T	*next_curwin,	// pointer to current window to be or NULL
    ಪೂರ್ಣಾಙ್ಕ		current,	// do only frame with current window
    ಪೂರ್ಣಾಙ್ಕ		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    ಯದಿ (dir == 0)
	dir = *p_ead;
    win_equal_rec(next_curwin == ರಿಕ್ತ ? curwin : next_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_height(),
		      topframe->fr_width, topframe->fr_height);
#ಯದಿ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
    ಯದಿ (!is_aucmd_win(next_curwin))
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
    win_T	*next_curwin,	// pointer to current window to be or NULL
    ಪೂರ್ಣಾಙ್ಕ		current,	// do only frame with current window
    frame_T	*topfr,		// frame to set size off
    ಪೂರ್ಣಾಙ್ಕ		dir,		// 'v', 'h' or 'b', see win_equal()
    ಪೂರ್ಣಾಙ್ಕ		col,		// horizontal position for frame
    ಪೂರ್ಣಾಙ್ಕ		row,		// vertical position for frame
    ಪೂರ್ಣಾಙ್ಕ		width,		// new width of frame
    ಪೂರ್ಣಾಙ್ಕ		height)		// new height of frame
{
    ಪೂರ್ಣಾಙ್ಕ		n, m;
    ಪೂರ್ಣಾಙ್ಕ		extra_sep = 0;
    ಪೂರ್ಣಾಙ್ಕ		wincount, totwincount = 0;
    frame_T	*fr;
    ಪೂರ್ಣಾಙ್ಕ		next_curwin_size = 0;
    ಪೂರ್ಣಾಙ್ಕ		room = 0;
    ಪೂರ್ಣಾಙ್ಕ		new_size;
    ಪೂರ್ಣಾಙ್ಕ		has_next_curwin = 0;
    ಪೂರ್ಣಾಙ್ಕ		hnc;

    ಯದಿ (topfr->fr_layout == FR_LEAF)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	ಯದಿ (topfr->fr_height != height || topfr->fr_win->w_winrow != row
		|| topfr->fr_width != width || topfr->fr_win->w_wincol != col
	   )
	{
	    topfr->fr_win->w_winrow = row;
	    frame_new_height(topfr, height, FALSE, FALSE, FALSE);
	    topfr->fr_win->w_wincol = col;
	    frame_new_width(topfr, width, FALSE, FALSE);
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (topfr->fr_layout == FR_ROW)
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	ಯದಿ (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = frame_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    ಯದಿ (col + width >= firstwin->w_wincol + topframe->fr_width)
		extra_sep = 1;
	    ಅನ್ಯಥಾ
		extra_sep = 0;
	    totwincount = (n + extra_sep) / (p_wmw + 1);
	    has_next_curwin = frame_has_win(topfr, next_curwin);

	    /*
	     * Compute width for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal width when counting p_wiw for "next_curwin".
	     */
	    m = frame_minwidth(topfr, next_curwin);
	    room = width - m;
	    ಯದಿ (room < 0)
	    {
		next_curwin_size = p_wiw + room;
		room = 0;
	    }
	    ಅನ್ಯಥಾ
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    ಯದಿ (!frame_fixed_width(fr))
			ಅನುವರ್ತ;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minwidth(fr, NOWIN);
		    new_size = fr->fr_width;
		    ಯದಿ (frame_has_win(fr, next_curwin))
		    {
			room += p_wiw - p_wmw;
			next_curwin_size = 0;
			ಯದಿ (new_size < p_wiw)
			    new_size = p_wiw;
		    }
		    ಅನ್ಯಥಾ
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_next == ರಿಕ್ತ
					? extra_sep : 0)) / (p_wmw + 1);
		    room -= new_size - n;
		    ಯದಿ (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newwidth = new_size;
		}
		ಯದಿ (next_curwin_size == -1)
		{
		    ಯದಿ (!has_next_curwin)
			next_curwin_size = 0;
		    ಅನ್ಯಥಾ ಯದಿ (totwincount > 1
			    && (room + (totwincount - 2))
						  / (totwincount - 1) > p_wiw)
		    {
			// Can make all windows wider than 'winwidth', spread
			// the room equally.
			next_curwin_size = (room + p_wiw
					    + (totwincount - 1) * p_wmw
					    + (totwincount - 1)) / totwincount;
			room -= next_curwin_size - p_wiw;
		    }
		    ಅನ್ಯಥಾ
			next_curwin_size = p_wiw;
		}
	    }

	    ಯದಿ (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    ಯದಿ (fr->fr_next == ರಿಕ್ತ)
		// last frame gets all that remains (avoid roundoff error)
		new_size = width;
	    ಅನ್ಯಥಾ ಯದಿ (dir == 'v')
		new_size = fr->fr_width;
	    ಅನ್ಯಥಾ ಯದಿ (frame_fixed_width(fr))
	    {
		new_size = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    ಅನ್ಯಥಾ
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = frame_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_next == ರಿಕ್ತ ? extra_sep : 0))
								/ (p_wmw + 1);
		m = frame_minwidth(fr, next_curwin);
		ಯದಿ (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		ಅನ್ಯಥಾ
		    hnc = FALSE;
		ಯದಿ (hnc)	    // don't count next_curwin
		    --wincount;
		ಯದಿ (totwincount == 0)
		    new_size = room;
		ಅನ್ಯಥಾ
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		ಯದಿ (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wiw - (m - n);
		    ಯದಿ (next_curwin_size < 0)
			next_curwin_size = 0;
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		ಅನ್ಯಥಾ
		    room -= new_size;
		new_size += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    ಯದಿ (!current || dir != 'v' || topfr->fr_parent != ರಿಕ್ತ
		    || (new_size != fr->fr_width)
		    || frame_has_win(fr, next_curwin))
		win_equal_rec(next_curwin, current, fr, dir, col, row,
							    new_size, height);
	    col += new_size;
	    width -= new_size;
	    totwincount -= wincount;
	}
    }
    ಅನ್ಯಥಾ // topfr->fr_layout == FR_COL
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	ಯದಿ (dir != 'h')			// equalize frame heights
	{
	    totwincount = frame_wincount_in_height(topfr);
	    has_next_curwin = frame_has_win(topfr, next_curwin);

	    /*
	     * Compute height for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal height when counting p_wh for "next_curwin".
	     */
	    m = frame_minheight(topfr, next_curwin);
	    room = height - m;
	    ಯದಿ (room < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		next_curwin_size = p_wh + room;
		room = 0;
	    }
	    ಅನ್ಯಥಾ
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    ಯದಿ (!frame_fixed_height(fr))
			ಅನುವರ್ತ;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minheight(fr, NOWIN);
		    new_size = fr->fr_height;
		    ಯದಿ (frame_has_win(fr, next_curwin))
		    {
			room += p_wh - p_wmh;
			next_curwin_size = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			ಯದಿ (new_size < n + (p_wh - p_wmh))
			    new_size = n + (p_wh - p_wmh);
		    }
		    ಅನ್ಯಥಾ
			// These windows don't use up room.
			totwincount -= frame_wincount_in_height(fr);
		    room -= new_size - n;
		    ಯದಿ (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newheight = new_size;
		}
		ಯದಿ (next_curwin_size == -1)
		{
		    ಯದಿ (!has_next_curwin)
			next_curwin_size = 0;
		    ಅನ್ಯಥಾ ಯದಿ (totwincount > 1
			    && (room + (totwincount - 2))
						   / (totwincount - 1) > p_wh)
		    {
			// can make all windows higher than 'winheight',
			// spread the room equally.
			next_curwin_size = (room + p_wh
					   + (totwincount - 1) * p_wmh
					   + (totwincount - 1)) / totwincount;
			room -= next_curwin_size - p_wh;
		    }
		    ಅನ್ಯಥಾ
			next_curwin_size = p_wh;
		}
	    }

	    ಯದಿ (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    ಯದಿ (fr->fr_next == ರಿಕ್ತ)
		// last frame gets all that remains (avoid roundoff error)
		new_size = height;
	    ಅನ್ಯಥಾ ಯದಿ (dir == 'h')
		new_size = fr->fr_height;
	    ಅನ್ಯಥಾ ಯದಿ (frame_fixed_height(fr))
	    {
		new_size = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    ಅನ್ಯಥಾ
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = frame_minheight(fr, NOWIN);
		wincount = frame_wincount_in_height(fr);
		m = frame_minheight(fr, next_curwin);
		ಯದಿ (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		ಅನ್ಯಥಾ
		    hnc = FALSE;
		ಯದಿ (hnc)	    // don't count next_curwin
		    --wincount;
		ಯದಿ (totwincount == 0)
		    new_size = room;
		ಅನ್ಯಥಾ
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		ಯದಿ (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wh - (m - n);
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		ಅನ್ಯಥಾ
		    room -= new_size;
		new_size += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    ಯದಿ (!current || dir != 'h' || topfr->fr_parent != ರಿಕ್ತ
		    || (new_size != fr->fr_height)
		    || frame_has_win(fr, next_curwin))
		win_equal_rec(next_curwin, current, fr, dir, col, row,
							     width, new_size);
	    row += new_size;
	    height -= new_size;
	    totwincount -= wincount;
	}
    }
}

#ಯದಿ defined(FEAT_JOB_CHANNEL)
    ಶೂನ್ಯ
leaving_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    ಯದಿ (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	ಪ್ರತ್ಯಾಗಮ;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_buffer->b_prompt_ಸೇರಿಸು = restart_edit;
    ಯದಿ (restart_edit != 0 && mode_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    ಯದಿ ((State & MODE_ಸೇರಿಸು) && !stop_ಸೇರಿಸು_mode)
    {
	stop_ಸೇರಿಸು_mode = TRUE;
	ಯದಿ (win->w_buffer->b_prompt_ಸೇರಿಸು == NUL)
	    win->w_buffer->b_prompt_ಸೇರಿಸು = 'A';
    }
}

    ಶೂನ್ಯ
entering_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    ಯದಿ (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	ಪ್ರತ್ಯಾಗಮ;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    ಯದಿ (win->w_buffer->b_prompt_ಸೇರಿಸು != NUL)
	stop_ಸೇರಿಸು_mode = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    ಯದಿ ((State & MODE_ಸೇರಿಸು) == 0)
	restart_edit = win->w_buffer->b_prompt_ಸೇರಿಸು;
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
win_init_empty(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_ಸಾಲುಗಳು_valid = 0;
    wp->w_cursor.lnum = 1;
    wp->w_curswant = wp->w_cursor.col = 0;
    wp->w_cursor.coladd = 0;
    wp->w_pcmark.lnum = 1;	// pcmark not cleared but set to line 1
    wp->w_pcmark.col = 0;
    wp->w_prev_pcmark.lnum = 0;
    wp->w_prev_pcmark.col = 0;
    wp->w_topline = 1;
#ifdef FEAT_DIFF
    wp->w_topfill = 0;
#endif
    wp->w_botline = 2;
    wp->w_valid = 0;
#ಯದಿ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    wp->w_s = &wp->w_buffer->b_s;
#endif
#ifdef FEAT_TERMINAL
    term_reset_hlfwin(wp);
#endif
}

/*
 * Init the current window "curwin".
 * Called when a new file is being edited.
 */
    ಶೂನ್ಯ
curwin_init(ಶೂನ್ಯ)
{
    win_init_empty(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    ಶೂನ್ಯ
close_windows(
    buf_T	*buf,
    ಪೂರ್ಣಾಙ್ಕ		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = tabpage_index(ರಿಕ್ತ);

    ++RedrawingDisabled;

    ಪ್ರತಿ (wp = firstwin; wp != ರಿಕ್ತ && !ONE_WINDOW; )
    {
	ಯದಿ (wp->w_buffer == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_buffer->b_locked > 0))
	{
	    ಯದಿ (window_layout_locked(CMD_SIZE))
		ಗಚ್ಛ theend;  // Only give one error message.
	    ಯದಿ (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		ಭಙ್ಗ;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	ಅನ್ಯಥಾ
	    wp = wp->w_next;
    }

    // Also check windows in other tab pages.
    ಪ್ರತಿ (tp = first_tabpage; tp != ರಿಕ್ತ; tp = nexttp)
    {
	nexttp = tp->tp_next;
	ಯದಿ (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		ಯದಿ (wp->w_buffer == buf
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0))
		{
		    ಯದಿ (window_layout_locked(CMD_SIZE))
			ಗಚ್ಛ theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = first_tabpage;
		    ಭಙ್ಗ;
		}
    }

theend:
    ಯದಿ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    ಯದಿ (ಎಣಿಕೆ != tabpage_index(ರಿಕ್ತ))
	apply_autocmds(EVENT_TABCLOSED, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    ಪೂರ್ಣಾಙ್ಕ
last_window(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (one_window() && first_tabpage->tp_next == ರಿಕ್ತ);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    ಪೂರ್ಣಾಙ್ಕ
one_window(ಶೂನ್ಯ)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		seen_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	ಯದಿ (!is_aucmd_win(wp))
	{
	    ಯದಿ (seen_one)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    seen_one = TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
close_last_window_tabpage(
    win_T	*win,
    ಪೂರ್ಣಾಙ್ಕ		free_buf,
    tabpage_T   *prev_curtab)
{
    ಯದಿ (!ONE_WINDOW)
	ಪ್ರತ್ಯಾಗಮ FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_buffer != ರಿಕ್ತ);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    ಯದಿ (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    ಯದಿ (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
#ಯದಿ defined(FEAT_TABPANEL)
    ಯದಿ (p_stpl > 0)
	shell_new_columns();
#endif
    apply_autocmds(EVENT_WINENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    ಯದಿ (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_close_buffer(win_T *win, ಪೂರ್ಣಾಙ್ಕ action, ಪೂರ್ಣಾಙ್ಕ abort_if_last)
{
#ifdef FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    ಯದಿ (win->w_buffer != ರಿಕ್ತ)
	reset_synblock(win);
#endif

#ifdef FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    ಯದಿ (win->w_buffer != ರಿಕ್ತ && bt_quickfix(win->w_buffer)
					&& win->w_buffer->b_nwindows == 1)
	win->w_buffer->b_p_bl = FALSE;
#endif

    // Close the link to the buffer.
    ಯದಿ (win->w_buffer != ರಿಕ್ತ)
    {
	bufref_T    bufref;

	set_bufref(&bufref, curbuf);
	++win->w_locked;
	close_buffer(win, win->w_buffer, action, abort_if_last, TRUE, TRUE);
	ಯದಿ (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	ಯದಿ (!bufref_valid(&bufref))
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
    ಪೂರ್ಣಾಙ್ಕ
win_close(win_T *win, ಪೂರ್ಣಾಙ್ಕ free_buf)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		other_buffer = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		close_curwin = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		dir;
    ಪೂರ್ಣಾಙ್ಕ		help_window = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		quickfix_window = FALSE;
    tabpage_T   *prev_curtab = curtab;
    frame_T	*win_frame = win->w_frame->fr_parent;
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ		had_diffmode = win->w_p_diff;
#endif
#ifdef MESSAGE_QUEUE
    ಪೂರ್ಣಾಙ್ಕ		did_decrement = FALSE;
#endif

#ಯದಿ defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    ಯದಿ (may_close_term_popup() == OK)
	ಪ್ರತ್ಯಾಗಮ OK;
#endif
    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (last_window())
    {
	emsg(_(e_cannot_close_last_window));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (window_layout_locked(CMD_close))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (win_locked(win) || (win->w_buffer != ರಿಕ್ತ
					       && win->w_buffer->b_locked > 0))
	ಪ್ರತ್ಯಾಗಮ FAIL; // window is already being closed
    ಯದಿ (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_window())
    {
	emsg(_(e_cannot_close_window_only_autocmd_window_would_remain));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    ಯದಿ (close_last_window_tabpage(win, free_buf, prev_curtab))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    ಯದಿ (bt_help(win->w_buffer))
	help_window = TRUE;
    ಅನ್ಯಥಾ
	clear_snapshot(curtab, SNAP_HELP_IDX);

    ಯದಿ (bt_quickfix(win->w_buffer))
	quickfix_window = TRUE;
    ಅನ್ಯಥಾ
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    ಯದಿ (win == curwin)
    {
#ifdef FEAT_JOB_CHANNEL
	leaving_window(curwin);
#endif
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, ರಿಕ್ತ));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	ಯದಿ (wp->w_buffer != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_buffer = TRUE;
	    ಯದಿ (!win_valid(win))
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	    ಯದಿ (!win_valid(win))
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    --win->w_locked;
	    ಯದಿ (last_window())
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	ಯದಿ (!win_valid(win))
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	--win->w_locked;
	ಯದಿ (last_window())
	    ಪ್ರತ್ಯಾಗಮ FAIL;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	ಯದಿ (aborting())
	    ಪ್ರತ್ಯಾಗಮ FAIL;
#endif
    }

#ifdef FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    ಯದಿ (gui.in_use)
	out_flush();
#endif

#ifdef FEAT_PROP_POPUP
    ಯದಿ (popup_win_closed(win) && !win_valid(win))
	ಪ್ರತ್ಯಾಗಮ FAIL;
#endif

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    ಯದಿ (!win_valid_any_tab(win))
	ಪ್ರತ್ಯಾಗಮ OK;

    win_close_buffer(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    ಯದಿ (win_valid(win) && win->w_buffer == ರಿಕ್ತ
#ಯದಿ defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#endif
	    && last_window())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	ಯದಿ (curwin->w_buffer == ರಿಕ್ತ)
	    curwin->w_buffer = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    ಯದಿ (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_buffer == ರಿಕ್ತ)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    ಯದಿ (!win_valid(win) || last_window()
	    || close_last_window_tabpage(win, free_buf, prev_curtab))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++split_disallowed;
#ifdef MESSAGE_QUEUE
    ++dont_parse_messages;
#endif

    ಯದಿ (win->w_buffer != ರಿಕ್ತ)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, ರಿಕ್ತ);

    ಯದಿ (help_window || quickfix_window)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	ಯದಿ (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    ಯದಿ (win == curwin)
    {
	curwin = wp;
#ifdef FEAT_QUICKFIX
	ಯದಿ (wp->w_p_pvw || bt_quickfix(wp->w_buffer))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    ಪ್ರತಿ (;;)
	    {
		ಯದಿ (wp->w_next == ರಿಕ್ತ)
		    wp = firstwin;
		ಅನ್ಯಥಾ
		    wp = wp->w_next;
		ಯದಿ (wp == curwin)
		    ಭಙ್ಗ;
		ಯದಿ (!wp->w_p_pvw && !bt_quickfix(wp->w_buffer))
		{
		    curwin = wp;
		    ಭಙ್ಗ;
		}
	    }
	}
#endif
	curbuf = curwin->w_buffer;
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
    last_status(FALSE);

    ಯದಿ (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_frame->fr_parent == win_frame, dir);
    ಅನ್ಯಥಾ
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    ಯದಿ (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ifdef MESSAGE_QUEUE
	did_decrement =
#ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)
#endif
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	ಯದಿ (other_buffer)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    }

    ಯದಿ (ONE_WINDOW && curwin->w_locked && curbuf->b_locked_split
	    && first_tabpage->tp_next != ರಿಕ್ತ)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);

    --split_disallowed;
#ifdef MESSAGE_QUEUE
    ಯದಿ (!did_decrement)
	--dont_parse_messages;
#endif

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    ಯದಿ (help_window || quickfix_window)
	restore_snapshot(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ifdef FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    ಯದಿ (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	ಪೂರ್ಣಾಙ್ಕ	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    ಯದಿ (dwin->w_p_diff)
		++diffcount;
	ಯದಿ (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#endif

#ಯದಿ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    ಯದಿ (gui.in_use && !win_hasvertsplit())
	gui_init_which_components(ರಿಕ್ತ);
#endif

    redraw_all_later(UPD_NOT_VALID);
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
trigger_winnewpre(ಶೂನ್ಯ)
{
    window_layout_lock();
    apply_autocmds(EVENT_WINNEWPRE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, ರಿಕ್ತ);
    window_layout_unlock();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
trigger_winclosed(win_T *win)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ;
    recursive = TRUE;
    vim_snprintf((ವರ್ಣ *)winid, ಪರಿಮಾಣ(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_buffer);
    recursive = FALSE;
}

    ಶೂನ್ಯ
trigger_tabclosedpre(tabpage_T *tp)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    ಯದಿ (!has_tabclosedpre() || recursive)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    window_layout_lock();
    apply_autocmds(EVENT_TABCLOSEDPRE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, ರಿಕ್ತ);
    window_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    ಯದಿ (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    ಅನ್ಯಥಾ
	// fall back to the first tabpage
	goto_tabpage_tp(first_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    ಶೂನ್ಯ
snapshot_windows_scroll_size(ಶೂನ್ಯ)
{
    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	wp->w_last_topline = wp->w_topline;
#ifdef FEAT_DIFF
	wp->w_last_topfill = wp->w_topfill;
#endif
	wp->w_last_leftcol = wp->w_leftcol;
	wp->w_last_skipcol = wp->w_skipcol;
	wp->w_last_width = wp->w_width;
	wp->w_last_height = wp->w_height;
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ did_initial_scroll_size_snapshot = FALSE;

    ಶೂನ್ಯ
may_make_initial_scroll_size_snapshot(ಶೂನ್ಯ)
{
    ಯದಿ (!did_initial_scroll_size_snapshot)
    {
	did_initial_scroll_size_snapshot = TRUE;
	snapshot_windows_scroll_size();
    }
}

#ifdef FEAT_EVAL
/*
 * Create a dictionary with information about size and scroll changes in a
 * window.
 * Returns the dictionary with refcount set to one.
 * Returns NULL when out of memory.
 */
    ಸ್ಥಿರ dict_T *
make_win_info_dict(
	ಪೂರ್ಣಾಙ್ಕ width,
	ಪೂರ್ಣಾಙ್ಕ height,
	ಪೂರ್ಣಾಙ್ಕ topline,
# ifdef FEAT_DIFF
	ಪೂರ್ಣಾಙ್ಕ topfill,
# endif
	ಪೂರ್ಣಾಙ್ಕ leftcol,
	ಪೂರ್ಣಾಙ್ಕ skipcol)
{
    dict_T *d = dict_alloc();
    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    ಯಾವತ್ (1)
    {
	typval_T tv;
	tv.v_lock = 0;
	tv.v_type = VAR_ಸಂಖ್ಯೆ;

	tv.vval.v_ಸಂಖ್ಯೆ = width;
	ಯದಿ (dict_add_tv(d, "width", &tv) == FAIL)
	    ಭಙ್ಗ;
	tv.vval.v_ಸಂಖ್ಯೆ = height;
	ಯದಿ (dict_add_tv(d, "height", &tv) == FAIL)
	    ಭಙ್ಗ;
	tv.vval.v_ಸಂಖ್ಯೆ = topline;
	ಯದಿ (dict_add_tv(d, "topline", &tv) == FAIL)
	    ಭಙ್ಗ;
# ifdef FEAT_DIFF
	tv.vval.v_ಸಂಖ್ಯೆ = topfill;
# ಅನ್ಯಥಾ
	tv.vval.v_ಸಂಖ್ಯೆ = 0;
# endif
	ಯದಿ (dict_add_tv(d, "topfill", &tv) == FAIL)
	    ಭಙ್ಗ;
	tv.vval.v_ಸಂಖ್ಯೆ = leftcol;
	ಯದಿ (dict_add_tv(d, "leftcol", &tv) == FAIL)
	    ಭಙ್ಗ;
	tv.vval.v_ಸಂಖ್ಯೆ = skipcol;
	ಯದಿ (dict_add_tv(d, "skipcol", &tv) == FAIL)
	    ಭಙ್ಗ;
	ಪ್ರತ್ಯಾಗಮ d;
    }
    dict_unref(d);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

// Return values of check_window_scroll_resize():
#define CWSR_SCROLLED	1  // at least one window scrolled
#define CWSR_RESIZED	2  // at least one window size changed

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
check_window_scroll_resize(
	ಪೂರ್ಣಾಙ್ಕ	*size_ಎಣಿಕೆ,
	win_T	**first_scroll_win,
	win_T	**first_size_win,
	list_T	*winlist UNUSED,
	dict_T	*v_event UNUSED)
{
#ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ listidx = 0;
    ಪೂರ್ಣಾಙ್ಕ tot_width = 0;
    ಪೂರ್ಣಾಙ್ಕ tot_height = 0;
    ಪೂರ್ಣಾಙ್ಕ tot_topline = 0;
# ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ tot_topfill = 0;
# endif
    ಪೂರ್ಣಾಙ್ಕ tot_leftcol = 0;
    ಪೂರ್ಣಾಙ್ಕ tot_skipcol = 0;
#endif

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	ಪೂರ್ಣಾಙ್ಕ ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	ಪೂರ್ಣಾಙ್ಕ size_changed = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_last_width != wp->w_width
			    || wp->w_last_height != wp->w_height);
	ಯದಿ (size_changed)
	{
#ifdef FEAT_EVAL
	    ಯದಿ (winlist != ರಿಕ್ತ)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_lock = 0;
		tv.v_type = VAR_ಸಂಖ್ಯೆ;
		tv.vval.v_ಸಂಖ್ಯೆ = wp->w_id;
		list_set_item(winlist, listidx++, &tv);
	    }
	    ಅನ್ಯಥಾ
#endif
		ಯದಿ (size_ಎಣಿಕೆ != ರಿಕ್ತ)
	    {
		++*size_ಎಣಿಕೆ;
		ಯದಿ (*first_size_win == ರಿಕ್ತ)
		    *first_size_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		ಯದಿ (*first_scroll_win == ರಿಕ್ತ && !ignore_scroll)
		    *first_scroll_win = wp;
	    }
	}

	ಪೂರ್ಣಾಙ್ಕ scroll_changed = !ignore_scroll
				&& (wp->w_last_topline != wp->w_topline
#ifdef FEAT_DIFF
				|| wp->w_last_topfill != wp->w_topfill
#endif
				|| wp->w_last_leftcol != wp->w_leftcol
				|| wp->w_last_skipcol != wp->w_skipcol);
	ಯದಿ (scroll_changed
	    && first_scroll_win != ರಿಕ್ತ && *first_scroll_win == ರಿಕ್ತ)
	    *first_scroll_win = wp;

#ifdef FEAT_EVAL
	ಯದಿ ((size_changed || scroll_changed) && v_event != ರಿಕ್ತ)
	{
	    // Add info about this window to the v:event dictionary.
	    ಪೂರ್ಣಾಙ್ಕ width = wp->w_width - wp->w_last_width;
	    ಪೂರ್ಣಾಙ್ಕ height = wp->w_height - wp->w_last_height;
	    ಪೂರ್ಣಾಙ್ಕ topline = wp->w_topline - wp->w_last_topline;
# ifdef FEAT_DIFF
	    ಪೂರ್ಣಾಙ್ಕ topfill = wp->w_topfill - wp->w_last_topfill;
# endif
	    ಪೂರ್ಣಾಙ್ಕ leftcol = wp->w_leftcol - wp->w_last_leftcol;
	    ಪೂರ್ಣಾಙ್ಕ skipcol = wp->w_skipcol - wp->w_last_skipcol;
	    dict_T *d = make_win_info_dict(width, height, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    ಯದಿ (d == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    ವರ್ಣ winid[NUMBUFLEN];
	    vim_snprintf(winid, ಪರಿಮಾಣ(winid), "%d", wp->w_id);
	    ಯದಿ (dict_add_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		ಭಙ್ಗ;
	    }
	    --d->dv_refcount;

	    tot_width += abs(width);
	    tot_height += abs(height);
	    tot_topline += abs(topline);
# ifdef FEAT_DIFF
	    tot_topfill += abs(topfill);
# endif
	    tot_leftcol += abs(leftcol);
	    tot_skipcol += abs(skipcol);
	}
#endif
    }

#ifdef FEAT_EVAL
    ಯದಿ (v_event != ರಿಕ್ತ)
    {
	dict_T *alldict = make_win_info_dict(tot_width, tot_height, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	ಯದಿ (alldict != ರಿಕ್ತ)
	{
	    ಯದಿ (dict_add_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    ಅನ್ಯಥಾ
		--alldict->dv_refcount;
	}
    }
#endif
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    ಶೂನ್ಯ
may_trigger_win_scrolled_resized(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    recursive = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		    do_resize = has_winresized();
    ಪೂರ್ಣಾಙ್ಕ		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    ಯದಿ (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_size_snapshot)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ size_ಎಣಿಕೆ = 0;
    win_T *first_scroll_win = ರಿಕ್ತ, *first_size_win = ರಿಕ್ತ;
    check_window_scroll_resize(&size_ಎಣಿಕೆ, &first_scroll_win, &first_size_win,
								    ರಿಕ್ತ, ರಿಕ್ತ);
    ಪೂರ್ಣಾಙ್ಕ trigger_resize = do_resize && size_ಎಣಿಕೆ > 0;
    ಪೂರ್ಣಾಙ್ಕ trigger_scroll = do_scroll && first_scroll_win != ರಿಕ್ತ;
    ಯದಿ (!trigger_resize && !trigger_scroll)
	ಪ್ರತ್ಯಾಗಮ;  // no relevant changes
#ifdef FEAT_EVAL
    list_T *windows_list = ರಿಕ್ತ;
    ಯದಿ (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_ವಸ್ತುಗಳು(size_ಎಣಿಕೆ);
	ಯದಿ (windows_list != ರಿಕ್ತ)
	    check_window_scroll_resize(ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, windows_list, ರಿಕ್ತ);
    }

    dict_T *scroll_dict = ರಿಕ್ತ;
    ಯದಿ (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	ಯದಿ (scroll_dict != ರಿಕ್ತ)
	{
	    scroll_dict->dv_refcount = 1;
	    check_window_scroll_resize(ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, scroll_dict);
	}
    }
#endif

    // WinScrolled/WinResized are triggered only once, even when multiple
    // windows scrolled or changed size.  Store the current values before
    // triggering the event, if a scroll or resize happens as a side effect
    // then WinScrolled/WinResized is triggered for that later.
    snapshot_windows_scroll_size();

    // "curwin" may be different from the actual current window, make
    // sure it can be restored.
    window_layout_lock();
    recursive = TRUE;

    // If both are to be triggered do WinResized first.
    ಯದಿ (trigger_resize
#ifdef FEAT_EVAL
	    && windows_list != ರಿಕ್ತ
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	ಯದಿ (dict_add_list(v_event, "windows", windows_list) == OK)
	{
	    dict_set_ವಸ್ತುಗಳು_ro(v_event);
#endif
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((ವರ್ಣ *)winid, ಪರಿಮಾಣ(winid), "%d",
							 first_size_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     first_size_win->w_buffer);
#ifdef FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#endif
    }

    ಯದಿ (trigger_scroll
#ifdef FEAT_EVAL
	    && scroll_dict != ರಿಕ್ತ
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", ರಿಕ್ತ);
	dict_set_ವಸ್ತುಗಳು_ro(v_event);
	dict_unref(scroll_dict);
#endif
	char_u winid[NUMBUFLEN];
	vim_snprintf((ವರ್ಣ *)winid, ಪರಿಮಾಣ(winid), "%d",
						       first_scroll_win->w_id);
	apply_autocmds(EVENT_WINSCROLLED, winid, winid, FALSE,
						   first_scroll_win->w_buffer);
#ifdef FEAT_EVAL
	restore_v_event(v_event, &save_v_event);
#endif
    }

    recursive = FALSE;
    window_layout_unlock();
}

/*
 * Close window "win" in tab page "tp", which is not the current tab page.
 * This may be the last window in that tab page and result in closing the tab,
 * thus "tp" may become invalid!
 * Caller must check if buffer is hidden and whether the tabline needs to be
 * updated.
 */
    ಶೂನ್ಯ
win_close_othertab(win_T *win, ಪೂರ್ಣಾಙ್ಕ free_buf, tabpage_T *tp)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		dir;
    tabpage_T   *ptp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    ಯದಿ (window_layout_locked(CMD_SIZE))
	ಪ್ರತ್ಯಾಗಮ;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    ಯದಿ (win_locked(win) || (win->w_buffer != ರಿಕ್ತ
					       && win->w_buffer->b_locked > 0))
	ಪ್ರತ್ಯಾಗಮ; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    ಯದಿ (win->w_buffer != ರಿಕ್ತ)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	ಯದಿ (!win_valid_any_tab(win))
	    ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	ಯದಿ (!win_valid_any_tab(win))
	    ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (win->w_buffer != ರಿಕ್ತ)
	// Close the link to the buffer.
	close_buffer(win, win->w_buffer, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    ಪ್ರತಿ (ptp = first_tabpage; ptp != ರಿಕ್ತ && ptp != tp; ptp = ptp->tp_next)
	;
    ಯದಿ (ptp == ರಿಕ್ತ || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	ಯದಿ (win_valid_any_tab(win) && win->w_buffer == ರಿಕ್ತ)
	{
	    win->w_buffer = firstbuf;
	    ++firstbuf->b_nwindows;
	    ಯದಿ (win == curwin)
		curbuf = curwin->w_buffer;
	    win_init_empty(win);
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Autocommands may have closed the window already.
    ಪ್ರತಿ (wp = tp->tp_firstwin; wp != ರಿಕ್ತ && wp != win; wp = wp->w_next)
	;
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // When closing the last window in a tab page remove the tab page.
    ಯದಿ (tp->tp_firstwin == tp->tp_lastwin)
    {
	ಪೂರ್ಣಾಙ್ಕ	h = tabline_height();

	ಯದಿ (tp == first_tabpage)
	    first_tabpage = tp->tp_next;
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (ptp = first_tabpage; ptp != ರಿಕ್ತ && ptp->tp_next != tp;
							   ptp = ptp->tp_next)
		;
	    ಯದಿ (ptp == ರಿಕ್ತ)
	    {
		internal_error("win_close_othertab()");
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ptp->tp_next = tp->tp_next;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	ಯದಿ (h != tabline_height())
	    shell_new_rows();
    }

    ಯದಿ (win->w_buffer != ರಿಕ್ತ)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    ಯದಿ (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    ಸ್ಥಿರ win_T *
win_free_mem(
    win_T	*win,
    ಪೂರ್ಣಾಙ್ಕ		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == ರಿಕ್ತ ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_frame;
    wp = winframe_remove(win, dirp, tp, ರಿಕ್ತ);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    ಯದಿ (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    ಪ್ರತ್ಯಾಗಮ wp;
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
win_free_all(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = ರಿಕ್ತ;
    cmdwin_win = ರಿಕ್ತ;

    ಯಾವತ್ (first_tabpage->tp_next != ರಿಕ್ತ)
	tabpage_close(TRUE);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < AUCMD_WIN_ಎಣಿಕೆ; ++i)
	ಯದಿ (aucmd_win[i].auc_win != ರಿಕ್ತ)
	{
	    (ಶೂನ್ಯ)win_free_mem(aucmd_win[i].auc_win, &dummy, ರಿಕ್ತ);
	    aucmd_win[i].auc_win = ರಿಕ್ತ;
	}

    ಯಾವತ್ (firstwin != ರಿಕ್ತ)
	(ಶೂನ್ಯ)win_free_mem(firstwin, &dummy, ರಿಕ್ತ);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = ರಿಕ್ತ;
}
#endif

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    ಪೂರ್ಣಾಙ್ಕ		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    frame_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    frame_T	*frp, *frp2, *frp3;
    frame_T	*frp_close = win->w_frame;
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		row, col;

    /*
     * If there is only one window there is nothing to remove.
     */
    ಯದಿ (tp == ರಿಕ್ತ ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    frame_locked++;

    // Save the position of the containing frame (which will also contain the
    // altframe) before we remove anything, to recompute window positions later.
    wp = frame2win(frp_close->fr_parent);
    row = wp->w_winrow;
    col = wp->w_wincol;

    /*
     * Remove the window from its frame.
     */
    frp2 = win_altframe(win, tp);
    wp = frame2win(frp2);

    // If this is a rightmost window, remove vertical separators to the left.
    ಯದಿ (win->w_vsep_width == 0 && frp_close->fr_parent->fr_layout == FR_ROW &&
	    frp_close->fr_prev != ರಿಕ್ತ)
	frame_set_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    frame_remove(frp_close);

    ಯದಿ (frp_close->fr_parent->fr_layout == FR_COL)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	ಯದಿ (frp2->fr_win != ರಿಕ್ತ && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    ಯಾವತ್ (frp != ರಿಕ್ತ || frp3 != ರಿಕ್ತ)
	    {
		ಯದಿ (frp != ರಿಕ್ತ)
		{
		    ಯದಿ (!frame_fixed_height(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			ಭಙ್ಗ;
		    }
		    frp = frp->fr_prev;
		}
		ಯದಿ (frp3 != ರಿಕ್ತ)
		{
		    ಯದಿ (frp3->fr_win != ರಿಕ್ತ && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			ಭಙ್ಗ;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_height(frp2, frp2->fr_height + frp_close->fr_height,
			    frp2 == frp_close->fr_next, !frame_fixed_height(frp2), FALSE);
	*dirp = 'v';
    }
    ಅನ್ಯಥಾ
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	ಯದಿ (frp2->fr_win != ರಿಕ್ತ && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    ಯಾವತ್ (frp != ರಿಕ್ತ || frp3 != ರಿಕ್ತ)
	    {
		ಯದಿ (frp != ರಿಕ್ತ)
		{
		    ಯದಿ (!frame_fixed_width(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			ಭಙ್ಗ;
		    }
		    frp = frp->fr_prev;
		}
		ಯದಿ (frp3 != ರಿಕ್ತ)
		{
		    ಯದಿ (frp3->fr_win != ರಿಕ್ತ && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			ಭಙ್ಗ;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_width(frp2, frp2->fr_width + frp_close->fr_width,
			    frp2 == frp_close->fr_next, !frame_fixed_width(frp2));
	*dirp = 'h';
    }

    // If the altframe wasn't adjacent and left/above, resizing it will have
    // changed window positions within the parent frame.  Recompute them.
    ಯದಿ (frp2 != frp_close->fr_prev)
	frame_comp_pos(frp_close->fr_parent, &row, &col);

    ಯದಿ (unflat_altfr == ರಿಕ್ತ)
	frame_flatten(frp2);
    ಅನ್ಯಥಾ
	*unflat_altfr = frp2;

    frame_locked--;

    ಪ್ರತ್ಯಾಗಮ wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_flatten(frame_T *frp)
{
    frame_T	*frp2, *frp3;

    ಯದಿ (frp->fr_next != ರಿಕ್ತ || frp->fr_prev != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_parent->fr_layout = frp->fr_layout;
    frp->fr_parent->fr_child = frp->fr_child;
    FOR_ALL_FRAMES(frp2, frp->fr_child)
	frp2->fr_parent = frp->fr_parent;
    frp->fr_parent->fr_win = frp->fr_win;
    ಯದಿ (frp->fr_win != ರಿಕ್ತ)
	frp->fr_win->w_frame = frp->fr_parent;
    frp2 = frp->fr_parent;
    ಯದಿ (topframe->fr_child == frp)
	topframe->fr_child = frp2;
    vim_free(frp);

    frp = frp2->fr_parent;
    ಯದಿ (frp != ರಿಕ್ತ && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	ಯದಿ (frp->fr_child == frp2)
	    frp->fr_child = frp2->fr_child;
	frp2->fr_child->fr_prev = frp2->fr_prev;
	ಯದಿ (frp2->fr_prev != ರಿಕ್ತ)
	    frp2->fr_prev->fr_next = frp2->fr_child;
	ಪ್ರತಿ (frp3 = frp2->fr_child; ; frp3 = frp3->fr_next)
	{
	    frp3->fr_parent = frp;
	    ಯದಿ (frp3->fr_next == ರಿಕ್ತ)
	    {
		frp3->fr_next = frp2->fr_next;
		ಯದಿ (frp2->fr_next != ರಿಕ್ತ)
		    frp2->fr_next->fr_prev = frp3;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (topframe->fr_child == frp2)
	    topframe->fr_child = frp;
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
winframe_restore(win_T *wp, ಪೂರ್ಣಾಙ್ಕ dir, frame_T *unflat_altfr)
{
    frame_T	*frp = wp->w_frame;

    // Put "wp"'s frame back where it was.
    ಯದಿ (frp->fr_prev != ರಿಕ್ತ)
	frame_append(frp->fr_prev, frp);
    ಅನ್ಯಥಾ
	frame_ಸೇರಿಸು(frp->fr_next, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    ಯದಿ (wp->w_vsep_width == 0
	    && frp->fr_parent->fr_layout == FR_ROW && frp->fr_prev != ರಿಕ್ತ)
	frame_set_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    ಯದಿ (wp->w_status_height == 0
	    && frp->fr_parent->fr_layout == FR_COL && frp->fr_prev != ರಿಕ್ತ)
	frame_add_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    ಯದಿ (dir == 'v')
    {
	frame_new_height(unflat_altfr, unflat_altfr->fr_height - frp->fr_height,
		unflat_altfr == frp->fr_next, FALSE, FALSE);
    }
    ಅನ್ಯಥಾ ಯದಿ (dir == 'h')
    {
	frame_new_width(unflat_altfr, unflat_altfr->fr_width - frp->fr_width,
		unflat_altfr == frp->fr_next, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    ಯದಿ (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_parent);
	ಪೂರ್ಣಾಙ್ಕ	row = topleft->w_winrow;
	ಪೂರ್ಣಾಙ್ಕ	col = topleft->w_wincol;

	frame_comp_pos(frp->fr_parent, &row, &col);
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
    ಸ್ಥಿರ frame_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    frame_T	*other_fr, *target_fr;

    ಯದಿ (tp == ರಿಕ್ತ ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	ಪ್ರತ್ಯಾಗಮ alt_tabpage()->tp_curwin->w_frame;

    frp = win->w_frame;

    ಯದಿ (frp->fr_prev == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ frp->fr_next;
    ಯದಿ (frp->fr_next == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    // window
    target_fr = frp->fr_next;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    ಯದಿ (frp->fr_parent != ರಿಕ್ತ && frp->fr_parent->fr_layout == FR_COL && p_sb)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    ಯದಿ (frp->fr_parent != ರಿಕ್ತ && frp->fr_parent->fr_layout == FR_ROW && p_spr)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    ಯದಿ (frp->fr_parent != ರಿಕ್ತ && frp->fr_parent->fr_layout == FR_ROW)
    {
	ಯದಿ (frame_fixed_width(target_fr) && !frame_fixed_width(other_fr))
	    target_fr = other_fr;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (frame_fixed_height(target_fr) && !frame_fixed_height(other_fr))
	    target_fr = other_fr;
    }

    ಪ್ರತ್ಯಾಗಮ target_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    ಸ್ಥಿರ tabpage_T *
alt_tabpage(ಶೂನ್ಯ)
{
    tabpage_T	*tp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		forward;

    // Use the last accessed tab page, if possible.
    ಯದಿ ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	ಪ್ರತ್ಯಾಗಮ lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_next != ರಿಕ್ತ &&
	    ((tcl_flags & TCL_ಎಡ) == 0 || curtab == first_tabpage);

    ಯದಿ (forward)
	tp = curtab->tp_next;
    ಅನ್ಯಥಾ
	// Use the previous tab page.
	ಪ್ರತಿ (tp = first_tabpage; tp->tp_next != curtab; tp = tp->tp_next)
	    ;

    ಪ್ರತ್ಯಾಗಮ tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    ಸ್ಥಿರ win_T *
frame2win(frame_T *frp)
{
    ಯಾವತ್ (frp->fr_win == ರಿಕ್ತ)
	frp = frp->fr_child;
    ಪ್ರತ್ಯಾಗಮ frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
frame_has_win(frame_T *frp, win_T *wp)
{
    frame_T	*p;

    ಯದಿ (frp->fr_layout == FR_LEAF)
	ಪ್ರತ್ಯಾಗಮ frp->fr_win == wp;

    FOR_ALL_FRAMES(p, frp->fr_child)
	ಯದಿ (frame_has_win(p, wp))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ min_set_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_new_height(
    frame_T	*topfrp,
    ಪೂರ್ಣಾಙ್ಕ		height,
    ಪೂರ್ಣಾಙ್ಕ		topfirst,	// resize topmost contained frame first
    ಪೂರ್ಣಾಙ್ಕ		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    ಪೂರ್ಣಾಙ್ಕ		set_ch)		// set 'cmdheight' to resize topframe
{
    frame_T	*frp;
    ಪೂರ್ಣಾಙ್ಕ		extra_ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಙ್ಕ		h;

    ಯದಿ (topfrp->fr_parent == ರಿಕ್ತ && set_ch)
    {
	// topframe: update the command line height, with side effects.
	ಪೂರ್ಣಾಙ್ಕ new_ch = MAX(min_set_ch, p_ch + topfrp->fr_height - height);
	ಪೂರ್ಣಾಙ್ಕ save_ch = min_set_ch;
	ಯದಿ (new_ch != p_ch)
	    set_option_ಮೌಲ್ಯ((char_u *)"cmdheight", new_ch, ರಿಕ್ತ, 0);
	min_set_ch = save_ch;
	height = MIN(height, ROWS_AVAIL);
    }
    ಯದಿ (topfrp->fr_win != ರಿಕ್ತ)
    {
	// Simple case: just one window.
	win_new_height(topfrp->fr_win,
				    height - topfrp->fr_win->w_status_height
					      - WINBAR_HEIGHT(topfrp->fr_win));
    }
    ಅನ್ಯಥಾ ಯದಿ (topfrp->fr_layout == FR_ROW)
    {
	ಕುರು
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_height(frp, height, topfirst, wfh, set_ch);
		ಯದಿ (frp->fr_height > height)
		{
		    // Could not fit the windows, make the whole row higher.
		    height = frp->fr_height;
		    ಭಙ್ಗ;
		}
	    }
	}
	ಯಾವತ್ (frp != ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_child;
	ಯದಿ (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    ಯಾವತ್ (frame_fixed_height(frp))
	    {
		frp = frp->fr_next;
		ಯದಿ (frp == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ;	    // no frame without 'wfh', give up
	    }
	ಯದಿ (!topfirst)
	{
	    // Find the bottom frame of this column
	    ಯಾವತ್ (frp->fr_next != ರಿಕ್ತ)
		frp = frp->fr_next;
	    ಯದಿ (wfh)
		// Advance back for frames with one window with 'wfh' set.
		ಯಾವತ್ (frame_fixed_height(frp))
		    frp = frp->fr_prev;
	}

	extra_ಸಾಲುಗಳು = height - topfrp->fr_height;
	ಯದಿ (extra_ಸಾಲುಗಳು < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    ಯಾವತ್ (frp != ರಿಕ್ತ)
	    {
		h = frame_minheight(frp, ರಿಕ್ತ);
		ಯದಿ (frp->fr_height + extra_ಸಾಲುಗಳು < h)
		{
		    extra_ಸಾಲುಗಳು += frp->fr_height - h;
		    frame_new_height(frp, h, topfirst, wfh, set_ch);
		}
		ಅನ್ಯಥಾ
		{
		    frame_new_height(frp, frp->fr_height + extra_ಸಾಲುಗಳು,
							topfirst, wfh, set_ch);
		    ಭಙ್ಗ;
		}
		ಯದಿ (topfirst)
		{
		    ಕುರು
			frp = frp->fr_next;
		    ಯಾವತ್ (wfh && frp != ರಿಕ್ತ && frame_fixed_height(frp));
		}
		ಅನ್ಯಥಾ
		{
		    ಕುರು
			frp = frp->fr_prev;
		    ಯಾವತ್ (wfh && frp != ರಿಕ್ತ && frame_fixed_height(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		ಯದಿ (frp == ರಿಕ್ತ)
		    height -= extra_ಸಾಲುಗಳು;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (extra_ಸಾಲುಗಳು > 0)
	{
	    // increase height of bottom or top frame
	    frame_new_height(frp, frp->fr_height + extra_ಸಾಲುಗಳು, topfirst, wfh,
									set_ch);
	}
    }
    topfrp->fr_height = height;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
frame_fixed_height(frame_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    ಯದಿ (frp->fr_win != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ frp->fr_win->w_p_wfh;

    ಯದಿ (frp->fr_layout == FR_ROW)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    ಯದಿ (frame_fixed_height(frp))
		ಪ್ರತ್ಯಾಗಮ TRUE;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	ಯದಿ (!frame_fixed_height(frp))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
frame_fixed_width(frame_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    ಯದಿ (frp->fr_win != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ frp->fr_win->w_p_wfw;

    ಯದಿ (frp->fr_layout == FR_COL)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    ಯದಿ (frame_fixed_width(frp))
		ಪ್ರತ್ಯಾಗಮ TRUE;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	ಯದಿ (!frame_fixed_width(frp))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_add_statusline(frame_T *frp)
{
    win_T	*wp;

    ಯದಿ (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	wp->w_status_height = statusline_height(wp);
    }
    ಅನ್ಯಥಾ ಯದಿ (frp->fr_layout == FR_ROW)
    {
	// Handle all the frames in the row.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_statusline(frp);
    }
    ಅನ್ಯಥಾ // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	ಪ್ರತಿ (frp = frp->fr_child; frp->fr_next != ರಿಕ್ತ; frp = frp->fr_next)
	    ;
	frame_add_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_new_width(
    frame_T	*topfrp,
    ಪೂರ್ಣಾಙ್ಕ		width,
    ಪೂರ್ಣಾಙ್ಕ		leftfirst,	// resize leftmost contained frame first
    ಪೂರ್ಣಾಙ್ಕ		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    frame_T	*frp;
    ಪೂರ್ಣಾಙ್ಕ		extra_cols;
    ಪೂರ್ಣಾಙ್ಕ		w;
    win_T	*wp;

    ಯದಿ (topfrp->fr_layout == FR_LEAF)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	ಪ್ರತಿ (frp = topfrp; frp->fr_parent != ರಿಕ್ತ; frp = frp->fr_parent)
	    ಯದಿ (frp->fr_parent->fr_layout == FR_ROW && frp->fr_next != ರಿಕ್ತ)
		ಭಙ್ಗ;
	ಯದಿ (frp->fr_parent == ರಿಕ್ತ)
	    wp->w_vsep_width = 0;
	win_new_width(wp, width - wp->w_vsep_width);
    }
    ಅನ್ಯಥಾ ಯದಿ (topfrp->fr_layout == FR_COL)
    {
	ಕುರು
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_width(frp, width, leftfirst, wfw);
		ಯದಿ (frp->fr_width > width)
		{
		    // Could not fit the windows, make whole column wider.
		    width = frp->fr_width;
		    ಭಙ್ಗ;
		}
	    }
	} ಯಾವತ್ (frp != ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_child;
	ಯದಿ (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    ಯಾವತ್ (frame_fixed_width(frp))
	    {
		frp = frp->fr_next;
		ಯದಿ (frp == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ;	    // no frame without 'wfw', give up
	    }
	ಯದಿ (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    ಯಾವತ್ (frp->fr_next != ರಿಕ್ತ)
		frp = frp->fr_next;
	    ಯದಿ (wfw)
		// Advance back for frames with one window with 'wfw' set.
		ಯಾವತ್ (frame_fixed_width(frp))
		    frp = frp->fr_prev;
	}

	extra_cols = width - topfrp->fr_width;
	ಯದಿ (extra_cols < 0)
	{
	    // reduce frame width, rightmost frame first
	    ಯಾವತ್ (frp != ರಿಕ್ತ)
	    {
		w = frame_minwidth(frp, ರಿಕ್ತ);
		ಯದಿ (frp->fr_width + extra_cols < w)
		{
		    extra_cols += frp->fr_width - w;
		    frame_new_width(frp, w, leftfirst, wfw);
		}
		ಅನ್ಯಥಾ
		{
		    frame_new_width(frp, frp->fr_width + extra_cols,
							      leftfirst, wfw);
		    ಭಙ್ಗ;
		}
		ಯದಿ (leftfirst)
		{
		    ಕುರು
			frp = frp->fr_next;
		    ಯಾವತ್ (wfw && frp != ರಿಕ್ತ && frame_fixed_width(frp));
		}
		ಅನ್ಯಥಾ
		{
		    ಕುರು
			frp = frp->fr_prev;
		    ಯಾವತ್ (wfw && frp != ರಿಕ್ತ && frame_fixed_width(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		ಯದಿ (frp == ರಿಕ್ತ)
		    width -= extra_cols;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (extra_cols > 0)
	{
	    // increase width of rightmost frame
	    frame_new_width(frp, frp->fr_width + extra_cols, leftfirst, wfw);
	}
    }
    topfrp->fr_width = width;
}

/*
 * Add or remove the vertical separator of windows to the right side of "frp".
 * Note: Does not check if there is room!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_set_vsep(frame_T *frp, ಪೂರ್ಣಾಙ್ಕ add)
{
    win_T	*wp;

    ಯದಿ (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	ಯದಿ (add && wp->w_vsep_width == 0)
	{
	    ಯದಿ (wp->w_width > 0)	// don't make it negative
		win_new_width(wp, wp->w_width - 1);
	    wp->w_vsep_width = 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (!add && wp->w_vsep_width == 1)
	{
	    win_new_width(wp, wp->w_width + 1);
	    wp->w_vsep_width = 0;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (frp->fr_layout == FR_COL)
    {
	// Handle all the frames in the column.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_set_vsep(frp, add);
    }
    ಅನ್ಯಥಾ // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_child;
	ಯಾವತ್ (frp->fr_next != ರಿಕ್ತ)
	    frp = frp->fr_next;
	frame_set_vsep(frp, add);
    }
}

/*
 * Set frame width from the window it contains.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_fix_width(win_T *wp)
{
    wp->w_frame->fr_width = wp->w_width + wp->w_vsep_width;
}

/*
 * Set frame height from the window it contains.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_fix_height(win_T *wp)
{
    wp->w_frame->fr_height = VISIBLE_HEIGHT(wp) + wp->w_status_height;
}

/*
 * Compute the minimal height for frame "topfrp".
 * Uses the 'winminheight' option.
 * When "next_curwin" isn't NULL, use p_wh for this window.
 * When "next_curwin" is NOWIN, don't use at least one line for the current
 * window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
frame_minheight(frame_T *topfrp, win_T *next_curwin)
{
    frame_T	*frp;
    ಪೂರ್ಣಾಙ್ಕ		m;
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯದಿ (topfrp->fr_win != ರಿಕ್ತ)
    {
	ಯದಿ (topfrp->fr_win == next_curwin)
	    m = p_wh + topfrp->fr_win->w_status_height;
	ಅನ್ಯಥಾ
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_status_height;
	    ಯದಿ (topfrp->fr_win == curwin && next_curwin == ರಿಕ್ತ)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		ಯದಿ (p_wmh == 0)
		    ++m;
		m += WINBAR_HEIGHT(curwin);
	    }
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (topfrp->fr_layout == FR_ROW)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minheight(frp, next_curwin);
	    ಯದಿ (n > m)
		m = n;
	}
    }
    ಅನ್ಯಥಾ
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minheight(frp, next_curwin);
    }

    ಪ್ರತ್ಯಾಗಮ m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
frame_wincount_in_height(frame_T *topfrp)
{
    frame_T	*frp;
    ಪೂರ್ಣಾಙ್ಕ		c = 0;
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯದಿ (topfrp->fr_win != ರಿಕ್ತ)
	c = 1;
    ಅನ್ಯಥಾ ಯದಿ (topfrp->fr_layout == FR_ROW)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_wincount_in_height(frp);
	    ಯದಿ (n > c)
		c = n;
	}
    }
    ಅನ್ಯಥಾ
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    c += frame_wincount_in_height(frp);
    }

    ಪ್ರತ್ಯಾಗಮ c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
frame_minwidth(
    frame_T	*topfrp,
    win_T	*next_curwin)	// use p_wh and p_wiw for next_curwin
{
    frame_T	*frp;
    ಪೂರ್ಣಾಙ್ಕ		m, n;

    ಯದಿ (topfrp->fr_win != ರಿಕ್ತ)
    {
	ಯದಿ (topfrp->fr_win == next_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_width;
	ಅನ್ಯಥಾ
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_width;
	    // Current window is minimal one column wide
	    ಯದಿ (p_wmw == 0 && topfrp->fr_win == curwin && next_curwin == ರಿಕ್ತ)
		++m;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (topfrp->fr_layout == FR_COL)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minwidth(frp, next_curwin);
	    ಯದಿ (n > m)
		m = n;
	}
    }
    ಅನ್ಯಥಾ
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minwidth(frp, next_curwin);
    }

    ಪ್ರತ್ಯಾಗಮ m;
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
    ಪೂರ್ಣಾಙ್ಕ		message,
    ಪೂರ್ಣಾಙ್ಕ		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    ಪೂರ್ಣಾಙ್ಕ		r;

    ಯದಿ (one_window())
    {
	ಯದಿ (message && !autocmd_busy)
	    ಸಂದೇಶ(_(m_onlyone));
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Be very careful here: autocommands may change the window layout.
    ಪ್ರತಿ (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_next;

	// autocommands messed this one up
	ಯದಿ (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
	}

	ಯದಿ (wp == curwin)		// don't close current window
	    ಅನುವರ್ತ;

	// autoccommands messed this one up
	ಯದಿ (!buf_valid(wp->w_buffer) && win_valid(wp))
	{
	    wp->w_buffer = ರಿಕ್ತ;
	    win_close(wp, 0);
	    ಅನುವರ್ತ;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_buffer, forceit);
	ಯದಿ (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (!r)
	{
#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ಯದಿ (message && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_changed(wp->w_buffer, FALSE);
		ಯದಿ (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    ಅನುವರ್ತ;
		}
	    }
	    ಯದಿ (bufIsChanged(wp->w_buffer))
#endif
		ಅನುವರ್ತ;
	}
	win_close(wp, !buf_hide(wp->w_buffer) && !bufIsChanged(wp->w_buffer));
    }

    ಯದಿ (message && !ONE_WINDOW)
	emsg(_(e_other_window_contains_changes));
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ command_frame_height = TRUE;

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
    ಪೂರ್ಣಾಙ್ಕ
win_alloc_first(ಶೂನ್ಯ)
{
    ಯದಿ (win_alloc_firstwin(ರಿಕ್ತ) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    first_tabpage = alloc_tabpage();
    ಯದಿ (first_tabpage == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    curtab = first_tabpage;
    unuse_tabpage(first_tabpage);

    ಪ್ರತ್ಯಾಗಮ OK;
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

    wp = win_alloc(ರಿಕ್ತ, TRUE);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_frame(wp);
    ಪ್ರತ್ಯಾಗಮ wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    ಶೂನ್ಯ
win_init_popup_win(win_T *wp, buf_T *buf)
{
    wp->w_buffer = buf;
    ++buf->b_nwindows;
    win_init_empty(wp); // set cursor and topline to safe values

    // Make sure w_localdir is NULL to avoid a chdir() in win_enter_ext().
    VIM_CLEAR(wp->w_localdir);
}

/*
 * Allocate the first window or the first window in a new tab page.
 * When "oldwin" is NULL create an empty buffer for it.
 * When "oldwin" is not NULL copy info from it to the new window.
 * Return FAIL when something goes wrong (out of memory).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(ರಿಕ್ತ, FALSE);
    ಯದಿ (curwin == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (oldwin == ರಿಕ್ತ)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(ರಿಕ್ತ, ರಿಕ್ತ, 1L, BLN_LISTED);
	ಯದಿ (curwin == ರಿಕ್ತ || curbuf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	curwin->w_buffer = curbuf;
#ifdef FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#endif
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    ಅನ್ಯಥಾ
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_frame(curwin);
    ಯದಿ (curwin->w_frame == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    topframe = curwin->w_frame;
    topframe->fr_width = COLUMNS_WITHOUT_TPL();
    topframe->fr_height = Rows - p_ch;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Create a frame for window "wp".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
new_frame(win_T *wp)
{
    frame_T *frp = ALLOC_CLEAR_ONE(frame_T);

    wp->w_frame = frp;
    ಯದಿ (frp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    frp->fr_layout = FR_LEAF;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    ಶೂನ್ಯ
win_init_size(ಶೂನ್ಯ)
{
    firstwin->w_height = ROWS_AVAIL;
    firstwin->w_prev_height = ROWS_AVAIL;
    topframe->fr_height = ROWS_AVAIL;
    firstwin->w_width = topframe->fr_width;
}

/*
 * Allocate a new tabpage_T and init the values.
 * Returns NULL when out of memory.
 */
    ಸ್ಥಿರ tabpage_T *
alloc_tabpage(ಶೂನ್ಯ)
{
    tabpage_T	*tp;
#ifdef FEAT_GUI
    ಪೂರ್ಣಾಙ್ಕ		i;
#endif


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    ಯದಿ (tp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

#ifdef FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    ಯದಿ (tp->tp_vars == ರಿಕ್ತ)
    {
	vim_free(tp);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#endif

#ifdef FEAT_GUI
    ಪ್ರತಿ (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#endif
#ifdef FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#endif
    tp->tp_ch_used = p_ch;

    ಪ್ರತ್ಯಾಗಮ tp;
}

    ಶೂನ್ಯ
free_tabpage(tabpage_T *tp)
{
    ಪೂರ್ಣಾಙ್ಕ idx;

#ifdef FEAT_DIFF
    diff_clear(tp);
#endif
#ifdef FEAT_PROP_POPUP
    ಯಾವತ್ (tp->tp_first_popupwin != ರಿಕ್ತ)
	popup_close_tabpage(tp, tp->tp_first_popupwin->w_id, TRUE);
#endif
    ಪ್ರತಿ (idx = 0; idx < SNAP_ಎಣಿಕೆ; ++idx)
	clear_snapshot(tp, idx);
#ifdef FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#endif

    ಯದಿ (tp == lastused_tabpage)
	lastused_tabpage = ರಿಕ್ತ;
#ifdef FEAT_TABPANEL
    tabpanel_forget_tabpage(tp);
#endif

    vim_free(tp->tp_localdir);
    vim_free(tp->tp_prevdir);

#ifdef FEAT_PYTHON
    python_tabpage_free(tp);
#endif

#ifdef FEAT_PYTHON3
    python3_tabpage_free(tp);
#endif

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
    ಪೂರ್ಣಾಙ್ಕ
win_new_tabpage(ಪೂರ್ಣಾಙ್ಕ after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    ಪೂರ್ಣಾಙ್ಕ		n;
#ಯದಿ defined(FEAT_TABPANEL)
    ಪೂರ್ಣಾಙ್ಕ prev_columns = COLUMNS_WITHOUT_TPL();
#endif

    ಯದಿ (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (window_layout_locked(CMD_tabnew))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    newtp = alloc_tabpage();
    ಯದಿ (newtp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Remember the current windows in this Tab page.
    ಯದಿ (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == ರಿಕ್ತ)
				    ? ರಿಕ್ತ : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    ಯದಿ (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	ಯದಿ (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_next = first_tabpage;
	    first_tabpage = newtp;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		ಪ್ರತಿ (tp = first_tabpage; tp->tp_next != ರಿಕ್ತ
					       && n < after; tp = tp->tp_next)
		    ++n;
	    }
	    newtp->tp_next = tp->tp_next;
	    tp->tp_next = newtp;
	}
	newtp->tp_firstwin = newtp->tp_lastwin = newtp->tp_curwin = curwin;

	win_init_size();
	firstwin->w_winrow = tabline_height();
	firstwin->w_prev_winrow = firstwin->w_winrow;
	win_comp_scroll(curwin);

	newtp->tp_topframe = topframe;
	last_status(FALSE);

	lastused_tabpage = prev_tp;

#ಯದಿ defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#endif
#ifdef FEAT_JOB_CHANNEL
	entering_window(curwin);
#endif
#ಯದಿ defined(FEAT_TABPANEL)
	ಯದಿ (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    ಪೂರ್ಣಾಙ್ಕ		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		ಯದಿ (tp2 != curtab)
		{
		    frame_new_width(tp2->tp_topframe, w, FALSE, TRUE);
		    ಯದಿ (!frame_check_width(tp2->tp_topframe, w))
			frame_new_width(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#endif
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
may_open_tabpage(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		n = (cmdmod.cmod_tab == 0)
				       ? postponed_split_tab : cmdmod.cmod_tab;

    ಯದಿ (n == 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_split_tab = 0;
    ಪ್ರತ್ಯಾಗಮ win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    ಪೂರ್ಣಾಙ್ಕ
make_tabpages(ಪೂರ್ಣಾಙ್ಕ maxcount)
{
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = maxcount;
    ಪೂರ್ಣಾಙ್ಕ		todo;

    // Limit to 'tabpagemax' tabs.
    ಯದಿ (ಎಣಿಕೆ > p_tpm)
	ಎಣಿಕೆ = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    ಪ್ರತಿ (todo = ಎಣಿಕೆ - 1; todo > 0; --todo)
	ಯದಿ (win_new_tabpage(0) == FAIL)
	    ಭಙ್ಗ;

    unblock_autocmds();

    // return actual number of tab pages
    ಪ್ರತ್ಯಾಗಮ (ಎಣಿಕೆ - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    ಪೂರ್ಣಾಙ್ಕ
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	ಯದಿ (tp == tpc)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    ಪೂರ್ಣಾಙ್ಕ
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	ಯದಿ (tp == tpc)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		ಯದಿ (win_valid_any_tab(wp))
		    ಪ್ರತ್ಯಾಗಮ TRUE;
	    }
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }
    // shouldn't happen
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    ಶೂನ್ಯ
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    ಯದಿ (tab == first_tabpage)
    {
	first_tabpage = tab->tp_next;
	ptp = first_tabpage;
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (ptp = first_tabpage; ptp != ರಿಕ್ತ && ptp->tp_next != tab;
							    ptp = ptp->tp_next)
	    ;
	ಪ್ರತಿಜ್ಞಾ(ptp != ರಿಕ್ತ);
	ptp->tp_next = tab->tp_next;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
find_tabpage(ಪೂರ್ಣಾಙ್ಕ n)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಙ್ಕ		i = 1;

    ಯದಿ (n == 0)
	ಪ್ರತ್ಯಾಗಮ curtab;

    ಪ್ರತಿ (tp = first_tabpage; tp != ರಿಕ್ತ && i != n; tp = tp->tp_next)
	++i;
    ಪ್ರತ್ಯಾಗಮ tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    ಪೂರ್ಣಾಙ್ಕ
tabpage_index(tabpage_T *ftp)
{
    ಪೂರ್ಣಾಙ್ಕ		i = 1;
    tabpage_T	*tp;

    ಪ್ರತಿ (tp = first_tabpage; tp != ರಿಕ್ತ && tp != ftp; tp = tp->tp_next)
	++i;
    ಪ್ರತ್ಯಾಗಮ i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    ಪೂರ್ಣಾಙ್ಕ		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ifdef FEAT_JOB_CHANNEL
    leaving_window(curwin);
#endif
    reset_VIsual_and_resel();	// stop Visual mode
    ಯದಿ (trigger_leave_autocmds)
    {
	ಯದಿ (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	    ಯದಿ (curtab != tp)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	ಯದಿ (curtab != tp)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	apply_autocmds(EVENT_TABLEAVE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	ಯದಿ (curtab != tp)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    reset_dragwin();
#ಯದಿ defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    ಯದಿ (gui.in_use)
	gui_remove_scrollbars();
#endif
#ifdef FEAT_IMAGE
    // Clear all image placements, they will be redrawn again if needed.
    clear_all_image_placements();
#endif
    tp->tp_curwin = curwin;
    tp->tp_prevwin = prevwin;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_old_Rows = ROWS_AVAIL;
    ಯದಿ (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_width;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = ರಿಕ್ತ;
    lastwin = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ		trigger_enter_autocmds,
    ಪೂರ್ಣಾಙ್ಕ		trigger_leave_autocmds)
{
    ಪೂರ್ಣಾಙ್ಕ		old_off = tp->tp_firstwin->w_winrow;
    win_T	*next_prevwin = tp->tp_prevwin;
    tabpage_T	*last_tab = curtab;

    use_tabpage(tp);

    ಯದಿ (p_ch != curtab->tp_ch_used)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	ಪೂರ್ಣಾಙ್ಕ new_ch = curtab->tp_ch_used;
	curtab->tp_ch_used = p_ch;
	command_frame_height = FALSE;
	set_option_ಮೌಲ್ಯ((char_u *)"cmdheight", new_ch, ರಿಕ್ತ, 0);
	command_frame_height = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (ಶೂನ್ಯ)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
		  | (trigger_enter_autocmds ? WEE_TRIGGER_ENTER_AUTOCMDS : 0)
		  | (trigger_leave_autocmds ? WEE_TRIGGER_LEAVE_AUTOCMDS : 0));
    prevwin = next_prevwin;

    last_status(FALSE);		// status line may appear or disappear
    win_comp_pos();		// recompute w_winrow for all windows
#ifdef FEAT_DIFF
    diff_need_scrollbind = TRUE;
#endif

    // If there was a click in a window, it won't be usable for a following
    // drag.
    reset_dragwin();

    // The tabpage line may have appeared or disappeared, may need to resize
    // the frames for that.  When the Vim window was resized need to update
    // frame sizes too.
    ಯದಿ (curtab->tp_old_Rows != ROWS_AVAIL || (old_off != firstwin->w_winrow
#ifdef FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#endif
		))
	shell_new_rows();
    ಯದಿ (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	ಯದಿ (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_width;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	ಅನ್ಯಥಾ
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = last_tab;

#ಯದಿ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    ಯದಿ (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	ಯದಿ (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    ಶೂನ್ಯ
goto_tabpage(ಪೂರ್ಣಾಙ್ಕ n)
{
    tabpage_T	*tp = ರಿಕ್ತ;  // shut up compiler
    tabpage_T	*ttp;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (text_locked())
    {
	// Not allowed when editing the command line.
	text_locked_ಸಂದೇಶ();
	ಪ್ರತ್ಯಾಗಮ;
    }

    // If there is only one it can't work.
    ಯದಿ (first_tabpage->tp_next == ರಿಕ್ತ)
    {
	ಯದಿ (n > 1)
	    beep_flush();
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	ಯದಿ (curtab->tp_next == ರಿಕ್ತ)
	    tp = first_tabpage;
	ಅನ್ಯಥಾ
	    tp = curtab->tp_next;
    }
    ಅನ್ಯಥಾ ಯದಿ (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	ಪ್ರತಿ (i = n; i < 0; ++i)
	{
	    ಪ್ರತಿ (tp = first_tabpage; tp->tp_next != ttp && tp->tp_next != ರಿಕ್ತ;
		    tp = tp->tp_next)
		;
	    ttp = tp;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (n == 9999)
    {
	// Go to last tab page.
	ಪ್ರತಿ (tp = first_tabpage; tp->tp_next != ರಿಕ್ತ; tp = tp->tp_next)
	    ;
    }
    ಅನ್ಯಥಾ
    {
	// Go to tab page "n".
	tp = find_tabpage(n);
	ಯದಿ (tp == ರಿಕ್ತ)
	{
	    beep_flush();
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ifdef FEAT_GUI_TABLINE
    ಯದಿ (gui_use_tabline())
	gui_mch_set_curtab(tabpage_index(curtab));
#endif
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
    ಪೂರ್ಣಾಙ್ಕ		trigger_enter_autocmds,
    ಪೂರ್ಣಾಙ್ಕ		trigger_leave_autocmds)
{
    ಯದಿ (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    set_keep_ಸಂದೇಶ(ರಿಕ್ತ, 0);

    skip_win_fix_scroll = TRUE;
    ಯದಿ (tp != curtab && leave_tabpage(tp->tp_curwin->w_buffer,
					trigger_leave_autocmds) == OK)
    {
	ಯದಿ (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	ಅನ್ಯಥಾ
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    ಪೂರ್ಣಾಙ್ಕ
goto_tabpage_lastused(ಶೂನ್ಯ)
{
    ಯದಿ (!valid_tabpage(lastused_tabpage))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    ಶೂನ್ಯ
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    ಯದಿ (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ifdef FEAT_GUI_TABLINE
	ಯದಿ (gui_use_tabline())
	    gui_mch_set_curtab(tabpage_index(curtab));
#endif
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    ಶೂನ್ಯ
tabpage_move(ಪೂರ್ಣಾಙ್ಕ nr)
{
    ಪೂರ್ಣಾಙ್ಕ		n = 1;
    tabpage_T	*tp, *tp_dst;

    ಯದಿ (first_tabpage->tp_next == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (tabpage_move_disallowed)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (tp = first_tabpage; tp->tp_next != ರಿಕ್ತ && n < nr; tp = tp->tp_next)
	++n;

    ಯದಿ (tp == curtab || (nr > 0 && tp->tp_next != ರಿಕ್ತ
						    && tp->tp_next == curtab))
	ಪ್ರತ್ಯಾಗಮ;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    ಯದಿ (curtab == first_tabpage)
	first_tabpage = curtab->tp_next;
    ಅನ್ಯಥಾ
    {
	FOR_ALL_TABPAGES(tp)
	    ಯದಿ (tp->tp_next == curtab)
		ಭಙ್ಗ;
	ಯದಿ (tp == ರಿಕ್ತ)	// "cannot happen"
	    ಪ್ರತ್ಯಾಗಮ;
	tp->tp_next = curtab->tp_next;
    }

    // Re-insert it at the specified position.
    ಯದಿ (nr <= 0)
    {
	curtab->tp_next = first_tabpage;
	first_tabpage = curtab;
    }
    ಅನ್ಯಥಾ
    {
	curtab->tp_next = tp_dst->tp_next;
	tp_dst->tp_next = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#ಯದಿ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
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
#ifdef FEAT_CONCEAL
    win_T	*owp = curwin;
#endif

#ifdef FEAT_PROP_POPUP
    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_to_enter_popup_window));
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif
    ಯದಿ (text_or_buf_locked())
    {
	beep_flush();
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (wp->w_buffer != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    ಅನ್ಯಥಾ ಯದಿ (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    ಯದಿ (!win_valid(wp))
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    win_enter(wp, TRUE);

#ifdef FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    ಯದಿ (win_valid(owp) && owp->w_p_cole > 0 && !ಸಂದೇಶ_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    ಯದಿ (curwin->w_p_cole > 0 && !ಸಂದೇಶ_scrolled)
	need_cursor_ಸಾಲು_redraw = TRUE;
#endif
}

#ಯದಿ defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_find_nr(ಪೂರ್ಣಾಙ್ಕ winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (--winnr == 0)
	    ಭಙ್ಗ;
    ಪ್ರತ್ಯಾಗಮ wp;
}
#endif

#ಯದಿ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_find_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಯದಿ (wp == win)
		ಪ್ರತ್ಯಾಗಮ tp;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, ಪೂರ್ಣಾಙ್ಕ up, long ಎಣಿಕೆ)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    ಯದಿ (popup_is_popup(wp))
	// popups don't have neighbors.
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
    foundfr = wp->w_frame;
    ಯಾವತ್ (ಎಣಿಕೆ--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (fr == tp->tp_topframe)
		ಗಚ್ಛ end;
	    ಯದಿ (up)
		nfr = fr->fr_prev;
	    ಅನ್ಯಥಾ
		nfr = fr->fr_next;
	    ಯದಿ (fr->fr_parent->fr_layout == FR_COL && nfr != ರಿಕ್ತ)
		ಭಙ್ಗ;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		ಭಙ್ಗ;
	    }
	    fr = nfr->fr_child;
	    ಯದಿ (nfr->fr_layout == FR_ROW)
	    {
		// Find the frame at the cursor row.
		ಯಾವತ್ (fr->fr_next != ರಿಕ್ತ
			&& frame2win(fr)->w_wincol + fr->fr_width
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_next;
	    }
	    ಯದಿ (nfr->fr_layout == FR_COL && up)
		ಯಾವತ್ (fr->fr_next != ರಿಕ್ತ)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    ಪ್ರತ್ಯಾಗಮ foundfr != ರಿಕ್ತ ? foundfr->fr_win : ರಿಕ್ತ;
}

/*
 * Move to window above or below "count" times.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_goto_ver(
    ಪೂರ್ಣಾಙ್ಕ		up,		// TRUE to go to win above
    long	ಎಣಿಕೆ)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    ಯದಿ (ERROR_IF_TERM_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;
#endif
    win = win_vert_neighbor(curtab, curwin, up, ಎಣಿಕೆ);
    ಯದಿ (win != ರಿಕ್ತ)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, ಪೂರ್ಣಾಙ್ಕ ಎಡ, long ಎಣಿಕೆ)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    ಯದಿ (popup_is_popup(wp))
	// popups don't have neighbors.
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
    foundfr = wp->w_frame;
    ಯಾವತ್ (ಎಣಿಕೆ--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (fr == tp->tp_topframe)
		ಗಚ್ಛ end;
	    ಯದಿ (ಎಡ)
		nfr = fr->fr_prev;
	    ಅನ್ಯಥಾ
		nfr = fr->fr_next;
	    ಯದಿ (fr->fr_parent->fr_layout == FR_ROW && nfr != ರಿಕ್ತ)
		ಭಙ್ಗ;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		ಭಙ್ಗ;
	    }
	    fr = nfr->fr_child;
	    ಯದಿ (nfr->fr_layout == FR_COL)
	    {
		// Find the frame at the cursor row.
		ಯಾವತ್ (fr->fr_next != ರಿಕ್ತ
			&& frame2win(fr)->w_winrow + fr->fr_height
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_next;
	    }
	    ಯದಿ (nfr->fr_layout == FR_ROW && ಎಡ)
		ಯಾವತ್ (fr->fr_next != ರಿಕ್ತ)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    ಪ್ರತ್ಯಾಗಮ foundfr != ರಿಕ್ತ ? foundfr->fr_win : ರಿಕ್ತ;
}

/*
 * Move to left or right window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_goto_hor(
    ಪೂರ್ಣಾಙ್ಕ		ಎಡ,		// TRUE to go to left win
    long	ಎಣಿಕೆ)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    ಯದಿ (ERROR_IF_TERM_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;
#endif
    win = win_horz_neighbor(curtab, curwin, ಎಡ, ಎಣಿಕೆ);
    ಯದಿ (win != ರಿಕ್ತ)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    ಶೂನ್ಯ
win_enter(win_T *wp, ಪೂರ್ಣಾಙ್ಕ undo_sync)
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
    ಯದಿ (curwin->w_localdir != ರಿಕ್ತ || curtab->tp_localdir != ರಿಕ್ತ)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	ಯದಿ (globaldir == ರಿಕ್ತ)
	{
	    char_u	cwd[MAXPATHL];

	    ಯದಿ (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	ಯದಿ (curwin->w_localdir != ರಿಕ್ತ)
	    dirname = curwin->w_localdir;
	ಅನ್ಯಥಾ
	    dirname = curtab->tp_localdir;

	ಯದಿ (mch_chdir((ವರ್ಣ *)dirname) == 0)
	{
	    last_chdir_reason = ರಿಕ್ತ;
	    shorten_fnames(TRUE);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (globaldir != ರಿಕ್ತ)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((ವರ್ಣ *)globaldir);
	VIM_CLEAR(globaldir);
	last_chdir_reason = ರಿಕ್ತ;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
win_enter_ext(win_T *wp, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಪೂರ್ಣಾಙ್ಕ		other_buffer = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    ಪೂರ್ಣಾಙ್ಕ		did_decrement = FALSE;

    ಯದಿ (wp == curwin && curwin_invalid == 0)	// nothing to do
	ಪ್ರತ್ಯಾಗಮ FALSE;

#ifdef FEAT_JOB_CHANNEL
    ಯದಿ (curwin_invalid == 0)
	leaving_window(curwin);
#endif

    ಯದಿ (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	ಯದಿ (wp->w_buffer != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	    other_buffer = TRUE;
	    ಯದಿ (!win_valid(wp))
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	ಯದಿ (!win_valid(wp))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	ಯದಿ (aborting())
	    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
    }

    // sync undo before leaving the current buffer
    ಯದಿ ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_buffer)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    ಯದಿ (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    ಯದಿ (wp->w_buffer != curbuf)
	buf_copy_options(wp->w_buffer, BCO_ENTER | BCO_NOHELP);
    ಯದಿ (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_status = true;
    }
    curwin = wp;
    curbuf = wp->w_buffer;
    check_cursor();
    ಯದಿ (!virtual_active())
	curwin->w_cursor.coladd = 0;
    ಯದಿ (*p_spk == 'c')		// assume cursor position needs updating
	changed_ಸಾಲು_abv_curs();
    ಅನ್ಯಥಾ
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ifdef MESSAGE_QUEUE
    ಯದಿ (flags & WEE_ALLOW_PARSE_MESSAGES)
    {
	--dont_parse_messages;
	did_decrement = TRUE;
    }
#endif

    win_fix_current_dir();

#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Careful: autocommands may close the window and make "wp" invalid
    ಯದಿ (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    ಯದಿ (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	ಯದಿ (other_buffer)
	    apply_autocmds(EVENT_BUFENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_status = true;
#ifdef FEAT_TERMINAL
    ಯದಿ (bt_terminal(curwin->w_buffer))
	// terminal is likely in another mode
	redraw_mode = TRUE;
#endif
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#ಯದಿ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    ಯದಿ (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    ಯದಿ (curwin->w_height < p_wh && !curwin->w_p_wfh
#ifdef FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#endif
	    )
	win_setheight((ಪೂರ್ಣಾಙ್ಕ)p_wh);
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_height == 0)
	win_setheight(1);

    // set window width to desired minimal value
    ಯದಿ (curwin->w_width < p_wiw && !curwin->w_p_wfw)
	win_setwidth((ಪೂರ್ಣಾಙ್ಕ)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    ಪ್ರತ್ಯಾಗಮ did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = ರಿಕ್ತ;

    ಯದಿ (curwin->w_buffer == buf)
	wp = curwin;
    ಅನ್ಯಥಾ
	FOR_ALL_WINDOWS(wp)
	    ಯದಿ (wp->w_buffer == buf)
		ಭಙ್ಗ;
    ಯದಿ (wp != ರಿಕ್ತ)
	win_enter(wp, FALSE);
    ಪ್ರತ್ಯಾಗಮ wp;
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

    ಯದಿ (wp != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ wp;

    FOR_ALL_TABPAGES(tp)
	ಯದಿ (tp != curtab)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		ಯದಿ (wp->w_buffer == buf)
		    ಭಙ್ಗ;
	    ಯದಿ (wp != ರಿಕ್ತ)
	    {
		goto_tabpage_win(tp, wp);
		ಯದಿ (curwin != wp)
		    wp = ರಿಕ್ತ;	// something went wrong
		ಭಙ್ಗ;
	    }
	}
    ಪ್ರತ್ಯಾಗಮ wp;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ last_win_id = LOWEST_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    ಸ್ಥಿರ win_T *
win_alloc(win_T *after, ಪೂರ್ಣಾಙ್ಕ hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    ಯದಿ (new_wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (win_alloc_ಸಾಲುಗಳು(new_wp) == FAIL)
    {
	vim_free(new_wp);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    new_wp->w_id = ++last_win_id;

#ifdef FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    ಯದಿ (new_wp->w_vars == ರಿಕ್ತ)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    init_var_dict(new_wp->w_vars, &new_wp->w_winvar, VAR_SCOPE);
#endif

    // Don't execute autocommands while the window is not properly
    // initialized yet.  gui_create_scrollbar() may trigger a FocusGained
    // event.
    block_autocmds();

    /*
     * link the window in the window list
     */
    ಯದಿ (!hidden)
	win_append(after, new_wp);
    new_wp->w_wincol = TPL_LCOL();
    new_wp->w_width = COLUMNS_WITHOUT_TPL();

    // position the display and the cursor at the top of the file.
    new_wp->w_topline = 1;
#ifdef FEAT_DIFF
    new_wp->w_topfill = 0;
#endif
    new_wp->w_botline = 2;
    new_wp->w_cursor.lnum = 1;
    new_wp->w_scbind_pos = 1;

    // use global option value for global-local options
    new_wp->w_allbuf_opt.wo_so = new_wp->w_p_so = -1;
    new_wp->w_allbuf_opt.wo_sop = new_wp->w_p_sop = -1;
    new_wp->w_allbuf_opt.wo_siso = new_wp->w_p_siso = -1;

    // We won't calculate w_fraction until resizing the window
    new_wp->w_fraction = 0;
    new_wp->w_prev_fraction_row = -1;

#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ಎಡ],
		SBAR_ಎಡ, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ಬಲ],
		SBAR_ಬಲ, new_wp);
    }
#endif
#ifdef FEAT_FOLDING
    foldInitWin(new_wp);
#endif
    unblock_autocmds();
#ifdef FEAT_ಹುಡುಕು_EXTRA
    new_wp->w_next_match_id = 1000;  // up to 1000 can be picked by the user
#endif
    ಪ್ರತ್ಯಾಗಮ new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    buf_T	*buf;
    wininfo_T	*wip;

#ifdef FEAT_FOLDING
    clearFolding(wp);
#endif

    // reduce the reference count to the argument list.
    alist_unlink(wp->w_alist);

    // Don't execute autocommands while the window is halfway being deleted.
    // gui_mch_destroy_scrollbar() may trigger a FocusGained event.
    block_autocmds();

#ifdef FEAT_LUA
    lua_window_free(wp);
#endif

#ifdef FEAT_MZSCHEME
    mzscheme_window_free(wp);
#endif

#ifdef FEAT_PERL
    perl_win_free(wp);
#endif

#ifdef FEAT_PYTHON
    python_window_free(wp);
#endif

#ifdef FEAT_PYTHON3
    python3_window_free(wp);
#endif

#ifdef FEAT_TCL
    tcl_window_free(wp);
#endif

#ifdef FEAT_RUBY
    ruby_window_free(wp);
#endif

    remove_highlight_overrides(wp->w_hl);
    vim_free(wp->w_hl);

    // Free statusline click regions.
    ಯದಿ (wp->w_stl_click != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (i = 0; i < wp->w_stl_click_ಎಣಿಕೆ; i++)
	    vim_free(wp->w_stl_click[i].funcname);
	vim_free(wp->w_stl_click);
    }

    clear_winopt(&wp->w_onebuf_opt);
    clear_winopt(&wp->w_allbuf_opt);

    vim_free(wp->w_lcs_chars.multispace);
    vim_free(wp->w_lcs_chars.leadmultispace);

#ifdef FEAT_EVAL
    vars_clear(&wp->w_vars->dv_hashtab);	// free all w: variables
    hash_init(&wp->w_vars->dv_hashtab);
    unref_var_dict(wp->w_vars);
#endif

    {
	tabpage_T	*ttp;

	ಯದಿ (prevwin == wp)
	    prevwin = ರಿಕ್ತ;
	FOR_ALL_TABPAGES(ttp)
	    ಯದಿ (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = ರಿಕ್ತ;
    }
    win_free_lsize(wp);

    ಪ್ರತಿ (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    ಯದಿ (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    ಯದಿ (wip2 != wip && wip2->wi_win == ರಿಕ್ತ)
		    {
			ಯದಿ (wip2->wi_next != ರಿಕ್ತ)
			    wip2->wi_next->wi_prev = wip2->wi_prev;
			ಯದಿ (wip2->wi_prev == ರಿಕ್ತ)
			    buf->b_wininfo = wip2->wi_next;
			ಅನ್ಯಥಾ
			    wip2->wi_prev->wi_next = wip2->wi_next;
			free_wininfo(wip2);
			ಭಙ್ಗ;
		    }

		wip->wi_win = ರಿಕ್ತ;
	    }

#ifdef FEAT_ಹುಡುಕು_EXTRA
    clear_matches(wp);
#endif

    free_jumplist(wp);

#ifdef FEAT_QUICKFIX
    qf_free_all(wp);
#endif

#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ಎಡ]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ಬಲ]);
    }
#endif // FEAT_GUI

#ifdef FEAT_MENU
    remove_winbar(wp);
#endif
#ifdef FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    ಪ್ರತಿ (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    vim_free(wp->w_scrollbar_highlight);
    vim_free(wp->w_thumb_highlight);
    vim_free(wp->w_popup_title);
    list_unref(wp->w_popup_mask);
    vim_free(wp->w_popup_mask_cells);
#endif

#ifdef FEAT_SYN_HL
    vim_free(wp->w_p_cc_cols);
#endif

    ಯದಿ (win_valid_any_tab(wp))
	win_remove(wp, tp);
    ಯದಿ (autocmd_busy)
    {
	wp->w_next = au_pending_free_win;
	au_pending_free_win = wp;
    }
    ಅನ್ಯಥಾ
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    ಪೂರ್ಣಾಙ್ಕ
win_unlisted(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#ಯದಿ defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    ಶೂನ್ಯ
win_free_popup(win_T *win)
{
    ಯದಿ (win->w_buffer != ರಿಕ್ತ)
    {
	ಯದಿ (bt_popup(win->w_buffer))
	    win_close_buffer(win, DOBUF_WIPE_REUSE, FALSE);
	ಅನ್ಯಥಾ
	    close_buffer(win, win->w_buffer, 0, FALSE, FALSE, TRUE);

	ಯದಿ (win->w_buffer != ರಿಕ್ತ)
	    --win->w_buffer->b_nwindows;
    }
# ಯದಿ defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    ಯದಿ (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_frame);
    win_free(win, ರಿಕ್ತ);
}
#endif

/*
 * Append window "wp" in the window list after window "after".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    ಯದಿ (after == ರಿಕ್ತ)	    // after NULL is in front of the first
	before = firstwin;
    ಅನ್ಯಥಾ
	before = after->w_next;

    wp->w_next = before;
    wp->w_prev = after;
    ಯದಿ (after == ರಿಕ್ತ)
	firstwin = wp;
    ಅನ್ಯಥಾ
	after->w_next = wp;
    ಯದಿ (before == ರಿಕ್ತ)
	lastwin = wp;
    ಅನ್ಯಥಾ
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
    ಯದಿ (wp->w_prev != ರಿಕ್ತ)
	wp->w_prev->w_next = wp->w_next;
    ಅನ್ಯಥಾ ಯದಿ (tp == ರಿಕ್ತ)
	firstwin = curtab->tp_firstwin = wp->w_next;
    ಅನ್ಯಥಾ
	tp->tp_firstwin = wp->w_next;

    ಯದಿ (wp->w_next != ರಿಕ್ತ)
	wp->w_next->w_prev = wp->w_prev;
    ಅನ್ಯಥಾ ಯದಿ (tp == ರಿಕ್ತ)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    ಅನ್ಯಥಾ
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_append(frame_T *after, frame_T *frp)
{
    frp->fr_next = after->fr_next;
    after->fr_next = frp;
    ಯದಿ (frp->fr_next != ರಿಕ್ತ)
	frp->fr_next->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_ಸೇರಿಸು(frame_T *before, frame_T *frp)
{
    frp->fr_next = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    ಯದಿ (frp->fr_prev != ರಿಕ್ತ)
	frp->fr_prev->fr_next = frp;
    ಅನ್ಯಥಾ
	frp->fr_parent->fr_child = frp;
}

/*
 * Remove a frame from a frame list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_remove(frame_T *frp)
{
    ಯದಿ (frp->fr_prev != ರಿಕ್ತ)
	frp->fr_prev->fr_next = frp->fr_next;
    ಅನ್ಯಥಾ
	frp->fr_parent->fr_child = frp->fr_next;
    ಯದಿ (frp->fr_next != ರಿಕ್ತ)
	frp->fr_next->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    ಪೂರ್ಣಾಙ್ಕ
win_alloc_ಸಾಲುಗಳು(win_T *wp)
{
    wp->w_ಸಾಲುಗಳು_valid = 0;
    wp->w_ಸಾಲುಗಳು = ALLOC_CLEAR_MULT(wline_T, Rows);
    ಯದಿ (wp->w_ಸಾಲುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * free lsize arrays for a window
 */
    ಶೂನ್ಯ
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    ಯದಿ (wp != ರಿಕ್ತ)
	VIM_CLEAR(wp->w_ಸಾಲುಗಳು);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    ಶೂನ್ಯ
shell_new_rows(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		h = (ಪೂರ್ಣಾಙ್ಕ)ROWS_AVAIL;

    ಯದಿ (firstwin == ರಿಕ್ತ)	// not initialized yet
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (h < frame_minheight(topframe, ರಿಕ್ತ))
	h = frame_minheight(topframe, ರಿಕ್ತ);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    frame_new_height(topframe, h, FALSE, TRUE, FALSE);
    ಯದಿ (!frame_check_height(topframe, h))
	frame_new_height(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_used = p_ch;

    ಯದಿ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#ಯದಿ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#ಯದಿ 0
    // Disabled: don't want making the screen smaller make a window larger.
    ಯದಿ (p_ea)
	win_equal(curwin, FALSE, 'v');
#endif
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    ಶೂನ್ಯ
shell_new_columns(ಶೂನ್ಯ)
{
    ಯದಿ (firstwin == ರಿಕ್ತ)	// not initialized yet
	ಪ್ರತ್ಯಾಗಮ;

#ಯದಿ defined(FEAT_TABPANEL)
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prev_tp_width;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prev_wincol;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prev_fr_width;
#endif
    ಪೂರ್ಣಾಙ್ಕ w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    frame_new_width(topframe, w, FALSE, TRUE);
    ಯದಿ (!frame_check_width(topframe, w))
	frame_new_width(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#ಯದಿ defined(FEAT_TABPANEL)
    ಪೂರ್ಣಾಙ್ಕ tp_width = tabpanel_width();
    ಪೂರ್ಣಾಙ್ಕ tp_width_changed = tp_width != prev_tp_width;
    // tabpanel width changed
    ಯದಿ (tp_width_changed && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    ಯದಿ (tp_width_changed
	    || (tp_width > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_width != prev_fr_width)))
    {
	screen_fill(cmdline_row, (ಪೂರ್ಣಾಙ್ಕ)Rows, 0, (ಪೂರ್ಣಾಙ್ಕ)Columns, ' ', ' ', 0);
    }
    prev_tp_width = tabpanel_width();
    prev_wincol = firstwin->w_wincol;
    prev_fr_width = topframe->fr_width;
#endif
    // Adjust offset for command line start column
    cmdline_col_off = firstwin->w_wincol;
    cmdline_width = topframe->fr_width;
    comp_col();
    ಯದಿ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#ಯದಿ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#ಯದಿ 0
    // Disabled: don't want making the screen smaller make a window larger.
    ಯದಿ (p_ea)
	win_equal(curwin, FALSE, 'h');
#endif
}

/*
 * Save the size of all windows in "gap".
 */
    ಶೂನ್ಯ
win_size_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ), 1);
    ಯದಿ (ga_grow(gap, win_ಎಣಿಕೆ() * 2 + 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // first entry is the total lines available for windows
    ((ಪೂರ್ಣಾಙ್ಕ *)gap->ga_data)[gap->ga_len++] = ROWS_AVAIL - last_stl_height(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((ಪೂರ್ಣಾಙ್ಕ *)gap->ga_data)[gap->ga_len++] =
	    wp->w_width + wp->w_vsep_width;
	((ಪೂರ್ಣಾಙ್ಕ *)gap->ga_data)[gap->ga_len++] = wp->w_height;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    ಶೂನ್ಯ
win_size_restore(garray_T *gap)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		i, j;

    ಯದಿ (win_ಎಣಿಕೆ() * 2 + 1 == gap->ga_len
	    && ((ಪೂರ್ಣಾಙ್ಕ *)gap->ga_data)[0] == ROWS_AVAIL - last_stl_height(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	ಪ್ರತಿ (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		frame_setwidth(wp->w_frame, ((ಪೂರ್ಣಾಙ್ಕ *)gap->ga_data)[i++]);
		win_setheight_win(((ಪೂರ್ಣಾಙ್ಕ *)gap->ga_data)[i++], wp);
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
    ಪೂರ್ಣಾಙ್ಕ		row = tabline_height();
    ಪೂರ್ಣಾಙ್ಕ		col = TPL_LCOL();

    frame_comp_pos(topframe, &row, &col);
#ಯದಿ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_comp_pos(frame_T *topfrp, ಪೂರ್ಣಾಙ್ಕ *row, ಪೂರ್ಣಾಙ್ಕ *col)
{
    win_T	*wp;
    frame_T	*frp;
    ಪೂರ್ಣಾಙ್ಕ		startcol;
    ಪೂರ್ಣಾಙ್ಕ		startrow;
    ಪೂರ್ಣಾಙ್ಕ		h;

    wp = topfrp->fr_win;
    ಯದಿ (wp != ರಿಕ್ತ)
    {
	ಯದಿ (wp->w_winrow != *row || wp->w_wincol != *col)
	{
	    // position changed, redraw
	    wp->w_winrow = *row;
	    wp->w_wincol = *col;
	    redraw_win_later(wp, UPD_NOT_VALID);
	    wp->w_redr_status = true;
	}
	// WinBar will not show if the window height is zero
	h = VISIBLE_HEIGHT(wp) + wp->w_status_height;
	*row += h > topfrp->fr_height ? topfrp->fr_height : h;
	*col += wp->w_width + wp->w_vsep_width;
    }
    ಅನ್ಯಥಾ
    {
	startrow = *row;
	startcol = *col;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    ಯದಿ (topfrp->fr_layout == FR_ROW)
		*row = startrow;	// all frames are at the same row
	    ಅನ್ಯಥಾ
		*col = startcol;	// all frames are at the same col
	    frame_comp_pos(frp, row, col);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    ಶೂನ್ಯ
win_ensure_size(ಶೂನ್ಯ)
{
    ಯದಿ (curwin->w_height == 0)
	win_setheight(1);
    ಯದಿ (curwin->w_width == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    ಶೂನ್ಯ
win_setheight(ಪೂರ್ಣಾಙ್ಕ height)
{
    win_setheight_win(height, curwin);
#ಯದಿ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    ಶೂನ್ಯ
win_setheight_win(ಪೂರ್ಣಾಙ್ಕ height, win_T *win)
{
    ಯದಿ (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	ಯದಿ (height < p_wmh)
	    height = p_wmh;
	ಯದಿ (height == 0)
	    height = 1;
	height += WINBAR_HEIGHT(curwin);
    }

    frame_setheight(win->w_frame, height + win->w_status_height);

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
frame_setheight(frame_T *curfrp, ಪೂರ್ಣಾಙ್ಕ height)
{
    ಪೂರ್ಣಾಙ್ಕ		room;		// total number of lines available
    ಪೂರ್ಣಾಙ್ಕ		take;		// number of lines taken from other windows
    ಪೂರ್ಣಾಙ್ಕ		room_cmdline;	// lines available from cmdline
    ಪೂರ್ಣಾಙ್ಕ		run;
    frame_T	*frp;
    ಪೂರ್ಣಾಙ್ಕ		h;
    ಪೂರ್ಣಾಙ್ಕ		room_reserved;

    // If the height already is the desired value, nothing to do.
    ಯದಿ (curfrp->fr_height == height)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (curfrp->fr_parent == ರಿಕ್ತ)
    {
	// topframe: can only change the command line height
	ಯದಿ (height > 0)
	    frame_new_height(curfrp, height, FALSE, FALSE, TRUE);
    }
    ಅನ್ಯಥಾ ಯದಿ (curfrp->fr_parent->fr_layout == FR_ROW)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = frame_minheight(curfrp->fr_parent, ರಿಕ್ತ);
	ಯದಿ (height < h)
	    height = h;
	frame_setheight(curfrp->fr_parent, height);
    }
    ಅನ್ಯಥಾ
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
	ಪ್ರತಿ (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		ಯದಿ (frp != curfrp
			&& frp->fr_win != ರಿಕ್ತ
			&& frp->fr_win->w_p_wfh)
		    room_reserved += frp->fr_height;
		room += frp->fr_height;
		ಯದಿ (frp != curfrp)
		    room -= frame_minheight(frp, ರಿಕ್ತ);
	    }
	    ಯದಿ (curfrp->fr_width != topframe->fr_width)
		room_cmdline = 0;
	    ಅನ್ಯಥಾ
	    {
		room_cmdline = Rows - p_ch - (lastwin->w_winrow
						+ VISIBLE_HEIGHT(lastwin)
						+ lastwin->w_status_height);
		ಯದಿ (room_cmdline < 0)
		    room_cmdline = 0;
	    }

	    ಯದಿ (height <= room + room_cmdline)
		ಭಙ್ಗ;
	    ಯದಿ (run == 2 || curfrp->fr_width == topframe->fr_width)
	    {
		height = room + room_cmdline;
		ಭಙ್ಗ;
	    }
	    frame_setheight(curfrp->fr_parent, height
		+ frame_minheight(curfrp->fr_parent, NOWIN) - (ಪೂರ್ಣಾಙ್ಕ)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = height - curfrp->fr_height;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	ಯದಿ (height > room + room_cmdline - room_reserved)
	    room_reserved = room + room_cmdline - height;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	ಯದಿ (take < 0 && room - curfrp->fr_height <= room_reserved)
	    room_reserved = 0;

	ಯದಿ (take > 0 && room_cmdline > 0)
	{
	    // use lines from cmdline first
	    ಯದಿ (take < room_cmdline)
		room_cmdline = take;
	    take -= room_cmdline;
	    topframe->fr_height += room_cmdline;
	}

	/*
	 * set the current frame to the new height
	 */
	frame_new_height(curfrp, height, FALSE, FALSE, TRUE);

	/*
	 * First take lines from the frames after the current frame.  If
	 * that is not enough, takes lines from frames above the current
	 * frame.
	 */
	ಪ್ರತಿ (run = 0; run < 2; ++run)
	{
	    ಯದಿ (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    ಅನ್ಯಥಾ
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    ಯಾವತ್ (frp != ರಿಕ್ತ && take != 0)
	    {
		h = frame_minheight(frp, ರಿಕ್ತ);
		ಯದಿ (room_reserved > 0
			&& frp->fr_win != ರಿಕ್ತ
			&& frp->fr_win->w_p_wfh)
		{
		    ಯದಿ (room_reserved >= frp->fr_height)
			room_reserved -= frp->fr_height;
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (frp->fr_height - room_reserved > take)
			    room_reserved = frp->fr_height - take;
			take -= frp->fr_height - room_reserved;
			frame_new_height(frp, room_reserved, FALSE, FALSE,
									TRUE);
			room_reserved = 0;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (frp->fr_height - take < h)
		    {
			take -= frp->fr_height - h;
			frame_new_height(frp, h, FALSE, FALSE, TRUE);
		    }
		    ಅನ್ಯಥಾ
		    {
			frame_new_height(frp, frp->fr_height - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		ಯದಿ (run == 0)
		    frp = frp->fr_next;
		ಅನ್ಯಥಾ
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
win_setwidth(ಪೂರ್ಣಾಙ್ಕ width)
{
    win_setwidth_win(width, curwin);
}

    ಶೂನ್ಯ
win_setwidth_win(ಪೂರ್ಣಾಙ್ಕ width, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    ಯದಿ (wp == curwin)
    {
	ಯದಿ (width < p_wmw)
	    width = p_wmw;
	ಯದಿ (width == 0)
	    width = 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (width < 0)
	width = 0;

    frame_setwidth(wp->w_frame, width + wp->w_vsep_width);

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
frame_setwidth(frame_T *curfrp, ಪೂರ್ಣಾಙ್ಕ width)
{
    ಪೂರ್ಣಾಙ್ಕ		room;		// total number of lines available
    ಪೂರ್ಣಾಙ್ಕ		take;		// number of lines taken from other windows
    ಪೂರ್ಣಾಙ್ಕ		run;
    frame_T	*frp;
    ಪೂರ್ಣಾಙ್ಕ		w;
    ಪೂರ್ಣಾಙ್ಕ		room_reserved;

    // If the width already is the desired value, nothing to do.
    ಯದಿ (curfrp->fr_width == width)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (curfrp->fr_parent == ರಿಕ್ತ)
	// topframe: can't change width
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (curfrp->fr_parent->fr_layout == FR_COL)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = frame_minwidth(curfrp->fr_parent, ರಿಕ್ತ);
	ಯದಿ (width < w)
	    width = w;
	frame_setwidth(curfrp->fr_parent, width);
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	ಪ್ರತಿ (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		ಯದಿ (frp != curfrp
			&& frp->fr_win != ರಿಕ್ತ
			&& frp->fr_win->w_p_wfw)
		    room_reserved += frp->fr_width;
		room += frp->fr_width;
		ಯದಿ (frp != curfrp)
		    room -= frame_minwidth(frp, ರಿಕ್ತ);
	    }

	    ಯದಿ (width <= room)
		ಭಙ್ಗ;
	    ಯದಿ (run == 2 || curfrp->fr_height >= ROWS_AVAIL)
	    {
		width = room;
		ಭಙ್ಗ;
	    }
	    frame_setwidth(curfrp->fr_parent, width
		 + frame_minwidth(curfrp->fr_parent, NOWIN) - (ಪೂರ್ಣಾಙ್ಕ)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = width - curfrp->fr_width;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	ಯದಿ (width > room - room_reserved)
	    room_reserved = room - width;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	ಯದಿ (take < 0 && room - curfrp->fr_width < room_reserved)
	    room_reserved = 0;

	/*
	 * set the current frame to the new width
	 */
	frame_new_width(curfrp, width, FALSE, FALSE);

	/*
	 * First take lines from the frames right of the current frame.  If
	 * that is not enough, takes lines from frames left of the current
	 * frame.
	 */
	ಪ್ರತಿ (run = 0; run < 2; ++run)
	{
	    ಯದಿ (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    ಅನ್ಯಥಾ
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    ಯಾವತ್ (frp != ರಿಕ್ತ && take != 0)
	    {
		w = frame_minwidth(frp, ರಿಕ್ತ);
		ಯದಿ (room_reserved > 0
			&& frp->fr_win != ರಿಕ್ತ
			&& frp->fr_win->w_p_wfw)
		{
		    ಯದಿ (room_reserved >= frp->fr_width)
			room_reserved -= frp->fr_width;
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (frp->fr_width - room_reserved > take)
			    room_reserved = frp->fr_width - take;
			take -= frp->fr_width - room_reserved;
			frame_new_width(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (frp->fr_width - take < w)
		    {
			take -= frp->fr_width - w;
			frame_new_width(frp, w, FALSE, FALSE);
		    }
		    ಅನ್ಯಥಾ
		    {
			frame_new_width(frp, frp->fr_width - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		ಯದಿ (run == 0)
		    frp = frp->fr_next;
		ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ		room;
    ಪೂರ್ಣಾಙ್ಕ		needed;
    ಪೂರ್ಣಾಙ್ಕ		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    ಯಾವತ್ (p_wmh > 0)
    {
	room = Rows - p_ch;
	needed = min_rows_for_all_tabpages() - 1;  // 1 was added for the cmdline
	ಯದಿ (room >= needed)
	    ಭಙ್ಗ;
	--p_wmh;
	ಯದಿ (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    ಶೂನ್ಯ
win_setminwidth(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		room;
    ಪೂರ್ಣಾಙ್ಕ		needed;
    ಪೂರ್ಣಾಙ್ಕ		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    ಯಾವತ್ (p_wmw > 0)
    {
	room = topframe->fr_width;
	needed = frame_minwidth(topframe, ರಿಕ್ತ);
	ಯದಿ (room >= needed)
	    ಭಙ್ಗ;
	--p_wmw;
	ಯದಿ (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    ಶೂನ್ಯ
win_drag_status_ಸಾಲು(win_T *dragwin, ಪೂರ್ಣಾಙ್ಕ offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    ಪೂರ್ಣಾಙ್ಕ		room;
    ಪೂರ್ಣಾಙ್ಕ		up;	// if TRUE, drag status line up, otherwise down
    ಪೂರ್ಣಾಙ್ಕ		n;

    fr = dragwin->w_frame;
    curfr = fr;
    ಯದಿ (fr != topframe)		// more than one window
    {
	fr = fr->fr_parent;
	// When the parent frame is not a column of frames, its parent should
	// be.
	ಯದಿ (fr->fr_layout != FR_COL)
	{
	    curfr = fr;
	    ಯದಿ (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_parent;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    ಯಾವತ್ (curfr != topframe && curfr->fr_next == ರಿಕ್ತ)
    {
	ಯದಿ (fr != topframe)
	    fr = fr->fr_parent;
	curfr = fr;
	ಯದಿ (fr != topframe)
	    fr = fr->fr_parent;
    }

    ಯದಿ (offset < 0) // drag up
    {
	up = TRUE;
	offset = -offset;
	// sum up the room of the current frame and above it
	ಯದಿ (fr == curfr)
	{
	    // only one window
	    room = fr->fr_height - frame_minheight(fr, ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ
	{
	    room = 0;
	    ಪ್ರತಿ (fr = fr->fr_child; ; fr = fr->fr_next)
	    {
		room += fr->fr_height - frame_minheight(fr, ರಿಕ್ತ);
		ಯದಿ (fr == curfr)
		    ಭಙ್ಗ;
	    }
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    ಅನ್ಯಥಾ    // drag down
    {
	up = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	room = Rows - cmdline_row;
	ಯದಿ (curfr->fr_next == ರಿಕ್ತ)
	    --room;
	ಅನ್ಯಥಾ
	    room -= p_ch;
	ಯದಿ (room < 0)
	    room = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_height - frame_minheight(fr, ರಿಕ್ತ);
	fr = curfr;			// put fr at window that grows
    }

    ಯದಿ (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    ಯದಿ (offset <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    ಯದಿ (fr != ರಿಕ್ತ)
	frame_new_height(fr, fr->fr_height + offset, up, FALSE, TRUE);

    ಯದಿ (up)
	fr = curfr;		// current frame gets smaller
    ಅನ್ಯಥಾ
	fr = curfr->fr_next;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    ಯಾವತ್ (fr != ರಿಕ್ತ && offset > 0)
    {
	n = frame_minheight(fr, ರಿಕ್ತ);
	ಯದಿ (fr->fr_height - offset <= n)
	{
	    offset -= fr->fr_height - n;
	    frame_new_height(fr, n, !up, FALSE, TRUE);
	}
	ಅನ್ಯಥಾ
	{
	    frame_new_height(fr, fr->fr_height - offset, !up, FALSE, TRUE);
	    ಭಙ್ಗ;
	}
	ಯದಿ (up)
	    fr = fr->fr_prev;
	ಅನ್ಯಥಾ
	    fr = fr->fr_next;
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
win_drag_vsep_ಸಾಲು(win_T *dragwin, ಪೂರ್ಣಾಙ್ಕ offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    ಪೂರ್ಣಾಙ್ಕ		room;
    ಪೂರ್ಣಾಙ್ಕ		ಎಡ;	// if TRUE, drag separator line left, otherwise right
    ಪೂರ್ಣಾಙ್ಕ		n;

    fr = dragwin->w_frame;
    ಯದಿ (fr == topframe)		// only one window (cannot happen?)
	ಪ್ರತ್ಯಾಗಮ;
    curfr = fr;
    fr = fr->fr_parent;
    // When the parent frame is not a row of frames, its parent should be.
    ಯದಿ (fr->fr_layout != FR_ROW)
    {
	ಯದಿ (fr == topframe)	// only a column of windows (cannot happen?)
	    ಪ್ರತ್ಯಾಗಮ;
	curfr = fr;
	fr = fr->fr_parent;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    ಯಾವತ್ (curfr->fr_next == ರಿಕ್ತ)
    {
	ಯದಿ (fr == topframe)
	    ಭಙ್ಗ;
	curfr = fr;
	fr = fr->fr_parent;
	ಯದಿ (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_parent;
	}
    }

    ಯದಿ (offset < 0) // drag left
    {
	ಎಡ = TRUE;
	offset = -offset;
	// sum up the room of the current frame and left of it
	room = 0;
	ಪ್ರತಿ (fr = fr->fr_child; ; fr = fr->fr_next)
	{
	    room += fr->fr_width - frame_minwidth(fr, ರಿಕ್ತ);
	    ಯದಿ (fr == curfr)
		ಭಙ್ಗ;
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    ಅನ್ಯಥಾ    // drag right
    {
	ಎಡ = FALSE;
	// sum up the room of frames right of the current one
	room = 0;
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_width - frame_minwidth(fr, ರಿಕ್ತ);
	fr = curfr;			// put fr at window that grows
    }

    ಯದಿ (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    ಯದಿ (offset <= 0)		// No room at all, quit.
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (fr == ರಿಕ್ತ)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	ಪ್ರತ್ಯಾಗಮ;

    // grow frame fr by offset lines
    frame_new_width(fr, fr->fr_width + offset, ಎಡ, FALSE);

    // shrink other frames: current and at the left or at the right
    ಯದಿ (ಎಡ)
	fr = curfr;		// current frame gets smaller
    ಅನ್ಯಥಾ
	fr = curfr->fr_next;	// next frame gets smaller

    ಯಾವತ್ (fr != ರಿಕ್ತ && offset > 0)
    {
	n = frame_minwidth(fr, ರಿಕ್ತ);
	ಯದಿ (fr->fr_width - offset <= n)
	{
	    offset -= fr->fr_width - n;
	    frame_new_width(fr, n, !ಎಡ, FALSE);
	}
	ಅನ್ಯಥಾ
	{
	    frame_new_width(fr, fr->fr_width - offset, !ಎಡ, FALSE);
	    ಭಙ್ಗ;
	}
	ಯದಿ (ಎಡ)
	    fr = fr->fr_prev;
	ಅನ್ಯಥಾ
	    fr = fr->fr_next;
    }
    win_comp_pos();
    redraw_all_later(UPD_NOT_VALID);
}

#define FRACTION_MULT	16384L

/*
 * Set wp->w_fraction for the current w_wrow and w_height.
 * Has no effect when the window is less than two lines.
 */
    ಶೂನ್ಯ
set_fraction(win_T *wp)
{
    ಯದಿ (wp->w_height > 1)
	// When cursor is in the first line the percentage is computed as if
	// it's halfway that line.  Thus with two lines it is 25%, with three
	// lines 17%, etc.  Similarly for the last line: 75%, 83%, etc.
	wp->w_fraction = ((long)wp->w_wrow * FRACTION_MULT
				     + FRACTION_MULT / 2) / (long)wp->w_height;
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
win_fix_scroll(ಪೂರ್ಣಾಙ್ಕ resize)
{
    ಯದಿ (*p_spk == 'c')
	ಪ್ರತ್ಯಾಗಮ;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	ಯದಿ (wp->w_height != wp->w_prev_height)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    ಯದಿ (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		ಪೂರ್ಣಾಙ್ಕ diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_height - wp->w_prev_height);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		ಯದಿ (diff > 0)
		    cursor_down_inner(wp, diff);
		ಅನ್ಯಥಾ
		    cursor_up_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_to_fraction(wp, wp->w_prev_height);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		ಯದಿ (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    ಯದಿ (!(get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL)))
	win_fix_cursor(FALSE);
    ಅನ್ಯಥಾ ಯದಿ (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_fix_cursor(ಪೂರ್ಣಾಙ್ಕ normal)
{
    win_T	*wp = curwin;

    ಯದಿ (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ < wp->w_height)
	ಪ್ರತ್ಯಾಗಮ;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    long so = MIN(wp->w_height / 2, get_scrolloff_ಮೌಲ್ಯ());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_down_inner(wp, so);
    linenr_T ತುದಿ = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_up_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    ಯದಿ (lnum > bot && (wp->w_botline - wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ) != 1)
	nlnum = bot;
    ಅನ್ಯಥಾ ಯದಿ (lnum < ತುದಿ && wp->w_topline != 1)
	nlnum = (so == wp->w_height / 2) ? bot : ತುದಿ;

    ಯದಿ (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	ಯದಿ (normal)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	ಅನ್ಯಥಾ  // Scroll instead when not in normal mode.
	{
	    wp->w_fraction = (nlnum == bot) ? FRACTION_MULT : 0;
	    scroll_to_fraction(wp, wp->w_prev_height);
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
win_new_height(win_T *wp, ಪೂರ್ಣಾಙ್ಕ height)
{
    ಪೂರ್ಣಾಙ್ಕ		prev_height = wp->w_height;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    ಯದಿ (height < 0)
	height = 0;
    ಯದಿ (wp->w_height == height)
	ಪ್ರತ್ಯಾಗಮ;	    // nothing to do

    ಯದಿ (wp->w_height > 0)
    {
	ಯದಿ (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	ಯದಿ (wp->w_height != prev_height)
	    ಪ್ರತ್ಯಾಗಮ;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	ಯದಿ (wp->w_wrow != wp->w_prev_fraction_row)
	    set_fraction(wp);
    }

    wp->w_height = height;
    wp->w_redr_status = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    ಯದಿ (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	ಯದಿ (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_to_fraction(wp, prev_height);
    }
}

    ಶೂನ್ಯ
scroll_to_fraction(win_T *wp, ಪೂರ್ಣಾಙ್ಕ prev_height)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		sline, ಸಾಲು_size;
    ಪೂರ್ಣಾಙ್ಕ		height = wp->w_height;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    ಯದಿ (height > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (height < wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	ಯದಿ (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((long)wp->w_fraction * (long)height - 1L)
							       / FRACTION_MULT;
	ಸಾಲು_size = plines_win_col(wp, lnum, (long)(wp->w_cursor.col)) - 1;
	sline = wp->w_wrow - ಸಾಲು_size;

	ಯದಿ (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    ಪೂರ್ಣಾಙ್ಕ rows = plines_win(wp, lnum, FALSE);

	    ಯದಿ (sline > wp->w_height - rows)
	    {
		sline = wp->w_height - rows;
		wp->w_wrow -= rows - ಸಾಲು_size;
	    }
	}

	ಯದಿ (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    ಪೂರ್ಣಾಙ್ಕ		want_row = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = ಸಾಲು_size;
	    ಯದಿ (wp->w_wrow >= wp->w_height
				       && (wp->w_width - win_col_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_width - win_col_off(wp);

		--wp->w_wrow;
		ಯಾವತ್ (wp->w_wrow > want_row)
		{
		    skipcol += wp->w_width - win_col_off(wp) + win_col_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (sline > 0)
	{
	    ಯಾವತ್ (sline > 0 && lnum > 1)
	    {
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, ರಿಕ್ತ, TRUE, ರಿಕ್ತ);
		ಯದಿ (lnum == 1)
		{
		    // first line in buffer is folded
		    ಸಾಲು_size = 1;
		    --sline;
		    ಭಙ್ಗ;
		}
#endif
		--lnum;
#ifdef FEAT_DIFF
		ಯದಿ (lnum == wp->w_topline)
		    ಸಾಲು_size = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		ಅನ್ಯಥಾ
#endif
		    ಸಾಲು_size = plines_win(wp, lnum, TRUE);
		sline -= ಸಾಲು_size;
	    }

	    ಯದಿ (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, ರಿಕ್ತ, &lnum, TRUE, ರಿಕ್ತ);
#endif
		lnum++;
		wp->w_wrow -= ಸಾಲು_size + sline;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	set_topline(wp, lnum);
    }

    ಯದಿ (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    ಯದಿ (prev_height > 0)
	wp->w_prev_fraction_row = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    ಶೂನ್ಯ
win_new_width(win_T *wp, ಪೂರ್ಣಾಙ್ಕ width)
{
    // Should we give an error if width < 0?
    wp->w_width = width < 0 ? 0 : width;
    wp->w_ಸಾಲುಗಳು_valid = 0;
    changed_ಸಾಲು_abv_curs_win(wp);
    invalidate_botline_win(wp);

    ಯದಿ (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_status = true;
}

    ಶೂನ್ಯ
win_comp_scroll(win_T *wp)
{
#ಯದಿ defined(FEAT_EVAL)
    ಪೂರ್ಣಾಙ್ಕ old_w_p_scr = wp->w_p_scr;
#endif

    wp->w_p_scr = ((unsigned)wp->w_height >> 1);
    ಯದಿ (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#ಯದಿ defined(FEAT_EVAL)
    ಯದಿ (wp->w_p_scr != old_w_p_scr)
    {
	// Used by "verbose set scroll".
	wp->w_p_script_ctx[WV_SCROLL].sc_sid = SID_WINLAYOUT;
	wp->w_p_script_ctx[WV_SCROLL].sc_lnum = 0;
    }
#endif
}

/*
 * Command_height: called whenever p_ch has been changed.
 */
    ಶೂನ್ಯ
command_height(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		old_p_ch = curtab->tp_ch_used;

    // Find bottom frame with width of screen.
    frame_T *frp = lastwin->w_frame;
    ಯಾವತ್ (frp->fr_width != topframe->fr_width && frp->fr_parent != ರಿಕ್ತ)
	frp = frp->fr_parent;

    // Avoid changing the height of a window with 'winfixheight' set.
    ಯಾವತ್ (frp->fr_prev != ರಿಕ್ತ && frp->fr_layout == FR_LEAF
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    ಯಾವತ್ (p_ch > old_p_ch && command_frame_height)
    {
	ಯದಿ (frp == ರಿಕ್ತ)
	{
	    emsg(_(e_not_enough_room));
	    p_ch = old_p_ch;
	    ಭಙ್ಗ;
	}
	ಪೂರ್ಣಾಙ್ಕ h = MIN(p_ch - old_p_ch,
			frp->fr_height - frame_minheight(frp, ರಿಕ್ತ));
	frame_add_height(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    ಯದಿ (p_ch < old_p_ch && command_frame_height && frp != ರಿಕ್ತ)
	frame_add_height(frp, (ಪೂರ್ಣಾಙ್ಕ)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_row = Rows - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    ಯದಿ (ಸಂದೇಶ_scrolled == 0 && full_screen)
    {
	screen_fill(cmdline_row, (ಪೂರ್ಣಾಙ್ಕ)Rows, 0, (ಪೂರ್ಣಾಙ್ಕ)Columns, ' ', ' ', 0);
	ಸಂದೇಶ_row = cmdline_row;
    }

    // Use the value of p_ch that we remembered.  This is needed for when the
    // GUI starts up, we can't be sure in what order things happen.  And when
    // p_ch was changed in another tab page.
    curtab->tp_ch_used = p_ch;
    min_set_ch = p_ch;
}

/*
 * Resize frame "frp" to be "n" lines higher (negative for less high).
 * Also resize the frames it is contained in.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_add_height(frame_T *frp, ಪೂರ್ಣಾಙ್ಕ n)
{
    frame_new_height(frp, frp->fr_height + n, FALSE, FALSE, FALSE);
    ಪ್ರತಿ (;;)
    {
	frp = frp->fr_parent;
	ಯದಿ (frp == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	frp->fr_height += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    ಶೂನ್ಯ
last_status(
    ಪೂರ್ಣಾಙ್ಕ		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    last_status_rec(topframe, last_stl_height(morewin) > 0);
#ಯದಿ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
last_status_rec(frame_T *fr, ಪೂರ್ಣಾಙ್ಕ statusline)
{
    frame_T	*fp;
    win_T	*wp;

    ಯದಿ (fr->fr_layout == FR_LEAF)
    {
	wp = fr->fr_win;
	ಯದಿ (wp->w_status_height != 0 && !statusline)
	{
	    // remove status line
	    win_new_height(wp, wp->w_height + wp->w_status_height);
	    wp->w_status_height = 0;
	    comp_col();
	}
	ಅನ್ಯಥಾ ಯದಿ (wp->w_status_height == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    ಯಾವತ್ (fp->fr_height <= frame_minheight(fp, ರಿಕ್ತ))
	    {
		ಯದಿ (fp == topframe)
		{
		    emsg(_(e_not_enough_room));
		    ಪ್ರತ್ಯಾಗಮ;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		ಯದಿ (fp->fr_parent->fr_layout == FR_COL && fp->fr_prev != ರಿಕ್ತ)
		    fp = fp->fr_prev;
		ಅನ್ಯಥಾ
		    fp = fp->fr_parent;
	    }
	    wp->w_status_height = statusline_height(wp);
	    ಯದಿ (fp != fr)
	    {
		frame_new_height(fp, fp->fr_height - wp->w_status_height,
			FALSE, FALSE, FALSE);
		frame_fix_height(wp);
		win_comp_pos();
	    }
	    ಅನ್ಯಥಾ
		win_new_height(wp, wp->w_height - wp->w_status_height);
	    comp_col();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	ಯದಿ (abs(wp->w_height - wp->w_prev_height) == 1)
	    wp->w_prev_height = wp->w_height;
    }
    ಅನ್ಯಥಾ ಯದಿ (fr->fr_layout == FR_ROW)
    {
	// vertically split windows, set status line for each one
	ಯದಿ (!statusline)
	{
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		last_status_rec(fp, statusline);
	}
	ಅನ್ಯಥಾ
	{
	    frame_T	*fp2;
	    ಪೂರ್ಣಾಙ್ಕ		max_stlh = 0;
	    ಪೂರ್ಣಾಙ್ಕ		new_row_height;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		ಯದಿ (fp->fr_win != ರಿಕ್ತ && fp->fr_win->w_status_height == 0)
		{
		    ಪೂರ್ಣಾಙ್ಕ h = statusline_height(fp->fr_win);
		    ಯದಿ (h > max_stlh)
			max_stlh = h;
		}
	    ಯದಿ (max_stlh == 0)
		ಪ್ರತ್ಯಾಗಮ;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    ಯಾವತ್ (fp2->fr_height - frame_minheight(fp2, ರಿಕ್ತ) < max_stlh)
	    {
		ಯದಿ (fp2 == topframe)
		{
		    emsg(_(e_not_enough_room));
		    ಪ್ರತ್ಯಾಗಮ;
		}
		ಯದಿ (fp2->fr_parent->fr_layout == FR_COL
						&& fp2->fr_prev != ರಿಕ್ತ)
		    fp2 = fp2->fr_prev;
		ಅನ್ಯಥಾ
		    fp2 = fp2->fr_parent;
	    }

	    ಯದಿ (fp2 != fr)
	    {
		frame_new_height(fp2, fp2->fr_height - max_stlh,
							FALSE, FALSE, FALSE);
		new_row_height = fr->fr_height + max_stlh;
	    }
	    ಅನ್ಯಥಾ
		new_row_height = fr->fr_height;

	    // Set status and content heights for all leaves.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
	    {
		ಯದಿ (fp->fr_win != ರಿಕ್ತ)
		{
		    wp = fp->fr_win;
		    ಯದಿ (wp->w_status_height == 0)
		    {
			wp->w_status_height = statusline_height(wp);
			win_new_height(wp,
				  new_row_height - wp->w_status_height);
			frame_fix_height(wp);
			comp_col();
			redraw_all_later(UPD_SOME_VALID);
			ಯದಿ (abs(wp->w_height - wp->w_prev_height) == 1)
			    wp->w_prev_height = wp->w_height;
		    }
		}
		ಅನ್ಯಥಾ
		    last_status_rec(fp, statusline); // nested frames
	    }

	    fr->fr_height = new_row_height;
	    win_comp_pos();
	}
    }
    ಅನ್ಯಥಾ
    {
	// horizontally split window, set status line for last one
	ಪ್ರತಿ (fp = fr->fr_child; fp->fr_next != ರಿಕ್ತ; fp = fp->fr_next)
	    ;
	last_status_rec(fp, statusline);
    }
}

#ಯದಿ defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    ಶೂನ್ಯ
update_stl_rendered_height(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == ರಿಕ್ತ) ? p_stl : wp->w_p_stl;
    ಪೂರ್ಣಾಙ್ಕ		opt_flags = (wp == ರಿಕ್ತ) ? 0 : OPT_LOCAL;
    ಪೂರ್ಣಾಙ್ಕ		*height = (wp == ರಿಕ್ತ) ? &stl_rendered_height
						: &wp->w_stl_rendered_height;

    *height = get_stl_rendered_height(wp == ರಿಕ್ತ ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
frame_find_global_stlh_rec(frame_T *frp, ಪೂರ್ಣಾಙ್ಕ h)
{
    ಯದಿ (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	ಯದಿ (wp->w_height > 0 && wp->w_status_height > 0
		&& *wp->w_p_stlo == NUL)
	{
	    ಪೂರ್ಣಾಙ್ಕ win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    ಯದಿ (win_free_height - p_wmh < h)
		h = win_free_height - p_wmh;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (frp = frp->fr_child; frp != ರಿಕ್ತ; frp = frp->fr_next)
	    h = frame_find_global_stlh_rec(frp, h);
    }
    ಪ್ರತ್ಯಾಗಮ h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
frame_change_statusline_height_rec(frame_T *frp, ಪೂರ್ಣಾಙ್ಕ global_stlh)
{
    ಯದಿ (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	ಯದಿ (wp->w_height > 0 && wp->w_status_height > 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    wp->w_status_height = statusline_height(wp);
	    ಯದಿ (wp->w_p_stlo == ರಿಕ್ತ || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		ಯದಿ (wp->w_status_height > global_stlh)
		    wp->w_status_height = global_stlh;
	    }
	    ಯದಿ (wp->w_status_height > win_free_height - p_wmh)
		wp->w_status_height = win_free_height - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    ಪೂರ್ಣಾಙ್ಕ	     saved_topfill = wp->w_topfill;
# endif
	    win_new_height(wp, win_free_height - wp->w_status_height);
	    wp->w_topline = saved_topline;
# ifdef FEAT_DIFF
	    wp->w_topfill = saved_topfill;
# endif
	    wp->w_valid &= ~(VALID_WROW | VALID_CROW
			     | VALID_BOTLINE | VALID_BOTLINE_AP);
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (frp = frp->fr_child; frp != ರಿಕ್ತ; frp = frp->fr_next)
	    frame_change_statusline_height_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    ಪೂರ್ಣಾಙ್ಕ
frame_change_statusline_height(ಶೂನ್ಯ)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಙ್ಕ		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = frame_find_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	frame_change_statusline_height_rec(tp->tp_topframe, global_stlh);
    comp_col();
    redraw_all_later(UPD_SOME_VALID);
    ಪ್ರತ್ಯಾಗಮ global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    ಶೂನ್ಯ
set_stlo_mh(ಪೂರ್ಣಾಙ್ಕ mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
statuslineopt_changed(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		l_stlo_fh = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		l_stlo_mh = 1;

    ಯದಿ (stlopt != ರಿಕ್ತ)
	p = stlopt;
    ಅನ್ಯಥಾ
	p = wp->w_p_stlo;

    ಯಾವತ್ (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	ಯದಿ (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    ಯದಿ (l_stlo_mh < 1)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	ಯದಿ (*p != ',' && *p != NUL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (*p == ',')
	    ++p;
    }

    ಯದಿ (wp == ರಿಕ್ತ)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    ಅನ್ಯಥಾ
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    ಪೂರ್ಣಾಙ್ಕ
statusline_height(win_T *wp UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ stl_height = 1;
#ಯದಿ defined(FEAT_STL_OPT)
    bool fixed;
    ಪೂರ್ಣಾಙ್ಕ l_stl_rendered_height;

    ಯದಿ (wp != ರಿಕ್ತ && *wp->w_p_stlo != NUL)
    {
	stl_height = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_height = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }
    ಅನ್ಯಥಾ
    {
	stl_height = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_height = (wp != ರಿಕ್ತ && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    ಯದಿ (!fixed)
    {
	ಪೂರ್ಣಾಙ್ಕ has_custom_stl = (*p_stl != NUL)
	    || (wp != ರಿಕ್ತ && *wp->w_p_stl != NUL);
	ಯದಿ (has_custom_stl)
	    stl_height = MAX(1, MIN(l_stl_rendered_height, stl_height));
	ಅನ್ಯಥಾ
	    stl_height = 1;
    }
#endif

    ಪ್ರತ್ಯಾಗಮ stl_height;
}

/*
 * Return the number of lines used by the tab page line.
 */
    ಪೂರ್ಣಾಙ್ಕ
tabline_height(ಶೂನ್ಯ)
{
#ifdef FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    ಯದಿ (gui_use_tabline())
	ಪ್ರತ್ಯಾಗಮ 0;
#endif
    ವಿಕಲ್ಪ (p_stal)
    {
	ಪಕ್ಷ 0: ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ 1: ಪ್ರತ್ಯಾಗಮ (first_tabpage->tp_next == ರಿಕ್ತ) ? 0 : 1;
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * Return the height of the last window's statusline.
 */
    ಪೂರ್ಣಾಙ್ಕ
last_stl_height(
    ಪೂರ್ಣಾಙ್ಕ		morewin)	// pretend there are two or more windows
{
    ಪ್ರತ್ಯಾಗಮ (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_WINDOW)))
		? STATUS_HEIGHT : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    ಪೂರ್ಣಾಙ್ಕ
min_rows(ಶೂನ್ಯ)
{
    ಯದಿ (firstwin == ರಿಕ್ತ)	// not initialized yet
	ಪ್ರತ್ಯಾಗಮ MIN_ಸಾಲುಗಳು;

    ಪ್ರತ್ಯಾಗಮ frame_minheight(curtab->tp_topframe, ರಿಕ್ತ) + tabline_height()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    ಪೂರ್ಣಾಙ್ಕ
min_rows_for_all_tabpages(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		total;
    tabpage_T	*tp;
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯದಿ (firstwin == ರಿಕ್ತ)	// not initialized yet
	ಪ್ರತ್ಯಾಗಮ MIN_ಸಾಲುಗಳು;

    total = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = frame_minheight(tp->tp_topframe, ರಿಕ್ತ);
	ಯದಿ (total < n)
	    total = n;
    }
    total += tabline_height();
    total += MIN_CMDHEIGHT;
    ಪ್ರತ್ಯಾಗಮ total;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    ಪೂರ್ಣಾಙ್ಕ
only_one_window(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    win_T	*wp;

#ಯದಿ defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    ಯದಿ (popup_is_popup(curwin))
	ಪ್ರತ್ಯಾಗಮ FALSE;
#endif

    // If there is another tab page there always is another window.
    ಯದಿ (first_tabpage->tp_next != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_buffer != ರಿಕ್ತ
		&& (!((bt_help(wp->w_buffer) && !bt_help(curbuf))
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ (ಎಣಿಕೆ <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_lnums_both(ಪೂರ್ಣಾಙ್ಕ do_curwin, ಪೂರ್ಣಾಙ್ಕ nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಯದಿ ((do_curwin || wp != curwin) && wp->w_buffer == curbuf)
	{
	    ಪೂರ್ಣಾಙ್ಕ need_adjust;

	    ಯದಿ (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಯದಿ (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಯದಿ (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಯದಿ (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಯದಿ (need_adjust || !nested)
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
check_lnums(ಪೂರ್ಣಾಙ್ಕ do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    ಶೂನ್ಯ
check_lnums_nested(ಪೂರ್ಣಾಙ್ಕ do_curwin)
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
	ಯದಿ (wp->w_buffer == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    ಯದಿ (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    ಯದಿ (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    ಯದಿ (wp->w_save_cursor.w_topline_save > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
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
    ಪೂರ್ಣಾಙ್ಕ
make_snapshot(ಪೂರ್ಣಾಙ್ಕ idx)
{
    clear_snapshot(curtab, idx);
    ಯದಿ (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
make_snapshot_rec(frame_T *fr, frame_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(frame_T);
    ಯದಿ (*frp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_width = fr->fr_width;
    (*frp)->fr_height = fr->fr_height;
    ಯದಿ (fr->fr_next != ರಿಕ್ತ)
    {
	ಯದಿ (make_snapshot_rec(fr->fr_next, &((*frp)->fr_next)) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (fr->fr_child != ರಿಕ್ತ)
    {
	ಯದಿ (make_snapshot_rec(fr->fr_child, &((*frp)->fr_child)) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (fr->fr_layout == FR_LEAF && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Remove any existing snapshot.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_snapshot(tabpage_T *tp, ಪೂರ್ಣಾಙ್ಕ idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_snapshot_rec(frame_T *fr)
{
    ಯದಿ (fr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    clear_snapshot_rec(fr->fr_next);
    clear_snapshot_rec(fr->fr_child);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    ಸ್ಥಿರ win_T *
get_snapshot_curwin_rec(frame_T *ft)
{
    win_T	*wp;

    ಯದಿ (ft->fr_next != ರಿಕ್ತ)
    {
	ಯದಿ ((wp = get_snapshot_curwin_rec(ft->fr_next)) != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ wp;
    }
    ಯದಿ (ft->fr_child != ರಿಕ್ತ)
    {
	ಯದಿ ((wp = get_snapshot_curwin_rec(ft->fr_child)) != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ wp;
    }

    ಪ್ರತ್ಯಾಗಮ ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    ಸ್ಥಿರ win_T *
get_snapshot_curwin(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (curtab->tp_snapshot[idx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    ಶೂನ್ಯ
restore_snapshot(
    ಪೂರ್ಣಾಙ್ಕ		idx,
    ಪೂರ್ಣಾಙ್ಕ		close_curwin)	    // closing current window
{
    win_T	*wp;

    ಯದಿ (curtab->tp_snapshot[idx] != ರಿಕ್ತ
	    && curtab->tp_snapshot[idx]->fr_width == topframe->fr_width
	    && curtab->tp_snapshot[idx]->fr_height == topframe->fr_height
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	ಯದಿ (wp != ರಿಕ್ತ && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_snapshot_rec(frame_T *sn, frame_T *fr)
{
    ಯದಿ (sn->fr_layout != fr->fr_layout
	    || (sn->fr_next == ರಿಕ್ತ) != (fr->fr_next == ರಿಕ್ತ)
	    || (sn->fr_child == ರಿಕ್ತ) != (fr->fr_child == ರಿಕ್ತ)
	    || (sn->fr_next != ರಿಕ್ತ
		&& check_snapshot_rec(sn->fr_next, fr->fr_next) == FAIL)
	    || (sn->fr_child != ರಿಕ್ತ
		&& check_snapshot_rec(sn->fr_child, fr->fr_child) == FAIL)
	    || (sn->fr_win != ರಿಕ್ತ && !win_valid(sn->fr_win)))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    ಸ್ಥಿರ win_T *
restore_snapshot_rec(frame_T *sn, frame_T *fr)
{
    win_T	*wp = ರಿಕ್ತ;
    win_T	*wp2;

    fr->fr_height = sn->fr_height;
    fr->fr_width = sn->fr_width;
    ಯದಿ (fr->fr_layout == FR_LEAF)
    {
	frame_new_height(fr, fr->fr_height, FALSE, FALSE, FALSE);
	frame_new_width(fr, fr->fr_width, FALSE, FALSE);
	wp = sn->fr_win;
    }
    ಯದಿ (sn->fr_next != ರಿಕ್ತ)
    {
	wp2 = restore_snapshot_rec(sn->fr_next, fr->fr_next);
	ಯದಿ (wp2 != ರಿಕ್ತ)
	    wp = wp2;
    }
    ಯದಿ (sn->fr_child != ರಿಕ್ತ)
    {
	wp2 = restore_snapshot_rec(sn->fr_child, fr->fr_child);
	ಯದಿ (wp2 != ರಿಕ್ತ)
	    wp = wp2;
    }
    ಪ್ರತ್ಯಾಗಮ wp;
}

#ಯದಿ defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    ಪೂರ್ಣಾಙ್ಕ
win_hasvertsplit(ಶೂನ್ಯ)
{
    frame_T	*fr;

    ಯದಿ (topframe->fr_layout == FR_ROW)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಯದಿ (topframe->fr_layout == FR_COL)
	FOR_ALL_FRAMES(fr, topframe->fr_child)
	    ಯದಿ (fr->fr_layout == FR_ROW)
		ಪ್ರತ್ಯಾಗಮ TRUE;

    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

#ಯದಿ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    ಪೂರ್ಣಾಙ್ಕ
get_win_ಸಂಖ್ಯೆ(win_T *wp, win_T *first_win)
{
    ಪೂರ್ಣಾಙ್ಕ		i = 1;
    win_T	*w;

    ಪ್ರತಿ (w = first_win; w != ರಿಕ್ತ && w != wp; w = W_NEXT(w))
	++i;

    ಯದಿ (w == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ i;
}

    ಪೂರ್ಣಾಙ್ಕ
get_tab_ಸಂಖ್ಯೆ(tabpage_T *tp)
{
    ಪೂರ್ಣಾಙ್ಕ		i = 1;
    tabpage_T	*t;

    ಪ್ರತಿ (t = first_tabpage; t != ರಿಕ್ತ && t != tp; t = t->tp_next)
	++i;

    ಯದಿ (t == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ i;
}
#endif

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
frame_check_height(frame_T *topfrp, ಪೂರ್ಣಾಙ್ಕ height)
{
    frame_T *frp;

    ಯದಿ (topfrp->fr_height != height)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (topfrp->fr_layout == FR_ROW)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    ಯದಿ (frp->fr_height != height)
		ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
frame_check_width(frame_T *topfrp, ಪೂರ್ಣಾಙ್ಕ width)
{
    frame_T *frp;

    ಯದಿ (topfrp->fr_width != width)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (topfrp->fr_layout == FR_COL)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    ಯದಿ (frp->fr_width != width)
		ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ಯದಿ defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
int_cmp(ಧ್ರುವ ಶೂನ್ಯ *pa, ಧ್ರುವ ಶೂನ್ಯ *pb)
{
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ a = *(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *)pa;
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ b = *(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *)pb;
    ಯದಿ (a > b)
	ಪ್ರತ್ಯಾಗಮ 1;
    ಯದಿ (a < b)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    ವರ್ಣ *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = empty_option;
    ಪೂರ್ಣಾಙ್ಕ		tw;
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		color_cols[256];
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		j = 0;

    ಯದಿ (wp != ರಿಕ್ತ && wp->w_buffer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;  // buffer was closed

    ಯದಿ (cc != ರಿಕ್ತ)
	s = cc;
    ಅನ್ಯಥಾ ಯದಿ (wp != ರಿಕ್ತ)
	s = wp->w_p_cc;

    ಯದಿ (wp != ರಿಕ್ತ)
	tw = wp->w_buffer->b_p_tw;
    ಅನ್ಯಥಾ
	// buffer-local value not set, assume zero
	tw = 0;

    ಯಾವತ್ (*s != NUL && ಎಣಿಕೆ < 255)
    {
	ಯದಿ (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    col = (*s == '-') ? -1 : 1;
	    ++s;
	    ಯದಿ (!VIM_ISDIGIT(*s))
		ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	    col = col * getdigits(&s);
	    ಯದಿ (tw == 0)
		ಗಚ್ಛ skip;  // 'textwidth' not set, skip this item
	    col += tw;
	    ಯದಿ (col < 0)
		ಗಚ್ಛ skip;
	}
	ಅನ್ಯಥಾ ಯದಿ (VIM_ISDIGIT(*s))
	    col = getdigits(&s);
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	color_cols[ಎಣಿಕೆ++] = col - 1;  // 1-based to 0-based
skip:
	ಯದಿ (*s == NUL)
	    ಭಙ್ಗ;
	ಯದಿ (*s != ',')
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	ಯದಿ (*++s == NUL)
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;  // illegal trailing comma as in "set cc=80,"
    }

    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;  // only parse "cc"

    vim_free(wp->w_p_cc_cols);
    ಯದಿ (ಎಣಿಕೆ == 0)
	wp->w_p_cc_cols = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	wp->w_p_cc_cols = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, ಎಣಿಕೆ + 1);
	ಯದಿ (wp->w_p_cc_cols != ರಿಕ್ತ)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_cols, ಎಣಿಕೆ, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ), int_cmp);

	    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
		// skip duplicates
		ಯದಿ (j == 0 || wp->w_p_cc_cols[j - 1] != color_cols[i])
		    wp->w_p_cc_cols[j++] = color_cols[i];
	    wp->w_p_cc_cols[j] = -1;  // end marker
	}
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;  // no error
}
#endif

    ಪೂರ್ಣಾಙ್ಕ
get_last_winid(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ last_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   ಪೂರ್ಣಾಙ್ಕ
win_locked(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ wp->w_locked;
}
