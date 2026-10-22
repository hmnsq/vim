/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#অন্তর্ভাব "vim.h"

স্থির শূন্য cmd_with_গুনতি(অক্ষর *cmd, char_u *bufp, size_t bufsize, long Prenum);
স্থির শূন্য win_init(win_T *newp, win_T *oldp, পূর্ণসংখ্যা flags);
স্থির শূন্য win_init_some(win_T *newp, win_T *oldp);
স্থির শূন্য frame_comp_pos(frame_T *topfrp, পূর্ণসংখ্যা *row, পূর্ণসংখ্যা *col);
স্থির শূন্য frame_setheight(frame_T *curfrp, পূর্ণসংখ্যা height);
স্থির শূন্য frame_setwidth(frame_T *curfrp, পূর্ণসংখ্যা width);
স্থির শূন্য win_exchange(long);
স্থির শূন্য win_rotate(পূর্ণসংখ্যা, পূর্ণসংখ্যা);
স্থির শূন্য win_equal_rec(win_T *পরবর্তী_curwin, পূর্ণসংখ্যা current, frame_T *topfr, পূর্ণসংখ্যা dir, পূর্ণসংখ্যা col, পূর্ণসংখ্যা row, পূর্ণসংখ্যা width, পূর্ণসংখ্যা height);
স্থির শূন্য trigger_winnewpre(শূন্য);
স্থির শূন্য trigger_winclosed(win_T *win);
স্থির win_T *win_free_mem(win_T *win, পূর্ণসংখ্যা *dirp, tabpage_T *tp);
স্থির frame_T *win_altframe(win_T *win, tabpage_T *tp);
স্থির tabpage_T *alt_tabpage(শূন্য);
স্থির win_T *frame2win(frame_T *frp);
স্থির পূর্ণসংখ্যা frame_has_win(frame_T *frp, win_T *wp);
স্থির শূন্য win_fix_scroll(পূর্ণসংখ্যা resize);
স্থির শূন্য win_fix_cursor(পূর্ণসংখ্যা normal);
স্থির শূন্য frame_new_height(frame_T *topfrp, পূর্ণসংখ্যা height, পূর্ণসংখ্যা topfirst, পূর্ণসংখ্যা wfh, পূর্ণসংখ্যা set_ch);
স্থির পূর্ণসংখ্যা frame_fixed_height(frame_T *frp);
স্থির পূর্ণসংখ্যা frame_fixed_width(frame_T *frp);
স্থির শূন্য frame_add_statusline(frame_T *frp);
স্থির শূন্য frame_new_width(frame_T *topfrp, পূর্ণসংখ্যা width, পূর্ণসংখ্যা leftfirst, পূর্ণসংখ্যা wfw);
স্থির শূন্য frame_set_vsep(frame_T *frp, পূর্ণসংখ্যা add);
স্থির পূর্ণসংখ্যা frame_minwidth(frame_T *topfrp, win_T *পরবর্তী_curwin);
স্থির শূন্য frame_fix_width(win_T *wp);
স্থির পূর্ণসংখ্যা win_alloc_firstwin(win_T *oldwin);
স্থির শূন্য new_frame(win_T *wp);
স্থির tabpage_T *alloc_tabpage(শূন্য);
স্থির পূর্ণসংখ্যা leave_tabpage(buf_T *new_curbuf, পূর্ণসংখ্যা trigger_leave_autocmds);
স্থির শূন্য enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, পূর্ণসংখ্যা trigger_enter_autocmds, পূর্ণসংখ্যা trigger_leave_autocmds);
স্থির শূন্য frame_fix_height(win_T *wp);
স্থির পূর্ণসংখ্যা frame_minheight(frame_T *topfrp, win_T *পরবর্তী_curwin);
স্থির পূর্ণসংখ্যা frame_wincount_in_height(frame_T *topfrp);
স্থির পূর্ণসংখ্যা may_open_tabpage(শূন্য);
স্থির পূর্ণসংখ্যা win_enter_ext(win_T *wp, পূর্ণসংখ্যা flags);
স্থির শূন্য win_free(win_T *wp, tabpage_T *tp);
স্থির শূন্য win_append(win_T *after, win_T *wp);
স্থির শূন্য frame_append(frame_T *after, frame_T *frp);
স্থির শূন্য frame_ঢোকাও(frame_T *before, frame_T *frp);
স্থির শূন্য frame_remove(frame_T *frp);
স্থির শূন্য win_goto_ver(পূর্ণসংখ্যা up, long গুনতি);
স্থির শূন্য win_goto_hor(পূর্ণসংখ্যা বাম, long গুনতি);
স্থির শূন্য frame_add_height(frame_T *frp, পূর্ণসংখ্যা n);
স্থির শূন্য last_status_rec(frame_T *fr, পূর্ণসংখ্যা statusline);
#যদি defined(FEAT_STL_OPT)
স্থির পূর্ণসংখ্যা frame_find_global_stlh_rec(frame_T *frp, পূর্ণসংখ্যা h);
স্থির শূন্য frame_change_statusline_height_rec(frame_T *frp, পূর্ণসংখ্যা global_stlh);
#endif
স্থির শূন্য frame_flatten(frame_T *frp);
স্থির শূন্য winframe_restore(win_T *wp, পূর্ণসংখ্যা dir, frame_T *unflat_altfr);

স্থির পূর্ণসংখ্যা make_snapshot_rec(frame_T *fr, frame_T **frp);
স্থির শূন্য clear_snapshot(tabpage_T *tp, পূর্ণসংখ্যা idx);
স্থির শূন্য clear_snapshot_rec(frame_T *fr);
স্থির পূর্ণসংখ্যা check_snapshot_rec(frame_T *sn, frame_T *fr);
স্থির win_T *restore_snapshot_rec(frame_T *sn, frame_T *fr);
স্থির win_T *get_snapshot_curwin(পূর্ণসংখ্যা idx);

স্থির পূর্ণসংখ্যা frame_check_height(frame_T *topfrp, পূর্ণসংখ্যা height);
স্থির পূর্ণসংখ্যা frame_check_width(frame_T *topfrp, পূর্ণসংখ্যা width);

স্থির win_T *win_alloc(win_T *after, পূর্ণসংখ্যা hidden);

#define NOWIN		((win_T *)-1)	// non-existing window

#define ROWS_AVAIL (Rows - p_ch - tabline_height())

// flags for win_enter_ext()
#define WEE_UNDO_SYNC			0x01
#define WEE_CURWIN_INVALID		0x02
#define WEE_TRIGGER_NEW_AUTOCMDS	0x04
#define WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#define WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#define WEE_ALLOW_PARSE_MESSAGES	0x20

স্থির অক্ষর *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
স্থির পূর্ণসংখ্যা split_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
স্থির পূর্ণসংখ্যা close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
স্থির পূর্ণসংখ্যা frame_locked = 0;

#যদি defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
স্থির পূর্ণসংখ্যা stl_rendered_height = 1;
স্থির পূর্ণসংখ্যা stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
স্থির পূর্ণসংখ্যা stlo_mh = STATUS_HEIGHT; // "maxheight" value cache of global 'stlo'
#endif

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    শূন্য
window_layout_lock(শূন্য)
{
    ++split_disallowed;
    ++close_disallowed;
}

    শূন্য
window_layout_unlock(শূন্য)
{
    --split_disallowed;
    --close_disallowed;
}

    পূর্ণসংখ্যা
frames_locked(শূন্য)
{
    ফেরত frame_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    পূর্ণসংখ্যা
window_layout_locked(গণনা CMD_সূচক cmd)
{
    যদি (split_disallowed > 0 || close_disallowed > 0)
    {
	যদি (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_split_window_when_closing_বাফার));
	অন্যথা
	    emsg(_(e_not_allowed_to_change_window_layout_in_this_autocmd));
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

// #define WIN_DEBUG
#ifdef WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    স্থির শূন্য
log_frame_layout(frame_T *frame)
{
    ch_log(ফাঁকা, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    frame->fr_layout == FR_LEAF ? "LEAF"
				  : frame->fr_layout == FR_ROW ? "ROW" : "COL",
	    frame->fr_width,
	    frame->fr_height,
	    frame->fr_win == ফাঁকা ? -1 : frame->fr_win->w_width,
	    frame->fr_win == ফাঁকা ? -1 : frame->fr_win->w_height,
	    frame->fr_win == ফাঁকা ? -1 : frame->fr_win->w_id);
    যদি (frame->fr_child != ফাঁকা)
    {
	ch_log(ফাঁকা, "children");
	log_frame_layout(frame->fr_child);
	যদি (frame->fr_পরবর্তী != ফাঁকা)
	    ch_log(ফাঁকা, "END of children");
    }
    যদি (frame->fr_পরবর্তী != ফাঁকা)
	log_frame_layout(frame->fr_পরবর্তী);
}
#endif

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    পূর্ণসংখ্যা
check_can_set_curbuf_disabled(শূন্য)
{
    যদি (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_বাফার));
	ফেরত FALSE;
    }
    ফেরত TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    পূর্ণসংখ্যা
check_can_set_curbuf_forceit(পূর্ণসংখ্যা forceit)
{
    যদি (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_বাফার));
	ফেরত FALSE;
    }
    ফেরত TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(শূন্য)
{
    // In cmdwin, the alternative buffer should be used.
    ফেরত is_in_cmdwin() && prevwin != ফাঁকা ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = ফাঁকা;

    যদি (buf == ফাঁকা)
	ফেরত wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    যদি (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    যদি (wp == ফাঁকা && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    ফেরত wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    শূন্য
do_window(
    পূর্ণসংখ্যা		nchar,
    long	Prenum,
    পূর্ণসংখ্যা		xchar)	    // extra char from ":wincmd gx" or NUL
{
    long	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ifdef FEAT_FIND_ID
    পূর্ণসংখ্যা		type = FIND_DEFINE;
    পূর্ণসংখ্যা		len;
#endif
    char_u	cbuf[40];

    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত;

#define CHECK_CMDWIN \
    করো { \
	যদি (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_in_cmdline_window)); \
	    ফেরত; \
	} \
    } যখন (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    নির্বাচন (nchar)
    {
// split current window in two parts, horizontally
    ক্ষেত্র 'S':
    ক্ষেত্র Ctrl_S:
    ক্ষেত্র 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		যদি (bt_quickfix(curbuf))
		    যাও newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(শূন্য)win_split((পূর্ণসংখ্যা)Prenum, 0);
		বিরতি;

// split current window in two parts, vertically
    ক্ষেত্র Ctrl_V:
    ক্ষেত্র 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		যদি (bt_quickfix(curbuf))
		    যাও newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(শূন্য)win_split((পূর্ণসংখ্যা)Prenum, WSP_VERT);
		বিরতি;

// split current window and edit alternate file
    ক্ষেত্র Ctrl_HAT:
    ক্ষেত্র '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		যদি (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == ফাঁকা)
		{
		    যদি (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    অন্যথা
			semsg(_(e_বাফার_nr_not_found), Prenum);
		    বিরতি;
		}

		যদি (!curbuf_locked() && win_split(0, 0) == OK)
		    (শূন্য)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		বিরতি;

// open new window
    ক্ষেত্র Ctrl_N:
    ক্ষেত্র 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		যদি (Prenum)
		    // window height
		    vim_snprintf((অক্ষর *)cbuf, আকার(cbuf) - 5, "%ld", Prenum);
		অন্যথা
		    cbuf[0] = NUL;
#যদি defined(FEAT_QUICKFIX)
		যদি (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#endif
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		বিরতি;

// quit current window
    ক্ষেত্র Ctrl_Q:
    ক্ষেত্র 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_গুনতি("quit", cbuf, আকার(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		বিরতি;

// close current window
    ক্ষেত্র Ctrl_C:
    ক্ষেত্র 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_গুনতি("close", cbuf, আকার(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		বিরতি;

#যদি defined(FEAT_QUICKFIX)
// close preview window
    ক্ষেত্র Ctrl_Z:
    ক্ষেত্র 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		বিরতি;

// cursor to preview window
    ক্ষেত্র 'P':
		FOR_ALL_WINDOWS(wp)
		    যদি (wp->w_p_pvw)
			বিরতি;
		যদি (wp == ফাঁকা)
		    emsg(_(e_there_is_no_preview_window));
		অন্যথা
		    win_goto(wp);
		বিরতি;
#endif

// close all but current window
    ক্ষেত্র Ctrl_O:
    ক্ষেত্র 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_গুনতি("only", cbuf, আকার(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		বিরতি;

// cursor to next window with wrap around
    ক্ষেত্র Ctrl_W:
    ক্ষেত্র 'w':
// cursor to previous window with wrap around
    ক্ষেত্র 'W':
		CHECK_CMDWIN;
		যদি (ONE_WINDOW && Prenum != 1)	// just one window
		    beep_flush();
		অন্যথা
		{
		    যদি (Prenum)			// go to specified window
		    {
			জন্য (wp = firstwin; --Prenum > 0; )
			{
			    যদি (wp->w_পরবর্তী == ফাঁকা)
				বিরতি;
			    অন্যথা
				wp = wp->w_পরবর্তী;
			}
		    }
		    অন্যথা
		    {
			যদি (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_পূর্ব;
			    যদি (wp == ফাঁকা)
				wp = lastwin;	    // wrap around
			}
			অন্যথা			    // go to next window
			{
			    wp = curwin->w_পরবর্তী;
			    যদি (wp == ফাঁকা)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		বিরতি;

// cursor to window below
    ক্ষেত্র 'j':
    ক্ষেত্র K_DOWN:
    ক্ষেত্র Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		বিরতি;

// cursor to window above
    ক্ষেত্র 'k':
    ক্ষেত্র K_UP:
    ক্ষেত্র Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		বিরতি;

// cursor to left window
    ক্ষেত্র 'h':
    ক্ষেত্র K_বাম:
    ক্ষেত্র Ctrl_H:
    ক্ষেত্র K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		বিরতি;

// cursor to right window
    ক্ষেত্র 'l':
    ক্ষেত্র K_ডান:
    ক্ষেত্র Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		বিরতি;

// move window to new tab page
    ক্ষেত্র 'T':
		CHECK_CMDWIN;
		যদি (one_window())
		    বার্তা(_(m_onlyone));
		অন্যথা
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    যদি (win_new_tabpage((পূর্ণসংখ্যা)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			যদি (curwin == wp)
			    win_close(curwin, FALSE);
			যদি (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		বিরতি;

// cursor to top-left window
    ক্ষেত্র 't':
    ক্ষেত্র Ctrl_T:
		win_goto(firstwin);
		বিরতি;

// cursor to bottom-right window
    ক্ষেত্র 'b':
    ক্ষেত্র Ctrl_B:
		win_goto(lastwin);
		বিরতি;

// cursor to last accessed (previous) window
    ক্ষেত্র 'p':
    ক্ষেত্র Ctrl_P:
		যদি (!win_valid(prevwin))
		    beep_flush();
		অন্যথা
		    win_goto(prevwin);
		বিরতি;

// exchange current and next window
    ক্ষেত্র 'x':
    ক্ষেত্র Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		বিরতি;

// rotate windows downwards
    ক্ষেত্র Ctrl_R:
    ক্ষেত্র 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (পূর্ণসংখ্যা)Prenum1);    // downwards
		বিরতি;

// rotate windows upwards
    ক্ষেত্র 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (পূর্ণসংখ্যা)Prenum1);	    // upwards
		বিরতি;

// move window to the very top/bottom/left/right
    ক্ষেত্র 'K':
    ক্ষেত্র 'J':
    ক্ষেত্র 'H':
    ক্ষেত্র 'L':
		CHECK_CMDWIN;
		যদি (ONE_WINDOW)
		    beep_flush();
		অন্যথা
		{
		    পূর্ণসংখ্যা dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_শীর্ষ : WSP_BOT);

		    (শূন্য)win_splitmove(curwin, (পূর্ণসংখ্যা)Prenum, dir);
		}
		বিরতি;

// make all windows the same width and/or height
    ক্ষেত্র '=':
		{
		    পূর্ণসংখ্যা mod = cmdmod.cmod_split & (WSP_VERT | WSP_HOR);
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_equal(ফাঁকা, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		বিরতি;

// increase current window height
    ক্ষেত্র '+':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height + (পূর্ণসংখ্যা)Prenum1);
		বিরতি;

// decrease current window height
    ক্ষেত্র '-':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height - (পূর্ণসংখ্যা)Prenum1);
		বিরতি;

// set current window height
    ক্ষেত্র Ctrl__:
    ক্ষেত্র '_':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(Prenum ? (পূর্ণসংখ্যা)Prenum : 9999);
		বিরতি;

// increase current window width
    ক্ষেত্র '>':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width + (পূর্ণসংখ্যা)Prenum1);
		বিরতি;

// decrease current window width
    ক্ষেত্র '<':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width - (পূর্ণসংখ্যা)Prenum1);
		বিরতি;

// set current window width
    ক্ষেত্র '|':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(Prenum != 0 ? (পূর্ণসংখ্যা)Prenum : 9999);
		বিরতি;

// jump to tag and split window if tag exists (in preview window)
#যদি defined(FEAT_QUICKFIX)
    ক্ষেত্র '}':
		CHECK_CMDWIN;
		যদি (Prenum)
		    g_do_tagpreview = Prenum;
		অন্যথা
		    g_do_tagpreview = p_pvh;
#endif
		// FALLTHROUGH
    ক্ষেত্র ']':
    ক্ষেত্র Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		যদি (Prenum)
		    postponed_split = Prenum;
		অন্যথা
		    postponed_split = -1;
#ifdef FEAT_QUICKFIX
		যদি (nchar != '}')
		    g_do_tagpreview = 0;
#endif

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_split = 0;
		বিরতি;

// edit file name under cursor in a new window
    ক্ষেত্র 'f':
    ক্ষেত্র 'F':
    ক্ষেত্র Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		যদি (check_text_or_curbuf_locked(ফাঁকা))
		    বিরতি;

		ptr = grab_file_name(Prenum1, &lnum);
		যদি (ptr != ফাঁকা)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = ফাঁকা;
		    যদি ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    যদি (wp == ফাঁকা && win_split(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			যদি (do_ecmd(0, ptr, ফাঁকা, ফাঁকা, ECMD_LASTL,
						   ECMD_HIDE, ফাঁকা) == FAIL)
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
			অন্যথা
			    wp = curwin;
		    }

		    যদি (wp != ফাঁকা && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		বিরতি;

#ifdef FEAT_FIND_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    ক্ষেত্র 'i':			    // Go to any match
    ক্ষেত্র Ctrl_I:
		type = FIND_ANY;
		// FALLTHROUGH
    ক্ষেত্র 'd':			    // Go to definition, using 'define'
    ক্ষেত্র Ctrl_D:
		CHECK_CMDWIN;
		যদি ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
		    বিরতি;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		যদি (ptr == ফাঁকা)
		    বিরতি;

		find_pattern_in_path(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_SPLIT, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_set_curswant = true;
		বিরতি;
#endif

// Quickfix window only: view the result under the cursor in a new split.
#যদি defined(FEAT_QUICKFIX)
    ক্ষেত্র K_KENTER:
    ক্ষেত্র CAR:
		যদি (bt_quickfix(curbuf))
		    qf_view_ফলাফল(TRUE);
		বিরতি;
#endif

// CTRL-W g  extended commands
    ক্ষেত্র 'g':
    ক্ষেত্র Ctrl_G:
		CHECK_CMDWIN;
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#endif
		++no_mapping;
		++allow_চাবিগুলি;   // no mapping for xchar, but allow key codes
		যদি (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_চাবিগুলি;
		(শূন্য)add_to_showcmd(xchar);

		নির্বাচন (xchar)
		{
#যদি defined(FEAT_QUICKFIX)
		    ক্ষেত্র '}':
			xchar = Ctrl_RSB;
			যদি (Prenum)
			    g_do_tagpreview = Prenum;
			অন্যথা
			    g_do_tagpreview = p_pvh;
#endif
			// FALLTHROUGH
		    ক্ষেত্র ']':
		    ক্ষেত্র Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			যদি (Prenum)
			    postponed_split = Prenum;
			অন্যথা
			    postponed_split = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_split = 0;
			বিরতি;

		    ক্ষেত্র 'f':	    // CTRL-W gf: "gf" in a new tab page
		    ক্ষেত্র 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_সূচক(curtab) + 1;
			nchar = xchar;
			যাও wingotofile;

		    ক্ষেত্র 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((পূর্ণসংখ্যা)Prenum);
			বিরতি;

		    ক্ষেত্র 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(পূর্ণসংখ্যা)Prenum1);
			বিরতি;

		    ক্ষেত্র TAB:	    // CTRL-W g<Tab>: go to last used tab page
			যদি (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			বিরতি;

		    পূর্বনির্ধারিত:
			beep_flush();
			বিরতি;
		}
		বিরতি;

    পূর্বনির্ধারিত:	beep_flush();
		বিরতি;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    শূন্য
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    নির্বাচন (*arg)
    {
    ক্ষেত্র 'S':
    ক্ষেত্র Ctrl_S:
    ক্ষেত্র 's':
    ক্ষেত্র Ctrl_N:
    ক্ষেত্র 'n':
    ক্ষেত্র 'j':
    ক্ষেত্র Ctrl_J:
    ক্ষেত্র 'k':
    ক্ষেত্র Ctrl_K:
    ক্ষেত্র 'T':
    ক্ষেত্র Ctrl_R:
    ক্ষেত্র 'r':
    ক্ষেত্র 'R':
    ক্ষেত্র 'K':
    ক্ষেত্র 'J':
    ক্ষেত্র '+':
    ক্ষেত্র '-':
    ক্ষেত্র Ctrl__:
    ক্ষেত্র '_':
    ক্ষেত্র '|':
    ক্ষেত্র ']':
    ক্ষেত্র Ctrl_RSB:
    ক্ষেত্র 'g':
    ক্ষেত্র Ctrl_G:
    ক্ষেত্র Ctrl_V:
    ক্ষেত্র 'v':
    ক্ষেত্র 'h':
    ক্ষেত্র Ctrl_H:
    ক্ষেত্র 'l':
    ক্ষেত্র Ctrl_L:
    ক্ষেত্র 'H':
    ক্ষেত্র 'L':
    ক্ষেত্র '>':
    ক্ষেত্র '<':
#যদি defined(FEAT_QUICKFIX)
    ক্ষেত্র '}':
#endif
    ক্ষেত্র 'f':
    ক্ষেত্র 'F':
    ক্ষেত্র Ctrl_F:
#ifdef FEAT_FIND_ID
    ক্ষেত্র 'i':
    ক্ষেত্র Ctrl_I:
    ক্ষেত্র 'd':
    ক্ষেত্র Ctrl_D:
#endif
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		বিরতি;

    ক্ষেত্র Ctrl_HAT:
    ক্ষেত্র '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		বিরতি;

    ক্ষেত্র Ctrl_Q:
    ক্ষেত্র 'q':
    ক্ষেত্র Ctrl_C:
    ক্ষেত্র 'c':
    ক্ষেত্র Ctrl_O:
    ক্ষেত্র 'o':
    ক্ষেত্র Ctrl_W:
    ক্ষেত্র 'w':
    ক্ষেত্র 'W':
    ক্ষেত্র 'x':
    ক্ষেত্র Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		বিরতি;

#যদি defined(FEAT_QUICKFIX)
    ক্ষেত্র Ctrl_Z:
    ক্ষেত্র 'z':
    ক্ষেত্র 'P':
#endif
    ক্ষেত্র 't':
    ক্ষেত্র Ctrl_T:
    ক্ষেত্র 'b':
    ক্ষেত্র Ctrl_B:
    ক্ষেত্র 'p':
    ক্ষেত্র Ctrl_P:
    ক্ষেত্র '=':
    ক্ষেত্র CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		বিরতি;
    }
}

    স্থির শূন্য
cmd_with_গুনতি(
    অক্ষর	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    long	Prenum)
{
    যদি (Prenum > 0)
	vim_snprintf((অক্ষর *)bufp, bufsize, "%s %ld", cmd, Prenum);
    অন্যথা
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    পূর্ণসংখ্যা
check_split_disallowed(win_T *wp)
{
    যদি (split_disallowed > 0)
    {
	emsg(_(e_cant_split_window_while_closing_another));
	ফেরত FAIL;
    }
    যদি (wp->w_বাফার->b_locked_split)
    {
	emsg(_(e_cannot_split_window_when_closing_বাফার));
	ফেরত FAIL;
    }
    ফেরত OK;
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
    পূর্ণসংখ্যা
win_split(পূর্ণসংখ্যা আকার_2, পূর্ণসংখ্যা flags)
{
    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত FAIL;

    যদি (check_split_disallowed(curwin) == FAIL)
	ফেরত FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    যদি (may_open_tabpage() == OK)
	ফেরত OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_split;
    যদি ((flags & WSP_শীর্ষ) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_split_topleft_and_botright_at_the_same_time));
	ফেরত FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    যদি (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    অন্যথা
	clear_snapshot(curtab, SNAP_HELP_IDX);

    যদি (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    অন্যথা
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    ফেরত win_split_ins(আকার_2, flags, ফাঁকা, 0, ফাঁকা);
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
    পূর্ণসংখ্যা
win_split_ins(
    পূর্ণসংখ্যা		আকার_2,
    পূর্ণসংখ্যা		flags,
    win_T	*new_wp,
    পূর্ণসংখ্যা		dir,
    frame_T	*to_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    পূর্ণসংখ্যা		new_আকার = আকার_2;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		need_status = 0;
    পূর্ণসংখ্যা		do_equal = FALSE;
    পূর্ণসংখ্যা		needed;
    পূর্ণসংখ্যা		available;
    পূর্ণসংখ্যা		oldwin_height = 0;
    পূর্ণসংখ্যা		layout;
    frame_T	*frp, *curfrp, *frp2, *prevfrp;
    পূর্ণসংখ্যা		before;
    পূর্ণসংখ্যা		minheight;
    পূর্ণসংখ্যা		wmh1;
    পূর্ণসংখ্যা		did_set_fraction = FALSE;
    পূর্ণসংখ্যা		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    যদি (new_wp == ফাঁকা)
	trigger_winnewpre();

    যদি (flags & WSP_শীর্ষ)
	oldwin = firstwin;
    অন্যথা যদি (flags & WSP_BOT)
	oldwin = lastwin;
    অন্যথা
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    যদি (ONE_WINDOW && p_ls == 1 && oldwin->w_status_height == 0)
    {
	যদি (!(flags & WSP_FORCE_ROOM) && VISIBLE_HEIGHT(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_room));
	    যাও theend;
	}
	need_status = statusline_height(oldwin);
    }

#ifdef FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    যদি (gui.in_use)
	out_flush();
#endif

    যদি (flags & WSP_VERT)
    {
	পূর্ণসংখ্যা	wmw1;
	পূর্ণসংখ্যা	minwidth;

	layout = FR_ROW;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	যদি (flags & WSP_ROOM)
	    needed += p_wiw - wmw1;
	যদি (flags & (WSP_BOT | WSP_শীর্ষ))
	{
	    minwidth = frame_minwidth(topframe, NOWIN);
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	অন্যথা যদি (p_ea)
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    prevfrp = oldwin->w_frame;
	    জন্য (frp = oldwin->w_frame->fr_parent; frp != ফাঁকা;
							frp = frp->fr_parent)
	    {
		যদি (frp->fr_layout == FR_ROW)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			যদি (frp2 != prevfrp)
			    minwidth += frame_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	অন্যথা
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    available = oldwin->w_frame->fr_width;
	    needed += minwidth;
	}
	যদি (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    যাও theend;
	}
	যদি (new_আকার == 0)
	    new_আকার = oldwin->w_width / 2;
	যদি (new_আকার > available - minwidth - 1)
	    new_আকার = available - minwidth - 1;
	যদি (new_আকার < wmw1)
	    new_আকার = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	যদি (oldwin->w_width - new_আকার - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	যদি (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_width + new_আকার + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	যদি (!do_equal && p_ea && আকার_2 == 0 && *p_ead != 'v'
	    && oldwin->w_frame->fr_parent != ফাঁকা)
	    do_equal = TRUE;
    }
    অন্যথা
    {
	layout = FR_COL;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_HEIGHT(curwin);
	needed = wmh1 + statusline_height(oldwin);
	যদি (flags & WSP_ROOM)
	    needed += p_wh - wmh1;
	যদি (flags & (WSP_BOT | WSP_শীর্ষ))
	{
	    minheight = frame_minheight(topframe, NOWIN) + need_status;
	    available = topframe->fr_height;
	    needed += minheight;
	}
	অন্যথা যদি (p_ea)
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    prevfrp = oldwin->w_frame;
	    জন্য (frp = oldwin->w_frame->fr_parent; frp != ফাঁকা;
							frp = frp->fr_parent)
	    {
		যদি (frp->fr_layout == FR_COL)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			যদি (frp2 != prevfrp)
			    minheight += frame_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_height;
	    needed += minheight;
	}
	অন্যথা
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    available = oldwin->w_frame->fr_height;
	    needed += minheight;
	}
	যদি (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    যাও theend;
	}
	oldwin_height = oldwin->w_height;
	যদি (need_status)
	{
	    oldwin->w_status_height = statusline_height(oldwin);
	    oldwin_height -= oldwin->w_status_height;
	}
	// Estimated stl height for the new window.
	পূর্ণসংখ্যা new_win_stlh = need_status != 0 ? need_status
			: statusline_height(new_wp != ফাঁকা ? new_wp : curwin);
	যদি (new_আকার == 0)
	    new_আকার = (oldwin_height - new_win_stlh + 1) / 2;
	যদি (new_আকার > available - minheight - new_win_stlh)
	    new_আকার = available - minheight - new_win_stlh;
	যদি (new_আকার < wmh1)
	    new_আকার = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	যদি (oldwin_height - new_আকার - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	যদি (oldwin->w_p_wfh)
	{
	    // Set w_fraction now so that the cursor keeps the same relative
	    // vertical position using the old height.
	    set_fraction(oldwin);
	    did_set_fraction = TRUE;

	    win_setheight_win(oldwin->w_height + new_আকার
		    + statusline_height(oldwin), oldwin);
	    // w_height now excludes the status line
	    oldwin_height = oldwin->w_height;
	}

	// Make all windows the same height if 'equalalways' is set, 'ead' is
	// not set to 'h', a size was not provided and there is a parent frame.
	যদি (!do_equal && p_ea && আকার_2 == 0 && *p_ead != 'h'
	    && oldwin->w_frame->fr_parent != ফাঁকা)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    যদি ((flags & WSP_শীর্ষ) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_BELOW)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	যদি (new_wp == ফাঁকা)
	    wp = win_alloc(oldwin, FALSE);
	অন্যথা
	    win_append(oldwin, wp);
    }
    অন্যথা
    {
	যদি (new_wp == ফাঁকা)
	    wp = win_alloc(oldwin->w_পূর্ব, FALSE);
	অন্যথা
	    win_append(oldwin->w_পূর্ব, wp);
    }

    যদি (new_wp == ফাঁকা)
    {
	যদি (wp == ফাঁকা)
	    যাও theend;

	new_frame(wp);
	যদি (wp->w_frame == ফাঁকা)
	{
	    win_free(wp, ফাঁকা);
	    যাও theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    যদি (to_flatten != ফাঁকা)
	frame_flatten(to_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    যদি (flags & (WSP_শীর্ষ | WSP_BOT))
    {
	যদি ((topframe->fr_layout == FR_COL && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ROW && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_child;
	    যদি (flags & WSP_BOT)
		যখন (curfrp->fr_পরবর্তী != ফাঁকা)
		    curfrp = curfrp->fr_পরবর্তী;
	}
	অন্যথা
	    curfrp = topframe;
	before = (flags & WSP_শীর্ষ);
    }
    অন্যথা
    {
	curfrp = oldwin->w_frame;
	যদি (flags & WSP_BELOW)
	    before = FALSE;
	অন্যথা যদি (flags & WSP_ABOVE)
	    before = TRUE;
	অন্যথা যদি (flags & WSP_VERT)
	    before = !p_spr;
	অন্যথা
	    before = !p_sb;
    }
    যদি (curfrp->fr_parent == ফাঁকা || curfrp->fr_parent->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(frame_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_parent = curfrp;
	frp->fr_পরবর্তী = ফাঁকা;
	frp->fr_পূর্ব = ফাঁকা;
	curfrp->fr_child = frp;
	curfrp->fr_win = ফাঁকা;
	curfrp = frp;
	যদি (frp->fr_win != ফাঁকা)
	    oldwin->w_frame = frp;
	অন্যথা
	    FOR_ALL_FRAMES(frp, frp->fr_child)
		frp->fr_parent = curfrp;
    }

    যদি (new_wp == ফাঁকা)
	frp = wp->w_frame;
    অন্যথা
	frp = new_wp->w_frame;
    frp->fr_parent = curfrp->fr_parent;

    // Insert the new frame at the right place in the frame list.
    যদি (before)
	frame_ঢোকাও(curfrp, frp);
    অন্যথা
	frame_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    যদি (!did_set_fraction)
	set_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    যদি (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	যদি (need_status)
	{
	    win_new_height(oldwin, oldwin->w_height - need_status);
	    oldwin->w_status_height = need_status;
	}
	যদি (flags & (WSP_শীর্ষ | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    পূর্ণসংখ্যা stl_height = p_ls > 0 ? statusline_height(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_height();
	    win_new_height(wp, curfrp->fr_height - stl_height
							  - WINBAR_HEIGHT(wp));
	    wp->w_status_height = stl_height;
	}
	অন্যথা
	{
	    // height and row of new window is same as current window
	    wp->w_winrow = oldwin->w_winrow;
	    win_new_height(wp, VISIBLE_HEIGHT(oldwin));
	    wp->w_status_height = oldwin->w_status_height;
	}
	frp->fr_height = curfrp->fr_height;

	// "new_size" of the current window goes to the new window, use
	// one column for the vertical separator
	win_new_width(wp, new_আকার);
	যদি (before)
	    wp->w_vsep_width = 1;
	অন্যথা
	{
	    wp->w_vsep_width = oldwin->w_vsep_width;
	    oldwin->w_vsep_width = 1;
	}
	যদি (flags & (WSP_শীর্ষ | WSP_BOT))
	{
	    যদি (flags & WSP_BOT)
		frame_set_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    frame_new_width(curfrp, curfrp->fr_width
		     - (new_আকার + ((flags & WSP_শীর্ষ) != 0)), flags & WSP_শীর্ষ,
								       FALSE);
	}
	অন্যথা
	    win_new_width(oldwin, oldwin->w_width - (new_আকার + 1));
	যদি (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_আকার + 1;
	}
	অন্যথা		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_width + 1;
	frame_fix_width(oldwin);
	frame_fix_width(wp);
    }
    অন্যথা
    {
	// width and column of new window is same as current window
	যদি (flags & (WSP_শীর্ষ | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_width(wp, topframe->fr_width);
	    wp->w_vsep_width = 0;
	}
	অন্যথা
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_width(wp, oldwin->w_width);
	    wp->w_vsep_width = oldwin->w_vsep_width;
	}
	frp->fr_width = curfrp->fr_width;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_height(wp, new_আকার);
	পূর্ণসংখ্যা old_status_height = oldwin->w_status_height;
	যদি (flags & (WSP_শীর্ষ | WSP_BOT))
	{
	    পূর্ণসংখ্যা new_fr_height = curfrp->fr_height - new_আকার
							  + WINBAR_HEIGHT(wp);

	    যদি (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_height -= statusline_height(curfrp->fr_win);
	    যদি (flags & WSP_BOT)
		frame_add_statusline(curfrp);
	    frame_new_height(curfrp, new_fr_height, flags & WSP_শীর্ষ, FALSE,
									FALSE);
	}
	অন্যথা
	    win_new_height(oldwin, oldwin_height - (new_আকার
			+ statusline_height(wp)));
	যদি (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_status_height = statusline_height(wp);
	    oldwin->w_winrow += wp->w_height + statusline_height(wp);
	}
	অন্যথা		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_HEIGHT(oldwin)
						    + statusline_height(oldwin);
	    wp->w_status_height = old_status_height;
	    যদি (!(flags & WSP_BOT))
		oldwin->w_status_height = statusline_height(oldwin);
	}
	frame_fix_height(wp);
	frame_fix_height(oldwin);
    }

    যদি (flags & (WSP_শীর্ষ | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    status_redraw_all();

    যদি (need_status)
    {
	বার্তা_row = Rows - 1;
	বার্তা_col = sc_col;
	বার্তা_clr_eos_force();	// Old command/ruler may still be there
	comp_col();
	বার্তা_row = Rows - 1;
	বার্তা_col = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    যদি (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    অন্যথা যদি (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    যদি (flags & WSP_VERT)
    {
	i = p_wiw;
	যদি (আকার_2 != 0)
	    p_wiw = আকার_2;

#ifdef FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	যদি (gui.in_use)
	    gui_init_which_components(ফাঁকা);
#endif
    }
    অন্যথা
    {
	i = p_wh;
	যদি (আকার_2 != 0)
	    p_wh = আকার_2;
    }

    /*
     * make the new window the current window
     */
    (শূন্য)win_enter_ext(wp, (new_wp == ফাঁকা ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    যদি (flags & WSP_VERT)
	p_wiw = i;
    অন্যথা
	p_wh = i;
    retval = OK;

theend:
    যদি (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ফেরত retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    স্থির শূন্য
win_init(win_T *newp, win_T *oldp, পূর্ণসংখ্যা flags UNUSED)
{
    পূর্ণসংখ্যা		i;

    newp->w_বাফার = oldp->w_বাফার;
#ifdef FEAT_SYN_HL
    newp->w_s = &(oldp->w_বাফার->b_s);
#endif
    oldp->w_বাফার->b_nwindows++;
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
    newp->w_পূর্ব_pcmark = oldp->w_পূর্ব_pcmark;
    newp->w_alt_fnum = oldp->w_alt_fnum;
    newp->w_wrow = oldp->w_wrow;
    newp->w_fraction = oldp->w_fraction;
    newp->w_পূর্ব_fraction_row = oldp->w_পূর্ব_fraction_row;

    // Not sure if this is needed, but be safe
    remove_highlight_overrides(newp->w_hl);
    VIM_CLEAR(newp->w_hl);

    copy_jumplist(oldp, newp);
#ifdef FEAT_QUICKFIX
    যদি (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = ফাঁকা;
	newp->w_llist_ref = ফাঁকা;
    }
    অন্যথা
	copy_loclist_স্তূপ(oldp, newp);
#endif
    newp->w_localdir = (oldp->w_localdir == ফাঁকা)
				    ? ফাঁকা : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == ফাঁকা)
				    ? ফাঁকা : vim_strsave(oldp->w_prevdir);

    যদি (*p_spk != 'c')
    {
	যদি (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_পূর্ব_height = oldp->w_height - WINBAR_HEIGHT(oldp);
	newp->w_পূর্ব_winrow = oldp->w_winrow + 2 * WINBAR_HEIGHT(oldp);
    }

    // copy tagstack and folds
    জন্য (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*tag = &newp->w_tagstack[i];
	*tag = oldp->w_tagstack[i];
	যদি (tag->tagname != ফাঁকা)
	    tag->tagname = vim_strsave(tag->tagname);
	যদি (tag->user_তথ্য != ফাঁকা)
	    tag->user_তথ্য = vim_strsave(tag->user_তথ্য);
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
	bool pushed = ঠেলো_highlight_overrides(newp->w_hl, newp->w_hl_len);
	যদি (newp->w_বাফার->b_term != ফাঁকা)
	    term_init_default_colors(newp->w_বাফার->b_term);
	যদি (pushed)
	    বের_করো_highlight_overrides();
    }
#endif
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    স্থির শূন্য
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
    পূর্ণসংখ্যা
win_valid_popup(win_T *win UNUSED)
{
#ifdef FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	যদি (wp == win)
	    ফেরত TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	যদি (wp == win)
	    ফেরত TRUE;
#endif
    ফেরত FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    পূর্ণসংখ্যা
win_valid(win_T *win)
{
    win_T	*wp;

    যদি (win == ফাঁকা)
	ফেরত FALSE;
    FOR_ALL_WINDOWS(wp)
	যদি (wp == win)
	    ফেরত TRUE;
    ফেরত win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_find_by_id(পূর্ণসংখ্যা id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_id == id)
	    ফেরত wp;
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	যদি (wp->w_id == id)
	    ফেরত wp;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	যদি (wp->w_id == id)
	    ফেরত wp;
#endif
    ফেরত ফাঁকা;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    পূর্ণসংখ্যা
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    যদি (win == ফাঁকা)
	ফেরত FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    যদি (wp == win)
		ফেরত TRUE;
	}
#ifdef FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    যদি (wp == win)
		ফেরত TRUE;
#endif
    }
    ফেরত win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    পূর্ণসংখ্যা
win_গুনতি(শূন্য)
{
    win_T	*wp;
    পূর্ণসংখ্যা		গুনতি = 0;

    FOR_ALL_WINDOWS(wp)
	++গুনতি;
    ফেরত গুনতি;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    পূর্ণসংখ্যা
make_windows(
    পূর্ণসংখ্যা		গুনতি,
    পূর্ণসংখ্যা		vertical)  // split windows vertically if TRUE
{
    পূর্ণসংখ্যা		maxcount;
    পূর্ণসংখ্যা		todo;

    যদি (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_width + curwin->w_vsep_width
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    অন্যথা
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_HEIGHT(curwin) + curwin->w_status_height
			- (p_wh - p_wmh)) / (p_wmh + statusline_height(curwin));
    }

    যদি (maxcount < 2)
	maxcount = 2;
    যদি (গুনতি > maxcount)
	গুনতি = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    যদি (গুনতি > 1)
	last_status(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    // todo is number of windows left to create
    জন্য (todo = গুনতি - 1; todo > 0; --todo)
	যদি (vertical)
	{
	    যদি (win_split(curwin->w_width - (curwin->w_width - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		বিরতি;
	}
	অন্যথা
	{
	    যদি (win_split(curwin->w_height - (curwin->w_height - todo
			    * statusline_height(curwin)) / (todo + 1)
			- statusline_height(curwin), WSP_ABOVE) == FAIL)
		বিরতি;
	}

    unblock_autocmds();

    // return actual number of windows
    ফেরত (গুনতি - todo);
}

/*
 * Exchange current and next window
 */
    স্থির শূন্য
win_exchange(long Prenum)
{
    frame_T	*frp;
    frame_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    পূর্ণসংখ্যা		অস্থায়ী;

    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত;
    যদি (ONE_WINDOW)	    // just one window
    {
	beep_flush();
	ফেরত;
    }
    যদি (text_or_buf_locked())
    {
	beep_flush();
	ফেরত;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * find window to exchange with
     */
    যদি (Prenum)
    {
	frp = curwin->w_frame->fr_parent->fr_child;
	যখন (frp != ফাঁকা && --Prenum > 0)
	    frp = frp->fr_পরবর্তী;
    }
    অন্যথা যদি (curwin->w_frame->fr_পরবর্তী != ফাঁকা)	// Swap with next
	frp = curwin->w_frame->fr_পরবর্তী;
    অন্যথা    // Swap last window in row/col with previous
	frp = curwin->w_frame->fr_পূর্ব;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    যদি (frp == ফাঁকা || frp->fr_win == ফাঁকা || frp->fr_win == curwin)
	ফেরত;
    wp = frp->fr_win;

/*
 * 1. remove curwin from the list. Remember after which window it was in wp2
 * 2. insert curwin before wp in the list
 * if wp != wp2
 *    3. remove wp from the list
 *    4. insert wp after wp2
 * 5. exchange the status line height and vsep width.
 */
    wp2 = curwin->w_পূর্ব;
    frp2 = curwin->w_frame->fr_পূর্ব;
    যদি (wp->w_পূর্ব != curwin)
    {
	win_remove(curwin, ফাঁকা);
	frame_remove(curwin->w_frame);
	win_append(wp->w_পূর্ব, curwin);
	frame_ঢোকাও(frp, curwin->w_frame);
    }
    যদি (wp != wp2)
    {
	win_remove(wp, ফাঁকা);
	frame_remove(wp->w_frame);
	win_append(wp2, wp);
	যদি (frp2 == ফাঁকা)
	    frame_ঢোকাও(wp->w_frame->fr_parent->fr_child, wp->w_frame);
	অন্যথা
	    frame_append(frp2, wp->w_frame);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    অস্থায়ী = curwin->w_status_height;
    যদি (অস্থায়ী != wp->w_status_height)
    {
	win_new_height(curwin, curwin->w_height + অস্থায়ী - wp->w_status_height);
	win_new_height(wp, wp->w_height + wp->w_status_height - অস্থায়ী);
    }
    curwin->w_status_height = wp->w_status_height;
    wp->w_status_height = অস্থায়ী;
    অস্থায়ী = curwin->w_vsep_width;
    curwin->w_vsep_width = wp->w_vsep_width;
    wp->w_vsep_width = অস্থায়ী;

    frame_fix_height(curwin);
    frame_fix_height(wp);
    frame_fix_width(curwin);
    frame_fix_width(wp);

    win_comp_pos();		// recompute window positions
#যদি defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    যদি (wp->w_বাফার != curbuf)
	reset_VIsual_and_resel();
    অন্যথা যদি (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    স্থির শূন্য
win_rotate(পূর্ণসংখ্যা upwards, পূর্ণসংখ্যা গুনতি)
{
    win_T	*wp1;
    win_T	*wp2;
    frame_T	*frp;
    পূর্ণসংখ্যা		n;

    যদি (ONE_WINDOW)		// nothing to do
    {
	beep_flush();
	ফেরত;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Check if all frames in this row/col have one window.
    FOR_ALL_FRAMES(frp, curwin->w_frame->fr_parent->fr_child)
	যদি (frp->fr_win == ফাঁকা)
	{
	    emsg(_(e_cannot_rotate_when_another_window_is_split));
	    ফেরত;
	}

    যখন (গুনতি--)
    {
	যদি (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_frame->fr_parent->fr_child;
	    wp1 = frp->fr_win;
	    win_remove(wp1, ফাঁকা);
	    frame_remove(frp);

	    // find last frame and append removed window/frame after it
	    জন্য ( ; frp->fr_পরবর্তী != ফাঁকা; frp = frp->fr_পরবর্তী)
		;
	    win_append(frp->fr_win, wp1);
	    frame_append(frp, wp1->w_frame);

	    wp2 = frp->fr_win;		// previously last window
	}
	অন্যথা			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    জন্য (frp = curwin->w_frame; frp->fr_পরবর্তী != ফাঁকা;
							   frp = frp->fr_পরবর্তী)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_পূর্ব;		    // will become last window
	    win_remove(wp1, ফাঁকা);
	    frame_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_parent->fr_child->fr_win->w_পূর্ব, wp1);
	    frame_ঢোকাও(frp->fr_parent->fr_child, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_status_height;
	যদি (n != wp1->w_status_height)
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
#যদি defined(FEAT_STL_OPT)
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
    পূর্ণসংখ্যা
win_splitmove(win_T *wp, পূর্ণসংখ্যা আকার_2, পূর্ণসংখ্যা flags)
{
    পূর্ণসংখ্যা		dir;
    পূর্ণসংখ্যা		height = wp->w_height;
    frame_T	*unflat_altfr;

    যদি (ONE_WINDOW)
	ফেরত OK;	// nothing to do
    যদি (check_split_disallowed(wp) == FAIL)
	ফেরত FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, ফাঁকা, &unflat_altfr);
    win_remove(wp, ফাঁকা);
    last_status(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    যদি (win_split_ins(আকার_2, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_পূর্ব, wp);
	ফেরত FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    যদি (আকার_2 == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(height, wp);
	যদি (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#যদি defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif
    ফেরত OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    শূন্য
win_move_after(win_T *win1, win_T *win2)
{
    পূর্ণসংখ্যা		height;

    // check if the arguments are reasonable
    যদি (win1 == win2)
	ফেরত;

    // check if there is something to do
    যদি (win2->w_পরবর্তী != win1)
    {
	যদি (win1->w_frame->fr_parent != win2->w_frame->fr_parent)
	{
	    iemsg("Trying to move a window into another frame");
	    ফেরত;
	}

	// may need to move the status line/vertical separator of the last
	// window
	যদি (win1 == lastwin)
	{
	    height = win1->w_পূর্ব->w_status_height;
	    win1->w_পূর্ব->w_status_height = win1->w_status_height;
	    win1->w_status_height = height;
	    যদি (win1->w_পূর্ব->w_vsep_width == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_পূর্ব->w_vsep_width = 0;
		win1->w_পূর্ব->w_frame->fr_width -= 1;
		win1->w_vsep_width = 1;
		win1->w_frame->fr_width += 1;
	    }
	}
	অন্যথা যদি (win2 == lastwin)
	{
	    height = win1->w_status_height;
	    win1->w_status_height = win2->w_status_height;
	    win2->w_status_height = height;
	    যদি (win1->w_vsep_width == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_width = 1;
		win2->w_frame->fr_width += 1;
		win1->w_vsep_width = 0;
		win1->w_frame->fr_width -= 1;
	    }
	}
	win_remove(win1, ফাঁকা);
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
    শূন্য
win_equal(
    win_T	*পরবর্তী_curwin,	// pointer to current window to be or NULL
    পূর্ণসংখ্যা		current,	// do only frame with current window
    পূর্ণসংখ্যা		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    যদি (dir == 0)
	dir = *p_ead;
    win_equal_rec(পরবর্তী_curwin == ফাঁকা ? curwin : পরবর্তী_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_height(),
		      topframe->fr_width, topframe->fr_height);
#যদি defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
    যদি (!is_aucmd_win(পরবর্তী_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    স্থির শূন্য
win_equal_rec(
    win_T	*পরবর্তী_curwin,	// pointer to current window to be or NULL
    পূর্ণসংখ্যা		current,	// do only frame with current window
    frame_T	*topfr,		// frame to set size off
    পূর্ণসংখ্যা		dir,		// 'v', 'h' or 'b', see win_equal()
    পূর্ণসংখ্যা		col,		// horizontal position for frame
    পূর্ণসংখ্যা		row,		// vertical position for frame
    পূর্ণসংখ্যা		width,		// new width of frame
    পূর্ণসংখ্যা		height)		// new height of frame
{
    পূর্ণসংখ্যা		n, m;
    পূর্ণসংখ্যা		extra_sep = 0;
    পূর্ণসংখ্যা		wincount, totwincount = 0;
    frame_T	*fr;
    পূর্ণসংখ্যা		পরবর্তী_curwin_আকার = 0;
    পূর্ণসংখ্যা		room = 0;
    পূর্ণসংখ্যা		new_আকার;
    পূর্ণসংখ্যা		has_পরবর্তী_curwin = 0;
    পূর্ণসংখ্যা		hnc;

    যদি (topfr->fr_layout == FR_LEAF)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	যদি (topfr->fr_height != height || topfr->fr_win->w_winrow != row
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
    অন্যথা যদি (topfr->fr_layout == FR_ROW)
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	যদি (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = frame_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    যদি (col + width >= firstwin->w_wincol + topframe->fr_width)
		extra_sep = 1;
	    অন্যথা
		extra_sep = 0;
	    totwincount = (n + extra_sep) / (p_wmw + 1);
	    has_পরবর্তী_curwin = frame_has_win(topfr, পরবর্তী_curwin);

	    /*
	     * Compute width for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal width when counting p_wiw for "next_curwin".
	     */
	    m = frame_minwidth(topfr, পরবর্তী_curwin);
	    room = width - m;
	    যদি (room < 0)
	    {
		পরবর্তী_curwin_আকার = p_wiw + room;
		room = 0;
	    }
	    অন্যথা
	    {
		পরবর্তী_curwin_আকার = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    যদি (!frame_fixed_width(fr))
			চালু;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minwidth(fr, NOWIN);
		    new_আকার = fr->fr_width;
		    যদি (frame_has_win(fr, পরবর্তী_curwin))
		    {
			room += p_wiw - p_wmw;
			পরবর্তী_curwin_আকার = 0;
			যদি (new_আকার < p_wiw)
			    new_আকার = p_wiw;
		    }
		    অন্যথা
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_পরবর্তী == ফাঁকা
					? extra_sep : 0)) / (p_wmw + 1);
		    room -= new_আকার - n;
		    যদি (room < 0)
		    {
			new_আকার += room;
			room = 0;
		    }
		    fr->fr_newwidth = new_আকার;
		}
		যদি (পরবর্তী_curwin_আকার == -1)
		{
		    যদি (!has_পরবর্তী_curwin)
			পরবর্তী_curwin_আকার = 0;
		    অন্যথা যদি (totwincount > 1
			    && (room + (totwincount - 2))
						  / (totwincount - 1) > p_wiw)
		    {
			// Can make all windows wider than 'winwidth', spread
			// the room equally.
			পরবর্তী_curwin_আকার = (room + p_wiw
					    + (totwincount - 1) * p_wmw
					    + (totwincount - 1)) / totwincount;
			room -= পরবর্তী_curwin_আকার - p_wiw;
		    }
		    অন্যথা
			পরবর্তী_curwin_আকার = p_wiw;
		}
	    }

	    যদি (has_পরবর্তী_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    যদি (fr->fr_পরবর্তী == ফাঁকা)
		// last frame gets all that remains (avoid roundoff error)
		new_আকার = width;
	    অন্যথা যদি (dir == 'v')
		new_আকার = fr->fr_width;
	    অন্যথা যদি (frame_fixed_width(fr))
	    {
		new_আকার = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    অন্যথা
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = frame_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_পরবর্তী == ফাঁকা ? extra_sep : 0))
								/ (p_wmw + 1);
		m = frame_minwidth(fr, পরবর্তী_curwin);
		যদি (has_পরবর্তী_curwin)
		    hnc = frame_has_win(fr, পরবর্তী_curwin);
		অন্যথা
		    hnc = FALSE;
		যদি (hnc)	    // don't count next_curwin
		    --wincount;
		যদি (totwincount == 0)
		    new_আকার = room;
		অন্যথা
		    new_আকার = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		যদি (hnc)	    // add next_curwin size
		{
		    পরবর্তী_curwin_আকার -= p_wiw - (m - n);
		    যদি (পরবর্তী_curwin_আকার < 0)
			পরবর্তী_curwin_আকার = 0;
		    new_আকার += পরবর্তী_curwin_আকার;
		    room -= new_আকার - পরবর্তী_curwin_আকার;
		}
		অন্যথা
		    room -= new_আকার;
		new_আকার += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    যদি (!current || dir != 'v' || topfr->fr_parent != ফাঁকা
		    || (new_আকার != fr->fr_width)
		    || frame_has_win(fr, পরবর্তী_curwin))
		win_equal_rec(পরবর্তী_curwin, current, fr, dir, col, row,
							    new_আকার, height);
	    col += new_আকার;
	    width -= new_আকার;
	    totwincount -= wincount;
	}
    }
    অন্যথা // topfr->fr_layout == FR_COL
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	যদি (dir != 'h')			// equalize frame heights
	{
	    totwincount = frame_wincount_in_height(topfr);
	    has_পরবর্তী_curwin = frame_has_win(topfr, পরবর্তী_curwin);

	    /*
	     * Compute height for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal height when counting p_wh for "next_curwin".
	     */
	    m = frame_minheight(topfr, পরবর্তী_curwin);
	    room = height - m;
	    যদি (room < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		পরবর্তী_curwin_আকার = p_wh + room;
		room = 0;
	    }
	    অন্যথা
	    {
		পরবর্তী_curwin_আকার = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    যদি (!frame_fixed_height(fr))
			চালু;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minheight(fr, NOWIN);
		    new_আকার = fr->fr_height;
		    যদি (frame_has_win(fr, পরবর্তী_curwin))
		    {
			room += p_wh - p_wmh;
			পরবর্তী_curwin_আকার = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			যদি (new_আকার < n + (p_wh - p_wmh))
			    new_আকার = n + (p_wh - p_wmh);
		    }
		    অন্যথা
			// These windows don't use up room.
			totwincount -= frame_wincount_in_height(fr);
		    room -= new_আকার - n;
		    যদি (room < 0)
		    {
			new_আকার += room;
			room = 0;
		    }
		    fr->fr_newheight = new_আকার;
		}
		যদি (পরবর্তী_curwin_আকার == -1)
		{
		    যদি (!has_পরবর্তী_curwin)
			পরবর্তী_curwin_আকার = 0;
		    অন্যথা যদি (totwincount > 1
			    && (room + (totwincount - 2))
						   / (totwincount - 1) > p_wh)
		    {
			// can make all windows higher than 'winheight',
			// spread the room equally.
			পরবর্তী_curwin_আকার = (room + p_wh
					   + (totwincount - 1) * p_wmh
					   + (totwincount - 1)) / totwincount;
			room -= পরবর্তী_curwin_আকার - p_wh;
		    }
		    অন্যথা
			পরবর্তী_curwin_আকার = p_wh;
		}
	    }

	    যদি (has_পরবর্তী_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    যদি (fr->fr_পরবর্তী == ফাঁকা)
		// last frame gets all that remains (avoid roundoff error)
		new_আকার = height;
	    অন্যথা যদি (dir == 'h')
		new_আকার = fr->fr_height;
	    অন্যথা যদি (frame_fixed_height(fr))
	    {
		new_আকার = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    অন্যথা
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = frame_minheight(fr, NOWIN);
		wincount = frame_wincount_in_height(fr);
		m = frame_minheight(fr, পরবর্তী_curwin);
		যদি (has_পরবর্তী_curwin)
		    hnc = frame_has_win(fr, পরবর্তী_curwin);
		অন্যথা
		    hnc = FALSE;
		যদি (hnc)	    // don't count next_curwin
		    --wincount;
		যদি (totwincount == 0)
		    new_আকার = room;
		অন্যথা
		    new_আকার = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		যদি (hnc)	    // add next_curwin size
		{
		    পরবর্তী_curwin_আকার -= p_wh - (m - n);
		    new_আকার += পরবর্তী_curwin_আকার;
		    room -= new_আকার - পরবর্তী_curwin_আকার;
		}
		অন্যথা
		    room -= new_আকার;
		new_আকার += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    যদি (!current || dir != 'h' || topfr->fr_parent != ফাঁকা
		    || (new_আকার != fr->fr_height)
		    || frame_has_win(fr, পরবর্তী_curwin))
		win_equal_rec(পরবর্তী_curwin, current, fr, dir, col, row,
							     width, new_আকার);
	    row += new_আকার;
	    height -= new_আকার;
	    totwincount -= wincount;
	}
    }
}

#যদি defined(FEAT_JOB_CHANNEL)
    শূন্য
leaving_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    যদি (!bt_prompt(win->w_বাফার) || is_aucmd_win(win))
	ফেরত;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_বাফার->b_prompt_ঢোকাও = restart_edit;
    যদি (restart_edit != 0 && mode_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    যদি ((State & MODE_ঢোকাও) && !stop_ঢোকাও_mode)
    {
	stop_ঢোকাও_mode = TRUE;
	যদি (win->w_বাফার->b_prompt_ঢোকাও == NUL)
	    win->w_বাফার->b_prompt_ঢোকাও = 'A';
    }
}

    শূন্য
entering_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    যদি (!bt_prompt(win->w_বাফার) || is_aucmd_win(win))
	ফেরত;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    যদি (win->w_বাফার->b_prompt_ঢোকাও != NUL)
	stop_ঢোকাও_mode = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    যদি ((State & MODE_ঢোকাও) == 0)
	restart_edit = win->w_বাফার->b_prompt_ঢোকাও;
}
#endif

    স্থির শূন্য
win_init_empty(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_সারিগুলি_valid = 0;
    wp->w_cursor.lnum = 1;
    wp->w_curswant = wp->w_cursor.col = 0;
    wp->w_cursor.coladd = 0;
    wp->w_pcmark.lnum = 1;	// pcmark not cleared but set to line 1
    wp->w_pcmark.col = 0;
    wp->w_পূর্ব_pcmark.lnum = 0;
    wp->w_পূর্ব_pcmark.col = 0;
    wp->w_topline = 1;
#ifdef FEAT_DIFF
    wp->w_topfill = 0;
#endif
    wp->w_botline = 2;
    wp->w_valid = 0;
#যদি defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    wp->w_s = &wp->w_বাফার->b_s;
#endif
#ifdef FEAT_TERMINAL
    term_reset_hlfwin(wp);
#endif
}

/*
 * Init the current window "curwin".
 * Called when a new file is being edited.
 */
    শূন্য
curwin_init(শূন্য)
{
    win_init_empty(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    শূন্য
close_windows(
    buf_T	*buf,
    পূর্ণসংখ্যা		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    পূর্ণসংখ্যা		গুনতি = tabpage_সূচক(ফাঁকা);

    ++RedrawingDisabled;

    জন্য (wp = firstwin; wp != ফাঁকা && !ONE_WINDOW; )
    {
	যদি (wp->w_বাফার == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_বাফার->b_locked > 0))
	{
	    যদি (window_layout_locked(CMD_আকার))
		যাও theend;  // Only give one error message.
	    যদি (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		বিরতি;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	অন্যথা
	    wp = wp->w_পরবর্তী;
    }

    // Also check windows in other tab pages.
    জন্য (tp = first_tabpage; tp != ফাঁকা; tp = nexttp)
    {
	nexttp = tp->tp_পরবর্তী;
	যদি (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		যদি (wp->w_বাফার == buf
		    && !(win_locked(wp) || wp->w_বাফার->b_locked > 0))
		{
		    যদি (window_layout_locked(CMD_আকার))
			যাও theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = first_tabpage;
		    বিরতি;
		}
    }

theend:
    যদি (RedrawingDisabled > 0)
	--RedrawingDisabled;

    যদি (গুনতি != tabpage_সূচক(ফাঁকা))
	apply_autocmds(EVENT_TABCLOSED, ফাঁকা, ফাঁকা, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    পূর্ণসংখ্যা
last_window(শূন্য)
{
    ফেরত (one_window() && first_tabpage->tp_পরবর্তী == ফাঁকা);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    পূর্ণসংখ্যা
one_window(শূন্য)
{
    win_T	*wp;
    পূর্ণসংখ্যা		seen_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	যদি (!is_aucmd_win(wp))
	{
	    যদি (seen_one)
		ফেরত FALSE;
	    seen_one = TRUE;
	}
    }
    ফেরত TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    স্থির পূর্ণসংখ্যা
close_last_window_tabpage(
    win_T	*win,
    পূর্ণসংখ্যা		free_buf,
    tabpage_T   *পূর্ব_curtab)
{
    যদি (!ONE_WINDOW)
	ফেরত FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_বাফার != ফাঁকা);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    যদি (valid_tabpage(পূর্ব_curtab) && পূর্ব_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, পূর্ব_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    যদি (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, ফাঁকা, ফাঁকা, FALSE, curbuf);
#যদি defined(FEAT_TABPANEL)
    যদি (p_stpl > 0)
	shell_new_columns();
#endif
    apply_autocmds(EVENT_WINENTER, ফাঁকা, ফাঁকা, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, ফাঁকা, ফাঁকা, FALSE, curbuf);
    যদি (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, ফাঁকা, ফাঁকা, FALSE, curbuf);
    ফেরত TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    স্থির শূন্য
win_close_বাফার(win_T *win, পূর্ণসংখ্যা action, পূর্ণসংখ্যা abort_if_last)
{
#ifdef FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    যদি (win->w_বাফার != ফাঁকা)
	reset_synblock(win);
#endif

#ifdef FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    যদি (win->w_বাফার != ফাঁকা && bt_quickfix(win->w_বাফার)
					&& win->w_বাফার->b_nwindows == 1)
	win->w_বাফার->b_p_bl = FALSE;
#endif

    // Close the link to the buffer.
    যদি (win->w_বাফার != ফাঁকা)
    {
	bufref_T    bufref;

	set_bufref(&bufref, curbuf);
	++win->w_locked;
	close_বাফার(win, win->w_বাফার, action, abort_if_last, TRUE, TRUE);
	যদি (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	যদি (!bufref_valid(&bufref))
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
    পূর্ণসংখ্যা
win_close(win_T *win, পূর্ণসংখ্যা free_buf)
{
    win_T	*wp;
    পূর্ণসংখ্যা		other_বাফার = FALSE;
    পূর্ণসংখ্যা		close_curwin = FALSE;
    পূর্ণসংখ্যা		dir;
    পূর্ণসংখ্যা		help_window = FALSE;
    পূর্ণসংখ্যা		quickfix_window = FALSE;
    tabpage_T   *পূর্ব_curtab = curtab;
    frame_T	*win_frame = win->w_frame->fr_parent;
#ifdef FEAT_DIFF
    পূর্ণসংখ্যা		had_diffmode = win->w_p_diff;
#endif
#ifdef MESSAGE_QUEUE
    পূর্ণসংখ্যা		did_decrement = FALSE;
#endif

#যদি defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    যদি (may_close_term_popup() == OK)
	ফেরত OK;
#endif
    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত FAIL;

    যদি (last_window())
    {
	emsg(_(e_cannot_close_last_window));
	ফেরত FAIL;
    }
    যদি (window_layout_locked(CMD_close))
	ফেরত FAIL;

    যদি (win_locked(win) || (win->w_বাফার != ফাঁকা
					       && win->w_বাফার->b_locked > 0))
	ফেরত FAIL; // window is already being closed
    যদি (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	ফেরত FAIL;
    }
    যদি ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_window())
    {
	emsg(_(e_cannot_close_window_only_autocmd_window_would_remain));
	ফেরত FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    যদি (close_last_window_tabpage(win, free_buf, পূর্ব_curtab))
	ফেরত FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    যদি (bt_help(win->w_বাফার))
	help_window = TRUE;
    অন্যথা
	clear_snapshot(curtab, SNAP_HELP_IDX);

    যদি (bt_quickfix(win->w_বাফার))
	quickfix_window = TRUE;
    অন্যথা
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    যদি (win == curwin)
    {
#ifdef FEAT_JOB_CHANNEL
	leaving_window(curwin);
#endif
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, ফাঁকা));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	যদি (wp->w_বাফার != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_বাফার = TRUE;
	    যদি (!win_valid(win))
		ফেরত FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, ফাঁকা, ফাঁকা, FALSE, curbuf);
	    যদি (!win_valid(win))
		ফেরত FAIL;
	    --win->w_locked;
	    যদি (last_window())
		ফেরত FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, ফাঁকা, ফাঁকা, FALSE, curbuf);
	যদি (!win_valid(win))
	    ফেরত FAIL;
	--win->w_locked;
	যদি (last_window())
	    ফেরত FAIL;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	যদি (aborting())
	    ফেরত FAIL;
#endif
    }

#ifdef FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    যদি (gui.in_use)
	out_flush();
#endif

#ifdef FEAT_PROP_POPUP
    যদি (popup_win_closed(win) && !win_valid(win))
	ফেরত FAIL;
#endif

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    যদি (!win_valid_any_tab(win))
	ফেরত OK;

    win_close_বাফার(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    যদি (win_valid(win) && win->w_বাফার == ফাঁকা
#যদি defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#endif
	    && last_window())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	যদি (curwin->w_বাফার == ফাঁকা)
	    curwin->w_বাফার = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    যদি (curtab != পূর্ব_curtab && win_valid_any_tab(win)
						      && win->w_বাফার == ফাঁকা)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, পূর্ব_curtab);
	ফেরত FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    যদি (!win_valid(win) || last_window()
	    || close_last_window_tabpage(win, free_buf, পূর্ব_curtab))
	ফেরত FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++split_disallowed;
#ifdef MESSAGE_QUEUE
    ++dont_parse_messages;
#endif

    যদি (win->w_বাফার != ফাঁকা)
	--win->w_বাফার->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, ফাঁকা);

    যদি (help_window || quickfix_window)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *পূর্ব_win = get_snapshot_curwin(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	যদি (win_valid(পূর্ব_win))
	    wp = পূর্ব_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    যদি (win == curwin)
    {
	curwin = wp;
#ifdef FEAT_QUICKFIX
	যদি (wp->w_p_pvw || bt_quickfix(wp->w_বাফার))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    জন্য (;;)
	    {
		যদি (wp->w_পরবর্তী == ফাঁকা)
		    wp = firstwin;
		অন্যথা
		    wp = wp->w_পরবর্তী;
		যদি (wp == curwin)
		    বিরতি;
		যদি (!wp->w_p_pvw && !bt_quickfix(wp->w_বাফার))
		{
		    curwin = wp;
		    বিরতি;
		}
	    }
	}
#endif
	curbuf = curwin->w_বাফার;
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

    যদি (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_frame->fr_parent == win_frame, dir);
    অন্যথা
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    যদি (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ifdef MESSAGE_QUEUE
	did_decrement =
#অন্যথা
	(শূন্য)
#endif
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	যদি (other_বাফার)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, ফাঁকা, ফাঁকা, FALSE, curbuf);
    }

    যদি (ONE_WINDOW && curwin->w_locked && curbuf->b_locked_split
	    && first_tabpage->tp_পরবর্তী != ফাঁকা)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, ফাঁকা, ফাঁকা, FALSE, curbuf);

    --split_disallowed;
#ifdef MESSAGE_QUEUE
    যদি (!did_decrement)
	--dont_parse_messages;
#endif

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    যদি (help_window || quickfix_window)
	restore_snapshot(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ifdef FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    যদি (diffopt_closeoff() && had_diffmode && curtab == পূর্ব_curtab)
    {
	পূর্ণসংখ্যা	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    যদি (dwin->w_p_diff)
		++diffcount;
	যদি (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#endif

#যদি defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    যদি (gui.in_use && !win_hasvertsplit())
	gui_init_which_components(ফাঁকা);
#endif

    redraw_all_later(UPD_NOT_VALID);
    ফেরত OK;
}

    স্থির শূন্য
trigger_winnewpre(শূন্য)
{
    window_layout_lock();
    apply_autocmds(EVENT_WINNEWPRE, ফাঁকা, ফাঁকা, FALSE, ফাঁকা);
    window_layout_unlock();
}

    স্থির শূন্য
trigger_winclosed(win_T *win)
{
    স্থির পূর্ণসংখ্যা	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    যদি (recursive)
	ফেরত;
    recursive = TRUE;
    vim_snprintf((অক্ষর *)winid, আকার(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_বাফার);
    recursive = FALSE;
}

    শূন্য
trigger_tabclosedpre(tabpage_T *tp)
{
    স্থির পূর্ণসংখ্যা	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    যদি (!has_tabclosedpre() || recursive)
	ফেরত;

    যদি (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    window_layout_lock();
    apply_autocmds(EVENT_TABCLOSEDPRE, ফাঁকা, ফাঁকা, FALSE, ফাঁকা);
    window_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    যদি (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    অন্যথা
	// fall back to the first tabpage
	goto_tabpage_tp(first_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    শূন্য
snapshot_windows_scroll_আকার(শূন্য)
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

স্থির পূর্ণসংখ্যা did_initial_scroll_আকার_snapshot = FALSE;

    শূন্য
may_make_initial_scroll_আকার_snapshot(শূন্য)
{
    যদি (!did_initial_scroll_আকার_snapshot)
    {
	did_initial_scroll_আকার_snapshot = TRUE;
	snapshot_windows_scroll_আকার();
    }
}

#ifdef FEAT_EVAL
/*
 * Create a dictionary with information about size and scroll changes in a
 * window.
 * Returns the dictionary with refcount set to one.
 * Returns NULL when out of memory.
 */
    স্থির dict_T *
make_win_info_dict(
	পূর্ণসংখ্যা width,
	পূর্ণসংখ্যা height,
	পূর্ণসংখ্যা topline,
# ifdef FEAT_DIFF
	পূর্ণসংখ্যা topfill,
# endif
	পূর্ণসংখ্যা leftcol,
	পূর্ণসংখ্যা skipcol)
{
    dict_T *d = dict_alloc();
    যদি (d == ফাঁকা)
	ফেরত ফাঁকা;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    যখন (1)
    {
	typval_T tv;
	tv.v_lock = 0;
	tv.v_type = VAR_সংখ্যা;

	tv.vval.v_সংখ্যা = width;
	যদি (dict_add_tv(d, "width", &tv) == FAIL)
	    বিরতি;
	tv.vval.v_সংখ্যা = height;
	যদি (dict_add_tv(d, "height", &tv) == FAIL)
	    বিরতি;
	tv.vval.v_সংখ্যা = topline;
	যদি (dict_add_tv(d, "topline", &tv) == FAIL)
	    বিরতি;
# ifdef FEAT_DIFF
	tv.vval.v_সংখ্যা = topfill;
# অন্যথা
	tv.vval.v_সংখ্যা = 0;
# endif
	যদি (dict_add_tv(d, "topfill", &tv) == FAIL)
	    বিরতি;
	tv.vval.v_সংখ্যা = leftcol;
	যদি (dict_add_tv(d, "leftcol", &tv) == FAIL)
	    বিরতি;
	tv.vval.v_সংখ্যা = skipcol;
	যদি (dict_add_tv(d, "skipcol", &tv) == FAIL)
	    বিরতি;
	ফেরত d;
    }
    dict_unref(d);
    ফেরত ফাঁকা;
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
    স্থির শূন্য
check_window_scroll_resize(
	পূর্ণসংখ্যা	*আকার_গুনতি,
	win_T	**first_scroll_win,
	win_T	**first_আকার_win,
	list_T	*winlist UNUSED,
	dict_T	*v_event UNUSED)
{
#ifdef FEAT_EVAL
    পূর্ণসংখ্যা listidx = 0;
    পূর্ণসংখ্যা tot_width = 0;
    পূর্ণসংখ্যা tot_height = 0;
    পূর্ণসংখ্যা tot_topline = 0;
# ifdef FEAT_DIFF
    পূর্ণসংখ্যা tot_topfill = 0;
# endif
    পূর্ণসংখ্যা tot_leftcol = 0;
    পূর্ণসংখ্যা tot_skipcol = 0;
#endif

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	পূর্ণসংখ্যা ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	পূর্ণসংখ্যা আকার_changed = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_last_width != wp->w_width
			    || wp->w_last_height != wp->w_height);
	যদি (আকার_changed)
	{
#ifdef FEAT_EVAL
	    যদি (winlist != ফাঁকা)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_lock = 0;
		tv.v_type = VAR_সংখ্যা;
		tv.vval.v_সংখ্যা = wp->w_id;
		list_set_item(winlist, listidx++, &tv);
	    }
	    অন্যথা
#endif
		যদি (আকার_গুনতি != ফাঁকা)
	    {
		++*আকার_গুনতি;
		যদি (*first_আকার_win == ফাঁকা)
		    *first_আকার_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		যদি (*first_scroll_win == ফাঁকা && !ignore_scroll)
		    *first_scroll_win = wp;
	    }
	}

	পূর্ণসংখ্যা scroll_changed = !ignore_scroll
				&& (wp->w_last_topline != wp->w_topline
#ifdef FEAT_DIFF
				|| wp->w_last_topfill != wp->w_topfill
#endif
				|| wp->w_last_leftcol != wp->w_leftcol
				|| wp->w_last_skipcol != wp->w_skipcol);
	যদি (scroll_changed
	    && first_scroll_win != ফাঁকা && *first_scroll_win == ফাঁকা)
	    *first_scroll_win = wp;

#ifdef FEAT_EVAL
	যদি ((আকার_changed || scroll_changed) && v_event != ফাঁকা)
	{
	    // Add info about this window to the v:event dictionary.
	    পূর্ণসংখ্যা width = wp->w_width - wp->w_last_width;
	    পূর্ণসংখ্যা height = wp->w_height - wp->w_last_height;
	    পূর্ণসংখ্যা topline = wp->w_topline - wp->w_last_topline;
# ifdef FEAT_DIFF
	    পূর্ণসংখ্যা topfill = wp->w_topfill - wp->w_last_topfill;
# endif
	    পূর্ণসংখ্যা leftcol = wp->w_leftcol - wp->w_last_leftcol;
	    পূর্ণসংখ্যা skipcol = wp->w_skipcol - wp->w_last_skipcol;
	    dict_T *d = make_win_info_dict(width, height, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    যদি (d == ফাঁকা)
		বিরতি;
	    অক্ষর winid[NUMBUFLEN];
	    vim_snprintf(winid, আকার(winid), "%d", wp->w_id);
	    যদি (dict_add_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		বিরতি;
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
    যদি (v_event != ফাঁকা)
    {
	dict_T *alldict = make_win_info_dict(tot_width, tot_height, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	যদি (alldict != ফাঁকা)
	{
	    যদি (dict_add_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    অন্যথা
		--alldict->dv_refcount;
	}
    }
#endif
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    শূন্য
may_trigger_win_scrolled_resized(শূন্য)
{
    স্থির পূর্ণসংখ্যা	    recursive = FALSE;
    পূর্ণসংখ্যা		    do_resize = has_winresized();
    পূর্ণসংখ্যা		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    যদি (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_আকার_snapshot)
	ফেরত;

    পূর্ণসংখ্যা আকার_গুনতি = 0;
    win_T *first_scroll_win = ফাঁকা, *first_আকার_win = ফাঁকা;
    check_window_scroll_resize(&আকার_গুনতি, &first_scroll_win, &first_আকার_win,
								    ফাঁকা, ফাঁকা);
    পূর্ণসংখ্যা trigger_resize = do_resize && আকার_গুনতি > 0;
    পূর্ণসংখ্যা trigger_scroll = do_scroll && first_scroll_win != ফাঁকা;
    যদি (!trigger_resize && !trigger_scroll)
	ফেরত;  // no relevant changes
#ifdef FEAT_EVAL
    list_T *windows_list = ফাঁকা;
    যদি (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_বস্তুসমূহ(আকার_গুনতি);
	যদি (windows_list != ফাঁকা)
	    check_window_scroll_resize(ফাঁকা, ফাঁকা, ফাঁকা, windows_list, ফাঁকা);
    }

    dict_T *scroll_dict = ফাঁকা;
    যদি (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	যদি (scroll_dict != ফাঁকা)
	{
	    scroll_dict->dv_refcount = 1;
	    check_window_scroll_resize(ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, scroll_dict);
	}
    }
#endif

    // WinScrolled/WinResized are triggered only once, even when multiple
    // windows scrolled or changed size.  Store the current values before
    // triggering the event, if a scroll or resize happens as a side effect
    // then WinScrolled/WinResized is triggered for that later.
    snapshot_windows_scroll_আকার();

    // "curwin" may be different from the actual current window, make
    // sure it can be restored.
    window_layout_lock();
    recursive = TRUE;

    // If both are to be triggered do WinResized first.
    যদি (trigger_resize
#ifdef FEAT_EVAL
	    && windows_list != ফাঁকা
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	যদি (dict_add_list(v_event, "windows", windows_list) == OK)
	{
	    dict_set_বস্তুসমূহ_ro(v_event);
#endif
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((অক্ষর *)winid, আকার(winid), "%d",
							 first_আকার_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     first_আকার_win->w_বাফার);
#ifdef FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#endif
    }

    যদি (trigger_scroll
#ifdef FEAT_EVAL
	    && scroll_dict != ফাঁকা
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", ফাঁকা);
	dict_set_বস্তুসমূহ_ro(v_event);
	dict_unref(scroll_dict);
#endif
	char_u winid[NUMBUFLEN];
	vim_snprintf((অক্ষর *)winid, আকার(winid), "%d",
						       first_scroll_win->w_id);
	apply_autocmds(EVENT_WINSCROLLED, winid, winid, FALSE,
						   first_scroll_win->w_বাফার);
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
    শূন্য
win_close_othertab(win_T *win, পূর্ণসংখ্যা free_buf, tabpage_T *tp)
{
    win_T	*wp;
    পূর্ণসংখ্যা		dir;
    tabpage_T   *ptp = ফাঁকা;
    পূর্ণসংখ্যা		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    যদি (window_layout_locked(CMD_আকার))
	ফেরত;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    যদি (win_locked(win) || (win->w_বাফার != ফাঁকা
					       && win->w_বাফার->b_locked > 0))
	ফেরত; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    যদি (win->w_বাফার != ফাঁকা)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	যদি (!win_valid_any_tab(win))
	    ফেরত;
    }

    যদি (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	যদি (!win_valid_any_tab(win))
	    ফেরত;
    }

    যদি (win->w_বাফার != ফাঁকা)
	// Close the link to the buffer.
	close_বাফার(win, win->w_বাফার, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    জন্য (ptp = first_tabpage; ptp != ফাঁকা && ptp != tp; ptp = ptp->tp_পরবর্তী)
	;
    যদি (ptp == ফাঁকা || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	যদি (win_valid_any_tab(win) && win->w_বাফার == ফাঁকা)
	{
	    win->w_বাফার = firstbuf;
	    ++firstbuf->b_nwindows;
	    যদি (win == curwin)
		curbuf = curwin->w_বাফার;
	    win_init_empty(win);
	}
	ফেরত;
    }

    // Autocommands may have closed the window already.
    জন্য (wp = tp->tp_firstwin; wp != ফাঁকা && wp != win; wp = wp->w_পরবর্তী)
	;
    যদি (wp == ফাঁকা)
	ফেরত;

    // When closing the last window in a tab page remove the tab page.
    যদি (tp->tp_firstwin == tp->tp_lastwin)
    {
	পূর্ণসংখ্যা	h = tabline_height();

	যদি (tp == first_tabpage)
	    first_tabpage = tp->tp_পরবর্তী;
	অন্যথা
	{
	    জন্য (ptp = first_tabpage; ptp != ফাঁকা && ptp->tp_পরবর্তী != tp;
							   ptp = ptp->tp_পরবর্তী)
		;
	    যদি (ptp == ফাঁকা)
	    {
		internal_error("win_close_othertab()");
		ফেরত;
	    }
	    ptp->tp_পরবর্তী = tp->tp_পরবর্তী;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	যদি (h != tabline_height())
	    shell_new_rows();
    }

    যদি (win->w_বাফার != ফাঁকা)
	--win->w_বাফার->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    যদি (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    স্থির win_T *
win_free_mem(
    win_T	*win,
    পূর্ণসংখ্যা		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == ফাঁকা ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_frame;
    wp = winframe_remove(win, dirp, tp, ফাঁকা);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    যদি (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    ফেরত wp;
}

#যদি defined(EXITFREE)
    শূন্য
win_free_all(শূন্য)
{
    পূর্ণসংখ্যা		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = ফাঁকা;
    cmdwin_win = ফাঁকা;

    যখন (first_tabpage->tp_পরবর্তী != ফাঁকা)
	tabpage_close(TRUE);

    জন্য (পূর্ণসংখ্যা i = 0; i < AUCMD_WIN_গুনতি; ++i)
	যদি (aucmd_win[i].auc_win != ফাঁকা)
	{
	    (শূন্য)win_free_mem(aucmd_win[i].auc_win, &dummy, ফাঁকা);
	    aucmd_win[i].auc_win = ফাঁকা;
	}

    যখন (firstwin != ফাঁকা)
	(শূন্য)win_free_mem(firstwin, &dummy, ফাঁকা);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = ফাঁকা;
}
#endif

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    পূর্ণসংখ্যা		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    frame_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    frame_T	*frp, *frp2, *frp3;
    frame_T	*frp_close = win->w_frame;
    win_T	*wp;
    পূর্ণসংখ্যা		row, col;

    /*
     * If there is only one window there is nothing to remove.
     */
    যদি (tp == ফাঁকা ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	ফেরত ফাঁকা;

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
    যদি (win->w_vsep_width == 0 && frp_close->fr_parent->fr_layout == FR_ROW &&
	    frp_close->fr_পূর্ব != ফাঁকা)
	frame_set_vsep(frp_close->fr_পূর্ব, FALSE);

    // Remove this frame from the list of frames.
    frame_remove(frp_close);

    যদি (frp_close->fr_parent->fr_layout == FR_COL)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	যদি (frp2->fr_win != ফাঁকা && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_পূর্ব;
	    frp3 = frp_close->fr_পরবর্তী;
	    যখন (frp != ফাঁকা || frp3 != ফাঁকা)
	    {
		যদি (frp != ফাঁকা)
		{
		    যদি (!frame_fixed_height(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			বিরতি;
		    }
		    frp = frp->fr_পূর্ব;
		}
		যদি (frp3 != ফাঁকা)
		{
		    যদি (frp3->fr_win != ফাঁকা && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			বিরতি;
		    }
		    frp3 = frp3->fr_পরবর্তী;
		}
	    }
	}
	frame_new_height(frp2, frp2->fr_height + frp_close->fr_height,
			    frp2 == frp_close->fr_পরবর্তী, !frame_fixed_height(frp2), FALSE);
	*dirp = 'v';
    }
    অন্যথা
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	যদি (frp2->fr_win != ফাঁকা && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_পূর্ব;
	    frp3 = frp_close->fr_পরবর্তী;
	    যখন (frp != ফাঁকা || frp3 != ফাঁকা)
	    {
		যদি (frp != ফাঁকা)
		{
		    যদি (!frame_fixed_width(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			বিরতি;
		    }
		    frp = frp->fr_পূর্ব;
		}
		যদি (frp3 != ফাঁকা)
		{
		    যদি (frp3->fr_win != ফাঁকা && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			বিরতি;
		    }
		    frp3 = frp3->fr_পরবর্তী;
		}
	    }
	}
	frame_new_width(frp2, frp2->fr_width + frp_close->fr_width,
			    frp2 == frp_close->fr_পরবর্তী, !frame_fixed_width(frp2));
	*dirp = 'h';
    }

    // If the altframe wasn't adjacent and left/above, resizing it will have
    // changed window positions within the parent frame.  Recompute them.
    যদি (frp2 != frp_close->fr_পূর্ব)
	frame_comp_pos(frp_close->fr_parent, &row, &col);

    যদি (unflat_altfr == ফাঁকা)
	frame_flatten(frp2);
    অন্যথা
	*unflat_altfr = frp2;

    frame_locked--;

    ফেরত wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    স্থির শূন্য
frame_flatten(frame_T *frp)
{
    frame_T	*frp2, *frp3;

    যদি (frp->fr_পরবর্তী != ফাঁকা || frp->fr_পূর্ব != ফাঁকা)
	ফেরত;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_parent->fr_layout = frp->fr_layout;
    frp->fr_parent->fr_child = frp->fr_child;
    FOR_ALL_FRAMES(frp2, frp->fr_child)
	frp2->fr_parent = frp->fr_parent;
    frp->fr_parent->fr_win = frp->fr_win;
    যদি (frp->fr_win != ফাঁকা)
	frp->fr_win->w_frame = frp->fr_parent;
    frp2 = frp->fr_parent;
    যদি (topframe->fr_child == frp)
	topframe->fr_child = frp2;
    vim_free(frp);

    frp = frp2->fr_parent;
    যদি (frp != ফাঁকা && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	যদি (frp->fr_child == frp2)
	    frp->fr_child = frp2->fr_child;
	frp2->fr_child->fr_পূর্ব = frp2->fr_পূর্ব;
	যদি (frp2->fr_পূর্ব != ফাঁকা)
	    frp2->fr_পূর্ব->fr_পরবর্তী = frp2->fr_child;
	জন্য (frp3 = frp2->fr_child; ; frp3 = frp3->fr_পরবর্তী)
	{
	    frp3->fr_parent = frp;
	    যদি (frp3->fr_পরবর্তী == ফাঁকা)
	    {
		frp3->fr_পরবর্তী = frp2->fr_পরবর্তী;
		যদি (frp2->fr_পরবর্তী != ফাঁকা)
		    frp2->fr_পরবর্তী->fr_পূর্ব = frp3;
		বিরতি;
	    }
	}
	যদি (topframe->fr_child == frp2)
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
    স্থির শূন্য
winframe_restore(win_T *wp, পূর্ণসংখ্যা dir, frame_T *unflat_altfr)
{
    frame_T	*frp = wp->w_frame;

    // Put "wp"'s frame back where it was.
    যদি (frp->fr_পূর্ব != ফাঁকা)
	frame_append(frp->fr_পূর্ব, frp);
    অন্যথা
	frame_ঢোকাও(frp->fr_পরবর্তী, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    যদি (wp->w_vsep_width == 0
	    && frp->fr_parent->fr_layout == FR_ROW && frp->fr_পূর্ব != ফাঁকা)
	frame_set_vsep(frp->fr_পূর্ব, TRUE);

    // Statuslines above may have been lost.  Restore them.
    যদি (wp->w_status_height == 0
	    && frp->fr_parent->fr_layout == FR_COL && frp->fr_পূর্ব != ফাঁকা)
	frame_add_statusline(frp->fr_পূর্ব);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    যদি (dir == 'v')
    {
	frame_new_height(unflat_altfr, unflat_altfr->fr_height - frp->fr_height,
		unflat_altfr == frp->fr_পরবর্তী, FALSE, FALSE);
    }
    অন্যথা যদি (dir == 'h')
    {
	frame_new_width(unflat_altfr, unflat_altfr->fr_width - frp->fr_width,
		unflat_altfr == frp->fr_পরবর্তী, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    যদি (unflat_altfr != frp->fr_পূর্ব)
    {
	win_T	*topleft = frame2win(frp->fr_parent);
	পূর্ণসংখ্যা	row = topleft->w_winrow;
	পূর্ণসংখ্যা	col = topleft->w_wincol;

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
    স্থির frame_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    frame_T	*other_fr, *target_fr;

    যদি (tp == ফাঁকা ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	ফেরত alt_tabpage()->tp_curwin->w_frame;

    frp = win->w_frame;

    যদি (frp->fr_পূর্ব == ফাঁকা)
	ফেরত frp->fr_পরবর্তী;
    যদি (frp->fr_পরবর্তী == ফাঁকা)
	ফেরত frp->fr_পূর্ব;

    // By default the next window will get the space that was abandoned by this
    // window
    target_fr = frp->fr_পরবর্তী;
    other_fr  = frp->fr_পূর্ব;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    যদি (frp->fr_parent != ফাঁকা && frp->fr_parent->fr_layout == FR_COL && p_sb)
    {
	target_fr = frp->fr_পূর্ব;
	other_fr  = frp->fr_পরবর্তী;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    যদি (frp->fr_parent != ফাঁকা && frp->fr_parent->fr_layout == FR_ROW && p_spr)
    {
	target_fr = frp->fr_পূর্ব;
	other_fr  = frp->fr_পরবর্তী;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    যদি (frp->fr_parent != ফাঁকা && frp->fr_parent->fr_layout == FR_ROW)
    {
	যদি (frame_fixed_width(target_fr) && !frame_fixed_width(other_fr))
	    target_fr = other_fr;
    }
    অন্যথা
    {
	যদি (frame_fixed_height(target_fr) && !frame_fixed_height(other_fr))
	    target_fr = other_fr;
    }

    ফেরত target_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    স্থির tabpage_T *
alt_tabpage(শূন্য)
{
    tabpage_T	*tp = ফাঁকা;
    পূর্ণসংখ্যা		forward;

    // Use the last accessed tab page, if possible.
    যদি ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	ফেরত lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_পরবর্তী != ফাঁকা &&
	    ((tcl_flags & TCL_বাম) == 0 || curtab == first_tabpage);

    যদি (forward)
	tp = curtab->tp_পরবর্তী;
    অন্যথা
	// Use the previous tab page.
	জন্য (tp = first_tabpage; tp->tp_পরবর্তী != curtab; tp = tp->tp_পরবর্তী)
	    ;

    ফেরত tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    স্থির win_T *
frame2win(frame_T *frp)
{
    যখন (frp->fr_win == ফাঁকা)
	frp = frp->fr_child;
    ফেরত frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    স্থির পূর্ণসংখ্যা
frame_has_win(frame_T *frp, win_T *wp)
{
    frame_T	*p;

    যদি (frp->fr_layout == FR_LEAF)
	ফেরত frp->fr_win == wp;

    FOR_ALL_FRAMES(p, frp->fr_child)
	যদি (frame_has_win(p, wp))
	    ফেরত TRUE;
    ফেরত FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
স্থির পূর্ণসংখ্যা min_set_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    স্থির শূন্য
frame_new_height(
    frame_T	*topfrp,
    পূর্ণসংখ্যা		height,
    পূর্ণসংখ্যা		topfirst,	// resize topmost contained frame first
    পূর্ণসংখ্যা		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    পূর্ণসংখ্যা		set_ch)		// set 'cmdheight' to resize topframe
{
    frame_T	*frp;
    পূর্ণসংখ্যা		extra_সারিগুলি;
    পূর্ণসংখ্যা		h;

    যদি (topfrp->fr_parent == ফাঁকা && set_ch)
    {
	// topframe: update the command line height, with side effects.
	পূর্ণসংখ্যা new_ch = MAX(min_set_ch, p_ch + topfrp->fr_height - height);
	পূর্ণসংখ্যা save_ch = min_set_ch;
	যদি (new_ch != p_ch)
	    set_option_মান((char_u *)"cmdheight", new_ch, ফাঁকা, 0);
	min_set_ch = save_ch;
	height = MIN(height, ROWS_AVAIL);
    }
    যদি (topfrp->fr_win != ফাঁকা)
    {
	// Simple case: just one window.
	win_new_height(topfrp->fr_win,
				    height - topfrp->fr_win->w_status_height
					      - WINBAR_HEIGHT(topfrp->fr_win));
    }
    অন্যথা যদি (topfrp->fr_layout == FR_ROW)
    {
	করো
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_height(frp, height, topfirst, wfh, set_ch);
		যদি (frp->fr_height > height)
		{
		    // Could not fit the windows, make the whole row higher.
		    height = frp->fr_height;
		    বিরতি;
		}
	    }
	}
	যখন (frp != ফাঁকা);
    }
    অন্যথা    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_child;
	যদি (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    যখন (frame_fixed_height(frp))
	    {
		frp = frp->fr_পরবর্তী;
		যদি (frp == ফাঁকা)
		    ফেরত;	    // no frame without 'wfh', give up
	    }
	যদি (!topfirst)
	{
	    // Find the bottom frame of this column
	    যখন (frp->fr_পরবর্তী != ফাঁকা)
		frp = frp->fr_পরবর্তী;
	    যদি (wfh)
		// Advance back for frames with one window with 'wfh' set.
		যখন (frame_fixed_height(frp))
		    frp = frp->fr_পূর্ব;
	}

	extra_সারিগুলি = height - topfrp->fr_height;
	যদি (extra_সারিগুলি < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    যখন (frp != ফাঁকা)
	    {
		h = frame_minheight(frp, ফাঁকা);
		যদি (frp->fr_height + extra_সারিগুলি < h)
		{
		    extra_সারিগুলি += frp->fr_height - h;
		    frame_new_height(frp, h, topfirst, wfh, set_ch);
		}
		অন্যথা
		{
		    frame_new_height(frp, frp->fr_height + extra_সারিগুলি,
							topfirst, wfh, set_ch);
		    বিরতি;
		}
		যদি (topfirst)
		{
		    করো
			frp = frp->fr_পরবর্তী;
		    যখন (wfh && frp != ফাঁকা && frame_fixed_height(frp));
		}
		অন্যথা
		{
		    করো
			frp = frp->fr_পূর্ব;
		    যখন (wfh && frp != ফাঁকা && frame_fixed_height(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		যদি (frp == ফাঁকা)
		    height -= extra_সারিগুলি;
	    }
	}
	অন্যথা যদি (extra_সারিগুলি > 0)
	{
	    // increase height of bottom or top frame
	    frame_new_height(frp, frp->fr_height + extra_সারিগুলি, topfirst, wfh,
									set_ch);
	}
    }
    topfrp->fr_height = height;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    স্থির পূর্ণসংখ্যা
frame_fixed_height(frame_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    যদি (frp->fr_win != ফাঁকা)
	ফেরত frp->fr_win->w_p_wfh;

    যদি (frp->fr_layout == FR_ROW)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    যদি (frame_fixed_height(frp))
		ফেরত TRUE;
	ফেরত FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	যদি (!frame_fixed_height(frp))
	    ফেরত FALSE;
    ফেরত TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    স্থির পূর্ণসংখ্যা
frame_fixed_width(frame_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    যদি (frp->fr_win != ফাঁকা)
	ফেরত frp->fr_win->w_p_wfw;

    যদি (frp->fr_layout == FR_COL)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    যদি (frame_fixed_width(frp))
		ফেরত TRUE;
	ফেরত FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	যদি (!frame_fixed_width(frp))
	    ফেরত FALSE;
    ফেরত TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    স্থির শূন্য
frame_add_statusline(frame_T *frp)
{
    win_T	*wp;

    যদি (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	wp->w_status_height = statusline_height(wp);
    }
    অন্যথা যদি (frp->fr_layout == FR_ROW)
    {
	// Handle all the frames in the row.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_statusline(frp);
    }
    অন্যথা // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	জন্য (frp = frp->fr_child; frp->fr_পরবর্তী != ফাঁকা; frp = frp->fr_পরবর্তী)
	    ;
	frame_add_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    স্থির শূন্য
frame_new_width(
    frame_T	*topfrp,
    পূর্ণসংখ্যা		width,
    পূর্ণসংখ্যা		leftfirst,	// resize leftmost contained frame first
    পূর্ণসংখ্যা		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    frame_T	*frp;
    পূর্ণসংখ্যা		extra_cols;
    পূর্ণসংখ্যা		w;
    win_T	*wp;

    যদি (topfrp->fr_layout == FR_LEAF)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	জন্য (frp = topfrp; frp->fr_parent != ফাঁকা; frp = frp->fr_parent)
	    যদি (frp->fr_parent->fr_layout == FR_ROW && frp->fr_পরবর্তী != ফাঁকা)
		বিরতি;
	যদি (frp->fr_parent == ফাঁকা)
	    wp->w_vsep_width = 0;
	win_new_width(wp, width - wp->w_vsep_width);
    }
    অন্যথা যদি (topfrp->fr_layout == FR_COL)
    {
	করো
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_width(frp, width, leftfirst, wfw);
		যদি (frp->fr_width > width)
		{
		    // Could not fit the windows, make whole column wider.
		    width = frp->fr_width;
		    বিরতি;
		}
	    }
	} যখন (frp != ফাঁকা);
    }
    অন্যথা    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_child;
	যদি (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    যখন (frame_fixed_width(frp))
	    {
		frp = frp->fr_পরবর্তী;
		যদি (frp == ফাঁকা)
		    ফেরত;	    // no frame without 'wfw', give up
	    }
	যদি (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    যখন (frp->fr_পরবর্তী != ফাঁকা)
		frp = frp->fr_পরবর্তী;
	    যদি (wfw)
		// Advance back for frames with one window with 'wfw' set.
		যখন (frame_fixed_width(frp))
		    frp = frp->fr_পূর্ব;
	}

	extra_cols = width - topfrp->fr_width;
	যদি (extra_cols < 0)
	{
	    // reduce frame width, rightmost frame first
	    যখন (frp != ফাঁকা)
	    {
		w = frame_minwidth(frp, ফাঁকা);
		যদি (frp->fr_width + extra_cols < w)
		{
		    extra_cols += frp->fr_width - w;
		    frame_new_width(frp, w, leftfirst, wfw);
		}
		অন্যথা
		{
		    frame_new_width(frp, frp->fr_width + extra_cols,
							      leftfirst, wfw);
		    বিরতি;
		}
		যদি (leftfirst)
		{
		    করো
			frp = frp->fr_পরবর্তী;
		    যখন (wfw && frp != ফাঁকা && frame_fixed_width(frp));
		}
		অন্যথা
		{
		    করো
			frp = frp->fr_পূর্ব;
		    যখন (wfw && frp != ফাঁকা && frame_fixed_width(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		যদি (frp == ফাঁকা)
		    width -= extra_cols;
	    }
	}
	অন্যথা যদি (extra_cols > 0)
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
    স্থির শূন্য
frame_set_vsep(frame_T *frp, পূর্ণসংখ্যা add)
{
    win_T	*wp;

    যদি (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	যদি (add && wp->w_vsep_width == 0)
	{
	    যদি (wp->w_width > 0)	// don't make it negative
		win_new_width(wp, wp->w_width - 1);
	    wp->w_vsep_width = 1;
	}
	অন্যথা যদি (!add && wp->w_vsep_width == 1)
	{
	    win_new_width(wp, wp->w_width + 1);
	    wp->w_vsep_width = 0;
	}
    }
    অন্যথা যদি (frp->fr_layout == FR_COL)
    {
	// Handle all the frames in the column.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_set_vsep(frp, add);
    }
    অন্যথা // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_child;
	যখন (frp->fr_পরবর্তী != ফাঁকা)
	    frp = frp->fr_পরবর্তী;
	frame_set_vsep(frp, add);
    }
}

/*
 * Set frame width from the window it contains.
 */
    স্থির শূন্য
frame_fix_width(win_T *wp)
{
    wp->w_frame->fr_width = wp->w_width + wp->w_vsep_width;
}

/*
 * Set frame height from the window it contains.
 */
    স্থির শূন্য
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
    স্থির পূর্ণসংখ্যা
frame_minheight(frame_T *topfrp, win_T *পরবর্তী_curwin)
{
    frame_T	*frp;
    পূর্ণসংখ্যা		m;
    পূর্ণসংখ্যা		n;

    যদি (topfrp->fr_win != ফাঁকা)
    {
	যদি (topfrp->fr_win == পরবর্তী_curwin)
	    m = p_wh + topfrp->fr_win->w_status_height;
	অন্যথা
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_status_height;
	    যদি (topfrp->fr_win == curwin && পরবর্তী_curwin == ফাঁকা)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		যদি (p_wmh == 0)
		    ++m;
		m += WINBAR_HEIGHT(curwin);
	    }
	}
    }
    অন্যথা যদি (topfrp->fr_layout == FR_ROW)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minheight(frp, পরবর্তী_curwin);
	    যদি (n > m)
		m = n;
	}
    }
    অন্যথা
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minheight(frp, পরবর্তী_curwin);
    }

    ফেরত m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
স্থির পূর্ণসংখ্যা
frame_wincount_in_height(frame_T *topfrp)
{
    frame_T	*frp;
    পূর্ণসংখ্যা		c = 0;
    পূর্ণসংখ্যা		n;

    যদি (topfrp->fr_win != ফাঁকা)
	c = 1;
    অন্যথা যদি (topfrp->fr_layout == FR_ROW)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_wincount_in_height(frp);
	    যদি (n > c)
		c = n;
	}
    }
    অন্যথা
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    c += frame_wincount_in_height(frp);
    }

    ফেরত c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    স্থির পূর্ণসংখ্যা
frame_minwidth(
    frame_T	*topfrp,
    win_T	*পরবর্তী_curwin)	// use p_wh and p_wiw for next_curwin
{
    frame_T	*frp;
    পূর্ণসংখ্যা		m, n;

    যদি (topfrp->fr_win != ফাঁকা)
    {
	যদি (topfrp->fr_win == পরবর্তী_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_width;
	অন্যথা
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_width;
	    // Current window is minimal one column wide
	    যদি (p_wmw == 0 && topfrp->fr_win == curwin && পরবর্তী_curwin == ফাঁকা)
		++m;
	}
    }
    অন্যথা যদি (topfrp->fr_layout == FR_COL)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minwidth(frp, পরবর্তী_curwin);
	    যদি (n > m)
		m = n;
	}
    }
    অন্যথা
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minwidth(frp, পরবর্তী_curwin);
    }

    ফেরত m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    শূন্য
close_others(
    পূর্ণসংখ্যা		message,
    পূর্ণসংখ্যা		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    পূর্ণসংখ্যা		r;

    যদি (one_window())
    {
	যদি (message && !autocmd_busy)
	    বার্তা(_(m_onlyone));
	ফেরত;
    }

    // Be very careful here: autocommands may change the window layout.
    জন্য (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_পরবর্তী;

	// autocommands messed this one up
	যদি (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_বাফার;
	}

	যদি (wp == curwin)		// don't close current window
	    চালু;

	// autoccommands messed this one up
	যদি (!buf_valid(wp->w_বাফার) && win_valid(wp))
	{
	    wp->w_বাফার = ফাঁকা;
	    win_close(wp, 0);
	    চালু;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_বাফার, forceit);
	যদি (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    চালু;
	}
	যদি (!r)
	{
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    যদি (message && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_changed(wp->w_বাফার, FALSE);
		যদি (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    চালু;
		}
	    }
	    যদি (bufIsChanged(wp->w_বাফার))
#endif
		চালু;
	}
	win_close(wp, !buf_hide(wp->w_বাফার) && !bufIsChanged(wp->w_বাফার));
    }

    যদি (message && !ONE_WINDOW)
	emsg(_(e_other_window_contains_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    শূন্য
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
স্থির পূর্ণসংখ্যা command_frame_height = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    শূন্য
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
    পূর্ণসংখ্যা
win_alloc_first(শূন্য)
{
    যদি (win_alloc_firstwin(ফাঁকা) == FAIL)
	ফেরত FAIL;

    first_tabpage = alloc_tabpage();
    যদি (first_tabpage == ফাঁকা)
	ফেরত FAIL;
    curtab = first_tabpage;
    unuse_tabpage(first_tabpage);

    ফেরত OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(শূন্য)
{
    win_T *wp;

    wp = win_alloc(ফাঁকা, TRUE);
    যদি (wp == ফাঁকা)
	ফেরত ফাঁকা;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_frame(wp);
    ফেরত wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    শূন্য
win_init_popup_win(win_T *wp, buf_T *buf)
{
    wp->w_বাফার = buf;
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
    স্থির পূর্ণসংখ্যা
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(ফাঁকা, FALSE);
    যদি (curwin == ফাঁকা)
	ফেরত FAIL;
    যদি (oldwin == ফাঁকা)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(ফাঁকা, ফাঁকা, 1L, BLN_LISTED);
	যদি (curwin == ফাঁকা || curbuf == ফাঁকা)
	    ফেরত FAIL;
	curwin->w_বাফার = curbuf;
#ifdef FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#endif
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    অন্যথা
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_frame(curwin);
    যদি (curwin->w_frame == ফাঁকা)
	ফেরত FAIL;
    topframe = curwin->w_frame;
    topframe->fr_width = COLUMNS_WITHOUT_TPL();
    topframe->fr_height = Rows - p_ch;

    ফেরত OK;
}

/*
 * Create a frame for window "wp".
 */
    স্থির শূন্য
new_frame(win_T *wp)
{
    frame_T *frp = ALLOC_CLEAR_ONE(frame_T);

    wp->w_frame = frp;
    যদি (frp == ফাঁকা)
	ফেরত;
    frp->fr_layout = FR_LEAF;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    শূন্য
win_init_আকার(শূন্য)
{
    firstwin->w_height = ROWS_AVAIL;
    firstwin->w_পূর্ব_height = ROWS_AVAIL;
    topframe->fr_height = ROWS_AVAIL;
    firstwin->w_width = topframe->fr_width;
}

/*
 * Allocate a new tabpage_T and init the values.
 * Returns NULL when out of memory.
 */
    স্থির tabpage_T *
alloc_tabpage(শূন্য)
{
    tabpage_T	*tp;
#ifdef FEAT_GUI
    পূর্ণসংখ্যা		i;
#endif


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    যদি (tp == ফাঁকা)
	ফেরত ফাঁকা;

#ifdef FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    যদি (tp->tp_vars == ফাঁকা)
    {
	vim_free(tp);
	ফেরত ফাঁকা;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#endif

#ifdef FEAT_GUI
    জন্য (i = 0; i < 3; i++)
	tp->tp_পূর্ব_which_scrollbars[i] = -1;
#endif
#ifdef FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#endif
    tp->tp_ch_used = p_ch;

    ফেরত tp;
}

    শূন্য
free_tabpage(tabpage_T *tp)
{
    পূর্ণসংখ্যা idx;

#ifdef FEAT_DIFF
    diff_clear(tp);
#endif
#ifdef FEAT_PROP_POPUP
    যখন (tp->tp_first_popupwin != ফাঁকা)
	popup_close_tabpage(tp, tp->tp_first_popupwin->w_id, TRUE);
#endif
    জন্য (idx = 0; idx < SNAP_গুনতি; ++idx)
	clear_snapshot(tp, idx);
#ifdef FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#endif

    যদি (tp == lastused_tabpage)
	lastused_tabpage = ফাঁকা;
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
    পূর্ণসংখ্যা
win_new_tabpage(পূর্ণসংখ্যা after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*পূর্ব_tp = curtab;
    tabpage_T	*newtp;
    পূর্ণসংখ্যা		n;
#যদি defined(FEAT_TABPANEL)
    পূর্ণসংখ্যা পূর্ব_columns = COLUMNS_WITHOUT_TPL();
#endif

    যদি (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	ফেরত FAIL;
    }
    যদি (window_layout_locked(CMD_tabnew))
	ফেরত FAIL;

    newtp = alloc_tabpage();
    যদি (newtp == ফাঁকা)
	ফেরত FAIL;

    // Remember the current windows in this Tab page.
    যদি (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	ফেরত FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == ফাঁকা)
				    ? ফাঁকা : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    যদি (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	যদি (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_পরবর্তী = first_tabpage;
	    first_tabpage = newtp;
	}
	অন্যথা
	{
	    যদি (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		জন্য (tp = first_tabpage; tp->tp_পরবর্তী != ফাঁকা
					       && n < after; tp = tp->tp_পরবর্তী)
		    ++n;
	    }
	    newtp->tp_পরবর্তী = tp->tp_পরবর্তী;
	    tp->tp_পরবর্তী = newtp;
	}
	newtp->tp_firstwin = newtp->tp_lastwin = newtp->tp_curwin = curwin;

	win_init_আকার();
	firstwin->w_winrow = tabline_height();
	firstwin->w_পূর্ব_winrow = firstwin->w_winrow;
	win_comp_scroll(curwin);

	newtp->tp_topframe = topframe;
	last_status(FALSE);

	lastused_tabpage = পূর্ব_tp;

#যদি defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#endif
#ifdef FEAT_JOB_CHANNEL
	entering_window(curwin);
#endif
#যদি defined(FEAT_TABPANEL)
	যদি (পূর্ব_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    পূর্ণসংখ্যা		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		যদি (tp2 != curtab)
		{
		    frame_new_width(tp2->tp_topframe, w, FALSE, TRUE);
		    যদি (!frame_check_width(tp2->tp_topframe, w))
			frame_new_width(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#endif
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, ফাঁকা, ফাঁকা, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, ফাঁকা, ফাঁকা, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, ফাঁকা, ফাঁকা, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, ফাঁকা, ফাঁকা, FALSE, curbuf);
	ফেরত OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    ফেরত FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    স্থির পূর্ণসংখ্যা
may_open_tabpage(শূন্য)
{
    পূর্ণসংখ্যা		n = (cmdmod.cmod_tab == 0)
				       ? postponed_split_tab : cmdmod.cmod_tab;

    যদি (n == 0)
	ফেরত FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_split_tab = 0;
    ফেরত win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    পূর্ণসংখ্যা
make_tabpages(পূর্ণসংখ্যা maxcount)
{
    পূর্ণসংখ্যা		গুনতি = maxcount;
    পূর্ণসংখ্যা		todo;

    // Limit to 'tabpagemax' tabs.
    যদি (গুনতি > p_tpm)
	গুনতি = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    জন্য (todo = গুনতি - 1; todo > 0; --todo)
	যদি (win_new_tabpage(0) == FAIL)
	    বিরতি;

    unblock_autocmds();

    // return actual number of tab pages
    ফেরত (গুনতি - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    পূর্ণসংখ্যা
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	যদি (tp == tpc)
	    ফেরত TRUE;
    ফেরত FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    পূর্ণসংখ্যা
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	যদি (tp == tpc)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		যদি (win_valid_any_tab(wp))
		    ফেরত TRUE;
	    }
	    ফেরত FALSE;
	}
    }
    // shouldn't happen
    ফেরত FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    শূন্য
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    যদি (tab == first_tabpage)
    {
	first_tabpage = tab->tp_পরবর্তী;
	ptp = first_tabpage;
    }
    অন্যথা
    {
	জন্য (ptp = first_tabpage; ptp != ফাঁকা && ptp->tp_পরবর্তী != tab;
							    ptp = ptp->tp_পরবর্তী)
	    ;
	প্রতিজ্ঞা(ptp != ফাঁকা);
	ptp->tp_পরবর্তী = tab->tp_পরবর্তী;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
find_tabpage(পূর্ণসংখ্যা n)
{
    tabpage_T	*tp;
    পূর্ণসংখ্যা		i = 1;

    যদি (n == 0)
	ফেরত curtab;

    জন্য (tp = first_tabpage; tp != ফাঁকা && i != n; tp = tp->tp_পরবর্তী)
	++i;
    ফেরত tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    পূর্ণসংখ্যা
tabpage_সূচক(tabpage_T *ftp)
{
    পূর্ণসংখ্যা		i = 1;
    tabpage_T	*tp;

    জন্য (tp = first_tabpage; tp != ফাঁকা && tp != ftp; tp = tp->tp_পরবর্তী)
	++i;
    ফেরত i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    স্থির পূর্ণসংখ্যা
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    পূর্ণসংখ্যা		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ifdef FEAT_JOB_CHANNEL
    leaving_window(curwin);
#endif
    reset_VIsual_and_resel();	// stop Visual mode
    যদি (trigger_leave_autocmds)
    {
	যদি (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ফাঁকা, ফাঁকা, FALSE, curbuf);
	    যদি (curtab != tp)
		ফেরত FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, ফাঁকা, ফাঁকা, FALSE, curbuf);
	যদি (curtab != tp)
	    ফেরত FAIL;
	apply_autocmds(EVENT_TABLEAVE, ফাঁকা, ফাঁকা, FALSE, curbuf);
	যদি (curtab != tp)
	    ফেরত FAIL;
    }

    reset_dragwin();
#যদি defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    যদি (gui.in_use)
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
    যদি (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_width;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = ফাঁকা;
    lastwin = ফাঁকা;
    ফেরত OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    স্থির শূন্য
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    পূর্ণসংখ্যা		trigger_enter_autocmds,
    পূর্ণসংখ্যা		trigger_leave_autocmds)
{
    পূর্ণসংখ্যা		old_off = tp->tp_firstwin->w_winrow;
    win_T	*পরবর্তী_prevwin = tp->tp_prevwin;
    tabpage_T	*last_tab = curtab;

    use_tabpage(tp);

    যদি (p_ch != curtab->tp_ch_used)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	পূর্ণসংখ্যা new_ch = curtab->tp_ch_used;
	curtab->tp_ch_used = p_ch;
	command_frame_height = FALSE;
	set_option_মান((char_u *)"cmdheight", new_ch, ফাঁকা, 0);
	command_frame_height = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (শূন্য)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
		  | (trigger_enter_autocmds ? WEE_TRIGGER_ENTER_AUTOCMDS : 0)
		  | (trigger_leave_autocmds ? WEE_TRIGGER_LEAVE_AUTOCMDS : 0));
    prevwin = পরবর্তী_prevwin;

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
    যদি (curtab->tp_old_Rows != ROWS_AVAIL || (old_off != firstwin->w_winrow
#ifdef FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#endif
		))
	shell_new_rows();
    যদি (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	যদি (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_width;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	অন্যথা
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = last_tab;

#যদি defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    যদি (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, ফাঁকা, ফাঁকা, FALSE, curbuf);
	যদি (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, ফাঁকা, ফাঁকা, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    শূন্য
goto_tabpage(পূর্ণসংখ্যা n)
{
    tabpage_T	*tp = ফাঁকা;  // shut up compiler
    tabpage_T	*ttp;
    পূর্ণসংখ্যা		i;

    যদি (text_locked())
    {
	// Not allowed when editing the command line.
	text_locked_বার্তা();
	ফেরত;
    }

    // If there is only one it can't work.
    যদি (first_tabpage->tp_পরবর্তী == ফাঁকা)
    {
	যদি (n > 1)
	    beep_flush();
	ফেরত;
    }

    যদি (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	যদি (curtab->tp_পরবর্তী == ফাঁকা)
	    tp = first_tabpage;
	অন্যথা
	    tp = curtab->tp_পরবর্তী;
    }
    অন্যথা যদি (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	জন্য (i = n; i < 0; ++i)
	{
	    জন্য (tp = first_tabpage; tp->tp_পরবর্তী != ttp && tp->tp_পরবর্তী != ফাঁকা;
		    tp = tp->tp_পরবর্তী)
		;
	    ttp = tp;
	}
    }
    অন্যথা যদি (n == 9999)
    {
	// Go to last tab page.
	জন্য (tp = first_tabpage; tp->tp_পরবর্তী != ফাঁকা; tp = tp->tp_পরবর্তী)
	    ;
    }
    অন্যথা
    {
	// Go to tab page "n".
	tp = find_tabpage(n);
	যদি (tp == ফাঁকা)
	{
	    beep_flush();
	    ফেরত;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ifdef FEAT_GUI_TABLINE
    যদি (gui_use_tabline())
	gui_mch_set_curtab(tabpage_সূচক(curtab));
#endif
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    শূন্য
goto_tabpage_tp(
    tabpage_T	*tp,
    পূর্ণসংখ্যা		trigger_enter_autocmds,
    পূর্ণসংখ্যা		trigger_leave_autocmds)
{
    যদি (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    set_keep_বার্তা(ফাঁকা, 0);

    skip_win_fix_scroll = TRUE;
    যদি (tp != curtab && leave_tabpage(tp->tp_curwin->w_বাফার,
					trigger_leave_autocmds) == OK)
    {
	যদি (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	অন্যথা
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    পূর্ণসংখ্যা
goto_tabpage_lastused(শূন্য)
{
    যদি (!valid_tabpage(lastused_tabpage))
	ফেরত FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    ফেরত OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    শূন্য
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    যদি (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ifdef FEAT_GUI_TABLINE
	যদি (gui_use_tabline())
	    gui_mch_set_curtab(tabpage_সূচক(curtab));
#endif
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    শূন্য
tabpage_move(পূর্ণসংখ্যা nr)
{
    পূর্ণসংখ্যা		n = 1;
    tabpage_T	*tp, *tp_dst;

    যদি (first_tabpage->tp_পরবর্তী == ফাঁকা)
	ফেরত;

    যদি (tabpage_move_disallowed)
	ফেরত;

    জন্য (tp = first_tabpage; tp->tp_পরবর্তী != ফাঁকা && n < nr; tp = tp->tp_পরবর্তী)
	++n;

    যদি (tp == curtab || (nr > 0 && tp->tp_পরবর্তী != ফাঁকা
						    && tp->tp_পরবর্তী == curtab))
	ফেরত;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    যদি (curtab == first_tabpage)
	first_tabpage = curtab->tp_পরবর্তী;
    অন্যথা
    {
	FOR_ALL_TABPAGES(tp)
	    যদি (tp->tp_পরবর্তী == curtab)
		বিরতি;
	যদি (tp == ফাঁকা)	// "cannot happen"
	    ফেরত;
	tp->tp_পরবর্তী = curtab->tp_পরবর্তী;
    }

    // Re-insert it at the specified position.
    যদি (nr <= 0)
    {
	curtab->tp_পরবর্তী = first_tabpage;
	first_tabpage = curtab;
    }
    অন্যথা
    {
	curtab->tp_পরবর্তী = tp_dst->tp_পরবর্তী;
	tp_dst->tp_পরবর্তী = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#যদি defined(FEAT_TABPANEL)
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
    শূন্য
win_goto(win_T *wp)
{
#ifdef FEAT_CONCEAL
    win_T	*owp = curwin;
#endif

#ifdef FEAT_PROP_POPUP
    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত;
    যদি (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_to_enter_popup_window));
	ফেরত;
    }
#endif
    যদি (text_or_buf_locked())
    {
	beep_flush();
	ফেরত;
    }

    যদি (wp->w_বাফার != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    অন্যথা যদি (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    যদি (!win_valid(wp))
	ফেরত;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    win_enter(wp, TRUE);

#ifdef FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    যদি (win_valid(owp) && owp->w_p_cole > 0 && !বার্তা_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    যদি (curwin->w_p_cole > 0 && !বার্তা_scrolled)
	need_cursor_সারি_redraw = TRUE;
#endif
}

#যদি defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_find_nr(পূর্ণসংখ্যা winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	যদি (--winnr == 0)
	    বিরতি;
    ফেরত wp;
}
#endif

#যদি defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_find_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    যদি (wp == win)
		ফেরত tp;
    ফেরত ফাঁকা;
}
#endif

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, পূর্ণসংখ্যা up, long গুনতি)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    যদি (popup_is_popup(wp))
	// popups don't have neighbors.
	ফেরত ফাঁকা;
#endif
    foundfr = wp->w_frame;
    যখন (গুনতি--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	জন্য (;;)
	{
	    যদি (fr == tp->tp_topframe)
		যাও end;
	    যদি (up)
		nfr = fr->fr_পূর্ব;
	    অন্যথা
		nfr = fr->fr_পরবর্তী;
	    যদি (fr->fr_parent->fr_layout == FR_COL && nfr != ফাঁকা)
		বিরতি;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	জন্য (;;)
	{
	    যদি (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		বিরতি;
	    }
	    fr = nfr->fr_child;
	    যদি (nfr->fr_layout == FR_ROW)
	    {
		// Find the frame at the cursor row.
		যখন (fr->fr_পরবর্তী != ফাঁকা
			&& frame2win(fr)->w_wincol + fr->fr_width
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_পরবর্তী;
	    }
	    যদি (nfr->fr_layout == FR_COL && up)
		যখন (fr->fr_পরবর্তী != ফাঁকা)
		    fr = fr->fr_পরবর্তী;
	    nfr = fr;
	}
    }
end:
    ফেরত foundfr != ফাঁকা ? foundfr->fr_win : ফাঁকা;
}

/*
 * Move to window above or below "count" times.
 */
    স্থির শূন্য
win_goto_ver(
    পূর্ণসংখ্যা		up,		// TRUE to go to win above
    long	গুনতি)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    যদি (ERROR_IF_TERM_POPUP_WINDOW)
	ফেরত;
#endif
    win = win_vert_neighbor(curtab, curwin, up, গুনতি);
    যদি (win != ফাঁকা)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, পূর্ণসংখ্যা বাম, long গুনতি)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    যদি (popup_is_popup(wp))
	// popups don't have neighbors.
	ফেরত ফাঁকা;
#endif
    foundfr = wp->w_frame;
    যখন (গুনতি--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	জন্য (;;)
	{
	    যদি (fr == tp->tp_topframe)
		যাও end;
	    যদি (বাম)
		nfr = fr->fr_পূর্ব;
	    অন্যথা
		nfr = fr->fr_পরবর্তী;
	    যদি (fr->fr_parent->fr_layout == FR_ROW && nfr != ফাঁকা)
		বিরতি;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	জন্য (;;)
	{
	    যদি (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		বিরতি;
	    }
	    fr = nfr->fr_child;
	    যদি (nfr->fr_layout == FR_COL)
	    {
		// Find the frame at the cursor row.
		যখন (fr->fr_পরবর্তী != ফাঁকা
			&& frame2win(fr)->w_winrow + fr->fr_height
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_পরবর্তী;
	    }
	    যদি (nfr->fr_layout == FR_ROW && বাম)
		যখন (fr->fr_পরবর্তী != ফাঁকা)
		    fr = fr->fr_পরবর্তী;
	    nfr = fr;
	}
    }
end:
    ফেরত foundfr != ফাঁকা ? foundfr->fr_win : ফাঁকা;
}

/*
 * Move to left or right window.
 */
    স্থির শূন্য
win_goto_hor(
    পূর্ণসংখ্যা		বাম,		// TRUE to go to left win
    long	গুনতি)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    যদি (ERROR_IF_TERM_POPUP_WINDOW)
	ফেরত;
#endif
    win = win_horz_neighbor(curtab, curwin, বাম, গুনতি);
    যদি (win != ফাঁকা)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    শূন্য
win_enter(win_T *wp, পূর্ণসংখ্যা undo_sync)
{
    (শূন্য)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    শূন্য
win_fix_current_dir(শূন্য)
{
    যদি (curwin->w_localdir != ফাঁকা || curtab->tp_localdir != ফাঁকা)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	যদি (globaldir == ফাঁকা)
	{
	    char_u	cwd[MAXPATHL];

	    যদি (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	যদি (curwin->w_localdir != ফাঁকা)
	    dirname = curwin->w_localdir;
	অন্যথা
	    dirname = curtab->tp_localdir;

	যদি (mch_chdir((অক্ষর *)dirname) == 0)
	{
	    last_chdir_reason = ফাঁকা;
	    shorten_fnames(TRUE);
	}
    }
    অন্যথা যদি (globaldir != ফাঁকা)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((অক্ষর *)globaldir);
	VIM_CLEAR(globaldir);
	last_chdir_reason = ফাঁকা;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    স্থির পূর্ণসংখ্যা
win_enter_ext(win_T *wp, পূর্ণসংখ্যা flags)
{
    পূর্ণসংখ্যা		other_বাফার = FALSE;
    পূর্ণসংখ্যা		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    পূর্ণসংখ্যা		did_decrement = FALSE;

    যদি (wp == curwin && curwin_invalid == 0)	// nothing to do
	ফেরত FALSE;

#ifdef FEAT_JOB_CHANNEL
    যদি (curwin_invalid == 0)
	leaving_window(curwin);
#endif

    যদি (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	যদি (wp->w_বাফার != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ফাঁকা, ফাঁকা, FALSE, curbuf);
	    other_বাফার = TRUE;
	    যদি (!win_valid(wp))
		ফেরত FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, ফাঁকা, ফাঁকা, FALSE, curbuf);
	যদি (!win_valid(wp))
	    ফেরত FALSE;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	যদি (aborting())
	    ফেরত FALSE;
#endif
    }

    // sync undo before leaving the current buffer
    যদি ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_বাফার)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    যদি (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    যদি (wp->w_বাফার != curbuf)
	buf_copy_options(wp->w_বাফার, BCO_ENTER | BCO_NOHELP);
    যদি (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_status = true;
    }
    curwin = wp;
    curbuf = wp->w_বাফার;
    check_cursor();
    যদি (!virtual_active())
	curwin->w_cursor.coladd = 0;
    যদি (*p_spk == 'c')		// assume cursor position needs updating
	changed_সারি_abv_curs();
    অন্যথা
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ifdef MESSAGE_QUEUE
    যদি (flags & WEE_ALLOW_PARSE_MESSAGES)
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
    যদি (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, ফাঁকা, ফাঁকা, FALSE, curbuf);
    যদি (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, ফাঁকা, ফাঁকা, FALSE, curbuf);
	যদি (other_বাফার)
	    apply_autocmds(EVENT_BUFENTER, ফাঁকা, ফাঁকা, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_status = true;
#ifdef FEAT_TERMINAL
    যদি (bt_terminal(curwin->w_বাফার))
	// terminal is likely in another mode
	redraw_mode = TRUE;
#endif
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#যদি defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    যদি (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    যদি (curwin->w_height < p_wh && !curwin->w_p_wfh
#ifdef FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#endif
	    )
	win_setheight((পূর্ণসংখ্যা)p_wh);
    অন্যথা যদি (curwin->w_height == 0)
	win_setheight(1);

    // set window width to desired minimal value
    যদি (curwin->w_width < p_wiw && !curwin->w_p_wfw)
	win_setwidth((পূর্ণসংখ্যা)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    ফেরত did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = ফাঁকা;

    যদি (curwin->w_বাফার == buf)
	wp = curwin;
    অন্যথা
	FOR_ALL_WINDOWS(wp)
	    যদি (wp->w_বাফার == buf)
		বিরতি;
    যদি (wp != ফাঁকা)
	win_enter(wp, FALSE);
    ফেরত wp;
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

    যদি (wp != ফাঁকা)
	ফেরত wp;

    FOR_ALL_TABPAGES(tp)
	যদি (tp != curtab)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		যদি (wp->w_বাফার == buf)
		    বিরতি;
	    যদি (wp != ফাঁকা)
	    {
		goto_tabpage_win(tp, wp);
		যদি (curwin != wp)
		    wp = ফাঁকা;	// something went wrong
		বিরতি;
	    }
	}
    ফেরত wp;
}

স্থির পূর্ণসংখ্যা last_win_id = LOWEST_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    স্থির win_T *
win_alloc(win_T *after, পূর্ণসংখ্যা hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    যদি (new_wp == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (win_alloc_সারিগুলি(new_wp) == FAIL)
    {
	vim_free(new_wp);
	ফেরত ফাঁকা;
    }

    new_wp->w_id = ++last_win_id;

#ifdef FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    যদি (new_wp->w_vars == ফাঁকা)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	ফেরত ফাঁকা;
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
    যদি (!hidden)
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
    new_wp->w_পূর্ব_fraction_row = -1;

#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_বাম],
		SBAR_বাম, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ডান],
		SBAR_ডান, new_wp);
    }
#endif
#ifdef FEAT_FOLDING
    foldInitWin(new_wp);
#endif
    unblock_autocmds();
#ifdef FEAT_অনুসন্ধান_EXTRA
    new_wp->w_পরবর্তী_match_id = 1000;  // up to 1000 can be picked by the user
#endif
    ফেরত new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    স্থির শূন্য
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    পূর্ণসংখ্যা		i;
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
    যদি (wp->w_stl_click != ফাঁকা)
    {
	জন্য (i = 0; i < wp->w_stl_click_গুনতি; i++)
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

	যদি (prevwin == wp)
	    prevwin = ফাঁকা;
	FOR_ALL_TABPAGES(ttp)
	    যদি (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = ফাঁকা;
    }
    win_free_lsize(wp);

    জন্য (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    যদি (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    যদি (wip2 != wip && wip2->wi_win == ফাঁকা)
		    {
			যদি (wip2->wi_পরবর্তী != ফাঁকা)
			    wip2->wi_পরবর্তী->wi_পূর্ব = wip2->wi_পূর্ব;
			যদি (wip2->wi_পূর্ব == ফাঁকা)
			    buf->b_wininfo = wip2->wi_পরবর্তী;
			অন্যথা
			    wip2->wi_পূর্ব->wi_পরবর্তী = wip2->wi_পরবর্তী;
			free_wininfo(wip2);
			বিরতি;
		    }

		wip->wi_win = ফাঁকা;
	    }

#ifdef FEAT_অনুসন্ধান_EXTRA
    clear_matches(wp);
#endif

    free_jumplist(wp);

#ifdef FEAT_QUICKFIX
    qf_free_all(wp);
#endif

#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_বাম]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ডান]);
    }
#endif // FEAT_GUI

#ifdef FEAT_MENU
    remove_winbar(wp);
#endif
#ifdef FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    জন্য (i = 0; i < 4; ++i)
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

    যদি (win_valid_any_tab(wp))
	win_remove(wp, tp);
    যদি (autocmd_busy)
    {
	wp->w_পরবর্তী = au_pending_free_win;
	au_pending_free_win = wp;
    }
    অন্যথা
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    পূর্ণসংখ্যা
win_unlisted(win_T *wp)
{
    ফেরত is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#যদি defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    শূন্য
win_free_popup(win_T *win)
{
    যদি (win->w_বাফার != ফাঁকা)
    {
	যদি (bt_popup(win->w_বাফার))
	    win_close_বাফার(win, DOBUF_WIPE_REUSE, FALSE);
	অন্যথা
	    close_বাফার(win, win->w_বাফার, 0, FALSE, FALSE, TRUE);

	যদি (win->w_বাফার != ফাঁকা)
	    --win->w_বাফার->b_nwindows;
    }
# যদি defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    যদি (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_frame);
    win_free(win, ফাঁকা);
}
#endif

/*
 * Append window "wp" in the window list after window "after".
 */
    স্থির শূন্য
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    যদি (after == ফাঁকা)	    // after NULL is in front of the first
	before = firstwin;
    অন্যথা
	before = after->w_পরবর্তী;

    wp->w_পরবর্তী = before;
    wp->w_পূর্ব = after;
    যদি (after == ফাঁকা)
	firstwin = wp;
    অন্যথা
	after->w_পরবর্তী = wp;
    যদি (before == ফাঁকা)
	lastwin = wp;
    অন্যথা
	before->w_পূর্ব = wp;
}

/*
 * Remove a window from the window list.
 */
    শূন্য
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    যদি (wp->w_পূর্ব != ফাঁকা)
	wp->w_পূর্ব->w_পরবর্তী = wp->w_পরবর্তী;
    অন্যথা যদি (tp == ফাঁকা)
	firstwin = curtab->tp_firstwin = wp->w_পরবর্তী;
    অন্যথা
	tp->tp_firstwin = wp->w_পরবর্তী;

    যদি (wp->w_পরবর্তী != ফাঁকা)
	wp->w_পরবর্তী->w_পূর্ব = wp->w_পূর্ব;
    অন্যথা যদি (tp == ফাঁকা)
	lastwin = curtab->tp_lastwin = wp->w_পূর্ব;
    অন্যথা
	tp->tp_lastwin = wp->w_পূর্ব;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    স্থির শূন্য
frame_append(frame_T *after, frame_T *frp)
{
    frp->fr_পরবর্তী = after->fr_পরবর্তী;
    after->fr_পরবর্তী = frp;
    যদি (frp->fr_পরবর্তী != ফাঁকা)
	frp->fr_পরবর্তী->fr_পূর্ব = frp;
    frp->fr_পূর্ব = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    স্থির শূন্য
frame_ঢোকাও(frame_T *before, frame_T *frp)
{
    frp->fr_পরবর্তী = before;
    frp->fr_পূর্ব = before->fr_পূর্ব;
    before->fr_পূর্ব = frp;
    যদি (frp->fr_পূর্ব != ফাঁকা)
	frp->fr_পূর্ব->fr_পরবর্তী = frp;
    অন্যথা
	frp->fr_parent->fr_child = frp;
}

/*
 * Remove a frame from a frame list.
 */
    স্থির শূন্য
frame_remove(frame_T *frp)
{
    যদি (frp->fr_পূর্ব != ফাঁকা)
	frp->fr_পূর্ব->fr_পরবর্তী = frp->fr_পরবর্তী;
    অন্যথা
	frp->fr_parent->fr_child = frp->fr_পরবর্তী;
    যদি (frp->fr_পরবর্তী != ফাঁকা)
	frp->fr_পরবর্তী->fr_পূর্ব = frp->fr_পূর্ব;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    পূর্ণসংখ্যা
win_alloc_সারিগুলি(win_T *wp)
{
    wp->w_সারিগুলি_valid = 0;
    wp->w_সারিগুলি = ALLOC_CLEAR_MULT(wline_T, Rows);
    যদি (wp->w_সারিগুলি == ফাঁকা)
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * free lsize arrays for a window
 */
    শূন্য
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    যদি (wp != ফাঁকা)
	VIM_CLEAR(wp->w_সারিগুলি);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    শূন্য
shell_new_rows(শূন্য)
{
    পূর্ণসংখ্যা		h = (পূর্ণসংখ্যা)ROWS_AVAIL;

    যদি (firstwin == ফাঁকা)	// not initialized yet
	ফেরত;
    যদি (h < frame_minheight(topframe, ফাঁকা))
	h = frame_minheight(topframe, ফাঁকা);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    frame_new_height(topframe, h, FALSE, TRUE, FALSE);
    যদি (!frame_check_height(topframe, h))
	frame_new_height(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_used = p_ch;

    যদি (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#যদি defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#যদি 0
    // Disabled: don't want making the screen smaller make a window larger.
    যদি (p_ea)
	win_equal(curwin, FALSE, 'v');
#endif
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    শূন্য
shell_new_columns(শূন্য)
{
    যদি (firstwin == ফাঁকা)	// not initialized yet
	ফেরত;

#যদি defined(FEAT_TABPANEL)
    স্থির পূর্ণসংখ্যা পূর্ব_tp_width;
    স্থির পূর্ণসংখ্যা পূর্ব_wincol;
    স্থির পূর্ণসংখ্যা পূর্ব_fr_width;
#endif
    পূর্ণসংখ্যা w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    frame_new_width(topframe, w, FALSE, TRUE);
    যদি (!frame_check_width(topframe, w))
	frame_new_width(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#যদি defined(FEAT_TABPANEL)
    পূর্ণসংখ্যা tp_width = tabpanel_width();
    পূর্ণসংখ্যা tp_width_changed = tp_width != পূর্ব_tp_width;
    // tabpanel width changed
    যদি (tp_width_changed && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    যদি (tp_width_changed
	    || (tp_width > 0 && (firstwin->w_wincol != পূর্ব_wincol
		    || topframe->fr_width != পূর্ব_fr_width)))
    {
	screen_fill(cmdline_row, (পূর্ণসংখ্যা)Rows, 0, (পূর্ণসংখ্যা)Columns, ' ', ' ', 0);
    }
    পূর্ব_tp_width = tabpanel_width();
    পূর্ব_wincol = firstwin->w_wincol;
    পূর্ব_fr_width = topframe->fr_width;
#endif
    // Adjust offset for command line start column
    cmdline_col_off = firstwin->w_wincol;
    cmdline_width = topframe->fr_width;
    comp_col();
    যদি (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#যদি defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#যদি 0
    // Disabled: don't want making the screen smaller make a window larger.
    যদি (p_ea)
	win_equal(curwin, FALSE, 'h');
#endif
}

/*
 * Save the size of all windows in "gap".
 */
    শূন্য
win_আকার_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, আকার(পূর্ণসংখ্যা), 1);
    যদি (ga_grow(gap, win_গুনতি() * 2 + 1) == FAIL)
	ফেরত;

    // first entry is the total lines available for windows
    ((পূর্ণসংখ্যা *)gap->ga_তথ্য)[gap->ga_len++] = ROWS_AVAIL - last_stl_height(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((পূর্ণসংখ্যা *)gap->ga_তথ্য)[gap->ga_len++] =
	    wp->w_width + wp->w_vsep_width;
	((পূর্ণসংখ্যা *)gap->ga_তথ্য)[gap->ga_len++] = wp->w_height;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    শূন্য
win_আকার_restore(garray_T *gap)
{
    win_T	*wp;
    পূর্ণসংখ্যা		i, j;

    যদি (win_গুনতি() * 2 + 1 == gap->ga_len
	    && ((পূর্ণসংখ্যা *)gap->ga_তথ্য)[0] == ROWS_AVAIL - last_stl_height(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	জন্য (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		frame_setwidth(wp->w_frame, ((পূর্ণসংখ্যা *)gap->ga_তথ্য)[i++]);
		win_setheight_win(((পূর্ণসংখ্যা *)gap->ga_তথ্য)[i++], wp);
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
    শূন্য
win_comp_pos(শূন্য)
{
    পূর্ণসংখ্যা		row = tabline_height();
    পূর্ণসংখ্যা		col = TPL_LCOL();

    frame_comp_pos(topframe, &row, &col);
#যদি defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    স্থির শূন্য
frame_comp_pos(frame_T *topfrp, পূর্ণসংখ্যা *row, পূর্ণসংখ্যা *col)
{
    win_T	*wp;
    frame_T	*frp;
    পূর্ণসংখ্যা		startcol;
    পূর্ণসংখ্যা		startrow;
    পূর্ণসংখ্যা		h;

    wp = topfrp->fr_win;
    যদি (wp != ফাঁকা)
    {
	যদি (wp->w_winrow != *row || wp->w_wincol != *col)
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
    অন্যথা
    {
	startrow = *row;
	startcol = *col;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    যদি (topfrp->fr_layout == FR_ROW)
		*row = startrow;	// all frames are at the same row
	    অন্যথা
		*col = startcol;	// all frames are at the same col
	    frame_comp_pos(frp, row, col);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    শূন্য
win_ensure_আকার(শূন্য)
{
    যদি (curwin->w_height == 0)
	win_setheight(1);
    যদি (curwin->w_width == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    শূন্য
win_setheight(পূর্ণসংখ্যা height)
{
    win_setheight_win(height, curwin);
#যদি defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    শূন্য
win_setheight_win(পূর্ণসংখ্যা height, win_T *win)
{
    যদি (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	যদি (height < p_wmh)
	    height = p_wmh;
	যদি (height == 0)
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
    স্থির শূন্য
frame_setheight(frame_T *curfrp, পূর্ণসংখ্যা height)
{
    পূর্ণসংখ্যা		room;		// total number of lines available
    পূর্ণসংখ্যা		take;		// number of lines taken from other windows
    পূর্ণসংখ্যা		room_cmdline;	// lines available from cmdline
    পূর্ণসংখ্যা		run;
    frame_T	*frp;
    পূর্ণসংখ্যা		h;
    পূর্ণসংখ্যা		room_reserved;

    // If the height already is the desired value, nothing to do.
    যদি (curfrp->fr_height == height)
	ফেরত;

    যদি (curfrp->fr_parent == ফাঁকা)
    {
	// topframe: can only change the command line height
	যদি (height > 0)
	    frame_new_height(curfrp, height, FALSE, FALSE, TRUE);
    }
    অন্যথা যদি (curfrp->fr_parent->fr_layout == FR_ROW)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = frame_minheight(curfrp->fr_parent, ফাঁকা);
	যদি (height < h)
	    height = h;
	frame_setheight(curfrp->fr_parent, height);
    }
    অন্যথা
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
	জন্য (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		যদি (frp != curfrp
			&& frp->fr_win != ফাঁকা
			&& frp->fr_win->w_p_wfh)
		    room_reserved += frp->fr_height;
		room += frp->fr_height;
		যদি (frp != curfrp)
		    room -= frame_minheight(frp, ফাঁকা);
	    }
	    যদি (curfrp->fr_width != topframe->fr_width)
		room_cmdline = 0;
	    অন্যথা
	    {
		room_cmdline = Rows - p_ch - (lastwin->w_winrow
						+ VISIBLE_HEIGHT(lastwin)
						+ lastwin->w_status_height);
		যদি (room_cmdline < 0)
		    room_cmdline = 0;
	    }

	    যদি (height <= room + room_cmdline)
		বিরতি;
	    যদি (run == 2 || curfrp->fr_width == topframe->fr_width)
	    {
		height = room + room_cmdline;
		বিরতি;
	    }
	    frame_setheight(curfrp->fr_parent, height
		+ frame_minheight(curfrp->fr_parent, NOWIN) - (পূর্ণসংখ্যা)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = height - curfrp->fr_height;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	যদি (height > room + room_cmdline - room_reserved)
	    room_reserved = room + room_cmdline - height;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	যদি (take < 0 && room - curfrp->fr_height <= room_reserved)
	    room_reserved = 0;

	যদি (take > 0 && room_cmdline > 0)
	{
	    // use lines from cmdline first
	    যদি (take < room_cmdline)
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
	জন্য (run = 0; run < 2; ++run)
	{
	    যদি (run == 0)
		frp = curfrp->fr_পরবর্তী;	// 1st run: start with next window
	    অন্যথা
		frp = curfrp->fr_পূর্ব;	// 2nd run: start with prev window
	    যখন (frp != ফাঁকা && take != 0)
	    {
		h = frame_minheight(frp, ফাঁকা);
		যদি (room_reserved > 0
			&& frp->fr_win != ফাঁকা
			&& frp->fr_win->w_p_wfh)
		{
		    যদি (room_reserved >= frp->fr_height)
			room_reserved -= frp->fr_height;
		    অন্যথা
		    {
			যদি (frp->fr_height - room_reserved > take)
			    room_reserved = frp->fr_height - take;
			take -= frp->fr_height - room_reserved;
			frame_new_height(frp, room_reserved, FALSE, FALSE,
									TRUE);
			room_reserved = 0;
		    }
		}
		অন্যথা
		{
		    যদি (frp->fr_height - take < h)
		    {
			take -= frp->fr_height - h;
			frame_new_height(frp, h, FALSE, FALSE, TRUE);
		    }
		    অন্যথা
		    {
			frame_new_height(frp, frp->fr_height - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		যদি (run == 0)
		    frp = frp->fr_পরবর্তী;
		অন্যথা
		    frp = frp->fr_পূর্ব;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    শূন্য
win_setwidth(পূর্ণসংখ্যা width)
{
    win_setwidth_win(width, curwin);
}

    শূন্য
win_setwidth_win(পূর্ণসংখ্যা width, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    যদি (wp == curwin)
    {
	যদি (width < p_wmw)
	    width = p_wmw;
	যদি (width == 0)
	    width = 1;
    }
    অন্যথা যদি (width < 0)
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
    স্থির শূন্য
frame_setwidth(frame_T *curfrp, পূর্ণসংখ্যা width)
{
    পূর্ণসংখ্যা		room;		// total number of lines available
    পূর্ণসংখ্যা		take;		// number of lines taken from other windows
    পূর্ণসংখ্যা		run;
    frame_T	*frp;
    পূর্ণসংখ্যা		w;
    পূর্ণসংখ্যা		room_reserved;

    // If the width already is the desired value, nothing to do.
    যদি (curfrp->fr_width == width)
	ফেরত;

    যদি (curfrp->fr_parent == ফাঁকা)
	// topframe: can't change width
	ফেরত;

    যদি (curfrp->fr_parent->fr_layout == FR_COL)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = frame_minwidth(curfrp->fr_parent, ফাঁকা);
	যদি (width < w)
	    width = w;
	frame_setwidth(curfrp->fr_parent, width);
    }
    অন্যথা
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	জন্য (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		যদি (frp != curfrp
			&& frp->fr_win != ফাঁকা
			&& frp->fr_win->w_p_wfw)
		    room_reserved += frp->fr_width;
		room += frp->fr_width;
		যদি (frp != curfrp)
		    room -= frame_minwidth(frp, ফাঁকা);
	    }

	    যদি (width <= room)
		বিরতি;
	    যদি (run == 2 || curfrp->fr_height >= ROWS_AVAIL)
	    {
		width = room;
		বিরতি;
	    }
	    frame_setwidth(curfrp->fr_parent, width
		 + frame_minwidth(curfrp->fr_parent, NOWIN) - (পূর্ণসংখ্যা)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = width - curfrp->fr_width;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	যদি (width > room - room_reserved)
	    room_reserved = room - width;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	যদি (take < 0 && room - curfrp->fr_width < room_reserved)
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
	জন্য (run = 0; run < 2; ++run)
	{
	    যদি (run == 0)
		frp = curfrp->fr_পরবর্তী;	// 1st run: start with next window
	    অন্যথা
		frp = curfrp->fr_পূর্ব;	// 2nd run: start with prev window
	    যখন (frp != ফাঁকা && take != 0)
	    {
		w = frame_minwidth(frp, ফাঁকা);
		যদি (room_reserved > 0
			&& frp->fr_win != ফাঁকা
			&& frp->fr_win->w_p_wfw)
		{
		    যদি (room_reserved >= frp->fr_width)
			room_reserved -= frp->fr_width;
		    অন্যথা
		    {
			যদি (frp->fr_width - room_reserved > take)
			    room_reserved = frp->fr_width - take;
			take -= frp->fr_width - room_reserved;
			frame_new_width(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		অন্যথা
		{
		    যদি (frp->fr_width - take < w)
		    {
			take -= frp->fr_width - w;
			frame_new_width(frp, w, FALSE, FALSE);
		    }
		    অন্যথা
		    {
			frame_new_width(frp, frp->fr_width - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		যদি (run == 0)
		    frp = frp->fr_পরবর্তী;
		অন্যথা
		    frp = frp->fr_পূর্ব;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    শূন্য
win_setminheight(শূন্য)
{
    পূর্ণসংখ্যা		room;
    পূর্ণসংখ্যা		needed;
    পূর্ণসংখ্যা		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    যখন (p_wmh > 0)
    {
	room = Rows - p_ch;
	needed = min_rows_for_all_tabpages() - 1;  // 1 was added for the cmdline
	যদি (room >= needed)
	    বিরতি;
	--p_wmh;
	যদি (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    শূন্য
win_setminwidth(শূন্য)
{
    পূর্ণসংখ্যা		room;
    পূর্ণসংখ্যা		needed;
    পূর্ণসংখ্যা		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    যখন (p_wmw > 0)
    {
	room = topframe->fr_width;
	needed = frame_minwidth(topframe, ফাঁকা);
	যদি (room >= needed)
	    বিরতি;
	--p_wmw;
	যদি (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    শূন্য
win_drag_status_সারি(win_T *dragwin, পূর্ণসংখ্যা offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    পূর্ণসংখ্যা		room;
    পূর্ণসংখ্যা		up;	// if TRUE, drag status line up, otherwise down
    পূর্ণসংখ্যা		n;

    fr = dragwin->w_frame;
    curfr = fr;
    যদি (fr != topframe)		// more than one window
    {
	fr = fr->fr_parent;
	// When the parent frame is not a column of frames, its parent should
	// be.
	যদি (fr->fr_layout != FR_COL)
	{
	    curfr = fr;
	    যদি (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_parent;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    যখন (curfr != topframe && curfr->fr_পরবর্তী == ফাঁকা)
    {
	যদি (fr != topframe)
	    fr = fr->fr_parent;
	curfr = fr;
	যদি (fr != topframe)
	    fr = fr->fr_parent;
    }

    যদি (offset < 0) // drag up
    {
	up = TRUE;
	offset = -offset;
	// sum up the room of the current frame and above it
	যদি (fr == curfr)
	{
	    // only one window
	    room = fr->fr_height - frame_minheight(fr, ফাঁকা);
	}
	অন্যথা
	{
	    room = 0;
	    জন্য (fr = fr->fr_child; ; fr = fr->fr_পরবর্তী)
	    {
		room += fr->fr_height - frame_minheight(fr, ফাঁকা);
		যদি (fr == curfr)
		    বিরতি;
	    }
	}
	fr = curfr->fr_পরবর্তী;		// put fr at frame that grows
    }
    অন্যথা    // drag down
    {
	up = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	room = Rows - cmdline_row;
	যদি (curfr->fr_পরবর্তী == ফাঁকা)
	    --room;
	অন্যথা
	    room -= p_ch;
	যদি (room < 0)
	    room = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_FRAMES(fr, curfr->fr_পরবর্তী)
	    room += fr->fr_height - frame_minheight(fr, ফাঁকা);
	fr = curfr;			// put fr at window that grows
    }

    যদি (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    যদি (offset <= 0)
	ফেরত;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    যদি (fr != ফাঁকা)
	frame_new_height(fr, fr->fr_height + offset, up, FALSE, TRUE);

    যদি (up)
	fr = curfr;		// current frame gets smaller
    অন্যথা
	fr = curfr->fr_পরবর্তী;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    যখন (fr != ফাঁকা && offset > 0)
    {
	n = frame_minheight(fr, ফাঁকা);
	যদি (fr->fr_height - offset <= n)
	{
	    offset -= fr->fr_height - n;
	    frame_new_height(fr, n, !up, FALSE, TRUE);
	}
	অন্যথা
	{
	    frame_new_height(fr, fr->fr_height - offset, !up, FALSE, TRUE);
	    বিরতি;
	}
	যদি (up)
	    fr = fr->fr_পূর্ব;
	অন্যথা
	    fr = fr->fr_পরবর্তী;
    }
    win_comp_pos();
    win_fix_scroll(TRUE);

    redraw_all_later(UPD_SOME_VALID);
    showmode();
}

/*
 * Separator line of dragwin is dragged "offset" lines right (negative is left).
 */
    শূন্য
win_drag_vsep_সারি(win_T *dragwin, পূর্ণসংখ্যা offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    পূর্ণসংখ্যা		room;
    পূর্ণসংখ্যা		বাম;	// if TRUE, drag separator line left, otherwise right
    পূর্ণসংখ্যা		n;

    fr = dragwin->w_frame;
    যদি (fr == topframe)		// only one window (cannot happen?)
	ফেরত;
    curfr = fr;
    fr = fr->fr_parent;
    // When the parent frame is not a row of frames, its parent should be.
    যদি (fr->fr_layout != FR_ROW)
    {
	যদি (fr == topframe)	// only a column of windows (cannot happen?)
	    ফেরত;
	curfr = fr;
	fr = fr->fr_parent;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    যখন (curfr->fr_পরবর্তী == ফাঁকা)
    {
	যদি (fr == topframe)
	    বিরতি;
	curfr = fr;
	fr = fr->fr_parent;
	যদি (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_parent;
	}
    }

    যদি (offset < 0) // drag left
    {
	বাম = TRUE;
	offset = -offset;
	// sum up the room of the current frame and left of it
	room = 0;
	জন্য (fr = fr->fr_child; ; fr = fr->fr_পরবর্তী)
	{
	    room += fr->fr_width - frame_minwidth(fr, ফাঁকা);
	    যদি (fr == curfr)
		বিরতি;
	}
	fr = curfr->fr_পরবর্তী;		// put fr at frame that grows
    }
    অন্যথা    // drag right
    {
	বাম = FALSE;
	// sum up the room of frames right of the current one
	room = 0;
	FOR_ALL_FRAMES(fr, curfr->fr_পরবর্তী)
	    room += fr->fr_width - frame_minwidth(fr, ফাঁকা);
	fr = curfr;			// put fr at window that grows
    }

    যদি (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    যদি (offset <= 0)		// No room at all, quit.
	ফেরত;
    যদি (fr == ফাঁকা)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	ফেরত;

    // grow frame fr by offset lines
    frame_new_width(fr, fr->fr_width + offset, বাম, FALSE);

    // shrink other frames: current and at the left or at the right
    যদি (বাম)
	fr = curfr;		// current frame gets smaller
    অন্যথা
	fr = curfr->fr_পরবর্তী;	// next frame gets smaller

    যখন (fr != ফাঁকা && offset > 0)
    {
	n = frame_minwidth(fr, ফাঁকা);
	যদি (fr->fr_width - offset <= n)
	{
	    offset -= fr->fr_width - n;
	    frame_new_width(fr, n, !বাম, FALSE);
	}
	অন্যথা
	{
	    frame_new_width(fr, fr->fr_width - offset, !বাম, FALSE);
	    বিরতি;
	}
	যদি (বাম)
	    fr = fr->fr_পূর্ব;
	অন্যথা
	    fr = fr->fr_পরবর্তী;
    }
    win_comp_pos();
    redraw_all_later(UPD_NOT_VALID);
}

#define FRACTION_MULT	16384L

/*
 * Set wp->w_fraction for the current w_wrow and w_height.
 * Has no effect when the window is less than two lines.
 */
    শূন্য
set_fraction(win_T *wp)
{
    যদি (wp->w_height > 1)
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
    স্থির শূন্য
win_fix_scroll(পূর্ণসংখ্যা resize)
{
    যদি (*p_spk == 'c')
	ফেরত;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	যদি (wp->w_height != wp->w_পূর্ব_height)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    যদি (*p_spk == 's' && wp->w_winrow != wp->w_পূর্ব_winrow
		      && wp->w_botline - 1 <= wp->w_বাফার->b_ml.ml_সারি_গুনতি)
	    {
		পূর্ণসংখ্যা diff = (wp->w_winrow - wp->w_পূর্ব_winrow)
					  + (wp->w_height - wp->w_পূর্ব_height);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		যদি (diff > 0)
		    cursor_down_inner(wp, diff);
		অন্যথা
		    cursor_up_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_to_fraction(wp, wp->w_পূর্ব_height);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		যদি (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    অন্যথা যদি (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_পূর্ব_height = wp->w_height;
	wp->w_পূর্ব_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    যদি (!(get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL)))
	win_fix_cursor(FALSE);
    অন্যথা যদি (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    স্থির শূন্য
win_fix_cursor(পূর্ণসংখ্যা normal)
{
    win_T	*wp = curwin;

    যদি (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_বাফার->b_ml.ml_সারি_গুনতি < wp->w_height)
	ফেরত;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    long so = MIN(wp->w_height / 2, get_scrolloff_মান());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_down_inner(wp, so);
    linenr_T শীর্ষ_2 = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_up_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    যদি (lnum > bot && (wp->w_botline - wp->w_বাফার->b_ml.ml_সারি_গুনতি) != 1)
	nlnum = bot;
    অন্যথা যদি (lnum < শীর্ষ_2 && wp->w_topline != 1)
	nlnum = (so == wp->w_height / 2) ? bot : শীর্ষ_2;

    যদি (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	যদি (normal)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	অন্যথা  // Scroll instead when not in normal mode.
	{
	    wp->w_fraction = (nlnum == bot) ? FRACTION_MULT : 0;
	    scroll_to_fraction(wp, wp->w_পূর্ব_height);
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
    শূন্য
win_new_height(win_T *wp, পূর্ণসংখ্যা height)
{
    পূর্ণসংখ্যা		পূর্ব_height = wp->w_height;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    যদি (height < 0)
	height = 0;
    যদি (wp->w_height == height)
	ফেরত;	    // nothing to do

    যদি (wp->w_height > 0)
    {
	যদি (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	যদি (wp->w_height != পূর্ব_height)
	    ফেরত;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	যদি (wp->w_wrow != wp->w_পূর্ব_fraction_row)
	    set_fraction(wp);
    }

    wp->w_height = height;
    wp->w_redr_status = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    যদি (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	যদি (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_to_fraction(wp, পূর্ব_height);
    }
}

    শূন্য
scroll_to_fraction(win_T *wp, পূর্ণসংখ্যা পূর্ব_height)
{
    linenr_T	lnum;
    পূর্ণসংখ্যা		sline, সারি_আকার;
    পূর্ণসংখ্যা		height = wp->w_height;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    যদি (height > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (height < wp->w_বাফার->b_ml.ml_সারি_গুনতি || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	যদি (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((long)wp->w_fraction * (long)height - 1L)
							       / FRACTION_MULT;
	সারি_আকার = plines_win_col(wp, lnum, (long)(wp->w_cursor.col)) - 1;
	sline = wp->w_wrow - সারি_আকার;

	যদি (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    পূর্ণসংখ্যা rows = plines_win(wp, lnum, FALSE);

	    যদি (sline > wp->w_height - rows)
	    {
		sline = wp->w_height - rows;
		wp->w_wrow -= rows - সারি_আকার;
	    }
	}

	যদি (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    পূর্ণসংখ্যা		want_row = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = সারি_আকার;
	    যদি (wp->w_wrow >= wp->w_height
				       && (wp->w_width - win_col_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_width - win_col_off(wp);

		--wp->w_wrow;
		যখন (wp->w_wrow > want_row)
		{
		    skipcol += wp->w_width - win_col_off(wp) + win_col_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	অন্যথা যদি (sline > 0)
	{
	    যখন (sline > 0 && lnum > 1)
	    {
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, ফাঁকা, TRUE, ফাঁকা);
		যদি (lnum == 1)
		{
		    // first line in buffer is folded
		    সারি_আকার = 1;
		    --sline;
		    বিরতি;
		}
#endif
		--lnum;
#ifdef FEAT_DIFF
		যদি (lnum == wp->w_topline)
		    সারি_আকার = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		অন্যথা
#endif
		    সারি_আকার = plines_win(wp, lnum, TRUE);
		sline -= সারি_আকার;
	    }

	    যদি (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, ফাঁকা, &lnum, TRUE, ফাঁকা);
#endif
		lnum++;
		wp->w_wrow -= সারি_আকার + sline;
	    }
	    অন্যথা যদি (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	set_topline(wp, lnum);
    }

    যদি (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    যদি (পূর্ব_height > 0)
	wp->w_পূর্ব_fraction_row = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    শূন্য
win_new_width(win_T *wp, পূর্ণসংখ্যা width)
{
    // Should we give an error if width < 0?
    wp->w_width = width < 0 ? 0 : width;
    wp->w_সারিগুলি_valid = 0;
    changed_সারি_abv_curs_win(wp);
    invalidate_botline_win(wp);

    যদি (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_status = true;
}

    শূন্য
win_comp_scroll(win_T *wp)
{
#যদি defined(FEAT_EVAL)
    পূর্ণসংখ্যা old_w_p_scr = wp->w_p_scr;
#endif

    wp->w_p_scr = ((unsigned)wp->w_height >> 1);
    যদি (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#যদি defined(FEAT_EVAL)
    যদি (wp->w_p_scr != old_w_p_scr)
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
    শূন্য
command_height(শূন্য)
{
    পূর্ণসংখ্যা		old_p_ch = curtab->tp_ch_used;

    // Find bottom frame with width of screen.
    frame_T *frp = lastwin->w_frame;
    যখন (frp->fr_width != topframe->fr_width && frp->fr_parent != ফাঁকা)
	frp = frp->fr_parent;

    // Avoid changing the height of a window with 'winfixheight' set.
    যখন (frp->fr_পূর্ব != ফাঁকা && frp->fr_layout == FR_LEAF
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_পূর্ব;

    যখন (p_ch > old_p_ch && command_frame_height)
    {
	যদি (frp == ফাঁকা)
	{
	    emsg(_(e_not_enough_room));
	    p_ch = old_p_ch;
	    বিরতি;
	}
	পূর্ণসংখ্যা h = MIN(p_ch - old_p_ch,
			frp->fr_height - frame_minheight(frp, ফাঁকা));
	frame_add_height(frp, -h);
	old_p_ch += h;
	frp = frp->fr_পূর্ব;
    }
    যদি (p_ch < old_p_ch && command_frame_height && frp != ফাঁকা)
	frame_add_height(frp, (পূর্ণসংখ্যা)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_row = Rows - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    যদি (বার্তা_scrolled == 0 && full_screen)
    {
	screen_fill(cmdline_row, (পূর্ণসংখ্যা)Rows, 0, (পূর্ণসংখ্যা)Columns, ' ', ' ', 0);
	বার্তা_row = cmdline_row;
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
    স্থির শূন্য
frame_add_height(frame_T *frp, পূর্ণসংখ্যা n)
{
    frame_new_height(frp, frp->fr_height + n, FALSE, FALSE, FALSE);
    জন্য (;;)
    {
	frp = frp->fr_parent;
	যদি (frp == ফাঁকা)
	    বিরতি;
	frp->fr_height += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    শূন্য
last_status(
    পূর্ণসংখ্যা		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    last_status_rec(topframe, last_stl_height(morewin) > 0);
#যদি defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

    স্থির শূন্য
last_status_rec(frame_T *fr, পূর্ণসংখ্যা statusline)
{
    frame_T	*fp;
    win_T	*wp;

    যদি (fr->fr_layout == FR_LEAF)
    {
	wp = fr->fr_win;
	যদি (wp->w_status_height != 0 && !statusline)
	{
	    // remove status line
	    win_new_height(wp, wp->w_height + wp->w_status_height);
	    wp->w_status_height = 0;
	    comp_col();
	}
	অন্যথা যদি (wp->w_status_height == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    যখন (fp->fr_height <= frame_minheight(fp, ফাঁকা))
	    {
		যদি (fp == topframe)
		{
		    emsg(_(e_not_enough_room));
		    ফেরত;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		যদি (fp->fr_parent->fr_layout == FR_COL && fp->fr_পূর্ব != ফাঁকা)
		    fp = fp->fr_পূর্ব;
		অন্যথা
		    fp = fp->fr_parent;
	    }
	    wp->w_status_height = statusline_height(wp);
	    যদি (fp != fr)
	    {
		frame_new_height(fp, fp->fr_height - wp->w_status_height,
			FALSE, FALSE, FALSE);
		frame_fix_height(wp);
		win_comp_pos();
	    }
	    অন্যথা
		win_new_height(wp, wp->w_height - wp->w_status_height);
	    comp_col();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	যদি (abs(wp->w_height - wp->w_পূর্ব_height) == 1)
	    wp->w_পূর্ব_height = wp->w_height;
    }
    অন্যথা যদি (fr->fr_layout == FR_ROW)
    {
	// vertically split windows, set status line for each one
	যদি (!statusline)
	{
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		last_status_rec(fp, statusline);
	}
	অন্যথা
	{
	    frame_T	*fp2;
	    পূর্ণসংখ্যা		max_stlh = 0;
	    পূর্ণসংখ্যা		new_row_height;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		যদি (fp->fr_win != ফাঁকা && fp->fr_win->w_status_height == 0)
		{
		    পূর্ণসংখ্যা h = statusline_height(fp->fr_win);
		    যদি (h > max_stlh)
			max_stlh = h;
		}
	    যদি (max_stlh == 0)
		ফেরত;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    যখন (fp2->fr_height - frame_minheight(fp2, ফাঁকা) < max_stlh)
	    {
		যদি (fp2 == topframe)
		{
		    emsg(_(e_not_enough_room));
		    ফেরত;
		}
		যদি (fp2->fr_parent->fr_layout == FR_COL
						&& fp2->fr_পূর্ব != ফাঁকা)
		    fp2 = fp2->fr_পূর্ব;
		অন্যথা
		    fp2 = fp2->fr_parent;
	    }

	    যদি (fp2 != fr)
	    {
		frame_new_height(fp2, fp2->fr_height - max_stlh,
							FALSE, FALSE, FALSE);
		new_row_height = fr->fr_height + max_stlh;
	    }
	    অন্যথা
		new_row_height = fr->fr_height;

	    // Set status and content heights for all leaves.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
	    {
		যদি (fp->fr_win != ফাঁকা)
		{
		    wp = fp->fr_win;
		    যদি (wp->w_status_height == 0)
		    {
			wp->w_status_height = statusline_height(wp);
			win_new_height(wp,
				  new_row_height - wp->w_status_height);
			frame_fix_height(wp);
			comp_col();
			redraw_all_later(UPD_SOME_VALID);
			যদি (abs(wp->w_height - wp->w_পূর্ব_height) == 1)
			    wp->w_পূর্ব_height = wp->w_height;
		    }
		}
		অন্যথা
		    last_status_rec(fp, statusline); // nested frames
	    }

	    fr->fr_height = new_row_height;
	    win_comp_pos();
	}
    }
    অন্যথা
    {
	// horizontally split window, set status line for last one
	জন্য (fp = fr->fr_child; fp->fr_পরবর্তী != ফাঁকা; fp = fp->fr_পরবর্তী)
	    ;
	last_status_rec(fp, statusline);
    }
}

#যদি defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    শূন্য
update_stl_rendered_height(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == ফাঁকা) ? p_stl : wp->w_p_stl;
    পূর্ণসংখ্যা		opt_flags = (wp == ফাঁকা) ? 0 : OPT_LOCAL;
    পূর্ণসংখ্যা		*height = (wp == ফাঁকা) ? &stl_rendered_height
						: &wp->w_stl_rendered_height;

    *height = get_stl_rendered_height(wp == ফাঁকা ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    স্থির পূর্ণসংখ্যা
frame_find_global_stlh_rec(frame_T *frp, পূর্ণসংখ্যা h)
{
    যদি (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	যদি (wp->w_height > 0 && wp->w_status_height > 0
		&& *wp->w_p_stlo == NUL)
	{
	    পূর্ণসংখ্যা win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    যদি (win_free_height - p_wmh < h)
		h = win_free_height - p_wmh;
	}
    }
    অন্যথা
    {
	জন্য (frp = frp->fr_child; frp != ফাঁকা; frp = frp->fr_পরবর্তী)
	    h = frame_find_global_stlh_rec(frp, h);
    }
    ফেরত h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    স্থির শূন্য
frame_change_statusline_height_rec(frame_T *frp, পূর্ণসংখ্যা global_stlh)
{
    যদি (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	যদি (wp->w_height > 0 && wp->w_status_height > 0)
	{
	    পূর্ণসংখ্যা win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    wp->w_status_height = statusline_height(wp);
	    যদি (wp->w_p_stlo == ফাঁকা || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		যদি (wp->w_status_height > global_stlh)
		    wp->w_status_height = global_stlh;
	    }
	    যদি (wp->w_status_height > win_free_height - p_wmh)
		wp->w_status_height = win_free_height - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    পূর্ণসংখ্যা	     saved_topfill = wp->w_topfill;
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
    অন্যথা
    {
	জন্য (frp = frp->fr_child; frp != ফাঁকা; frp = frp->fr_পরবর্তী)
	    frame_change_statusline_height_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    পূর্ণসংখ্যা
frame_change_statusline_height(শূন্য)
{
    tabpage_T	*tp;
    পূর্ণসংখ্যা		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = frame_find_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	frame_change_statusline_height_rec(tp->tp_topframe, global_stlh);
    comp_col();
    redraw_all_later(UPD_SOME_VALID);
    ফেরত global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    শূন্য
set_stlo_mh(পূর্ণসংখ্যা mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    পূর্ণসংখ্যা
statuslineopt_changed(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    পূর্ণসংখ্যা		l_stlo_fh = FALSE;
    পূর্ণসংখ্যা		l_stlo_mh = 1;

    যদি (stlopt != ফাঁকা)
	p = stlopt;
    অন্যথা
	p = wp->w_p_stlo;

    যখন (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	যদি (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    যদি (l_stlo_mh < 1)
		ফেরত FAIL;
	}
	অন্যথা যদি (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	যদি (*p != ',' && *p != NUL)
	    ফেরত FAIL;
	যদি (*p == ',')
	    ++p;
    }

    যদি (wp == ফাঁকা)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    অন্যথা
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    ফেরত OK;
}
#endif

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    পূর্ণসংখ্যা
statusline_height(win_T *wp UNUSED)
{
    পূর্ণসংখ্যা stl_height = 1;
#যদি defined(FEAT_STL_OPT)
    bool fixed;
    পূর্ণসংখ্যা l_stl_rendered_height;

    যদি (wp != ফাঁকা && *wp->w_p_stlo != NUL)
    {
	stl_height = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_height = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }
    অন্যথা
    {
	stl_height = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_height = (wp != ফাঁকা && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    যদি (!fixed)
    {
	পূর্ণসংখ্যা has_custom_stl = (*p_stl != NUL)
	    || (wp != ফাঁকা && *wp->w_p_stl != NUL);
	যদি (has_custom_stl)
	    stl_height = MAX(1, MIN(l_stl_rendered_height, stl_height));
	অন্যথা
	    stl_height = 1;
    }
#endif

    ফেরত stl_height;
}

/*
 * Return the number of lines used by the tab page line.
 */
    পূর্ণসংখ্যা
tabline_height(শূন্য)
{
#ifdef FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    যদি (gui_use_tabline())
	ফেরত 0;
#endif
    নির্বাচন (p_stal)
    {
	ক্ষেত্র 0: ফেরত 0;
	ক্ষেত্র 1: ফেরত (first_tabpage->tp_পরবর্তী == ফাঁকা) ? 0 : 1;
    }
    ফেরত 1;
}

/*
 * Return the height of the last window's statusline.
 */
    পূর্ণসংখ্যা
last_stl_height(
    পূর্ণসংখ্যা		morewin)	// pretend there are two or more windows
{
    ফেরত (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_WINDOW)))
		? STATUS_HEIGHT : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    পূর্ণসংখ্যা
min_rows(শূন্য)
{
    যদি (firstwin == ফাঁকা)	// not initialized yet
	ফেরত MIN_সারিগুলি;

    ফেরত frame_minheight(curtab->tp_topframe, ফাঁকা) + tabline_height()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    পূর্ণসংখ্যা
min_rows_for_all_tabpages(শূন্য)
{
    পূর্ণসংখ্যা		যোগ;
    tabpage_T	*tp;
    পূর্ণসংখ্যা		n;

    যদি (firstwin == ফাঁকা)	// not initialized yet
	ফেরত MIN_সারিগুলি;

    যোগ = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = frame_minheight(tp->tp_topframe, ফাঁকা);
	যদি (যোগ < n)
	    যোগ = n;
    }
    যোগ += tabline_height();
    যোগ += MIN_CMDHEIGHT;
    ফেরত যোগ;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    পূর্ণসংখ্যা
only_one_window(শূন্য)
{
    পূর্ণসংখ্যা		গুনতি = 0;
    win_T	*wp;

#যদি defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    যদি (popup_is_popup(curwin))
	ফেরত FALSE;
#endif

    // If there is another tab page there always is another window.
    যদি (first_tabpage->tp_পরবর্তী != ফাঁকা)
	ফেরত FALSE;

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_বাফার != ফাঁকা
		&& (!((bt_help(wp->w_বাফার) && !bt_help(curbuf))
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++গুনতি;
    ফেরত (গুনতি <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    স্থির শূন্য
check_lnums_both(পূর্ণসংখ্যা do_curwin, পূর্ণসংখ্যা nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	যদি ((do_curwin || wp != curwin) && wp->w_বাফার == curbuf)
	{
	    পূর্ণসংখ্যা need_adjust;

	    যদি (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_সারি_গুনতি;
	    যদি (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_সারি_গুনতি;
	    যদি (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_সারি_গুনতি;
	    যদি (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_সারি_গুনতি;
	    যদি (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    শূন্য
check_lnums(পূর্ণসংখ্যা do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    শূন্য
check_lnums_nested(পূর্ণসংখ্যা do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    শূন্য
reset_lnums(শূন্য)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	যদি (wp->w_বাফার == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    যদি (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    যদি (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    যদি (wp->w_save_cursor.w_topline_save > wp->w_বাফার->b_ml.ml_সারি_গুনতি)
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
    পূর্ণসংখ্যা
make_snapshot(পূর্ণসংখ্যা idx)
{
    clear_snapshot(curtab, idx);
    যদি (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	ফেরত FAIL;
    }
    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
make_snapshot_rec(frame_T *fr, frame_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(frame_T);
    যদি (*frp == ফাঁকা)
	ফেরত FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_width = fr->fr_width;
    (*frp)->fr_height = fr->fr_height;
    যদি (fr->fr_পরবর্তী != ফাঁকা)
    {
	যদি (make_snapshot_rec(fr->fr_পরবর্তী, &((*frp)->fr_পরবর্তী)) == FAIL)
	    ফেরত FAIL;
    }
    যদি (fr->fr_child != ফাঁকা)
    {
	যদি (make_snapshot_rec(fr->fr_child, &((*frp)->fr_child)) == FAIL)
	    ফেরত FAIL;
    }
    যদি (fr->fr_layout == FR_LEAF && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    ফেরত OK;
}

/*
 * Remove any existing snapshot.
 */
    স্থির শূন্য
clear_snapshot(tabpage_T *tp, পূর্ণসংখ্যা idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = ফাঁকা;
}

    স্থির শূন্য
clear_snapshot_rec(frame_T *fr)
{
    যদি (fr == ফাঁকা)
	ফেরত;
    clear_snapshot_rec(fr->fr_পরবর্তী);
    clear_snapshot_rec(fr->fr_child);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    স্থির win_T *
get_snapshot_curwin_rec(frame_T *ft)
{
    win_T	*wp;

    যদি (ft->fr_পরবর্তী != ফাঁকা)
    {
	যদি ((wp = get_snapshot_curwin_rec(ft->fr_পরবর্তী)) != ফাঁকা)
	    ফেরত wp;
    }
    যদি (ft->fr_child != ফাঁকা)
    {
	যদি ((wp = get_snapshot_curwin_rec(ft->fr_child)) != ফাঁকা)
	    ফেরত wp;
    }

    ফেরত ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    স্থির win_T *
get_snapshot_curwin(পূর্ণসংখ্যা idx)
{
    যদি (curtab->tp_snapshot[idx] == ফাঁকা)
	ফেরত ফাঁকা;

    ফেরত get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    শূন্য
restore_snapshot(
    পূর্ণসংখ্যা		idx,
    পূর্ণসংখ্যা		close_curwin)	    // closing current window
{
    win_T	*wp;

    যদি (curtab->tp_snapshot[idx] != ফাঁকা
	    && curtab->tp_snapshot[idx]->fr_width == topframe->fr_width
	    && curtab->tp_snapshot[idx]->fr_height == topframe->fr_height
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	যদি (wp != ফাঁকা && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    স্থির পূর্ণসংখ্যা
check_snapshot_rec(frame_T *sn, frame_T *fr)
{
    যদি (sn->fr_layout != fr->fr_layout
	    || (sn->fr_পরবর্তী == ফাঁকা) != (fr->fr_পরবর্তী == ফাঁকা)
	    || (sn->fr_child == ফাঁকা) != (fr->fr_child == ফাঁকা)
	    || (sn->fr_পরবর্তী != ফাঁকা
		&& check_snapshot_rec(sn->fr_পরবর্তী, fr->fr_পরবর্তী) == FAIL)
	    || (sn->fr_child != ফাঁকা
		&& check_snapshot_rec(sn->fr_child, fr->fr_child) == FAIL)
	    || (sn->fr_win != ফাঁকা && !win_valid(sn->fr_win)))
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    স্থির win_T *
restore_snapshot_rec(frame_T *sn, frame_T *fr)
{
    win_T	*wp = ফাঁকা;
    win_T	*wp2;

    fr->fr_height = sn->fr_height;
    fr->fr_width = sn->fr_width;
    যদি (fr->fr_layout == FR_LEAF)
    {
	frame_new_height(fr, fr->fr_height, FALSE, FALSE, FALSE);
	frame_new_width(fr, fr->fr_width, FALSE, FALSE);
	wp = sn->fr_win;
    }
    যদি (sn->fr_পরবর্তী != ফাঁকা)
    {
	wp2 = restore_snapshot_rec(sn->fr_পরবর্তী, fr->fr_পরবর্তী);
	যদি (wp2 != ফাঁকা)
	    wp = wp2;
    }
    যদি (sn->fr_child != ফাঁকা)
    {
	wp2 = restore_snapshot_rec(sn->fr_child, fr->fr_child);
	যদি (wp2 != ফাঁকা)
	    wp = wp2;
    }
    ফেরত wp;
}

#যদি defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    পূর্ণসংখ্যা
win_hasvertsplit(শূন্য)
{
    frame_T	*fr;

    যদি (topframe->fr_layout == FR_ROW)
	ফেরত TRUE;

    যদি (topframe->fr_layout == FR_COL)
	FOR_ALL_FRAMES(fr, topframe->fr_child)
	    যদি (fr->fr_layout == FR_ROW)
		ফেরত TRUE;

    ফেরত FALSE;
}
#endif

#যদি defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    পূর্ণসংখ্যা
get_win_সংখ্যা(win_T *wp, win_T *first_win)
{
    পূর্ণসংখ্যা		i = 1;
    win_T	*w;

    জন্য (w = first_win; w != ফাঁকা && w != wp; w = W_পরবর্তী(w))
	++i;

    যদি (w == ফাঁকা)
	ফেরত 0;
    অন্যথা
	ফেরত i;
}

    পূর্ণসংখ্যা
get_tab_সংখ্যা(tabpage_T *tp)
{
    পূর্ণসংখ্যা		i = 1;
    tabpage_T	*t;

    জন্য (t = first_tabpage; t != ফাঁকা && t != tp; t = t->tp_পরবর্তী)
	++i;

    যদি (t == ফাঁকা)
	ফেরত 0;
    অন্যথা
	ফেরত i;
}
#endif

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    স্থির পূর্ণসংখ্যা
frame_check_height(frame_T *topfrp, পূর্ণসংখ্যা height)
{
    frame_T *frp;

    যদি (topfrp->fr_height != height)
	ফেরত FALSE;

    যদি (topfrp->fr_layout == FR_ROW)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    যদি (frp->fr_height != height)
		ফেরত FALSE;

    ফেরত TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    স্থির পূর্ণসংখ্যা
frame_check_width(frame_T *topfrp, পূর্ণসংখ্যা width)
{
    frame_T *frp;

    যদি (topfrp->fr_width != width)
	ফেরত FALSE;

    যদি (topfrp->fr_layout == FR_COL)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    যদি (frp->fr_width != width)
		ফেরত FALSE;

    ফেরত TRUE;
}

#যদি defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    স্থির পূর্ণসংখ্যা
int_cmp(ধ্রুবক শূন্য *pa, ধ্রুবক শূন্য *pb)
{
    ধ্রুবক পূর্ণসংখ্যা a = *(ধ্রুবক পূর্ণসংখ্যা *)pa;
    ধ্রুবক পূর্ণসংখ্যা b = *(ধ্রুবক পূর্ণসংখ্যা *)pb;
    যদি (a > b)
	ফেরত 1;
    যদি (a < b)
	ফেরত -1;
    ফেরত 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    অক্ষর *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = empty_option;
    পূর্ণসংখ্যা		tw;
    পূর্ণসংখ্যা		col;
    পূর্ণসংখ্যা		গুনতি = 0;
    পূর্ণসংখ্যা		color_cols[256];
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		j = 0;

    যদি (wp != ফাঁকা && wp->w_বাফার == ফাঁকা)
	ফেরত ফাঁকা;  // buffer was closed

    যদি (cc != ফাঁকা)
	s = cc;
    অন্যথা যদি (wp != ফাঁকা)
	s = wp->w_p_cc;

    যদি (wp != ফাঁকা)
	tw = wp->w_বাফার->b_p_tw;
    অন্যথা
	// buffer-local value not set, assume zero
	tw = 0;

    যখন (*s != NUL && গুনতি < 255)
    {
	যদি (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    col = (*s == '-') ? -1 : 1;
	    ++s;
	    যদি (!VIM_ISDIGIT(*s))
		ফেরত e_invalid_argument;
	    col = col * getdigits(&s);
	    যদি (tw == 0)
		যাও skip;  // 'textwidth' not set, skip this item
	    col += tw;
	    যদি (col < 0)
		যাও skip;
	}
	অন্যথা যদি (VIM_ISDIGIT(*s))
	    col = getdigits(&s);
	অন্যথা
	    ফেরত e_invalid_argument;
	color_cols[গুনতি++] = col - 1;  // 1-based to 0-based
skip:
	যদি (*s == NUL)
	    বিরতি;
	যদি (*s != ',')
	    ফেরত e_invalid_argument;
	যদি (*++s == NUL)
	    ফেরত e_invalid_argument;  // illegal trailing comma as in "set cc=80,"
    }

    যদি (wp == ফাঁকা)
	ফেরত ফাঁকা;  // only parse "cc"

    vim_free(wp->w_p_cc_cols);
    যদি (গুনতি == 0)
	wp->w_p_cc_cols = ফাঁকা;
    অন্যথা
    {
	wp->w_p_cc_cols = ALLOC_MULT(পূর্ণসংখ্যা, গুনতি + 1);
	যদি (wp->w_p_cc_cols != ফাঁকা)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_cols, গুনতি, আকার(পূর্ণসংখ্যা), int_cmp);

	    জন্য (i = 0; i < গুনতি; ++i)
		// skip duplicates
		যদি (j == 0 || wp->w_p_cc_cols[j - 1] != color_cols[i])
		    wp->w_p_cc_cols[j++] = color_cols[i];
	    wp->w_p_cc_cols[j] = -1;  // end marker
	}
    }

    ফেরত ফাঁকা;  // no error
}
#endif

    পূর্ণসংখ্যা
get_last_winid(শূন্য)
{
    ফেরত last_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   পূর্ণসংখ্যা
win_locked(win_T *wp)
{
    ফেরত wp->w_locked;
}
