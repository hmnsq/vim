/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

ساكن فراغ cmd_with_گنتی(حرف *cmd, char_u *bufp, size_t bufsize, لما Prenum);
ساكن فراغ win_init(win_T *newp, win_T *oldp, صحيح flags);
ساكن فراغ win_init_some(win_T *newp, win_T *oldp);
ساكن فراغ چوکھٹا_comp_pos(چوکھٹا_T *topfrp, صحيح *صف, صحيح *خانہ);
ساكن فراغ چوکھٹا_setheight(چوکھٹا_T *curfrp, صحيح اچائی);
ساكن فراغ چوکھٹا_setwidth(چوکھٹا_T *curfrp, صحيح چوڑائی);
ساكن فراغ win_exchange(لما);
ساكن فراغ win_rotate(صحيح, صحيح);
ساكن فراغ win_equal_rec(win_T *اگلا_curwin, صحيح current, چوکھٹا_T *topfr, صحيح dir, صحيح خانہ, صحيح صف, صحيح چوڑائی, صحيح اچائی);
ساكن فراغ trigger_winnewpre(فراغ);
ساكن فراغ trigger_winclosed(win_T *win);
ساكن win_T *win_free_mem(win_T *win, صحيح *dirp, tabpage_T *tp);
ساكن چوکھٹا_T *win_altframe(win_T *win, tabpage_T *tp);
ساكن tabpage_T *alt_tabpage(فراغ);
ساكن win_T *frame2win(چوکھٹا_T *frp);
ساكن صحيح چوکھٹا_has_win(چوکھٹا_T *frp, win_T *wp);
ساكن فراغ win_fix_scroll(صحيح resize);
ساكن فراغ win_fix_cursor(صحيح عام_5);
ساكن فراغ چوکھٹا_new_اچائی(چوکھٹا_T *topfrp, صحيح اچائی, صحيح topfirst, صحيح wfh, صحيح سیٹ_ch);
ساكن صحيح چوکھٹا_fixed_اچائی(چوکھٹا_T *frp);
ساكن صحيح چوکھٹا_fixed_چوڑائی(چوکھٹا_T *frp);
ساكن فراغ چوکھٹا_جوڑو_statusline(چوکھٹا_T *frp);
ساكن فراغ چوکھٹا_new_چوڑائی(چوکھٹا_T *topfrp, صحيح چوڑائی, صحيح leftfirst, صحيح wfw);
ساكن فراغ چوکھٹا_سیٹ_vsep(چوکھٹا_T *frp, صحيح جوڑو);
ساكن صحيح چوکھٹا_minwidth(چوکھٹا_T *topfrp, win_T *اگلا_curwin);
ساكن فراغ چوکھٹا_fix_چوڑائی(win_T *wp);
ساكن صحيح win_alloc_firstwin(win_T *oldwin);
ساكن فراغ new_چوکھٹا(win_T *wp);
ساكن tabpage_T *alloc_tabpage(فراغ);
ساكن صحيح leave_tabpage(buf_T *new_curbuf, صحيح trigger_leave_autocmds);
ساكن فراغ enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, صحيح trigger_enter_autocmds, صحيح trigger_leave_autocmds);
ساكن فراغ چوکھٹا_fix_اچائی(win_T *wp);
ساكن صحيح چوکھٹا_minheight(چوکھٹا_T *topfrp, win_T *اگلا_curwin);
ساكن صحيح چوکھٹا_wincount_وچ_اچائی(چوکھٹا_T *topfrp);
ساكن صحيح may_open_tabpage(فراغ);
ساكن صحيح win_enter_ext(win_T *wp, صحيح flags);
ساكن فراغ win_free(win_T *wp, tabpage_T *tp);
ساكن فراغ win_append(win_T *after, win_T *wp);
ساكن فراغ چوکھٹا_append(چوکھٹا_T *after, چوکھٹا_T *frp);
ساكن فراغ چوکھٹا_داخل_کرو(چوکھٹا_T *before, چوکھٹا_T *frp);
ساكن فراغ چوکھٹا_remove(چوکھٹا_T *frp);
ساكن فراغ win_goto_ver(صحيح اتے, لما گنتی);
ساكن فراغ win_goto_hor(صحيح بایاں, لما گنتی);
ساكن فراغ چوکھٹا_جوڑو_اچائی(چوکھٹا_T *frp, صحيح n);
ساكن فراغ آخری_حالت_rec(چوکھٹا_T *fr, صحيح statusline);
#إذا defined(FEAT_STL_OPT)
ساكن صحيح چوکھٹا_لبھو_global_stlh_rec(چوکھٹا_T *frp, صحيح h);
ساكن فراغ چوکھٹا_change_statusline_اچائی_rec(چوکھٹا_T *frp, صحيح global_stlh);
#جے_انت
ساكن فراغ چوکھٹا_flatten(چوکھٹا_T *frp);
ساكن فراغ winframe_restore(win_T *wp, صحيح dir, چوکھٹا_T *unflat_altfr);

ساكن صحيح make_snapshot_rec(چوکھٹا_T *fr, چوکھٹا_T **frp);
ساكن فراغ clear_snapshot(tabpage_T *tp, صحيح idx);
ساكن فراغ clear_snapshot_rec(چوکھٹا_T *fr);
ساكن صحيح check_snapshot_rec(چوکھٹا_T *sn, چوکھٹا_T *fr);
ساكن win_T *restore_snapshot_rec(چوکھٹا_T *sn, چوکھٹا_T *fr);
ساكن win_T *get_snapshot_curwin(صحيح idx);

ساكن صحيح چوکھٹا_check_اچائی(چوکھٹا_T *topfrp, صحيح اچائی);
ساكن صحيح چوکھٹا_check_چوڑائی(چوکھٹا_T *topfrp, صحيح چوڑائی);

ساكن win_T *win_alloc(win_T *after, صحيح hidden);

#تعریف_کرو NOWIN		((win_T *)-1)	// non-existing window

#تعریف_کرو صفاں_AVAIL (صفاں - p_ch - tabline_اچائی())

// flags for win_enter_ext()
#تعریف_کرو WEE_UNDO_SYNC			0x01
#تعریف_کرو WEE_CURWIN_INVALID		0x02
#تعریف_کرو WEE_TRIGGER_NEW_AUTOCMDS	0x04
#تعریف_کرو WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#تعریف_کرو WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#تعریف_کرو WEE_ALLOW_PARSE_MESSAGES	0x20

ساكن حرف *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ساكن صحيح ونڈو_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ساكن صحيح close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
ساكن صحيح چوکھٹا_locked = 0;

#إذا defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
ساكن صحيح stl_rendered_اچائی = 1;
ساكن صحيح stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
ساكن صحيح stlo_mh = حالت_اچائی_2; // "maxheight" value cache of global 'stlo'
#جے_انت

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    فراغ
کھڑکی_layout_جندرا(فراغ)
{
    ++ونڈو_disallowed;
    ++close_disallowed;
}

    فراغ
کھڑکی_layout_unlock(فراغ)
{
    --ونڈو_disallowed;
    --close_disallowed;
}

    صحيح
چوکھٹے_locked(فراغ)
{
    أرجع چوکھٹا_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    صحيح
کھڑکی_layout_locked(تعداد CMD_اشاریہ cmd)
{
    إذا (ونڈو_disallowed > 0 || close_disallowed > 0)
    {
	إذا (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_ونڈو_کھڑکی_when_closing_بفر));
	وإلا
	    emsg(_(e_not_allowed_منزل_change_کھڑکی_layout_وچ_this_autocmd));
	أرجع TRUE;
    }
    أرجع FALSE;
}

// #define WIN_DEBUG
#جے_تعریف_شدہ WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    ساكن فراغ
log_چوکھٹا_layout(چوکھٹا_T *چوکھٹا)
{
    ch_log(عدم, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    چوکھٹا->fr_layout == FR_پتی ? "LEAF"
				  : چوکھٹا->fr_layout == FR_صف ? "ROW" : "COL",
	    چوکھٹا->fr_چوڑائی,
	    چوکھٹا->fr_اچائی,
	    چوکھٹا->fr_win == عدم ? -1 : چوکھٹا->fr_win->w_چوڑائی,
	    چوکھٹا->fr_win == عدم ? -1 : چوکھٹا->fr_win->w_اچائی,
	    چوکھٹا->fr_win == عدم ? -1 : چوکھٹا->fr_win->w_id);
    إذا (چوکھٹا->fr_بچہ != عدم)
    {
	ch_log(عدم, "children");
	log_چوکھٹا_layout(چوکھٹا->fr_بچہ);
	إذا (چوکھٹا->fr_اگلا != عدم)
	    ch_log(عدم, "END of children");
    }
    إذا (چوکھٹا->fr_اگلا != عدم)
	log_چوکھٹا_layout(چوکھٹا->fr_اگلا);
}
#جے_انت

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    صحيح
check_can_سیٹ_curbuf_disabled(فراغ)
{
    إذا (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_منزل_بفر));
	أرجع FALSE;
    }
    أرجع TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    صحيح
check_can_سیٹ_curbuf_forceit(صحيح forceit)
{
    إذا (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_منزل_بفر));
	أرجع FALSE;
    }
    أرجع TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(فراغ)
{
    // In cmdwin, the alternative buffer should be used.
    أرجع is_وچ_cmdwin() && prevwin != عدم ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = عدم;

    إذا (buf == عدم)
	أرجع wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    إذا (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    إذا (wp == عدم && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    أرجع wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    فراغ
do_کھڑکی(
    صحيح		nchar,
    لما	Prenum,
    صحيح		xchar)	    // extra char from ":wincmd gx" or NUL
{
    لما	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#جے_تعریف_شدہ FEAT_لبھو_ID
    صحيح		type = لبھو_DEFINE;
    صحيح		len;
#جے_انت
    char_u	cbuf[40];

    إذا (ERROR_IF_ANY_POPUP_کھڑکی)
	أرجع;

#تعریف_کرو CHECK_CMDWIN \
    افعل { \
	إذا (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_وچ_cmdline_کھڑکی)); \
	    أرجع; \
	} \
    } طالما (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    بدل (nchar)
    {
// split current window in two parts, horizontally
    حالة 'S':
    حالة Ctrl_S:
    حالة 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		إذا (bt_quickfix(curbuf))
		    اذهب newwindow;
#جے_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#جے_انت
		(فراغ)win_ونڈو((صحيح)Prenum, 0);
		اكسر;

// split current window in two parts, vertically
    حالة Ctrl_V:
    حالة 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		إذا (bt_quickfix(curbuf))
		    اذهب newwindow;
#جے_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#جے_انت
		(فراغ)win_ونڈو((صحيح)Prenum, WSP_VERT);
		اكسر;

// split current window and edit alternate file
    حالة Ctrl_HAT:
    حالة '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		إذا (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == عدم)
		{
		    إذا (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    وإلا
			semsg(_(e_بفر_nr_not_لبھیا), Prenum);
		    اكسر;
		}

		إذا (!curbuf_locked() && win_ونڈو(0, 0) == OK)
		    (فراغ)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		اكسر;

// open new window
    حالة Ctrl_N:
    حالة 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		إذا (Prenum)
		    // window height
		    vim_snprintf((حرف *)cbuf, حجم(cbuf) - 5, "%ld", Prenum);
		وإلا
		    cbuf[0] = NUL;
#إذا defined(FEAT_QUICKFIX)
		إذا (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#جے_انت
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		اكسر;

// quit current window
    حالة Ctrl_Q:
    حالة 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_گنتی("quit", cbuf, حجم(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		اكسر;

// close current window
    حالة Ctrl_C:
    حالة 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_گنتی("close", cbuf, حجم(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		اكسر;

#إذا defined(FEAT_QUICKFIX)
// close preview window
    حالة Ctrl_Z:
    حالة 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		اكسر;

// cursor to preview window
    حالة 'P':
		FOR_ALL_WINDOWS(wp)
		    إذا (wp->w_p_pvw)
			اكسر;
		إذا (wp == عدم)
		    emsg(_(e_there_is_no_preview_کھڑکی));
		وإلا
		    win_goto(wp);
		اكسر;
#جے_انت

// close all but current window
    حالة Ctrl_O:
    حالة 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_گنتی("only", cbuf, حجم(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		اكسر;

// cursor to next window with wrap around
    حالة Ctrl_W:
    حالة 'w':
// cursor to previous window with wrap around
    حالة 'W':
		CHECK_CMDWIN;
		إذا (ONE_کھڑکی && Prenum != 1)	// just one window
		    beep_flush();
		وإلا
		{
		    إذا (Prenum)			// go to specified window
		    {
			لكل (wp = firstwin; --Prenum > 0; )
			{
			    إذا (wp->w_اگلا == عدم)
				اكسر;
			    وإلا
				wp = wp->w_اگلا;
			}
		    }
		    وإلا
		    {
			إذا (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    إذا (wp == عدم)
				wp = lastwin;	    // wrap around
			}
			وإلا			    // go to next window
			{
			    wp = curwin->w_اگلا;
			    إذا (wp == عدم)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		اكسر;

// cursor to window below
    حالة 'j':
    حالة K_تھلے:
    حالة Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		اكسر;

// cursor to window above
    حالة 'k':
    حالة K_اتے:
    حالة Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		اكسر;

// cursor to left window
    حالة 'h':
    حالة K_بایاں:
    حالة Ctrl_H:
    حالة K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		اكسر;

// cursor to right window
    حالة 'l':
    حالة K_دایاں:
    حالة Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		اكسر;

// move window to new tab page
    حالة 'T':
		CHECK_CMDWIN;
		إذا (one_کھڑکی())
		    پیغام(_(m_onlyone));
		وإلا
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    إذا (win_new_tabpage((صحيح)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			إذا (curwin == wp)
			    win_close(curwin, FALSE);
			إذا (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		اكسر;

// cursor to top-left window
    حالة 't':
    حالة Ctrl_T:
		win_goto(firstwin);
		اكسر;

// cursor to bottom-right window
    حالة 'b':
    حالة Ctrl_B:
		win_goto(lastwin);
		اكسر;

// cursor to last accessed (previous) window
    حالة 'p':
    حالة Ctrl_P:
		إذا (!win_valid(prevwin))
		    beep_flush();
		وإلا
		    win_goto(prevwin);
		اكسر;

// exchange current and next window
    حالة 'x':
    حالة Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		اكسر;

// rotate windows downwards
    حالة Ctrl_R:
    حالة 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (صحيح)Prenum1);    // downwards
		اكسر;

// rotate windows upwards
    حالة 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (صحيح)Prenum1);	    // upwards
		اكسر;

// move window to the very top/bottom/left/right
    حالة 'K':
    حالة 'J':
    حالة 'H':
    حالة 'L':
		CHECK_CMDWIN;
		إذا (ONE_کھڑکی)
		    beep_flush();
		وإلا
		{
		    صحيح dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_چوٹی : WSP_BOT);

		    (فراغ)win_splitmove(curwin, (صحيح)Prenum, dir);
		}
		اكسر;

// make all windows the same width and/or height
    حالة '=':
		{
		    صحيح mod = cmdmod.cmod_ونڈو & (WSP_VERT | WSP_HOR);
#جے_تعریف_شدہ FEAT_GUI
		    need_mouse_correct = TRUE;
#جے_انت
		    win_equal(عدم, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		اكسر;

// increase current window height
    حالة '+':
#جے_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#جے_انت
		win_setheight(curwin->w_اچائی + (صحيح)Prenum1);
		اكسر;

// decrease current window height
    حالة '-':
#جے_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#جے_انت
		win_setheight(curwin->w_اچائی - (صحيح)Prenum1);
		اكسر;

// set current window height
    حالة Ctrl__:
    حالة '_':
#جے_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#جے_انت
		win_setheight(Prenum ? (صحيح)Prenum : 9999);
		اكسر;

// increase current window width
    حالة '>':
#جے_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#جے_انت
		win_setwidth(curwin->w_چوڑائی + (صحيح)Prenum1);
		اكسر;

// decrease current window width
    حالة '<':
#جے_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#جے_انت
		win_setwidth(curwin->w_چوڑائی - (صحيح)Prenum1);
		اكسر;

// set current window width
    حالة '|':
#جے_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#جے_انت
		win_setwidth(Prenum != 0 ? (صحيح)Prenum : 9999);
		اكسر;

// jump to tag and split window if tag exists (in preview window)
#إذا defined(FEAT_QUICKFIX)
    حالة '}':
		CHECK_CMDWIN;
		إذا (Prenum)
		    g_do_tagpreview = Prenum;
		وإلا
		    g_do_tagpreview = p_pvh;
#جے_انت
		// FALLTHROUGH
    حالة ']':
    حالة Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		إذا (Prenum)
		    postponed_ونڈو = Prenum;
		وإلا
		    postponed_ونڈو = -1;
#جے_تعریف_شدہ FEAT_QUICKFIX
		إذا (nchar != '}')
		    g_do_tagpreview = 0;
#جے_انت

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_ونڈو = 0;
		اكسر;

// edit file name under cursor in a new window
    حالة 'f':
    حالة 'F':
    حالة Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		إذا (check_متن_or_curbuf_locked(عدم))
		    اكسر;

		ptr = grab_file_name(Prenum1, &lnum);
		إذا (ptr != عدم)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#جے_تعریف_شدہ FEAT_GUI
		    need_mouse_correct = TRUE;
#جے_انت
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = عدم;
		    إذا ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    إذا (wp == عدم && win_ونڈو(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			إذا (do_ecmd(0, ptr, عدم, عدم, ECMD_LASTL,
						   ECMD_HIDE, عدم) == FAIL)
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
			وإلا
			    wp = curwin;
		    }

		    إذا (wp != عدم && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		اكسر;

#جے_تعریف_شدہ FEAT_لبھو_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    حالة 'i':			    // Go to any match
    حالة Ctrl_I:
		type = لبھو_ANY;
		// FALLTHROUGH
    حالة 'd':			    // Go to definition, using 'define'
    حالة Ctrl_D:
		CHECK_CMDWIN;
		إذا ((len = لبھو_ident_under_cursor(&ptr, لبھو_IDENT)) == 0)
		    اكسر;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		إذا (ptr == عدم)
		    اكسر;

		لبھو_نمونہ_وچ_رستہ(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_ونڈو, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_سیٹ_curswant = true;
		اكسر;
#جے_انت

// Quickfix window only: view the result under the cursor in a new split.
#إذا defined(FEAT_QUICKFIX)
    حالة K_KENTER:
    حالة CAR:
		إذا (bt_quickfix(curbuf))
		    qf_view_نتیجہ(TRUE);
		اكسر;
#جے_انت

// CTRL-W g  extended commands
    حالة 'g':
    حالة Ctrl_G:
		CHECK_CMDWIN;
#جے_تعریف_شدہ USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#جے_انت
		++no_mapping;
		++allow_کلیدیں;   // no mapping for xchar, but allow key codes
		إذا (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_کلیدیں;
		(فراغ)جوڑو_منزل_showcmd(xchar);

		بدل (xchar)
		{
#إذا defined(FEAT_QUICKFIX)
		    حالة '}':
			xchar = Ctrl_RSB;
			إذا (Prenum)
			    g_do_tagpreview = Prenum;
			وإلا
			    g_do_tagpreview = p_pvh;
#جے_انت
			// FALLTHROUGH
		    حالة ']':
		    حالة Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			إذا (Prenum)
			    postponed_ونڈو = Prenum;
			وإلا
			    postponed_ونڈو = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_ونڈو = 0;
			اكسر;

		    حالة 'f':	    // CTRL-W gf: "gf" in a new tab page
		    حالة 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_اشاریہ(curtab) + 1;
			nchar = xchar;
			اذهب wingotofile;

		    حالة 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((صحيح)Prenum);
			اكسر;

		    حالة 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(صحيح)Prenum1);
			اكسر;

		    حالة TAB:	    // CTRL-W g<Tab>: go to last used tab page
			إذا (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			اكسر;

		    افتراضي:
			beep_flush();
			اكسر;
		}
		اكسر;

    افتراضي:	beep_flush();
		اكسر;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    فراغ
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    بدل (*arg)
    {
    حالة 'S':
    حالة Ctrl_S:
    حالة 's':
    حالة Ctrl_N:
    حالة 'n':
    حالة 'j':
    حالة Ctrl_J:
    حالة 'k':
    حالة Ctrl_K:
    حالة 'T':
    حالة Ctrl_R:
    حالة 'r':
    حالة 'R':
    حالة 'K':
    حالة 'J':
    حالة '+':
    حالة '-':
    حالة Ctrl__:
    حالة '_':
    حالة '|':
    حالة ']':
    حالة Ctrl_RSB:
    حالة 'g':
    حالة Ctrl_G:
    حالة Ctrl_V:
    حالة 'v':
    حالة 'h':
    حالة Ctrl_H:
    حالة 'l':
    حالة Ctrl_L:
    حالة 'H':
    حالة 'L':
    حالة '>':
    حالة '<':
#إذا defined(FEAT_QUICKFIX)
    حالة '}':
#جے_انت
    حالة 'f':
    حالة 'F':
    حالة Ctrl_F:
#جے_تعریف_شدہ FEAT_لبھو_ID
    حالة 'i':
    حالة Ctrl_I:
    حالة 'd':
    حالة Ctrl_D:
#جے_انت
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		اكسر;

    حالة Ctrl_HAT:
    حالة '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		اكسر;

    حالة Ctrl_Q:
    حالة 'q':
    حالة Ctrl_C:
    حالة 'c':
    حالة Ctrl_O:
    حالة 'o':
    حالة Ctrl_W:
    حالة 'w':
    حالة 'W':
    حالة 'x':
    حالة Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		اكسر;

#إذا defined(FEAT_QUICKFIX)
    حالة Ctrl_Z:
    حالة 'z':
    حالة 'P':
#جے_انت
    حالة 't':
    حالة Ctrl_T:
    حالة 'b':
    حالة Ctrl_B:
    حالة 'p':
    حالة Ctrl_P:
    حالة '=':
    حالة CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		اكسر;
    }
}

    ساكن فراغ
cmd_with_گنتی(
    حرف	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    لما	Prenum)
{
    إذا (Prenum > 0)
	vim_snprintf((حرف *)bufp, bufsize, "%s %ld", cmd, Prenum);
    وإلا
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    صحيح
check_ونڈو_disallowed(win_T *wp)
{
    إذا (ونڈو_disallowed > 0)
    {
	emsg(_(e_cant_ونڈو_کھڑکی_while_closing_another));
	أرجع FAIL;
    }
    إذا (wp->w_بفر->b_locked_ونڈو)
    {
	emsg(_(e_cannot_ونڈو_کھڑکی_when_closing_بفر));
	أرجع FAIL;
    }
    أرجع OK;
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
    صحيح
win_ونڈو(صحيح ناپ, صحيح flags)
{
    إذا (ERROR_IF_ANY_POPUP_کھڑکی)
	أرجع FAIL;

    إذا (check_ونڈو_disallowed(curwin) == FAIL)
	أرجع FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    إذا (may_open_tabpage() == OK)
	أرجع OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_ونڈو;
    إذا ((flags & WSP_چوٹی) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_ونڈو_topleft_and_botright_at_the_same_ویلا));
	أرجع FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    إذا (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    وإلا
	clear_snapshot(curtab, SNAP_HELP_IDX);

    إذا (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    وإلا
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    أرجع win_ونڈو_ins(ناپ, flags, عدم, 0, عدم);
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
    صحيح
win_ونڈو_ins(
    صحيح		ناپ,
    صحيح		flags,
    win_T	*new_wp,
    صحيح		dir,
    چوکھٹا_T	*منزل_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    صحيح		new_ناپ = ناپ;
    صحيح		i;
    صحيح		need_حالت = 0;
    صحيح		do_equal = FALSE;
    صحيح		needed;
    صحيح		دستیاب;
    صحيح		oldwin_اچائی = 0;
    صحيح		layout;
    چوکھٹا_T	*frp, *curfrp, *frp2, *prevfrp;
    صحيح		before;
    صحيح		minheight;
    صحيح		wmh1;
    صحيح		did_سیٹ_fraction = FALSE;
    صحيح		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    إذا (new_wp == عدم)
	trigger_winnewpre();

    إذا (flags & WSP_چوٹی)
	oldwin = firstwin;
    وإلا إذا (flags & WSP_BOT)
	oldwin = lastwin;
    وإلا
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    إذا (ONE_کھڑکی && p_ls == 1 && oldwin->w_حالت_اچائی == 0)
    {
	إذا (!(flags & WSP_FORCE_گنجائش) && VISIBLE_اچائی(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_گنجائش));
	    اذهب theend;
	}
	need_حالت = statusline_اچائی(oldwin);
    }

#جے_تعریف_شدہ FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    إذا (gui.وچ_use)
	باہرلا_flush();
#جے_انت

    إذا (flags & WSP_VERT)
    {
	صحيح	wmw1;
	صحيح	minwidth;

	layout = FR_صف;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	إذا (flags & WSP_گنجائش)
	    needed += p_wiw - wmw1;
	إذا (flags & (WSP_BOT | WSP_چوٹی))
	{
	    minwidth = چوکھٹا_minwidth(topframe, NOWIN);
	    دستیاب = topframe->fr_چوڑائی;
	    needed += minwidth;
	}
	وإلا إذا (p_ea)
	{
	    minwidth = چوکھٹا_minwidth(oldwin->w_چوکھٹا, NOWIN);
	    prevfrp = oldwin->w_چوکھٹا;
	    لكل (frp = oldwin->w_چوکھٹا->fr_ماپا; frp != عدم;
							frp = frp->fr_ماپا)
	    {
		إذا (frp->fr_layout == FR_صف)
		    FOR_ALL_چوکھٹے(frp2, frp->fr_بچہ)
			إذا (frp2 != prevfrp)
			    minwidth += چوکھٹا_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    دستیاب = topframe->fr_چوڑائی;
	    needed += minwidth;
	}
	وإلا
	{
	    minwidth = چوکھٹا_minwidth(oldwin->w_چوکھٹا, NOWIN);
	    دستیاب = oldwin->w_چوکھٹا->fr_چوڑائی;
	    needed += minwidth;
	}
	إذا (!(flags & WSP_FORCE_گنجائش) && دستیاب < needed)
	{
	    emsg(_(e_not_enough_گنجائش));
	    اذهب theend;
	}
	إذا (new_ناپ == 0)
	    new_ناپ = oldwin->w_چوڑائی / 2;
	إذا (new_ناپ > دستیاب - minwidth - 1)
	    new_ناپ = دستیاب - minwidth - 1;
	إذا (new_ناپ < wmw1)
	    new_ناپ = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	إذا (oldwin->w_چوڑائی - new_ناپ - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	إذا (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_چوڑائی + new_ناپ + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	إذا (!do_equal && p_ea && ناپ == 0 && *p_ead != 'v'
	    && oldwin->w_چوکھٹا->fr_ماپا != عدم)
	    do_equal = TRUE;
    }
    وإلا
    {
	layout = FR_خانہ;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_اچائی(curwin);
	needed = wmh1 + statusline_اچائی(oldwin);
	إذا (flags & WSP_گنجائش)
	    needed += p_wh - wmh1;
	إذا (flags & (WSP_BOT | WSP_چوٹی))
	{
	    minheight = چوکھٹا_minheight(topframe, NOWIN) + need_حالت;
	    دستیاب = topframe->fr_اچائی;
	    needed += minheight;
	}
	وإلا إذا (p_ea)
	{
	    minheight = چوکھٹا_minheight(oldwin->w_چوکھٹا, NOWIN) + need_حالت;
	    prevfrp = oldwin->w_چوکھٹا;
	    لكل (frp = oldwin->w_چوکھٹا->fr_ماپا; frp != عدم;
							frp = frp->fr_ماپا)
	    {
		إذا (frp->fr_layout == FR_خانہ)
		    FOR_ALL_چوکھٹے(frp2, frp->fr_بچہ)
			إذا (frp2 != prevfrp)
			    minheight += چوکھٹا_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    دستیاب = topframe->fr_اچائی;
	    needed += minheight;
	}
	وإلا
	{
	    minheight = چوکھٹا_minheight(oldwin->w_چوکھٹا, NOWIN) + need_حالت;
	    دستیاب = oldwin->w_چوکھٹا->fr_اچائی;
	    needed += minheight;
	}
	إذا (!(flags & WSP_FORCE_گنجائش) && دستیاب < needed)
	{
	    emsg(_(e_not_enough_گنجائش));
	    اذهب theend;
	}
	oldwin_اچائی = oldwin->w_اچائی;
	إذا (need_حالت)
	{
	    oldwin->w_حالت_اچائی = statusline_اچائی(oldwin);
	    oldwin_اچائی -= oldwin->w_حالت_اچائی;
	}
	// Estimated stl height for the new window.
	صحيح new_win_stlh = need_حالت != 0 ? need_حالت
			: statusline_اچائی(new_wp != عدم ? new_wp : curwin);
	إذا (new_ناپ == 0)
	    new_ناپ = (oldwin_اچائی - new_win_stlh + 1) / 2;
	إذا (new_ناپ > دستیاب - minheight - new_win_stlh)
	    new_ناپ = دستیاب - minheight - new_win_stlh;
	إذا (new_ناپ < wmh1)
	    new_ناپ = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	إذا (oldwin_اچائی - new_ناپ - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	إذا (oldwin->w_p_wfh)
	{
	    // Set w_fraction now so that the cursor keeps the same relative
	    // vertical position using the old height.
	    سیٹ_fraction(oldwin);
	    did_سیٹ_fraction = TRUE;

	    win_setheight_win(oldwin->w_اچائی + new_ناپ
		    + statusline_اچائی(oldwin), oldwin);
	    // w_height now excludes the status line
	    oldwin_اچائی = oldwin->w_اچائی;
	}

	// Make all windows the same height if 'equalalways' is set, 'ead' is
	// not set to 'h', a size was not provided and there is a parent frame.
	إذا (!do_equal && p_ea && ناپ == 0 && *p_ead != 'h'
	    && oldwin->w_چوکھٹا->fr_ماپا != عدم)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    إذا ((flags & WSP_چوٹی) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_ہیٹھاں)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	إذا (new_wp == عدم)
	    wp = win_alloc(oldwin, FALSE);
	وإلا
	    win_append(oldwin, wp);
    }
    وإلا
    {
	إذا (new_wp == عدم)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	وإلا
	    win_append(oldwin->w_prev, wp);
    }

    إذا (new_wp == عدم)
    {
	إذا (wp == عدم)
	    اذهب theend;

	new_چوکھٹا(wp);
	إذا (wp->w_چوکھٹا == عدم)
	{
	    win_free(wp, عدم);
	    اذهب theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    إذا (منزل_flatten != عدم)
	چوکھٹا_flatten(منزل_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    إذا (flags & (WSP_چوٹی | WSP_BOT))
    {
	إذا ((topframe->fr_layout == FR_خانہ && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_صف && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_بچہ;
	    إذا (flags & WSP_BOT)
		طالما (curfrp->fr_اگلا != عدم)
		    curfrp = curfrp->fr_اگلا;
	}
	وإلا
	    curfrp = topframe;
	before = (flags & WSP_چوٹی);
    }
    وإلا
    {
	curfrp = oldwin->w_چوکھٹا;
	إذا (flags & WSP_ہیٹھاں)
	    before = FALSE;
	وإلا إذا (flags & WSP_ABOVE)
	    before = TRUE;
	وإلا إذا (flags & WSP_VERT)
	    before = !p_spr;
	وإلا
	    before = !p_sb;
    }
    إذا (curfrp->fr_ماپا == عدم || curfrp->fr_ماپا->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(چوکھٹا_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_ماپا = curfrp;
	frp->fr_اگلا = عدم;
	frp->fr_prev = عدم;
	curfrp->fr_بچہ = frp;
	curfrp->fr_win = عدم;
	curfrp = frp;
	إذا (frp->fr_win != عدم)
	    oldwin->w_چوکھٹا = frp;
	وإلا
	    FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
		frp->fr_ماپا = curfrp;
    }

    إذا (new_wp == عدم)
	frp = wp->w_چوکھٹا;
    وإلا
	frp = new_wp->w_چوکھٹا;
    frp->fr_ماپا = curfrp->fr_ماپا;

    // Insert the new frame at the right place in the frame list.
    إذا (before)
	چوکھٹا_داخل_کرو(curfrp, frp);
    وإلا
	چوکھٹا_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    إذا (!did_سیٹ_fraction)
	سیٹ_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    إذا (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	إذا (need_حالت)
	{
	    win_new_اچائی(oldwin, oldwin->w_اچائی - need_حالت);
	    oldwin->w_حالت_اچائی = need_حالت;
	}
	إذا (flags & (WSP_چوٹی | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    صحيح stl_اچائی = p_ls > 0 ? statusline_اچائی(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_اچائی();
	    win_new_اچائی(wp, curfrp->fr_اچائی - stl_اچائی
							  - WINBAR_اچائی(wp));
	    wp->w_حالت_اچائی = stl_اچائی;
	}
	وإلا
	{
	    // height and row of new window is same as current window
	    wp->w_winrow = oldwin->w_winrow;
	    win_new_اچائی(wp, VISIBLE_اچائی(oldwin));
	    wp->w_حالت_اچائی = oldwin->w_حالت_اچائی;
	}
	frp->fr_اچائی = curfrp->fr_اچائی;

	// "new_size" of the current window goes to the new window, use
	// one column for the vertical separator
	win_new_چوڑائی(wp, new_ناپ);
	إذا (before)
	    wp->w_vsep_چوڑائی = 1;
	وإلا
	{
	    wp->w_vsep_چوڑائی = oldwin->w_vsep_چوڑائی;
	    oldwin->w_vsep_چوڑائی = 1;
	}
	إذا (flags & (WSP_چوٹی | WSP_BOT))
	{
	    إذا (flags & WSP_BOT)
		چوکھٹا_سیٹ_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    چوکھٹا_new_چوڑائی(curfrp, curfrp->fr_چوڑائی
		     - (new_ناپ + ((flags & WSP_چوٹی) != 0)), flags & WSP_چوٹی,
								       FALSE);
	}
	وإلا
	    win_new_چوڑائی(oldwin, oldwin->w_چوڑائی - (new_ناپ + 1));
	إذا (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_ناپ + 1;
	}
	وإلا		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_چوڑائی + 1;
	چوکھٹا_fix_چوڑائی(oldwin);
	چوکھٹا_fix_چوڑائی(wp);
    }
    وإلا
    {
	// width and column of new window is same as current window
	إذا (flags & (WSP_چوٹی | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_چوڑائی(wp, topframe->fr_چوڑائی);
	    wp->w_vsep_چوڑائی = 0;
	}
	وإلا
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_چوڑائی(wp, oldwin->w_چوڑائی);
	    wp->w_vsep_چوڑائی = oldwin->w_vsep_چوڑائی;
	}
	frp->fr_چوڑائی = curfrp->fr_چوڑائی;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_اچائی(wp, new_ناپ);
	صحيح old_حالت_اچائی = oldwin->w_حالت_اچائی;
	إذا (flags & (WSP_چوٹی | WSP_BOT))
	{
	    صحيح new_fr_اچائی = curfrp->fr_اچائی - new_ناپ
							  + WINBAR_اچائی(wp);

	    إذا (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_اچائی -= statusline_اچائی(curfrp->fr_win);
	    إذا (flags & WSP_BOT)
		چوکھٹا_جوڑو_statusline(curfrp);
	    چوکھٹا_new_اچائی(curfrp, new_fr_اچائی, flags & WSP_چوٹی, FALSE,
									FALSE);
	}
	وإلا
	    win_new_اچائی(oldwin, oldwin_اچائی - (new_ناپ
			+ statusline_اچائی(wp)));
	إذا (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_حالت_اچائی = statusline_اچائی(wp);
	    oldwin->w_winrow += wp->w_اچائی + statusline_اچائی(wp);
	}
	وإلا		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_اچائی(oldwin)
						    + statusline_اچائی(oldwin);
	    wp->w_حالت_اچائی = old_حالت_اچائی;
	    إذا (!(flags & WSP_BOT))
		oldwin->w_حالت_اچائی = statusline_اچائی(oldwin);
	}
	چوکھٹا_fix_اچائی(wp);
	چوکھٹا_fix_اچائی(oldwin);
    }

    إذا (flags & (WSP_چوٹی | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    حالت_redraw_all();

    إذا (need_حالت)
    {
	پیغام_صف = صفاں - 1;
	پیغام_خانہ = sc_خانہ;
	پیغام_clr_eos_force();	// Old command/ruler may still be there
	comp_خانہ();
	پیغام_صف = صفاں - 1;
	پیغام_خانہ = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    إذا (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    وإلا إذا (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    إذا (flags & WSP_VERT)
    {
	i = p_wiw;
	إذا (ناپ != 0)
	    p_wiw = ناپ;

#جے_تعریف_شدہ FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	إذا (gui.وچ_use)
	    gui_init_which_components(عدم);
#جے_انت
    }
    وإلا
    {
	i = p_wh;
	إذا (ناپ != 0)
	    p_wh = ناپ;
    }

    /*
     * make the new window the current window
     */
    (فراغ)win_enter_ext(wp, (new_wp == عدم ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    إذا (flags & WSP_VERT)
	p_wiw = i;
    وإلا
	p_wh = i;
    retval = OK;

theend:
    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;
    أرجع retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    ساكن فراغ
win_init(win_T *newp, win_T *oldp, صحيح flags بن_ورتیا)
{
    صحيح		i;

    newp->w_بفر = oldp->w_بفر;
#جے_تعریف_شدہ FEAT_SYN_HL
    newp->w_s = &(oldp->w_بفر->b_s);
#جے_انت
    oldp->w_بفر->b_nwindows++;
    newp->w_cursor = oldp->w_cursor;
    newp->w_valid = 0;
    newp->w_curswant = oldp->w_curswant;
    newp->w_سیٹ_curswant = oldp->w_سیٹ_curswant;
    newp->w_topline = oldp->w_topline;
#جے_تعریف_شدہ FEAT_DIFF
    newp->w_topfill = oldp->w_topfill;
#جے_انت
    newp->w_leftcol = oldp->w_leftcol;
    newp->w_pcmark = oldp->w_pcmark;
    newp->w_prev_pcmark = oldp->w_prev_pcmark;
    newp->w_alt_fnum = oldp->w_alt_fnum;
    newp->w_wrow = oldp->w_wrow;
    newp->w_fraction = oldp->w_fraction;
    newp->w_prev_fraction_صف = oldp->w_prev_fraction_صف;

    // Not sure if this is needed, but be safe
    remove_highlight_overrides(newp->w_hl);
    VIM_CLEAR(newp->w_hl);

    copy_jumplist(oldp, newp);
#جے_تعریف_شدہ FEAT_QUICKFIX
    إذا (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = عدم;
	newp->w_llist_ref = عدم;
    }
    وإلا
	copy_loclist_ڈھیر(oldp, newp);
#جے_انت
    newp->w_localdir = (oldp->w_localdir == عدم)
				    ? عدم : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == عدم)
				    ? عدم : vim_strsave(oldp->w_prevdir);

    إذا (*p_spk != 'c')
    {
	إذا (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_اچائی = oldp->w_اچائی - WINBAR_اچائی(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_اچائی(oldp);
    }

    // copy tagstack and folds
    لكل (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*ٹیگ = &newp->w_tagstack[i];
	*ٹیگ = oldp->w_tagstack[i];
	إذا (ٹیگ->tagname != عدم)
	    ٹیگ->tagname = vim_strsave(ٹیگ->tagname);
	إذا (ٹیگ->user_انکڑے != عدم)
	    ٹیگ->user_انکڑے = vim_strsave(ٹیگ->user_انکڑے);
    }
    newp->w_tagstackidx = oldp->w_tagstackidx;
    newp->w_tagstacklen = oldp->w_tagstacklen;

    // Keep same changelist position in new window.
    newp->w_changelistidx = oldp->w_changelistidx;

#جے_تعریف_شدہ FEAT_FOLDING
    copyFoldingState(oldp, newp);
#جے_انت

    win_init_some(newp, oldp);

#جے_تعریف_شدہ FEAT_TERMINAL
    {
	// Make sure to also handle highlight overrides copied over from oldp.
	bool pushed = ڈالو_highlight_overrides(newp->w_hl, newp->w_hl_len);
	إذا (newp->w_بفر->b_رکن != عدم)
	    رکن_init_default_colors(newp->w_بفر->b_رکن);
	إذا (pushed)
	    نکالو_highlight_overrides();
    }
#جے_انت
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    ساكن فراغ
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
    صحيح
win_valid_popup(win_T *win بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	إذا (wp == win)
	    أرجع TRUE;
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
	إذا (wp == win)
	    أرجع TRUE;
#جے_انت
    أرجع FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    صحيح
win_valid(win_T *win)
{
    win_T	*wp;

    إذا (win == عدم)
	أرجع FALSE;
    FOR_ALL_WINDOWS(wp)
	إذا (wp == win)
	    أرجع TRUE;
    أرجع win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_لبھو_by_id(صحيح id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_id == id)
	    أرجع wp;
#جے_تعریف_شدہ FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	إذا (wp->w_id == id)
	    أرجع wp;
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
	إذا (wp->w_id == id)
	    أرجع wp;
#جے_انت
    أرجع عدم;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    صحيح
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    إذا (win == عدم)
	أرجع FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_وچ_TAB(tp, wp)
	{
	    إذا (wp == win)
		أرجع TRUE;
	}
#جے_تعریف_شدہ FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_وچ_TAB(tp, wp)
	    إذا (wp == win)
		أرجع TRUE;
#جے_انت
    }
    أرجع win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    صحيح
win_گنتی(فراغ)
{
    win_T	*wp;
    صحيح		گنتی = 0;

    FOR_ALL_WINDOWS(wp)
	++گنتی;
    أرجع گنتی;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    صحيح
make_windows(
    صحيح		گنتی,
    صحيح		vertical)  // split windows vertically if TRUE
{
    صحيح		maxcount;
    صحيح		todo;

    إذا (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_چوڑائی + curwin->w_vsep_چوڑائی
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    وإلا
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_اچائی(curwin) + curwin->w_حالت_اچائی
			- (p_wh - p_wmh)) / (p_wmh + statusline_اچائی(curwin));
    }

    إذا (maxcount < 2)
	maxcount = 2;
    إذا (گنتی > maxcount)
	گنتی = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    إذا (گنتی > 1)
	آخری_حالت(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    بلاک_autocmds();

    // todo is number of windows left to create
    لكل (todo = گنتی - 1; todo > 0; --todo)
	إذا (vertical)
	{
	    إذا (win_ونڈو(curwin->w_چوڑائی - (curwin->w_چوڑائی - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		اكسر;
	}
	وإلا
	{
	    إذا (win_ونڈو(curwin->w_اچائی - (curwin->w_اچائی - todo
			    * statusline_اچائی(curwin)) / (todo + 1)
			- statusline_اچائی(curwin), WSP_ABOVE) == FAIL)
		اكسر;
	}

    unblock_autocmds();

    // return actual number of windows
    أرجع (گنتی - todo);
}

/*
 * Exchange current and next window
 */
    ساكن فراغ
win_exchange(لما Prenum)
{
    چوکھٹا_T	*frp;
    چوکھٹا_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    صحيح		عارضی;

    إذا (ERROR_IF_ANY_POPUP_کھڑکی)
	أرجع;
    إذا (ONE_کھڑکی)	    // just one window
    {
	beep_flush();
	أرجع;
    }
    إذا (متن_or_buf_locked())
    {
	beep_flush();
	أرجع;
    }

#جے_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#جے_انت

    /*
     * find window to exchange with
     */
    إذا (Prenum)
    {
	frp = curwin->w_چوکھٹا->fr_ماپا->fr_بچہ;
	طالما (frp != عدم && --Prenum > 0)
	    frp = frp->fr_اگلا;
    }
    وإلا إذا (curwin->w_چوکھٹا->fr_اگلا != عدم)	// Swap with next
	frp = curwin->w_چوکھٹا->fr_اگلا;
    وإلا    // Swap last window in row/col with previous
	frp = curwin->w_چوکھٹا->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    إذا (frp == عدم || frp->fr_win == عدم || frp->fr_win == curwin)
	أرجع;
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
    frp2 = curwin->w_چوکھٹا->fr_prev;
    إذا (wp->w_prev != curwin)
    {
	win_remove(curwin, عدم);
	چوکھٹا_remove(curwin->w_چوکھٹا);
	win_append(wp->w_prev, curwin);
	چوکھٹا_داخل_کرو(frp, curwin->w_چوکھٹا);
    }
    إذا (wp != wp2)
    {
	win_remove(wp, عدم);
	چوکھٹا_remove(wp->w_چوکھٹا);
	win_append(wp2, wp);
	إذا (frp2 == عدم)
	    چوکھٹا_داخل_کرو(wp->w_چوکھٹا->fr_ماپا->fr_بچہ, wp->w_چوکھٹا);
	وإلا
	    چوکھٹا_append(frp2, wp->w_چوکھٹا);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    عارضی = curwin->w_حالت_اچائی;
    إذا (عارضی != wp->w_حالت_اچائی)
    {
	win_new_اچائی(curwin, curwin->w_اچائی + عارضی - wp->w_حالت_اچائی);
	win_new_اچائی(wp, wp->w_اچائی + wp->w_حالت_اچائی - عارضی);
    }
    curwin->w_حالت_اچائی = wp->w_حالت_اچائی;
    wp->w_حالت_اچائی = عارضی;
    عارضی = curwin->w_vsep_چوڑائی;
    curwin->w_vsep_چوڑائی = wp->w_vsep_چوڑائی;
    wp->w_vsep_چوڑائی = عارضی;

    چوکھٹا_fix_اچائی(curwin);
    چوکھٹا_fix_اچائی(wp);
    چوکھٹا_fix_چوڑائی(curwin);
    چوکھٹا_fix_چوڑائی(wp);

    win_comp_pos();		// recompute window positions
#إذا defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اچائی();
#جے_انت

    إذا (wp->w_بفر != curbuf)
	reset_VIsual_and_resel();
    وإلا إذا (VIsual_سرگرم)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    ساكن فراغ
win_rotate(صحيح upwards, صحيح گنتی)
{
    win_T	*wp1;
    win_T	*wp2;
    چوکھٹا_T	*frp;
    صحيح		n;

    إذا (ONE_کھڑکی)		// nothing to do
    {
	beep_flush();
	أرجع;
    }

#جے_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#جے_انت

    // Check if all frames in this row/col have one window.
    FOR_ALL_چوکھٹے(frp, curwin->w_چوکھٹا->fr_ماپا->fr_بچہ)
	إذا (frp->fr_win == عدم)
	{
	    emsg(_(e_cannot_rotate_when_another_کھڑکی_is_ونڈو));
	    أرجع;
	}

    طالما (گنتی--)
    {
	إذا (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_چوکھٹا->fr_ماپا->fr_بچہ;
	    wp1 = frp->fr_win;
	    win_remove(wp1, عدم);
	    چوکھٹا_remove(frp);

	    // find last frame and append removed window/frame after it
	    لكل ( ; frp->fr_اگلا != عدم; frp = frp->fr_اگلا)
		;
	    win_append(frp->fr_win, wp1);
	    چوکھٹا_append(frp, wp1->w_چوکھٹا);

	    wp2 = frp->fr_win;		// previously last window
	}
	وإلا			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    لكل (frp = curwin->w_چوکھٹا; frp->fr_اگلا != عدم;
							   frp = frp->fr_اگلا)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, عدم);
	    چوکھٹا_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_ماپا->fr_بچہ->fr_win->w_prev, wp1);
	    چوکھٹا_داخل_کرو(frp->fr_ماپا->fr_بچہ, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_حالت_اچائی;
	إذا (n != wp1->w_حالت_اچائی)
	{
	    win_new_اچائی(wp2, wp2->w_اچائی + n - wp1->w_حالت_اچائی);
	    win_new_اچائی(wp1, wp1->w_اچائی + wp1->w_حالت_اچائی - n);
	}
	wp2->w_حالت_اچائی = wp1->w_حالت_اچائی;
	wp1->w_حالت_اچائی = n;
	چوکھٹا_fix_اچائی(wp1);
	چوکھٹا_fix_اچائی(wp2);
	n = wp2->w_vsep_چوڑائی;
	wp2->w_vsep_چوڑائی = wp1->w_vsep_چوڑائی;
	wp1->w_vsep_چوڑائی = n;
	چوکھٹا_fix_چوڑائی(wp1);
	چوکھٹا_fix_چوڑائی(wp2);

	// recompute w_winrow and w_wincol for all windows
	win_comp_pos();
    }
#إذا defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اچائی();
#جے_انت

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Move "wp" into a new split in a given direction, possibly relative to the
 * current window.
 * "wp" must be valid in the current tabpage.
 * Returns FAIL for failure, OK otherwise.
 */
    صحيح
win_splitmove(win_T *wp, صحيح ناپ, صحيح flags)
{
    صحيح		dir;
    صحيح		اچائی = wp->w_اچائی;
    چوکھٹا_T	*unflat_altfr;

    إذا (ONE_کھڑکی)
	أرجع OK;	// nothing to do
    إذا (check_ونڈو_disallowed(wp) == FAIL)
	أرجع FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, عدم, &unflat_altfr);
    win_remove(wp, عدم);
    آخری_حالت(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    إذا (win_ونڈو_ins(ناپ, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	أرجع FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    إذا (ناپ == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(اچائی, wp);
	إذا (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#إذا defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#جے_انت
    أرجع OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    فراغ
win_move_after(win_T *win1, win_T *win2)
{
    صحيح		اچائی;

    // check if the arguments are reasonable
    إذا (win1 == win2)
	أرجع;

    // check if there is something to do
    إذا (win2->w_اگلا != win1)
    {
	إذا (win1->w_چوکھٹا->fr_ماپا != win2->w_چوکھٹا->fr_ماپا)
	{
	    iemsg("Trying to move a window into another frame");
	    أرجع;
	}

	// may need to move the status line/vertical separator of the last
	//کھڑکی
	إذا (win1 == lastwin)
	{
	    اچائی = win1->w_prev->w_حالت_اچائی;
	    win1->w_prev->w_حالت_اچائی = win1->w_حالت_اچائی;
	    win1->w_حالت_اچائی = اچائی;
	    إذا (win1->w_prev->w_vsep_چوڑائی == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_چوڑائی = 0;
		win1->w_prev->w_چوکھٹا->fr_چوڑائی -= 1;
		win1->w_vsep_چوڑائی = 1;
		win1->w_چوکھٹا->fr_چوڑائی += 1;
	    }
	}
	وإلا إذا (win2 == lastwin)
	{
	    اچائی = win1->w_حالت_اچائی;
	    win1->w_حالت_اچائی = win2->w_حالت_اچائی;
	    win2->w_حالت_اچائی = اچائی;
	    إذا (win1->w_vsep_چوڑائی == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_چوڑائی = 1;
		win2->w_چوکھٹا->fr_چوڑائی += 1;
		win1->w_vsep_چوڑائی = 0;
		win1->w_چوکھٹا->fr_چوڑائی -= 1;
	    }
	}
	win_remove(win1, عدم);
	چوکھٹا_remove(win1->w_چوکھٹا);
	win_append(win2, win1);
	چوکھٹا_append(win2->w_چوکھٹا, win1->w_چوکھٹا);

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
    فراغ
win_equal(
    win_T	*اگلا_curwin,	// pointer to current window to be or NULL
    صحيح		current,	// do only frame with current window
    صحيح		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    إذا (dir == 0)
	dir = *p_ead;
    win_equal_rec(اگلا_curwin == عدم ? curwin : اگلا_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_اچائی(),
		      topframe->fr_چوڑائی, topframe->fr_اچائی);
#إذا defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اچائی();
#جے_انت
    إذا (!is_aucmd_win(اگلا_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    ساكن فراغ
win_equal_rec(
    win_T	*اگلا_curwin,	// pointer to current window to be or NULL
    صحيح		current,	// do only frame with current window
    چوکھٹا_T	*topfr,		// frame to set size off
    صحيح		dir,		// 'v', 'h' or 'b', see win_equal()
    صحيح		خانہ,		// horizontal position for frame
    صحيح		صف,		// vertical position for frame
    صحيح		چوڑائی,		// new width of frame
    صحيح		اچائی)		// new height of frame
{
    صحيح		n, m;
    صحيح		extra_sep = 0;
    صحيح		wincount, totwincount = 0;
    چوکھٹا_T	*fr;
    صحيح		اگلا_curwin_ناپ = 0;
    صحيح		گنجائش = 0;
    صحيح		new_ناپ;
    صحيح		has_اگلا_curwin = 0;
    صحيح		hnc;

    إذا (topfr->fr_layout == FR_پتی)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	إذا (topfr->fr_اچائی != اچائی || topfr->fr_win->w_winrow != صف
		|| topfr->fr_چوڑائی != چوڑائی || topfr->fr_win->w_wincol != خانہ
	   )
	{
	    topfr->fr_win->w_winrow = صف;
	    چوکھٹا_new_اچائی(topfr, اچائی, FALSE, FALSE, FALSE);
	    topfr->fr_win->w_wincol = خانہ;
	    چوکھٹا_new_چوڑائی(topfr, چوڑائی, FALSE, FALSE);
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    وإلا إذا (topfr->fr_layout == FR_صف)
    {
	topfr->fr_چوڑائی = چوڑائی;
	topfr->fr_اچائی = اچائی;

	إذا (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = چوکھٹا_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    إذا (خانہ + چوڑائی >= firstwin->w_wincol + topframe->fr_چوڑائی)
		extra_sep = 1;
	    وإلا
		extra_sep = 0;
	    totwincount = (n + extra_sep) / (p_wmw + 1);
	    has_اگلا_curwin = چوکھٹا_has_win(topfr, اگلا_curwin);

	    /*
	     * Compute width for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal width when counting p_wiw for "next_curwin".
	     */
	    m = چوکھٹا_minwidth(topfr, اگلا_curwin);
	    گنجائش = چوڑائی - m;
	    إذا (گنجائش < 0)
	    {
		اگلا_curwin_ناپ = p_wiw + گنجائش;
		گنجائش = 0;
	    }
	    وإلا
	    {
		اگلا_curwin_ناپ = -1;
		FOR_ALL_چوکھٹے(fr, topfr->fr_بچہ)
		{
		    إذا (!چوکھٹا_fixed_چوڑائی(fr))
			استمر;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = چوکھٹا_minwidth(fr, NOWIN);
		    new_ناپ = fr->fr_چوڑائی;
		    إذا (چوکھٹا_has_win(fr, اگلا_curwin))
		    {
			گنجائش += p_wiw - p_wmw;
			اگلا_curwin_ناپ = 0;
			إذا (new_ناپ < p_wiw)
			    new_ناپ = p_wiw;
		    }
		    وإلا
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_اگلا == عدم
					? extra_sep : 0)) / (p_wmw + 1);
		    گنجائش -= new_ناپ - n;
		    إذا (گنجائش < 0)
		    {
			new_ناپ += گنجائش;
			گنجائش = 0;
		    }
		    fr->fr_newwidth = new_ناپ;
		}
		إذا (اگلا_curwin_ناپ == -1)
		{
		    إذا (!has_اگلا_curwin)
			اگلا_curwin_ناپ = 0;
		    وإلا إذا (totwincount > 1
			    && (گنجائش + (totwincount - 2))
						  / (totwincount - 1) > p_wiw)
		    {
			// Can make all windows wider than 'winwidth', spread
			// the room equally.
			اگلا_curwin_ناپ = (گنجائش + p_wiw
					    + (totwincount - 1) * p_wmw
					    + (totwincount - 1)) / totwincount;
			گنجائش -= اگلا_curwin_ناپ - p_wiw;
		    }
		    وإلا
			اگلا_curwin_ناپ = p_wiw;
		}
	    }

	    إذا (has_اگلا_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_چوکھٹے(fr, topfr->fr_بچہ)
	{
	    wincount = 1;
	    إذا (fr->fr_اگلا == عدم)
		// last frame gets all that remains (avoid roundoff error)
		new_ناپ = چوڑائی;
	    وإلا إذا (dir == 'v')
		new_ناپ = fr->fr_چوڑائی;
	    وإلا إذا (چوکھٹا_fixed_چوڑائی(fr))
	    {
		new_ناپ = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    وإلا
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = چوکھٹا_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_اگلا == عدم ? extra_sep : 0))
								/ (p_wmw + 1);
		m = چوکھٹا_minwidth(fr, اگلا_curwin);
		إذا (has_اگلا_curwin)
		    hnc = چوکھٹا_has_win(fr, اگلا_curwin);
		وإلا
		    hnc = FALSE;
		إذا (hnc)	    // don't count next_curwin
		    --wincount;
		إذا (totwincount == 0)
		    new_ناپ = گنجائش;
		وإلا
		    new_ناپ = (wincount * گنجائش + ((بنانشانی)totwincount >> 1))
								/ totwincount;
		إذا (hnc)	    // add next_curwin size
		{
		    اگلا_curwin_ناپ -= p_wiw - (m - n);
		    إذا (اگلا_curwin_ناپ < 0)
			اگلا_curwin_ناپ = 0;
		    new_ناپ += اگلا_curwin_ناپ;
		    گنجائش -= new_ناپ - اگلا_curwin_ناپ;
		}
		وإلا
		    گنجائش -= new_ناپ;
		new_ناپ += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    إذا (!current || dir != 'v' || topfr->fr_ماپا != عدم
		    || (new_ناپ != fr->fr_چوڑائی)
		    || چوکھٹا_has_win(fr, اگلا_curwin))
		win_equal_rec(اگلا_curwin, current, fr, dir, خانہ, صف,
							    new_ناپ, اچائی);
	    خانہ += new_ناپ;
	    چوڑائی -= new_ناپ;
	    totwincount -= wincount;
	}
    }
    وإلا // topfr->fr_layout == FR_COL
    {
	topfr->fr_چوڑائی = چوڑائی;
	topfr->fr_اچائی = اچائی;

	إذا (dir != 'h')			// equalize frame heights
	{
	    totwincount = چوکھٹا_wincount_وچ_اچائی(topfr);
	    has_اگلا_curwin = چوکھٹا_has_win(topfr, اگلا_curwin);

	    /*
	     * Compute height for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal height when counting p_wh for "next_curwin".
	     */
	    m = چوکھٹا_minheight(topfr, اگلا_curwin);
	    گنجائش = اچائی - m;
	    إذا (گنجائش < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		اگلا_curwin_ناپ = p_wh + گنجائش;
		گنجائش = 0;
	    }
	    وإلا
	    {
		اگلا_curwin_ناپ = -1;
		FOR_ALL_چوکھٹے(fr, topfr->fr_بچہ)
		{
		    إذا (!چوکھٹا_fixed_اچائی(fr))
			استمر;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = چوکھٹا_minheight(fr, NOWIN);
		    new_ناپ = fr->fr_اچائی;
		    إذا (چوکھٹا_has_win(fr, اگلا_curwin))
		    {
			گنجائش += p_wh - p_wmh;
			اگلا_curwin_ناپ = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			إذا (new_ناپ < n + (p_wh - p_wmh))
			    new_ناپ = n + (p_wh - p_wmh);
		    }
		    وإلا
			// These windows don't use up room.
			totwincount -= چوکھٹا_wincount_وچ_اچائی(fr);
		    گنجائش -= new_ناپ - n;
		    إذا (گنجائش < 0)
		    {
			new_ناپ += گنجائش;
			گنجائش = 0;
		    }
		    fr->fr_newheight = new_ناپ;
		}
		إذا (اگلا_curwin_ناپ == -1)
		{
		    إذا (!has_اگلا_curwin)
			اگلا_curwin_ناپ = 0;
		    وإلا إذا (totwincount > 1
			    && (گنجائش + (totwincount - 2))
						   / (totwincount - 1) > p_wh)
		    {
			// can make all windows higher than 'winheight',
			// spread the room equally.
			اگلا_curwin_ناپ = (گنجائش + p_wh
					   + (totwincount - 1) * p_wmh
					   + (totwincount - 1)) / totwincount;
			گنجائش -= اگلا_curwin_ناپ - p_wh;
		    }
		    وإلا
			اگلا_curwin_ناپ = p_wh;
		}
	    }

	    إذا (has_اگلا_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_چوکھٹے(fr, topfr->fr_بچہ)
	{
	    wincount = 1;
	    إذا (fr->fr_اگلا == عدم)
		// last frame gets all that remains (avoid roundoff error)
		new_ناپ = اچائی;
	    وإلا إذا (dir == 'h')
		new_ناپ = fr->fr_اچائی;
	    وإلا إذا (چوکھٹا_fixed_اچائی(fr))
	    {
		new_ناپ = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    وإلا
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = چوکھٹا_minheight(fr, NOWIN);
		wincount = چوکھٹا_wincount_وچ_اچائی(fr);
		m = چوکھٹا_minheight(fr, اگلا_curwin);
		إذا (has_اگلا_curwin)
		    hnc = چوکھٹا_has_win(fr, اگلا_curwin);
		وإلا
		    hnc = FALSE;
		إذا (hnc)	    // don't count next_curwin
		    --wincount;
		إذا (totwincount == 0)
		    new_ناپ = گنجائش;
		وإلا
		    new_ناپ = (wincount * گنجائش + ((بنانشانی)totwincount >> 1))
								/ totwincount;
		إذا (hnc)	    // add next_curwin size
		{
		    اگلا_curwin_ناپ -= p_wh - (m - n);
		    new_ناپ += اگلا_curwin_ناپ;
		    گنجائش -= new_ناپ - اگلا_curwin_ناپ;
		}
		وإلا
		    گنجائش -= new_ناپ;
		new_ناپ += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    إذا (!current || dir != 'h' || topfr->fr_ماپا != عدم
		    || (new_ناپ != fr->fr_اچائی)
		    || چوکھٹا_has_win(fr, اگلا_curwin))
		win_equal_rec(اگلا_curwin, current, fr, dir, خانہ, صف,
							     چوڑائی, new_ناپ);
	    صف += new_ناپ;
	    اچائی -= new_ناپ;
	    totwincount -= wincount;
	}
    }
}

#إذا defined(FEAT_کم_CHANNEL)
    فراغ
leaving_کھڑکی(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    إذا (!bt_prompt(win->w_بفر) || is_aucmd_win(win))
	أرجع;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_بفر->b_prompt_داخل_کرو = restart_edit;
    إذا (restart_edit != 0 && منوال_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    إذا ((State & منوال_داخل_کرو) && !stop_داخل_کرو_منوال)
    {
	stop_داخل_کرو_منوال = TRUE;
	إذا (win->w_بفر->b_prompt_داخل_کرو == NUL)
	    win->w_بفر->b_prompt_داخل_کرو = 'A';
    }
}

    فراغ
entering_کھڑکی(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    إذا (!bt_prompt(win->w_بفر) || is_aucmd_win(win))
	أرجع;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    إذا (win->w_بفر->b_prompt_داخل_کرو != NUL)
	stop_داخل_کرو_منوال = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    إذا ((State & منوال_داخل_کرو) == 0)
	restart_edit = win->w_بفر->b_prompt_داخل_کرو;
}
#جے_انت

    ساكن فراغ
win_init_خالی_تھاواں(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_سطریں_valid = 0;
    wp->w_cursor.lnum = 1;
    wp->w_curswant = wp->w_cursor.خانہ = 0;
    wp->w_cursor.coladd = 0;
    wp->w_pcmark.lnum = 1;	// pcmark not cleared but set to line 1
    wp->w_pcmark.خانہ = 0;
    wp->w_prev_pcmark.lnum = 0;
    wp->w_prev_pcmark.خانہ = 0;
    wp->w_topline = 1;
#جے_تعریف_شدہ FEAT_DIFF
    wp->w_topfill = 0;
#جے_انت
    wp->w_botline = 2;
    wp->w_valid = 0;
#إذا defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    wp->w_s = &wp->w_بفر->b_s;
#جے_انت
#جے_تعریف_شدہ FEAT_TERMINAL
    رکن_reset_hlfwin(wp);
#جے_انت
}

/*
 * Init the current window "curwin".
 * Called when a new file is being edited.
 */
    فراغ
curwin_init(فراغ)
{
    win_init_خالی_تھاواں(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    فراغ
close_windows(
    buf_T	*buf,
    صحيح		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    صحيح		گنتی = tabpage_اشاریہ(عدم);

    ++RedrawingDisabled;

    لكل (wp = firstwin; wp != عدم && !ONE_کھڑکی; )
    {
	إذا (wp->w_بفر == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_بفر->b_locked > 0))
	{
	    إذا (کھڑکی_layout_locked(CMD_ناپ))
		اذهب theend;  // Only give one error message.
	    إذا (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		اكسر;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	وإلا
	    wp = wp->w_اگلا;
    }

    // Also check windows in other tab pages.
    لكل (tp = پہلا_tabpage; tp != عدم; tp = nexttp)
    {
	nexttp = tp->tp_اگلا;
	إذا (tp != curtab)
	    FOR_ALL_WINDOWS_وچ_TAB(tp, wp)
		إذا (wp->w_بفر == buf
		    && !(win_locked(wp) || wp->w_بفر->b_locked > 0))
		{
		    إذا (کھڑکی_layout_locked(CMD_ناپ))
			اذهب theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = پہلا_tabpage;
		    اكسر;
		}
    }

theend:
    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;

    إذا (گنتی != tabpage_اشاریہ(عدم))
	apply_autocmds(EVENT_TABCLOSED, عدم, عدم, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    صحيح
آخری_کھڑکی(فراغ)
{
    أرجع (one_کھڑکی() && پہلا_tabpage->tp_اگلا == عدم);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    صحيح
one_کھڑکی(فراغ)
{
    win_T	*wp;
    صحيح		تکیا_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	إذا (!is_aucmd_win(wp))
	{
	    إذا (تکیا_one)
		أرجع FALSE;
	    تکیا_one = TRUE;
	}
    }
    أرجع TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    ساكن صحيح
close_آخری_کھڑکی_tabpage(
    win_T	*win,
    صحيح		free_buf,
    tabpage_T   *prev_curtab)
{
    إذا (!ONE_کھڑکی)
	أرجع FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_بفر != عدم);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    إذا (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    إذا (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#جے_تعریف_شدہ FEAT_کم_CHANNEL
    entering_کھڑکی(curwin);
#جے_انت
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, عدم, عدم, FALSE, curbuf);
#إذا defined(FEAT_TABPANEL)
    إذا (p_stpl > 0)
	shell_new_columns();
#جے_انت
    apply_autocmds(EVENT_WINENTER, عدم, عدم, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, عدم, عدم, FALSE, curbuf);
    إذا (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, عدم, عدم, FALSE, curbuf);
    أرجع TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    ساكن فراغ
win_close_بفر(win_T *win, صحيح action, صحيح abort_if_آخری)
{
#جے_تعریف_شدہ FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    إذا (win->w_بفر != عدم)
	reset_synblock(win);
#جے_انت

#جے_تعریف_شدہ FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    إذا (win->w_بفر != عدم && bt_quickfix(win->w_بفر)
					&& win->w_بفر->b_nwindows == 1)
	win->w_بفر->b_p_bl = FALSE;
#جے_انت

    // Close the link to the buffer.
    إذا (win->w_بفر != عدم)
    {
	bufref_T    bufref;

	سیٹ_bufref(&bufref, curbuf);
	++win->w_locked;
	close_بفر(win, win->w_بفر, action, abort_if_آخری, TRUE, TRUE);
	إذا (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	إذا (!bufref_valid(&bufref))
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
    صحيح
win_close(win_T *win, صحيح free_buf)
{
    win_T	*wp;
    صحيح		other_بفر = FALSE;
    صحيح		close_curwin = FALSE;
    صحيح		dir;
    صحيح		help_کھڑکی = FALSE;
    صحيح		quickfix_کھڑکی = FALSE;
    tabpage_T   *prev_curtab = curtab;
    چوکھٹا_T	*win_چوکھٹا = win->w_چوکھٹا->fr_ماپا;
#جے_تعریف_شدہ FEAT_DIFF
    صحيح		had_diffmode = win->w_p_diff;
#جے_انت
#جے_تعریف_شدہ سنیہا_قطار
    صحيح		did_decrement = FALSE;
#جے_انت

#إذا defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    إذا (may_close_رکن_popup() == OK)
	أرجع OK;
#جے_انت
    إذا (ERROR_IF_ANY_POPUP_کھڑکی)
	أرجع FAIL;

    إذا (آخری_کھڑکی())
    {
	emsg(_(e_cannot_close_آخری_کھڑکی));
	أرجع FAIL;
    }
    إذا (کھڑکی_layout_locked(CMD_close))
	أرجع FAIL;

    إذا (win_locked(win) || (win->w_بفر != عدم
					       && win->w_بفر->b_locked > 0))
	أرجع FAIL; // window is already being closed
    إذا (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_کھڑکی));
	أرجع FAIL;
    }
    إذا ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_کھڑکی())
    {
	emsg(_(e_cannot_close_کھڑکی_only_autocmd_کھڑکی_would_remain));
	أرجع FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    إذا (close_آخری_کھڑکی_tabpage(win, free_buf, prev_curtab))
	أرجع FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    إذا (bt_help(win->w_بفر))
	help_کھڑکی = TRUE;
    وإلا
	clear_snapshot(curtab, SNAP_HELP_IDX);

    إذا (bt_quickfix(win->w_بفر))
	quickfix_کھڑکی = TRUE;
    وإلا
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    إذا (win == curwin)
    {
#جے_تعریف_شدہ FEAT_کم_CHANNEL
	leaving_کھڑکی(curwin);
#جے_انت
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, عدم));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	إذا (wp->w_بفر != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_بفر = TRUE;
	    إذا (!win_valid(win))
		أرجع FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, عدم, عدم, FALSE, curbuf);
	    إذا (!win_valid(win))
		أرجع FAIL;
	    --win->w_locked;
	    إذا (آخری_کھڑکی())
		أرجع FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, عدم, عدم, FALSE, curbuf);
	إذا (!win_valid(win))
	    أرجع FAIL;
	--win->w_locked;
	إذا (آخری_کھڑکی())
	    أرجع FAIL;
#جے_تعریف_شدہ FEAT_EVAL
	// autocmds may abort script processing
	إذا (aborting())
	    أرجع FAIL;
#جے_انت
    }

#جے_تعریف_شدہ FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    إذا (gui.وچ_use)
	باہرلا_flush();
#جے_انت

#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (popup_win_closed(win) && !win_valid(win))
	أرجع FAIL;
#جے_انت

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    إذا (!win_valid_any_tab(win))
	أرجع OK;

    win_close_بفر(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    إذا (win_valid(win) && win->w_بفر == عدم
#إذا defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#جے_انت
	    && آخری_کھڑکی())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	إذا (curwin->w_بفر == عدم)
	    curwin->w_بفر = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    إذا (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_بفر == عدم)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	أرجع FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    إذا (!win_valid(win) || آخری_کھڑکی()
	    || close_آخری_کھڑکی_tabpage(win, free_buf, prev_curtab))
	أرجع FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++ونڈو_disallowed;
#جے_تعریف_شدہ سنیہا_قطار
    ++dont_parse_messages;
#جے_انت

    إذا (win->w_بفر != عدم)
	--win->w_بفر->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, عدم);

    إذا (help_کھڑکی || quickfix_کھڑکی)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_کھڑکی ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	إذا (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    إذا (win == curwin)
    {
	curwin = wp;
#جے_تعریف_شدہ FEAT_QUICKFIX
	إذا (wp->w_p_pvw || bt_quickfix(wp->w_بفر))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    لكل (;;)
	    {
		إذا (wp->w_اگلا == عدم)
		    wp = firstwin;
		وإلا
		    wp = wp->w_اگلا;
		إذا (wp == curwin)
		    اكسر;
		إذا (!wp->w_p_pvw && !bt_quickfix(wp->w_بفر))
		{
		    curwin = wp;
		    اكسر;
		}
	    }
	}
#جے_انت
	curbuf = curwin->w_بفر;
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
    آخری_حالت(FALSE);

    إذا (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_چوکھٹا->fr_ماپا == win_چوکھٹا, dir);
    وإلا
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    إذا (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#جے_تعریف_شدہ سنیہا_قطار
	did_decrement =
#وإلا
	(فراغ)
#جے_انت
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	إذا (other_بفر)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, عدم, عدم, FALSE, curbuf);
    }

    إذا (ONE_کھڑکی && curwin->w_locked && curbuf->b_locked_ونڈو
	    && پہلا_tabpage->tp_اگلا != عدم)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, عدم, عدم, FALSE, curbuf);

    --ونڈو_disallowed;
#جے_تعریف_شدہ سنیہا_قطار
    إذا (!did_decrement)
	--dont_parse_messages;
#جے_انت

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    إذا (help_کھڑکی || quickfix_کھڑکی)
	restore_snapshot(help_کھڑکی ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#جے_تعریف_شدہ FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    إذا (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	صحيح	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    إذا (dwin->w_p_diff)
		++diffcount;
	إذا (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#جے_انت

#إذا defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    إذا (gui.وچ_use && !win_hasvertsplit())
	gui_init_which_components(عدم);
#جے_انت

    redraw_all_later(UPD_NOT_VALID);
    أرجع OK;
}

    ساكن فراغ
trigger_winnewpre(فراغ)
{
    کھڑکی_layout_جندرا();
    apply_autocmds(EVENT_WINNEWPRE, عدم, عدم, FALSE, عدم);
    کھڑکی_layout_unlock();
}

    ساكن فراغ
trigger_winclosed(win_T *win)
{
    ساكن صحيح	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    إذا (recursive)
	أرجع;
    recursive = TRUE;
    vim_snprintf((حرف *)winid, حجم(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_بفر);
    recursive = FALSE;
}

    فراغ
trigger_tabclosedpre(tabpage_T *tp)
{
    ساكن صحيح	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    إذا (!has_tabclosedpre() || recursive)
	أرجع;

    إذا (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    کھڑکی_layout_جندرا();
    apply_autocmds(EVENT_TABCLOSEDPRE, عدم, عدم, FALSE, عدم);
    کھڑکی_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    إذا (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    وإلا
	// fall back to the first tabpage
	goto_tabpage_tp(پہلا_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    فراغ
snapshot_windows_scroll_ناپ(فراغ)
{
    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	wp->w_آخری_topline = wp->w_topline;
#جے_تعریف_شدہ FEAT_DIFF
	wp->w_آخری_topfill = wp->w_topfill;
#جے_انت
	wp->w_آخری_leftcol = wp->w_leftcol;
	wp->w_آخری_skipcol = wp->w_skipcol;
	wp->w_آخری_چوڑائی = wp->w_چوڑائی;
	wp->w_آخری_اچائی = wp->w_اچائی;
    }
}

ساكن صحيح did_initial_scroll_ناپ_snapshot = FALSE;

    فراغ
may_make_initial_scroll_ناپ_snapshot(فراغ)
{
    إذا (!did_initial_scroll_ناپ_snapshot)
    {
	did_initial_scroll_ناپ_snapshot = TRUE;
	snapshot_windows_scroll_ناپ();
    }
}

#جے_تعریف_شدہ FEAT_EVAL
/*
 * Create a dictionary with information about size and scroll changes in a
 * window.
 * Returns the dictionary with refcount set to one.
 * Returns NULL when out of memory.
 */
    ساكن dict_T *
make_win_info_dict(
	صحيح چوڑائی,
	صحيح اچائی,
	صحيح topline,
# ifdef FEAT_DIFF
	صحيح topfill,
# endif
	صحيح leftcol,
	صحيح skipcol)
{
    dict_T *d = dict_alloc();
    إذا (d == عدم)
	أرجع عدم;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    طالما (1)
    {
	typval_T tv;
	tv.v_جندرا = 0;
	tv.v_type = VAR_نمبر;

	tv.vval.v_نمبر = چوڑائی;
	إذا (dict_جوڑو_tv(d, "width", &tv) == FAIL)
	    اكسر;
	tv.vval.v_نمبر = اچائی;
	إذا (dict_جوڑو_tv(d, "height", &tv) == FAIL)
	    اكسر;
	tv.vval.v_نمبر = topline;
	إذا (dict_جوڑو_tv(d, "topline", &tv) == FAIL)
	    اكسر;
# ifdef FEAT_DIFF
	tv.vval.v_نمبر = topfill;
# وإلا
	tv.vval.v_نمبر = 0;
# endif
	إذا (dict_جوڑو_tv(d, "topfill", &tv) == FAIL)
	    اكسر;
	tv.vval.v_نمبر = leftcol;
	إذا (dict_جوڑو_tv(d, "leftcol", &tv) == FAIL)
	    اكسر;
	tv.vval.v_نمبر = skipcol;
	إذا (dict_جوڑو_tv(d, "skipcol", &tv) == FAIL)
	    اكسر;
	أرجع d;
    }
    dict_unref(d);
    أرجع عدم;
}
#جے_انت

// Return values of check_window_scroll_resize():
#تعریف_کرو CWSR_SCROLLED	1  // at least one window scrolled
#تعریف_کرو CWSR_RESIZED	2  // at least one window size changed

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
    ساكن فراغ
check_کھڑکی_scroll_resize(
	صحيح	*ناپ_گنتی,
	win_T	**پہلا_scroll_win,
	win_T	**پہلا_ناپ_win,
	list_T	*winlist بن_ورتیا,
	dict_T	*v_event بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_EVAL
    صحيح listidx = 0;
    صحيح tot_چوڑائی = 0;
    صحيح tot_اچائی = 0;
    صحيح tot_topline = 0;
# ifdef FEAT_DIFF
    صحيح tot_topfill = 0;
# endif
    صحيح tot_leftcol = 0;
    صحيح tot_skipcol = 0;
#جے_انت

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	صحيح ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	صحيح ناپ_بدلیا = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_آخری_چوڑائی != wp->w_چوڑائی
			    || wp->w_آخری_اچائی != wp->w_اچائی);
	إذا (ناپ_بدلیا)
	{
#جے_تعریف_شدہ FEAT_EVAL
	    إذا (winlist != عدم)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_جندرا = 0;
		tv.v_type = VAR_نمبر;
		tv.vval.v_نمبر = wp->w_id;
		list_سیٹ_شے(winlist, listidx++, &tv);
	    }
	    وإلا
#جے_انت
		إذا (ناپ_گنتی != عدم)
	    {
		++*ناپ_گنتی;
		إذا (*پہلا_ناپ_win == عدم)
		    *پہلا_ناپ_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		إذا (*پہلا_scroll_win == عدم && !ignore_scroll)
		    *پہلا_scroll_win = wp;
	    }
	}

	صحيح scroll_بدلیا = !ignore_scroll
				&& (wp->w_آخری_topline != wp->w_topline
#جے_تعریف_شدہ FEAT_DIFF
				|| wp->w_آخری_topfill != wp->w_topfill
#جے_انت
				|| wp->w_آخری_leftcol != wp->w_leftcol
				|| wp->w_آخری_skipcol != wp->w_skipcol);
	إذا (scroll_بدلیا
	    && پہلا_scroll_win != عدم && *پہلا_scroll_win == عدم)
	    *پہلا_scroll_win = wp;

#جے_تعریف_شدہ FEAT_EVAL
	إذا ((ناپ_بدلیا || scroll_بدلیا) && v_event != عدم)
	{
	    // Add info about this window to the v:event dictionary.
	    صحيح چوڑائی = wp->w_چوڑائی - wp->w_آخری_چوڑائی;
	    صحيح اچائی = wp->w_اچائی - wp->w_آخری_اچائی;
	    صحيح topline = wp->w_topline - wp->w_آخری_topline;
# ifdef FEAT_DIFF
	    صحيح topfill = wp->w_topfill - wp->w_آخری_topfill;
# endif
	    صحيح leftcol = wp->w_leftcol - wp->w_آخری_leftcol;
	    صحيح skipcol = wp->w_skipcol - wp->w_آخری_skipcol;
	    dict_T *d = make_win_info_dict(چوڑائی, اچائی, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    إذا (d == عدم)
		اكسر;
	    حرف winid[NUMBUFLEN];
	    vim_snprintf(winid, حجم(winid), "%d", wp->w_id);
	    إذا (dict_جوڑو_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		اكسر;
	    }
	    --d->dv_refcount;

	    tot_چوڑائی += مطلق_قدر(چوڑائی);
	    tot_اچائی += مطلق_قدر(اچائی);
	    tot_topline += مطلق_قدر(topline);
# ifdef FEAT_DIFF
	    tot_topfill += مطلق_قدر(topfill);
# endif
	    tot_leftcol += مطلق_قدر(leftcol);
	    tot_skipcol += مطلق_قدر(skipcol);
	}
#جے_انت
    }

#جے_تعریف_شدہ FEAT_EVAL
    إذا (v_event != عدم)
    {
	dict_T *alldict = make_win_info_dict(tot_چوڑائی, tot_اچائی, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	إذا (alldict != عدم)
	{
	    إذا (dict_جوڑو_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    وإلا
		--alldict->dv_refcount;
	}
    }
#جے_انت
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    فراغ
may_trigger_win_scrolled_resized(فراغ)
{
    ساكن صحيح	    recursive = FALSE;
    صحيح		    do_resize = has_winresized();
    صحيح		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    إذا (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_ناپ_snapshot)
	أرجع;

    صحيح ناپ_گنتی = 0;
    win_T *پہلا_scroll_win = عدم, *پہلا_ناپ_win = عدم;
    check_کھڑکی_scroll_resize(&ناپ_گنتی, &پہلا_scroll_win, &پہلا_ناپ_win,
								    عدم, عدم);
    صحيح trigger_resize = do_resize && ناپ_گنتی > 0;
    صحيح trigger_scroll = do_scroll && پہلا_scroll_win != عدم;
    إذا (!trigger_resize && !trigger_scroll)
	أرجع;  // no relevant changes
#جے_تعریف_شدہ FEAT_EVAL
    list_T *windows_list = عدم;
    إذا (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_اشیاء(ناپ_گنتی);
	إذا (windows_list != عدم)
	    check_کھڑکی_scroll_resize(عدم, عدم, عدم, windows_list, عدم);
    }

    dict_T *scroll_dict = عدم;
    إذا (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	إذا (scroll_dict != عدم)
	{
	    scroll_dict->dv_refcount = 1;
	    check_کھڑکی_scroll_resize(عدم, عدم, عدم, عدم, scroll_dict);
	}
    }
#جے_انت

    // WinScrolled/WinResized are triggered only once, even when multiple
    // windows scrolled or changed size.  Store the current values before
    // triggering the event, if a scroll or resize happens as a side effect
    // then WinScrolled/WinResized is triggered for that later.
    snapshot_windows_scroll_ناپ();

    // "curwin" may be different from the actual current window, make
    // sure it can be restored.
    کھڑکی_layout_جندرا();
    recursive = TRUE;

    // If both are to be triggered do WinResized first.
    إذا (trigger_resize
#جے_تعریف_شدہ FEAT_EVAL
	    && windows_list != عدم
#جے_انت
	    )
    {
#جے_تعریف_شدہ FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	إذا (dict_جوڑو_list(v_event, "windows", windows_list) == OK)
	{
	    dict_سیٹ_اشیاء_ro(v_event);
#جے_انت
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((حرف *)winid, حجم(winid), "%d",
							 پہلا_ناپ_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     پہلا_ناپ_win->w_بفر);
#جے_تعریف_شدہ FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#جے_انت
    }

    إذا (trigger_scroll
#جے_تعریف_شدہ FEAT_EVAL
	    && scroll_dict != عدم
#جے_انت
	    )
    {
#جے_تعریف_شدہ FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", عدم);
	dict_سیٹ_اشیاء_ro(v_event);
	dict_unref(scroll_dict);
#جے_انت
	char_u winid[NUMBUFLEN];
	vim_snprintf((حرف *)winid, حجم(winid), "%d",
						       پہلا_scroll_win->w_id);
	apply_autocmds(EVENT_WINSCROLLED, winid, winid, FALSE,
						   پہلا_scroll_win->w_بفر);
#جے_تعریف_شدہ FEAT_EVAL
	restore_v_event(v_event, &save_v_event);
#جے_انت
    }

    recursive = FALSE;
    کھڑکی_layout_unlock();
}

/*
 * Close window "win" in tab page "tp", which is not the current tab page.
 * This may be the last window in that tab page and result in closing the tab,
 * thus "tp" may become invalid!
 * Caller must check if buffer is hidden and whether the tabline needs to be
 * updated.
 */
    فراغ
win_close_othertab(win_T *win, صحيح free_buf, tabpage_T *tp)
{
    win_T	*wp;
    صحيح		dir;
    tabpage_T   *ptp = عدم;
    صحيح		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    إذا (کھڑکی_layout_locked(CMD_ناپ))
	أرجع;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    إذا (win_locked(win) || (win->w_بفر != عدم
					       && win->w_بفر->b_locked > 0))
	أرجع; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    إذا (win->w_بفر != عدم)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	إذا (!win_valid_any_tab(win))
	    أرجع;
    }

    إذا (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	إذا (!win_valid_any_tab(win))
	    أرجع;
    }

    إذا (win->w_بفر != عدم)
	// Close the link to the buffer.
	close_بفر(win, win->w_بفر, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    لكل (ptp = پہلا_tabpage; ptp != عدم && ptp != tp; ptp = ptp->tp_اگلا)
	;
    إذا (ptp == عدم || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	إذا (win_valid_any_tab(win) && win->w_بفر == عدم)
	{
	    win->w_بفر = firstbuf;
	    ++firstbuf->b_nwindows;
	    إذا (win == curwin)
		curbuf = curwin->w_بفر;
	    win_init_خالی_تھاواں(win);
	}
	أرجع;
    }

    // Autocommands may have closed the window already.
    لكل (wp = tp->tp_firstwin; wp != عدم && wp != win; wp = wp->w_اگلا)
	;
    إذا (wp == عدم)
	أرجع;

    // When closing the last window in a tab page remove the tab page.
    إذا (tp->tp_firstwin == tp->tp_lastwin)
    {
	صحيح	h = tabline_اچائی();

	إذا (tp == پہلا_tabpage)
	    پہلا_tabpage = tp->tp_اگلا;
	وإلا
	{
	    لكل (ptp = پہلا_tabpage; ptp != عدم && ptp->tp_اگلا != tp;
							   ptp = ptp->tp_اگلا)
		;
	    إذا (ptp == عدم)
	    {
		اندرونی_error("win_close_othertab()");
		أرجع;
	    }
	    ptp->tp_اگلا = tp->tp_اگلا;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	إذا (h != tabline_اچائی())
	    shell_new_صفاں();
    }

    إذا (win->w_بفر != عدم)
	--win->w_بفر->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    إذا (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    ساكن win_T *
win_free_mem(
    win_T	*win,
    صحيح		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    چوکھٹا_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == عدم ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_چوکھٹا;
    wp = winframe_remove(win, dirp, tp, عدم);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    إذا (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    أرجع wp;
}

#إذا defined(EXITFREE)
    فراغ
win_free_all(فراغ)
{
    صحيح		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = عدم;
    cmdwin_win = عدم;

    طالما (پہلا_tabpage->tp_اگلا != عدم)
	tabpage_close(TRUE);

    لكل (صحيح i = 0; i < AUCMD_WIN_گنتی; ++i)
	إذا (aucmd_win[i].auc_win != عدم)
	{
	    (فراغ)win_free_mem(aucmd_win[i].auc_win, &dummy, عدم);
	    aucmd_win[i].auc_win = عدم;
	}

    طالما (firstwin != عدم)
	(فراغ)win_free_mem(firstwin, &dummy, عدم);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = عدم;
}
#جے_انت

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    صحيح		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    چوکھٹا_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    چوکھٹا_T	*frp, *frp2, *frp3;
    چوکھٹا_T	*frp_close = win->w_چوکھٹا;
    win_T	*wp;
    صحيح		صف, خانہ;

    /*
     * If there is only one window there is nothing to remove.
     */
    إذا (tp == عدم ? ONE_کھڑکی : tp->tp_firstwin == tp->tp_lastwin)
	أرجع عدم;

    چوکھٹا_locked++;

    // Save the position of the containing frame (which will also contain the
    // altframe) before we remove anything, to recompute window positions later.
    wp = frame2win(frp_close->fr_ماپا);
    صف = wp->w_winrow;
    خانہ = wp->w_wincol;

    /*
     * Remove the window from its frame.
     */
    frp2 = win_altframe(win, tp);
    wp = frame2win(frp2);

    // If this is a rightmost window, remove vertical separators to the left.
    إذا (win->w_vsep_چوڑائی == 0 && frp_close->fr_ماپا->fr_layout == FR_صف &&
	    frp_close->fr_prev != عدم)
	چوکھٹا_سیٹ_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    چوکھٹا_remove(frp_close);

    إذا (frp_close->fr_ماپا->fr_layout == FR_خانہ)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	إذا (frp2->fr_win != عدم && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_اگلا;
	    طالما (frp != عدم || frp3 != عدم)
	    {
		إذا (frp != عدم)
		{
		    إذا (!چوکھٹا_fixed_اچائی(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			اكسر;
		    }
		    frp = frp->fr_prev;
		}
		إذا (frp3 != عدم)
		{
		    إذا (frp3->fr_win != عدم && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			اكسر;
		    }
		    frp3 = frp3->fr_اگلا;
		}
	    }
	}
	چوکھٹا_new_اچائی(frp2, frp2->fr_اچائی + frp_close->fr_اچائی,
			    frp2 == frp_close->fr_اگلا, !چوکھٹا_fixed_اچائی(frp2), FALSE);
	*dirp = 'v';
    }
    وإلا
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	إذا (frp2->fr_win != عدم && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_اگلا;
	    طالما (frp != عدم || frp3 != عدم)
	    {
		إذا (frp != عدم)
		{
		    إذا (!چوکھٹا_fixed_چوڑائی(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			اكسر;
		    }
		    frp = frp->fr_prev;
		}
		إذا (frp3 != عدم)
		{
		    إذا (frp3->fr_win != عدم && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			اكسر;
		    }
		    frp3 = frp3->fr_اگلا;
		}
	    }
	}
	چوکھٹا_new_چوڑائی(frp2, frp2->fr_چوڑائی + frp_close->fr_چوڑائی,
			    frp2 == frp_close->fr_اگلا, !چوکھٹا_fixed_چوڑائی(frp2));
	*dirp = 'h';
    }

    // If the altframe wasn't adjacent and left/above, resizing it will have
    // changed window positions within the parent frame.  Recompute them.
    إذا (frp2 != frp_close->fr_prev)
	چوکھٹا_comp_pos(frp_close->fr_ماپا, &صف, &خانہ);

    إذا (unflat_altfr == عدم)
	چوکھٹا_flatten(frp2);
    وإلا
	*unflat_altfr = frp2;

    چوکھٹا_locked--;

    أرجع wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    ساكن فراغ
چوکھٹا_flatten(چوکھٹا_T *frp)
{
    چوکھٹا_T	*frp2, *frp3;

    إذا (frp->fr_اگلا != عدم || frp->fr_prev != عدم)
	أرجع;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_ماپا->fr_layout = frp->fr_layout;
    frp->fr_ماپا->fr_بچہ = frp->fr_بچہ;
    FOR_ALL_چوکھٹے(frp2, frp->fr_بچہ)
	frp2->fr_ماپا = frp->fr_ماپا;
    frp->fr_ماپا->fr_win = frp->fr_win;
    إذا (frp->fr_win != عدم)
	frp->fr_win->w_چوکھٹا = frp->fr_ماپا;
    frp2 = frp->fr_ماپا;
    إذا (topframe->fr_بچہ == frp)
	topframe->fr_بچہ = frp2;
    vim_free(frp);

    frp = frp2->fr_ماپا;
    إذا (frp != عدم && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	إذا (frp->fr_بچہ == frp2)
	    frp->fr_بچہ = frp2->fr_بچہ;
	frp2->fr_بچہ->fr_prev = frp2->fr_prev;
	إذا (frp2->fr_prev != عدم)
	    frp2->fr_prev->fr_اگلا = frp2->fr_بچہ;
	لكل (frp3 = frp2->fr_بچہ; ; frp3 = frp3->fr_اگلا)
	{
	    frp3->fr_ماپا = frp;
	    إذا (frp3->fr_اگلا == عدم)
	    {
		frp3->fr_اگلا = frp2->fr_اگلا;
		إذا (frp2->fr_اگلا != عدم)
		    frp2->fr_اگلا->fr_prev = frp3;
		اكسر;
	    }
	}
	إذا (topframe->fr_بچہ == frp2)
	    topframe->fr_بچہ = frp;
	vim_free(frp2);
    }
}

/*
 * Undo changes from a prior call to winframe_remove, also restoring lost
 * vertical separators and statuslines, and changed window positions for
 * windows within "unflat_altfr".
 * Caller must ensure no other changes were made to the layout or window sizes!
 */
    ساكن فراغ
winframe_restore(win_T *wp, صحيح dir, چوکھٹا_T *unflat_altfr)
{
    چوکھٹا_T	*frp = wp->w_چوکھٹا;

    // Put "wp"'s frame back where it was.
    إذا (frp->fr_prev != عدم)
	چوکھٹا_append(frp->fr_prev, frp);
    وإلا
	چوکھٹا_داخل_کرو(frp->fr_اگلا, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    إذا (wp->w_vsep_چوڑائی == 0
	    && frp->fr_ماپا->fr_layout == FR_صف && frp->fr_prev != عدم)
	چوکھٹا_سیٹ_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    إذا (wp->w_حالت_اچائی == 0
	    && frp->fr_ماپا->fr_layout == FR_خانہ && frp->fr_prev != عدم)
	چوکھٹا_جوڑو_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    إذا (dir == 'v')
    {
	چوکھٹا_new_اچائی(unflat_altfr, unflat_altfr->fr_اچائی - frp->fr_اچائی,
		unflat_altfr == frp->fr_اگلا, FALSE, FALSE);
    }
    وإلا إذا (dir == 'h')
    {
	چوکھٹا_new_چوڑائی(unflat_altfr, unflat_altfr->fr_چوڑائی - frp->fr_چوڑائی,
		unflat_altfr == frp->fr_اگلا, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    إذا (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_ماپا);
	صحيح	صف = topleft->w_winrow;
	صحيح	خانہ = topleft->w_wincol;

	چوکھٹا_comp_pos(frp->fr_ماپا, &صف, &خانہ);
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
    ساكن چوکھٹا_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    چوکھٹا_T	*frp;
    چوکھٹا_T	*other_fr, *نشانہ_fr;

    إذا (tp == عدم ? ONE_کھڑکی : tp->tp_firstwin == tp->tp_lastwin)
	أرجع alt_tabpage()->tp_curwin->w_چوکھٹا;

    frp = win->w_چوکھٹا;

    إذا (frp->fr_prev == عدم)
	أرجع frp->fr_اگلا;
    إذا (frp->fr_اگلا == عدم)
	أرجع frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    //کھڑکی
    نشانہ_fr = frp->fr_اگلا;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    إذا (frp->fr_ماپا != عدم && frp->fr_ماپا->fr_layout == FR_خانہ && p_sb)
    {
	نشانہ_fr = frp->fr_prev;
	other_fr  = frp->fr_اگلا;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    إذا (frp->fr_ماپا != عدم && frp->fr_ماپا->fr_layout == FR_صف && p_spr)
    {
	نشانہ_fr = frp->fr_prev;
	other_fr  = frp->fr_اگلا;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    إذا (frp->fr_ماپا != عدم && frp->fr_ماپا->fr_layout == FR_صف)
    {
	إذا (چوکھٹا_fixed_چوڑائی(نشانہ_fr) && !چوکھٹا_fixed_چوڑائی(other_fr))
	    نشانہ_fr = other_fr;
    }
    وإلا
    {
	إذا (چوکھٹا_fixed_اچائی(نشانہ_fr) && !چوکھٹا_fixed_اچائی(other_fr))
	    نشانہ_fr = other_fr;
    }

    أرجع نشانہ_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    ساكن tabpage_T *
alt_tabpage(فراغ)
{
    tabpage_T	*tp = عدم;
    صحيح		forward;

    // Use the last accessed tab page, if possible.
    إذا ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	أرجع lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_اگلا != عدم &&
	    ((tcl_flags & TCL_بایاں) == 0 || curtab == پہلا_tabpage);

    إذا (forward)
	tp = curtab->tp_اگلا;
    وإلا
	// Use the previous tab page.
	لكل (tp = پہلا_tabpage; tp->tp_اگلا != curtab; tp = tp->tp_اگلا)
	    ;

    أرجع tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    ساكن win_T *
frame2win(چوکھٹا_T *frp)
{
    طالما (frp->fr_win == عدم)
	frp = frp->fr_بچہ;
    أرجع frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    ساكن صحيح
چوکھٹا_has_win(چوکھٹا_T *frp, win_T *wp)
{
    چوکھٹا_T	*p;

    إذا (frp->fr_layout == FR_پتی)
	أرجع frp->fr_win == wp;

    FOR_ALL_چوکھٹے(p, frp->fr_بچہ)
	إذا (چوکھٹا_has_win(p, wp))
	    أرجع TRUE;
    أرجع FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
ساكن صحيح min_سیٹ_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    ساكن فراغ
چوکھٹا_new_اچائی(
    چوکھٹا_T	*topfrp,
    صحيح		اچائی,
    صحيح		topfirst,	// resize topmost contained frame first
    صحيح		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    صحيح		سیٹ_ch)		// set 'cmdheight' to resize topframe
{
    چوکھٹا_T	*frp;
    صحيح		extra_سطریں;
    صحيح		h;

    إذا (topfrp->fr_ماپا == عدم && سیٹ_ch)
    {
	// topframe: update the command line height, with side effects.
	صحيح new_ch = MAX(min_سیٹ_ch, p_ch + topfrp->fr_اچائی - اچائی);
	صحيح save_ch = min_سیٹ_ch;
	إذا (new_ch != p_ch)
	    سیٹ_option_قدر((char_u *)"cmdheight", new_ch, عدم, 0);
	min_سیٹ_ch = save_ch;
	اچائی = MIN(اچائی, صفاں_AVAIL);
    }
    إذا (topfrp->fr_win != عدم)
    {
	// Simple case: just one window.
	win_new_اچائی(topfrp->fr_win,
				    اچائی - topfrp->fr_win->w_حالت_اچائی
					      - WINBAR_اچائی(topfrp->fr_win));
    }
    وإلا إذا (topfrp->fr_layout == FR_صف)
    {
	افعل
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    {
		چوکھٹا_new_اچائی(frp, اچائی, topfirst, wfh, سیٹ_ch);
		إذا (frp->fr_اچائی > اچائی)
		{
		    // Could not fit the windows, make the whole row higher.
		    اچائی = frp->fr_اچائی;
		    اكسر;
		}
	    }
	}
	طالما (frp != عدم);
    }
    وإلا    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_بچہ;
	إذا (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    طالما (چوکھٹا_fixed_اچائی(frp))
	    {
		frp = frp->fr_اگلا;
		إذا (frp == عدم)
		    أرجع;	    // no frame without 'wfh', give up
	    }
	إذا (!topfirst)
	{
	    // Find the bottom frame of this column
	    طالما (frp->fr_اگلا != عدم)
		frp = frp->fr_اگلا;
	    إذا (wfh)
		// Advance back for frames with one window with 'wfh' set.
		طالما (چوکھٹا_fixed_اچائی(frp))
		    frp = frp->fr_prev;
	}

	extra_سطریں = اچائی - topfrp->fr_اچائی;
	إذا (extra_سطریں < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    طالما (frp != عدم)
	    {
		h = چوکھٹا_minheight(frp, عدم);
		إذا (frp->fr_اچائی + extra_سطریں < h)
		{
		    extra_سطریں += frp->fr_اچائی - h;
		    چوکھٹا_new_اچائی(frp, h, topfirst, wfh, سیٹ_ch);
		}
		وإلا
		{
		    چوکھٹا_new_اچائی(frp, frp->fr_اچائی + extra_سطریں,
							topfirst, wfh, سیٹ_ch);
		    اكسر;
		}
		إذا (topfirst)
		{
		    افعل
			frp = frp->fr_اگلا;
		    طالما (wfh && frp != عدم && چوکھٹا_fixed_اچائی(frp));
		}
		وإلا
		{
		    افعل
			frp = frp->fr_prev;
		    طالما (wfh && frp != عدم && چوکھٹا_fixed_اچائی(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		إذا (frp == عدم)
		    اچائی -= extra_سطریں;
	    }
	}
	وإلا إذا (extra_سطریں > 0)
	{
	    // increase height of bottom or top frame
	    چوکھٹا_new_اچائی(frp, frp->fr_اچائی + extra_سطریں, topfirst, wfh,
									سیٹ_ch);
	}
    }
    topfrp->fr_اچائی = اچائی;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    ساكن صحيح
چوکھٹا_fixed_اچائی(چوکھٹا_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    إذا (frp->fr_win != عدم)
	أرجع frp->fr_win->w_p_wfh;

    إذا (frp->fr_layout == FR_صف)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	    إذا (چوکھٹا_fixed_اچائی(frp))
		أرجع TRUE;
	أرجع FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	إذا (!چوکھٹا_fixed_اچائی(frp))
	    أرجع FALSE;
    أرجع TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    ساكن صحيح
چوکھٹا_fixed_چوڑائی(چوکھٹا_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    إذا (frp->fr_win != عدم)
	أرجع frp->fr_win->w_p_wfw;

    إذا (frp->fr_layout == FR_خانہ)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	    إذا (چوکھٹا_fixed_چوڑائی(frp))
		أرجع TRUE;
	أرجع FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	إذا (!چوکھٹا_fixed_چوڑائی(frp))
	    أرجع FALSE;
    أرجع TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    ساكن فراغ
چوکھٹا_جوڑو_statusline(چوکھٹا_T *frp)
{
    win_T	*wp;

    إذا (frp->fr_layout == FR_پتی)
    {
	wp = frp->fr_win;
	wp->w_حالت_اچائی = statusline_اچائی(wp);
    }
    وإلا إذا (frp->fr_layout == FR_صف)
    {
	// Handle all the frames in the row.
	FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	    چوکھٹا_جوڑو_statusline(frp);
    }
    وإلا // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	لكل (frp = frp->fr_بچہ; frp->fr_اگلا != عدم; frp = frp->fr_اگلا)
	    ;
	چوکھٹا_جوڑو_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    ساكن فراغ
چوکھٹا_new_چوڑائی(
    چوکھٹا_T	*topfrp,
    صحيح		چوڑائی,
    صحيح		leftfirst,	// resize leftmost contained frame first
    صحيح		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    چوکھٹا_T	*frp;
    صحيح		extra_خانے_گنتی;
    صحيح		w;
    win_T	*wp;

    إذا (topfrp->fr_layout == FR_پتی)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	لكل (frp = topfrp; frp->fr_ماپا != عدم; frp = frp->fr_ماپا)
	    إذا (frp->fr_ماپا->fr_layout == FR_صف && frp->fr_اگلا != عدم)
		اكسر;
	إذا (frp->fr_ماپا == عدم)
	    wp->w_vsep_چوڑائی = 0;
	win_new_چوڑائی(wp, چوڑائی - wp->w_vsep_چوڑائی);
    }
    وإلا إذا (topfrp->fr_layout == FR_خانہ)
    {
	افعل
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    {
		چوکھٹا_new_چوڑائی(frp, چوڑائی, leftfirst, wfw);
		إذا (frp->fr_چوڑائی > چوڑائی)
		{
		    // Could not fit the windows, make whole column wider.
		    چوڑائی = frp->fr_چوڑائی;
		    اكسر;
		}
	    }
	} طالما (frp != عدم);
    }
    وإلا    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_بچہ;
	إذا (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    طالما (چوکھٹا_fixed_چوڑائی(frp))
	    {
		frp = frp->fr_اگلا;
		إذا (frp == عدم)
		    أرجع;	    // no frame without 'wfw', give up
	    }
	إذا (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    طالما (frp->fr_اگلا != عدم)
		frp = frp->fr_اگلا;
	    إذا (wfw)
		// Advance back for frames with one window with 'wfw' set.
		طالما (چوکھٹا_fixed_چوڑائی(frp))
		    frp = frp->fr_prev;
	}

	extra_خانے_گنتی = چوڑائی - topfrp->fr_چوڑائی;
	إذا (extra_خانے_گنتی < 0)
	{
	    // reduce frame width, rightmost frame first
	    طالما (frp != عدم)
	    {
		w = چوکھٹا_minwidth(frp, عدم);
		إذا (frp->fr_چوڑائی + extra_خانے_گنتی < w)
		{
		    extra_خانے_گنتی += frp->fr_چوڑائی - w;
		    چوکھٹا_new_چوڑائی(frp, w, leftfirst, wfw);
		}
		وإلا
		{
		    چوکھٹا_new_چوڑائی(frp, frp->fr_چوڑائی + extra_خانے_گنتی,
							      leftfirst, wfw);
		    اكسر;
		}
		إذا (leftfirst)
		{
		    افعل
			frp = frp->fr_اگلا;
		    طالما (wfw && frp != عدم && چوکھٹا_fixed_چوڑائی(frp));
		}
		وإلا
		{
		    افعل
			frp = frp->fr_prev;
		    طالما (wfw && frp != عدم && چوکھٹا_fixed_چوڑائی(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		إذا (frp == عدم)
		    چوڑائی -= extra_خانے_گنتی;
	    }
	}
	وإلا إذا (extra_خانے_گنتی > 0)
	{
	    // increase width of rightmost frame
	    چوکھٹا_new_چوڑائی(frp, frp->fr_چوڑائی + extra_خانے_گنتی, leftfirst, wfw);
	}
    }
    topfrp->fr_چوڑائی = چوڑائی;
}

/*
 * Add or remove the vertical separator of windows to the right side of "frp".
 * Note: Does not check if there is room!
 */
    ساكن فراغ
چوکھٹا_سیٹ_vsep(چوکھٹا_T *frp, صحيح جوڑو)
{
    win_T	*wp;

    إذا (frp->fr_layout == FR_پتی)
    {
	wp = frp->fr_win;
	إذا (جوڑو && wp->w_vsep_چوڑائی == 0)
	{
	    إذا (wp->w_چوڑائی > 0)	// don't make it negative
		win_new_چوڑائی(wp, wp->w_چوڑائی - 1);
	    wp->w_vsep_چوڑائی = 1;
	}
	وإلا إذا (!جوڑو && wp->w_vsep_چوڑائی == 1)
	{
	    win_new_چوڑائی(wp, wp->w_چوڑائی + 1);
	    wp->w_vsep_چوڑائی = 0;
	}
    }
    وإلا إذا (frp->fr_layout == FR_خانہ)
    {
	// Handle all the frames in the column.
	FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	    چوکھٹا_سیٹ_vsep(frp, جوڑو);
    }
    وإلا // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_بچہ;
	طالما (frp->fr_اگلا != عدم)
	    frp = frp->fr_اگلا;
	چوکھٹا_سیٹ_vsep(frp, جوڑو);
    }
}

/*
 * Set frame width from the window it contains.
 */
    ساكن فراغ
چوکھٹا_fix_چوڑائی(win_T *wp)
{
    wp->w_چوکھٹا->fr_چوڑائی = wp->w_چوڑائی + wp->w_vsep_چوڑائی;
}

/*
 * Set frame height from the window it contains.
 */
    ساكن فراغ
چوکھٹا_fix_اچائی(win_T *wp)
{
    wp->w_چوکھٹا->fr_اچائی = VISIBLE_اچائی(wp) + wp->w_حالت_اچائی;
}

/*
 * Compute the minimal height for frame "topfrp".
 * Uses the 'winminheight' option.
 * When "next_curwin" isn't NULL, use p_wh for this window.
 * When "next_curwin" is NOWIN, don't use at least one line for the current
 * window.
 */
    ساكن صحيح
چوکھٹا_minheight(چوکھٹا_T *topfrp, win_T *اگلا_curwin)
{
    چوکھٹا_T	*frp;
    صحيح		m;
    صحيح		n;

    إذا (topfrp->fr_win != عدم)
    {
	إذا (topfrp->fr_win == اگلا_curwin)
	    m = p_wh + topfrp->fr_win->w_حالت_اچائی;
	وإلا
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_حالت_اچائی;
	    إذا (topfrp->fr_win == curwin && اگلا_curwin == عدم)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		إذا (p_wmh == 0)
		    ++m;
		m += WINBAR_اچائی(curwin);
	    }
	}
    }
    وإلا إذا (topfrp->fr_layout == FR_صف)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	{
	    n = چوکھٹا_minheight(frp, اگلا_curwin);
	    إذا (n > m)
		m = n;
	}
    }
    وإلا
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    m += چوکھٹا_minheight(frp, اگلا_curwin);
    }

    أرجع m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
ساكن صحيح
چوکھٹا_wincount_وچ_اچائی(چوکھٹا_T *topfrp)
{
    چوکھٹا_T	*frp;
    صحيح		c = 0;
    صحيح		n;

    إذا (topfrp->fr_win != عدم)
	c = 1;
    وإلا إذا (topfrp->fr_layout == FR_صف)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	{
	    n = چوکھٹا_wincount_وچ_اچائی(frp);
	    إذا (n > c)
		c = n;
	}
    }
    وإلا
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    c += چوکھٹا_wincount_وچ_اچائی(frp);
    }

    أرجع c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    ساكن صحيح
چوکھٹا_minwidth(
    چوکھٹا_T	*topfrp,
    win_T	*اگلا_curwin)	// use p_wh and p_wiw for next_curwin
{
    چوکھٹا_T	*frp;
    صحيح		m, n;

    إذا (topfrp->fr_win != عدم)
    {
	إذا (topfrp->fr_win == اگلا_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_چوڑائی;
	وإلا
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_چوڑائی;
	    // Current window is minimal one column wide
	    إذا (p_wmw == 0 && topfrp->fr_win == curwin && اگلا_curwin == عدم)
		++m;
	}
    }
    وإلا إذا (topfrp->fr_layout == FR_خانہ)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	{
	    n = چوکھٹا_minwidth(frp, اگلا_curwin);
	    إذا (n > m)
		m = n;
	}
    }
    وإلا
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    m += چوکھٹا_minwidth(frp, اگلا_curwin);
    }

    أرجع m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    فراغ
close_others(
    صحيح		سنیہا,
    صحيح		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    صحيح		r;

    إذا (one_کھڑکی())
    {
	إذا (سنیہا && !autocmd_busy)
	    پیغام(_(m_onlyone));
	أرجع;
    }

    // Be very careful here: autocommands may change the window layout.
    لكل (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_اگلا;

	// autocommands messed this one up
	إذا (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_بفر;
	}

	إذا (wp == curwin)		// don't close current window
	    استمر;

	// autoccommands messed this one up
	إذا (!buf_valid(wp->w_بفر) && win_valid(wp))
	{
	    wp->w_بفر = عدم;
	    win_close(wp, 0);
	    استمر;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_بفر, forceit);
	إذا (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    استمر;
	}
	إذا (!r)
	{
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    إذا (سنیہا && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_بدلیا(wp->w_بفر, FALSE);
		إذا (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    استمر;
		}
	    }
	    إذا (buf_Is_بدلیا(wp->w_بفر))
#جے_انت
		استمر;
	}
	win_close(wp, !buf_hide(wp->w_بفر) && !buf_Is_بدلیا(wp->w_بفر));
    }

    إذا (سنیہا && !ONE_کھڑکی)
	emsg(_(e_other_کھڑکی_شامل_اے_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    فراغ
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
ساكن صحيح command_چوکھٹا_اچائی = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    فراغ
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
    صحيح
win_alloc_پہلا(فراغ)
{
    إذا (win_alloc_firstwin(عدم) == FAIL)
	أرجع FAIL;

    پہلا_tabpage = alloc_tabpage();
    إذا (پہلا_tabpage == عدم)
	أرجع FAIL;
    curtab = پہلا_tabpage;
    unuse_tabpage(پہلا_tabpage);

    أرجع OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(فراغ)
{
    win_T *wp;

    wp = win_alloc(عدم, TRUE);
    إذا (wp == عدم)
	أرجع عدم;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_چوکھٹا(wp);
    أرجع wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    فراغ
win_init_popup_win(win_T *wp, buf_T *buf)
{
    wp->w_بفر = buf;
    ++buf->b_nwindows;
    win_init_خالی_تھاواں(wp); // set cursor and topline to safe values

    // Make sure w_localdir is NULL to avoid a chdir() in win_enter_ext().
    VIM_CLEAR(wp->w_localdir);
}

/*
 * Allocate the first window or the first window in a new tab page.
 * When "oldwin" is NULL create an empty buffer for it.
 * When "oldwin" is not NULL copy info from it to the new window.
 * Return FAIL when something goes wrong (out of memory).
 */
    ساكن صحيح
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(عدم, FALSE);
    إذا (curwin == عدم)
	أرجع FAIL;
    إذا (oldwin == عدم)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(عدم, عدم, 1L, BLN_LISTED);
	إذا (curwin == عدم || curbuf == عدم)
	    أرجع FAIL;
	curwin->w_بفر = curbuf;
#جے_تعریف_شدہ FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#جے_انت
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    وإلا
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_چوکھٹا(curwin);
    إذا (curwin->w_چوکھٹا == عدم)
	أرجع FAIL;
    topframe = curwin->w_چوکھٹا;
    topframe->fr_چوڑائی = COLUMNS_WITHOUT_TPL();
    topframe->fr_اچائی = صفاں - p_ch;

    أرجع OK;
}

/*
 * Create a frame for window "wp".
 */
    ساكن فراغ
new_چوکھٹا(win_T *wp)
{
    چوکھٹا_T *frp = ALLOC_CLEAR_ONE(چوکھٹا_T);

    wp->w_چوکھٹا = frp;
    إذا (frp == عدم)
	أرجع;
    frp->fr_layout = FR_پتی;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    فراغ
win_init_ناپ(فراغ)
{
    firstwin->w_اچائی = صفاں_AVAIL;
    firstwin->w_prev_اچائی = صفاں_AVAIL;
    topframe->fr_اچائی = صفاں_AVAIL;
    firstwin->w_چوڑائی = topframe->fr_چوڑائی;
}

/*
 * Allocate a new tabpage_T and init the values.
 * Returns NULL when out of memory.
 */
    ساكن tabpage_T *
alloc_tabpage(فراغ)
{
    tabpage_T	*tp;
#جے_تعریف_شدہ FEAT_GUI
    صحيح		i;
#جے_انت


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    إذا (tp == عدم)
	أرجع عدم;

#جے_تعریف_شدہ FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    إذا (tp->tp_vars == عدم)
    {
	vim_free(tp);
	أرجع عدم;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#جے_انت

#جے_تعریف_شدہ FEAT_GUI
    لكل (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#جے_انت
#جے_تعریف_شدہ FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#جے_انت
    tp->tp_ch_ورتیا = p_ch;

    أرجع tp;
}

    فراغ
free_tabpage(tabpage_T *tp)
{
    صحيح idx;

#جے_تعریف_شدہ FEAT_DIFF
    diff_clear(tp);
#جے_انت
#جے_تعریف_شدہ FEAT_PROP_POPUP
    طالما (tp->tp_پہلا_popupwin != عدم)
	popup_close_tabpage(tp, tp->tp_پہلا_popupwin->w_id, TRUE);
#جے_انت
    لكل (idx = 0; idx < SNAP_گنتی; ++idx)
	clear_snapshot(tp, idx);
#جے_تعریف_شدہ FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#جے_انت

    إذا (tp == lastused_tabpage)
	lastused_tabpage = عدم;
#جے_تعریف_شدہ FEAT_TABPANEL
    tabpanel_forget_tabpage(tp);
#جے_انت

    vim_free(tp->tp_localdir);
    vim_free(tp->tp_prevdir);

#جے_تعریف_شدہ FEAT_PYTHON
    python_tabpage_free(tp);
#جے_انت

#جے_تعریف_شدہ FEAT_PYTHON3
    python3_tabpage_free(tp);
#جے_انت

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
    صحيح
win_new_tabpage(صحيح after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    صحيح		n;
#إذا defined(FEAT_TABPANEL)
    صحيح prev_columns = COLUMNS_WITHOUT_TPL();
#جے_انت

    إذا (cmdwin_type != 0)
    {
	emsg(_(e_invalid_وچ_cmdline_کھڑکی));
	أرجع FAIL;
    }
    إذا (کھڑکی_layout_locked(CMD_tabnew))
	أرجع FAIL;

    newtp = alloc_tabpage();
    إذا (newtp == عدم)
	أرجع FAIL;

    // Remember the current windows in this Tab page.
    إذا (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	أرجع FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == عدم)
				    ? عدم : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    إذا (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	إذا (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_اگلا = پہلا_tabpage;
	    پہلا_tabpage = newtp;
	}
	وإلا
	{
	    إذا (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		لكل (tp = پہلا_tabpage; tp->tp_اگلا != عدم
					       && n < after; tp = tp->tp_اگلا)
		    ++n;
	    }
	    newtp->tp_اگلا = tp->tp_اگلا;
	    tp->tp_اگلا = newtp;
	}
	newtp->tp_firstwin = newtp->tp_lastwin = newtp->tp_curwin = curwin;

	win_init_ناپ();
	firstwin->w_winrow = tabline_اچائی();
	firstwin->w_prev_winrow = firstwin->w_winrow;
	win_comp_scroll(curwin);

	newtp->tp_topframe = topframe;
	آخری_حالت(FALSE);

	lastused_tabpage = prev_tp;

#إذا defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#جے_انت
#جے_تعریف_شدہ FEAT_کم_CHANNEL
	entering_کھڑکی(curwin);
#جے_انت
#إذا defined(FEAT_TABPANEL)
	إذا (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    صحيح		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		إذا (tp2 != curtab)
		{
		    چوکھٹا_new_چوڑائی(tp2->tp_topframe, w, FALSE, TRUE);
		    إذا (!چوکھٹا_check_چوڑائی(tp2->tp_topframe, w))
			چوکھٹا_new_چوڑائی(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#جے_انت
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, عدم, عدم, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, عدم, عدم, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, عدم, عدم, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, عدم, عدم, FALSE, curbuf);
	أرجع OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    أرجع FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    ساكن صحيح
may_open_tabpage(فراغ)
{
    صحيح		n = (cmdmod.cmod_tab == 0)
				       ? postponed_ونڈو_tab : cmdmod.cmod_tab;

    إذا (n == 0)
	أرجع FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_ونڈو_tab = 0;
    أرجع win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    صحيح
make_tabpages(صحيح maxcount)
{
    صحيح		گنتی = maxcount;
    صحيح		todo;

    // Limit to 'tabpagemax' tabs.
    إذا (گنتی > p_tpm)
	گنتی = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    بلاک_autocmds();

    لكل (todo = گنتی - 1; todo > 0; --todo)
	إذا (win_new_tabpage(0) == FAIL)
	    اكسر;

    unblock_autocmds();

    // return actual number of tab pages
    أرجع (گنتی - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    صحيح
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	إذا (tp == tpc)
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    صحيح
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	إذا (tp == tpc)
	{
	    FOR_ALL_WINDOWS_وچ_TAB(tp, wp)
	    {
		إذا (win_valid_any_tab(wp))
		    أرجع TRUE;
	    }
	    أرجع FALSE;
	}
    }
    // shouldn't happen
    أرجع FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    فراغ
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    إذا (tab == پہلا_tabpage)
    {
	پہلا_tabpage = tab->tp_اگلا;
	ptp = پہلا_tabpage;
    }
    وإلا
    {
	لكل (ptp = پہلا_tabpage; ptp != عدم && ptp->tp_اگلا != tab;
							    ptp = ptp->tp_اگلا)
	    ;
	أكد(ptp != عدم);
	ptp->tp_اگلا = tab->tp_اگلا;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
لبھو_tabpage(صحيح n)
{
    tabpage_T	*tp;
    صحيح		i = 1;

    إذا (n == 0)
	أرجع curtab;

    لكل (tp = پہلا_tabpage; tp != عدم && i != n; tp = tp->tp_اگلا)
	++i;
    أرجع tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    صحيح
tabpage_اشاریہ(tabpage_T *ftp)
{
    صحيح		i = 1;
    tabpage_T	*tp;

    لكل (tp = پہلا_tabpage; tp != عدم && tp != ftp; tp = tp->tp_اگلا)
	++i;
    أرجع i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    ساكن صحيح
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    صحيح		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#جے_تعریف_شدہ FEAT_کم_CHANNEL
    leaving_کھڑکی(curwin);
#جے_انت
    reset_VIsual_and_resel();	// stop Visual mode
    إذا (trigger_leave_autocmds)
    {
	إذا (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, عدم, عدم, FALSE, curbuf);
	    إذا (curtab != tp)
		أرجع FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, عدم, عدم, FALSE, curbuf);
	إذا (curtab != tp)
	    أرجع FAIL;
	apply_autocmds(EVENT_TABLEAVE, عدم, عدم, FALSE, curbuf);
	إذا (curtab != tp)
	    أرجع FAIL;
    }

    reset_dragwin();
#إذا defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    إذا (gui.وچ_use)
	gui_remove_scrollbars();
#جے_انت
#جے_تعریف_شدہ FEAT_IMAGE
    // Clear all image placements, they will be redrawn again if needed.
    clear_all_image_placements();
#جے_انت
    tp->tp_curwin = curwin;
    tp->tp_prevwin = prevwin;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_old_صفاں = صفاں_AVAIL;
    إذا (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_چوڑائی;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = عدم;
    lastwin = عدم;
    أرجع OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    ساكن فراغ
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    صحيح		trigger_enter_autocmds,
    صحيح		trigger_leave_autocmds)
{
    صحيح		old_off = tp->tp_firstwin->w_winrow;
    win_T	*اگلا_prevwin = tp->tp_prevwin;
    tabpage_T	*آخری_tab = curtab;

    use_tabpage(tp);

    إذا (p_ch != curtab->tp_ch_ورتیا)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	صحيح new_ch = curtab->tp_ch_ورتیا;
	curtab->tp_ch_ورتیا = p_ch;
	command_چوکھٹا_اچائی = FALSE;
	سیٹ_option_قدر((char_u *)"cmdheight", new_ch, عدم, 0);
	command_چوکھٹا_اچائی = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (فراغ)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
		  | (trigger_enter_autocmds ? WEE_TRIGGER_ENTER_AUTOCMDS : 0)
		  | (trigger_leave_autocmds ? WEE_TRIGGER_LEAVE_AUTOCMDS : 0));
    prevwin = اگلا_prevwin;

    آخری_حالت(FALSE);		// status line may appear or disappear
    win_comp_pos();		// recompute w_winrow for all windows
#جے_تعریف_شدہ FEAT_DIFF
    diff_need_scrollbind = TRUE;
#جے_انت

    // If there was a click in a window, it won't be usable for a following
    // drag.
    reset_dragwin();

    // The tabpage line may have appeared or disappeared, may need to resize
    // the frames for that.  When the Vim window was resized need to update
    // frame sizes too.
    إذا (curtab->tp_old_صفاں != صفاں_AVAIL || (old_off != firstwin->w_winrow
#جے_تعریف_شدہ FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#جے_انت
		))
	shell_new_صفاں();
    إذا (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	إذا (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_چوڑائی;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	وإلا
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = آخری_tab;

#إذا defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#جے_انت

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    إذا (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, عدم, عدم, FALSE, curbuf);
	إذا (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, عدم, عدم, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    فراغ
goto_tabpage(صحيح n)
{
    tabpage_T	*tp = عدم;  // shut up compiler
    tabpage_T	*ttp;
    صحيح		i;

    إذا (متن_locked())
    {
	// Not allowed when editing the command line.
	متن_locked_پیغام();
	أرجع;
    }

    // If there is only one it can't work.
    إذا (پہلا_tabpage->tp_اگلا == عدم)
    {
	إذا (n > 1)
	    beep_flush();
	أرجع;
    }

    إذا (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	إذا (curtab->tp_اگلا == عدم)
	    tp = پہلا_tabpage;
	وإلا
	    tp = curtab->tp_اگلا;
    }
    وإلا إذا (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	لكل (i = n; i < 0; ++i)
	{
	    لكل (tp = پہلا_tabpage; tp->tp_اگلا != ttp && tp->tp_اگلا != عدم;
		    tp = tp->tp_اگلا)
		;
	    ttp = tp;
	}
    }
    وإلا إذا (n == 9999)
    {
	// Go to last tab page.
	لكل (tp = پہلا_tabpage; tp->tp_اگلا != عدم; tp = tp->tp_اگلا)
	    ;
    }
    وإلا
    {
	// Go to tab page "n".
	tp = لبھو_tabpage(n);
	إذا (tp == عدم)
	{
	    beep_flush();
	    أرجع;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#جے_تعریف_شدہ FEAT_GUI_TABLINE
    إذا (gui_use_tabline())
	gui_mch_سیٹ_curtab(tabpage_اشاریہ(curtab));
#جے_انت
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    فراغ
goto_tabpage_tp(
    tabpage_T	*tp,
    صحيح		trigger_enter_autocmds,
    صحيح		trigger_leave_autocmds)
{
    إذا (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    سیٹ_keep_پیغام(عدم, 0);

    skip_win_fix_scroll = TRUE;
    إذا (tp != curtab && leave_tabpage(tp->tp_curwin->w_بفر,
					trigger_leave_autocmds) == OK)
    {
	إذا (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	وإلا
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    صحيح
goto_tabpage_lastused(فراغ)
{
    إذا (!valid_tabpage(lastused_tabpage))
	أرجع FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    أرجع OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    فراغ
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    إذا (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#جے_تعریف_شدہ FEAT_GUI_TABLINE
	إذا (gui_use_tabline())
	    gui_mch_سیٹ_curtab(tabpage_اشاریہ(curtab));
#جے_انت
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    فراغ
tabpage_move(صحيح nr)
{
    صحيح		n = 1;
    tabpage_T	*tp, *tp_dst;

    إذا (پہلا_tabpage->tp_اگلا == عدم)
	أرجع;

    إذا (tabpage_move_disallowed)
	أرجع;

    لكل (tp = پہلا_tabpage; tp->tp_اگلا != عدم && n < nr; tp = tp->tp_اگلا)
	++n;

    إذا (tp == curtab || (nr > 0 && tp->tp_اگلا != عدم
						    && tp->tp_اگلا == curtab))
	أرجع;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    إذا (curtab == پہلا_tabpage)
	پہلا_tabpage = curtab->tp_اگلا;
    وإلا
    {
	FOR_ALL_TABPAGES(tp)
	    إذا (tp->tp_اگلا == curtab)
		اكسر;
	إذا (tp == عدم)	// "cannot happen"
	    أرجع;
	tp->tp_اگلا = curtab->tp_اگلا;
    }

    // Re-insert it at the specified position.
    إذا (nr <= 0)
    {
	curtab->tp_اگلا = پہلا_tabpage;
	پہلا_tabpage = curtab;
    }
    وإلا
    {
	curtab->tp_اگلا = tp_dst->tp_اگلا;
	tp_dst->tp_اگلا = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#جے_انت
}


/*
 * Go to another window.
 * When jumping to another buffer, stop Visual mode.  Do this before
 * changing windows so we can yank the selection into the '*' register.
 * (note: this may trigger ModeChanged autocommand!)
 * When jumping to another window on the same buffer, adjust its cursor
 * position to keep the same Visual area.
 */
    فراغ
win_goto(win_T *wp)
{
#جے_تعریف_شدہ FEAT_CONCEAL
    win_T	*owp = curwin;
#جے_انت

#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (ERROR_IF_ANY_POPUP_کھڑکی)
	أرجع;
    إذا (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_منزل_enter_popup_کھڑکی));
	أرجع;
    }
#جے_انت
    إذا (متن_or_buf_locked())
    {
	beep_flush();
	أرجع;
    }

    إذا (wp->w_بفر != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    وإلا إذا (VIsual_سرگرم)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    إذا (!win_valid(wp))
	أرجع;

#جے_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#جے_انت
    win_enter(wp, TRUE);

#جے_تعریف_شدہ FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    إذا (win_valid(owp) && owp->w_p_cole > 0 && !پیغام_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    إذا (curwin->w_p_cole > 0 && !پیغام_scrolled)
	need_cursor_سطر_redraw = TRUE;
#جے_انت
}

#إذا defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_لبھو_nr(صحيح winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (--winnr == 0)
	    اكسر;
    أرجع wp;
}
#جے_انت

#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_لبھو_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    إذا (wp == win)
		أرجع tp;
    أرجع عدم;
}
#جے_انت

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, صحيح اتے, لما گنتی)
{
    چوکھٹا_T	*fr;
    چوکھٹا_T	*nfr;
    چوکھٹا_T	*foundfr;

#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (popup_is_popup(wp))
	// popups don't have neighbors.
	أرجع عدم;
#جے_انت
    foundfr = wp->w_چوکھٹا;
    طالما (گنتی--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	لكل (;;)
	{
	    إذا (fr == tp->tp_topframe)
		اذهب سرا;
	    إذا (اتے)
		nfr = fr->fr_prev;
	    وإلا
		nfr = fr->fr_اگلا;
	    إذا (fr->fr_ماپا->fr_layout == FR_خانہ && nfr != عدم)
		اكسر;
	    fr = fr->fr_ماپا;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	لكل (;;)
	{
	    إذا (nfr->fr_layout == FR_پتی)
	    {
		foundfr = nfr;
		اكسر;
	    }
	    fr = nfr->fr_بچہ;
	    إذا (nfr->fr_layout == FR_صف)
	    {
		// Find the frame at the cursor row.
		طالما (fr->fr_اگلا != عدم
			&& frame2win(fr)->w_wincol + fr->fr_چوڑائی
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_اگلا;
	    }
	    إذا (nfr->fr_layout == FR_خانہ && اتے)
		طالما (fr->fr_اگلا != عدم)
		    fr = fr->fr_اگلا;
	    nfr = fr;
	}
    }
سرا:
    أرجع foundfr != عدم ? foundfr->fr_win : عدم;
}

/*
 * Move to window above or below "count" times.
 */
    ساكن فراغ
win_goto_ver(
    صحيح		اتے,		// TRUE to go to win above
    لما	گنتی)
{
    win_T	*win;

#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (ERROR_IF_رکن_POPUP_کھڑکی)
	أرجع;
#جے_انت
    win = win_vert_neighbor(curtab, curwin, اتے, گنتی);
    إذا (win != عدم)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, صحيح بایاں, لما گنتی)
{
    چوکھٹا_T	*fr;
    چوکھٹا_T	*nfr;
    چوکھٹا_T	*foundfr;

#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (popup_is_popup(wp))
	// popups don't have neighbors.
	أرجع عدم;
#جے_انت
    foundfr = wp->w_چوکھٹا;
    طالما (گنتی--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	لكل (;;)
	{
	    إذا (fr == tp->tp_topframe)
		اذهب سرا;
	    إذا (بایاں)
		nfr = fr->fr_prev;
	    وإلا
		nfr = fr->fr_اگلا;
	    إذا (fr->fr_ماپا->fr_layout == FR_صف && nfr != عدم)
		اكسر;
	    fr = fr->fr_ماپا;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	لكل (;;)
	{
	    إذا (nfr->fr_layout == FR_پتی)
	    {
		foundfr = nfr;
		اكسر;
	    }
	    fr = nfr->fr_بچہ;
	    إذا (nfr->fr_layout == FR_خانہ)
	    {
		// Find the frame at the cursor row.
		طالما (fr->fr_اگلا != عدم
			&& frame2win(fr)->w_winrow + fr->fr_اچائی
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_اگلا;
	    }
	    إذا (nfr->fr_layout == FR_صف && بایاں)
		طالما (fr->fr_اگلا != عدم)
		    fr = fr->fr_اگلا;
	    nfr = fr;
	}
    }
سرا:
    أرجع foundfr != عدم ? foundfr->fr_win : عدم;
}

/*
 * Move to left or right window.
 */
    ساكن فراغ
win_goto_hor(
    صحيح		بایاں,		// TRUE to go to left win
    لما	گنتی)
{
    win_T	*win;

#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (ERROR_IF_رکن_POPUP_کھڑکی)
	أرجع;
#جے_انت
    win = win_horz_neighbor(curtab, curwin, بایاں, گنتی);
    إذا (win != عدم)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    فراغ
win_enter(win_T *wp, صحيح undo_sync)
{
    (فراغ)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    فراغ
win_fix_current_dir(فراغ)
{
    إذا (curwin->w_localdir != عدم || curtab->tp_localdir != عدم)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	إذا (globaldir == عدم)
	{
	    char_u	cwd[MAXPATHL];

	    إذا (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	إذا (curwin->w_localdir != عدم)
	    dirname = curwin->w_localdir;
	وإلا
	    dirname = curtab->tp_localdir;

	إذا (mch_chdir((حرف *)dirname) == 0)
	{
	    آخری_chdir_reason = عدم;
	    shorten_fnames(TRUE);
	}
    }
    وإلا إذا (globaldir != عدم)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((حرف *)globaldir);
	VIM_CLEAR(globaldir);
	آخری_chdir_reason = عدم;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    ساكن صحيح
win_enter_ext(win_T *wp, صحيح flags)
{
    صحيح		other_بفر = FALSE;
    صحيح		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    صحيح		did_decrement = FALSE;

    إذا (wp == curwin && curwin_invalid == 0)	// nothing to do
	أرجع FALSE;

#جے_تعریف_شدہ FEAT_کم_CHANNEL
    إذا (curwin_invalid == 0)
	leaving_کھڑکی(curwin);
#جے_انت

    إذا (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	إذا (wp->w_بفر != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, عدم, عدم, FALSE, curbuf);
	    other_بفر = TRUE;
	    إذا (!win_valid(wp))
		أرجع FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, عدم, عدم, FALSE, curbuf);
	إذا (!win_valid(wp))
	    أرجع FALSE;
#جے_تعریف_شدہ FEAT_EVAL
	// autocmds may abort script processing
	إذا (aborting())
	    أرجع FALSE;
#جے_انت
    }

    // sync undo before leaving the current buffer
    إذا ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_بفر)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    إذا (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    إذا (wp->w_بفر != curbuf)
	buf_copy_options(wp->w_بفر, BCO_ENTER | BCO_NOHELP);
    إذا (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_حالت = true;
    }
    curwin = wp;
    curbuf = wp->w_بفر;
    check_cursor();
    إذا (!virtual_سرگرم())
	curwin->w_cursor.coladd = 0;
    إذا (*p_spk == 'c')		// assume cursor position needs updating
	بدلیا_سطر_abv_curs();
    وإلا
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (منوال_عام|منوال_CMDLINE|منوال_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#جے_تعریف_شدہ سنیہا_قطار
    إذا (flags & WEE_ALLOW_PARSE_MESSAGES)
    {
	--dont_parse_messages;
	did_decrement = TRUE;
    }
#جے_انت

    win_fix_current_dir();

#جے_تعریف_شدہ FEAT_کم_CHANNEL
    entering_کھڑکی(curwin);
#جے_انت
    // Careful: autocommands may close the window and make "wp" invalid
    إذا (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, عدم, عدم, FALSE, curbuf);
    إذا (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, عدم, عدم, FALSE, curbuf);
	إذا (other_بفر)
	    apply_autocmds(EVENT_BUFENTER, عدم, عدم, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_حالت = true;
#جے_تعریف_شدہ FEAT_TERMINAL
    إذا (bt_terminal(curwin->w_بفر))
	// terminal is likely in another mode
	redraw_منوال = TRUE;
#جے_انت
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#جے_انت
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    إذا (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    إذا (curwin->w_اچائی < p_wh && !curwin->w_p_wfh
#جے_تعریف_شدہ FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#جے_انت
	    )
	win_setheight((صحيح)p_wh);
    وإلا إذا (curwin->w_اچائی == 0)
	win_setheight(1);

    // set window width to desired minimal value
    إذا (curwin->w_چوڑائی < p_wiw && !curwin->w_p_wfw)
	win_setwidth((صحيح)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    أرجع did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = عدم;

    إذا (curwin->w_بفر == buf)
	wp = curwin;
    وإلا
	FOR_ALL_WINDOWS(wp)
	    إذا (wp->w_بفر == buf)
		اكسر;
    إذا (wp != عدم)
	win_enter(wp, FALSE);
    أرجع wp;
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

    إذا (wp != عدم)
	أرجع wp;

    FOR_ALL_TABPAGES(tp)
	إذا (tp != curtab)
	{
	    FOR_ALL_WINDOWS_وچ_TAB(tp, wp)
		إذا (wp->w_بفر == buf)
		    اكسر;
	    إذا (wp != عدم)
	    {
		goto_tabpage_win(tp, wp);
		إذا (curwin != wp)
		    wp = عدم;	// something went wrong
		اكسر;
	    }
	}
    أرجع wp;
}

ساكن صحيح آخری_win_id = سبھ_توں_نیواں_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    ساكن win_T *
win_alloc(win_T *after, صحيح hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    إذا (new_wp == عدم)
	أرجع عدم;

    إذا (win_alloc_سطریں(new_wp) == FAIL)
    {
	vim_free(new_wp);
	أرجع عدم;
    }

    new_wp->w_id = ++آخری_win_id;

#جے_تعریف_شدہ FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    إذا (new_wp->w_vars == عدم)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	أرجع عدم;
    }
    init_var_dict(new_wp->w_vars, &new_wp->w_winvar, VAR_SCOPE);
#جے_انت

    // Don't execute autocommands while the window is not properly
    // initialized yet.  gui_create_scrollbar() may trigger a FocusGained
    // event.
    بلاک_autocmds();

    /*
     * link the window in the window list
     */
    إذا (!hidden)
	win_append(after, new_wp);
    new_wp->w_wincol = TPL_LCOL();
    new_wp->w_چوڑائی = COLUMNS_WITHOUT_TPL();

    // position the display and the cursor at the top of the file.
    new_wp->w_topline = 1;
#جے_تعریف_شدہ FEAT_DIFF
    new_wp->w_topfill = 0;
#جے_انت
    new_wp->w_botline = 2;
    new_wp->w_cursor.lnum = 1;
    new_wp->w_scbind_pos = 1;

    // use global option value for global-local options
    new_wp->w_allbuf_opt.wo_so = new_wp->w_p_so = -1;
    new_wp->w_allbuf_opt.wo_sop = new_wp->w_p_sop = -1;
    new_wp->w_allbuf_opt.wo_siso = new_wp->w_p_siso = -1;

    // We won't calculate w_fraction until resizing the window
    new_wp->w_fraction = 0;
    new_wp->w_prev_fraction_صف = -1;

#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_بایاں],
		SBAR_بایاں, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_دایاں],
		SBAR_دایاں, new_wp);
    }
#جے_انت
#جے_تعریف_شدہ FEAT_FOLDING
    foldInitWin(new_wp);
#جے_انت
    unblock_autocmds();
#جے_تعریف_شدہ FEAT_تلاش_EXTRA
    new_wp->w_اگلا_میل_id = 1000;  // up to 1000 can be picked by the user
#جے_انت
    أرجع new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    ساكن فراغ
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    صحيح		i;
    buf_T	*buf;
    wininfo_T	*wip;

#جے_تعریف_شدہ FEAT_FOLDING
    clearFolding(wp);
#جے_انت

    // reduce the reference count to the argument list.
    alist_unlink(wp->w_alist);

    // Don't execute autocommands while the window is halfway being deleted.
    // gui_mch_destroy_scrollbar() may trigger a FocusGained event.
    بلاک_autocmds();

#جے_تعریف_شدہ FEAT_LUA
    lua_کھڑکی_free(wp);
#جے_انت

#جے_تعریف_شدہ FEAT_MZSCHEME
    mzscheme_کھڑکی_free(wp);
#جے_انت

#جے_تعریف_شدہ FEAT_PERL
    perl_win_free(wp);
#جے_انت

#جے_تعریف_شدہ FEAT_PYTHON
    python_کھڑکی_free(wp);
#جے_انت

#جے_تعریف_شدہ FEAT_PYTHON3
    python3_کھڑکی_free(wp);
#جے_انت

#جے_تعریف_شدہ FEAT_TCL
    tcl_کھڑکی_free(wp);
#جے_انت

#جے_تعریف_شدہ FEAT_RUBY
    ruby_کھڑکی_free(wp);
#جے_انت

    remove_highlight_overrides(wp->w_hl);
    vim_free(wp->w_hl);

    // Free statusline click regions.
    إذا (wp->w_stl_click != عدم)
    {
	لكل (i = 0; i < wp->w_stl_click_گنتی; i++)
	    vim_free(wp->w_stl_click[i].funcname);
	vim_free(wp->w_stl_click);
    }

    clear_winopt(&wp->w_onebuf_opt);
    clear_winopt(&wp->w_allbuf_opt);

    vim_free(wp->w_lcs_chars.multispace);
    vim_free(wp->w_lcs_chars.leadmultispace);

#جے_تعریف_شدہ FEAT_EVAL
    vars_clear(&wp->w_vars->dv_hashtab);	// free all w: variables
    hash_init(&wp->w_vars->dv_hashtab);
    unref_var_dict(wp->w_vars);
#جے_انت

    {
	tabpage_T	*ttp;

	إذا (prevwin == wp)
	    prevwin = عدم;
	FOR_ALL_TABPAGES(ttp)
	    إذا (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = عدم;
    }
    win_free_lsize(wp);

    لكل (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_اندراج(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    إذا (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    إذا (wip2 != wip && wip2->wi_win == عدم)
		    {
			إذا (wip2->wi_اگلا != عدم)
			    wip2->wi_اگلا->wi_prev = wip2->wi_prev;
			إذا (wip2->wi_prev == عدم)
			    buf->b_wininfo = wip2->wi_اگلا;
			وإلا
			    wip2->wi_prev->wi_اگلا = wip2->wi_اگلا;
			free_wininfo(wip2);
			اكسر;
		    }

		wip->wi_win = عدم;
	    }

#جے_تعریف_شدہ FEAT_تلاش_EXTRA
    clear_matches(wp);
#جے_انت

    free_jumplist(wp);

#جے_تعریف_شدہ FEAT_QUICKFIX
    qf_free_all(wp);
#جے_انت

#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_بایاں]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_دایاں]);
    }
#جے_انت // FEAT_GUI

#جے_تعریف_شدہ FEAT_MENU
    remove_winbar(wp);
#جے_انت
#جے_تعریف_شدہ FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    لكل (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    vim_free(wp->w_scrollbar_highlight);
    vim_free(wp->w_thumb_highlight);
    vim_free(wp->w_popup_title);
    list_unref(wp->w_popup_نقاب);
    vim_free(wp->w_popup_نقاب_cells);
#جے_انت

#جے_تعریف_شدہ FEAT_SYN_HL
    vim_free(wp->w_p_cc_خانے_گنتی);
#جے_انت

    إذا (win_valid_any_tab(wp))
	win_remove(wp, tp);
    إذا (autocmd_busy)
    {
	wp->w_اگلا = au_pending_free_win;
	au_pending_free_win = wp;
    }
    وإلا
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    صحيح
win_unlisted(win_T *wp)
{
    أرجع is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    فراغ
win_free_popup(win_T *win)
{
    إذا (win->w_بفر != عدم)
    {
	إذا (bt_popup(win->w_بفر))
	    win_close_بفر(win, DOBUF_WIPE_REUSE, FALSE);
	وإلا
	    close_بفر(win, win->w_بفر, 0, FALSE, FALSE, TRUE);

	إذا (win->w_بفر != عدم)
	    --win->w_بفر->b_nwindows;
    }
# إذا defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    إذا (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_چوکھٹا);
    win_free(win, عدم);
}
#جے_انت

/*
 * Append window "wp" in the window list after window "after".
 */
    ساكن فراغ
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    إذا (after == عدم)	    // after NULL is in front of the first
	before = firstwin;
    وإلا
	before = after->w_اگلا;

    wp->w_اگلا = before;
    wp->w_prev = after;
    إذا (after == عدم)
	firstwin = wp;
    وإلا
	after->w_اگلا = wp;
    إذا (before == عدم)
	lastwin = wp;
    وإلا
	before->w_prev = wp;
}

/*
 * Remove a window from the window list.
 */
    فراغ
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    إذا (wp->w_prev != عدم)
	wp->w_prev->w_اگلا = wp->w_اگلا;
    وإلا إذا (tp == عدم)
	firstwin = curtab->tp_firstwin = wp->w_اگلا;
    وإلا
	tp->tp_firstwin = wp->w_اگلا;

    إذا (wp->w_اگلا != عدم)
	wp->w_اگلا->w_prev = wp->w_prev;
    وإلا إذا (tp == عدم)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    وإلا
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    ساكن فراغ
چوکھٹا_append(چوکھٹا_T *after, چوکھٹا_T *frp)
{
    frp->fr_اگلا = after->fr_اگلا;
    after->fr_اگلا = frp;
    إذا (frp->fr_اگلا != عدم)
	frp->fr_اگلا->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    ساكن فراغ
چوکھٹا_داخل_کرو(چوکھٹا_T *before, چوکھٹا_T *frp)
{
    frp->fr_اگلا = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    إذا (frp->fr_prev != عدم)
	frp->fr_prev->fr_اگلا = frp;
    وإلا
	frp->fr_ماپا->fr_بچہ = frp;
}

/*
 * Remove a frame from a frame list.
 */
    ساكن فراغ
چوکھٹا_remove(چوکھٹا_T *frp)
{
    إذا (frp->fr_prev != عدم)
	frp->fr_prev->fr_اگلا = frp->fr_اگلا;
    وإلا
	frp->fr_ماپا->fr_بچہ = frp->fr_اگلا;
    إذا (frp->fr_اگلا != عدم)
	frp->fr_اگلا->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    صحيح
win_alloc_سطریں(win_T *wp)
{
    wp->w_سطریں_valid = 0;
    wp->w_سطریں = ALLOC_CLEAR_MULT(wline_T, صفاں);
    إذا (wp->w_سطریں == عدم)
	أرجع FAIL;
    أرجع OK;
}

/*
 * free lsize arrays for a window
 */
    فراغ
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    إذا (wp != عدم)
	VIM_CLEAR(wp->w_سطریں);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    فراغ
shell_new_صفاں(فراغ)
{
    صحيح		h = (صحيح)صفاں_AVAIL;

    إذا (firstwin == عدم)	// not initialized yet
	أرجع;
    إذا (h < چوکھٹا_minheight(topframe, عدم))
	h = چوکھٹا_minheight(topframe, عدم);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    چوکھٹا_new_اچائی(topframe, h, FALSE, TRUE, FALSE);
    إذا (!چوکھٹا_check_اچائی(topframe, h))
	چوکھٹا_new_اچائی(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_ورتیا = p_ch;

    إذا (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#جے_انت
#إذا 0
    // Disabled: don't want making the screen smaller make a window larger.
    إذا (p_ea)
	win_equal(curwin, FALSE, 'v');
#جے_انت
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    فراغ
shell_new_columns(فراغ)
{
    إذا (firstwin == عدم)	// not initialized yet
	أرجع;

#إذا defined(FEAT_TABPANEL)
    ساكن صحيح prev_tp_چوڑائی;
    ساكن صحيح prev_wincol;
    ساكن صحيح prev_fr_چوڑائی;
#جے_انت
    صحيح w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    چوکھٹا_new_چوڑائی(topframe, w, FALSE, TRUE);
    إذا (!چوکھٹا_check_چوڑائی(topframe, w))
	چوکھٹا_new_چوڑائی(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#إذا defined(FEAT_TABPANEL)
    صحيح tp_چوڑائی = tabpanel_چوڑائی();
    صحيح tp_چوڑائی_بدلیا = tp_چوڑائی != prev_tp_چوڑائی;
    // tabpanel width changed
    إذا (tp_چوڑائی_بدلیا && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    إذا (tp_چوڑائی_بدلیا
	    || (tp_چوڑائی > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_چوڑائی != prev_fr_چوڑائی)))
    {
	screen_fill(cmdline_صف, (صحيح)صفاں, 0, (صحيح)Columns, ' ', ' ', 0);
    }
    prev_tp_چوڑائی = tabpanel_چوڑائی();
    prev_wincol = firstwin->w_wincol;
    prev_fr_چوڑائی = topframe->fr_چوڑائی;
#جے_انت
    // Adjust offset for command line start column
    cmdline_خانہ_off = firstwin->w_wincol;
    cmdline_چوڑائی = topframe->fr_چوڑائی;
    comp_خانہ();
    إذا (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#جے_انت
#إذا 0
    // Disabled: don't want making the screen smaller make a window larger.
    إذا (p_ea)
	win_equal(curwin, FALSE, 'h');
#جے_انت
}

/*
 * Save the size of all windows in "gap".
 */
    فراغ
win_ناپ_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, حجم(صحيح), 1);
    إذا (ga_grow(gap, win_گنتی() * 2 + 1) == FAIL)
	أرجع;

    // first entry is the total lines available for windows
    ((صحيح *)gap->ga_انکڑے)[gap->ga_len++] = صفاں_AVAIL - آخری_stl_اچائی(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((صحيح *)gap->ga_انکڑے)[gap->ga_len++] =
	    wp->w_چوڑائی + wp->w_vsep_چوڑائی;
	((صحيح *)gap->ga_انکڑے)[gap->ga_len++] = wp->w_اچائی;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    فراغ
win_ناپ_restore(garray_T *gap)
{
    win_T	*wp;
    صحيح		i, j;

    إذا (win_گنتی() * 2 + 1 == gap->ga_len
	    && ((صحيح *)gap->ga_انکڑے)[0] == صفاں_AVAIL - آخری_stl_اچائی(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	لكل (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		چوکھٹا_setwidth(wp->w_چوکھٹا, ((صحيح *)gap->ga_انکڑے)[i++]);
		win_setheight_win(((صحيح *)gap->ga_انکڑے)[i++], wp);
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
    فراغ
win_comp_pos(فراغ)
{
    صحيح		صف = tabline_اچائی();
    صحيح		خانہ = TPL_LCOL();

    چوکھٹا_comp_pos(topframe, &صف, &خانہ);
#إذا defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اچائی();
#جے_انت
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    ساكن فراغ
چوکھٹا_comp_pos(چوکھٹا_T *topfrp, صحيح *صف, صحيح *خانہ)
{
    win_T	*wp;
    چوکھٹا_T	*frp;
    صحيح		startcol;
    صحيح		startrow;
    صحيح		h;

    wp = topfrp->fr_win;
    إذا (wp != عدم)
    {
	إذا (wp->w_winrow != *صف || wp->w_wincol != *خانہ)
	{
	    // position changed, redraw
	    wp->w_winrow = *صف;
	    wp->w_wincol = *خانہ;
	    redraw_win_later(wp, UPD_NOT_VALID);
	    wp->w_redr_حالت = true;
	}
	// WinBar will not show if the window height is zero
	h = VISIBLE_اچائی(wp) + wp->w_حالت_اچائی;
	*صف += h > topfrp->fr_اچائی ? topfrp->fr_اچائی : h;
	*خانہ += wp->w_چوڑائی + wp->w_vsep_چوڑائی;
    }
    وإلا
    {
	startrow = *صف;
	startcol = *خانہ;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	{
	    إذا (topfrp->fr_layout == FR_صف)
		*صف = startrow;	// all frames are at the same row
	    وإلا
		*خانہ = startcol;	// all frames are at the same col
	    چوکھٹا_comp_pos(frp, صف, خانہ);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    فراغ
win_ensure_ناپ(فراغ)
{
    إذا (curwin->w_اچائی == 0)
	win_setheight(1);
    إذا (curwin->w_چوڑائی == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    فراغ
win_setheight(صحيح اچائی)
{
    win_setheight_win(اچائی, curwin);
#إذا defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اچائی();
#جے_انت
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    فراغ
win_setheight_win(صحيح اچائی, win_T *win)
{
    إذا (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	إذا (اچائی < p_wmh)
	    اچائی = p_wmh;
	إذا (اچائی == 0)
	    اچائی = 1;
	اچائی += WINBAR_اچائی(curwin);
    }

    چوکھٹا_setheight(win->w_چوکھٹا, اچائی + win->w_حالت_اچائی);

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
    ساكن فراغ
چوکھٹا_setheight(چوکھٹا_T *curfrp, صحيح اچائی)
{
    صحيح		گنجائش;		// total number of lines available
    صحيح		take;		// number of lines taken from other windows
    صحيح		گنجائش_cmdline;	// lines available from cmdline
    صحيح		لگاتار;
    چوکھٹا_T	*frp;
    صحيح		h;
    صحيح		گنجائش_reserved;

    // If the height already is the desired value, nothing to do.
    إذا (curfrp->fr_اچائی == اچائی)
	أرجع;

    إذا (curfrp->fr_ماپا == عدم)
    {
	// topframe: can only change the command line height
	إذا (اچائی > 0)
	    چوکھٹا_new_اچائی(curfrp, اچائی, FALSE, FALSE, TRUE);
    }
    وإلا إذا (curfrp->fr_ماپا->fr_layout == FR_صف)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = چوکھٹا_minheight(curfrp->fr_ماپا, عدم);
	إذا (اچائی < h)
	    اچائی = h;
	چوکھٹا_setheight(curfrp->fr_ماپا, اچائی);
    }
    وإلا
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
	لكل (لگاتار = 1; لگاتار <= 2; ++لگاتار)
	{
	    گنجائش = 0;
	    گنجائش_reserved = 0;
	    FOR_ALL_چوکھٹے(frp, curfrp->fr_ماپا->fr_بچہ)
	    {
		إذا (frp != curfrp
			&& frp->fr_win != عدم
			&& frp->fr_win->w_p_wfh)
		    گنجائش_reserved += frp->fr_اچائی;
		گنجائش += frp->fr_اچائی;
		إذا (frp != curfrp)
		    گنجائش -= چوکھٹا_minheight(frp, عدم);
	    }
	    إذا (curfrp->fr_چوڑائی != topframe->fr_چوڑائی)
		گنجائش_cmdline = 0;
	    وإلا
	    {
		گنجائش_cmdline = صفاں - p_ch - (lastwin->w_winrow
						+ VISIBLE_اچائی(lastwin)
						+ lastwin->w_حالت_اچائی);
		إذا (گنجائش_cmdline < 0)
		    گنجائش_cmdline = 0;
	    }

	    إذا (اچائی <= گنجائش + گنجائش_cmdline)
		اكسر;
	    إذا (لگاتار == 2 || curfrp->fr_چوڑائی == topframe->fr_چوڑائی)
	    {
		اچائی = گنجائش + گنجائش_cmdline;
		اكسر;
	    }
	    چوکھٹا_setheight(curfrp->fr_ماپا, اچائی
		+ چوکھٹا_minheight(curfrp->fr_ماپا, NOWIN) - (صحيح)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = اچائی - curfrp->fr_اچائی;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	إذا (اچائی > گنجائش + گنجائش_cmdline - گنجائش_reserved)
	    گنجائش_reserved = گنجائش + گنجائش_cmdline - اچائی;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	إذا (take < 0 && گنجائش - curfrp->fr_اچائی <= گنجائش_reserved)
	    گنجائش_reserved = 0;

	إذا (take > 0 && گنجائش_cmdline > 0)
	{
	    // use lines from cmdline first
	    إذا (take < گنجائش_cmdline)
		گنجائش_cmdline = take;
	    take -= گنجائش_cmdline;
	    topframe->fr_اچائی += گنجائش_cmdline;
	}

	/*
	 * set the current frame to the new height
	 */
	چوکھٹا_new_اچائی(curfrp, اچائی, FALSE, FALSE, TRUE);

	/*
	 * First take lines from the frames after the current frame.  If
	 * that is not enough, takes lines from frames above the current
	 * frame.
	 */
	لكل (لگاتار = 0; لگاتار < 2; ++لگاتار)
	{
	    إذا (لگاتار == 0)
		frp = curfrp->fr_اگلا;	// 1st run: start with next window
	    وإلا
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    طالما (frp != عدم && take != 0)
	    {
		h = چوکھٹا_minheight(frp, عدم);
		إذا (گنجائش_reserved > 0
			&& frp->fr_win != عدم
			&& frp->fr_win->w_p_wfh)
		{
		    إذا (گنجائش_reserved >= frp->fr_اچائی)
			گنجائش_reserved -= frp->fr_اچائی;
		    وإلا
		    {
			إذا (frp->fr_اچائی - گنجائش_reserved > take)
			    گنجائش_reserved = frp->fr_اچائی - take;
			take -= frp->fr_اچائی - گنجائش_reserved;
			چوکھٹا_new_اچائی(frp, گنجائش_reserved, FALSE, FALSE,
									TRUE);
			گنجائش_reserved = 0;
		    }
		}
		وإلا
		{
		    إذا (frp->fr_اچائی - take < h)
		    {
			take -= frp->fr_اچائی - h;
			چوکھٹا_new_اچائی(frp, h, FALSE, FALSE, TRUE);
		    }
		    وإلا
		    {
			چوکھٹا_new_اچائی(frp, frp->fr_اچائی - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		إذا (لگاتار == 0)
		    frp = frp->fr_اگلا;
		وإلا
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    فراغ
win_setwidth(صحيح چوڑائی)
{
    win_setwidth_win(چوڑائی, curwin);
}

    فراغ
win_setwidth_win(صحيح چوڑائی, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    إذا (wp == curwin)
    {
	إذا (چوڑائی < p_wmw)
	    چوڑائی = p_wmw;
	إذا (چوڑائی == 0)
	    چوڑائی = 1;
    }
    وإلا إذا (چوڑائی < 0)
	چوڑائی = 0;

    چوکھٹا_setwidth(wp->w_چوکھٹا, چوڑائی + wp->w_vsep_چوڑائی);

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
    ساكن فراغ
چوکھٹا_setwidth(چوکھٹا_T *curfrp, صحيح چوڑائی)
{
    صحيح		گنجائش;		// total number of lines available
    صحيح		take;		// number of lines taken from other windows
    صحيح		لگاتار;
    چوکھٹا_T	*frp;
    صحيح		w;
    صحيح		گنجائش_reserved;

    // If the width already is the desired value, nothing to do.
    إذا (curfrp->fr_چوڑائی == چوڑائی)
	أرجع;

    إذا (curfrp->fr_ماپا == عدم)
	// topframe: can't change width
	أرجع;

    إذا (curfrp->fr_ماپا->fr_layout == FR_خانہ)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = چوکھٹا_minwidth(curfrp->fr_ماپا, عدم);
	إذا (چوڑائی < w)
	    چوڑائی = w;
	چوکھٹا_setwidth(curfrp->fr_ماپا, چوڑائی);
    }
    وإلا
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	لكل (لگاتار = 1; لگاتار <= 2; ++لگاتار)
	{
	    گنجائش = 0;
	    گنجائش_reserved = 0;
	    FOR_ALL_چوکھٹے(frp, curfrp->fr_ماپا->fr_بچہ)
	    {
		إذا (frp != curfrp
			&& frp->fr_win != عدم
			&& frp->fr_win->w_p_wfw)
		    گنجائش_reserved += frp->fr_چوڑائی;
		گنجائش += frp->fr_چوڑائی;
		إذا (frp != curfrp)
		    گنجائش -= چوکھٹا_minwidth(frp, عدم);
	    }

	    إذا (چوڑائی <= گنجائش)
		اكسر;
	    إذا (لگاتار == 2 || curfrp->fr_اچائی >= صفاں_AVAIL)
	    {
		چوڑائی = گنجائش;
		اكسر;
	    }
	    چوکھٹا_setwidth(curfrp->fr_ماپا, چوڑائی
		 + چوکھٹا_minwidth(curfrp->fr_ماپا, NOWIN) - (صحيح)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = چوڑائی - curfrp->fr_چوڑائی;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	إذا (چوڑائی > گنجائش - گنجائش_reserved)
	    گنجائش_reserved = گنجائش - چوڑائی;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	إذا (take < 0 && گنجائش - curfrp->fr_چوڑائی < گنجائش_reserved)
	    گنجائش_reserved = 0;

	/*
	 * set the current frame to the new width
	 */
	چوکھٹا_new_چوڑائی(curfrp, چوڑائی, FALSE, FALSE);

	/*
	 * First take lines from the frames right of the current frame.  If
	 * that is not enough, takes lines from frames left of the current
	 * frame.
	 */
	لكل (لگاتار = 0; لگاتار < 2; ++لگاتار)
	{
	    إذا (لگاتار == 0)
		frp = curfrp->fr_اگلا;	// 1st run: start with next window
	    وإلا
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    طالما (frp != عدم && take != 0)
	    {
		w = چوکھٹا_minwidth(frp, عدم);
		إذا (گنجائش_reserved > 0
			&& frp->fr_win != عدم
			&& frp->fr_win->w_p_wfw)
		{
		    إذا (گنجائش_reserved >= frp->fr_چوڑائی)
			گنجائش_reserved -= frp->fr_چوڑائی;
		    وإلا
		    {
			إذا (frp->fr_چوڑائی - گنجائش_reserved > take)
			    گنجائش_reserved = frp->fr_چوڑائی - take;
			take -= frp->fr_چوڑائی - گنجائش_reserved;
			چوکھٹا_new_چوڑائی(frp, گنجائش_reserved, FALSE, FALSE);
			گنجائش_reserved = 0;
		    }
		}
		وإلا
		{
		    إذا (frp->fr_چوڑائی - take < w)
		    {
			take -= frp->fr_چوڑائی - w;
			چوکھٹا_new_چوڑائی(frp, w, FALSE, FALSE);
		    }
		    وإلا
		    {
			چوکھٹا_new_چوڑائی(frp, frp->fr_چوڑائی - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		إذا (لگاتار == 0)
		    frp = frp->fr_اگلا;
		وإلا
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    فراغ
win_setminheight(فراغ)
{
    صحيح		گنجائش;
    صحيح		needed;
    صحيح		پہلا = TRUE;

    // loop until there is a 'winminheight' that is possible
    طالما (p_wmh > 0)
    {
	گنجائش = صفاں - p_ch;
	needed = min_صفاں_for_all_tabpages() - 1;  // 1 was added for the cmdline
	إذا (گنجائش >= needed)
	    اكسر;
	--p_wmh;
	إذا (پہلا)
	{
	    emsg(_(e_not_enough_گنجائش));
	    پہلا = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    فراغ
win_setminwidth(فراغ)
{
    صحيح		گنجائش;
    صحيح		needed;
    صحيح		پہلا = TRUE;

    // loop until there is a 'winminheight' that is possible
    طالما (p_wmw > 0)
    {
	گنجائش = topframe->fr_چوڑائی;
	needed = چوکھٹا_minwidth(topframe, عدم);
	إذا (گنجائش >= needed)
	    اكسر;
	--p_wmw;
	إذا (پہلا)
	{
	    emsg(_(e_not_enough_گنجائش));
	    پہلا = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    فراغ
win_drag_حالت_سطر(win_T *dragwin, صحيح وتھ)
{
    چوکھٹا_T	*curfr;
    چوکھٹا_T	*fr;
    صحيح		گنجائش;
    صحيح		اتے;	// if TRUE, drag status line up, otherwise down
    صحيح		n;

    fr = dragwin->w_چوکھٹا;
    curfr = fr;
    إذا (fr != topframe)		// more than one window
    {
	fr = fr->fr_ماپا;
	// When the parent frame is not a column of frames, its parent should
	// be.
	إذا (fr->fr_layout != FR_خانہ)
	{
	    curfr = fr;
	    إذا (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_ماپا;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    طالما (curfr != topframe && curfr->fr_اگلا == عدم)
    {
	إذا (fr != topframe)
	    fr = fr->fr_ماپا;
	curfr = fr;
	إذا (fr != topframe)
	    fr = fr->fr_ماپا;
    }

    إذا (وتھ < 0) // drag up
    {
	اتے = TRUE;
	وتھ = -وتھ;
	// sum up the room of the current frame and above it
	إذا (fr == curfr)
	{
	    // only one window
	    گنجائش = fr->fr_اچائی - چوکھٹا_minheight(fr, عدم);
	}
	وإلا
	{
	    گنجائش = 0;
	    لكل (fr = fr->fr_بچہ; ; fr = fr->fr_اگلا)
	    {
		گنجائش += fr->fr_اچائی - چوکھٹا_minheight(fr, عدم);
		إذا (fr == curfr)
		    اكسر;
	    }
	}
	fr = curfr->fr_اگلا;		// put fr at frame that grows
    }
    وإلا    // drag down
    {
	اتے = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	گنجائش = صفاں - cmdline_صف;
	إذا (curfr->fr_اگلا == عدم)
	    --گنجائش;
	وإلا
	    گنجائش -= p_ch;
	إذا (گنجائش < 0)
	    گنجائش = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_چوکھٹے(fr, curfr->fr_اگلا)
	    گنجائش += fr->fr_اچائی - چوکھٹا_minheight(fr, عدم);
	fr = curfr;			// put fr at window that grows
    }

    إذا (گنجائش < وتھ)		// Not enough room
	وتھ = گنجائش;		// Move as far as we can
    إذا (وتھ <= 0)
	أرجع;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    إذا (fr != عدم)
	چوکھٹا_new_اچائی(fr, fr->fr_اچائی + وتھ, اتے, FALSE, TRUE);

    إذا (اتے)
	fr = curfr;		// current frame gets smaller
    وإلا
	fr = curfr->fr_اگلا;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    طالما (fr != عدم && وتھ > 0)
    {
	n = چوکھٹا_minheight(fr, عدم);
	إذا (fr->fr_اچائی - وتھ <= n)
	{
	    وتھ -= fr->fr_اچائی - n;
	    چوکھٹا_new_اچائی(fr, n, !اتے, FALSE, TRUE);
	}
	وإلا
	{
	    چوکھٹا_new_اچائی(fr, fr->fr_اچائی - وتھ, !اتے, FALSE, TRUE);
	    اكسر;
	}
	إذا (اتے)
	    fr = fr->fr_prev;
	وإلا
	    fr = fr->fr_اگلا;
    }
    win_comp_pos();
    win_fix_scroll(TRUE);

    redraw_all_later(UPD_SOME_VALID);
    showmode();
}

/*
 * Separator line of dragwin is dragged "offset" lines right (negative is left).
 */
    فراغ
win_drag_vsep_سطر(win_T *dragwin, صحيح وتھ)
{
    چوکھٹا_T	*curfr;
    چوکھٹا_T	*fr;
    صحيح		گنجائش;
    صحيح		بایاں;	// if TRUE, drag separator line left, otherwise right
    صحيح		n;

    fr = dragwin->w_چوکھٹا;
    إذا (fr == topframe)		// only one window (cannot happen?)
	أرجع;
    curfr = fr;
    fr = fr->fr_ماپا;
    // When the parent frame is not a row of frames, its parent should be.
    إذا (fr->fr_layout != FR_صف)
    {
	إذا (fr == topframe)	// only a column of windows (cannot happen?)
	    أرجع;
	curfr = fr;
	fr = fr->fr_ماپا;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    طالما (curfr->fr_اگلا == عدم)
    {
	إذا (fr == topframe)
	    اكسر;
	curfr = fr;
	fr = fr->fr_ماپا;
	إذا (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_ماپا;
	}
    }

    إذا (وتھ < 0) // drag left
    {
	بایاں = TRUE;
	وتھ = -وتھ;
	// sum up the room of the current frame and left of it
	گنجائش = 0;
	لكل (fr = fr->fr_بچہ; ; fr = fr->fr_اگلا)
	{
	    گنجائش += fr->fr_چوڑائی - چوکھٹا_minwidth(fr, عدم);
	    إذا (fr == curfr)
		اكسر;
	}
	fr = curfr->fr_اگلا;		// put fr at frame that grows
    }
    وإلا    // drag right
    {
	بایاں = FALSE;
	// sum up the room of frames right of the current one
	گنجائش = 0;
	FOR_ALL_چوکھٹے(fr, curfr->fr_اگلا)
	    گنجائش += fr->fr_چوڑائی - چوکھٹا_minwidth(fr, عدم);
	fr = curfr;			// put fr at window that grows
    }

    إذا (گنجائش < وتھ)		// Not enough room
	وتھ = گنجائش;		// Move as far as we can
    إذا (وتھ <= 0)		// No room at all, quit.
	أرجع;
    إذا (fr == عدم)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	أرجع;

    // grow frame fr by offset lines
    چوکھٹا_new_چوڑائی(fr, fr->fr_چوڑائی + وتھ, بایاں, FALSE);

    // shrink other frames: current and at the left or at the right
    إذا (بایاں)
	fr = curfr;		// current frame gets smaller
    وإلا
	fr = curfr->fr_اگلا;	// next frame gets smaller

    طالما (fr != عدم && وتھ > 0)
    {
	n = چوکھٹا_minwidth(fr, عدم);
	إذا (fr->fr_چوڑائی - وتھ <= n)
	{
	    وتھ -= fr->fr_چوڑائی - n;
	    چوکھٹا_new_چوڑائی(fr, n, !بایاں, FALSE);
	}
	وإلا
	{
	    چوکھٹا_new_چوڑائی(fr, fr->fr_چوڑائی - وتھ, !بایاں, FALSE);
	    اكسر;
	}
	إذا (بایاں)
	    fr = fr->fr_prev;
	وإلا
	    fr = fr->fr_اگلا;
    }
    win_comp_pos();
    redraw_all_later(UPD_NOT_VALID);
}

#تعریف_کرو FRACTION_MULT	16384L

/*
 * Set wp->w_fraction for the current w_wrow and w_height.
 * Has no effect when the window is less than two lines.
 */
    فراغ
سیٹ_fraction(win_T *wp)
{
    إذا (wp->w_اچائی > 1)
	// When cursor is in the first line the percentage is computed as if
	// it's halfway that line.  Thus with two lines it is 25%, with three
	// lines 17%, etc.  Similarly for the last line: 75%, 83%, etc.
	wp->w_fraction = ((لما)wp->w_wrow * FRACTION_MULT
				     + FRACTION_MULT / 2) / (لما)wp->w_اچائی;
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
    ساكن فراغ
win_fix_scroll(صحيح resize)
{
    إذا (*p_spk == 'c')
	أرجع;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	إذا (wp->w_اچائی != wp->w_prev_اچائی)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    إذا (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_بفر->b_ml.ml_سطر_گنتی)
	    {
		صحيح diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_اچائی - wp->w_prev_اچائی);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		إذا (diff > 0)
		    cursor_تھلے_inner(wp, diff);
		وإلا
		    cursor_اتے_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_منزل_fraction(wp, wp->w_prev_اچائی);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		إذا (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    وإلا إذا (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_اچائی = wp->w_اچائی;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    إذا (!(get_real_state() & (منوال_عام|منوال_CMDLINE|منوال_TERMINAL)))
	win_fix_cursor(FALSE);
    وإلا إذا (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    ساكن فراغ
win_fix_cursor(صحيح عام_5)
{
    win_T	*wp = curwin;

    إذا (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_بفر->b_ml.ml_سطر_گنتی < wp->w_اچائی)
	أرجع;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    لما so = MIN(wp->w_اچائی / 2, get_scrolloff_قدر());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_تھلے_inner(wp, so);
    linenr_T چوٹی = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_اتے_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    إذا (lnum > bot && (wp->w_botline - wp->w_بفر->b_ml.ml_سطر_گنتی) != 1)
	nlnum = bot;
    وإلا إذا (lnum < چوٹی && wp->w_topline != 1)
	nlnum = (so == wp->w_اچائی / 2) ? bot : چوٹی;

    إذا (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	إذا (عام_5)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	وإلا  // Scroll instead when not in normal mode.
	{
	    wp->w_fraction = (nlnum == bot) ? FRACTION_MULT : 0;
	    scroll_منزل_fraction(wp, wp->w_prev_اچائی);
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
    فراغ
win_new_اچائی(win_T *wp, صحيح اچائی)
{
    صحيح		prev_اچائی = wp->w_اچائی;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    إذا (اچائی < 0)
	اچائی = 0;
    إذا (wp->w_اچائی == اچائی)
	أرجع;	    // nothing to do

    إذا (wp->w_اچائی > 0)
    {
	إذا (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	إذا (wp->w_اچائی != prev_اچائی)
	    أرجع;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	إذا (wp->w_wrow != wp->w_prev_fraction_صف)
	    سیٹ_fraction(wp);
    }

    wp->w_اچائی = اچائی;
    wp->w_redr_حالت = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    إذا (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	إذا (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_منزل_fraction(wp, prev_اچائی);
    }
}

    فراغ
scroll_منزل_fraction(win_T *wp, صحيح prev_اچائی)
{
    linenr_T	lnum;
    صحيح		sline, سطر_ناپ;
    صحيح		اچائی = wp->w_اچائی;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    إذا (اچائی > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (اچائی < wp->w_بفر->b_ml.ml_سطر_گنتی || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	إذا (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((لما)wp->w_fraction * (لما)اچائی - 1L)
							       / FRACTION_MULT;
	سطر_ناپ = plines_win_خانہ(wp, lnum, (لما)(wp->w_cursor.خانہ)) - 1;
	sline = wp->w_wrow - سطر_ناپ;

	إذا (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    صحيح صفاں_2 = plines_win(wp, lnum, FALSE);

	    إذا (sline > wp->w_اچائی - صفاں_2)
	    {
		sline = wp->w_اچائی - صفاں_2;
		wp->w_wrow -= صفاں_2 - سطر_ناپ;
	    }
	}

	إذا (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    صحيح		want_صف = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = سطر_ناپ;
	    إذا (wp->w_wrow >= wp->w_اچائی
				       && (wp->w_چوڑائی - win_خانہ_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_چوڑائی - win_خانہ_off(wp);

		--wp->w_wrow;
		طالما (wp->w_wrow > want_صف)
		{
		    skipcol += wp->w_چوڑائی - win_خانہ_off(wp) + win_خانہ_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	وإلا إذا (sline > 0)
	{
	    طالما (sline > 0 && lnum > 1)
	    {
#جے_تعریف_شدہ FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);
		إذا (lnum == 1)
		{
		    // first line in buffer is folded
		    سطر_ناپ = 1;
		    --sline;
		    اكسر;
		}
#جے_انت
		--lnum;
#جے_تعریف_شدہ FEAT_DIFF
		إذا (lnum == wp->w_topline)
		    سطر_ناپ = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		وإلا
#جے_انت
		    سطر_ناپ = plines_win(wp, lnum, TRUE);
		sline -= سطر_ناپ;
	    }

	    إذا (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#جے_تعریف_شدہ FEAT_FOLDING
		hasFoldingWin(wp, lnum, عدم, &lnum, TRUE, عدم);
#جے_انت
		lnum++;
		wp->w_wrow -= سطر_ناپ + sline;
	    }
	    وإلا إذا (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	سیٹ_topline(wp, lnum);
    }

    إذا (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    إذا (prev_اچائی > 0)
	wp->w_prev_fraction_صف = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    فراغ
win_new_چوڑائی(win_T *wp, صحيح چوڑائی)
{
    // Should we give an error if width < 0?
    wp->w_چوڑائی = چوڑائی < 0 ? 0 : چوڑائی;
    wp->w_سطریں_valid = 0;
    بدلیا_سطر_abv_curs_win(wp);
    invalidate_botline_win(wp);

    إذا (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_حالت = true;
}

    فراغ
win_comp_scroll(win_T *wp)
{
#إذا defined(FEAT_EVAL)
    صحيح old_w_p_scr = wp->w_p_scr;
#جے_انت

    wp->w_p_scr = ((بنانشانی)wp->w_اچائی >> 1);
    إذا (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#إذا defined(FEAT_EVAL)
    إذا (wp->w_p_scr != old_w_p_scr)
    {
	// Used by "verbose set scroll".
	wp->w_p_script_ctx[WV_SCROLL].sc_sid = SID_WINLAYOUT;
	wp->w_p_script_ctx[WV_SCROLL].sc_lnum = 0;
    }
#جے_انت
}

/*
 * Command_height: called whenever p_ch has been changed.
 */
    فراغ
command_اچائی(فراغ)
{
    صحيح		old_p_ch = curtab->tp_ch_ورتیا;

    // Find bottom frame with width of screen.
    چوکھٹا_T *frp = lastwin->w_چوکھٹا;
    طالما (frp->fr_چوڑائی != topframe->fr_چوڑائی && frp->fr_ماپا != عدم)
	frp = frp->fr_ماپا;

    // Avoid changing the height of a window with 'winfixheight' set.
    طالما (frp->fr_prev != عدم && frp->fr_layout == FR_پتی
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    طالما (p_ch > old_p_ch && command_چوکھٹا_اچائی)
    {
	إذا (frp == عدم)
	{
	    emsg(_(e_not_enough_گنجائش));
	    p_ch = old_p_ch;
	    اكسر;
	}
	صحيح h = MIN(p_ch - old_p_ch,
			frp->fr_اچائی - چوکھٹا_minheight(frp, عدم));
	چوکھٹا_جوڑو_اچائی(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    إذا (p_ch < old_p_ch && command_چوکھٹا_اچائی && frp != عدم)
	چوکھٹا_جوڑو_اچائی(frp, (صحيح)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_صف = صفاں - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    إذا (پیغام_scrolled == 0 && بھریا_screen)
    {
	screen_fill(cmdline_صف, (صحيح)صفاں, 0, (صحيح)Columns, ' ', ' ', 0);
	پیغام_صف = cmdline_صف;
    }

    // Use the value of p_ch that we remembered.  This is needed for when the
    // GUI starts up, we can't be sure in what order things happen.  And when
    // p_ch was changed in another tab page.
    curtab->tp_ch_ورتیا = p_ch;
    min_سیٹ_ch = p_ch;
}

/*
 * Resize frame "frp" to be "n" lines higher (negative for less high).
 * Also resize the frames it is contained in.
 */
    ساكن فراغ
چوکھٹا_جوڑو_اچائی(چوکھٹا_T *frp, صحيح n)
{
    چوکھٹا_new_اچائی(frp, frp->fr_اچائی + n, FALSE, FALSE, FALSE);
    لكل (;;)
    {
	frp = frp->fr_ماپا;
	إذا (frp == عدم)
	    اكسر;
	frp->fr_اچائی += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    فراغ
آخری_حالت(
    صحيح		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    آخری_حالت_rec(topframe, آخری_stl_اچائی(morewin) > 0);
#إذا defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اچائی();
#جے_انت
}

    ساكن فراغ
آخری_حالت_rec(چوکھٹا_T *fr, صحيح statusline)
{
    چوکھٹا_T	*fp;
    win_T	*wp;

    إذا (fr->fr_layout == FR_پتی)
    {
	wp = fr->fr_win;
	إذا (wp->w_حالت_اچائی != 0 && !statusline)
	{
	    // remove status line
	    win_new_اچائی(wp, wp->w_اچائی + wp->w_حالت_اچائی);
	    wp->w_حالت_اچائی = 0;
	    comp_خانہ();
	}
	وإلا إذا (wp->w_حالت_اچائی == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    طالما (fp->fr_اچائی <= چوکھٹا_minheight(fp, عدم))
	    {
		إذا (fp == topframe)
		{
		    emsg(_(e_not_enough_گنجائش));
		    أرجع;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		إذا (fp->fr_ماپا->fr_layout == FR_خانہ && fp->fr_prev != عدم)
		    fp = fp->fr_prev;
		وإلا
		    fp = fp->fr_ماپا;
	    }
	    wp->w_حالت_اچائی = statusline_اچائی(wp);
	    إذا (fp != fr)
	    {
		چوکھٹا_new_اچائی(fp, fp->fr_اچائی - wp->w_حالت_اچائی,
			FALSE, FALSE, FALSE);
		چوکھٹا_fix_اچائی(wp);
		win_comp_pos();
	    }
	    وإلا
		win_new_اچائی(wp, wp->w_اچائی - wp->w_حالت_اچائی);
	    comp_خانہ();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	إذا (مطلق_قدر(wp->w_اچائی - wp->w_prev_اچائی) == 1)
	    wp->w_prev_اچائی = wp->w_اچائی;
    }
    وإلا إذا (fr->fr_layout == FR_صف)
    {
	// vertically split windows, set status line for each one
	إذا (!statusline)
	{
	    FOR_ALL_چوکھٹے(fp, fr->fr_بچہ)
		آخری_حالت_rec(fp, statusline);
	}
	وإلا
	{
	    چوکھٹا_T	*fp2;
	    صحيح		max_stlh = 0;
	    صحيح		new_صف_اچائی;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_چوکھٹے(fp, fr->fr_بچہ)
		إذا (fp->fr_win != عدم && fp->fr_win->w_حالت_اچائی == 0)
		{
		    صحيح h = statusline_اچائی(fp->fr_win);
		    إذا (h > max_stlh)
			max_stlh = h;
		}
	    إذا (max_stlh == 0)
		أرجع;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    طالما (fp2->fr_اچائی - چوکھٹا_minheight(fp2, عدم) < max_stlh)
	    {
		إذا (fp2 == topframe)
		{
		    emsg(_(e_not_enough_گنجائش));
		    أرجع;
		}
		إذا (fp2->fr_ماپا->fr_layout == FR_خانہ
						&& fp2->fr_prev != عدم)
		    fp2 = fp2->fr_prev;
		وإلا
		    fp2 = fp2->fr_ماپا;
	    }

	    إذا (fp2 != fr)
	    {
		چوکھٹا_new_اچائی(fp2, fp2->fr_اچائی - max_stlh,
							FALSE, FALSE, FALSE);
		new_صف_اچائی = fr->fr_اچائی + max_stlh;
	    }
	    وإلا
		new_صف_اچائی = fr->fr_اچائی;

	    // Set status and content heights for all leaves.
	    FOR_ALL_چوکھٹے(fp, fr->fr_بچہ)
	    {
		إذا (fp->fr_win != عدم)
		{
		    wp = fp->fr_win;
		    إذا (wp->w_حالت_اچائی == 0)
		    {
			wp->w_حالت_اچائی = statusline_اچائی(wp);
			win_new_اچائی(wp,
				  new_صف_اچائی - wp->w_حالت_اچائی);
			چوکھٹا_fix_اچائی(wp);
			comp_خانہ();
			redraw_all_later(UPD_SOME_VALID);
			إذا (مطلق_قدر(wp->w_اچائی - wp->w_prev_اچائی) == 1)
			    wp->w_prev_اچائی = wp->w_اچائی;
		    }
		}
		وإلا
		    آخری_حالت_rec(fp, statusline); // nested frames
	    }

	    fr->fr_اچائی = new_صف_اچائی;
	    win_comp_pos();
	}
    }
    وإلا
    {
	// horizontally split window, set status line for last one
	لكل (fp = fr->fr_بچہ; fp->fr_اگلا != عدم; fp = fp->fr_اگلا)
	    ;
	آخری_حالت_rec(fp, statusline);
    }
}

#إذا defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    فراغ
update_stl_rendered_اچائی(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == عدم) ? p_stl : wp->w_p_stl;
    صحيح		opt_flags = (wp == عدم) ? 0 : OPT_LOCAL;
    صحيح		*اچائی = (wp == عدم) ? &stl_rendered_اچائی
						: &wp->w_stl_rendered_اچائی;

    *اچائی = get_stl_rendered_اچائی(wp == عدم ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    ساكن صحيح
چوکھٹا_لبھو_global_stlh_rec(چوکھٹا_T *frp, صحيح h)
{
    إذا (frp->fr_layout == FR_پتی)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	إذا (wp->w_اچائی > 0 && wp->w_حالت_اچائی > 0
		&& *wp->w_p_stlo == NUL)
	{
	    صحيح win_free_اچائی = frp->fr_اچائی - WINBAR_اچائی(wp);

	    إذا (win_free_اچائی - p_wmh < h)
		h = win_free_اچائی - p_wmh;
	}
    }
    وإلا
    {
	لكل (frp = frp->fr_بچہ; frp != عدم; frp = frp->fr_اگلا)
	    h = چوکھٹا_لبھو_global_stlh_rec(frp, h);
    }
    أرجع h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    ساكن فراغ
چوکھٹا_change_statusline_اچائی_rec(چوکھٹا_T *frp, صحيح global_stlh)
{
    إذا (frp->fr_layout == FR_پتی)
    {
	win_T  *wp = frp->fr_win;

	إذا (wp->w_اچائی > 0 && wp->w_حالت_اچائی > 0)
	{
	    صحيح win_free_اچائی = frp->fr_اچائی - WINBAR_اچائی(wp);

	    wp->w_حالت_اچائی = statusline_اچائی(wp);
	    إذا (wp->w_p_stlo == عدم || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		إذا (wp->w_حالت_اچائی > global_stlh)
		    wp->w_حالت_اچائی = global_stlh;
	    }
	    إذا (wp->w_حالت_اچائی > win_free_اچائی - p_wmh)
		wp->w_حالت_اچائی = win_free_اچائی - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    صحيح	     saved_topfill = wp->w_topfill;
# endif
	    win_new_اچائی(wp, win_free_اچائی - wp->w_حالت_اچائی);
	    wp->w_topline = saved_topline;
# ifdef FEAT_DIFF
	    wp->w_topfill = saved_topfill;
# endif
	    wp->w_valid &= ~(VALID_WROW | VALID_CROW
			     | VALID_BOTLINE | VALID_BOTLINE_AP);
	}
    }
    وإلا
    {
	لكل (frp = frp->fr_بچہ; frp != عدم; frp = frp->fr_اگلا)
	    چوکھٹا_change_statusline_اچائی_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    صحيح
چوکھٹا_change_statusline_اچائی(فراغ)
{
    tabpage_T	*tp;
    صحيح		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = چوکھٹا_لبھو_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	چوکھٹا_change_statusline_اچائی_rec(tp->tp_topframe, global_stlh);
    comp_خانہ();
    redraw_all_later(UPD_SOME_VALID);
    أرجع global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    فراغ
سیٹ_stlo_mh(صحيح mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    صحيح
statuslineopt_بدلیا(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    صحيح		l_stlo_fh = FALSE;
    صحيح		l_stlo_mh = 1;

    إذا (stlopt != عدم)
	p = stlopt;
    وإلا
	p = wp->w_p_stlo;

    طالما (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	إذا (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    إذا (l_stlo_mh < 1)
		أرجع FAIL;
	}
	وإلا إذا (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	إذا (*p != ',' && *p != NUL)
	    أرجع FAIL;
	إذا (*p == ',')
	    ++p;
    }

    إذا (wp == عدم)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    وإلا
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    أرجع OK;
}
#جے_انت

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    صحيح
statusline_اچائی(win_T *wp بن_ورتیا)
{
    صحيح stl_اچائی = 1;
#إذا defined(FEAT_STL_OPT)
    bool fixed;
    صحيح l_stl_rendered_اچائی;

    إذا (wp != عدم && *wp->w_p_stlo != NUL)
    {
	stl_اچائی = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_اچائی = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_اچائی
				    : stl_rendered_اچائی;
    }
    وإلا
    {
	stl_اچائی = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_اچائی = (wp != عدم && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_اچائی
				    : stl_rendered_اچائی;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    إذا (!fixed)
    {
	صحيح has_custom_stl = (*p_stl != NUL)
	    || (wp != عدم && *wp->w_p_stl != NUL);
	إذا (has_custom_stl)
	    stl_اچائی = MAX(1, MIN(l_stl_rendered_اچائی, stl_اچائی));
	وإلا
	    stl_اچائی = 1;
    }
#جے_انت

    أرجع stl_اچائی;
}

/*
 * Return the number of lines used by the tab page line.
 */
    صحيح
tabline_اچائی(فراغ)
{
#جے_تعریف_شدہ FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    إذا (gui_use_tabline())
	أرجع 0;
#جے_انت
    بدل (p_stal)
    {
	حالة 0: أرجع 0;
	حالة 1: أرجع (پہلا_tabpage->tp_اگلا == عدم) ? 0 : 1;
    }
    أرجع 1;
}

/*
 * Return the height of the last window's statusline.
 */
    صحيح
آخری_stl_اچائی(
    صحيح		morewin)	// pretend there are two or more windows
{
    أرجع (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_کھڑکی)))
		? حالت_اچائی_2 : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    صحيح
min_صفاں(فراغ)
{
    إذا (firstwin == عدم)	// not initialized yet
	أرجع MIN_سطریں;

    أرجع چوکھٹا_minheight(curtab->tp_topframe, عدم) + tabline_اچائی()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    صحيح
min_صفاں_for_all_tabpages(فراغ)
{
    صحيح		کل;
    tabpage_T	*tp;
    صحيح		n;

    إذا (firstwin == عدم)	// not initialized yet
	أرجع MIN_سطریں;

    کل = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = چوکھٹا_minheight(tp->tp_topframe, عدم);
	إذا (کل < n)
	    کل = n;
    }
    کل += tabline_اچائی();
    کل += MIN_CMDHEIGHT;
    أرجع کل;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    صحيح
only_one_کھڑکی(فراغ)
{
    صحيح		گنتی = 0;
    win_T	*wp;

#إذا defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    إذا (popup_is_popup(curwin))
	أرجع FALSE;
#جے_انت

    // If there is another tab page there always is another window.
    إذا (پہلا_tabpage->tp_اگلا != عدم)
	أرجع FALSE;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_بفر != عدم
		&& (!((bt_help(wp->w_بفر) && !bt_help(curbuf))
#جے_تعریف_شدہ FEAT_QUICKFIX
		    || wp->w_p_pvw
#جے_انت
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++گنتی;
    أرجع (گنتی <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    ساكن فراغ
check_lnums_both(صحيح do_curwin, صحيح nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	إذا ((do_curwin || wp != curwin) && wp->w_بفر == curbuf)
	{
	    صحيح need_adjust;

	    إذا (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی;
	    إذا (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
	    إذا (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_سطر_گنتی;
	    إذا (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_سطر_گنتی;
	    إذا (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    فراغ
check_lnums(صحيح do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    فراغ
check_lnums_nested(صحيح do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    فراغ
reset_lnums(فراغ)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	إذا (wp->w_بفر == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    إذا (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    إذا (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    إذا (wp->w_save_cursor.w_topline_save > wp->w_بفر->b_ml.ml_سطر_گنتی)
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
    صحيح
make_snapshot(صحيح idx)
{
    clear_snapshot(curtab, idx);
    إذا (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	أرجع FAIL;
    }
    أرجع OK;
}

    ساكن صحيح
make_snapshot_rec(چوکھٹا_T *fr, چوکھٹا_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(چوکھٹا_T);
    إذا (*frp == عدم)
	أرجع FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_چوڑائی = fr->fr_چوڑائی;
    (*frp)->fr_اچائی = fr->fr_اچائی;
    إذا (fr->fr_اگلا != عدم)
    {
	إذا (make_snapshot_rec(fr->fr_اگلا, &((*frp)->fr_اگلا)) == FAIL)
	    أرجع FAIL;
    }
    إذا (fr->fr_بچہ != عدم)
    {
	إذا (make_snapshot_rec(fr->fr_بچہ, &((*frp)->fr_بچہ)) == FAIL)
	    أرجع FAIL;
    }
    إذا (fr->fr_layout == FR_پتی && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    أرجع OK;
}

/*
 * Remove any existing snapshot.
 */
    ساكن فراغ
clear_snapshot(tabpage_T *tp, صحيح idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = عدم;
}

    ساكن فراغ
clear_snapshot_rec(چوکھٹا_T *fr)
{
    إذا (fr == عدم)
	أرجع;
    clear_snapshot_rec(fr->fr_اگلا);
    clear_snapshot_rec(fr->fr_بچہ);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    ساكن win_T *
get_snapshot_curwin_rec(چوکھٹا_T *ft)
{
    win_T	*wp;

    إذا (ft->fr_اگلا != عدم)
    {
	إذا ((wp = get_snapshot_curwin_rec(ft->fr_اگلا)) != عدم)
	    أرجع wp;
    }
    إذا (ft->fr_بچہ != عدم)
    {
	إذا ((wp = get_snapshot_curwin_rec(ft->fr_بچہ)) != عدم)
	    أرجع wp;
    }

    أرجع ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    ساكن win_T *
get_snapshot_curwin(صحيح idx)
{
    إذا (curtab->tp_snapshot[idx] == عدم)
	أرجع عدم;

    أرجع get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    فراغ
restore_snapshot(
    صحيح		idx,
    صحيح		close_curwin)	    // closing current window
{
    win_T	*wp;

    إذا (curtab->tp_snapshot[idx] != عدم
	    && curtab->tp_snapshot[idx]->fr_چوڑائی == topframe->fr_چوڑائی
	    && curtab->tp_snapshot[idx]->fr_اچائی == topframe->fr_اچائی
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	إذا (wp != عدم && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    ساكن صحيح
check_snapshot_rec(چوکھٹا_T *sn, چوکھٹا_T *fr)
{
    إذا (sn->fr_layout != fr->fr_layout
	    || (sn->fr_اگلا == عدم) != (fr->fr_اگلا == عدم)
	    || (sn->fr_بچہ == عدم) != (fr->fr_بچہ == عدم)
	    || (sn->fr_اگلا != عدم
		&& check_snapshot_rec(sn->fr_اگلا, fr->fr_اگلا) == FAIL)
	    || (sn->fr_بچہ != عدم
		&& check_snapshot_rec(sn->fr_بچہ, fr->fr_بچہ) == FAIL)
	    || (sn->fr_win != عدم && !win_valid(sn->fr_win)))
	أرجع FAIL;
    أرجع OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    ساكن win_T *
restore_snapshot_rec(چوکھٹا_T *sn, چوکھٹا_T *fr)
{
    win_T	*wp = عدم;
    win_T	*wp2;

    fr->fr_اچائی = sn->fr_اچائی;
    fr->fr_چوڑائی = sn->fr_چوڑائی;
    إذا (fr->fr_layout == FR_پتی)
    {
	چوکھٹا_new_اچائی(fr, fr->fr_اچائی, FALSE, FALSE, FALSE);
	چوکھٹا_new_چوڑائی(fr, fr->fr_چوڑائی, FALSE, FALSE);
	wp = sn->fr_win;
    }
    إذا (sn->fr_اگلا != عدم)
    {
	wp2 = restore_snapshot_rec(sn->fr_اگلا, fr->fr_اگلا);
	إذا (wp2 != عدم)
	    wp = wp2;
    }
    إذا (sn->fr_بچہ != عدم)
    {
	wp2 = restore_snapshot_rec(sn->fr_بچہ, fr->fr_بچہ);
	إذا (wp2 != عدم)
	    wp = wp2;
    }
    أرجع wp;
}

#إذا defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    صحيح
win_hasvertsplit(فراغ)
{
    چوکھٹا_T	*fr;

    إذا (topframe->fr_layout == FR_صف)
	أرجع TRUE;

    إذا (topframe->fr_layout == FR_خانہ)
	FOR_ALL_چوکھٹے(fr, topframe->fr_بچہ)
	    إذا (fr->fr_layout == FR_صف)
		أرجع TRUE;

    أرجع FALSE;
}
#جے_انت

#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    صحيح
get_win_نمبر(win_T *wp, win_T *پہلا_win)
{
    صحيح		i = 1;
    win_T	*w;

    لكل (w = پہلا_win; w != عدم && w != wp; w = W_اگلا(w))
	++i;

    إذا (w == عدم)
	أرجع 0;
    وإلا
	أرجع i;
}

    صحيح
get_tab_نمبر(tabpage_T *tp)
{
    صحيح		i = 1;
    tabpage_T	*t;

    لكل (t = پہلا_tabpage; t != عدم && t != tp; t = t->tp_اگلا)
	++i;

    إذا (t == عدم)
	أرجع 0;
    وإلا
	أرجع i;
}
#جے_انت

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    ساكن صحيح
چوکھٹا_check_اچائی(چوکھٹا_T *topfrp, صحيح اچائی)
{
    چوکھٹا_T *frp;

    إذا (topfrp->fr_اچائی != اچائی)
	أرجع FALSE;

    إذا (topfrp->fr_layout == FR_صف)
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    إذا (frp->fr_اچائی != اچائی)
		أرجع FALSE;

    أرجع TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    ساكن صحيح
چوکھٹا_check_چوڑائی(چوکھٹا_T *topfrp, صحيح چوڑائی)
{
    چوکھٹا_T *frp;

    إذا (topfrp->fr_چوڑائی != چوڑائی)
	أرجع FALSE;

    إذا (topfrp->fr_layout == FR_خانہ)
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    إذا (frp->fr_چوڑائی != چوڑائی)
		أرجع FALSE;

    أرجع TRUE;
}

#إذا defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    ساكن صحيح
int_cmp(ثابت فراغ *pa, ثابت فراغ *pb)
{
    ثابت صحيح a = *(ثابت صحيح *)pa;
    ثابت صحيح b = *(ثابت صحيح *)pb;
    إذا (a > b)
	أرجع 1;
    إذا (a < b)
	أرجع -1;
    أرجع 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    حرف *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = خالی_تھاواں_option;
    صحيح		tw;
    صحيح		خانہ;
    صحيح		گنتی = 0;
    صحيح		color_خانے_گنتی[256];
    صحيح		i;
    صحيح		j = 0;

    إذا (wp != عدم && wp->w_بفر == عدم)
	أرجع عدم;  // buffer was closed

    إذا (cc != عدم)
	s = cc;
    وإلا إذا (wp != عدم)
	s = wp->w_p_cc;

    إذا (wp != عدم)
	tw = wp->w_بفر->b_p_tw;
    وإلا
	// buffer-local value not set, assume zero
	tw = 0;

    طالما (*s != NUL && گنتی < 255)
    {
	إذا (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    خانہ = (*s == '-') ? -1 : 1;
	    ++s;
	    إذا (!VIM_ISDIGIT(*s))
		أرجع e_invalid_دلیل;
	    خانہ = خانہ * getdigits(&s);
	    إذا (tw == 0)
		اذهب skip;  // 'textwidth' not set, skip this item
	    خانہ += tw;
	    إذا (خانہ < 0)
		اذهب skip;
	}
	وإلا إذا (VIM_ISDIGIT(*s))
	    خانہ = getdigits(&s);
	وإلا
	    أرجع e_invalid_دلیل;
	color_خانے_گنتی[گنتی++] = خانہ - 1;  // 1-based to 0-based
skip:
	إذا (*s == NUL)
	    اكسر;
	إذا (*s != ',')
	    أرجع e_invalid_دلیل;
	إذا (*++s == NUL)
	    أرجع e_invalid_دلیل;  // illegal trailing comma as in "set cc=80,"
    }

    إذا (wp == عدم)
	أرجع عدم;  // only parse "cc"

    vim_free(wp->w_p_cc_خانے_گنتی);
    إذا (گنتی == 0)
	wp->w_p_cc_خانے_گنتی = عدم;
    وإلا
    {
	wp->w_p_cc_خانے_گنتی = ALLOC_MULT(صحيح, گنتی + 1);
	إذا (wp->w_p_cc_خانے_گنتی != عدم)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_خانے_گنتی, گنتی, حجم(صحيح), int_cmp);

	    لكل (i = 0; i < گنتی; ++i)
		// skip duplicates
		إذا (j == 0 || wp->w_p_cc_خانے_گنتی[j - 1] != color_خانے_گنتی[i])
		    wp->w_p_cc_خانے_گنتی[j++] = color_خانے_گنتی[i];
	    wp->w_p_cc_خانے_گنتی[j] = -1;  // end marker
	}
    }

    أرجع عدم;  // no error
}
#جے_انت

    صحيح
get_آخری_winid(فراغ)
{
    أرجع آخری_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   صحيح
win_locked(win_T *wp)
{
    أرجع wp->w_locked;
}
