/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#અન્તર્ભાવ "vim.h"

સ્થિર શૂન્ય cmd_with_ગણતરી(અક્ષર *cmd, char_u *bufp, size_t bufsize, long Prenum);
સ્થિર શૂન્ય win_init(win_T *newp, win_T *oldp, પૂર્ણાંક flags);
સ્થિર શૂન્ય win_init_some(win_T *newp, win_T *oldp);
સ્થિર શૂન્ય frame_comp_pos(frame_T *topfrp, પૂર્ણાંક *row, પૂર્ણાંક *col);
સ્થિર શૂન્ય frame_setheight(frame_T *curfrp, પૂર્ણાંક height);
સ્થિર શૂન્ય frame_setwidth(frame_T *curfrp, પૂર્ણાંક width);
સ્થિર શૂન્ય win_exchange(long);
સ્થિર શૂન્ય win_rotate(પૂર્ણાંક, પૂર્ણાંક);
સ્થિર શૂન્ય win_equal_rec(win_T *next_curwin, પૂર્ણાંક current, frame_T *topfr, પૂર્ણાંક dir, પૂર્ણાંક col, પૂર્ણાંક row, પૂર્ણાંક width, પૂર્ણાંક height);
સ્થિર શૂન્ય trigger_winnewpre(શૂન્ય);
સ્થિર શૂન્ય trigger_winclosed(win_T *win);
સ્થિર win_T *win_free_mem(win_T *win, પૂર્ણાંક *dirp, tabpage_T *tp);
સ્થિર frame_T *win_altframe(win_T *win, tabpage_T *tp);
સ્થિર tabpage_T *alt_tabpage(શૂન્ય);
સ્થિર win_T *frame2win(frame_T *frp);
સ્થિર પૂર્ણાંક frame_has_win(frame_T *frp, win_T *wp);
સ્થિર શૂન્ય win_fix_scroll(પૂર્ણાંક resize);
સ્થિર શૂન્ય win_fix_cursor(પૂર્ણાંક normal);
સ્થિર શૂન્ય frame_new_height(frame_T *topfrp, પૂર્ણાંક height, પૂર્ણાંક topfirst, પૂર્ણાંક wfh, પૂર્ણાંક set_ch);
સ્થિર પૂર્ણાંક frame_fixed_height(frame_T *frp);
સ્થિર પૂર્ણાંક frame_fixed_width(frame_T *frp);
સ્થિર શૂન્ય frame_add_statusline(frame_T *frp);
સ્થિર શૂન્ય frame_new_width(frame_T *topfrp, પૂર્ણાંક width, પૂર્ણાંક leftfirst, પૂર્ણાંક wfw);
સ્થિર શૂન્ય frame_set_vsep(frame_T *frp, પૂર્ણાંક add);
સ્થિર પૂર્ણાંક frame_minwidth(frame_T *topfrp, win_T *next_curwin);
સ્થિર શૂન્ય frame_fix_width(win_T *wp);
સ્થિર પૂર્ણાંક win_alloc_firstwin(win_T *oldwin);
સ્થિર શૂન્ય new_frame(win_T *wp);
સ્થિર tabpage_T *alloc_tabpage(શૂન્ય);
સ્થિર પૂર્ણાંક leave_tabpage(buf_T *new_curbuf, પૂર્ણાંક trigger_leave_autocmds);
સ્થિર શૂન્ય enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, પૂર્ણાંક trigger_enter_autocmds, પૂર્ણાંક trigger_leave_autocmds);
સ્થિર શૂન્ય frame_fix_height(win_T *wp);
સ્થિર પૂર્ણાંક frame_minheight(frame_T *topfrp, win_T *next_curwin);
સ્થિર પૂર્ણાંક frame_wincount_in_height(frame_T *topfrp);
સ્થિર પૂર્ણાંક may_open_tabpage(શૂન્ય);
સ્થિર પૂર્ણાંક win_enter_ext(win_T *wp, પૂર્ણાંક flags);
સ્થિર શૂન્ય win_free(win_T *wp, tabpage_T *tp);
સ્થિર શૂન્ય win_append(win_T *after, win_T *wp);
સ્થિર શૂન્ય frame_append(frame_T *after, frame_T *frp);
સ્થિર શૂન્ય frame_ઉમેરો(frame_T *before, frame_T *frp);
સ્થિર શૂન્ય frame_remove(frame_T *frp);
સ્થિર શૂન્ય win_goto_ver(પૂર્ણાંક up, long ગણતરી);
સ્થિર શૂન્ય win_goto_hor(પૂર્ણાંક ડાબું, long ગણતરી);
સ્થિર શૂન્ય frame_add_height(frame_T *frp, પૂર્ણાંક n);
સ્થિર શૂન્ય last_status_rec(frame_T *fr, પૂર્ણાંક statusline);
#જો defined(FEAT_STL_OPT)
સ્થિર પૂર્ણાંક frame_find_global_stlh_rec(frame_T *frp, પૂર્ણાંક h);
સ્થિર શૂન્ય frame_change_statusline_height_rec(frame_T *frp, પૂર્ણાંક global_stlh);
#endif
સ્થિર શૂન્ય frame_flatten(frame_T *frp);
સ્થિર શૂન્ય winframe_restore(win_T *wp, પૂર્ણાંક dir, frame_T *unflat_altfr);

સ્થિર પૂર્ણાંક make_snapshot_rec(frame_T *fr, frame_T **frp);
સ્થિર શૂન્ય clear_snapshot(tabpage_T *tp, પૂર્ણાંક idx);
સ્થિર શૂન્ય clear_snapshot_rec(frame_T *fr);
સ્થિર પૂર્ણાંક check_snapshot_rec(frame_T *sn, frame_T *fr);
સ્થિર win_T *restore_snapshot_rec(frame_T *sn, frame_T *fr);
સ્થિર win_T *get_snapshot_curwin(પૂર્ણાંક idx);

સ્થિર પૂર્ણાંક frame_check_height(frame_T *topfrp, પૂર્ણાંક height);
સ્થિર પૂર્ણાંક frame_check_width(frame_T *topfrp, પૂર્ણાંક width);

સ્થિર win_T *win_alloc(win_T *after, પૂર્ણાંક hidden);

#define NOWIN		((win_T *)-1)	// non-existing window

#define ROWS_AVAIL (Rows - p_ch - tabline_height())

// flags for win_enter_ext()
#define WEE_UNDO_SYNC			0x01
#define WEE_CURWIN_INVALID		0x02
#define WEE_TRIGGER_NEW_AUTOCMDS	0x04
#define WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#define WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#define WEE_ALLOW_PARSE_MESSAGES	0x20

સ્થિર અક્ષર *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
સ્થિર પૂર્ણાંક split_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
સ્થિર પૂર્ણાંક close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
સ્થિર પૂર્ણાંક frame_locked = 0;

#જો defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
સ્થિર પૂર્ણાંક stl_rendered_height = 1;
સ્થિર પૂર્ણાંક stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
સ્થિર પૂર્ણાંક stlo_mh = STATUS_HEIGHT; // "maxheight" value cache of global 'stlo'
#endif

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    શૂન્ય
window_layout_lock(શૂન્ય)
{
    ++split_disallowed;
    ++close_disallowed;
}

    શૂન્ય
window_layout_unlock(શૂન્ય)
{
    --split_disallowed;
    --close_disallowed;
}

    પૂર્ણાંક
frames_locked(શૂન્ય)
{
    પાછા frame_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    પૂર્ણાંક
window_layout_locked(ગણના CMD_index cmd)
{
    જો (split_disallowed > 0 || close_disallowed > 0)
    {
	જો (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_split_window_when_closing_buffer));
	નહીંતર
	    emsg(_(e_not_allowed_to_change_window_layout_in_this_autocmd));
	પાછા TRUE;
    }
    પાછા FALSE;
}

// #define WIN_DEBUG
#ifdef WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    સ્થિર શૂન્ય
log_frame_layout(frame_T *frame)
{
    ch_log(ખાલી, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    frame->fr_layout == FR_LEAF ? "LEAF"
				  : frame->fr_layout == FR_ROW ? "ROW" : "COL",
	    frame->fr_width,
	    frame->fr_height,
	    frame->fr_win == ખાલી ? -1 : frame->fr_win->w_width,
	    frame->fr_win == ખાલી ? -1 : frame->fr_win->w_height,
	    frame->fr_win == ખાલી ? -1 : frame->fr_win->w_id);
    જો (frame->fr_child != ખાલી)
    {
	ch_log(ખાલી, "children");
	log_frame_layout(frame->fr_child);
	જો (frame->fr_next != ખાલી)
	    ch_log(ખાલી, "END of children");
    }
    જો (frame->fr_next != ખાલી)
	log_frame_layout(frame->fr_next);
}
#endif

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    પૂર્ણાંક
check_can_set_curbuf_disabled(શૂન્ય)
{
    જો (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    પૂર્ણાંક
check_can_set_curbuf_forceit(પૂર્ણાંક forceit)
{
    જો (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(શૂન્ય)
{
    // In cmdwin, the alternative buffer should be used.
    પાછા is_in_cmdwin() && prevwin != ખાલી ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = ખાલી;

    જો (buf == ખાલી)
	પાછા wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    જો (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    જો (wp == ખાલી && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    પાછા wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    શૂન્ય
do_window(
    પૂર્ણાંક		nchar,
    long	Prenum,
    પૂર્ણાંક		xchar)	    // extra char from ":wincmd gx" or NUL
{
    long	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ifdef FEAT_FIND_ID
    પૂર્ણાંક		type = FIND_DEFINE;
    પૂર્ણાંક		len;
#endif
    char_u	cbuf[40];

    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;

#define CHECK_CMDWIN \
    કરો { \
	જો (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_in_cmdline_window)); \
	    પાછા; \
	} \
    } જ્યારે (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    પસંદગી (nchar)
    {
// split current window in two parts, horizontally
    સ્થિતિ 'S':
    સ્થિતિ Ctrl_S:
    સ્થિતિ 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		જો (bt_quickfix(curbuf))
		    જાઓ newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(શૂન્ય)win_split((પૂર્ણાંક)Prenum, 0);
		તોડો;

// split current window in two parts, vertically
    સ્થિતિ Ctrl_V:
    સ્થિતિ 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		જો (bt_quickfix(curbuf))
		    જાઓ newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(શૂન્ય)win_split((પૂર્ણાંક)Prenum, WSP_VERT);
		તોડો;

// split current window and edit alternate file
    સ્થિતિ Ctrl_HAT:
    સ્થિતિ '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		જો (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == ખાલી)
		{
		    જો (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    નહીંતર
			semsg(_(e_buffer_nr_not_found), Prenum);
		    તોડો;
		}

		જો (!curbuf_locked() && win_split(0, 0) == OK)
		    (શૂન્ય)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		તોડો;

// open new window
    સ્થિતિ Ctrl_N:
    સ્થિતિ 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		જો (Prenum)
		    // window height
		    vim_snprintf((અક્ષર *)cbuf, કદ(cbuf) - 5, "%ld", Prenum);
		નહીંતર
		    cbuf[0] = NUL;
#જો defined(FEAT_QUICKFIX)
		જો (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#endif
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		તોડો;

// quit current window
    સ્થિતિ Ctrl_Q:
    સ્થિતિ 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ગણતરી("quit", cbuf, કદ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		તોડો;

// close current window
    સ્થિતિ Ctrl_C:
    સ્થિતિ 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ગણતરી("close", cbuf, કદ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		તોડો;

#જો defined(FEAT_QUICKFIX)
// close preview window
    સ્થિતિ Ctrl_Z:
    સ્થિતિ 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		તોડો;

// cursor to preview window
    સ્થિતિ 'P':
		FOR_ALL_WINDOWS(wp)
		    જો (wp->w_p_pvw)
			તોડો;
		જો (wp == ખાલી)
		    emsg(_(e_there_is_no_preview_window));
		નહીંતર
		    win_goto(wp);
		તોડો;
#endif

// close all but current window
    સ્થિતિ Ctrl_O:
    સ્થિતિ 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ગણતરી("only", cbuf, કદ(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		તોડો;

// cursor to next window with wrap around
    સ્થિતિ Ctrl_W:
    સ્થિતિ 'w':
// cursor to previous window with wrap around
    સ્થિતિ 'W':
		CHECK_CMDWIN;
		જો (ONE_WINDOW && Prenum != 1)	// just one window
		    beep_flush();
		નહીંતર
		{
		    જો (Prenum)			// go to specified window
		    {
			માટે (wp = firstwin; --Prenum > 0; )
			{
			    જો (wp->w_next == ખાલી)
				તોડો;
			    નહીંતર
				wp = wp->w_next;
			}
		    }
		    નહીંતર
		    {
			જો (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    જો (wp == ખાલી)
				wp = lastwin;	    // wrap around
			}
			નહીંતર			    // go to next window
			{
			    wp = curwin->w_next;
			    જો (wp == ખાલી)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		તોડો;

// cursor to window below
    સ્થિતિ 'j':
    સ્થિતિ K_DOWN:
    સ્થિતિ Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		તોડો;

// cursor to window above
    સ્થિતિ 'k':
    સ્થિતિ K_UP:
    સ્થિતિ Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		તોડો;

// cursor to left window
    સ્થિતિ 'h':
    સ્થિતિ K_ડાબું:
    સ્થિતિ Ctrl_H:
    સ્થિતિ K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		તોડો;

// cursor to right window
    સ્થિતિ 'l':
    સ્થિતિ K_જમણું:
    સ્થિતિ Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		તોડો;

// move window to new tab page
    સ્થિતિ 'T':
		CHECK_CMDWIN;
		જો (one_window())
		    સંદેશ(_(m_onlyone));
		નહીંતર
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    જો (win_new_tabpage((પૂર્ણાંક)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			જો (curwin == wp)
			    win_close(curwin, FALSE);
			જો (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		તોડો;

// cursor to top-left window
    સ્થિતિ 't':
    સ્થિતિ Ctrl_T:
		win_goto(firstwin);
		તોડો;

// cursor to bottom-right window
    સ્થિતિ 'b':
    સ્થિતિ Ctrl_B:
		win_goto(lastwin);
		તોડો;

// cursor to last accessed (previous) window
    સ્થિતિ 'p':
    સ્થિતિ Ctrl_P:
		જો (!win_valid(prevwin))
		    beep_flush();
		નહીંતર
		    win_goto(prevwin);
		તોડો;

// exchange current and next window
    સ્થિતિ 'x':
    સ્થિતિ Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		તોડો;

// rotate windows downwards
    સ્થિતિ Ctrl_R:
    સ્થિતિ 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (પૂર્ણાંક)Prenum1);    // downwards
		તોડો;

// rotate windows upwards
    સ્થિતિ 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (પૂર્ણાંક)Prenum1);	    // upwards
		તોડો;

// move window to the very top/bottom/left/right
    સ્થિતિ 'K':
    સ્થિતિ 'J':
    સ્થિતિ 'H':
    સ્થિતિ 'L':
		CHECK_CMDWIN;
		જો (ONE_WINDOW)
		    beep_flush();
		નહીંતર
		{
		    પૂર્ણાંક dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_ટોચ : WSP_BOT);

		    (શૂન્ય)win_splitmove(curwin, (પૂર્ણાંક)Prenum, dir);
		}
		તોડો;

// make all windows the same width and/or height
    સ્થિતિ '=':
		{
		    પૂર્ણાંક mod = cmdmod.cmod_split & (WSP_VERT | WSP_HOR);
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_equal(ખાલી, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		તોડો;

// increase current window height
    સ્થિતિ '+':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height + (પૂર્ણાંક)Prenum1);
		તોડો;

// decrease current window height
    સ્થિતિ '-':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height - (પૂર્ણાંક)Prenum1);
		તોડો;

// set current window height
    સ્થિતિ Ctrl__:
    સ્થિતિ '_':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(Prenum ? (પૂર્ણાંક)Prenum : 9999);
		તોડો;

// increase current window width
    સ્થિતિ '>':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width + (પૂર્ણાંક)Prenum1);
		તોડો;

// decrease current window width
    સ્થિતિ '<':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width - (પૂર્ણાંક)Prenum1);
		તોડો;

// set current window width
    સ્થિતિ '|':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(Prenum != 0 ? (પૂર્ણાંક)Prenum : 9999);
		તોડો;

// jump to tag and split window if tag exists (in preview window)
#જો defined(FEAT_QUICKFIX)
    સ્થિતિ '}':
		CHECK_CMDWIN;
		જો (Prenum)
		    g_do_tagpreview = Prenum;
		નહીંતર
		    g_do_tagpreview = p_pvh;
#endif
		// FALLTHROUGH
    સ્થિતિ ']':
    સ્થિતિ Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		જો (Prenum)
		    postponed_split = Prenum;
		નહીંતર
		    postponed_split = -1;
#ifdef FEAT_QUICKFIX
		જો (nchar != '}')
		    g_do_tagpreview = 0;
#endif

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_split = 0;
		તોડો;

// edit file name under cursor in a new window
    સ્થિતિ 'f':
    સ્થિતિ 'F':
    સ્થિતિ Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		જો (check_text_or_curbuf_locked(ખાલી))
		    તોડો;

		ptr = grab_file_name(Prenum1, &lnum);
		જો (ptr != ખાલી)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = ખાલી;
		    જો ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    જો (wp == ખાલી && win_split(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			જો (do_ecmd(0, ptr, ખાલી, ખાલી, ECMD_LASTL,
						   ECMD_HIDE, ખાલી) == FAIL)
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
			નહીંતર
			    wp = curwin;
		    }

		    જો (wp != ખાલી && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		તોડો;

#ifdef FEAT_FIND_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    સ્થિતિ 'i':			    // Go to any match
    સ્થિતિ Ctrl_I:
		type = FIND_ANY;
		// FALLTHROUGH
    સ્થિતિ 'd':			    // Go to definition, using 'define'
    સ્થિતિ Ctrl_D:
		CHECK_CMDWIN;
		જો ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
		    તોડો;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		જો (ptr == ખાલી)
		    તોડો;

		find_pattern_in_path(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_SPLIT, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_set_curswant = true;
		તોડો;
#endif

// Quickfix window only: view the result under the cursor in a new split.
#જો defined(FEAT_QUICKFIX)
    સ્થિતિ K_KENTER:
    સ્થિતિ CAR:
		જો (bt_quickfix(curbuf))
		    qf_view_result(TRUE);
		તોડો;
#endif

// CTRL-W g  extended commands
    સ્થિતિ 'g':
    સ્થિતિ Ctrl_G:
		CHECK_CMDWIN;
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#endif
		++no_mapping;
		++allow_ચાવીઓ;   // no mapping for xchar, but allow key codes
		જો (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_ચાવીઓ;
		(શૂન્ય)add_to_showcmd(xchar);

		પસંદગી (xchar)
		{
#જો defined(FEAT_QUICKFIX)
		    સ્થિતિ '}':
			xchar = Ctrl_RSB;
			જો (Prenum)
			    g_do_tagpreview = Prenum;
			નહીંતર
			    g_do_tagpreview = p_pvh;
#endif
			// FALLTHROUGH
		    સ્થિતિ ']':
		    સ્થિતિ Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			જો (Prenum)
			    postponed_split = Prenum;
			નહીંતર
			    postponed_split = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_split = 0;
			તોડો;

		    સ્થિતિ 'f':	    // CTRL-W gf: "gf" in a new tab page
		    સ્થિતિ 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_index(curtab) + 1;
			nchar = xchar;
			જાઓ wingotofile;

		    સ્થિતિ 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((પૂર્ણાંક)Prenum);
			તોડો;

		    સ્થિતિ 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(પૂર્ણાંક)Prenum1);
			તોડો;

		    સ્થિતિ TAB:	    // CTRL-W g<Tab>: go to last used tab page
			જો (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			તોડો;

		    મૂળભૂત:
			beep_flush();
			તોડો;
		}
		તોડો;

    મૂળભૂત:	beep_flush();
		તોડો;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    શૂન્ય
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    પસંદગી (*arg)
    {
    સ્થિતિ 'S':
    સ્થિતિ Ctrl_S:
    સ્થિતિ 's':
    સ્થિતિ Ctrl_N:
    સ્થિતિ 'n':
    સ્થિતિ 'j':
    સ્થિતિ Ctrl_J:
    સ્થિતિ 'k':
    સ્થિતિ Ctrl_K:
    સ્થિતિ 'T':
    સ્થિતિ Ctrl_R:
    સ્થિતિ 'r':
    સ્થિતિ 'R':
    સ્થિતિ 'K':
    સ્થિતિ 'J':
    સ્થિતિ '+':
    સ્થિતિ '-':
    સ્થિતિ Ctrl__:
    સ્થિતિ '_':
    સ્થિતિ '|':
    સ્થિતિ ']':
    સ્થિતિ Ctrl_RSB:
    સ્થિતિ 'g':
    સ્થિતિ Ctrl_G:
    સ્થિતિ Ctrl_V:
    સ્થિતિ 'v':
    સ્થિતિ 'h':
    સ્થિતિ Ctrl_H:
    સ્થિતિ 'l':
    સ્થિતિ Ctrl_L:
    સ્થિતિ 'H':
    સ્થિતિ 'L':
    સ્થિતિ '>':
    સ્થિતિ '<':
#જો defined(FEAT_QUICKFIX)
    સ્થિતિ '}':
#endif
    સ્થિતિ 'f':
    સ્થિતિ 'F':
    સ્થિતિ Ctrl_F:
#ifdef FEAT_FIND_ID
    સ્થિતિ 'i':
    સ્થિતિ Ctrl_I:
    સ્થિતિ 'd':
    સ્થિતિ Ctrl_D:
#endif
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		તોડો;

    સ્થિતિ Ctrl_HAT:
    સ્થિતિ '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		તોડો;

    સ્થિતિ Ctrl_Q:
    સ્થિતિ 'q':
    સ્થિતિ Ctrl_C:
    સ્થિતિ 'c':
    સ્થિતિ Ctrl_O:
    સ્થિતિ 'o':
    સ્થિતિ Ctrl_W:
    સ્થિતિ 'w':
    સ્થિતિ 'W':
    સ્થિતિ 'x':
    સ્થિતિ Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		તોડો;

#જો defined(FEAT_QUICKFIX)
    સ્થિતિ Ctrl_Z:
    સ્થિતિ 'z':
    સ્થિતિ 'P':
#endif
    સ્થિતિ 't':
    સ્થિતિ Ctrl_T:
    સ્થિતિ 'b':
    સ્થિતિ Ctrl_B:
    સ્થિતિ 'p':
    સ્થિતિ Ctrl_P:
    સ્થિતિ '=':
    સ્થિતિ CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		તોડો;
    }
}

    સ્થિર શૂન્ય
cmd_with_ગણતરી(
    અક્ષર	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    long	Prenum)
{
    જો (Prenum > 0)
	vim_snprintf((અક્ષર *)bufp, bufsize, "%s %ld", cmd, Prenum);
    નહીંતર
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    પૂર્ણાંક
check_split_disallowed(win_T *wp)
{
    જો (split_disallowed > 0)
    {
	emsg(_(e_cant_split_window_while_closing_another));
	પાછા FAIL;
    }
    જો (wp->w_buffer->b_locked_split)
    {
	emsg(_(e_cannot_split_window_when_closing_buffer));
	પાછા FAIL;
    }
    પાછા OK;
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
    પૂર્ણાંક
win_split(પૂર્ણાંક size, પૂર્ણાંક flags)
{
    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા FAIL;

    જો (check_split_disallowed(curwin) == FAIL)
	પાછા FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    જો (may_open_tabpage() == OK)
	પાછા OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_split;
    જો ((flags & WSP_ટોચ) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_split_topleft_and_botright_at_the_same_time));
	પાછા FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    જો (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    નહીંતર
	clear_snapshot(curtab, SNAP_HELP_IDX);

    જો (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    નહીંતર
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    પાછા win_split_ins(size, flags, ખાલી, 0, ખાલી);
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
    પૂર્ણાંક
win_split_ins(
    પૂર્ણાંક		size,
    પૂર્ણાંક		flags,
    win_T	*new_wp,
    પૂર્ણાંક		dir,
    frame_T	*to_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    પૂર્ણાંક		new_size = size;
    પૂર્ણાંક		i;
    પૂર્ણાંક		need_status = 0;
    પૂર્ણાંક		do_equal = FALSE;
    પૂર્ણાંક		needed;
    પૂર્ણાંક		available;
    પૂર્ણાંક		oldwin_height = 0;
    પૂર્ણાંક		layout;
    frame_T	*frp, *curfrp, *frp2, *prevfrp;
    પૂર્ણાંક		before;
    પૂર્ણાંક		minheight;
    પૂર્ણાંક		wmh1;
    પૂર્ણાંક		did_set_fraction = FALSE;
    પૂર્ણાંક		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    જો (new_wp == ખાલી)
	trigger_winnewpre();

    જો (flags & WSP_ટોચ)
	oldwin = firstwin;
    નહીંતર જો (flags & WSP_BOT)
	oldwin = lastwin;
    નહીંતર
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    જો (ONE_WINDOW && p_ls == 1 && oldwin->w_status_height == 0)
    {
	જો (!(flags & WSP_FORCE_ROOM) && VISIBLE_HEIGHT(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_room));
	    જાઓ theend;
	}
	need_status = statusline_height(oldwin);
    }

#ifdef FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    જો (gui.in_use)
	out_flush();
#endif

    જો (flags & WSP_VERT)
    {
	પૂર્ણાંક	wmw1;
	પૂર્ણાંક	minwidth;

	layout = FR_ROW;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	જો (flags & WSP_ROOM)
	    needed += p_wiw - wmw1;
	જો (flags & (WSP_BOT | WSP_ટોચ))
	{
	    minwidth = frame_minwidth(topframe, NOWIN);
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	નહીંતર જો (p_ea)
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    prevfrp = oldwin->w_frame;
	    માટે (frp = oldwin->w_frame->fr_parent; frp != ખાલી;
							frp = frp->fr_parent)
	    {
		જો (frp->fr_layout == FR_ROW)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			જો (frp2 != prevfrp)
			    minwidth += frame_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	નહીંતર
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    available = oldwin->w_frame->fr_width;
	    needed += minwidth;
	}
	જો (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    જાઓ theend;
	}
	જો (new_size == 0)
	    new_size = oldwin->w_width / 2;
	જો (new_size > available - minwidth - 1)
	    new_size = available - minwidth - 1;
	જો (new_size < wmw1)
	    new_size = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	જો (oldwin->w_width - new_size - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	જો (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_width + new_size + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	જો (!do_equal && p_ea && size == 0 && *p_ead != 'v'
	    && oldwin->w_frame->fr_parent != ખાલી)
	    do_equal = TRUE;
    }
    નહીંતર
    {
	layout = FR_COL;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_HEIGHT(curwin);
	needed = wmh1 + statusline_height(oldwin);
	જો (flags & WSP_ROOM)
	    needed += p_wh - wmh1;
	જો (flags & (WSP_BOT | WSP_ટોચ))
	{
	    minheight = frame_minheight(topframe, NOWIN) + need_status;
	    available = topframe->fr_height;
	    needed += minheight;
	}
	નહીંતર જો (p_ea)
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    prevfrp = oldwin->w_frame;
	    માટે (frp = oldwin->w_frame->fr_parent; frp != ખાલી;
							frp = frp->fr_parent)
	    {
		જો (frp->fr_layout == FR_COL)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			જો (frp2 != prevfrp)
			    minheight += frame_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_height;
	    needed += minheight;
	}
	નહીંતર
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    available = oldwin->w_frame->fr_height;
	    needed += minheight;
	}
	જો (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    જાઓ theend;
	}
	oldwin_height = oldwin->w_height;
	જો (need_status)
	{
	    oldwin->w_status_height = statusline_height(oldwin);
	    oldwin_height -= oldwin->w_status_height;
	}
	// Estimated stl height for the new window.
	પૂર્ણાંક new_win_stlh = need_status != 0 ? need_status
			: statusline_height(new_wp != ખાલી ? new_wp : curwin);
	જો (new_size == 0)
	    new_size = (oldwin_height - new_win_stlh + 1) / 2;
	જો (new_size > available - minheight - new_win_stlh)
	    new_size = available - minheight - new_win_stlh;
	જો (new_size < wmh1)
	    new_size = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	જો (oldwin_height - new_size - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	જો (oldwin->w_p_wfh)
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
	જો (!do_equal && p_ea && size == 0 && *p_ead != 'h'
	    && oldwin->w_frame->fr_parent != ખાલી)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    જો ((flags & WSP_ટોચ) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_BELOW)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	જો (new_wp == ખાલી)
	    wp = win_alloc(oldwin, FALSE);
	નહીંતર
	    win_append(oldwin, wp);
    }
    નહીંતર
    {
	જો (new_wp == ખાલી)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	નહીંતર
	    win_append(oldwin->w_prev, wp);
    }

    જો (new_wp == ખાલી)
    {
	જો (wp == ખાલી)
	    જાઓ theend;

	new_frame(wp);
	જો (wp->w_frame == ખાલી)
	{
	    win_free(wp, ખાલી);
	    જાઓ theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    જો (to_flatten != ખાલી)
	frame_flatten(to_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    જો (flags & (WSP_ટોચ | WSP_BOT))
    {
	જો ((topframe->fr_layout == FR_COL && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ROW && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_child;
	    જો (flags & WSP_BOT)
		જ્યારે (curfrp->fr_next != ખાલી)
		    curfrp = curfrp->fr_next;
	}
	નહીંતર
	    curfrp = topframe;
	before = (flags & WSP_ટોચ);
    }
    નહીંતર
    {
	curfrp = oldwin->w_frame;
	જો (flags & WSP_BELOW)
	    before = FALSE;
	નહીંતર જો (flags & WSP_ABOVE)
	    before = TRUE;
	નહીંતર જો (flags & WSP_VERT)
	    before = !p_spr;
	નહીંતર
	    before = !p_sb;
    }
    જો (curfrp->fr_parent == ખાલી || curfrp->fr_parent->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(frame_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_parent = curfrp;
	frp->fr_next = ખાલી;
	frp->fr_prev = ખાલી;
	curfrp->fr_child = frp;
	curfrp->fr_win = ખાલી;
	curfrp = frp;
	જો (frp->fr_win != ખાલી)
	    oldwin->w_frame = frp;
	નહીંતર
	    FOR_ALL_FRAMES(frp, frp->fr_child)
		frp->fr_parent = curfrp;
    }

    જો (new_wp == ખાલી)
	frp = wp->w_frame;
    નહીંતર
	frp = new_wp->w_frame;
    frp->fr_parent = curfrp->fr_parent;

    // Insert the new frame at the right place in the frame list.
    જો (before)
	frame_ઉમેરો(curfrp, frp);
    નહીંતર
	frame_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    જો (!did_set_fraction)
	set_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    જો (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	જો (need_status)
	{
	    win_new_height(oldwin, oldwin->w_height - need_status);
	    oldwin->w_status_height = need_status;
	}
	જો (flags & (WSP_ટોચ | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    પૂર્ણાંક stl_height = p_ls > 0 ? statusline_height(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_height();
	    win_new_height(wp, curfrp->fr_height - stl_height
							  - WINBAR_HEIGHT(wp));
	    wp->w_status_height = stl_height;
	}
	નહીંતર
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
	જો (before)
	    wp->w_vsep_width = 1;
	નહીંતર
	{
	    wp->w_vsep_width = oldwin->w_vsep_width;
	    oldwin->w_vsep_width = 1;
	}
	જો (flags & (WSP_ટોચ | WSP_BOT))
	{
	    જો (flags & WSP_BOT)
		frame_set_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    frame_new_width(curfrp, curfrp->fr_width
		     - (new_size + ((flags & WSP_ટોચ) != 0)), flags & WSP_ટોચ,
								       FALSE);
	}
	નહીંતર
	    win_new_width(oldwin, oldwin->w_width - (new_size + 1));
	જો (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_size + 1;
	}
	નહીંતર		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_width + 1;
	frame_fix_width(oldwin);
	frame_fix_width(wp);
    }
    નહીંતર
    {
	// width and column of new window is same as current window
	જો (flags & (WSP_ટોચ | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_width(wp, topframe->fr_width);
	    wp->w_vsep_width = 0;
	}
	નહીંતર
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_width(wp, oldwin->w_width);
	    wp->w_vsep_width = oldwin->w_vsep_width;
	}
	frp->fr_width = curfrp->fr_width;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_height(wp, new_size);
	પૂર્ણાંક old_status_height = oldwin->w_status_height;
	જો (flags & (WSP_ટોચ | WSP_BOT))
	{
	    પૂર્ણાંક new_fr_height = curfrp->fr_height - new_size
							  + WINBAR_HEIGHT(wp);

	    જો (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_height -= statusline_height(curfrp->fr_win);
	    જો (flags & WSP_BOT)
		frame_add_statusline(curfrp);
	    frame_new_height(curfrp, new_fr_height, flags & WSP_ટોચ, FALSE,
									FALSE);
	}
	નહીંતર
	    win_new_height(oldwin, oldwin_height - (new_size
			+ statusline_height(wp)));
	જો (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_status_height = statusline_height(wp);
	    oldwin->w_winrow += wp->w_height + statusline_height(wp);
	}
	નહીંતર		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_HEIGHT(oldwin)
						    + statusline_height(oldwin);
	    wp->w_status_height = old_status_height;
	    જો (!(flags & WSP_BOT))
		oldwin->w_status_height = statusline_height(oldwin);
	}
	frame_fix_height(wp);
	frame_fix_height(oldwin);
    }

    જો (flags & (WSP_ટોચ | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    status_redraw_all();

    જો (need_status)
    {
	સંદેશ_row = Rows - 1;
	સંદેશ_col = sc_col;
	સંદેશ_clr_eos_force();	// Old command/ruler may still be there
	comp_col();
	સંદેશ_row = Rows - 1;
	સંદેશ_col = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    જો (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    નહીંતર જો (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    જો (flags & WSP_VERT)
    {
	i = p_wiw;
	જો (size != 0)
	    p_wiw = size;

#ifdef FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	જો (gui.in_use)
	    gui_init_which_components(ખાલી);
#endif
    }
    નહીંતર
    {
	i = p_wh;
	જો (size != 0)
	    p_wh = size;
    }

    /*
     * make the new window the current window
     */
    (શૂન્ય)win_enter_ext(wp, (new_wp == ખાલી ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    જો (flags & WSP_VERT)
	p_wiw = i;
    નહીંતર
	p_wh = i;
    retval = OK;

theend:
    જો (RedrawingDisabled > 0)
	--RedrawingDisabled;
    પાછા retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    સ્થિર શૂન્ય
win_init(win_T *newp, win_T *oldp, પૂર્ણાંક flags UNUSED)
{
    પૂર્ણાંક		i;

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
    જો (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = ખાલી;
	newp->w_llist_ref = ખાલી;
    }
    નહીંતર
	copy_loclist_ઢગલો(oldp, newp);
#endif
    newp->w_localdir = (oldp->w_localdir == ખાલી)
				    ? ખાલી : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == ખાલી)
				    ? ખાલી : vim_strsave(oldp->w_prevdir);

    જો (*p_spk != 'c')
    {
	જો (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_height = oldp->w_height - WINBAR_HEIGHT(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_HEIGHT(oldp);
    }

    // copy tagstack and folds
    માટે (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*tag = &newp->w_tagstack[i];
	*tag = oldp->w_tagstack[i];
	જો (tag->tagname != ખાલી)
	    tag->tagname = vim_strsave(tag->tagname);
	જો (tag->user_data != ખાલી)
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
	bool pushed = ધકેલો_highlight_overrides(newp->w_hl, newp->w_hl_len);
	જો (newp->w_buffer->b_term != ખાલી)
	    term_init_default_colors(newp->w_buffer->b_term);
	જો (pushed)
	    કાઢો_highlight_overrides();
    }
#endif
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    સ્થિર શૂન્ય
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
    પૂર્ણાંક
win_valid_popup(win_T *win UNUSED)
{
#ifdef FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	જો (wp == win)
	    પાછા TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	જો (wp == win)
	    પાછા TRUE;
#endif
    પાછા FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    પૂર્ણાંક
win_valid(win_T *win)
{
    win_T	*wp;

    જો (win == ખાલી)
	પાછા FALSE;
    FOR_ALL_WINDOWS(wp)
	જો (wp == win)
	    પાછા TRUE;
    પાછા win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_find_by_id(પૂર્ણાંક id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	જો (wp->w_id == id)
	    પાછા wp;
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	જો (wp->w_id == id)
	    પાછા wp;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	જો (wp->w_id == id)
	    પાછા wp;
#endif
    પાછા ખાલી;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    પૂર્ણાંક
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    જો (win == ખાલી)
	પાછા FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    જો (wp == win)
		પાછા TRUE;
	}
#ifdef FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    જો (wp == win)
		પાછા TRUE;
#endif
    }
    પાછા win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    પૂર્ણાંક
win_ગણતરી(શૂન્ય)
{
    win_T	*wp;
    પૂર્ણાંક		ગણતરી = 0;

    FOR_ALL_WINDOWS(wp)
	++ગણતરી;
    પાછા ગણતરી;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    પૂર્ણાંક
make_windows(
    પૂર્ણાંક		ગણતરી,
    પૂર્ણાંક		vertical)  // split windows vertically if TRUE
{
    પૂર્ણાંક		maxcount;
    પૂર્ણાંક		todo;

    જો (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_width + curwin->w_vsep_width
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    નહીંતર
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_HEIGHT(curwin) + curwin->w_status_height
			- (p_wh - p_wmh)) / (p_wmh + statusline_height(curwin));
    }

    જો (maxcount < 2)
	maxcount = 2;
    જો (ગણતરી > maxcount)
	ગણતરી = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    જો (ગણતરી > 1)
	last_status(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    // todo is number of windows left to create
    માટે (todo = ગણતરી - 1; todo > 0; --todo)
	જો (vertical)
	{
	    જો (win_split(curwin->w_width - (curwin->w_width - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		તોડો;
	}
	નહીંતર
	{
	    જો (win_split(curwin->w_height - (curwin->w_height - todo
			    * statusline_height(curwin)) / (todo + 1)
			- statusline_height(curwin), WSP_ABOVE) == FAIL)
		તોડો;
	}

    unblock_autocmds();

    // return actual number of windows
    પાછા (ગણતરી - todo);
}

/*
 * Exchange current and next window
 */
    સ્થિર શૂન્ય
win_exchange(long Prenum)
{
    frame_T	*frp;
    frame_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    પૂર્ણાંક		temp;

    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;
    જો (ONE_WINDOW)	    // just one window
    {
	beep_flush();
	પાછા;
    }
    જો (text_or_buf_locked())
    {
	beep_flush();
	પાછા;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * find window to exchange with
     */
    જો (Prenum)
    {
	frp = curwin->w_frame->fr_parent->fr_child;
	જ્યારે (frp != ખાલી && --Prenum > 0)
	    frp = frp->fr_next;
    }
    નહીંતર જો (curwin->w_frame->fr_next != ખાલી)	// Swap with next
	frp = curwin->w_frame->fr_next;
    નહીંતર    // Swap last window in row/col with previous
	frp = curwin->w_frame->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    જો (frp == ખાલી || frp->fr_win == ખાલી || frp->fr_win == curwin)
	પાછા;
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
    જો (wp->w_prev != curwin)
    {
	win_remove(curwin, ખાલી);
	frame_remove(curwin->w_frame);
	win_append(wp->w_prev, curwin);
	frame_ઉમેરો(frp, curwin->w_frame);
    }
    જો (wp != wp2)
    {
	win_remove(wp, ખાલી);
	frame_remove(wp->w_frame);
	win_append(wp2, wp);
	જો (frp2 == ખાલી)
	    frame_ઉમેરો(wp->w_frame->fr_parent->fr_child, wp->w_frame);
	નહીંતર
	    frame_append(frp2, wp->w_frame);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    temp = curwin->w_status_height;
    જો (temp != wp->w_status_height)
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
#જો defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    જો (wp->w_buffer != curbuf)
	reset_VIsual_and_resel();
    નહીંતર જો (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    સ્થિર શૂન્ય
win_rotate(પૂર્ણાંક upwards, પૂર્ણાંક ગણતરી)
{
    win_T	*wp1;
    win_T	*wp2;
    frame_T	*frp;
    પૂર્ણાંક		n;

    જો (ONE_WINDOW)		// nothing to do
    {
	beep_flush();
	પાછા;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Check if all frames in this row/col have one window.
    FOR_ALL_FRAMES(frp, curwin->w_frame->fr_parent->fr_child)
	જો (frp->fr_win == ખાલી)
	{
	    emsg(_(e_cannot_rotate_when_another_window_is_split));
	    પાછા;
	}

    જ્યારે (ગણતરી--)
    {
	જો (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_frame->fr_parent->fr_child;
	    wp1 = frp->fr_win;
	    win_remove(wp1, ખાલી);
	    frame_remove(frp);

	    // find last frame and append removed window/frame after it
	    માટે ( ; frp->fr_next != ખાલી; frp = frp->fr_next)
		;
	    win_append(frp->fr_win, wp1);
	    frame_append(frp, wp1->w_frame);

	    wp2 = frp->fr_win;		// previously last window
	}
	નહીંતર			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    માટે (frp = curwin->w_frame; frp->fr_next != ખાલી;
							   frp = frp->fr_next)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, ખાલી);
	    frame_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_parent->fr_child->fr_win->w_prev, wp1);
	    frame_ઉમેરો(frp->fr_parent->fr_child, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_status_height;
	જો (n != wp1->w_status_height)
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
#જો defined(FEAT_STL_OPT)
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
    પૂર્ણાંક
win_splitmove(win_T *wp, પૂર્ણાંક size, પૂર્ણાંક flags)
{
    પૂર્ણાંક		dir;
    પૂર્ણાંક		height = wp->w_height;
    frame_T	*unflat_altfr;

    જો (ONE_WINDOW)
	પાછા OK;	// nothing to do
    જો (check_split_disallowed(wp) == FAIL)
	પાછા FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, ખાલી, &unflat_altfr);
    win_remove(wp, ખાલી);
    last_status(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    જો (win_split_ins(size, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	પાછા FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    જો (size == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(height, wp);
	જો (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#જો defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif
    પાછા OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    શૂન્ય
win_move_after(win_T *win1, win_T *win2)
{
    પૂર્ણાંક		height;

    // check if the arguments are reasonable
    જો (win1 == win2)
	પાછા;

    // check if there is something to do
    જો (win2->w_next != win1)
    {
	જો (win1->w_frame->fr_parent != win2->w_frame->fr_parent)
	{
	    iemsg("Trying to move a window into another frame");
	    પાછા;
	}

	// may need to move the status line/vertical separator of the last
	// window
	જો (win1 == lastwin)
	{
	    height = win1->w_prev->w_status_height;
	    win1->w_prev->w_status_height = win1->w_status_height;
	    win1->w_status_height = height;
	    જો (win1->w_prev->w_vsep_width == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_width = 0;
		win1->w_prev->w_frame->fr_width -= 1;
		win1->w_vsep_width = 1;
		win1->w_frame->fr_width += 1;
	    }
	}
	નહીંતર જો (win2 == lastwin)
	{
	    height = win1->w_status_height;
	    win1->w_status_height = win2->w_status_height;
	    win2->w_status_height = height;
	    જો (win1->w_vsep_width == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_width = 1;
		win2->w_frame->fr_width += 1;
		win1->w_vsep_width = 0;
		win1->w_frame->fr_width -= 1;
	    }
	}
	win_remove(win1, ખાલી);
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
    શૂન્ય
win_equal(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    પૂર્ણાંક		current,	// do only frame with current window
    પૂર્ણાંક		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    જો (dir == 0)
	dir = *p_ead;
    win_equal_rec(next_curwin == ખાલી ? curwin : next_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_height(),
		      topframe->fr_width, topframe->fr_height);
#જો defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
    જો (!is_aucmd_win(next_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    સ્થિર શૂન્ય
win_equal_rec(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    પૂર્ણાંક		current,	// do only frame with current window
    frame_T	*topfr,		// frame to set size off
    પૂર્ણાંક		dir,		// 'v', 'h' or 'b', see win_equal()
    પૂર્ણાંક		col,		// horizontal position for frame
    પૂર્ણાંક		row,		// vertical position for frame
    પૂર્ણાંક		width,		// new width of frame
    પૂર્ણાંક		height)		// new height of frame
{
    પૂર્ણાંક		n, m;
    પૂર્ણાંક		extra_sep = 0;
    પૂર્ણાંક		wincount, totwincount = 0;
    frame_T	*fr;
    પૂર્ણાંક		next_curwin_size = 0;
    પૂર્ણાંક		room = 0;
    પૂર્ણાંક		new_size;
    પૂર્ણાંક		has_next_curwin = 0;
    પૂર્ણાંક		hnc;

    જો (topfr->fr_layout == FR_LEAF)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	જો (topfr->fr_height != height || topfr->fr_win->w_winrow != row
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
    નહીંતર જો (topfr->fr_layout == FR_ROW)
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	જો (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = frame_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    જો (col + width >= firstwin->w_wincol + topframe->fr_width)
		extra_sep = 1;
	    નહીંતર
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
	    જો (room < 0)
	    {
		next_curwin_size = p_wiw + room;
		room = 0;
	    }
	    નહીંતર
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    જો (!frame_fixed_width(fr))
			ચાલુ;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minwidth(fr, NOWIN);
		    new_size = fr->fr_width;
		    જો (frame_has_win(fr, next_curwin))
		    {
			room += p_wiw - p_wmw;
			next_curwin_size = 0;
			જો (new_size < p_wiw)
			    new_size = p_wiw;
		    }
		    નહીંતર
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_next == ખાલી
					? extra_sep : 0)) / (p_wmw + 1);
		    room -= new_size - n;
		    જો (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newwidth = new_size;
		}
		જો (next_curwin_size == -1)
		{
		    જો (!has_next_curwin)
			next_curwin_size = 0;
		    નહીંતર જો (totwincount > 1
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
		    નહીંતર
			next_curwin_size = p_wiw;
		}
	    }

	    જો (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    જો (fr->fr_next == ખાલી)
		// last frame gets all that remains (avoid roundoff error)
		new_size = width;
	    નહીંતર જો (dir == 'v')
		new_size = fr->fr_width;
	    નહીંતર જો (frame_fixed_width(fr))
	    {
		new_size = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    નહીંતર
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = frame_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_next == ખાલી ? extra_sep : 0))
								/ (p_wmw + 1);
		m = frame_minwidth(fr, next_curwin);
		જો (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		નહીંતર
		    hnc = FALSE;
		જો (hnc)	    // don't count next_curwin
		    --wincount;
		જો (totwincount == 0)
		    new_size = room;
		નહીંતર
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		જો (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wiw - (m - n);
		    જો (next_curwin_size < 0)
			next_curwin_size = 0;
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		નહીંતર
		    room -= new_size;
		new_size += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    જો (!current || dir != 'v' || topfr->fr_parent != ખાલી
		    || (new_size != fr->fr_width)
		    || frame_has_win(fr, next_curwin))
		win_equal_rec(next_curwin, current, fr, dir, col, row,
							    new_size, height);
	    col += new_size;
	    width -= new_size;
	    totwincount -= wincount;
	}
    }
    નહીંતર // topfr->fr_layout == FR_COL
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	જો (dir != 'h')			// equalize frame heights
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
	    જો (room < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		next_curwin_size = p_wh + room;
		room = 0;
	    }
	    નહીંતર
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    જો (!frame_fixed_height(fr))
			ચાલુ;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minheight(fr, NOWIN);
		    new_size = fr->fr_height;
		    જો (frame_has_win(fr, next_curwin))
		    {
			room += p_wh - p_wmh;
			next_curwin_size = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			જો (new_size < n + (p_wh - p_wmh))
			    new_size = n + (p_wh - p_wmh);
		    }
		    નહીંતર
			// These windows don't use up room.
			totwincount -= frame_wincount_in_height(fr);
		    room -= new_size - n;
		    જો (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newheight = new_size;
		}
		જો (next_curwin_size == -1)
		{
		    જો (!has_next_curwin)
			next_curwin_size = 0;
		    નહીંતર જો (totwincount > 1
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
		    નહીંતર
			next_curwin_size = p_wh;
		}
	    }

	    જો (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    જો (fr->fr_next == ખાલી)
		// last frame gets all that remains (avoid roundoff error)
		new_size = height;
	    નહીંતર જો (dir == 'h')
		new_size = fr->fr_height;
	    નહીંતર જો (frame_fixed_height(fr))
	    {
		new_size = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    નહીંતર
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = frame_minheight(fr, NOWIN);
		wincount = frame_wincount_in_height(fr);
		m = frame_minheight(fr, next_curwin);
		જો (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		નહીંતર
		    hnc = FALSE;
		જો (hnc)	    // don't count next_curwin
		    --wincount;
		જો (totwincount == 0)
		    new_size = room;
		નહીંતર
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		જો (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wh - (m - n);
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		નહીંતર
		    room -= new_size;
		new_size += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    જો (!current || dir != 'h' || topfr->fr_parent != ખાલી
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

#જો defined(FEAT_JOB_CHANNEL)
    શૂન્ય
leaving_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    જો (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	પાછા;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_buffer->b_prompt_ઉમેરો = restart_edit;
    જો (restart_edit != 0 && mode_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    જો ((State & MODE_ઉમેરો) && !stop_ઉમેરો_mode)
    {
	stop_ઉમેરો_mode = TRUE;
	જો (win->w_buffer->b_prompt_ઉમેરો == NUL)
	    win->w_buffer->b_prompt_ઉમેરો = 'A';
    }
}

    શૂન્ય
entering_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    જો (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	પાછા;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    જો (win->w_buffer->b_prompt_ઉમેરો != NUL)
	stop_ઉમેરો_mode = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    જો ((State & MODE_ઉમેરો) == 0)
	restart_edit = win->w_buffer->b_prompt_ઉમેરો;
}
#endif

    સ્થિર શૂન્ય
win_init_empty(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_લીટીઓ_valid = 0;
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
#જો defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
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
    શૂન્ય
curwin_init(શૂન્ય)
{
    win_init_empty(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    શૂન્ય
close_windows(
    buf_T	*buf,
    પૂર્ણાંક		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    પૂર્ણાંક		ગણતરી = tabpage_index(ખાલી);

    ++RedrawingDisabled;

    માટે (wp = firstwin; wp != ખાલી && !ONE_WINDOW; )
    {
	જો (wp->w_buffer == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_buffer->b_locked > 0))
	{
	    જો (window_layout_locked(CMD_SIZE))
		જાઓ theend;  // Only give one error message.
	    જો (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		તોડો;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	નહીંતર
	    wp = wp->w_next;
    }

    // Also check windows in other tab pages.
    માટે (tp = first_tabpage; tp != ખાલી; tp = nexttp)
    {
	nexttp = tp->tp_next;
	જો (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		જો (wp->w_buffer == buf
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0))
		{
		    જો (window_layout_locked(CMD_SIZE))
			જાઓ theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = first_tabpage;
		    તોડો;
		}
    }

theend:
    જો (RedrawingDisabled > 0)
	--RedrawingDisabled;

    જો (ગણતરી != tabpage_index(ખાલી))
	apply_autocmds(EVENT_TABCLOSED, ખાલી, ખાલી, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    પૂર્ણાંક
last_window(શૂન્ય)
{
    પાછા (one_window() && first_tabpage->tp_next == ખાલી);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    પૂર્ણાંક
one_window(શૂન્ય)
{
    win_T	*wp;
    પૂર્ણાંક		seen_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	જો (!is_aucmd_win(wp))
	{
	    જો (seen_one)
		પાછા FALSE;
	    seen_one = TRUE;
	}
    }
    પાછા TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    સ્થિર પૂર્ણાંક
close_last_window_tabpage(
    win_T	*win,
    પૂર્ણાંક		free_buf,
    tabpage_T   *prev_curtab)
{
    જો (!ONE_WINDOW)
	પાછા FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_buffer != ખાલી);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    જો (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    જો (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, ખાલી, ખાલી, FALSE, curbuf);
#જો defined(FEAT_TABPANEL)
    જો (p_stpl > 0)
	shell_new_columns();
#endif
    apply_autocmds(EVENT_WINENTER, ખાલી, ખાલી, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, ખાલી, ખાલી, FALSE, curbuf);
    જો (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, ખાલી, ખાલી, FALSE, curbuf);
    પાછા TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    સ્થિર શૂન્ય
win_close_buffer(win_T *win, પૂર્ણાંક action, પૂર્ણાંક abort_if_last)
{
#ifdef FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    જો (win->w_buffer != ખાલી)
	reset_synblock(win);
#endif

#ifdef FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    જો (win->w_buffer != ખાલી && bt_quickfix(win->w_buffer)
					&& win->w_buffer->b_nwindows == 1)
	win->w_buffer->b_p_bl = FALSE;
#endif

    // Close the link to the buffer.
    જો (win->w_buffer != ખાલી)
    {
	bufref_T    bufref;

	set_bufref(&bufref, curbuf);
	++win->w_locked;
	close_buffer(win, win->w_buffer, action, abort_if_last, TRUE, TRUE);
	જો (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	જો (!bufref_valid(&bufref))
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
    પૂર્ણાંક
win_close(win_T *win, પૂર્ણાંક free_buf)
{
    win_T	*wp;
    પૂર્ણાંક		other_buffer = FALSE;
    પૂર્ણાંક		close_curwin = FALSE;
    પૂર્ણાંક		dir;
    પૂર્ણાંક		help_window = FALSE;
    પૂર્ણાંક		quickfix_window = FALSE;
    tabpage_T   *prev_curtab = curtab;
    frame_T	*win_frame = win->w_frame->fr_parent;
#ifdef FEAT_DIFF
    પૂર્ણાંક		had_diffmode = win->w_p_diff;
#endif
#ifdef MESSAGE_QUEUE
    પૂર્ણાંક		did_decrement = FALSE;
#endif

#જો defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    જો (may_close_term_popup() == OK)
	પાછા OK;
#endif
    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા FAIL;

    જો (last_window())
    {
	emsg(_(e_cannot_close_last_window));
	પાછા FAIL;
    }
    જો (window_layout_locked(CMD_close))
	પાછા FAIL;

    જો (win_locked(win) || (win->w_buffer != ખાલી
					       && win->w_buffer->b_locked > 0))
	પાછા FAIL; // window is already being closed
    જો (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	પાછા FAIL;
    }
    જો ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_window())
    {
	emsg(_(e_cannot_close_window_only_autocmd_window_would_remain));
	પાછા FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    જો (close_last_window_tabpage(win, free_buf, prev_curtab))
	પાછા FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    જો (bt_help(win->w_buffer))
	help_window = TRUE;
    નહીંતર
	clear_snapshot(curtab, SNAP_HELP_IDX);

    જો (bt_quickfix(win->w_buffer))
	quickfix_window = TRUE;
    નહીંતર
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    જો (win == curwin)
    {
#ifdef FEAT_JOB_CHANNEL
	leaving_window(curwin);
#endif
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, ખાલી));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	જો (wp->w_buffer != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_buffer = TRUE;
	    જો (!win_valid(win))
		પાછા FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, ખાલી, ખાલી, FALSE, curbuf);
	    જો (!win_valid(win))
		પાછા FAIL;
	    --win->w_locked;
	    જો (last_window())
		પાછા FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, ખાલી, ખાલી, FALSE, curbuf);
	જો (!win_valid(win))
	    પાછા FAIL;
	--win->w_locked;
	જો (last_window())
	    પાછા FAIL;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	જો (aborting())
	    પાછા FAIL;
#endif
    }

#ifdef FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    જો (gui.in_use)
	out_flush();
#endif

#ifdef FEAT_PROP_POPUP
    જો (popup_win_closed(win) && !win_valid(win))
	પાછા FAIL;
#endif

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    જો (!win_valid_any_tab(win))
	પાછા OK;

    win_close_buffer(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    જો (win_valid(win) && win->w_buffer == ખાલી
#જો defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#endif
	    && last_window())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	જો (curwin->w_buffer == ખાલી)
	    curwin->w_buffer = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    જો (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_buffer == ખાલી)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	પાછા FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    જો (!win_valid(win) || last_window()
	    || close_last_window_tabpage(win, free_buf, prev_curtab))
	પાછા FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++split_disallowed;
#ifdef MESSAGE_QUEUE
    ++dont_parse_messages;
#endif

    જો (win->w_buffer != ખાલી)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, ખાલી);

    જો (help_window || quickfix_window)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	જો (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    જો (win == curwin)
    {
	curwin = wp;
#ifdef FEAT_QUICKFIX
	જો (wp->w_p_pvw || bt_quickfix(wp->w_buffer))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    માટે (;;)
	    {
		જો (wp->w_next == ખાલી)
		    wp = firstwin;
		નહીંતર
		    wp = wp->w_next;
		જો (wp == curwin)
		    તોડો;
		જો (!wp->w_p_pvw && !bt_quickfix(wp->w_buffer))
		{
		    curwin = wp;
		    તોડો;
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

    જો (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_frame->fr_parent == win_frame, dir);
    નહીંતર
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    જો (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ifdef MESSAGE_QUEUE
	did_decrement =
#નહીંતર
	(શૂન્ય)
#endif
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	જો (other_buffer)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, ખાલી, ખાલી, FALSE, curbuf);
    }

    જો (ONE_WINDOW && curwin->w_locked && curbuf->b_locked_split
	    && first_tabpage->tp_next != ખાલી)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, ખાલી, ખાલી, FALSE, curbuf);

    --split_disallowed;
#ifdef MESSAGE_QUEUE
    જો (!did_decrement)
	--dont_parse_messages;
#endif

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    જો (help_window || quickfix_window)
	restore_snapshot(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ifdef FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    જો (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	પૂર્ણાંક	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    જો (dwin->w_p_diff)
		++diffcount;
	જો (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#endif

#જો defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    જો (gui.in_use && !win_hasvertsplit())
	gui_init_which_components(ખાલી);
#endif

    redraw_all_later(UPD_NOT_VALID);
    પાછા OK;
}

    સ્થિર શૂન્ય
trigger_winnewpre(શૂન્ય)
{
    window_layout_lock();
    apply_autocmds(EVENT_WINNEWPRE, ખાલી, ખાલી, FALSE, ખાલી);
    window_layout_unlock();
}

    સ્થિર શૂન્ય
trigger_winclosed(win_T *win)
{
    સ્થિર પૂર્ણાંક	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    જો (recursive)
	પાછા;
    recursive = TRUE;
    vim_snprintf((અક્ષર *)winid, કદ(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_buffer);
    recursive = FALSE;
}

    શૂન્ય
trigger_tabclosedpre(tabpage_T *tp)
{
    સ્થિર પૂર્ણાંક	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    જો (!has_tabclosedpre() || recursive)
	પાછા;

    જો (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    window_layout_lock();
    apply_autocmds(EVENT_TABCLOSEDPRE, ખાલી, ખાલી, FALSE, ખાલી);
    window_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    જો (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    નહીંતર
	// fall back to the first tabpage
	goto_tabpage_tp(first_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    શૂન્ય
snapshot_windows_scroll_size(શૂન્ય)
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

સ્થિર પૂર્ણાંક did_initial_scroll_size_snapshot = FALSE;

    શૂન્ય
may_make_initial_scroll_size_snapshot(શૂન્ય)
{
    જો (!did_initial_scroll_size_snapshot)
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
    સ્થિર dict_T *
make_win_info_dict(
	પૂર્ણાંક width,
	પૂર્ણાંક height,
	પૂર્ણાંક topline,
# ifdef FEAT_DIFF
	પૂર્ણાંક topfill,
# endif
	પૂર્ણાંક leftcol,
	પૂર્ણાંક skipcol)
{
    dict_T *d = dict_alloc();
    જો (d == ખાલી)
	પાછા ખાલી;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    જ્યારે (1)
    {
	typval_T tv;
	tv.v_lock = 0;
	tv.v_type = VAR_સંખ્યા;

	tv.vval.v_સંખ્યા = width;
	જો (dict_add_tv(d, "width", &tv) == FAIL)
	    તોડો;
	tv.vval.v_સંખ્યા = height;
	જો (dict_add_tv(d, "height", &tv) == FAIL)
	    તોડો;
	tv.vval.v_સંખ્યા = topline;
	જો (dict_add_tv(d, "topline", &tv) == FAIL)
	    તોડો;
# ifdef FEAT_DIFF
	tv.vval.v_સંખ્યા = topfill;
# નહીંતર
	tv.vval.v_સંખ્યા = 0;
# endif
	જો (dict_add_tv(d, "topfill", &tv) == FAIL)
	    તોડો;
	tv.vval.v_સંખ્યા = leftcol;
	જો (dict_add_tv(d, "leftcol", &tv) == FAIL)
	    તોડો;
	tv.vval.v_સંખ્યા = skipcol;
	જો (dict_add_tv(d, "skipcol", &tv) == FAIL)
	    તોડો;
	પાછા d;
    }
    dict_unref(d);
    પાછા ખાલી;
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
    સ્થિર શૂન્ય
check_window_scroll_resize(
	પૂર્ણાંક	*size_ગણતરી,
	win_T	**first_scroll_win,
	win_T	**first_size_win,
	list_T	*winlist UNUSED,
	dict_T	*v_event UNUSED)
{
#ifdef FEAT_EVAL
    પૂર્ણાંક listidx = 0;
    પૂર્ણાંક tot_width = 0;
    પૂર્ણાંક tot_height = 0;
    પૂર્ણાંક tot_topline = 0;
# ifdef FEAT_DIFF
    પૂર્ણાંક tot_topfill = 0;
# endif
    પૂર્ણાંક tot_leftcol = 0;
    પૂર્ણાંક tot_skipcol = 0;
#endif

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	પૂર્ણાંક ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	પૂર્ણાંક size_changed = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_last_width != wp->w_width
			    || wp->w_last_height != wp->w_height);
	જો (size_changed)
	{
#ifdef FEAT_EVAL
	    જો (winlist != ખાલી)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_lock = 0;
		tv.v_type = VAR_સંખ્યા;
		tv.vval.v_સંખ્યા = wp->w_id;
		list_set_item(winlist, listidx++, &tv);
	    }
	    નહીંતર
#endif
		જો (size_ગણતરી != ખાલી)
	    {
		++*size_ગણતરી;
		જો (*first_size_win == ખાલી)
		    *first_size_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		જો (*first_scroll_win == ખાલી && !ignore_scroll)
		    *first_scroll_win = wp;
	    }
	}

	પૂર્ણાંક scroll_changed = !ignore_scroll
				&& (wp->w_last_topline != wp->w_topline
#ifdef FEAT_DIFF
				|| wp->w_last_topfill != wp->w_topfill
#endif
				|| wp->w_last_leftcol != wp->w_leftcol
				|| wp->w_last_skipcol != wp->w_skipcol);
	જો (scroll_changed
	    && first_scroll_win != ખાલી && *first_scroll_win == ખાલી)
	    *first_scroll_win = wp;

#ifdef FEAT_EVAL
	જો ((size_changed || scroll_changed) && v_event != ખાલી)
	{
	    // Add info about this window to the v:event dictionary.
	    પૂર્ણાંક width = wp->w_width - wp->w_last_width;
	    પૂર્ણાંક height = wp->w_height - wp->w_last_height;
	    પૂર્ણાંક topline = wp->w_topline - wp->w_last_topline;
# ifdef FEAT_DIFF
	    પૂર્ણાંક topfill = wp->w_topfill - wp->w_last_topfill;
# endif
	    પૂર્ણાંક leftcol = wp->w_leftcol - wp->w_last_leftcol;
	    પૂર્ણાંક skipcol = wp->w_skipcol - wp->w_last_skipcol;
	    dict_T *d = make_win_info_dict(width, height, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    જો (d == ખાલી)
		તોડો;
	    અક્ષર winid[NUMBUFLEN];
	    vim_snprintf(winid, કદ(winid), "%d", wp->w_id);
	    જો (dict_add_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		તોડો;
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
    જો (v_event != ખાલી)
    {
	dict_T *alldict = make_win_info_dict(tot_width, tot_height, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	જો (alldict != ખાલી)
	{
	    જો (dict_add_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    નહીંતર
		--alldict->dv_refcount;
	}
    }
#endif
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    શૂન્ય
may_trigger_win_scrolled_resized(શૂન્ય)
{
    સ્થિર પૂર્ણાંક	    recursive = FALSE;
    પૂર્ણાંક		    do_resize = has_winresized();
    પૂર્ણાંક		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    જો (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_size_snapshot)
	પાછા;

    પૂર્ણાંક size_ગણતરી = 0;
    win_T *first_scroll_win = ખાલી, *first_size_win = ખાલી;
    check_window_scroll_resize(&size_ગણતરી, &first_scroll_win, &first_size_win,
								    ખાલી, ખાલી);
    પૂર્ણાંક trigger_resize = do_resize && size_ગણતરી > 0;
    પૂર્ણાંક trigger_scroll = do_scroll && first_scroll_win != ખાલી;
    જો (!trigger_resize && !trigger_scroll)
	પાછા;  // no relevant changes
#ifdef FEAT_EVAL
    list_T *windows_list = ખાલી;
    જો (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_વસ્તુઓ(size_ગણતરી);
	જો (windows_list != ખાલી)
	    check_window_scroll_resize(ખાલી, ખાલી, ખાલી, windows_list, ખાલી);
    }

    dict_T *scroll_dict = ખાલી;
    જો (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	જો (scroll_dict != ખાલી)
	{
	    scroll_dict->dv_refcount = 1;
	    check_window_scroll_resize(ખાલી, ખાલી, ખાલી, ખાલી, scroll_dict);
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
    જો (trigger_resize
#ifdef FEAT_EVAL
	    && windows_list != ખાલી
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	જો (dict_add_list(v_event, "windows", windows_list) == OK)
	{
	    dict_set_વસ્તુઓ_ro(v_event);
#endif
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((અક્ષર *)winid, કદ(winid), "%d",
							 first_size_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     first_size_win->w_buffer);
#ifdef FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#endif
    }

    જો (trigger_scroll
#ifdef FEAT_EVAL
	    && scroll_dict != ખાલી
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", ખાલી);
	dict_set_વસ્તુઓ_ro(v_event);
	dict_unref(scroll_dict);
#endif
	char_u winid[NUMBUFLEN];
	vim_snprintf((અક્ષર *)winid, કદ(winid), "%d",
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
    શૂન્ય
win_close_othertab(win_T *win, પૂર્ણાંક free_buf, tabpage_T *tp)
{
    win_T	*wp;
    પૂર્ણાંક		dir;
    tabpage_T   *ptp = ખાલી;
    પૂર્ણાંક		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    જો (window_layout_locked(CMD_SIZE))
	પાછા;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    જો (win_locked(win) || (win->w_buffer != ખાલી
					       && win->w_buffer->b_locked > 0))
	પાછા; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    જો (win->w_buffer != ખાલી)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	જો (!win_valid_any_tab(win))
	    પાછા;
    }

    જો (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	જો (!win_valid_any_tab(win))
	    પાછા;
    }

    જો (win->w_buffer != ખાલી)
	// Close the link to the buffer.
	close_buffer(win, win->w_buffer, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    માટે (ptp = first_tabpage; ptp != ખાલી && ptp != tp; ptp = ptp->tp_next)
	;
    જો (ptp == ખાલી || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	જો (win_valid_any_tab(win) && win->w_buffer == ખાલી)
	{
	    win->w_buffer = firstbuf;
	    ++firstbuf->b_nwindows;
	    જો (win == curwin)
		curbuf = curwin->w_buffer;
	    win_init_empty(win);
	}
	પાછા;
    }

    // Autocommands may have closed the window already.
    માટે (wp = tp->tp_firstwin; wp != ખાલી && wp != win; wp = wp->w_next)
	;
    જો (wp == ખાલી)
	પાછા;

    // When closing the last window in a tab page remove the tab page.
    જો (tp->tp_firstwin == tp->tp_lastwin)
    {
	પૂર્ણાંક	h = tabline_height();

	જો (tp == first_tabpage)
	    first_tabpage = tp->tp_next;
	નહીંતર
	{
	    માટે (ptp = first_tabpage; ptp != ખાલી && ptp->tp_next != tp;
							   ptp = ptp->tp_next)
		;
	    જો (ptp == ખાલી)
	    {
		internal_error("win_close_othertab()");
		પાછા;
	    }
	    ptp->tp_next = tp->tp_next;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	જો (h != tabline_height())
	    shell_new_rows();
    }

    જો (win->w_buffer != ખાલી)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    જો (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    સ્થિર win_T *
win_free_mem(
    win_T	*win,
    પૂર્ણાંક		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == ખાલી ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_frame;
    wp = winframe_remove(win, dirp, tp, ખાલી);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    જો (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    પાછા wp;
}

#જો defined(EXITFREE)
    શૂન્ય
win_free_all(શૂન્ય)
{
    પૂર્ણાંક		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = ખાલી;
    cmdwin_win = ખાલી;

    જ્યારે (first_tabpage->tp_next != ખાલી)
	tabpage_close(TRUE);

    માટે (પૂર્ણાંક i = 0; i < AUCMD_WIN_ગણતરી; ++i)
	જો (aucmd_win[i].auc_win != ખાલી)
	{
	    (શૂન્ય)win_free_mem(aucmd_win[i].auc_win, &dummy, ખાલી);
	    aucmd_win[i].auc_win = ખાલી;
	}

    જ્યારે (firstwin != ખાલી)
	(શૂન્ય)win_free_mem(firstwin, &dummy, ખાલી);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = ખાલી;
}
#endif

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    પૂર્ણાંક		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    frame_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    frame_T	*frp, *frp2, *frp3;
    frame_T	*frp_close = win->w_frame;
    win_T	*wp;
    પૂર્ણાંક		row, col;

    /*
     * If there is only one window there is nothing to remove.
     */
    જો (tp == ખાલી ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	પાછા ખાલી;

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
    જો (win->w_vsep_width == 0 && frp_close->fr_parent->fr_layout == FR_ROW &&
	    frp_close->fr_prev != ખાલી)
	frame_set_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    frame_remove(frp_close);

    જો (frp_close->fr_parent->fr_layout == FR_COL)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	જો (frp2->fr_win != ખાલી && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    જ્યારે (frp != ખાલી || frp3 != ખાલી)
	    {
		જો (frp != ખાલી)
		{
		    જો (!frame_fixed_height(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			તોડો;
		    }
		    frp = frp->fr_prev;
		}
		જો (frp3 != ખાલી)
		{
		    જો (frp3->fr_win != ખાલી && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			તોડો;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_height(frp2, frp2->fr_height + frp_close->fr_height,
			    frp2 == frp_close->fr_next, !frame_fixed_height(frp2), FALSE);
	*dirp = 'v';
    }
    નહીંતર
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	જો (frp2->fr_win != ખાલી && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    જ્યારે (frp != ખાલી || frp3 != ખાલી)
	    {
		જો (frp != ખાલી)
		{
		    જો (!frame_fixed_width(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			તોડો;
		    }
		    frp = frp->fr_prev;
		}
		જો (frp3 != ખાલી)
		{
		    જો (frp3->fr_win != ખાલી && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			તોડો;
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
    જો (frp2 != frp_close->fr_prev)
	frame_comp_pos(frp_close->fr_parent, &row, &col);

    જો (unflat_altfr == ખાલી)
	frame_flatten(frp2);
    નહીંતર
	*unflat_altfr = frp2;

    frame_locked--;

    પાછા wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    સ્થિર શૂન્ય
frame_flatten(frame_T *frp)
{
    frame_T	*frp2, *frp3;

    જો (frp->fr_next != ખાલી || frp->fr_prev != ખાલી)
	પાછા;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_parent->fr_layout = frp->fr_layout;
    frp->fr_parent->fr_child = frp->fr_child;
    FOR_ALL_FRAMES(frp2, frp->fr_child)
	frp2->fr_parent = frp->fr_parent;
    frp->fr_parent->fr_win = frp->fr_win;
    જો (frp->fr_win != ખાલી)
	frp->fr_win->w_frame = frp->fr_parent;
    frp2 = frp->fr_parent;
    જો (topframe->fr_child == frp)
	topframe->fr_child = frp2;
    vim_free(frp);

    frp = frp2->fr_parent;
    જો (frp != ખાલી && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	જો (frp->fr_child == frp2)
	    frp->fr_child = frp2->fr_child;
	frp2->fr_child->fr_prev = frp2->fr_prev;
	જો (frp2->fr_prev != ખાલી)
	    frp2->fr_prev->fr_next = frp2->fr_child;
	માટે (frp3 = frp2->fr_child; ; frp3 = frp3->fr_next)
	{
	    frp3->fr_parent = frp;
	    જો (frp3->fr_next == ખાલી)
	    {
		frp3->fr_next = frp2->fr_next;
		જો (frp2->fr_next != ખાલી)
		    frp2->fr_next->fr_prev = frp3;
		તોડો;
	    }
	}
	જો (topframe->fr_child == frp2)
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
    સ્થિર શૂન્ય
winframe_restore(win_T *wp, પૂર્ણાંક dir, frame_T *unflat_altfr)
{
    frame_T	*frp = wp->w_frame;

    // Put "wp"'s frame back where it was.
    જો (frp->fr_prev != ખાલી)
	frame_append(frp->fr_prev, frp);
    નહીંતર
	frame_ઉમેરો(frp->fr_next, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    જો (wp->w_vsep_width == 0
	    && frp->fr_parent->fr_layout == FR_ROW && frp->fr_prev != ખાલી)
	frame_set_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    જો (wp->w_status_height == 0
	    && frp->fr_parent->fr_layout == FR_COL && frp->fr_prev != ખાલી)
	frame_add_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    જો (dir == 'v')
    {
	frame_new_height(unflat_altfr, unflat_altfr->fr_height - frp->fr_height,
		unflat_altfr == frp->fr_next, FALSE, FALSE);
    }
    નહીંતર જો (dir == 'h')
    {
	frame_new_width(unflat_altfr, unflat_altfr->fr_width - frp->fr_width,
		unflat_altfr == frp->fr_next, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    જો (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_parent);
	પૂર્ણાંક	row = topleft->w_winrow;
	પૂર્ણાંક	col = topleft->w_wincol;

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
    સ્થિર frame_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    frame_T	*other_fr, *target_fr;

    જો (tp == ખાલી ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	પાછા alt_tabpage()->tp_curwin->w_frame;

    frp = win->w_frame;

    જો (frp->fr_prev == ખાલી)
	પાછા frp->fr_next;
    જો (frp->fr_next == ખાલી)
	પાછા frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    // window
    target_fr = frp->fr_next;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    જો (frp->fr_parent != ખાલી && frp->fr_parent->fr_layout == FR_COL && p_sb)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    જો (frp->fr_parent != ખાલી && frp->fr_parent->fr_layout == FR_ROW && p_spr)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    જો (frp->fr_parent != ખાલી && frp->fr_parent->fr_layout == FR_ROW)
    {
	જો (frame_fixed_width(target_fr) && !frame_fixed_width(other_fr))
	    target_fr = other_fr;
    }
    નહીંતર
    {
	જો (frame_fixed_height(target_fr) && !frame_fixed_height(other_fr))
	    target_fr = other_fr;
    }

    પાછા target_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    સ્થિર tabpage_T *
alt_tabpage(શૂન્ય)
{
    tabpage_T	*tp = ખાલી;
    પૂર્ણાંક		forward;

    // Use the last accessed tab page, if possible.
    જો ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	પાછા lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_next != ખાલી &&
	    ((tcl_flags & TCL_ડાબું) == 0 || curtab == first_tabpage);

    જો (forward)
	tp = curtab->tp_next;
    નહીંતર
	// Use the previous tab page.
	માટે (tp = first_tabpage; tp->tp_next != curtab; tp = tp->tp_next)
	    ;

    પાછા tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    સ્થિર win_T *
frame2win(frame_T *frp)
{
    જ્યારે (frp->fr_win == ખાલી)
	frp = frp->fr_child;
    પાછા frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    સ્થિર પૂર્ણાંક
frame_has_win(frame_T *frp, win_T *wp)
{
    frame_T	*p;

    જો (frp->fr_layout == FR_LEAF)
	પાછા frp->fr_win == wp;

    FOR_ALL_FRAMES(p, frp->fr_child)
	જો (frame_has_win(p, wp))
	    પાછા TRUE;
    પાછા FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
સ્થિર પૂર્ણાંક min_set_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    સ્થિર શૂન્ય
frame_new_height(
    frame_T	*topfrp,
    પૂર્ણાંક		height,
    પૂર્ણાંક		topfirst,	// resize topmost contained frame first
    પૂર્ણાંક		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    પૂર્ણાંક		set_ch)		// set 'cmdheight' to resize topframe
{
    frame_T	*frp;
    પૂર્ણાંક		extra_લીટીઓ;
    પૂર્ણાંક		h;

    જો (topfrp->fr_parent == ખાલી && set_ch)
    {
	// topframe: update the command line height, with side effects.
	પૂર્ણાંક new_ch = MAX(min_set_ch, p_ch + topfrp->fr_height - height);
	પૂર્ણાંક save_ch = min_set_ch;
	જો (new_ch != p_ch)
	    set_option_કિંમત((char_u *)"cmdheight", new_ch, ખાલી, 0);
	min_set_ch = save_ch;
	height = MIN(height, ROWS_AVAIL);
    }
    જો (topfrp->fr_win != ખાલી)
    {
	// Simple case: just one window.
	win_new_height(topfrp->fr_win,
				    height - topfrp->fr_win->w_status_height
					      - WINBAR_HEIGHT(topfrp->fr_win));
    }
    નહીંતર જો (topfrp->fr_layout == FR_ROW)
    {
	કરો
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_height(frp, height, topfirst, wfh, set_ch);
		જો (frp->fr_height > height)
		{
		    // Could not fit the windows, make the whole row higher.
		    height = frp->fr_height;
		    તોડો;
		}
	    }
	}
	જ્યારે (frp != ખાલી);
    }
    નહીંતર    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_child;
	જો (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    જ્યારે (frame_fixed_height(frp))
	    {
		frp = frp->fr_next;
		જો (frp == ખાલી)
		    પાછા;	    // no frame without 'wfh', give up
	    }
	જો (!topfirst)
	{
	    // Find the bottom frame of this column
	    જ્યારે (frp->fr_next != ખાલી)
		frp = frp->fr_next;
	    જો (wfh)
		// Advance back for frames with one window with 'wfh' set.
		જ્યારે (frame_fixed_height(frp))
		    frp = frp->fr_prev;
	}

	extra_લીટીઓ = height - topfrp->fr_height;
	જો (extra_લીટીઓ < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    જ્યારે (frp != ખાલી)
	    {
		h = frame_minheight(frp, ખાલી);
		જો (frp->fr_height + extra_લીટીઓ < h)
		{
		    extra_લીટીઓ += frp->fr_height - h;
		    frame_new_height(frp, h, topfirst, wfh, set_ch);
		}
		નહીંતર
		{
		    frame_new_height(frp, frp->fr_height + extra_લીટીઓ,
							topfirst, wfh, set_ch);
		    તોડો;
		}
		જો (topfirst)
		{
		    કરો
			frp = frp->fr_next;
		    જ્યારે (wfh && frp != ખાલી && frame_fixed_height(frp));
		}
		નહીંતર
		{
		    કરો
			frp = frp->fr_prev;
		    જ્યારે (wfh && frp != ખાલી && frame_fixed_height(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		જો (frp == ખાલી)
		    height -= extra_લીટીઓ;
	    }
	}
	નહીંતર જો (extra_લીટીઓ > 0)
	{
	    // increase height of bottom or top frame
	    frame_new_height(frp, frp->fr_height + extra_લીટીઓ, topfirst, wfh,
									set_ch);
	}
    }
    topfrp->fr_height = height;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    સ્થિર પૂર્ણાંક
frame_fixed_height(frame_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    જો (frp->fr_win != ખાલી)
	પાછા frp->fr_win->w_p_wfh;

    જો (frp->fr_layout == FR_ROW)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    જો (frame_fixed_height(frp))
		પાછા TRUE;
	પાછા FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	જો (!frame_fixed_height(frp))
	    પાછા FALSE;
    પાછા TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    સ્થિર પૂર્ણાંક
frame_fixed_width(frame_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    જો (frp->fr_win != ખાલી)
	પાછા frp->fr_win->w_p_wfw;

    જો (frp->fr_layout == FR_COL)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    જો (frame_fixed_width(frp))
		પાછા TRUE;
	પાછા FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	જો (!frame_fixed_width(frp))
	    પાછા FALSE;
    પાછા TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    સ્થિર શૂન્ય
frame_add_statusline(frame_T *frp)
{
    win_T	*wp;

    જો (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	wp->w_status_height = statusline_height(wp);
    }
    નહીંતર જો (frp->fr_layout == FR_ROW)
    {
	// Handle all the frames in the row.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_statusline(frp);
    }
    નહીંતર // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	માટે (frp = frp->fr_child; frp->fr_next != ખાલી; frp = frp->fr_next)
	    ;
	frame_add_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    સ્થિર શૂન્ય
frame_new_width(
    frame_T	*topfrp,
    પૂર્ણાંક		width,
    પૂર્ણાંક		leftfirst,	// resize leftmost contained frame first
    પૂર્ણાંક		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    frame_T	*frp;
    પૂર્ણાંક		extra_cols;
    પૂર્ણાંક		w;
    win_T	*wp;

    જો (topfrp->fr_layout == FR_LEAF)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	માટે (frp = topfrp; frp->fr_parent != ખાલી; frp = frp->fr_parent)
	    જો (frp->fr_parent->fr_layout == FR_ROW && frp->fr_next != ખાલી)
		તોડો;
	જો (frp->fr_parent == ખાલી)
	    wp->w_vsep_width = 0;
	win_new_width(wp, width - wp->w_vsep_width);
    }
    નહીંતર જો (topfrp->fr_layout == FR_COL)
    {
	કરો
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_width(frp, width, leftfirst, wfw);
		જો (frp->fr_width > width)
		{
		    // Could not fit the windows, make whole column wider.
		    width = frp->fr_width;
		    તોડો;
		}
	    }
	} જ્યારે (frp != ખાલી);
    }
    નહીંતર    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_child;
	જો (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    જ્યારે (frame_fixed_width(frp))
	    {
		frp = frp->fr_next;
		જો (frp == ખાલી)
		    પાછા;	    // no frame without 'wfw', give up
	    }
	જો (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    જ્યારે (frp->fr_next != ખાલી)
		frp = frp->fr_next;
	    જો (wfw)
		// Advance back for frames with one window with 'wfw' set.
		જ્યારે (frame_fixed_width(frp))
		    frp = frp->fr_prev;
	}

	extra_cols = width - topfrp->fr_width;
	જો (extra_cols < 0)
	{
	    // reduce frame width, rightmost frame first
	    જ્યારે (frp != ખાલી)
	    {
		w = frame_minwidth(frp, ખાલી);
		જો (frp->fr_width + extra_cols < w)
		{
		    extra_cols += frp->fr_width - w;
		    frame_new_width(frp, w, leftfirst, wfw);
		}
		નહીંતર
		{
		    frame_new_width(frp, frp->fr_width + extra_cols,
							      leftfirst, wfw);
		    તોડો;
		}
		જો (leftfirst)
		{
		    કરો
			frp = frp->fr_next;
		    જ્યારે (wfw && frp != ખાલી && frame_fixed_width(frp));
		}
		નહીંતર
		{
		    કરો
			frp = frp->fr_prev;
		    જ્યારે (wfw && frp != ખાલી && frame_fixed_width(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		જો (frp == ખાલી)
		    width -= extra_cols;
	    }
	}
	નહીંતર જો (extra_cols > 0)
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
    સ્થિર શૂન્ય
frame_set_vsep(frame_T *frp, પૂર્ણાંક add)
{
    win_T	*wp;

    જો (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	જો (add && wp->w_vsep_width == 0)
	{
	    જો (wp->w_width > 0)	// don't make it negative
		win_new_width(wp, wp->w_width - 1);
	    wp->w_vsep_width = 1;
	}
	નહીંતર જો (!add && wp->w_vsep_width == 1)
	{
	    win_new_width(wp, wp->w_width + 1);
	    wp->w_vsep_width = 0;
	}
    }
    નહીંતર જો (frp->fr_layout == FR_COL)
    {
	// Handle all the frames in the column.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_set_vsep(frp, add);
    }
    નહીંતર // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_child;
	જ્યારે (frp->fr_next != ખાલી)
	    frp = frp->fr_next;
	frame_set_vsep(frp, add);
    }
}

/*
 * Set frame width from the window it contains.
 */
    સ્થિર શૂન્ય
frame_fix_width(win_T *wp)
{
    wp->w_frame->fr_width = wp->w_width + wp->w_vsep_width;
}

/*
 * Set frame height from the window it contains.
 */
    સ્થિર શૂન્ય
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
    સ્થિર પૂર્ણાંક
frame_minheight(frame_T *topfrp, win_T *next_curwin)
{
    frame_T	*frp;
    પૂર્ણાંક		m;
    પૂર્ણાંક		n;

    જો (topfrp->fr_win != ખાલી)
    {
	જો (topfrp->fr_win == next_curwin)
	    m = p_wh + topfrp->fr_win->w_status_height;
	નહીંતર
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_status_height;
	    જો (topfrp->fr_win == curwin && next_curwin == ખાલી)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		જો (p_wmh == 0)
		    ++m;
		m += WINBAR_HEIGHT(curwin);
	    }
	}
    }
    નહીંતર જો (topfrp->fr_layout == FR_ROW)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minheight(frp, next_curwin);
	    જો (n > m)
		m = n;
	}
    }
    નહીંતર
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minheight(frp, next_curwin);
    }

    પાછા m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
સ્થિર પૂર્ણાંક
frame_wincount_in_height(frame_T *topfrp)
{
    frame_T	*frp;
    પૂર્ણાંક		c = 0;
    પૂર્ણાંક		n;

    જો (topfrp->fr_win != ખાલી)
	c = 1;
    નહીંતર જો (topfrp->fr_layout == FR_ROW)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_wincount_in_height(frp);
	    જો (n > c)
		c = n;
	}
    }
    નહીંતર
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    c += frame_wincount_in_height(frp);
    }

    પાછા c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    સ્થિર પૂર્ણાંક
frame_minwidth(
    frame_T	*topfrp,
    win_T	*next_curwin)	// use p_wh and p_wiw for next_curwin
{
    frame_T	*frp;
    પૂર્ણાંક		m, n;

    જો (topfrp->fr_win != ખાલી)
    {
	જો (topfrp->fr_win == next_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_width;
	નહીંતર
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_width;
	    // Current window is minimal one column wide
	    જો (p_wmw == 0 && topfrp->fr_win == curwin && next_curwin == ખાલી)
		++m;
	}
    }
    નહીંતર જો (topfrp->fr_layout == FR_COL)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minwidth(frp, next_curwin);
	    જો (n > m)
		m = n;
	}
    }
    નહીંતર
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minwidth(frp, next_curwin);
    }

    પાછા m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    શૂન્ય
close_others(
    પૂર્ણાંક		message,
    પૂર્ણાંક		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    પૂર્ણાંક		r;

    જો (one_window())
    {
	જો (message && !autocmd_busy)
	    સંદેશ(_(m_onlyone));
	પાછા;
    }

    // Be very careful here: autocommands may change the window layout.
    માટે (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_next;

	// autocommands messed this one up
	જો (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
	}

	જો (wp == curwin)		// don't close current window
	    ચાલુ;

	// autoccommands messed this one up
	જો (!buf_valid(wp->w_buffer) && win_valid(wp))
	{
	    wp->w_buffer = ખાલી;
	    win_close(wp, 0);
	    ચાલુ;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_buffer, forceit);
	જો (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    ચાલુ;
	}
	જો (!r)
	{
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    જો (message && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_changed(wp->w_buffer, FALSE);
		જો (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    ચાલુ;
		}
	    }
	    જો (bufIsChanged(wp->w_buffer))
#endif
		ચાલુ;
	}
	win_close(wp, !buf_hide(wp->w_buffer) && !bufIsChanged(wp->w_buffer));
    }

    જો (message && !ONE_WINDOW)
	emsg(_(e_other_window_contains_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    શૂન્ય
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
સ્થિર પૂર્ણાંક command_frame_height = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    શૂન્ય
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
    પૂર્ણાંક
win_alloc_first(શૂન્ય)
{
    જો (win_alloc_firstwin(ખાલી) == FAIL)
	પાછા FAIL;

    first_tabpage = alloc_tabpage();
    જો (first_tabpage == ખાલી)
	પાછા FAIL;
    curtab = first_tabpage;
    unuse_tabpage(first_tabpage);

    પાછા OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(શૂન્ય)
{
    win_T *wp;

    wp = win_alloc(ખાલી, TRUE);
    જો (wp == ખાલી)
	પાછા ખાલી;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_frame(wp);
    પાછા wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    શૂન્ય
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
    સ્થિર પૂર્ણાંક
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(ખાલી, FALSE);
    જો (curwin == ખાલી)
	પાછા FAIL;
    જો (oldwin == ખાલી)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(ખાલી, ખાલી, 1L, BLN_LISTED);
	જો (curwin == ખાલી || curbuf == ખાલી)
	    પાછા FAIL;
	curwin->w_buffer = curbuf;
#ifdef FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#endif
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    નહીંતર
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_frame(curwin);
    જો (curwin->w_frame == ખાલી)
	પાછા FAIL;
    topframe = curwin->w_frame;
    topframe->fr_width = COLUMNS_WITHOUT_TPL();
    topframe->fr_height = Rows - p_ch;

    પાછા OK;
}

/*
 * Create a frame for window "wp".
 */
    સ્થિર શૂન્ય
new_frame(win_T *wp)
{
    frame_T *frp = ALLOC_CLEAR_ONE(frame_T);

    wp->w_frame = frp;
    જો (frp == ખાલી)
	પાછા;
    frp->fr_layout = FR_LEAF;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    શૂન્ય
win_init_size(શૂન્ય)
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
    સ્થિર tabpage_T *
alloc_tabpage(શૂન્ય)
{
    tabpage_T	*tp;
#ifdef FEAT_GUI
    પૂર્ણાંક		i;
#endif


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    જો (tp == ખાલી)
	પાછા ખાલી;

#ifdef FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    જો (tp->tp_vars == ખાલી)
    {
	vim_free(tp);
	પાછા ખાલી;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#endif

#ifdef FEAT_GUI
    માટે (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#endif
#ifdef FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#endif
    tp->tp_ch_used = p_ch;

    પાછા tp;
}

    શૂન્ય
free_tabpage(tabpage_T *tp)
{
    પૂર્ણાંક idx;

#ifdef FEAT_DIFF
    diff_clear(tp);
#endif
#ifdef FEAT_PROP_POPUP
    જ્યારે (tp->tp_first_popupwin != ખાલી)
	popup_close_tabpage(tp, tp->tp_first_popupwin->w_id, TRUE);
#endif
    માટે (idx = 0; idx < SNAP_ગણતરી; ++idx)
	clear_snapshot(tp, idx);
#ifdef FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#endif

    જો (tp == lastused_tabpage)
	lastused_tabpage = ખાલી;
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
    પૂર્ણાંક
win_new_tabpage(પૂર્ણાંક after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    પૂર્ણાંક		n;
#જો defined(FEAT_TABPANEL)
    પૂર્ણાંક prev_columns = COLUMNS_WITHOUT_TPL();
#endif

    જો (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	પાછા FAIL;
    }
    જો (window_layout_locked(CMD_tabnew))
	પાછા FAIL;

    newtp = alloc_tabpage();
    જો (newtp == ખાલી)
	પાછા FAIL;

    // Remember the current windows in this Tab page.
    જો (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	પાછા FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == ખાલી)
				    ? ખાલી : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    જો (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	જો (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_next = first_tabpage;
	    first_tabpage = newtp;
	}
	નહીંતર
	{
	    જો (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		માટે (tp = first_tabpage; tp->tp_next != ખાલી
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

#જો defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#endif
#ifdef FEAT_JOB_CHANNEL
	entering_window(curwin);
#endif
#જો defined(FEAT_TABPANEL)
	જો (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    પૂર્ણાંક		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		જો (tp2 != curtab)
		{
		    frame_new_width(tp2->tp_topframe, w, FALSE, TRUE);
		    જો (!frame_check_width(tp2->tp_topframe, w))
			frame_new_width(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#endif
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, ખાલી, ખાલી, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, ખાલી, ખાલી, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, ખાલી, ખાલી, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, ખાલી, ખાલી, FALSE, curbuf);
	પાછા OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    પાછા FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    સ્થિર પૂર્ણાંક
may_open_tabpage(શૂન્ય)
{
    પૂર્ણાંક		n = (cmdmod.cmod_tab == 0)
				       ? postponed_split_tab : cmdmod.cmod_tab;

    જો (n == 0)
	પાછા FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_split_tab = 0;
    પાછા win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    પૂર્ણાંક
make_tabpages(પૂર્ણાંક maxcount)
{
    પૂર્ણાંક		ગણતરી = maxcount;
    પૂર્ણાંક		todo;

    // Limit to 'tabpagemax' tabs.
    જો (ગણતરી > p_tpm)
	ગણતરી = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    માટે (todo = ગણતરી - 1; todo > 0; --todo)
	જો (win_new_tabpage(0) == FAIL)
	    તોડો;

    unblock_autocmds();

    // return actual number of tab pages
    પાછા (ગણતરી - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    પૂર્ણાંક
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	જો (tp == tpc)
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    પૂર્ણાંક
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	જો (tp == tpc)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		જો (win_valid_any_tab(wp))
		    પાછા TRUE;
	    }
	    પાછા FALSE;
	}
    }
    // shouldn't happen
    પાછા FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    શૂન્ય
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    જો (tab == first_tabpage)
    {
	first_tabpage = tab->tp_next;
	ptp = first_tabpage;
    }
    નહીંતર
    {
	માટે (ptp = first_tabpage; ptp != ખાલી && ptp->tp_next != tab;
							    ptp = ptp->tp_next)
	    ;
	પ્રતિજ્ઞા(ptp != ખાલી);
	ptp->tp_next = tab->tp_next;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
find_tabpage(પૂર્ણાંક n)
{
    tabpage_T	*tp;
    પૂર્ણાંક		i = 1;

    જો (n == 0)
	પાછા curtab;

    માટે (tp = first_tabpage; tp != ખાલી && i != n; tp = tp->tp_next)
	++i;
    પાછા tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    પૂર્ણાંક
tabpage_index(tabpage_T *ftp)
{
    પૂર્ણાંક		i = 1;
    tabpage_T	*tp;

    માટે (tp = first_tabpage; tp != ખાલી && tp != ftp; tp = tp->tp_next)
	++i;
    પાછા i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    સ્થિર પૂર્ણાંક
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    પૂર્ણાંક		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ifdef FEAT_JOB_CHANNEL
    leaving_window(curwin);
#endif
    reset_VIsual_and_resel();	// stop Visual mode
    જો (trigger_leave_autocmds)
    {
	જો (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ખાલી, ખાલી, FALSE, curbuf);
	    જો (curtab != tp)
		પાછા FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, ખાલી, ખાલી, FALSE, curbuf);
	જો (curtab != tp)
	    પાછા FAIL;
	apply_autocmds(EVENT_TABLEAVE, ખાલી, ખાલી, FALSE, curbuf);
	જો (curtab != tp)
	    પાછા FAIL;
    }

    reset_dragwin();
#જો defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    જો (gui.in_use)
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
    જો (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_width;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = ખાલી;
    lastwin = ખાલી;
    પાછા OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    સ્થિર શૂન્ય
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    પૂર્ણાંક		trigger_enter_autocmds,
    પૂર્ણાંક		trigger_leave_autocmds)
{
    પૂર્ણાંક		old_off = tp->tp_firstwin->w_winrow;
    win_T	*next_prevwin = tp->tp_prevwin;
    tabpage_T	*last_tab = curtab;

    use_tabpage(tp);

    જો (p_ch != curtab->tp_ch_used)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	પૂર્ણાંક new_ch = curtab->tp_ch_used;
	curtab->tp_ch_used = p_ch;
	command_frame_height = FALSE;
	set_option_કિંમત((char_u *)"cmdheight", new_ch, ખાલી, 0);
	command_frame_height = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (શૂન્ય)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
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
    જો (curtab->tp_old_Rows != ROWS_AVAIL || (old_off != firstwin->w_winrow
#ifdef FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#endif
		))
	shell_new_rows();
    જો (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	જો (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_width;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	નહીંતર
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = last_tab;

#જો defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    જો (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, ખાલી, ખાલી, FALSE, curbuf);
	જો (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, ખાલી, ખાલી, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    શૂન્ય
goto_tabpage(પૂર્ણાંક n)
{
    tabpage_T	*tp = ખાલી;  // shut up compiler
    tabpage_T	*ttp;
    પૂર્ણાંક		i;

    જો (text_locked())
    {
	// Not allowed when editing the command line.
	text_locked_સંદેશ();
	પાછા;
    }

    // If there is only one it can't work.
    જો (first_tabpage->tp_next == ખાલી)
    {
	જો (n > 1)
	    beep_flush();
	પાછા;
    }

    જો (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	જો (curtab->tp_next == ખાલી)
	    tp = first_tabpage;
	નહીંતર
	    tp = curtab->tp_next;
    }
    નહીંતર જો (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	માટે (i = n; i < 0; ++i)
	{
	    માટે (tp = first_tabpage; tp->tp_next != ttp && tp->tp_next != ખાલી;
		    tp = tp->tp_next)
		;
	    ttp = tp;
	}
    }
    નહીંતર જો (n == 9999)
    {
	// Go to last tab page.
	માટે (tp = first_tabpage; tp->tp_next != ખાલી; tp = tp->tp_next)
	    ;
    }
    નહીંતર
    {
	// Go to tab page "n".
	tp = find_tabpage(n);
	જો (tp == ખાલી)
	{
	    beep_flush();
	    પાછા;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ifdef FEAT_GUI_TABLINE
    જો (gui_use_tabline())
	gui_mch_set_curtab(tabpage_index(curtab));
#endif
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    શૂન્ય
goto_tabpage_tp(
    tabpage_T	*tp,
    પૂર્ણાંક		trigger_enter_autocmds,
    પૂર્ણાંક		trigger_leave_autocmds)
{
    જો (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    set_keep_સંદેશ(ખાલી, 0);

    skip_win_fix_scroll = TRUE;
    જો (tp != curtab && leave_tabpage(tp->tp_curwin->w_buffer,
					trigger_leave_autocmds) == OK)
    {
	જો (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	નહીંતર
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    પૂર્ણાંક
goto_tabpage_lastused(શૂન્ય)
{
    જો (!valid_tabpage(lastused_tabpage))
	પાછા FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    પાછા OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    શૂન્ય
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    જો (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ifdef FEAT_GUI_TABLINE
	જો (gui_use_tabline())
	    gui_mch_set_curtab(tabpage_index(curtab));
#endif
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    શૂન્ય
tabpage_move(પૂર્ણાંક nr)
{
    પૂર્ણાંક		n = 1;
    tabpage_T	*tp, *tp_dst;

    જો (first_tabpage->tp_next == ખાલી)
	પાછા;

    જો (tabpage_move_disallowed)
	પાછા;

    માટે (tp = first_tabpage; tp->tp_next != ખાલી && n < nr; tp = tp->tp_next)
	++n;

    જો (tp == curtab || (nr > 0 && tp->tp_next != ખાલી
						    && tp->tp_next == curtab))
	પાછા;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    જો (curtab == first_tabpage)
	first_tabpage = curtab->tp_next;
    નહીંતર
    {
	FOR_ALL_TABPAGES(tp)
	    જો (tp->tp_next == curtab)
		તોડો;
	જો (tp == ખાલી)	// "cannot happen"
	    પાછા;
	tp->tp_next = curtab->tp_next;
    }

    // Re-insert it at the specified position.
    જો (nr <= 0)
    {
	curtab->tp_next = first_tabpage;
	first_tabpage = curtab;
    }
    નહીંતર
    {
	curtab->tp_next = tp_dst->tp_next;
	tp_dst->tp_next = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#જો defined(FEAT_TABPANEL)
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
    શૂન્ય
win_goto(win_T *wp)
{
#ifdef FEAT_CONCEAL
    win_T	*owp = curwin;
#endif

#ifdef FEAT_PROP_POPUP
    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;
    જો (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_to_enter_popup_window));
	પાછા;
    }
#endif
    જો (text_or_buf_locked())
    {
	beep_flush();
	પાછા;
    }

    જો (wp->w_buffer != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    નહીંતર જો (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    જો (!win_valid(wp))
	પાછા;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    win_enter(wp, TRUE);

#ifdef FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    જો (win_valid(owp) && owp->w_p_cole > 0 && !સંદેશ_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    જો (curwin->w_p_cole > 0 && !સંદેશ_scrolled)
	need_cursor_લીટી_redraw = TRUE;
#endif
}

#જો defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_find_nr(પૂર્ણાંક winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	જો (--winnr == 0)
	    તોડો;
    પાછા wp;
}
#endif

#જો defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_find_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    જો (wp == win)
		પાછા tp;
    પાછા ખાલી;
}
#endif

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, પૂર્ણાંક up, long ગણતરી)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    જો (popup_is_popup(wp))
	// popups don't have neighbors.
	પાછા ખાલી;
#endif
    foundfr = wp->w_frame;
    જ્યારે (ગણતરી--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	માટે (;;)
	{
	    જો (fr == tp->tp_topframe)
		જાઓ end;
	    જો (up)
		nfr = fr->fr_prev;
	    નહીંતર
		nfr = fr->fr_next;
	    જો (fr->fr_parent->fr_layout == FR_COL && nfr != ખાલી)
		તોડો;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	માટે (;;)
	{
	    જો (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		તોડો;
	    }
	    fr = nfr->fr_child;
	    જો (nfr->fr_layout == FR_ROW)
	    {
		// Find the frame at the cursor row.
		જ્યારે (fr->fr_next != ખાલી
			&& frame2win(fr)->w_wincol + fr->fr_width
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_next;
	    }
	    જો (nfr->fr_layout == FR_COL && up)
		જ્યારે (fr->fr_next != ખાલી)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    પાછા foundfr != ખાલી ? foundfr->fr_win : ખાલી;
}

/*
 * Move to window above or below "count" times.
 */
    સ્થિર શૂન્ય
win_goto_ver(
    પૂર્ણાંક		up,		// TRUE to go to win above
    long	ગણતરી)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    જો (ERROR_IF_TERM_POPUP_WINDOW)
	પાછા;
#endif
    win = win_vert_neighbor(curtab, curwin, up, ગણતરી);
    જો (win != ખાલી)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, પૂર્ણાંક ડાબું, long ગણતરી)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    જો (popup_is_popup(wp))
	// popups don't have neighbors.
	પાછા ખાલી;
#endif
    foundfr = wp->w_frame;
    જ્યારે (ગણતરી--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	માટે (;;)
	{
	    જો (fr == tp->tp_topframe)
		જાઓ end;
	    જો (ડાબું)
		nfr = fr->fr_prev;
	    નહીંતર
		nfr = fr->fr_next;
	    જો (fr->fr_parent->fr_layout == FR_ROW && nfr != ખાલી)
		તોડો;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	માટે (;;)
	{
	    જો (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		તોડો;
	    }
	    fr = nfr->fr_child;
	    જો (nfr->fr_layout == FR_COL)
	    {
		// Find the frame at the cursor row.
		જ્યારે (fr->fr_next != ખાલી
			&& frame2win(fr)->w_winrow + fr->fr_height
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_next;
	    }
	    જો (nfr->fr_layout == FR_ROW && ડાબું)
		જ્યારે (fr->fr_next != ખાલી)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    પાછા foundfr != ખાલી ? foundfr->fr_win : ખાલી;
}

/*
 * Move to left or right window.
 */
    સ્થિર શૂન્ય
win_goto_hor(
    પૂર્ણાંક		ડાબું,		// TRUE to go to left win
    long	ગણતરી)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    જો (ERROR_IF_TERM_POPUP_WINDOW)
	પાછા;
#endif
    win = win_horz_neighbor(curtab, curwin, ડાબું, ગણતરી);
    જો (win != ખાલી)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    શૂન્ય
win_enter(win_T *wp, પૂર્ણાંક undo_sync)
{
    (શૂન્ય)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    શૂન્ય
win_fix_current_dir(શૂન્ય)
{
    જો (curwin->w_localdir != ખાલી || curtab->tp_localdir != ખાલી)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	જો (globaldir == ખાલી)
	{
	    char_u	cwd[MAXPATHL];

	    જો (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	જો (curwin->w_localdir != ખાલી)
	    dirname = curwin->w_localdir;
	નહીંતર
	    dirname = curtab->tp_localdir;

	જો (mch_chdir((અક્ષર *)dirname) == 0)
	{
	    last_chdir_reason = ખાલી;
	    shorten_fnames(TRUE);
	}
    }
    નહીંતર જો (globaldir != ખાલી)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((અક્ષર *)globaldir);
	VIM_CLEAR(globaldir);
	last_chdir_reason = ખાલી;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    સ્થિર પૂર્ણાંક
win_enter_ext(win_T *wp, પૂર્ણાંક flags)
{
    પૂર્ણાંક		other_buffer = FALSE;
    પૂર્ણાંક		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    પૂર્ણાંક		did_decrement = FALSE;

    જો (wp == curwin && curwin_invalid == 0)	// nothing to do
	પાછા FALSE;

#ifdef FEAT_JOB_CHANNEL
    જો (curwin_invalid == 0)
	leaving_window(curwin);
#endif

    જો (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	જો (wp->w_buffer != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ખાલી, ખાલી, FALSE, curbuf);
	    other_buffer = TRUE;
	    જો (!win_valid(wp))
		પાછા FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, ખાલી, ખાલી, FALSE, curbuf);
	જો (!win_valid(wp))
	    પાછા FALSE;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	જો (aborting())
	    પાછા FALSE;
#endif
    }

    // sync undo before leaving the current buffer
    જો ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_buffer)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    જો (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    જો (wp->w_buffer != curbuf)
	buf_copy_options(wp->w_buffer, BCO_ENTER | BCO_NOHELP);
    જો (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_status = true;
    }
    curwin = wp;
    curbuf = wp->w_buffer;
    check_cursor();
    જો (!virtual_active())
	curwin->w_cursor.coladd = 0;
    જો (*p_spk == 'c')		// assume cursor position needs updating
	changed_લીટી_abv_curs();
    નહીંતર
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ifdef MESSAGE_QUEUE
    જો (flags & WEE_ALLOW_PARSE_MESSAGES)
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
    જો (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, ખાલી, ખાલી, FALSE, curbuf);
    જો (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, ખાલી, ખાલી, FALSE, curbuf);
	જો (other_buffer)
	    apply_autocmds(EVENT_BUFENTER, ખાલી, ખાલી, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_status = true;
#ifdef FEAT_TERMINAL
    જો (bt_terminal(curwin->w_buffer))
	// terminal is likely in another mode
	redraw_mode = TRUE;
#endif
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#જો defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    જો (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    જો (curwin->w_height < p_wh && !curwin->w_p_wfh
#ifdef FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#endif
	    )
	win_setheight((પૂર્ણાંક)p_wh);
    નહીંતર જો (curwin->w_height == 0)
	win_setheight(1);

    // set window width to desired minimal value
    જો (curwin->w_width < p_wiw && !curwin->w_p_wfw)
	win_setwidth((પૂર્ણાંક)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    પાછા did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = ખાલી;

    જો (curwin->w_buffer == buf)
	wp = curwin;
    નહીંતર
	FOR_ALL_WINDOWS(wp)
	    જો (wp->w_buffer == buf)
		તોડો;
    જો (wp != ખાલી)
	win_enter(wp, FALSE);
    પાછા wp;
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

    જો (wp != ખાલી)
	પાછા wp;

    FOR_ALL_TABPAGES(tp)
	જો (tp != curtab)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		જો (wp->w_buffer == buf)
		    તોડો;
	    જો (wp != ખાલી)
	    {
		goto_tabpage_win(tp, wp);
		જો (curwin != wp)
		    wp = ખાલી;	// something went wrong
		તોડો;
	    }
	}
    પાછા wp;
}

સ્થિર પૂર્ણાંક last_win_id = LOWEST_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    સ્થિર win_T *
win_alloc(win_T *after, પૂર્ણાંક hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    જો (new_wp == ખાલી)
	પાછા ખાલી;

    જો (win_alloc_લીટીઓ(new_wp) == FAIL)
    {
	vim_free(new_wp);
	પાછા ખાલી;
    }

    new_wp->w_id = ++last_win_id;

#ifdef FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    જો (new_wp->w_vars == ખાલી)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	પાછા ખાલી;
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
    જો (!hidden)
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
    જો (gui.in_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ડાબું],
		SBAR_ડાબું, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_જમણું],
		SBAR_જમણું, new_wp);
    }
#endif
#ifdef FEAT_FOLDING
    foldInitWin(new_wp);
#endif
    unblock_autocmds();
#ifdef FEAT_શોધ_EXTRA
    new_wp->w_next_match_id = 1000;  // up to 1000 can be picked by the user
#endif
    પાછા new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    સ્થિર શૂન્ય
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    પૂર્ણાંક		i;
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
    જો (wp->w_stl_click != ખાલી)
    {
	માટે (i = 0; i < wp->w_stl_click_ગણતરી; i++)
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

	જો (prevwin == wp)
	    prevwin = ખાલી;
	FOR_ALL_TABPAGES(ttp)
	    જો (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = ખાલી;
    }
    win_free_lsize(wp);

    માટે (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    જો (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    જો (wip2 != wip && wip2->wi_win == ખાલી)
		    {
			જો (wip2->wi_next != ખાલી)
			    wip2->wi_next->wi_prev = wip2->wi_prev;
			જો (wip2->wi_prev == ખાલી)
			    buf->b_wininfo = wip2->wi_next;
			નહીંતર
			    wip2->wi_prev->wi_next = wip2->wi_next;
			free_wininfo(wip2);
			તોડો;
		    }

		wip->wi_win = ખાલી;
	    }

#ifdef FEAT_શોધ_EXTRA
    clear_matches(wp);
#endif

    free_jumplist(wp);

#ifdef FEAT_QUICKFIX
    qf_free_all(wp);
#endif

#ifdef FEAT_GUI
    જો (gui.in_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ડાબું]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_જમણું]);
    }
#endif // FEAT_GUI

#ifdef FEAT_MENU
    remove_winbar(wp);
#endif
#ifdef FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    માટે (i = 0; i < 4; ++i)
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

    જો (win_valid_any_tab(wp))
	win_remove(wp, tp);
    જો (autocmd_busy)
    {
	wp->w_next = au_pending_free_win;
	au_pending_free_win = wp;
    }
    નહીંતર
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    પૂર્ણાંક
win_unlisted(win_T *wp)
{
    પાછા is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#જો defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    શૂન્ય
win_free_popup(win_T *win)
{
    જો (win->w_buffer != ખાલી)
    {
	જો (bt_popup(win->w_buffer))
	    win_close_buffer(win, DOBUF_WIPE_REUSE, FALSE);
	નહીંતર
	    close_buffer(win, win->w_buffer, 0, FALSE, FALSE, TRUE);

	જો (win->w_buffer != ખાલી)
	    --win->w_buffer->b_nwindows;
    }
# જો defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    જો (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_frame);
    win_free(win, ખાલી);
}
#endif

/*
 * Append window "wp" in the window list after window "after".
 */
    સ્થિર શૂન્ય
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    જો (after == ખાલી)	    // after NULL is in front of the first
	before = firstwin;
    નહીંતર
	before = after->w_next;

    wp->w_next = before;
    wp->w_prev = after;
    જો (after == ખાલી)
	firstwin = wp;
    નહીંતર
	after->w_next = wp;
    જો (before == ખાલી)
	lastwin = wp;
    નહીંતર
	before->w_prev = wp;
}

/*
 * Remove a window from the window list.
 */
    શૂન્ય
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    જો (wp->w_prev != ખાલી)
	wp->w_prev->w_next = wp->w_next;
    નહીંતર જો (tp == ખાલી)
	firstwin = curtab->tp_firstwin = wp->w_next;
    નહીંતર
	tp->tp_firstwin = wp->w_next;

    જો (wp->w_next != ખાલી)
	wp->w_next->w_prev = wp->w_prev;
    નહીંતર જો (tp == ખાલી)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    નહીંતર
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    સ્થિર શૂન્ય
frame_append(frame_T *after, frame_T *frp)
{
    frp->fr_next = after->fr_next;
    after->fr_next = frp;
    જો (frp->fr_next != ખાલી)
	frp->fr_next->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    સ્થિર શૂન્ય
frame_ઉમેરો(frame_T *before, frame_T *frp)
{
    frp->fr_next = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    જો (frp->fr_prev != ખાલી)
	frp->fr_prev->fr_next = frp;
    નહીંતર
	frp->fr_parent->fr_child = frp;
}

/*
 * Remove a frame from a frame list.
 */
    સ્થિર શૂન્ય
frame_remove(frame_T *frp)
{
    જો (frp->fr_prev != ખાલી)
	frp->fr_prev->fr_next = frp->fr_next;
    નહીંતર
	frp->fr_parent->fr_child = frp->fr_next;
    જો (frp->fr_next != ખાલી)
	frp->fr_next->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    પૂર્ણાંક
win_alloc_લીટીઓ(win_T *wp)
{
    wp->w_લીટીઓ_valid = 0;
    wp->w_લીટીઓ = ALLOC_CLEAR_MULT(wline_T, Rows);
    જો (wp->w_લીટીઓ == ખાલી)
	પાછા FAIL;
    પાછા OK;
}

/*
 * free lsize arrays for a window
 */
    શૂન્ય
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    જો (wp != ખાલી)
	VIM_CLEAR(wp->w_લીટીઓ);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    શૂન્ય
shell_new_rows(શૂન્ય)
{
    પૂર્ણાંક		h = (પૂર્ણાંક)ROWS_AVAIL;

    જો (firstwin == ખાલી)	// not initialized yet
	પાછા;
    જો (h < frame_minheight(topframe, ખાલી))
	h = frame_minheight(topframe, ખાલી);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    frame_new_height(topframe, h, FALSE, TRUE, FALSE);
    જો (!frame_check_height(topframe, h))
	frame_new_height(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_used = p_ch;

    જો (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#જો defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#જો 0
    // Disabled: don't want making the screen smaller make a window larger.
    જો (p_ea)
	win_equal(curwin, FALSE, 'v');
#endif
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    શૂન્ય
shell_new_columns(શૂન્ય)
{
    જો (firstwin == ખાલી)	// not initialized yet
	પાછા;

#જો defined(FEAT_TABPANEL)
    સ્થિર પૂર્ણાંક prev_tp_width;
    સ્થિર પૂર્ણાંક prev_wincol;
    સ્થિર પૂર્ણાંક prev_fr_width;
#endif
    પૂર્ણાંક w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    frame_new_width(topframe, w, FALSE, TRUE);
    જો (!frame_check_width(topframe, w))
	frame_new_width(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#જો defined(FEAT_TABPANEL)
    પૂર્ણાંક tp_width = tabpanel_width();
    પૂર્ણાંક tp_width_changed = tp_width != prev_tp_width;
    // tabpanel width changed
    જો (tp_width_changed && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    જો (tp_width_changed
	    || (tp_width > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_width != prev_fr_width)))
    {
	screen_fill(cmdline_row, (પૂર્ણાંક)Rows, 0, (પૂર્ણાંક)Columns, ' ', ' ', 0);
    }
    prev_tp_width = tabpanel_width();
    prev_wincol = firstwin->w_wincol;
    prev_fr_width = topframe->fr_width;
#endif
    // Adjust offset for command line start column
    cmdline_col_off = firstwin->w_wincol;
    cmdline_width = topframe->fr_width;
    comp_col();
    જો (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#જો defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#જો 0
    // Disabled: don't want making the screen smaller make a window larger.
    જો (p_ea)
	win_equal(curwin, FALSE, 'h');
#endif
}

/*
 * Save the size of all windows in "gap".
 */
    શૂન્ય
win_size_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, કદ(પૂર્ણાંક), 1);
    જો (ga_grow(gap, win_ગણતરી() * 2 + 1) == FAIL)
	પાછા;

    // first entry is the total lines available for windows
    ((પૂર્ણાંક *)gap->ga_data)[gap->ga_len++] = ROWS_AVAIL - last_stl_height(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((પૂર્ણાંક *)gap->ga_data)[gap->ga_len++] =
	    wp->w_width + wp->w_vsep_width;
	((પૂર્ણાંક *)gap->ga_data)[gap->ga_len++] = wp->w_height;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    શૂન્ય
win_size_restore(garray_T *gap)
{
    win_T	*wp;
    પૂર્ણાંક		i, j;

    જો (win_ગણતરી() * 2 + 1 == gap->ga_len
	    && ((પૂર્ણાંક *)gap->ga_data)[0] == ROWS_AVAIL - last_stl_height(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	માટે (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		frame_setwidth(wp->w_frame, ((પૂર્ણાંક *)gap->ga_data)[i++]);
		win_setheight_win(((પૂર્ણાંક *)gap->ga_data)[i++], wp);
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
    શૂન્ય
win_comp_pos(શૂન્ય)
{
    પૂર્ણાંક		row = tabline_height();
    પૂર્ણાંક		col = TPL_LCOL();

    frame_comp_pos(topframe, &row, &col);
#જો defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    સ્થિર શૂન્ય
frame_comp_pos(frame_T *topfrp, પૂર્ણાંક *row, પૂર્ણાંક *col)
{
    win_T	*wp;
    frame_T	*frp;
    પૂર્ણાંક		startcol;
    પૂર્ણાંક		startrow;
    પૂર્ણાંક		h;

    wp = topfrp->fr_win;
    જો (wp != ખાલી)
    {
	જો (wp->w_winrow != *row || wp->w_wincol != *col)
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
    નહીંતર
    {
	startrow = *row;
	startcol = *col;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    જો (topfrp->fr_layout == FR_ROW)
		*row = startrow;	// all frames are at the same row
	    નહીંતર
		*col = startcol;	// all frames are at the same col
	    frame_comp_pos(frp, row, col);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    શૂન્ય
win_ensure_size(શૂન્ય)
{
    જો (curwin->w_height == 0)
	win_setheight(1);
    જો (curwin->w_width == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    શૂન્ય
win_setheight(પૂર્ણાંક height)
{
    win_setheight_win(height, curwin);
#જો defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    શૂન્ય
win_setheight_win(પૂર્ણાંક height, win_T *win)
{
    જો (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	જો (height < p_wmh)
	    height = p_wmh;
	જો (height == 0)
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
    સ્થિર શૂન્ય
frame_setheight(frame_T *curfrp, પૂર્ણાંક height)
{
    પૂર્ણાંક		room;		// total number of lines available
    પૂર્ણાંક		take;		// number of lines taken from other windows
    પૂર્ણાંક		room_cmdline;	// lines available from cmdline
    પૂર્ણાંક		run;
    frame_T	*frp;
    પૂર્ણાંક		h;
    પૂર્ણાંક		room_reserved;

    // If the height already is the desired value, nothing to do.
    જો (curfrp->fr_height == height)
	પાછા;

    જો (curfrp->fr_parent == ખાલી)
    {
	// topframe: can only change the command line height
	જો (height > 0)
	    frame_new_height(curfrp, height, FALSE, FALSE, TRUE);
    }
    નહીંતર જો (curfrp->fr_parent->fr_layout == FR_ROW)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = frame_minheight(curfrp->fr_parent, ખાલી);
	જો (height < h)
	    height = h;
	frame_setheight(curfrp->fr_parent, height);
    }
    નહીંતર
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
	માટે (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		જો (frp != curfrp
			&& frp->fr_win != ખાલી
			&& frp->fr_win->w_p_wfh)
		    room_reserved += frp->fr_height;
		room += frp->fr_height;
		જો (frp != curfrp)
		    room -= frame_minheight(frp, ખાલી);
	    }
	    જો (curfrp->fr_width != topframe->fr_width)
		room_cmdline = 0;
	    નહીંતર
	    {
		room_cmdline = Rows - p_ch - (lastwin->w_winrow
						+ VISIBLE_HEIGHT(lastwin)
						+ lastwin->w_status_height);
		જો (room_cmdline < 0)
		    room_cmdline = 0;
	    }

	    જો (height <= room + room_cmdline)
		તોડો;
	    જો (run == 2 || curfrp->fr_width == topframe->fr_width)
	    {
		height = room + room_cmdline;
		તોડો;
	    }
	    frame_setheight(curfrp->fr_parent, height
		+ frame_minheight(curfrp->fr_parent, NOWIN) - (પૂર્ણાંક)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = height - curfrp->fr_height;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	જો (height > room + room_cmdline - room_reserved)
	    room_reserved = room + room_cmdline - height;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	જો (take < 0 && room - curfrp->fr_height <= room_reserved)
	    room_reserved = 0;

	જો (take > 0 && room_cmdline > 0)
	{
	    // use lines from cmdline first
	    જો (take < room_cmdline)
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
	માટે (run = 0; run < 2; ++run)
	{
	    જો (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    નહીંતર
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    જ્યારે (frp != ખાલી && take != 0)
	    {
		h = frame_minheight(frp, ખાલી);
		જો (room_reserved > 0
			&& frp->fr_win != ખાલી
			&& frp->fr_win->w_p_wfh)
		{
		    જો (room_reserved >= frp->fr_height)
			room_reserved -= frp->fr_height;
		    નહીંતર
		    {
			જો (frp->fr_height - room_reserved > take)
			    room_reserved = frp->fr_height - take;
			take -= frp->fr_height - room_reserved;
			frame_new_height(frp, room_reserved, FALSE, FALSE,
									TRUE);
			room_reserved = 0;
		    }
		}
		નહીંતર
		{
		    જો (frp->fr_height - take < h)
		    {
			take -= frp->fr_height - h;
			frame_new_height(frp, h, FALSE, FALSE, TRUE);
		    }
		    નહીંતર
		    {
			frame_new_height(frp, frp->fr_height - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		જો (run == 0)
		    frp = frp->fr_next;
		નહીંતર
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    શૂન્ય
win_setwidth(પૂર્ણાંક width)
{
    win_setwidth_win(width, curwin);
}

    શૂન્ય
win_setwidth_win(પૂર્ણાંક width, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    જો (wp == curwin)
    {
	જો (width < p_wmw)
	    width = p_wmw;
	જો (width == 0)
	    width = 1;
    }
    નહીંતર જો (width < 0)
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
    સ્થિર શૂન્ય
frame_setwidth(frame_T *curfrp, પૂર્ણાંક width)
{
    પૂર્ણાંક		room;		// total number of lines available
    પૂર્ણાંક		take;		// number of lines taken from other windows
    પૂર્ણાંક		run;
    frame_T	*frp;
    પૂર્ણાંક		w;
    પૂર્ણાંક		room_reserved;

    // If the width already is the desired value, nothing to do.
    જો (curfrp->fr_width == width)
	પાછા;

    જો (curfrp->fr_parent == ખાલી)
	// topframe: can't change width
	પાછા;

    જો (curfrp->fr_parent->fr_layout == FR_COL)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = frame_minwidth(curfrp->fr_parent, ખાલી);
	જો (width < w)
	    width = w;
	frame_setwidth(curfrp->fr_parent, width);
    }
    નહીંતર
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	માટે (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		જો (frp != curfrp
			&& frp->fr_win != ખાલી
			&& frp->fr_win->w_p_wfw)
		    room_reserved += frp->fr_width;
		room += frp->fr_width;
		જો (frp != curfrp)
		    room -= frame_minwidth(frp, ખાલી);
	    }

	    જો (width <= room)
		તોડો;
	    જો (run == 2 || curfrp->fr_height >= ROWS_AVAIL)
	    {
		width = room;
		તોડો;
	    }
	    frame_setwidth(curfrp->fr_parent, width
		 + frame_minwidth(curfrp->fr_parent, NOWIN) - (પૂર્ણાંક)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = width - curfrp->fr_width;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	જો (width > room - room_reserved)
	    room_reserved = room - width;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	જો (take < 0 && room - curfrp->fr_width < room_reserved)
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
	માટે (run = 0; run < 2; ++run)
	{
	    જો (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    નહીંતર
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    જ્યારે (frp != ખાલી && take != 0)
	    {
		w = frame_minwidth(frp, ખાલી);
		જો (room_reserved > 0
			&& frp->fr_win != ખાલી
			&& frp->fr_win->w_p_wfw)
		{
		    જો (room_reserved >= frp->fr_width)
			room_reserved -= frp->fr_width;
		    નહીંતર
		    {
			જો (frp->fr_width - room_reserved > take)
			    room_reserved = frp->fr_width - take;
			take -= frp->fr_width - room_reserved;
			frame_new_width(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		નહીંતર
		{
		    જો (frp->fr_width - take < w)
		    {
			take -= frp->fr_width - w;
			frame_new_width(frp, w, FALSE, FALSE);
		    }
		    નહીંતર
		    {
			frame_new_width(frp, frp->fr_width - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		જો (run == 0)
		    frp = frp->fr_next;
		નહીંતર
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    શૂન્ય
win_setminheight(શૂન્ય)
{
    પૂર્ણાંક		room;
    પૂર્ણાંક		needed;
    પૂર્ણાંક		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    જ્યારે (p_wmh > 0)
    {
	room = Rows - p_ch;
	needed = min_rows_for_all_tabpages() - 1;  // 1 was added for the cmdline
	જો (room >= needed)
	    તોડો;
	--p_wmh;
	જો (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    શૂન્ય
win_setminwidth(શૂન્ય)
{
    પૂર્ણાંક		room;
    પૂર્ણાંક		needed;
    પૂર્ણાંક		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    જ્યારે (p_wmw > 0)
    {
	room = topframe->fr_width;
	needed = frame_minwidth(topframe, ખાલી);
	જો (room >= needed)
	    તોડો;
	--p_wmw;
	જો (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    શૂન્ય
win_drag_status_લીટી(win_T *dragwin, પૂર્ણાંક offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    પૂર્ણાંક		room;
    પૂર્ણાંક		up;	// if TRUE, drag status line up, otherwise down
    પૂર્ણાંક		n;

    fr = dragwin->w_frame;
    curfr = fr;
    જો (fr != topframe)		// more than one window
    {
	fr = fr->fr_parent;
	// When the parent frame is not a column of frames, its parent should
	// be.
	જો (fr->fr_layout != FR_COL)
	{
	    curfr = fr;
	    જો (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_parent;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    જ્યારે (curfr != topframe && curfr->fr_next == ખાલી)
    {
	જો (fr != topframe)
	    fr = fr->fr_parent;
	curfr = fr;
	જો (fr != topframe)
	    fr = fr->fr_parent;
    }

    જો (offset < 0) // drag up
    {
	up = TRUE;
	offset = -offset;
	// sum up the room of the current frame and above it
	જો (fr == curfr)
	{
	    // only one window
	    room = fr->fr_height - frame_minheight(fr, ખાલી);
	}
	નહીંતર
	{
	    room = 0;
	    માટે (fr = fr->fr_child; ; fr = fr->fr_next)
	    {
		room += fr->fr_height - frame_minheight(fr, ખાલી);
		જો (fr == curfr)
		    તોડો;
	    }
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    નહીંતર    // drag down
    {
	up = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	room = Rows - cmdline_row;
	જો (curfr->fr_next == ખાલી)
	    --room;
	નહીંતર
	    room -= p_ch;
	જો (room < 0)
	    room = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_height - frame_minheight(fr, ખાલી);
	fr = curfr;			// put fr at window that grows
    }

    જો (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    જો (offset <= 0)
	પાછા;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    જો (fr != ખાલી)
	frame_new_height(fr, fr->fr_height + offset, up, FALSE, TRUE);

    જો (up)
	fr = curfr;		// current frame gets smaller
    નહીંતર
	fr = curfr->fr_next;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    જ્યારે (fr != ખાલી && offset > 0)
    {
	n = frame_minheight(fr, ખાલી);
	જો (fr->fr_height - offset <= n)
	{
	    offset -= fr->fr_height - n;
	    frame_new_height(fr, n, !up, FALSE, TRUE);
	}
	નહીંતર
	{
	    frame_new_height(fr, fr->fr_height - offset, !up, FALSE, TRUE);
	    તોડો;
	}
	જો (up)
	    fr = fr->fr_prev;
	નહીંતર
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
    શૂન્ય
win_drag_vsep_લીટી(win_T *dragwin, પૂર્ણાંક offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    પૂર્ણાંક		room;
    પૂર્ણાંક		ડાબું;	// if TRUE, drag separator line left, otherwise right
    પૂર્ણાંક		n;

    fr = dragwin->w_frame;
    જો (fr == topframe)		// only one window (cannot happen?)
	પાછા;
    curfr = fr;
    fr = fr->fr_parent;
    // When the parent frame is not a row of frames, its parent should be.
    જો (fr->fr_layout != FR_ROW)
    {
	જો (fr == topframe)	// only a column of windows (cannot happen?)
	    પાછા;
	curfr = fr;
	fr = fr->fr_parent;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    જ્યારે (curfr->fr_next == ખાલી)
    {
	જો (fr == topframe)
	    તોડો;
	curfr = fr;
	fr = fr->fr_parent;
	જો (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_parent;
	}
    }

    જો (offset < 0) // drag left
    {
	ડાબું = TRUE;
	offset = -offset;
	// sum up the room of the current frame and left of it
	room = 0;
	માટે (fr = fr->fr_child; ; fr = fr->fr_next)
	{
	    room += fr->fr_width - frame_minwidth(fr, ખાલી);
	    જો (fr == curfr)
		તોડો;
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    નહીંતર    // drag right
    {
	ડાબું = FALSE;
	// sum up the room of frames right of the current one
	room = 0;
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_width - frame_minwidth(fr, ખાલી);
	fr = curfr;			// put fr at window that grows
    }

    જો (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    જો (offset <= 0)		// No room at all, quit.
	પાછા;
    જો (fr == ખાલી)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	પાછા;

    // grow frame fr by offset lines
    frame_new_width(fr, fr->fr_width + offset, ડાબું, FALSE);

    // shrink other frames: current and at the left or at the right
    જો (ડાબું)
	fr = curfr;		// current frame gets smaller
    નહીંતર
	fr = curfr->fr_next;	// next frame gets smaller

    જ્યારે (fr != ખાલી && offset > 0)
    {
	n = frame_minwidth(fr, ખાલી);
	જો (fr->fr_width - offset <= n)
	{
	    offset -= fr->fr_width - n;
	    frame_new_width(fr, n, !ડાબું, FALSE);
	}
	નહીંતર
	{
	    frame_new_width(fr, fr->fr_width - offset, !ડાબું, FALSE);
	    તોડો;
	}
	જો (ડાબું)
	    fr = fr->fr_prev;
	નહીંતર
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
    શૂન્ય
set_fraction(win_T *wp)
{
    જો (wp->w_height > 1)
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
    સ્થિર શૂન્ય
win_fix_scroll(પૂર્ણાંક resize)
{
    જો (*p_spk == 'c')
	પાછા;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	જો (wp->w_height != wp->w_prev_height)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    જો (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	    {
		પૂર્ણાંક diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_height - wp->w_prev_height);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		જો (diff > 0)
		    cursor_down_inner(wp, diff);
		નહીંતર
		    cursor_up_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_to_fraction(wp, wp->w_prev_height);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		જો (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    નહીંતર જો (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    જો (!(get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL)))
	win_fix_cursor(FALSE);
    નહીંતર જો (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    સ્થિર શૂન્ય
win_fix_cursor(પૂર્ણાંક normal)
{
    win_T	*wp = curwin;

    જો (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_buffer->b_ml.ml_લીટી_ગણતરી < wp->w_height)
	પાછા;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    long so = MIN(wp->w_height / 2, get_scrolloff_કિંમત());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_down_inner(wp, so);
    linenr_T ટોચ = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_up_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    જો (lnum > bot && (wp->w_botline - wp->w_buffer->b_ml.ml_લીટી_ગણતરી) != 1)
	nlnum = bot;
    નહીંતર જો (lnum < ટોચ && wp->w_topline != 1)
	nlnum = (so == wp->w_height / 2) ? bot : ટોચ;

    જો (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	જો (normal)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	નહીંતર  // Scroll instead when not in normal mode.
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
    શૂન્ય
win_new_height(win_T *wp, પૂર્ણાંક height)
{
    પૂર્ણાંક		prev_height = wp->w_height;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    જો (height < 0)
	height = 0;
    જો (wp->w_height == height)
	પાછા;	    // nothing to do

    જો (wp->w_height > 0)
    {
	જો (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	જો (wp->w_height != prev_height)
	    પાછા;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	જો (wp->w_wrow != wp->w_prev_fraction_row)
	    set_fraction(wp);
    }

    wp->w_height = height;
    wp->w_redr_status = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    જો (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	જો (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_to_fraction(wp, prev_height);
    }
}

    શૂન્ય
scroll_to_fraction(win_T *wp, પૂર્ણાંક prev_height)
{
    linenr_T	lnum;
    પૂર્ણાંક		sline, લીટી_size;
    પૂર્ણાંક		height = wp->w_height;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    જો (height > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (height < wp->w_buffer->b_ml.ml_લીટી_ગણતરી || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	જો (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((long)wp->w_fraction * (long)height - 1L)
							       / FRACTION_MULT;
	લીટી_size = plines_win_col(wp, lnum, (long)(wp->w_cursor.col)) - 1;
	sline = wp->w_wrow - લીટી_size;

	જો (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    પૂર્ણાંક rows = plines_win(wp, lnum, FALSE);

	    જો (sline > wp->w_height - rows)
	    {
		sline = wp->w_height - rows;
		wp->w_wrow -= rows - લીટી_size;
	    }
	}

	જો (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    પૂર્ણાંક		want_row = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = લીટી_size;
	    જો (wp->w_wrow >= wp->w_height
				       && (wp->w_width - win_col_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_width - win_col_off(wp);

		--wp->w_wrow;
		જ્યારે (wp->w_wrow > want_row)
		{
		    skipcol += wp->w_width - win_col_off(wp) + win_col_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	નહીંતર જો (sline > 0)
	{
	    જ્યારે (sline > 0 && lnum > 1)
	    {
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, ખાલી, TRUE, ખાલી);
		જો (lnum == 1)
		{
		    // first line in buffer is folded
		    લીટી_size = 1;
		    --sline;
		    તોડો;
		}
#endif
		--lnum;
#ifdef FEAT_DIFF
		જો (lnum == wp->w_topline)
		    લીટી_size = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		નહીંતર
#endif
		    લીટી_size = plines_win(wp, lnum, TRUE);
		sline -= લીટી_size;
	    }

	    જો (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, ખાલી, &lnum, TRUE, ખાલી);
#endif
		lnum++;
		wp->w_wrow -= લીટી_size + sline;
	    }
	    નહીંતર જો (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	set_topline(wp, lnum);
    }

    જો (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    જો (prev_height > 0)
	wp->w_prev_fraction_row = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    શૂન્ય
win_new_width(win_T *wp, પૂર્ણાંક width)
{
    // Should we give an error if width < 0?
    wp->w_width = width < 0 ? 0 : width;
    wp->w_લીટીઓ_valid = 0;
    changed_લીટી_abv_curs_win(wp);
    invalidate_botline_win(wp);

    જો (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_status = true;
}

    શૂન્ય
win_comp_scroll(win_T *wp)
{
#જો defined(FEAT_EVAL)
    પૂર્ણાંક old_w_p_scr = wp->w_p_scr;
#endif

    wp->w_p_scr = ((unsigned)wp->w_height >> 1);
    જો (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#જો defined(FEAT_EVAL)
    જો (wp->w_p_scr != old_w_p_scr)
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
    શૂન્ય
command_height(શૂન્ય)
{
    પૂર્ણાંક		old_p_ch = curtab->tp_ch_used;

    // Find bottom frame with width of screen.
    frame_T *frp = lastwin->w_frame;
    જ્યારે (frp->fr_width != topframe->fr_width && frp->fr_parent != ખાલી)
	frp = frp->fr_parent;

    // Avoid changing the height of a window with 'winfixheight' set.
    જ્યારે (frp->fr_prev != ખાલી && frp->fr_layout == FR_LEAF
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    જ્યારે (p_ch > old_p_ch && command_frame_height)
    {
	જો (frp == ખાલી)
	{
	    emsg(_(e_not_enough_room));
	    p_ch = old_p_ch;
	    તોડો;
	}
	પૂર્ણાંક h = MIN(p_ch - old_p_ch,
			frp->fr_height - frame_minheight(frp, ખાલી));
	frame_add_height(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    જો (p_ch < old_p_ch && command_frame_height && frp != ખાલી)
	frame_add_height(frp, (પૂર્ણાંક)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_row = Rows - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    જો (સંદેશ_scrolled == 0 && full_screen)
    {
	screen_fill(cmdline_row, (પૂર્ણાંક)Rows, 0, (પૂર્ણાંક)Columns, ' ', ' ', 0);
	સંદેશ_row = cmdline_row;
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
    સ્થિર શૂન્ય
frame_add_height(frame_T *frp, પૂર્ણાંક n)
{
    frame_new_height(frp, frp->fr_height + n, FALSE, FALSE, FALSE);
    માટે (;;)
    {
	frp = frp->fr_parent;
	જો (frp == ખાલી)
	    તોડો;
	frp->fr_height += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    શૂન્ય
last_status(
    પૂર્ણાંક		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    last_status_rec(topframe, last_stl_height(morewin) > 0);
#જો defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

    સ્થિર શૂન્ય
last_status_rec(frame_T *fr, પૂર્ણાંક statusline)
{
    frame_T	*fp;
    win_T	*wp;

    જો (fr->fr_layout == FR_LEAF)
    {
	wp = fr->fr_win;
	જો (wp->w_status_height != 0 && !statusline)
	{
	    // remove status line
	    win_new_height(wp, wp->w_height + wp->w_status_height);
	    wp->w_status_height = 0;
	    comp_col();
	}
	નહીંતર જો (wp->w_status_height == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    જ્યારે (fp->fr_height <= frame_minheight(fp, ખાલી))
	    {
		જો (fp == topframe)
		{
		    emsg(_(e_not_enough_room));
		    પાછા;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		જો (fp->fr_parent->fr_layout == FR_COL && fp->fr_prev != ખાલી)
		    fp = fp->fr_prev;
		નહીંતર
		    fp = fp->fr_parent;
	    }
	    wp->w_status_height = statusline_height(wp);
	    જો (fp != fr)
	    {
		frame_new_height(fp, fp->fr_height - wp->w_status_height,
			FALSE, FALSE, FALSE);
		frame_fix_height(wp);
		win_comp_pos();
	    }
	    નહીંતર
		win_new_height(wp, wp->w_height - wp->w_status_height);
	    comp_col();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	જો (abs(wp->w_height - wp->w_prev_height) == 1)
	    wp->w_prev_height = wp->w_height;
    }
    નહીંતર જો (fr->fr_layout == FR_ROW)
    {
	// vertically split windows, set status line for each one
	જો (!statusline)
	{
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		last_status_rec(fp, statusline);
	}
	નહીંતર
	{
	    frame_T	*fp2;
	    પૂર્ણાંક		max_stlh = 0;
	    પૂર્ણાંક		new_row_height;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		જો (fp->fr_win != ખાલી && fp->fr_win->w_status_height == 0)
		{
		    પૂર્ણાંક h = statusline_height(fp->fr_win);
		    જો (h > max_stlh)
			max_stlh = h;
		}
	    જો (max_stlh == 0)
		પાછા;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    જ્યારે (fp2->fr_height - frame_minheight(fp2, ખાલી) < max_stlh)
	    {
		જો (fp2 == topframe)
		{
		    emsg(_(e_not_enough_room));
		    પાછા;
		}
		જો (fp2->fr_parent->fr_layout == FR_COL
						&& fp2->fr_prev != ખાલી)
		    fp2 = fp2->fr_prev;
		નહીંતર
		    fp2 = fp2->fr_parent;
	    }

	    જો (fp2 != fr)
	    {
		frame_new_height(fp2, fp2->fr_height - max_stlh,
							FALSE, FALSE, FALSE);
		new_row_height = fr->fr_height + max_stlh;
	    }
	    નહીંતર
		new_row_height = fr->fr_height;

	    // Set status and content heights for all leaves.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
	    {
		જો (fp->fr_win != ખાલી)
		{
		    wp = fp->fr_win;
		    જો (wp->w_status_height == 0)
		    {
			wp->w_status_height = statusline_height(wp);
			win_new_height(wp,
				  new_row_height - wp->w_status_height);
			frame_fix_height(wp);
			comp_col();
			redraw_all_later(UPD_SOME_VALID);
			જો (abs(wp->w_height - wp->w_prev_height) == 1)
			    wp->w_prev_height = wp->w_height;
		    }
		}
		નહીંતર
		    last_status_rec(fp, statusline); // nested frames
	    }

	    fr->fr_height = new_row_height;
	    win_comp_pos();
	}
    }
    નહીંતર
    {
	// horizontally split window, set status line for last one
	માટે (fp = fr->fr_child; fp->fr_next != ખાલી; fp = fp->fr_next)
	    ;
	last_status_rec(fp, statusline);
    }
}

#જો defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    શૂન્ય
update_stl_rendered_height(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == ખાલી) ? p_stl : wp->w_p_stl;
    પૂર્ણાંક		opt_flags = (wp == ખાલી) ? 0 : OPT_LOCAL;
    પૂર્ણાંક		*height = (wp == ખાલી) ? &stl_rendered_height
						: &wp->w_stl_rendered_height;

    *height = get_stl_rendered_height(wp == ખાલી ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    સ્થિર પૂર્ણાંક
frame_find_global_stlh_rec(frame_T *frp, પૂર્ણાંક h)
{
    જો (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	જો (wp->w_height > 0 && wp->w_status_height > 0
		&& *wp->w_p_stlo == NUL)
	{
	    પૂર્ણાંક win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    જો (win_free_height - p_wmh < h)
		h = win_free_height - p_wmh;
	}
    }
    નહીંતર
    {
	માટે (frp = frp->fr_child; frp != ખાલી; frp = frp->fr_next)
	    h = frame_find_global_stlh_rec(frp, h);
    }
    પાછા h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    સ્થિર શૂન્ય
frame_change_statusline_height_rec(frame_T *frp, પૂર્ણાંક global_stlh)
{
    જો (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	જો (wp->w_height > 0 && wp->w_status_height > 0)
	{
	    પૂર્ણાંક win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    wp->w_status_height = statusline_height(wp);
	    જો (wp->w_p_stlo == ખાલી || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		જો (wp->w_status_height > global_stlh)
		    wp->w_status_height = global_stlh;
	    }
	    જો (wp->w_status_height > win_free_height - p_wmh)
		wp->w_status_height = win_free_height - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    પૂર્ણાંક	     saved_topfill = wp->w_topfill;
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
    નહીંતર
    {
	માટે (frp = frp->fr_child; frp != ખાલી; frp = frp->fr_next)
	    frame_change_statusline_height_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    પૂર્ણાંક
frame_change_statusline_height(શૂન્ય)
{
    tabpage_T	*tp;
    પૂર્ણાંક		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = frame_find_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	frame_change_statusline_height_rec(tp->tp_topframe, global_stlh);
    comp_col();
    redraw_all_later(UPD_SOME_VALID);
    પાછા global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    શૂન્ય
set_stlo_mh(પૂર્ણાંક mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
statuslineopt_changed(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    પૂર્ણાંક		l_stlo_fh = FALSE;
    પૂર્ણાંક		l_stlo_mh = 1;

    જો (stlopt != ખાલી)
	p = stlopt;
    નહીંતર
	p = wp->w_p_stlo;

    જ્યારે (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	જો (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    જો (l_stlo_mh < 1)
		પાછા FAIL;
	}
	નહીંતર જો (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	જો (*p != ',' && *p != NUL)
	    પાછા FAIL;
	જો (*p == ',')
	    ++p;
    }

    જો (wp == ખાલી)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    નહીંતર
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    પાછા OK;
}
#endif

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    પૂર્ણાંક
statusline_height(win_T *wp UNUSED)
{
    પૂર્ણાંક stl_height = 1;
#જો defined(FEAT_STL_OPT)
    bool fixed;
    પૂર્ણાંક l_stl_rendered_height;

    જો (wp != ખાલી && *wp->w_p_stlo != NUL)
    {
	stl_height = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_height = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }
    નહીંતર
    {
	stl_height = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_height = (wp != ખાલી && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    જો (!fixed)
    {
	પૂર્ણાંક has_custom_stl = (*p_stl != NUL)
	    || (wp != ખાલી && *wp->w_p_stl != NUL);
	જો (has_custom_stl)
	    stl_height = MAX(1, MIN(l_stl_rendered_height, stl_height));
	નહીંતર
	    stl_height = 1;
    }
#endif

    પાછા stl_height;
}

/*
 * Return the number of lines used by the tab page line.
 */
    પૂર્ણાંક
tabline_height(શૂન્ય)
{
#ifdef FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    જો (gui_use_tabline())
	પાછા 0;
#endif
    પસંદગી (p_stal)
    {
	સ્થિતિ 0: પાછા 0;
	સ્થિતિ 1: પાછા (first_tabpage->tp_next == ખાલી) ? 0 : 1;
    }
    પાછા 1;
}

/*
 * Return the height of the last window's statusline.
 */
    પૂર્ણાંક
last_stl_height(
    પૂર્ણાંક		morewin)	// pretend there are two or more windows
{
    પાછા (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_WINDOW)))
		? STATUS_HEIGHT : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    પૂર્ણાંક
min_rows(શૂન્ય)
{
    જો (firstwin == ખાલી)	// not initialized yet
	પાછા MIN_લીટીઓ;

    પાછા frame_minheight(curtab->tp_topframe, ખાલી) + tabline_height()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    પૂર્ણાંક
min_rows_for_all_tabpages(શૂન્ય)
{
    પૂર્ણાંક		total;
    tabpage_T	*tp;
    પૂર્ણાંક		n;

    જો (firstwin == ખાલી)	// not initialized yet
	પાછા MIN_લીટીઓ;

    total = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = frame_minheight(tp->tp_topframe, ખાલી);
	જો (total < n)
	    total = n;
    }
    total += tabline_height();
    total += MIN_CMDHEIGHT;
    પાછા total;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    પૂર્ણાંક
only_one_window(શૂન્ય)
{
    પૂર્ણાંક		ગણતરી = 0;
    win_T	*wp;

#જો defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    જો (popup_is_popup(curwin))
	પાછા FALSE;
#endif

    // If there is another tab page there always is another window.
    જો (first_tabpage->tp_next != ખાલી)
	પાછા FALSE;

    FOR_ALL_WINDOWS(wp)
	જો (wp->w_buffer != ખાલી
		&& (!((bt_help(wp->w_buffer) && !bt_help(curbuf))
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++ગણતરી;
    પાછા (ગણતરી <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    સ્થિર શૂન્ય
check_lnums_both(પૂર્ણાંક do_curwin, પૂર્ણાંક nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	જો ((do_curwin || wp != curwin) && wp->w_buffer == curbuf)
	{
	    પૂર્ણાંક need_adjust;

	    જો (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_લીટી_ગણતરી;
	    જો (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	    જો (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_લીટી_ગણતરી;
	    જો (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_લીટી_ગણતરી;
	    જો (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    શૂન્ય
check_lnums(પૂર્ણાંક do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    શૂન્ય
check_lnums_nested(પૂર્ણાંક do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    શૂન્ય
reset_lnums(શૂન્ય)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	જો (wp->w_buffer == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    જો (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    જો (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    જો (wp->w_save_cursor.w_topline_save > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
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
    પૂર્ણાંક
make_snapshot(પૂર્ણાંક idx)
{
    clear_snapshot(curtab, idx);
    જો (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	પાછા FAIL;
    }
    પાછા OK;
}

    સ્થિર પૂર્ણાંક
make_snapshot_rec(frame_T *fr, frame_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(frame_T);
    જો (*frp == ખાલી)
	પાછા FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_width = fr->fr_width;
    (*frp)->fr_height = fr->fr_height;
    જો (fr->fr_next != ખાલી)
    {
	જો (make_snapshot_rec(fr->fr_next, &((*frp)->fr_next)) == FAIL)
	    પાછા FAIL;
    }
    જો (fr->fr_child != ખાલી)
    {
	જો (make_snapshot_rec(fr->fr_child, &((*frp)->fr_child)) == FAIL)
	    પાછા FAIL;
    }
    જો (fr->fr_layout == FR_LEAF && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    પાછા OK;
}

/*
 * Remove any existing snapshot.
 */
    સ્થિર શૂન્ય
clear_snapshot(tabpage_T *tp, પૂર્ણાંક idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = ખાલી;
}

    સ્થિર શૂન્ય
clear_snapshot_rec(frame_T *fr)
{
    જો (fr == ખાલી)
	પાછા;
    clear_snapshot_rec(fr->fr_next);
    clear_snapshot_rec(fr->fr_child);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    સ્થિર win_T *
get_snapshot_curwin_rec(frame_T *ft)
{
    win_T	*wp;

    જો (ft->fr_next != ખાલી)
    {
	જો ((wp = get_snapshot_curwin_rec(ft->fr_next)) != ખાલી)
	    પાછા wp;
    }
    જો (ft->fr_child != ખાલી)
    {
	જો ((wp = get_snapshot_curwin_rec(ft->fr_child)) != ખાલી)
	    પાછા wp;
    }

    પાછા ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    સ્થિર win_T *
get_snapshot_curwin(પૂર્ણાંક idx)
{
    જો (curtab->tp_snapshot[idx] == ખાલી)
	પાછા ખાલી;

    પાછા get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    શૂન્ય
restore_snapshot(
    પૂર્ણાંક		idx,
    પૂર્ણાંક		close_curwin)	    // closing current window
{
    win_T	*wp;

    જો (curtab->tp_snapshot[idx] != ખાલી
	    && curtab->tp_snapshot[idx]->fr_width == topframe->fr_width
	    && curtab->tp_snapshot[idx]->fr_height == topframe->fr_height
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	જો (wp != ખાલી && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    સ્થિર પૂર્ણાંક
check_snapshot_rec(frame_T *sn, frame_T *fr)
{
    જો (sn->fr_layout != fr->fr_layout
	    || (sn->fr_next == ખાલી) != (fr->fr_next == ખાલી)
	    || (sn->fr_child == ખાલી) != (fr->fr_child == ખાલી)
	    || (sn->fr_next != ખાલી
		&& check_snapshot_rec(sn->fr_next, fr->fr_next) == FAIL)
	    || (sn->fr_child != ખાલી
		&& check_snapshot_rec(sn->fr_child, fr->fr_child) == FAIL)
	    || (sn->fr_win != ખાલી && !win_valid(sn->fr_win)))
	પાછા FAIL;
    પાછા OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    સ્થિર win_T *
restore_snapshot_rec(frame_T *sn, frame_T *fr)
{
    win_T	*wp = ખાલી;
    win_T	*wp2;

    fr->fr_height = sn->fr_height;
    fr->fr_width = sn->fr_width;
    જો (fr->fr_layout == FR_LEAF)
    {
	frame_new_height(fr, fr->fr_height, FALSE, FALSE, FALSE);
	frame_new_width(fr, fr->fr_width, FALSE, FALSE);
	wp = sn->fr_win;
    }
    જો (sn->fr_next != ખાલી)
    {
	wp2 = restore_snapshot_rec(sn->fr_next, fr->fr_next);
	જો (wp2 != ખાલી)
	    wp = wp2;
    }
    જો (sn->fr_child != ખાલી)
    {
	wp2 = restore_snapshot_rec(sn->fr_child, fr->fr_child);
	જો (wp2 != ખાલી)
	    wp = wp2;
    }
    પાછા wp;
}

#જો defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    પૂર્ણાંક
win_hasvertsplit(શૂન્ય)
{
    frame_T	*fr;

    જો (topframe->fr_layout == FR_ROW)
	પાછા TRUE;

    જો (topframe->fr_layout == FR_COL)
	FOR_ALL_FRAMES(fr, topframe->fr_child)
	    જો (fr->fr_layout == FR_ROW)
		પાછા TRUE;

    પાછા FALSE;
}
#endif

#જો defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    પૂર્ણાંક
get_win_સંખ્યા(win_T *wp, win_T *first_win)
{
    પૂર્ણાંક		i = 1;
    win_T	*w;

    માટે (w = first_win; w != ખાલી && w != wp; w = W_NEXT(w))
	++i;

    જો (w == ખાલી)
	પાછા 0;
    નહીંતર
	પાછા i;
}

    પૂર્ણાંક
get_tab_સંખ્યા(tabpage_T *tp)
{
    પૂર્ણાંક		i = 1;
    tabpage_T	*t;

    માટે (t = first_tabpage; t != ખાલી && t != tp; t = t->tp_next)
	++i;

    જો (t == ખાલી)
	પાછા 0;
    નહીંતર
	પાછા i;
}
#endif

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    સ્થિર પૂર્ણાંક
frame_check_height(frame_T *topfrp, પૂર્ણાંક height)
{
    frame_T *frp;

    જો (topfrp->fr_height != height)
	પાછા FALSE;

    જો (topfrp->fr_layout == FR_ROW)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    જો (frp->fr_height != height)
		પાછા FALSE;

    પાછા TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    સ્થિર પૂર્ણાંક
frame_check_width(frame_T *topfrp, પૂર્ણાંક width)
{
    frame_T *frp;

    જો (topfrp->fr_width != width)
	પાછા FALSE;

    જો (topfrp->fr_layout == FR_COL)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    જો (frp->fr_width != width)
		પાછા FALSE;

    પાછા TRUE;
}

#જો defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    સ્થિર પૂર્ણાંક
int_cmp(અચલ શૂન્ય *pa, અચલ શૂન્ય *pb)
{
    અચલ પૂર્ણાંક a = *(અચલ પૂર્ણાંક *)pa;
    અચલ પૂર્ણાંક b = *(અચલ પૂર્ણાંક *)pb;
    જો (a > b)
	પાછા 1;
    જો (a < b)
	પાછા -1;
    પાછા 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    અક્ષર *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = empty_option;
    પૂર્ણાંક		tw;
    પૂર્ણાંક		col;
    પૂર્ણાંક		ગણતરી = 0;
    પૂર્ણાંક		color_cols[256];
    પૂર્ણાંક		i;
    પૂર્ણાંક		j = 0;

    જો (wp != ખાલી && wp->w_buffer == ખાલી)
	પાછા ખાલી;  // buffer was closed

    જો (cc != ખાલી)
	s = cc;
    નહીંતર જો (wp != ખાલી)
	s = wp->w_p_cc;

    જો (wp != ખાલી)
	tw = wp->w_buffer->b_p_tw;
    નહીંતર
	// buffer-local value not set, assume zero
	tw = 0;

    જ્યારે (*s != NUL && ગણતરી < 255)
    {
	જો (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    col = (*s == '-') ? -1 : 1;
	    ++s;
	    જો (!VIM_ISDIGIT(*s))
		પાછા e_invalid_argument;
	    col = col * getdigits(&s);
	    જો (tw == 0)
		જાઓ skip;  // 'textwidth' not set, skip this item
	    col += tw;
	    જો (col < 0)
		જાઓ skip;
	}
	નહીંતર જો (VIM_ISDIGIT(*s))
	    col = getdigits(&s);
	નહીંતર
	    પાછા e_invalid_argument;
	color_cols[ગણતરી++] = col - 1;  // 1-based to 0-based
skip:
	જો (*s == NUL)
	    તોડો;
	જો (*s != ',')
	    પાછા e_invalid_argument;
	જો (*++s == NUL)
	    પાછા e_invalid_argument;  // illegal trailing comma as in "set cc=80,"
    }

    જો (wp == ખાલી)
	પાછા ખાલી;  // only parse "cc"

    vim_free(wp->w_p_cc_cols);
    જો (ગણતરી == 0)
	wp->w_p_cc_cols = ખાલી;
    નહીંતર
    {
	wp->w_p_cc_cols = ALLOC_MULT(પૂર્ણાંક, ગણતરી + 1);
	જો (wp->w_p_cc_cols != ખાલી)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_cols, ગણતરી, કદ(પૂર્ણાંક), int_cmp);

	    માટે (i = 0; i < ગણતરી; ++i)
		// skip duplicates
		જો (j == 0 || wp->w_p_cc_cols[j - 1] != color_cols[i])
		    wp->w_p_cc_cols[j++] = color_cols[i];
	    wp->w_p_cc_cols[j] = -1;  // end marker
	}
    }

    પાછા ખાલી;  // no error
}
#endif

    પૂર્ણાંક
get_last_winid(શૂન્ય)
{
    પાછા last_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   પૂર્ણાંક
win_locked(win_T *wp)
{
    પાછા wp->w_locked;
}
