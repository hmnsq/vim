/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#അന്തര്ഭാവ "vim.h"

സ്ഥിതം ശൂന്യം cmd_with_എണ്ണം(അക്ഷരം *cmd, char_u *bufp, size_t bufsize, long Prenum);
സ്ഥിതം ശൂന്യം win_init(win_T *newp, win_T *oldp, പൂർണ്ണസംഖ്യ flags);
സ്ഥിതം ശൂന്യം win_init_some(win_T *newp, win_T *oldp);
സ്ഥിതം ശൂന്യം frame_comp_pos(frame_T *topfrp, പൂർണ്ണസംഖ്യ *row, പൂർണ്ണസംഖ്യ *col);
സ്ഥിതം ശൂന്യം frame_setheight(frame_T *curfrp, പൂർണ്ണസംഖ്യ height);
സ്ഥിതം ശൂന്യം frame_setwidth(frame_T *curfrp, പൂർണ്ണസംഖ്യ width);
സ്ഥിതം ശൂന്യം win_exchange(long);
സ്ഥിതം ശൂന്യം win_rotate(പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ);
സ്ഥിതം ശൂന്യം win_equal_rec(win_T *next_curwin, പൂർണ്ണസംഖ്യ current, frame_T *topfr, പൂർണ്ണസംഖ്യ dir, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height);
സ്ഥിതം ശൂന്യം trigger_winnewpre(ശൂന്യം);
സ്ഥിതം ശൂന്യം trigger_winclosed(win_T *win);
സ്ഥിതം win_T *win_free_mem(win_T *win, പൂർണ്ണസംഖ്യ *dirp, tabpage_T *tp);
സ്ഥിതം frame_T *win_altframe(win_T *win, tabpage_T *tp);
സ്ഥിതം tabpage_T *alt_tabpage(ശൂന്യം);
സ്ഥിതം win_T *frame2win(frame_T *frp);
സ്ഥിതം പൂർണ്ണസംഖ്യ frame_has_win(frame_T *frp, win_T *wp);
സ്ഥിതം ശൂന്യം win_fix_scroll(പൂർണ്ണസംഖ്യ resize);
സ്ഥിതം ശൂന്യം win_fix_cursor(പൂർണ്ണസംഖ്യ normal);
സ്ഥിതം ശൂന്യം frame_new_height(frame_T *topfrp, പൂർണ്ണസംഖ്യ height, പൂർണ്ണസംഖ്യ topfirst, പൂർണ്ണസംഖ്യ wfh, പൂർണ്ണസംഖ്യ set_ch);
സ്ഥിതം പൂർണ്ണസംഖ്യ frame_fixed_height(frame_T *frp);
സ്ഥിതം പൂർണ്ണസംഖ്യ frame_fixed_width(frame_T *frp);
സ്ഥിതം ശൂന്യം frame_add_statusline(frame_T *frp);
സ്ഥിതം ശൂന്യം frame_new_width(frame_T *topfrp, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ leftfirst, പൂർണ്ണസംഖ്യ wfw);
സ്ഥിതം ശൂന്യം frame_set_vsep(frame_T *frp, പൂർണ്ണസംഖ്യ add);
സ്ഥിതം പൂർണ്ണസംഖ്യ frame_minwidth(frame_T *topfrp, win_T *next_curwin);
സ്ഥിതം ശൂന്യം frame_fix_width(win_T *wp);
സ്ഥിതം പൂർണ്ണസംഖ്യ win_alloc_firstwin(win_T *oldwin);
സ്ഥിതം ശൂന്യം new_frame(win_T *wp);
സ്ഥിതം tabpage_T *alloc_tabpage(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ leave_tabpage(buf_T *new_curbuf, പൂർണ്ണസംഖ്യ trigger_leave_autocmds);
സ്ഥിതം ശൂന്യം enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, പൂർണ്ണസംഖ്യ trigger_enter_autocmds, പൂർണ്ണസംഖ്യ trigger_leave_autocmds);
സ്ഥിതം ശൂന്യം frame_fix_height(win_T *wp);
സ്ഥിതം പൂർണ്ണസംഖ്യ frame_minheight(frame_T *topfrp, win_T *next_curwin);
സ്ഥിതം പൂർണ്ണസംഖ്യ frame_wincount_in_height(frame_T *topfrp);
സ്ഥിതം പൂർണ്ണസംഖ്യ may_open_tabpage(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ win_enter_ext(win_T *wp, പൂർണ്ണസംഖ്യ flags);
സ്ഥിതം ശൂന്യം win_free(win_T *wp, tabpage_T *tp);
സ്ഥിതം ശൂന്യം win_append(win_T *after, win_T *wp);
സ്ഥിതം ശൂന്യം frame_append(frame_T *after, frame_T *frp);
സ്ഥിതം ശൂന്യം frame_തിരുകുക(frame_T *before, frame_T *frp);
സ്ഥിതം ശൂന്യം frame_remove(frame_T *frp);
സ്ഥിതം ശൂന്യം win_goto_ver(പൂർണ്ണസംഖ്യ up, long എണ്ണം);
സ്ഥിതം ശൂന്യം win_goto_hor(പൂർണ്ണസംഖ്യ ഇടത്, long എണ്ണം);
സ്ഥിതം ശൂന്യം frame_add_height(frame_T *frp, പൂർണ്ണസംഖ്യ n);
സ്ഥിതം ശൂന്യം last_status_rec(frame_T *fr, പൂർണ്ണസംഖ്യ statusline);
#എങ്കിൽ defined(FEAT_STL_OPT)
സ്ഥിതം പൂർണ്ണസംഖ്യ frame_find_global_stlh_rec(frame_T *frp, പൂർണ്ണസംഖ്യ h);
സ്ഥിതം ശൂന്യം frame_change_statusline_height_rec(frame_T *frp, പൂർണ്ണസംഖ്യ global_stlh);
#endif
സ്ഥിതം ശൂന്യം frame_flatten(frame_T *frp);
സ്ഥിതം ശൂന്യം winframe_restore(win_T *wp, പൂർണ്ണസംഖ്യ dir, frame_T *unflat_altfr);

സ്ഥിതം പൂർണ്ണസംഖ്യ make_snapshot_rec(frame_T *fr, frame_T **frp);
സ്ഥിതം ശൂന്യം clear_snapshot(tabpage_T *tp, പൂർണ്ണസംഖ്യ idx);
സ്ഥിതം ശൂന്യം clear_snapshot_rec(frame_T *fr);
സ്ഥിതം പൂർണ്ണസംഖ്യ check_snapshot_rec(frame_T *sn, frame_T *fr);
സ്ഥിതം win_T *restore_snapshot_rec(frame_T *sn, frame_T *fr);
സ്ഥിതം win_T *get_snapshot_curwin(പൂർണ്ണസംഖ്യ idx);

സ്ഥിതം പൂർണ്ണസംഖ്യ frame_check_height(frame_T *topfrp, പൂർണ്ണസംഖ്യ height);
സ്ഥിതം പൂർണ്ണസംഖ്യ frame_check_width(frame_T *topfrp, പൂർണ്ണസംഖ്യ width);

സ്ഥിതം win_T *win_alloc(win_T *after, പൂർണ്ണസംഖ്യ hidden);

#define NOWIN		((win_T *)-1)	// non-existing window

#define ROWS_AVAIL (Rows - p_ch - tabline_height())

// flags for win_enter_ext()
#define WEE_UNDO_SYNC			0x01
#define WEE_CURWIN_INVALID		0x02
#define WEE_TRIGGER_NEW_AUTOCMDS	0x04
#define WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#define WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#define WEE_ALLOW_PARSE_MESSAGES	0x20

സ്ഥിതം അക്ഷരം *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
സ്ഥിതം പൂർണ്ണസംഖ്യ split_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
സ്ഥിതം പൂർണ്ണസംഖ്യ close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
സ്ഥിതം പൂർണ്ണസംഖ്യ frame_locked = 0;

#എങ്കിൽ defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
സ്ഥിതം പൂർണ്ണസംഖ്യ stl_rendered_height = 1;
സ്ഥിതം പൂർണ്ണസംഖ്യ stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
സ്ഥിതം പൂർണ്ണസംഖ്യ stlo_mh = STATUS_HEIGHT; // "maxheight" value cache of global 'stlo'
#endif

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    ശൂന്യം
window_layout_lock(ശൂന്യം)
{
    ++split_disallowed;
    ++close_disallowed;
}

    ശൂന്യം
window_layout_unlock(ശൂന്യം)
{
    --split_disallowed;
    --close_disallowed;
}

    പൂർണ്ണസംഖ്യ
frames_locked(ശൂന്യം)
{
    തിരികെ frame_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    പൂർണ്ണസംഖ്യ
window_layout_locked(എണ്ണൽ CMD_index cmd)
{
    എങ്കിൽ (split_disallowed > 0 || close_disallowed > 0)
    {
	എങ്കിൽ (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_split_window_when_closing_buffer));
	അല്ലെങ്കിൽ
	    emsg(_(e_not_allowed_to_change_window_layout_in_this_autocmd));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

// #define WIN_DEBUG
#ifdef WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    സ്ഥിതം ശൂന്യം
log_frame_layout(frame_T *frame)
{
    ch_log(ശൂന്യ, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    frame->fr_layout == FR_LEAF ? "LEAF"
				  : frame->fr_layout == FR_ROW ? "ROW" : "COL",
	    frame->fr_width,
	    frame->fr_height,
	    frame->fr_win == ശൂന്യ ? -1 : frame->fr_win->w_width,
	    frame->fr_win == ശൂന്യ ? -1 : frame->fr_win->w_height,
	    frame->fr_win == ശൂന്യ ? -1 : frame->fr_win->w_id);
    എങ്കിൽ (frame->fr_child != ശൂന്യ)
    {
	ch_log(ശൂന്യ, "children");
	log_frame_layout(frame->fr_child);
	എങ്കിൽ (frame->fr_next != ശൂന്യ)
	    ch_log(ശൂന്യ, "END of children");
    }
    എങ്കിൽ (frame->fr_next != ശൂന്യ)
	log_frame_layout(frame->fr_next);
}
#endif

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    പൂർണ്ണസംഖ്യ
check_can_set_curbuf_disabled(ശൂന്യം)
{
    എങ്കിൽ (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    പൂർണ്ണസംഖ്യ
check_can_set_curbuf_forceit(പൂർണ്ണസംഖ്യ forceit)
{
    എങ്കിൽ (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(ശൂന്യം)
{
    // In cmdwin, the alternative buffer should be used.
    തിരികെ is_in_cmdwin() && prevwin != ശൂന്യ ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = ശൂന്യ;

    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    എങ്കിൽ (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    എങ്കിൽ (wp == ശൂന്യ && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    തിരികെ wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    ശൂന്യം
do_window(
    പൂർണ്ണസംഖ്യ		nchar,
    long	Prenum,
    പൂർണ്ണസംഖ്യ		xchar)	    // extra char from ":wincmd gx" or NUL
{
    long	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ifdef FEAT_FIND_ID
    പൂർണ്ണസംഖ്യ		type = FIND_DEFINE;
    പൂർണ്ണസംഖ്യ		len;
#endif
    char_u	cbuf[40];

    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;

#define CHECK_CMDWIN \
    ചെയ്യുക { \
	എങ്കിൽ (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_in_cmdline_window)); \
	    തിരികെ; \
	} \
    } ആകുമ്പോൾ (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    തിരഞ്ഞെടുപ്പ് (nchar)
    {
// split current window in two parts, horizontally
    അവസ്ഥ 'S':
    അവസ്ഥ Ctrl_S:
    അവസ്ഥ 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		എങ്കിൽ (bt_quickfix(curbuf))
		    പോകുക newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(ശൂന്യം)win_split((പൂർണ്ണസംഖ്യ)Prenum, 0);
		നിർത്തുക;

// split current window in two parts, vertically
    അവസ്ഥ Ctrl_V:
    അവസ്ഥ 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		എങ്കിൽ (bt_quickfix(curbuf))
		    പോകുക newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(ശൂന്യം)win_split((പൂർണ്ണസംഖ്യ)Prenum, WSP_VERT);
		നിർത്തുക;

// split current window and edit alternate file
    അവസ്ഥ Ctrl_HAT:
    അവസ്ഥ '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		എങ്കിൽ (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == ശൂന്യ)
		{
		    എങ്കിൽ (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    അല്ലെങ്കിൽ
			semsg(_(e_buffer_nr_not_found), Prenum);
		    നിർത്തുക;
		}

		എങ്കിൽ (!curbuf_locked() && win_split(0, 0) == OK)
		    (ശൂന്യം)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		നിർത്തുക;

// open new window
    അവസ്ഥ Ctrl_N:
    അവസ്ഥ 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		എങ്കിൽ (Prenum)
		    // window height
		    vim_snprintf((അക്ഷരം *)cbuf, വലുപ്പം(cbuf) - 5, "%ld", Prenum);
		അല്ലെങ്കിൽ
		    cbuf[0] = NUL;
#എങ്കിൽ defined(FEAT_QUICKFIX)
		എങ്കിൽ (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#endif
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		നിർത്തുക;

// quit current window
    അവസ്ഥ Ctrl_Q:
    അവസ്ഥ 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_എണ്ണം("quit", cbuf, വലുപ്പം(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		നിർത്തുക;

// close current window
    അവസ്ഥ Ctrl_C:
    അവസ്ഥ 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_എണ്ണം("close", cbuf, വലുപ്പം(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		നിർത്തുക;

#എങ്കിൽ defined(FEAT_QUICKFIX)
// close preview window
    അവസ്ഥ Ctrl_Z:
    അവസ്ഥ 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		നിർത്തുക;

// cursor to preview window
    അവസ്ഥ 'P':
		FOR_ALL_WINDOWS(wp)
		    എങ്കിൽ (wp->w_p_pvw)
			നിർത്തുക;
		എങ്കിൽ (wp == ശൂന്യ)
		    emsg(_(e_there_is_no_preview_window));
		അല്ലെങ്കിൽ
		    win_goto(wp);
		നിർത്തുക;
#endif

// close all but current window
    അവസ്ഥ Ctrl_O:
    അവസ്ഥ 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_എണ്ണം("only", cbuf, വലുപ്പം(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		നിർത്തുക;

// cursor to next window with wrap around
    അവസ്ഥ Ctrl_W:
    അവസ്ഥ 'w':
// cursor to previous window with wrap around
    അവസ്ഥ 'W':
		CHECK_CMDWIN;
		എങ്കിൽ (ONE_WINDOW && Prenum != 1)	// just one window
		    beep_flush();
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (Prenum)			// go to specified window
		    {
			വേണ്ടി (wp = firstwin; --Prenum > 0; )
			{
			    എങ്കിൽ (wp->w_next == ശൂന്യ)
				നിർത്തുക;
			    അല്ലെങ്കിൽ
				wp = wp->w_next;
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    എങ്കിൽ (wp == ശൂന്യ)
				wp = lastwin;	    // wrap around
			}
			അല്ലെങ്കിൽ			    // go to next window
			{
			    wp = curwin->w_next;
			    എങ്കിൽ (wp == ശൂന്യ)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		നിർത്തുക;

// cursor to window below
    അവസ്ഥ 'j':
    അവസ്ഥ K_DOWN:
    അവസ്ഥ Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		നിർത്തുക;

// cursor to window above
    അവസ്ഥ 'k':
    അവസ്ഥ K_UP:
    അവസ്ഥ Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		നിർത്തുക;

// cursor to left window
    അവസ്ഥ 'h':
    അവസ്ഥ K_ഇടത്:
    അവസ്ഥ Ctrl_H:
    അവസ്ഥ K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		നിർത്തുക;

// cursor to right window
    അവസ്ഥ 'l':
    അവസ്ഥ K_വലത്:
    അവസ്ഥ Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		നിർത്തുക;

// move window to new tab page
    അവസ്ഥ 'T':
		CHECK_CMDWIN;
		എങ്കിൽ (one_window())
		    സന്ദേശം(_(m_onlyone));
		അല്ലെങ്കിൽ
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    എങ്കിൽ (win_new_tabpage((പൂർണ്ണസംഖ്യ)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			എങ്കിൽ (curwin == wp)
			    win_close(curwin, FALSE);
			എങ്കിൽ (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		നിർത്തുക;

// cursor to top-left window
    അവസ്ഥ 't':
    അവസ്ഥ Ctrl_T:
		win_goto(firstwin);
		നിർത്തുക;

// cursor to bottom-right window
    അവസ്ഥ 'b':
    അവസ്ഥ Ctrl_B:
		win_goto(lastwin);
		നിർത്തുക;

// cursor to last accessed (previous) window
    അവസ്ഥ 'p':
    അവസ്ഥ Ctrl_P:
		എങ്കിൽ (!win_valid(prevwin))
		    beep_flush();
		അല്ലെങ്കിൽ
		    win_goto(prevwin);
		നിർത്തുക;

// exchange current and next window
    അവസ്ഥ 'x':
    അവസ്ഥ Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		നിർത്തുക;

// rotate windows downwards
    അവസ്ഥ Ctrl_R:
    അവസ്ഥ 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (പൂർണ്ണസംഖ്യ)Prenum1);    // downwards
		നിർത്തുക;

// rotate windows upwards
    അവസ്ഥ 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (പൂർണ്ണസംഖ്യ)Prenum1);	    // upwards
		നിർത്തുക;

// move window to the very top/bottom/left/right
    അവസ്ഥ 'K':
    അവസ്ഥ 'J':
    അവസ്ഥ 'H':
    അവസ്ഥ 'L':
		CHECK_CMDWIN;
		എങ്കിൽ (ONE_WINDOW)
		    beep_flush();
		അല്ലെങ്കിൽ
		{
		    പൂർണ്ണസംഖ്യ dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_മുകൾ : WSP_BOT);

		    (ശൂന്യം)win_splitmove(curwin, (പൂർണ്ണസംഖ്യ)Prenum, dir);
		}
		നിർത്തുക;

// make all windows the same width and/or height
    അവസ്ഥ '=':
		{
		    പൂർണ്ണസംഖ്യ mod = cmdmod.cmod_split & (WSP_VERT | WSP_HOR);
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_equal(ശൂന്യ, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		നിർത്തുക;

// increase current window height
    അവസ്ഥ '+':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height + (പൂർണ്ണസംഖ്യ)Prenum1);
		നിർത്തുക;

// decrease current window height
    അവസ്ഥ '-':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height - (പൂർണ്ണസംഖ്യ)Prenum1);
		നിർത്തുക;

// set current window height
    അവസ്ഥ Ctrl__:
    അവസ്ഥ '_':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(Prenum ? (പൂർണ്ണസംഖ്യ)Prenum : 9999);
		നിർത്തുക;

// increase current window width
    അവസ്ഥ '>':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width + (പൂർണ്ണസംഖ്യ)Prenum1);
		നിർത്തുക;

// decrease current window width
    അവസ്ഥ '<':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width - (പൂർണ്ണസംഖ്യ)Prenum1);
		നിർത്തുക;

// set current window width
    അവസ്ഥ '|':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(Prenum != 0 ? (പൂർണ്ണസംഖ്യ)Prenum : 9999);
		നിർത്തുക;

// jump to tag and split window if tag exists (in preview window)
#എങ്കിൽ defined(FEAT_QUICKFIX)
    അവസ്ഥ '}':
		CHECK_CMDWIN;
		എങ്കിൽ (Prenum)
		    g_do_tagpreview = Prenum;
		അല്ലെങ്കിൽ
		    g_do_tagpreview = p_pvh;
#endif
		// FALLTHROUGH
    അവസ്ഥ ']':
    അവസ്ഥ Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		എങ്കിൽ (Prenum)
		    postponed_split = Prenum;
		അല്ലെങ്കിൽ
		    postponed_split = -1;
#ifdef FEAT_QUICKFIX
		എങ്കിൽ (nchar != '}')
		    g_do_tagpreview = 0;
#endif

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_split = 0;
		നിർത്തുക;

// edit file name under cursor in a new window
    അവസ്ഥ 'f':
    അവസ്ഥ 'F':
    അവസ്ഥ Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		എങ്കിൽ (check_text_or_curbuf_locked(ശൂന്യ))
		    നിർത്തുക;

		ptr = grab_file_name(Prenum1, &lnum);
		എങ്കിൽ (ptr != ശൂന്യ)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = ശൂന്യ;
		    എങ്കിൽ ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    എങ്കിൽ (wp == ശൂന്യ && win_split(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			എങ്കിൽ (do_ecmd(0, ptr, ശൂന്യ, ശൂന്യ, ECMD_LASTL,
						   ECMD_HIDE, ശൂന്യ) == FAIL)
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
			അല്ലെങ്കിൽ
			    wp = curwin;
		    }

		    എങ്കിൽ (wp != ശൂന്യ && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		നിർത്തുക;

#ifdef FEAT_FIND_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    അവസ്ഥ 'i':			    // Go to any match
    അവസ്ഥ Ctrl_I:
		type = FIND_ANY;
		// FALLTHROUGH
    അവസ്ഥ 'd':			    // Go to definition, using 'define'
    അവസ്ഥ Ctrl_D:
		CHECK_CMDWIN;
		എങ്കിൽ ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
		    നിർത്തുക;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		എങ്കിൽ (ptr == ശൂന്യ)
		    നിർത്തുക;

		find_pattern_in_path(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_SPLIT, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_set_curswant = true;
		നിർത്തുക;
#endif

// Quickfix window only: view the result under the cursor in a new split.
#എങ്കിൽ defined(FEAT_QUICKFIX)
    അവസ്ഥ K_KENTER:
    അവസ്ഥ CAR:
		എങ്കിൽ (bt_quickfix(curbuf))
		    qf_view_result(TRUE);
		നിർത്തുക;
#endif

// CTRL-W g  extended commands
    അവസ്ഥ 'g':
    അവസ്ഥ Ctrl_G:
		CHECK_CMDWIN;
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#endif
		++no_mapping;
		++allow_താക്കോലുകൾ;   // no mapping for xchar, but allow key codes
		എങ്കിൽ (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_താക്കോലുകൾ;
		(ശൂന്യം)add_to_showcmd(xchar);

		തിരഞ്ഞെടുപ്പ് (xchar)
		{
#എങ്കിൽ defined(FEAT_QUICKFIX)
		    അവസ്ഥ '}':
			xchar = Ctrl_RSB;
			എങ്കിൽ (Prenum)
			    g_do_tagpreview = Prenum;
			അല്ലെങ്കിൽ
			    g_do_tagpreview = p_pvh;
#endif
			// FALLTHROUGH
		    അവസ്ഥ ']':
		    അവസ്ഥ Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			എങ്കിൽ (Prenum)
			    postponed_split = Prenum;
			അല്ലെങ്കിൽ
			    postponed_split = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_split = 0;
			നിർത്തുക;

		    അവസ്ഥ 'f':	    // CTRL-W gf: "gf" in a new tab page
		    അവസ്ഥ 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_index(curtab) + 1;
			nchar = xchar;
			പോകുക wingotofile;

		    അവസ്ഥ 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((പൂർണ്ണസംഖ്യ)Prenum);
			നിർത്തുക;

		    അവസ്ഥ 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(പൂർണ്ണസംഖ്യ)Prenum1);
			നിർത്തുക;

		    അവസ്ഥ TAB:	    // CTRL-W g<Tab>: go to last used tab page
			എങ്കിൽ (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			നിർത്തുക;

		    സ്ഥിരസ്ഥിതി:
			beep_flush();
			നിർത്തുക;
		}
		നിർത്തുക;

    സ്ഥിരസ്ഥിതി:	beep_flush();
		നിർത്തുക;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    ശൂന്യം
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    തിരഞ്ഞെടുപ്പ് (*arg)
    {
    അവസ്ഥ 'S':
    അവസ്ഥ Ctrl_S:
    അവസ്ഥ 's':
    അവസ്ഥ Ctrl_N:
    അവസ്ഥ 'n':
    അവസ്ഥ 'j':
    അവസ്ഥ Ctrl_J:
    അവസ്ഥ 'k':
    അവസ്ഥ Ctrl_K:
    അവസ്ഥ 'T':
    അവസ്ഥ Ctrl_R:
    അവസ്ഥ 'r':
    അവസ്ഥ 'R':
    അവസ്ഥ 'K':
    അവസ്ഥ 'J':
    അവസ്ഥ '+':
    അവസ്ഥ '-':
    അവസ്ഥ Ctrl__:
    അവസ്ഥ '_':
    അവസ്ഥ '|':
    അവസ്ഥ ']':
    അവസ്ഥ Ctrl_RSB:
    അവസ്ഥ 'g':
    അവസ്ഥ Ctrl_G:
    അവസ്ഥ Ctrl_V:
    അവസ്ഥ 'v':
    അവസ്ഥ 'h':
    അവസ്ഥ Ctrl_H:
    അവസ്ഥ 'l':
    അവസ്ഥ Ctrl_L:
    അവസ്ഥ 'H':
    അവസ്ഥ 'L':
    അവസ്ഥ '>':
    അവസ്ഥ '<':
#എങ്കിൽ defined(FEAT_QUICKFIX)
    അവസ്ഥ '}':
#endif
    അവസ്ഥ 'f':
    അവസ്ഥ 'F':
    അവസ്ഥ Ctrl_F:
#ifdef FEAT_FIND_ID
    അവസ്ഥ 'i':
    അവസ്ഥ Ctrl_I:
    അവസ്ഥ 'd':
    അവസ്ഥ Ctrl_D:
#endif
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		നിർത്തുക;

    അവസ്ഥ Ctrl_HAT:
    അവസ്ഥ '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		നിർത്തുക;

    അവസ്ഥ Ctrl_Q:
    അവസ്ഥ 'q':
    അവസ്ഥ Ctrl_C:
    അവസ്ഥ 'c':
    അവസ്ഥ Ctrl_O:
    അവസ്ഥ 'o':
    അവസ്ഥ Ctrl_W:
    അവസ്ഥ 'w':
    അവസ്ഥ 'W':
    അവസ്ഥ 'x':
    അവസ്ഥ Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		നിർത്തുക;

#എങ്കിൽ defined(FEAT_QUICKFIX)
    അവസ്ഥ Ctrl_Z:
    അവസ്ഥ 'z':
    അവസ്ഥ 'P':
#endif
    അവസ്ഥ 't':
    അവസ്ഥ Ctrl_T:
    അവസ്ഥ 'b':
    അവസ്ഥ Ctrl_B:
    അവസ്ഥ 'p':
    അവസ്ഥ Ctrl_P:
    അവസ്ഥ '=':
    അവസ്ഥ CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		നിർത്തുക;
    }
}

    സ്ഥിതം ശൂന്യം
cmd_with_എണ്ണം(
    അക്ഷരം	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    long	Prenum)
{
    എങ്കിൽ (Prenum > 0)
	vim_snprintf((അക്ഷരം *)bufp, bufsize, "%s %ld", cmd, Prenum);
    അല്ലെങ്കിൽ
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    പൂർണ്ണസംഖ്യ
check_split_disallowed(win_T *wp)
{
    എങ്കിൽ (split_disallowed > 0)
    {
	emsg(_(e_cant_split_window_while_closing_another));
	തിരികെ FAIL;
    }
    എങ്കിൽ (wp->w_buffer->b_locked_split)
    {
	emsg(_(e_cannot_split_window_when_closing_buffer));
	തിരികെ FAIL;
    }
    തിരികെ OK;
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
    പൂർണ്ണസംഖ്യ
win_split(പൂർണ്ണസംഖ്യ size, പൂർണ്ണസംഖ്യ flags)
{
    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ FAIL;

    എങ്കിൽ (check_split_disallowed(curwin) == FAIL)
	തിരികെ FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    എങ്കിൽ (may_open_tabpage() == OK)
	തിരികെ OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_split;
    എങ്കിൽ ((flags & WSP_മുകൾ) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_split_topleft_and_botright_at_the_same_time));
	തിരികെ FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    എങ്കിൽ (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    അല്ലെങ്കിൽ
	clear_snapshot(curtab, SNAP_HELP_IDX);

    എങ്കിൽ (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    അല്ലെങ്കിൽ
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    തിരികെ win_split_ins(size, flags, ശൂന്യ, 0, ശൂന്യ);
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
    പൂർണ്ണസംഖ്യ
win_split_ins(
    പൂർണ്ണസംഖ്യ		size,
    പൂർണ്ണസംഖ്യ		flags,
    win_T	*new_wp,
    പൂർണ്ണസംഖ്യ		dir,
    frame_T	*to_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    പൂർണ്ണസംഖ്യ		new_size = size;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		need_status = 0;
    പൂർണ്ണസംഖ്യ		do_equal = FALSE;
    പൂർണ്ണസംഖ്യ		needed;
    പൂർണ്ണസംഖ്യ		available;
    പൂർണ്ണസംഖ്യ		oldwin_height = 0;
    പൂർണ്ണസംഖ്യ		layout;
    frame_T	*frp, *curfrp, *frp2, *prevfrp;
    പൂർണ്ണസംഖ്യ		before;
    പൂർണ്ണസംഖ്യ		minheight;
    പൂർണ്ണസംഖ്യ		wmh1;
    പൂർണ്ണസംഖ്യ		did_set_fraction = FALSE;
    പൂർണ്ണസംഖ്യ		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    എങ്കിൽ (new_wp == ശൂന്യ)
	trigger_winnewpre();

    എങ്കിൽ (flags & WSP_മുകൾ)
	oldwin = firstwin;
    അല്ലെങ്കിൽ എങ്കിൽ (flags & WSP_BOT)
	oldwin = lastwin;
    അല്ലെങ്കിൽ
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    എങ്കിൽ (ONE_WINDOW && p_ls == 1 && oldwin->w_status_height == 0)
    {
	എങ്കിൽ (!(flags & WSP_FORCE_ROOM) && VISIBLE_HEIGHT(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_room));
	    പോകുക theend;
	}
	need_status = statusline_height(oldwin);
    }

#ifdef FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    എങ്കിൽ (gui.in_use)
	out_flush();
#endif

    എങ്കിൽ (flags & WSP_VERT)
    {
	പൂർണ്ണസംഖ്യ	wmw1;
	പൂർണ്ണസംഖ്യ	minwidth;

	layout = FR_ROW;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	എങ്കിൽ (flags & WSP_ROOM)
	    needed += p_wiw - wmw1;
	എങ്കിൽ (flags & (WSP_BOT | WSP_മുകൾ))
	{
	    minwidth = frame_minwidth(topframe, NOWIN);
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p_ea)
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    prevfrp = oldwin->w_frame;
	    വേണ്ടി (frp = oldwin->w_frame->fr_parent; frp != ശൂന്യ;
							frp = frp->fr_parent)
	    {
		എങ്കിൽ (frp->fr_layout == FR_ROW)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			എങ്കിൽ (frp2 != prevfrp)
			    minwidth += frame_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	അല്ലെങ്കിൽ
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    available = oldwin->w_frame->fr_width;
	    needed += minwidth;
	}
	എങ്കിൽ (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    പോകുക theend;
	}
	എങ്കിൽ (new_size == 0)
	    new_size = oldwin->w_width / 2;
	എങ്കിൽ (new_size > available - minwidth - 1)
	    new_size = available - minwidth - 1;
	എങ്കിൽ (new_size < wmw1)
	    new_size = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	എങ്കിൽ (oldwin->w_width - new_size - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	എങ്കിൽ (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_width + new_size + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	എങ്കിൽ (!do_equal && p_ea && size == 0 && *p_ead != 'v'
	    && oldwin->w_frame->fr_parent != ശൂന്യ)
	    do_equal = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	layout = FR_COL;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_HEIGHT(curwin);
	needed = wmh1 + statusline_height(oldwin);
	എങ്കിൽ (flags & WSP_ROOM)
	    needed += p_wh - wmh1;
	എങ്കിൽ (flags & (WSP_BOT | WSP_മുകൾ))
	{
	    minheight = frame_minheight(topframe, NOWIN) + need_status;
	    available = topframe->fr_height;
	    needed += minheight;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p_ea)
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    prevfrp = oldwin->w_frame;
	    വേണ്ടി (frp = oldwin->w_frame->fr_parent; frp != ശൂന്യ;
							frp = frp->fr_parent)
	    {
		എങ്കിൽ (frp->fr_layout == FR_COL)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			എങ്കിൽ (frp2 != prevfrp)
			    minheight += frame_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_height;
	    needed += minheight;
	}
	അല്ലെങ്കിൽ
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    available = oldwin->w_frame->fr_height;
	    needed += minheight;
	}
	എങ്കിൽ (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    പോകുക theend;
	}
	oldwin_height = oldwin->w_height;
	എങ്കിൽ (need_status)
	{
	    oldwin->w_status_height = statusline_height(oldwin);
	    oldwin_height -= oldwin->w_status_height;
	}
	// Estimated stl height for the new window.
	പൂർണ്ണസംഖ്യ new_win_stlh = need_status != 0 ? need_status
			: statusline_height(new_wp != ശൂന്യ ? new_wp : curwin);
	എങ്കിൽ (new_size == 0)
	    new_size = (oldwin_height - new_win_stlh + 1) / 2;
	എങ്കിൽ (new_size > available - minheight - new_win_stlh)
	    new_size = available - minheight - new_win_stlh;
	എങ്കിൽ (new_size < wmh1)
	    new_size = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	എങ്കിൽ (oldwin_height - new_size - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	എങ്കിൽ (oldwin->w_p_wfh)
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
	എങ്കിൽ (!do_equal && p_ea && size == 0 && *p_ead != 'h'
	    && oldwin->w_frame->fr_parent != ശൂന്യ)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    എങ്കിൽ ((flags & WSP_മുകൾ) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_BELOW)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	എങ്കിൽ (new_wp == ശൂന്യ)
	    wp = win_alloc(oldwin, FALSE);
	അല്ലെങ്കിൽ
	    win_append(oldwin, wp);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (new_wp == ശൂന്യ)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	അല്ലെങ്കിൽ
	    win_append(oldwin->w_prev, wp);
    }

    എങ്കിൽ (new_wp == ശൂന്യ)
    {
	എങ്കിൽ (wp == ശൂന്യ)
	    പോകുക theend;

	new_frame(wp);
	എങ്കിൽ (wp->w_frame == ശൂന്യ)
	{
	    win_free(wp, ശൂന്യ);
	    പോകുക theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    എങ്കിൽ (to_flatten != ശൂന്യ)
	frame_flatten(to_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    എങ്കിൽ (flags & (WSP_മുകൾ | WSP_BOT))
    {
	എങ്കിൽ ((topframe->fr_layout == FR_COL && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ROW && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_child;
	    എങ്കിൽ (flags & WSP_BOT)
		ആകുമ്പോൾ (curfrp->fr_next != ശൂന്യ)
		    curfrp = curfrp->fr_next;
	}
	അല്ലെങ്കിൽ
	    curfrp = topframe;
	before = (flags & WSP_മുകൾ);
    }
    അല്ലെങ്കിൽ
    {
	curfrp = oldwin->w_frame;
	എങ്കിൽ (flags & WSP_BELOW)
	    before = FALSE;
	അല്ലെങ്കിൽ എങ്കിൽ (flags & WSP_ABOVE)
	    before = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (flags & WSP_VERT)
	    before = !p_spr;
	അല്ലെങ്കിൽ
	    before = !p_sb;
    }
    എങ്കിൽ (curfrp->fr_parent == ശൂന്യ || curfrp->fr_parent->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(frame_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_parent = curfrp;
	frp->fr_next = ശൂന്യ;
	frp->fr_prev = ശൂന്യ;
	curfrp->fr_child = frp;
	curfrp->fr_win = ശൂന്യ;
	curfrp = frp;
	എങ്കിൽ (frp->fr_win != ശൂന്യ)
	    oldwin->w_frame = frp;
	അല്ലെങ്കിൽ
	    FOR_ALL_FRAMES(frp, frp->fr_child)
		frp->fr_parent = curfrp;
    }

    എങ്കിൽ (new_wp == ശൂന്യ)
	frp = wp->w_frame;
    അല്ലെങ്കിൽ
	frp = new_wp->w_frame;
    frp->fr_parent = curfrp->fr_parent;

    // Insert the new frame at the right place in the frame list.
    എങ്കിൽ (before)
	frame_തിരുകുക(curfrp, frp);
    അല്ലെങ്കിൽ
	frame_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    എങ്കിൽ (!did_set_fraction)
	set_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    എങ്കിൽ (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	എങ്കിൽ (need_status)
	{
	    win_new_height(oldwin, oldwin->w_height - need_status);
	    oldwin->w_status_height = need_status;
	}
	എങ്കിൽ (flags & (WSP_മുകൾ | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    പൂർണ്ണസംഖ്യ stl_height = p_ls > 0 ? statusline_height(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_height();
	    win_new_height(wp, curfrp->fr_height - stl_height
							  - WINBAR_HEIGHT(wp));
	    wp->w_status_height = stl_height;
	}
	അല്ലെങ്കിൽ
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
	എങ്കിൽ (before)
	    wp->w_vsep_width = 1;
	അല്ലെങ്കിൽ
	{
	    wp->w_vsep_width = oldwin->w_vsep_width;
	    oldwin->w_vsep_width = 1;
	}
	എങ്കിൽ (flags & (WSP_മുകൾ | WSP_BOT))
	{
	    എങ്കിൽ (flags & WSP_BOT)
		frame_set_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    frame_new_width(curfrp, curfrp->fr_width
		     - (new_size + ((flags & WSP_മുകൾ) != 0)), flags & WSP_മുകൾ,
								       FALSE);
	}
	അല്ലെങ്കിൽ
	    win_new_width(oldwin, oldwin->w_width - (new_size + 1));
	എങ്കിൽ (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_size + 1;
	}
	അല്ലെങ്കിൽ		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_width + 1;
	frame_fix_width(oldwin);
	frame_fix_width(wp);
    }
    അല്ലെങ്കിൽ
    {
	// width and column of new window is same as current window
	എങ്കിൽ (flags & (WSP_മുകൾ | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_width(wp, topframe->fr_width);
	    wp->w_vsep_width = 0;
	}
	അല്ലെങ്കിൽ
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_width(wp, oldwin->w_width);
	    wp->w_vsep_width = oldwin->w_vsep_width;
	}
	frp->fr_width = curfrp->fr_width;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_height(wp, new_size);
	പൂർണ്ണസംഖ്യ old_status_height = oldwin->w_status_height;
	എങ്കിൽ (flags & (WSP_മുകൾ | WSP_BOT))
	{
	    പൂർണ്ണസംഖ്യ new_fr_height = curfrp->fr_height - new_size
							  + WINBAR_HEIGHT(wp);

	    എങ്കിൽ (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_height -= statusline_height(curfrp->fr_win);
	    എങ്കിൽ (flags & WSP_BOT)
		frame_add_statusline(curfrp);
	    frame_new_height(curfrp, new_fr_height, flags & WSP_മുകൾ, FALSE,
									FALSE);
	}
	അല്ലെങ്കിൽ
	    win_new_height(oldwin, oldwin_height - (new_size
			+ statusline_height(wp)));
	എങ്കിൽ (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_status_height = statusline_height(wp);
	    oldwin->w_winrow += wp->w_height + statusline_height(wp);
	}
	അല്ലെങ്കിൽ		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_HEIGHT(oldwin)
						    + statusline_height(oldwin);
	    wp->w_status_height = old_status_height;
	    എങ്കിൽ (!(flags & WSP_BOT))
		oldwin->w_status_height = statusline_height(oldwin);
	}
	frame_fix_height(wp);
	frame_fix_height(oldwin);
    }

    എങ്കിൽ (flags & (WSP_മുകൾ | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    status_redraw_all();

    എങ്കിൽ (need_status)
    {
	സന്ദേശം_row = Rows - 1;
	സന്ദേശം_col = sc_col;
	സന്ദേശം_clr_eos_force();	// Old command/ruler may still be there
	comp_col();
	സന്ദേശം_row = Rows - 1;
	സന്ദേശം_col = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    എങ്കിൽ (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    അല്ലെങ്കിൽ എങ്കിൽ (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    എങ്കിൽ (flags & WSP_VERT)
    {
	i = p_wiw;
	എങ്കിൽ (size != 0)
	    p_wiw = size;

#ifdef FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	എങ്കിൽ (gui.in_use)
	    gui_init_which_components(ശൂന്യ);
#endif
    }
    അല്ലെങ്കിൽ
    {
	i = p_wh;
	എങ്കിൽ (size != 0)
	    p_wh = size;
    }

    /*
     * make the new window the current window
     */
    (ശൂന്യം)win_enter_ext(wp, (new_wp == ശൂന്യ ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    എങ്കിൽ (flags & WSP_VERT)
	p_wiw = i;
    അല്ലെങ്കിൽ
	p_wh = i;
    retval = OK;

theend:
    എങ്കിൽ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    തിരികെ retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    സ്ഥിതം ശൂന്യം
win_init(win_T *newp, win_T *oldp, പൂർണ്ണസംഖ്യ flags UNUSED)
{
    പൂർണ്ണസംഖ്യ		i;

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
    എങ്കിൽ (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = ശൂന്യ;
	newp->w_llist_ref = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
	copy_loclist_അട്ടി(oldp, newp);
#endif
    newp->w_localdir = (oldp->w_localdir == ശൂന്യ)
				    ? ശൂന്യ : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == ശൂന്യ)
				    ? ശൂന്യ : vim_strsave(oldp->w_prevdir);

    എങ്കിൽ (*p_spk != 'c')
    {
	എങ്കിൽ (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_height = oldp->w_height - WINBAR_HEIGHT(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_HEIGHT(oldp);
    }

    // copy tagstack and folds
    വേണ്ടി (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*tag = &newp->w_tagstack[i];
	*tag = oldp->w_tagstack[i];
	എങ്കിൽ (tag->tagname != ശൂന്യ)
	    tag->tagname = vim_strsave(tag->tagname);
	എങ്കിൽ (tag->user_data != ശൂന്യ)
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
	bool pushed = തള്ളുക_highlight_overrides(newp->w_hl, newp->w_hl_len);
	എങ്കിൽ (newp->w_buffer->b_term != ശൂന്യ)
	    term_init_default_colors(newp->w_buffer->b_term);
	എങ്കിൽ (pushed)
	    എടുക്കുക_highlight_overrides();
    }
#endif
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    സ്ഥിതം ശൂന്യം
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
    പൂർണ്ണസംഖ്യ
win_valid_popup(win_T *win UNUSED)
{
#ifdef FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	എങ്കിൽ (wp == win)
	    തിരികെ TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	എങ്കിൽ (wp == win)
	    തിരികെ TRUE;
#endif
    തിരികെ FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    പൂർണ്ണസംഖ്യ
win_valid(win_T *win)
{
    win_T	*wp;

    എങ്കിൽ (win == ശൂന്യ)
	തിരികെ FALSE;
    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (wp == win)
	    തിരികെ TRUE;
    തിരികെ win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_find_by_id(പൂർണ്ണസംഖ്യ id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (wp->w_id == id)
	    തിരികെ wp;
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	എങ്കിൽ (wp->w_id == id)
	    തിരികെ wp;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	എങ്കിൽ (wp->w_id == id)
	    തിരികെ wp;
#endif
    തിരികെ ശൂന്യ;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    പൂർണ്ണസംഖ്യ
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    എങ്കിൽ (win == ശൂന്യ)
	തിരികെ FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    എങ്കിൽ (wp == win)
		തിരികെ TRUE;
	}
#ifdef FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    എങ്കിൽ (wp == win)
		തിരികെ TRUE;
#endif
    }
    തിരികെ win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    പൂർണ്ണസംഖ്യ
win_എണ്ണം(ശൂന്യം)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;

    FOR_ALL_WINDOWS(wp)
	++എണ്ണം;
    തിരികെ എണ്ണം;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    പൂർണ്ണസംഖ്യ
make_windows(
    പൂർണ്ണസംഖ്യ		എണ്ണം,
    പൂർണ്ണസംഖ്യ		vertical)  // split windows vertically if TRUE
{
    പൂർണ്ണസംഖ്യ		maxcount;
    പൂർണ്ണസംഖ്യ		todo;

    എങ്കിൽ (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_width + curwin->w_vsep_width
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    അല്ലെങ്കിൽ
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_HEIGHT(curwin) + curwin->w_status_height
			- (p_wh - p_wmh)) / (p_wmh + statusline_height(curwin));
    }

    എങ്കിൽ (maxcount < 2)
	maxcount = 2;
    എങ്കിൽ (എണ്ണം > maxcount)
	എണ്ണം = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    എങ്കിൽ (എണ്ണം > 1)
	last_status(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    // todo is number of windows left to create
    വേണ്ടി (todo = എണ്ണം - 1; todo > 0; --todo)
	എങ്കിൽ (vertical)
	{
	    എങ്കിൽ (win_split(curwin->w_width - (curwin->w_width - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (win_split(curwin->w_height - (curwin->w_height - todo
			    * statusline_height(curwin)) / (todo + 1)
			- statusline_height(curwin), WSP_ABOVE) == FAIL)
		നിർത്തുക;
	}

    unblock_autocmds();

    // return actual number of windows
    തിരികെ (എണ്ണം - todo);
}

/*
 * Exchange current and next window
 */
    സ്ഥിതം ശൂന്യം
win_exchange(long Prenum)
{
    frame_T	*frp;
    frame_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    പൂർണ്ണസംഖ്യ		temp;

    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;
    എങ്കിൽ (ONE_WINDOW)	    // just one window
    {
	beep_flush();
	തിരികെ;
    }
    എങ്കിൽ (text_or_buf_locked())
    {
	beep_flush();
	തിരികെ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * find window to exchange with
     */
    എങ്കിൽ (Prenum)
    {
	frp = curwin->w_frame->fr_parent->fr_child;
	ആകുമ്പോൾ (frp != ശൂന്യ && --Prenum > 0)
	    frp = frp->fr_next;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_frame->fr_next != ശൂന്യ)	// Swap with next
	frp = curwin->w_frame->fr_next;
    അല്ലെങ്കിൽ    // Swap last window in row/col with previous
	frp = curwin->w_frame->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    എങ്കിൽ (frp == ശൂന്യ || frp->fr_win == ശൂന്യ || frp->fr_win == curwin)
	തിരികെ;
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
    എങ്കിൽ (wp->w_prev != curwin)
    {
	win_remove(curwin, ശൂന്യ);
	frame_remove(curwin->w_frame);
	win_append(wp->w_prev, curwin);
	frame_തിരുകുക(frp, curwin->w_frame);
    }
    എങ്കിൽ (wp != wp2)
    {
	win_remove(wp, ശൂന്യ);
	frame_remove(wp->w_frame);
	win_append(wp2, wp);
	എങ്കിൽ (frp2 == ശൂന്യ)
	    frame_തിരുകുക(wp->w_frame->fr_parent->fr_child, wp->w_frame);
	അല്ലെങ്കിൽ
	    frame_append(frp2, wp->w_frame);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    temp = curwin->w_status_height;
    എങ്കിൽ (temp != wp->w_status_height)
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
#എങ്കിൽ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    എങ്കിൽ (wp->w_buffer != curbuf)
	reset_VIsual_and_resel();
    അല്ലെങ്കിൽ എങ്കിൽ (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    സ്ഥിതം ശൂന്യം
win_rotate(പൂർണ്ണസംഖ്യ upwards, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    win_T	*wp1;
    win_T	*wp2;
    frame_T	*frp;
    പൂർണ്ണസംഖ്യ		n;

    എങ്കിൽ (ONE_WINDOW)		// nothing to do
    {
	beep_flush();
	തിരികെ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Check if all frames in this row/col have one window.
    FOR_ALL_FRAMES(frp, curwin->w_frame->fr_parent->fr_child)
	എങ്കിൽ (frp->fr_win == ശൂന്യ)
	{
	    emsg(_(e_cannot_rotate_when_another_window_is_split));
	    തിരികെ;
	}

    ആകുമ്പോൾ (എണ്ണം--)
    {
	എങ്കിൽ (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_frame->fr_parent->fr_child;
	    wp1 = frp->fr_win;
	    win_remove(wp1, ശൂന്യ);
	    frame_remove(frp);

	    // find last frame and append removed window/frame after it
	    വേണ്ടി ( ; frp->fr_next != ശൂന്യ; frp = frp->fr_next)
		;
	    win_append(frp->fr_win, wp1);
	    frame_append(frp, wp1->w_frame);

	    wp2 = frp->fr_win;		// previously last window
	}
	അല്ലെങ്കിൽ			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    വേണ്ടി (frp = curwin->w_frame; frp->fr_next != ശൂന്യ;
							   frp = frp->fr_next)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, ശൂന്യ);
	    frame_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_parent->fr_child->fr_win->w_prev, wp1);
	    frame_തിരുകുക(frp->fr_parent->fr_child, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_status_height;
	എങ്കിൽ (n != wp1->w_status_height)
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
#എങ്കിൽ defined(FEAT_STL_OPT)
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
    പൂർണ്ണസംഖ്യ
win_splitmove(win_T *wp, പൂർണ്ണസംഖ്യ size, പൂർണ്ണസംഖ്യ flags)
{
    പൂർണ്ണസംഖ്യ		dir;
    പൂർണ്ണസംഖ്യ		height = wp->w_height;
    frame_T	*unflat_altfr;

    എങ്കിൽ (ONE_WINDOW)
	തിരികെ OK;	// nothing to do
    എങ്കിൽ (check_split_disallowed(wp) == FAIL)
	തിരികെ FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, ശൂന്യ, &unflat_altfr);
    win_remove(wp, ശൂന്യ);
    last_status(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    എങ്കിൽ (win_split_ins(size, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	തിരികെ FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    എങ്കിൽ (size == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(height, wp);
	എങ്കിൽ (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#എങ്കിൽ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif
    തിരികെ OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    ശൂന്യം
win_move_after(win_T *win1, win_T *win2)
{
    പൂർണ്ണസംഖ്യ		height;

    // check if the arguments are reasonable
    എങ്കിൽ (win1 == win2)
	തിരികെ;

    // check if there is something to do
    എങ്കിൽ (win2->w_next != win1)
    {
	എങ്കിൽ (win1->w_frame->fr_parent != win2->w_frame->fr_parent)
	{
	    iemsg("Trying to move a window into another frame");
	    തിരികെ;
	}

	// may need to move the status line/vertical separator of the last
	// window
	എങ്കിൽ (win1 == lastwin)
	{
	    height = win1->w_prev->w_status_height;
	    win1->w_prev->w_status_height = win1->w_status_height;
	    win1->w_status_height = height;
	    എങ്കിൽ (win1->w_prev->w_vsep_width == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_width = 0;
		win1->w_prev->w_frame->fr_width -= 1;
		win1->w_vsep_width = 1;
		win1->w_frame->fr_width += 1;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (win2 == lastwin)
	{
	    height = win1->w_status_height;
	    win1->w_status_height = win2->w_status_height;
	    win2->w_status_height = height;
	    എങ്കിൽ (win1->w_vsep_width == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_width = 1;
		win2->w_frame->fr_width += 1;
		win1->w_vsep_width = 0;
		win1->w_frame->fr_width -= 1;
	    }
	}
	win_remove(win1, ശൂന്യ);
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
    ശൂന്യം
win_equal(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    പൂർണ്ണസംഖ്യ		current,	// do only frame with current window
    പൂർണ്ണസംഖ്യ		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    എങ്കിൽ (dir == 0)
	dir = *p_ead;
    win_equal_rec(next_curwin == ശൂന്യ ? curwin : next_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_height(),
		      topframe->fr_width, topframe->fr_height);
#എങ്കിൽ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
    എങ്കിൽ (!is_aucmd_win(next_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    സ്ഥിതം ശൂന്യം
win_equal_rec(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    പൂർണ്ണസംഖ്യ		current,	// do only frame with current window
    frame_T	*topfr,		// frame to set size off
    പൂർണ്ണസംഖ്യ		dir,		// 'v', 'h' or 'b', see win_equal()
    പൂർണ്ണസംഖ്യ		col,		// horizontal position for frame
    പൂർണ്ണസംഖ്യ		row,		// vertical position for frame
    പൂർണ്ണസംഖ്യ		width,		// new width of frame
    പൂർണ്ണസംഖ്യ		height)		// new height of frame
{
    പൂർണ്ണസംഖ്യ		n, m;
    പൂർണ്ണസംഖ്യ		extra_sep = 0;
    പൂർണ്ണസംഖ്യ		wincount, totwincount = 0;
    frame_T	*fr;
    പൂർണ്ണസംഖ്യ		next_curwin_size = 0;
    പൂർണ്ണസംഖ്യ		room = 0;
    പൂർണ്ണസംഖ്യ		new_size;
    പൂർണ്ണസംഖ്യ		has_next_curwin = 0;
    പൂർണ്ണസംഖ്യ		hnc;

    എങ്കിൽ (topfr->fr_layout == FR_LEAF)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	എങ്കിൽ (topfr->fr_height != height || topfr->fr_win->w_winrow != row
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
    അല്ലെങ്കിൽ എങ്കിൽ (topfr->fr_layout == FR_ROW)
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	എങ്കിൽ (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = frame_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    എങ്കിൽ (col + width >= firstwin->w_wincol + topframe->fr_width)
		extra_sep = 1;
	    അല്ലെങ്കിൽ
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
	    എങ്കിൽ (room < 0)
	    {
		next_curwin_size = p_wiw + room;
		room = 0;
	    }
	    അല്ലെങ്കിൽ
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    എങ്കിൽ (!frame_fixed_width(fr))
			തുടരുക;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minwidth(fr, NOWIN);
		    new_size = fr->fr_width;
		    എങ്കിൽ (frame_has_win(fr, next_curwin))
		    {
			room += p_wiw - p_wmw;
			next_curwin_size = 0;
			എങ്കിൽ (new_size < p_wiw)
			    new_size = p_wiw;
		    }
		    അല്ലെങ്കിൽ
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_next == ശൂന്യ
					? extra_sep : 0)) / (p_wmw + 1);
		    room -= new_size - n;
		    എങ്കിൽ (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newwidth = new_size;
		}
		എങ്കിൽ (next_curwin_size == -1)
		{
		    എങ്കിൽ (!has_next_curwin)
			next_curwin_size = 0;
		    അല്ലെങ്കിൽ എങ്കിൽ (totwincount > 1
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
		    അല്ലെങ്കിൽ
			next_curwin_size = p_wiw;
		}
	    }

	    എങ്കിൽ (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    എങ്കിൽ (fr->fr_next == ശൂന്യ)
		// last frame gets all that remains (avoid roundoff error)
		new_size = width;
	    അല്ലെങ്കിൽ എങ്കിൽ (dir == 'v')
		new_size = fr->fr_width;
	    അല്ലെങ്കിൽ എങ്കിൽ (frame_fixed_width(fr))
	    {
		new_size = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    അല്ലെങ്കിൽ
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = frame_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_next == ശൂന്യ ? extra_sep : 0))
								/ (p_wmw + 1);
		m = frame_minwidth(fr, next_curwin);
		എങ്കിൽ (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		അല്ലെങ്കിൽ
		    hnc = FALSE;
		എങ്കിൽ (hnc)	    // don't count next_curwin
		    --wincount;
		എങ്കിൽ (totwincount == 0)
		    new_size = room;
		അല്ലെങ്കിൽ
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		എങ്കിൽ (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wiw - (m - n);
		    എങ്കിൽ (next_curwin_size < 0)
			next_curwin_size = 0;
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		അല്ലെങ്കിൽ
		    room -= new_size;
		new_size += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    എങ്കിൽ (!current || dir != 'v' || topfr->fr_parent != ശൂന്യ
		    || (new_size != fr->fr_width)
		    || frame_has_win(fr, next_curwin))
		win_equal_rec(next_curwin, current, fr, dir, col, row,
							    new_size, height);
	    col += new_size;
	    width -= new_size;
	    totwincount -= wincount;
	}
    }
    അല്ലെങ്കിൽ // topfr->fr_layout == FR_COL
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	എങ്കിൽ (dir != 'h')			// equalize frame heights
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
	    എങ്കിൽ (room < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		next_curwin_size = p_wh + room;
		room = 0;
	    }
	    അല്ലെങ്കിൽ
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    എങ്കിൽ (!frame_fixed_height(fr))
			തുടരുക;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minheight(fr, NOWIN);
		    new_size = fr->fr_height;
		    എങ്കിൽ (frame_has_win(fr, next_curwin))
		    {
			room += p_wh - p_wmh;
			next_curwin_size = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			എങ്കിൽ (new_size < n + (p_wh - p_wmh))
			    new_size = n + (p_wh - p_wmh);
		    }
		    അല്ലെങ്കിൽ
			// These windows don't use up room.
			totwincount -= frame_wincount_in_height(fr);
		    room -= new_size - n;
		    എങ്കിൽ (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newheight = new_size;
		}
		എങ്കിൽ (next_curwin_size == -1)
		{
		    എങ്കിൽ (!has_next_curwin)
			next_curwin_size = 0;
		    അല്ലെങ്കിൽ എങ്കിൽ (totwincount > 1
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
		    അല്ലെങ്കിൽ
			next_curwin_size = p_wh;
		}
	    }

	    എങ്കിൽ (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    എങ്കിൽ (fr->fr_next == ശൂന്യ)
		// last frame gets all that remains (avoid roundoff error)
		new_size = height;
	    അല്ലെങ്കിൽ എങ്കിൽ (dir == 'h')
		new_size = fr->fr_height;
	    അല്ലെങ്കിൽ എങ്കിൽ (frame_fixed_height(fr))
	    {
		new_size = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    അല്ലെങ്കിൽ
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = frame_minheight(fr, NOWIN);
		wincount = frame_wincount_in_height(fr);
		m = frame_minheight(fr, next_curwin);
		എങ്കിൽ (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		അല്ലെങ്കിൽ
		    hnc = FALSE;
		എങ്കിൽ (hnc)	    // don't count next_curwin
		    --wincount;
		എങ്കിൽ (totwincount == 0)
		    new_size = room;
		അല്ലെങ്കിൽ
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		എങ്കിൽ (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wh - (m - n);
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		അല്ലെങ്കിൽ
		    room -= new_size;
		new_size += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    എങ്കിൽ (!current || dir != 'h' || topfr->fr_parent != ശൂന്യ
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

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
    ശൂന്യം
leaving_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    എങ്കിൽ (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	തിരികെ;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_buffer->b_prompt_തിരുകുക = restart_edit;
    എങ്കിൽ (restart_edit != 0 && mode_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    എങ്കിൽ ((State & MODE_തിരുകുക) && !stop_തിരുകുക_mode)
    {
	stop_തിരുകുക_mode = TRUE;
	എങ്കിൽ (win->w_buffer->b_prompt_തിരുകുക == NUL)
	    win->w_buffer->b_prompt_തിരുകുക = 'A';
    }
}

    ശൂന്യം
entering_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    എങ്കിൽ (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	തിരികെ;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    എങ്കിൽ (win->w_buffer->b_prompt_തിരുകുക != NUL)
	stop_തിരുകുക_mode = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    എങ്കിൽ ((State & MODE_തിരുകുക) == 0)
	restart_edit = win->w_buffer->b_prompt_തിരുകുക;
}
#endif

    സ്ഥിതം ശൂന്യം
win_init_empty(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_വരികൾ_valid = 0;
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
#എങ്കിൽ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
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
    ശൂന്യം
curwin_init(ശൂന്യം)
{
    win_init_empty(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    ശൂന്യം
close_windows(
    buf_T	*buf,
    പൂർണ്ണസംഖ്യ		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    പൂർണ്ണസംഖ്യ		എണ്ണം = tabpage_index(ശൂന്യ);

    ++RedrawingDisabled;

    വേണ്ടി (wp = firstwin; wp != ശൂന്യ && !ONE_WINDOW; )
    {
	എങ്കിൽ (wp->w_buffer == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_buffer->b_locked > 0))
	{
	    എങ്കിൽ (window_layout_locked(CMD_SIZE))
		പോകുക theend;  // Only give one error message.
	    എങ്കിൽ (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		നിർത്തുക;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	അല്ലെങ്കിൽ
	    wp = wp->w_next;
    }

    // Also check windows in other tab pages.
    വേണ്ടി (tp = first_tabpage; tp != ശൂന്യ; tp = nexttp)
    {
	nexttp = tp->tp_next;
	എങ്കിൽ (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		എങ്കിൽ (wp->w_buffer == buf
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0))
		{
		    എങ്കിൽ (window_layout_locked(CMD_SIZE))
			പോകുക theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = first_tabpage;
		    നിർത്തുക;
		}
    }

theend:
    എങ്കിൽ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    എങ്കിൽ (എണ്ണം != tabpage_index(ശൂന്യ))
	apply_autocmds(EVENT_TABCLOSED, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    പൂർണ്ണസംഖ്യ
last_window(ശൂന്യം)
{
    തിരികെ (one_window() && first_tabpage->tp_next == ശൂന്യ);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    പൂർണ്ണസംഖ്യ
one_window(ശൂന്യം)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		seen_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	എങ്കിൽ (!is_aucmd_win(wp))
	{
	    എങ്കിൽ (seen_one)
		തിരികെ FALSE;
	    seen_one = TRUE;
	}
    }
    തിരികെ TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
close_last_window_tabpage(
    win_T	*win,
    പൂർണ്ണസംഖ്യ		free_buf,
    tabpage_T   *prev_curtab)
{
    എങ്കിൽ (!ONE_WINDOW)
	തിരികെ FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_buffer != ശൂന്യ);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    എങ്കിൽ (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    എങ്കിൽ (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
#എങ്കിൽ defined(FEAT_TABPANEL)
    എങ്കിൽ (p_stpl > 0)
	shell_new_columns();
#endif
    apply_autocmds(EVENT_WINENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    എങ്കിൽ (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    തിരികെ TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    സ്ഥിതം ശൂന്യം
win_close_buffer(win_T *win, പൂർണ്ണസംഖ്യ action, പൂർണ്ണസംഖ്യ abort_if_last)
{
#ifdef FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    എങ്കിൽ (win->w_buffer != ശൂന്യ)
	reset_synblock(win);
#endif

#ifdef FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    എങ്കിൽ (win->w_buffer != ശൂന്യ && bt_quickfix(win->w_buffer)
					&& win->w_buffer->b_nwindows == 1)
	win->w_buffer->b_p_bl = FALSE;
#endif

    // Close the link to the buffer.
    എങ്കിൽ (win->w_buffer != ശൂന്യ)
    {
	bufref_T    bufref;

	set_bufref(&bufref, curbuf);
	++win->w_locked;
	close_buffer(win, win->w_buffer, action, abort_if_last, TRUE, TRUE);
	എങ്കിൽ (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	എങ്കിൽ (!bufref_valid(&bufref))
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
    പൂർണ്ണസംഖ്യ
win_close(win_T *win, പൂർണ്ണസംഖ്യ free_buf)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		other_buffer = FALSE;
    പൂർണ്ണസംഖ്യ		close_curwin = FALSE;
    പൂർണ്ണസംഖ്യ		dir;
    പൂർണ്ണസംഖ്യ		help_window = FALSE;
    പൂർണ്ണസംഖ്യ		quickfix_window = FALSE;
    tabpage_T   *prev_curtab = curtab;
    frame_T	*win_frame = win->w_frame->fr_parent;
#ifdef FEAT_DIFF
    പൂർണ്ണസംഖ്യ		had_diffmode = win->w_p_diff;
#endif
#ifdef MESSAGE_QUEUE
    പൂർണ്ണസംഖ്യ		did_decrement = FALSE;
#endif

#എങ്കിൽ defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    എങ്കിൽ (may_close_term_popup() == OK)
	തിരികെ OK;
#endif
    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ FAIL;

    എങ്കിൽ (last_window())
    {
	emsg(_(e_cannot_close_last_window));
	തിരികെ FAIL;
    }
    എങ്കിൽ (window_layout_locked(CMD_close))
	തിരികെ FAIL;

    എങ്കിൽ (win_locked(win) || (win->w_buffer != ശൂന്യ
					       && win->w_buffer->b_locked > 0))
	തിരികെ FAIL; // window is already being closed
    എങ്കിൽ (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	തിരികെ FAIL;
    }
    എങ്കിൽ ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_window())
    {
	emsg(_(e_cannot_close_window_only_autocmd_window_would_remain));
	തിരികെ FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    എങ്കിൽ (close_last_window_tabpage(win, free_buf, prev_curtab))
	തിരികെ FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    എങ്കിൽ (bt_help(win->w_buffer))
	help_window = TRUE;
    അല്ലെങ്കിൽ
	clear_snapshot(curtab, SNAP_HELP_IDX);

    എങ്കിൽ (bt_quickfix(win->w_buffer))
	quickfix_window = TRUE;
    അല്ലെങ്കിൽ
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    എങ്കിൽ (win == curwin)
    {
#ifdef FEAT_JOB_CHANNEL
	leaving_window(curwin);
#endif
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, ശൂന്യ));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	എങ്കിൽ (wp->w_buffer != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_buffer = TRUE;
	    എങ്കിൽ (!win_valid(win))
		തിരികെ FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	    എങ്കിൽ (!win_valid(win))
		തിരികെ FAIL;
	    --win->w_locked;
	    എങ്കിൽ (last_window())
		തിരികെ FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	എങ്കിൽ (!win_valid(win))
	    തിരികെ FAIL;
	--win->w_locked;
	എങ്കിൽ (last_window())
	    തിരികെ FAIL;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	എങ്കിൽ (aborting())
	    തിരികെ FAIL;
#endif
    }

#ifdef FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    എങ്കിൽ (gui.in_use)
	out_flush();
#endif

#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (popup_win_closed(win) && !win_valid(win))
	തിരികെ FAIL;
#endif

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    എങ്കിൽ (!win_valid_any_tab(win))
	തിരികെ OK;

    win_close_buffer(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    എങ്കിൽ (win_valid(win) && win->w_buffer == ശൂന്യ
#എങ്കിൽ defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#endif
	    && last_window())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	എങ്കിൽ (curwin->w_buffer == ശൂന്യ)
	    curwin->w_buffer = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    എങ്കിൽ (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_buffer == ശൂന്യ)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	തിരികെ FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    എങ്കിൽ (!win_valid(win) || last_window()
	    || close_last_window_tabpage(win, free_buf, prev_curtab))
	തിരികെ FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++split_disallowed;
#ifdef MESSAGE_QUEUE
    ++dont_parse_messages;
#endif

    എങ്കിൽ (win->w_buffer != ശൂന്യ)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, ശൂന്യ);

    എങ്കിൽ (help_window || quickfix_window)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	എങ്കിൽ (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    എങ്കിൽ (win == curwin)
    {
	curwin = wp;
#ifdef FEAT_QUICKFIX
	എങ്കിൽ (wp->w_p_pvw || bt_quickfix(wp->w_buffer))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    വേണ്ടി (;;)
	    {
		എങ്കിൽ (wp->w_next == ശൂന്യ)
		    wp = firstwin;
		അല്ലെങ്കിൽ
		    wp = wp->w_next;
		എങ്കിൽ (wp == curwin)
		    നിർത്തുക;
		എങ്കിൽ (!wp->w_p_pvw && !bt_quickfix(wp->w_buffer))
		{
		    curwin = wp;
		    നിർത്തുക;
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

    എങ്കിൽ (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_frame->fr_parent == win_frame, dir);
    അല്ലെങ്കിൽ
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    എങ്കിൽ (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ifdef MESSAGE_QUEUE
	did_decrement =
#അല്ലെങ്കിൽ
	(ശൂന്യം)
#endif
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	എങ്കിൽ (other_buffer)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    }

    എങ്കിൽ (ONE_WINDOW && curwin->w_locked && curbuf->b_locked_split
	    && first_tabpage->tp_next != ശൂന്യ)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);

    --split_disallowed;
#ifdef MESSAGE_QUEUE
    എങ്കിൽ (!did_decrement)
	--dont_parse_messages;
#endif

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    എങ്കിൽ (help_window || quickfix_window)
	restore_snapshot(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ifdef FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    എങ്കിൽ (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	പൂർണ്ണസംഖ്യ	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    എങ്കിൽ (dwin->w_p_diff)
		++diffcount;
	എങ്കിൽ (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#endif

#എങ്കിൽ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    എങ്കിൽ (gui.in_use && !win_hasvertsplit())
	gui_init_which_components(ശൂന്യ);
#endif

    redraw_all_later(UPD_NOT_VALID);
    തിരികെ OK;
}

    സ്ഥിതം ശൂന്യം
trigger_winnewpre(ശൂന്യം)
{
    window_layout_lock();
    apply_autocmds(EVENT_WINNEWPRE, ശൂന്യ, ശൂന്യ, FALSE, ശൂന്യ);
    window_layout_unlock();
}

    സ്ഥിതം ശൂന്യം
trigger_winclosed(win_T *win)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    എങ്കിൽ (recursive)
	തിരികെ;
    recursive = TRUE;
    vim_snprintf((അക്ഷരം *)winid, വലുപ്പം(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_buffer);
    recursive = FALSE;
}

    ശൂന്യം
trigger_tabclosedpre(tabpage_T *tp)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    എങ്കിൽ (!has_tabclosedpre() || recursive)
	തിരികെ;

    എങ്കിൽ (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    window_layout_lock();
    apply_autocmds(EVENT_TABCLOSEDPRE, ശൂന്യ, ശൂന്യ, FALSE, ശൂന്യ);
    window_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    എങ്കിൽ (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    അല്ലെങ്കിൽ
	// fall back to the first tabpage
	goto_tabpage_tp(first_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    ശൂന്യം
snapshot_windows_scroll_size(ശൂന്യം)
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

സ്ഥിതം പൂർണ്ണസംഖ്യ did_initial_scroll_size_snapshot = FALSE;

    ശൂന്യം
may_make_initial_scroll_size_snapshot(ശൂന്യം)
{
    എങ്കിൽ (!did_initial_scroll_size_snapshot)
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
    സ്ഥിതം dict_T *
make_win_info_dict(
	പൂർണ്ണസംഖ്യ width,
	പൂർണ്ണസംഖ്യ height,
	പൂർണ്ണസംഖ്യ topline,
# ifdef FEAT_DIFF
	പൂർണ്ണസംഖ്യ topfill,
# endif
	പൂർണ്ണസംഖ്യ leftcol,
	പൂർണ്ണസംഖ്യ skipcol)
{
    dict_T *d = dict_alloc();
    എങ്കിൽ (d == ശൂന്യ)
	തിരികെ ശൂന്യ;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    ആകുമ്പോൾ (1)
    {
	typval_T tv;
	tv.v_lock = 0;
	tv.v_type = VAR_സംഖ്യ;

	tv.vval.v_സംഖ്യ = width;
	എങ്കിൽ (dict_add_tv(d, "width", &tv) == FAIL)
	    നിർത്തുക;
	tv.vval.v_സംഖ്യ = height;
	എങ്കിൽ (dict_add_tv(d, "height", &tv) == FAIL)
	    നിർത്തുക;
	tv.vval.v_സംഖ്യ = topline;
	എങ്കിൽ (dict_add_tv(d, "topline", &tv) == FAIL)
	    നിർത്തുക;
# ifdef FEAT_DIFF
	tv.vval.v_സംഖ്യ = topfill;
# അല്ലെങ്കിൽ
	tv.vval.v_സംഖ്യ = 0;
# endif
	എങ്കിൽ (dict_add_tv(d, "topfill", &tv) == FAIL)
	    നിർത്തുക;
	tv.vval.v_സംഖ്യ = leftcol;
	എങ്കിൽ (dict_add_tv(d, "leftcol", &tv) == FAIL)
	    നിർത്തുക;
	tv.vval.v_സംഖ്യ = skipcol;
	എങ്കിൽ (dict_add_tv(d, "skipcol", &tv) == FAIL)
	    നിർത്തുക;
	തിരികെ d;
    }
    dict_unref(d);
    തിരികെ ശൂന്യ;
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
    സ്ഥിതം ശൂന്യം
check_window_scroll_resize(
	പൂർണ്ണസംഖ്യ	*size_എണ്ണം,
	win_T	**first_scroll_win,
	win_T	**first_size_win,
	list_T	*winlist UNUSED,
	dict_T	*v_event UNUSED)
{
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ listidx = 0;
    പൂർണ്ണസംഖ്യ tot_width = 0;
    പൂർണ്ണസംഖ്യ tot_height = 0;
    പൂർണ്ണസംഖ്യ tot_topline = 0;
# ifdef FEAT_DIFF
    പൂർണ്ണസംഖ്യ tot_topfill = 0;
# endif
    പൂർണ്ണസംഖ്യ tot_leftcol = 0;
    പൂർണ്ണസംഖ്യ tot_skipcol = 0;
#endif

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	പൂർണ്ണസംഖ്യ ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	പൂർണ്ണസംഖ്യ size_changed = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_last_width != wp->w_width
			    || wp->w_last_height != wp->w_height);
	എങ്കിൽ (size_changed)
	{
#ifdef FEAT_EVAL
	    എങ്കിൽ (winlist != ശൂന്യ)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_lock = 0;
		tv.v_type = VAR_സംഖ്യ;
		tv.vval.v_സംഖ്യ = wp->w_id;
		list_set_item(winlist, listidx++, &tv);
	    }
	    അല്ലെങ്കിൽ
#endif
		എങ്കിൽ (size_എണ്ണം != ശൂന്യ)
	    {
		++*size_എണ്ണം;
		എങ്കിൽ (*first_size_win == ശൂന്യ)
		    *first_size_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		എങ്കിൽ (*first_scroll_win == ശൂന്യ && !ignore_scroll)
		    *first_scroll_win = wp;
	    }
	}

	പൂർണ്ണസംഖ്യ scroll_changed = !ignore_scroll
				&& (wp->w_last_topline != wp->w_topline
#ifdef FEAT_DIFF
				|| wp->w_last_topfill != wp->w_topfill
#endif
				|| wp->w_last_leftcol != wp->w_leftcol
				|| wp->w_last_skipcol != wp->w_skipcol);
	എങ്കിൽ (scroll_changed
	    && first_scroll_win != ശൂന്യ && *first_scroll_win == ശൂന്യ)
	    *first_scroll_win = wp;

#ifdef FEAT_EVAL
	എങ്കിൽ ((size_changed || scroll_changed) && v_event != ശൂന്യ)
	{
	    // Add info about this window to the v:event dictionary.
	    പൂർണ്ണസംഖ്യ width = wp->w_width - wp->w_last_width;
	    പൂർണ്ണസംഖ്യ height = wp->w_height - wp->w_last_height;
	    പൂർണ്ണസംഖ്യ topline = wp->w_topline - wp->w_last_topline;
# ifdef FEAT_DIFF
	    പൂർണ്ണസംഖ്യ topfill = wp->w_topfill - wp->w_last_topfill;
# endif
	    പൂർണ്ണസംഖ്യ leftcol = wp->w_leftcol - wp->w_last_leftcol;
	    പൂർണ്ണസംഖ്യ skipcol = wp->w_skipcol - wp->w_last_skipcol;
	    dict_T *d = make_win_info_dict(width, height, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    എങ്കിൽ (d == ശൂന്യ)
		നിർത്തുക;
	    അക്ഷരം winid[NUMBUFLEN];
	    vim_snprintf(winid, വലുപ്പം(winid), "%d", wp->w_id);
	    എങ്കിൽ (dict_add_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		നിർത്തുക;
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
    എങ്കിൽ (v_event != ശൂന്യ)
    {
	dict_T *alldict = make_win_info_dict(tot_width, tot_height, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	എങ്കിൽ (alldict != ശൂന്യ)
	{
	    എങ്കിൽ (dict_add_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    അല്ലെങ്കിൽ
		--alldict->dv_refcount;
	}
    }
#endif
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    ശൂന്യം
may_trigger_win_scrolled_resized(ശൂന്യം)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    recursive = FALSE;
    പൂർണ്ണസംഖ്യ		    do_resize = has_winresized();
    പൂർണ്ണസംഖ്യ		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    എങ്കിൽ (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_size_snapshot)
	തിരികെ;

    പൂർണ്ണസംഖ്യ size_എണ്ണം = 0;
    win_T *first_scroll_win = ശൂന്യ, *first_size_win = ശൂന്യ;
    check_window_scroll_resize(&size_എണ്ണം, &first_scroll_win, &first_size_win,
								    ശൂന്യ, ശൂന്യ);
    പൂർണ്ണസംഖ്യ trigger_resize = do_resize && size_എണ്ണം > 0;
    പൂർണ്ണസംഖ്യ trigger_scroll = do_scroll && first_scroll_win != ശൂന്യ;
    എങ്കിൽ (!trigger_resize && !trigger_scroll)
	തിരികെ;  // no relevant changes
#ifdef FEAT_EVAL
    list_T *windows_list = ശൂന്യ;
    എങ്കിൽ (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_ഇനങ്ങൾ(size_എണ്ണം);
	എങ്കിൽ (windows_list != ശൂന്യ)
	    check_window_scroll_resize(ശൂന്യ, ശൂന്യ, ശൂന്യ, windows_list, ശൂന്യ);
    }

    dict_T *scroll_dict = ശൂന്യ;
    എങ്കിൽ (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	എങ്കിൽ (scroll_dict != ശൂന്യ)
	{
	    scroll_dict->dv_refcount = 1;
	    check_window_scroll_resize(ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, scroll_dict);
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
    എങ്കിൽ (trigger_resize
#ifdef FEAT_EVAL
	    && windows_list != ശൂന്യ
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	എങ്കിൽ (dict_add_list(v_event, "windows", windows_list) == OK)
	{
	    dict_set_ഇനങ്ങൾ_ro(v_event);
#endif
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((അക്ഷരം *)winid, വലുപ്പം(winid), "%d",
							 first_size_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     first_size_win->w_buffer);
#ifdef FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#endif
    }

    എങ്കിൽ (trigger_scroll
#ifdef FEAT_EVAL
	    && scroll_dict != ശൂന്യ
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", ശൂന്യ);
	dict_set_ഇനങ്ങൾ_ro(v_event);
	dict_unref(scroll_dict);
#endif
	char_u winid[NUMBUFLEN];
	vim_snprintf((അക്ഷരം *)winid, വലുപ്പം(winid), "%d",
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
    ശൂന്യം
win_close_othertab(win_T *win, പൂർണ്ണസംഖ്യ free_buf, tabpage_T *tp)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		dir;
    tabpage_T   *ptp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    എങ്കിൽ (window_layout_locked(CMD_SIZE))
	തിരികെ;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    എങ്കിൽ (win_locked(win) || (win->w_buffer != ശൂന്യ
					       && win->w_buffer->b_locked > 0))
	തിരികെ; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    എങ്കിൽ (win->w_buffer != ശൂന്യ)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	എങ്കിൽ (!win_valid_any_tab(win))
	    തിരികെ;
    }

    എങ്കിൽ (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	എങ്കിൽ (!win_valid_any_tab(win))
	    തിരികെ;
    }

    എങ്കിൽ (win->w_buffer != ശൂന്യ)
	// Close the link to the buffer.
	close_buffer(win, win->w_buffer, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    വേണ്ടി (ptp = first_tabpage; ptp != ശൂന്യ && ptp != tp; ptp = ptp->tp_next)
	;
    എങ്കിൽ (ptp == ശൂന്യ || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	എങ്കിൽ (win_valid_any_tab(win) && win->w_buffer == ശൂന്യ)
	{
	    win->w_buffer = firstbuf;
	    ++firstbuf->b_nwindows;
	    എങ്കിൽ (win == curwin)
		curbuf = curwin->w_buffer;
	    win_init_empty(win);
	}
	തിരികെ;
    }

    // Autocommands may have closed the window already.
    വേണ്ടി (wp = tp->tp_firstwin; wp != ശൂന്യ && wp != win; wp = wp->w_next)
	;
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;

    // When closing the last window in a tab page remove the tab page.
    എങ്കിൽ (tp->tp_firstwin == tp->tp_lastwin)
    {
	പൂർണ്ണസംഖ്യ	h = tabline_height();

	എങ്കിൽ (tp == first_tabpage)
	    first_tabpage = tp->tp_next;
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (ptp = first_tabpage; ptp != ശൂന്യ && ptp->tp_next != tp;
							   ptp = ptp->tp_next)
		;
	    എങ്കിൽ (ptp == ശൂന്യ)
	    {
		internal_error("win_close_othertab()");
		തിരികെ;
	    }
	    ptp->tp_next = tp->tp_next;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	എങ്കിൽ (h != tabline_height())
	    shell_new_rows();
    }

    എങ്കിൽ (win->w_buffer != ശൂന്യ)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    എങ്കിൽ (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    സ്ഥിതം win_T *
win_free_mem(
    win_T	*win,
    പൂർണ്ണസംഖ്യ		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == ശൂന്യ ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_frame;
    wp = winframe_remove(win, dirp, tp, ശൂന്യ);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    എങ്കിൽ (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    തിരികെ wp;
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
win_free_all(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = ശൂന്യ;
    cmdwin_win = ശൂന്യ;

    ആകുമ്പോൾ (first_tabpage->tp_next != ശൂന്യ)
	tabpage_close(TRUE);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < AUCMD_WIN_എണ്ണം; ++i)
	എങ്കിൽ (aucmd_win[i].auc_win != ശൂന്യ)
	{
	    (ശൂന്യം)win_free_mem(aucmd_win[i].auc_win, &dummy, ശൂന്യ);
	    aucmd_win[i].auc_win = ശൂന്യ;
	}

    ആകുമ്പോൾ (firstwin != ശൂന്യ)
	(ശൂന്യം)win_free_mem(firstwin, &dummy, ശൂന്യ);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = ശൂന്യ;
}
#endif

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    പൂർണ്ണസംഖ്യ		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    frame_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    frame_T	*frp, *frp2, *frp3;
    frame_T	*frp_close = win->w_frame;
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		row, col;

    /*
     * If there is only one window there is nothing to remove.
     */
    എങ്കിൽ (tp == ശൂന്യ ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	തിരികെ ശൂന്യ;

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
    എങ്കിൽ (win->w_vsep_width == 0 && frp_close->fr_parent->fr_layout == FR_ROW &&
	    frp_close->fr_prev != ശൂന്യ)
	frame_set_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    frame_remove(frp_close);

    എങ്കിൽ (frp_close->fr_parent->fr_layout == FR_COL)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	എങ്കിൽ (frp2->fr_win != ശൂന്യ && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    ആകുമ്പോൾ (frp != ശൂന്യ || frp3 != ശൂന്യ)
	    {
		എങ്കിൽ (frp != ശൂന്യ)
		{
		    എങ്കിൽ (!frame_fixed_height(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			നിർത്തുക;
		    }
		    frp = frp->fr_prev;
		}
		എങ്കിൽ (frp3 != ശൂന്യ)
		{
		    എങ്കിൽ (frp3->fr_win != ശൂന്യ && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			നിർത്തുക;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_height(frp2, frp2->fr_height + frp_close->fr_height,
			    frp2 == frp_close->fr_next, !frame_fixed_height(frp2), FALSE);
	*dirp = 'v';
    }
    അല്ലെങ്കിൽ
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	എങ്കിൽ (frp2->fr_win != ശൂന്യ && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    ആകുമ്പോൾ (frp != ശൂന്യ || frp3 != ശൂന്യ)
	    {
		എങ്കിൽ (frp != ശൂന്യ)
		{
		    എങ്കിൽ (!frame_fixed_width(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			നിർത്തുക;
		    }
		    frp = frp->fr_prev;
		}
		എങ്കിൽ (frp3 != ശൂന്യ)
		{
		    എങ്കിൽ (frp3->fr_win != ശൂന്യ && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			നിർത്തുക;
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
    എങ്കിൽ (frp2 != frp_close->fr_prev)
	frame_comp_pos(frp_close->fr_parent, &row, &col);

    എങ്കിൽ (unflat_altfr == ശൂന്യ)
	frame_flatten(frp2);
    അല്ലെങ്കിൽ
	*unflat_altfr = frp2;

    frame_locked--;

    തിരികെ wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    സ്ഥിതം ശൂന്യം
frame_flatten(frame_T *frp)
{
    frame_T	*frp2, *frp3;

    എങ്കിൽ (frp->fr_next != ശൂന്യ || frp->fr_prev != ശൂന്യ)
	തിരികെ;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_parent->fr_layout = frp->fr_layout;
    frp->fr_parent->fr_child = frp->fr_child;
    FOR_ALL_FRAMES(frp2, frp->fr_child)
	frp2->fr_parent = frp->fr_parent;
    frp->fr_parent->fr_win = frp->fr_win;
    എങ്കിൽ (frp->fr_win != ശൂന്യ)
	frp->fr_win->w_frame = frp->fr_parent;
    frp2 = frp->fr_parent;
    എങ്കിൽ (topframe->fr_child == frp)
	topframe->fr_child = frp2;
    vim_free(frp);

    frp = frp2->fr_parent;
    എങ്കിൽ (frp != ശൂന്യ && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	എങ്കിൽ (frp->fr_child == frp2)
	    frp->fr_child = frp2->fr_child;
	frp2->fr_child->fr_prev = frp2->fr_prev;
	എങ്കിൽ (frp2->fr_prev != ശൂന്യ)
	    frp2->fr_prev->fr_next = frp2->fr_child;
	വേണ്ടി (frp3 = frp2->fr_child; ; frp3 = frp3->fr_next)
	{
	    frp3->fr_parent = frp;
	    എങ്കിൽ (frp3->fr_next == ശൂന്യ)
	    {
		frp3->fr_next = frp2->fr_next;
		എങ്കിൽ (frp2->fr_next != ശൂന്യ)
		    frp2->fr_next->fr_prev = frp3;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (topframe->fr_child == frp2)
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
    സ്ഥിതം ശൂന്യം
winframe_restore(win_T *wp, പൂർണ്ണസംഖ്യ dir, frame_T *unflat_altfr)
{
    frame_T	*frp = wp->w_frame;

    // Put "wp"'s frame back where it was.
    എങ്കിൽ (frp->fr_prev != ശൂന്യ)
	frame_append(frp->fr_prev, frp);
    അല്ലെങ്കിൽ
	frame_തിരുകുക(frp->fr_next, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    എങ്കിൽ (wp->w_vsep_width == 0
	    && frp->fr_parent->fr_layout == FR_ROW && frp->fr_prev != ശൂന്യ)
	frame_set_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    എങ്കിൽ (wp->w_status_height == 0
	    && frp->fr_parent->fr_layout == FR_COL && frp->fr_prev != ശൂന്യ)
	frame_add_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    എങ്കിൽ (dir == 'v')
    {
	frame_new_height(unflat_altfr, unflat_altfr->fr_height - frp->fr_height,
		unflat_altfr == frp->fr_next, FALSE, FALSE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (dir == 'h')
    {
	frame_new_width(unflat_altfr, unflat_altfr->fr_width - frp->fr_width,
		unflat_altfr == frp->fr_next, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    എങ്കിൽ (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_parent);
	പൂർണ്ണസംഖ്യ	row = topleft->w_winrow;
	പൂർണ്ണസംഖ്യ	col = topleft->w_wincol;

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
    സ്ഥിതം frame_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    frame_T	*other_fr, *target_fr;

    എങ്കിൽ (tp == ശൂന്യ ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	തിരികെ alt_tabpage()->tp_curwin->w_frame;

    frp = win->w_frame;

    എങ്കിൽ (frp->fr_prev == ശൂന്യ)
	തിരികെ frp->fr_next;
    എങ്കിൽ (frp->fr_next == ശൂന്യ)
	തിരികെ frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    // window
    target_fr = frp->fr_next;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    എങ്കിൽ (frp->fr_parent != ശൂന്യ && frp->fr_parent->fr_layout == FR_COL && p_sb)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    എങ്കിൽ (frp->fr_parent != ശൂന്യ && frp->fr_parent->fr_layout == FR_ROW && p_spr)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    എങ്കിൽ (frp->fr_parent != ശൂന്യ && frp->fr_parent->fr_layout == FR_ROW)
    {
	എങ്കിൽ (frame_fixed_width(target_fr) && !frame_fixed_width(other_fr))
	    target_fr = other_fr;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (frame_fixed_height(target_fr) && !frame_fixed_height(other_fr))
	    target_fr = other_fr;
    }

    തിരികെ target_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    സ്ഥിതം tabpage_T *
alt_tabpage(ശൂന്യം)
{
    tabpage_T	*tp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		forward;

    // Use the last accessed tab page, if possible.
    എങ്കിൽ ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	തിരികെ lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_next != ശൂന്യ &&
	    ((tcl_flags & TCL_ഇടത്) == 0 || curtab == first_tabpage);

    എങ്കിൽ (forward)
	tp = curtab->tp_next;
    അല്ലെങ്കിൽ
	// Use the previous tab page.
	വേണ്ടി (tp = first_tabpage; tp->tp_next != curtab; tp = tp->tp_next)
	    ;

    തിരികെ tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    സ്ഥിതം win_T *
frame2win(frame_T *frp)
{
    ആകുമ്പോൾ (frp->fr_win == ശൂന്യ)
	frp = frp->fr_child;
    തിരികെ frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
frame_has_win(frame_T *frp, win_T *wp)
{
    frame_T	*p;

    എങ്കിൽ (frp->fr_layout == FR_LEAF)
	തിരികെ frp->fr_win == wp;

    FOR_ALL_FRAMES(p, frp->fr_child)
	എങ്കിൽ (frame_has_win(p, wp))
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
സ്ഥിതം പൂർണ്ണസംഖ്യ min_set_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    സ്ഥിതം ശൂന്യം
frame_new_height(
    frame_T	*topfrp,
    പൂർണ്ണസംഖ്യ		height,
    പൂർണ്ണസംഖ്യ		topfirst,	// resize topmost contained frame first
    പൂർണ്ണസംഖ്യ		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    പൂർണ്ണസംഖ്യ		set_ch)		// set 'cmdheight' to resize topframe
{
    frame_T	*frp;
    പൂർണ്ണസംഖ്യ		extra_വരികൾ;
    പൂർണ്ണസംഖ്യ		h;

    എങ്കിൽ (topfrp->fr_parent == ശൂന്യ && set_ch)
    {
	// topframe: update the command line height, with side effects.
	പൂർണ്ണസംഖ്യ new_ch = MAX(min_set_ch, p_ch + topfrp->fr_height - height);
	പൂർണ്ണസംഖ്യ save_ch = min_set_ch;
	എങ്കിൽ (new_ch != p_ch)
	    set_option_മൂല്യം((char_u *)"cmdheight", new_ch, ശൂന്യ, 0);
	min_set_ch = save_ch;
	height = MIN(height, ROWS_AVAIL);
    }
    എങ്കിൽ (topfrp->fr_win != ശൂന്യ)
    {
	// Simple case: just one window.
	win_new_height(topfrp->fr_win,
				    height - topfrp->fr_win->w_status_height
					      - WINBAR_HEIGHT(topfrp->fr_win));
    }
    അല്ലെങ്കിൽ എങ്കിൽ (topfrp->fr_layout == FR_ROW)
    {
	ചെയ്യുക
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_height(frp, height, topfirst, wfh, set_ch);
		എങ്കിൽ (frp->fr_height > height)
		{
		    // Could not fit the windows, make the whole row higher.
		    height = frp->fr_height;
		    നിർത്തുക;
		}
	    }
	}
	ആകുമ്പോൾ (frp != ശൂന്യ);
    }
    അല്ലെങ്കിൽ    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_child;
	എങ്കിൽ (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    ആകുമ്പോൾ (frame_fixed_height(frp))
	    {
		frp = frp->fr_next;
		എങ്കിൽ (frp == ശൂന്യ)
		    തിരികെ;	    // no frame without 'wfh', give up
	    }
	എങ്കിൽ (!topfirst)
	{
	    // Find the bottom frame of this column
	    ആകുമ്പോൾ (frp->fr_next != ശൂന്യ)
		frp = frp->fr_next;
	    എങ്കിൽ (wfh)
		// Advance back for frames with one window with 'wfh' set.
		ആകുമ്പോൾ (frame_fixed_height(frp))
		    frp = frp->fr_prev;
	}

	extra_വരികൾ = height - topfrp->fr_height;
	എങ്കിൽ (extra_വരികൾ < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    ആകുമ്പോൾ (frp != ശൂന്യ)
	    {
		h = frame_minheight(frp, ശൂന്യ);
		എങ്കിൽ (frp->fr_height + extra_വരികൾ < h)
		{
		    extra_വരികൾ += frp->fr_height - h;
		    frame_new_height(frp, h, topfirst, wfh, set_ch);
		}
		അല്ലെങ്കിൽ
		{
		    frame_new_height(frp, frp->fr_height + extra_വരികൾ,
							topfirst, wfh, set_ch);
		    നിർത്തുക;
		}
		എങ്കിൽ (topfirst)
		{
		    ചെയ്യുക
			frp = frp->fr_next;
		    ആകുമ്പോൾ (wfh && frp != ശൂന്യ && frame_fixed_height(frp));
		}
		അല്ലെങ്കിൽ
		{
		    ചെയ്യുക
			frp = frp->fr_prev;
		    ആകുമ്പോൾ (wfh && frp != ശൂന്യ && frame_fixed_height(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		എങ്കിൽ (frp == ശൂന്യ)
		    height -= extra_വരികൾ;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (extra_വരികൾ > 0)
	{
	    // increase height of bottom or top frame
	    frame_new_height(frp, frp->fr_height + extra_വരികൾ, topfirst, wfh,
									set_ch);
	}
    }
    topfrp->fr_height = height;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
frame_fixed_height(frame_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    എങ്കിൽ (frp->fr_win != ശൂന്യ)
	തിരികെ frp->fr_win->w_p_wfh;

    എങ്കിൽ (frp->fr_layout == FR_ROW)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    എങ്കിൽ (frame_fixed_height(frp))
		തിരികെ TRUE;
	തിരികെ FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	എങ്കിൽ (!frame_fixed_height(frp))
	    തിരികെ FALSE;
    തിരികെ TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
frame_fixed_width(frame_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    എങ്കിൽ (frp->fr_win != ശൂന്യ)
	തിരികെ frp->fr_win->w_p_wfw;

    എങ്കിൽ (frp->fr_layout == FR_COL)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    എങ്കിൽ (frame_fixed_width(frp))
		തിരികെ TRUE;
	തിരികെ FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	എങ്കിൽ (!frame_fixed_width(frp))
	    തിരികെ FALSE;
    തിരികെ TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    സ്ഥിതം ശൂന്യം
frame_add_statusline(frame_T *frp)
{
    win_T	*wp;

    എങ്കിൽ (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	wp->w_status_height = statusline_height(wp);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (frp->fr_layout == FR_ROW)
    {
	// Handle all the frames in the row.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_statusline(frp);
    }
    അല്ലെങ്കിൽ // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	വേണ്ടി (frp = frp->fr_child; frp->fr_next != ശൂന്യ; frp = frp->fr_next)
	    ;
	frame_add_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    സ്ഥിതം ശൂന്യം
frame_new_width(
    frame_T	*topfrp,
    പൂർണ്ണസംഖ്യ		width,
    പൂർണ്ണസംഖ്യ		leftfirst,	// resize leftmost contained frame first
    പൂർണ്ണസംഖ്യ		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    frame_T	*frp;
    പൂർണ്ണസംഖ്യ		extra_cols;
    പൂർണ്ണസംഖ്യ		w;
    win_T	*wp;

    എങ്കിൽ (topfrp->fr_layout == FR_LEAF)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	വേണ്ടി (frp = topfrp; frp->fr_parent != ശൂന്യ; frp = frp->fr_parent)
	    എങ്കിൽ (frp->fr_parent->fr_layout == FR_ROW && frp->fr_next != ശൂന്യ)
		നിർത്തുക;
	എങ്കിൽ (frp->fr_parent == ശൂന്യ)
	    wp->w_vsep_width = 0;
	win_new_width(wp, width - wp->w_vsep_width);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (topfrp->fr_layout == FR_COL)
    {
	ചെയ്യുക
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_width(frp, width, leftfirst, wfw);
		എങ്കിൽ (frp->fr_width > width)
		{
		    // Could not fit the windows, make whole column wider.
		    width = frp->fr_width;
		    നിർത്തുക;
		}
	    }
	} ആകുമ്പോൾ (frp != ശൂന്യ);
    }
    അല്ലെങ്കിൽ    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_child;
	എങ്കിൽ (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    ആകുമ്പോൾ (frame_fixed_width(frp))
	    {
		frp = frp->fr_next;
		എങ്കിൽ (frp == ശൂന്യ)
		    തിരികെ;	    // no frame without 'wfw', give up
	    }
	എങ്കിൽ (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    ആകുമ്പോൾ (frp->fr_next != ശൂന്യ)
		frp = frp->fr_next;
	    എങ്കിൽ (wfw)
		// Advance back for frames with one window with 'wfw' set.
		ആകുമ്പോൾ (frame_fixed_width(frp))
		    frp = frp->fr_prev;
	}

	extra_cols = width - topfrp->fr_width;
	എങ്കിൽ (extra_cols < 0)
	{
	    // reduce frame width, rightmost frame first
	    ആകുമ്പോൾ (frp != ശൂന്യ)
	    {
		w = frame_minwidth(frp, ശൂന്യ);
		എങ്കിൽ (frp->fr_width + extra_cols < w)
		{
		    extra_cols += frp->fr_width - w;
		    frame_new_width(frp, w, leftfirst, wfw);
		}
		അല്ലെങ്കിൽ
		{
		    frame_new_width(frp, frp->fr_width + extra_cols,
							      leftfirst, wfw);
		    നിർത്തുക;
		}
		എങ്കിൽ (leftfirst)
		{
		    ചെയ്യുക
			frp = frp->fr_next;
		    ആകുമ്പോൾ (wfw && frp != ശൂന്യ && frame_fixed_width(frp));
		}
		അല്ലെങ്കിൽ
		{
		    ചെയ്യുക
			frp = frp->fr_prev;
		    ആകുമ്പോൾ (wfw && frp != ശൂന്യ && frame_fixed_width(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		എങ്കിൽ (frp == ശൂന്യ)
		    width -= extra_cols;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (extra_cols > 0)
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
    സ്ഥിതം ശൂന്യം
frame_set_vsep(frame_T *frp, പൂർണ്ണസംഖ്യ add)
{
    win_T	*wp;

    എങ്കിൽ (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	എങ്കിൽ (add && wp->w_vsep_width == 0)
	{
	    എങ്കിൽ (wp->w_width > 0)	// don't make it negative
		win_new_width(wp, wp->w_width - 1);
	    wp->w_vsep_width = 1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!add && wp->w_vsep_width == 1)
	{
	    win_new_width(wp, wp->w_width + 1);
	    wp->w_vsep_width = 0;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (frp->fr_layout == FR_COL)
    {
	// Handle all the frames in the column.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_set_vsep(frp, add);
    }
    അല്ലെങ്കിൽ // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_child;
	ആകുമ്പോൾ (frp->fr_next != ശൂന്യ)
	    frp = frp->fr_next;
	frame_set_vsep(frp, add);
    }
}

/*
 * Set frame width from the window it contains.
 */
    സ്ഥിതം ശൂന്യം
frame_fix_width(win_T *wp)
{
    wp->w_frame->fr_width = wp->w_width + wp->w_vsep_width;
}

/*
 * Set frame height from the window it contains.
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
frame_minheight(frame_T *topfrp, win_T *next_curwin)
{
    frame_T	*frp;
    പൂർണ്ണസംഖ്യ		m;
    പൂർണ്ണസംഖ്യ		n;

    എങ്കിൽ (topfrp->fr_win != ശൂന്യ)
    {
	എങ്കിൽ (topfrp->fr_win == next_curwin)
	    m = p_wh + topfrp->fr_win->w_status_height;
	അല്ലെങ്കിൽ
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_status_height;
	    എങ്കിൽ (topfrp->fr_win == curwin && next_curwin == ശൂന്യ)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		എങ്കിൽ (p_wmh == 0)
		    ++m;
		m += WINBAR_HEIGHT(curwin);
	    }
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (topfrp->fr_layout == FR_ROW)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minheight(frp, next_curwin);
	    എങ്കിൽ (n > m)
		m = n;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minheight(frp, next_curwin);
    }

    തിരികെ m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
frame_wincount_in_height(frame_T *topfrp)
{
    frame_T	*frp;
    പൂർണ്ണസംഖ്യ		c = 0;
    പൂർണ്ണസംഖ്യ		n;

    എങ്കിൽ (topfrp->fr_win != ശൂന്യ)
	c = 1;
    അല്ലെങ്കിൽ എങ്കിൽ (topfrp->fr_layout == FR_ROW)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_wincount_in_height(frp);
	    എങ്കിൽ (n > c)
		c = n;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    c += frame_wincount_in_height(frp);
    }

    തിരികെ c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
frame_minwidth(
    frame_T	*topfrp,
    win_T	*next_curwin)	// use p_wh and p_wiw for next_curwin
{
    frame_T	*frp;
    പൂർണ്ണസംഖ്യ		m, n;

    എങ്കിൽ (topfrp->fr_win != ശൂന്യ)
    {
	എങ്കിൽ (topfrp->fr_win == next_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_width;
	അല്ലെങ്കിൽ
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_width;
	    // Current window is minimal one column wide
	    എങ്കിൽ (p_wmw == 0 && topfrp->fr_win == curwin && next_curwin == ശൂന്യ)
		++m;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (topfrp->fr_layout == FR_COL)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minwidth(frp, next_curwin);
	    എങ്കിൽ (n > m)
		m = n;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minwidth(frp, next_curwin);
    }

    തിരികെ m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    ശൂന്യം
close_others(
    പൂർണ്ണസംഖ്യ		message,
    പൂർണ്ണസംഖ്യ		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    പൂർണ്ണസംഖ്യ		r;

    എങ്കിൽ (one_window())
    {
	എങ്കിൽ (message && !autocmd_busy)
	    സന്ദേശം(_(m_onlyone));
	തിരികെ;
    }

    // Be very careful here: autocommands may change the window layout.
    വേണ്ടി (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_next;

	// autocommands messed this one up
	എങ്കിൽ (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
	}

	എങ്കിൽ (wp == curwin)		// don't close current window
	    തുടരുക;

	// autoccommands messed this one up
	എങ്കിൽ (!buf_valid(wp->w_buffer) && win_valid(wp))
	{
	    wp->w_buffer = ശൂന്യ;
	    win_close(wp, 0);
	    തുടരുക;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_buffer, forceit);
	എങ്കിൽ (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    തുടരുക;
	}
	എങ്കിൽ (!r)
	{
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    എങ്കിൽ (message && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_changed(wp->w_buffer, FALSE);
		എങ്കിൽ (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    തുടരുക;
		}
	    }
	    എങ്കിൽ (bufIsChanged(wp->w_buffer))
#endif
		തുടരുക;
	}
	win_close(wp, !buf_hide(wp->w_buffer) && !bufIsChanged(wp->w_buffer));
    }

    എങ്കിൽ (message && !ONE_WINDOW)
	emsg(_(e_other_window_contains_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    ശൂന്യം
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
സ്ഥിതം പൂർണ്ണസംഖ്യ command_frame_height = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    ശൂന്യം
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
    പൂർണ്ണസംഖ്യ
win_alloc_first(ശൂന്യം)
{
    എങ്കിൽ (win_alloc_firstwin(ശൂന്യ) == FAIL)
	തിരികെ FAIL;

    first_tabpage = alloc_tabpage();
    എങ്കിൽ (first_tabpage == ശൂന്യ)
	തിരികെ FAIL;
    curtab = first_tabpage;
    unuse_tabpage(first_tabpage);

    തിരികെ OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(ശൂന്യം)
{
    win_T *wp;

    wp = win_alloc(ശൂന്യ, TRUE);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ ശൂന്യ;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_frame(wp);
    തിരികെ wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    ശൂന്യം
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(ശൂന്യ, FALSE);
    എങ്കിൽ (curwin == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (oldwin == ശൂന്യ)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(ശൂന്യ, ശൂന്യ, 1L, BLN_LISTED);
	എങ്കിൽ (curwin == ശൂന്യ || curbuf == ശൂന്യ)
	    തിരികെ FAIL;
	curwin->w_buffer = curbuf;
#ifdef FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#endif
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    അല്ലെങ്കിൽ
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_frame(curwin);
    എങ്കിൽ (curwin->w_frame == ശൂന്യ)
	തിരികെ FAIL;
    topframe = curwin->w_frame;
    topframe->fr_width = COLUMNS_WITHOUT_TPL();
    topframe->fr_height = Rows - p_ch;

    തിരികെ OK;
}

/*
 * Create a frame for window "wp".
 */
    സ്ഥിതം ശൂന്യം
new_frame(win_T *wp)
{
    frame_T *frp = ALLOC_CLEAR_ONE(frame_T);

    wp->w_frame = frp;
    എങ്കിൽ (frp == ശൂന്യ)
	തിരികെ;
    frp->fr_layout = FR_LEAF;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    ശൂന്യം
win_init_size(ശൂന്യം)
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
    സ്ഥിതം tabpage_T *
alloc_tabpage(ശൂന്യം)
{
    tabpage_T	*tp;
#ifdef FEAT_GUI
    പൂർണ്ണസംഖ്യ		i;
#endif


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    എങ്കിൽ (tp == ശൂന്യ)
	തിരികെ ശൂന്യ;

#ifdef FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    എങ്കിൽ (tp->tp_vars == ശൂന്യ)
    {
	vim_free(tp);
	തിരികെ ശൂന്യ;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#endif

#ifdef FEAT_GUI
    വേണ്ടി (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#endif
#ifdef FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#endif
    tp->tp_ch_used = p_ch;

    തിരികെ tp;
}

    ശൂന്യം
free_tabpage(tabpage_T *tp)
{
    പൂർണ്ണസംഖ്യ idx;

#ifdef FEAT_DIFF
    diff_clear(tp);
#endif
#ifdef FEAT_PROP_POPUP
    ആകുമ്പോൾ (tp->tp_first_popupwin != ശൂന്യ)
	popup_close_tabpage(tp, tp->tp_first_popupwin->w_id, TRUE);
#endif
    വേണ്ടി (idx = 0; idx < SNAP_എണ്ണം; ++idx)
	clear_snapshot(tp, idx);
#ifdef FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#endif

    എങ്കിൽ (tp == lastused_tabpage)
	lastused_tabpage = ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ
win_new_tabpage(പൂർണ്ണസംഖ്യ after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    പൂർണ്ണസംഖ്യ		n;
#എങ്കിൽ defined(FEAT_TABPANEL)
    പൂർണ്ണസംഖ്യ prev_columns = COLUMNS_WITHOUT_TPL();
#endif

    എങ്കിൽ (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	തിരികെ FAIL;
    }
    എങ്കിൽ (window_layout_locked(CMD_tabnew))
	തിരികെ FAIL;

    newtp = alloc_tabpage();
    എങ്കിൽ (newtp == ശൂന്യ)
	തിരികെ FAIL;

    // Remember the current windows in this Tab page.
    എങ്കിൽ (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	തിരികെ FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == ശൂന്യ)
				    ? ശൂന്യ : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    എങ്കിൽ (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	എങ്കിൽ (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_next = first_tabpage;
	    first_tabpage = newtp;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		വേണ്ടി (tp = first_tabpage; tp->tp_next != ശൂന്യ
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

#എങ്കിൽ defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#endif
#ifdef FEAT_JOB_CHANNEL
	entering_window(curwin);
#endif
#എങ്കിൽ defined(FEAT_TABPANEL)
	എങ്കിൽ (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    പൂർണ്ണസംഖ്യ		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		എങ്കിൽ (tp2 != curtab)
		{
		    frame_new_width(tp2->tp_topframe, w, FALSE, TRUE);
		    എങ്കിൽ (!frame_check_width(tp2->tp_topframe, w))
			frame_new_width(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#endif
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	തിരികെ OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    തിരികെ FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
may_open_tabpage(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		n = (cmdmod.cmod_tab == 0)
				       ? postponed_split_tab : cmdmod.cmod_tab;

    എങ്കിൽ (n == 0)
	തിരികെ FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_split_tab = 0;
    തിരികെ win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    പൂർണ്ണസംഖ്യ
make_tabpages(പൂർണ്ണസംഖ്യ maxcount)
{
    പൂർണ്ണസംഖ്യ		എണ്ണം = maxcount;
    പൂർണ്ണസംഖ്യ		todo;

    // Limit to 'tabpagemax' tabs.
    എങ്കിൽ (എണ്ണം > p_tpm)
	എണ്ണം = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    വേണ്ടി (todo = എണ്ണം - 1; todo > 0; --todo)
	എങ്കിൽ (win_new_tabpage(0) == FAIL)
	    നിർത്തുക;

    unblock_autocmds();

    // return actual number of tab pages
    തിരികെ (എണ്ണം - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    പൂർണ്ണസംഖ്യ
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	എങ്കിൽ (tp == tpc)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    പൂർണ്ണസംഖ്യ
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	എങ്കിൽ (tp == tpc)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		എങ്കിൽ (win_valid_any_tab(wp))
		    തിരികെ TRUE;
	    }
	    തിരികെ FALSE;
	}
    }
    // shouldn't happen
    തിരികെ FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    ശൂന്യം
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    എങ്കിൽ (tab == first_tabpage)
    {
	first_tabpage = tab->tp_next;
	ptp = first_tabpage;
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (ptp = first_tabpage; ptp != ശൂന്യ && ptp->tp_next != tab;
							    ptp = ptp->tp_next)
	    ;
	പ്രതിജ്ഞാ(ptp != ശൂന്യ);
	ptp->tp_next = tab->tp_next;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
find_tabpage(പൂർണ്ണസംഖ്യ n)
{
    tabpage_T	*tp;
    പൂർണ്ണസംഖ്യ		i = 1;

    എങ്കിൽ (n == 0)
	തിരികെ curtab;

    വേണ്ടി (tp = first_tabpage; tp != ശൂന്യ && i != n; tp = tp->tp_next)
	++i;
    തിരികെ tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    പൂർണ്ണസംഖ്യ
tabpage_index(tabpage_T *ftp)
{
    പൂർണ്ണസംഖ്യ		i = 1;
    tabpage_T	*tp;

    വേണ്ടി (tp = first_tabpage; tp != ശൂന്യ && tp != ftp; tp = tp->tp_next)
	++i;
    തിരികെ i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    പൂർണ്ണസംഖ്യ		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ifdef FEAT_JOB_CHANNEL
    leaving_window(curwin);
#endif
    reset_VIsual_and_resel();	// stop Visual mode
    എങ്കിൽ (trigger_leave_autocmds)
    {
	എങ്കിൽ (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	    എങ്കിൽ (curtab != tp)
		തിരികെ FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	എങ്കിൽ (curtab != tp)
	    തിരികെ FAIL;
	apply_autocmds(EVENT_TABLEAVE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	എങ്കിൽ (curtab != tp)
	    തിരികെ FAIL;
    }

    reset_dragwin();
#എങ്കിൽ defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    എങ്കിൽ (gui.in_use)
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
    എങ്കിൽ (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_width;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = ശൂന്യ;
    lastwin = ശൂന്യ;
    തിരികെ OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    സ്ഥിതം ശൂന്യം
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    പൂർണ്ണസംഖ്യ		trigger_enter_autocmds,
    പൂർണ്ണസംഖ്യ		trigger_leave_autocmds)
{
    പൂർണ്ണസംഖ്യ		old_off = tp->tp_firstwin->w_winrow;
    win_T	*next_prevwin = tp->tp_prevwin;
    tabpage_T	*last_tab = curtab;

    use_tabpage(tp);

    എങ്കിൽ (p_ch != curtab->tp_ch_used)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	പൂർണ്ണസംഖ്യ new_ch = curtab->tp_ch_used;
	curtab->tp_ch_used = p_ch;
	command_frame_height = FALSE;
	set_option_മൂല്യം((char_u *)"cmdheight", new_ch, ശൂന്യ, 0);
	command_frame_height = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (ശൂന്യം)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
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
    എങ്കിൽ (curtab->tp_old_Rows != ROWS_AVAIL || (old_off != firstwin->w_winrow
#ifdef FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#endif
		))
	shell_new_rows();
    എങ്കിൽ (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	എങ്കിൽ (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_width;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	അല്ലെങ്കിൽ
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = last_tab;

#എങ്കിൽ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    എങ്കിൽ (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	എങ്കിൽ (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    ശൂന്യം
goto_tabpage(പൂർണ്ണസംഖ്യ n)
{
    tabpage_T	*tp = ശൂന്യ;  // shut up compiler
    tabpage_T	*ttp;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (text_locked())
    {
	// Not allowed when editing the command line.
	text_locked_സന്ദേശം();
	തിരികെ;
    }

    // If there is only one it can't work.
    എങ്കിൽ (first_tabpage->tp_next == ശൂന്യ)
    {
	എങ്കിൽ (n > 1)
	    beep_flush();
	തിരികെ;
    }

    എങ്കിൽ (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	എങ്കിൽ (curtab->tp_next == ശൂന്യ)
	    tp = first_tabpage;
	അല്ലെങ്കിൽ
	    tp = curtab->tp_next;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	വേണ്ടി (i = n; i < 0; ++i)
	{
	    വേണ്ടി (tp = first_tabpage; tp->tp_next != ttp && tp->tp_next != ശൂന്യ;
		    tp = tp->tp_next)
		;
	    ttp = tp;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n == 9999)
    {
	// Go to last tab page.
	വേണ്ടി (tp = first_tabpage; tp->tp_next != ശൂന്യ; tp = tp->tp_next)
	    ;
    }
    അല്ലെങ്കിൽ
    {
	// Go to tab page "n".
	tp = find_tabpage(n);
	എങ്കിൽ (tp == ശൂന്യ)
	{
	    beep_flush();
	    തിരികെ;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ifdef FEAT_GUI_TABLINE
    എങ്കിൽ (gui_use_tabline())
	gui_mch_set_curtab(tabpage_index(curtab));
#endif
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    ശൂന്യം
goto_tabpage_tp(
    tabpage_T	*tp,
    പൂർണ്ണസംഖ്യ		trigger_enter_autocmds,
    പൂർണ്ണസംഖ്യ		trigger_leave_autocmds)
{
    എങ്കിൽ (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    set_keep_സന്ദേശം(ശൂന്യ, 0);

    skip_win_fix_scroll = TRUE;
    എങ്കിൽ (tp != curtab && leave_tabpage(tp->tp_curwin->w_buffer,
					trigger_leave_autocmds) == OK)
    {
	എങ്കിൽ (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	അല്ലെങ്കിൽ
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    പൂർണ്ണസംഖ്യ
goto_tabpage_lastused(ശൂന്യം)
{
    എങ്കിൽ (!valid_tabpage(lastused_tabpage))
	തിരികെ FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    തിരികെ OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    ശൂന്യം
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    എങ്കിൽ (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ifdef FEAT_GUI_TABLINE
	എങ്കിൽ (gui_use_tabline())
	    gui_mch_set_curtab(tabpage_index(curtab));
#endif
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    ശൂന്യം
tabpage_move(പൂർണ്ണസംഖ്യ nr)
{
    പൂർണ്ണസംഖ്യ		n = 1;
    tabpage_T	*tp, *tp_dst;

    എങ്കിൽ (first_tabpage->tp_next == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (tabpage_move_disallowed)
	തിരികെ;

    വേണ്ടി (tp = first_tabpage; tp->tp_next != ശൂന്യ && n < nr; tp = tp->tp_next)
	++n;

    എങ്കിൽ (tp == curtab || (nr > 0 && tp->tp_next != ശൂന്യ
						    && tp->tp_next == curtab))
	തിരികെ;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    എങ്കിൽ (curtab == first_tabpage)
	first_tabpage = curtab->tp_next;
    അല്ലെങ്കിൽ
    {
	FOR_ALL_TABPAGES(tp)
	    എങ്കിൽ (tp->tp_next == curtab)
		നിർത്തുക;
	എങ്കിൽ (tp == ശൂന്യ)	// "cannot happen"
	    തിരികെ;
	tp->tp_next = curtab->tp_next;
    }

    // Re-insert it at the specified position.
    എങ്കിൽ (nr <= 0)
    {
	curtab->tp_next = first_tabpage;
	first_tabpage = curtab;
    }
    അല്ലെങ്കിൽ
    {
	curtab->tp_next = tp_dst->tp_next;
	tp_dst->tp_next = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#എങ്കിൽ defined(FEAT_TABPANEL)
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
    ശൂന്യം
win_goto(win_T *wp)
{
#ifdef FEAT_CONCEAL
    win_T	*owp = curwin;
#endif

#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;
    എങ്കിൽ (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_to_enter_popup_window));
	തിരികെ;
    }
#endif
    എങ്കിൽ (text_or_buf_locked())
    {
	beep_flush();
	തിരികെ;
    }

    എങ്കിൽ (wp->w_buffer != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    അല്ലെങ്കിൽ എങ്കിൽ (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    എങ്കിൽ (!win_valid(wp))
	തിരികെ;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    win_enter(wp, TRUE);

#ifdef FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    എങ്കിൽ (win_valid(owp) && owp->w_p_cole > 0 && !സന്ദേശം_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    എങ്കിൽ (curwin->w_p_cole > 0 && !സന്ദേശം_scrolled)
	need_cursor_വരി_redraw = TRUE;
#endif
}

#എങ്കിൽ defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_find_nr(പൂർണ്ണസംഖ്യ winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (--winnr == 0)
	    നിർത്തുക;
    തിരികെ wp;
}
#endif

#എങ്കിൽ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_find_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    എങ്കിൽ (wp == win)
		തിരികെ tp;
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, പൂർണ്ണസംഖ്യ up, long എണ്ണം)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (popup_is_popup(wp))
	// popups don't have neighbors.
	തിരികെ ശൂന്യ;
#endif
    foundfr = wp->w_frame;
    ആകുമ്പോൾ (എണ്ണം--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (fr == tp->tp_topframe)
		പോകുക end;
	    എങ്കിൽ (up)
		nfr = fr->fr_prev;
	    അല്ലെങ്കിൽ
		nfr = fr->fr_next;
	    എങ്കിൽ (fr->fr_parent->fr_layout == FR_COL && nfr != ശൂന്യ)
		നിർത്തുക;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		നിർത്തുക;
	    }
	    fr = nfr->fr_child;
	    എങ്കിൽ (nfr->fr_layout == FR_ROW)
	    {
		// Find the frame at the cursor row.
		ആകുമ്പോൾ (fr->fr_next != ശൂന്യ
			&& frame2win(fr)->w_wincol + fr->fr_width
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_next;
	    }
	    എങ്കിൽ (nfr->fr_layout == FR_COL && up)
		ആകുമ്പോൾ (fr->fr_next != ശൂന്യ)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    തിരികെ foundfr != ശൂന്യ ? foundfr->fr_win : ശൂന്യ;
}

/*
 * Move to window above or below "count" times.
 */
    സ്ഥിതം ശൂന്യം
win_goto_ver(
    പൂർണ്ണസംഖ്യ		up,		// TRUE to go to win above
    long	എണ്ണം)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (ERROR_IF_TERM_POPUP_WINDOW)
	തിരികെ;
#endif
    win = win_vert_neighbor(curtab, curwin, up, എണ്ണം);
    എങ്കിൽ (win != ശൂന്യ)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, പൂർണ്ണസംഖ്യ ഇടത്, long എണ്ണം)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (popup_is_popup(wp))
	// popups don't have neighbors.
	തിരികെ ശൂന്യ;
#endif
    foundfr = wp->w_frame;
    ആകുമ്പോൾ (എണ്ണം--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (fr == tp->tp_topframe)
		പോകുക end;
	    എങ്കിൽ (ഇടത്)
		nfr = fr->fr_prev;
	    അല്ലെങ്കിൽ
		nfr = fr->fr_next;
	    എങ്കിൽ (fr->fr_parent->fr_layout == FR_ROW && nfr != ശൂന്യ)
		നിർത്തുക;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		നിർത്തുക;
	    }
	    fr = nfr->fr_child;
	    എങ്കിൽ (nfr->fr_layout == FR_COL)
	    {
		// Find the frame at the cursor row.
		ആകുമ്പോൾ (fr->fr_next != ശൂന്യ
			&& frame2win(fr)->w_winrow + fr->fr_height
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_next;
	    }
	    എങ്കിൽ (nfr->fr_layout == FR_ROW && ഇടത്)
		ആകുമ്പോൾ (fr->fr_next != ശൂന്യ)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    തിരികെ foundfr != ശൂന്യ ? foundfr->fr_win : ശൂന്യ;
}

/*
 * Move to left or right window.
 */
    സ്ഥിതം ശൂന്യം
win_goto_hor(
    പൂർണ്ണസംഖ്യ		ഇടത്,		// TRUE to go to left win
    long	എണ്ണം)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (ERROR_IF_TERM_POPUP_WINDOW)
	തിരികെ;
#endif
    win = win_horz_neighbor(curtab, curwin, ഇടത്, എണ്ണം);
    എങ്കിൽ (win != ശൂന്യ)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    ശൂന്യം
win_enter(win_T *wp, പൂർണ്ണസംഖ്യ undo_sync)
{
    (ശൂന്യം)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    ശൂന്യം
win_fix_current_dir(ശൂന്യം)
{
    എങ്കിൽ (curwin->w_localdir != ശൂന്യ || curtab->tp_localdir != ശൂന്യ)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	എങ്കിൽ (globaldir == ശൂന്യ)
	{
	    char_u	cwd[MAXPATHL];

	    എങ്കിൽ (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	എങ്കിൽ (curwin->w_localdir != ശൂന്യ)
	    dirname = curwin->w_localdir;
	അല്ലെങ്കിൽ
	    dirname = curtab->tp_localdir;

	എങ്കിൽ (mch_chdir((അക്ഷരം *)dirname) == 0)
	{
	    last_chdir_reason = ശൂന്യ;
	    shorten_fnames(TRUE);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (globaldir != ശൂന്യ)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((അക്ഷരം *)globaldir);
	VIM_CLEAR(globaldir);
	last_chdir_reason = ശൂന്യ;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
win_enter_ext(win_T *wp, പൂർണ്ണസംഖ്യ flags)
{
    പൂർണ്ണസംഖ്യ		other_buffer = FALSE;
    പൂർണ്ണസംഖ്യ		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    പൂർണ്ണസംഖ്യ		did_decrement = FALSE;

    എങ്കിൽ (wp == curwin && curwin_invalid == 0)	// nothing to do
	തിരികെ FALSE;

#ifdef FEAT_JOB_CHANNEL
    എങ്കിൽ (curwin_invalid == 0)
	leaving_window(curwin);
#endif

    എങ്കിൽ (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	എങ്കിൽ (wp->w_buffer != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	    other_buffer = TRUE;
	    എങ്കിൽ (!win_valid(wp))
		തിരികെ FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	എങ്കിൽ (!win_valid(wp))
	    തിരികെ FALSE;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	എങ്കിൽ (aborting())
	    തിരികെ FALSE;
#endif
    }

    // sync undo before leaving the current buffer
    എങ്കിൽ ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_buffer)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    എങ്കിൽ (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    എങ്കിൽ (wp->w_buffer != curbuf)
	buf_copy_options(wp->w_buffer, BCO_ENTER | BCO_NOHELP);
    എങ്കിൽ (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_status = true;
    }
    curwin = wp;
    curbuf = wp->w_buffer;
    check_cursor();
    എങ്കിൽ (!virtual_active())
	curwin->w_cursor.coladd = 0;
    എങ്കിൽ (*p_spk == 'c')		// assume cursor position needs updating
	changed_വരി_abv_curs();
    അല്ലെങ്കിൽ
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ifdef MESSAGE_QUEUE
    എങ്കിൽ (flags & WEE_ALLOW_PARSE_MESSAGES)
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
    എങ്കിൽ (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    എങ്കിൽ (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	എങ്കിൽ (other_buffer)
	    apply_autocmds(EVENT_BUFENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_status = true;
#ifdef FEAT_TERMINAL
    എങ്കിൽ (bt_terminal(curwin->w_buffer))
	// terminal is likely in another mode
	redraw_mode = TRUE;
#endif
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#എങ്കിൽ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    എങ്കിൽ (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    എങ്കിൽ (curwin->w_height < p_wh && !curwin->w_p_wfh
#ifdef FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#endif
	    )
	win_setheight((പൂർണ്ണസംഖ്യ)p_wh);
    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_height == 0)
	win_setheight(1);

    // set window width to desired minimal value
    എങ്കിൽ (curwin->w_width < p_wiw && !curwin->w_p_wfw)
	win_setwidth((പൂർണ്ണസംഖ്യ)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    തിരികെ did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = ശൂന്യ;

    എങ്കിൽ (curwin->w_buffer == buf)
	wp = curwin;
    അല്ലെങ്കിൽ
	FOR_ALL_WINDOWS(wp)
	    എങ്കിൽ (wp->w_buffer == buf)
		നിർത്തുക;
    എങ്കിൽ (wp != ശൂന്യ)
	win_enter(wp, FALSE);
    തിരികെ wp;
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

    എങ്കിൽ (wp != ശൂന്യ)
	തിരികെ wp;

    FOR_ALL_TABPAGES(tp)
	എങ്കിൽ (tp != curtab)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		എങ്കിൽ (wp->w_buffer == buf)
		    നിർത്തുക;
	    എങ്കിൽ (wp != ശൂന്യ)
	    {
		goto_tabpage_win(tp, wp);
		എങ്കിൽ (curwin != wp)
		    wp = ശൂന്യ;	// something went wrong
		നിർത്തുക;
	    }
	}
    തിരികെ wp;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ last_win_id = LOWEST_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    സ്ഥിതം win_T *
win_alloc(win_T *after, പൂർണ്ണസംഖ്യ hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    എങ്കിൽ (new_wp == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (win_alloc_വരികൾ(new_wp) == FAIL)
    {
	vim_free(new_wp);
	തിരികെ ശൂന്യ;
    }

    new_wp->w_id = ++last_win_id;

#ifdef FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    എങ്കിൽ (new_wp->w_vars == ശൂന്യ)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	തിരികെ ശൂന്യ;
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
    എങ്കിൽ (!hidden)
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
    എങ്കിൽ (gui.in_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ഇടത്],
		SBAR_ഇടത്, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_വലത്],
		SBAR_വലത്, new_wp);
    }
#endif
#ifdef FEAT_FOLDING
    foldInitWin(new_wp);
#endif
    unblock_autocmds();
#ifdef FEAT_തിരയൽ_EXTRA
    new_wp->w_next_match_id = 1000;  // up to 1000 can be picked by the user
#endif
    തിരികെ new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    സ്ഥിതം ശൂന്യം
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    പൂർണ്ണസംഖ്യ		i;
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
    എങ്കിൽ (wp->w_stl_click != ശൂന്യ)
    {
	വേണ്ടി (i = 0; i < wp->w_stl_click_എണ്ണം; i++)
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

	എങ്കിൽ (prevwin == wp)
	    prevwin = ശൂന്യ;
	FOR_ALL_TABPAGES(ttp)
	    എങ്കിൽ (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = ശൂന്യ;
    }
    win_free_lsize(wp);

    വേണ്ടി (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    എങ്കിൽ (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    എങ്കിൽ (wip2 != wip && wip2->wi_win == ശൂന്യ)
		    {
			എങ്കിൽ (wip2->wi_next != ശൂന്യ)
			    wip2->wi_next->wi_prev = wip2->wi_prev;
			എങ്കിൽ (wip2->wi_prev == ശൂന്യ)
			    buf->b_wininfo = wip2->wi_next;
			അല്ലെങ്കിൽ
			    wip2->wi_prev->wi_next = wip2->wi_next;
			free_wininfo(wip2);
			നിർത്തുക;
		    }

		wip->wi_win = ശൂന്യ;
	    }

#ifdef FEAT_തിരയൽ_EXTRA
    clear_matches(wp);
#endif

    free_jumplist(wp);

#ifdef FEAT_QUICKFIX
    qf_free_all(wp);
#endif

#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ഇടത്]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_വലത്]);
    }
#endif // FEAT_GUI

#ifdef FEAT_MENU
    remove_winbar(wp);
#endif
#ifdef FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    വേണ്ടി (i = 0; i < 4; ++i)
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

    എങ്കിൽ (win_valid_any_tab(wp))
	win_remove(wp, tp);
    എങ്കിൽ (autocmd_busy)
    {
	wp->w_next = au_pending_free_win;
	au_pending_free_win = wp;
    }
    അല്ലെങ്കിൽ
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    പൂർണ്ണസംഖ്യ
win_unlisted(win_T *wp)
{
    തിരികെ is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#എങ്കിൽ defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    ശൂന്യം
win_free_popup(win_T *win)
{
    എങ്കിൽ (win->w_buffer != ശൂന്യ)
    {
	എങ്കിൽ (bt_popup(win->w_buffer))
	    win_close_buffer(win, DOBUF_WIPE_REUSE, FALSE);
	അല്ലെങ്കിൽ
	    close_buffer(win, win->w_buffer, 0, FALSE, FALSE, TRUE);

	എങ്കിൽ (win->w_buffer != ശൂന്യ)
	    --win->w_buffer->b_nwindows;
    }
# എങ്കിൽ defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    എങ്കിൽ (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_frame);
    win_free(win, ശൂന്യ);
}
#endif

/*
 * Append window "wp" in the window list after window "after".
 */
    സ്ഥിതം ശൂന്യം
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    എങ്കിൽ (after == ശൂന്യ)	    // after NULL is in front of the first
	before = firstwin;
    അല്ലെങ്കിൽ
	before = after->w_next;

    wp->w_next = before;
    wp->w_prev = after;
    എങ്കിൽ (after == ശൂന്യ)
	firstwin = wp;
    അല്ലെങ്കിൽ
	after->w_next = wp;
    എങ്കിൽ (before == ശൂന്യ)
	lastwin = wp;
    അല്ലെങ്കിൽ
	before->w_prev = wp;
}

/*
 * Remove a window from the window list.
 */
    ശൂന്യം
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    എങ്കിൽ (wp->w_prev != ശൂന്യ)
	wp->w_prev->w_next = wp->w_next;
    അല്ലെങ്കിൽ എങ്കിൽ (tp == ശൂന്യ)
	firstwin = curtab->tp_firstwin = wp->w_next;
    അല്ലെങ്കിൽ
	tp->tp_firstwin = wp->w_next;

    എങ്കിൽ (wp->w_next != ശൂന്യ)
	wp->w_next->w_prev = wp->w_prev;
    അല്ലെങ്കിൽ എങ്കിൽ (tp == ശൂന്യ)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    അല്ലെങ്കിൽ
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    സ്ഥിതം ശൂന്യം
frame_append(frame_T *after, frame_T *frp)
{
    frp->fr_next = after->fr_next;
    after->fr_next = frp;
    എങ്കിൽ (frp->fr_next != ശൂന്യ)
	frp->fr_next->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    സ്ഥിതം ശൂന്യം
frame_തിരുകുക(frame_T *before, frame_T *frp)
{
    frp->fr_next = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    എങ്കിൽ (frp->fr_prev != ശൂന്യ)
	frp->fr_prev->fr_next = frp;
    അല്ലെങ്കിൽ
	frp->fr_parent->fr_child = frp;
}

/*
 * Remove a frame from a frame list.
 */
    സ്ഥിതം ശൂന്യം
frame_remove(frame_T *frp)
{
    എങ്കിൽ (frp->fr_prev != ശൂന്യ)
	frp->fr_prev->fr_next = frp->fr_next;
    അല്ലെങ്കിൽ
	frp->fr_parent->fr_child = frp->fr_next;
    എങ്കിൽ (frp->fr_next != ശൂന്യ)
	frp->fr_next->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    പൂർണ്ണസംഖ്യ
win_alloc_വരികൾ(win_T *wp)
{
    wp->w_വരികൾ_valid = 0;
    wp->w_വരികൾ = ALLOC_CLEAR_MULT(wline_T, Rows);
    എങ്കിൽ (wp->w_വരികൾ == ശൂന്യ)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * free lsize arrays for a window
 */
    ശൂന്യം
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    എങ്കിൽ (wp != ശൂന്യ)
	VIM_CLEAR(wp->w_വരികൾ);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    ശൂന്യം
shell_new_rows(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		h = (പൂർണ്ണസംഖ്യ)ROWS_AVAIL;

    എങ്കിൽ (firstwin == ശൂന്യ)	// not initialized yet
	തിരികെ;
    എങ്കിൽ (h < frame_minheight(topframe, ശൂന്യ))
	h = frame_minheight(topframe, ശൂന്യ);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    frame_new_height(topframe, h, FALSE, TRUE, FALSE);
    എങ്കിൽ (!frame_check_height(topframe, h))
	frame_new_height(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_used = p_ch;

    എങ്കിൽ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#എങ്കിൽ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#എങ്കിൽ 0
    // Disabled: don't want making the screen smaller make a window larger.
    എങ്കിൽ (p_ea)
	win_equal(curwin, FALSE, 'v');
#endif
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    ശൂന്യം
shell_new_columns(ശൂന്യം)
{
    എങ്കിൽ (firstwin == ശൂന്യ)	// not initialized yet
	തിരികെ;

#എങ്കിൽ defined(FEAT_TABPANEL)
    സ്ഥിതം പൂർണ്ണസംഖ്യ prev_tp_width;
    സ്ഥിതം പൂർണ്ണസംഖ്യ prev_wincol;
    സ്ഥിതം പൂർണ്ണസംഖ്യ prev_fr_width;
#endif
    പൂർണ്ണസംഖ്യ w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    frame_new_width(topframe, w, FALSE, TRUE);
    എങ്കിൽ (!frame_check_width(topframe, w))
	frame_new_width(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#എങ്കിൽ defined(FEAT_TABPANEL)
    പൂർണ്ണസംഖ്യ tp_width = tabpanel_width();
    പൂർണ്ണസംഖ്യ tp_width_changed = tp_width != prev_tp_width;
    // tabpanel width changed
    എങ്കിൽ (tp_width_changed && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    എങ്കിൽ (tp_width_changed
	    || (tp_width > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_width != prev_fr_width)))
    {
	screen_fill(cmdline_row, (പൂർണ്ണസംഖ്യ)Rows, 0, (പൂർണ്ണസംഖ്യ)Columns, ' ', ' ', 0);
    }
    prev_tp_width = tabpanel_width();
    prev_wincol = firstwin->w_wincol;
    prev_fr_width = topframe->fr_width;
#endif
    // Adjust offset for command line start column
    cmdline_col_off = firstwin->w_wincol;
    cmdline_width = topframe->fr_width;
    comp_col();
    എങ്കിൽ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#എങ്കിൽ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#എങ്കിൽ 0
    // Disabled: don't want making the screen smaller make a window larger.
    എങ്കിൽ (p_ea)
	win_equal(curwin, FALSE, 'h');
#endif
}

/*
 * Save the size of all windows in "gap".
 */
    ശൂന്യം
win_size_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, വലുപ്പം(പൂർണ്ണസംഖ്യ), 1);
    എങ്കിൽ (ga_grow(gap, win_എണ്ണം() * 2 + 1) == FAIL)
	തിരികെ;

    // first entry is the total lines available for windows
    ((പൂർണ്ണസംഖ്യ *)gap->ga_data)[gap->ga_len++] = ROWS_AVAIL - last_stl_height(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((പൂർണ്ണസംഖ്യ *)gap->ga_data)[gap->ga_len++] =
	    wp->w_width + wp->w_vsep_width;
	((പൂർണ്ണസംഖ്യ *)gap->ga_data)[gap->ga_len++] = wp->w_height;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    ശൂന്യം
win_size_restore(garray_T *gap)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		i, j;

    എങ്കിൽ (win_എണ്ണം() * 2 + 1 == gap->ga_len
	    && ((പൂർണ്ണസംഖ്യ *)gap->ga_data)[0] == ROWS_AVAIL - last_stl_height(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	വേണ്ടി (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		frame_setwidth(wp->w_frame, ((പൂർണ്ണസംഖ്യ *)gap->ga_data)[i++]);
		win_setheight_win(((പൂർണ്ണസംഖ്യ *)gap->ga_data)[i++], wp);
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
    ശൂന്യം
win_comp_pos(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		row = tabline_height();
    പൂർണ്ണസംഖ്യ		col = TPL_LCOL();

    frame_comp_pos(topframe, &row, &col);
#എങ്കിൽ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    സ്ഥിതം ശൂന്യം
frame_comp_pos(frame_T *topfrp, പൂർണ്ണസംഖ്യ *row, പൂർണ്ണസംഖ്യ *col)
{
    win_T	*wp;
    frame_T	*frp;
    പൂർണ്ണസംഖ്യ		startcol;
    പൂർണ്ണസംഖ്യ		startrow;
    പൂർണ്ണസംഖ്യ		h;

    wp = topfrp->fr_win;
    എങ്കിൽ (wp != ശൂന്യ)
    {
	എങ്കിൽ (wp->w_winrow != *row || wp->w_wincol != *col)
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
    അല്ലെങ്കിൽ
    {
	startrow = *row;
	startcol = *col;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    എങ്കിൽ (topfrp->fr_layout == FR_ROW)
		*row = startrow;	// all frames are at the same row
	    അല്ലെങ്കിൽ
		*col = startcol;	// all frames are at the same col
	    frame_comp_pos(frp, row, col);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    ശൂന്യം
win_ensure_size(ശൂന്യം)
{
    എങ്കിൽ (curwin->w_height == 0)
	win_setheight(1);
    എങ്കിൽ (curwin->w_width == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    ശൂന്യം
win_setheight(പൂർണ്ണസംഖ്യ height)
{
    win_setheight_win(height, curwin);
#എങ്കിൽ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    ശൂന്യം
win_setheight_win(പൂർണ്ണസംഖ്യ height, win_T *win)
{
    എങ്കിൽ (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	എങ്കിൽ (height < p_wmh)
	    height = p_wmh;
	എങ്കിൽ (height == 0)
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
    സ്ഥിതം ശൂന്യം
frame_setheight(frame_T *curfrp, പൂർണ്ണസംഖ്യ height)
{
    പൂർണ്ണസംഖ്യ		room;		// total number of lines available
    പൂർണ്ണസംഖ്യ		take;		// number of lines taken from other windows
    പൂർണ്ണസംഖ്യ		room_cmdline;	// lines available from cmdline
    പൂർണ്ണസംഖ്യ		run;
    frame_T	*frp;
    പൂർണ്ണസംഖ്യ		h;
    പൂർണ്ണസംഖ്യ		room_reserved;

    // If the height already is the desired value, nothing to do.
    എങ്കിൽ (curfrp->fr_height == height)
	തിരികെ;

    എങ്കിൽ (curfrp->fr_parent == ശൂന്യ)
    {
	// topframe: can only change the command line height
	എങ്കിൽ (height > 0)
	    frame_new_height(curfrp, height, FALSE, FALSE, TRUE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curfrp->fr_parent->fr_layout == FR_ROW)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = frame_minheight(curfrp->fr_parent, ശൂന്യ);
	എങ്കിൽ (height < h)
	    height = h;
	frame_setheight(curfrp->fr_parent, height);
    }
    അല്ലെങ്കിൽ
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
	വേണ്ടി (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		എങ്കിൽ (frp != curfrp
			&& frp->fr_win != ശൂന്യ
			&& frp->fr_win->w_p_wfh)
		    room_reserved += frp->fr_height;
		room += frp->fr_height;
		എങ്കിൽ (frp != curfrp)
		    room -= frame_minheight(frp, ശൂന്യ);
	    }
	    എങ്കിൽ (curfrp->fr_width != topframe->fr_width)
		room_cmdline = 0;
	    അല്ലെങ്കിൽ
	    {
		room_cmdline = Rows - p_ch - (lastwin->w_winrow
						+ VISIBLE_HEIGHT(lastwin)
						+ lastwin->w_status_height);
		എങ്കിൽ (room_cmdline < 0)
		    room_cmdline = 0;
	    }

	    എങ്കിൽ (height <= room + room_cmdline)
		നിർത്തുക;
	    എങ്കിൽ (run == 2 || curfrp->fr_width == topframe->fr_width)
	    {
		height = room + room_cmdline;
		നിർത്തുക;
	    }
	    frame_setheight(curfrp->fr_parent, height
		+ frame_minheight(curfrp->fr_parent, NOWIN) - (പൂർണ്ണസംഖ്യ)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = height - curfrp->fr_height;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	എങ്കിൽ (height > room + room_cmdline - room_reserved)
	    room_reserved = room + room_cmdline - height;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	എങ്കിൽ (take < 0 && room - curfrp->fr_height <= room_reserved)
	    room_reserved = 0;

	എങ്കിൽ (take > 0 && room_cmdline > 0)
	{
	    // use lines from cmdline first
	    എങ്കിൽ (take < room_cmdline)
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
	വേണ്ടി (run = 0; run < 2; ++run)
	{
	    എങ്കിൽ (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    അല്ലെങ്കിൽ
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    ആകുമ്പോൾ (frp != ശൂന്യ && take != 0)
	    {
		h = frame_minheight(frp, ശൂന്യ);
		എങ്കിൽ (room_reserved > 0
			&& frp->fr_win != ശൂന്യ
			&& frp->fr_win->w_p_wfh)
		{
		    എങ്കിൽ (room_reserved >= frp->fr_height)
			room_reserved -= frp->fr_height;
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (frp->fr_height - room_reserved > take)
			    room_reserved = frp->fr_height - take;
			take -= frp->fr_height - room_reserved;
			frame_new_height(frp, room_reserved, FALSE, FALSE,
									TRUE);
			room_reserved = 0;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (frp->fr_height - take < h)
		    {
			take -= frp->fr_height - h;
			frame_new_height(frp, h, FALSE, FALSE, TRUE);
		    }
		    അല്ലെങ്കിൽ
		    {
			frame_new_height(frp, frp->fr_height - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		എങ്കിൽ (run == 0)
		    frp = frp->fr_next;
		അല്ലെങ്കിൽ
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    ശൂന്യം
win_setwidth(പൂർണ്ണസംഖ്യ width)
{
    win_setwidth_win(width, curwin);
}

    ശൂന്യം
win_setwidth_win(പൂർണ്ണസംഖ്യ width, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    എങ്കിൽ (wp == curwin)
    {
	എങ്കിൽ (width < p_wmw)
	    width = p_wmw;
	എങ്കിൽ (width == 0)
	    width = 1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (width < 0)
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
    സ്ഥിതം ശൂന്യം
frame_setwidth(frame_T *curfrp, പൂർണ്ണസംഖ്യ width)
{
    പൂർണ്ണസംഖ്യ		room;		// total number of lines available
    പൂർണ്ണസംഖ്യ		take;		// number of lines taken from other windows
    പൂർണ്ണസംഖ്യ		run;
    frame_T	*frp;
    പൂർണ്ണസംഖ്യ		w;
    പൂർണ്ണസംഖ്യ		room_reserved;

    // If the width already is the desired value, nothing to do.
    എങ്കിൽ (curfrp->fr_width == width)
	തിരികെ;

    എങ്കിൽ (curfrp->fr_parent == ശൂന്യ)
	// topframe: can't change width
	തിരികെ;

    എങ്കിൽ (curfrp->fr_parent->fr_layout == FR_COL)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = frame_minwidth(curfrp->fr_parent, ശൂന്യ);
	എങ്കിൽ (width < w)
	    width = w;
	frame_setwidth(curfrp->fr_parent, width);
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	വേണ്ടി (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		എങ്കിൽ (frp != curfrp
			&& frp->fr_win != ശൂന്യ
			&& frp->fr_win->w_p_wfw)
		    room_reserved += frp->fr_width;
		room += frp->fr_width;
		എങ്കിൽ (frp != curfrp)
		    room -= frame_minwidth(frp, ശൂന്യ);
	    }

	    എങ്കിൽ (width <= room)
		നിർത്തുക;
	    എങ്കിൽ (run == 2 || curfrp->fr_height >= ROWS_AVAIL)
	    {
		width = room;
		നിർത്തുക;
	    }
	    frame_setwidth(curfrp->fr_parent, width
		 + frame_minwidth(curfrp->fr_parent, NOWIN) - (പൂർണ്ണസംഖ്യ)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = width - curfrp->fr_width;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	എങ്കിൽ (width > room - room_reserved)
	    room_reserved = room - width;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	എങ്കിൽ (take < 0 && room - curfrp->fr_width < room_reserved)
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
	വേണ്ടി (run = 0; run < 2; ++run)
	{
	    എങ്കിൽ (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    അല്ലെങ്കിൽ
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    ആകുമ്പോൾ (frp != ശൂന്യ && take != 0)
	    {
		w = frame_minwidth(frp, ശൂന്യ);
		എങ്കിൽ (room_reserved > 0
			&& frp->fr_win != ശൂന്യ
			&& frp->fr_win->w_p_wfw)
		{
		    എങ്കിൽ (room_reserved >= frp->fr_width)
			room_reserved -= frp->fr_width;
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (frp->fr_width - room_reserved > take)
			    room_reserved = frp->fr_width - take;
			take -= frp->fr_width - room_reserved;
			frame_new_width(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (frp->fr_width - take < w)
		    {
			take -= frp->fr_width - w;
			frame_new_width(frp, w, FALSE, FALSE);
		    }
		    അല്ലെങ്കിൽ
		    {
			frame_new_width(frp, frp->fr_width - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		എങ്കിൽ (run == 0)
		    frp = frp->fr_next;
		അല്ലെങ്കിൽ
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    ശൂന്യം
win_setminheight(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		room;
    പൂർണ്ണസംഖ്യ		needed;
    പൂർണ്ണസംഖ്യ		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    ആകുമ്പോൾ (p_wmh > 0)
    {
	room = Rows - p_ch;
	needed = min_rows_for_all_tabpages() - 1;  // 1 was added for the cmdline
	എങ്കിൽ (room >= needed)
	    നിർത്തുക;
	--p_wmh;
	എങ്കിൽ (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    ശൂന്യം
win_setminwidth(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		room;
    പൂർണ്ണസംഖ്യ		needed;
    പൂർണ്ണസംഖ്യ		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    ആകുമ്പോൾ (p_wmw > 0)
    {
	room = topframe->fr_width;
	needed = frame_minwidth(topframe, ശൂന്യ);
	എങ്കിൽ (room >= needed)
	    നിർത്തുക;
	--p_wmw;
	എങ്കിൽ (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    ശൂന്യം
win_drag_status_വരി(win_T *dragwin, പൂർണ്ണസംഖ്യ offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    പൂർണ്ണസംഖ്യ		room;
    പൂർണ്ണസംഖ്യ		up;	// if TRUE, drag status line up, otherwise down
    പൂർണ്ണസംഖ്യ		n;

    fr = dragwin->w_frame;
    curfr = fr;
    എങ്കിൽ (fr != topframe)		// more than one window
    {
	fr = fr->fr_parent;
	// When the parent frame is not a column of frames, its parent should
	// be.
	എങ്കിൽ (fr->fr_layout != FR_COL)
	{
	    curfr = fr;
	    എങ്കിൽ (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_parent;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    ആകുമ്പോൾ (curfr != topframe && curfr->fr_next == ശൂന്യ)
    {
	എങ്കിൽ (fr != topframe)
	    fr = fr->fr_parent;
	curfr = fr;
	എങ്കിൽ (fr != topframe)
	    fr = fr->fr_parent;
    }

    എങ്കിൽ (offset < 0) // drag up
    {
	up = TRUE;
	offset = -offset;
	// sum up the room of the current frame and above it
	എങ്കിൽ (fr == curfr)
	{
	    // only one window
	    room = fr->fr_height - frame_minheight(fr, ശൂന്യ);
	}
	അല്ലെങ്കിൽ
	{
	    room = 0;
	    വേണ്ടി (fr = fr->fr_child; ; fr = fr->fr_next)
	    {
		room += fr->fr_height - frame_minheight(fr, ശൂന്യ);
		എങ്കിൽ (fr == curfr)
		    നിർത്തുക;
	    }
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    അല്ലെങ്കിൽ    // drag down
    {
	up = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	room = Rows - cmdline_row;
	എങ്കിൽ (curfr->fr_next == ശൂന്യ)
	    --room;
	അല്ലെങ്കിൽ
	    room -= p_ch;
	എങ്കിൽ (room < 0)
	    room = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_height - frame_minheight(fr, ശൂന്യ);
	fr = curfr;			// put fr at window that grows
    }

    എങ്കിൽ (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    എങ്കിൽ (offset <= 0)
	തിരികെ;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    എങ്കിൽ (fr != ശൂന്യ)
	frame_new_height(fr, fr->fr_height + offset, up, FALSE, TRUE);

    എങ്കിൽ (up)
	fr = curfr;		// current frame gets smaller
    അല്ലെങ്കിൽ
	fr = curfr->fr_next;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    ആകുമ്പോൾ (fr != ശൂന്യ && offset > 0)
    {
	n = frame_minheight(fr, ശൂന്യ);
	എങ്കിൽ (fr->fr_height - offset <= n)
	{
	    offset -= fr->fr_height - n;
	    frame_new_height(fr, n, !up, FALSE, TRUE);
	}
	അല്ലെങ്കിൽ
	{
	    frame_new_height(fr, fr->fr_height - offset, !up, FALSE, TRUE);
	    നിർത്തുക;
	}
	എങ്കിൽ (up)
	    fr = fr->fr_prev;
	അല്ലെങ്കിൽ
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
    ശൂന്യം
win_drag_vsep_വരി(win_T *dragwin, പൂർണ്ണസംഖ്യ offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    പൂർണ്ണസംഖ്യ		room;
    പൂർണ്ണസംഖ്യ		ഇടത്;	// if TRUE, drag separator line left, otherwise right
    പൂർണ്ണസംഖ്യ		n;

    fr = dragwin->w_frame;
    എങ്കിൽ (fr == topframe)		// only one window (cannot happen?)
	തിരികെ;
    curfr = fr;
    fr = fr->fr_parent;
    // When the parent frame is not a row of frames, its parent should be.
    എങ്കിൽ (fr->fr_layout != FR_ROW)
    {
	എങ്കിൽ (fr == topframe)	// only a column of windows (cannot happen?)
	    തിരികെ;
	curfr = fr;
	fr = fr->fr_parent;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    ആകുമ്പോൾ (curfr->fr_next == ശൂന്യ)
    {
	എങ്കിൽ (fr == topframe)
	    നിർത്തുക;
	curfr = fr;
	fr = fr->fr_parent;
	എങ്കിൽ (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_parent;
	}
    }

    എങ്കിൽ (offset < 0) // drag left
    {
	ഇടത് = TRUE;
	offset = -offset;
	// sum up the room of the current frame and left of it
	room = 0;
	വേണ്ടി (fr = fr->fr_child; ; fr = fr->fr_next)
	{
	    room += fr->fr_width - frame_minwidth(fr, ശൂന്യ);
	    എങ്കിൽ (fr == curfr)
		നിർത്തുക;
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    അല്ലെങ്കിൽ    // drag right
    {
	ഇടത് = FALSE;
	// sum up the room of frames right of the current one
	room = 0;
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_width - frame_minwidth(fr, ശൂന്യ);
	fr = curfr;			// put fr at window that grows
    }

    എങ്കിൽ (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    എങ്കിൽ (offset <= 0)		// No room at all, quit.
	തിരികെ;
    എങ്കിൽ (fr == ശൂന്യ)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	തിരികെ;

    // grow frame fr by offset lines
    frame_new_width(fr, fr->fr_width + offset, ഇടത്, FALSE);

    // shrink other frames: current and at the left or at the right
    എങ്കിൽ (ഇടത്)
	fr = curfr;		// current frame gets smaller
    അല്ലെങ്കിൽ
	fr = curfr->fr_next;	// next frame gets smaller

    ആകുമ്പോൾ (fr != ശൂന്യ && offset > 0)
    {
	n = frame_minwidth(fr, ശൂന്യ);
	എങ്കിൽ (fr->fr_width - offset <= n)
	{
	    offset -= fr->fr_width - n;
	    frame_new_width(fr, n, !ഇടത്, FALSE);
	}
	അല്ലെങ്കിൽ
	{
	    frame_new_width(fr, fr->fr_width - offset, !ഇടത്, FALSE);
	    നിർത്തുക;
	}
	എങ്കിൽ (ഇടത്)
	    fr = fr->fr_prev;
	അല്ലെങ്കിൽ
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
    ശൂന്യം
set_fraction(win_T *wp)
{
    എങ്കിൽ (wp->w_height > 1)
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
    സ്ഥിതം ശൂന്യം
win_fix_scroll(പൂർണ്ണസംഖ്യ resize)
{
    എങ്കിൽ (*p_spk == 'c')
	തിരികെ;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	എങ്കിൽ (wp->w_height != wp->w_prev_height)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    എങ്കിൽ (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	    {
		പൂർണ്ണസംഖ്യ diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_height - wp->w_prev_height);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		എങ്കിൽ (diff > 0)
		    cursor_down_inner(wp, diff);
		അല്ലെങ്കിൽ
		    cursor_up_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_to_fraction(wp, wp->w_prev_height);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		എങ്കിൽ (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    എങ്കിൽ (!(get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL)))
	win_fix_cursor(FALSE);
    അല്ലെങ്കിൽ എങ്കിൽ (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    സ്ഥിതം ശൂന്യം
win_fix_cursor(പൂർണ്ണസംഖ്യ normal)
{
    win_T	*wp = curwin;

    എങ്കിൽ (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_buffer->b_ml.ml_വരി_എണ്ണം < wp->w_height)
	തിരികെ;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    long so = MIN(wp->w_height / 2, get_scrolloff_മൂല്യം());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_down_inner(wp, so);
    linenr_T മുകൾ = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_up_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    എങ്കിൽ (lnum > bot && (wp->w_botline - wp->w_buffer->b_ml.ml_വരി_എണ്ണം) != 1)
	nlnum = bot;
    അല്ലെങ്കിൽ എങ്കിൽ (lnum < മുകൾ && wp->w_topline != 1)
	nlnum = (so == wp->w_height / 2) ? bot : മുകൾ;

    എങ്കിൽ (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	എങ്കിൽ (normal)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	അല്ലെങ്കിൽ  // Scroll instead when not in normal mode.
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
    ശൂന്യം
win_new_height(win_T *wp, പൂർണ്ണസംഖ്യ height)
{
    പൂർണ്ണസംഖ്യ		prev_height = wp->w_height;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    എങ്കിൽ (height < 0)
	height = 0;
    എങ്കിൽ (wp->w_height == height)
	തിരികെ;	    // nothing to do

    എങ്കിൽ (wp->w_height > 0)
    {
	എങ്കിൽ (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	എങ്കിൽ (wp->w_height != prev_height)
	    തിരികെ;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	എങ്കിൽ (wp->w_wrow != wp->w_prev_fraction_row)
	    set_fraction(wp);
    }

    wp->w_height = height;
    wp->w_redr_status = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    എങ്കിൽ (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	എങ്കിൽ (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_to_fraction(wp, prev_height);
    }
}

    ശൂന്യം
scroll_to_fraction(win_T *wp, പൂർണ്ണസംഖ്യ prev_height)
{
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		sline, വരി_size;
    പൂർണ്ണസംഖ്യ		height = wp->w_height;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    എങ്കിൽ (height > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (height < wp->w_buffer->b_ml.ml_വരി_എണ്ണം || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	എങ്കിൽ (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((long)wp->w_fraction * (long)height - 1L)
							       / FRACTION_MULT;
	വരി_size = plines_win_col(wp, lnum, (long)(wp->w_cursor.col)) - 1;
	sline = wp->w_wrow - വരി_size;

	എങ്കിൽ (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    പൂർണ്ണസംഖ്യ rows = plines_win(wp, lnum, FALSE);

	    എങ്കിൽ (sline > wp->w_height - rows)
	    {
		sline = wp->w_height - rows;
		wp->w_wrow -= rows - വരി_size;
	    }
	}

	എങ്കിൽ (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    പൂർണ്ണസംഖ്യ		want_row = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = വരി_size;
	    എങ്കിൽ (wp->w_wrow >= wp->w_height
				       && (wp->w_width - win_col_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_width - win_col_off(wp);

		--wp->w_wrow;
		ആകുമ്പോൾ (wp->w_wrow > want_row)
		{
		    skipcol += wp->w_width - win_col_off(wp) + win_col_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (sline > 0)
	{
	    ആകുമ്പോൾ (sline > 0 && lnum > 1)
	    {
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, ശൂന്യ, TRUE, ശൂന്യ);
		എങ്കിൽ (lnum == 1)
		{
		    // first line in buffer is folded
		    വരി_size = 1;
		    --sline;
		    നിർത്തുക;
		}
#endif
		--lnum;
#ifdef FEAT_DIFF
		എങ്കിൽ (lnum == wp->w_topline)
		    വരി_size = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		അല്ലെങ്കിൽ
#endif
		    വരി_size = plines_win(wp, lnum, TRUE);
		sline -= വരി_size;
	    }

	    എങ്കിൽ (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, ശൂന്യ, &lnum, TRUE, ശൂന്യ);
#endif
		lnum++;
		wp->w_wrow -= വരി_size + sline;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	set_topline(wp, lnum);
    }

    എങ്കിൽ (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    എങ്കിൽ (prev_height > 0)
	wp->w_prev_fraction_row = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    ശൂന്യം
win_new_width(win_T *wp, പൂർണ്ണസംഖ്യ width)
{
    // Should we give an error if width < 0?
    wp->w_width = width < 0 ? 0 : width;
    wp->w_വരികൾ_valid = 0;
    changed_വരി_abv_curs_win(wp);
    invalidate_botline_win(wp);

    എങ്കിൽ (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_status = true;
}

    ശൂന്യം
win_comp_scroll(win_T *wp)
{
#എങ്കിൽ defined(FEAT_EVAL)
    പൂർണ്ണസംഖ്യ old_w_p_scr = wp->w_p_scr;
#endif

    wp->w_p_scr = ((unsigned)wp->w_height >> 1);
    എങ്കിൽ (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ (wp->w_p_scr != old_w_p_scr)
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
    ശൂന്യം
command_height(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		old_p_ch = curtab->tp_ch_used;

    // Find bottom frame with width of screen.
    frame_T *frp = lastwin->w_frame;
    ആകുമ്പോൾ (frp->fr_width != topframe->fr_width && frp->fr_parent != ശൂന്യ)
	frp = frp->fr_parent;

    // Avoid changing the height of a window with 'winfixheight' set.
    ആകുമ്പോൾ (frp->fr_prev != ശൂന്യ && frp->fr_layout == FR_LEAF
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    ആകുമ്പോൾ (p_ch > old_p_ch && command_frame_height)
    {
	എങ്കിൽ (frp == ശൂന്യ)
	{
	    emsg(_(e_not_enough_room));
	    p_ch = old_p_ch;
	    നിർത്തുക;
	}
	പൂർണ്ണസംഖ്യ h = MIN(p_ch - old_p_ch,
			frp->fr_height - frame_minheight(frp, ശൂന്യ));
	frame_add_height(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    എങ്കിൽ (p_ch < old_p_ch && command_frame_height && frp != ശൂന്യ)
	frame_add_height(frp, (പൂർണ്ണസംഖ്യ)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_row = Rows - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    എങ്കിൽ (സന്ദേശം_scrolled == 0 && full_screen)
    {
	screen_fill(cmdline_row, (പൂർണ്ണസംഖ്യ)Rows, 0, (പൂർണ്ണസംഖ്യ)Columns, ' ', ' ', 0);
	സന്ദേശം_row = cmdline_row;
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
    സ്ഥിതം ശൂന്യം
frame_add_height(frame_T *frp, പൂർണ്ണസംഖ്യ n)
{
    frame_new_height(frp, frp->fr_height + n, FALSE, FALSE, FALSE);
    വേണ്ടി (;;)
    {
	frp = frp->fr_parent;
	എങ്കിൽ (frp == ശൂന്യ)
	    നിർത്തുക;
	frp->fr_height += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    ശൂന്യം
last_status(
    പൂർണ്ണസംഖ്യ		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    last_status_rec(topframe, last_stl_height(morewin) > 0);
#എങ്കിൽ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

    സ്ഥിതം ശൂന്യം
last_status_rec(frame_T *fr, പൂർണ്ണസംഖ്യ statusline)
{
    frame_T	*fp;
    win_T	*wp;

    എങ്കിൽ (fr->fr_layout == FR_LEAF)
    {
	wp = fr->fr_win;
	എങ്കിൽ (wp->w_status_height != 0 && !statusline)
	{
	    // remove status line
	    win_new_height(wp, wp->w_height + wp->w_status_height);
	    wp->w_status_height = 0;
	    comp_col();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (wp->w_status_height == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    ആകുമ്പോൾ (fp->fr_height <= frame_minheight(fp, ശൂന്യ))
	    {
		എങ്കിൽ (fp == topframe)
		{
		    emsg(_(e_not_enough_room));
		    തിരികെ;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		എങ്കിൽ (fp->fr_parent->fr_layout == FR_COL && fp->fr_prev != ശൂന്യ)
		    fp = fp->fr_prev;
		അല്ലെങ്കിൽ
		    fp = fp->fr_parent;
	    }
	    wp->w_status_height = statusline_height(wp);
	    എങ്കിൽ (fp != fr)
	    {
		frame_new_height(fp, fp->fr_height - wp->w_status_height,
			FALSE, FALSE, FALSE);
		frame_fix_height(wp);
		win_comp_pos();
	    }
	    അല്ലെങ്കിൽ
		win_new_height(wp, wp->w_height - wp->w_status_height);
	    comp_col();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	എങ്കിൽ (abs(wp->w_height - wp->w_prev_height) == 1)
	    wp->w_prev_height = wp->w_height;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (fr->fr_layout == FR_ROW)
    {
	// vertically split windows, set status line for each one
	എങ്കിൽ (!statusline)
	{
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		last_status_rec(fp, statusline);
	}
	അല്ലെങ്കിൽ
	{
	    frame_T	*fp2;
	    പൂർണ്ണസംഖ്യ		max_stlh = 0;
	    പൂർണ്ണസംഖ്യ		new_row_height;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		എങ്കിൽ (fp->fr_win != ശൂന്യ && fp->fr_win->w_status_height == 0)
		{
		    പൂർണ്ണസംഖ്യ h = statusline_height(fp->fr_win);
		    എങ്കിൽ (h > max_stlh)
			max_stlh = h;
		}
	    എങ്കിൽ (max_stlh == 0)
		തിരികെ;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    ആകുമ്പോൾ (fp2->fr_height - frame_minheight(fp2, ശൂന്യ) < max_stlh)
	    {
		എങ്കിൽ (fp2 == topframe)
		{
		    emsg(_(e_not_enough_room));
		    തിരികെ;
		}
		എങ്കിൽ (fp2->fr_parent->fr_layout == FR_COL
						&& fp2->fr_prev != ശൂന്യ)
		    fp2 = fp2->fr_prev;
		അല്ലെങ്കിൽ
		    fp2 = fp2->fr_parent;
	    }

	    എങ്കിൽ (fp2 != fr)
	    {
		frame_new_height(fp2, fp2->fr_height - max_stlh,
							FALSE, FALSE, FALSE);
		new_row_height = fr->fr_height + max_stlh;
	    }
	    അല്ലെങ്കിൽ
		new_row_height = fr->fr_height;

	    // Set status and content heights for all leaves.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
	    {
		എങ്കിൽ (fp->fr_win != ശൂന്യ)
		{
		    wp = fp->fr_win;
		    എങ്കിൽ (wp->w_status_height == 0)
		    {
			wp->w_status_height = statusline_height(wp);
			win_new_height(wp,
				  new_row_height - wp->w_status_height);
			frame_fix_height(wp);
			comp_col();
			redraw_all_later(UPD_SOME_VALID);
			എങ്കിൽ (abs(wp->w_height - wp->w_prev_height) == 1)
			    wp->w_prev_height = wp->w_height;
		    }
		}
		അല്ലെങ്കിൽ
		    last_status_rec(fp, statusline); // nested frames
	    }

	    fr->fr_height = new_row_height;
	    win_comp_pos();
	}
    }
    അല്ലെങ്കിൽ
    {
	// horizontally split window, set status line for last one
	വേണ്ടി (fp = fr->fr_child; fp->fr_next != ശൂന്യ; fp = fp->fr_next)
	    ;
	last_status_rec(fp, statusline);
    }
}

#എങ്കിൽ defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    ശൂന്യം
update_stl_rendered_height(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == ശൂന്യ) ? p_stl : wp->w_p_stl;
    പൂർണ്ണസംഖ്യ		opt_flags = (wp == ശൂന്യ) ? 0 : OPT_LOCAL;
    പൂർണ്ണസംഖ്യ		*height = (wp == ശൂന്യ) ? &stl_rendered_height
						: &wp->w_stl_rendered_height;

    *height = get_stl_rendered_height(wp == ശൂന്യ ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
frame_find_global_stlh_rec(frame_T *frp, പൂർണ്ണസംഖ്യ h)
{
    എങ്കിൽ (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	എങ്കിൽ (wp->w_height > 0 && wp->w_status_height > 0
		&& *wp->w_p_stlo == NUL)
	{
	    പൂർണ്ണസംഖ്യ win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    എങ്കിൽ (win_free_height - p_wmh < h)
		h = win_free_height - p_wmh;
	}
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (frp = frp->fr_child; frp != ശൂന്യ; frp = frp->fr_next)
	    h = frame_find_global_stlh_rec(frp, h);
    }
    തിരികെ h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    സ്ഥിതം ശൂന്യം
frame_change_statusline_height_rec(frame_T *frp, പൂർണ്ണസംഖ്യ global_stlh)
{
    എങ്കിൽ (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	എങ്കിൽ (wp->w_height > 0 && wp->w_status_height > 0)
	{
	    പൂർണ്ണസംഖ്യ win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    wp->w_status_height = statusline_height(wp);
	    എങ്കിൽ (wp->w_p_stlo == ശൂന്യ || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		എങ്കിൽ (wp->w_status_height > global_stlh)
		    wp->w_status_height = global_stlh;
	    }
	    എങ്കിൽ (wp->w_status_height > win_free_height - p_wmh)
		wp->w_status_height = win_free_height - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    പൂർണ്ണസംഖ്യ	     saved_topfill = wp->w_topfill;
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
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (frp = frp->fr_child; frp != ശൂന്യ; frp = frp->fr_next)
	    frame_change_statusline_height_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    പൂർണ്ണസംഖ്യ
frame_change_statusline_height(ശൂന്യം)
{
    tabpage_T	*tp;
    പൂർണ്ണസംഖ്യ		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = frame_find_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	frame_change_statusline_height_rec(tp->tp_topframe, global_stlh);
    comp_col();
    redraw_all_later(UPD_SOME_VALID);
    തിരികെ global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    ശൂന്യം
set_stlo_mh(പൂർണ്ണസംഖ്യ mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
statuslineopt_changed(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		l_stlo_fh = FALSE;
    പൂർണ്ണസംഖ്യ		l_stlo_mh = 1;

    എങ്കിൽ (stlopt != ശൂന്യ)
	p = stlopt;
    അല്ലെങ്കിൽ
	p = wp->w_p_stlo;

    ആകുമ്പോൾ (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	എങ്കിൽ (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    എങ്കിൽ (l_stlo_mh < 1)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	എങ്കിൽ (*p != ',' && *p != NUL)
	    തിരികെ FAIL;
	എങ്കിൽ (*p == ',')
	    ++p;
    }

    എങ്കിൽ (wp == ശൂന്യ)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    അല്ലെങ്കിൽ
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    തിരികെ OK;
}
#endif

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    പൂർണ്ണസംഖ്യ
statusline_height(win_T *wp UNUSED)
{
    പൂർണ്ണസംഖ്യ stl_height = 1;
#എങ്കിൽ defined(FEAT_STL_OPT)
    bool fixed;
    പൂർണ്ണസംഖ്യ l_stl_rendered_height;

    എങ്കിൽ (wp != ശൂന്യ && *wp->w_p_stlo != NUL)
    {
	stl_height = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_height = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }
    അല്ലെങ്കിൽ
    {
	stl_height = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_height = (wp != ശൂന്യ && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    എങ്കിൽ (!fixed)
    {
	പൂർണ്ണസംഖ്യ has_custom_stl = (*p_stl != NUL)
	    || (wp != ശൂന്യ && *wp->w_p_stl != NUL);
	എങ്കിൽ (has_custom_stl)
	    stl_height = MAX(1, MIN(l_stl_rendered_height, stl_height));
	അല്ലെങ്കിൽ
	    stl_height = 1;
    }
#endif

    തിരികെ stl_height;
}

/*
 * Return the number of lines used by the tab page line.
 */
    പൂർണ്ണസംഖ്യ
tabline_height(ശൂന്യം)
{
#ifdef FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    എങ്കിൽ (gui_use_tabline())
	തിരികെ 0;
#endif
    തിരഞ്ഞെടുപ്പ് (p_stal)
    {
	അവസ്ഥ 0: തിരികെ 0;
	അവസ്ഥ 1: തിരികെ (first_tabpage->tp_next == ശൂന്യ) ? 0 : 1;
    }
    തിരികെ 1;
}

/*
 * Return the height of the last window's statusline.
 */
    പൂർണ്ണസംഖ്യ
last_stl_height(
    പൂർണ്ണസംഖ്യ		morewin)	// pretend there are two or more windows
{
    തിരികെ (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_WINDOW)))
		? STATUS_HEIGHT : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    പൂർണ്ണസംഖ്യ
min_rows(ശൂന്യം)
{
    എങ്കിൽ (firstwin == ശൂന്യ)	// not initialized yet
	തിരികെ MIN_വരികൾ;

    തിരികെ frame_minheight(curtab->tp_topframe, ശൂന്യ) + tabline_height()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    പൂർണ്ണസംഖ്യ
min_rows_for_all_tabpages(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		total;
    tabpage_T	*tp;
    പൂർണ്ണസംഖ്യ		n;

    എങ്കിൽ (firstwin == ശൂന്യ)	// not initialized yet
	തിരികെ MIN_വരികൾ;

    total = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = frame_minheight(tp->tp_topframe, ശൂന്യ);
	എങ്കിൽ (total < n)
	    total = n;
    }
    total += tabline_height();
    total += MIN_CMDHEIGHT;
    തിരികെ total;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    പൂർണ്ണസംഖ്യ
only_one_window(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    win_T	*wp;

#എങ്കിൽ defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    എങ്കിൽ (popup_is_popup(curwin))
	തിരികെ FALSE;
#endif

    // If there is another tab page there always is another window.
    എങ്കിൽ (first_tabpage->tp_next != ശൂന്യ)
	തിരികെ FALSE;

    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (wp->w_buffer != ശൂന്യ
		&& (!((bt_help(wp->w_buffer) && !bt_help(curbuf))
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++എണ്ണം;
    തിരികെ (എണ്ണം <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    സ്ഥിതം ശൂന്യം
check_lnums_both(പൂർണ്ണസംഖ്യ do_curwin, പൂർണ്ണസംഖ്യ nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	എങ്കിൽ ((do_curwin || wp != curwin) && wp->w_buffer == curbuf)
	{
	    പൂർണ്ണസംഖ്യ need_adjust;

	    എങ്കിൽ (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_വരി_എണ്ണം;
	    എങ്കിൽ (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	    എങ്കിൽ (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_വരി_എണ്ണം;
	    എങ്കിൽ (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_വരി_എണ്ണം;
	    എങ്കിൽ (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    ശൂന്യം
check_lnums(പൂർണ്ണസംഖ്യ do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    ശൂന്യം
check_lnums_nested(പൂർണ്ണസംഖ്യ do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    ശൂന്യം
reset_lnums(ശൂന്യം)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	എങ്കിൽ (wp->w_buffer == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    എങ്കിൽ (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    എങ്കിൽ (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    എങ്കിൽ (wp->w_save_cursor.w_topline_save > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
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
    പൂർണ്ണസംഖ്യ
make_snapshot(പൂർണ്ണസംഖ്യ idx)
{
    clear_snapshot(curtab, idx);
    എങ്കിൽ (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
make_snapshot_rec(frame_T *fr, frame_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(frame_T);
    എങ്കിൽ (*frp == ശൂന്യ)
	തിരികെ FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_width = fr->fr_width;
    (*frp)->fr_height = fr->fr_height;
    എങ്കിൽ (fr->fr_next != ശൂന്യ)
    {
	എങ്കിൽ (make_snapshot_rec(fr->fr_next, &((*frp)->fr_next)) == FAIL)
	    തിരികെ FAIL;
    }
    എങ്കിൽ (fr->fr_child != ശൂന്യ)
    {
	എങ്കിൽ (make_snapshot_rec(fr->fr_child, &((*frp)->fr_child)) == FAIL)
	    തിരികെ FAIL;
    }
    എങ്കിൽ (fr->fr_layout == FR_LEAF && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    തിരികെ OK;
}

/*
 * Remove any existing snapshot.
 */
    സ്ഥിതം ശൂന്യം
clear_snapshot(tabpage_T *tp, പൂർണ്ണസംഖ്യ idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = ശൂന്യ;
}

    സ്ഥിതം ശൂന്യം
clear_snapshot_rec(frame_T *fr)
{
    എങ്കിൽ (fr == ശൂന്യ)
	തിരികെ;
    clear_snapshot_rec(fr->fr_next);
    clear_snapshot_rec(fr->fr_child);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    സ്ഥിതം win_T *
get_snapshot_curwin_rec(frame_T *ft)
{
    win_T	*wp;

    എങ്കിൽ (ft->fr_next != ശൂന്യ)
    {
	എങ്കിൽ ((wp = get_snapshot_curwin_rec(ft->fr_next)) != ശൂന്യ)
	    തിരികെ wp;
    }
    എങ്കിൽ (ft->fr_child != ശൂന്യ)
    {
	എങ്കിൽ ((wp = get_snapshot_curwin_rec(ft->fr_child)) != ശൂന്യ)
	    തിരികെ wp;
    }

    തിരികെ ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    സ്ഥിതം win_T *
get_snapshot_curwin(പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (curtab->tp_snapshot[idx] == ശൂന്യ)
	തിരികെ ശൂന്യ;

    തിരികെ get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    ശൂന്യം
restore_snapshot(
    പൂർണ്ണസംഖ്യ		idx,
    പൂർണ്ണസംഖ്യ		close_curwin)	    // closing current window
{
    win_T	*wp;

    എങ്കിൽ (curtab->tp_snapshot[idx] != ശൂന്യ
	    && curtab->tp_snapshot[idx]->fr_width == topframe->fr_width
	    && curtab->tp_snapshot[idx]->fr_height == topframe->fr_height
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	എങ്കിൽ (wp != ശൂന്യ && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_snapshot_rec(frame_T *sn, frame_T *fr)
{
    എങ്കിൽ (sn->fr_layout != fr->fr_layout
	    || (sn->fr_next == ശൂന്യ) != (fr->fr_next == ശൂന്യ)
	    || (sn->fr_child == ശൂന്യ) != (fr->fr_child == ശൂന്യ)
	    || (sn->fr_next != ശൂന്യ
		&& check_snapshot_rec(sn->fr_next, fr->fr_next) == FAIL)
	    || (sn->fr_child != ശൂന്യ
		&& check_snapshot_rec(sn->fr_child, fr->fr_child) == FAIL)
	    || (sn->fr_win != ശൂന്യ && !win_valid(sn->fr_win)))
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    സ്ഥിതം win_T *
restore_snapshot_rec(frame_T *sn, frame_T *fr)
{
    win_T	*wp = ശൂന്യ;
    win_T	*wp2;

    fr->fr_height = sn->fr_height;
    fr->fr_width = sn->fr_width;
    എങ്കിൽ (fr->fr_layout == FR_LEAF)
    {
	frame_new_height(fr, fr->fr_height, FALSE, FALSE, FALSE);
	frame_new_width(fr, fr->fr_width, FALSE, FALSE);
	wp = sn->fr_win;
    }
    എങ്കിൽ (sn->fr_next != ശൂന്യ)
    {
	wp2 = restore_snapshot_rec(sn->fr_next, fr->fr_next);
	എങ്കിൽ (wp2 != ശൂന്യ)
	    wp = wp2;
    }
    എങ്കിൽ (sn->fr_child != ശൂന്യ)
    {
	wp2 = restore_snapshot_rec(sn->fr_child, fr->fr_child);
	എങ്കിൽ (wp2 != ശൂന്യ)
	    wp = wp2;
    }
    തിരികെ wp;
}

#എങ്കിൽ defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    പൂർണ്ണസംഖ്യ
win_hasvertsplit(ശൂന്യം)
{
    frame_T	*fr;

    എങ്കിൽ (topframe->fr_layout == FR_ROW)
	തിരികെ TRUE;

    എങ്കിൽ (topframe->fr_layout == FR_COL)
	FOR_ALL_FRAMES(fr, topframe->fr_child)
	    എങ്കിൽ (fr->fr_layout == FR_ROW)
		തിരികെ TRUE;

    തിരികെ FALSE;
}
#endif

#എങ്കിൽ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    പൂർണ്ണസംഖ്യ
get_win_സംഖ്യ(win_T *wp, win_T *first_win)
{
    പൂർണ്ണസംഖ്യ		i = 1;
    win_T	*w;

    വേണ്ടി (w = first_win; w != ശൂന്യ && w != wp; w = W_NEXT(w))
	++i;

    എങ്കിൽ (w == ശൂന്യ)
	തിരികെ 0;
    അല്ലെങ്കിൽ
	തിരികെ i;
}

    പൂർണ്ണസംഖ്യ
get_tab_സംഖ്യ(tabpage_T *tp)
{
    പൂർണ്ണസംഖ്യ		i = 1;
    tabpage_T	*t;

    വേണ്ടി (t = first_tabpage; t != ശൂന്യ && t != tp; t = t->tp_next)
	++i;

    എങ്കിൽ (t == ശൂന്യ)
	തിരികെ 0;
    അല്ലെങ്കിൽ
	തിരികെ i;
}
#endif

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
frame_check_height(frame_T *topfrp, പൂർണ്ണസംഖ്യ height)
{
    frame_T *frp;

    എങ്കിൽ (topfrp->fr_height != height)
	തിരികെ FALSE;

    എങ്കിൽ (topfrp->fr_layout == FR_ROW)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    എങ്കിൽ (frp->fr_height != height)
		തിരികെ FALSE;

    തിരികെ TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
frame_check_width(frame_T *topfrp, പൂർണ്ണസംഖ്യ width)
{
    frame_T *frp;

    എങ്കിൽ (topfrp->fr_width != width)
	തിരികെ FALSE;

    എങ്കിൽ (topfrp->fr_layout == FR_COL)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    എങ്കിൽ (frp->fr_width != width)
		തിരികെ FALSE;

    തിരികെ TRUE;
}

#എങ്കിൽ defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
int_cmp(സ്ഥിരം ശൂന്യം *pa, സ്ഥിരം ശൂന്യം *pb)
{
    സ്ഥിരം പൂർണ്ണസംഖ്യ a = *(സ്ഥിരം പൂർണ്ണസംഖ്യ *)pa;
    സ്ഥിരം പൂർണ്ണസംഖ്യ b = *(സ്ഥിരം പൂർണ്ണസംഖ്യ *)pb;
    എങ്കിൽ (a > b)
	തിരികെ 1;
    എങ്കിൽ (a < b)
	തിരികെ -1;
    തിരികെ 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    അക്ഷരം *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = empty_option;
    പൂർണ്ണസംഖ്യ		tw;
    പൂർണ്ണസംഖ്യ		col;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		color_cols[256];
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		j = 0;

    എങ്കിൽ (wp != ശൂന്യ && wp->w_buffer == ശൂന്യ)
	തിരികെ ശൂന്യ;  // buffer was closed

    എങ്കിൽ (cc != ശൂന്യ)
	s = cc;
    അല്ലെങ്കിൽ എങ്കിൽ (wp != ശൂന്യ)
	s = wp->w_p_cc;

    എങ്കിൽ (wp != ശൂന്യ)
	tw = wp->w_buffer->b_p_tw;
    അല്ലെങ്കിൽ
	// buffer-local value not set, assume zero
	tw = 0;

    ആകുമ്പോൾ (*s != NUL && എണ്ണം < 255)
    {
	എങ്കിൽ (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    col = (*s == '-') ? -1 : 1;
	    ++s;
	    എങ്കിൽ (!VIM_ISDIGIT(*s))
		തിരികെ e_invalid_argument;
	    col = col * getdigits(&s);
	    എങ്കിൽ (tw == 0)
		പോകുക skip;  // 'textwidth' not set, skip this item
	    col += tw;
	    എങ്കിൽ (col < 0)
		പോകുക skip;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISDIGIT(*s))
	    col = getdigits(&s);
	അല്ലെങ്കിൽ
	    തിരികെ e_invalid_argument;
	color_cols[എണ്ണം++] = col - 1;  // 1-based to 0-based
skip:
	എങ്കിൽ (*s == NUL)
	    നിർത്തുക;
	എങ്കിൽ (*s != ',')
	    തിരികെ e_invalid_argument;
	എങ്കിൽ (*++s == NUL)
	    തിരികെ e_invalid_argument;  // illegal trailing comma as in "set cc=80,"
    }

    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ ശൂന്യ;  // only parse "cc"

    vim_free(wp->w_p_cc_cols);
    എങ്കിൽ (എണ്ണം == 0)
	wp->w_p_cc_cols = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	wp->w_p_cc_cols = ALLOC_MULT(പൂർണ്ണസംഖ്യ, എണ്ണം + 1);
	എങ്കിൽ (wp->w_p_cc_cols != ശൂന്യ)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_cols, എണ്ണം, വലുപ്പം(പൂർണ്ണസംഖ്യ), int_cmp);

	    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
		// skip duplicates
		എങ്കിൽ (j == 0 || wp->w_p_cc_cols[j - 1] != color_cols[i])
		    wp->w_p_cc_cols[j++] = color_cols[i];
	    wp->w_p_cc_cols[j] = -1;  // end marker
	}
    }

    തിരികെ ശൂന്യ;  // no error
}
#endif

    പൂർണ്ണസംഖ്യ
get_last_winid(ശൂന്യം)
{
    തിരികെ last_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   പൂർണ്ണസംഖ്യ
win_locked(win_T *wp)
{
    തിരികെ wp->w_locked;
}
