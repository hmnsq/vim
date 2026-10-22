/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

ساكن فراغ cmd_with_شمار(حرف *cmd, char_u *bufp, size_t bufsize, دراز Prenum);
ساكن فراغ win_init(win_T *newp, win_T *oldp, صحيح flags);
ساكن فراغ win_init_some(win_T *newp, win_T *oldp);
ساكن فراغ قاب_comp_pos(قاب_T *topfrp, صحيح *ردیف, صحيح *ستون);
ساكن فراغ قاب_setheight(قاب_T *curfrp, صحيح ارتفاع);
ساكن فراغ قاب_setwidth(قاب_T *curfrp, صحيح عرض);
ساكن فراغ win_exchange(دراز);
ساكن فراغ win_rotate(صحيح, صحيح);
ساكن فراغ win_equal_rec(win_T *بعدی_curwin, صحيح current, قاب_T *topfr, صحيح dir, صحيح ستون, صحيح ردیف, صحيح عرض, صحيح ارتفاع);
ساكن فراغ trigger_winnewpre(فراغ);
ساكن فراغ trigger_winclosed(win_T *win);
ساكن win_T *win_free_mem(win_T *win, صحيح *dirp, tabpage_T *tp);
ساكن قاب_T *win_altframe(win_T *win, tabpage_T *tp);
ساكن tabpage_T *alt_tabpage(فراغ);
ساكن win_T *frame2win(قاب_T *frp);
ساكن صحيح قاب_has_win(قاب_T *frp, win_T *wp);
ساكن فراغ win_fix_scroll(صحيح resize);
ساكن فراغ win_fix_cursor(صحيح نرمال_4);
ساكن فراغ قاب_new_ارتفاع(قاب_T *topfrp, صحيح ارتفاع, صحيح topfirst, صحيح wfh, صحيح مجموعه_ch);
ساكن صحيح قاب_fixed_ارتفاع(قاب_T *frp);
ساكن صحيح قاب_fixed_عرض(قاب_T *frp);
ساكن فراغ قاب_جمع_کن_statusline(قاب_T *frp);
ساكن فراغ قاب_new_عرض(قاب_T *topfrp, صحيح عرض, صحيح leftfirst, صحيح wfw);
ساكن فراغ قاب_مجموعه_vsep(قاب_T *frp, صحيح جمع_کن);
ساكن صحيح قاب_minwidth(قاب_T *topfrp, win_T *بعدی_curwin);
ساكن فراغ قاب_fix_عرض(win_T *wp);
ساكن صحيح win_alloc_firstwin(win_T *oldwin);
ساكن فراغ new_قاب(win_T *wp);
ساكن tabpage_T *alloc_tabpage(فراغ);
ساكن صحيح leave_tabpage(buf_T *new_curbuf, صحيح trigger_leave_autocmds);
ساكن فراغ enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, صحيح trigger_enter_autocmds, صحيح trigger_leave_autocmds);
ساكن فراغ قاب_fix_ارتفاع(win_T *wp);
ساكن صحيح قاب_minheight(قاب_T *topfrp, win_T *بعدی_curwin);
ساكن صحيح قاب_wincount_درون_ارتفاع(قاب_T *topfrp);
ساكن صحيح may_open_tabpage(فراغ);
ساكن صحيح win_enter_ext(win_T *wp, صحيح flags);
ساكن فراغ win_free(win_T *wp, tabpage_T *tp);
ساكن فراغ win_append(win_T *after, win_T *wp);
ساكن فراغ قاب_append(قاب_T *after, قاب_T *frp);
ساكن فراغ قاب_درج(قاب_T *before, قاب_T *frp);
ساكن فراغ قاب_remove(قاب_T *frp);
ساكن فراغ win_goto_ver(صحيح به_بالا, دراز شمار);
ساكن فراغ win_goto_hor(صحيح چپ_2, دراز شمار);
ساكن فراغ قاب_جمع_کن_ارتفاع(قاب_T *frp, صحيح n);
ساكن فراغ آخرین_وضعیت_rec(قاب_T *fr, صحيح statusline);
#إذا defined(FEAT_STL_OPT)
ساكن صحيح قاب_پیدا_کن_global_stlh_rec(قاب_T *frp, صحيح h);
ساكن فراغ قاب_change_statusline_ارتفاع_rec(قاب_T *frp, صحيح global_stlh);
#پایان_اگر
ساكن فراغ قاب_flatten(قاب_T *frp);
ساكن فراغ winframe_restore(win_T *wp, صحيح dir, قاب_T *unflat_altfr);

ساكن صحيح make_snapshot_rec(قاب_T *fr, قاب_T **frp);
ساكن فراغ clear_snapshot(tabpage_T *tp, صحيح idx);
ساكن فراغ clear_snapshot_rec(قاب_T *fr);
ساكن صحيح check_snapshot_rec(قاب_T *sn, قاب_T *fr);
ساكن win_T *restore_snapshot_rec(قاب_T *sn, قاب_T *fr);
ساكن win_T *get_snapshot_curwin(صحيح idx);

ساكن صحيح قاب_check_ارتفاع(قاب_T *topfrp, صحيح ارتفاع);
ساكن صحيح قاب_check_عرض(قاب_T *topfrp, صحيح عرض);

ساكن win_T *win_alloc(win_T *after, صحيح hidden);

#تعریف_کن NOWIN		((win_T *)-1)	// non-existing window

#تعریف_کن ردیفها_AVAIL (ردیفها - p_ch - tabline_ارتفاع())

// flags for win_enter_ext()
#تعریف_کن WEE_UNDO_SYNC			0x01
#تعریف_کن WEE_CURWIN_INVALID		0x02
#تعریف_کن WEE_TRIGGER_NEW_AUTOCMDS	0x04
#تعریف_کن WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#تعریف_کن WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#تعریف_کن WEE_ALLOW_PARSE_MESSAGES	0x20

ساكن حرف *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ساكن صحيح تقسیم_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ساكن صحيح close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
ساكن صحيح قاب_locked = 0;

#إذا defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
ساكن صحيح stl_rendered_ارتفاع = 1;
ساكن صحيح stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
ساكن صحيح stlo_mh = وضعیت_ارتفاع_2; // "maxheight" value cache of global 'stlo'
#پایان_اگر

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    فراغ
پنجره_layout_قفل(فراغ)
{
    ++تقسیم_disallowed;
    ++close_disallowed;
}

    فراغ
پنجره_layout_unlock(فراغ)
{
    --تقسیم_disallowed;
    --close_disallowed;
}

    صحيح
قابها_locked(فراغ)
{
    أرجع قاب_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    صحيح
پنجره_layout_locked(تعداد CMD_شاخص cmd)
{
    إذا (تقسیم_disallowed > 0 || close_disallowed > 0)
    {
	إذا (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_تقسیم_پنجره_when_closing_میانگیر));
	وإلا
	    emsg(_(e_not_allowed_مقصد_change_پنجره_layout_درون_this_autocmd));
	أرجع TRUE;
    }
    أرجع FALSE;
}

// #define WIN_DEBUG
#اگر_تعریف_شده WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    ساكن فراغ
log_قاب_layout(قاب_T *قاب)
{
    ch_log(عدم, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    قاب->fr_layout == FR_برگ ? "LEAF"
				  : قاب->fr_layout == FR_ردیف ? "ROW" : "COL",
	    قاب->fr_عرض,
	    قاب->fr_ارتفاع,
	    قاب->fr_win == عدم ? -1 : قاب->fr_win->w_عرض,
	    قاب->fr_win == عدم ? -1 : قاب->fr_win->w_ارتفاع,
	    قاب->fr_win == عدم ? -1 : قاب->fr_win->w_id);
    إذا (قاب->fr_فرزند != عدم)
    {
	ch_log(عدم, "children");
	log_قاب_layout(قاب->fr_فرزند);
	إذا (قاب->fr_بعدی != عدم)
	    ch_log(عدم, "END of children");
    }
    إذا (قاب->fr_بعدی != عدم)
	log_قاب_layout(قاب->fr_بعدی);
}
#پایان_اگر

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    صحيح
check_can_مجموعه_curbuf_disabled(فراغ)
{
    إذا (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_مقصد_میانگیر));
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
check_can_مجموعه_curbuf_forceit(صحيح forceit)
{
    إذا (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_مقصد_میانگیر));
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
    أرجع is_درون_cmdwin() && prevwin != عدم ? prevwin : curwin;
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
do_پنجره(
    صحيح		nchar,
    دراز	Prenum,
    صحيح		xchar)	    // extra char from ":wincmd gx" or NUL
{
    دراز	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#اگر_تعریف_شده FEAT_پیدا_کن_ID
    صحيح		type = پیدا_کن_DEFINE;
    صحيح		len;
#پایان_اگر
    char_u	cbuf[40];

    إذا (ERROR_IF_ANY_POPUP_پنجره)
	أرجع;

#تعریف_کن CHECK_CMDWIN \
    افعل { \
	إذا (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_درون_cmdline_پنجره)); \
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
#اگر_تعریف_شده FEAT_GUI
		need_mouse_correct = TRUE;
#پایان_اگر
		(فراغ)win_تقسیم((صحيح)Prenum, 0);
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
#اگر_تعریف_شده FEAT_GUI
		need_mouse_correct = TRUE;
#پایان_اگر
		(فراغ)win_تقسیم((صحيح)Prenum, WSP_VERT);
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
			semsg(_(e_میانگیر_nr_not_یافت), Prenum);
		    اكسر;
		}

		إذا (!curbuf_locked() && win_تقسیم(0, 0) == OK)
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
#پایان_اگر
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		اكسر;

// quit current window
    حالة Ctrl_Q:
    حالة 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_شمار("quit", cbuf, حجم(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		اكسر;

// close current window
    حالة Ctrl_C:
    حالة 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_شمار("close", cbuf, حجم(cbuf), Prenum);
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
		    emsg(_(e_there_is_no_preview_پنجره));
		وإلا
		    win_goto(wp);
		اكسر;
#پایان_اگر

// close all but current window
    حالة Ctrl_O:
    حالة 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_شمار("only", cbuf, حجم(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		اكسر;

// cursor to next window with wrap around
    حالة Ctrl_W:
    حالة 'w':
// cursor to previous window with wrap around
    حالة 'W':
		CHECK_CMDWIN;
		إذا (ONE_پنجره && Prenum != 1)	// just one window
		    beep_flush();
		وإلا
		{
		    إذا (Prenum)			// go to specified window
		    {
			لكل (wp = firstwin; --Prenum > 0; )
			{
			    إذا (wp->w_بعدی == عدم)
				اكسر;
			    وإلا
				wp = wp->w_بعدی;
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
			    wp = curwin->w_بعدی;
			    إذا (wp == عدم)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		اكسر;

// cursor to window below
    حالة 'j':
    حالة K_پایین:
    حالة Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		اكسر;

// cursor to window above
    حالة 'k':
    حالة K_به_بالا:
    حالة Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		اكسر;

// cursor to left window
    حالة 'h':
    حالة K_چپ:
    حالة Ctrl_H:
    حالة K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		اكسر;

// cursor to right window
    حالة 'l':
    حالة K_راست:
    حالة Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		اكسر;

// move window to new tab page
    حالة 'T':
		CHECK_CMDWIN;
		إذا (one_پنجره())
		    پیام(_(m_onlyone));
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
		إذا (ONE_پنجره)
		    beep_flush();
		وإلا
		{
		    صحيح dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_بالاترین : WSP_BOT);

		    (فراغ)win_splitmove(curwin, (صحيح)Prenum, dir);
		}
		اكسر;

// make all windows the same width and/or height
    حالة '=':
		{
		    صحيح mod = cmdmod.cmod_تقسیم & (WSP_VERT | WSP_HOR);
#اگر_تعریف_شده FEAT_GUI
		    need_mouse_correct = TRUE;
#پایان_اگر
		    win_equal(عدم, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		اكسر;

// increase current window height
    حالة '+':
#اگر_تعریف_شده FEAT_GUI
		need_mouse_correct = TRUE;
#پایان_اگر
		win_setheight(curwin->w_ارتفاع + (صحيح)Prenum1);
		اكسر;

// decrease current window height
    حالة '-':
#اگر_تعریف_شده FEAT_GUI
		need_mouse_correct = TRUE;
#پایان_اگر
		win_setheight(curwin->w_ارتفاع - (صحيح)Prenum1);
		اكسر;

// set current window height
    حالة Ctrl__:
    حالة '_':
#اگر_تعریف_شده FEAT_GUI
		need_mouse_correct = TRUE;
#پایان_اگر
		win_setheight(Prenum ? (صحيح)Prenum : 9999);
		اكسر;

// increase current window width
    حالة '>':
#اگر_تعریف_شده FEAT_GUI
		need_mouse_correct = TRUE;
#پایان_اگر
		win_setwidth(curwin->w_عرض + (صحيح)Prenum1);
		اكسر;

// decrease current window width
    حالة '<':
#اگر_تعریف_شده FEAT_GUI
		need_mouse_correct = TRUE;
#پایان_اگر
		win_setwidth(curwin->w_عرض - (صحيح)Prenum1);
		اكسر;

// set current window width
    حالة '|':
#اگر_تعریف_شده FEAT_GUI
		need_mouse_correct = TRUE;
#پایان_اگر
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
#پایان_اگر
		// FALLTHROUGH
    حالة ']':
    حالة Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		إذا (Prenum)
		    postponed_تقسیم = Prenum;
		وإلا
		    postponed_تقسیم = -1;
#اگر_تعریف_شده FEAT_QUICKFIX
		إذا (nchar != '}')
		    g_do_tagpreview = 0;
#پایان_اگر

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_تقسیم = 0;
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
#اگر_تعریف_شده FEAT_GUI
		    need_mouse_correct = TRUE;
#پایان_اگر
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = عدم;
		    إذا ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    إذا (wp == عدم && win_تقسیم(0, 0) == OK)
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

#اگر_تعریف_شده FEAT_پیدا_کن_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    حالة 'i':			    // Go to any match
    حالة Ctrl_I:
		type = پیدا_کن_ANY;
		// FALLTHROUGH
    حالة 'd':			    // Go to definition, using 'define'
    حالة Ctrl_D:
		CHECK_CMDWIN;
		إذا ((len = پیدا_کن_ident_under_cursor(&ptr, پیدا_کن_IDENT)) == 0)
		    اكسر;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		إذا (ptr == عدم)
		    اكسر;

		پیدا_کن_الگو_درون_مسیر(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_تقسیم, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_مجموعه_curswant = true;
		اكسر;
#پایان_اگر

// Quickfix window only: view the result under the cursor in a new split.
#إذا defined(FEAT_QUICKFIX)
    حالة K_KENTER:
    حالة CAR:
		إذا (bt_quickfix(curbuf))
		    qf_view_نتیجه(TRUE);
		اكسر;
#پایان_اگر

// CTRL-W g  extended commands
    حالة 'g':
    حالة Ctrl_G:
		CHECK_CMDWIN;
#اگر_تعریف_شده USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#پایان_اگر
		++no_mapping;
		++allow_کلیدها;   // no mapping for xchar, but allow key codes
		إذا (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_کلیدها;
		(فراغ)جمع_کن_مقصد_showcmd(xchar);

		بدل (xchar)
		{
#إذا defined(FEAT_QUICKFIX)
		    حالة '}':
			xchar = Ctrl_RSB;
			إذا (Prenum)
			    g_do_tagpreview = Prenum;
			وإلا
			    g_do_tagpreview = p_pvh;
#پایان_اگر
			// FALLTHROUGH
		    حالة ']':
		    حالة Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			إذا (Prenum)
			    postponed_تقسیم = Prenum;
			وإلا
			    postponed_تقسیم = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_تقسیم = 0;
			اكسر;

		    حالة 'f':	    // CTRL-W gf: "gf" in a new tab page
		    حالة 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_شاخص(curtab) + 1;
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
#پایان_اگر
    حالة 'f':
    حالة 'F':
    حالة Ctrl_F:
#اگر_تعریف_شده FEAT_پیدا_کن_ID
    حالة 'i':
    حالة Ctrl_I:
    حالة 'd':
    حالة Ctrl_D:
#پایان_اگر
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
#پایان_اگر
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
cmd_with_شمار(
    حرف	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    دراز	Prenum)
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
check_تقسیم_disallowed(win_T *wp)
{
    إذا (تقسیم_disallowed > 0)
    {
	emsg(_(e_cant_تقسیم_پنجره_while_closing_another));
	أرجع FAIL;
    }
    إذا (wp->w_میانگیر->b_locked_تقسیم)
    {
	emsg(_(e_cannot_تقسیم_پنجره_when_closing_میانگیر));
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
win_تقسیم(صحيح اندازه, صحيح flags)
{
    إذا (ERROR_IF_ANY_POPUP_پنجره)
	أرجع FAIL;

    إذا (check_تقسیم_disallowed(curwin) == FAIL)
	أرجع FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    إذا (may_open_tabpage() == OK)
	أرجع OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_تقسیم;
    إذا ((flags & WSP_بالاترین) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_تقسیم_topleft_and_botright_at_the_same_زمان));
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

    أرجع win_تقسیم_ins(اندازه, flags, عدم, 0, عدم);
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
win_تقسیم_ins(
    صحيح		اندازه,
    صحيح		flags,
    win_T	*new_wp,
    صحيح		dir,
    قاب_T	*مقصد_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    صحيح		new_اندازه = اندازه;
    صحيح		i;
    صحيح		need_وضعیت = 0;
    صحيح		do_equal = FALSE;
    صحيح		needed;
    صحيح		موجود;
    صحيح		oldwin_ارتفاع = 0;
    صحيح		layout;
    قاب_T	*frp, *curfrp, *frp2, *prevfrp;
    صحيح		before;
    صحيح		minheight;
    صحيح		wmh1;
    صحيح		did_مجموعه_fraction = FALSE;
    صحيح		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    إذا (new_wp == عدم)
	trigger_winnewpre();

    إذا (flags & WSP_بالاترین)
	oldwin = firstwin;
    وإلا إذا (flags & WSP_BOT)
	oldwin = lastwin;
    وإلا
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    إذا (ONE_پنجره && p_ls == 1 && oldwin->w_وضعیت_ارتفاع == 0)
    {
	إذا (!(flags & WSP_FORCE_جا) && VISIBLE_ارتفاع(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_جا));
	    اذهب theend;
	}
	need_وضعیت = statusline_ارتفاع(oldwin);
    }

#اگر_تعریف_شده FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    إذا (gui.درون_use)
	بیرون_flush();
#پایان_اگر

    إذا (flags & WSP_VERT)
    {
	صحيح	wmw1;
	صحيح	minwidth;

	layout = FR_ردیف;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	إذا (flags & WSP_جا)
	    needed += p_wiw - wmw1;
	إذا (flags & (WSP_BOT | WSP_بالاترین))
	{
	    minwidth = قاب_minwidth(topframe, NOWIN);
	    موجود = topframe->fr_عرض;
	    needed += minwidth;
	}
	وإلا إذا (p_ea)
	{
	    minwidth = قاب_minwidth(oldwin->w_قاب, NOWIN);
	    prevfrp = oldwin->w_قاب;
	    لكل (frp = oldwin->w_قاب->fr_والد; frp != عدم;
							frp = frp->fr_والد)
	    {
		إذا (frp->fr_layout == FR_ردیف)
		    FOR_ALL_قابها(frp2, frp->fr_فرزند)
			إذا (frp2 != prevfrp)
			    minwidth += قاب_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    موجود = topframe->fr_عرض;
	    needed += minwidth;
	}
	وإلا
	{
	    minwidth = قاب_minwidth(oldwin->w_قاب, NOWIN);
	    موجود = oldwin->w_قاب->fr_عرض;
	    needed += minwidth;
	}
	إذا (!(flags & WSP_FORCE_جا) && موجود < needed)
	{
	    emsg(_(e_not_enough_جا));
	    اذهب theend;
	}
	إذا (new_اندازه == 0)
	    new_اندازه = oldwin->w_عرض / 2;
	إذا (new_اندازه > موجود - minwidth - 1)
	    new_اندازه = موجود - minwidth - 1;
	إذا (new_اندازه < wmw1)
	    new_اندازه = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	إذا (oldwin->w_عرض - new_اندازه - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	إذا (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_عرض + new_اندازه + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	إذا (!do_equal && p_ea && اندازه == 0 && *p_ead != 'v'
	    && oldwin->w_قاب->fr_والد != عدم)
	    do_equal = TRUE;
    }
    وإلا
    {
	layout = FR_ستون;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_ارتفاع(curwin);
	needed = wmh1 + statusline_ارتفاع(oldwin);
	إذا (flags & WSP_جا)
	    needed += p_wh - wmh1;
	إذا (flags & (WSP_BOT | WSP_بالاترین))
	{
	    minheight = قاب_minheight(topframe, NOWIN) + need_وضعیت;
	    موجود = topframe->fr_ارتفاع;
	    needed += minheight;
	}
	وإلا إذا (p_ea)
	{
	    minheight = قاب_minheight(oldwin->w_قاب, NOWIN) + need_وضعیت;
	    prevfrp = oldwin->w_قاب;
	    لكل (frp = oldwin->w_قاب->fr_والد; frp != عدم;
							frp = frp->fr_والد)
	    {
		إذا (frp->fr_layout == FR_ستون)
		    FOR_ALL_قابها(frp2, frp->fr_فرزند)
			إذا (frp2 != prevfrp)
			    minheight += قاب_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    موجود = topframe->fr_ارتفاع;
	    needed += minheight;
	}
	وإلا
	{
	    minheight = قاب_minheight(oldwin->w_قاب, NOWIN) + need_وضعیت;
	    موجود = oldwin->w_قاب->fr_ارتفاع;
	    needed += minheight;
	}
	إذا (!(flags & WSP_FORCE_جا) && موجود < needed)
	{
	    emsg(_(e_not_enough_جا));
	    اذهب theend;
	}
	oldwin_ارتفاع = oldwin->w_ارتفاع;
	إذا (need_وضعیت)
	{
	    oldwin->w_وضعیت_ارتفاع = statusline_ارتفاع(oldwin);
	    oldwin_ارتفاع -= oldwin->w_وضعیت_ارتفاع;
	}
	// Estimated stl height for the new window.
	صحيح new_win_stlh = need_وضعیت != 0 ? need_وضعیت
			: statusline_ارتفاع(new_wp != عدم ? new_wp : curwin);
	إذا (new_اندازه == 0)
	    new_اندازه = (oldwin_ارتفاع - new_win_stlh + 1) / 2;
	إذا (new_اندازه > موجود - minheight - new_win_stlh)
	    new_اندازه = موجود - minheight - new_win_stlh;
	إذا (new_اندازه < wmh1)
	    new_اندازه = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	إذا (oldwin_ارتفاع - new_اندازه - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	إذا (oldwin->w_p_wfh)
	{
	    // Set w_fraction now so that the cursor keeps the same relative
	    // vertical position using the old height.
	    مجموعه_fraction(oldwin);
	    did_مجموعه_fraction = TRUE;

	    win_setheight_win(oldwin->w_ارتفاع + new_اندازه
		    + statusline_ارتفاع(oldwin), oldwin);
	    // w_height now excludes the status line
	    oldwin_ارتفاع = oldwin->w_ارتفاع;
	}

	// Make all windows the same height if 'equalalways' is set, 'ead' is
	// not set to 'h', a size was not provided and there is a parent frame.
	إذا (!do_equal && p_ea && اندازه == 0 && *p_ead != 'h'
	    && oldwin->w_قاب->fr_والد != عدم)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    إذا ((flags & WSP_بالاترین) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_زیر)
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

	new_قاب(wp);
	إذا (wp->w_قاب == عدم)
	{
	    win_free(wp, عدم);
	    اذهب theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    إذا (مقصد_flatten != عدم)
	قاب_flatten(مقصد_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    إذا (flags & (WSP_بالاترین | WSP_BOT))
    {
	إذا ((topframe->fr_layout == FR_ستون && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ردیف && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_فرزند;
	    إذا (flags & WSP_BOT)
		طالما (curfrp->fr_بعدی != عدم)
		    curfrp = curfrp->fr_بعدی;
	}
	وإلا
	    curfrp = topframe;
	before = (flags & WSP_بالاترین);
    }
    وإلا
    {
	curfrp = oldwin->w_قاب;
	إذا (flags & WSP_زیر)
	    before = FALSE;
	وإلا إذا (flags & WSP_ABOVE)
	    before = TRUE;
	وإلا إذا (flags & WSP_VERT)
	    before = !p_spr;
	وإلا
	    before = !p_sb;
    }
    إذا (curfrp->fr_والد == عدم || curfrp->fr_والد->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(قاب_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_والد = curfrp;
	frp->fr_بعدی = عدم;
	frp->fr_prev = عدم;
	curfrp->fr_فرزند = frp;
	curfrp->fr_win = عدم;
	curfrp = frp;
	إذا (frp->fr_win != عدم)
	    oldwin->w_قاب = frp;
	وإلا
	    FOR_ALL_قابها(frp, frp->fr_فرزند)
		frp->fr_والد = curfrp;
    }

    إذا (new_wp == عدم)
	frp = wp->w_قاب;
    وإلا
	frp = new_wp->w_قاب;
    frp->fr_والد = curfrp->fr_والد;

    // Insert the new frame at the right place in the frame list.
    إذا (before)
	قاب_درج(curfrp, frp);
    وإلا
	قاب_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    إذا (!did_مجموعه_fraction)
	مجموعه_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    إذا (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	إذا (need_وضعیت)
	{
	    win_new_ارتفاع(oldwin, oldwin->w_ارتفاع - need_وضعیت);
	    oldwin->w_وضعیت_ارتفاع = need_وضعیت;
	}
	إذا (flags & (WSP_بالاترین | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    صحيح stl_ارتفاع = p_ls > 0 ? statusline_ارتفاع(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_ارتفاع();
	    win_new_ارتفاع(wp, curfrp->fr_ارتفاع - stl_ارتفاع
							  - WINBAR_ارتفاع(wp));
	    wp->w_وضعیت_ارتفاع = stl_ارتفاع;
	}
	وإلا
	{
	    // height and row of new window is same as current window
	    wp->w_winrow = oldwin->w_winrow;
	    win_new_ارتفاع(wp, VISIBLE_ارتفاع(oldwin));
	    wp->w_وضعیت_ارتفاع = oldwin->w_وضعیت_ارتفاع;
	}
	frp->fr_ارتفاع = curfrp->fr_ارتفاع;

	// "new_size" of the current window goes to the new window, use
	// one column for the vertical separator
	win_new_عرض(wp, new_اندازه);
	إذا (before)
	    wp->w_vsep_عرض = 1;
	وإلا
	{
	    wp->w_vsep_عرض = oldwin->w_vsep_عرض;
	    oldwin->w_vsep_عرض = 1;
	}
	إذا (flags & (WSP_بالاترین | WSP_BOT))
	{
	    إذا (flags & WSP_BOT)
		قاب_مجموعه_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    قاب_new_عرض(curfrp, curfrp->fr_عرض
		     - (new_اندازه + ((flags & WSP_بالاترین) != 0)), flags & WSP_بالاترین,
								       FALSE);
	}
	وإلا
	    win_new_عرض(oldwin, oldwin->w_عرض - (new_اندازه + 1));
	إذا (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_اندازه + 1;
	}
	وإلا		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_عرض + 1;
	قاب_fix_عرض(oldwin);
	قاب_fix_عرض(wp);
    }
    وإلا
    {
	// width and column of new window is same as current window
	إذا (flags & (WSP_بالاترین | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_عرض(wp, topframe->fr_عرض);
	    wp->w_vsep_عرض = 0;
	}
	وإلا
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_عرض(wp, oldwin->w_عرض);
	    wp->w_vsep_عرض = oldwin->w_vsep_عرض;
	}
	frp->fr_عرض = curfrp->fr_عرض;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_ارتفاع(wp, new_اندازه);
	صحيح old_وضعیت_ارتفاع = oldwin->w_وضعیت_ارتفاع;
	إذا (flags & (WSP_بالاترین | WSP_BOT))
	{
	    صحيح new_fr_ارتفاع = curfrp->fr_ارتفاع - new_اندازه
							  + WINBAR_ارتفاع(wp);

	    إذا (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_ارتفاع -= statusline_ارتفاع(curfrp->fr_win);
	    إذا (flags & WSP_BOT)
		قاب_جمع_کن_statusline(curfrp);
	    قاب_new_ارتفاع(curfrp, new_fr_ارتفاع, flags & WSP_بالاترین, FALSE,
									FALSE);
	}
	وإلا
	    win_new_ارتفاع(oldwin, oldwin_ارتفاع - (new_اندازه
			+ statusline_ارتفاع(wp)));
	إذا (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_وضعیت_ارتفاع = statusline_ارتفاع(wp);
	    oldwin->w_winrow += wp->w_ارتفاع + statusline_ارتفاع(wp);
	}
	وإلا		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_ارتفاع(oldwin)
						    + statusline_ارتفاع(oldwin);
	    wp->w_وضعیت_ارتفاع = old_وضعیت_ارتفاع;
	    إذا (!(flags & WSP_BOT))
		oldwin->w_وضعیت_ارتفاع = statusline_ارتفاع(oldwin);
	}
	قاب_fix_ارتفاع(wp);
	قاب_fix_ارتفاع(oldwin);
    }

    إذا (flags & (WSP_بالاترین | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    وضعیت_redraw_all();

    إذا (need_وضعیت)
    {
	پیام_ردیف = ردیفها - 1;
	پیام_ستون = sc_ستون;
	پیام_clr_eos_force();	// Old command/ruler may still be there
	comp_ستون();
	پیام_ردیف = ردیفها - 1;
	پیام_ستون = 0;	// put position back at start of line
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
	إذا (اندازه != 0)
	    p_wiw = اندازه;

#اگر_تعریف_شده FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	إذا (gui.درون_use)
	    gui_init_which_components(عدم);
#پایان_اگر
    }
    وإلا
    {
	i = p_wh;
	إذا (اندازه != 0)
	    p_wh = اندازه;
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
win_init(win_T *newp, win_T *oldp, صحيح flags استفادهنشده)
{
    صحيح		i;

    newp->w_میانگیر = oldp->w_میانگیر;
#اگر_تعریف_شده FEAT_SYN_HL
    newp->w_s = &(oldp->w_میانگیر->b_s);
#پایان_اگر
    oldp->w_میانگیر->b_nwindows++;
    newp->w_cursor = oldp->w_cursor;
    newp->w_valid = 0;
    newp->w_curswant = oldp->w_curswant;
    newp->w_مجموعه_curswant = oldp->w_مجموعه_curswant;
    newp->w_topline = oldp->w_topline;
#اگر_تعریف_شده FEAT_DIFF
    newp->w_topfill = oldp->w_topfill;
#پایان_اگر
    newp->w_leftcol = oldp->w_leftcol;
    newp->w_pcmark = oldp->w_pcmark;
    newp->w_prev_pcmark = oldp->w_prev_pcmark;
    newp->w_alt_fnum = oldp->w_alt_fnum;
    newp->w_wrow = oldp->w_wrow;
    newp->w_fraction = oldp->w_fraction;
    newp->w_prev_fraction_ردیف = oldp->w_prev_fraction_ردیف;

    // Not sure if this is needed, but be safe
    remove_highlight_overrides(newp->w_hl);
    VIM_CLEAR(newp->w_hl);

    copy_jumplist(oldp, newp);
#اگر_تعریف_شده FEAT_QUICKFIX
    إذا (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = عدم;
	newp->w_llist_ref = عدم;
    }
    وإلا
	copy_loclist_پشته(oldp, newp);
#پایان_اگر
    newp->w_localdir = (oldp->w_localdir == عدم)
				    ? عدم : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == عدم)
				    ? عدم : vim_strsave(oldp->w_prevdir);

    إذا (*p_spk != 'c')
    {
	إذا (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_ارتفاع = oldp->w_ارتفاع - WINBAR_ارتفاع(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_ارتفاع(oldp);
    }

    // copy tagstack and folds
    لكل (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*برچسب = &newp->w_tagstack[i];
	*برچسب = oldp->w_tagstack[i];
	إذا (برچسب->tagname != عدم)
	    برچسب->tagname = vim_strsave(برچسب->tagname);
	إذا (برچسب->user_داده != عدم)
	    برچسب->user_داده = vim_strsave(برچسب->user_داده);
    }
    newp->w_tagstackidx = oldp->w_tagstackidx;
    newp->w_tagstacklen = oldp->w_tagstacklen;

    // Keep same changelist position in new window.
    newp->w_changelistidx = oldp->w_changelistidx;

#اگر_تعریف_شده FEAT_FOLDING
    copyFoldingState(oldp, newp);
#پایان_اگر

    win_init_some(newp, oldp);

#اگر_تعریف_شده FEAT_TERMINAL
    {
	// Make sure to also handle highlight overrides copied over from oldp.
	bool pushed = افزودن_highlight_overrides(newp->w_hl, newp->w_hl_len);
	إذا (newp->w_میانگیر->b_جمله != عدم)
	    جمله_init_default_colors(newp->w_میانگیر->b_جمله);
	إذا (pushed)
	    برداشت_highlight_overrides();
    }
#پایان_اگر
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
win_valid_popup(win_T *win استفادهنشده)
{
#اگر_تعریف_شده FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	إذا (wp == win)
	    أرجع TRUE;
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	إذا (wp == win)
	    أرجع TRUE;
#پایان_اگر
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
win_پیدا_کن_by_id(صحيح id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_id == id)
	    أرجع wp;
#اگر_تعریف_شده FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	إذا (wp->w_id == id)
	    أرجع wp;
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	إذا (wp->w_id == id)
	    أرجع wp;
#پایان_اگر
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
	FOR_ALL_WINDOWS_درون_TAB(tp, wp)
	{
	    إذا (wp == win)
		أرجع TRUE;
	}
#اگر_تعریف_شده FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_درون_TAB(tp, wp)
	    إذا (wp == win)
		أرجع TRUE;
#پایان_اگر
    }
    أرجع win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    صحيح
win_شمار(فراغ)
{
    win_T	*wp;
    صحيح		شمار = 0;

    FOR_ALL_WINDOWS(wp)
	++شمار;
    أرجع شمار;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    صحيح
make_windows(
    صحيح		شمار,
    صحيح		vertical)  // split windows vertically if TRUE
{
    صحيح		maxcount;
    صحيح		todo;

    إذا (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_عرض + curwin->w_vsep_عرض
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    وإلا
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_ارتفاع(curwin) + curwin->w_وضعیت_ارتفاع
			- (p_wh - p_wmh)) / (p_wmh + statusline_ارتفاع(curwin));
    }

    إذا (maxcount < 2)
	maxcount = 2;
    إذا (شمار > maxcount)
	شمار = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    إذا (شمار > 1)
	آخرین_وضعیت(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    بلوک_autocmds();

    // todo is number of windows left to create
    لكل (todo = شمار - 1; todo > 0; --todo)
	إذا (vertical)
	{
	    إذا (win_تقسیم(curwin->w_عرض - (curwin->w_عرض - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		اكسر;
	}
	وإلا
	{
	    إذا (win_تقسیم(curwin->w_ارتفاع - (curwin->w_ارتفاع - todo
			    * statusline_ارتفاع(curwin)) / (todo + 1)
			- statusline_ارتفاع(curwin), WSP_ABOVE) == FAIL)
		اكسر;
	}

    unblock_autocmds();

    // return actual number of windows
    أرجع (شمار - todo);
}

/*
 * Exchange current and next window
 */
    ساكن فراغ
win_exchange(دراز Prenum)
{
    قاب_T	*frp;
    قاب_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    صحيح		موقت;

    إذا (ERROR_IF_ANY_POPUP_پنجره)
	أرجع;
    إذا (ONE_پنجره)	    // just one window
    {
	beep_flush();
	أرجع;
    }
    إذا (متن_or_buf_locked())
    {
	beep_flush();
	أرجع;
    }

#اگر_تعریف_شده FEAT_GUI
    need_mouse_correct = TRUE;
#پایان_اگر

    /*
     * find window to exchange with
     */
    إذا (Prenum)
    {
	frp = curwin->w_قاب->fr_والد->fr_فرزند;
	طالما (frp != عدم && --Prenum > 0)
	    frp = frp->fr_بعدی;
    }
    وإلا إذا (curwin->w_قاب->fr_بعدی != عدم)	// Swap with next
	frp = curwin->w_قاب->fr_بعدی;
    وإلا    // Swap last window in row/col with previous
	frp = curwin->w_قاب->fr_prev;

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
    frp2 = curwin->w_قاب->fr_prev;
    إذا (wp->w_prev != curwin)
    {
	win_remove(curwin, عدم);
	قاب_remove(curwin->w_قاب);
	win_append(wp->w_prev, curwin);
	قاب_درج(frp, curwin->w_قاب);
    }
    إذا (wp != wp2)
    {
	win_remove(wp, عدم);
	قاب_remove(wp->w_قاب);
	win_append(wp2, wp);
	إذا (frp2 == عدم)
	    قاب_درج(wp->w_قاب->fr_والد->fr_فرزند, wp->w_قاب);
	وإلا
	    قاب_append(frp2, wp->w_قاب);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    موقت = curwin->w_وضعیت_ارتفاع;
    إذا (موقت != wp->w_وضعیت_ارتفاع)
    {
	win_new_ارتفاع(curwin, curwin->w_ارتفاع + موقت - wp->w_وضعیت_ارتفاع);
	win_new_ارتفاع(wp, wp->w_ارتفاع + wp->w_وضعیت_ارتفاع - موقت);
    }
    curwin->w_وضعیت_ارتفاع = wp->w_وضعیت_ارتفاع;
    wp->w_وضعیت_ارتفاع = موقت;
    موقت = curwin->w_vsep_عرض;
    curwin->w_vsep_عرض = wp->w_vsep_عرض;
    wp->w_vsep_عرض = موقت;

    قاب_fix_ارتفاع(curwin);
    قاب_fix_ارتفاع(wp);
    قاب_fix_عرض(curwin);
    قاب_fix_عرض(wp);

    win_comp_pos();		// recompute window positions
#إذا defined(FEAT_STL_OPT)
    قاب_change_statusline_ارتفاع();
#پایان_اگر

    إذا (wp->w_میانگیر != curbuf)
	reset_VIsual_and_resel();
    وإلا إذا (VIsual_فعال)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    ساكن فراغ
win_rotate(صحيح upwards, صحيح شمار)
{
    win_T	*wp1;
    win_T	*wp2;
    قاب_T	*frp;
    صحيح		n;

    إذا (ONE_پنجره)		// nothing to do
    {
	beep_flush();
	أرجع;
    }

#اگر_تعریف_شده FEAT_GUI
    need_mouse_correct = TRUE;
#پایان_اگر

    // Check if all frames in this row/col have one window.
    FOR_ALL_قابها(frp, curwin->w_قاب->fr_والد->fr_فرزند)
	إذا (frp->fr_win == عدم)
	{
	    emsg(_(e_cannot_rotate_when_another_پنجره_is_تقسیم));
	    أرجع;
	}

    طالما (شمار--)
    {
	إذا (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_قاب->fr_والد->fr_فرزند;
	    wp1 = frp->fr_win;
	    win_remove(wp1, عدم);
	    قاب_remove(frp);

	    // find last frame and append removed window/frame after it
	    لكل ( ; frp->fr_بعدی != عدم; frp = frp->fr_بعدی)
		;
	    win_append(frp->fr_win, wp1);
	    قاب_append(frp, wp1->w_قاب);

	    wp2 = frp->fr_win;		// previously last window
	}
	وإلا			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    لكل (frp = curwin->w_قاب; frp->fr_بعدی != عدم;
							   frp = frp->fr_بعدی)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, عدم);
	    قاب_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_والد->fr_فرزند->fr_win->w_prev, wp1);
	    قاب_درج(frp->fr_والد->fr_فرزند, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_وضعیت_ارتفاع;
	إذا (n != wp1->w_وضعیت_ارتفاع)
	{
	    win_new_ارتفاع(wp2, wp2->w_ارتفاع + n - wp1->w_وضعیت_ارتفاع);
	    win_new_ارتفاع(wp1, wp1->w_ارتفاع + wp1->w_وضعیت_ارتفاع - n);
	}
	wp2->w_وضعیت_ارتفاع = wp1->w_وضعیت_ارتفاع;
	wp1->w_وضعیت_ارتفاع = n;
	قاب_fix_ارتفاع(wp1);
	قاب_fix_ارتفاع(wp2);
	n = wp2->w_vsep_عرض;
	wp2->w_vsep_عرض = wp1->w_vsep_عرض;
	wp1->w_vsep_عرض = n;
	قاب_fix_عرض(wp1);
	قاب_fix_عرض(wp2);

	// recompute w_winrow and w_wincol for all windows
	win_comp_pos();
    }
#إذا defined(FEAT_STL_OPT)
    قاب_change_statusline_ارتفاع();
#پایان_اگر

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Move "wp" into a new split in a given direction, possibly relative to the
 * current window.
 * "wp" must be valid in the current tabpage.
 * Returns FAIL for failure, OK otherwise.
 */
    صحيح
win_splitmove(win_T *wp, صحيح اندازه, صحيح flags)
{
    صحيح		dir;
    صحيح		ارتفاع = wp->w_ارتفاع;
    قاب_T	*unflat_altfr;

    إذا (ONE_پنجره)
	أرجع OK;	// nothing to do
    إذا (check_تقسیم_disallowed(wp) == FAIL)
	أرجع FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, عدم, &unflat_altfr);
    win_remove(wp, عدم);
    آخرین_وضعیت(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    إذا (win_تقسیم_ins(اندازه, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	أرجع FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    إذا (اندازه == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(ارتفاع, wp);
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
#پایان_اگر
    أرجع OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    فراغ
win_move_after(win_T *win1, win_T *win2)
{
    صحيح		ارتفاع;

    // check if the arguments are reasonable
    إذا (win1 == win2)
	أرجع;

    // check if there is something to do
    إذا (win2->w_بعدی != win1)
    {
	إذا (win1->w_قاب->fr_والد != win2->w_قاب->fr_والد)
	{
	    iemsg("Trying to move a window into another frame");
	    أرجع;
	}

	// may need to move the status line/vertical separator of the last
	//پنجره
	إذا (win1 == lastwin)
	{
	    ارتفاع = win1->w_prev->w_وضعیت_ارتفاع;
	    win1->w_prev->w_وضعیت_ارتفاع = win1->w_وضعیت_ارتفاع;
	    win1->w_وضعیت_ارتفاع = ارتفاع;
	    إذا (win1->w_prev->w_vsep_عرض == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_عرض = 0;
		win1->w_prev->w_قاب->fr_عرض -= 1;
		win1->w_vsep_عرض = 1;
		win1->w_قاب->fr_عرض += 1;
	    }
	}
	وإلا إذا (win2 == lastwin)
	{
	    ارتفاع = win1->w_وضعیت_ارتفاع;
	    win1->w_وضعیت_ارتفاع = win2->w_وضعیت_ارتفاع;
	    win2->w_وضعیت_ارتفاع = ارتفاع;
	    إذا (win1->w_vsep_عرض == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_عرض = 1;
		win2->w_قاب->fr_عرض += 1;
		win1->w_vsep_عرض = 0;
		win1->w_قاب->fr_عرض -= 1;
	    }
	}
	win_remove(win1, عدم);
	قاب_remove(win1->w_قاب);
	win_append(win2, win1);
	قاب_append(win2->w_قاب, win1->w_قاب);

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
    win_T	*بعدی_curwin,	// pointer to current window to be or NULL
    صحيح		current,	// do only frame with current window
    صحيح		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    إذا (dir == 0)
	dir = *p_ead;
    win_equal_rec(بعدی_curwin == عدم ? curwin : بعدی_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_ارتفاع(),
		      topframe->fr_عرض, topframe->fr_ارتفاع);
#إذا defined(FEAT_STL_OPT)
    قاب_change_statusline_ارتفاع();
#پایان_اگر
    إذا (!is_aucmd_win(بعدی_curwin))
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
    win_T	*بعدی_curwin,	// pointer to current window to be or NULL
    صحيح		current,	// do only frame with current window
    قاب_T	*topfr,		// frame to set size off
    صحيح		dir,		// 'v', 'h' or 'b', see win_equal()
    صحيح		ستون,		// horizontal position for frame
    صحيح		ردیف,		// vertical position for frame
    صحيح		عرض,		// new width of frame
    صحيح		ارتفاع)		// new height of frame
{
    صحيح		n, m;
    صحيح		extra_sep = 0;
    صحيح		wincount, totwincount = 0;
    قاب_T	*fr;
    صحيح		بعدی_curwin_اندازه = 0;
    صحيح		جا = 0;
    صحيح		new_اندازه;
    صحيح		has_بعدی_curwin = 0;
    صحيح		hnc;

    إذا (topfr->fr_layout == FR_برگ)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	إذا (topfr->fr_ارتفاع != ارتفاع || topfr->fr_win->w_winrow != ردیف
		|| topfr->fr_عرض != عرض || topfr->fr_win->w_wincol != ستون
	   )
	{
	    topfr->fr_win->w_winrow = ردیف;
	    قاب_new_ارتفاع(topfr, ارتفاع, FALSE, FALSE, FALSE);
	    topfr->fr_win->w_wincol = ستون;
	    قاب_new_عرض(topfr, عرض, FALSE, FALSE);
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    وإلا إذا (topfr->fr_layout == FR_ردیف)
    {
	topfr->fr_عرض = عرض;
	topfr->fr_ارتفاع = ارتفاع;

	إذا (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = قاب_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    إذا (ستون + عرض >= firstwin->w_wincol + topframe->fr_عرض)
		extra_sep = 1;
	    وإلا
		extra_sep = 0;
	    totwincount = (n + extra_sep) / (p_wmw + 1);
	    has_بعدی_curwin = قاب_has_win(topfr, بعدی_curwin);

	    /*
	     * Compute width for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal width when counting p_wiw for "next_curwin".
	     */
	    m = قاب_minwidth(topfr, بعدی_curwin);
	    جا = عرض - m;
	    إذا (جا < 0)
	    {
		بعدی_curwin_اندازه = p_wiw + جا;
		جا = 0;
	    }
	    وإلا
	    {
		بعدی_curwin_اندازه = -1;
		FOR_ALL_قابها(fr, topfr->fr_فرزند)
		{
		    إذا (!قاب_fixed_عرض(fr))
			استمر;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = قاب_minwidth(fr, NOWIN);
		    new_اندازه = fr->fr_عرض;
		    إذا (قاب_has_win(fr, بعدی_curwin))
		    {
			جا += p_wiw - p_wmw;
			بعدی_curwin_اندازه = 0;
			إذا (new_اندازه < p_wiw)
			    new_اندازه = p_wiw;
		    }
		    وإلا
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_بعدی == عدم
					? extra_sep : 0)) / (p_wmw + 1);
		    جا -= new_اندازه - n;
		    إذا (جا < 0)
		    {
			new_اندازه += جا;
			جا = 0;
		    }
		    fr->fr_newwidth = new_اندازه;
		}
		إذا (بعدی_curwin_اندازه == -1)
		{
		    إذا (!has_بعدی_curwin)
			بعدی_curwin_اندازه = 0;
		    وإلا إذا (totwincount > 1
			    && (جا + (totwincount - 2))
						  / (totwincount - 1) > p_wiw)
		    {
			// Can make all windows wider than 'winwidth', spread
			// the room equally.
			بعدی_curwin_اندازه = (جا + p_wiw
					    + (totwincount - 1) * p_wmw
					    + (totwincount - 1)) / totwincount;
			جا -= بعدی_curwin_اندازه - p_wiw;
		    }
		    وإلا
			بعدی_curwin_اندازه = p_wiw;
		}
	    }

	    إذا (has_بعدی_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_قابها(fr, topfr->fr_فرزند)
	{
	    wincount = 1;
	    إذا (fr->fr_بعدی == عدم)
		// last frame gets all that remains (avoid roundoff error)
		new_اندازه = عرض;
	    وإلا إذا (dir == 'v')
		new_اندازه = fr->fr_عرض;
	    وإلا إذا (قاب_fixed_عرض(fr))
	    {
		new_اندازه = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    وإلا
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = قاب_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_بعدی == عدم ? extra_sep : 0))
								/ (p_wmw + 1);
		m = قاب_minwidth(fr, بعدی_curwin);
		إذا (has_بعدی_curwin)
		    hnc = قاب_has_win(fr, بعدی_curwin);
		وإلا
		    hnc = FALSE;
		إذا (hnc)	    // don't count next_curwin
		    --wincount;
		إذا (totwincount == 0)
		    new_اندازه = جا;
		وإلا
		    new_اندازه = (wincount * جا + ((بیعلامت)totwincount >> 1))
								/ totwincount;
		إذا (hnc)	    // add next_curwin size
		{
		    بعدی_curwin_اندازه -= p_wiw - (m - n);
		    إذا (بعدی_curwin_اندازه < 0)
			بعدی_curwin_اندازه = 0;
		    new_اندازه += بعدی_curwin_اندازه;
		    جا -= new_اندازه - بعدی_curwin_اندازه;
		}
		وإلا
		    جا -= new_اندازه;
		new_اندازه += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    إذا (!current || dir != 'v' || topfr->fr_والد != عدم
		    || (new_اندازه != fr->fr_عرض)
		    || قاب_has_win(fr, بعدی_curwin))
		win_equal_rec(بعدی_curwin, current, fr, dir, ستون, ردیف,
							    new_اندازه, ارتفاع);
	    ستون += new_اندازه;
	    عرض -= new_اندازه;
	    totwincount -= wincount;
	}
    }
    وإلا // topfr->fr_layout == FR_COL
    {
	topfr->fr_عرض = عرض;
	topfr->fr_ارتفاع = ارتفاع;

	إذا (dir != 'h')			// equalize frame heights
	{
	    totwincount = قاب_wincount_درون_ارتفاع(topfr);
	    has_بعدی_curwin = قاب_has_win(topfr, بعدی_curwin);

	    /*
	     * Compute height for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal height when counting p_wh for "next_curwin".
	     */
	    m = قاب_minheight(topfr, بعدی_curwin);
	    جا = ارتفاع - m;
	    إذا (جا < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		بعدی_curwin_اندازه = p_wh + جا;
		جا = 0;
	    }
	    وإلا
	    {
		بعدی_curwin_اندازه = -1;
		FOR_ALL_قابها(fr, topfr->fr_فرزند)
		{
		    إذا (!قاب_fixed_ارتفاع(fr))
			استمر;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = قاب_minheight(fr, NOWIN);
		    new_اندازه = fr->fr_ارتفاع;
		    إذا (قاب_has_win(fr, بعدی_curwin))
		    {
			جا += p_wh - p_wmh;
			بعدی_curwin_اندازه = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			إذا (new_اندازه < n + (p_wh - p_wmh))
			    new_اندازه = n + (p_wh - p_wmh);
		    }
		    وإلا
			// These windows don't use up room.
			totwincount -= قاب_wincount_درون_ارتفاع(fr);
		    جا -= new_اندازه - n;
		    إذا (جا < 0)
		    {
			new_اندازه += جا;
			جا = 0;
		    }
		    fr->fr_newheight = new_اندازه;
		}
		إذا (بعدی_curwin_اندازه == -1)
		{
		    إذا (!has_بعدی_curwin)
			بعدی_curwin_اندازه = 0;
		    وإلا إذا (totwincount > 1
			    && (جا + (totwincount - 2))
						   / (totwincount - 1) > p_wh)
		    {
			// can make all windows higher than 'winheight',
			// spread the room equally.
			بعدی_curwin_اندازه = (جا + p_wh
					   + (totwincount - 1) * p_wmh
					   + (totwincount - 1)) / totwincount;
			جا -= بعدی_curwin_اندازه - p_wh;
		    }
		    وإلا
			بعدی_curwin_اندازه = p_wh;
		}
	    }

	    إذا (has_بعدی_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_قابها(fr, topfr->fr_فرزند)
	{
	    wincount = 1;
	    إذا (fr->fr_بعدی == عدم)
		// last frame gets all that remains (avoid roundoff error)
		new_اندازه = ارتفاع;
	    وإلا إذا (dir == 'h')
		new_اندازه = fr->fr_ارتفاع;
	    وإلا إذا (قاب_fixed_ارتفاع(fr))
	    {
		new_اندازه = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    وإلا
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = قاب_minheight(fr, NOWIN);
		wincount = قاب_wincount_درون_ارتفاع(fr);
		m = قاب_minheight(fr, بعدی_curwin);
		إذا (has_بعدی_curwin)
		    hnc = قاب_has_win(fr, بعدی_curwin);
		وإلا
		    hnc = FALSE;
		إذا (hnc)	    // don't count next_curwin
		    --wincount;
		إذا (totwincount == 0)
		    new_اندازه = جا;
		وإلا
		    new_اندازه = (wincount * جا + ((بیعلامت)totwincount >> 1))
								/ totwincount;
		إذا (hnc)	    // add next_curwin size
		{
		    بعدی_curwin_اندازه -= p_wh - (m - n);
		    new_اندازه += بعدی_curwin_اندازه;
		    جا -= new_اندازه - بعدی_curwin_اندازه;
		}
		وإلا
		    جا -= new_اندازه;
		new_اندازه += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    إذا (!current || dir != 'h' || topfr->fr_والد != عدم
		    || (new_اندازه != fr->fr_ارتفاع)
		    || قاب_has_win(fr, بعدی_curwin))
		win_equal_rec(بعدی_curwin, current, fr, dir, ستون, ردیف,
							     عرض, new_اندازه);
	    ردیف += new_اندازه;
	    ارتفاع -= new_اندازه;
	    totwincount -= wincount;
	}
    }
}

#إذا defined(FEAT_کار_CHANNEL)
    فراغ
leaving_پنجره(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    إذا (!bt_prompt(win->w_میانگیر) || is_aucmd_win(win))
	أرجع;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_میانگیر->b_prompt_درج = restart_edit;
    إذا (restart_edit != 0 && نما_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    إذا ((State & نما_درج) && !stop_درج_نما)
    {
	stop_درج_نما = TRUE;
	إذا (win->w_میانگیر->b_prompt_درج == NUL)
	    win->w_میانگیر->b_prompt_درج = 'A';
    }
}

    فراغ
entering_پنجره(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    إذا (!bt_prompt(win->w_میانگیر) || is_aucmd_win(win))
	أرجع;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    إذا (win->w_میانگیر->b_prompt_درج != NUL)
	stop_درج_نما = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    إذا ((State & نما_درج) == 0)
	restart_edit = win->w_میانگیر->b_prompt_درج;
}
#پایان_اگر

    ساكن فراغ
win_init_خالی(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_سطرها_valid = 0;
    wp->w_cursor.lnum = 1;
    wp->w_curswant = wp->w_cursor.ستون = 0;
    wp->w_cursor.coladd = 0;
    wp->w_pcmark.lnum = 1;	// pcmark not cleared but set to line 1
    wp->w_pcmark.ستون = 0;
    wp->w_prev_pcmark.lnum = 0;
    wp->w_prev_pcmark.ستون = 0;
    wp->w_topline = 1;
#اگر_تعریف_شده FEAT_DIFF
    wp->w_topfill = 0;
#پایان_اگر
    wp->w_botline = 2;
    wp->w_valid = 0;
#إذا defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    wp->w_s = &wp->w_میانگیر->b_s;
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
    جمله_reset_hlfwin(wp);
#پایان_اگر
}

/*
 * Init the current window "curwin".
 * Called when a new file is being edited.
 */
    فراغ
curwin_init(فراغ)
{
    win_init_خالی(curwin);
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
    صحيح		شمار = tabpage_شاخص(عدم);

    ++RedrawingDisabled;

    لكل (wp = firstwin; wp != عدم && !ONE_پنجره; )
    {
	إذا (wp->w_میانگیر == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_میانگیر->b_locked > 0))
	{
	    إذا (پنجره_layout_locked(CMD_اندازه))
		اذهب theend;  // Only give one error message.
	    إذا (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		اكسر;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	وإلا
	    wp = wp->w_بعدی;
    }

    // Also check windows in other tab pages.
    لكل (tp = اولی_tabpage; tp != عدم; tp = nexttp)
    {
	nexttp = tp->tp_بعدی;
	إذا (tp != curtab)
	    FOR_ALL_WINDOWS_درون_TAB(tp, wp)
		إذا (wp->w_میانگیر == buf
		    && !(win_locked(wp) || wp->w_میانگیر->b_locked > 0))
		{
		    إذا (پنجره_layout_locked(CMD_اندازه))
			اذهب theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = اولی_tabpage;
		    اكسر;
		}
    }

theend:
    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;

    إذا (شمار != tabpage_شاخص(عدم))
	apply_autocmds(EVENT_TABCLOSED, عدم, عدم, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    صحيح
آخرین_پنجره(فراغ)
{
    أرجع (one_پنجره() && اولی_tabpage->tp_بعدی == عدم);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    صحيح
one_پنجره(فراغ)
{
    win_T	*wp;
    صحيح		دیده_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	إذا (!is_aucmd_win(wp))
	{
	    إذا (دیده_one)
		أرجع FALSE;
	    دیده_one = TRUE;
	}
    }
    أرجع TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    ساكن صحيح
close_آخرین_پنجره_tabpage(
    win_T	*win,
    صحيح		free_buf,
    tabpage_T   *prev_curtab)
{
    إذا (!ONE_پنجره)
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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_میانگیر != عدم);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    إذا (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    إذا (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#اگر_تعریف_شده FEAT_کار_CHANNEL
    entering_پنجره(curwin);
#پایان_اگر
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, عدم, عدم, FALSE, curbuf);
#إذا defined(FEAT_TABPANEL)
    إذا (p_stpl > 0)
	shell_new_columns();
#پایان_اگر
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
win_close_میانگیر(win_T *win, صحيح action, صحيح abort_if_آخرین)
{
#اگر_تعریف_شده FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    إذا (win->w_میانگیر != عدم)
	reset_synblock(win);
#پایان_اگر

#اگر_تعریف_شده FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    إذا (win->w_میانگیر != عدم && bt_quickfix(win->w_میانگیر)
					&& win->w_میانگیر->b_nwindows == 1)
	win->w_میانگیر->b_p_bl = FALSE;
#پایان_اگر

    // Close the link to the buffer.
    إذا (win->w_میانگیر != عدم)
    {
	bufref_T    bufref;

	مجموعه_bufref(&bufref, curbuf);
	++win->w_locked;
	close_میانگیر(win, win->w_میانگیر, action, abort_if_آخرین, TRUE, TRUE);
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
    صحيح		other_میانگیر = FALSE;
    صحيح		close_curwin = FALSE;
    صحيح		dir;
    صحيح		help_پنجره = FALSE;
    صحيح		quickfix_پنجره = FALSE;
    tabpage_T   *prev_curtab = curtab;
    قاب_T	*win_قاب = win->w_قاب->fr_والد;
#اگر_تعریف_شده FEAT_DIFF
    صحيح		had_diffmode = win->w_p_diff;
#پایان_اگر
#اگر_تعریف_شده پیام_صف
    صحيح		did_decrement = FALSE;
#پایان_اگر

#إذا defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    إذا (may_close_جمله_popup() == OK)
	أرجع OK;
#پایان_اگر
    إذا (ERROR_IF_ANY_POPUP_پنجره)
	أرجع FAIL;

    إذا (آخرین_پنجره())
    {
	emsg(_(e_cannot_close_آخرین_پنجره));
	أرجع FAIL;
    }
    إذا (پنجره_layout_locked(CMD_close))
	أرجع FAIL;

    إذا (win_locked(win) || (win->w_میانگیر != عدم
					       && win->w_میانگیر->b_locked > 0))
	أرجع FAIL; // window is already being closed
    إذا (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_پنجره));
	أرجع FAIL;
    }
    إذا ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_پنجره())
    {
	emsg(_(e_cannot_close_پنجره_only_autocmd_پنجره_would_remain));
	أرجع FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    إذا (close_آخرین_پنجره_tabpage(win, free_buf, prev_curtab))
	أرجع FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    إذا (bt_help(win->w_میانگیر))
	help_پنجره = TRUE;
    وإلا
	clear_snapshot(curtab, SNAP_HELP_IDX);

    إذا (bt_quickfix(win->w_میانگیر))
	quickfix_پنجره = TRUE;
    وإلا
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    إذا (win == curwin)
    {
#اگر_تعریف_شده FEAT_کار_CHANNEL
	leaving_پنجره(curwin);
#پایان_اگر
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, عدم));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	إذا (wp->w_میانگیر != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_میانگیر = TRUE;
	    إذا (!win_valid(win))
		أرجع FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, عدم, عدم, FALSE, curbuf);
	    إذا (!win_valid(win))
		أرجع FAIL;
	    --win->w_locked;
	    إذا (آخرین_پنجره())
		أرجع FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, عدم, عدم, FALSE, curbuf);
	إذا (!win_valid(win))
	    أرجع FAIL;
	--win->w_locked;
	إذا (آخرین_پنجره())
	    أرجع FAIL;
#اگر_تعریف_شده FEAT_EVAL
	// autocmds may abort script processing
	إذا (aborting())
	    أرجع FAIL;
#پایان_اگر
    }

#اگر_تعریف_شده FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    إذا (gui.درون_use)
	بیرون_flush();
#پایان_اگر

#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (popup_win_closed(win) && !win_valid(win))
	أرجع FAIL;
#پایان_اگر

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    إذا (!win_valid_any_tab(win))
	أرجع OK;

    win_close_میانگیر(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    إذا (win_valid(win) && win->w_میانگیر == عدم
#إذا defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#پایان_اگر
	    && آخرین_پنجره())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	إذا (curwin->w_میانگیر == عدم)
	    curwin->w_میانگیر = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    إذا (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_میانگیر == عدم)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	أرجع FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    إذا (!win_valid(win) || آخرین_پنجره()
	    || close_آخرین_پنجره_tabpage(win, free_buf, prev_curtab))
	أرجع FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++تقسیم_disallowed;
#اگر_تعریف_شده پیام_صف
    ++dont_parse_messages;
#پایان_اگر

    إذا (win->w_میانگیر != عدم)
	--win->w_میانگیر->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, عدم);

    إذا (help_پنجره || quickfix_پنجره)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_پنجره ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	إذا (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    إذا (win == curwin)
    {
	curwin = wp;
#اگر_تعریف_شده FEAT_QUICKFIX
	إذا (wp->w_p_pvw || bt_quickfix(wp->w_میانگیر))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    لكل (;;)
	    {
		إذا (wp->w_بعدی == عدم)
		    wp = firstwin;
		وإلا
		    wp = wp->w_بعدی;
		إذا (wp == curwin)
		    اكسر;
		إذا (!wp->w_p_pvw && !bt_quickfix(wp->w_میانگیر))
		{
		    curwin = wp;
		    اكسر;
		}
	    }
	}
#پایان_اگر
	curbuf = curwin->w_میانگیر;
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
    آخرین_وضعیت(FALSE);

    إذا (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_قاب->fr_والد == win_قاب, dir);
    وإلا
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    إذا (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#اگر_تعریف_شده پیام_صف
	did_decrement =
#وإلا
	(فراغ)
#پایان_اگر
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	إذا (other_میانگیر)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, عدم, عدم, FALSE, curbuf);
    }

    إذا (ONE_پنجره && curwin->w_locked && curbuf->b_locked_تقسیم
	    && اولی_tabpage->tp_بعدی != عدم)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, عدم, عدم, FALSE, curbuf);

    --تقسیم_disallowed;
#اگر_تعریف_شده پیام_صف
    إذا (!did_decrement)
	--dont_parse_messages;
#پایان_اگر

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    إذا (help_پنجره || quickfix_پنجره)
	restore_snapshot(help_پنجره ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#اگر_تعریف_شده FEAT_DIFF
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
#پایان_اگر

#إذا defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    إذا (gui.درون_use && !win_hasvertsplit())
	gui_init_which_components(عدم);
#پایان_اگر

    redraw_all_later(UPD_NOT_VALID);
    أرجع OK;
}

    ساكن فراغ
trigger_winnewpre(فراغ)
{
    پنجره_layout_قفل();
    apply_autocmds(EVENT_WINNEWPRE, عدم, عدم, FALSE, عدم);
    پنجره_layout_unlock();
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
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_میانگیر);
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
    پنجره_layout_قفل();
    apply_autocmds(EVENT_TABCLOSEDPRE, عدم, عدم, FALSE, عدم);
    پنجره_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    إذا (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    وإلا
	// fall back to the first tabpage
	goto_tabpage_tp(اولی_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    فراغ
snapshot_windows_scroll_اندازه(فراغ)
{
    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	wp->w_آخرین_topline = wp->w_topline;
#اگر_تعریف_شده FEAT_DIFF
	wp->w_آخرین_topfill = wp->w_topfill;
#پایان_اگر
	wp->w_آخرین_leftcol = wp->w_leftcol;
	wp->w_آخرین_skipcol = wp->w_skipcol;
	wp->w_آخرین_عرض = wp->w_عرض;
	wp->w_آخرین_ارتفاع = wp->w_ارتفاع;
    }
}

ساكن صحيح did_initial_scroll_اندازه_snapshot = FALSE;

    فراغ
may_make_initial_scroll_اندازه_snapshot(فراغ)
{
    إذا (!did_initial_scroll_اندازه_snapshot)
    {
	did_initial_scroll_اندازه_snapshot = TRUE;
	snapshot_windows_scroll_اندازه();
    }
}

#اگر_تعریف_شده FEAT_EVAL
/*
 * Create a dictionary with information about size and scroll changes in a
 * window.
 * Returns the dictionary with refcount set to one.
 * Returns NULL when out of memory.
 */
    ساكن dict_T *
make_win_info_dict(
	صحيح عرض,
	صحيح ارتفاع,
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
	tv.v_قفل = 0;
	tv.v_type = VAR_عدد;

	tv.vval.v_عدد = عرض;
	إذا (dict_جمع_کن_tv(d, "width", &tv) == FAIL)
	    اكسر;
	tv.vval.v_عدد = ارتفاع;
	إذا (dict_جمع_کن_tv(d, "height", &tv) == FAIL)
	    اكسر;
	tv.vval.v_عدد = topline;
	إذا (dict_جمع_کن_tv(d, "topline", &tv) == FAIL)
	    اكسر;
# ifdef FEAT_DIFF
	tv.vval.v_عدد = topfill;
# وإلا
	tv.vval.v_عدد = 0;
# endif
	إذا (dict_جمع_کن_tv(d, "topfill", &tv) == FAIL)
	    اكسر;
	tv.vval.v_عدد = leftcol;
	إذا (dict_جمع_کن_tv(d, "leftcol", &tv) == FAIL)
	    اكسر;
	tv.vval.v_عدد = skipcol;
	إذا (dict_جمع_کن_tv(d, "skipcol", &tv) == FAIL)
	    اكسر;
	أرجع d;
    }
    dict_unref(d);
    أرجع عدم;
}
#پایان_اگر

// Return values of check_window_scroll_resize():
#تعریف_کن CWSR_SCROLLED	1  // at least one window scrolled
#تعریف_کن CWSR_RESIZED	2  // at least one window size changed

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
check_پنجره_scroll_resize(
	صحيح	*اندازه_شمار,
	win_T	**اولی_scroll_win,
	win_T	**اولی_اندازه_win,
	list_T	*winlist استفادهنشده,
	dict_T	*v_event استفادهنشده)
{
#اگر_تعریف_شده FEAT_EVAL
    صحيح listidx = 0;
    صحيح tot_عرض = 0;
    صحيح tot_ارتفاع = 0;
    صحيح tot_topline = 0;
# ifdef FEAT_DIFF
    صحيح tot_topfill = 0;
# endif
    صحيح tot_leftcol = 0;
    صحيح tot_skipcol = 0;
#پایان_اگر

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	صحيح ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	صحيح اندازه_تغییریافته = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_آخرین_عرض != wp->w_عرض
			    || wp->w_آخرین_ارتفاع != wp->w_ارتفاع);
	إذا (اندازه_تغییریافته)
	{
#اگر_تعریف_شده FEAT_EVAL
	    إذا (winlist != عدم)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_قفل = 0;
		tv.v_type = VAR_عدد;
		tv.vval.v_عدد = wp->w_id;
		list_مجموعه_قلم(winlist, listidx++, &tv);
	    }
	    وإلا
#پایان_اگر
		إذا (اندازه_شمار != عدم)
	    {
		++*اندازه_شمار;
		إذا (*اولی_اندازه_win == عدم)
		    *اولی_اندازه_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		إذا (*اولی_scroll_win == عدم && !ignore_scroll)
		    *اولی_scroll_win = wp;
	    }
	}

	صحيح scroll_تغییریافته = !ignore_scroll
				&& (wp->w_آخرین_topline != wp->w_topline
#اگر_تعریف_شده FEAT_DIFF
				|| wp->w_آخرین_topfill != wp->w_topfill
#پایان_اگر
				|| wp->w_آخرین_leftcol != wp->w_leftcol
				|| wp->w_آخرین_skipcol != wp->w_skipcol);
	إذا (scroll_تغییریافته
	    && اولی_scroll_win != عدم && *اولی_scroll_win == عدم)
	    *اولی_scroll_win = wp;

#اگر_تعریف_شده FEAT_EVAL
	إذا ((اندازه_تغییریافته || scroll_تغییریافته) && v_event != عدم)
	{
	    // Add info about this window to the v:event dictionary.
	    صحيح عرض = wp->w_عرض - wp->w_آخرین_عرض;
	    صحيح ارتفاع = wp->w_ارتفاع - wp->w_آخرین_ارتفاع;
	    صحيح topline = wp->w_topline - wp->w_آخرین_topline;
# ifdef FEAT_DIFF
	    صحيح topfill = wp->w_topfill - wp->w_آخرین_topfill;
# endif
	    صحيح leftcol = wp->w_leftcol - wp->w_آخرین_leftcol;
	    صحيح skipcol = wp->w_skipcol - wp->w_آخرین_skipcol;
	    dict_T *d = make_win_info_dict(عرض, ارتفاع, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    إذا (d == عدم)
		اكسر;
	    حرف winid[NUMBUFLEN];
	    vim_snprintf(winid, حجم(winid), "%d", wp->w_id);
	    إذا (dict_جمع_کن_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		اكسر;
	    }
	    --d->dv_refcount;

	    tot_عرض += قدر_مطلق(عرض);
	    tot_ارتفاع += قدر_مطلق(ارتفاع);
	    tot_topline += قدر_مطلق(topline);
# ifdef FEAT_DIFF
	    tot_topfill += قدر_مطلق(topfill);
# endif
	    tot_leftcol += قدر_مطلق(leftcol);
	    tot_skipcol += قدر_مطلق(skipcol);
	}
#پایان_اگر
    }

#اگر_تعریف_شده FEAT_EVAL
    إذا (v_event != عدم)
    {
	dict_T *alldict = make_win_info_dict(tot_عرض, tot_ارتفاع, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	إذا (alldict != عدم)
	{
	    إذا (dict_جمع_کن_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    وإلا
		--alldict->dv_refcount;
	}
    }
#پایان_اگر
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
	    || !did_initial_scroll_اندازه_snapshot)
	أرجع;

    صحيح اندازه_شمار = 0;
    win_T *اولی_scroll_win = عدم, *اولی_اندازه_win = عدم;
    check_پنجره_scroll_resize(&اندازه_شمار, &اولی_scroll_win, &اولی_اندازه_win,
								    عدم, عدم);
    صحيح trigger_resize = do_resize && اندازه_شمار > 0;
    صحيح trigger_scroll = do_scroll && اولی_scroll_win != عدم;
    إذا (!trigger_resize && !trigger_scroll)
	أرجع;  // no relevant changes
#اگر_تعریف_شده FEAT_EVAL
    list_T *windows_list = عدم;
    إذا (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_اقلام(اندازه_شمار);
	إذا (windows_list != عدم)
	    check_پنجره_scroll_resize(عدم, عدم, عدم, windows_list, عدم);
    }

    dict_T *scroll_dict = عدم;
    إذا (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	إذا (scroll_dict != عدم)
	{
	    scroll_dict->dv_refcount = 1;
	    check_پنجره_scroll_resize(عدم, عدم, عدم, عدم, scroll_dict);
	}
    }
#پایان_اگر

    // WinScrolled/WinResized are triggered only once, even when multiple
    // windows scrolled or changed size.  Store the current values before
    // triggering the event, if a scroll or resize happens as a side effect
    // then WinScrolled/WinResized is triggered for that later.
    snapshot_windows_scroll_اندازه();

    // "curwin" may be different from the actual current window, make
    // sure it can be restored.
    پنجره_layout_قفل();
    recursive = TRUE;

    // If both are to be triggered do WinResized first.
    إذا (trigger_resize
#اگر_تعریف_شده FEAT_EVAL
	    && windows_list != عدم
#پایان_اگر
	    )
    {
#اگر_تعریف_شده FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	إذا (dict_جمع_کن_list(v_event, "windows", windows_list) == OK)
	{
	    dict_مجموعه_اقلام_ro(v_event);
#پایان_اگر
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((حرف *)winid, حجم(winid), "%d",
							 اولی_اندازه_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     اولی_اندازه_win->w_میانگیر);
#اگر_تعریف_شده FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#پایان_اگر
    }

    إذا (trigger_scroll
#اگر_تعریف_شده FEAT_EVAL
	    && scroll_dict != عدم
#پایان_اگر
	    )
    {
#اگر_تعریف_شده FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", عدم);
	dict_مجموعه_اقلام_ro(v_event);
	dict_unref(scroll_dict);
#پایان_اگر
	char_u winid[NUMBUFLEN];
	vim_snprintf((حرف *)winid, حجم(winid), "%d",
						       اولی_scroll_win->w_id);
	apply_autocmds(EVENT_WINSCROLLED, winid, winid, FALSE,
						   اولی_scroll_win->w_میانگیر);
#اگر_تعریف_شده FEAT_EVAL
	restore_v_event(v_event, &save_v_event);
#پایان_اگر
    }

    recursive = FALSE;
    پنجره_layout_unlock();
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
    إذا (پنجره_layout_locked(CMD_اندازه))
	أرجع;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    إذا (win_locked(win) || (win->w_میانگیر != عدم
					       && win->w_میانگیر->b_locked > 0))
	أرجع; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    إذا (win->w_میانگیر != عدم)
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

    إذا (win->w_میانگیر != عدم)
	// Close the link to the buffer.
	close_میانگیر(win, win->w_میانگیر, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    لكل (ptp = اولی_tabpage; ptp != عدم && ptp != tp; ptp = ptp->tp_بعدی)
	;
    إذا (ptp == عدم || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	إذا (win_valid_any_tab(win) && win->w_میانگیر == عدم)
	{
	    win->w_میانگیر = firstbuf;
	    ++firstbuf->b_nwindows;
	    إذا (win == curwin)
		curbuf = curwin->w_میانگیر;
	    win_init_خالی(win);
	}
	أرجع;
    }

    // Autocommands may have closed the window already.
    لكل (wp = tp->tp_firstwin; wp != عدم && wp != win; wp = wp->w_بعدی)
	;
    إذا (wp == عدم)
	أرجع;

    // When closing the last window in a tab page remove the tab page.
    إذا (tp->tp_firstwin == tp->tp_lastwin)
    {
	صحيح	h = tabline_ارتفاع();

	إذا (tp == اولی_tabpage)
	    اولی_tabpage = tp->tp_بعدی;
	وإلا
	{
	    لكل (ptp = اولی_tabpage; ptp != عدم && ptp->tp_بعدی != tp;
							   ptp = ptp->tp_بعدی)
		;
	    إذا (ptp == عدم)
	    {
		داخلی_error("win_close_othertab()");
		أرجع;
	    }
	    ptp->tp_بعدی = tp->tp_بعدی;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	إذا (h != tabline_ارتفاع())
	    shell_new_ردیفها();
    }

    إذا (win->w_میانگیر != عدم)
	--win->w_میانگیر->b_nwindows;

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
    قاب_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == عدم ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_قاب;
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

    طالما (اولی_tabpage->tp_بعدی != عدم)
	tabpage_close(TRUE);

    لكل (صحيح i = 0; i < AUCMD_WIN_شمار; ++i)
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
#پایان_اگر

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    صحيح		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    قاب_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    قاب_T	*frp, *frp2, *frp3;
    قاب_T	*frp_close = win->w_قاب;
    win_T	*wp;
    صحيح		ردیف, ستون;

    /*
     * If there is only one window there is nothing to remove.
     */
    إذا (tp == عدم ? ONE_پنجره : tp->tp_firstwin == tp->tp_lastwin)
	أرجع عدم;

    قاب_locked++;

    // Save the position of the containing frame (which will also contain the
    // altframe) before we remove anything, to recompute window positions later.
    wp = frame2win(frp_close->fr_والد);
    ردیف = wp->w_winrow;
    ستون = wp->w_wincol;

    /*
     * Remove the window from its frame.
     */
    frp2 = win_altframe(win, tp);
    wp = frame2win(frp2);

    // If this is a rightmost window, remove vertical separators to the left.
    إذا (win->w_vsep_عرض == 0 && frp_close->fr_والد->fr_layout == FR_ردیف &&
	    frp_close->fr_prev != عدم)
	قاب_مجموعه_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    قاب_remove(frp_close);

    إذا (frp_close->fr_والد->fr_layout == FR_ستون)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	إذا (frp2->fr_win != عدم && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_بعدی;
	    طالما (frp != عدم || frp3 != عدم)
	    {
		إذا (frp != عدم)
		{
		    إذا (!قاب_fixed_ارتفاع(frp))
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
		    frp3 = frp3->fr_بعدی;
		}
	    }
	}
	قاب_new_ارتفاع(frp2, frp2->fr_ارتفاع + frp_close->fr_ارتفاع,
			    frp2 == frp_close->fr_بعدی, !قاب_fixed_ارتفاع(frp2), FALSE);
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
	    frp3 = frp_close->fr_بعدی;
	    طالما (frp != عدم || frp3 != عدم)
	    {
		إذا (frp != عدم)
		{
		    إذا (!قاب_fixed_عرض(frp))
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
		    frp3 = frp3->fr_بعدی;
		}
	    }
	}
	قاب_new_عرض(frp2, frp2->fr_عرض + frp_close->fr_عرض,
			    frp2 == frp_close->fr_بعدی, !قاب_fixed_عرض(frp2));
	*dirp = 'h';
    }

    // If the altframe wasn't adjacent and left/above, resizing it will have
    // changed window positions within the parent frame.  Recompute them.
    إذا (frp2 != frp_close->fr_prev)
	قاب_comp_pos(frp_close->fr_والد, &ردیف, &ستون);

    إذا (unflat_altfr == عدم)
	قاب_flatten(frp2);
    وإلا
	*unflat_altfr = frp2;

    قاب_locked--;

    أرجع wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    ساكن فراغ
قاب_flatten(قاب_T *frp)
{
    قاب_T	*frp2, *frp3;

    إذا (frp->fr_بعدی != عدم || frp->fr_prev != عدم)
	أرجع;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_والد->fr_layout = frp->fr_layout;
    frp->fr_والد->fr_فرزند = frp->fr_فرزند;
    FOR_ALL_قابها(frp2, frp->fr_فرزند)
	frp2->fr_والد = frp->fr_والد;
    frp->fr_والد->fr_win = frp->fr_win;
    إذا (frp->fr_win != عدم)
	frp->fr_win->w_قاب = frp->fr_والد;
    frp2 = frp->fr_والد;
    إذا (topframe->fr_فرزند == frp)
	topframe->fr_فرزند = frp2;
    vim_free(frp);

    frp = frp2->fr_والد;
    إذا (frp != عدم && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	إذا (frp->fr_فرزند == frp2)
	    frp->fr_فرزند = frp2->fr_فرزند;
	frp2->fr_فرزند->fr_prev = frp2->fr_prev;
	إذا (frp2->fr_prev != عدم)
	    frp2->fr_prev->fr_بعدی = frp2->fr_فرزند;
	لكل (frp3 = frp2->fr_فرزند; ; frp3 = frp3->fr_بعدی)
	{
	    frp3->fr_والد = frp;
	    إذا (frp3->fr_بعدی == عدم)
	    {
		frp3->fr_بعدی = frp2->fr_بعدی;
		إذا (frp2->fr_بعدی != عدم)
		    frp2->fr_بعدی->fr_prev = frp3;
		اكسر;
	    }
	}
	إذا (topframe->fr_فرزند == frp2)
	    topframe->fr_فرزند = frp;
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
winframe_restore(win_T *wp, صحيح dir, قاب_T *unflat_altfr)
{
    قاب_T	*frp = wp->w_قاب;

    // Put "wp"'s frame back where it was.
    إذا (frp->fr_prev != عدم)
	قاب_append(frp->fr_prev, frp);
    وإلا
	قاب_درج(frp->fr_بعدی, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    إذا (wp->w_vsep_عرض == 0
	    && frp->fr_والد->fr_layout == FR_ردیف && frp->fr_prev != عدم)
	قاب_مجموعه_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    إذا (wp->w_وضعیت_ارتفاع == 0
	    && frp->fr_والد->fr_layout == FR_ستون && frp->fr_prev != عدم)
	قاب_جمع_کن_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    إذا (dir == 'v')
    {
	قاب_new_ارتفاع(unflat_altfr, unflat_altfr->fr_ارتفاع - frp->fr_ارتفاع,
		unflat_altfr == frp->fr_بعدی, FALSE, FALSE);
    }
    وإلا إذا (dir == 'h')
    {
	قاب_new_عرض(unflat_altfr, unflat_altfr->fr_عرض - frp->fr_عرض,
		unflat_altfr == frp->fr_بعدی, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    إذا (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_والد);
	صحيح	ردیف = topleft->w_winrow;
	صحيح	ستون = topleft->w_wincol;

	قاب_comp_pos(frp->fr_والد, &ردیف, &ستون);
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
    ساكن قاب_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    قاب_T	*frp;
    قاب_T	*other_fr, *هدف_fr;

    إذا (tp == عدم ? ONE_پنجره : tp->tp_firstwin == tp->tp_lastwin)
	أرجع alt_tabpage()->tp_curwin->w_قاب;

    frp = win->w_قاب;

    إذا (frp->fr_prev == عدم)
	أرجع frp->fr_بعدی;
    إذا (frp->fr_بعدی == عدم)
	أرجع frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    //پنجره
    هدف_fr = frp->fr_بعدی;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    إذا (frp->fr_والد != عدم && frp->fr_والد->fr_layout == FR_ستون && p_sb)
    {
	هدف_fr = frp->fr_prev;
	other_fr  = frp->fr_بعدی;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    إذا (frp->fr_والد != عدم && frp->fr_والد->fr_layout == FR_ردیف && p_spr)
    {
	هدف_fr = frp->fr_prev;
	other_fr  = frp->fr_بعدی;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    إذا (frp->fr_والد != عدم && frp->fr_والد->fr_layout == FR_ردیف)
    {
	إذا (قاب_fixed_عرض(هدف_fr) && !قاب_fixed_عرض(other_fr))
	    هدف_fr = other_fr;
    }
    وإلا
    {
	إذا (قاب_fixed_ارتفاع(هدف_fr) && !قاب_fixed_ارتفاع(other_fr))
	    هدف_fr = other_fr;
    }

    أرجع هدف_fr;
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
    forward = curtab->tp_بعدی != عدم &&
	    ((tcl_flags & TCL_چپ) == 0 || curtab == اولی_tabpage);

    إذا (forward)
	tp = curtab->tp_بعدی;
    وإلا
	// Use the previous tab page.
	لكل (tp = اولی_tabpage; tp->tp_بعدی != curtab; tp = tp->tp_بعدی)
	    ;

    أرجع tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    ساكن win_T *
frame2win(قاب_T *frp)
{
    طالما (frp->fr_win == عدم)
	frp = frp->fr_فرزند;
    أرجع frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    ساكن صحيح
قاب_has_win(قاب_T *frp, win_T *wp)
{
    قاب_T	*p;

    إذا (frp->fr_layout == FR_برگ)
	أرجع frp->fr_win == wp;

    FOR_ALL_قابها(p, frp->fr_فرزند)
	إذا (قاب_has_win(p, wp))
	    أرجع TRUE;
    أرجع FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
ساكن صحيح min_مجموعه_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    ساكن فراغ
قاب_new_ارتفاع(
    قاب_T	*topfrp,
    صحيح		ارتفاع,
    صحيح		topfirst,	// resize topmost contained frame first
    صحيح		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    صحيح		مجموعه_ch)		// set 'cmdheight' to resize topframe
{
    قاب_T	*frp;
    صحيح		extra_سطرها;
    صحيح		h;

    إذا (topfrp->fr_والد == عدم && مجموعه_ch)
    {
	// topframe: update the command line height, with side effects.
	صحيح new_ch = MAX(min_مجموعه_ch, p_ch + topfrp->fr_ارتفاع - ارتفاع);
	صحيح save_ch = min_مجموعه_ch;
	إذا (new_ch != p_ch)
	    مجموعه_option_مقدار((char_u *)"cmdheight", new_ch, عدم, 0);
	min_مجموعه_ch = save_ch;
	ارتفاع = MIN(ارتفاع, ردیفها_AVAIL);
    }
    إذا (topfrp->fr_win != عدم)
    {
	// Simple case: just one window.
	win_new_ارتفاع(topfrp->fr_win,
				    ارتفاع - topfrp->fr_win->w_وضعیت_ارتفاع
					      - WINBAR_ارتفاع(topfrp->fr_win));
    }
    وإلا إذا (topfrp->fr_layout == FR_ردیف)
    {
	افعل
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_قابها(frp, topfrp->fr_فرزند)
	    {
		قاب_new_ارتفاع(frp, ارتفاع, topfirst, wfh, مجموعه_ch);
		إذا (frp->fr_ارتفاع > ارتفاع)
		{
		    // Could not fit the windows, make the whole row higher.
		    ارتفاع = frp->fr_ارتفاع;
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

	frp = topfrp->fr_فرزند;
	إذا (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    طالما (قاب_fixed_ارتفاع(frp))
	    {
		frp = frp->fr_بعدی;
		إذا (frp == عدم)
		    أرجع;	    // no frame without 'wfh', give up
	    }
	إذا (!topfirst)
	{
	    // Find the bottom frame of this column
	    طالما (frp->fr_بعدی != عدم)
		frp = frp->fr_بعدی;
	    إذا (wfh)
		// Advance back for frames with one window with 'wfh' set.
		طالما (قاب_fixed_ارتفاع(frp))
		    frp = frp->fr_prev;
	}

	extra_سطرها = ارتفاع - topfrp->fr_ارتفاع;
	إذا (extra_سطرها < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    طالما (frp != عدم)
	    {
		h = قاب_minheight(frp, عدم);
		إذا (frp->fr_ارتفاع + extra_سطرها < h)
		{
		    extra_سطرها += frp->fr_ارتفاع - h;
		    قاب_new_ارتفاع(frp, h, topfirst, wfh, مجموعه_ch);
		}
		وإلا
		{
		    قاب_new_ارتفاع(frp, frp->fr_ارتفاع + extra_سطرها,
							topfirst, wfh, مجموعه_ch);
		    اكسر;
		}
		إذا (topfirst)
		{
		    افعل
			frp = frp->fr_بعدی;
		    طالما (wfh && frp != عدم && قاب_fixed_ارتفاع(frp));
		}
		وإلا
		{
		    افعل
			frp = frp->fr_prev;
		    طالما (wfh && frp != عدم && قاب_fixed_ارتفاع(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		إذا (frp == عدم)
		    ارتفاع -= extra_سطرها;
	    }
	}
	وإلا إذا (extra_سطرها > 0)
	{
	    // increase height of bottom or top frame
	    قاب_new_ارتفاع(frp, frp->fr_ارتفاع + extra_سطرها, topfirst, wfh,
									مجموعه_ch);
	}
    }
    topfrp->fr_ارتفاع = ارتفاع;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    ساكن صحيح
قاب_fixed_ارتفاع(قاب_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    إذا (frp->fr_win != عدم)
	أرجع frp->fr_win->w_p_wfh;

    إذا (frp->fr_layout == FR_ردیف)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_قابها(frp, frp->fr_فرزند)
	    إذا (قاب_fixed_ارتفاع(frp))
		أرجع TRUE;
	أرجع FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_قابها(frp, frp->fr_فرزند)
	إذا (!قاب_fixed_ارتفاع(frp))
	    أرجع FALSE;
    أرجع TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    ساكن صحيح
قاب_fixed_عرض(قاب_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    إذا (frp->fr_win != عدم)
	أرجع frp->fr_win->w_p_wfw;

    إذا (frp->fr_layout == FR_ستون)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_قابها(frp, frp->fr_فرزند)
	    إذا (قاب_fixed_عرض(frp))
		أرجع TRUE;
	أرجع FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_قابها(frp, frp->fr_فرزند)
	إذا (!قاب_fixed_عرض(frp))
	    أرجع FALSE;
    أرجع TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    ساكن فراغ
قاب_جمع_کن_statusline(قاب_T *frp)
{
    win_T	*wp;

    إذا (frp->fr_layout == FR_برگ)
    {
	wp = frp->fr_win;
	wp->w_وضعیت_ارتفاع = statusline_ارتفاع(wp);
    }
    وإلا إذا (frp->fr_layout == FR_ردیف)
    {
	// Handle all the frames in the row.
	FOR_ALL_قابها(frp, frp->fr_فرزند)
	    قاب_جمع_کن_statusline(frp);
    }
    وإلا // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	لكل (frp = frp->fr_فرزند; frp->fr_بعدی != عدم; frp = frp->fr_بعدی)
	    ;
	قاب_جمع_کن_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    ساكن فراغ
قاب_new_عرض(
    قاب_T	*topfrp,
    صحيح		عرض,
    صحيح		leftfirst,	// resize leftmost contained frame first
    صحيح		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    قاب_T	*frp;
    صحيح		extra_تعداد_ستون;
    صحيح		w;
    win_T	*wp;

    إذا (topfrp->fr_layout == FR_برگ)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	لكل (frp = topfrp; frp->fr_والد != عدم; frp = frp->fr_والد)
	    إذا (frp->fr_والد->fr_layout == FR_ردیف && frp->fr_بعدی != عدم)
		اكسر;
	إذا (frp->fr_والد == عدم)
	    wp->w_vsep_عرض = 0;
	win_new_عرض(wp, عرض - wp->w_vsep_عرض);
    }
    وإلا إذا (topfrp->fr_layout == FR_ستون)
    {
	افعل
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_قابها(frp, topfrp->fr_فرزند)
	    {
		قاب_new_عرض(frp, عرض, leftfirst, wfw);
		إذا (frp->fr_عرض > عرض)
		{
		    // Could not fit the windows, make whole column wider.
		    عرض = frp->fr_عرض;
		    اكسر;
		}
	    }
	} طالما (frp != عدم);
    }
    وإلا    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_فرزند;
	إذا (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    طالما (قاب_fixed_عرض(frp))
	    {
		frp = frp->fr_بعدی;
		إذا (frp == عدم)
		    أرجع;	    // no frame without 'wfw', give up
	    }
	إذا (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    طالما (frp->fr_بعدی != عدم)
		frp = frp->fr_بعدی;
	    إذا (wfw)
		// Advance back for frames with one window with 'wfw' set.
		طالما (قاب_fixed_عرض(frp))
		    frp = frp->fr_prev;
	}

	extra_تعداد_ستون = عرض - topfrp->fr_عرض;
	إذا (extra_تعداد_ستون < 0)
	{
	    // reduce frame width, rightmost frame first
	    طالما (frp != عدم)
	    {
		w = قاب_minwidth(frp, عدم);
		إذا (frp->fr_عرض + extra_تعداد_ستون < w)
		{
		    extra_تعداد_ستون += frp->fr_عرض - w;
		    قاب_new_عرض(frp, w, leftfirst, wfw);
		}
		وإلا
		{
		    قاب_new_عرض(frp, frp->fr_عرض + extra_تعداد_ستون,
							      leftfirst, wfw);
		    اكسر;
		}
		إذا (leftfirst)
		{
		    افعل
			frp = frp->fr_بعدی;
		    طالما (wfw && frp != عدم && قاب_fixed_عرض(frp));
		}
		وإلا
		{
		    افعل
			frp = frp->fr_prev;
		    طالما (wfw && frp != عدم && قاب_fixed_عرض(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		إذا (frp == عدم)
		    عرض -= extra_تعداد_ستون;
	    }
	}
	وإلا إذا (extra_تعداد_ستون > 0)
	{
	    // increase width of rightmost frame
	    قاب_new_عرض(frp, frp->fr_عرض + extra_تعداد_ستون, leftfirst, wfw);
	}
    }
    topfrp->fr_عرض = عرض;
}

/*
 * Add or remove the vertical separator of windows to the right side of "frp".
 * Note: Does not check if there is room!
 */
    ساكن فراغ
قاب_مجموعه_vsep(قاب_T *frp, صحيح جمع_کن)
{
    win_T	*wp;

    إذا (frp->fr_layout == FR_برگ)
    {
	wp = frp->fr_win;
	إذا (جمع_کن && wp->w_vsep_عرض == 0)
	{
	    إذا (wp->w_عرض > 0)	// don't make it negative
		win_new_عرض(wp, wp->w_عرض - 1);
	    wp->w_vsep_عرض = 1;
	}
	وإلا إذا (!جمع_کن && wp->w_vsep_عرض == 1)
	{
	    win_new_عرض(wp, wp->w_عرض + 1);
	    wp->w_vsep_عرض = 0;
	}
    }
    وإلا إذا (frp->fr_layout == FR_ستون)
    {
	// Handle all the frames in the column.
	FOR_ALL_قابها(frp, frp->fr_فرزند)
	    قاب_مجموعه_vsep(frp, جمع_کن);
    }
    وإلا // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_فرزند;
	طالما (frp->fr_بعدی != عدم)
	    frp = frp->fr_بعدی;
	قاب_مجموعه_vsep(frp, جمع_کن);
    }
}

/*
 * Set frame width from the window it contains.
 */
    ساكن فراغ
قاب_fix_عرض(win_T *wp)
{
    wp->w_قاب->fr_عرض = wp->w_عرض + wp->w_vsep_عرض;
}

/*
 * Set frame height from the window it contains.
 */
    ساكن فراغ
قاب_fix_ارتفاع(win_T *wp)
{
    wp->w_قاب->fr_ارتفاع = VISIBLE_ارتفاع(wp) + wp->w_وضعیت_ارتفاع;
}

/*
 * Compute the minimal height for frame "topfrp".
 * Uses the 'winminheight' option.
 * When "next_curwin" isn't NULL, use p_wh for this window.
 * When "next_curwin" is NOWIN, don't use at least one line for the current
 * window.
 */
    ساكن صحيح
قاب_minheight(قاب_T *topfrp, win_T *بعدی_curwin)
{
    قاب_T	*frp;
    صحيح		m;
    صحيح		n;

    إذا (topfrp->fr_win != عدم)
    {
	إذا (topfrp->fr_win == بعدی_curwin)
	    m = p_wh + topfrp->fr_win->w_وضعیت_ارتفاع;
	وإلا
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_وضعیت_ارتفاع;
	    إذا (topfrp->fr_win == curwin && بعدی_curwin == عدم)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		إذا (p_wmh == 0)
		    ++m;
		m += WINBAR_ارتفاع(curwin);
	    }
	}
    }
    وإلا إذا (topfrp->fr_layout == FR_ردیف)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_قابها(frp, topfrp->fr_فرزند)
	{
	    n = قاب_minheight(frp, بعدی_curwin);
	    إذا (n > m)
		m = n;
	}
    }
    وإلا
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_قابها(frp, topfrp->fr_فرزند)
	    m += قاب_minheight(frp, بعدی_curwin);
    }

    أرجع m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
ساكن صحيح
قاب_wincount_درون_ارتفاع(قاب_T *topfrp)
{
    قاب_T	*frp;
    صحيح		c = 0;
    صحيح		n;

    إذا (topfrp->fr_win != عدم)
	c = 1;
    وإلا إذا (topfrp->fr_layout == FR_ردیف)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_قابها(frp, topfrp->fr_فرزند)
	{
	    n = قاب_wincount_درون_ارتفاع(frp);
	    إذا (n > c)
		c = n;
	}
    }
    وإلا
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_قابها(frp, topfrp->fr_فرزند)
	    c += قاب_wincount_درون_ارتفاع(frp);
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
قاب_minwidth(
    قاب_T	*topfrp,
    win_T	*بعدی_curwin)	// use p_wh and p_wiw for next_curwin
{
    قاب_T	*frp;
    صحيح		m, n;

    إذا (topfrp->fr_win != عدم)
    {
	إذا (topfrp->fr_win == بعدی_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_عرض;
	وإلا
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_عرض;
	    // Current window is minimal one column wide
	    إذا (p_wmw == 0 && topfrp->fr_win == curwin && بعدی_curwin == عدم)
		++m;
	}
    }
    وإلا إذا (topfrp->fr_layout == FR_ستون)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_قابها(frp, topfrp->fr_فرزند)
	{
	    n = قاب_minwidth(frp, بعدی_curwin);
	    إذا (n > m)
		m = n;
	}
    }
    وإلا
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_قابها(frp, topfrp->fr_فرزند)
	    m += قاب_minwidth(frp, بعدی_curwin);
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
    صحيح		پیام_2,
    صحيح		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    صحيح		r;

    إذا (one_پنجره())
    {
	إذا (پیام_2 && !autocmd_busy)
	    پیام(_(m_onlyone));
	أرجع;
    }

    // Be very careful here: autocommands may change the window layout.
    لكل (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_بعدی;

	// autocommands messed this one up
	إذا (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_میانگیر;
	}

	إذا (wp == curwin)		// don't close current window
	    استمر;

	// autoccommands messed this one up
	إذا (!buf_valid(wp->w_میانگیر) && win_valid(wp))
	{
	    wp->w_میانگیر = عدم;
	    win_close(wp, 0);
	    استمر;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_میانگیر, forceit);
	إذا (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    استمر;
	}
	إذا (!r)
	{
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    إذا (پیام_2 && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_تغییریافته(wp->w_میانگیر, FALSE);
		إذا (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    استمر;
		}
	    }
	    إذا (buf_Is_تغییریافته(wp->w_میانگیر))
#پایان_اگر
		استمر;
	}
	win_close(wp, !buf_hide(wp->w_میانگیر) && !buf_Is_تغییریافته(wp->w_میانگیر));
    }

    إذا (پیام_2 && !ONE_پنجره)
	emsg(_(e_other_پنجره_شامل_است_changes));
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
ساكن صحيح command_قاب_ارتفاع = TRUE;

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
win_alloc_اولی(فراغ)
{
    إذا (win_alloc_firstwin(عدم) == FAIL)
	أرجع FAIL;

    اولی_tabpage = alloc_tabpage();
    إذا (اولی_tabpage == عدم)
	أرجع FAIL;
    curtab = اولی_tabpage;
    unuse_tabpage(اولی_tabpage);

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
    new_قاب(wp);
    أرجع wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    فراغ
win_init_popup_win(win_T *wp, buf_T *buf)
{
    wp->w_میانگیر = buf;
    ++buf->b_nwindows;
    win_init_خالی(wp); // set cursor and topline to safe values

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
	curwin->w_میانگیر = curbuf;
#اگر_تعریف_شده FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#پایان_اگر
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

    new_قاب(curwin);
    إذا (curwin->w_قاب == عدم)
	أرجع FAIL;
    topframe = curwin->w_قاب;
    topframe->fr_عرض = COLUMNS_WITHOUT_TPL();
    topframe->fr_ارتفاع = ردیفها - p_ch;

    أرجع OK;
}

/*
 * Create a frame for window "wp".
 */
    ساكن فراغ
new_قاب(win_T *wp)
{
    قاب_T *frp = ALLOC_CLEAR_ONE(قاب_T);

    wp->w_قاب = frp;
    إذا (frp == عدم)
	أرجع;
    frp->fr_layout = FR_برگ;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    فراغ
win_init_اندازه(فراغ)
{
    firstwin->w_ارتفاع = ردیفها_AVAIL;
    firstwin->w_prev_ارتفاع = ردیفها_AVAIL;
    topframe->fr_ارتفاع = ردیفها_AVAIL;
    firstwin->w_عرض = topframe->fr_عرض;
}

/*
 * Allocate a new tabpage_T and init the values.
 * Returns NULL when out of memory.
 */
    ساكن tabpage_T *
alloc_tabpage(فراغ)
{
    tabpage_T	*tp;
#اگر_تعریف_شده FEAT_GUI
    صحيح		i;
#پایان_اگر


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    إذا (tp == عدم)
	أرجع عدم;

#اگر_تعریف_شده FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    إذا (tp->tp_vars == عدم)
    {
	vim_free(tp);
	أرجع عدم;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI
    لكل (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#پایان_اگر
    tp->tp_ch_استفاده_شده = p_ch;

    أرجع tp;
}

    فراغ
free_tabpage(tabpage_T *tp)
{
    صحيح idx;

#اگر_تعریف_شده FEAT_DIFF
    diff_clear(tp);
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
    طالما (tp->tp_اولی_popupwin != عدم)
	popup_close_tabpage(tp, tp->tp_اولی_popupwin->w_id, TRUE);
#پایان_اگر
    لكل (idx = 0; idx < SNAP_شمار; ++idx)
	clear_snapshot(tp, idx);
#اگر_تعریف_شده FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#پایان_اگر

    إذا (tp == lastused_tabpage)
	lastused_tabpage = عدم;
#اگر_تعریف_شده FEAT_TABPANEL
    tabpanel_forget_tabpage(tp);
#پایان_اگر

    vim_free(tp->tp_localdir);
    vim_free(tp->tp_prevdir);

#اگر_تعریف_شده FEAT_PYTHON
    python_tabpage_free(tp);
#پایان_اگر

#اگر_تعریف_شده FEAT_PYTHON3
    python3_tabpage_free(tp);
#پایان_اگر

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
#پایان_اگر

    إذا (cmdwin_type != 0)
    {
	emsg(_(e_invalid_درون_cmdline_پنجره));
	أرجع FAIL;
    }
    إذا (پنجره_layout_locked(CMD_tabnew))
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
	    newtp->tp_بعدی = اولی_tabpage;
	    اولی_tabpage = newtp;
	}
	وإلا
	{
	    إذا (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		لكل (tp = اولی_tabpage; tp->tp_بعدی != عدم
					       && n < after; tp = tp->tp_بعدی)
		    ++n;
	    }
	    newtp->tp_بعدی = tp->tp_بعدی;
	    tp->tp_بعدی = newtp;
	}
	newtp->tp_firstwin = newtp->tp_lastwin = newtp->tp_curwin = curwin;

	win_init_اندازه();
	firstwin->w_winrow = tabline_ارتفاع();
	firstwin->w_prev_winrow = firstwin->w_winrow;
	win_comp_scroll(curwin);

	newtp->tp_topframe = topframe;
	آخرین_وضعیت(FALSE);

	lastused_tabpage = prev_tp;

#إذا defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#پایان_اگر
#اگر_تعریف_شده FEAT_کار_CHANNEL
	entering_پنجره(curwin);
#پایان_اگر
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
		    قاب_new_عرض(tp2->tp_topframe, w, FALSE, TRUE);
		    إذا (!قاب_check_عرض(tp2->tp_topframe, w))
			قاب_new_عرض(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#پایان_اگر
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
				       ? postponed_تقسیم_tab : cmdmod.cmod_tab;

    إذا (n == 0)
	أرجع FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_تقسیم_tab = 0;
    أرجع win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    صحيح
make_tabpages(صحيح maxcount)
{
    صحيح		شمار = maxcount;
    صحيح		todo;

    // Limit to 'tabpagemax' tabs.
    إذا (شمار > p_tpm)
	شمار = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    بلوک_autocmds();

    لكل (todo = شمار - 1; todo > 0; --todo)
	إذا (win_new_tabpage(0) == FAIL)
	    اكسر;

    unblock_autocmds();

    // return actual number of tab pages
    أرجع (شمار - todo);
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
	    FOR_ALL_WINDOWS_درون_TAB(tp, wp)
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

    إذا (tab == اولی_tabpage)
    {
	اولی_tabpage = tab->tp_بعدی;
	ptp = اولی_tabpage;
    }
    وإلا
    {
	لكل (ptp = اولی_tabpage; ptp != عدم && ptp->tp_بعدی != tab;
							    ptp = ptp->tp_بعدی)
	    ;
	أكد(ptp != عدم);
	ptp->tp_بعدی = tab->tp_بعدی;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
پیدا_کن_tabpage(صحيح n)
{
    tabpage_T	*tp;
    صحيح		i = 1;

    إذا (n == 0)
	أرجع curtab;

    لكل (tp = اولی_tabpage; tp != عدم && i != n; tp = tp->tp_بعدی)
	++i;
    أرجع tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    صحيح
tabpage_شاخص(tabpage_T *ftp)
{
    صحيح		i = 1;
    tabpage_T	*tp;

    لكل (tp = اولی_tabpage; tp != عدم && tp != ftp; tp = tp->tp_بعدی)
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

#اگر_تعریف_شده FEAT_کار_CHANNEL
    leaving_پنجره(curwin);
#پایان_اگر
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
    إذا (gui.درون_use)
	gui_remove_scrollbars();
#پایان_اگر
#اگر_تعریف_شده FEAT_IMAGE
    // Clear all image placements, they will be redrawn again if needed.
    clear_all_image_placements();
#پایان_اگر
    tp->tp_curwin = curwin;
    tp->tp_prevwin = prevwin;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_old_ردیفها = ردیفها_AVAIL;
    إذا (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_عرض;
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
    win_T	*بعدی_prevwin = tp->tp_prevwin;
    tabpage_T	*آخرین_tab = curtab;

    use_tabpage(tp);

    إذا (p_ch != curtab->tp_ch_استفاده_شده)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	صحيح new_ch = curtab->tp_ch_استفاده_شده;
	curtab->tp_ch_استفاده_شده = p_ch;
	command_قاب_ارتفاع = FALSE;
	مجموعه_option_مقدار((char_u *)"cmdheight", new_ch, عدم, 0);
	command_قاب_ارتفاع = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (فراغ)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
		  | (trigger_enter_autocmds ? WEE_TRIGGER_ENTER_AUTOCMDS : 0)
		  | (trigger_leave_autocmds ? WEE_TRIGGER_LEAVE_AUTOCMDS : 0));
    prevwin = بعدی_prevwin;

    آخرین_وضعیت(FALSE);		// status line may appear or disappear
    win_comp_pos();		// recompute w_winrow for all windows
#اگر_تعریف_شده FEAT_DIFF
    diff_need_scrollbind = TRUE;
#پایان_اگر

    // If there was a click in a window, it won't be usable for a following
    // drag.
    reset_dragwin();

    // The tabpage line may have appeared or disappeared, may need to resize
    // the frames for that.  When the Vim window was resized need to update
    // frame sizes too.
    إذا (curtab->tp_old_ردیفها != ردیفها_AVAIL || (old_off != firstwin->w_winrow
#اگر_تعریف_شده FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#پایان_اگر
		))
	shell_new_ردیفها();
    إذا (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	إذا (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_عرض;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	وإلا
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = آخرین_tab;

#إذا defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#پایان_اگر

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
	متن_locked_پیام();
	أرجع;
    }

    // If there is only one it can't work.
    إذا (اولی_tabpage->tp_بعدی == عدم)
    {
	إذا (n > 1)
	    beep_flush();
	أرجع;
    }

    إذا (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	إذا (curtab->tp_بعدی == عدم)
	    tp = اولی_tabpage;
	وإلا
	    tp = curtab->tp_بعدی;
    }
    وإلا إذا (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	لكل (i = n; i < 0; ++i)
	{
	    لكل (tp = اولی_tabpage; tp->tp_بعدی != ttp && tp->tp_بعدی != عدم;
		    tp = tp->tp_بعدی)
		;
	    ttp = tp;
	}
    }
    وإلا إذا (n == 9999)
    {
	// Go to last tab page.
	لكل (tp = اولی_tabpage; tp->tp_بعدی != عدم; tp = tp->tp_بعدی)
	    ;
    }
    وإلا
    {
	// Go to tab page "n".
	tp = پیدا_کن_tabpage(n);
	إذا (tp == عدم)
	{
	    beep_flush();
	    أرجع;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#اگر_تعریف_شده FEAT_GUI_TABLINE
    إذا (gui_use_tabline())
	gui_mch_مجموعه_curtab(tabpage_شاخص(curtab));
#پایان_اگر
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
    مجموعه_keep_پیام(عدم, 0);

    skip_win_fix_scroll = TRUE;
    إذا (tp != curtab && leave_tabpage(tp->tp_curwin->w_میانگیر,
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
#اگر_تعریف_شده FEAT_GUI_TABLINE
	إذا (gui_use_tabline())
	    gui_mch_مجموعه_curtab(tabpage_شاخص(curtab));
#پایان_اگر
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

    إذا (اولی_tabpage->tp_بعدی == عدم)
	أرجع;

    إذا (tabpage_move_disallowed)
	أرجع;

    لكل (tp = اولی_tabpage; tp->tp_بعدی != عدم && n < nr; tp = tp->tp_بعدی)
	++n;

    إذا (tp == curtab || (nr > 0 && tp->tp_بعدی != عدم
						    && tp->tp_بعدی == curtab))
	أرجع;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    إذا (curtab == اولی_tabpage)
	اولی_tabpage = curtab->tp_بعدی;
    وإلا
    {
	FOR_ALL_TABPAGES(tp)
	    إذا (tp->tp_بعدی == curtab)
		اكسر;
	إذا (tp == عدم)	// "cannot happen"
	    أرجع;
	tp->tp_بعدی = curtab->tp_بعدی;
    }

    // Re-insert it at the specified position.
    إذا (nr <= 0)
    {
	curtab->tp_بعدی = اولی_tabpage;
	اولی_tabpage = curtab;
    }
    وإلا
    {
	curtab->tp_بعدی = tp_dst->tp_بعدی;
	tp_dst->tp_بعدی = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#پایان_اگر
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
#اگر_تعریف_شده FEAT_CONCEAL
    win_T	*owp = curwin;
#پایان_اگر

#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (ERROR_IF_ANY_POPUP_پنجره)
	أرجع;
    إذا (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_مقصد_enter_popup_پنجره));
	أرجع;
    }
#پایان_اگر
    إذا (متن_or_buf_locked())
    {
	beep_flush();
	أرجع;
    }

    إذا (wp->w_میانگیر != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    وإلا إذا (VIsual_فعال)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    إذا (!win_valid(wp))
	أرجع;

#اگر_تعریف_شده FEAT_GUI
    need_mouse_correct = TRUE;
#پایان_اگر
    win_enter(wp, TRUE);

#اگر_تعریف_شده FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    إذا (win_valid(owp) && owp->w_p_cole > 0 && !پیام_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    إذا (curwin->w_p_cole > 0 && !پیام_scrolled)
	need_cursor_سطر_redraw = TRUE;
#پایان_اگر
}

#إذا defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_پیدا_کن_nr(صحيح winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (--winnr == 0)
	    اكسر;
    أرجع wp;
}
#پایان_اگر

#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_پیدا_کن_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    إذا (wp == win)
		أرجع tp;
    أرجع عدم;
}
#پایان_اگر

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, صحيح به_بالا, دراز شمار)
{
    قاب_T	*fr;
    قاب_T	*nfr;
    قاب_T	*foundfr;

#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (popup_is_popup(wp))
	// popups don't have neighbors.
	أرجع عدم;
#پایان_اگر
    foundfr = wp->w_قاب;
    طالما (شمار--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	لكل (;;)
	{
	    إذا (fr == tp->tp_topframe)
		اذهب انتها;
	    إذا (به_بالا)
		nfr = fr->fr_prev;
	    وإلا
		nfr = fr->fr_بعدی;
	    إذا (fr->fr_والد->fr_layout == FR_ستون && nfr != عدم)
		اكسر;
	    fr = fr->fr_والد;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	لكل (;;)
	{
	    إذا (nfr->fr_layout == FR_برگ)
	    {
		foundfr = nfr;
		اكسر;
	    }
	    fr = nfr->fr_فرزند;
	    إذا (nfr->fr_layout == FR_ردیف)
	    {
		// Find the frame at the cursor row.
		طالما (fr->fr_بعدی != عدم
			&& frame2win(fr)->w_wincol + fr->fr_عرض
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_بعدی;
	    }
	    إذا (nfr->fr_layout == FR_ستون && به_بالا)
		طالما (fr->fr_بعدی != عدم)
		    fr = fr->fr_بعدی;
	    nfr = fr;
	}
    }
انتها:
    أرجع foundfr != عدم ? foundfr->fr_win : عدم;
}

/*
 * Move to window above or below "count" times.
 */
    ساكن فراغ
win_goto_ver(
    صحيح		به_بالا,		// TRUE to go to win above
    دراز	شمار)
{
    win_T	*win;

#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (ERROR_IF_جمله_POPUP_پنجره)
	أرجع;
#پایان_اگر
    win = win_vert_neighbor(curtab, curwin, به_بالا, شمار);
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
win_horz_neighbor(tabpage_T *tp, win_T *wp, صحيح چپ_2, دراز شمار)
{
    قاب_T	*fr;
    قاب_T	*nfr;
    قاب_T	*foundfr;

#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (popup_is_popup(wp))
	// popups don't have neighbors.
	أرجع عدم;
#پایان_اگر
    foundfr = wp->w_قاب;
    طالما (شمار--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	لكل (;;)
	{
	    إذا (fr == tp->tp_topframe)
		اذهب انتها;
	    إذا (چپ_2)
		nfr = fr->fr_prev;
	    وإلا
		nfr = fr->fr_بعدی;
	    إذا (fr->fr_والد->fr_layout == FR_ردیف && nfr != عدم)
		اكسر;
	    fr = fr->fr_والد;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	لكل (;;)
	{
	    إذا (nfr->fr_layout == FR_برگ)
	    {
		foundfr = nfr;
		اكسر;
	    }
	    fr = nfr->fr_فرزند;
	    إذا (nfr->fr_layout == FR_ستون)
	    {
		// Find the frame at the cursor row.
		طالما (fr->fr_بعدی != عدم
			&& frame2win(fr)->w_winrow + fr->fr_ارتفاع
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_بعدی;
	    }
	    إذا (nfr->fr_layout == FR_ردیف && چپ_2)
		طالما (fr->fr_بعدی != عدم)
		    fr = fr->fr_بعدی;
	    nfr = fr;
	}
    }
انتها:
    أرجع foundfr != عدم ? foundfr->fr_win : عدم;
}

/*
 * Move to left or right window.
 */
    ساكن فراغ
win_goto_hor(
    صحيح		چپ_2,		// TRUE to go to left win
    دراز	شمار)
{
    win_T	*win;

#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (ERROR_IF_جمله_POPUP_پنجره)
	أرجع;
#پایان_اگر
    win = win_horz_neighbor(curtab, curwin, چپ_2, شمار);
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
	    آخرین_chdir_reason = عدم;
	    shorten_fnames(TRUE);
	}
    }
    وإلا إذا (globaldir != عدم)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((حرف *)globaldir);
	VIM_CLEAR(globaldir);
	آخرین_chdir_reason = عدم;
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
    صحيح		other_میانگیر = FALSE;
    صحيح		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    صحيح		did_decrement = FALSE;

    إذا (wp == curwin && curwin_invalid == 0)	// nothing to do
	أرجع FALSE;

#اگر_تعریف_شده FEAT_کار_CHANNEL
    إذا (curwin_invalid == 0)
	leaving_پنجره(curwin);
#پایان_اگر

    إذا (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	إذا (wp->w_میانگیر != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, عدم, عدم, FALSE, curbuf);
	    other_میانگیر = TRUE;
	    إذا (!win_valid(wp))
		أرجع FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, عدم, عدم, FALSE, curbuf);
	إذا (!win_valid(wp))
	    أرجع FALSE;
#اگر_تعریف_شده FEAT_EVAL
	// autocmds may abort script processing
	إذا (aborting())
	    أرجع FALSE;
#پایان_اگر
    }

    // sync undo before leaving the current buffer
    إذا ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_میانگیر)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    إذا (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    إذا (wp->w_میانگیر != curbuf)
	buf_copy_options(wp->w_میانگیر, BCO_ENTER | BCO_NOHELP);
    إذا (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_وضعیت = true;
    }
    curwin = wp;
    curbuf = wp->w_میانگیر;
    check_cursor();
    إذا (!virtual_فعال())
	curwin->w_cursor.coladd = 0;
    إذا (*p_spk == 'c')		// assume cursor position needs updating
	تغییریافته_سطر_abv_curs();
    وإلا
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (نما_نرمال|نما_CMDLINE|نما_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#اگر_تعریف_شده پیام_صف
    إذا (flags & WEE_ALLOW_PARSE_MESSAGES)
    {
	--dont_parse_messages;
	did_decrement = TRUE;
    }
#پایان_اگر

    win_fix_current_dir();

#اگر_تعریف_شده FEAT_کار_CHANNEL
    entering_پنجره(curwin);
#پایان_اگر
    // Careful: autocommands may close the window and make "wp" invalid
    إذا (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, عدم, عدم, FALSE, curbuf);
    إذا (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, عدم, عدم, FALSE, curbuf);
	إذا (other_میانگیر)
	    apply_autocmds(EVENT_BUFENTER, عدم, عدم, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_وضعیت = true;
#اگر_تعریف_شده FEAT_TERMINAL
    إذا (bt_terminal(curwin->w_میانگیر))
	// terminal is likely in another mode
	redraw_نما = TRUE;
#پایان_اگر
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#پایان_اگر
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    إذا (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    إذا (curwin->w_ارتفاع < p_wh && !curwin->w_p_wfh
#اگر_تعریف_شده FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#پایان_اگر
	    )
	win_setheight((صحيح)p_wh);
    وإلا إذا (curwin->w_ارتفاع == 0)
	win_setheight(1);

    // set window width to desired minimal value
    إذا (curwin->w_عرض < p_wiw && !curwin->w_p_wfw)
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

    إذا (curwin->w_میانگیر == buf)
	wp = curwin;
    وإلا
	FOR_ALL_WINDOWS(wp)
	    إذا (wp->w_میانگیر == buf)
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
	    FOR_ALL_WINDOWS_درون_TAB(tp, wp)
		إذا (wp->w_میانگیر == buf)
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

ساكن صحيح آخرین_win_id = پایینترین_WIN_ID - 1;

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

    إذا (win_alloc_سطرها(new_wp) == FAIL)
    {
	vim_free(new_wp);
	أرجع عدم;
    }

    new_wp->w_id = ++آخرین_win_id;

#اگر_تعریف_شده FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    إذا (new_wp->w_vars == عدم)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	أرجع عدم;
    }
    init_var_dict(new_wp->w_vars, &new_wp->w_winvar, VAR_SCOPE);
#پایان_اگر

    // Don't execute autocommands while the window is not properly
    // initialized yet.  gui_create_scrollbar() may trigger a FocusGained
    // event.
    بلوک_autocmds();

    /*
     * link the window in the window list
     */
    إذا (!hidden)
	win_append(after, new_wp);
    new_wp->w_wincol = TPL_LCOL();
    new_wp->w_عرض = COLUMNS_WITHOUT_TPL();

    // position the display and the cursor at the top of the file.
    new_wp->w_topline = 1;
#اگر_تعریف_شده FEAT_DIFF
    new_wp->w_topfill = 0;
#پایان_اگر
    new_wp->w_botline = 2;
    new_wp->w_cursor.lnum = 1;
    new_wp->w_scbind_pos = 1;

    // use global option value for global-local options
    new_wp->w_allbuf_opt.wo_so = new_wp->w_p_so = -1;
    new_wp->w_allbuf_opt.wo_sop = new_wp->w_p_sop = -1;
    new_wp->w_allbuf_opt.wo_siso = new_wp->w_p_siso = -1;

    // We won't calculate w_fraction until resizing the window
    new_wp->w_fraction = 0;
    new_wp->w_prev_fraction_ردیف = -1;

#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_چپ],
		SBAR_چپ, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_راست],
		SBAR_راست, new_wp);
    }
#پایان_اگر
#اگر_تعریف_شده FEAT_FOLDING
    foldInitWin(new_wp);
#پایان_اگر
    unblock_autocmds();
#اگر_تعریف_شده FEAT_جستجو_EXTRA
    new_wp->w_بعدی_تطابق_id = 1000;  // up to 1000 can be picked by the user
#پایان_اگر
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

#اگر_تعریف_شده FEAT_FOLDING
    clearFolding(wp);
#پایان_اگر

    // reduce the reference count to the argument list.
    alist_unlink(wp->w_alist);

    // Don't execute autocommands while the window is halfway being deleted.
    // gui_mch_destroy_scrollbar() may trigger a FocusGained event.
    بلوک_autocmds();

#اگر_تعریف_شده FEAT_LUA
    lua_پنجره_free(wp);
#پایان_اگر

#اگر_تعریف_شده FEAT_MZSCHEME
    mzscheme_پنجره_free(wp);
#پایان_اگر

#اگر_تعریف_شده FEAT_PERL
    perl_win_free(wp);
#پایان_اگر

#اگر_تعریف_شده FEAT_PYTHON
    python_پنجره_free(wp);
#پایان_اگر

#اگر_تعریف_شده FEAT_PYTHON3
    python3_پنجره_free(wp);
#پایان_اگر

#اگر_تعریف_شده FEAT_TCL
    tcl_پنجره_free(wp);
#پایان_اگر

#اگر_تعریف_شده FEAT_RUBY
    ruby_پنجره_free(wp);
#پایان_اگر

    remove_highlight_overrides(wp->w_hl);
    vim_free(wp->w_hl);

    // Free statusline click regions.
    إذا (wp->w_stl_click != عدم)
    {
	لكل (i = 0; i < wp->w_stl_click_شمار; i++)
	    vim_free(wp->w_stl_click[i].funcname);
	vim_free(wp->w_stl_click);
    }

    clear_winopt(&wp->w_onebuf_opt);
    clear_winopt(&wp->w_allbuf_opt);

    vim_free(wp->w_lcs_chars.multispace);
    vim_free(wp->w_lcs_chars.leadmultispace);

#اگر_تعریف_شده FEAT_EVAL
    vars_clear(&wp->w_vars->dv_hashtab);	// free all w: variables
    hash_init(&wp->w_vars->dv_hashtab);
    unref_var_dict(wp->w_vars);
#پایان_اگر

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
	tagstack_clear_مدخل(&wp->w_tagstack[i]);
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
			إذا (wip2->wi_بعدی != عدم)
			    wip2->wi_بعدی->wi_prev = wip2->wi_prev;
			إذا (wip2->wi_prev == عدم)
			    buf->b_wininfo = wip2->wi_بعدی;
			وإلا
			    wip2->wi_prev->wi_بعدی = wip2->wi_بعدی;
			free_wininfo(wip2);
			اكسر;
		    }

		wip->wi_win = عدم;
	    }

#اگر_تعریف_شده FEAT_جستجو_EXTRA
    clear_matches(wp);
#پایان_اگر

    free_jumplist(wp);

#اگر_تعریف_شده FEAT_QUICKFIX
    qf_free_all(wp);
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_چپ]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_راست]);
    }
#پایان_اگر // FEAT_GUI

#اگر_تعریف_شده FEAT_MENU
    remove_winbar(wp);
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    لكل (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    vim_free(wp->w_scrollbar_highlight);
    vim_free(wp->w_thumb_highlight);
    vim_free(wp->w_popup_title);
    list_unref(wp->w_popup_ماسک);
    vim_free(wp->w_popup_ماسک_cells);
#پایان_اگر

#اگر_تعریف_شده FEAT_SYN_HL
    vim_free(wp->w_p_cc_تعداد_ستون);
#پایان_اگر

    إذا (win_valid_any_tab(wp))
	win_remove(wp, tp);
    إذا (autocmd_busy)
    {
	wp->w_بعدی = au_pending_free_win;
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
    إذا (win->w_میانگیر != عدم)
    {
	إذا (bt_popup(win->w_میانگیر))
	    win_close_میانگیر(win, DOBUF_WIPE_REUSE, FALSE);
	وإلا
	    close_میانگیر(win, win->w_میانگیر, 0, FALSE, FALSE, TRUE);

	إذا (win->w_میانگیر != عدم)
	    --win->w_میانگیر->b_nwindows;
    }
# إذا defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    إذا (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_قاب);
    win_free(win, عدم);
}
#پایان_اگر

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
	before = after->w_بعدی;

    wp->w_بعدی = before;
    wp->w_prev = after;
    إذا (after == عدم)
	firstwin = wp;
    وإلا
	after->w_بعدی = wp;
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
	wp->w_prev->w_بعدی = wp->w_بعدی;
    وإلا إذا (tp == عدم)
	firstwin = curtab->tp_firstwin = wp->w_بعدی;
    وإلا
	tp->tp_firstwin = wp->w_بعدی;

    إذا (wp->w_بعدی != عدم)
	wp->w_بعدی->w_prev = wp->w_prev;
    وإلا إذا (tp == عدم)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    وإلا
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    ساكن فراغ
قاب_append(قاب_T *after, قاب_T *frp)
{
    frp->fr_بعدی = after->fr_بعدی;
    after->fr_بعدی = frp;
    إذا (frp->fr_بعدی != عدم)
	frp->fr_بعدی->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    ساكن فراغ
قاب_درج(قاب_T *before, قاب_T *frp)
{
    frp->fr_بعدی = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    إذا (frp->fr_prev != عدم)
	frp->fr_prev->fr_بعدی = frp;
    وإلا
	frp->fr_والد->fr_فرزند = frp;
}

/*
 * Remove a frame from a frame list.
 */
    ساكن فراغ
قاب_remove(قاب_T *frp)
{
    إذا (frp->fr_prev != عدم)
	frp->fr_prev->fr_بعدی = frp->fr_بعدی;
    وإلا
	frp->fr_والد->fr_فرزند = frp->fr_بعدی;
    إذا (frp->fr_بعدی != عدم)
	frp->fr_بعدی->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    صحيح
win_alloc_سطرها(win_T *wp)
{
    wp->w_سطرها_valid = 0;
    wp->w_سطرها = ALLOC_CLEAR_MULT(wline_T, ردیفها);
    إذا (wp->w_سطرها == عدم)
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
	VIM_CLEAR(wp->w_سطرها);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    فراغ
shell_new_ردیفها(فراغ)
{
    صحيح		h = (صحيح)ردیفها_AVAIL;

    إذا (firstwin == عدم)	// not initialized yet
	أرجع;
    إذا (h < قاب_minheight(topframe, عدم))
	h = قاب_minheight(topframe, عدم);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    قاب_new_ارتفاع(topframe, h, FALSE, TRUE, FALSE);
    إذا (!قاب_check_ارتفاع(topframe, h))
	قاب_new_ارتفاع(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_استفاده_شده = p_ch;

    إذا (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#پایان_اگر
#إذا 0
    // Disabled: don't want making the screen smaller make a window larger.
    إذا (p_ea)
	win_equal(curwin, FALSE, 'v');
#پایان_اگر
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
    ساكن صحيح prev_tp_عرض;
    ساكن صحيح prev_wincol;
    ساكن صحيح prev_fr_عرض;
#پایان_اگر
    صحيح w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    قاب_new_عرض(topframe, w, FALSE, TRUE);
    إذا (!قاب_check_عرض(topframe, w))
	قاب_new_عرض(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#إذا defined(FEAT_TABPANEL)
    صحيح tp_عرض = tabpanel_عرض();
    صحيح tp_عرض_تغییریافته = tp_عرض != prev_tp_عرض;
    // tabpanel width changed
    إذا (tp_عرض_تغییریافته && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    إذا (tp_عرض_تغییریافته
	    || (tp_عرض > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_عرض != prev_fr_عرض)))
    {
	screen_fill(cmdline_ردیف, (صحيح)ردیفها, 0, (صحيح)Columns, ' ', ' ', 0);
    }
    prev_tp_عرض = tabpanel_عرض();
    prev_wincol = firstwin->w_wincol;
    prev_fr_عرض = topframe->fr_عرض;
#پایان_اگر
    // Adjust offset for command line start column
    cmdline_ستون_off = firstwin->w_wincol;
    cmdline_عرض = topframe->fr_عرض;
    comp_ستون();
    إذا (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#پایان_اگر
#إذا 0
    // Disabled: don't want making the screen smaller make a window larger.
    إذا (p_ea)
	win_equal(curwin, FALSE, 'h');
#پایان_اگر
}

/*
 * Save the size of all windows in "gap".
 */
    فراغ
win_اندازه_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, حجم(صحيح), 1);
    إذا (ga_grow(gap, win_شمار() * 2 + 1) == FAIL)
	أرجع;

    // first entry is the total lines available for windows
    ((صحيح *)gap->ga_داده)[gap->ga_len++] = ردیفها_AVAIL - آخرین_stl_ارتفاع(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((صحيح *)gap->ga_داده)[gap->ga_len++] =
	    wp->w_عرض + wp->w_vsep_عرض;
	((صحيح *)gap->ga_داده)[gap->ga_len++] = wp->w_ارتفاع;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    فراغ
win_اندازه_restore(garray_T *gap)
{
    win_T	*wp;
    صحيح		i, j;

    إذا (win_شمار() * 2 + 1 == gap->ga_len
	    && ((صحيح *)gap->ga_داده)[0] == ردیفها_AVAIL - آخرین_stl_ارتفاع(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	لكل (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		قاب_setwidth(wp->w_قاب, ((صحيح *)gap->ga_داده)[i++]);
		win_setheight_win(((صحيح *)gap->ga_داده)[i++], wp);
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
    صحيح		ردیف = tabline_ارتفاع();
    صحيح		ستون = TPL_LCOL();

    قاب_comp_pos(topframe, &ردیف, &ستون);
#إذا defined(FEAT_STL_OPT)
    قاب_change_statusline_ارتفاع();
#پایان_اگر
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    ساكن فراغ
قاب_comp_pos(قاب_T *topfrp, صحيح *ردیف, صحيح *ستون)
{
    win_T	*wp;
    قاب_T	*frp;
    صحيح		startcol;
    صحيح		startrow;
    صحيح		h;

    wp = topfrp->fr_win;
    إذا (wp != عدم)
    {
	إذا (wp->w_winrow != *ردیف || wp->w_wincol != *ستون)
	{
	    // position changed, redraw
	    wp->w_winrow = *ردیف;
	    wp->w_wincol = *ستون;
	    redraw_win_later(wp, UPD_NOT_VALID);
	    wp->w_redr_وضعیت = true;
	}
	// WinBar will not show if the window height is zero
	h = VISIBLE_ارتفاع(wp) + wp->w_وضعیت_ارتفاع;
	*ردیف += h > topfrp->fr_ارتفاع ? topfrp->fr_ارتفاع : h;
	*ستون += wp->w_عرض + wp->w_vsep_عرض;
    }
    وإلا
    {
	startrow = *ردیف;
	startcol = *ستون;
	FOR_ALL_قابها(frp, topfrp->fr_فرزند)
	{
	    إذا (topfrp->fr_layout == FR_ردیف)
		*ردیف = startrow;	// all frames are at the same row
	    وإلا
		*ستون = startcol;	// all frames are at the same col
	    قاب_comp_pos(frp, ردیف, ستون);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    فراغ
win_ensure_اندازه(فراغ)
{
    إذا (curwin->w_ارتفاع == 0)
	win_setheight(1);
    إذا (curwin->w_عرض == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    فراغ
win_setheight(صحيح ارتفاع)
{
    win_setheight_win(ارتفاع, curwin);
#إذا defined(FEAT_STL_OPT)
    قاب_change_statusline_ارتفاع();
#پایان_اگر
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    فراغ
win_setheight_win(صحيح ارتفاع, win_T *win)
{
    إذا (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	إذا (ارتفاع < p_wmh)
	    ارتفاع = p_wmh;
	إذا (ارتفاع == 0)
	    ارتفاع = 1;
	ارتفاع += WINBAR_ارتفاع(curwin);
    }

    قاب_setheight(win->w_قاب, ارتفاع + win->w_وضعیت_ارتفاع);

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
قاب_setheight(قاب_T *curfrp, صحيح ارتفاع)
{
    صحيح		جا;		// total number of lines available
    صحيح		take;		// number of lines taken from other windows
    صحيح		جا_cmdline;	// lines available from cmdline
    صحيح		پیاپی;
    قاب_T	*frp;
    صحيح		h;
    صحيح		جا_reserved;

    // If the height already is the desired value, nothing to do.
    إذا (curfrp->fr_ارتفاع == ارتفاع)
	أرجع;

    إذا (curfrp->fr_والد == عدم)
    {
	// topframe: can only change the command line height
	إذا (ارتفاع > 0)
	    قاب_new_ارتفاع(curfrp, ارتفاع, FALSE, FALSE, TRUE);
    }
    وإلا إذا (curfrp->fr_والد->fr_layout == FR_ردیف)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = قاب_minheight(curfrp->fr_والد, عدم);
	إذا (ارتفاع < h)
	    ارتفاع = h;
	قاب_setheight(curfrp->fr_والد, ارتفاع);
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
	لكل (پیاپی = 1; پیاپی <= 2; ++پیاپی)
	{
	    جا = 0;
	    جا_reserved = 0;
	    FOR_ALL_قابها(frp, curfrp->fr_والد->fr_فرزند)
	    {
		إذا (frp != curfrp
			&& frp->fr_win != عدم
			&& frp->fr_win->w_p_wfh)
		    جا_reserved += frp->fr_ارتفاع;
		جا += frp->fr_ارتفاع;
		إذا (frp != curfrp)
		    جا -= قاب_minheight(frp, عدم);
	    }
	    إذا (curfrp->fr_عرض != topframe->fr_عرض)
		جا_cmdline = 0;
	    وإلا
	    {
		جا_cmdline = ردیفها - p_ch - (lastwin->w_winrow
						+ VISIBLE_ارتفاع(lastwin)
						+ lastwin->w_وضعیت_ارتفاع);
		إذا (جا_cmdline < 0)
		    جا_cmdline = 0;
	    }

	    إذا (ارتفاع <= جا + جا_cmdline)
		اكسر;
	    إذا (پیاپی == 2 || curfrp->fr_عرض == topframe->fr_عرض)
	    {
		ارتفاع = جا + جا_cmdline;
		اكسر;
	    }
	    قاب_setheight(curfrp->fr_والد, ارتفاع
		+ قاب_minheight(curfrp->fr_والد, NOWIN) - (صحيح)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = ارتفاع - curfrp->fr_ارتفاع;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	إذا (ارتفاع > جا + جا_cmdline - جا_reserved)
	    جا_reserved = جا + جا_cmdline - ارتفاع;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	إذا (take < 0 && جا - curfrp->fr_ارتفاع <= جا_reserved)
	    جا_reserved = 0;

	إذا (take > 0 && جا_cmdline > 0)
	{
	    // use lines from cmdline first
	    إذا (take < جا_cmdline)
		جا_cmdline = take;
	    take -= جا_cmdline;
	    topframe->fr_ارتفاع += جا_cmdline;
	}

	/*
	 * set the current frame to the new height
	 */
	قاب_new_ارتفاع(curfrp, ارتفاع, FALSE, FALSE, TRUE);

	/*
	 * First take lines from the frames after the current frame.  If
	 * that is not enough, takes lines from frames above the current
	 * frame.
	 */
	لكل (پیاپی = 0; پیاپی < 2; ++پیاپی)
	{
	    إذا (پیاپی == 0)
		frp = curfrp->fr_بعدی;	// 1st run: start with next window
	    وإلا
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    طالما (frp != عدم && take != 0)
	    {
		h = قاب_minheight(frp, عدم);
		إذا (جا_reserved > 0
			&& frp->fr_win != عدم
			&& frp->fr_win->w_p_wfh)
		{
		    إذا (جا_reserved >= frp->fr_ارتفاع)
			جا_reserved -= frp->fr_ارتفاع;
		    وإلا
		    {
			إذا (frp->fr_ارتفاع - جا_reserved > take)
			    جا_reserved = frp->fr_ارتفاع - take;
			take -= frp->fr_ارتفاع - جا_reserved;
			قاب_new_ارتفاع(frp, جا_reserved, FALSE, FALSE,
									TRUE);
			جا_reserved = 0;
		    }
		}
		وإلا
		{
		    إذا (frp->fr_ارتفاع - take < h)
		    {
			take -= frp->fr_ارتفاع - h;
			قاب_new_ارتفاع(frp, h, FALSE, FALSE, TRUE);
		    }
		    وإلا
		    {
			قاب_new_ارتفاع(frp, frp->fr_ارتفاع - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		إذا (پیاپی == 0)
		    frp = frp->fr_بعدی;
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
win_setwidth(صحيح عرض)
{
    win_setwidth_win(عرض, curwin);
}

    فراغ
win_setwidth_win(صحيح عرض, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    إذا (wp == curwin)
    {
	إذا (عرض < p_wmw)
	    عرض = p_wmw;
	إذا (عرض == 0)
	    عرض = 1;
    }
    وإلا إذا (عرض < 0)
	عرض = 0;

    قاب_setwidth(wp->w_قاب, عرض + wp->w_vsep_عرض);

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
قاب_setwidth(قاب_T *curfrp, صحيح عرض)
{
    صحيح		جا;		// total number of lines available
    صحيح		take;		// number of lines taken from other windows
    صحيح		پیاپی;
    قاب_T	*frp;
    صحيح		w;
    صحيح		جا_reserved;

    // If the width already is the desired value, nothing to do.
    إذا (curfrp->fr_عرض == عرض)
	أرجع;

    إذا (curfrp->fr_والد == عدم)
	// topframe: can't change width
	أرجع;

    إذا (curfrp->fr_والد->fr_layout == FR_ستون)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = قاب_minwidth(curfrp->fr_والد, عدم);
	إذا (عرض < w)
	    عرض = w;
	قاب_setwidth(curfrp->fr_والد, عرض);
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
	لكل (پیاپی = 1; پیاپی <= 2; ++پیاپی)
	{
	    جا = 0;
	    جا_reserved = 0;
	    FOR_ALL_قابها(frp, curfrp->fr_والد->fr_فرزند)
	    {
		إذا (frp != curfrp
			&& frp->fr_win != عدم
			&& frp->fr_win->w_p_wfw)
		    جا_reserved += frp->fr_عرض;
		جا += frp->fr_عرض;
		إذا (frp != curfrp)
		    جا -= قاب_minwidth(frp, عدم);
	    }

	    إذا (عرض <= جا)
		اكسر;
	    إذا (پیاپی == 2 || curfrp->fr_ارتفاع >= ردیفها_AVAIL)
	    {
		عرض = جا;
		اكسر;
	    }
	    قاب_setwidth(curfrp->fr_والد, عرض
		 + قاب_minwidth(curfrp->fr_والد, NOWIN) - (صحيح)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = عرض - curfrp->fr_عرض;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	إذا (عرض > جا - جا_reserved)
	    جا_reserved = جا - عرض;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	إذا (take < 0 && جا - curfrp->fr_عرض < جا_reserved)
	    جا_reserved = 0;

	/*
	 * set the current frame to the new width
	 */
	قاب_new_عرض(curfrp, عرض, FALSE, FALSE);

	/*
	 * First take lines from the frames right of the current frame.  If
	 * that is not enough, takes lines from frames left of the current
	 * frame.
	 */
	لكل (پیاپی = 0; پیاپی < 2; ++پیاپی)
	{
	    إذا (پیاپی == 0)
		frp = curfrp->fr_بعدی;	// 1st run: start with next window
	    وإلا
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    طالما (frp != عدم && take != 0)
	    {
		w = قاب_minwidth(frp, عدم);
		إذا (جا_reserved > 0
			&& frp->fr_win != عدم
			&& frp->fr_win->w_p_wfw)
		{
		    إذا (جا_reserved >= frp->fr_عرض)
			جا_reserved -= frp->fr_عرض;
		    وإلا
		    {
			إذا (frp->fr_عرض - جا_reserved > take)
			    جا_reserved = frp->fr_عرض - take;
			take -= frp->fr_عرض - جا_reserved;
			قاب_new_عرض(frp, جا_reserved, FALSE, FALSE);
			جا_reserved = 0;
		    }
		}
		وإلا
		{
		    إذا (frp->fr_عرض - take < w)
		    {
			take -= frp->fr_عرض - w;
			قاب_new_عرض(frp, w, FALSE, FALSE);
		    }
		    وإلا
		    {
			قاب_new_عرض(frp, frp->fr_عرض - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		إذا (پیاپی == 0)
		    frp = frp->fr_بعدی;
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
    صحيح		جا;
    صحيح		needed;
    صحيح		اولی = TRUE;

    // loop until there is a 'winminheight' that is possible
    طالما (p_wmh > 0)
    {
	جا = ردیفها - p_ch;
	needed = min_ردیفها_for_all_tabpages() - 1;  // 1 was added for the cmdline
	إذا (جا >= needed)
	    اكسر;
	--p_wmh;
	إذا (اولی)
	{
	    emsg(_(e_not_enough_جا));
	    اولی = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    فراغ
win_setminwidth(فراغ)
{
    صحيح		جا;
    صحيح		needed;
    صحيح		اولی = TRUE;

    // loop until there is a 'winminheight' that is possible
    طالما (p_wmw > 0)
    {
	جا = topframe->fr_عرض;
	needed = قاب_minwidth(topframe, عدم);
	إذا (جا >= needed)
	    اكسر;
	--p_wmw;
	إذا (اولی)
	{
	    emsg(_(e_not_enough_جا));
	    اولی = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    فراغ
win_drag_وضعیت_سطر(win_T *dragwin, صحيح جابه_جایی)
{
    قاب_T	*curfr;
    قاب_T	*fr;
    صحيح		جا;
    صحيح		به_بالا;	// if TRUE, drag status line up, otherwise down
    صحيح		n;

    fr = dragwin->w_قاب;
    curfr = fr;
    إذا (fr != topframe)		// more than one window
    {
	fr = fr->fr_والد;
	// When the parent frame is not a column of frames, its parent should
	// be.
	إذا (fr->fr_layout != FR_ستون)
	{
	    curfr = fr;
	    إذا (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_والد;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    طالما (curfr != topframe && curfr->fr_بعدی == عدم)
    {
	إذا (fr != topframe)
	    fr = fr->fr_والد;
	curfr = fr;
	إذا (fr != topframe)
	    fr = fr->fr_والد;
    }

    إذا (جابه_جایی < 0) // drag up
    {
	به_بالا = TRUE;
	جابه_جایی = -جابه_جایی;
	// sum up the room of the current frame and above it
	إذا (fr == curfr)
	{
	    // only one window
	    جا = fr->fr_ارتفاع - قاب_minheight(fr, عدم);
	}
	وإلا
	{
	    جا = 0;
	    لكل (fr = fr->fr_فرزند; ; fr = fr->fr_بعدی)
	    {
		جا += fr->fr_ارتفاع - قاب_minheight(fr, عدم);
		إذا (fr == curfr)
		    اكسر;
	    }
	}
	fr = curfr->fr_بعدی;		// put fr at frame that grows
    }
    وإلا    // drag down
    {
	به_بالا = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	جا = ردیفها - cmdline_ردیف;
	إذا (curfr->fr_بعدی == عدم)
	    --جا;
	وإلا
	    جا -= p_ch;
	إذا (جا < 0)
	    جا = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_قابها(fr, curfr->fr_بعدی)
	    جا += fr->fr_ارتفاع - قاب_minheight(fr, عدم);
	fr = curfr;			// put fr at window that grows
    }

    إذا (جا < جابه_جایی)		// Not enough room
	جابه_جایی = جا;		// Move as far as we can
    إذا (جابه_جایی <= 0)
	أرجع;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    إذا (fr != عدم)
	قاب_new_ارتفاع(fr, fr->fr_ارتفاع + جابه_جایی, به_بالا, FALSE, TRUE);

    إذا (به_بالا)
	fr = curfr;		// current frame gets smaller
    وإلا
	fr = curfr->fr_بعدی;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    طالما (fr != عدم && جابه_جایی > 0)
    {
	n = قاب_minheight(fr, عدم);
	إذا (fr->fr_ارتفاع - جابه_جایی <= n)
	{
	    جابه_جایی -= fr->fr_ارتفاع - n;
	    قاب_new_ارتفاع(fr, n, !به_بالا, FALSE, TRUE);
	}
	وإلا
	{
	    قاب_new_ارتفاع(fr, fr->fr_ارتفاع - جابه_جایی, !به_بالا, FALSE, TRUE);
	    اكسر;
	}
	إذا (به_بالا)
	    fr = fr->fr_prev;
	وإلا
	    fr = fr->fr_بعدی;
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
win_drag_vsep_سطر(win_T *dragwin, صحيح جابه_جایی)
{
    قاب_T	*curfr;
    قاب_T	*fr;
    صحيح		جا;
    صحيح		چپ_2;	// if TRUE, drag separator line left, otherwise right
    صحيح		n;

    fr = dragwin->w_قاب;
    إذا (fr == topframe)		// only one window (cannot happen?)
	أرجع;
    curfr = fr;
    fr = fr->fr_والد;
    // When the parent frame is not a row of frames, its parent should be.
    إذا (fr->fr_layout != FR_ردیف)
    {
	إذا (fr == topframe)	// only a column of windows (cannot happen?)
	    أرجع;
	curfr = fr;
	fr = fr->fr_والد;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    طالما (curfr->fr_بعدی == عدم)
    {
	إذا (fr == topframe)
	    اكسر;
	curfr = fr;
	fr = fr->fr_والد;
	إذا (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_والد;
	}
    }

    إذا (جابه_جایی < 0) // drag left
    {
	چپ_2 = TRUE;
	جابه_جایی = -جابه_جایی;
	// sum up the room of the current frame and left of it
	جا = 0;
	لكل (fr = fr->fr_فرزند; ; fr = fr->fr_بعدی)
	{
	    جا += fr->fr_عرض - قاب_minwidth(fr, عدم);
	    إذا (fr == curfr)
		اكسر;
	}
	fr = curfr->fr_بعدی;		// put fr at frame that grows
    }
    وإلا    // drag right
    {
	چپ_2 = FALSE;
	// sum up the room of frames right of the current one
	جا = 0;
	FOR_ALL_قابها(fr, curfr->fr_بعدی)
	    جا += fr->fr_عرض - قاب_minwidth(fr, عدم);
	fr = curfr;			// put fr at window that grows
    }

    إذا (جا < جابه_جایی)		// Not enough room
	جابه_جایی = جا;		// Move as far as we can
    إذا (جابه_جایی <= 0)		// No room at all, quit.
	أرجع;
    إذا (fr == عدم)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	أرجع;

    // grow frame fr by offset lines
    قاب_new_عرض(fr, fr->fr_عرض + جابه_جایی, چپ_2, FALSE);

    // shrink other frames: current and at the left or at the right
    إذا (چپ_2)
	fr = curfr;		// current frame gets smaller
    وإلا
	fr = curfr->fr_بعدی;	// next frame gets smaller

    طالما (fr != عدم && جابه_جایی > 0)
    {
	n = قاب_minwidth(fr, عدم);
	إذا (fr->fr_عرض - جابه_جایی <= n)
	{
	    جابه_جایی -= fr->fr_عرض - n;
	    قاب_new_عرض(fr, n, !چپ_2, FALSE);
	}
	وإلا
	{
	    قاب_new_عرض(fr, fr->fr_عرض - جابه_جایی, !چپ_2, FALSE);
	    اكسر;
	}
	إذا (چپ_2)
	    fr = fr->fr_prev;
	وإلا
	    fr = fr->fr_بعدی;
    }
    win_comp_pos();
    redraw_all_later(UPD_NOT_VALID);
}

#تعریف_کن FRACTION_MULT	16384L

/*
 * Set wp->w_fraction for the current w_wrow and w_height.
 * Has no effect when the window is less than two lines.
 */
    فراغ
مجموعه_fraction(win_T *wp)
{
    إذا (wp->w_ارتفاع > 1)
	// When cursor is in the first line the percentage is computed as if
	// it's halfway that line.  Thus with two lines it is 25%, with three
	// lines 17%, etc.  Similarly for the last line: 75%, 83%, etc.
	wp->w_fraction = ((دراز)wp->w_wrow * FRACTION_MULT
				     + FRACTION_MULT / 2) / (دراز)wp->w_ارتفاع;
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
	إذا (wp->w_ارتفاع != wp->w_prev_ارتفاع)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    إذا (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_میانگیر->b_ml.ml_سطر_شمار)
	    {
		صحيح diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_ارتفاع - wp->w_prev_ارتفاع);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		إذا (diff > 0)
		    cursor_پایین_inner(wp, diff);
		وإلا
		    cursor_به_بالا_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_مقصد_fraction(wp, wp->w_prev_ارتفاع);

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
	wp->w_prev_ارتفاع = wp->w_ارتفاع;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    إذا (!(get_real_state() & (نما_نرمال|نما_CMDLINE|نما_TERMINAL)))
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
win_fix_cursor(صحيح نرمال_4)
{
    win_T	*wp = curwin;

    إذا (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_میانگیر->b_ml.ml_سطر_شمار < wp->w_ارتفاع)
	أرجع;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    دراز so = MIN(wp->w_ارتفاع / 2, get_scrolloff_مقدار());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_پایین_inner(wp, so);
    linenr_T بالاترین = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_به_بالا_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    إذا (lnum > bot && (wp->w_botline - wp->w_میانگیر->b_ml.ml_سطر_شمار) != 1)
	nlnum = bot;
    وإلا إذا (lnum < بالاترین && wp->w_topline != 1)
	nlnum = (so == wp->w_ارتفاع / 2) ? bot : بالاترین;

    إذا (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	إذا (نرمال_4)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	وإلا  // Scroll instead when not in normal mode.
	{
	    wp->w_fraction = (nlnum == bot) ? FRACTION_MULT : 0;
	    scroll_مقصد_fraction(wp, wp->w_prev_ارتفاع);
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
win_new_ارتفاع(win_T *wp, صحيح ارتفاع)
{
    صحيح		prev_ارتفاع = wp->w_ارتفاع;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    إذا (ارتفاع < 0)
	ارتفاع = 0;
    إذا (wp->w_ارتفاع == ارتفاع)
	أرجع;	    // nothing to do

    إذا (wp->w_ارتفاع > 0)
    {
	إذا (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	إذا (wp->w_ارتفاع != prev_ارتفاع)
	    أرجع;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	إذا (wp->w_wrow != wp->w_prev_fraction_ردیف)
	    مجموعه_fraction(wp);
    }

    wp->w_ارتفاع = ارتفاع;
    wp->w_redr_وضعیت = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    إذا (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	إذا (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_مقصد_fraction(wp, prev_ارتفاع);
    }
}

    فراغ
scroll_مقصد_fraction(win_T *wp, صحيح prev_ارتفاع)
{
    linenr_T	lnum;
    صحيح		sline, سطر_اندازه;
    صحيح		ارتفاع = wp->w_ارتفاع;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    إذا (ارتفاع > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (ارتفاع < wp->w_میانگیر->b_ml.ml_سطر_شمار || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	إذا (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((دراز)wp->w_fraction * (دراز)ارتفاع - 1L)
							       / FRACTION_MULT;
	سطر_اندازه = plines_win_ستون(wp, lnum, (دراز)(wp->w_cursor.ستون)) - 1;
	sline = wp->w_wrow - سطر_اندازه;

	إذا (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    صحيح ردیفها_2 = plines_win(wp, lnum, FALSE);

	    إذا (sline > wp->w_ارتفاع - ردیفها_2)
	    {
		sline = wp->w_ارتفاع - ردیفها_2;
		wp->w_wrow -= ردیفها_2 - سطر_اندازه;
	    }
	}

	إذا (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    صحيح		want_ردیف = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = سطر_اندازه;
	    إذا (wp->w_wrow >= wp->w_ارتفاع
				       && (wp->w_عرض - win_ستون_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_عرض - win_ستون_off(wp);

		--wp->w_wrow;
		طالما (wp->w_wrow > want_ردیف)
		{
		    skipcol += wp->w_عرض - win_ستون_off(wp) + win_ستون_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	وإلا إذا (sline > 0)
	{
	    طالما (sline > 0 && lnum > 1)
	    {
#اگر_تعریف_شده FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);
		إذا (lnum == 1)
		{
		    // first line in buffer is folded
		    سطر_اندازه = 1;
		    --sline;
		    اكسر;
		}
#پایان_اگر
		--lnum;
#اگر_تعریف_شده FEAT_DIFF
		إذا (lnum == wp->w_topline)
		    سطر_اندازه = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		وإلا
#پایان_اگر
		    سطر_اندازه = plines_win(wp, lnum, TRUE);
		sline -= سطر_اندازه;
	    }

	    إذا (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#اگر_تعریف_شده FEAT_FOLDING
		hasFoldingWin(wp, lnum, عدم, &lnum, TRUE, عدم);
#پایان_اگر
		lnum++;
		wp->w_wrow -= سطر_اندازه + sline;
	    }
	    وإلا إذا (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	مجموعه_topline(wp, lnum);
    }

    إذا (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    إذا (prev_ارتفاع > 0)
	wp->w_prev_fraction_ردیف = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    فراغ
win_new_عرض(win_T *wp, صحيح عرض)
{
    // Should we give an error if width < 0?
    wp->w_عرض = عرض < 0 ? 0 : عرض;
    wp->w_سطرها_valid = 0;
    تغییریافته_سطر_abv_curs_win(wp);
    invalidate_botline_win(wp);

    إذا (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_وضعیت = true;
}

    فراغ
win_comp_scroll(win_T *wp)
{
#إذا defined(FEAT_EVAL)
    صحيح old_w_p_scr = wp->w_p_scr;
#پایان_اگر

    wp->w_p_scr = ((بیعلامت)wp->w_ارتفاع >> 1);
    إذا (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#إذا defined(FEAT_EVAL)
    إذا (wp->w_p_scr != old_w_p_scr)
    {
	// Used by "verbose set scroll".
	wp->w_p_script_ctx[WV_SCROLL].sc_sid = SID_WINLAYOUT;
	wp->w_p_script_ctx[WV_SCROLL].sc_lnum = 0;
    }
#پایان_اگر
}

/*
 * Command_height: called whenever p_ch has been changed.
 */
    فراغ
command_ارتفاع(فراغ)
{
    صحيح		old_p_ch = curtab->tp_ch_استفاده_شده;

    // Find bottom frame with width of screen.
    قاب_T *frp = lastwin->w_قاب;
    طالما (frp->fr_عرض != topframe->fr_عرض && frp->fr_والد != عدم)
	frp = frp->fr_والد;

    // Avoid changing the height of a window with 'winfixheight' set.
    طالما (frp->fr_prev != عدم && frp->fr_layout == FR_برگ
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    طالما (p_ch > old_p_ch && command_قاب_ارتفاع)
    {
	إذا (frp == عدم)
	{
	    emsg(_(e_not_enough_جا));
	    p_ch = old_p_ch;
	    اكسر;
	}
	صحيح h = MIN(p_ch - old_p_ch,
			frp->fr_ارتفاع - قاب_minheight(frp, عدم));
	قاب_جمع_کن_ارتفاع(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    إذا (p_ch < old_p_ch && command_قاب_ارتفاع && frp != عدم)
	قاب_جمع_کن_ارتفاع(frp, (صحيح)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_ردیف = ردیفها - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    إذا (پیام_scrolled == 0 && پر_screen)
    {
	screen_fill(cmdline_ردیف, (صحيح)ردیفها, 0, (صحيح)Columns, ' ', ' ', 0);
	پیام_ردیف = cmdline_ردیف;
    }

    // Use the value of p_ch that we remembered.  This is needed for when the
    // GUI starts up, we can't be sure in what order things happen.  And when
    // p_ch was changed in another tab page.
    curtab->tp_ch_استفاده_شده = p_ch;
    min_مجموعه_ch = p_ch;
}

/*
 * Resize frame "frp" to be "n" lines higher (negative for less high).
 * Also resize the frames it is contained in.
 */
    ساكن فراغ
قاب_جمع_کن_ارتفاع(قاب_T *frp, صحيح n)
{
    قاب_new_ارتفاع(frp, frp->fr_ارتفاع + n, FALSE, FALSE, FALSE);
    لكل (;;)
    {
	frp = frp->fr_والد;
	إذا (frp == عدم)
	    اكسر;
	frp->fr_ارتفاع += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    فراغ
آخرین_وضعیت(
    صحيح		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    آخرین_وضعیت_rec(topframe, آخرین_stl_ارتفاع(morewin) > 0);
#إذا defined(FEAT_STL_OPT)
    قاب_change_statusline_ارتفاع();
#پایان_اگر
}

    ساكن فراغ
آخرین_وضعیت_rec(قاب_T *fr, صحيح statusline)
{
    قاب_T	*fp;
    win_T	*wp;

    إذا (fr->fr_layout == FR_برگ)
    {
	wp = fr->fr_win;
	إذا (wp->w_وضعیت_ارتفاع != 0 && !statusline)
	{
	    // remove status line
	    win_new_ارتفاع(wp, wp->w_ارتفاع + wp->w_وضعیت_ارتفاع);
	    wp->w_وضعیت_ارتفاع = 0;
	    comp_ستون();
	}
	وإلا إذا (wp->w_وضعیت_ارتفاع == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    طالما (fp->fr_ارتفاع <= قاب_minheight(fp, عدم))
	    {
		إذا (fp == topframe)
		{
		    emsg(_(e_not_enough_جا));
		    أرجع;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		إذا (fp->fr_والد->fr_layout == FR_ستون && fp->fr_prev != عدم)
		    fp = fp->fr_prev;
		وإلا
		    fp = fp->fr_والد;
	    }
	    wp->w_وضعیت_ارتفاع = statusline_ارتفاع(wp);
	    إذا (fp != fr)
	    {
		قاب_new_ارتفاع(fp, fp->fr_ارتفاع - wp->w_وضعیت_ارتفاع,
			FALSE, FALSE, FALSE);
		قاب_fix_ارتفاع(wp);
		win_comp_pos();
	    }
	    وإلا
		win_new_ارتفاع(wp, wp->w_ارتفاع - wp->w_وضعیت_ارتفاع);
	    comp_ستون();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	إذا (قدر_مطلق(wp->w_ارتفاع - wp->w_prev_ارتفاع) == 1)
	    wp->w_prev_ارتفاع = wp->w_ارتفاع;
    }
    وإلا إذا (fr->fr_layout == FR_ردیف)
    {
	// vertically split windows, set status line for each one
	إذا (!statusline)
	{
	    FOR_ALL_قابها(fp, fr->fr_فرزند)
		آخرین_وضعیت_rec(fp, statusline);
	}
	وإلا
	{
	    قاب_T	*fp2;
	    صحيح		max_stlh = 0;
	    صحيح		new_ردیف_ارتفاع;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_قابها(fp, fr->fr_فرزند)
		إذا (fp->fr_win != عدم && fp->fr_win->w_وضعیت_ارتفاع == 0)
		{
		    صحيح h = statusline_ارتفاع(fp->fr_win);
		    إذا (h > max_stlh)
			max_stlh = h;
		}
	    إذا (max_stlh == 0)
		أرجع;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    طالما (fp2->fr_ارتفاع - قاب_minheight(fp2, عدم) < max_stlh)
	    {
		إذا (fp2 == topframe)
		{
		    emsg(_(e_not_enough_جا));
		    أرجع;
		}
		إذا (fp2->fr_والد->fr_layout == FR_ستون
						&& fp2->fr_prev != عدم)
		    fp2 = fp2->fr_prev;
		وإلا
		    fp2 = fp2->fr_والد;
	    }

	    إذا (fp2 != fr)
	    {
		قاب_new_ارتفاع(fp2, fp2->fr_ارتفاع - max_stlh,
							FALSE, FALSE, FALSE);
		new_ردیف_ارتفاع = fr->fr_ارتفاع + max_stlh;
	    }
	    وإلا
		new_ردیف_ارتفاع = fr->fr_ارتفاع;

	    // Set status and content heights for all leaves.
	    FOR_ALL_قابها(fp, fr->fr_فرزند)
	    {
		إذا (fp->fr_win != عدم)
		{
		    wp = fp->fr_win;
		    إذا (wp->w_وضعیت_ارتفاع == 0)
		    {
			wp->w_وضعیت_ارتفاع = statusline_ارتفاع(wp);
			win_new_ارتفاع(wp,
				  new_ردیف_ارتفاع - wp->w_وضعیت_ارتفاع);
			قاب_fix_ارتفاع(wp);
			comp_ستون();
			redraw_all_later(UPD_SOME_VALID);
			إذا (قدر_مطلق(wp->w_ارتفاع - wp->w_prev_ارتفاع) == 1)
			    wp->w_prev_ارتفاع = wp->w_ارتفاع;
		    }
		}
		وإلا
		    آخرین_وضعیت_rec(fp, statusline); // nested frames
	    }

	    fr->fr_ارتفاع = new_ردیف_ارتفاع;
	    win_comp_pos();
	}
    }
    وإلا
    {
	// horizontally split window, set status line for last one
	لكل (fp = fr->fr_فرزند; fp->fr_بعدی != عدم; fp = fp->fr_بعدی)
	    ;
	آخرین_وضعیت_rec(fp, statusline);
    }
}

#إذا defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    فراغ
update_stl_rendered_ارتفاع(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == عدم) ? p_stl : wp->w_p_stl;
    صحيح		opt_flags = (wp == عدم) ? 0 : OPT_LOCAL;
    صحيح		*ارتفاع = (wp == عدم) ? &stl_rendered_ارتفاع
						: &wp->w_stl_rendered_ارتفاع;

    *ارتفاع = get_stl_rendered_ارتفاع(wp == عدم ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    ساكن صحيح
قاب_پیدا_کن_global_stlh_rec(قاب_T *frp, صحيح h)
{
    إذا (frp->fr_layout == FR_برگ)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	إذا (wp->w_ارتفاع > 0 && wp->w_وضعیت_ارتفاع > 0
		&& *wp->w_p_stlo == NUL)
	{
	    صحيح win_free_ارتفاع = frp->fr_ارتفاع - WINBAR_ارتفاع(wp);

	    إذا (win_free_ارتفاع - p_wmh < h)
		h = win_free_ارتفاع - p_wmh;
	}
    }
    وإلا
    {
	لكل (frp = frp->fr_فرزند; frp != عدم; frp = frp->fr_بعدی)
	    h = قاب_پیدا_کن_global_stlh_rec(frp, h);
    }
    أرجع h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    ساكن فراغ
قاب_change_statusline_ارتفاع_rec(قاب_T *frp, صحيح global_stlh)
{
    إذا (frp->fr_layout == FR_برگ)
    {
	win_T  *wp = frp->fr_win;

	إذا (wp->w_ارتفاع > 0 && wp->w_وضعیت_ارتفاع > 0)
	{
	    صحيح win_free_ارتفاع = frp->fr_ارتفاع - WINBAR_ارتفاع(wp);

	    wp->w_وضعیت_ارتفاع = statusline_ارتفاع(wp);
	    إذا (wp->w_p_stlo == عدم || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		إذا (wp->w_وضعیت_ارتفاع > global_stlh)
		    wp->w_وضعیت_ارتفاع = global_stlh;
	    }
	    إذا (wp->w_وضعیت_ارتفاع > win_free_ارتفاع - p_wmh)
		wp->w_وضعیت_ارتفاع = win_free_ارتفاع - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    صحيح	     saved_topfill = wp->w_topfill;
# endif
	    win_new_ارتفاع(wp, win_free_ارتفاع - wp->w_وضعیت_ارتفاع);
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
	لكل (frp = frp->fr_فرزند; frp != عدم; frp = frp->fr_بعدی)
	    قاب_change_statusline_ارتفاع_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    صحيح
قاب_change_statusline_ارتفاع(فراغ)
{
    tabpage_T	*tp;
    صحيح		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = قاب_پیدا_کن_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	قاب_change_statusline_ارتفاع_rec(tp->tp_topframe, global_stlh);
    comp_ستون();
    redraw_all_later(UPD_SOME_VALID);
    أرجع global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    فراغ
مجموعه_stlo_mh(صحيح mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    صحيح
statuslineopt_تغییریافته(
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
#پایان_اگر

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    صحيح
statusline_ارتفاع(win_T *wp استفادهنشده)
{
    صحيح stl_ارتفاع = 1;
#إذا defined(FEAT_STL_OPT)
    bool fixed;
    صحيح l_stl_rendered_ارتفاع;

    إذا (wp != عدم && *wp->w_p_stlo != NUL)
    {
	stl_ارتفاع = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_ارتفاع = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_ارتفاع
				    : stl_rendered_ارتفاع;
    }
    وإلا
    {
	stl_ارتفاع = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_ارتفاع = (wp != عدم && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_ارتفاع
				    : stl_rendered_ارتفاع;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    إذا (!fixed)
    {
	صحيح has_custom_stl = (*p_stl != NUL)
	    || (wp != عدم && *wp->w_p_stl != NUL);
	إذا (has_custom_stl)
	    stl_ارتفاع = MAX(1, MIN(l_stl_rendered_ارتفاع, stl_ارتفاع));
	وإلا
	    stl_ارتفاع = 1;
    }
#پایان_اگر

    أرجع stl_ارتفاع;
}

/*
 * Return the number of lines used by the tab page line.
 */
    صحيح
tabline_ارتفاع(فراغ)
{
#اگر_تعریف_شده FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    إذا (gui_use_tabline())
	أرجع 0;
#پایان_اگر
    بدل (p_stal)
    {
	حالة 0: أرجع 0;
	حالة 1: أرجع (اولی_tabpage->tp_بعدی == عدم) ? 0 : 1;
    }
    أرجع 1;
}

/*
 * Return the height of the last window's statusline.
 */
    صحيح
آخرین_stl_ارتفاع(
    صحيح		morewin)	// pretend there are two or more windows
{
    أرجع (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_پنجره)))
		? وضعیت_ارتفاع_2 : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    صحيح
min_ردیفها(فراغ)
{
    إذا (firstwin == عدم)	// not initialized yet
	أرجع MIN_سطرها;

    أرجع قاب_minheight(curtab->tp_topframe, عدم) + tabline_ارتفاع()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    صحيح
min_ردیفها_for_all_tabpages(فراغ)
{
    صحيح		مجموع;
    tabpage_T	*tp;
    صحيح		n;

    إذا (firstwin == عدم)	// not initialized yet
	أرجع MIN_سطرها;

    مجموع = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = قاب_minheight(tp->tp_topframe, عدم);
	إذا (مجموع < n)
	    مجموع = n;
    }
    مجموع += tabline_ارتفاع();
    مجموع += MIN_CMDHEIGHT;
    أرجع مجموع;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    صحيح
only_one_پنجره(فراغ)
{
    صحيح		شمار = 0;
    win_T	*wp;

#إذا defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    إذا (popup_is_popup(curwin))
	أرجع FALSE;
#پایان_اگر

    // If there is another tab page there always is another window.
    إذا (اولی_tabpage->tp_بعدی != عدم)
	أرجع FALSE;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_میانگیر != عدم
		&& (!((bt_help(wp->w_میانگیر) && !bt_help(curbuf))
#اگر_تعریف_شده FEAT_QUICKFIX
		    || wp->w_p_pvw
#پایان_اگر
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++شمار;
    أرجع (شمار <= 1);
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
	إذا ((do_curwin || wp != curwin) && wp->w_میانگیر == curbuf)
	{
	    صحيح need_adjust;

	    إذا (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_سطر_شمار;
	    إذا (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;
	    إذا (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_سطر_شمار;
	    إذا (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_سطر_شمار;
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
	إذا (wp->w_میانگیر == curbuf)
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
	    إذا (wp->w_save_cursor.w_topline_save > wp->w_میانگیر->b_ml.ml_سطر_شمار)
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
make_snapshot_rec(قاب_T *fr, قاب_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(قاب_T);
    إذا (*frp == عدم)
	أرجع FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_عرض = fr->fr_عرض;
    (*frp)->fr_ارتفاع = fr->fr_ارتفاع;
    إذا (fr->fr_بعدی != عدم)
    {
	إذا (make_snapshot_rec(fr->fr_بعدی, &((*frp)->fr_بعدی)) == FAIL)
	    أرجع FAIL;
    }
    إذا (fr->fr_فرزند != عدم)
    {
	إذا (make_snapshot_rec(fr->fr_فرزند, &((*frp)->fr_فرزند)) == FAIL)
	    أرجع FAIL;
    }
    إذا (fr->fr_layout == FR_برگ && fr->fr_win == curwin)
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
clear_snapshot_rec(قاب_T *fr)
{
    إذا (fr == عدم)
	أرجع;
    clear_snapshot_rec(fr->fr_بعدی);
    clear_snapshot_rec(fr->fr_فرزند);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    ساكن win_T *
get_snapshot_curwin_rec(قاب_T *ft)
{
    win_T	*wp;

    إذا (ft->fr_بعدی != عدم)
    {
	إذا ((wp = get_snapshot_curwin_rec(ft->fr_بعدی)) != عدم)
	    أرجع wp;
    }
    إذا (ft->fr_فرزند != عدم)
    {
	إذا ((wp = get_snapshot_curwin_rec(ft->fr_فرزند)) != عدم)
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
	    && curtab->tp_snapshot[idx]->fr_عرض == topframe->fr_عرض
	    && curtab->tp_snapshot[idx]->fr_ارتفاع == topframe->fr_ارتفاع
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
check_snapshot_rec(قاب_T *sn, قاب_T *fr)
{
    إذا (sn->fr_layout != fr->fr_layout
	    || (sn->fr_بعدی == عدم) != (fr->fr_بعدی == عدم)
	    || (sn->fr_فرزند == عدم) != (fr->fr_فرزند == عدم)
	    || (sn->fr_بعدی != عدم
		&& check_snapshot_rec(sn->fr_بعدی, fr->fr_بعدی) == FAIL)
	    || (sn->fr_فرزند != عدم
		&& check_snapshot_rec(sn->fr_فرزند, fr->fr_فرزند) == FAIL)
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
restore_snapshot_rec(قاب_T *sn, قاب_T *fr)
{
    win_T	*wp = عدم;
    win_T	*wp2;

    fr->fr_ارتفاع = sn->fr_ارتفاع;
    fr->fr_عرض = sn->fr_عرض;
    إذا (fr->fr_layout == FR_برگ)
    {
	قاب_new_ارتفاع(fr, fr->fr_ارتفاع, FALSE, FALSE, FALSE);
	قاب_new_عرض(fr, fr->fr_عرض, FALSE, FALSE);
	wp = sn->fr_win;
    }
    إذا (sn->fr_بعدی != عدم)
    {
	wp2 = restore_snapshot_rec(sn->fr_بعدی, fr->fr_بعدی);
	إذا (wp2 != عدم)
	    wp = wp2;
    }
    إذا (sn->fr_فرزند != عدم)
    {
	wp2 = restore_snapshot_rec(sn->fr_فرزند, fr->fr_فرزند);
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
    قاب_T	*fr;

    إذا (topframe->fr_layout == FR_ردیف)
	أرجع TRUE;

    إذا (topframe->fr_layout == FR_ستون)
	FOR_ALL_قابها(fr, topframe->fr_فرزند)
	    إذا (fr->fr_layout == FR_ردیف)
		أرجع TRUE;

    أرجع FALSE;
}
#پایان_اگر

#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    صحيح
get_win_عدد(win_T *wp, win_T *اولی_win)
{
    صحيح		i = 1;
    win_T	*w;

    لكل (w = اولی_win; w != عدم && w != wp; w = W_بعدی(w))
	++i;

    إذا (w == عدم)
	أرجع 0;
    وإلا
	أرجع i;
}

    صحيح
get_tab_عدد(tabpage_T *tp)
{
    صحيح		i = 1;
    tabpage_T	*t;

    لكل (t = اولی_tabpage; t != عدم && t != tp; t = t->tp_بعدی)
	++i;

    إذا (t == عدم)
	أرجع 0;
    وإلا
	أرجع i;
}
#پایان_اگر

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    ساكن صحيح
قاب_check_ارتفاع(قاب_T *topfrp, صحيح ارتفاع)
{
    قاب_T *frp;

    إذا (topfrp->fr_ارتفاع != ارتفاع)
	أرجع FALSE;

    إذا (topfrp->fr_layout == FR_ردیف)
	FOR_ALL_قابها(frp, topfrp->fr_فرزند)
	    إذا (frp->fr_ارتفاع != ارتفاع)
		أرجع FALSE;

    أرجع TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    ساكن صحيح
قاب_check_عرض(قاب_T *topfrp, صحيح عرض)
{
    قاب_T *frp;

    إذا (topfrp->fr_عرض != عرض)
	أرجع FALSE;

    إذا (topfrp->fr_layout == FR_ستون)
	FOR_ALL_قابها(frp, topfrp->fr_فرزند)
	    إذا (frp->fr_عرض != عرض)
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
    char_u	*s = خالی_option;
    صحيح		tw;
    صحيح		ستون;
    صحيح		شمار = 0;
    صحيح		color_تعداد_ستون[256];
    صحيح		i;
    صحيح		j = 0;

    إذا (wp != عدم && wp->w_میانگیر == عدم)
	أرجع عدم;  // buffer was closed

    إذا (cc != عدم)
	s = cc;
    وإلا إذا (wp != عدم)
	s = wp->w_p_cc;

    إذا (wp != عدم)
	tw = wp->w_میانگیر->b_p_tw;
    وإلا
	// buffer-local value not set, assume zero
	tw = 0;

    طالما (*s != NUL && شمار < 255)
    {
	إذا (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    ستون = (*s == '-') ? -1 : 1;
	    ++s;
	    إذا (!VIM_ISDIGIT(*s))
		أرجع e_invalid_آرگومان;
	    ستون = ستون * getdigits(&s);
	    إذا (tw == 0)
		اذهب skip;  // 'textwidth' not set, skip this item
	    ستون += tw;
	    إذا (ستون < 0)
		اذهب skip;
	}
	وإلا إذا (VIM_ISDIGIT(*s))
	    ستون = getdigits(&s);
	وإلا
	    أرجع e_invalid_آرگومان;
	color_تعداد_ستون[شمار++] = ستون - 1;  // 1-based to 0-based
skip:
	إذا (*s == NUL)
	    اكسر;
	إذا (*s != ',')
	    أرجع e_invalid_آرگومان;
	إذا (*++s == NUL)
	    أرجع e_invalid_آرگومان;  // illegal trailing comma as in "set cc=80,"
    }

    إذا (wp == عدم)
	أرجع عدم;  // only parse "cc"

    vim_free(wp->w_p_cc_تعداد_ستون);
    إذا (شمار == 0)
	wp->w_p_cc_تعداد_ستون = عدم;
    وإلا
    {
	wp->w_p_cc_تعداد_ستون = ALLOC_MULT(صحيح, شمار + 1);
	إذا (wp->w_p_cc_تعداد_ستون != عدم)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_تعداد_ستون, شمار, حجم(صحيح), int_cmp);

	    لكل (i = 0; i < شمار; ++i)
		// skip duplicates
		إذا (j == 0 || wp->w_p_cc_تعداد_ستون[j - 1] != color_تعداد_ستون[i])
		    wp->w_p_cc_تعداد_ستون[j++] = color_تعداد_ستون[i];
	    wp->w_p_cc_تعداد_ستون[j] = -1;  // end marker
	}
    }

    أرجع عدم;  // no error
}
#پایان_اگر

    صحيح
get_آخرین_winid(فراغ)
{
    أرجع آخرین_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   صحيح
win_locked(win_T *wp)
{
    أرجع wp->w_locked;
}
