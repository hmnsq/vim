/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#अन्तर्भाव "vim.h"

स्थिर शून्य cmd_with_गणनम्(वर्ण *cmd, char_u *bufp, size_t bufsize, long Prenum);
स्थिर शून्य win_init(win_T *newp, win_T *oldp, पूर्णाङ्क flags);
स्थिर शून्य win_init_some(win_T *newp, win_T *oldp);
स्थिर शून्य frame_comp_pos(frame_T *topfrp, पूर्णाङ्क *row, पूर्णाङ्क *col);
स्थिर शून्य frame_setheight(frame_T *curfrp, पूर्णाङ्क height);
स्थिर शून्य frame_setwidth(frame_T *curfrp, पूर्णाङ्क width);
स्थिर शून्य win_exchange(long);
स्थिर शून्य win_rotate(पूर्णाङ्क, पूर्णाङ्क);
स्थिर शून्य win_equal_rec(win_T *next_curwin, पूर्णाङ्क current, frame_T *topfr, पूर्णाङ्क dir, पूर्णाङ्क col, पूर्णाङ्क row, पूर्णाङ्क width, पूर्णाङ्क height);
स्थिर शून्य trigger_winnewpre(शून्य);
स्थिर शून्य trigger_winclosed(win_T *win);
स्थिर win_T *win_free_mem(win_T *win, पूर्णाङ्क *dirp, tabpage_T *tp);
स्थिर frame_T *win_altframe(win_T *win, tabpage_T *tp);
स्थिर tabpage_T *alt_tabpage(शून्य);
स्थिर win_T *frame2win(frame_T *frp);
स्थिर पूर्णाङ्क frame_has_win(frame_T *frp, win_T *wp);
स्थिर शून्य win_fix_scroll(पूर्णाङ्क resize);
स्थिर शून्य win_fix_cursor(पूर्णाङ्क normal);
स्थिर शून्य frame_new_height(frame_T *topfrp, पूर्णाङ्क height, पूर्णाङ्क topfirst, पूर्णाङ्क wfh, पूर्णाङ्क set_ch);
स्थिर पूर्णाङ्क frame_fixed_height(frame_T *frp);
स्थिर पूर्णाङ्क frame_fixed_width(frame_T *frp);
स्थिर शून्य frame_add_statusline(frame_T *frp);
स्थिर शून्य frame_new_width(frame_T *topfrp, पूर्णाङ्क width, पूर्णाङ्क leftfirst, पूर्णाङ्क wfw);
स्थिर शून्य frame_set_vsep(frame_T *frp, पूर्णाङ्क add);
स्थिर पूर्णाङ्क frame_minwidth(frame_T *topfrp, win_T *next_curwin);
स्थिर शून्य frame_fix_width(win_T *wp);
स्थिर पूर्णाङ्क win_alloc_firstwin(win_T *oldwin);
स्थिर शून्य new_frame(win_T *wp);
स्थिर tabpage_T *alloc_tabpage(शून्य);
स्थिर पूर्णाङ्क leave_tabpage(buf_T *new_curbuf, पूर्णाङ्क trigger_leave_autocmds);
स्थिर शून्य enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, पूर्णाङ्क trigger_enter_autocmds, पूर्णाङ्क trigger_leave_autocmds);
स्थिर शून्य frame_fix_height(win_T *wp);
स्थिर पूर्णाङ्क frame_minheight(frame_T *topfrp, win_T *next_curwin);
स्थिर पूर्णाङ्क frame_wincount_in_height(frame_T *topfrp);
स्थिर पूर्णाङ्क may_open_tabpage(शून्य);
स्थिर पूर्णाङ्क win_enter_ext(win_T *wp, पूर्णाङ्क flags);
स्थिर शून्य win_free(win_T *wp, tabpage_T *tp);
स्थिर शून्य win_append(win_T *after, win_T *wp);
स्थिर शून्य frame_append(frame_T *after, frame_T *frp);
स्थिर शून्य frame_निवेशय(frame_T *before, frame_T *frp);
स्थिर शून्य frame_remove(frame_T *frp);
स्थिर शून्य win_goto_ver(पूर्णाङ्क up, long गणनम्);
स्थिर शून्य win_goto_hor(पूर्णाङ्क वामः, long गणनम्);
स्थिर शून्य frame_add_height(frame_T *frp, पूर्णाङ्क n);
स्थिर शून्य last_status_rec(frame_T *fr, पूर्णाङ्क statusline);
#सचे defined(FEAT_STL_OPT)
स्थिर पूर्णाङ्क frame_find_global_stlh_rec(frame_T *frp, पूर्णाङ्क h);
स्थिर शून्य frame_change_statusline_height_rec(frame_T *frp, पूर्णाङ्क global_stlh);
#endif
स्थिर शून्य frame_flatten(frame_T *frp);
स्थिर शून्य winframe_restore(win_T *wp, पूर्णाङ्क dir, frame_T *unflat_altfr);

स्थिर पूर्णाङ्क make_snapshot_rec(frame_T *fr, frame_T **frp);
स्थिर शून्य clear_snapshot(tabpage_T *tp, पूर्णाङ्क idx);
स्थिर शून्य clear_snapshot_rec(frame_T *fr);
स्थिर पूर्णाङ्क check_snapshot_rec(frame_T *sn, frame_T *fr);
स्थिर win_T *restore_snapshot_rec(frame_T *sn, frame_T *fr);
स्थिर win_T *get_snapshot_curwin(पूर्णाङ्क idx);

स्थिर पूर्णाङ्क frame_check_height(frame_T *topfrp, पूर्णाङ्क height);
स्थिर पूर्णाङ्क frame_check_width(frame_T *topfrp, पूर्णाङ्क width);

स्थिर win_T *win_alloc(win_T *after, पूर्णाङ्क hidden);

#define NOWIN		((win_T *)-1)	// non-existing window

#define ROWS_AVAIL (Rows - p_ch - tabline_height())

// flags for win_enter_ext()
#define WEE_UNDO_SYNC			0x01
#define WEE_CURWIN_INVALID		0x02
#define WEE_TRIGGER_NEW_AUTOCMDS	0x04
#define WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#define WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#define WEE_ALLOW_PARSE_MESSAGES	0x20

स्थिर वर्ण *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
स्थिर पूर्णाङ्क split_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
स्थिर पूर्णाङ्क close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
स्थिर पूर्णाङ्क frame_locked = 0;

#सचे defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
स्थिर पूर्णाङ्क stl_rendered_height = 1;
स्थिर पूर्णाङ्क stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
स्थिर पूर्णाङ्क stlo_mh = STATUS_HEIGHT; // "maxheight" value cache of global 'stlo'
#endif

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    शून्य
window_layout_lock(शून्य)
{
    ++split_disallowed;
    ++close_disallowed;
}

    शून्य
window_layout_unlock(शून्य)
{
    --split_disallowed;
    --close_disallowed;
}

    पूर्णाङ्क
frames_locked(शून्य)
{
    पटिनिवत्त frame_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    पूर्णाङ्क
window_layout_locked(गणना CMD_index cmd)
{
    सचे (split_disallowed > 0 || close_disallowed > 0)
    {
	सचे (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_split_window_when_closing_buffer));
	अञ्ञथा
	    emsg(_(e_not_allowed_to_change_window_layout_in_this_autocmd));
	पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

// #define WIN_DEBUG
#ifdef WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    स्थिर शून्य
log_frame_layout(frame_T *frame)
{
    ch_log(रिक्त, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    frame->fr_layout == FR_LEAF ? "LEAF"
				  : frame->fr_layout == FR_ROW ? "ROW" : "COL",
	    frame->fr_width,
	    frame->fr_height,
	    frame->fr_win == रिक्त ? -1 : frame->fr_win->w_width,
	    frame->fr_win == रिक्त ? -1 : frame->fr_win->w_height,
	    frame->fr_win == रिक्त ? -1 : frame->fr_win->w_id);
    सचे (frame->fr_child != रिक्त)
    {
	ch_log(रिक्त, "children");
	log_frame_layout(frame->fr_child);
	सचे (frame->fr_next != रिक्त)
	    ch_log(रिक्त, "END of children");
    }
    सचे (frame->fr_next != रिक्त)
	log_frame_layout(frame->fr_next);
}
#endif

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    पूर्णाङ्क
check_can_set_curbuf_disabled(शून्य)
{
    सचे (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	पटिनिवत्त FALSE;
    }
    पटिनिवत्त TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    पूर्णाङ्क
check_can_set_curbuf_forceit(पूर्णाङ्क forceit)
{
    सचे (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	पटिनिवत्त FALSE;
    }
    पटिनिवत्त TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(शून्य)
{
    // In cmdwin, the alternative buffer should be used.
    पटिनिवत्त is_in_cmdwin() && prevwin != रिक्त ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = रिक्त;

    सचे (buf == रिक्त)
	पटिनिवत्त wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    सचे (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    सचे (wp == रिक्त && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    पटिनिवत्त wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    शून्य
do_window(
    पूर्णाङ्क		nchar,
    long	Prenum,
    पूर्णाङ्क		xchar)	    // extra char from ":wincmd gx" or NUL
{
    long	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ifdef FEAT_FIND_ID
    पूर्णाङ्क		type = FIND_DEFINE;
    पूर्णाङ्क		len;
#endif
    char_u	cbuf[40];

    सचे (ERROR_IF_ANY_POPUP_WINDOW)
	पटिनिवत्त;

#define CHECK_CMDWIN \
    करोहि { \
	सचे (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_in_cmdline_window)); \
	    पटिनिवत्त; \
	} \
    } याव (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    विकल्प (nchar)
    {
// split current window in two parts, horizontally
    पक्ष 'S':
    पक्ष Ctrl_S:
    पक्ष 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		सचे (bt_quickfix(curbuf))
		    गच्छ newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(शून्य)win_split((पूर्णाङ्क)Prenum, 0);
		भङ्ग;

// split current window in two parts, vertically
    पक्ष Ctrl_V:
    पक्ष 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		सचे (bt_quickfix(curbuf))
		    गच्छ newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(शून्य)win_split((पूर्णाङ्क)Prenum, WSP_VERT);
		भङ्ग;

// split current window and edit alternate file
    पक्ष Ctrl_HAT:
    पक्ष '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		सचे (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == रिक्त)
		{
		    सचे (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    अञ्ञथा
			semsg(_(e_buffer_nr_not_found), Prenum);
		    भङ्ग;
		}

		सचे (!curbuf_locked() && win_split(0, 0) == OK)
		    (शून्य)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		भङ्ग;

// open new window
    पक्ष Ctrl_N:
    पक्ष 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		सचे (Prenum)
		    // window height
		    vim_snprintf((वर्ण *)cbuf, परिमाण(cbuf) - 5, "%ld", Prenum);
		अञ्ञथा
		    cbuf[0] = NUL;
#सचे defined(FEAT_QUICKFIX)
		सचे (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#endif
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		भङ्ग;

// quit current window
    पक्ष Ctrl_Q:
    पक्ष 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_गणनम्("quit", cbuf, परिमाण(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		भङ्ग;

// close current window
    पक्ष Ctrl_C:
    पक्ष 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_गणनम्("close", cbuf, परिमाण(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		भङ्ग;

#सचे defined(FEAT_QUICKFIX)
// close preview window
    पक्ष Ctrl_Z:
    पक्ष 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		भङ्ग;

// cursor to preview window
    पक्ष 'P':
		FOR_ALL_WINDOWS(wp)
		    सचे (wp->w_p_pvw)
			भङ्ग;
		सचे (wp == रिक्त)
		    emsg(_(e_there_is_no_preview_window));
		अञ्ञथा
		    win_goto(wp);
		भङ्ग;
#endif

// close all but current window
    पक्ष Ctrl_O:
    पक्ष 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_गणनम्("only", cbuf, परिमाण(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		भङ्ग;

// cursor to next window with wrap around
    पक्ष Ctrl_W:
    पक्ष 'w':
// cursor to previous window with wrap around
    पक्ष 'W':
		CHECK_CMDWIN;
		सचे (ONE_WINDOW && Prenum != 1)	// just one window
		    beep_flush();
		अञ्ञथा
		{
		    सचे (Prenum)			// go to specified window
		    {
			अत्थाय (wp = firstwin; --Prenum > 0; )
			{
			    सचे (wp->w_next == रिक्त)
				भङ्ग;
			    अञ्ञथा
				wp = wp->w_next;
			}
		    }
		    अञ्ञथा
		    {
			सचे (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    सचे (wp == रिक्त)
				wp = lastwin;	    // wrap around
			}
			अञ्ञथा			    // go to next window
			{
			    wp = curwin->w_next;
			    सचे (wp == रिक्त)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		भङ्ग;

// cursor to window below
    पक्ष 'j':
    पक्ष K_DOWN:
    पक्ष Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		भङ्ग;

// cursor to window above
    पक्ष 'k':
    पक्ष K_UP:
    पक्ष Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		भङ्ग;

// cursor to left window
    पक्ष 'h':
    पक्ष K_वामः:
    पक्ष Ctrl_H:
    पक्ष K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		भङ्ग;

// cursor to right window
    पक्ष 'l':
    पक्ष K_दक्षिणः:
    पक्ष Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		भङ्ग;

// move window to new tab page
    पक्ष 'T':
		CHECK_CMDWIN;
		सचे (one_window())
		    सन्देशः(_(m_onlyone));
		अञ्ञथा
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    सचे (win_new_tabpage((पूर्णाङ्क)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			सचे (curwin == wp)
			    win_close(curwin, FALSE);
			सचे (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		भङ्ग;

// cursor to top-left window
    पक्ष 't':
    पक्ष Ctrl_T:
		win_goto(firstwin);
		भङ्ग;

// cursor to bottom-right window
    पक्ष 'b':
    पक्ष Ctrl_B:
		win_goto(lastwin);
		भङ्ग;

// cursor to last accessed (previous) window
    पक्ष 'p':
    पक्ष Ctrl_P:
		सचे (!win_valid(prevwin))
		    beep_flush();
		अञ्ञथा
		    win_goto(prevwin);
		भङ्ग;

// exchange current and next window
    पक्ष 'x':
    पक्ष Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		भङ्ग;

// rotate windows downwards
    पक्ष Ctrl_R:
    पक्ष 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (पूर्णाङ्क)Prenum1);    // downwards
		भङ्ग;

// rotate windows upwards
    पक्ष 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (पूर्णाङ्क)Prenum1);	    // upwards
		भङ्ग;

// move window to the very top/bottom/left/right
    पक्ष 'K':
    पक्ष 'J':
    पक्ष 'H':
    पक्ष 'L':
		CHECK_CMDWIN;
		सचे (ONE_WINDOW)
		    beep_flush();
		अञ्ञथा
		{
		    पूर्णाङ्क dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_शिखरम् : WSP_BOT);

		    (शून्य)win_splitmove(curwin, (पूर्णाङ्क)Prenum, dir);
		}
		भङ्ग;

// make all windows the same width and/or height
    पक्ष '=':
		{
		    पूर्णाङ्क mod = cmdmod.cmod_split & (WSP_VERT | WSP_HOR);
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_equal(रिक्त, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		भङ्ग;

// increase current window height
    पक्ष '+':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height + (पूर्णाङ्क)Prenum1);
		भङ्ग;

// decrease current window height
    पक्ष '-':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height - (पूर्णाङ्क)Prenum1);
		भङ्ग;

// set current window height
    पक्ष Ctrl__:
    पक्ष '_':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(Prenum ? (पूर्णाङ्क)Prenum : 9999);
		भङ्ग;

// increase current window width
    पक्ष '>':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width + (पूर्णाङ्क)Prenum1);
		भङ्ग;

// decrease current window width
    पक्ष '<':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width - (पूर्णाङ्क)Prenum1);
		भङ्ग;

// set current window width
    पक्ष '|':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(Prenum != 0 ? (पूर्णाङ्क)Prenum : 9999);
		भङ्ग;

// jump to tag and split window if tag exists (in preview window)
#सचे defined(FEAT_QUICKFIX)
    पक्ष '}':
		CHECK_CMDWIN;
		सचे (Prenum)
		    g_do_tagpreview = Prenum;
		अञ्ञथा
		    g_do_tagpreview = p_pvh;
#endif
		// FALLTHROUGH
    पक्ष ']':
    पक्ष Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		सचे (Prenum)
		    postponed_split = Prenum;
		अञ्ञथा
		    postponed_split = -1;
#ifdef FEAT_QUICKFIX
		सचे (nchar != '}')
		    g_do_tagpreview = 0;
#endif

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_split = 0;
		भङ्ग;

// edit file name under cursor in a new window
    पक्ष 'f':
    पक्ष 'F':
    पक्ष Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		सचे (check_text_or_curbuf_locked(रिक्त))
		    भङ्ग;

		ptr = grab_file_name(Prenum1, &lnum);
		सचे (ptr != रिक्त)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = रिक्त;
		    सचे ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    सचे (wp == रिक्त && win_split(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			सचे (do_ecmd(0, ptr, रिक्त, रिक्त, ECMD_LASTL,
						   ECMD_HIDE, रिक्त) == FAIL)
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
			अञ्ञथा
			    wp = curwin;
		    }

		    सचे (wp != रिक्त && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		भङ्ग;

#ifdef FEAT_FIND_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    पक्ष 'i':			    // Go to any match
    पक्ष Ctrl_I:
		type = FIND_ANY;
		// FALLTHROUGH
    पक्ष 'd':			    // Go to definition, using 'define'
    पक्ष Ctrl_D:
		CHECK_CMDWIN;
		सचे ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
		    भङ्ग;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		सचे (ptr == रिक्त)
		    भङ्ग;

		find_pattern_in_path(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_SPLIT, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_set_curswant = true;
		भङ्ग;
#endif

// Quickfix window only: view the result under the cursor in a new split.
#सचे defined(FEAT_QUICKFIX)
    पक्ष K_KENTER:
    पक्ष CAR:
		सचे (bt_quickfix(curbuf))
		    qf_view_result(TRUE);
		भङ्ग;
#endif

// CTRL-W g  extended commands
    पक्ष 'g':
    पक्ष Ctrl_G:
		CHECK_CMDWIN;
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#endif
		++no_mapping;
		++allow_कुञ्चिकाः;   // no mapping for xchar, but allow key codes
		सचे (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_कुञ्चिकाः;
		(शून्य)add_to_showcmd(xchar);

		विकल्प (xchar)
		{
#सचे defined(FEAT_QUICKFIX)
		    पक्ष '}':
			xchar = Ctrl_RSB;
			सचे (Prenum)
			    g_do_tagpreview = Prenum;
			अञ्ञथा
			    g_do_tagpreview = p_pvh;
#endif
			// FALLTHROUGH
		    पक्ष ']':
		    पक्ष Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			सचे (Prenum)
			    postponed_split = Prenum;
			अञ्ञथा
			    postponed_split = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_split = 0;
			भङ्ग;

		    पक्ष 'f':	    // CTRL-W gf: "gf" in a new tab page
		    पक्ष 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_index(curtab) + 1;
			nchar = xchar;
			गच्छ wingotofile;

		    पक्ष 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((पूर्णाङ्क)Prenum);
			भङ्ग;

		    पक्ष 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(पूर्णाङ्क)Prenum1);
			भङ्ग;

		    पक्ष TAB:	    // CTRL-W g<Tab>: go to last used tab page
			सचे (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			भङ्ग;

		    स्वभाव:
			beep_flush();
			भङ्ग;
		}
		भङ्ग;

    स्वभाव:	beep_flush();
		भङ्ग;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    शून्य
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    विकल्प (*arg)
    {
    पक्ष 'S':
    पक्ष Ctrl_S:
    पक्ष 's':
    पक्ष Ctrl_N:
    पक्ष 'n':
    पक्ष 'j':
    पक्ष Ctrl_J:
    पक्ष 'k':
    पक्ष Ctrl_K:
    पक्ष 'T':
    पक्ष Ctrl_R:
    पक्ष 'r':
    पक्ष 'R':
    पक्ष 'K':
    पक्ष 'J':
    पक्ष '+':
    पक्ष '-':
    पक्ष Ctrl__:
    पक्ष '_':
    पक्ष '|':
    पक्ष ']':
    पक्ष Ctrl_RSB:
    पक्ष 'g':
    पक्ष Ctrl_G:
    पक्ष Ctrl_V:
    पक्ष 'v':
    पक्ष 'h':
    पक्ष Ctrl_H:
    पक्ष 'l':
    पक्ष Ctrl_L:
    पक्ष 'H':
    पक्ष 'L':
    पक्ष '>':
    पक्ष '<':
#सचे defined(FEAT_QUICKFIX)
    पक्ष '}':
#endif
    पक्ष 'f':
    पक्ष 'F':
    पक्ष Ctrl_F:
#ifdef FEAT_FIND_ID
    पक्ष 'i':
    पक्ष Ctrl_I:
    पक्ष 'd':
    पक्ष Ctrl_D:
#endif
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		भङ्ग;

    पक्ष Ctrl_HAT:
    पक्ष '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		भङ्ग;

    पक्ष Ctrl_Q:
    पक्ष 'q':
    पक्ष Ctrl_C:
    पक्ष 'c':
    पक्ष Ctrl_O:
    पक्ष 'o':
    पक्ष Ctrl_W:
    पक्ष 'w':
    पक्ष 'W':
    पक्ष 'x':
    पक्ष Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		भङ्ग;

#सचे defined(FEAT_QUICKFIX)
    पक्ष Ctrl_Z:
    पक्ष 'z':
    पक्ष 'P':
#endif
    पक्ष 't':
    पक्ष Ctrl_T:
    पक्ष 'b':
    पक्ष Ctrl_B:
    पक्ष 'p':
    पक्ष Ctrl_P:
    पक्ष '=':
    पक्ष CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		भङ्ग;
    }
}

    स्थिर शून्य
cmd_with_गणनम्(
    वर्ण	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    long	Prenum)
{
    सचे (Prenum > 0)
	vim_snprintf((वर्ण *)bufp, bufsize, "%s %ld", cmd, Prenum);
    अञ्ञथा
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    पूर्णाङ्क
check_split_disallowed(win_T *wp)
{
    सचे (split_disallowed > 0)
    {
	emsg(_(e_cant_split_window_while_closing_another));
	पटिनिवत्त FAIL;
    }
    सचे (wp->w_buffer->b_locked_split)
    {
	emsg(_(e_cannot_split_window_when_closing_buffer));
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
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
    पूर्णाङ्क
win_split(पूर्णाङ्क size, पूर्णाङ्क flags)
{
    सचे (ERROR_IF_ANY_POPUP_WINDOW)
	पटिनिवत्त FAIL;

    सचे (check_split_disallowed(curwin) == FAIL)
	पटिनिवत्त FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    सचे (may_open_tabpage() == OK)
	पटिनिवत्त OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_split;
    सचे ((flags & WSP_शिखरम्) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_split_topleft_and_botright_at_the_same_time));
	पटिनिवत्त FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    सचे (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    अञ्ञथा
	clear_snapshot(curtab, SNAP_HELP_IDX);

    सचे (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    अञ्ञथा
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    पटिनिवत्त win_split_ins(size, flags, रिक्त, 0, रिक्त);
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
    पूर्णाङ्क
win_split_ins(
    पूर्णाङ्क		size,
    पूर्णाङ्क		flags,
    win_T	*new_wp,
    पूर्णाङ्क		dir,
    frame_T	*to_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    पूर्णाङ्क		new_size = size;
    पूर्णाङ्क		i;
    पूर्णाङ्क		need_status = 0;
    पूर्णाङ्क		do_equal = FALSE;
    पूर्णाङ्क		needed;
    पूर्णाङ्क		available;
    पूर्णाङ्क		oldwin_height = 0;
    पूर्णाङ्क		layout;
    frame_T	*frp, *curfrp, *frp2, *prevfrp;
    पूर्णाङ्क		before;
    पूर्णाङ्क		minheight;
    पूर्णाङ्क		wmh1;
    पूर्णाङ्क		did_set_fraction = FALSE;
    पूर्णाङ्क		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    सचे (new_wp == रिक्त)
	trigger_winnewpre();

    सचे (flags & WSP_शिखरम्)
	oldwin = firstwin;
    अञ्ञथा सचे (flags & WSP_BOT)
	oldwin = lastwin;
    अञ्ञथा
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    सचे (ONE_WINDOW && p_ls == 1 && oldwin->w_status_height == 0)
    {
	सचे (!(flags & WSP_FORCE_ROOM) && VISIBLE_HEIGHT(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_room));
	    गच्छ theend;
	}
	need_status = statusline_height(oldwin);
    }

#ifdef FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    सचे (gui.in_use)
	out_flush();
#endif

    सचे (flags & WSP_VERT)
    {
	पूर्णाङ्क	wmw1;
	पूर्णाङ्क	minwidth;

	layout = FR_ROW;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	सचे (flags & WSP_ROOM)
	    needed += p_wiw - wmw1;
	सचे (flags & (WSP_BOT | WSP_शिखरम्))
	{
	    minwidth = frame_minwidth(topframe, NOWIN);
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	अञ्ञथा सचे (p_ea)
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    prevfrp = oldwin->w_frame;
	    अत्थाय (frp = oldwin->w_frame->fr_parent; frp != रिक्त;
							frp = frp->fr_parent)
	    {
		सचे (frp->fr_layout == FR_ROW)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			सचे (frp2 != prevfrp)
			    minwidth += frame_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	अञ्ञथा
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    available = oldwin->w_frame->fr_width;
	    needed += minwidth;
	}
	सचे (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    गच्छ theend;
	}
	सचे (new_size == 0)
	    new_size = oldwin->w_width / 2;
	सचे (new_size > available - minwidth - 1)
	    new_size = available - minwidth - 1;
	सचे (new_size < wmw1)
	    new_size = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	सचे (oldwin->w_width - new_size - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	सचे (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_width + new_size + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	सचे (!do_equal && p_ea && size == 0 && *p_ead != 'v'
	    && oldwin->w_frame->fr_parent != रिक्त)
	    do_equal = TRUE;
    }
    अञ्ञथा
    {
	layout = FR_COL;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_HEIGHT(curwin);
	needed = wmh1 + statusline_height(oldwin);
	सचे (flags & WSP_ROOM)
	    needed += p_wh - wmh1;
	सचे (flags & (WSP_BOT | WSP_शिखरम्))
	{
	    minheight = frame_minheight(topframe, NOWIN) + need_status;
	    available = topframe->fr_height;
	    needed += minheight;
	}
	अञ्ञथा सचे (p_ea)
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    prevfrp = oldwin->w_frame;
	    अत्थाय (frp = oldwin->w_frame->fr_parent; frp != रिक्त;
							frp = frp->fr_parent)
	    {
		सचे (frp->fr_layout == FR_COL)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			सचे (frp2 != prevfrp)
			    minheight += frame_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_height;
	    needed += minheight;
	}
	अञ्ञथा
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    available = oldwin->w_frame->fr_height;
	    needed += minheight;
	}
	सचे (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    गच्छ theend;
	}
	oldwin_height = oldwin->w_height;
	सचे (need_status)
	{
	    oldwin->w_status_height = statusline_height(oldwin);
	    oldwin_height -= oldwin->w_status_height;
	}
	// Estimated stl height for the new window.
	पूर्णाङ्क new_win_stlh = need_status != 0 ? need_status
			: statusline_height(new_wp != रिक्त ? new_wp : curwin);
	सचे (new_size == 0)
	    new_size = (oldwin_height - new_win_stlh + 1) / 2;
	सचे (new_size > available - minheight - new_win_stlh)
	    new_size = available - minheight - new_win_stlh;
	सचे (new_size < wmh1)
	    new_size = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	सचे (oldwin_height - new_size - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	सचे (oldwin->w_p_wfh)
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
	सचे (!do_equal && p_ea && size == 0 && *p_ead != 'h'
	    && oldwin->w_frame->fr_parent != रिक्त)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    सचे ((flags & WSP_शिखरम्) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_BELOW)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	सचे (new_wp == रिक्त)
	    wp = win_alloc(oldwin, FALSE);
	अञ्ञथा
	    win_append(oldwin, wp);
    }
    अञ्ञथा
    {
	सचे (new_wp == रिक्त)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	अञ्ञथा
	    win_append(oldwin->w_prev, wp);
    }

    सचे (new_wp == रिक्त)
    {
	सचे (wp == रिक्त)
	    गच्छ theend;

	new_frame(wp);
	सचे (wp->w_frame == रिक्त)
	{
	    win_free(wp, रिक्त);
	    गच्छ theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    सचे (to_flatten != रिक्त)
	frame_flatten(to_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    सचे (flags & (WSP_शिखरम् | WSP_BOT))
    {
	सचे ((topframe->fr_layout == FR_COL && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ROW && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_child;
	    सचे (flags & WSP_BOT)
		याव (curfrp->fr_next != रिक्त)
		    curfrp = curfrp->fr_next;
	}
	अञ्ञथा
	    curfrp = topframe;
	before = (flags & WSP_शिखरम्);
    }
    अञ्ञथा
    {
	curfrp = oldwin->w_frame;
	सचे (flags & WSP_BELOW)
	    before = FALSE;
	अञ्ञथा सचे (flags & WSP_ABOVE)
	    before = TRUE;
	अञ्ञथा सचे (flags & WSP_VERT)
	    before = !p_spr;
	अञ्ञथा
	    before = !p_sb;
    }
    सचे (curfrp->fr_parent == रिक्त || curfrp->fr_parent->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(frame_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_parent = curfrp;
	frp->fr_next = रिक्त;
	frp->fr_prev = रिक्त;
	curfrp->fr_child = frp;
	curfrp->fr_win = रिक्त;
	curfrp = frp;
	सचे (frp->fr_win != रिक्त)
	    oldwin->w_frame = frp;
	अञ्ञथा
	    FOR_ALL_FRAMES(frp, frp->fr_child)
		frp->fr_parent = curfrp;
    }

    सचे (new_wp == रिक्त)
	frp = wp->w_frame;
    अञ्ञथा
	frp = new_wp->w_frame;
    frp->fr_parent = curfrp->fr_parent;

    // Insert the new frame at the right place in the frame list.
    सचे (before)
	frame_निवेशय(curfrp, frp);
    अञ्ञथा
	frame_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    सचे (!did_set_fraction)
	set_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    सचे (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	सचे (need_status)
	{
	    win_new_height(oldwin, oldwin->w_height - need_status);
	    oldwin->w_status_height = need_status;
	}
	सचे (flags & (WSP_शिखरम् | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    पूर्णाङ्क stl_height = p_ls > 0 ? statusline_height(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_height();
	    win_new_height(wp, curfrp->fr_height - stl_height
							  - WINBAR_HEIGHT(wp));
	    wp->w_status_height = stl_height;
	}
	अञ्ञथा
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
	सचे (before)
	    wp->w_vsep_width = 1;
	अञ्ञथा
	{
	    wp->w_vsep_width = oldwin->w_vsep_width;
	    oldwin->w_vsep_width = 1;
	}
	सचे (flags & (WSP_शिखरम् | WSP_BOT))
	{
	    सचे (flags & WSP_BOT)
		frame_set_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    frame_new_width(curfrp, curfrp->fr_width
		     - (new_size + ((flags & WSP_शिखरम्) != 0)), flags & WSP_शिखरम्,
								       FALSE);
	}
	अञ्ञथा
	    win_new_width(oldwin, oldwin->w_width - (new_size + 1));
	सचे (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_size + 1;
	}
	अञ्ञथा		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_width + 1;
	frame_fix_width(oldwin);
	frame_fix_width(wp);
    }
    अञ्ञथा
    {
	// width and column of new window is same as current window
	सचे (flags & (WSP_शिखरम् | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_width(wp, topframe->fr_width);
	    wp->w_vsep_width = 0;
	}
	अञ्ञथा
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_width(wp, oldwin->w_width);
	    wp->w_vsep_width = oldwin->w_vsep_width;
	}
	frp->fr_width = curfrp->fr_width;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_height(wp, new_size);
	पूर्णाङ्क old_status_height = oldwin->w_status_height;
	सचे (flags & (WSP_शिखरम् | WSP_BOT))
	{
	    पूर्णाङ्क new_fr_height = curfrp->fr_height - new_size
							  + WINBAR_HEIGHT(wp);

	    सचे (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_height -= statusline_height(curfrp->fr_win);
	    सचे (flags & WSP_BOT)
		frame_add_statusline(curfrp);
	    frame_new_height(curfrp, new_fr_height, flags & WSP_शिखरम्, FALSE,
									FALSE);
	}
	अञ्ञथा
	    win_new_height(oldwin, oldwin_height - (new_size
			+ statusline_height(wp)));
	सचे (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_status_height = statusline_height(wp);
	    oldwin->w_winrow += wp->w_height + statusline_height(wp);
	}
	अञ्ञथा		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_HEIGHT(oldwin)
						    + statusline_height(oldwin);
	    wp->w_status_height = old_status_height;
	    सचे (!(flags & WSP_BOT))
		oldwin->w_status_height = statusline_height(oldwin);
	}
	frame_fix_height(wp);
	frame_fix_height(oldwin);
    }

    सचे (flags & (WSP_शिखरम् | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    status_redraw_all();

    सचे (need_status)
    {
	सन्देशः_row = Rows - 1;
	सन्देशः_col = sc_col;
	सन्देशः_clr_eos_force();	// Old command/ruler may still be there
	comp_col();
	सन्देशः_row = Rows - 1;
	सन्देशः_col = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    सचे (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    अञ्ञथा सचे (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    सचे (flags & WSP_VERT)
    {
	i = p_wiw;
	सचे (size != 0)
	    p_wiw = size;

#ifdef FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	सचे (gui.in_use)
	    gui_init_which_components(रिक्त);
#endif
    }
    अञ्ञथा
    {
	i = p_wh;
	सचे (size != 0)
	    p_wh = size;
    }

    /*
     * make the new window the current window
     */
    (शून्य)win_enter_ext(wp, (new_wp == रिक्त ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    सचे (flags & WSP_VERT)
	p_wiw = i;
    अञ्ञथा
	p_wh = i;
    retval = OK;

theend:
    सचे (RedrawingDisabled > 0)
	--RedrawingDisabled;
    पटिनिवत्त retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    स्थिर शून्य
win_init(win_T *newp, win_T *oldp, पूर्णाङ्क flags UNUSED)
{
    पूर्णाङ्क		i;

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
    सचे (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = रिक्त;
	newp->w_llist_ref = रिक्त;
    }
    अञ्ञथा
	copy_loclist_राशिः(oldp, newp);
#endif
    newp->w_localdir = (oldp->w_localdir == रिक्त)
				    ? रिक्त : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == रिक्त)
				    ? रिक्त : vim_strsave(oldp->w_prevdir);

    सचे (*p_spk != 'c')
    {
	सचे (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_height = oldp->w_height - WINBAR_HEIGHT(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_HEIGHT(oldp);
    }

    // copy tagstack and folds
    अत्थाय (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*tag = &newp->w_tagstack[i];
	*tag = oldp->w_tagstack[i];
	सचे (tag->tagname != रिक्त)
	    tag->tagname = vim_strsave(tag->tagname);
	सचे (tag->user_data != रिक्त)
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
	bool pushed = प्रक्षिप_highlight_overrides(newp->w_hl, newp->w_hl_len);
	सचे (newp->w_buffer->b_term != रिक्त)
	    term_init_default_colors(newp->w_buffer->b_term);
	सचे (pushed)
	    निष्कासय_highlight_overrides();
    }
#endif
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    स्थिर शून्य
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
    पूर्णाङ्क
win_valid_popup(win_T *win UNUSED)
{
#ifdef FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	सचे (wp == win)
	    पटिनिवत्त TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	सचे (wp == win)
	    पटिनिवत्त TRUE;
#endif
    पटिनिवत्त FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    पूर्णाङ्क
win_valid(win_T *win)
{
    win_T	*wp;

    सचे (win == रिक्त)
	पटिनिवत्त FALSE;
    FOR_ALL_WINDOWS(wp)
	सचे (wp == win)
	    पटिनिवत्त TRUE;
    पटिनिवत्त win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_find_by_id(पूर्णाङ्क id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	सचे (wp->w_id == id)
	    पटिनिवत्त wp;
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	सचे (wp->w_id == id)
	    पटिनिवत्त wp;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	सचे (wp->w_id == id)
	    पटिनिवत्त wp;
#endif
    पटिनिवत्त रिक्त;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    पूर्णाङ्क
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    सचे (win == रिक्त)
	पटिनिवत्त FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    सचे (wp == win)
		पटिनिवत्त TRUE;
	}
#ifdef FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    सचे (wp == win)
		पटिनिवत्त TRUE;
#endif
    }
    पटिनिवत्त win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    पूर्णाङ्क
win_गणनम्(शून्य)
{
    win_T	*wp;
    पूर्णाङ्क		गणनम् = 0;

    FOR_ALL_WINDOWS(wp)
	++गणनम्;
    पटिनिवत्त गणनम्;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    पूर्णाङ्क
make_windows(
    पूर्णाङ्क		गणनम्,
    पूर्णाङ्क		vertical)  // split windows vertically if TRUE
{
    पूर्णाङ्क		maxcount;
    पूर्णाङ्क		todo;

    सचे (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_width + curwin->w_vsep_width
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    अञ्ञथा
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_HEIGHT(curwin) + curwin->w_status_height
			- (p_wh - p_wmh)) / (p_wmh + statusline_height(curwin));
    }

    सचे (maxcount < 2)
	maxcount = 2;
    सचे (गणनम् > maxcount)
	गणनम् = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    सचे (गणनम् > 1)
	last_status(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    // todo is number of windows left to create
    अत्थाय (todo = गणनम् - 1; todo > 0; --todo)
	सचे (vertical)
	{
	    सचे (win_split(curwin->w_width - (curwin->w_width - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		भङ्ग;
	}
	अञ्ञथा
	{
	    सचे (win_split(curwin->w_height - (curwin->w_height - todo
			    * statusline_height(curwin)) / (todo + 1)
			- statusline_height(curwin), WSP_ABOVE) == FAIL)
		भङ्ग;
	}

    unblock_autocmds();

    // return actual number of windows
    पटिनिवत्त (गणनम् - todo);
}

/*
 * Exchange current and next window
 */
    स्थिर शून्य
win_exchange(long Prenum)
{
    frame_T	*frp;
    frame_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    पूर्णाङ्क		temp;

    सचे (ERROR_IF_ANY_POPUP_WINDOW)
	पटिनिवत्त;
    सचे (ONE_WINDOW)	    // just one window
    {
	beep_flush();
	पटिनिवत्त;
    }
    सचे (text_or_buf_locked())
    {
	beep_flush();
	पटिनिवत्त;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * find window to exchange with
     */
    सचे (Prenum)
    {
	frp = curwin->w_frame->fr_parent->fr_child;
	याव (frp != रिक्त && --Prenum > 0)
	    frp = frp->fr_next;
    }
    अञ्ञथा सचे (curwin->w_frame->fr_next != रिक्त)	// Swap with next
	frp = curwin->w_frame->fr_next;
    अञ्ञथा    // Swap last window in row/col with previous
	frp = curwin->w_frame->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    सचे (frp == रिक्त || frp->fr_win == रिक्त || frp->fr_win == curwin)
	पटिनिवत्त;
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
    सचे (wp->w_prev != curwin)
    {
	win_remove(curwin, रिक्त);
	frame_remove(curwin->w_frame);
	win_append(wp->w_prev, curwin);
	frame_निवेशय(frp, curwin->w_frame);
    }
    सचे (wp != wp2)
    {
	win_remove(wp, रिक्त);
	frame_remove(wp->w_frame);
	win_append(wp2, wp);
	सचे (frp2 == रिक्त)
	    frame_निवेशय(wp->w_frame->fr_parent->fr_child, wp->w_frame);
	अञ्ञथा
	    frame_append(frp2, wp->w_frame);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    temp = curwin->w_status_height;
    सचे (temp != wp->w_status_height)
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
#सचे defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    सचे (wp->w_buffer != curbuf)
	reset_VIsual_and_resel();
    अञ्ञथा सचे (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    स्थिर शून्य
win_rotate(पूर्णाङ्क upwards, पूर्णाङ्क गणनम्)
{
    win_T	*wp1;
    win_T	*wp2;
    frame_T	*frp;
    पूर्णाङ्क		n;

    सचे (ONE_WINDOW)		// nothing to do
    {
	beep_flush();
	पटिनिवत्त;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Check if all frames in this row/col have one window.
    FOR_ALL_FRAMES(frp, curwin->w_frame->fr_parent->fr_child)
	सचे (frp->fr_win == रिक्त)
	{
	    emsg(_(e_cannot_rotate_when_another_window_is_split));
	    पटिनिवत्त;
	}

    याव (गणनम्--)
    {
	सचे (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_frame->fr_parent->fr_child;
	    wp1 = frp->fr_win;
	    win_remove(wp1, रिक्त);
	    frame_remove(frp);

	    // find last frame and append removed window/frame after it
	    अत्थाय ( ; frp->fr_next != रिक्त; frp = frp->fr_next)
		;
	    win_append(frp->fr_win, wp1);
	    frame_append(frp, wp1->w_frame);

	    wp2 = frp->fr_win;		// previously last window
	}
	अञ्ञथा			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    अत्थाय (frp = curwin->w_frame; frp->fr_next != रिक्त;
							   frp = frp->fr_next)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, रिक्त);
	    frame_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_parent->fr_child->fr_win->w_prev, wp1);
	    frame_निवेशय(frp->fr_parent->fr_child, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_status_height;
	सचे (n != wp1->w_status_height)
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
#सचे defined(FEAT_STL_OPT)
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
    पूर्णाङ्क
win_splitmove(win_T *wp, पूर्णाङ्क size, पूर्णाङ्क flags)
{
    पूर्णाङ्क		dir;
    पूर्णाङ्क		height = wp->w_height;
    frame_T	*unflat_altfr;

    सचे (ONE_WINDOW)
	पटिनिवत्त OK;	// nothing to do
    सचे (check_split_disallowed(wp) == FAIL)
	पटिनिवत्त FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, रिक्त, &unflat_altfr);
    win_remove(wp, रिक्त);
    last_status(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    सचे (win_split_ins(size, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	पटिनिवत्त FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    सचे (size == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(height, wp);
	सचे (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#सचे defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif
    पटिनिवत्त OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    शून्य
win_move_after(win_T *win1, win_T *win2)
{
    पूर्णाङ्क		height;

    // check if the arguments are reasonable
    सचे (win1 == win2)
	पटिनिवत्त;

    // check if there is something to do
    सचे (win2->w_next != win1)
    {
	सचे (win1->w_frame->fr_parent != win2->w_frame->fr_parent)
	{
	    iemsg("Trying to move a window into another frame");
	    पटिनिवत्त;
	}

	// may need to move the status line/vertical separator of the last
	// window
	सचे (win1 == lastwin)
	{
	    height = win1->w_prev->w_status_height;
	    win1->w_prev->w_status_height = win1->w_status_height;
	    win1->w_status_height = height;
	    सचे (win1->w_prev->w_vsep_width == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_width = 0;
		win1->w_prev->w_frame->fr_width -= 1;
		win1->w_vsep_width = 1;
		win1->w_frame->fr_width += 1;
	    }
	}
	अञ्ञथा सचे (win2 == lastwin)
	{
	    height = win1->w_status_height;
	    win1->w_status_height = win2->w_status_height;
	    win2->w_status_height = height;
	    सचे (win1->w_vsep_width == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_width = 1;
		win2->w_frame->fr_width += 1;
		win1->w_vsep_width = 0;
		win1->w_frame->fr_width -= 1;
	    }
	}
	win_remove(win1, रिक्त);
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
    शून्य
win_equal(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    पूर्णाङ्क		current,	// do only frame with current window
    पूर्णाङ्क		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    सचे (dir == 0)
	dir = *p_ead;
    win_equal_rec(next_curwin == रिक्त ? curwin : next_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_height(),
		      topframe->fr_width, topframe->fr_height);
#सचे defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
    सचे (!is_aucmd_win(next_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    स्थिर शून्य
win_equal_rec(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    पूर्णाङ्क		current,	// do only frame with current window
    frame_T	*topfr,		// frame to set size off
    पूर्णाङ्क		dir,		// 'v', 'h' or 'b', see win_equal()
    पूर्णाङ्क		col,		// horizontal position for frame
    पूर्णाङ्क		row,		// vertical position for frame
    पूर्णाङ्क		width,		// new width of frame
    पूर्णाङ्क		height)		// new height of frame
{
    पूर्णाङ्क		n, m;
    पूर्णाङ्क		extra_sep = 0;
    पूर्णाङ्क		wincount, totwincount = 0;
    frame_T	*fr;
    पूर्णाङ्क		next_curwin_size = 0;
    पूर्णाङ्क		room = 0;
    पूर्णाङ्क		new_size;
    पूर्णाङ्क		has_next_curwin = 0;
    पूर्णाङ्क		hnc;

    सचे (topfr->fr_layout == FR_LEAF)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	सचे (topfr->fr_height != height || topfr->fr_win->w_winrow != row
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
    अञ्ञथा सचे (topfr->fr_layout == FR_ROW)
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	सचे (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = frame_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    सचे (col + width >= firstwin->w_wincol + topframe->fr_width)
		extra_sep = 1;
	    अञ्ञथा
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
	    सचे (room < 0)
	    {
		next_curwin_size = p_wiw + room;
		room = 0;
	    }
	    अञ्ञथा
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    सचे (!frame_fixed_width(fr))
			अनुवर्त;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minwidth(fr, NOWIN);
		    new_size = fr->fr_width;
		    सचे (frame_has_win(fr, next_curwin))
		    {
			room += p_wiw - p_wmw;
			next_curwin_size = 0;
			सचे (new_size < p_wiw)
			    new_size = p_wiw;
		    }
		    अञ्ञथा
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_next == रिक्त
					? extra_sep : 0)) / (p_wmw + 1);
		    room -= new_size - n;
		    सचे (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newwidth = new_size;
		}
		सचे (next_curwin_size == -1)
		{
		    सचे (!has_next_curwin)
			next_curwin_size = 0;
		    अञ्ञथा सचे (totwincount > 1
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
		    अञ्ञथा
			next_curwin_size = p_wiw;
		}
	    }

	    सचे (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    सचे (fr->fr_next == रिक्त)
		// last frame gets all that remains (avoid roundoff error)
		new_size = width;
	    अञ्ञथा सचे (dir == 'v')
		new_size = fr->fr_width;
	    अञ्ञथा सचे (frame_fixed_width(fr))
	    {
		new_size = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    अञ्ञथा
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = frame_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_next == रिक्त ? extra_sep : 0))
								/ (p_wmw + 1);
		m = frame_minwidth(fr, next_curwin);
		सचे (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		अञ्ञथा
		    hnc = FALSE;
		सचे (hnc)	    // don't count next_curwin
		    --wincount;
		सचे (totwincount == 0)
		    new_size = room;
		अञ्ञथा
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		सचे (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wiw - (m - n);
		    सचे (next_curwin_size < 0)
			next_curwin_size = 0;
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		अञ्ञथा
		    room -= new_size;
		new_size += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    सचे (!current || dir != 'v' || topfr->fr_parent != रिक्त
		    || (new_size != fr->fr_width)
		    || frame_has_win(fr, next_curwin))
		win_equal_rec(next_curwin, current, fr, dir, col, row,
							    new_size, height);
	    col += new_size;
	    width -= new_size;
	    totwincount -= wincount;
	}
    }
    अञ्ञथा // topfr->fr_layout == FR_COL
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	सचे (dir != 'h')			// equalize frame heights
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
	    सचे (room < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		next_curwin_size = p_wh + room;
		room = 0;
	    }
	    अञ्ञथा
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    सचे (!frame_fixed_height(fr))
			अनुवर्त;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minheight(fr, NOWIN);
		    new_size = fr->fr_height;
		    सचे (frame_has_win(fr, next_curwin))
		    {
			room += p_wh - p_wmh;
			next_curwin_size = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			सचे (new_size < n + (p_wh - p_wmh))
			    new_size = n + (p_wh - p_wmh);
		    }
		    अञ्ञथा
			// These windows don't use up room.
			totwincount -= frame_wincount_in_height(fr);
		    room -= new_size - n;
		    सचे (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newheight = new_size;
		}
		सचे (next_curwin_size == -1)
		{
		    सचे (!has_next_curwin)
			next_curwin_size = 0;
		    अञ्ञथा सचे (totwincount > 1
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
		    अञ्ञथा
			next_curwin_size = p_wh;
		}
	    }

	    सचे (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    सचे (fr->fr_next == रिक्त)
		// last frame gets all that remains (avoid roundoff error)
		new_size = height;
	    अञ्ञथा सचे (dir == 'h')
		new_size = fr->fr_height;
	    अञ्ञथा सचे (frame_fixed_height(fr))
	    {
		new_size = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    अञ्ञथा
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = frame_minheight(fr, NOWIN);
		wincount = frame_wincount_in_height(fr);
		m = frame_minheight(fr, next_curwin);
		सचे (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		अञ्ञथा
		    hnc = FALSE;
		सचे (hnc)	    // don't count next_curwin
		    --wincount;
		सचे (totwincount == 0)
		    new_size = room;
		अञ्ञथा
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		सचे (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wh - (m - n);
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		अञ्ञथा
		    room -= new_size;
		new_size += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    सचे (!current || dir != 'h' || topfr->fr_parent != रिक्त
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

#सचे defined(FEAT_JOB_CHANNEL)
    शून्य
leaving_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    सचे (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	पटिनिवत्त;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_buffer->b_prompt_निवेशय = restart_edit;
    सचे (restart_edit != 0 && mode_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    सचे ((State & MODE_निवेशय) && !stop_निवेशय_mode)
    {
	stop_निवेशय_mode = TRUE;
	सचे (win->w_buffer->b_prompt_निवेशय == NUL)
	    win->w_buffer->b_prompt_निवेशय = 'A';
    }
}

    शून्य
entering_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    सचे (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	पटिनिवत्त;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    सचे (win->w_buffer->b_prompt_निवेशय != NUL)
	stop_निवेशय_mode = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    सचे ((State & MODE_निवेशय) == 0)
	restart_edit = win->w_buffer->b_prompt_निवेशय;
}
#endif

    स्थिर शून्य
win_init_empty(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_पङ्क्तयः_valid = 0;
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
#सचे defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
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
    शून्य
curwin_init(शून्य)
{
    win_init_empty(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    शून्य
close_windows(
    buf_T	*buf,
    पूर्णाङ्क		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    पूर्णाङ्क		गणनम् = tabpage_index(रिक्त);

    ++RedrawingDisabled;

    अत्थाय (wp = firstwin; wp != रिक्त && !ONE_WINDOW; )
    {
	सचे (wp->w_buffer == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_buffer->b_locked > 0))
	{
	    सचे (window_layout_locked(CMD_SIZE))
		गच्छ theend;  // Only give one error message.
	    सचे (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		भङ्ग;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	अञ्ञथा
	    wp = wp->w_next;
    }

    // Also check windows in other tab pages.
    अत्थाय (tp = first_tabpage; tp != रिक्त; tp = nexttp)
    {
	nexttp = tp->tp_next;
	सचे (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		सचे (wp->w_buffer == buf
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0))
		{
		    सचे (window_layout_locked(CMD_SIZE))
			गच्छ theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = first_tabpage;
		    भङ्ग;
		}
    }

theend:
    सचे (RedrawingDisabled > 0)
	--RedrawingDisabled;

    सचे (गणनम् != tabpage_index(रिक्त))
	apply_autocmds(EVENT_TABCLOSED, रिक्त, रिक्त, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    पूर्णाङ्क
last_window(शून्य)
{
    पटिनिवत्त (one_window() && first_tabpage->tp_next == रिक्त);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    पूर्णाङ्क
one_window(शून्य)
{
    win_T	*wp;
    पूर्णाङ्क		seen_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	सचे (!is_aucmd_win(wp))
	{
	    सचे (seen_one)
		पटिनिवत्त FALSE;
	    seen_one = TRUE;
	}
    }
    पटिनिवत्त TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    स्थिर पूर्णाङ्क
close_last_window_tabpage(
    win_T	*win,
    पूर्णाङ्क		free_buf,
    tabpage_T   *prev_curtab)
{
    सचे (!ONE_WINDOW)
	पटिनिवत्त FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_buffer != रिक्त);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    सचे (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    सचे (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, रिक्त, रिक्त, FALSE, curbuf);
#सचे defined(FEAT_TABPANEL)
    सचे (p_stpl > 0)
	shell_new_columns();
#endif
    apply_autocmds(EVENT_WINENTER, रिक्त, रिक्त, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, रिक्त, रिक्त, FALSE, curbuf);
    सचे (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
    पटिनिवत्त TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    स्थिर शून्य
win_close_buffer(win_T *win, पूर्णाङ्क action, पूर्णाङ्क abort_if_last)
{
#ifdef FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    सचे (win->w_buffer != रिक्त)
	reset_synblock(win);
#endif

#ifdef FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    सचे (win->w_buffer != रिक्त && bt_quickfix(win->w_buffer)
					&& win->w_buffer->b_nwindows == 1)
	win->w_buffer->b_p_bl = FALSE;
#endif

    // Close the link to the buffer.
    सचे (win->w_buffer != रिक्त)
    {
	bufref_T    bufref;

	set_bufref(&bufref, curbuf);
	++win->w_locked;
	close_buffer(win, win->w_buffer, action, abort_if_last, TRUE, TRUE);
	सचे (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	सचे (!bufref_valid(&bufref))
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
    पूर्णाङ्क
win_close(win_T *win, पूर्णाङ्क free_buf)
{
    win_T	*wp;
    पूर्णाङ्क		other_buffer = FALSE;
    पूर्णाङ्क		close_curwin = FALSE;
    पूर्णाङ्क		dir;
    पूर्णाङ्क		help_window = FALSE;
    पूर्णाङ्क		quickfix_window = FALSE;
    tabpage_T   *prev_curtab = curtab;
    frame_T	*win_frame = win->w_frame->fr_parent;
#ifdef FEAT_DIFF
    पूर्णाङ्क		had_diffmode = win->w_p_diff;
#endif
#ifdef MESSAGE_QUEUE
    पूर्णाङ्क		did_decrement = FALSE;
#endif

#सचे defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    सचे (may_close_term_popup() == OK)
	पटिनिवत्त OK;
#endif
    सचे (ERROR_IF_ANY_POPUP_WINDOW)
	पटिनिवत्त FAIL;

    सचे (last_window())
    {
	emsg(_(e_cannot_close_last_window));
	पटिनिवत्त FAIL;
    }
    सचे (window_layout_locked(CMD_close))
	पटिनिवत्त FAIL;

    सचे (win_locked(win) || (win->w_buffer != रिक्त
					       && win->w_buffer->b_locked > 0))
	पटिनिवत्त FAIL; // window is already being closed
    सचे (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	पटिनिवत्त FAIL;
    }
    सचे ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_window())
    {
	emsg(_(e_cannot_close_window_only_autocmd_window_would_remain));
	पटिनिवत्त FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    सचे (close_last_window_tabpage(win, free_buf, prev_curtab))
	पटिनिवत्त FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    सचे (bt_help(win->w_buffer))
	help_window = TRUE;
    अञ्ञथा
	clear_snapshot(curtab, SNAP_HELP_IDX);

    सचे (bt_quickfix(win->w_buffer))
	quickfix_window = TRUE;
    अञ्ञथा
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    सचे (win == curwin)
    {
#ifdef FEAT_JOB_CHANNEL
	leaving_window(curwin);
#endif
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, रिक्त));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	सचे (wp->w_buffer != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_buffer = TRUE;
	    सचे (!win_valid(win))
		पटिनिवत्त FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	    सचे (!win_valid(win))
		पटिनिवत्त FAIL;
	    --win->w_locked;
	    सचे (last_window())
		पटिनिवत्त FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	सचे (!win_valid(win))
	    पटिनिवत्त FAIL;
	--win->w_locked;
	सचे (last_window())
	    पटिनिवत्त FAIL;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	सचे (aborting())
	    पटिनिवत्त FAIL;
#endif
    }

#ifdef FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    सचे (gui.in_use)
	out_flush();
#endif

#ifdef FEAT_PROP_POPUP
    सचे (popup_win_closed(win) && !win_valid(win))
	पटिनिवत्त FAIL;
#endif

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    सचे (!win_valid_any_tab(win))
	पटिनिवत्त OK;

    win_close_buffer(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    सचे (win_valid(win) && win->w_buffer == रिक्त
#सचे defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#endif
	    && last_window())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	सचे (curwin->w_buffer == रिक्त)
	    curwin->w_buffer = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    सचे (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_buffer == रिक्त)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	पटिनिवत्त FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    सचे (!win_valid(win) || last_window()
	    || close_last_window_tabpage(win, free_buf, prev_curtab))
	पटिनिवत्त FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++split_disallowed;
#ifdef MESSAGE_QUEUE
    ++dont_parse_messages;
#endif

    सचे (win->w_buffer != रिक्त)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, रिक्त);

    सचे (help_window || quickfix_window)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	सचे (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    सचे (win == curwin)
    {
	curwin = wp;
#ifdef FEAT_QUICKFIX
	सचे (wp->w_p_pvw || bt_quickfix(wp->w_buffer))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    अत्थाय (;;)
	    {
		सचे (wp->w_next == रिक्त)
		    wp = firstwin;
		अञ्ञथा
		    wp = wp->w_next;
		सचे (wp == curwin)
		    भङ्ग;
		सचे (!wp->w_p_pvw && !bt_quickfix(wp->w_buffer))
		{
		    curwin = wp;
		    भङ्ग;
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

    सचे (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_frame->fr_parent == win_frame, dir);
    अञ्ञथा
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    सचे (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ifdef MESSAGE_QUEUE
	did_decrement =
#अञ्ञथा
	(शून्य)
#endif
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	सचे (other_buffer)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
    }

    सचे (ONE_WINDOW && curwin->w_locked && curbuf->b_locked_split
	    && first_tabpage->tp_next != रिक्त)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, रिक्त, रिक्त, FALSE, curbuf);

    --split_disallowed;
#ifdef MESSAGE_QUEUE
    सचे (!did_decrement)
	--dont_parse_messages;
#endif

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    सचे (help_window || quickfix_window)
	restore_snapshot(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ifdef FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    सचे (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	पूर्णाङ्क	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    सचे (dwin->w_p_diff)
		++diffcount;
	सचे (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#endif

#सचे defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    सचे (gui.in_use && !win_hasvertsplit())
	gui_init_which_components(रिक्त);
#endif

    redraw_all_later(UPD_NOT_VALID);
    पटिनिवत्त OK;
}

    स्थिर शून्य
trigger_winnewpre(शून्य)
{
    window_layout_lock();
    apply_autocmds(EVENT_WINNEWPRE, रिक्त, रिक्त, FALSE, रिक्त);
    window_layout_unlock();
}

    स्थिर शून्य
trigger_winclosed(win_T *win)
{
    स्थिर पूर्णाङ्क	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    सचे (recursive)
	पटिनिवत्त;
    recursive = TRUE;
    vim_snprintf((वर्ण *)winid, परिमाण(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_buffer);
    recursive = FALSE;
}

    शून्य
trigger_tabclosedpre(tabpage_T *tp)
{
    स्थिर पूर्णाङ्क	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    सचे (!has_tabclosedpre() || recursive)
	पटिनिवत्त;

    सचे (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    window_layout_lock();
    apply_autocmds(EVENT_TABCLOSEDPRE, रिक्त, रिक्त, FALSE, रिक्त);
    window_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    सचे (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    अञ्ञथा
	// fall back to the first tabpage
	goto_tabpage_tp(first_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    शून्य
snapshot_windows_scroll_size(शून्य)
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

स्थिर पूर्णाङ्क did_initial_scroll_size_snapshot = FALSE;

    शून्य
may_make_initial_scroll_size_snapshot(शून्य)
{
    सचे (!did_initial_scroll_size_snapshot)
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
    स्थिर dict_T *
make_win_info_dict(
	पूर्णाङ्क width,
	पूर्णाङ्क height,
	पूर्णाङ्क topline,
# ifdef FEAT_DIFF
	पूर्णाङ्क topfill,
# endif
	पूर्णाङ्क leftcol,
	पूर्णाङ्क skipcol)
{
    dict_T *d = dict_alloc();
    सचे (d == रिक्त)
	पटिनिवत्त रिक्त;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    याव (1)
    {
	typval_T tv;
	tv.v_lock = 0;
	tv.v_type = VAR_सङ्ख्या;

	tv.vval.v_सङ्ख्या = width;
	सचे (dict_add_tv(d, "width", &tv) == FAIL)
	    भङ्ग;
	tv.vval.v_सङ्ख्या = height;
	सचे (dict_add_tv(d, "height", &tv) == FAIL)
	    भङ्ग;
	tv.vval.v_सङ्ख्या = topline;
	सचे (dict_add_tv(d, "topline", &tv) == FAIL)
	    भङ्ग;
# ifdef FEAT_DIFF
	tv.vval.v_सङ्ख्या = topfill;
# अञ्ञथा
	tv.vval.v_सङ्ख्या = 0;
# endif
	सचे (dict_add_tv(d, "topfill", &tv) == FAIL)
	    भङ्ग;
	tv.vval.v_सङ्ख्या = leftcol;
	सचे (dict_add_tv(d, "leftcol", &tv) == FAIL)
	    भङ्ग;
	tv.vval.v_सङ्ख्या = skipcol;
	सचे (dict_add_tv(d, "skipcol", &tv) == FAIL)
	    भङ्ग;
	पटिनिवत्त d;
    }
    dict_unref(d);
    पटिनिवत्त रिक्त;
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
    स्थिर शून्य
check_window_scroll_resize(
	पूर्णाङ्क	*size_गणनम्,
	win_T	**first_scroll_win,
	win_T	**first_size_win,
	list_T	*winlist UNUSED,
	dict_T	*v_event UNUSED)
{
#ifdef FEAT_EVAL
    पूर्णाङ्क listidx = 0;
    पूर्णाङ्क tot_width = 0;
    पूर्णाङ्क tot_height = 0;
    पूर्णाङ्क tot_topline = 0;
# ifdef FEAT_DIFF
    पूर्णाङ्क tot_topfill = 0;
# endif
    पूर्णाङ्क tot_leftcol = 0;
    पूर्णाङ्क tot_skipcol = 0;
#endif

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	पूर्णाङ्क ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	पूर्णाङ्क size_changed = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_last_width != wp->w_width
			    || wp->w_last_height != wp->w_height);
	सचे (size_changed)
	{
#ifdef FEAT_EVAL
	    सचे (winlist != रिक्त)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_lock = 0;
		tv.v_type = VAR_सङ्ख्या;
		tv.vval.v_सङ्ख्या = wp->w_id;
		list_set_item(winlist, listidx++, &tv);
	    }
	    अञ्ञथा
#endif
		सचे (size_गणनम् != रिक्त)
	    {
		++*size_गणनम्;
		सचे (*first_size_win == रिक्त)
		    *first_size_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		सचे (*first_scroll_win == रिक्त && !ignore_scroll)
		    *first_scroll_win = wp;
	    }
	}

	पूर्णाङ्क scroll_changed = !ignore_scroll
				&& (wp->w_last_topline != wp->w_topline
#ifdef FEAT_DIFF
				|| wp->w_last_topfill != wp->w_topfill
#endif
				|| wp->w_last_leftcol != wp->w_leftcol
				|| wp->w_last_skipcol != wp->w_skipcol);
	सचे (scroll_changed
	    && first_scroll_win != रिक्त && *first_scroll_win == रिक्त)
	    *first_scroll_win = wp;

#ifdef FEAT_EVAL
	सचे ((size_changed || scroll_changed) && v_event != रिक्त)
	{
	    // Add info about this window to the v:event dictionary.
	    पूर्णाङ्क width = wp->w_width - wp->w_last_width;
	    पूर्णाङ्क height = wp->w_height - wp->w_last_height;
	    पूर्णाङ्क topline = wp->w_topline - wp->w_last_topline;
# ifdef FEAT_DIFF
	    पूर्णाङ्क topfill = wp->w_topfill - wp->w_last_topfill;
# endif
	    पूर्णाङ्क leftcol = wp->w_leftcol - wp->w_last_leftcol;
	    पूर्णाङ्क skipcol = wp->w_skipcol - wp->w_last_skipcol;
	    dict_T *d = make_win_info_dict(width, height, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    सचे (d == रिक्त)
		भङ्ग;
	    वर्ण winid[NUMBUFLEN];
	    vim_snprintf(winid, परिमाण(winid), "%d", wp->w_id);
	    सचे (dict_add_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		भङ्ग;
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
    सचे (v_event != रिक्त)
    {
	dict_T *alldict = make_win_info_dict(tot_width, tot_height, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	सचे (alldict != रिक्त)
	{
	    सचे (dict_add_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    अञ्ञथा
		--alldict->dv_refcount;
	}
    }
#endif
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    शून्य
may_trigger_win_scrolled_resized(शून्य)
{
    स्थिर पूर्णाङ्क	    recursive = FALSE;
    पूर्णाङ्क		    do_resize = has_winresized();
    पूर्णाङ्क		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    सचे (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_size_snapshot)
	पटिनिवत्त;

    पूर्णाङ्क size_गणनम् = 0;
    win_T *first_scroll_win = रिक्त, *first_size_win = रिक्त;
    check_window_scroll_resize(&size_गणनम्, &first_scroll_win, &first_size_win,
								    रिक्त, रिक्त);
    पूर्णाङ्क trigger_resize = do_resize && size_गणनम् > 0;
    पूर्णाङ्क trigger_scroll = do_scroll && first_scroll_win != रिक्त;
    सचे (!trigger_resize && !trigger_scroll)
	पटिनिवत्त;  // no relevant changes
#ifdef FEAT_EVAL
    list_T *windows_list = रिक्त;
    सचे (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_वस्तूनि(size_गणनम्);
	सचे (windows_list != रिक्त)
	    check_window_scroll_resize(रिक्त, रिक्त, रिक्त, windows_list, रिक्त);
    }

    dict_T *scroll_dict = रिक्त;
    सचे (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	सचे (scroll_dict != रिक्त)
	{
	    scroll_dict->dv_refcount = 1;
	    check_window_scroll_resize(रिक्त, रिक्त, रिक्त, रिक्त, scroll_dict);
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
    सचे (trigger_resize
#ifdef FEAT_EVAL
	    && windows_list != रिक्त
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	सचे (dict_add_list(v_event, "windows", windows_list) == OK)
	{
	    dict_set_वस्तूनि_ro(v_event);
#endif
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((वर्ण *)winid, परिमाण(winid), "%d",
							 first_size_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     first_size_win->w_buffer);
#ifdef FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#endif
    }

    सचे (trigger_scroll
#ifdef FEAT_EVAL
	    && scroll_dict != रिक्त
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", रिक्त);
	dict_set_वस्तूनि_ro(v_event);
	dict_unref(scroll_dict);
#endif
	char_u winid[NUMBUFLEN];
	vim_snprintf((वर्ण *)winid, परिमाण(winid), "%d",
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
    शून्य
win_close_othertab(win_T *win, पूर्णाङ्क free_buf, tabpage_T *tp)
{
    win_T	*wp;
    पूर्णाङ्क		dir;
    tabpage_T   *ptp = रिक्त;
    पूर्णाङ्क		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    सचे (window_layout_locked(CMD_SIZE))
	पटिनिवत्त;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    सचे (win_locked(win) || (win->w_buffer != रिक्त
					       && win->w_buffer->b_locked > 0))
	पटिनिवत्त; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    सचे (win->w_buffer != रिक्त)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	सचे (!win_valid_any_tab(win))
	    पटिनिवत्त;
    }

    सचे (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	सचे (!win_valid_any_tab(win))
	    पटिनिवत्त;
    }

    सचे (win->w_buffer != रिक्त)
	// Close the link to the buffer.
	close_buffer(win, win->w_buffer, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    अत्थाय (ptp = first_tabpage; ptp != रिक्त && ptp != tp; ptp = ptp->tp_next)
	;
    सचे (ptp == रिक्त || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	सचे (win_valid_any_tab(win) && win->w_buffer == रिक्त)
	{
	    win->w_buffer = firstbuf;
	    ++firstbuf->b_nwindows;
	    सचे (win == curwin)
		curbuf = curwin->w_buffer;
	    win_init_empty(win);
	}
	पटिनिवत्त;
    }

    // Autocommands may have closed the window already.
    अत्थाय (wp = tp->tp_firstwin; wp != रिक्त && wp != win; wp = wp->w_next)
	;
    सचे (wp == रिक्त)
	पटिनिवत्त;

    // When closing the last window in a tab page remove the tab page.
    सचे (tp->tp_firstwin == tp->tp_lastwin)
    {
	पूर्णाङ्क	h = tabline_height();

	सचे (tp == first_tabpage)
	    first_tabpage = tp->tp_next;
	अञ्ञथा
	{
	    अत्थाय (ptp = first_tabpage; ptp != रिक्त && ptp->tp_next != tp;
							   ptp = ptp->tp_next)
		;
	    सचे (ptp == रिक्त)
	    {
		internal_error("win_close_othertab()");
		पटिनिवत्त;
	    }
	    ptp->tp_next = tp->tp_next;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	सचे (h != tabline_height())
	    shell_new_rows();
    }

    सचे (win->w_buffer != रिक्त)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    सचे (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    स्थिर win_T *
win_free_mem(
    win_T	*win,
    पूर्णाङ्क		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == रिक्त ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_frame;
    wp = winframe_remove(win, dirp, tp, रिक्त);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    सचे (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    पटिनिवत्त wp;
}

#सचे defined(EXITFREE)
    शून्य
win_free_all(शून्य)
{
    पूर्णाङ्क		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = रिक्त;
    cmdwin_win = रिक्त;

    याव (first_tabpage->tp_next != रिक्त)
	tabpage_close(TRUE);

    अत्थाय (पूर्णाङ्क i = 0; i < AUCMD_WIN_गणनम्; ++i)
	सचे (aucmd_win[i].auc_win != रिक्त)
	{
	    (शून्य)win_free_mem(aucmd_win[i].auc_win, &dummy, रिक्त);
	    aucmd_win[i].auc_win = रिक्त;
	}

    याव (firstwin != रिक्त)
	(शून्य)win_free_mem(firstwin, &dummy, रिक्त);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = रिक्त;
}
#endif

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    पूर्णाङ्क		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    frame_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    frame_T	*frp, *frp2, *frp3;
    frame_T	*frp_close = win->w_frame;
    win_T	*wp;
    पूर्णाङ्क		row, col;

    /*
     * If there is only one window there is nothing to remove.
     */
    सचे (tp == रिक्त ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	पटिनिवत्त रिक्त;

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
    सचे (win->w_vsep_width == 0 && frp_close->fr_parent->fr_layout == FR_ROW &&
	    frp_close->fr_prev != रिक्त)
	frame_set_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    frame_remove(frp_close);

    सचे (frp_close->fr_parent->fr_layout == FR_COL)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	सचे (frp2->fr_win != रिक्त && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    याव (frp != रिक्त || frp3 != रिक्त)
	    {
		सचे (frp != रिक्त)
		{
		    सचे (!frame_fixed_height(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			भङ्ग;
		    }
		    frp = frp->fr_prev;
		}
		सचे (frp3 != रिक्त)
		{
		    सचे (frp3->fr_win != रिक्त && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			भङ्ग;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_height(frp2, frp2->fr_height + frp_close->fr_height,
			    frp2 == frp_close->fr_next, !frame_fixed_height(frp2), FALSE);
	*dirp = 'v';
    }
    अञ्ञथा
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	सचे (frp2->fr_win != रिक्त && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    याव (frp != रिक्त || frp3 != रिक्त)
	    {
		सचे (frp != रिक्त)
		{
		    सचे (!frame_fixed_width(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			भङ्ग;
		    }
		    frp = frp->fr_prev;
		}
		सचे (frp3 != रिक्त)
		{
		    सचे (frp3->fr_win != रिक्त && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			भङ्ग;
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
    सचे (frp2 != frp_close->fr_prev)
	frame_comp_pos(frp_close->fr_parent, &row, &col);

    सचे (unflat_altfr == रिक्त)
	frame_flatten(frp2);
    अञ्ञथा
	*unflat_altfr = frp2;

    frame_locked--;

    पटिनिवत्त wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    स्थिर शून्य
frame_flatten(frame_T *frp)
{
    frame_T	*frp2, *frp3;

    सचे (frp->fr_next != रिक्त || frp->fr_prev != रिक्त)
	पटिनिवत्त;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_parent->fr_layout = frp->fr_layout;
    frp->fr_parent->fr_child = frp->fr_child;
    FOR_ALL_FRAMES(frp2, frp->fr_child)
	frp2->fr_parent = frp->fr_parent;
    frp->fr_parent->fr_win = frp->fr_win;
    सचे (frp->fr_win != रिक्त)
	frp->fr_win->w_frame = frp->fr_parent;
    frp2 = frp->fr_parent;
    सचे (topframe->fr_child == frp)
	topframe->fr_child = frp2;
    vim_free(frp);

    frp = frp2->fr_parent;
    सचे (frp != रिक्त && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	सचे (frp->fr_child == frp2)
	    frp->fr_child = frp2->fr_child;
	frp2->fr_child->fr_prev = frp2->fr_prev;
	सचे (frp2->fr_prev != रिक्त)
	    frp2->fr_prev->fr_next = frp2->fr_child;
	अत्थाय (frp3 = frp2->fr_child; ; frp3 = frp3->fr_next)
	{
	    frp3->fr_parent = frp;
	    सचे (frp3->fr_next == रिक्त)
	    {
		frp3->fr_next = frp2->fr_next;
		सचे (frp2->fr_next != रिक्त)
		    frp2->fr_next->fr_prev = frp3;
		भङ्ग;
	    }
	}
	सचे (topframe->fr_child == frp2)
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
    स्थिर शून्य
winframe_restore(win_T *wp, पूर्णाङ्क dir, frame_T *unflat_altfr)
{
    frame_T	*frp = wp->w_frame;

    // Put "wp"'s frame back where it was.
    सचे (frp->fr_prev != रिक्त)
	frame_append(frp->fr_prev, frp);
    अञ्ञथा
	frame_निवेशय(frp->fr_next, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    सचे (wp->w_vsep_width == 0
	    && frp->fr_parent->fr_layout == FR_ROW && frp->fr_prev != रिक्त)
	frame_set_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    सचे (wp->w_status_height == 0
	    && frp->fr_parent->fr_layout == FR_COL && frp->fr_prev != रिक्त)
	frame_add_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    सचे (dir == 'v')
    {
	frame_new_height(unflat_altfr, unflat_altfr->fr_height - frp->fr_height,
		unflat_altfr == frp->fr_next, FALSE, FALSE);
    }
    अञ्ञथा सचे (dir == 'h')
    {
	frame_new_width(unflat_altfr, unflat_altfr->fr_width - frp->fr_width,
		unflat_altfr == frp->fr_next, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    सचे (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_parent);
	पूर्णाङ्क	row = topleft->w_winrow;
	पूर्णाङ्क	col = topleft->w_wincol;

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
    स्थिर frame_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    frame_T	*other_fr, *target_fr;

    सचे (tp == रिक्त ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	पटिनिवत्त alt_tabpage()->tp_curwin->w_frame;

    frp = win->w_frame;

    सचे (frp->fr_prev == रिक्त)
	पटिनिवत्त frp->fr_next;
    सचे (frp->fr_next == रिक्त)
	पटिनिवत्त frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    // window
    target_fr = frp->fr_next;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    सचे (frp->fr_parent != रिक्त && frp->fr_parent->fr_layout == FR_COL && p_sb)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    सचे (frp->fr_parent != रिक्त && frp->fr_parent->fr_layout == FR_ROW && p_spr)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    सचे (frp->fr_parent != रिक्त && frp->fr_parent->fr_layout == FR_ROW)
    {
	सचे (frame_fixed_width(target_fr) && !frame_fixed_width(other_fr))
	    target_fr = other_fr;
    }
    अञ्ञथा
    {
	सचे (frame_fixed_height(target_fr) && !frame_fixed_height(other_fr))
	    target_fr = other_fr;
    }

    पटिनिवत्त target_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    स्थिर tabpage_T *
alt_tabpage(शून्य)
{
    tabpage_T	*tp = रिक्त;
    पूर्णाङ्क		forward;

    // Use the last accessed tab page, if possible.
    सचे ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	पटिनिवत्त lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_next != रिक्त &&
	    ((tcl_flags & TCL_वामः) == 0 || curtab == first_tabpage);

    सचे (forward)
	tp = curtab->tp_next;
    अञ्ञथा
	// Use the previous tab page.
	अत्थाय (tp = first_tabpage; tp->tp_next != curtab; tp = tp->tp_next)
	    ;

    पटिनिवत्त tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    स्थिर win_T *
frame2win(frame_T *frp)
{
    याव (frp->fr_win == रिक्त)
	frp = frp->fr_child;
    पटिनिवत्त frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    स्थिर पूर्णाङ्क
frame_has_win(frame_T *frp, win_T *wp)
{
    frame_T	*p;

    सचे (frp->fr_layout == FR_LEAF)
	पटिनिवत्त frp->fr_win == wp;

    FOR_ALL_FRAMES(p, frp->fr_child)
	सचे (frame_has_win(p, wp))
	    पटिनिवत्त TRUE;
    पटिनिवत्त FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
स्थिर पूर्णाङ्क min_set_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    स्थिर शून्य
frame_new_height(
    frame_T	*topfrp,
    पूर्णाङ्क		height,
    पूर्णाङ्क		topfirst,	// resize topmost contained frame first
    पूर्णाङ्क		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    पूर्णाङ्क		set_ch)		// set 'cmdheight' to resize topframe
{
    frame_T	*frp;
    पूर्णाङ्क		extra_पङ्क्तयः;
    पूर्णाङ्क		h;

    सचे (topfrp->fr_parent == रिक्त && set_ch)
    {
	// topframe: update the command line height, with side effects.
	पूर्णाङ्क new_ch = MAX(min_set_ch, p_ch + topfrp->fr_height - height);
	पूर्णाङ्क save_ch = min_set_ch;
	सचे (new_ch != p_ch)
	    set_option_मानम्((char_u *)"cmdheight", new_ch, रिक्त, 0);
	min_set_ch = save_ch;
	height = MIN(height, ROWS_AVAIL);
    }
    सचे (topfrp->fr_win != रिक्त)
    {
	// Simple case: just one window.
	win_new_height(topfrp->fr_win,
				    height - topfrp->fr_win->w_status_height
					      - WINBAR_HEIGHT(topfrp->fr_win));
    }
    अञ्ञथा सचे (topfrp->fr_layout == FR_ROW)
    {
	करोहि
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_height(frp, height, topfirst, wfh, set_ch);
		सचे (frp->fr_height > height)
		{
		    // Could not fit the windows, make the whole row higher.
		    height = frp->fr_height;
		    भङ्ग;
		}
	    }
	}
	याव (frp != रिक्त);
    }
    अञ्ञथा    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_child;
	सचे (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    याव (frame_fixed_height(frp))
	    {
		frp = frp->fr_next;
		सचे (frp == रिक्त)
		    पटिनिवत्त;	    // no frame without 'wfh', give up
	    }
	सचे (!topfirst)
	{
	    // Find the bottom frame of this column
	    याव (frp->fr_next != रिक्त)
		frp = frp->fr_next;
	    सचे (wfh)
		// Advance back for frames with one window with 'wfh' set.
		याव (frame_fixed_height(frp))
		    frp = frp->fr_prev;
	}

	extra_पङ्क्तयः = height - topfrp->fr_height;
	सचे (extra_पङ्क्तयः < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    याव (frp != रिक्त)
	    {
		h = frame_minheight(frp, रिक्त);
		सचे (frp->fr_height + extra_पङ्क्तयः < h)
		{
		    extra_पङ्क्तयः += frp->fr_height - h;
		    frame_new_height(frp, h, topfirst, wfh, set_ch);
		}
		अञ्ञथा
		{
		    frame_new_height(frp, frp->fr_height + extra_पङ्क्तयः,
							topfirst, wfh, set_ch);
		    भङ्ग;
		}
		सचे (topfirst)
		{
		    करोहि
			frp = frp->fr_next;
		    याव (wfh && frp != रिक्त && frame_fixed_height(frp));
		}
		अञ्ञथा
		{
		    करोहि
			frp = frp->fr_prev;
		    याव (wfh && frp != रिक्त && frame_fixed_height(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		सचे (frp == रिक्त)
		    height -= extra_पङ्क्तयः;
	    }
	}
	अञ्ञथा सचे (extra_पङ्क्तयः > 0)
	{
	    // increase height of bottom or top frame
	    frame_new_height(frp, frp->fr_height + extra_पङ्क्तयः, topfirst, wfh,
									set_ch);
	}
    }
    topfrp->fr_height = height;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    स्थिर पूर्णाङ्क
frame_fixed_height(frame_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    सचे (frp->fr_win != रिक्त)
	पटिनिवत्त frp->fr_win->w_p_wfh;

    सचे (frp->fr_layout == FR_ROW)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    सचे (frame_fixed_height(frp))
		पटिनिवत्त TRUE;
	पटिनिवत्त FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	सचे (!frame_fixed_height(frp))
	    पटिनिवत्त FALSE;
    पटिनिवत्त TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    स्थिर पूर्णाङ्क
frame_fixed_width(frame_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    सचे (frp->fr_win != रिक्त)
	पटिनिवत्त frp->fr_win->w_p_wfw;

    सचे (frp->fr_layout == FR_COL)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    सचे (frame_fixed_width(frp))
		पटिनिवत्त TRUE;
	पटिनिवत्त FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	सचे (!frame_fixed_width(frp))
	    पटिनिवत्त FALSE;
    पटिनिवत्त TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    स्थिर शून्य
frame_add_statusline(frame_T *frp)
{
    win_T	*wp;

    सचे (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	wp->w_status_height = statusline_height(wp);
    }
    अञ्ञथा सचे (frp->fr_layout == FR_ROW)
    {
	// Handle all the frames in the row.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_statusline(frp);
    }
    अञ्ञथा // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	अत्थाय (frp = frp->fr_child; frp->fr_next != रिक्त; frp = frp->fr_next)
	    ;
	frame_add_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    स्थिर शून्य
frame_new_width(
    frame_T	*topfrp,
    पूर्णाङ्क		width,
    पूर्णाङ्क		leftfirst,	// resize leftmost contained frame first
    पूर्णाङ्क		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    frame_T	*frp;
    पूर्णाङ्क		extra_cols;
    पूर्णाङ्क		w;
    win_T	*wp;

    सचे (topfrp->fr_layout == FR_LEAF)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	अत्थाय (frp = topfrp; frp->fr_parent != रिक्त; frp = frp->fr_parent)
	    सचे (frp->fr_parent->fr_layout == FR_ROW && frp->fr_next != रिक्त)
		भङ्ग;
	सचे (frp->fr_parent == रिक्त)
	    wp->w_vsep_width = 0;
	win_new_width(wp, width - wp->w_vsep_width);
    }
    अञ्ञथा सचे (topfrp->fr_layout == FR_COL)
    {
	करोहि
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_width(frp, width, leftfirst, wfw);
		सचे (frp->fr_width > width)
		{
		    // Could not fit the windows, make whole column wider.
		    width = frp->fr_width;
		    भङ्ग;
		}
	    }
	} याव (frp != रिक्त);
    }
    अञ्ञथा    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_child;
	सचे (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    याव (frame_fixed_width(frp))
	    {
		frp = frp->fr_next;
		सचे (frp == रिक्त)
		    पटिनिवत्त;	    // no frame without 'wfw', give up
	    }
	सचे (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    याव (frp->fr_next != रिक्त)
		frp = frp->fr_next;
	    सचे (wfw)
		// Advance back for frames with one window with 'wfw' set.
		याव (frame_fixed_width(frp))
		    frp = frp->fr_prev;
	}

	extra_cols = width - topfrp->fr_width;
	सचे (extra_cols < 0)
	{
	    // reduce frame width, rightmost frame first
	    याव (frp != रिक्त)
	    {
		w = frame_minwidth(frp, रिक्त);
		सचे (frp->fr_width + extra_cols < w)
		{
		    extra_cols += frp->fr_width - w;
		    frame_new_width(frp, w, leftfirst, wfw);
		}
		अञ्ञथा
		{
		    frame_new_width(frp, frp->fr_width + extra_cols,
							      leftfirst, wfw);
		    भङ्ग;
		}
		सचे (leftfirst)
		{
		    करोहि
			frp = frp->fr_next;
		    याव (wfw && frp != रिक्त && frame_fixed_width(frp));
		}
		अञ्ञथा
		{
		    करोहि
			frp = frp->fr_prev;
		    याव (wfw && frp != रिक्त && frame_fixed_width(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		सचे (frp == रिक्त)
		    width -= extra_cols;
	    }
	}
	अञ्ञथा सचे (extra_cols > 0)
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
    स्थिर शून्य
frame_set_vsep(frame_T *frp, पूर्णाङ्क add)
{
    win_T	*wp;

    सचे (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	सचे (add && wp->w_vsep_width == 0)
	{
	    सचे (wp->w_width > 0)	// don't make it negative
		win_new_width(wp, wp->w_width - 1);
	    wp->w_vsep_width = 1;
	}
	अञ्ञथा सचे (!add && wp->w_vsep_width == 1)
	{
	    win_new_width(wp, wp->w_width + 1);
	    wp->w_vsep_width = 0;
	}
    }
    अञ्ञथा सचे (frp->fr_layout == FR_COL)
    {
	// Handle all the frames in the column.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_set_vsep(frp, add);
    }
    अञ्ञथा // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_child;
	याव (frp->fr_next != रिक्त)
	    frp = frp->fr_next;
	frame_set_vsep(frp, add);
    }
}

/*
 * Set frame width from the window it contains.
 */
    स्थिर शून्य
frame_fix_width(win_T *wp)
{
    wp->w_frame->fr_width = wp->w_width + wp->w_vsep_width;
}

/*
 * Set frame height from the window it contains.
 */
    स्थिर शून्य
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
    स्थिर पूर्णाङ्क
frame_minheight(frame_T *topfrp, win_T *next_curwin)
{
    frame_T	*frp;
    पूर्णाङ्क		m;
    पूर्णाङ्क		n;

    सचे (topfrp->fr_win != रिक्त)
    {
	सचे (topfrp->fr_win == next_curwin)
	    m = p_wh + topfrp->fr_win->w_status_height;
	अञ्ञथा
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_status_height;
	    सचे (topfrp->fr_win == curwin && next_curwin == रिक्त)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		सचे (p_wmh == 0)
		    ++m;
		m += WINBAR_HEIGHT(curwin);
	    }
	}
    }
    अञ्ञथा सचे (topfrp->fr_layout == FR_ROW)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minheight(frp, next_curwin);
	    सचे (n > m)
		m = n;
	}
    }
    अञ्ञथा
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minheight(frp, next_curwin);
    }

    पटिनिवत्त m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
स्थिर पूर्णाङ्क
frame_wincount_in_height(frame_T *topfrp)
{
    frame_T	*frp;
    पूर्णाङ्क		c = 0;
    पूर्णाङ्क		n;

    सचे (topfrp->fr_win != रिक्त)
	c = 1;
    अञ्ञथा सचे (topfrp->fr_layout == FR_ROW)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_wincount_in_height(frp);
	    सचे (n > c)
		c = n;
	}
    }
    अञ्ञथा
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    c += frame_wincount_in_height(frp);
    }

    पटिनिवत्त c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    स्थिर पूर्णाङ्क
frame_minwidth(
    frame_T	*topfrp,
    win_T	*next_curwin)	// use p_wh and p_wiw for next_curwin
{
    frame_T	*frp;
    पूर्णाङ्क		m, n;

    सचे (topfrp->fr_win != रिक्त)
    {
	सचे (topfrp->fr_win == next_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_width;
	अञ्ञथा
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_width;
	    // Current window is minimal one column wide
	    सचे (p_wmw == 0 && topfrp->fr_win == curwin && next_curwin == रिक्त)
		++m;
	}
    }
    अञ्ञथा सचे (topfrp->fr_layout == FR_COL)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minwidth(frp, next_curwin);
	    सचे (n > m)
		m = n;
	}
    }
    अञ्ञथा
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minwidth(frp, next_curwin);
    }

    पटिनिवत्त m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    शून्य
close_others(
    पूर्णाङ्क		message,
    पूर्णाङ्क		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    पूर्णाङ्क		r;

    सचे (one_window())
    {
	सचे (message && !autocmd_busy)
	    सन्देशः(_(m_onlyone));
	पटिनिवत्त;
    }

    // Be very careful here: autocommands may change the window layout.
    अत्थाय (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_next;

	// autocommands messed this one up
	सचे (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
	}

	सचे (wp == curwin)		// don't close current window
	    अनुवर्त;

	// autoccommands messed this one up
	सचे (!buf_valid(wp->w_buffer) && win_valid(wp))
	{
	    wp->w_buffer = रिक्त;
	    win_close(wp, 0);
	    अनुवर्त;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_buffer, forceit);
	सचे (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    अनुवर्त;
	}
	सचे (!r)
	{
#सचे defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    सचे (message && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_changed(wp->w_buffer, FALSE);
		सचे (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    अनुवर्त;
		}
	    }
	    सचे (bufIsChanged(wp->w_buffer))
#endif
		अनुवर्त;
	}
	win_close(wp, !buf_hide(wp->w_buffer) && !bufIsChanged(wp->w_buffer));
    }

    सचे (message && !ONE_WINDOW)
	emsg(_(e_other_window_contains_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    शून्य
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
स्थिर पूर्णाङ्क command_frame_height = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    शून्य
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
    पूर्णाङ्क
win_alloc_first(शून्य)
{
    सचे (win_alloc_firstwin(रिक्त) == FAIL)
	पटिनिवत्त FAIL;

    first_tabpage = alloc_tabpage();
    सचे (first_tabpage == रिक्त)
	पटिनिवत्त FAIL;
    curtab = first_tabpage;
    unuse_tabpage(first_tabpage);

    पटिनिवत्त OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(शून्य)
{
    win_T *wp;

    wp = win_alloc(रिक्त, TRUE);
    सचे (wp == रिक्त)
	पटिनिवत्त रिक्त;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_frame(wp);
    पटिनिवत्त wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    शून्य
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
    स्थिर पूर्णाङ्क
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(रिक्त, FALSE);
    सचे (curwin == रिक्त)
	पटिनिवत्त FAIL;
    सचे (oldwin == रिक्त)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(रिक्त, रिक्त, 1L, BLN_LISTED);
	सचे (curwin == रिक्त || curbuf == रिक्त)
	    पटिनिवत्त FAIL;
	curwin->w_buffer = curbuf;
#ifdef FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#endif
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    अञ्ञथा
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_frame(curwin);
    सचे (curwin->w_frame == रिक्त)
	पटिनिवत्त FAIL;
    topframe = curwin->w_frame;
    topframe->fr_width = COLUMNS_WITHOUT_TPL();
    topframe->fr_height = Rows - p_ch;

    पटिनिवत्त OK;
}

/*
 * Create a frame for window "wp".
 */
    स्थिर शून्य
new_frame(win_T *wp)
{
    frame_T *frp = ALLOC_CLEAR_ONE(frame_T);

    wp->w_frame = frp;
    सचे (frp == रिक्त)
	पटिनिवत्त;
    frp->fr_layout = FR_LEAF;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    शून्य
win_init_size(शून्य)
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
    स्थिर tabpage_T *
alloc_tabpage(शून्य)
{
    tabpage_T	*tp;
#ifdef FEAT_GUI
    पूर्णाङ्क		i;
#endif


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    सचे (tp == रिक्त)
	पटिनिवत्त रिक्त;

#ifdef FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    सचे (tp->tp_vars == रिक्त)
    {
	vim_free(tp);
	पटिनिवत्त रिक्त;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#endif

#ifdef FEAT_GUI
    अत्थाय (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#endif
#ifdef FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#endif
    tp->tp_ch_used = p_ch;

    पटिनिवत्त tp;
}

    शून्य
free_tabpage(tabpage_T *tp)
{
    पूर्णाङ्क idx;

#ifdef FEAT_DIFF
    diff_clear(tp);
#endif
#ifdef FEAT_PROP_POPUP
    याव (tp->tp_first_popupwin != रिक्त)
	popup_close_tabpage(tp, tp->tp_first_popupwin->w_id, TRUE);
#endif
    अत्थाय (idx = 0; idx < SNAP_गणनम्; ++idx)
	clear_snapshot(tp, idx);
#ifdef FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#endif

    सचे (tp == lastused_tabpage)
	lastused_tabpage = रिक्त;
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
    पूर्णाङ्क
win_new_tabpage(पूर्णाङ्क after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    पूर्णाङ्क		n;
#सचे defined(FEAT_TABPANEL)
    पूर्णाङ्क prev_columns = COLUMNS_WITHOUT_TPL();
#endif

    सचे (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	पटिनिवत्त FAIL;
    }
    सचे (window_layout_locked(CMD_tabnew))
	पटिनिवत्त FAIL;

    newtp = alloc_tabpage();
    सचे (newtp == रिक्त)
	पटिनिवत्त FAIL;

    // Remember the current windows in this Tab page.
    सचे (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	पटिनिवत्त FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == रिक्त)
				    ? रिक्त : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    सचे (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	सचे (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_next = first_tabpage;
	    first_tabpage = newtp;
	}
	अञ्ञथा
	{
	    सचे (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		अत्थाय (tp = first_tabpage; tp->tp_next != रिक्त
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

#सचे defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#endif
#ifdef FEAT_JOB_CHANNEL
	entering_window(curwin);
#endif
#सचे defined(FEAT_TABPANEL)
	सचे (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    पूर्णाङ्क		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		सचे (tp2 != curtab)
		{
		    frame_new_width(tp2->tp_topframe, w, FALSE, TRUE);
		    सचे (!frame_check_width(tp2->tp_topframe, w))
			frame_new_width(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#endif
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, रिक्त, रिक्त, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, रिक्त, रिक्त, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, रिक्त, रिक्त, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, रिक्त, रिक्त, FALSE, curbuf);
	पटिनिवत्त OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    पटिनिवत्त FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    स्थिर पूर्णाङ्क
may_open_tabpage(शून्य)
{
    पूर्णाङ्क		n = (cmdmod.cmod_tab == 0)
				       ? postponed_split_tab : cmdmod.cmod_tab;

    सचे (n == 0)
	पटिनिवत्त FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_split_tab = 0;
    पटिनिवत्त win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    पूर्णाङ्क
make_tabpages(पूर्णाङ्क maxcount)
{
    पूर्णाङ्क		गणनम् = maxcount;
    पूर्णाङ्क		todo;

    // Limit to 'tabpagemax' tabs.
    सचे (गणनम् > p_tpm)
	गणनम् = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    अत्थाय (todo = गणनम् - 1; todo > 0; --todo)
	सचे (win_new_tabpage(0) == FAIL)
	    भङ्ग;

    unblock_autocmds();

    // return actual number of tab pages
    पटिनिवत्त (गणनम् - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    पूर्णाङ्क
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	सचे (tp == tpc)
	    पटिनिवत्त TRUE;
    पटिनिवत्त FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    पूर्णाङ्क
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	सचे (tp == tpc)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		सचे (win_valid_any_tab(wp))
		    पटिनिवत्त TRUE;
	    }
	    पटिनिवत्त FALSE;
	}
    }
    // shouldn't happen
    पटिनिवत्त FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    शून्य
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    सचे (tab == first_tabpage)
    {
	first_tabpage = tab->tp_next;
	ptp = first_tabpage;
    }
    अञ्ञथा
    {
	अत्थाय (ptp = first_tabpage; ptp != रिक्त && ptp->tp_next != tab;
							    ptp = ptp->tp_next)
	    ;
	प्रतिज्ञा(ptp != रिक्त);
	ptp->tp_next = tab->tp_next;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
find_tabpage(पूर्णाङ्क n)
{
    tabpage_T	*tp;
    पूर्णाङ्क		i = 1;

    सचे (n == 0)
	पटिनिवत्त curtab;

    अत्थाय (tp = first_tabpage; tp != रिक्त && i != n; tp = tp->tp_next)
	++i;
    पटिनिवत्त tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    पूर्णाङ्क
tabpage_index(tabpage_T *ftp)
{
    पूर्णाङ्क		i = 1;
    tabpage_T	*tp;

    अत्थाय (tp = first_tabpage; tp != रिक्त && tp != ftp; tp = tp->tp_next)
	++i;
    पटिनिवत्त i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    स्थिर पूर्णाङ्क
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    पूर्णाङ्क		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ifdef FEAT_JOB_CHANNEL
    leaving_window(curwin);
#endif
    reset_VIsual_and_resel();	// stop Visual mode
    सचे (trigger_leave_autocmds)
    {
	सचे (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	    सचे (curtab != tp)
		पटिनिवत्त FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	सचे (curtab != tp)
	    पटिनिवत्त FAIL;
	apply_autocmds(EVENT_TABLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	सचे (curtab != tp)
	    पटिनिवत्त FAIL;
    }

    reset_dragwin();
#सचे defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    सचे (gui.in_use)
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
    सचे (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_width;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = रिक्त;
    lastwin = रिक्त;
    पटिनिवत्त OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    स्थिर शून्य
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    पूर्णाङ्क		trigger_enter_autocmds,
    पूर्णाङ्क		trigger_leave_autocmds)
{
    पूर्णाङ्क		old_off = tp->tp_firstwin->w_winrow;
    win_T	*next_prevwin = tp->tp_prevwin;
    tabpage_T	*last_tab = curtab;

    use_tabpage(tp);

    सचे (p_ch != curtab->tp_ch_used)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	पूर्णाङ्क new_ch = curtab->tp_ch_used;
	curtab->tp_ch_used = p_ch;
	command_frame_height = FALSE;
	set_option_मानम्((char_u *)"cmdheight", new_ch, रिक्त, 0);
	command_frame_height = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (शून्य)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
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
    सचे (curtab->tp_old_Rows != ROWS_AVAIL || (old_off != firstwin->w_winrow
#ifdef FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#endif
		))
	shell_new_rows();
    सचे (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	सचे (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_width;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	अञ्ञथा
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = last_tab;

#सचे defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    सचे (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, रिक्त, रिक्त, FALSE, curbuf);
	सचे (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    शून्य
goto_tabpage(पूर्णाङ्क n)
{
    tabpage_T	*tp = रिक्त;  // shut up compiler
    tabpage_T	*ttp;
    पूर्णाङ्क		i;

    सचे (text_locked())
    {
	// Not allowed when editing the command line.
	text_locked_सन्देशः();
	पटिनिवत्त;
    }

    // If there is only one it can't work.
    सचे (first_tabpage->tp_next == रिक्त)
    {
	सचे (n > 1)
	    beep_flush();
	पटिनिवत्त;
    }

    सचे (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	सचे (curtab->tp_next == रिक्त)
	    tp = first_tabpage;
	अञ्ञथा
	    tp = curtab->tp_next;
    }
    अञ्ञथा सचे (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	अत्थाय (i = n; i < 0; ++i)
	{
	    अत्थाय (tp = first_tabpage; tp->tp_next != ttp && tp->tp_next != रिक्त;
		    tp = tp->tp_next)
		;
	    ttp = tp;
	}
    }
    अञ्ञथा सचे (n == 9999)
    {
	// Go to last tab page.
	अत्थाय (tp = first_tabpage; tp->tp_next != रिक्त; tp = tp->tp_next)
	    ;
    }
    अञ्ञथा
    {
	// Go to tab page "n".
	tp = find_tabpage(n);
	सचे (tp == रिक्त)
	{
	    beep_flush();
	    पटिनिवत्त;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ifdef FEAT_GUI_TABLINE
    सचे (gui_use_tabline())
	gui_mch_set_curtab(tabpage_index(curtab));
#endif
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    शून्य
goto_tabpage_tp(
    tabpage_T	*tp,
    पूर्णाङ्क		trigger_enter_autocmds,
    पूर्णाङ्क		trigger_leave_autocmds)
{
    सचे (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    set_keep_सन्देशः(रिक्त, 0);

    skip_win_fix_scroll = TRUE;
    सचे (tp != curtab && leave_tabpage(tp->tp_curwin->w_buffer,
					trigger_leave_autocmds) == OK)
    {
	सचे (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	अञ्ञथा
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    पूर्णाङ्क
goto_tabpage_lastused(शून्य)
{
    सचे (!valid_tabpage(lastused_tabpage))
	पटिनिवत्त FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    पटिनिवत्त OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    शून्य
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    सचे (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ifdef FEAT_GUI_TABLINE
	सचे (gui_use_tabline())
	    gui_mch_set_curtab(tabpage_index(curtab));
#endif
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    शून्य
tabpage_move(पूर्णाङ्क nr)
{
    पूर्णाङ्क		n = 1;
    tabpage_T	*tp, *tp_dst;

    सचे (first_tabpage->tp_next == रिक्त)
	पटिनिवत्त;

    सचे (tabpage_move_disallowed)
	पटिनिवत्त;

    अत्थाय (tp = first_tabpage; tp->tp_next != रिक्त && n < nr; tp = tp->tp_next)
	++n;

    सचे (tp == curtab || (nr > 0 && tp->tp_next != रिक्त
						    && tp->tp_next == curtab))
	पटिनिवत्त;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    सचे (curtab == first_tabpage)
	first_tabpage = curtab->tp_next;
    अञ्ञथा
    {
	FOR_ALL_TABPAGES(tp)
	    सचे (tp->tp_next == curtab)
		भङ्ग;
	सचे (tp == रिक्त)	// "cannot happen"
	    पटिनिवत्त;
	tp->tp_next = curtab->tp_next;
    }

    // Re-insert it at the specified position.
    सचे (nr <= 0)
    {
	curtab->tp_next = first_tabpage;
	first_tabpage = curtab;
    }
    अञ्ञथा
    {
	curtab->tp_next = tp_dst->tp_next;
	tp_dst->tp_next = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#सचे defined(FEAT_TABPANEL)
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
    शून्य
win_goto(win_T *wp)
{
#ifdef FEAT_CONCEAL
    win_T	*owp = curwin;
#endif

#ifdef FEAT_PROP_POPUP
    सचे (ERROR_IF_ANY_POPUP_WINDOW)
	पटिनिवत्त;
    सचे (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_to_enter_popup_window));
	पटिनिवत्त;
    }
#endif
    सचे (text_or_buf_locked())
    {
	beep_flush();
	पटिनिवत्त;
    }

    सचे (wp->w_buffer != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    अञ्ञथा सचे (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    सचे (!win_valid(wp))
	पटिनिवत्त;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    win_enter(wp, TRUE);

#ifdef FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    सचे (win_valid(owp) && owp->w_p_cole > 0 && !सन्देशः_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    सचे (curwin->w_p_cole > 0 && !सन्देशः_scrolled)
	need_cursor_पङ्क्तिः_redraw = TRUE;
#endif
}

#सचे defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_find_nr(पूर्णाङ्क winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	सचे (--winnr == 0)
	    भङ्ग;
    पटिनिवत्त wp;
}
#endif

#सचे defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_find_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    सचे (wp == win)
		पटिनिवत्त tp;
    पटिनिवत्त रिक्त;
}
#endif

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, पूर्णाङ्क up, long गणनम्)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    सचे (popup_is_popup(wp))
	// popups don't have neighbors.
	पटिनिवत्त रिक्त;
#endif
    foundfr = wp->w_frame;
    याव (गणनम्--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	अत्थाय (;;)
	{
	    सचे (fr == tp->tp_topframe)
		गच्छ end;
	    सचे (up)
		nfr = fr->fr_prev;
	    अञ्ञथा
		nfr = fr->fr_next;
	    सचे (fr->fr_parent->fr_layout == FR_COL && nfr != रिक्त)
		भङ्ग;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	अत्थाय (;;)
	{
	    सचे (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		भङ्ग;
	    }
	    fr = nfr->fr_child;
	    सचे (nfr->fr_layout == FR_ROW)
	    {
		// Find the frame at the cursor row.
		याव (fr->fr_next != रिक्त
			&& frame2win(fr)->w_wincol + fr->fr_width
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_next;
	    }
	    सचे (nfr->fr_layout == FR_COL && up)
		याव (fr->fr_next != रिक्त)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    पटिनिवत्त foundfr != रिक्त ? foundfr->fr_win : रिक्त;
}

/*
 * Move to window above or below "count" times.
 */
    स्थिर शून्य
win_goto_ver(
    पूर्णाङ्क		up,		// TRUE to go to win above
    long	गणनम्)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    सचे (ERROR_IF_TERM_POPUP_WINDOW)
	पटिनिवत्त;
#endif
    win = win_vert_neighbor(curtab, curwin, up, गणनम्);
    सचे (win != रिक्त)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, पूर्णाङ्क वामः, long गणनम्)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    सचे (popup_is_popup(wp))
	// popups don't have neighbors.
	पटिनिवत्त रिक्त;
#endif
    foundfr = wp->w_frame;
    याव (गणनम्--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	अत्थाय (;;)
	{
	    सचे (fr == tp->tp_topframe)
		गच्छ end;
	    सचे (वामः)
		nfr = fr->fr_prev;
	    अञ्ञथा
		nfr = fr->fr_next;
	    सचे (fr->fr_parent->fr_layout == FR_ROW && nfr != रिक्त)
		भङ्ग;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	अत्थाय (;;)
	{
	    सचे (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		भङ्ग;
	    }
	    fr = nfr->fr_child;
	    सचे (nfr->fr_layout == FR_COL)
	    {
		// Find the frame at the cursor row.
		याव (fr->fr_next != रिक्त
			&& frame2win(fr)->w_winrow + fr->fr_height
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_next;
	    }
	    सचे (nfr->fr_layout == FR_ROW && वामः)
		याव (fr->fr_next != रिक्त)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    पटिनिवत्त foundfr != रिक्त ? foundfr->fr_win : रिक्त;
}

/*
 * Move to left or right window.
 */
    स्थिर शून्य
win_goto_hor(
    पूर्णाङ्क		वामः,		// TRUE to go to left win
    long	गणनम्)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    सचे (ERROR_IF_TERM_POPUP_WINDOW)
	पटिनिवत्त;
#endif
    win = win_horz_neighbor(curtab, curwin, वामः, गणनम्);
    सचे (win != रिक्त)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    शून्य
win_enter(win_T *wp, पूर्णाङ्क undo_sync)
{
    (शून्य)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    शून्य
win_fix_current_dir(शून्य)
{
    सचे (curwin->w_localdir != रिक्त || curtab->tp_localdir != रिक्त)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	सचे (globaldir == रिक्त)
	{
	    char_u	cwd[MAXPATHL];

	    सचे (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	सचे (curwin->w_localdir != रिक्त)
	    dirname = curwin->w_localdir;
	अञ्ञथा
	    dirname = curtab->tp_localdir;

	सचे (mch_chdir((वर्ण *)dirname) == 0)
	{
	    last_chdir_reason = रिक्त;
	    shorten_fnames(TRUE);
	}
    }
    अञ्ञथा सचे (globaldir != रिक्त)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((वर्ण *)globaldir);
	VIM_CLEAR(globaldir);
	last_chdir_reason = रिक्त;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    स्थिर पूर्णाङ्क
win_enter_ext(win_T *wp, पूर्णाङ्क flags)
{
    पूर्णाङ्क		other_buffer = FALSE;
    पूर्णाङ्क		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    पूर्णाङ्क		did_decrement = FALSE;

    सचे (wp == curwin && curwin_invalid == 0)	// nothing to do
	पटिनिवत्त FALSE;

#ifdef FEAT_JOB_CHANNEL
    सचे (curwin_invalid == 0)
	leaving_window(curwin);
#endif

    सचे (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	सचे (wp->w_buffer != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	    other_buffer = TRUE;
	    सचे (!win_valid(wp))
		पटिनिवत्त FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	सचे (!win_valid(wp))
	    पटिनिवत्त FALSE;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	सचे (aborting())
	    पटिनिवत्त FALSE;
#endif
    }

    // sync undo before leaving the current buffer
    सचे ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_buffer)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    सचे (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    सचे (wp->w_buffer != curbuf)
	buf_copy_options(wp->w_buffer, BCO_ENTER | BCO_NOHELP);
    सचे (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_status = true;
    }
    curwin = wp;
    curbuf = wp->w_buffer;
    check_cursor();
    सचे (!virtual_active())
	curwin->w_cursor.coladd = 0;
    सचे (*p_spk == 'c')		// assume cursor position needs updating
	changed_पङ्क्तिः_abv_curs();
    अञ्ञथा
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ifdef MESSAGE_QUEUE
    सचे (flags & WEE_ALLOW_PARSE_MESSAGES)
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
    सचे (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, रिक्त, रिक्त, FALSE, curbuf);
    सचे (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, रिक्त, रिक्त, FALSE, curbuf);
	सचे (other_buffer)
	    apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_status = true;
#ifdef FEAT_TERMINAL
    सचे (bt_terminal(curwin->w_buffer))
	// terminal is likely in another mode
	redraw_mode = TRUE;
#endif
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#सचे defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    सचे (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    सचे (curwin->w_height < p_wh && !curwin->w_p_wfh
#ifdef FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#endif
	    )
	win_setheight((पूर्णाङ्क)p_wh);
    अञ्ञथा सचे (curwin->w_height == 0)
	win_setheight(1);

    // set window width to desired minimal value
    सचे (curwin->w_width < p_wiw && !curwin->w_p_wfw)
	win_setwidth((पूर्णाङ्क)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    पटिनिवत्त did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = रिक्त;

    सचे (curwin->w_buffer == buf)
	wp = curwin;
    अञ्ञथा
	FOR_ALL_WINDOWS(wp)
	    सचे (wp->w_buffer == buf)
		भङ्ग;
    सचे (wp != रिक्त)
	win_enter(wp, FALSE);
    पटिनिवत्त wp;
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

    सचे (wp != रिक्त)
	पटिनिवत्त wp;

    FOR_ALL_TABPAGES(tp)
	सचे (tp != curtab)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		सचे (wp->w_buffer == buf)
		    भङ्ग;
	    सचे (wp != रिक्त)
	    {
		goto_tabpage_win(tp, wp);
		सचे (curwin != wp)
		    wp = रिक्त;	// something went wrong
		भङ्ग;
	    }
	}
    पटिनिवत्त wp;
}

स्थिर पूर्णाङ्क last_win_id = LOWEST_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    स्थिर win_T *
win_alloc(win_T *after, पूर्णाङ्क hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    सचे (new_wp == रिक्त)
	पटिनिवत्त रिक्त;

    सचे (win_alloc_पङ्क्तयः(new_wp) == FAIL)
    {
	vim_free(new_wp);
	पटिनिवत्त रिक्त;
    }

    new_wp->w_id = ++last_win_id;

#ifdef FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    सचे (new_wp->w_vars == रिक्त)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	पटिनिवत्त रिक्त;
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
    सचे (!hidden)
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
    सचे (gui.in_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_वामः],
		SBAR_वामः, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_दक्षिणः],
		SBAR_दक्षिणः, new_wp);
    }
#endif
#ifdef FEAT_FOLDING
    foldInitWin(new_wp);
#endif
    unblock_autocmds();
#ifdef FEAT_अन्वेषणम्_EXTRA
    new_wp->w_next_match_id = 1000;  // up to 1000 can be picked by the user
#endif
    पटिनिवत्त new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    स्थिर शून्य
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    पूर्णाङ्क		i;
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
    सचे (wp->w_stl_click != रिक्त)
    {
	अत्थाय (i = 0; i < wp->w_stl_click_गणनम्; i++)
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

	सचे (prevwin == wp)
	    prevwin = रिक्त;
	FOR_ALL_TABPAGES(ttp)
	    सचे (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = रिक्त;
    }
    win_free_lsize(wp);

    अत्थाय (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    सचे (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    सचे (wip2 != wip && wip2->wi_win == रिक्त)
		    {
			सचे (wip2->wi_next != रिक्त)
			    wip2->wi_next->wi_prev = wip2->wi_prev;
			सचे (wip2->wi_prev == रिक्त)
			    buf->b_wininfo = wip2->wi_next;
			अञ्ञथा
			    wip2->wi_prev->wi_next = wip2->wi_next;
			free_wininfo(wip2);
			भङ्ग;
		    }

		wip->wi_win = रिक्त;
	    }

#ifdef FEAT_अन्वेषणम्_EXTRA
    clear_matches(wp);
#endif

    free_jumplist(wp);

#ifdef FEAT_QUICKFIX
    qf_free_all(wp);
#endif

#ifdef FEAT_GUI
    सचे (gui.in_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_वामः]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_दक्षिणः]);
    }
#endif // FEAT_GUI

#ifdef FEAT_MENU
    remove_winbar(wp);
#endif
#ifdef FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    अत्थाय (i = 0; i < 4; ++i)
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

    सचे (win_valid_any_tab(wp))
	win_remove(wp, tp);
    सचे (autocmd_busy)
    {
	wp->w_next = au_pending_free_win;
	au_pending_free_win = wp;
    }
    अञ्ञथा
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    पूर्णाङ्क
win_unlisted(win_T *wp)
{
    पटिनिवत्त is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#सचे defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    शून्य
win_free_popup(win_T *win)
{
    सचे (win->w_buffer != रिक्त)
    {
	सचे (bt_popup(win->w_buffer))
	    win_close_buffer(win, DOBUF_WIPE_REUSE, FALSE);
	अञ्ञथा
	    close_buffer(win, win->w_buffer, 0, FALSE, FALSE, TRUE);

	सचे (win->w_buffer != रिक्त)
	    --win->w_buffer->b_nwindows;
    }
# सचे defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    सचे (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_frame);
    win_free(win, रिक्त);
}
#endif

/*
 * Append window "wp" in the window list after window "after".
 */
    स्थिर शून्य
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    सचे (after == रिक्त)	    // after NULL is in front of the first
	before = firstwin;
    अञ्ञथा
	before = after->w_next;

    wp->w_next = before;
    wp->w_prev = after;
    सचे (after == रिक्त)
	firstwin = wp;
    अञ्ञथा
	after->w_next = wp;
    सचे (before == रिक्त)
	lastwin = wp;
    अञ्ञथा
	before->w_prev = wp;
}

/*
 * Remove a window from the window list.
 */
    शून्य
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    सचे (wp->w_prev != रिक्त)
	wp->w_prev->w_next = wp->w_next;
    अञ्ञथा सचे (tp == रिक्त)
	firstwin = curtab->tp_firstwin = wp->w_next;
    अञ्ञथा
	tp->tp_firstwin = wp->w_next;

    सचे (wp->w_next != रिक्त)
	wp->w_next->w_prev = wp->w_prev;
    अञ्ञथा सचे (tp == रिक्त)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    अञ्ञथा
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    स्थिर शून्य
frame_append(frame_T *after, frame_T *frp)
{
    frp->fr_next = after->fr_next;
    after->fr_next = frp;
    सचे (frp->fr_next != रिक्त)
	frp->fr_next->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    स्थिर शून्य
frame_निवेशय(frame_T *before, frame_T *frp)
{
    frp->fr_next = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    सचे (frp->fr_prev != रिक्त)
	frp->fr_prev->fr_next = frp;
    अञ्ञथा
	frp->fr_parent->fr_child = frp;
}

/*
 * Remove a frame from a frame list.
 */
    स्थिर शून्य
frame_remove(frame_T *frp)
{
    सचे (frp->fr_prev != रिक्त)
	frp->fr_prev->fr_next = frp->fr_next;
    अञ्ञथा
	frp->fr_parent->fr_child = frp->fr_next;
    सचे (frp->fr_next != रिक्त)
	frp->fr_next->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    पूर्णाङ्क
win_alloc_पङ्क्तयः(win_T *wp)
{
    wp->w_पङ्क्तयः_valid = 0;
    wp->w_पङ्क्तयः = ALLOC_CLEAR_MULT(wline_T, Rows);
    सचे (wp->w_पङ्क्तयः == रिक्त)
	पटिनिवत्त FAIL;
    पटिनिवत्त OK;
}

/*
 * free lsize arrays for a window
 */
    शून्य
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    सचे (wp != रिक्त)
	VIM_CLEAR(wp->w_पङ्क्तयः);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    शून्य
shell_new_rows(शून्य)
{
    पूर्णाङ्क		h = (पूर्णाङ्क)ROWS_AVAIL;

    सचे (firstwin == रिक्त)	// not initialized yet
	पटिनिवत्त;
    सचे (h < frame_minheight(topframe, रिक्त))
	h = frame_minheight(topframe, रिक्त);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    frame_new_height(topframe, h, FALSE, TRUE, FALSE);
    सचे (!frame_check_height(topframe, h))
	frame_new_height(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_used = p_ch;

    सचे (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#सचे defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#सचे 0
    // Disabled: don't want making the screen smaller make a window larger.
    सचे (p_ea)
	win_equal(curwin, FALSE, 'v');
#endif
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    शून्य
shell_new_columns(शून्य)
{
    सचे (firstwin == रिक्त)	// not initialized yet
	पटिनिवत्त;

#सचे defined(FEAT_TABPANEL)
    स्थिर पूर्णाङ्क prev_tp_width;
    स्थिर पूर्णाङ्क prev_wincol;
    स्थिर पूर्णाङ्क prev_fr_width;
#endif
    पूर्णाङ्क w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    frame_new_width(topframe, w, FALSE, TRUE);
    सचे (!frame_check_width(topframe, w))
	frame_new_width(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#सचे defined(FEAT_TABPANEL)
    पूर्णाङ्क tp_width = tabpanel_width();
    पूर्णाङ्क tp_width_changed = tp_width != prev_tp_width;
    // tabpanel width changed
    सचे (tp_width_changed && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    सचे (tp_width_changed
	    || (tp_width > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_width != prev_fr_width)))
    {
	screen_fill(cmdline_row, (पूर्णाङ्क)Rows, 0, (पूर्णाङ्क)Columns, ' ', ' ', 0);
    }
    prev_tp_width = tabpanel_width();
    prev_wincol = firstwin->w_wincol;
    prev_fr_width = topframe->fr_width;
#endif
    // Adjust offset for command line start column
    cmdline_col_off = firstwin->w_wincol;
    cmdline_width = topframe->fr_width;
    comp_col();
    सचे (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#सचे defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#सचे 0
    // Disabled: don't want making the screen smaller make a window larger.
    सचे (p_ea)
	win_equal(curwin, FALSE, 'h');
#endif
}

/*
 * Save the size of all windows in "gap".
 */
    शून्य
win_size_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, परिमाण(पूर्णाङ्क), 1);
    सचे (ga_grow(gap, win_गणनम्() * 2 + 1) == FAIL)
	पटिनिवत्त;

    // first entry is the total lines available for windows
    ((पूर्णाङ्क *)gap->ga_data)[gap->ga_len++] = ROWS_AVAIL - last_stl_height(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((पूर्णाङ्क *)gap->ga_data)[gap->ga_len++] =
	    wp->w_width + wp->w_vsep_width;
	((पूर्णाङ्क *)gap->ga_data)[gap->ga_len++] = wp->w_height;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    शून्य
win_size_restore(garray_T *gap)
{
    win_T	*wp;
    पूर्णाङ्क		i, j;

    सचे (win_गणनम्() * 2 + 1 == gap->ga_len
	    && ((पूर्णाङ्क *)gap->ga_data)[0] == ROWS_AVAIL - last_stl_height(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	अत्थाय (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		frame_setwidth(wp->w_frame, ((पूर्णाङ्क *)gap->ga_data)[i++]);
		win_setheight_win(((पूर्णाङ्क *)gap->ga_data)[i++], wp);
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
    शून्य
win_comp_pos(शून्य)
{
    पूर्णाङ्क		row = tabline_height();
    पूर्णाङ्क		col = TPL_LCOL();

    frame_comp_pos(topframe, &row, &col);
#सचे defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    स्थिर शून्य
frame_comp_pos(frame_T *topfrp, पूर्णाङ्क *row, पूर्णाङ्क *col)
{
    win_T	*wp;
    frame_T	*frp;
    पूर्णाङ्क		startcol;
    पूर्णाङ्क		startrow;
    पूर्णाङ्क		h;

    wp = topfrp->fr_win;
    सचे (wp != रिक्त)
    {
	सचे (wp->w_winrow != *row || wp->w_wincol != *col)
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
    अञ्ञथा
    {
	startrow = *row;
	startcol = *col;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    सचे (topfrp->fr_layout == FR_ROW)
		*row = startrow;	// all frames are at the same row
	    अञ्ञथा
		*col = startcol;	// all frames are at the same col
	    frame_comp_pos(frp, row, col);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    शून्य
win_ensure_size(शून्य)
{
    सचे (curwin->w_height == 0)
	win_setheight(1);
    सचे (curwin->w_width == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    शून्य
win_setheight(पूर्णाङ्क height)
{
    win_setheight_win(height, curwin);
#सचे defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    शून्य
win_setheight_win(पूर्णाङ्क height, win_T *win)
{
    सचे (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	सचे (height < p_wmh)
	    height = p_wmh;
	सचे (height == 0)
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
    स्थिर शून्य
frame_setheight(frame_T *curfrp, पूर्णाङ्क height)
{
    पूर्णाङ्क		room;		// total number of lines available
    पूर्णाङ्क		take;		// number of lines taken from other windows
    पूर्णाङ्क		room_cmdline;	// lines available from cmdline
    पूर्णाङ्क		run;
    frame_T	*frp;
    पूर्णाङ्क		h;
    पूर्णाङ्क		room_reserved;

    // If the height already is the desired value, nothing to do.
    सचे (curfrp->fr_height == height)
	पटिनिवत्त;

    सचे (curfrp->fr_parent == रिक्त)
    {
	// topframe: can only change the command line height
	सचे (height > 0)
	    frame_new_height(curfrp, height, FALSE, FALSE, TRUE);
    }
    अञ्ञथा सचे (curfrp->fr_parent->fr_layout == FR_ROW)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = frame_minheight(curfrp->fr_parent, रिक्त);
	सचे (height < h)
	    height = h;
	frame_setheight(curfrp->fr_parent, height);
    }
    अञ्ञथा
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
	अत्थाय (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		सचे (frp != curfrp
			&& frp->fr_win != रिक्त
			&& frp->fr_win->w_p_wfh)
		    room_reserved += frp->fr_height;
		room += frp->fr_height;
		सचे (frp != curfrp)
		    room -= frame_minheight(frp, रिक्त);
	    }
	    सचे (curfrp->fr_width != topframe->fr_width)
		room_cmdline = 0;
	    अञ्ञथा
	    {
		room_cmdline = Rows - p_ch - (lastwin->w_winrow
						+ VISIBLE_HEIGHT(lastwin)
						+ lastwin->w_status_height);
		सचे (room_cmdline < 0)
		    room_cmdline = 0;
	    }

	    सचे (height <= room + room_cmdline)
		भङ्ग;
	    सचे (run == 2 || curfrp->fr_width == topframe->fr_width)
	    {
		height = room + room_cmdline;
		भङ्ग;
	    }
	    frame_setheight(curfrp->fr_parent, height
		+ frame_minheight(curfrp->fr_parent, NOWIN) - (पूर्णाङ्क)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = height - curfrp->fr_height;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	सचे (height > room + room_cmdline - room_reserved)
	    room_reserved = room + room_cmdline - height;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	सचे (take < 0 && room - curfrp->fr_height <= room_reserved)
	    room_reserved = 0;

	सचे (take > 0 && room_cmdline > 0)
	{
	    // use lines from cmdline first
	    सचे (take < room_cmdline)
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
	अत्थाय (run = 0; run < 2; ++run)
	{
	    सचे (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    अञ्ञथा
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    याव (frp != रिक्त && take != 0)
	    {
		h = frame_minheight(frp, रिक्त);
		सचे (room_reserved > 0
			&& frp->fr_win != रिक्त
			&& frp->fr_win->w_p_wfh)
		{
		    सचे (room_reserved >= frp->fr_height)
			room_reserved -= frp->fr_height;
		    अञ्ञथा
		    {
			सचे (frp->fr_height - room_reserved > take)
			    room_reserved = frp->fr_height - take;
			take -= frp->fr_height - room_reserved;
			frame_new_height(frp, room_reserved, FALSE, FALSE,
									TRUE);
			room_reserved = 0;
		    }
		}
		अञ्ञथा
		{
		    सचे (frp->fr_height - take < h)
		    {
			take -= frp->fr_height - h;
			frame_new_height(frp, h, FALSE, FALSE, TRUE);
		    }
		    अञ्ञथा
		    {
			frame_new_height(frp, frp->fr_height - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		सचे (run == 0)
		    frp = frp->fr_next;
		अञ्ञथा
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    शून्य
win_setwidth(पूर्णाङ्क width)
{
    win_setwidth_win(width, curwin);
}

    शून्य
win_setwidth_win(पूर्णाङ्क width, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    सचे (wp == curwin)
    {
	सचे (width < p_wmw)
	    width = p_wmw;
	सचे (width == 0)
	    width = 1;
    }
    अञ्ञथा सचे (width < 0)
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
    स्थिर शून्य
frame_setwidth(frame_T *curfrp, पूर्णाङ्क width)
{
    पूर्णाङ्क		room;		// total number of lines available
    पूर्णाङ्क		take;		// number of lines taken from other windows
    पूर्णाङ्क		run;
    frame_T	*frp;
    पूर्णाङ्क		w;
    पूर्णाङ्क		room_reserved;

    // If the width already is the desired value, nothing to do.
    सचे (curfrp->fr_width == width)
	पटिनिवत्त;

    सचे (curfrp->fr_parent == रिक्त)
	// topframe: can't change width
	पटिनिवत्त;

    सचे (curfrp->fr_parent->fr_layout == FR_COL)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = frame_minwidth(curfrp->fr_parent, रिक्त);
	सचे (width < w)
	    width = w;
	frame_setwidth(curfrp->fr_parent, width);
    }
    अञ्ञथा
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	अत्थाय (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		सचे (frp != curfrp
			&& frp->fr_win != रिक्त
			&& frp->fr_win->w_p_wfw)
		    room_reserved += frp->fr_width;
		room += frp->fr_width;
		सचे (frp != curfrp)
		    room -= frame_minwidth(frp, रिक्त);
	    }

	    सचे (width <= room)
		भङ्ग;
	    सचे (run == 2 || curfrp->fr_height >= ROWS_AVAIL)
	    {
		width = room;
		भङ्ग;
	    }
	    frame_setwidth(curfrp->fr_parent, width
		 + frame_minwidth(curfrp->fr_parent, NOWIN) - (पूर्णाङ्क)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = width - curfrp->fr_width;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	सचे (width > room - room_reserved)
	    room_reserved = room - width;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	सचे (take < 0 && room - curfrp->fr_width < room_reserved)
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
	अत्थाय (run = 0; run < 2; ++run)
	{
	    सचे (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    अञ्ञथा
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    याव (frp != रिक्त && take != 0)
	    {
		w = frame_minwidth(frp, रिक्त);
		सचे (room_reserved > 0
			&& frp->fr_win != रिक्त
			&& frp->fr_win->w_p_wfw)
		{
		    सचे (room_reserved >= frp->fr_width)
			room_reserved -= frp->fr_width;
		    अञ्ञथा
		    {
			सचे (frp->fr_width - room_reserved > take)
			    room_reserved = frp->fr_width - take;
			take -= frp->fr_width - room_reserved;
			frame_new_width(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		अञ्ञथा
		{
		    सचे (frp->fr_width - take < w)
		    {
			take -= frp->fr_width - w;
			frame_new_width(frp, w, FALSE, FALSE);
		    }
		    अञ्ञथा
		    {
			frame_new_width(frp, frp->fr_width - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		सचे (run == 0)
		    frp = frp->fr_next;
		अञ्ञथा
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    शून्य
win_setminheight(शून्य)
{
    पूर्णाङ्क		room;
    पूर्णाङ्क		needed;
    पूर्णाङ्क		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    याव (p_wmh > 0)
    {
	room = Rows - p_ch;
	needed = min_rows_for_all_tabpages() - 1;  // 1 was added for the cmdline
	सचे (room >= needed)
	    भङ्ग;
	--p_wmh;
	सचे (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    शून्य
win_setminwidth(शून्य)
{
    पूर्णाङ्क		room;
    पूर्णाङ्क		needed;
    पूर्णाङ्क		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    याव (p_wmw > 0)
    {
	room = topframe->fr_width;
	needed = frame_minwidth(topframe, रिक्त);
	सचे (room >= needed)
	    भङ्ग;
	--p_wmw;
	सचे (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    शून्य
win_drag_status_पङ्क्तिः(win_T *dragwin, पूर्णाङ्क offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    पूर्णाङ्क		room;
    पूर्णाङ्क		up;	// if TRUE, drag status line up, otherwise down
    पूर्णाङ्क		n;

    fr = dragwin->w_frame;
    curfr = fr;
    सचे (fr != topframe)		// more than one window
    {
	fr = fr->fr_parent;
	// When the parent frame is not a column of frames, its parent should
	// be.
	सचे (fr->fr_layout != FR_COL)
	{
	    curfr = fr;
	    सचे (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_parent;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    याव (curfr != topframe && curfr->fr_next == रिक्त)
    {
	सचे (fr != topframe)
	    fr = fr->fr_parent;
	curfr = fr;
	सचे (fr != topframe)
	    fr = fr->fr_parent;
    }

    सचे (offset < 0) // drag up
    {
	up = TRUE;
	offset = -offset;
	// sum up the room of the current frame and above it
	सचे (fr == curfr)
	{
	    // only one window
	    room = fr->fr_height - frame_minheight(fr, रिक्त);
	}
	अञ्ञथा
	{
	    room = 0;
	    अत्थाय (fr = fr->fr_child; ; fr = fr->fr_next)
	    {
		room += fr->fr_height - frame_minheight(fr, रिक्त);
		सचे (fr == curfr)
		    भङ्ग;
	    }
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    अञ्ञथा    // drag down
    {
	up = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	room = Rows - cmdline_row;
	सचे (curfr->fr_next == रिक्त)
	    --room;
	अञ्ञथा
	    room -= p_ch;
	सचे (room < 0)
	    room = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_height - frame_minheight(fr, रिक्त);
	fr = curfr;			// put fr at window that grows
    }

    सचे (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    सचे (offset <= 0)
	पटिनिवत्त;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    सचे (fr != रिक्त)
	frame_new_height(fr, fr->fr_height + offset, up, FALSE, TRUE);

    सचे (up)
	fr = curfr;		// current frame gets smaller
    अञ्ञथा
	fr = curfr->fr_next;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    याव (fr != रिक्त && offset > 0)
    {
	n = frame_minheight(fr, रिक्त);
	सचे (fr->fr_height - offset <= n)
	{
	    offset -= fr->fr_height - n;
	    frame_new_height(fr, n, !up, FALSE, TRUE);
	}
	अञ्ञथा
	{
	    frame_new_height(fr, fr->fr_height - offset, !up, FALSE, TRUE);
	    भङ्ग;
	}
	सचे (up)
	    fr = fr->fr_prev;
	अञ्ञथा
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
    शून्य
win_drag_vsep_पङ्क्तिः(win_T *dragwin, पूर्णाङ्क offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    पूर्णाङ्क		room;
    पूर्णाङ्क		वामः;	// if TRUE, drag separator line left, otherwise right
    पूर्णाङ्क		n;

    fr = dragwin->w_frame;
    सचे (fr == topframe)		// only one window (cannot happen?)
	पटिनिवत्त;
    curfr = fr;
    fr = fr->fr_parent;
    // When the parent frame is not a row of frames, its parent should be.
    सचे (fr->fr_layout != FR_ROW)
    {
	सचे (fr == topframe)	// only a column of windows (cannot happen?)
	    पटिनिवत्त;
	curfr = fr;
	fr = fr->fr_parent;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    याव (curfr->fr_next == रिक्त)
    {
	सचे (fr == topframe)
	    भङ्ग;
	curfr = fr;
	fr = fr->fr_parent;
	सचे (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_parent;
	}
    }

    सचे (offset < 0) // drag left
    {
	वामः = TRUE;
	offset = -offset;
	// sum up the room of the current frame and left of it
	room = 0;
	अत्थाय (fr = fr->fr_child; ; fr = fr->fr_next)
	{
	    room += fr->fr_width - frame_minwidth(fr, रिक्त);
	    सचे (fr == curfr)
		भङ्ग;
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    अञ्ञथा    // drag right
    {
	वामः = FALSE;
	// sum up the room of frames right of the current one
	room = 0;
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_width - frame_minwidth(fr, रिक्त);
	fr = curfr;			// put fr at window that grows
    }

    सचे (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    सचे (offset <= 0)		// No room at all, quit.
	पटिनिवत्त;
    सचे (fr == रिक्त)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	पटिनिवत्त;

    // grow frame fr by offset lines
    frame_new_width(fr, fr->fr_width + offset, वामः, FALSE);

    // shrink other frames: current and at the left or at the right
    सचे (वामः)
	fr = curfr;		// current frame gets smaller
    अञ्ञथा
	fr = curfr->fr_next;	// next frame gets smaller

    याव (fr != रिक्त && offset > 0)
    {
	n = frame_minwidth(fr, रिक्त);
	सचे (fr->fr_width - offset <= n)
	{
	    offset -= fr->fr_width - n;
	    frame_new_width(fr, n, !वामः, FALSE);
	}
	अञ्ञथा
	{
	    frame_new_width(fr, fr->fr_width - offset, !वामः, FALSE);
	    भङ्ग;
	}
	सचे (वामः)
	    fr = fr->fr_prev;
	अञ्ञथा
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
    शून्य
set_fraction(win_T *wp)
{
    सचे (wp->w_height > 1)
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
    स्थिर शून्य
win_fix_scroll(पूर्णाङ्क resize)
{
    सचे (*p_spk == 'c')
	पटिनिवत्त;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	सचे (wp->w_height != wp->w_prev_height)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    सचे (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
	    {
		पूर्णाङ्क diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_height - wp->w_prev_height);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		सचे (diff > 0)
		    cursor_down_inner(wp, diff);
		अञ्ञथा
		    cursor_up_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_to_fraction(wp, wp->w_prev_height);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		सचे (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    अञ्ञथा सचे (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    सचे (!(get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL)))
	win_fix_cursor(FALSE);
    अञ्ञथा सचे (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    स्थिर शून्य
win_fix_cursor(पूर्णाङ्क normal)
{
    win_T	*wp = curwin;

    सचे (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम् < wp->w_height)
	पटिनिवत्त;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    long so = MIN(wp->w_height / 2, get_scrolloff_मानम्());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_down_inner(wp, so);
    linenr_T शिखरम् = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_up_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    सचे (lnum > bot && (wp->w_botline - wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्) != 1)
	nlnum = bot;
    अञ्ञथा सचे (lnum < शिखरम् && wp->w_topline != 1)
	nlnum = (so == wp->w_height / 2) ? bot : शिखरम्;

    सचे (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	सचे (normal)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	अञ्ञथा  // Scroll instead when not in normal mode.
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
    शून्य
win_new_height(win_T *wp, पूर्णाङ्क height)
{
    पूर्णाङ्क		prev_height = wp->w_height;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    सचे (height < 0)
	height = 0;
    सचे (wp->w_height == height)
	पटिनिवत्त;	    // nothing to do

    सचे (wp->w_height > 0)
    {
	सचे (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	सचे (wp->w_height != prev_height)
	    पटिनिवत्त;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	सचे (wp->w_wrow != wp->w_prev_fraction_row)
	    set_fraction(wp);
    }

    wp->w_height = height;
    wp->w_redr_status = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    सचे (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	सचे (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_to_fraction(wp, prev_height);
    }
}

    शून्य
scroll_to_fraction(win_T *wp, पूर्णाङ्क prev_height)
{
    linenr_T	lnum;
    पूर्णाङ्क		sline, पङ्क्तिः_size;
    पूर्णाङ्क		height = wp->w_height;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    सचे (height > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (height < wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम् || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	सचे (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((long)wp->w_fraction * (long)height - 1L)
							       / FRACTION_MULT;
	पङ्क्तिः_size = plines_win_col(wp, lnum, (long)(wp->w_cursor.col)) - 1;
	sline = wp->w_wrow - पङ्क्तिः_size;

	सचे (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    पूर्णाङ्क rows = plines_win(wp, lnum, FALSE);

	    सचे (sline > wp->w_height - rows)
	    {
		sline = wp->w_height - rows;
		wp->w_wrow -= rows - पङ्क्तिः_size;
	    }
	}

	सचे (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    पूर्णाङ्क		want_row = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = पङ्क्तिः_size;
	    सचे (wp->w_wrow >= wp->w_height
				       && (wp->w_width - win_col_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_width - win_col_off(wp);

		--wp->w_wrow;
		याव (wp->w_wrow > want_row)
		{
		    skipcol += wp->w_width - win_col_off(wp) + win_col_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	अञ्ञथा सचे (sline > 0)
	{
	    याव (sline > 0 && lnum > 1)
	    {
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, रिक्त, TRUE, रिक्त);
		सचे (lnum == 1)
		{
		    // first line in buffer is folded
		    पङ्क्तिः_size = 1;
		    --sline;
		    भङ्ग;
		}
#endif
		--lnum;
#ifdef FEAT_DIFF
		सचे (lnum == wp->w_topline)
		    पङ्क्तिः_size = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		अञ्ञथा
#endif
		    पङ्क्तिः_size = plines_win(wp, lnum, TRUE);
		sline -= पङ्क्तिः_size;
	    }

	    सचे (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, रिक्त, &lnum, TRUE, रिक्त);
#endif
		lnum++;
		wp->w_wrow -= पङ्क्तिः_size + sline;
	    }
	    अञ्ञथा सचे (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	set_topline(wp, lnum);
    }

    सचे (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    सचे (prev_height > 0)
	wp->w_prev_fraction_row = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    शून्य
win_new_width(win_T *wp, पूर्णाङ्क width)
{
    // Should we give an error if width < 0?
    wp->w_width = width < 0 ? 0 : width;
    wp->w_पङ्क्तयः_valid = 0;
    changed_पङ्क्तिः_abv_curs_win(wp);
    invalidate_botline_win(wp);

    सचे (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_status = true;
}

    शून्य
win_comp_scroll(win_T *wp)
{
#सचे defined(FEAT_EVAL)
    पूर्णाङ्क old_w_p_scr = wp->w_p_scr;
#endif

    wp->w_p_scr = ((unsigned)wp->w_height >> 1);
    सचे (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#सचे defined(FEAT_EVAL)
    सचे (wp->w_p_scr != old_w_p_scr)
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
    शून्य
command_height(शून्य)
{
    पूर्णाङ्क		old_p_ch = curtab->tp_ch_used;

    // Find bottom frame with width of screen.
    frame_T *frp = lastwin->w_frame;
    याव (frp->fr_width != topframe->fr_width && frp->fr_parent != रिक्त)
	frp = frp->fr_parent;

    // Avoid changing the height of a window with 'winfixheight' set.
    याव (frp->fr_prev != रिक्त && frp->fr_layout == FR_LEAF
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    याव (p_ch > old_p_ch && command_frame_height)
    {
	सचे (frp == रिक्त)
	{
	    emsg(_(e_not_enough_room));
	    p_ch = old_p_ch;
	    भङ्ग;
	}
	पूर्णाङ्क h = MIN(p_ch - old_p_ch,
			frp->fr_height - frame_minheight(frp, रिक्त));
	frame_add_height(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    सचे (p_ch < old_p_ch && command_frame_height && frp != रिक्त)
	frame_add_height(frp, (पूर्णाङ्क)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_row = Rows - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    सचे (सन्देशः_scrolled == 0 && full_screen)
    {
	screen_fill(cmdline_row, (पूर्णाङ्क)Rows, 0, (पूर्णाङ्क)Columns, ' ', ' ', 0);
	सन्देशः_row = cmdline_row;
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
    स्थिर शून्य
frame_add_height(frame_T *frp, पूर्णाङ्क n)
{
    frame_new_height(frp, frp->fr_height + n, FALSE, FALSE, FALSE);
    अत्थाय (;;)
    {
	frp = frp->fr_parent;
	सचे (frp == रिक्त)
	    भङ्ग;
	frp->fr_height += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    शून्य
last_status(
    पूर्णाङ्क		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    last_status_rec(topframe, last_stl_height(morewin) > 0);
#सचे defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

    स्थिर शून्य
last_status_rec(frame_T *fr, पूर्णाङ्क statusline)
{
    frame_T	*fp;
    win_T	*wp;

    सचे (fr->fr_layout == FR_LEAF)
    {
	wp = fr->fr_win;
	सचे (wp->w_status_height != 0 && !statusline)
	{
	    // remove status line
	    win_new_height(wp, wp->w_height + wp->w_status_height);
	    wp->w_status_height = 0;
	    comp_col();
	}
	अञ्ञथा सचे (wp->w_status_height == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    याव (fp->fr_height <= frame_minheight(fp, रिक्त))
	    {
		सचे (fp == topframe)
		{
		    emsg(_(e_not_enough_room));
		    पटिनिवत्त;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		सचे (fp->fr_parent->fr_layout == FR_COL && fp->fr_prev != रिक्त)
		    fp = fp->fr_prev;
		अञ्ञथा
		    fp = fp->fr_parent;
	    }
	    wp->w_status_height = statusline_height(wp);
	    सचे (fp != fr)
	    {
		frame_new_height(fp, fp->fr_height - wp->w_status_height,
			FALSE, FALSE, FALSE);
		frame_fix_height(wp);
		win_comp_pos();
	    }
	    अञ्ञथा
		win_new_height(wp, wp->w_height - wp->w_status_height);
	    comp_col();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	सचे (abs(wp->w_height - wp->w_prev_height) == 1)
	    wp->w_prev_height = wp->w_height;
    }
    अञ्ञथा सचे (fr->fr_layout == FR_ROW)
    {
	// vertically split windows, set status line for each one
	सचे (!statusline)
	{
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		last_status_rec(fp, statusline);
	}
	अञ्ञथा
	{
	    frame_T	*fp2;
	    पूर्णाङ्क		max_stlh = 0;
	    पूर्णाङ्क		new_row_height;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		सचे (fp->fr_win != रिक्त && fp->fr_win->w_status_height == 0)
		{
		    पूर्णाङ्क h = statusline_height(fp->fr_win);
		    सचे (h > max_stlh)
			max_stlh = h;
		}
	    सचे (max_stlh == 0)
		पटिनिवत्त;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    याव (fp2->fr_height - frame_minheight(fp2, रिक्त) < max_stlh)
	    {
		सचे (fp2 == topframe)
		{
		    emsg(_(e_not_enough_room));
		    पटिनिवत्त;
		}
		सचे (fp2->fr_parent->fr_layout == FR_COL
						&& fp2->fr_prev != रिक्त)
		    fp2 = fp2->fr_prev;
		अञ्ञथा
		    fp2 = fp2->fr_parent;
	    }

	    सचे (fp2 != fr)
	    {
		frame_new_height(fp2, fp2->fr_height - max_stlh,
							FALSE, FALSE, FALSE);
		new_row_height = fr->fr_height + max_stlh;
	    }
	    अञ्ञथा
		new_row_height = fr->fr_height;

	    // Set status and content heights for all leaves.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
	    {
		सचे (fp->fr_win != रिक्त)
		{
		    wp = fp->fr_win;
		    सचे (wp->w_status_height == 0)
		    {
			wp->w_status_height = statusline_height(wp);
			win_new_height(wp,
				  new_row_height - wp->w_status_height);
			frame_fix_height(wp);
			comp_col();
			redraw_all_later(UPD_SOME_VALID);
			सचे (abs(wp->w_height - wp->w_prev_height) == 1)
			    wp->w_prev_height = wp->w_height;
		    }
		}
		अञ्ञथा
		    last_status_rec(fp, statusline); // nested frames
	    }

	    fr->fr_height = new_row_height;
	    win_comp_pos();
	}
    }
    अञ्ञथा
    {
	// horizontally split window, set status line for last one
	अत्थाय (fp = fr->fr_child; fp->fr_next != रिक्त; fp = fp->fr_next)
	    ;
	last_status_rec(fp, statusline);
    }
}

#सचे defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    शून्य
update_stl_rendered_height(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == रिक्त) ? p_stl : wp->w_p_stl;
    पूर्णाङ्क		opt_flags = (wp == रिक्त) ? 0 : OPT_LOCAL;
    पूर्णाङ्क		*height = (wp == रिक्त) ? &stl_rendered_height
						: &wp->w_stl_rendered_height;

    *height = get_stl_rendered_height(wp == रिक्त ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    स्थिर पूर्णाङ्क
frame_find_global_stlh_rec(frame_T *frp, पूर्णाङ्क h)
{
    सचे (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	सचे (wp->w_height > 0 && wp->w_status_height > 0
		&& *wp->w_p_stlo == NUL)
	{
	    पूर्णाङ्क win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    सचे (win_free_height - p_wmh < h)
		h = win_free_height - p_wmh;
	}
    }
    अञ्ञथा
    {
	अत्थाय (frp = frp->fr_child; frp != रिक्त; frp = frp->fr_next)
	    h = frame_find_global_stlh_rec(frp, h);
    }
    पटिनिवत्त h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    स्थिर शून्य
frame_change_statusline_height_rec(frame_T *frp, पूर्णाङ्क global_stlh)
{
    सचे (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	सचे (wp->w_height > 0 && wp->w_status_height > 0)
	{
	    पूर्णाङ्क win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    wp->w_status_height = statusline_height(wp);
	    सचे (wp->w_p_stlo == रिक्त || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		सचे (wp->w_status_height > global_stlh)
		    wp->w_status_height = global_stlh;
	    }
	    सचे (wp->w_status_height > win_free_height - p_wmh)
		wp->w_status_height = win_free_height - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    पूर्णाङ्क	     saved_topfill = wp->w_topfill;
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
    अञ्ञथा
    {
	अत्थाय (frp = frp->fr_child; frp != रिक्त; frp = frp->fr_next)
	    frame_change_statusline_height_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    पूर्णाङ्क
frame_change_statusline_height(शून्य)
{
    tabpage_T	*tp;
    पूर्णाङ्क		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = frame_find_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	frame_change_statusline_height_rec(tp->tp_topframe, global_stlh);
    comp_col();
    redraw_all_later(UPD_SOME_VALID);
    पटिनिवत्त global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    शून्य
set_stlo_mh(पूर्णाङ्क mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    पूर्णाङ्क
statuslineopt_changed(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    पूर्णाङ्क		l_stlo_fh = FALSE;
    पूर्णाङ्क		l_stlo_mh = 1;

    सचे (stlopt != रिक्त)
	p = stlopt;
    अञ्ञथा
	p = wp->w_p_stlo;

    याव (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	सचे (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    सचे (l_stlo_mh < 1)
		पटिनिवत्त FAIL;
	}
	अञ्ञथा सचे (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	सचे (*p != ',' && *p != NUL)
	    पटिनिवत्त FAIL;
	सचे (*p == ',')
	    ++p;
    }

    सचे (wp == रिक्त)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    अञ्ञथा
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    पटिनिवत्त OK;
}
#endif

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    पूर्णाङ्क
statusline_height(win_T *wp UNUSED)
{
    पूर्णाङ्क stl_height = 1;
#सचे defined(FEAT_STL_OPT)
    bool fixed;
    पूर्णाङ्क l_stl_rendered_height;

    सचे (wp != रिक्त && *wp->w_p_stlo != NUL)
    {
	stl_height = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_height = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }
    अञ्ञथा
    {
	stl_height = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_height = (wp != रिक्त && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    सचे (!fixed)
    {
	पूर्णाङ्क has_custom_stl = (*p_stl != NUL)
	    || (wp != रिक्त && *wp->w_p_stl != NUL);
	सचे (has_custom_stl)
	    stl_height = MAX(1, MIN(l_stl_rendered_height, stl_height));
	अञ्ञथा
	    stl_height = 1;
    }
#endif

    पटिनिवत्त stl_height;
}

/*
 * Return the number of lines used by the tab page line.
 */
    पूर्णाङ्क
tabline_height(शून्य)
{
#ifdef FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    सचे (gui_use_tabline())
	पटिनिवत्त 0;
#endif
    विकल्प (p_stal)
    {
	पक्ष 0: पटिनिवत्त 0;
	पक्ष 1: पटिनिवत्त (first_tabpage->tp_next == रिक्त) ? 0 : 1;
    }
    पटिनिवत्त 1;
}

/*
 * Return the height of the last window's statusline.
 */
    पूर्णाङ्क
last_stl_height(
    पूर्णाङ्क		morewin)	// pretend there are two or more windows
{
    पटिनिवत्त (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_WINDOW)))
		? STATUS_HEIGHT : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    पूर्णाङ्क
min_rows(शून्य)
{
    सचे (firstwin == रिक्त)	// not initialized yet
	पटिनिवत्त MIN_पङ्क्तयः;

    पटिनिवत्त frame_minheight(curtab->tp_topframe, रिक्त) + tabline_height()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    पूर्णाङ्क
min_rows_for_all_tabpages(शून्य)
{
    पूर्णाङ्क		total;
    tabpage_T	*tp;
    पूर्णाङ्क		n;

    सचे (firstwin == रिक्त)	// not initialized yet
	पटिनिवत्त MIN_पङ्क्तयः;

    total = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = frame_minheight(tp->tp_topframe, रिक्त);
	सचे (total < n)
	    total = n;
    }
    total += tabline_height();
    total += MIN_CMDHEIGHT;
    पटिनिवत्त total;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    पूर्णाङ्क
only_one_window(शून्य)
{
    पूर्णाङ्क		गणनम् = 0;
    win_T	*wp;

#सचे defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    सचे (popup_is_popup(curwin))
	पटिनिवत्त FALSE;
#endif

    // If there is another tab page there always is another window.
    सचे (first_tabpage->tp_next != रिक्त)
	पटिनिवत्त FALSE;

    FOR_ALL_WINDOWS(wp)
	सचे (wp->w_buffer != रिक्त
		&& (!((bt_help(wp->w_buffer) && !bt_help(curbuf))
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++गणनम्;
    पटिनिवत्त (गणनम् <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    स्थिर शून्य
check_lnums_both(पूर्णाङ्क do_curwin, पूर्णाङ्क nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	सचे ((do_curwin || wp != curwin) && wp->w_buffer == curbuf)
	{
	    पूर्णाङ्क need_adjust;

	    सचे (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	    सचे (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	    सचे (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	    सचे (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	    सचे (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    शून्य
check_lnums(पूर्णाङ्क do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    शून्य
check_lnums_nested(पूर्णाङ्क do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    शून्य
reset_lnums(शून्य)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	सचे (wp->w_buffer == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    सचे (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    सचे (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    सचे (wp->w_save_cursor.w_topline_save > wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
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
    पूर्णाङ्क
make_snapshot(पूर्णाङ्क idx)
{
    clear_snapshot(curtab, idx);
    सचे (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
}

    स्थिर पूर्णाङ्क
make_snapshot_rec(frame_T *fr, frame_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(frame_T);
    सचे (*frp == रिक्त)
	पटिनिवत्त FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_width = fr->fr_width;
    (*frp)->fr_height = fr->fr_height;
    सचे (fr->fr_next != रिक्त)
    {
	सचे (make_snapshot_rec(fr->fr_next, &((*frp)->fr_next)) == FAIL)
	    पटिनिवत्त FAIL;
    }
    सचे (fr->fr_child != रिक्त)
    {
	सचे (make_snapshot_rec(fr->fr_child, &((*frp)->fr_child)) == FAIL)
	    पटिनिवत्त FAIL;
    }
    सचे (fr->fr_layout == FR_LEAF && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    पटिनिवत्त OK;
}

/*
 * Remove any existing snapshot.
 */
    स्थिर शून्य
clear_snapshot(tabpage_T *tp, पूर्णाङ्क idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = रिक्त;
}

    स्थिर शून्य
clear_snapshot_rec(frame_T *fr)
{
    सचे (fr == रिक्त)
	पटिनिवत्त;
    clear_snapshot_rec(fr->fr_next);
    clear_snapshot_rec(fr->fr_child);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    स्थिर win_T *
get_snapshot_curwin_rec(frame_T *ft)
{
    win_T	*wp;

    सचे (ft->fr_next != रिक्त)
    {
	सचे ((wp = get_snapshot_curwin_rec(ft->fr_next)) != रिक्त)
	    पटिनिवत्त wp;
    }
    सचे (ft->fr_child != रिक्त)
    {
	सचे ((wp = get_snapshot_curwin_rec(ft->fr_child)) != रिक्त)
	    पटिनिवत्त wp;
    }

    पटिनिवत्त ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    स्थिर win_T *
get_snapshot_curwin(पूर्णाङ्क idx)
{
    सचे (curtab->tp_snapshot[idx] == रिक्त)
	पटिनिवत्त रिक्त;

    पटिनिवत्त get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    शून्य
restore_snapshot(
    पूर्णाङ्क		idx,
    पूर्णाङ्क		close_curwin)	    // closing current window
{
    win_T	*wp;

    सचे (curtab->tp_snapshot[idx] != रिक्त
	    && curtab->tp_snapshot[idx]->fr_width == topframe->fr_width
	    && curtab->tp_snapshot[idx]->fr_height == topframe->fr_height
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	सचे (wp != रिक्त && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    स्थिर पूर्णाङ्क
check_snapshot_rec(frame_T *sn, frame_T *fr)
{
    सचे (sn->fr_layout != fr->fr_layout
	    || (sn->fr_next == रिक्त) != (fr->fr_next == रिक्त)
	    || (sn->fr_child == रिक्त) != (fr->fr_child == रिक्त)
	    || (sn->fr_next != रिक्त
		&& check_snapshot_rec(sn->fr_next, fr->fr_next) == FAIL)
	    || (sn->fr_child != रिक्त
		&& check_snapshot_rec(sn->fr_child, fr->fr_child) == FAIL)
	    || (sn->fr_win != रिक्त && !win_valid(sn->fr_win)))
	पटिनिवत्त FAIL;
    पटिनिवत्त OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    स्थिर win_T *
restore_snapshot_rec(frame_T *sn, frame_T *fr)
{
    win_T	*wp = रिक्त;
    win_T	*wp2;

    fr->fr_height = sn->fr_height;
    fr->fr_width = sn->fr_width;
    सचे (fr->fr_layout == FR_LEAF)
    {
	frame_new_height(fr, fr->fr_height, FALSE, FALSE, FALSE);
	frame_new_width(fr, fr->fr_width, FALSE, FALSE);
	wp = sn->fr_win;
    }
    सचे (sn->fr_next != रिक्त)
    {
	wp2 = restore_snapshot_rec(sn->fr_next, fr->fr_next);
	सचे (wp2 != रिक्त)
	    wp = wp2;
    }
    सचे (sn->fr_child != रिक्त)
    {
	wp2 = restore_snapshot_rec(sn->fr_child, fr->fr_child);
	सचे (wp2 != रिक्त)
	    wp = wp2;
    }
    पटिनिवत्त wp;
}

#सचे defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    पूर्णाङ्क
win_hasvertsplit(शून्य)
{
    frame_T	*fr;

    सचे (topframe->fr_layout == FR_ROW)
	पटिनिवत्त TRUE;

    सचे (topframe->fr_layout == FR_COL)
	FOR_ALL_FRAMES(fr, topframe->fr_child)
	    सचे (fr->fr_layout == FR_ROW)
		पटिनिवत्त TRUE;

    पटिनिवत्त FALSE;
}
#endif

#सचे defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    पूर्णाङ्क
get_win_सङ्ख्या(win_T *wp, win_T *first_win)
{
    पूर्णाङ्क		i = 1;
    win_T	*w;

    अत्थाय (w = first_win; w != रिक्त && w != wp; w = W_NEXT(w))
	++i;

    सचे (w == रिक्त)
	पटिनिवत्त 0;
    अञ्ञथा
	पटिनिवत्त i;
}

    पूर्णाङ्क
get_tab_सङ्ख्या(tabpage_T *tp)
{
    पूर्णाङ्क		i = 1;
    tabpage_T	*t;

    अत्थाय (t = first_tabpage; t != रिक्त && t != tp; t = t->tp_next)
	++i;

    सचे (t == रिक्त)
	पटिनिवत्त 0;
    अञ्ञथा
	पटिनिवत्त i;
}
#endif

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    स्थिर पूर्णाङ्क
frame_check_height(frame_T *topfrp, पूर्णाङ्क height)
{
    frame_T *frp;

    सचे (topfrp->fr_height != height)
	पटिनिवत्त FALSE;

    सचे (topfrp->fr_layout == FR_ROW)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    सचे (frp->fr_height != height)
		पटिनिवत्त FALSE;

    पटिनिवत्त TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    स्थिर पूर्णाङ्क
frame_check_width(frame_T *topfrp, पूर्णाङ्क width)
{
    frame_T *frp;

    सचे (topfrp->fr_width != width)
	पटिनिवत्त FALSE;

    सचे (topfrp->fr_layout == FR_COL)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    सचे (frp->fr_width != width)
		पटिनिवत्त FALSE;

    पटिनिवत्त TRUE;
}

#सचे defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    स्थिर पूर्णाङ्क
int_cmp(ध्रुव शून्य *pa, ध्रुव शून्य *pb)
{
    ध्रुव पूर्णाङ्क a = *(ध्रुव पूर्णाङ्क *)pa;
    ध्रुव पूर्णाङ्क b = *(ध्रुव पूर्णाङ्क *)pb;
    सचे (a > b)
	पटिनिवत्त 1;
    सचे (a < b)
	पटिनिवत्त -1;
    पटिनिवत्त 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    वर्ण *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = empty_option;
    पूर्णाङ्क		tw;
    पूर्णाङ्क		col;
    पूर्णाङ्क		गणनम् = 0;
    पूर्णाङ्क		color_cols[256];
    पूर्णाङ्क		i;
    पूर्णाङ्क		j = 0;

    सचे (wp != रिक्त && wp->w_buffer == रिक्त)
	पटिनिवत्त रिक्त;  // buffer was closed

    सचे (cc != रिक्त)
	s = cc;
    अञ्ञथा सचे (wp != रिक्त)
	s = wp->w_p_cc;

    सचे (wp != रिक्त)
	tw = wp->w_buffer->b_p_tw;
    अञ्ञथा
	// buffer-local value not set, assume zero
	tw = 0;

    याव (*s != NUL && गणनम् < 255)
    {
	सचे (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    col = (*s == '-') ? -1 : 1;
	    ++s;
	    सचे (!VIM_ISDIGIT(*s))
		पटिनिवत्त e_invalid_argument;
	    col = col * getdigits(&s);
	    सचे (tw == 0)
		गच्छ skip;  // 'textwidth' not set, skip this item
	    col += tw;
	    सचे (col < 0)
		गच्छ skip;
	}
	अञ्ञथा सचे (VIM_ISDIGIT(*s))
	    col = getdigits(&s);
	अञ्ञथा
	    पटिनिवत्त e_invalid_argument;
	color_cols[गणनम्++] = col - 1;  // 1-based to 0-based
skip:
	सचे (*s == NUL)
	    भङ्ग;
	सचे (*s != ',')
	    पटिनिवत्त e_invalid_argument;
	सचे (*++s == NUL)
	    पटिनिवत्त e_invalid_argument;  // illegal trailing comma as in "set cc=80,"
    }

    सचे (wp == रिक्त)
	पटिनिवत्त रिक्त;  // only parse "cc"

    vim_free(wp->w_p_cc_cols);
    सचे (गणनम् == 0)
	wp->w_p_cc_cols = रिक्त;
    अञ्ञथा
    {
	wp->w_p_cc_cols = ALLOC_MULT(पूर्णाङ्क, गणनम् + 1);
	सचे (wp->w_p_cc_cols != रिक्त)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_cols, गणनम्, परिमाण(पूर्णाङ्क), int_cmp);

	    अत्थाय (i = 0; i < गणनम्; ++i)
		// skip duplicates
		सचे (j == 0 || wp->w_p_cc_cols[j - 1] != color_cols[i])
		    wp->w_p_cc_cols[j++] = color_cols[i];
	    wp->w_p_cc_cols[j] = -1;  // end marker
	}
    }

    पटिनिवत्त रिक्त;  // no error
}
#endif

    पूर्णाङ्क
get_last_winid(शून्य)
{
    पटिनिवत्त last_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   पूर्णाङ्क
win_locked(win_T *wp)
{
    पटिनिवत्त wp->w_locked;
}
