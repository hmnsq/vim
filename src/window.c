/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#अन्तर्भाव "vim.h"

स्थिर शून्य cmd_with_मोजणी(अक्षर *cmd, char_u *bufp, माप_प्रकार bufsize, दीर्घ Prenum);
स्थिर शून्य win_init(win_T *newp, win_T *oldp, पूर्णांक flags);
स्थिर शून्य win_init_some(win_T *newp, win_T *oldp);
स्थिर शून्य frame_comp_pos(frame_T *topfrp, पूर्णांक *row, पूर्णांक *col);
स्थिर शून्य frame_setheight(frame_T *curfrp, पूर्णांक height);
स्थिर शून्य frame_setwidth(frame_T *curfrp, पूर्णांक width);
स्थिर शून्य win_exchange(दीर्घ);
स्थिर शून्य win_rotate(पूर्णांक, पूर्णांक);
स्थिर शून्य win_equal_rec(win_T *next_curwin, पूर्णांक current, frame_T *topfr, पूर्णांक dir, पूर्णांक col, पूर्णांक row, पूर्णांक width, पूर्णांक height);
स्थिर शून्य trigger_winnewpre(शून्य);
स्थिर शून्य trigger_winclosed(win_T *win);
स्थिर win_T *win_free_mem(win_T *win, पूर्णांक *dirp, tabpage_T *tp);
स्थिर frame_T *win_altframe(win_T *win, tabpage_T *tp);
स्थिर tabpage_T *alt_tabpage(शून्य);
स्थिर win_T *frame2win(frame_T *frp);
स्थिर पूर्णांक frame_has_win(frame_T *frp, win_T *wp);
स्थिर शून्य win_fix_scroll(पूर्णांक resize);
स्थिर शून्य win_fix_cursor(पूर्णांक normal);
स्थिर शून्य frame_new_height(frame_T *topfrp, पूर्णांक height, पूर्णांक topfirst, पूर्णांक wfh, पूर्णांक set_ch);
स्थिर पूर्णांक frame_fixed_height(frame_T *frp);
स्थिर पूर्णांक frame_fixed_width(frame_T *frp);
स्थिर शून्य frame_add_statusline(frame_T *frp);
स्थिर शून्य frame_new_width(frame_T *topfrp, पूर्णांक width, पूर्णांक leftfirst, पूर्णांक wfw);
स्थिर शून्य frame_set_vsep(frame_T *frp, पूर्णांक add);
स्थिर पूर्णांक frame_minwidth(frame_T *topfrp, win_T *next_curwin);
स्थिर शून्य frame_fix_width(win_T *wp);
स्थिर पूर्णांक win_alloc_firstwin(win_T *oldwin);
स्थिर शून्य new_frame(win_T *wp);
स्थिर tabpage_T *alloc_tabpage(शून्य);
स्थिर पूर्णांक leave_tabpage(buf_T *new_curbuf, पूर्णांक trigger_leave_autocmds);
स्थिर शून्य enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, पूर्णांक trigger_enter_autocmds, पूर्णांक trigger_leave_autocmds);
स्थिर शून्य frame_fix_height(win_T *wp);
स्थिर पूर्णांक frame_minheight(frame_T *topfrp, win_T *next_curwin);
स्थिर पूर्णांक frame_wincount_in_height(frame_T *topfrp);
स्थिर पूर्णांक may_open_tabpage(शून्य);
स्थिर पूर्णांक win_enter_ext(win_T *wp, पूर्णांक flags);
स्थिर शून्य win_free(win_T *wp, tabpage_T *tp);
स्थिर शून्य win_append(win_T *after, win_T *wp);
स्थिर शून्य frame_append(frame_T *after, frame_T *frp);
स्थिर शून्य frame_घाला(frame_T *before, frame_T *frp);
स्थिर शून्य frame_remove(frame_T *frp);
स्थिर शून्य win_goto_ver(पूर्णांक up, दीर्घ मोजणी);
स्थिर शून्य win_goto_hor(पूर्णांक डावा, दीर्घ मोजणी);
स्थिर शून्य frame_add_height(frame_T *frp, पूर्णांक n);
स्थिर शून्य last_status_rec(frame_T *fr, पूर्णांक statusline);
#जर defined(FEAT_STL_OPT)
स्थिर पूर्णांक frame_find_global_stlh_rec(frame_T *frp, पूर्णांक h);
स्थिर शून्य frame_change_statusline_height_rec(frame_T *frp, पूर्णांक global_stlh);
#endif
स्थिर शून्य frame_flatten(frame_T *frp);
स्थिर शून्य winframe_restore(win_T *wp, पूर्णांक dir, frame_T *unflat_altfr);

स्थिर पूर्णांक make_snapshot_rec(frame_T *fr, frame_T **frp);
स्थिर शून्य clear_snapshot(tabpage_T *tp, पूर्णांक idx);
स्थिर शून्य clear_snapshot_rec(frame_T *fr);
स्थिर पूर्णांक check_snapshot_rec(frame_T *sn, frame_T *fr);
स्थिर win_T *restore_snapshot_rec(frame_T *sn, frame_T *fr);
स्थिर win_T *get_snapshot_curwin(पूर्णांक idx);

स्थिर पूर्णांक frame_check_height(frame_T *topfrp, पूर्णांक height);
स्थिर पूर्णांक frame_check_width(frame_T *topfrp, पूर्णांक width);

स्थिर win_T *win_alloc(win_T *after, पूर्णांक hidden);

#define NOWIN		((win_T *)-1)	// non-existing window

#define ROWS_AVAIL (Rows - p_ch - tabline_height())

// flags for win_enter_ext()
#define WEE_UNDO_SYNC			0x01
#define WEE_CURWIN_INVALID		0x02
#define WEE_TRIGGER_NEW_AUTOCMDS	0x04
#define WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#define WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#define WEE_ALLOW_PARSE_MESSAGES	0x20

स्थिर अक्षर *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
स्थिर पूर्णांक split_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
स्थिर पूर्णांक close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
स्थिर पूर्णांक frame_locked = 0;

#जर defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
स्थिर पूर्णांक stl_rendered_height = 1;
स्थिर पूर्णांक stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
स्थिर पूर्णांक stlo_mh = STATUS_HEIGHT; // "maxheight" value cache of global 'stlo'
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

    पूर्णांक
frames_locked(शून्य)
{
    परत frame_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    पूर्णांक
window_layout_locked(गणना CMD_index cmd)
{
    जर (split_disallowed > 0 || close_disallowed > 0)
    {
	जर (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_split_window_when_closing_buffer));
	अन्यथा
	    emsg(_(e_not_allowed_to_change_window_layout_in_this_autocmd));
	परत TRUE;
    }
    परत FALSE;
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
    जर (frame->fr_child != रिक्त)
    {
	ch_log(रिक्त, "children");
	log_frame_layout(frame->fr_child);
	जर (frame->fr_next != रिक्त)
	    ch_log(रिक्त, "END of children");
    }
    जर (frame->fr_next != रिक्त)
	log_frame_layout(frame->fr_next);
}
#endif

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    पूर्णांक
check_can_set_curbuf_disabled(शून्य)
{
    जर (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	परत FALSE;
    }
    परत TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    पूर्णांक
check_can_set_curbuf_forceit(पूर्णांक forceit)
{
    जर (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	परत FALSE;
    }
    परत TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(शून्य)
{
    // In cmdwin, the alternative buffer should be used.
    परत is_in_cmdwin() && prevwin != रिक्त ? prevwin : curwin;
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

    जर (buf == रिक्त)
	परत wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    जर (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    जर (wp == रिक्त && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    परत wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    शून्य
do_window(
    पूर्णांक		nchar,
    दीर्घ	Prenum,
    पूर्णांक		xchar)	    // extra char from ":wincmd gx" or NUL
{
    दीर्घ	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ifdef FEAT_FIND_ID
    पूर्णांक		type = FIND_DEFINE;
    पूर्णांक		len;
#endif
    char_u	cbuf[40];

    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;

#define CHECK_CMDWIN \
    करा { \
	जर (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_in_cmdline_window)); \
	    परत; \
	} \
    } पर्यंत (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    निवड (nchar)
    {
// split current window in two parts, horizontally
    स्थिती 'S':
    स्थिती Ctrl_S:
    स्थिती 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		जर (bt_quickfix(curbuf))
		    जा newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(शून्य)win_split((पूर्णांक)Prenum, 0);
		थांबा;

// split current window in two parts, vertically
    स्थिती Ctrl_V:
    स्थिती 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		जर (bt_quickfix(curbuf))
		    जा newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(शून्य)win_split((पूर्णांक)Prenum, WSP_VERT);
		थांबा;

// split current window and edit alternate file
    स्थिती Ctrl_HAT:
    स्थिती '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		जर (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == रिक्त)
		{
		    जर (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    अन्यथा
			semsg(_(e_buffer_nr_not_found), Prenum);
		    थांबा;
		}

		जर (!curbuf_locked() && win_split(0, 0) == OK)
		    (शून्य)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		थांबा;

// open new window
    स्थिती Ctrl_N:
    स्थिती 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		जर (Prenum)
		    // window height
		    vim_snprintf((अक्षर *)cbuf, आकार(cbuf) - 5, "%ld", Prenum);
		अन्यथा
		    cbuf[0] = NUL;
#जर defined(FEAT_QUICKFIX)
		जर (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#endif
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		थांबा;

// quit current window
    स्थिती Ctrl_Q:
    स्थिती 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_मोजणी("quit", cbuf, आकार(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		थांबा;

// close current window
    स्थिती Ctrl_C:
    स्थिती 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_मोजणी("close", cbuf, आकार(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		थांबा;

#जर defined(FEAT_QUICKFIX)
// close preview window
    स्थिती Ctrl_Z:
    स्थिती 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		थांबा;

// cursor to preview window
    स्थिती 'P':
		FOR_ALL_WINDOWS(wp)
		    जर (wp->w_p_pvw)
			थांबा;
		जर (wp == रिक्त)
		    emsg(_(e_there_is_no_preview_window));
		अन्यथा
		    win_goto(wp);
		थांबा;
#endif

// close all but current window
    स्थिती Ctrl_O:
    स्थिती 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_मोजणी("only", cbuf, आकार(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		थांबा;

// cursor to next window with wrap around
    स्थिती Ctrl_W:
    स्थिती 'w':
// cursor to previous window with wrap around
    स्थिती 'W':
		CHECK_CMDWIN;
		जर (ONE_WINDOW && Prenum != 1)	// just one window
		    beep_flush();
		अन्यथा
		{
		    जर (Prenum)			// go to specified window
		    {
			साठी (wp = firstwin; --Prenum > 0; )
			{
			    जर (wp->w_next == रिक्त)
				थांबा;
			    अन्यथा
				wp = wp->w_next;
			}
		    }
		    अन्यथा
		    {
			जर (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    जर (wp == रिक्त)
				wp = lastwin;	    // wrap around
			}
			अन्यथा			    // go to next window
			{
			    wp = curwin->w_next;
			    जर (wp == रिक्त)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		थांबा;

// cursor to window below
    स्थिती 'j':
    स्थिती K_DOWN:
    स्थिती Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		थांबा;

// cursor to window above
    स्थिती 'k':
    स्थिती K_UP:
    स्थिती Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		थांबा;

// cursor to left window
    स्थिती 'h':
    स्थिती K_डावा:
    स्थिती Ctrl_H:
    स्थिती K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		थांबा;

// cursor to right window
    स्थिती 'l':
    स्थिती K_उजवा:
    स्थिती Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		थांबा;

// move window to new tab page
    स्थिती 'T':
		CHECK_CMDWIN;
		जर (one_window())
		    संदेश(_(m_onlyone));
		अन्यथा
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    जर (win_new_tabpage((पूर्णांक)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			जर (curwin == wp)
			    win_close(curwin, FALSE);
			जर (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		थांबा;

// cursor to top-left window
    स्थिती 't':
    स्थिती Ctrl_T:
		win_goto(firstwin);
		थांबा;

// cursor to bottom-right window
    स्थिती 'b':
    स्थिती Ctrl_B:
		win_goto(lastwin);
		थांबा;

// cursor to last accessed (previous) window
    स्थिती 'p':
    स्थिती Ctrl_P:
		जर (!win_valid(prevwin))
		    beep_flush();
		अन्यथा
		    win_goto(prevwin);
		थांबा;

// exchange current and next window
    स्थिती 'x':
    स्थिती Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		थांबा;

// rotate windows downwards
    स्थिती Ctrl_R:
    स्थिती 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (पूर्णांक)Prenum1);    // downwards
		थांबा;

// rotate windows upwards
    स्थिती 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (पूर्णांक)Prenum1);	    // upwards
		थांबा;

// move window to the very top/bottom/left/right
    स्थिती 'K':
    स्थिती 'J':
    स्थिती 'H':
    स्थिती 'L':
		CHECK_CMDWIN;
		जर (ONE_WINDOW)
		    beep_flush();
		अन्यथा
		{
		    पूर्णांक dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_शीर्ष : WSP_BOT);

		    (शून्य)win_splitmove(curwin, (पूर्णांक)Prenum, dir);
		}
		थांबा;

// make all windows the same width and/or height
    स्थिती '=':
		{
		    पूर्णांक mod = cmdmod.cmod_split & (WSP_VERT | WSP_HOR);
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_equal(रिक्त, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		थांबा;

// increase current window height
    स्थिती '+':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height + (पूर्णांक)Prenum1);
		थांबा;

// decrease current window height
    स्थिती '-':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height - (पूर्णांक)Prenum1);
		थांबा;

// set current window height
    स्थिती Ctrl__:
    स्थिती '_':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(Prenum ? (पूर्णांक)Prenum : 9999);
		थांबा;

// increase current window width
    स्थिती '>':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width + (पूर्णांक)Prenum1);
		थांबा;

// decrease current window width
    स्थिती '<':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width - (पूर्णांक)Prenum1);
		थांबा;

// set current window width
    स्थिती '|':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(Prenum != 0 ? (पूर्णांक)Prenum : 9999);
		थांबा;

// jump to tag and split window if tag exists (in preview window)
#जर defined(FEAT_QUICKFIX)
    स्थिती '}':
		CHECK_CMDWIN;
		जर (Prenum)
		    g_do_tagpreview = Prenum;
		अन्यथा
		    g_do_tagpreview = p_pvh;
#endif
		// FALLTHROUGH
    स्थिती ']':
    स्थिती Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		जर (Prenum)
		    postponed_split = Prenum;
		अन्यथा
		    postponed_split = -1;
#ifdef FEAT_QUICKFIX
		जर (nchar != '}')
		    g_do_tagpreview = 0;
#endif

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_split = 0;
		थांबा;

// edit file name under cursor in a new window
    स्थिती 'f':
    स्थिती 'F':
    स्थिती Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		जर (check_text_or_curbuf_locked(रिक्त))
		    थांबा;

		ptr = grab_file_name(Prenum1, &lnum);
		जर (ptr != रिक्त)
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
		    जर ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    जर (wp == रिक्त && win_split(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			जर (do_ecmd(0, ptr, रिक्त, रिक्त, ECMD_LASTL,
						   ECMD_HIDE, रिक्त) == FAIL)
			{
			    /*
			     * Note: if FEAT_EVAL is defined and do_ecmd() is aborted resulting
			     * in got_int be true, win_close() unconditionally fails. In such
			     * case, the window split for do_ecmd() is left unclosed, i.e. the
			     * current window is just duplicated. To avoid this, save and load
			     * got_int value before and after closing the window.
			     */
			    संक_पूर्ण_प्रकार    old_got_int = got_int;
			    got_int = FALSE;

			    // Failed to open the file, close the window
			    // opened for it.
			    win_close(curwin, FALSE);
			    got_int = got_int || old_got_int;

			    goto_tabpage_win(oldtab, oldwin);
			}
			अन्यथा
			    wp = curwin;
		    }

		    जर (wp != रिक्त && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		थांबा;

#ifdef FEAT_FIND_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    स्थिती 'i':			    // Go to any match
    स्थिती Ctrl_I:
		type = FIND_ANY;
		// FALLTHROUGH
    स्थिती 'd':			    // Go to definition, using 'define'
    स्थिती Ctrl_D:
		CHECK_CMDWIN;
		जर ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
		    थांबा;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		जर (ptr == रिक्त)
		    थांबा;

		find_pattern_in_path(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_SPLIT, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_set_curswant = true;
		थांबा;
#endif

// Quickfix window only: view the result under the cursor in a new split.
#जर defined(FEAT_QUICKFIX)
    स्थिती K_KENTER:
    स्थिती CAR:
		जर (bt_quickfix(curbuf))
		    qf_view_result(TRUE);
		थांबा;
#endif

// CTRL-W g  extended commands
    स्थिती 'g':
    स्थिती Ctrl_G:
		CHECK_CMDWIN;
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#endif
		++no_mapping;
		++allow_किल्ल्या;   // no mapping for xchar, but allow key codes
		जर (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_किल्ल्या;
		(शून्य)add_to_showcmd(xchar);

		निवड (xchar)
		{
#जर defined(FEAT_QUICKFIX)
		    स्थिती '}':
			xchar = Ctrl_RSB;
			जर (Prenum)
			    g_do_tagpreview = Prenum;
			अन्यथा
			    g_do_tagpreview = p_pvh;
#endif
			// FALLTHROUGH
		    स्थिती ']':
		    स्थिती Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			जर (Prenum)
			    postponed_split = Prenum;
			अन्यथा
			    postponed_split = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_split = 0;
			थांबा;

		    स्थिती 'f':	    // CTRL-W gf: "gf" in a new tab page
		    स्थिती 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_index(curtab) + 1;
			nchar = xchar;
			जा wingotofile;

		    स्थिती 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((पूर्णांक)Prenum);
			थांबा;

		    स्थिती 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(पूर्णांक)Prenum1);
			थांबा;

		    स्थिती TAB:	    // CTRL-W g<Tab>: go to last used tab page
			जर (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			थांबा;

		    पूर्वनिर्धारित:
			beep_flush();
			थांबा;
		}
		थांबा;

    पूर्वनिर्धारित:	beep_flush();
		थांबा;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    शून्य
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    निवड (*arg)
    {
    स्थिती 'S':
    स्थिती Ctrl_S:
    स्थिती 's':
    स्थिती Ctrl_N:
    स्थिती 'n':
    स्थिती 'j':
    स्थिती Ctrl_J:
    स्थिती 'k':
    स्थिती Ctrl_K:
    स्थिती 'T':
    स्थिती Ctrl_R:
    स्थिती 'r':
    स्थिती 'R':
    स्थिती 'K':
    स्थिती 'J':
    स्थिती '+':
    स्थिती '-':
    स्थिती Ctrl__:
    स्थिती '_':
    स्थिती '|':
    स्थिती ']':
    स्थिती Ctrl_RSB:
    स्थिती 'g':
    स्थिती Ctrl_G:
    स्थिती Ctrl_V:
    स्थिती 'v':
    स्थिती 'h':
    स्थिती Ctrl_H:
    स्थिती 'l':
    स्थिती Ctrl_L:
    स्थिती 'H':
    स्थिती 'L':
    स्थिती '>':
    स्थिती '<':
#जर defined(FEAT_QUICKFIX)
    स्थिती '}':
#endif
    स्थिती 'f':
    स्थिती 'F':
    स्थिती Ctrl_F:
#ifdef FEAT_FIND_ID
    स्थिती 'i':
    स्थिती Ctrl_I:
    स्थिती 'd':
    स्थिती Ctrl_D:
#endif
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		थांबा;

    स्थिती Ctrl_HAT:
    स्थिती '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		थांबा;

    स्थिती Ctrl_Q:
    स्थिती 'q':
    स्थिती Ctrl_C:
    स्थिती 'c':
    स्थिती Ctrl_O:
    स्थिती 'o':
    स्थिती Ctrl_W:
    स्थिती 'w':
    स्थिती 'W':
    स्थिती 'x':
    स्थिती Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		थांबा;

#जर defined(FEAT_QUICKFIX)
    स्थिती Ctrl_Z:
    स्थिती 'z':
    स्थिती 'P':
#endif
    स्थिती 't':
    स्थिती Ctrl_T:
    स्थिती 'b':
    स्थिती Ctrl_B:
    स्थिती 'p':
    स्थिती Ctrl_P:
    स्थिती '=':
    स्थिती CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		थांबा;
    }
}

    स्थिर शून्य
cmd_with_मोजणी(
    अक्षर	*cmd,
    char_u	*bufp,
    माप_प्रकार	bufsize,
    दीर्घ	Prenum)
{
    जर (Prenum > 0)
	vim_snprintf((अक्षर *)bufp, bufsize, "%s %ld", cmd, Prenum);
    अन्यथा
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    पूर्णांक
check_split_disallowed(win_T *wp)
{
    जर (split_disallowed > 0)
    {
	emsg(_(e_cant_split_window_while_closing_another));
	परत FAIL;
    }
    जर (wp->w_buffer->b_locked_split)
    {
	emsg(_(e_cannot_split_window_when_closing_buffer));
	परत FAIL;
    }
    परत OK;
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
    पूर्णांक
win_split(पूर्णांक size, पूर्णांक flags)
{
    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत FAIL;

    जर (check_split_disallowed(curwin) == FAIL)
	परत FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    जर (may_open_tabpage() == OK)
	परत OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_split;
    जर ((flags & WSP_शीर्ष) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_split_topleft_and_botright_at_the_same_time));
	परत FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    जर (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    अन्यथा
	clear_snapshot(curtab, SNAP_HELP_IDX);

    जर (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    अन्यथा
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    परत win_split_ins(size, flags, रिक्त, 0, रिक्त);
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
    पूर्णांक
win_split_ins(
    पूर्णांक		size,
    पूर्णांक		flags,
    win_T	*new_wp,
    पूर्णांक		dir,
    frame_T	*to_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    पूर्णांक		new_size = size;
    पूर्णांक		i;
    पूर्णांक		need_status = 0;
    पूर्णांक		do_equal = FALSE;
    पूर्णांक		needed;
    पूर्णांक		available;
    पूर्णांक		oldwin_height = 0;
    पूर्णांक		layout;
    frame_T	*frp, *curfrp, *frp2, *prevfrp;
    पूर्णांक		before;
    पूर्णांक		minheight;
    पूर्णांक		wmh1;
    पूर्णांक		did_set_fraction = FALSE;
    पूर्णांक		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    जर (new_wp == रिक्त)
	trigger_winnewpre();

    जर (flags & WSP_शीर्ष)
	oldwin = firstwin;
    अन्यथा जर (flags & WSP_BOT)
	oldwin = lastwin;
    अन्यथा
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    जर (ONE_WINDOW && p_ls == 1 && oldwin->w_status_height == 0)
    {
	जर (!(flags & WSP_FORCE_ROOM) && VISIBLE_HEIGHT(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_room));
	    जा theend;
	}
	need_status = statusline_height(oldwin);
    }

#ifdef FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    जर (gui.in_use)
	out_flush();
#endif

    जर (flags & WSP_VERT)
    {
	पूर्णांक	wmw1;
	पूर्णांक	minwidth;

	layout = FR_ROW;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	जर (flags & WSP_ROOM)
	    needed += p_wiw - wmw1;
	जर (flags & (WSP_BOT | WSP_शीर्ष))
	{
	    minwidth = frame_minwidth(topframe, NOWIN);
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	अन्यथा जर (p_ea)
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    prevfrp = oldwin->w_frame;
	    साठी (frp = oldwin->w_frame->fr_parent; frp != रिक्त;
							frp = frp->fr_parent)
	    {
		जर (frp->fr_layout == FR_ROW)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			जर (frp2 != prevfrp)
			    minwidth += frame_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	अन्यथा
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    available = oldwin->w_frame->fr_width;
	    needed += minwidth;
	}
	जर (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    जा theend;
	}
	जर (new_size == 0)
	    new_size = oldwin->w_width / 2;
	जर (new_size > available - minwidth - 1)
	    new_size = available - minwidth - 1;
	जर (new_size < wmw1)
	    new_size = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	जर (oldwin->w_width - new_size - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	जर (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_width + new_size + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	जर (!do_equal && p_ea && size == 0 && *p_ead != 'v'
	    && oldwin->w_frame->fr_parent != रिक्त)
	    do_equal = TRUE;
    }
    अन्यथा
    {
	layout = FR_COL;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_HEIGHT(curwin);
	needed = wmh1 + statusline_height(oldwin);
	जर (flags & WSP_ROOM)
	    needed += p_wh - wmh1;
	जर (flags & (WSP_BOT | WSP_शीर्ष))
	{
	    minheight = frame_minheight(topframe, NOWIN) + need_status;
	    available = topframe->fr_height;
	    needed += minheight;
	}
	अन्यथा जर (p_ea)
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    prevfrp = oldwin->w_frame;
	    साठी (frp = oldwin->w_frame->fr_parent; frp != रिक्त;
							frp = frp->fr_parent)
	    {
		जर (frp->fr_layout == FR_COL)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			जर (frp2 != prevfrp)
			    minheight += frame_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_height;
	    needed += minheight;
	}
	अन्यथा
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    available = oldwin->w_frame->fr_height;
	    needed += minheight;
	}
	जर (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    जा theend;
	}
	oldwin_height = oldwin->w_height;
	जर (need_status)
	{
	    oldwin->w_status_height = statusline_height(oldwin);
	    oldwin_height -= oldwin->w_status_height;
	}
	// Estimated stl height for the new window.
	पूर्णांक new_win_stlh = need_status != 0 ? need_status
			: statusline_height(new_wp != रिक्त ? new_wp : curwin);
	जर (new_size == 0)
	    new_size = (oldwin_height - new_win_stlh + 1) / 2;
	जर (new_size > available - minheight - new_win_stlh)
	    new_size = available - minheight - new_win_stlh;
	जर (new_size < wmh1)
	    new_size = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	जर (oldwin_height - new_size - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	जर (oldwin->w_p_wfh)
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
	जर (!do_equal && p_ea && size == 0 && *p_ead != 'h'
	    && oldwin->w_frame->fr_parent != रिक्त)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    जर ((flags & WSP_शीर्ष) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_BELOW)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	जर (new_wp == रिक्त)
	    wp = win_alloc(oldwin, FALSE);
	अन्यथा
	    win_append(oldwin, wp);
    }
    अन्यथा
    {
	जर (new_wp == रिक्त)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	अन्यथा
	    win_append(oldwin->w_prev, wp);
    }

    जर (new_wp == रिक्त)
    {
	जर (wp == रिक्त)
	    जा theend;

	new_frame(wp);
	जर (wp->w_frame == रिक्त)
	{
	    win_free(wp, रिक्त);
	    जा theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    जर (to_flatten != रिक्त)
	frame_flatten(to_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    जर (flags & (WSP_शीर्ष | WSP_BOT))
    {
	जर ((topframe->fr_layout == FR_COL && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ROW && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_child;
	    जर (flags & WSP_BOT)
		पर्यंत (curfrp->fr_next != रिक्त)
		    curfrp = curfrp->fr_next;
	}
	अन्यथा
	    curfrp = topframe;
	before = (flags & WSP_शीर्ष);
    }
    अन्यथा
    {
	curfrp = oldwin->w_frame;
	जर (flags & WSP_BELOW)
	    before = FALSE;
	अन्यथा जर (flags & WSP_ABOVE)
	    before = TRUE;
	अन्यथा जर (flags & WSP_VERT)
	    before = !p_spr;
	अन्यथा
	    before = !p_sb;
    }
    जर (curfrp->fr_parent == रिक्त || curfrp->fr_parent->fr_layout != layout)
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
	जर (frp->fr_win != रिक्त)
	    oldwin->w_frame = frp;
	अन्यथा
	    FOR_ALL_FRAMES(frp, frp->fr_child)
		frp->fr_parent = curfrp;
    }

    जर (new_wp == रिक्त)
	frp = wp->w_frame;
    अन्यथा
	frp = new_wp->w_frame;
    frp->fr_parent = curfrp->fr_parent;

    // Insert the new frame at the right place in the frame list.
    जर (before)
	frame_घाला(curfrp, frp);
    अन्यथा
	frame_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    जर (!did_set_fraction)
	set_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    जर (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	जर (need_status)
	{
	    win_new_height(oldwin, oldwin->w_height - need_status);
	    oldwin->w_status_height = need_status;
	}
	जर (flags & (WSP_शीर्ष | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    पूर्णांक stl_height = p_ls > 0 ? statusline_height(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_height();
	    win_new_height(wp, curfrp->fr_height - stl_height
							  - WINBAR_HEIGHT(wp));
	    wp->w_status_height = stl_height;
	}
	अन्यथा
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
	जर (before)
	    wp->w_vsep_width = 1;
	अन्यथा
	{
	    wp->w_vsep_width = oldwin->w_vsep_width;
	    oldwin->w_vsep_width = 1;
	}
	जर (flags & (WSP_शीर्ष | WSP_BOT))
	{
	    जर (flags & WSP_BOT)
		frame_set_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    frame_new_width(curfrp, curfrp->fr_width
		     - (new_size + ((flags & WSP_शीर्ष) != 0)), flags & WSP_शीर्ष,
								       FALSE);
	}
	अन्यथा
	    win_new_width(oldwin, oldwin->w_width - (new_size + 1));
	जर (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_size + 1;
	}
	अन्यथा		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_width + 1;
	frame_fix_width(oldwin);
	frame_fix_width(wp);
    }
    अन्यथा
    {
	// width and column of new window is same as current window
	जर (flags & (WSP_शीर्ष | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_width(wp, topframe->fr_width);
	    wp->w_vsep_width = 0;
	}
	अन्यथा
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_width(wp, oldwin->w_width);
	    wp->w_vsep_width = oldwin->w_vsep_width;
	}
	frp->fr_width = curfrp->fr_width;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_height(wp, new_size);
	पूर्णांक old_status_height = oldwin->w_status_height;
	जर (flags & (WSP_शीर्ष | WSP_BOT))
	{
	    पूर्णांक new_fr_height = curfrp->fr_height - new_size
							  + WINBAR_HEIGHT(wp);

	    जर (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_height -= statusline_height(curfrp->fr_win);
	    जर (flags & WSP_BOT)
		frame_add_statusline(curfrp);
	    frame_new_height(curfrp, new_fr_height, flags & WSP_शीर्ष, FALSE,
									FALSE);
	}
	अन्यथा
	    win_new_height(oldwin, oldwin_height - (new_size
			+ statusline_height(wp)));
	जर (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_status_height = statusline_height(wp);
	    oldwin->w_winrow += wp->w_height + statusline_height(wp);
	}
	अन्यथा		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_HEIGHT(oldwin)
						    + statusline_height(oldwin);
	    wp->w_status_height = old_status_height;
	    जर (!(flags & WSP_BOT))
		oldwin->w_status_height = statusline_height(oldwin);
	}
	frame_fix_height(wp);
	frame_fix_height(oldwin);
    }

    जर (flags & (WSP_शीर्ष | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    status_redraw_all();

    जर (need_status)
    {
	संदेश_row = Rows - 1;
	संदेश_col = sc_col;
	संदेश_clr_eos_force();	// Old command/ruler may still be there
	comp_col();
	संदेश_row = Rows - 1;
	संदेश_col = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    जर (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    अन्यथा जर (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    जर (flags & WSP_VERT)
    {
	i = p_wiw;
	जर (size != 0)
	    p_wiw = size;

#ifdef FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	जर (gui.in_use)
	    gui_init_which_components(रिक्त);
#endif
    }
    अन्यथा
    {
	i = p_wh;
	जर (size != 0)
	    p_wh = size;
    }

    /*
     * make the new window the current window
     */
    (शून्य)win_enter_ext(wp, (new_wp == रिक्त ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    जर (flags & WSP_VERT)
	p_wiw = i;
    अन्यथा
	p_wh = i;
    retval = OK;

theend:
    जर (RedrawingDisabled > 0)
	--RedrawingDisabled;
    परत retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    स्थिर शून्य
win_init(win_T *newp, win_T *oldp, पूर्णांक flags UNUSED)
{
    पूर्णांक		i;

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
    जर (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = रिक्त;
	newp->w_llist_ref = रिक्त;
    }
    अन्यथा
	copy_loclist_चळत(oldp, newp);
#endif
    newp->w_localdir = (oldp->w_localdir == रिक्त)
				    ? रिक्त : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == रिक्त)
				    ? रिक्त : vim_strsave(oldp->w_prevdir);

    जर (*p_spk != 'c')
    {
	जर (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_height = oldp->w_height - WINBAR_HEIGHT(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_HEIGHT(oldp);
    }

    // copy tagstack and folds
    साठी (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*tag = &newp->w_tagstack[i];
	*tag = oldp->w_tagstack[i];
	जर (tag->tagname != रिक्त)
	    tag->tagname = vim_strsave(tag->tagname);
	जर (tag->user_data != रिक्त)
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
	bool pushed = ढकला_highlight_overrides(newp->w_hl, newp->w_hl_len);
	जर (newp->w_buffer->b_term != रिक्त)
	    term_init_default_colors(newp->w_buffer->b_term);
	जर (pushed)
	    काढा_highlight_overrides();
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
    पूर्णांक
win_valid_popup(win_T *win UNUSED)
{
#ifdef FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	जर (wp == win)
	    परत TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	जर (wp == win)
	    परत TRUE;
#endif
    परत FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    पूर्णांक
win_valid(win_T *win)
{
    win_T	*wp;

    जर (win == रिक्त)
	परत FALSE;
    FOR_ALL_WINDOWS(wp)
	जर (wp == win)
	    परत TRUE;
    परत win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_find_by_id(पूर्णांक id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	जर (wp->w_id == id)
	    परत wp;
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	जर (wp->w_id == id)
	    परत wp;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	जर (wp->w_id == id)
	    परत wp;
#endif
    परत रिक्त;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    पूर्णांक
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    जर (win == रिक्त)
	परत FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    जर (wp == win)
		परत TRUE;
	}
#ifdef FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    जर (wp == win)
		परत TRUE;
#endif
    }
    परत win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    पूर्णांक
win_मोजणी(शून्य)
{
    win_T	*wp;
    पूर्णांक		मोजणी = 0;

    FOR_ALL_WINDOWS(wp)
	++मोजणी;
    परत मोजणी;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    पूर्णांक
make_windows(
    पूर्णांक		मोजणी,
    पूर्णांक		vertical)  // split windows vertically if TRUE
{
    पूर्णांक		maxcount;
    पूर्णांक		todo;

    जर (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_width + curwin->w_vsep_width
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    अन्यथा
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_HEIGHT(curwin) + curwin->w_status_height
			- (p_wh - p_wmh)) / (p_wmh + statusline_height(curwin));
    }

    जर (maxcount < 2)
	maxcount = 2;
    जर (मोजणी > maxcount)
	मोजणी = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    जर (मोजणी > 1)
	last_status(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    // todo is number of windows left to create
    साठी (todo = मोजणी - 1; todo > 0; --todo)
	जर (vertical)
	{
	    जर (win_split(curwin->w_width - (curwin->w_width - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		थांबा;
	}
	अन्यथा
	{
	    जर (win_split(curwin->w_height - (curwin->w_height - todo
			    * statusline_height(curwin)) / (todo + 1)
			- statusline_height(curwin), WSP_ABOVE) == FAIL)
		थांबा;
	}

    unblock_autocmds();

    // return actual number of windows
    परत (मोजणी - todo);
}

/*
 * Exchange current and next window
 */
    स्थिर शून्य
win_exchange(दीर्घ Prenum)
{
    frame_T	*frp;
    frame_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    पूर्णांक		temp;

    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;
    जर (ONE_WINDOW)	    // just one window
    {
	beep_flush();
	परत;
    }
    जर (text_or_buf_locked())
    {
	beep_flush();
	परत;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * find window to exchange with
     */
    जर (Prenum)
    {
	frp = curwin->w_frame->fr_parent->fr_child;
	पर्यंत (frp != रिक्त && --Prenum > 0)
	    frp = frp->fr_next;
    }
    अन्यथा जर (curwin->w_frame->fr_next != रिक्त)	// Swap with next
	frp = curwin->w_frame->fr_next;
    अन्यथा    // Swap last window in row/col with previous
	frp = curwin->w_frame->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    जर (frp == रिक्त || frp->fr_win == रिक्त || frp->fr_win == curwin)
	परत;
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
    जर (wp->w_prev != curwin)
    {
	win_remove(curwin, रिक्त);
	frame_remove(curwin->w_frame);
	win_append(wp->w_prev, curwin);
	frame_घाला(frp, curwin->w_frame);
    }
    जर (wp != wp2)
    {
	win_remove(wp, रिक्त);
	frame_remove(wp->w_frame);
	win_append(wp2, wp);
	जर (frp2 == रिक्त)
	    frame_घाला(wp->w_frame->fr_parent->fr_child, wp->w_frame);
	अन्यथा
	    frame_append(frp2, wp->w_frame);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    temp = curwin->w_status_height;
    जर (temp != wp->w_status_height)
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
#जर defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    जर (wp->w_buffer != curbuf)
	reset_VIsual_and_resel();
    अन्यथा जर (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    स्थिर शून्य
win_rotate(पूर्णांक upwards, पूर्णांक मोजणी)
{
    win_T	*wp1;
    win_T	*wp2;
    frame_T	*frp;
    पूर्णांक		n;

    जर (ONE_WINDOW)		// nothing to do
    {
	beep_flush();
	परत;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Check if all frames in this row/col have one window.
    FOR_ALL_FRAMES(frp, curwin->w_frame->fr_parent->fr_child)
	जर (frp->fr_win == रिक्त)
	{
	    emsg(_(e_cannot_rotate_when_another_window_is_split));
	    परत;
	}

    पर्यंत (मोजणी--)
    {
	जर (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_frame->fr_parent->fr_child;
	    wp1 = frp->fr_win;
	    win_remove(wp1, रिक्त);
	    frame_remove(frp);

	    // find last frame and append removed window/frame after it
	    साठी ( ; frp->fr_next != रिक्त; frp = frp->fr_next)
		;
	    win_append(frp->fr_win, wp1);
	    frame_append(frp, wp1->w_frame);

	    wp2 = frp->fr_win;		// previously last window
	}
	अन्यथा			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    साठी (frp = curwin->w_frame; frp->fr_next != रिक्त;
							   frp = frp->fr_next)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, रिक्त);
	    frame_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_parent->fr_child->fr_win->w_prev, wp1);
	    frame_घाला(frp->fr_parent->fr_child, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_status_height;
	जर (n != wp1->w_status_height)
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
#जर defined(FEAT_STL_OPT)
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
    पूर्णांक
win_splitmove(win_T *wp, पूर्णांक size, पूर्णांक flags)
{
    पूर्णांक		dir;
    पूर्णांक		height = wp->w_height;
    frame_T	*unflat_altfr;

    जर (ONE_WINDOW)
	परत OK;	// nothing to do
    जर (check_split_disallowed(wp) == FAIL)
	परत FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, रिक्त, &unflat_altfr);
    win_remove(wp, रिक्त);
    last_status(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    जर (win_split_ins(size, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	परत FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    जर (size == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(height, wp);
	जर (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#जर defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif
    परत OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    शून्य
win_move_after(win_T *win1, win_T *win2)
{
    पूर्णांक		height;

    // check if the arguments are reasonable
    जर (win1 == win2)
	परत;

    // check if there is something to do
    जर (win2->w_next != win1)
    {
	जर (win1->w_frame->fr_parent != win2->w_frame->fr_parent)
	{
	    iemsg("Trying to move a window into another frame");
	    परत;
	}

	// may need to move the status line/vertical separator of the last
	// window
	जर (win1 == lastwin)
	{
	    height = win1->w_prev->w_status_height;
	    win1->w_prev->w_status_height = win1->w_status_height;
	    win1->w_status_height = height;
	    जर (win1->w_prev->w_vsep_width == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_width = 0;
		win1->w_prev->w_frame->fr_width -= 1;
		win1->w_vsep_width = 1;
		win1->w_frame->fr_width += 1;
	    }
	}
	अन्यथा जर (win2 == lastwin)
	{
	    height = win1->w_status_height;
	    win1->w_status_height = win2->w_status_height;
	    win2->w_status_height = height;
	    जर (win1->w_vsep_width == 1)
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
    पूर्णांक		current,	// do only frame with current window
    पूर्णांक		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    जर (dir == 0)
	dir = *p_ead;
    win_equal_rec(next_curwin == रिक्त ? curwin : next_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_height(),
		      topframe->fr_width, topframe->fr_height);
#जर defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
    जर (!is_aucmd_win(next_curwin))
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
    पूर्णांक		current,	// do only frame with current window
    frame_T	*topfr,		// frame to set size off
    पूर्णांक		dir,		// 'v', 'h' or 'b', see win_equal()
    पूर्णांक		col,		// horizontal position for frame
    पूर्णांक		row,		// vertical position for frame
    पूर्णांक		width,		// new width of frame
    पूर्णांक		height)		// new height of frame
{
    पूर्णांक		n, m;
    पूर्णांक		extra_sep = 0;
    पूर्णांक		wincount, totwincount = 0;
    frame_T	*fr;
    पूर्णांक		next_curwin_size = 0;
    पूर्णांक		room = 0;
    पूर्णांक		new_size;
    पूर्णांक		has_next_curwin = 0;
    पूर्णांक		hnc;

    जर (topfr->fr_layout == FR_LEAF)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	जर (topfr->fr_height != height || topfr->fr_win->w_winrow != row
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
    अन्यथा जर (topfr->fr_layout == FR_ROW)
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	जर (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = frame_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    जर (col + width >= firstwin->w_wincol + topframe->fr_width)
		extra_sep = 1;
	    अन्यथा
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
	    जर (room < 0)
	    {
		next_curwin_size = p_wiw + room;
		room = 0;
	    }
	    अन्यथा
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    जर (!frame_fixed_width(fr))
			सुरूठेवा;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minwidth(fr, NOWIN);
		    new_size = fr->fr_width;
		    जर (frame_has_win(fr, next_curwin))
		    {
			room += p_wiw - p_wmw;
			next_curwin_size = 0;
			जर (new_size < p_wiw)
			    new_size = p_wiw;
		    }
		    अन्यथा
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_next == रिक्त
					? extra_sep : 0)) / (p_wmw + 1);
		    room -= new_size - n;
		    जर (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newwidth = new_size;
		}
		जर (next_curwin_size == -1)
		{
		    जर (!has_next_curwin)
			next_curwin_size = 0;
		    अन्यथा जर (totwincount > 1
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
		    अन्यथा
			next_curwin_size = p_wiw;
		}
	    }

	    जर (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    जर (fr->fr_next == रिक्त)
		// last frame gets all that remains (avoid roundoff error)
		new_size = width;
	    अन्यथा जर (dir == 'v')
		new_size = fr->fr_width;
	    अन्यथा जर (frame_fixed_width(fr))
	    {
		new_size = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    अन्यथा
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = frame_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_next == रिक्त ? extra_sep : 0))
								/ (p_wmw + 1);
		m = frame_minwidth(fr, next_curwin);
		जर (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		अन्यथा
		    hnc = FALSE;
		जर (hnc)	    // don't count next_curwin
		    --wincount;
		जर (totwincount == 0)
		    new_size = room;
		अन्यथा
		    new_size = (wincount * room + ((अचिन्हित)totwincount >> 1))
								/ totwincount;
		जर (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wiw - (m - n);
		    जर (next_curwin_size < 0)
			next_curwin_size = 0;
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		अन्यथा
		    room -= new_size;
		new_size += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    जर (!current || dir != 'v' || topfr->fr_parent != रिक्त
		    || (new_size != fr->fr_width)
		    || frame_has_win(fr, next_curwin))
		win_equal_rec(next_curwin, current, fr, dir, col, row,
							    new_size, height);
	    col += new_size;
	    width -= new_size;
	    totwincount -= wincount;
	}
    }
    अन्यथा // topfr->fr_layout == FR_COL
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	जर (dir != 'h')			// equalize frame heights
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
	    जर (room < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		next_curwin_size = p_wh + room;
		room = 0;
	    }
	    अन्यथा
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    जर (!frame_fixed_height(fr))
			सुरूठेवा;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minheight(fr, NOWIN);
		    new_size = fr->fr_height;
		    जर (frame_has_win(fr, next_curwin))
		    {
			room += p_wh - p_wmh;
			next_curwin_size = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			जर (new_size < n + (p_wh - p_wmh))
			    new_size = n + (p_wh - p_wmh);
		    }
		    अन्यथा
			// These windows don't use up room.
			totwincount -= frame_wincount_in_height(fr);
		    room -= new_size - n;
		    जर (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newheight = new_size;
		}
		जर (next_curwin_size == -1)
		{
		    जर (!has_next_curwin)
			next_curwin_size = 0;
		    अन्यथा जर (totwincount > 1
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
		    अन्यथा
			next_curwin_size = p_wh;
		}
	    }

	    जर (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    जर (fr->fr_next == रिक्त)
		// last frame gets all that remains (avoid roundoff error)
		new_size = height;
	    अन्यथा जर (dir == 'h')
		new_size = fr->fr_height;
	    अन्यथा जर (frame_fixed_height(fr))
	    {
		new_size = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    अन्यथा
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = frame_minheight(fr, NOWIN);
		wincount = frame_wincount_in_height(fr);
		m = frame_minheight(fr, next_curwin);
		जर (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		अन्यथा
		    hnc = FALSE;
		जर (hnc)	    // don't count next_curwin
		    --wincount;
		जर (totwincount == 0)
		    new_size = room;
		अन्यथा
		    new_size = (wincount * room + ((अचिन्हित)totwincount >> 1))
								/ totwincount;
		जर (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wh - (m - n);
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		अन्यथा
		    room -= new_size;
		new_size += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    जर (!current || dir != 'h' || topfr->fr_parent != रिक्त
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

#जर defined(FEAT_JOB_CHANNEL)
    शून्य
leaving_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    जर (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	परत;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_buffer->b_prompt_घाला = restart_edit;
    जर (restart_edit != 0 && mode_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    जर ((State & MODE_घाला) && !stop_घाला_mode)
    {
	stop_घाला_mode = TRUE;
	जर (win->w_buffer->b_prompt_घाला == NUL)
	    win->w_buffer->b_prompt_घाला = 'A';
    }
}

    शून्य
entering_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    जर (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	परत;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    जर (win->w_buffer->b_prompt_घाला != NUL)
	stop_घाला_mode = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    जर ((State & MODE_घाला) == 0)
	restart_edit = win->w_buffer->b_prompt_घाला;
}
#endif

    स्थिर शून्य
win_init_empty(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_ओळी_valid = 0;
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
#जर defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
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
    पूर्णांक		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    पूर्णांक		मोजणी = tabpage_index(रिक्त);

    ++RedrawingDisabled;

    साठी (wp = firstwin; wp != रिक्त && !ONE_WINDOW; )
    {
	जर (wp->w_buffer == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_buffer->b_locked > 0))
	{
	    जर (window_layout_locked(CMD_SIZE))
		जा theend;  // Only give one error message.
	    जर (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		थांबा;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	अन्यथा
	    wp = wp->w_next;
    }

    // Also check windows in other tab pages.
    साठी (tp = first_tabpage; tp != रिक्त; tp = nexttp)
    {
	nexttp = tp->tp_next;
	जर (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		जर (wp->w_buffer == buf
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0))
		{
		    जर (window_layout_locked(CMD_SIZE))
			जा theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = first_tabpage;
		    थांबा;
		}
    }

theend:
    जर (RedrawingDisabled > 0)
	--RedrawingDisabled;

    जर (मोजणी != tabpage_index(रिक्त))
	apply_autocmds(EVENT_TABCLOSED, रिक्त, रिक्त, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    पूर्णांक
last_window(शून्य)
{
    परत (one_window() && first_tabpage->tp_next == रिक्त);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    पूर्णांक
one_window(शून्य)
{
    win_T	*wp;
    पूर्णांक		seen_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	जर (!is_aucmd_win(wp))
	{
	    जर (seen_one)
		परत FALSE;
	    seen_one = TRUE;
	}
    }
    परत TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    स्थिर पूर्णांक
close_last_window_tabpage(
    win_T	*win,
    पूर्णांक		free_buf,
    tabpage_T   *prev_curtab)
{
    जर (!ONE_WINDOW)
	परत FALSE;

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
    जर (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    जर (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, रिक्त, रिक्त, FALSE, curbuf);
#जर defined(FEAT_TABPANEL)
    जर (p_stpl > 0)
	shell_new_columns();
#endif
    apply_autocmds(EVENT_WINENTER, रिक्त, रिक्त, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, रिक्त, रिक्त, FALSE, curbuf);
    जर (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
    परत TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    स्थिर शून्य
win_close_buffer(win_T *win, पूर्णांक action, पूर्णांक abort_if_last)
{
#ifdef FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    जर (win->w_buffer != रिक्त)
	reset_synblock(win);
#endif

#ifdef FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    जर (win->w_buffer != रिक्त && bt_quickfix(win->w_buffer)
					&& win->w_buffer->b_nwindows == 1)
	win->w_buffer->b_p_bl = FALSE;
#endif

    // Close the link to the buffer.
    जर (win->w_buffer != रिक्त)
    {
	bufref_T    bufref;

	set_bufref(&bufref, curbuf);
	++win->w_locked;
	close_buffer(win, win->w_buffer, action, abort_if_last, TRUE, TRUE);
	जर (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	जर (!bufref_valid(&bufref))
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
    पूर्णांक
win_close(win_T *win, पूर्णांक free_buf)
{
    win_T	*wp;
    पूर्णांक		other_buffer = FALSE;
    पूर्णांक		close_curwin = FALSE;
    पूर्णांक		dir;
    पूर्णांक		help_window = FALSE;
    पूर्णांक		quickfix_window = FALSE;
    tabpage_T   *prev_curtab = curtab;
    frame_T	*win_frame = win->w_frame->fr_parent;
#ifdef FEAT_DIFF
    पूर्णांक		had_diffmode = win->w_p_diff;
#endif
#ifdef MESSAGE_QUEUE
    पूर्णांक		did_decrement = FALSE;
#endif

#जर defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    जर (may_close_term_popup() == OK)
	परत OK;
#endif
    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत FAIL;

    जर (last_window())
    {
	emsg(_(e_cannot_close_last_window));
	परत FAIL;
    }
    जर (window_layout_locked(CMD_close))
	परत FAIL;

    जर (win_locked(win) || (win->w_buffer != रिक्त
					       && win->w_buffer->b_locked > 0))
	परत FAIL; // window is already being closed
    जर (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	परत FAIL;
    }
    जर ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_window())
    {
	emsg(_(e_cannot_close_window_only_autocmd_window_would_remain));
	परत FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    जर (close_last_window_tabpage(win, free_buf, prev_curtab))
	परत FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    जर (bt_help(win->w_buffer))
	help_window = TRUE;
    अन्यथा
	clear_snapshot(curtab, SNAP_HELP_IDX);

    जर (bt_quickfix(win->w_buffer))
	quickfix_window = TRUE;
    अन्यथा
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    जर (win == curwin)
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
	जर (wp->w_buffer != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_buffer = TRUE;
	    जर (!win_valid(win))
		परत FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	    जर (!win_valid(win))
		परत FAIL;
	    --win->w_locked;
	    जर (last_window())
		परत FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	जर (!win_valid(win))
	    परत FAIL;
	--win->w_locked;
	जर (last_window())
	    परत FAIL;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	जर (aborting())
	    परत FAIL;
#endif
    }

#ifdef FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    जर (gui.in_use)
	out_flush();
#endif

#ifdef FEAT_PROP_POPUP
    जर (popup_win_closed(win) && !win_valid(win))
	परत FAIL;
#endif

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    जर (!win_valid_any_tab(win))
	परत OK;

    win_close_buffer(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    जर (win_valid(win) && win->w_buffer == रिक्त
#जर defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#endif
	    && last_window())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	जर (curwin->w_buffer == रिक्त)
	    curwin->w_buffer = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    जर (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_buffer == रिक्त)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	परत FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    जर (!win_valid(win) || last_window()
	    || close_last_window_tabpage(win, free_buf, prev_curtab))
	परत FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++split_disallowed;
#ifdef MESSAGE_QUEUE
    ++dont_parse_messages;
#endif

    जर (win->w_buffer != रिक्त)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, रिक्त);

    जर (help_window || quickfix_window)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	जर (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    जर (win == curwin)
    {
	curwin = wp;
#ifdef FEAT_QUICKFIX
	जर (wp->w_p_pvw || bt_quickfix(wp->w_buffer))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    साठी (;;)
	    {
		जर (wp->w_next == रिक्त)
		    wp = firstwin;
		अन्यथा
		    wp = wp->w_next;
		जर (wp == curwin)
		    थांबा;
		जर (!wp->w_p_pvw && !bt_quickfix(wp->w_buffer))
		{
		    curwin = wp;
		    थांबा;
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

    जर (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_frame->fr_parent == win_frame, dir);
    अन्यथा
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    जर (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ifdef MESSAGE_QUEUE
	did_decrement =
#अन्यथा
	(शून्य)
#endif
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	जर (other_buffer)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
    }

    जर (ONE_WINDOW && curwin->w_locked && curbuf->b_locked_split
	    && first_tabpage->tp_next != रिक्त)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, रिक्त, रिक्त, FALSE, curbuf);

    --split_disallowed;
#ifdef MESSAGE_QUEUE
    जर (!did_decrement)
	--dont_parse_messages;
#endif

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    जर (help_window || quickfix_window)
	restore_snapshot(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ifdef FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    जर (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	पूर्णांक	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    जर (dwin->w_p_diff)
		++diffcount;
	जर (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#endif

#जर defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    जर (gui.in_use && !win_hasvertsplit())
	gui_init_which_components(रिक्त);
#endif

    redraw_all_later(UPD_NOT_VALID);
    परत OK;
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
    स्थिर पूर्णांक	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    जर (recursive)
	परत;
    recursive = TRUE;
    vim_snprintf((अक्षर *)winid, आकार(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_buffer);
    recursive = FALSE;
}

    शून्य
trigger_tabclosedpre(tabpage_T *tp)
{
    स्थिर पूर्णांक	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    जर (!has_tabclosedpre() || recursive)
	परत;

    जर (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    window_layout_lock();
    apply_autocmds(EVENT_TABCLOSEDPRE, रिक्त, रिक्त, FALSE, रिक्त);
    window_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    जर (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    अन्यथा
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

स्थिर पूर्णांक did_initial_scroll_size_snapshot = FALSE;

    शून्य
may_make_initial_scroll_size_snapshot(शून्य)
{
    जर (!did_initial_scroll_size_snapshot)
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
	पूर्णांक width,
	पूर्णांक height,
	पूर्णांक topline,
# ifdef FEAT_DIFF
	पूर्णांक topfill,
# endif
	पूर्णांक leftcol,
	पूर्णांक skipcol)
{
    dict_T *d = dict_alloc();
    जर (d == रिक्त)
	परत रिक्त;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    पर्यंत (1)
    {
	typval_T tv;
	tv.v_lock = 0;
	tv.v_type = VAR_संख्या;

	tv.vval.v_संख्या = width;
	जर (dict_add_tv(d, "width", &tv) == FAIL)
	    थांबा;
	tv.vval.v_संख्या = height;
	जर (dict_add_tv(d, "height", &tv) == FAIL)
	    थांबा;
	tv.vval.v_संख्या = topline;
	जर (dict_add_tv(d, "topline", &tv) == FAIL)
	    थांबा;
# ifdef FEAT_DIFF
	tv.vval.v_संख्या = topfill;
# अन्यथा
	tv.vval.v_संख्या = 0;
# endif
	जर (dict_add_tv(d, "topfill", &tv) == FAIL)
	    थांबा;
	tv.vval.v_संख्या = leftcol;
	जर (dict_add_tv(d, "leftcol", &tv) == FAIL)
	    थांबा;
	tv.vval.v_संख्या = skipcol;
	जर (dict_add_tv(d, "skipcol", &tv) == FAIL)
	    थांबा;
	परत d;
    }
    dict_unref(d);
    परत रिक्त;
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
	पूर्णांक	*size_मोजणी,
	win_T	**first_scroll_win,
	win_T	**first_size_win,
	list_T	*winlist UNUSED,
	dict_T	*v_event UNUSED)
{
#ifdef FEAT_EVAL
    पूर्णांक listidx = 0;
    पूर्णांक tot_width = 0;
    पूर्णांक tot_height = 0;
    पूर्णांक tot_topline = 0;
# ifdef FEAT_DIFF
    पूर्णांक tot_topfill = 0;
# endif
    पूर्णांक tot_leftcol = 0;
    पूर्णांक tot_skipcol = 0;
#endif

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	पूर्णांक ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	पूर्णांक size_changed = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_last_width != wp->w_width
			    || wp->w_last_height != wp->w_height);
	जर (size_changed)
	{
#ifdef FEAT_EVAL
	    जर (winlist != रिक्त)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_lock = 0;
		tv.v_type = VAR_संख्या;
		tv.vval.v_संख्या = wp->w_id;
		list_set_item(winlist, listidx++, &tv);
	    }
	    अन्यथा
#endif
		जर (size_मोजणी != रिक्त)
	    {
		++*size_मोजणी;
		जर (*first_size_win == रिक्त)
		    *first_size_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		जर (*first_scroll_win == रिक्त && !ignore_scroll)
		    *first_scroll_win = wp;
	    }
	}

	पूर्णांक scroll_changed = !ignore_scroll
				&& (wp->w_last_topline != wp->w_topline
#ifdef FEAT_DIFF
				|| wp->w_last_topfill != wp->w_topfill
#endif
				|| wp->w_last_leftcol != wp->w_leftcol
				|| wp->w_last_skipcol != wp->w_skipcol);
	जर (scroll_changed
	    && first_scroll_win != रिक्त && *first_scroll_win == रिक्त)
	    *first_scroll_win = wp;

#ifdef FEAT_EVAL
	जर ((size_changed || scroll_changed) && v_event != रिक्त)
	{
	    // Add info about this window to the v:event dictionary.
	    पूर्णांक width = wp->w_width - wp->w_last_width;
	    पूर्णांक height = wp->w_height - wp->w_last_height;
	    पूर्णांक topline = wp->w_topline - wp->w_last_topline;
# ifdef FEAT_DIFF
	    पूर्णांक topfill = wp->w_topfill - wp->w_last_topfill;
# endif
	    पूर्णांक leftcol = wp->w_leftcol - wp->w_last_leftcol;
	    पूर्णांक skipcol = wp->w_skipcol - wp->w_last_skipcol;
	    dict_T *d = make_win_info_dict(width, height, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    जर (d == रिक्त)
		थांबा;
	    अक्षर winid[NUMBUFLEN];
	    vim_snprintf(winid, आकार(winid), "%d", wp->w_id);
	    जर (dict_add_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		थांबा;
	    }
	    --d->dv_refcount;

	    tot_width += असल(width);
	    tot_height += असल(height);
	    tot_topline += असल(topline);
# ifdef FEAT_DIFF
	    tot_topfill += असल(topfill);
# endif
	    tot_leftcol += असल(leftcol);
	    tot_skipcol += असल(skipcol);
	}
#endif
    }

#ifdef FEAT_EVAL
    जर (v_event != रिक्त)
    {
	dict_T *alldict = make_win_info_dict(tot_width, tot_height, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	जर (alldict != रिक्त)
	{
	    जर (dict_add_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    अन्यथा
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
    स्थिर पूर्णांक	    recursive = FALSE;
    पूर्णांक		    do_resize = has_winresized();
    पूर्णांक		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    जर (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_size_snapshot)
	परत;

    पूर्णांक size_मोजणी = 0;
    win_T *first_scroll_win = रिक्त, *first_size_win = रिक्त;
    check_window_scroll_resize(&size_मोजणी, &first_scroll_win, &first_size_win,
								    रिक्त, रिक्त);
    पूर्णांक trigger_resize = do_resize && size_मोजणी > 0;
    पूर्णांक trigger_scroll = do_scroll && first_scroll_win != रिक्त;
    जर (!trigger_resize && !trigger_scroll)
	परत;  // no relevant changes
#ifdef FEAT_EVAL
    list_T *windows_list = रिक्त;
    जर (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_वस्तुसंच(size_मोजणी);
	जर (windows_list != रिक्त)
	    check_window_scroll_resize(रिक्त, रिक्त, रिक्त, windows_list, रिक्त);
    }

    dict_T *scroll_dict = रिक्त;
    जर (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	जर (scroll_dict != रिक्त)
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
    जर (trigger_resize
#ifdef FEAT_EVAL
	    && windows_list != रिक्त
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	जर (dict_add_list(v_event, "windows", windows_list) == OK)
	{
	    dict_set_वस्तुसंच_ro(v_event);
#endif
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((अक्षर *)winid, आकार(winid), "%d",
							 first_size_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     first_size_win->w_buffer);
#ifdef FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#endif
    }

    जर (trigger_scroll
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
	dict_set_वस्तुसंच_ro(v_event);
	dict_unref(scroll_dict);
#endif
	char_u winid[NUMBUFLEN];
	vim_snprintf((अक्षर *)winid, आकार(winid), "%d",
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
win_close_othertab(win_T *win, पूर्णांक free_buf, tabpage_T *tp)
{
    win_T	*wp;
    पूर्णांक		dir;
    tabpage_T   *ptp = रिक्त;
    पूर्णांक		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    जर (window_layout_locked(CMD_SIZE))
	परत;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    जर (win_locked(win) || (win->w_buffer != रिक्त
					       && win->w_buffer->b_locked > 0))
	परत; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    जर (win->w_buffer != रिक्त)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	जर (!win_valid_any_tab(win))
	    परत;
    }

    जर (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	जर (!win_valid_any_tab(win))
	    परत;
    }

    जर (win->w_buffer != रिक्त)
	// Close the link to the buffer.
	close_buffer(win, win->w_buffer, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    साठी (ptp = first_tabpage; ptp != रिक्त && ptp != tp; ptp = ptp->tp_next)
	;
    जर (ptp == रिक्त || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	जर (win_valid_any_tab(win) && win->w_buffer == रिक्त)
	{
	    win->w_buffer = firstbuf;
	    ++firstbuf->b_nwindows;
	    जर (win == curwin)
		curbuf = curwin->w_buffer;
	    win_init_empty(win);
	}
	परत;
    }

    // Autocommands may have closed the window already.
    साठी (wp = tp->tp_firstwin; wp != रिक्त && wp != win; wp = wp->w_next)
	;
    जर (wp == रिक्त)
	परत;

    // When closing the last window in a tab page remove the tab page.
    जर (tp->tp_firstwin == tp->tp_lastwin)
    {
	पूर्णांक	h = tabline_height();

	जर (tp == first_tabpage)
	    first_tabpage = tp->tp_next;
	अन्यथा
	{
	    साठी (ptp = first_tabpage; ptp != रिक्त && ptp->tp_next != tp;
							   ptp = ptp->tp_next)
		;
	    जर (ptp == रिक्त)
	    {
		internal_error("win_close_othertab()");
		परत;
	    }
	    ptp->tp_next = tp->tp_next;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	जर (h != tabline_height())
	    shell_new_rows();
    }

    जर (win->w_buffer != रिक्त)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    जर (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    स्थिर win_T *
win_free_mem(
    win_T	*win,
    पूर्णांक		*dirp,		// set to 'v' or 'h' for direction if 'ea'
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
    जर (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    परत wp;
}

#जर defined(EXITFREE)
    शून्य
win_free_all(शून्य)
{
    पूर्णांक		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = रिक्त;
    cmdwin_win = रिक्त;

    पर्यंत (first_tabpage->tp_next != रिक्त)
	tabpage_close(TRUE);

    साठी (पूर्णांक i = 0; i < AUCMD_WIN_मोजणी; ++i)
	जर (aucmd_win[i].auc_win != रिक्त)
	{
	    (शून्य)win_free_mem(aucmd_win[i].auc_win, &dummy, रिक्त);
	    aucmd_win[i].auc_win = रिक्त;
	}

    पर्यंत (firstwin != रिक्त)
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
    पूर्णांक		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    frame_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    frame_T	*frp, *frp2, *frp3;
    frame_T	*frp_close = win->w_frame;
    win_T	*wp;
    पूर्णांक		row, col;

    /*
     * If there is only one window there is nothing to remove.
     */
    जर (tp == रिक्त ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	परत रिक्त;

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
    जर (win->w_vsep_width == 0 && frp_close->fr_parent->fr_layout == FR_ROW &&
	    frp_close->fr_prev != रिक्त)
	frame_set_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    frame_remove(frp_close);

    जर (frp_close->fr_parent->fr_layout == FR_COL)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	जर (frp2->fr_win != रिक्त && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    पर्यंत (frp != रिक्त || frp3 != रिक्त)
	    {
		जर (frp != रिक्त)
		{
		    जर (!frame_fixed_height(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			थांबा;
		    }
		    frp = frp->fr_prev;
		}
		जर (frp3 != रिक्त)
		{
		    जर (frp3->fr_win != रिक्त && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			थांबा;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_height(frp2, frp2->fr_height + frp_close->fr_height,
			    frp2 == frp_close->fr_next, !frame_fixed_height(frp2), FALSE);
	*dirp = 'v';
    }
    अन्यथा
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	जर (frp2->fr_win != रिक्त && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    पर्यंत (frp != रिक्त || frp3 != रिक्त)
	    {
		जर (frp != रिक्त)
		{
		    जर (!frame_fixed_width(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			थांबा;
		    }
		    frp = frp->fr_prev;
		}
		जर (frp3 != रिक्त)
		{
		    जर (frp3->fr_win != रिक्त && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			थांबा;
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
    जर (frp2 != frp_close->fr_prev)
	frame_comp_pos(frp_close->fr_parent, &row, &col);

    जर (unflat_altfr == रिक्त)
	frame_flatten(frp2);
    अन्यथा
	*unflat_altfr = frp2;

    frame_locked--;

    परत wp;
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

    जर (frp->fr_next != रिक्त || frp->fr_prev != रिक्त)
	परत;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_parent->fr_layout = frp->fr_layout;
    frp->fr_parent->fr_child = frp->fr_child;
    FOR_ALL_FRAMES(frp2, frp->fr_child)
	frp2->fr_parent = frp->fr_parent;
    frp->fr_parent->fr_win = frp->fr_win;
    जर (frp->fr_win != रिक्त)
	frp->fr_win->w_frame = frp->fr_parent;
    frp2 = frp->fr_parent;
    जर (topframe->fr_child == frp)
	topframe->fr_child = frp2;
    vim_free(frp);

    frp = frp2->fr_parent;
    जर (frp != रिक्त && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	जर (frp->fr_child == frp2)
	    frp->fr_child = frp2->fr_child;
	frp2->fr_child->fr_prev = frp2->fr_prev;
	जर (frp2->fr_prev != रिक्त)
	    frp2->fr_prev->fr_next = frp2->fr_child;
	साठी (frp3 = frp2->fr_child; ; frp3 = frp3->fr_next)
	{
	    frp3->fr_parent = frp;
	    जर (frp3->fr_next == रिक्त)
	    {
		frp3->fr_next = frp2->fr_next;
		जर (frp2->fr_next != रिक्त)
		    frp2->fr_next->fr_prev = frp3;
		थांबा;
	    }
	}
	जर (topframe->fr_child == frp2)
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
winframe_restore(win_T *wp, पूर्णांक dir, frame_T *unflat_altfr)
{
    frame_T	*frp = wp->w_frame;

    // Put "wp"'s frame back where it was.
    जर (frp->fr_prev != रिक्त)
	frame_append(frp->fr_prev, frp);
    अन्यथा
	frame_घाला(frp->fr_next, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    जर (wp->w_vsep_width == 0
	    && frp->fr_parent->fr_layout == FR_ROW && frp->fr_prev != रिक्त)
	frame_set_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    जर (wp->w_status_height == 0
	    && frp->fr_parent->fr_layout == FR_COL && frp->fr_prev != रिक्त)
	frame_add_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    जर (dir == 'v')
    {
	frame_new_height(unflat_altfr, unflat_altfr->fr_height - frp->fr_height,
		unflat_altfr == frp->fr_next, FALSE, FALSE);
    }
    अन्यथा जर (dir == 'h')
    {
	frame_new_width(unflat_altfr, unflat_altfr->fr_width - frp->fr_width,
		unflat_altfr == frp->fr_next, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    जर (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_parent);
	पूर्णांक	row = topleft->w_winrow;
	पूर्णांक	col = topleft->w_wincol;

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

    जर (tp == रिक्त ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	परत alt_tabpage()->tp_curwin->w_frame;

    frp = win->w_frame;

    जर (frp->fr_prev == रिक्त)
	परत frp->fr_next;
    जर (frp->fr_next == रिक्त)
	परत frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    // window
    target_fr = frp->fr_next;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    जर (frp->fr_parent != रिक्त && frp->fr_parent->fr_layout == FR_COL && p_sb)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    जर (frp->fr_parent != रिक्त && frp->fr_parent->fr_layout == FR_ROW && p_spr)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    जर (frp->fr_parent != रिक्त && frp->fr_parent->fr_layout == FR_ROW)
    {
	जर (frame_fixed_width(target_fr) && !frame_fixed_width(other_fr))
	    target_fr = other_fr;
    }
    अन्यथा
    {
	जर (frame_fixed_height(target_fr) && !frame_fixed_height(other_fr))
	    target_fr = other_fr;
    }

    परत target_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    स्थिर tabpage_T *
alt_tabpage(शून्य)
{
    tabpage_T	*tp = रिक्त;
    पूर्णांक		forward;

    // Use the last accessed tab page, if possible.
    जर ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	परत lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_next != रिक्त &&
	    ((tcl_flags & TCL_डावा) == 0 || curtab == first_tabpage);

    जर (forward)
	tp = curtab->tp_next;
    अन्यथा
	// Use the previous tab page.
	साठी (tp = first_tabpage; tp->tp_next != curtab; tp = tp->tp_next)
	    ;

    परत tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    स्थिर win_T *
frame2win(frame_T *frp)
{
    पर्यंत (frp->fr_win == रिक्त)
	frp = frp->fr_child;
    परत frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    स्थिर पूर्णांक
frame_has_win(frame_T *frp, win_T *wp)
{
    frame_T	*p;

    जर (frp->fr_layout == FR_LEAF)
	परत frp->fr_win == wp;

    FOR_ALL_FRAMES(p, frp->fr_child)
	जर (frame_has_win(p, wp))
	    परत TRUE;
    परत FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
स्थिर पूर्णांक min_set_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    स्थिर शून्य
frame_new_height(
    frame_T	*topfrp,
    पूर्णांक		height,
    पूर्णांक		topfirst,	// resize topmost contained frame first
    पूर्णांक		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    पूर्णांक		set_ch)		// set 'cmdheight' to resize topframe
{
    frame_T	*frp;
    पूर्णांक		extra_ओळी;
    पूर्णांक		h;

    जर (topfrp->fr_parent == रिक्त && set_ch)
    {
	// topframe: update the command line height, with side effects.
	पूर्णांक new_ch = MAX(min_set_ch, p_ch + topfrp->fr_height - height);
	पूर्णांक save_ch = min_set_ch;
	जर (new_ch != p_ch)
	    set_option_मूल्य((char_u *)"cmdheight", new_ch, रिक्त, 0);
	min_set_ch = save_ch;
	height = MIN(height, ROWS_AVAIL);
    }
    जर (topfrp->fr_win != रिक्त)
    {
	// Simple case: just one window.
	win_new_height(topfrp->fr_win,
				    height - topfrp->fr_win->w_status_height
					      - WINBAR_HEIGHT(topfrp->fr_win));
    }
    अन्यथा जर (topfrp->fr_layout == FR_ROW)
    {
	करा
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_height(frp, height, topfirst, wfh, set_ch);
		जर (frp->fr_height > height)
		{
		    // Could not fit the windows, make the whole row higher.
		    height = frp->fr_height;
		    थांबा;
		}
	    }
	}
	पर्यंत (frp != रिक्त);
    }
    अन्यथा    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_child;
	जर (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    पर्यंत (frame_fixed_height(frp))
	    {
		frp = frp->fr_next;
		जर (frp == रिक्त)
		    परत;	    // no frame without 'wfh', give up
	    }
	जर (!topfirst)
	{
	    // Find the bottom frame of this column
	    पर्यंत (frp->fr_next != रिक्त)
		frp = frp->fr_next;
	    जर (wfh)
		// Advance back for frames with one window with 'wfh' set.
		पर्यंत (frame_fixed_height(frp))
		    frp = frp->fr_prev;
	}

	extra_ओळी = height - topfrp->fr_height;
	जर (extra_ओळी < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    पर्यंत (frp != रिक्त)
	    {
		h = frame_minheight(frp, रिक्त);
		जर (frp->fr_height + extra_ओळी < h)
		{
		    extra_ओळी += frp->fr_height - h;
		    frame_new_height(frp, h, topfirst, wfh, set_ch);
		}
		अन्यथा
		{
		    frame_new_height(frp, frp->fr_height + extra_ओळी,
							topfirst, wfh, set_ch);
		    थांबा;
		}
		जर (topfirst)
		{
		    करा
			frp = frp->fr_next;
		    पर्यंत (wfh && frp != रिक्त && frame_fixed_height(frp));
		}
		अन्यथा
		{
		    करा
			frp = frp->fr_prev;
		    पर्यंत (wfh && frp != रिक्त && frame_fixed_height(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		जर (frp == रिक्त)
		    height -= extra_ओळी;
	    }
	}
	अन्यथा जर (extra_ओळी > 0)
	{
	    // increase height of bottom or top frame
	    frame_new_height(frp, frp->fr_height + extra_ओळी, topfirst, wfh,
									set_ch);
	}
    }
    topfrp->fr_height = height;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    स्थिर पूर्णांक
frame_fixed_height(frame_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    जर (frp->fr_win != रिक्त)
	परत frp->fr_win->w_p_wfh;

    जर (frp->fr_layout == FR_ROW)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    जर (frame_fixed_height(frp))
		परत TRUE;
	परत FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	जर (!frame_fixed_height(frp))
	    परत FALSE;
    परत TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    स्थिर पूर्णांक
frame_fixed_width(frame_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    जर (frp->fr_win != रिक्त)
	परत frp->fr_win->w_p_wfw;

    जर (frp->fr_layout == FR_COL)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    जर (frame_fixed_width(frp))
		परत TRUE;
	परत FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	जर (!frame_fixed_width(frp))
	    परत FALSE;
    परत TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    स्थिर शून्य
frame_add_statusline(frame_T *frp)
{
    win_T	*wp;

    जर (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	wp->w_status_height = statusline_height(wp);
    }
    अन्यथा जर (frp->fr_layout == FR_ROW)
    {
	// Handle all the frames in the row.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_statusline(frp);
    }
    अन्यथा // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	साठी (frp = frp->fr_child; frp->fr_next != रिक्त; frp = frp->fr_next)
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
    पूर्णांक		width,
    पूर्णांक		leftfirst,	// resize leftmost contained frame first
    पूर्णांक		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    frame_T	*frp;
    पूर्णांक		extra_cols;
    पूर्णांक		w;
    win_T	*wp;

    जर (topfrp->fr_layout == FR_LEAF)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	साठी (frp = topfrp; frp->fr_parent != रिक्त; frp = frp->fr_parent)
	    जर (frp->fr_parent->fr_layout == FR_ROW && frp->fr_next != रिक्त)
		थांबा;
	जर (frp->fr_parent == रिक्त)
	    wp->w_vsep_width = 0;
	win_new_width(wp, width - wp->w_vsep_width);
    }
    अन्यथा जर (topfrp->fr_layout == FR_COL)
    {
	करा
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_width(frp, width, leftfirst, wfw);
		जर (frp->fr_width > width)
		{
		    // Could not fit the windows, make whole column wider.
		    width = frp->fr_width;
		    थांबा;
		}
	    }
	} पर्यंत (frp != रिक्त);
    }
    अन्यथा    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_child;
	जर (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    पर्यंत (frame_fixed_width(frp))
	    {
		frp = frp->fr_next;
		जर (frp == रिक्त)
		    परत;	    // no frame without 'wfw', give up
	    }
	जर (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    पर्यंत (frp->fr_next != रिक्त)
		frp = frp->fr_next;
	    जर (wfw)
		// Advance back for frames with one window with 'wfw' set.
		पर्यंत (frame_fixed_width(frp))
		    frp = frp->fr_prev;
	}

	extra_cols = width - topfrp->fr_width;
	जर (extra_cols < 0)
	{
	    // reduce frame width, rightmost frame first
	    पर्यंत (frp != रिक्त)
	    {
		w = frame_minwidth(frp, रिक्त);
		जर (frp->fr_width + extra_cols < w)
		{
		    extra_cols += frp->fr_width - w;
		    frame_new_width(frp, w, leftfirst, wfw);
		}
		अन्यथा
		{
		    frame_new_width(frp, frp->fr_width + extra_cols,
							      leftfirst, wfw);
		    थांबा;
		}
		जर (leftfirst)
		{
		    करा
			frp = frp->fr_next;
		    पर्यंत (wfw && frp != रिक्त && frame_fixed_width(frp));
		}
		अन्यथा
		{
		    करा
			frp = frp->fr_prev;
		    पर्यंत (wfw && frp != रिक्त && frame_fixed_width(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		जर (frp == रिक्त)
		    width -= extra_cols;
	    }
	}
	अन्यथा जर (extra_cols > 0)
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
frame_set_vsep(frame_T *frp, पूर्णांक add)
{
    win_T	*wp;

    जर (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	जर (add && wp->w_vsep_width == 0)
	{
	    जर (wp->w_width > 0)	// don't make it negative
		win_new_width(wp, wp->w_width - 1);
	    wp->w_vsep_width = 1;
	}
	अन्यथा जर (!add && wp->w_vsep_width == 1)
	{
	    win_new_width(wp, wp->w_width + 1);
	    wp->w_vsep_width = 0;
	}
    }
    अन्यथा जर (frp->fr_layout == FR_COL)
    {
	// Handle all the frames in the column.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_set_vsep(frp, add);
    }
    अन्यथा // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_child;
	पर्यंत (frp->fr_next != रिक्त)
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
    स्थिर पूर्णांक
frame_minheight(frame_T *topfrp, win_T *next_curwin)
{
    frame_T	*frp;
    पूर्णांक		m;
    पूर्णांक		n;

    जर (topfrp->fr_win != रिक्त)
    {
	जर (topfrp->fr_win == next_curwin)
	    m = p_wh + topfrp->fr_win->w_status_height;
	अन्यथा
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_status_height;
	    जर (topfrp->fr_win == curwin && next_curwin == रिक्त)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		जर (p_wmh == 0)
		    ++m;
		m += WINBAR_HEIGHT(curwin);
	    }
	}
    }
    अन्यथा जर (topfrp->fr_layout == FR_ROW)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minheight(frp, next_curwin);
	    जर (n > m)
		m = n;
	}
    }
    अन्यथा
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minheight(frp, next_curwin);
    }

    परत m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
स्थिर पूर्णांक
frame_wincount_in_height(frame_T *topfrp)
{
    frame_T	*frp;
    पूर्णांक		c = 0;
    पूर्णांक		n;

    जर (topfrp->fr_win != रिक्त)
	c = 1;
    अन्यथा जर (topfrp->fr_layout == FR_ROW)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_wincount_in_height(frp);
	    जर (n > c)
		c = n;
	}
    }
    अन्यथा
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    c += frame_wincount_in_height(frp);
    }

    परत c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    स्थिर पूर्णांक
frame_minwidth(
    frame_T	*topfrp,
    win_T	*next_curwin)	// use p_wh and p_wiw for next_curwin
{
    frame_T	*frp;
    पूर्णांक		m, n;

    जर (topfrp->fr_win != रिक्त)
    {
	जर (topfrp->fr_win == next_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_width;
	अन्यथा
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_width;
	    // Current window is minimal one column wide
	    जर (p_wmw == 0 && topfrp->fr_win == curwin && next_curwin == रिक्त)
		++m;
	}
    }
    अन्यथा जर (topfrp->fr_layout == FR_COL)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minwidth(frp, next_curwin);
	    जर (n > m)
		m = n;
	}
    }
    अन्यथा
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minwidth(frp, next_curwin);
    }

    परत m;
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
    पूर्णांक		message,
    पूर्णांक		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    पूर्णांक		r;

    जर (one_window())
    {
	जर (message && !autocmd_busy)
	    संदेश(_(m_onlyone));
	परत;
    }

    // Be very careful here: autocommands may change the window layout.
    साठी (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_next;

	// autocommands messed this one up
	जर (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
	}

	जर (wp == curwin)		// don't close current window
	    सुरूठेवा;

	// autoccommands messed this one up
	जर (!buf_valid(wp->w_buffer) && win_valid(wp))
	{
	    wp->w_buffer = रिक्त;
	    win_close(wp, 0);
	    सुरूठेवा;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_buffer, forceit);
	जर (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    सुरूठेवा;
	}
	जर (!r)
	{
#जर defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    जर (message && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_changed(wp->w_buffer, FALSE);
		जर (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    सुरूठेवा;
		}
	    }
	    जर (bufIsChanged(wp->w_buffer))
#endif
		सुरूठेवा;
	}
	win_close(wp, !buf_hide(wp->w_buffer) && !bufIsChanged(wp->w_buffer));
    }

    जर (message && !ONE_WINDOW)
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
स्थिर पूर्णांक command_frame_height = TRUE;

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
    पूर्णांक
win_alloc_first(शून्य)
{
    जर (win_alloc_firstwin(रिक्त) == FAIL)
	परत FAIL;

    first_tabpage = alloc_tabpage();
    जर (first_tabpage == रिक्त)
	परत FAIL;
    curtab = first_tabpage;
    unuse_tabpage(first_tabpage);

    परत OK;
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
    जर (wp == रिक्त)
	परत रिक्त;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_frame(wp);
    परत wp;
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
    स्थिर पूर्णांक
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(रिक्त, FALSE);
    जर (curwin == रिक्त)
	परत FAIL;
    जर (oldwin == रिक्त)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(रिक्त, रिक्त, 1L, BLN_LISTED);
	जर (curwin == रिक्त || curbuf == रिक्त)
	    परत FAIL;
	curwin->w_buffer = curbuf;
#ifdef FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#endif
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    अन्यथा
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_frame(curwin);
    जर (curwin->w_frame == रिक्त)
	परत FAIL;
    topframe = curwin->w_frame;
    topframe->fr_width = COLUMNS_WITHOUT_TPL();
    topframe->fr_height = Rows - p_ch;

    परत OK;
}

/*
 * Create a frame for window "wp".
 */
    स्थिर शून्य
new_frame(win_T *wp)
{
    frame_T *frp = ALLOC_CLEAR_ONE(frame_T);

    wp->w_frame = frp;
    जर (frp == रिक्त)
	परत;
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
    पूर्णांक		i;
#endif


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    जर (tp == रिक्त)
	परत रिक्त;

#ifdef FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    जर (tp->tp_vars == रिक्त)
    {
	vim_free(tp);
	परत रिक्त;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#endif

#ifdef FEAT_GUI
    साठी (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#endif
#ifdef FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#endif
    tp->tp_ch_used = p_ch;

    परत tp;
}

    शून्य
free_tabpage(tabpage_T *tp)
{
    पूर्णांक idx;

#ifdef FEAT_DIFF
    diff_clear(tp);
#endif
#ifdef FEAT_PROP_POPUP
    पर्यंत (tp->tp_first_popupwin != रिक्त)
	popup_close_tabpage(tp, tp->tp_first_popupwin->w_id, TRUE);
#endif
    साठी (idx = 0; idx < SNAP_मोजणी; ++idx)
	clear_snapshot(tp, idx);
#ifdef FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#endif

    जर (tp == lastused_tabpage)
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
    पूर्णांक
win_new_tabpage(पूर्णांक after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    पूर्णांक		n;
#जर defined(FEAT_TABPANEL)
    पूर्णांक prev_columns = COLUMNS_WITHOUT_TPL();
#endif

    जर (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	परत FAIL;
    }
    जर (window_layout_locked(CMD_tabnew))
	परत FAIL;

    newtp = alloc_tabpage();
    जर (newtp == रिक्त)
	परत FAIL;

    // Remember the current windows in this Tab page.
    जर (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	परत FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == रिक्त)
				    ? रिक्त : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    जर (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	जर (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_next = first_tabpage;
	    first_tabpage = newtp;
	}
	अन्यथा
	{
	    जर (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		साठी (tp = first_tabpage; tp->tp_next != रिक्त
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

#जर defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#endif
#ifdef FEAT_JOB_CHANNEL
	entering_window(curwin);
#endif
#जर defined(FEAT_TABPANEL)
	जर (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    पूर्णांक		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		जर (tp2 != curtab)
		{
		    frame_new_width(tp2->tp_topframe, w, FALSE, TRUE);
		    जर (!frame_check_width(tp2->tp_topframe, w))
			frame_new_width(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#endif
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, रिक्त, रिक्त, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, रिक्त, रिक्त, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, रिक्त, रिक्त, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, रिक्त, रिक्त, FALSE, curbuf);
	परत OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    परत FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    स्थिर पूर्णांक
may_open_tabpage(शून्य)
{
    पूर्णांक		n = (cmdmod.cmod_tab == 0)
				       ? postponed_split_tab : cmdmod.cmod_tab;

    जर (n == 0)
	परत FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_split_tab = 0;
    परत win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    पूर्णांक
make_tabpages(पूर्णांक maxcount)
{
    पूर्णांक		मोजणी = maxcount;
    पूर्णांक		todo;

    // Limit to 'tabpagemax' tabs.
    जर (मोजणी > p_tpm)
	मोजणी = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    साठी (todo = मोजणी - 1; todo > 0; --todo)
	जर (win_new_tabpage(0) == FAIL)
	    थांबा;

    unblock_autocmds();

    // return actual number of tab pages
    परत (मोजणी - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    पूर्णांक
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	जर (tp == tpc)
	    परत TRUE;
    परत FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    पूर्णांक
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	जर (tp == tpc)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		जर (win_valid_any_tab(wp))
		    परत TRUE;
	    }
	    परत FALSE;
	}
    }
    // shouldn't happen
    परत FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    शून्य
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    जर (tab == first_tabpage)
    {
	first_tabpage = tab->tp_next;
	ptp = first_tabpage;
    }
    अन्यथा
    {
	साठी (ptp = first_tabpage; ptp != रिक्त && ptp->tp_next != tab;
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
find_tabpage(पूर्णांक n)
{
    tabpage_T	*tp;
    पूर्णांक		i = 1;

    जर (n == 0)
	परत curtab;

    साठी (tp = first_tabpage; tp != रिक्त && i != n; tp = tp->tp_next)
	++i;
    परत tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    पूर्णांक
tabpage_index(tabpage_T *ftp)
{
    पूर्णांक		i = 1;
    tabpage_T	*tp;

    साठी (tp = first_tabpage; tp != रिक्त && tp != ftp; tp = tp->tp_next)
	++i;
    परत i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    स्थिर पूर्णांक
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    पूर्णांक		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ifdef FEAT_JOB_CHANNEL
    leaving_window(curwin);
#endif
    reset_VIsual_and_resel();	// stop Visual mode
    जर (trigger_leave_autocmds)
    {
	जर (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	    जर (curtab != tp)
		परत FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	जर (curtab != tp)
	    परत FAIL;
	apply_autocmds(EVENT_TABLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	जर (curtab != tp)
	    परत FAIL;
    }

    reset_dragwin();
#जर defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    जर (gui.in_use)
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
    जर (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_width;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = रिक्त;
    lastwin = रिक्त;
    परत OK;
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
    पूर्णांक		trigger_enter_autocmds,
    पूर्णांक		trigger_leave_autocmds)
{
    पूर्णांक		old_off = tp->tp_firstwin->w_winrow;
    win_T	*next_prevwin = tp->tp_prevwin;
    tabpage_T	*last_tab = curtab;

    use_tabpage(tp);

    जर (p_ch != curtab->tp_ch_used)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	पूर्णांक new_ch = curtab->tp_ch_used;
	curtab->tp_ch_used = p_ch;
	command_frame_height = FALSE;
	set_option_मूल्य((char_u *)"cmdheight", new_ch, रिक्त, 0);
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
    जर (curtab->tp_old_Rows != ROWS_AVAIL || (old_off != firstwin->w_winrow
#ifdef FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#endif
		))
	shell_new_rows();
    जर (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	जर (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_width;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	अन्यथा
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = last_tab;

#जर defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    जर (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, रिक्त, रिक्त, FALSE, curbuf);
	जर (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    शून्य
goto_tabpage(पूर्णांक n)
{
    tabpage_T	*tp = रिक्त;  // shut up compiler
    tabpage_T	*ttp;
    पूर्णांक		i;

    जर (text_locked())
    {
	// Not allowed when editing the command line.
	text_locked_संदेश();
	परत;
    }

    // If there is only one it can't work.
    जर (first_tabpage->tp_next == रिक्त)
    {
	जर (n > 1)
	    beep_flush();
	परत;
    }

    जर (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	जर (curtab->tp_next == रिक्त)
	    tp = first_tabpage;
	अन्यथा
	    tp = curtab->tp_next;
    }
    अन्यथा जर (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	साठी (i = n; i < 0; ++i)
	{
	    साठी (tp = first_tabpage; tp->tp_next != ttp && tp->tp_next != रिक्त;
		    tp = tp->tp_next)
		;
	    ttp = tp;
	}
    }
    अन्यथा जर (n == 9999)
    {
	// Go to last tab page.
	साठी (tp = first_tabpage; tp->tp_next != रिक्त; tp = tp->tp_next)
	    ;
    }
    अन्यथा
    {
	// Go to tab page "n".
	tp = find_tabpage(n);
	जर (tp == रिक्त)
	{
	    beep_flush();
	    परत;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ifdef FEAT_GUI_TABLINE
    जर (gui_use_tabline())
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
    पूर्णांक		trigger_enter_autocmds,
    पूर्णांक		trigger_leave_autocmds)
{
    जर (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    set_keep_संदेश(रिक्त, 0);

    skip_win_fix_scroll = TRUE;
    जर (tp != curtab && leave_tabpage(tp->tp_curwin->w_buffer,
					trigger_leave_autocmds) == OK)
    {
	जर (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	अन्यथा
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    पूर्णांक
goto_tabpage_lastused(शून्य)
{
    जर (!valid_tabpage(lastused_tabpage))
	परत FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    परत OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    शून्य
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    जर (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ifdef FEAT_GUI_TABLINE
	जर (gui_use_tabline())
	    gui_mch_set_curtab(tabpage_index(curtab));
#endif
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    शून्य
tabpage_move(पूर्णांक nr)
{
    पूर्णांक		n = 1;
    tabpage_T	*tp, *tp_dst;

    जर (first_tabpage->tp_next == रिक्त)
	परत;

    जर (tabpage_move_disallowed)
	परत;

    साठी (tp = first_tabpage; tp->tp_next != रिक्त && n < nr; tp = tp->tp_next)
	++n;

    जर (tp == curtab || (nr > 0 && tp->tp_next != रिक्त
						    && tp->tp_next == curtab))
	परत;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    जर (curtab == first_tabpage)
	first_tabpage = curtab->tp_next;
    अन्यथा
    {
	FOR_ALL_TABPAGES(tp)
	    जर (tp->tp_next == curtab)
		थांबा;
	जर (tp == रिक्त)	// "cannot happen"
	    परत;
	tp->tp_next = curtab->tp_next;
    }

    // Re-insert it at the specified position.
    जर (nr <= 0)
    {
	curtab->tp_next = first_tabpage;
	first_tabpage = curtab;
    }
    अन्यथा
    {
	curtab->tp_next = tp_dst->tp_next;
	tp_dst->tp_next = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#जर defined(FEAT_TABPANEL)
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
    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;
    जर (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_to_enter_popup_window));
	परत;
    }
#endif
    जर (text_or_buf_locked())
    {
	beep_flush();
	परत;
    }

    जर (wp->w_buffer != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    अन्यथा जर (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    जर (!win_valid(wp))
	परत;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    win_enter(wp, TRUE);

#ifdef FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    जर (win_valid(owp) && owp->w_p_cole > 0 && !संदेश_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    जर (curwin->w_p_cole > 0 && !संदेश_scrolled)
	need_cursor_ओळ_redraw = TRUE;
#endif
}

#जर defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_find_nr(पूर्णांक winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	जर (--winnr == 0)
	    थांबा;
    परत wp;
}
#endif

#जर defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_find_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    जर (wp == win)
		परत tp;
    परत रिक्त;
}
#endif

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, पूर्णांक up, दीर्घ मोजणी)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    जर (popup_is_popup(wp))
	// popups don't have neighbors.
	परत रिक्त;
#endif
    foundfr = wp->w_frame;
    पर्यंत (मोजणी--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	साठी (;;)
	{
	    जर (fr == tp->tp_topframe)
		जा end;
	    जर (up)
		nfr = fr->fr_prev;
	    अन्यथा
		nfr = fr->fr_next;
	    जर (fr->fr_parent->fr_layout == FR_COL && nfr != रिक्त)
		थांबा;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	साठी (;;)
	{
	    जर (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		थांबा;
	    }
	    fr = nfr->fr_child;
	    जर (nfr->fr_layout == FR_ROW)
	    {
		// Find the frame at the cursor row.
		पर्यंत (fr->fr_next != रिक्त
			&& frame2win(fr)->w_wincol + fr->fr_width
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_next;
	    }
	    जर (nfr->fr_layout == FR_COL && up)
		पर्यंत (fr->fr_next != रिक्त)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    परत foundfr != रिक्त ? foundfr->fr_win : रिक्त;
}

/*
 * Move to window above or below "count" times.
 */
    स्थिर शून्य
win_goto_ver(
    पूर्णांक		up,		// TRUE to go to win above
    दीर्घ	मोजणी)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    जर (ERROR_IF_TERM_POPUP_WINDOW)
	परत;
#endif
    win = win_vert_neighbor(curtab, curwin, up, मोजणी);
    जर (win != रिक्त)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, पूर्णांक डावा, दीर्घ मोजणी)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    जर (popup_is_popup(wp))
	// popups don't have neighbors.
	परत रिक्त;
#endif
    foundfr = wp->w_frame;
    पर्यंत (मोजणी--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	साठी (;;)
	{
	    जर (fr == tp->tp_topframe)
		जा end;
	    जर (डावा)
		nfr = fr->fr_prev;
	    अन्यथा
		nfr = fr->fr_next;
	    जर (fr->fr_parent->fr_layout == FR_ROW && nfr != रिक्त)
		थांबा;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	साठी (;;)
	{
	    जर (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		थांबा;
	    }
	    fr = nfr->fr_child;
	    जर (nfr->fr_layout == FR_COL)
	    {
		// Find the frame at the cursor row.
		पर्यंत (fr->fr_next != रिक्त
			&& frame2win(fr)->w_winrow + fr->fr_height
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_next;
	    }
	    जर (nfr->fr_layout == FR_ROW && डावा)
		पर्यंत (fr->fr_next != रिक्त)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    परत foundfr != रिक्त ? foundfr->fr_win : रिक्त;
}

/*
 * Move to left or right window.
 */
    स्थिर शून्य
win_goto_hor(
    पूर्णांक		डावा,		// TRUE to go to left win
    दीर्घ	मोजणी)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    जर (ERROR_IF_TERM_POPUP_WINDOW)
	परत;
#endif
    win = win_horz_neighbor(curtab, curwin, डावा, मोजणी);
    जर (win != रिक्त)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    शून्य
win_enter(win_T *wp, पूर्णांक undo_sync)
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
    जर (curwin->w_localdir != रिक्त || curtab->tp_localdir != रिक्त)
    {
	char_u	*स_नाम;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	जर (globaldir == रिक्त)
	{
	    char_u	cwd[MAXPATHL];

	    जर (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	जर (curwin->w_localdir != रिक्त)
	    स_नाम = curwin->w_localdir;
	अन्यथा
	    स_नाम = curtab->tp_localdir;

	जर (mch_chdir((अक्षर *)स_नाम) == 0)
	{
	    last_chdir_reason = रिक्त;
	    shorten_fnames(TRUE);
	}
    }
    अन्यथा जर (globaldir != रिक्त)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((अक्षर *)globaldir);
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
    स्थिर पूर्णांक
win_enter_ext(win_T *wp, पूर्णांक flags)
{
    पूर्णांक		other_buffer = FALSE;
    पूर्णांक		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    पूर्णांक		did_decrement = FALSE;

    जर (wp == curwin && curwin_invalid == 0)	// nothing to do
	परत FALSE;

#ifdef FEAT_JOB_CHANNEL
    जर (curwin_invalid == 0)
	leaving_window(curwin);
#endif

    जर (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	जर (wp->w_buffer != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	    other_buffer = TRUE;
	    जर (!win_valid(wp))
		परत FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, रिक्त, रिक्त, FALSE, curbuf);
	जर (!win_valid(wp))
	    परत FALSE;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	जर (aborting())
	    परत FALSE;
#endif
    }

    // sync undo before leaving the current buffer
    जर ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_buffer)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    जर (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    जर (wp->w_buffer != curbuf)
	buf_copy_options(wp->w_buffer, BCO_ENTER | BCO_NOHELP);
    जर (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_status = true;
    }
    curwin = wp;
    curbuf = wp->w_buffer;
    check_cursor();
    जर (!virtual_active())
	curwin->w_cursor.coladd = 0;
    जर (*p_spk == 'c')		// assume cursor position needs updating
	changed_ओळ_abv_curs();
    अन्यथा
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ifdef MESSAGE_QUEUE
    जर (flags & WEE_ALLOW_PARSE_MESSAGES)
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
    जर (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, रिक्त, रिक्त, FALSE, curbuf);
    जर (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, रिक्त, रिक्त, FALSE, curbuf);
	जर (other_buffer)
	    apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_status = true;
#ifdef FEAT_TERMINAL
    जर (bt_terminal(curwin->w_buffer))
	// terminal is likely in another mode
	redraw_mode = TRUE;
#endif
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#जर defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    जर (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    जर (curwin->w_height < p_wh && !curwin->w_p_wfh
#ifdef FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#endif
	    )
	win_setheight((पूर्णांक)p_wh);
    अन्यथा जर (curwin->w_height == 0)
	win_setheight(1);

    // set window width to desired minimal value
    जर (curwin->w_width < p_wiw && !curwin->w_p_wfw)
	win_setwidth((पूर्णांक)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    परत did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = रिक्त;

    जर (curwin->w_buffer == buf)
	wp = curwin;
    अन्यथा
	FOR_ALL_WINDOWS(wp)
	    जर (wp->w_buffer == buf)
		थांबा;
    जर (wp != रिक्त)
	win_enter(wp, FALSE);
    परत wp;
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

    जर (wp != रिक्त)
	परत wp;

    FOR_ALL_TABPAGES(tp)
	जर (tp != curtab)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		जर (wp->w_buffer == buf)
		    थांबा;
	    जर (wp != रिक्त)
	    {
		goto_tabpage_win(tp, wp);
		जर (curwin != wp)
		    wp = रिक्त;	// something went wrong
		थांबा;
	    }
	}
    परत wp;
}

स्थिर पूर्णांक last_win_id = LOWEST_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    स्थिर win_T *
win_alloc(win_T *after, पूर्णांक hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    जर (new_wp == रिक्त)
	परत रिक्त;

    जर (win_alloc_ओळी(new_wp) == FAIL)
    {
	vim_free(new_wp);
	परत रिक्त;
    }

    new_wp->w_id = ++last_win_id;

#ifdef FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    जर (new_wp->w_vars == रिक्त)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	परत रिक्त;
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
    जर (!hidden)
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
    जर (gui.in_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_डावा],
		SBAR_डावा, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_उजवा],
		SBAR_उजवा, new_wp);
    }
#endif
#ifdef FEAT_FOLDING
    foldInitWin(new_wp);
#endif
    unblock_autocmds();
#ifdef FEAT_शोध_EXTRA
    new_wp->w_next_match_id = 1000;  // up to 1000 can be picked by the user
#endif
    परत new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    स्थिर शून्य
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    पूर्णांक		i;
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
    जर (wp->w_stl_click != रिक्त)
    {
	साठी (i = 0; i < wp->w_stl_click_मोजणी; i++)
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

	जर (prevwin == wp)
	    prevwin = रिक्त;
	FOR_ALL_TABPAGES(ttp)
	    जर (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = रिक्त;
    }
    win_free_lsize(wp);

    साठी (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    जर (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    जर (wip2 != wip && wip2->wi_win == रिक्त)
		    {
			जर (wip2->wi_next != रिक्त)
			    wip2->wi_next->wi_prev = wip2->wi_prev;
			जर (wip2->wi_prev == रिक्त)
			    buf->b_wininfo = wip2->wi_next;
			अन्यथा
			    wip2->wi_prev->wi_next = wip2->wi_next;
			free_wininfo(wip2);
			थांबा;
		    }

		wip->wi_win = रिक्त;
	    }

#ifdef FEAT_शोध_EXTRA
    clear_matches(wp);
#endif

    free_jumplist(wp);

#ifdef FEAT_QUICKFIX
    qf_free_all(wp);
#endif

#ifdef FEAT_GUI
    जर (gui.in_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_डावा]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_उजवा]);
    }
#endif // FEAT_GUI

#ifdef FEAT_MENU
    remove_winbar(wp);
#endif
#ifdef FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    साठी (i = 0; i < 4; ++i)
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

    जर (win_valid_any_tab(wp))
	win_remove(wp, tp);
    जर (autocmd_busy)
    {
	wp->w_next = au_pending_free_win;
	au_pending_free_win = wp;
    }
    अन्यथा
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    पूर्णांक
win_unlisted(win_T *wp)
{
    परत is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#जर defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    शून्य
win_free_popup(win_T *win)
{
    जर (win->w_buffer != रिक्त)
    {
	जर (bt_popup(win->w_buffer))
	    win_close_buffer(win, DOBUF_WIPE_REUSE, FALSE);
	अन्यथा
	    close_buffer(win, win->w_buffer, 0, FALSE, FALSE, TRUE);

	जर (win->w_buffer != रिक्त)
	    --win->w_buffer->b_nwindows;
    }
# जर defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    जर (timer_valid(win->w_popup_timer))
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

    जर (after == रिक्त)	    // after NULL is in front of the first
	before = firstwin;
    अन्यथा
	before = after->w_next;

    wp->w_next = before;
    wp->w_prev = after;
    जर (after == रिक्त)
	firstwin = wp;
    अन्यथा
	after->w_next = wp;
    जर (before == रिक्त)
	lastwin = wp;
    अन्यथा
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
    जर (wp->w_prev != रिक्त)
	wp->w_prev->w_next = wp->w_next;
    अन्यथा जर (tp == रिक्त)
	firstwin = curtab->tp_firstwin = wp->w_next;
    अन्यथा
	tp->tp_firstwin = wp->w_next;

    जर (wp->w_next != रिक्त)
	wp->w_next->w_prev = wp->w_prev;
    अन्यथा जर (tp == रिक्त)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    अन्यथा
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
    जर (frp->fr_next != रिक्त)
	frp->fr_next->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    स्थिर शून्य
frame_घाला(frame_T *before, frame_T *frp)
{
    frp->fr_next = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    जर (frp->fr_prev != रिक्त)
	frp->fr_prev->fr_next = frp;
    अन्यथा
	frp->fr_parent->fr_child = frp;
}

/*
 * Remove a frame from a frame list.
 */
    स्थिर शून्य
frame_remove(frame_T *frp)
{
    जर (frp->fr_prev != रिक्त)
	frp->fr_prev->fr_next = frp->fr_next;
    अन्यथा
	frp->fr_parent->fr_child = frp->fr_next;
    जर (frp->fr_next != रिक्त)
	frp->fr_next->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    पूर्णांक
win_alloc_ओळी(win_T *wp)
{
    wp->w_ओळी_valid = 0;
    wp->w_ओळी = ALLOC_CLEAR_MULT(wline_T, Rows);
    जर (wp->w_ओळी == रिक्त)
	परत FAIL;
    परत OK;
}

/*
 * free lsize arrays for a window
 */
    शून्य
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    जर (wp != रिक्त)
	VIM_CLEAR(wp->w_ओळी);
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
    पूर्णांक		h = (पूर्णांक)ROWS_AVAIL;

    जर (firstwin == रिक्त)	// not initialized yet
	परत;
    जर (h < frame_minheight(topframe, रिक्त))
	h = frame_minheight(topframe, रिक्त);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    frame_new_height(topframe, h, FALSE, TRUE, FALSE);
    जर (!frame_check_height(topframe, h))
	frame_new_height(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_used = p_ch;

    जर (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#जर defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#जर 0
    // Disabled: don't want making the screen smaller make a window larger.
    जर (p_ea)
	win_equal(curwin, FALSE, 'v');
#endif
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    शून्य
shell_new_columns(शून्य)
{
    जर (firstwin == रिक्त)	// not initialized yet
	परत;

#जर defined(FEAT_TABPANEL)
    स्थिर पूर्णांक prev_tp_width;
    स्थिर पूर्णांक prev_wincol;
    स्थिर पूर्णांक prev_fr_width;
#endif
    पूर्णांक w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    frame_new_width(topframe, w, FALSE, TRUE);
    जर (!frame_check_width(topframe, w))
	frame_new_width(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#जर defined(FEAT_TABPANEL)
    पूर्णांक tp_width = tabpanel_width();
    पूर्णांक tp_width_changed = tp_width != prev_tp_width;
    // tabpanel width changed
    जर (tp_width_changed && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    जर (tp_width_changed
	    || (tp_width > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_width != prev_fr_width)))
    {
	screen_fill(cmdline_row, (पूर्णांक)Rows, 0, (पूर्णांक)Columns, ' ', ' ', 0);
    }
    prev_tp_width = tabpanel_width();
    prev_wincol = firstwin->w_wincol;
    prev_fr_width = topframe->fr_width;
#endif
    // Adjust offset for command line start column
    cmdline_col_off = firstwin->w_wincol;
    cmdline_width = topframe->fr_width;
    comp_col();
    जर (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#जर defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#जर 0
    // Disabled: don't want making the screen smaller make a window larger.
    जर (p_ea)
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

    ga_init2(gap, आकार(पूर्णांक), 1);
    जर (ga_grow(gap, win_मोजणी() * 2 + 1) == FAIL)
	परत;

    // first entry is the total lines available for windows
    ((पूर्णांक *)gap->ga_data)[gap->ga_len++] = ROWS_AVAIL - last_stl_height(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((पूर्णांक *)gap->ga_data)[gap->ga_len++] =
	    wp->w_width + wp->w_vsep_width;
	((पूर्णांक *)gap->ga_data)[gap->ga_len++] = wp->w_height;
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
    पूर्णांक		i, j;

    जर (win_मोजणी() * 2 + 1 == gap->ga_len
	    && ((पूर्णांक *)gap->ga_data)[0] == ROWS_AVAIL - last_stl_height(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	साठी (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		frame_setwidth(wp->w_frame, ((पूर्णांक *)gap->ga_data)[i++]);
		win_setheight_win(((पूर्णांक *)gap->ga_data)[i++], wp);
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
    पूर्णांक		row = tabline_height();
    पूर्णांक		col = TPL_LCOL();

    frame_comp_pos(topframe, &row, &col);
#जर defined(FEAT_STL_OPT)
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
frame_comp_pos(frame_T *topfrp, पूर्णांक *row, पूर्णांक *col)
{
    win_T	*wp;
    frame_T	*frp;
    पूर्णांक		startcol;
    पूर्णांक		startrow;
    पूर्णांक		h;

    wp = topfrp->fr_win;
    जर (wp != रिक्त)
    {
	जर (wp->w_winrow != *row || wp->w_wincol != *col)
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
    अन्यथा
    {
	startrow = *row;
	startcol = *col;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    जर (topfrp->fr_layout == FR_ROW)
		*row = startrow;	// all frames are at the same row
	    अन्यथा
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
    जर (curwin->w_height == 0)
	win_setheight(1);
    जर (curwin->w_width == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    शून्य
win_setheight(पूर्णांक height)
{
    win_setheight_win(height, curwin);
#जर defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    शून्य
win_setheight_win(पूर्णांक height, win_T *win)
{
    जर (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	जर (height < p_wmh)
	    height = p_wmh;
	जर (height == 0)
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
frame_setheight(frame_T *curfrp, पूर्णांक height)
{
    पूर्णांक		room;		// total number of lines available
    पूर्णांक		take;		// number of lines taken from other windows
    पूर्णांक		room_cmdline;	// lines available from cmdline
    पूर्णांक		run;
    frame_T	*frp;
    पूर्णांक		h;
    पूर्णांक		room_reserved;

    // If the height already is the desired value, nothing to do.
    जर (curfrp->fr_height == height)
	परत;

    जर (curfrp->fr_parent == रिक्त)
    {
	// topframe: can only change the command line height
	जर (height > 0)
	    frame_new_height(curfrp, height, FALSE, FALSE, TRUE);
    }
    अन्यथा जर (curfrp->fr_parent->fr_layout == FR_ROW)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = frame_minheight(curfrp->fr_parent, रिक्त);
	जर (height < h)
	    height = h;
	frame_setheight(curfrp->fr_parent, height);
    }
    अन्यथा
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
	साठी (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		जर (frp != curfrp
			&& frp->fr_win != रिक्त
			&& frp->fr_win->w_p_wfh)
		    room_reserved += frp->fr_height;
		room += frp->fr_height;
		जर (frp != curfrp)
		    room -= frame_minheight(frp, रिक्त);
	    }
	    जर (curfrp->fr_width != topframe->fr_width)
		room_cmdline = 0;
	    अन्यथा
	    {
		room_cmdline = Rows - p_ch - (lastwin->w_winrow
						+ VISIBLE_HEIGHT(lastwin)
						+ lastwin->w_status_height);
		जर (room_cmdline < 0)
		    room_cmdline = 0;
	    }

	    जर (height <= room + room_cmdline)
		थांबा;
	    जर (run == 2 || curfrp->fr_width == topframe->fr_width)
	    {
		height = room + room_cmdline;
		थांबा;
	    }
	    frame_setheight(curfrp->fr_parent, height
		+ frame_minheight(curfrp->fr_parent, NOWIN) - (पूर्णांक)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = height - curfrp->fr_height;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	जर (height > room + room_cmdline - room_reserved)
	    room_reserved = room + room_cmdline - height;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	जर (take < 0 && room - curfrp->fr_height <= room_reserved)
	    room_reserved = 0;

	जर (take > 0 && room_cmdline > 0)
	{
	    // use lines from cmdline first
	    जर (take < room_cmdline)
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
	साठी (run = 0; run < 2; ++run)
	{
	    जर (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    अन्यथा
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    पर्यंत (frp != रिक्त && take != 0)
	    {
		h = frame_minheight(frp, रिक्त);
		जर (room_reserved > 0
			&& frp->fr_win != रिक्त
			&& frp->fr_win->w_p_wfh)
		{
		    जर (room_reserved >= frp->fr_height)
			room_reserved -= frp->fr_height;
		    अन्यथा
		    {
			जर (frp->fr_height - room_reserved > take)
			    room_reserved = frp->fr_height - take;
			take -= frp->fr_height - room_reserved;
			frame_new_height(frp, room_reserved, FALSE, FALSE,
									TRUE);
			room_reserved = 0;
		    }
		}
		अन्यथा
		{
		    जर (frp->fr_height - take < h)
		    {
			take -= frp->fr_height - h;
			frame_new_height(frp, h, FALSE, FALSE, TRUE);
		    }
		    अन्यथा
		    {
			frame_new_height(frp, frp->fr_height - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		जर (run == 0)
		    frp = frp->fr_next;
		अन्यथा
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
win_setwidth(पूर्णांक width)
{
    win_setwidth_win(width, curwin);
}

    शून्य
win_setwidth_win(पूर्णांक width, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    जर (wp == curwin)
    {
	जर (width < p_wmw)
	    width = p_wmw;
	जर (width == 0)
	    width = 1;
    }
    अन्यथा जर (width < 0)
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
frame_setwidth(frame_T *curfrp, पूर्णांक width)
{
    पूर्णांक		room;		// total number of lines available
    पूर्णांक		take;		// number of lines taken from other windows
    पूर्णांक		run;
    frame_T	*frp;
    पूर्णांक		w;
    पूर्णांक		room_reserved;

    // If the width already is the desired value, nothing to do.
    जर (curfrp->fr_width == width)
	परत;

    जर (curfrp->fr_parent == रिक्त)
	// topframe: can't change width
	परत;

    जर (curfrp->fr_parent->fr_layout == FR_COL)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = frame_minwidth(curfrp->fr_parent, रिक्त);
	जर (width < w)
	    width = w;
	frame_setwidth(curfrp->fr_parent, width);
    }
    अन्यथा
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	साठी (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		जर (frp != curfrp
			&& frp->fr_win != रिक्त
			&& frp->fr_win->w_p_wfw)
		    room_reserved += frp->fr_width;
		room += frp->fr_width;
		जर (frp != curfrp)
		    room -= frame_minwidth(frp, रिक्त);
	    }

	    जर (width <= room)
		थांबा;
	    जर (run == 2 || curfrp->fr_height >= ROWS_AVAIL)
	    {
		width = room;
		थांबा;
	    }
	    frame_setwidth(curfrp->fr_parent, width
		 + frame_minwidth(curfrp->fr_parent, NOWIN) - (पूर्णांक)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = width - curfrp->fr_width;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	जर (width > room - room_reserved)
	    room_reserved = room - width;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	जर (take < 0 && room - curfrp->fr_width < room_reserved)
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
	साठी (run = 0; run < 2; ++run)
	{
	    जर (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    अन्यथा
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    पर्यंत (frp != रिक्त && take != 0)
	    {
		w = frame_minwidth(frp, रिक्त);
		जर (room_reserved > 0
			&& frp->fr_win != रिक्त
			&& frp->fr_win->w_p_wfw)
		{
		    जर (room_reserved >= frp->fr_width)
			room_reserved -= frp->fr_width;
		    अन्यथा
		    {
			जर (frp->fr_width - room_reserved > take)
			    room_reserved = frp->fr_width - take;
			take -= frp->fr_width - room_reserved;
			frame_new_width(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		अन्यथा
		{
		    जर (frp->fr_width - take < w)
		    {
			take -= frp->fr_width - w;
			frame_new_width(frp, w, FALSE, FALSE);
		    }
		    अन्यथा
		    {
			frame_new_width(frp, frp->fr_width - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		जर (run == 0)
		    frp = frp->fr_next;
		अन्यथा
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
    पूर्णांक		room;
    पूर्णांक		needed;
    पूर्णांक		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    पर्यंत (p_wmh > 0)
    {
	room = Rows - p_ch;
	needed = min_rows_for_all_tabpages() - 1;  // 1 was added for the cmdline
	जर (room >= needed)
	    थांबा;
	--p_wmh;
	जर (first)
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
    पूर्णांक		room;
    पूर्णांक		needed;
    पूर्णांक		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    पर्यंत (p_wmw > 0)
    {
	room = topframe->fr_width;
	needed = frame_minwidth(topframe, रिक्त);
	जर (room >= needed)
	    थांबा;
	--p_wmw;
	जर (first)
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
win_drag_status_ओळ(win_T *dragwin, पूर्णांक offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    पूर्णांक		room;
    पूर्णांक		up;	// if TRUE, drag status line up, otherwise down
    पूर्णांक		n;

    fr = dragwin->w_frame;
    curfr = fr;
    जर (fr != topframe)		// more than one window
    {
	fr = fr->fr_parent;
	// When the parent frame is not a column of frames, its parent should
	// be.
	जर (fr->fr_layout != FR_COL)
	{
	    curfr = fr;
	    जर (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_parent;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    पर्यंत (curfr != topframe && curfr->fr_next == रिक्त)
    {
	जर (fr != topframe)
	    fr = fr->fr_parent;
	curfr = fr;
	जर (fr != topframe)
	    fr = fr->fr_parent;
    }

    जर (offset < 0) // drag up
    {
	up = TRUE;
	offset = -offset;
	// sum up the room of the current frame and above it
	जर (fr == curfr)
	{
	    // only one window
	    room = fr->fr_height - frame_minheight(fr, रिक्त);
	}
	अन्यथा
	{
	    room = 0;
	    साठी (fr = fr->fr_child; ; fr = fr->fr_next)
	    {
		room += fr->fr_height - frame_minheight(fr, रिक्त);
		जर (fr == curfr)
		    थांबा;
	    }
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    अन्यथा    // drag down
    {
	up = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	room = Rows - cmdline_row;
	जर (curfr->fr_next == रिक्त)
	    --room;
	अन्यथा
	    room -= p_ch;
	जर (room < 0)
	    room = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_height - frame_minheight(fr, रिक्त);
	fr = curfr;			// put fr at window that grows
    }

    जर (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    जर (offset <= 0)
	परत;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    जर (fr != रिक्त)
	frame_new_height(fr, fr->fr_height + offset, up, FALSE, TRUE);

    जर (up)
	fr = curfr;		// current frame gets smaller
    अन्यथा
	fr = curfr->fr_next;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    पर्यंत (fr != रिक्त && offset > 0)
    {
	n = frame_minheight(fr, रिक्त);
	जर (fr->fr_height - offset <= n)
	{
	    offset -= fr->fr_height - n;
	    frame_new_height(fr, n, !up, FALSE, TRUE);
	}
	अन्यथा
	{
	    frame_new_height(fr, fr->fr_height - offset, !up, FALSE, TRUE);
	    थांबा;
	}
	जर (up)
	    fr = fr->fr_prev;
	अन्यथा
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
win_drag_vsep_ओळ(win_T *dragwin, पूर्णांक offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    पूर्णांक		room;
    पूर्णांक		डावा;	// if TRUE, drag separator line left, otherwise right
    पूर्णांक		n;

    fr = dragwin->w_frame;
    जर (fr == topframe)		// only one window (cannot happen?)
	परत;
    curfr = fr;
    fr = fr->fr_parent;
    // When the parent frame is not a row of frames, its parent should be.
    जर (fr->fr_layout != FR_ROW)
    {
	जर (fr == topframe)	// only a column of windows (cannot happen?)
	    परत;
	curfr = fr;
	fr = fr->fr_parent;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    पर्यंत (curfr->fr_next == रिक्त)
    {
	जर (fr == topframe)
	    थांबा;
	curfr = fr;
	fr = fr->fr_parent;
	जर (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_parent;
	}
    }

    जर (offset < 0) // drag left
    {
	डावा = TRUE;
	offset = -offset;
	// sum up the room of the current frame and left of it
	room = 0;
	साठी (fr = fr->fr_child; ; fr = fr->fr_next)
	{
	    room += fr->fr_width - frame_minwidth(fr, रिक्त);
	    जर (fr == curfr)
		थांबा;
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    अन्यथा    // drag right
    {
	डावा = FALSE;
	// sum up the room of frames right of the current one
	room = 0;
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_width - frame_minwidth(fr, रिक्त);
	fr = curfr;			// put fr at window that grows
    }

    जर (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    जर (offset <= 0)		// No room at all, quit.
	परत;
    जर (fr == रिक्त)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	परत;

    // grow frame fr by offset lines
    frame_new_width(fr, fr->fr_width + offset, डावा, FALSE);

    // shrink other frames: current and at the left or at the right
    जर (डावा)
	fr = curfr;		// current frame gets smaller
    अन्यथा
	fr = curfr->fr_next;	// next frame gets smaller

    पर्यंत (fr != रिक्त && offset > 0)
    {
	n = frame_minwidth(fr, रिक्त);
	जर (fr->fr_width - offset <= n)
	{
	    offset -= fr->fr_width - n;
	    frame_new_width(fr, n, !डावा, FALSE);
	}
	अन्यथा
	{
	    frame_new_width(fr, fr->fr_width - offset, !डावा, FALSE);
	    थांबा;
	}
	जर (डावा)
	    fr = fr->fr_prev;
	अन्यथा
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
    जर (wp->w_height > 1)
	// When cursor is in the first line the percentage is computed as if
	// it's halfway that line.  Thus with two lines it is 25%, with three
	// lines 17%, etc.  Similarly for the last line: 75%, 83%, etc.
	wp->w_fraction = ((दीर्घ)wp->w_wrow * FRACTION_MULT
				     + FRACTION_MULT / 2) / (दीर्घ)wp->w_height;
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
win_fix_scroll(पूर्णांक resize)
{
    जर (*p_spk == 'c')
	परत;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	जर (wp->w_height != wp->w_prev_height)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    जर (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_buffer->b_ml.ml_ओळ_मोजणी)
	    {
		पूर्णांक diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_height - wp->w_prev_height);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		जर (diff > 0)
		    cursor_down_inner(wp, diff);
		अन्यथा
		    cursor_up_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_to_fraction(wp, wp->w_prev_height);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		जर (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    अन्यथा जर (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    जर (!(get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL)))
	win_fix_cursor(FALSE);
    अन्यथा जर (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    स्थिर शून्य
win_fix_cursor(पूर्णांक normal)
{
    win_T	*wp = curwin;

    जर (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_buffer->b_ml.ml_ओळ_मोजणी < wp->w_height)
	परत;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    दीर्घ so = MIN(wp->w_height / 2, get_scrolloff_मूल्य());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_down_inner(wp, so);
    linenr_T शीर्ष = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_up_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    जर (lnum > bot && (wp->w_botline - wp->w_buffer->b_ml.ml_ओळ_मोजणी) != 1)
	nlnum = bot;
    अन्यथा जर (lnum < शीर्ष && wp->w_topline != 1)
	nlnum = (so == wp->w_height / 2) ? bot : शीर्ष;

    जर (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	जर (normal)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	अन्यथा  // Scroll instead when not in normal mode.
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
win_new_height(win_T *wp, पूर्णांक height)
{
    पूर्णांक		prev_height = wp->w_height;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    जर (height < 0)
	height = 0;
    जर (wp->w_height == height)
	परत;	    // nothing to do

    जर (wp->w_height > 0)
    {
	जर (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	जर (wp->w_height != prev_height)
	    परत;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	जर (wp->w_wrow != wp->w_prev_fraction_row)
	    set_fraction(wp);
    }

    wp->w_height = height;
    wp->w_redr_status = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    जर (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	जर (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_to_fraction(wp, prev_height);
    }
}

    शून्य
scroll_to_fraction(win_T *wp, पूर्णांक prev_height)
{
    linenr_T	lnum;
    पूर्णांक		sline, ओळ_size;
    पूर्णांक		height = wp->w_height;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    जर (height > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (height < wp->w_buffer->b_ml.ml_ओळ_मोजणी || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	जर (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((दीर्घ)wp->w_fraction * (दीर्घ)height - 1L)
							       / FRACTION_MULT;
	ओळ_size = plines_win_col(wp, lnum, (दीर्घ)(wp->w_cursor.col)) - 1;
	sline = wp->w_wrow - ओळ_size;

	जर (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    पूर्णांक rows = plines_win(wp, lnum, FALSE);

	    जर (sline > wp->w_height - rows)
	    {
		sline = wp->w_height - rows;
		wp->w_wrow -= rows - ओळ_size;
	    }
	}

	जर (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    पूर्णांक		want_row = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = ओळ_size;
	    जर (wp->w_wrow >= wp->w_height
				       && (wp->w_width - win_col_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_width - win_col_off(wp);

		--wp->w_wrow;
		पर्यंत (wp->w_wrow > want_row)
		{
		    skipcol += wp->w_width - win_col_off(wp) + win_col_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	अन्यथा जर (sline > 0)
	{
	    पर्यंत (sline > 0 && lnum > 1)
	    {
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, रिक्त, TRUE, रिक्त);
		जर (lnum == 1)
		{
		    // first line in buffer is folded
		    ओळ_size = 1;
		    --sline;
		    थांबा;
		}
#endif
		--lnum;
#ifdef FEAT_DIFF
		जर (lnum == wp->w_topline)
		    ओळ_size = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		अन्यथा
#endif
		    ओळ_size = plines_win(wp, lnum, TRUE);
		sline -= ओळ_size;
	    }

	    जर (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, रिक्त, &lnum, TRUE, रिक्त);
#endif
		lnum++;
		wp->w_wrow -= ओळ_size + sline;
	    }
	    अन्यथा जर (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	set_topline(wp, lnum);
    }

    जर (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    जर (prev_height > 0)
	wp->w_prev_fraction_row = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    शून्य
win_new_width(win_T *wp, पूर्णांक width)
{
    // Should we give an error if width < 0?
    wp->w_width = width < 0 ? 0 : width;
    wp->w_ओळी_valid = 0;
    changed_ओळ_abv_curs_win(wp);
    invalidate_botline_win(wp);

    जर (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_status = true;
}

    शून्य
win_comp_scroll(win_T *wp)
{
#जर defined(FEAT_EVAL)
    पूर्णांक old_w_p_scr = wp->w_p_scr;
#endif

    wp->w_p_scr = ((अचिन्हित)wp->w_height >> 1);
    जर (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#जर defined(FEAT_EVAL)
    जर (wp->w_p_scr != old_w_p_scr)
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
    पूर्णांक		old_p_ch = curtab->tp_ch_used;

    // Find bottom frame with width of screen.
    frame_T *frp = lastwin->w_frame;
    पर्यंत (frp->fr_width != topframe->fr_width && frp->fr_parent != रिक्त)
	frp = frp->fr_parent;

    // Avoid changing the height of a window with 'winfixheight' set.
    पर्यंत (frp->fr_prev != रिक्त && frp->fr_layout == FR_LEAF
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    पर्यंत (p_ch > old_p_ch && command_frame_height)
    {
	जर (frp == रिक्त)
	{
	    emsg(_(e_not_enough_room));
	    p_ch = old_p_ch;
	    थांबा;
	}
	पूर्णांक h = MIN(p_ch - old_p_ch,
			frp->fr_height - frame_minheight(frp, रिक्त));
	frame_add_height(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    जर (p_ch < old_p_ch && command_frame_height && frp != रिक्त)
	frame_add_height(frp, (पूर्णांक)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_row = Rows - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    जर (संदेश_scrolled == 0 && full_screen)
    {
	screen_fill(cmdline_row, (पूर्णांक)Rows, 0, (पूर्णांक)Columns, ' ', ' ', 0);
	संदेश_row = cmdline_row;
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
frame_add_height(frame_T *frp, पूर्णांक n)
{
    frame_new_height(frp, frp->fr_height + n, FALSE, FALSE, FALSE);
    साठी (;;)
    {
	frp = frp->fr_parent;
	जर (frp == रिक्त)
	    थांबा;
	frp->fr_height += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    शून्य
last_status(
    पूर्णांक		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    last_status_rec(topframe, last_stl_height(morewin) > 0);
#जर defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

    स्थिर शून्य
last_status_rec(frame_T *fr, पूर्णांक statusline)
{
    frame_T	*fp;
    win_T	*wp;

    जर (fr->fr_layout == FR_LEAF)
    {
	wp = fr->fr_win;
	जर (wp->w_status_height != 0 && !statusline)
	{
	    // remove status line
	    win_new_height(wp, wp->w_height + wp->w_status_height);
	    wp->w_status_height = 0;
	    comp_col();
	}
	अन्यथा जर (wp->w_status_height == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    पर्यंत (fp->fr_height <= frame_minheight(fp, रिक्त))
	    {
		जर (fp == topframe)
		{
		    emsg(_(e_not_enough_room));
		    परत;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		जर (fp->fr_parent->fr_layout == FR_COL && fp->fr_prev != रिक्त)
		    fp = fp->fr_prev;
		अन्यथा
		    fp = fp->fr_parent;
	    }
	    wp->w_status_height = statusline_height(wp);
	    जर (fp != fr)
	    {
		frame_new_height(fp, fp->fr_height - wp->w_status_height,
			FALSE, FALSE, FALSE);
		frame_fix_height(wp);
		win_comp_pos();
	    }
	    अन्यथा
		win_new_height(wp, wp->w_height - wp->w_status_height);
	    comp_col();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	जर (असल(wp->w_height - wp->w_prev_height) == 1)
	    wp->w_prev_height = wp->w_height;
    }
    अन्यथा जर (fr->fr_layout == FR_ROW)
    {
	// vertically split windows, set status line for each one
	जर (!statusline)
	{
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		last_status_rec(fp, statusline);
	}
	अन्यथा
	{
	    frame_T	*fp2;
	    पूर्णांक		max_stlh = 0;
	    पूर्णांक		new_row_height;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		जर (fp->fr_win != रिक्त && fp->fr_win->w_status_height == 0)
		{
		    पूर्णांक h = statusline_height(fp->fr_win);
		    जर (h > max_stlh)
			max_stlh = h;
		}
	    जर (max_stlh == 0)
		परत;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    पर्यंत (fp2->fr_height - frame_minheight(fp2, रिक्त) < max_stlh)
	    {
		जर (fp2 == topframe)
		{
		    emsg(_(e_not_enough_room));
		    परत;
		}
		जर (fp2->fr_parent->fr_layout == FR_COL
						&& fp2->fr_prev != रिक्त)
		    fp2 = fp2->fr_prev;
		अन्यथा
		    fp2 = fp2->fr_parent;
	    }

	    जर (fp2 != fr)
	    {
		frame_new_height(fp2, fp2->fr_height - max_stlh,
							FALSE, FALSE, FALSE);
		new_row_height = fr->fr_height + max_stlh;
	    }
	    अन्यथा
		new_row_height = fr->fr_height;

	    // Set status and content heights for all leaves.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
	    {
		जर (fp->fr_win != रिक्त)
		{
		    wp = fp->fr_win;
		    जर (wp->w_status_height == 0)
		    {
			wp->w_status_height = statusline_height(wp);
			win_new_height(wp,
				  new_row_height - wp->w_status_height);
			frame_fix_height(wp);
			comp_col();
			redraw_all_later(UPD_SOME_VALID);
			जर (असल(wp->w_height - wp->w_prev_height) == 1)
			    wp->w_prev_height = wp->w_height;
		    }
		}
		अन्यथा
		    last_status_rec(fp, statusline); // nested frames
	    }

	    fr->fr_height = new_row_height;
	    win_comp_pos();
	}
    }
    अन्यथा
    {
	// horizontally split window, set status line for last one
	साठी (fp = fr->fr_child; fp->fr_next != रिक्त; fp = fp->fr_next)
	    ;
	last_status_rec(fp, statusline);
    }
}

#जर defined(FEAT_STL_OPT)
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
    पूर्णांक		opt_flags = (wp == रिक्त) ? 0 : OPT_LOCAL;
    पूर्णांक		*height = (wp == रिक्त) ? &stl_rendered_height
						: &wp->w_stl_rendered_height;

    *height = get_stl_rendered_height(wp == रिक्त ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    स्थिर पूर्णांक
frame_find_global_stlh_rec(frame_T *frp, पूर्णांक h)
{
    जर (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	जर (wp->w_height > 0 && wp->w_status_height > 0
		&& *wp->w_p_stlo == NUL)
	{
	    पूर्णांक win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    जर (win_free_height - p_wmh < h)
		h = win_free_height - p_wmh;
	}
    }
    अन्यथा
    {
	साठी (frp = frp->fr_child; frp != रिक्त; frp = frp->fr_next)
	    h = frame_find_global_stlh_rec(frp, h);
    }
    परत h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    स्थिर शून्य
frame_change_statusline_height_rec(frame_T *frp, पूर्णांक global_stlh)
{
    जर (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	जर (wp->w_height > 0 && wp->w_status_height > 0)
	{
	    पूर्णांक win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    wp->w_status_height = statusline_height(wp);
	    जर (wp->w_p_stlo == रिक्त || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		जर (wp->w_status_height > global_stlh)
		    wp->w_status_height = global_stlh;
	    }
	    जर (wp->w_status_height > win_free_height - p_wmh)
		wp->w_status_height = win_free_height - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    पूर्णांक	     saved_topfill = wp->w_topfill;
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
    अन्यथा
    {
	साठी (frp = frp->fr_child; frp != रिक्त; frp = frp->fr_next)
	    frame_change_statusline_height_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    पूर्णांक
frame_change_statusline_height(शून्य)
{
    tabpage_T	*tp;
    पूर्णांक		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = frame_find_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	frame_change_statusline_height_rec(tp->tp_topframe, global_stlh);
    comp_col();
    redraw_all_later(UPD_SOME_VALID);
    परत global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    शून्य
set_stlo_mh(पूर्णांक mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    पूर्णांक
statuslineopt_changed(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    पूर्णांक		l_stlo_fh = FALSE;
    पूर्णांक		l_stlo_mh = 1;

    जर (stlopt != रिक्त)
	p = stlopt;
    अन्यथा
	p = wp->w_p_stlo;

    पर्यंत (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	जर (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    जर (l_stlo_mh < 1)
		परत FAIL;
	}
	अन्यथा जर (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	जर (*p != ',' && *p != NUL)
	    परत FAIL;
	जर (*p == ',')
	    ++p;
    }

    जर (wp == रिक्त)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    अन्यथा
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    परत OK;
}
#endif

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    पूर्णांक
statusline_height(win_T *wp UNUSED)
{
    पूर्णांक stl_height = 1;
#जर defined(FEAT_STL_OPT)
    bool fixed;
    पूर्णांक l_stl_rendered_height;

    जर (wp != रिक्त && *wp->w_p_stlo != NUL)
    {
	stl_height = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_height = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }
    अन्यथा
    {
	stl_height = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_height = (wp != रिक्त && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    जर (!fixed)
    {
	पूर्णांक has_custom_stl = (*p_stl != NUL)
	    || (wp != रिक्त && *wp->w_p_stl != NUL);
	जर (has_custom_stl)
	    stl_height = MAX(1, MIN(l_stl_rendered_height, stl_height));
	अन्यथा
	    stl_height = 1;
    }
#endif

    परत stl_height;
}

/*
 * Return the number of lines used by the tab page line.
 */
    पूर्णांक
tabline_height(शून्य)
{
#ifdef FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    जर (gui_use_tabline())
	परत 0;
#endif
    निवड (p_stal)
    {
	स्थिती 0: परत 0;
	स्थिती 1: परत (first_tabpage->tp_next == रिक्त) ? 0 : 1;
    }
    परत 1;
}

/*
 * Return the height of the last window's statusline.
 */
    पूर्णांक
last_stl_height(
    पूर्णांक		morewin)	// pretend there are two or more windows
{
    परत (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_WINDOW)))
		? STATUS_HEIGHT : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    पूर्णांक
min_rows(शून्य)
{
    जर (firstwin == रिक्त)	// not initialized yet
	परत MIN_ओळी;

    परत frame_minheight(curtab->tp_topframe, रिक्त) + tabline_height()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    पूर्णांक
min_rows_for_all_tabpages(शून्य)
{
    पूर्णांक		total;
    tabpage_T	*tp;
    पूर्णांक		n;

    जर (firstwin == रिक्त)	// not initialized yet
	परत MIN_ओळी;

    total = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = frame_minheight(tp->tp_topframe, रिक्त);
	जर (total < n)
	    total = n;
    }
    total += tabline_height();
    total += MIN_CMDHEIGHT;
    परत total;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    पूर्णांक
only_one_window(शून्य)
{
    पूर्णांक		मोजणी = 0;
    win_T	*wp;

#जर defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    जर (popup_is_popup(curwin))
	परत FALSE;
#endif

    // If there is another tab page there always is another window.
    जर (first_tabpage->tp_next != रिक्त)
	परत FALSE;

    FOR_ALL_WINDOWS(wp)
	जर (wp->w_buffer != रिक्त
		&& (!((bt_help(wp->w_buffer) && !bt_help(curbuf))
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++मोजणी;
    परत (मोजणी <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    स्थिर शून्य
check_lnums_both(पूर्णांक do_curwin, पूर्णांक nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	जर ((do_curwin || wp != curwin) && wp->w_buffer == curbuf)
	{
	    पूर्णांक need_adjust;

	    जर (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_ओळ_मोजणी;
	    जर (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
	    जर (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_ओळ_मोजणी;
	    जर (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_ओळ_मोजणी;
	    जर (need_adjust || !nested)
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
check_lnums(पूर्णांक do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    शून्य
check_lnums_nested(पूर्णांक do_curwin)
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
	जर (wp->w_buffer == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    जर (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    जर (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    जर (wp->w_save_cursor.w_topline_save > wp->w_buffer->b_ml.ml_ओळ_मोजणी)
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
    पूर्णांक
make_snapshot(पूर्णांक idx)
{
    clear_snapshot(curtab, idx);
    जर (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	परत FAIL;
    }
    परत OK;
}

    स्थिर पूर्णांक
make_snapshot_rec(frame_T *fr, frame_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(frame_T);
    जर (*frp == रिक्त)
	परत FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_width = fr->fr_width;
    (*frp)->fr_height = fr->fr_height;
    जर (fr->fr_next != रिक्त)
    {
	जर (make_snapshot_rec(fr->fr_next, &((*frp)->fr_next)) == FAIL)
	    परत FAIL;
    }
    जर (fr->fr_child != रिक्त)
    {
	जर (make_snapshot_rec(fr->fr_child, &((*frp)->fr_child)) == FAIL)
	    परत FAIL;
    }
    जर (fr->fr_layout == FR_LEAF && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    परत OK;
}

/*
 * Remove any existing snapshot.
 */
    स्थिर शून्य
clear_snapshot(tabpage_T *tp, पूर्णांक idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = रिक्त;
}

    स्थिर शून्य
clear_snapshot_rec(frame_T *fr)
{
    जर (fr == रिक्त)
	परत;
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

    जर (ft->fr_next != रिक्त)
    {
	जर ((wp = get_snapshot_curwin_rec(ft->fr_next)) != रिक्त)
	    परत wp;
    }
    जर (ft->fr_child != रिक्त)
    {
	जर ((wp = get_snapshot_curwin_rec(ft->fr_child)) != रिक्त)
	    परत wp;
    }

    परत ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    स्थिर win_T *
get_snapshot_curwin(पूर्णांक idx)
{
    जर (curtab->tp_snapshot[idx] == रिक्त)
	परत रिक्त;

    परत get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    शून्य
restore_snapshot(
    पूर्णांक		idx,
    पूर्णांक		close_curwin)	    // closing current window
{
    win_T	*wp;

    जर (curtab->tp_snapshot[idx] != रिक्त
	    && curtab->tp_snapshot[idx]->fr_width == topframe->fr_width
	    && curtab->tp_snapshot[idx]->fr_height == topframe->fr_height
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	जर (wp != रिक्त && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    स्थिर पूर्णांक
check_snapshot_rec(frame_T *sn, frame_T *fr)
{
    जर (sn->fr_layout != fr->fr_layout
	    || (sn->fr_next == रिक्त) != (fr->fr_next == रिक्त)
	    || (sn->fr_child == रिक्त) != (fr->fr_child == रिक्त)
	    || (sn->fr_next != रिक्त
		&& check_snapshot_rec(sn->fr_next, fr->fr_next) == FAIL)
	    || (sn->fr_child != रिक्त
		&& check_snapshot_rec(sn->fr_child, fr->fr_child) == FAIL)
	    || (sn->fr_win != रिक्त && !win_valid(sn->fr_win)))
	परत FAIL;
    परत OK;
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
    जर (fr->fr_layout == FR_LEAF)
    {
	frame_new_height(fr, fr->fr_height, FALSE, FALSE, FALSE);
	frame_new_width(fr, fr->fr_width, FALSE, FALSE);
	wp = sn->fr_win;
    }
    जर (sn->fr_next != रिक्त)
    {
	wp2 = restore_snapshot_rec(sn->fr_next, fr->fr_next);
	जर (wp2 != रिक्त)
	    wp = wp2;
    }
    जर (sn->fr_child != रिक्त)
    {
	wp2 = restore_snapshot_rec(sn->fr_child, fr->fr_child);
	जर (wp2 != रिक्त)
	    wp = wp2;
    }
    परत wp;
}

#जर defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    पूर्णांक
win_hasvertsplit(शून्य)
{
    frame_T	*fr;

    जर (topframe->fr_layout == FR_ROW)
	परत TRUE;

    जर (topframe->fr_layout == FR_COL)
	FOR_ALL_FRAMES(fr, topframe->fr_child)
	    जर (fr->fr_layout == FR_ROW)
		परत TRUE;

    परत FALSE;
}
#endif

#जर defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    पूर्णांक
get_win_संख्या(win_T *wp, win_T *first_win)
{
    पूर्णांक		i = 1;
    win_T	*w;

    साठी (w = first_win; w != रिक्त && w != wp; w = W_NEXT(w))
	++i;

    जर (w == रिक्त)
	परत 0;
    अन्यथा
	परत i;
}

    पूर्णांक
get_tab_संख्या(tabpage_T *tp)
{
    पूर्णांक		i = 1;
    tabpage_T	*t;

    साठी (t = first_tabpage; t != रिक्त && t != tp; t = t->tp_next)
	++i;

    जर (t == रिक्त)
	परत 0;
    अन्यथा
	परत i;
}
#endif

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    स्थिर पूर्णांक
frame_check_height(frame_T *topfrp, पूर्णांक height)
{
    frame_T *frp;

    जर (topfrp->fr_height != height)
	परत FALSE;

    जर (topfrp->fr_layout == FR_ROW)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    जर (frp->fr_height != height)
		परत FALSE;

    परत TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    स्थिर पूर्णांक
frame_check_width(frame_T *topfrp, पूर्णांक width)
{
    frame_T *frp;

    जर (topfrp->fr_width != width)
	परत FALSE;

    जर (topfrp->fr_layout == FR_COL)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    जर (frp->fr_width != width)
		परत FALSE;

    परत TRUE;
}

#जर defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    स्थिर पूर्णांक
int_cmp(स्थिरांक शून्य *pa, स्थिरांक शून्य *pb)
{
    स्थिरांक पूर्णांक a = *(स्थिरांक पूर्णांक *)pa;
    स्थिरांक पूर्णांक b = *(स्थिरांक पूर्णांक *)pb;
    जर (a > b)
	परत 1;
    जर (a < b)
	परत -1;
    परत 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    अक्षर *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = empty_option;
    पूर्णांक		tw;
    पूर्णांक		col;
    पूर्णांक		मोजणी = 0;
    पूर्णांक		color_cols[256];
    पूर्णांक		i;
    पूर्णांक		j = 0;

    जर (wp != रिक्त && wp->w_buffer == रिक्त)
	परत रिक्त;  // buffer was closed

    जर (cc != रिक्त)
	s = cc;
    अन्यथा जर (wp != रिक्त)
	s = wp->w_p_cc;

    जर (wp != रिक्त)
	tw = wp->w_buffer->b_p_tw;
    अन्यथा
	// buffer-local value not set, assume zero
	tw = 0;

    पर्यंत (*s != NUL && मोजणी < 255)
    {
	जर (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    col = (*s == '-') ? -1 : 1;
	    ++s;
	    जर (!VIM_ISDIGIT(*s))
		परत e_invalid_argument;
	    col = col * getdigits(&s);
	    जर (tw == 0)
		जा skip;  // 'textwidth' not set, skip this item
	    col += tw;
	    जर (col < 0)
		जा skip;
	}
	अन्यथा जर (VIM_ISDIGIT(*s))
	    col = getdigits(&s);
	अन्यथा
	    परत e_invalid_argument;
	color_cols[मोजणी++] = col - 1;  // 1-based to 0-based
skip:
	जर (*s == NUL)
	    थांबा;
	जर (*s != ',')
	    परत e_invalid_argument;
	जर (*++s == NUL)
	    परत e_invalid_argument;  // illegal trailing comma as in "set cc=80,"
    }

    जर (wp == रिक्त)
	परत रिक्त;  // only parse "cc"

    vim_free(wp->w_p_cc_cols);
    जर (मोजणी == 0)
	wp->w_p_cc_cols = रिक्त;
    अन्यथा
    {
	wp->w_p_cc_cols = ALLOC_MULT(पूर्णांक, मोजणी + 1);
	जर (wp->w_p_cc_cols != रिक्त)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    क्विक(color_cols, मोजणी, आकार(पूर्णांक), int_cmp);

	    साठी (i = 0; i < मोजणी; ++i)
		// skip duplicates
		जर (j == 0 || wp->w_p_cc_cols[j - 1] != color_cols[i])
		    wp->w_p_cc_cols[j++] = color_cols[i];
	    wp->w_p_cc_cols[j] = -1;  // end marker
	}
    }

    परत रिक्त;  // no error
}
#endif

    पूर्णांक
get_last_winid(शून्य)
{
    परत last_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   पूर्णांक
win_locked(win_T *wp)
{
    परत wp->w_locked;
}
