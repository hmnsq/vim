/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ଅନ୍ତର୍ଭାଵ "vim.h"

ସ୍ଥିର ଶୂନ୍ୟ cmd_with_ଗଣନା(ଅକ୍ଷର *cmd, char_u *bufp, size_t bufsize, long Prenum);
ସ୍ଥିର ଶୂନ୍ୟ win_init(win_T *newp, win_T *oldp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ସ୍ଥିର ଶୂନ୍ୟ win_init_some(win_T *newp, win_T *oldp);
ସ୍ଥିର ଶୂନ୍ୟ frame_comp_pos(frame_T *topfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *col);
ସ୍ଥିର ଶୂନ୍ୟ frame_setheight(frame_T *curfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height);
ସ୍ଥିର ଶୂନ୍ୟ frame_setwidth(frame_T *curfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width);
ସ୍ଥିର ଶୂନ୍ୟ win_exchange(long);
ସ୍ଥିର ଶୂନ୍ୟ win_rotate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଶୂନ୍ୟ win_equal_rec(win_T *next_curwin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା current, frame_T *topfr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height);
ସ୍ଥିର ଶୂନ୍ୟ trigger_winnewpre(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ trigger_winclosed(win_T *win);
ସ୍ଥିର win_T *win_free_mem(win_T *win, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dirp, tabpage_T *tp);
ସ୍ଥିର frame_T *win_altframe(win_T *win, tabpage_T *tp);
ସ୍ଥିର tabpage_T *alt_tabpage(ଶୂନ୍ୟ);
ସ୍ଥିର win_T *frame2win(frame_T *frp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_has_win(frame_T *frp, win_T *wp);
ସ୍ଥିର ଶୂନ୍ୟ win_fix_scroll(ପୂର୍ଣ୍ଣସଂଖ୍ୟା resize);
ସ୍ଥିର ଶୂନ୍ୟ win_fix_cursor(ପୂର୍ଣ୍ଣସଂଖ୍ୟା normal);
ସ୍ଥିର ଶୂନ୍ୟ frame_new_height(frame_T *topfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height, ପୂର୍ଣ୍ଣସଂଖ୍ୟା topfirst, ପୂର୍ଣ୍ଣସଂଖ୍ୟା wfh, ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_ch);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_fixed_height(frame_T *frp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_fixed_width(frame_T *frp);
ସ୍ଥିର ଶୂନ୍ୟ frame_add_statusline(frame_T *frp);
ସ୍ଥିର ଶୂନ୍ୟ frame_new_width(frame_T *topfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା leftfirst, ପୂର୍ଣ୍ଣସଂଖ୍ୟା wfw);
ସ୍ଥିର ଶୂନ୍ୟ frame_set_vsep(frame_T *frp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା add);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_minwidth(frame_T *topfrp, win_T *next_curwin);
ସ୍ଥିର ଶୂନ୍ୟ frame_fix_width(win_T *wp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା win_alloc_firstwin(win_T *oldwin);
ସ୍ଥିର ଶୂନ୍ୟ new_frame(win_T *wp);
ସ୍ଥିର tabpage_T *alloc_tabpage(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା leave_tabpage(buf_T *new_curbuf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା trigger_leave_autocmds);
ସ୍ଥିର ଶୂନ୍ୟ enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା trigger_enter_autocmds, ପୂର୍ଣ୍ଣସଂଖ୍ୟା trigger_leave_autocmds);
ସ୍ଥିର ଶୂନ୍ୟ frame_fix_height(win_T *wp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_minheight(frame_T *topfrp, win_T *next_curwin);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_wincount_in_height(frame_T *topfrp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା may_open_tabpage(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା win_enter_ext(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ସ୍ଥିର ଶୂନ୍ୟ win_free(win_T *wp, tabpage_T *tp);
ସ୍ଥିର ଶୂନ୍ୟ win_append(win_T *after, win_T *wp);
ସ୍ଥିର ଶୂନ୍ୟ frame_append(frame_T *after, frame_T *frp);
ସ୍ଥିର ଶୂନ୍ୟ frame_ଭର୍ତ୍ତି(frame_T *before, frame_T *frp);
ସ୍ଥିର ଶୂନ୍ୟ frame_remove(frame_T *frp);
ସ୍ଥିର ଶୂନ୍ୟ win_goto_ver(ପୂର୍ଣ୍ଣସଂଖ୍ୟା up, long ଗଣନା_2);
ସ୍ଥିର ଶୂନ୍ୟ win_goto_hor(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ, long ଗଣନା_2);
ସ୍ଥିର ଶୂନ୍ୟ frame_add_height(frame_T *frp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
ସ୍ଥିର ଶୂନ୍ୟ last_status_rec(frame_T *fr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା statusline);
#ଯଦି defined(FEAT_STL_OPT)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_find_global_stlh_rec(frame_T *frp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h);
ସ୍ଥିର ଶୂନ୍ୟ frame_change_statusline_height_rec(frame_T *frp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା global_stlh);
#endif
ସ୍ଥିର ଶୂନ୍ୟ frame_flatten(frame_T *frp);
ସ୍ଥିର ଶୂନ୍ୟ winframe_restore(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, frame_T *unflat_altfr);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_snapshot_rec(frame_T *fr, frame_T **frp);
ସ୍ଥିର ଶୂନ୍ୟ clear_snapshot(tabpage_T *tp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ସ୍ଥିର ଶୂନ୍ୟ clear_snapshot_rec(frame_T *fr);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_snapshot_rec(frame_T *sn, frame_T *fr);
ସ୍ଥିର win_T *restore_snapshot_rec(frame_T *sn, frame_T *fr);
ସ୍ଥିର win_T *get_snapshot_curwin(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_check_height(frame_T *topfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_check_width(frame_T *topfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width);

ସ୍ଥିର win_T *win_alloc(win_T *after, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hidden);

#define NOWIN		((win_T *)-1)	// non-existing window

#define ROWS_AVAIL (Rows - p_ch - tabline_height())

// flags for win_enter_ext()
#define WEE_UNDO_SYNC			0x01
#define WEE_CURWIN_INVALID		0x02
#define WEE_TRIGGER_NEW_AUTOCMDS	0x04
#define WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#define WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#define WEE_ALLOW_PARSE_MESSAGES	0x20

ସ୍ଥିର ଅକ୍ଷର *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା split_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା frame_locked = 0;

#ଯଦି defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା stl_rendered_height = 1;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା stlo_mh = STATUS_HEIGHT; // "maxheight" value cache of global 'stlo'
#endif

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    ଶୂନ୍ୟ
window_layout_lock(ଶୂନ୍ୟ)
{
    ++split_disallowed;
    ++close_disallowed;
}

    ଶୂନ୍ୟ
window_layout_unlock(ଶୂନ୍ୟ)
{
    --split_disallowed;
    --close_disallowed;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
frames_locked(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ frame_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
window_layout_locked(ଗଣନା CMD_index cmd)
{
    ଯଦି (split_disallowed > 0 || close_disallowed > 0)
    {
	ଯଦି (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_split_window_when_closing_buffer));
	ଅନ୍ୟଥା
	    emsg(_(e_not_allowed_to_change_window_layout_in_this_autocmd));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

// #define WIN_DEBUG
#ifdef WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
log_frame_layout(frame_T *frame)
{
    ch_log(ଖାଲି, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    frame->fr_layout == FR_LEAF ? "LEAF"
				  : frame->fr_layout == FR_ROW ? "ROW" : "COL",
	    frame->fr_width,
	    frame->fr_height,
	    frame->fr_win == ଖାଲି ? -1 : frame->fr_win->w_width,
	    frame->fr_win == ଖାଲି ? -1 : frame->fr_win->w_height,
	    frame->fr_win == ଖାଲି ? -1 : frame->fr_win->w_id);
    ଯଦି (frame->fr_child != ଖାଲି)
    {
	ch_log(ଖାଲି, "children");
	log_frame_layout(frame->fr_child);
	ଯଦି (frame->fr_next != ଖାଲି)
	    ch_log(ଖାଲି, "END of children");
    }
    ଯଦି (frame->fr_next != ଖାଲି)
	log_frame_layout(frame->fr_next);
}
#endif

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_can_set_curbuf_disabled(ଶୂନ୍ୟ)
{
    ଯଦି (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_can_set_curbuf_forceit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    ଯଦି (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(ଶୂନ୍ୟ)
{
    // In cmdwin, the alternative buffer should be used.
    ଫେରନ୍ତୁ is_in_cmdwin() && prevwin != ଖାଲି ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = ଖାଲି;

    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    ଯଦି (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    ଯଦି (wp == ଖାଲି && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    ଫେରନ୍ତୁ wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    ଶୂନ୍ୟ
do_window(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nchar,
    long	Prenum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		xchar)	    // extra char from ":wincmd gx" or NUL
{
    long	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ifdef FEAT_FIND_ID
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type = FIND_DEFINE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
#endif
    char_u	cbuf[40];

    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;

#define CHECK_CMDWIN \
    କର { \
	ଯଦି (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_in_cmdline_window)); \
	    ଫେରନ୍ତୁ; \
	} \
    } ଯେତେବେଳେ (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    ବାଛ (nchar)
    {
// split current window in two parts, horizontally
    ସ୍ଥିତି 'S':
    ସ୍ଥିତି Ctrl_S:
    ସ୍ଥିତି 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		ଯଦି (bt_quickfix(curbuf))
		    ଯାଅ newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(ଶୂନ୍ୟ)win_split((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum, 0);
		ଭାଙ୍ଗ;

// split current window in two parts, vertically
    ସ୍ଥିତି Ctrl_V:
    ସ୍ଥିତି 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		ଯଦି (bt_quickfix(curbuf))
		    ଯାଅ newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(ଶୂନ୍ୟ)win_split((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum, WSP_VERT);
		ଭାଙ୍ଗ;

// split current window and edit alternate file
    ସ୍ଥିତି Ctrl_HAT:
    ସ୍ଥିତି '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		ଯଦି (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == ଖାଲି)
		{
		    ଯଦି (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    ଅନ୍ୟଥା
			semsg(_(e_buffer_nr_not_found), Prenum);
		    ଭାଙ୍ଗ;
		}

		ଯଦି (!curbuf_locked() && win_split(0, 0) == OK)
		    (ଶୂନ୍ୟ)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		ଭାଙ୍ଗ;

// open new window
    ସ୍ଥିତି Ctrl_N:
    ସ୍ଥିତି 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		ଯଦି (Prenum)
		    // window height
		    vim_snprintf((ଅକ୍ଷର *)cbuf, ଆକାର(cbuf) - 5, "%ld", Prenum);
		ଅନ୍ୟଥା
		    cbuf[0] = NUL;
#ଯଦି defined(FEAT_QUICKFIX)
		ଯଦି (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#endif
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		ଭାଙ୍ଗ;

// quit current window
    ସ୍ଥିତି Ctrl_Q:
    ସ୍ଥିତି 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ଗଣନା("quit", cbuf, ଆକାର(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ଭାଙ୍ଗ;

// close current window
    ସ୍ଥିତି Ctrl_C:
    ସ୍ଥିତି 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ଗଣନା("close", cbuf, ଆକାର(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ଭାଙ୍ଗ;

#ଯଦି defined(FEAT_QUICKFIX)
// close preview window
    ସ୍ଥିତି Ctrl_Z:
    ସ୍ଥିତି 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		ଭାଙ୍ଗ;

// cursor to preview window
    ସ୍ଥିତି 'P':
		FOR_ALL_WINDOWS(wp)
		    ଯଦି (wp->w_p_pvw)
			ଭାଙ୍ଗ;
		ଯଦି (wp == ଖାଲି)
		    emsg(_(e_there_is_no_preview_window));
		ଅନ୍ୟଥା
		    win_goto(wp);
		ଭାଙ୍ଗ;
#endif

// close all but current window
    ସ୍ଥିତି Ctrl_O:
    ସ୍ଥିତି 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_ଗଣନା("only", cbuf, ଆକାର(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ଭାଙ୍ଗ;

// cursor to next window with wrap around
    ସ୍ଥିତି Ctrl_W:
    ସ୍ଥିତି 'w':
// cursor to previous window with wrap around
    ସ୍ଥିତି 'W':
		CHECK_CMDWIN;
		ଯଦି (ONE_WINDOW && Prenum != 1)	// just one window
		    beep_flush();
		ଅନ୍ୟଥା
		{
		    ଯଦି (Prenum)			// go to specified window
		    {
			ପାଇଁ (wp = firstwin; --Prenum > 0; )
			{
			    ଯଦି (wp->w_next == ଖାଲି)
				ଭାଙ୍ଗ;
			    ଅନ୍ୟଥା
				wp = wp->w_next;
			}
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    ଯଦି (wp == ଖାଲି)
				wp = lastwin;	    // wrap around
			}
			ଅନ୍ୟଥା			    // go to next window
			{
			    wp = curwin->w_next;
			    ଯଦି (wp == ଖାଲି)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		ଭାଙ୍ଗ;

// cursor to window below
    ସ୍ଥିତି 'j':
    ସ୍ଥିତି K_DOWN:
    ସ୍ଥିତି Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		ଭାଙ୍ଗ;

// cursor to window above
    ସ୍ଥିତି 'k':
    ସ୍ଥିତି K_UP:
    ସ୍ଥିତି Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		ଭାଙ୍ଗ;

// cursor to left window
    ସ୍ଥିତି 'h':
    ସ୍ଥିତି K_ବାମ:
    ସ୍ଥିତି Ctrl_H:
    ସ୍ଥିତି K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		ଭାଙ୍ଗ;

// cursor to right window
    ସ୍ଥିତି 'l':
    ସ୍ଥିତି K_ଡାହାଣ:
    ସ୍ଥିତି Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		ଭାଙ୍ଗ;

// move window to new tab page
    ସ୍ଥିତି 'T':
		CHECK_CMDWIN;
		ଯଦି (one_window())
		    ସନ୍ଦେଶ(_(m_onlyone));
		ଅନ୍ୟଥା
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    ଯଦି (win_new_tabpage((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			ଯଦି (curwin == wp)
			    win_close(curwin, FALSE);
			ଯଦି (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		ଭାଙ୍ଗ;

// cursor to top-left window
    ସ୍ଥିତି 't':
    ସ୍ଥିତି Ctrl_T:
		win_goto(firstwin);
		ଭାଙ୍ଗ;

// cursor to bottom-right window
    ସ୍ଥିତି 'b':
    ସ୍ଥିତି Ctrl_B:
		win_goto(lastwin);
		ଭାଙ୍ଗ;

// cursor to last accessed (previous) window
    ସ୍ଥିତି 'p':
    ସ୍ଥିତି Ctrl_P:
		ଯଦି (!win_valid(prevwin))
		    beep_flush();
		ଅନ୍ୟଥା
		    win_goto(prevwin);
		ଭାଙ୍ଗ;

// exchange current and next window
    ସ୍ଥିତି 'x':
    ସ୍ଥିତି Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		ଭାଙ୍ଗ;

// rotate windows downwards
    ସ୍ଥିତି Ctrl_R:
    ସ୍ଥିତି 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum1);    // downwards
		ଭାଙ୍ଗ;

// rotate windows upwards
    ସ୍ଥିତି 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum1);	    // upwards
		ଭାଙ୍ଗ;

// move window to the very top/bottom/left/right
    ସ୍ଥିତି 'K':
    ସ୍ଥିତି 'J':
    ସ୍ଥିତି 'H':
    ସ୍ଥିତି 'L':
		CHECK_CMDWIN;
		ଯଦି (ONE_WINDOW)
		    beep_flush();
		ଅନ୍ୟଥା
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_ଶୀର୍ଷ : WSP_BOT);

		    (ଶୂନ୍ୟ)win_splitmove(curwin, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum, dir);
		}
		ଭାଙ୍ଗ;

// make all windows the same width and/or height
    ସ୍ଥିତି '=':
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mod = cmdmod.cmod_split & (WSP_VERT | WSP_HOR);
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_equal(ଖାଲି, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		ଭାଙ୍ଗ;

// increase current window height
    ସ୍ଥିତି '+':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum1);
		ଭାଙ୍ଗ;

// decrease current window height
    ସ୍ଥିତି '-':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum1);
		ଭାଙ୍ଗ;

// set current window height
    ସ୍ଥିତି Ctrl__:
    ସ୍ଥିତି '_':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(Prenum ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum : 9999);
		ଭାଙ୍ଗ;

// increase current window width
    ସ୍ଥିତି '>':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum1);
		ଭାଙ୍ଗ;

// decrease current window width
    ସ୍ଥିତି '<':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum1);
		ଭାଙ୍ଗ;

// set current window width
    ସ୍ଥିତି '|':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(Prenum != 0 ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum : 9999);
		ଭାଙ୍ଗ;

// jump to tag and split window if tag exists (in preview window)
#ଯଦି defined(FEAT_QUICKFIX)
    ସ୍ଥିତି '}':
		CHECK_CMDWIN;
		ଯଦି (Prenum)
		    g_do_tagpreview = Prenum;
		ଅନ୍ୟଥା
		    g_do_tagpreview = p_pvh;
#endif
		// FALLTHROUGH
    ସ୍ଥିତି ']':
    ସ୍ଥିତି Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		ଯଦି (Prenum)
		    postponed_split = Prenum;
		ଅନ୍ୟଥା
		    postponed_split = -1;
#ifdef FEAT_QUICKFIX
		ଯଦି (nchar != '}')
		    g_do_tagpreview = 0;
#endif

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_split = 0;
		ଭାଙ୍ଗ;

// edit file name under cursor in a new window
    ସ୍ଥିତି 'f':
    ସ୍ଥିତି 'F':
    ସ୍ଥିତି Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		ଯଦି (check_text_or_curbuf_locked(ଖାଲି))
		    ଭାଙ୍ଗ;

		ptr = grab_file_name(Prenum1, &lnum);
		ଯଦି (ptr != ଖାଲି)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = ଖାଲି;
		    ଯଦି ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    ଯଦି (wp == ଖାଲି && win_split(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			ଯଦି (do_ecmd(0, ptr, ଖାଲି, ଖାଲି, ECMD_LASTL,
						   ECMD_HIDE, ଖାଲି) == FAIL)
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
			ଅନ୍ୟଥା
			    wp = curwin;
		    }

		    ଯଦି (wp != ଖାଲି && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		ଭାଙ୍ଗ;

#ifdef FEAT_FIND_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    ସ୍ଥିତି 'i':			    // Go to any match
    ସ୍ଥିତି Ctrl_I:
		type = FIND_ANY;
		// FALLTHROUGH
    ସ୍ଥିତି 'd':			    // Go to definition, using 'define'
    ସ୍ଥିତି Ctrl_D:
		CHECK_CMDWIN;
		ଯଦି ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
		    ଭାଙ୍ଗ;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		ଯଦି (ptr == ଖାଲି)
		    ଭାଙ୍ଗ;

		find_pattern_in_path(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_SPLIT, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_set_curswant = true;
		ଭାଙ୍ଗ;
#endif

// Quickfix window only: view the result under the cursor in a new split.
#ଯଦି defined(FEAT_QUICKFIX)
    ସ୍ଥିତି K_KENTER:
    ସ୍ଥିତି CAR:
		ଯଦି (bt_quickfix(curbuf))
		    qf_view_result(TRUE);
		ଭାଙ୍ଗ;
#endif

// CTRL-W g  extended commands
    ସ୍ଥିତି 'g':
    ସ୍ଥିତି Ctrl_G:
		CHECK_CMDWIN;
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#endif
		++no_mapping;
		++allow_ଚାବିଗୁଡ଼ିକ;   // no mapping for xchar, but allow key codes
		ଯଦି (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_ଚାବିଗୁଡ଼ିକ;
		(ଶୂନ୍ୟ)add_to_showcmd(xchar);

		ବାଛ (xchar)
		{
#ଯଦି defined(FEAT_QUICKFIX)
		    ସ୍ଥିତି '}':
			xchar = Ctrl_RSB;
			ଯଦି (Prenum)
			    g_do_tagpreview = Prenum;
			ଅନ୍ୟଥା
			    g_do_tagpreview = p_pvh;
#endif
			// FALLTHROUGH
		    ସ୍ଥିତି ']':
		    ସ୍ଥିତି Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			ଯଦି (Prenum)
			    postponed_split = Prenum;
			ଅନ୍ୟଥା
			    postponed_split = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_split = 0;
			ଭାଙ୍ଗ;

		    ସ୍ଥିତି 'f':	    // CTRL-W gf: "gf" in a new tab page
		    ସ୍ଥିତି 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_index(curtab) + 1;
			nchar = xchar;
			ଯାଅ wingotofile;

		    ସ୍ଥିତି 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum);
			ଭାଙ୍ଗ;

		    ସ୍ଥିତି 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Prenum1);
			ଭାଙ୍ଗ;

		    ସ୍ଥିତି TAB:	    // CTRL-W g<Tab>: go to last used tab page
			ଯଦି (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			ଭାଙ୍ଗ;

		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			beep_flush();
			ଭାଙ୍ଗ;
		}
		ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	beep_flush();
		ଭାଙ୍ଗ;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    ଶୂନ୍ୟ
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    ବାଛ (*arg)
    {
    ସ୍ଥିତି 'S':
    ସ୍ଥିତି Ctrl_S:
    ସ୍ଥିତି 's':
    ସ୍ଥିତି Ctrl_N:
    ସ୍ଥିତି 'n':
    ସ୍ଥିତି 'j':
    ସ୍ଥିତି Ctrl_J:
    ସ୍ଥିତି 'k':
    ସ୍ଥିତି Ctrl_K:
    ସ୍ଥିତି 'T':
    ସ୍ଥିତି Ctrl_R:
    ସ୍ଥିତି 'r':
    ସ୍ଥିତି 'R':
    ସ୍ଥିତି 'K':
    ସ୍ଥିତି 'J':
    ସ୍ଥିତି '+':
    ସ୍ଥିତି '-':
    ସ୍ଥିତି Ctrl__:
    ସ୍ଥିତି '_':
    ସ୍ଥିତି '|':
    ସ୍ଥିତି ']':
    ସ୍ଥିତି Ctrl_RSB:
    ସ୍ଥିତି 'g':
    ସ୍ଥିତି Ctrl_G:
    ସ୍ଥିତି Ctrl_V:
    ସ୍ଥିତି 'v':
    ସ୍ଥିତି 'h':
    ସ୍ଥିତି Ctrl_H:
    ସ୍ଥିତି 'l':
    ସ୍ଥିତି Ctrl_L:
    ସ୍ଥିତି 'H':
    ସ୍ଥିତି 'L':
    ସ୍ଥିତି '>':
    ସ୍ଥିତି '<':
#ଯଦି defined(FEAT_QUICKFIX)
    ସ୍ଥିତି '}':
#endif
    ସ୍ଥିତି 'f':
    ସ୍ଥିତି 'F':
    ସ୍ଥିତି Ctrl_F:
#ifdef FEAT_FIND_ID
    ସ୍ଥିତି 'i':
    ସ୍ଥିତି Ctrl_I:
    ସ୍ଥିତି 'd':
    ସ୍ଥିତି Ctrl_D:
#endif
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		ଭାଙ୍ଗ;

    ସ୍ଥିତି Ctrl_HAT:
    ସ୍ଥିତି '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		ଭାଙ୍ଗ;

    ସ୍ଥିତି Ctrl_Q:
    ସ୍ଥିତି 'q':
    ସ୍ଥିତି Ctrl_C:
    ସ୍ଥିତି 'c':
    ସ୍ଥିତି Ctrl_O:
    ସ୍ଥିତି 'o':
    ସ୍ଥିତି Ctrl_W:
    ସ୍ଥିତି 'w':
    ସ୍ଥିତି 'W':
    ସ୍ଥିତି 'x':
    ସ୍ଥିତି Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		ଭାଙ୍ଗ;

#ଯଦି defined(FEAT_QUICKFIX)
    ସ୍ଥିତି Ctrl_Z:
    ସ୍ଥିତି 'z':
    ସ୍ଥିତି 'P':
#endif
    ସ୍ଥିତି 't':
    ସ୍ଥିତି Ctrl_T:
    ସ୍ଥିତି 'b':
    ସ୍ଥିତି Ctrl_B:
    ସ୍ଥିତି 'p':
    ସ୍ଥିତି Ctrl_P:
    ସ୍ଥିତି '=':
    ସ୍ଥିତି CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		ଭାଙ୍ଗ;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
cmd_with_ଗଣନା(
    ଅକ୍ଷର	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    long	Prenum)
{
    ଯଦି (Prenum > 0)
	vim_snprintf((ଅକ୍ଷର *)bufp, bufsize, "%s %ld", cmd, Prenum);
    ଅନ୍ୟଥା
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_split_disallowed(win_T *wp)
{
    ଯଦି (split_disallowed > 0)
    {
	emsg(_(e_cant_split_window_while_closing_another));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (wp->w_buffer->b_locked_split)
    {
	emsg(_(e_cannot_split_window_when_closing_buffer));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_split(ପୂର୍ଣ୍ଣସଂଖ୍ୟା size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (check_split_disallowed(curwin) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    ଯଦି (may_open_tabpage() == OK)
	ଫେରନ୍ତୁ OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_split;
    ଯଦି ((flags & WSP_ଶୀର୍ଷ) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_split_topleft_and_botright_at_the_same_time));
	ଫେରନ୍ତୁ FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    ଯଦି (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    ଅନ୍ୟଥା
	clear_snapshot(curtab, SNAP_HELP_IDX);

    ଯଦି (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    ଅନ୍ୟଥା
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    ଫେରନ୍ତୁ win_split_ins(size, flags, ଖାଲି, 0, ଖାଲି);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_split_ins(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		size,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    win_T	*new_wp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
    frame_T	*to_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_size = size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_status = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_equal = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		needed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		available;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oldwin_height = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		layout;
    frame_T	*frp, *curfrp, *frp2, *prevfrp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		before;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		minheight;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wmh1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_set_fraction = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    ଯଦି (new_wp == ଖାଲି)
	trigger_winnewpre();

    ଯଦି (flags & WSP_ଶୀର୍ଷ)
	oldwin = firstwin;
    ଅନ୍ୟଥା ଯଦି (flags & WSP_BOT)
	oldwin = lastwin;
    ଅନ୍ୟଥା
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    ଯଦି (ONE_WINDOW && p_ls == 1 && oldwin->w_status_height == 0)
    {
	ଯଦି (!(flags & WSP_FORCE_ROOM) && VISIBLE_HEIGHT(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_room));
	    ଯାଅ theend;
	}
	need_status = statusline_height(oldwin);
    }

#ifdef FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    ଯଦି (gui.in_use)
	out_flush();
#endif

    ଯଦି (flags & WSP_VERT)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	wmw1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	minwidth;

	layout = FR_ROW;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	ଯଦି (flags & WSP_ROOM)
	    needed += p_wiw - wmw1;
	ଯଦି (flags & (WSP_BOT | WSP_ଶୀର୍ଷ))
	{
	    minwidth = frame_minwidth(topframe, NOWIN);
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	ଅନ୍ୟଥା ଯଦି (p_ea)
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    prevfrp = oldwin->w_frame;
	    ପାଇଁ (frp = oldwin->w_frame->fr_parent; frp != ଖାଲି;
							frp = frp->fr_parent)
	    {
		ଯଦି (frp->fr_layout == FR_ROW)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			ଯଦି (frp2 != prevfrp)
			    minwidth += frame_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	ଅନ୍ୟଥା
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    available = oldwin->w_frame->fr_width;
	    needed += minwidth;
	}
	ଯଦି (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    ଯାଅ theend;
	}
	ଯଦି (new_size == 0)
	    new_size = oldwin->w_width / 2;
	ଯଦି (new_size > available - minwidth - 1)
	    new_size = available - minwidth - 1;
	ଯଦି (new_size < wmw1)
	    new_size = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	ଯଦି (oldwin->w_width - new_size - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	ଯଦି (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_width + new_size + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	ଯଦି (!do_equal && p_ea && size == 0 && *p_ead != 'v'
	    && oldwin->w_frame->fr_parent != ଖାଲି)
	    do_equal = TRUE;
    }
    ଅନ୍ୟଥା
    {
	layout = FR_COL;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_HEIGHT(curwin);
	needed = wmh1 + statusline_height(oldwin);
	ଯଦି (flags & WSP_ROOM)
	    needed += p_wh - wmh1;
	ଯଦି (flags & (WSP_BOT | WSP_ଶୀର୍ଷ))
	{
	    minheight = frame_minheight(topframe, NOWIN) + need_status;
	    available = topframe->fr_height;
	    needed += minheight;
	}
	ଅନ୍ୟଥା ଯଦି (p_ea)
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    prevfrp = oldwin->w_frame;
	    ପାଇଁ (frp = oldwin->w_frame->fr_parent; frp != ଖାଲି;
							frp = frp->fr_parent)
	    {
		ଯଦି (frp->fr_layout == FR_COL)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			ଯଦି (frp2 != prevfrp)
			    minheight += frame_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_height;
	    needed += minheight;
	}
	ଅନ୍ୟଥା
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    available = oldwin->w_frame->fr_height;
	    needed += minheight;
	}
	ଯଦି (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    ଯାଅ theend;
	}
	oldwin_height = oldwin->w_height;
	ଯଦି (need_status)
	{
	    oldwin->w_status_height = statusline_height(oldwin);
	    oldwin_height -= oldwin->w_status_height;
	}
	// Estimated stl height for the new window.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_win_stlh = need_status != 0 ? need_status
			: statusline_height(new_wp != ଖାଲି ? new_wp : curwin);
	ଯଦି (new_size == 0)
	    new_size = (oldwin_height - new_win_stlh + 1) / 2;
	ଯଦି (new_size > available - minheight - new_win_stlh)
	    new_size = available - minheight - new_win_stlh;
	ଯଦି (new_size < wmh1)
	    new_size = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	ଯଦି (oldwin_height - new_size - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	ଯଦି (oldwin->w_p_wfh)
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
	ଯଦି (!do_equal && p_ea && size == 0 && *p_ead != 'h'
	    && oldwin->w_frame->fr_parent != ଖାଲି)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    ଯଦି ((flags & WSP_ଶୀର୍ଷ) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_BELOW)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	ଯଦି (new_wp == ଖାଲି)
	    wp = win_alloc(oldwin, FALSE);
	ଅନ୍ୟଥା
	    win_append(oldwin, wp);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (new_wp == ଖାଲି)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	ଅନ୍ୟଥା
	    win_append(oldwin->w_prev, wp);
    }

    ଯଦି (new_wp == ଖାଲି)
    {
	ଯଦି (wp == ଖାଲି)
	    ଯାଅ theend;

	new_frame(wp);
	ଯଦି (wp->w_frame == ଖାଲି)
	{
	    win_free(wp, ଖାଲି);
	    ଯାଅ theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    ଯଦି (to_flatten != ଖାଲି)
	frame_flatten(to_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    ଯଦି (flags & (WSP_ଶୀର୍ଷ | WSP_BOT))
    {
	ଯଦି ((topframe->fr_layout == FR_COL && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ROW && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_child;
	    ଯଦି (flags & WSP_BOT)
		ଯେତେବେଳେ (curfrp->fr_next != ଖାଲି)
		    curfrp = curfrp->fr_next;
	}
	ଅନ୍ୟଥା
	    curfrp = topframe;
	before = (flags & WSP_ଶୀର୍ଷ);
    }
    ଅନ୍ୟଥା
    {
	curfrp = oldwin->w_frame;
	ଯଦି (flags & WSP_BELOW)
	    before = FALSE;
	ଅନ୍ୟଥା ଯଦି (flags & WSP_ABOVE)
	    before = TRUE;
	ଅନ୍ୟଥା ଯଦି (flags & WSP_VERT)
	    before = !p_spr;
	ଅନ୍ୟଥା
	    before = !p_sb;
    }
    ଯଦି (curfrp->fr_parent == ଖାଲି || curfrp->fr_parent->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(frame_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_parent = curfrp;
	frp->fr_next = ଖାଲି;
	frp->fr_prev = ଖାଲି;
	curfrp->fr_child = frp;
	curfrp->fr_win = ଖାଲି;
	curfrp = frp;
	ଯଦି (frp->fr_win != ଖାଲି)
	    oldwin->w_frame = frp;
	ଅନ୍ୟଥା
	    FOR_ALL_FRAMES(frp, frp->fr_child)
		frp->fr_parent = curfrp;
    }

    ଯଦି (new_wp == ଖାଲି)
	frp = wp->w_frame;
    ଅନ୍ୟଥା
	frp = new_wp->w_frame;
    frp->fr_parent = curfrp->fr_parent;

    // Insert the new frame at the right place in the frame list.
    ଯଦି (before)
	frame_ଭର୍ତ୍ତି(curfrp, frp);
    ଅନ୍ୟଥା
	frame_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    ଯଦି (!did_set_fraction)
	set_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    ଯଦି (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	ଯଦି (need_status)
	{
	    win_new_height(oldwin, oldwin->w_height - need_status);
	    oldwin->w_status_height = need_status;
	}
	ଯଦି (flags & (WSP_ଶୀର୍ଷ | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା stl_height = p_ls > 0 ? statusline_height(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_height();
	    win_new_height(wp, curfrp->fr_height - stl_height
							  - WINBAR_HEIGHT(wp));
	    wp->w_status_height = stl_height;
	}
	ଅନ୍ୟଥା
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
	ଯଦି (before)
	    wp->w_vsep_width = 1;
	ଅନ୍ୟଥା
	{
	    wp->w_vsep_width = oldwin->w_vsep_width;
	    oldwin->w_vsep_width = 1;
	}
	ଯଦି (flags & (WSP_ଶୀର୍ଷ | WSP_BOT))
	{
	    ଯଦି (flags & WSP_BOT)
		frame_set_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    frame_new_width(curfrp, curfrp->fr_width
		     - (new_size + ((flags & WSP_ଶୀର୍ଷ) != 0)), flags & WSP_ଶୀର୍ଷ,
								       FALSE);
	}
	ଅନ୍ୟଥା
	    win_new_width(oldwin, oldwin->w_width - (new_size + 1));
	ଯଦି (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_size + 1;
	}
	ଅନ୍ୟଥା		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_width + 1;
	frame_fix_width(oldwin);
	frame_fix_width(wp);
    }
    ଅନ୍ୟଥା
    {
	// width and column of new window is same as current window
	ଯଦି (flags & (WSP_ଶୀର୍ଷ | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_width(wp, topframe->fr_width);
	    wp->w_vsep_width = 0;
	}
	ଅନ୍ୟଥା
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_width(wp, oldwin->w_width);
	    wp->w_vsep_width = oldwin->w_vsep_width;
	}
	frp->fr_width = curfrp->fr_width;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_height(wp, new_size);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_status_height = oldwin->w_status_height;
	ଯଦି (flags & (WSP_ଶୀର୍ଷ | WSP_BOT))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_fr_height = curfrp->fr_height - new_size
							  + WINBAR_HEIGHT(wp);

	    ଯଦି (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_height -= statusline_height(curfrp->fr_win);
	    ଯଦି (flags & WSP_BOT)
		frame_add_statusline(curfrp);
	    frame_new_height(curfrp, new_fr_height, flags & WSP_ଶୀର୍ଷ, FALSE,
									FALSE);
	}
	ଅନ୍ୟଥା
	    win_new_height(oldwin, oldwin_height - (new_size
			+ statusline_height(wp)));
	ଯଦି (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_status_height = statusline_height(wp);
	    oldwin->w_winrow += wp->w_height + statusline_height(wp);
	}
	ଅନ୍ୟଥା		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_HEIGHT(oldwin)
						    + statusline_height(oldwin);
	    wp->w_status_height = old_status_height;
	    ଯଦି (!(flags & WSP_BOT))
		oldwin->w_status_height = statusline_height(oldwin);
	}
	frame_fix_height(wp);
	frame_fix_height(oldwin);
    }

    ଯଦି (flags & (WSP_ଶୀର୍ଷ | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    status_redraw_all();

    ଯଦି (need_status)
    {
	ସନ୍ଦେଶ_row = Rows - 1;
	ସନ୍ଦେଶ_col = sc_col;
	ସନ୍ଦେଶ_clr_eos_force();	// Old command/ruler may still be there
	comp_col();
	ସନ୍ଦେଶ_row = Rows - 1;
	ସନ୍ଦେଶ_col = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    ଯଦି (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    ଅନ୍ୟଥା ଯଦି (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    ଯଦି (flags & WSP_VERT)
    {
	i = p_wiw;
	ଯଦି (size != 0)
	    p_wiw = size;

#ifdef FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	ଯଦି (gui.in_use)
	    gui_init_which_components(ଖାଲି);
#endif
    }
    ଅନ୍ୟଥା
    {
	i = p_wh;
	ଯଦି (size != 0)
	    p_wh = size;
    }

    /*
     * make the new window the current window
     */
    (ଶୂନ୍ୟ)win_enter_ext(wp, (new_wp == ଖାଲି ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    ଯଦି (flags & WSP_VERT)
	p_wiw = i;
    ଅନ୍ୟଥା
	p_wh = i;
    retval = OK;

theend:
    ଯଦି (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ଫେରନ୍ତୁ retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_init(win_T *newp, win_T *oldp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

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
    ଯଦି (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = ଖାଲି;
	newp->w_llist_ref = ଖାଲି;
    }
    ଅନ୍ୟଥା
	copy_loclist_ଗଦା(oldp, newp);
#endif
    newp->w_localdir = (oldp->w_localdir == ଖାଲି)
				    ? ଖାଲି : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == ଖାଲି)
				    ? ଖାଲି : vim_strsave(oldp->w_prevdir);

    ଯଦି (*p_spk != 'c')
    {
	ଯଦି (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_height = oldp->w_height - WINBAR_HEIGHT(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_HEIGHT(oldp);
    }

    // copy tagstack and folds
    ପାଇଁ (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*tag = &newp->w_tagstack[i];
	*tag = oldp->w_tagstack[i];
	ଯଦି (tag->tagname != ଖାଲି)
	    tag->tagname = vim_strsave(tag->tagname);
	ଯଦି (tag->user_data != ଖାଲି)
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
	bool pushed = ଠେଲ_highlight_overrides(newp->w_hl, newp->w_hl_len);
	ଯଦି (newp->w_buffer->b_term != ଖାଲି)
	    term_init_default_colors(newp->w_buffer->b_term);
	ଯଦି (pushed)
	    କାଢ଼_highlight_overrides();
    }
#endif
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_valid_popup(win_T *win UNUSED)
{
#ifdef FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	ଯଦି (wp == win)
	    ଫେରନ୍ତୁ TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ଯଦି (wp == win)
	    ଫେରନ୍ତୁ TRUE;
#endif
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_valid(win_T *win)
{
    win_T	*wp;

    ଯଦି (win == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    FOR_ALL_WINDOWS(wp)
	ଯଦି (wp == win)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_find_by_id(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	ଯଦି (wp->w_id == id)
	    ଫେରନ୍ତୁ wp;
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	ଯଦି (wp->w_id == id)
	    ଫେରନ୍ତୁ wp;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ଯଦି (wp->w_id == id)
	    ଫେରନ୍ତୁ wp;
#endif
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    ଯଦି (win == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ଯଦି (wp == win)
		ଫେରନ୍ତୁ TRUE;
	}
#ifdef FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    ଯଦି (wp == win)
		ଫେରନ୍ତୁ TRUE;
#endif
    }
    ଫେରନ୍ତୁ win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_ଗଣନା(ଶୂନ୍ୟ)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;

    FOR_ALL_WINDOWS(wp)
	++ଗଣନା_2;
    ଫେରନ୍ତୁ ଗଣନା_2;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
make_windows(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vertical)  // split windows vertically if TRUE
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxcount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;

    ଯଦି (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_width + curwin->w_vsep_width
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    ଅନ୍ୟଥା
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_HEIGHT(curwin) + curwin->w_status_height
			- (p_wh - p_wmh)) / (p_wmh + statusline_height(curwin));
    }

    ଯଦି (maxcount < 2)
	maxcount = 2;
    ଯଦି (ଗଣନା_2 > maxcount)
	ଗଣନା_2 = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    ଯଦି (ଗଣନା_2 > 1)
	last_status(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    // todo is number of windows left to create
    ପାଇଁ (todo = ଗଣନା_2 - 1; todo > 0; --todo)
	ଯଦି (vertical)
	{
	    ଯଦି (win_split(curwin->w_width - (curwin->w_width - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (win_split(curwin->w_height - (curwin->w_height - todo
			    * statusline_height(curwin)) / (todo + 1)
			- statusline_height(curwin), WSP_ABOVE) == FAIL)
		ଭାଙ୍ଗ;
	}

    unblock_autocmds();

    // return actual number of windows
    ଫେରନ୍ତୁ (ଗଣନା_2 - todo);
}

/*
 * Exchange current and next window
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_exchange(long Prenum)
{
    frame_T	*frp;
    frame_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		temp;

    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;
    ଯଦି (ONE_WINDOW)	    // just one window
    {
	beep_flush();
	ଫେରନ୍ତୁ;
    }
    ଯଦି (text_or_buf_locked())
    {
	beep_flush();
	ଫେରନ୍ତୁ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * find window to exchange with
     */
    ଯଦି (Prenum)
    {
	frp = curwin->w_frame->fr_parent->fr_child;
	ଯେତେବେଳେ (frp != ଖାଲି && --Prenum > 0)
	    frp = frp->fr_next;
    }
    ଅନ୍ୟଥା ଯଦି (curwin->w_frame->fr_next != ଖାଲି)	// Swap with next
	frp = curwin->w_frame->fr_next;
    ଅନ୍ୟଥା    // Swap last window in row/col with previous
	frp = curwin->w_frame->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    ଯଦି (frp == ଖାଲି || frp->fr_win == ଖାଲି || frp->fr_win == curwin)
	ଫେରନ୍ତୁ;
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
    ଯଦି (wp->w_prev != curwin)
    {
	win_remove(curwin, ଖାଲି);
	frame_remove(curwin->w_frame);
	win_append(wp->w_prev, curwin);
	frame_ଭର୍ତ୍ତି(frp, curwin->w_frame);
    }
    ଯଦି (wp != wp2)
    {
	win_remove(wp, ଖାଲି);
	frame_remove(wp->w_frame);
	win_append(wp2, wp);
	ଯଦି (frp2 == ଖାଲି)
	    frame_ଭର୍ତ୍ତି(wp->w_frame->fr_parent->fr_child, wp->w_frame);
	ଅନ୍ୟଥା
	    frame_append(frp2, wp->w_frame);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    temp = curwin->w_status_height;
    ଯଦି (temp != wp->w_status_height)
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
#ଯଦି defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    ଯଦି (wp->w_buffer != curbuf)
	reset_VIsual_and_resel();
    ଅନ୍ୟଥା ଯଦି (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_rotate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା upwards, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    win_T	*wp1;
    win_T	*wp2;
    frame_T	*frp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    ଯଦି (ONE_WINDOW)		// nothing to do
    {
	beep_flush();
	ଫେରନ୍ତୁ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Check if all frames in this row/col have one window.
    FOR_ALL_FRAMES(frp, curwin->w_frame->fr_parent->fr_child)
	ଯଦି (frp->fr_win == ଖାଲି)
	{
	    emsg(_(e_cannot_rotate_when_another_window_is_split));
	    ଫେରନ୍ତୁ;
	}

    ଯେତେବେଳେ (ଗଣନା_2--)
    {
	ଯଦି (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_frame->fr_parent->fr_child;
	    wp1 = frp->fr_win;
	    win_remove(wp1, ଖାଲି);
	    frame_remove(frp);

	    // find last frame and append removed window/frame after it
	    ପାଇଁ ( ; frp->fr_next != ଖାଲି; frp = frp->fr_next)
		;
	    win_append(frp->fr_win, wp1);
	    frame_append(frp, wp1->w_frame);

	    wp2 = frp->fr_win;		// previously last window
	}
	ଅନ୍ୟଥା			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    ପାଇଁ (frp = curwin->w_frame; frp->fr_next != ଖାଲି;
							   frp = frp->fr_next)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, ଖାଲି);
	    frame_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_parent->fr_child->fr_win->w_prev, wp1);
	    frame_ଭର୍ତ୍ତି(frp->fr_parent->fr_child, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_status_height;
	ଯଦି (n != wp1->w_status_height)
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
#ଯଦି defined(FEAT_STL_OPT)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_splitmove(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height = wp->w_height;
    frame_T	*unflat_altfr;

    ଯଦି (ONE_WINDOW)
	ଫେରନ୍ତୁ OK;	// nothing to do
    ଯଦି (check_split_disallowed(wp) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, ଖାଲି, &unflat_altfr);
    win_remove(wp, ଖାଲି);
    last_status(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    ଯଦି (win_split_ins(size, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	ଫେରନ୍ତୁ FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    ଯଦି (size == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(height, wp);
	ଯଦି (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#ଯଦି defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif
    ଫେରନ୍ତୁ OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    ଶୂନ୍ୟ
win_move_after(win_T *win1, win_T *win2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height;

    // check if the arguments are reasonable
    ଯଦି (win1 == win2)
	ଫେରନ୍ତୁ;

    // check if there is something to do
    ଯଦି (win2->w_next != win1)
    {
	ଯଦି (win1->w_frame->fr_parent != win2->w_frame->fr_parent)
	{
	    iemsg("Trying to move a window into another frame");
	    ଫେରନ୍ତୁ;
	}

	// may need to move the status line/vertical separator of the last
	// window
	ଯଦି (win1 == lastwin)
	{
	    height = win1->w_prev->w_status_height;
	    win1->w_prev->w_status_height = win1->w_status_height;
	    win1->w_status_height = height;
	    ଯଦି (win1->w_prev->w_vsep_width == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_width = 0;
		win1->w_prev->w_frame->fr_width -= 1;
		win1->w_vsep_width = 1;
		win1->w_frame->fr_width += 1;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (win2 == lastwin)
	{
	    height = win1->w_status_height;
	    win1->w_status_height = win2->w_status_height;
	    win2->w_status_height = height;
	    ଯଦି (win1->w_vsep_width == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_width = 1;
		win2->w_frame->fr_width += 1;
		win1->w_vsep_width = 0;
		win1->w_frame->fr_width -= 1;
	    }
	}
	win_remove(win1, ଖାଲି);
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
    ଶୂନ୍ୟ
win_equal(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current,	// do only frame with current window
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    ଯଦି (dir == 0)
	dir = *p_ead;
    win_equal_rec(next_curwin == ଖାଲି ? curwin : next_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_height(),
		      topframe->fr_width, topframe->fr_height);
#ଯଦି defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
    ଯଦି (!is_aucmd_win(next_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_equal_rec(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current,	// do only frame with current window
    frame_T	*topfr,		// frame to set size off
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,		// 'v', 'h' or 'b', see win_equal()
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,		// horizontal position for frame
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row,		// vertical position for frame
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width,		// new width of frame
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height)		// new height of frame
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n, m;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra_sep = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wincount, totwincount = 0;
    frame_T	*fr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		next_curwin_size = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_next_curwin = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hnc;

    ଯଦି (topfr->fr_layout == FR_LEAF)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	ଯଦି (topfr->fr_height != height || topfr->fr_win->w_winrow != row
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
    ଅନ୍ୟଥା ଯଦି (topfr->fr_layout == FR_ROW)
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	ଯଦି (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = frame_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    ଯଦି (col + width >= firstwin->w_wincol + topframe->fr_width)
		extra_sep = 1;
	    ଅନ୍ୟଥା
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
	    ଯଦି (room < 0)
	    {
		next_curwin_size = p_wiw + room;
		room = 0;
	    }
	    ଅନ୍ୟଥା
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    ଯଦି (!frame_fixed_width(fr))
			ଜାରି;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minwidth(fr, NOWIN);
		    new_size = fr->fr_width;
		    ଯଦି (frame_has_win(fr, next_curwin))
		    {
			room += p_wiw - p_wmw;
			next_curwin_size = 0;
			ଯଦି (new_size < p_wiw)
			    new_size = p_wiw;
		    }
		    ଅନ୍ୟଥା
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_next == ଖାଲି
					? extra_sep : 0)) / (p_wmw + 1);
		    room -= new_size - n;
		    ଯଦି (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newwidth = new_size;
		}
		ଯଦି (next_curwin_size == -1)
		{
		    ଯଦି (!has_next_curwin)
			next_curwin_size = 0;
		    ଅନ୍ୟଥା ଯଦି (totwincount > 1
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
		    ଅନ୍ୟଥା
			next_curwin_size = p_wiw;
		}
	    }

	    ଯଦି (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    ଯଦି (fr->fr_next == ଖାଲି)
		// last frame gets all that remains (avoid roundoff error)
		new_size = width;
	    ଅନ୍ୟଥା ଯଦି (dir == 'v')
		new_size = fr->fr_width;
	    ଅନ୍ୟଥା ଯଦି (frame_fixed_width(fr))
	    {
		new_size = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    ଅନ୍ୟଥା
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = frame_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_next == ଖାଲି ? extra_sep : 0))
								/ (p_wmw + 1);
		m = frame_minwidth(fr, next_curwin);
		ଯଦି (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		ଅନ୍ୟଥା
		    hnc = FALSE;
		ଯଦି (hnc)	    // don't count next_curwin
		    --wincount;
		ଯଦି (totwincount == 0)
		    new_size = room;
		ଅନ୍ୟଥା
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		ଯଦି (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wiw - (m - n);
		    ଯଦି (next_curwin_size < 0)
			next_curwin_size = 0;
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		ଅନ୍ୟଥା
		    room -= new_size;
		new_size += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    ଯଦି (!current || dir != 'v' || topfr->fr_parent != ଖାଲି
		    || (new_size != fr->fr_width)
		    || frame_has_win(fr, next_curwin))
		win_equal_rec(next_curwin, current, fr, dir, col, row,
							    new_size, height);
	    col += new_size;
	    width -= new_size;
	    totwincount -= wincount;
	}
    }
    ଅନ୍ୟଥା // topfr->fr_layout == FR_COL
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	ଯଦି (dir != 'h')			// equalize frame heights
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
	    ଯଦି (room < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		next_curwin_size = p_wh + room;
		room = 0;
	    }
	    ଅନ୍ୟଥା
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    ଯଦି (!frame_fixed_height(fr))
			ଜାରି;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minheight(fr, NOWIN);
		    new_size = fr->fr_height;
		    ଯଦି (frame_has_win(fr, next_curwin))
		    {
			room += p_wh - p_wmh;
			next_curwin_size = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			ଯଦି (new_size < n + (p_wh - p_wmh))
			    new_size = n + (p_wh - p_wmh);
		    }
		    ଅନ୍ୟଥା
			// These windows don't use up room.
			totwincount -= frame_wincount_in_height(fr);
		    room -= new_size - n;
		    ଯଦି (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newheight = new_size;
		}
		ଯଦି (next_curwin_size == -1)
		{
		    ଯଦି (!has_next_curwin)
			next_curwin_size = 0;
		    ଅନ୍ୟଥା ଯଦି (totwincount > 1
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
		    ଅନ୍ୟଥା
			next_curwin_size = p_wh;
		}
	    }

	    ଯଦି (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    ଯଦି (fr->fr_next == ଖାଲି)
		// last frame gets all that remains (avoid roundoff error)
		new_size = height;
	    ଅନ୍ୟଥା ଯଦି (dir == 'h')
		new_size = fr->fr_height;
	    ଅନ୍ୟଥା ଯଦି (frame_fixed_height(fr))
	    {
		new_size = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    ଅନ୍ୟଥା
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = frame_minheight(fr, NOWIN);
		wincount = frame_wincount_in_height(fr);
		m = frame_minheight(fr, next_curwin);
		ଯଦି (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		ଅନ୍ୟଥା
		    hnc = FALSE;
		ଯଦି (hnc)	    // don't count next_curwin
		    --wincount;
		ଯଦି (totwincount == 0)
		    new_size = room;
		ଅନ୍ୟଥା
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		ଯଦି (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wh - (m - n);
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		ଅନ୍ୟଥା
		    room -= new_size;
		new_size += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    ଯଦି (!current || dir != 'h' || topfr->fr_parent != ଖାଲି
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

#ଯଦି defined(FEAT_JOB_CHANNEL)
    ଶୂନ୍ୟ
leaving_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    ଯଦି (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	ଫେରନ୍ତୁ;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_buffer->b_prompt_ଭର୍ତ୍ତି = restart_edit;
    ଯଦି (restart_edit != 0 && mode_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    ଯଦି ((State & MODE_ଭର୍ତ୍ତି) && !stop_ଭର୍ତ୍ତି_mode)
    {
	stop_ଭର୍ତ୍ତି_mode = TRUE;
	ଯଦି (win->w_buffer->b_prompt_ଭର୍ତ୍ତି == NUL)
	    win->w_buffer->b_prompt_ଭର୍ତ୍ତି = 'A';
    }
}

    ଶୂନ୍ୟ
entering_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    ଯଦି (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	ଫେରନ୍ତୁ;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    ଯଦି (win->w_buffer->b_prompt_ଭର୍ତ୍ତି != NUL)
	stop_ଭର୍ତ୍ତି_mode = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    ଯଦି ((State & MODE_ଭର୍ତ୍ତି) == 0)
	restart_edit = win->w_buffer->b_prompt_ଭର୍ତ୍ତି;
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
win_init_empty(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid = 0;
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
#ଯଦି defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
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
    ଶୂନ୍ୟ
curwin_init(ଶୂନ୍ୟ)
{
    win_init_empty(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    ଶୂନ୍ୟ
close_windows(
    buf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = tabpage_index(ଖାଲି);

    ++RedrawingDisabled;

    ପାଇଁ (wp = firstwin; wp != ଖାଲି && !ONE_WINDOW; )
    {
	ଯଦି (wp->w_buffer == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_buffer->b_locked > 0))
	{
	    ଯଦି (window_layout_locked(CMD_SIZE))
		ଯାଅ theend;  // Only give one error message.
	    ଯଦି (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		ଭାଙ୍ଗ;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	ଅନ୍ୟଥା
	    wp = wp->w_next;
    }

    // Also check windows in other tab pages.
    ପାଇଁ (tp = first_tabpage; tp != ଖାଲି; tp = nexttp)
    {
	nexttp = tp->tp_next;
	ଯଦି (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		ଯଦି (wp->w_buffer == buf
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0))
		{
		    ଯଦି (window_layout_locked(CMD_SIZE))
			ଯାଅ theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = first_tabpage;
		    ଭାଙ୍ଗ;
		}
    }

theend:
    ଯଦି (RedrawingDisabled > 0)
	--RedrawingDisabled;

    ଯଦି (ଗଣନା_2 != tabpage_index(ଖାଲି))
	apply_autocmds(EVENT_TABCLOSED, ଖାଲି, ଖାଲି, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
last_window(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (one_window() && first_tabpage->tp_next == ଖାଲି);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
one_window(ଶୂନ୍ୟ)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		seen_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	ଯଦି (!is_aucmd_win(wp))
	{
	    ଯଦି (seen_one)
		ଫେରନ୍ତୁ FALSE;
	    seen_one = TRUE;
	}
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
close_last_window_tabpage(
    win_T	*win,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		free_buf,
    tabpage_T   *prev_curtab)
{
    ଯଦି (!ONE_WINDOW)
	ଫେରନ୍ତୁ FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_buffer != ଖାଲି);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    ଯଦି (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    ଯଦି (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, ଖାଲି, ଖାଲି, FALSE, curbuf);
#ଯଦି defined(FEAT_TABPANEL)
    ଯଦି (p_stpl > 0)
	shell_new_columns();
#endif
    apply_autocmds(EVENT_WINENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
    ଯଦି (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_close_buffer(win_T *win, ପୂର୍ଣ୍ଣସଂଖ୍ୟା action, ପୂର୍ଣ୍ଣସଂଖ୍ୟା abort_if_last)
{
#ifdef FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    ଯଦି (win->w_buffer != ଖାଲି)
	reset_synblock(win);
#endif

#ifdef FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    ଯଦି (win->w_buffer != ଖାଲି && bt_quickfix(win->w_buffer)
					&& win->w_buffer->b_nwindows == 1)
	win->w_buffer->b_p_bl = FALSE;
#endif

    // Close the link to the buffer.
    ଯଦି (win->w_buffer != ଖାଲି)
    {
	bufref_T    bufref;

	set_bufref(&bufref, curbuf);
	++win->w_locked;
	close_buffer(win, win->w_buffer, action, abort_if_last, TRUE, TRUE);
	ଯଦି (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	ଯଦି (!bufref_valid(&bufref))
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_close(win_T *win, ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_buf)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		other_buffer = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		close_curwin = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		help_window = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quickfix_window = FALSE;
    tabpage_T   *prev_curtab = curtab;
    frame_T	*win_frame = win->w_frame->fr_parent;
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_diffmode = win->w_p_diff;
#endif
#ifdef MESSAGE_QUEUE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_decrement = FALSE;
#endif

#ଯଦି defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    ଯଦି (may_close_term_popup() == OK)
	ଫେରନ୍ତୁ OK;
#endif
    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (last_window())
    {
	emsg(_(e_cannot_close_last_window));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (window_layout_locked(CMD_close))
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (win_locked(win) || (win->w_buffer != ଖାଲି
					       && win->w_buffer->b_locked > 0))
	ଫେରନ୍ତୁ FAIL; // window is already being closed
    ଯଦି (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_window())
    {
	emsg(_(e_cannot_close_window_only_autocmd_window_would_remain));
	ଫେରନ୍ତୁ FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    ଯଦି (close_last_window_tabpage(win, free_buf, prev_curtab))
	ଫେରନ୍ତୁ FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    ଯଦି (bt_help(win->w_buffer))
	help_window = TRUE;
    ଅନ୍ୟଥା
	clear_snapshot(curtab, SNAP_HELP_IDX);

    ଯଦି (bt_quickfix(win->w_buffer))
	quickfix_window = TRUE;
    ଅନ୍ୟଥା
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    ଯଦି (win == curwin)
    {
#ifdef FEAT_JOB_CHANNEL
	leaving_window(curwin);
#endif
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, ଖାଲି));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	ଯଦି (wp->w_buffer != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_buffer = TRUE;
	    ଯଦି (!win_valid(win))
		ଫେରନ୍ତୁ FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, ଖାଲି, ଖାଲି, FALSE, curbuf);
	    ଯଦି (!win_valid(win))
		ଫେରନ୍ତୁ FAIL;
	    --win->w_locked;
	    ଯଦି (last_window())
		ଫେରନ୍ତୁ FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, ଖାଲି, ଖାଲି, FALSE, curbuf);
	ଯଦି (!win_valid(win))
	    ଫେରନ୍ତୁ FAIL;
	--win->w_locked;
	ଯଦି (last_window())
	    ଫେରନ୍ତୁ FAIL;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	ଯଦି (aborting())
	    ଫେରନ୍ତୁ FAIL;
#endif
    }

#ifdef FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    ଯଦି (gui.in_use)
	out_flush();
#endif

#ifdef FEAT_PROP_POPUP
    ଯଦି (popup_win_closed(win) && !win_valid(win))
	ଫେରନ୍ତୁ FAIL;
#endif

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    ଯଦି (!win_valid_any_tab(win))
	ଫେରନ୍ତୁ OK;

    win_close_buffer(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    ଯଦି (win_valid(win) && win->w_buffer == ଖାଲି
#ଯଦି defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#endif
	    && last_window())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	ଯଦି (curwin->w_buffer == ଖାଲି)
	    curwin->w_buffer = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    ଯଦି (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_buffer == ଖାଲି)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	ଫେରନ୍ତୁ FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    ଯଦି (!win_valid(win) || last_window()
	    || close_last_window_tabpage(win, free_buf, prev_curtab))
	ଫେରନ୍ତୁ FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++split_disallowed;
#ifdef MESSAGE_QUEUE
    ++dont_parse_messages;
#endif

    ଯଦି (win->w_buffer != ଖାଲି)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, ଖାଲି);

    ଯଦି (help_window || quickfix_window)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	ଯଦି (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    ଯଦି (win == curwin)
    {
	curwin = wp;
#ifdef FEAT_QUICKFIX
	ଯଦି (wp->w_p_pvw || bt_quickfix(wp->w_buffer))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    ପାଇଁ (;;)
	    {
		ଯଦି (wp->w_next == ଖାଲି)
		    wp = firstwin;
		ଅନ୍ୟଥା
		    wp = wp->w_next;
		ଯଦି (wp == curwin)
		    ଭାଙ୍ଗ;
		ଯଦି (!wp->w_p_pvw && !bt_quickfix(wp->w_buffer))
		{
		    curwin = wp;
		    ଭାଙ୍ଗ;
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

    ଯଦି (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_frame->fr_parent == win_frame, dir);
    ଅନ୍ୟଥା
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    ଯଦି (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ifdef MESSAGE_QUEUE
	did_decrement =
#ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)
#endif
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	ଯଦି (other_buffer)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
    }

    ଯଦି (ONE_WINDOW && curwin->w_locked && curbuf->b_locked_split
	    && first_tabpage->tp_next != ଖାଲି)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, ଖାଲି, ଖାଲି, FALSE, curbuf);

    --split_disallowed;
#ifdef MESSAGE_QUEUE
    ଯଦି (!did_decrement)
	--dont_parse_messages;
#endif

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    ଯଦି (help_window || quickfix_window)
	restore_snapshot(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ifdef FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    ଯଦି (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    ଯଦି (dwin->w_p_diff)
		++diffcount;
	ଯଦି (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#endif

#ଯଦି defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    ଯଦି (gui.in_use && !win_hasvertsplit())
	gui_init_which_components(ଖାଲି);
#endif

    redraw_all_later(UPD_NOT_VALID);
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ଶୂନ୍ୟ
trigger_winnewpre(ଶୂନ୍ୟ)
{
    window_layout_lock();
    apply_autocmds(EVENT_WINNEWPRE, ଖାଲି, ଖାଲି, FALSE, ଖାଲି);
    window_layout_unlock();
}

    ସ୍ଥିର ଶୂନ୍ୟ
trigger_winclosed(win_T *win)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    ଯଦି (recursive)
	ଫେରନ୍ତୁ;
    recursive = TRUE;
    vim_snprintf((ଅକ୍ଷର *)winid, ଆକାର(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_buffer);
    recursive = FALSE;
}

    ଶୂନ୍ୟ
trigger_tabclosedpre(tabpage_T *tp)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    ଯଦି (!has_tabclosedpre() || recursive)
	ଫେରନ୍ତୁ;

    ଯଦି (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    window_layout_lock();
    apply_autocmds(EVENT_TABCLOSEDPRE, ଖାଲି, ଖାଲି, FALSE, ଖାଲି);
    window_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    ଯଦି (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    ଅନ୍ୟଥା
	// fall back to the first tabpage
	goto_tabpage_tp(first_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    ଶୂନ୍ୟ
snapshot_windows_scroll_size(ଶୂନ୍ୟ)
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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_initial_scroll_size_snapshot = FALSE;

    ଶୂନ୍ୟ
may_make_initial_scroll_size_snapshot(ଶୂନ୍ୟ)
{
    ଯଦି (!did_initial_scroll_size_snapshot)
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
    ସ୍ଥିର dict_T *
make_win_info_dict(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା height,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା topline,
# ifdef FEAT_DIFF
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା topfill,
# endif
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା leftcol,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା skipcol)
{
    dict_T *d = dict_alloc();
    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    ଯେତେବେଳେ (1)
    {
	typval_T tv;
	tv.v_lock = 0;
	tv.v_type = VAR_ସଂଖ୍ୟା;

	tv.vval.v_ସଂଖ୍ୟା = width;
	ଯଦି (dict_add_tv(d, "width", &tv) == FAIL)
	    ଭାଙ୍ଗ;
	tv.vval.v_ସଂଖ୍ୟା = height;
	ଯଦି (dict_add_tv(d, "height", &tv) == FAIL)
	    ଭାଙ୍ଗ;
	tv.vval.v_ସଂଖ୍ୟା = topline;
	ଯଦି (dict_add_tv(d, "topline", &tv) == FAIL)
	    ଭାଙ୍ଗ;
# ifdef FEAT_DIFF
	tv.vval.v_ସଂଖ୍ୟା = topfill;
# ଅନ୍ୟଥା
	tv.vval.v_ସଂଖ୍ୟା = 0;
# endif
	ଯଦି (dict_add_tv(d, "topfill", &tv) == FAIL)
	    ଭାଙ୍ଗ;
	tv.vval.v_ସଂଖ୍ୟା = leftcol;
	ଯଦି (dict_add_tv(d, "leftcol", &tv) == FAIL)
	    ଭାଙ୍ଗ;
	tv.vval.v_ସଂଖ୍ୟା = skipcol;
	ଯଦି (dict_add_tv(d, "skipcol", &tv) == FAIL)
	    ଭାଙ୍ଗ;
	ଫେରନ୍ତୁ d;
    }
    dict_unref(d);
    ଫେରନ୍ତୁ ଖାଲି;
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
    ସ୍ଥିର ଶୂନ୍ୟ
check_window_scroll_resize(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*size_ଗଣନା,
	win_T	**first_scroll_win,
	win_T	**first_size_win,
	list_T	*winlist UNUSED,
	dict_T	*v_event UNUSED)
{
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା listidx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tot_width = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tot_height = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tot_topline = 0;
# ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tot_topfill = 0;
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tot_leftcol = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tot_skipcol = 0;
#endif

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size_changed = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_last_width != wp->w_width
			    || wp->w_last_height != wp->w_height);
	ଯଦି (size_changed)
	{
#ifdef FEAT_EVAL
	    ଯଦି (winlist != ଖାଲି)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_lock = 0;
		tv.v_type = VAR_ସଂଖ୍ୟା;
		tv.vval.v_ସଂଖ୍ୟା = wp->w_id;
		list_set_item(winlist, listidx++, &tv);
	    }
	    ଅନ୍ୟଥା
#endif
		ଯଦି (size_ଗଣନା != ଖାଲି)
	    {
		++*size_ଗଣନା;
		ଯଦି (*first_size_win == ଖାଲି)
		    *first_size_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		ଯଦି (*first_scroll_win == ଖାଲି && !ignore_scroll)
		    *first_scroll_win = wp;
	    }
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା scroll_changed = !ignore_scroll
				&& (wp->w_last_topline != wp->w_topline
#ifdef FEAT_DIFF
				|| wp->w_last_topfill != wp->w_topfill
#endif
				|| wp->w_last_leftcol != wp->w_leftcol
				|| wp->w_last_skipcol != wp->w_skipcol);
	ଯଦି (scroll_changed
	    && first_scroll_win != ଖାଲି && *first_scroll_win == ଖାଲି)
	    *first_scroll_win = wp;

#ifdef FEAT_EVAL
	ଯଦି ((size_changed || scroll_changed) && v_event != ଖାଲି)
	{
	    // Add info about this window to the v:event dictionary.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = wp->w_width - wp->w_last_width;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା height = wp->w_height - wp->w_last_height;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା topline = wp->w_topline - wp->w_last_topline;
# ifdef FEAT_DIFF
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା topfill = wp->w_topfill - wp->w_last_topfill;
# endif
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା leftcol = wp->w_leftcol - wp->w_last_leftcol;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା skipcol = wp->w_skipcol - wp->w_last_skipcol;
	    dict_T *d = make_win_info_dict(width, height, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    ଯଦି (d == ଖାଲି)
		ଭାଙ୍ଗ;
	    ଅକ୍ଷର winid[NUMBUFLEN];
	    vim_snprintf(winid, ଆକାର(winid), "%d", wp->w_id);
	    ଯଦି (dict_add_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		ଭାଙ୍ଗ;
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
    ଯଦି (v_event != ଖାଲି)
    {
	dict_T *alldict = make_win_info_dict(tot_width, tot_height, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	ଯଦି (alldict != ଖାଲି)
	{
	    ଯଦି (dict_add_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    ଅନ୍ୟଥା
		--alldict->dv_refcount;
	}
    }
#endif
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    ଶୂନ୍ୟ
may_trigger_win_scrolled_resized(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    recursive = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    do_resize = has_winresized();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    ଯଦି (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_size_snapshot)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା size_ଗଣନା = 0;
    win_T *first_scroll_win = ଖାଲି, *first_size_win = ଖାଲି;
    check_window_scroll_resize(&size_ଗଣନା, &first_scroll_win, &first_size_win,
								    ଖାଲି, ଖାଲି);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା trigger_resize = do_resize && size_ଗଣନା > 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା trigger_scroll = do_scroll && first_scroll_win != ଖାଲି;
    ଯଦି (!trigger_resize && !trigger_scroll)
	ଫେରନ୍ତୁ;  // no relevant changes
#ifdef FEAT_EVAL
    list_T *windows_list = ଖାଲି;
    ଯଦି (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(size_ଗଣନା);
	ଯଦି (windows_list != ଖାଲି)
	    check_window_scroll_resize(ଖାଲି, ଖାଲି, ଖାଲି, windows_list, ଖାଲି);
    }

    dict_T *scroll_dict = ଖାଲି;
    ଯଦି (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	ଯଦି (scroll_dict != ଖାଲି)
	{
	    scroll_dict->dv_refcount = 1;
	    check_window_scroll_resize(ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, scroll_dict);
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
    ଯଦି (trigger_resize
#ifdef FEAT_EVAL
	    && windows_list != ଖାଲି
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	ଯଦି (dict_add_list(v_event, "windows", windows_list) == OK)
	{
	    dict_set_ବସ୍ତୁଗୁଡ଼ିକ_ro(v_event);
#endif
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((ଅକ୍ଷର *)winid, ଆକାର(winid), "%d",
							 first_size_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     first_size_win->w_buffer);
#ifdef FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#endif
    }

    ଯଦି (trigger_scroll
#ifdef FEAT_EVAL
	    && scroll_dict != ଖାଲି
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", ଖାଲି);
	dict_set_ବସ୍ତୁଗୁଡ଼ିକ_ro(v_event);
	dict_unref(scroll_dict);
#endif
	char_u winid[NUMBUFLEN];
	vim_snprintf((ଅକ୍ଷର *)winid, ଆକାର(winid), "%d",
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
    ଶୂନ୍ୟ
win_close_othertab(win_T *win, ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_buf, tabpage_T *tp)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;
    tabpage_T   *ptp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    ଯଦି (window_layout_locked(CMD_SIZE))
	ଫେରନ୍ତୁ;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    ଯଦି (win_locked(win) || (win->w_buffer != ଖାଲି
					       && win->w_buffer->b_locked > 0))
	ଫେରନ୍ତୁ; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    ଯଦି (win->w_buffer != ଖାଲି)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	ଯଦି (!win_valid_any_tab(win))
	    ଫେରନ୍ତୁ;
    }

    ଯଦି (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	ଯଦି (!win_valid_any_tab(win))
	    ଫେରନ୍ତୁ;
    }

    ଯଦି (win->w_buffer != ଖାଲି)
	// Close the link to the buffer.
	close_buffer(win, win->w_buffer, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    ପାଇଁ (ptp = first_tabpage; ptp != ଖାଲି && ptp != tp; ptp = ptp->tp_next)
	;
    ଯଦି (ptp == ଖାଲି || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	ଯଦି (win_valid_any_tab(win) && win->w_buffer == ଖାଲି)
	{
	    win->w_buffer = firstbuf;
	    ++firstbuf->b_nwindows;
	    ଯଦି (win == curwin)
		curbuf = curwin->w_buffer;
	    win_init_empty(win);
	}
	ଫେରନ୍ତୁ;
    }

    // Autocommands may have closed the window already.
    ପାଇଁ (wp = tp->tp_firstwin; wp != ଖାଲି && wp != win; wp = wp->w_next)
	;
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    // When closing the last window in a tab page remove the tab page.
    ଯଦି (tp->tp_firstwin == tp->tp_lastwin)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	h = tabline_height();

	ଯଦି (tp == first_tabpage)
	    first_tabpage = tp->tp_next;
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (ptp = first_tabpage; ptp != ଖାଲି && ptp->tp_next != tp;
							   ptp = ptp->tp_next)
		;
	    ଯଦି (ptp == ଖାଲି)
	    {
		internal_error("win_close_othertab()");
		ଫେରନ୍ତୁ;
	    }
	    ptp->tp_next = tp->tp_next;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	ଯଦି (h != tabline_height())
	    shell_new_rows();
    }

    ଯଦି (win->w_buffer != ଖାଲି)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    ଯଦି (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    ସ୍ଥିର win_T *
win_free_mem(
    win_T	*win,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == ଖାଲି ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_frame;
    wp = winframe_remove(win, dirp, tp, ଖାଲି);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    ଯଦି (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    ଫେରନ୍ତୁ wp;
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
win_free_all(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = ଖାଲି;
    cmdwin_win = ଖାଲି;

    ଯେତେବେଳେ (first_tabpage->tp_next != ଖାଲି)
	tabpage_close(TRUE);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < AUCMD_WIN_ଗଣନା; ++i)
	ଯଦି (aucmd_win[i].auc_win != ଖାଲି)
	{
	    (ଶୂନ୍ୟ)win_free_mem(aucmd_win[i].auc_win, &dummy, ଖାଲି);
	    aucmd_win[i].auc_win = ଖାଲି;
	}

    ଯେତେବେଳେ (firstwin != ଖାଲି)
	(ଶୂନ୍ୟ)win_free_mem(firstwin, &dummy, ଖାଲି);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = ଖାଲି;
}
#endif

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    frame_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    frame_T	*frp, *frp2, *frp3;
    frame_T	*frp_close = win->w_frame;
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row, col;

    /*
     * If there is only one window there is nothing to remove.
     */
    ଯଦି (tp == ଖାଲି ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	ଫେରନ୍ତୁ ଖାଲି;

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
    ଯଦି (win->w_vsep_width == 0 && frp_close->fr_parent->fr_layout == FR_ROW &&
	    frp_close->fr_prev != ଖାଲି)
	frame_set_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    frame_remove(frp_close);

    ଯଦି (frp_close->fr_parent->fr_layout == FR_COL)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	ଯଦି (frp2->fr_win != ଖାଲି && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    ଯେତେବେଳେ (frp != ଖାଲି || frp3 != ଖାଲି)
	    {
		ଯଦି (frp != ଖାଲି)
		{
		    ଯଦି (!frame_fixed_height(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			ଭାଙ୍ଗ;
		    }
		    frp = frp->fr_prev;
		}
		ଯଦି (frp3 != ଖାଲି)
		{
		    ଯଦି (frp3->fr_win != ଖାଲି && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			ଭାଙ୍ଗ;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_height(frp2, frp2->fr_height + frp_close->fr_height,
			    frp2 == frp_close->fr_next, !frame_fixed_height(frp2), FALSE);
	*dirp = 'v';
    }
    ଅନ୍ୟଥା
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	ଯଦି (frp2->fr_win != ଖାଲି && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    ଯେତେବେଳେ (frp != ଖାଲି || frp3 != ଖାଲି)
	    {
		ଯଦି (frp != ଖାଲି)
		{
		    ଯଦି (!frame_fixed_width(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			ଭାଙ୍ଗ;
		    }
		    frp = frp->fr_prev;
		}
		ଯଦି (frp3 != ଖାଲି)
		{
		    ଯଦି (frp3->fr_win != ଖାଲି && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			ଭାଙ୍ଗ;
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
    ଯଦି (frp2 != frp_close->fr_prev)
	frame_comp_pos(frp_close->fr_parent, &row, &col);

    ଯଦି (unflat_altfr == ଖାଲି)
	frame_flatten(frp2);
    ଅନ୍ୟଥା
	*unflat_altfr = frp2;

    frame_locked--;

    ଫେରନ୍ତୁ wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
frame_flatten(frame_T *frp)
{
    frame_T	*frp2, *frp3;

    ଯଦି (frp->fr_next != ଖାଲି || frp->fr_prev != ଖାଲି)
	ଫେରନ୍ତୁ;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_parent->fr_layout = frp->fr_layout;
    frp->fr_parent->fr_child = frp->fr_child;
    FOR_ALL_FRAMES(frp2, frp->fr_child)
	frp2->fr_parent = frp->fr_parent;
    frp->fr_parent->fr_win = frp->fr_win;
    ଯଦି (frp->fr_win != ଖାଲି)
	frp->fr_win->w_frame = frp->fr_parent;
    frp2 = frp->fr_parent;
    ଯଦି (topframe->fr_child == frp)
	topframe->fr_child = frp2;
    vim_free(frp);

    frp = frp2->fr_parent;
    ଯଦି (frp != ଖାଲି && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	ଯଦି (frp->fr_child == frp2)
	    frp->fr_child = frp2->fr_child;
	frp2->fr_child->fr_prev = frp2->fr_prev;
	ଯଦି (frp2->fr_prev != ଖାଲି)
	    frp2->fr_prev->fr_next = frp2->fr_child;
	ପାଇଁ (frp3 = frp2->fr_child; ; frp3 = frp3->fr_next)
	{
	    frp3->fr_parent = frp;
	    ଯଦି (frp3->fr_next == ଖାଲି)
	    {
		frp3->fr_next = frp2->fr_next;
		ଯଦି (frp2->fr_next != ଖାଲି)
		    frp2->fr_next->fr_prev = frp3;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (topframe->fr_child == frp2)
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
    ସ୍ଥିର ଶୂନ୍ୟ
winframe_restore(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, frame_T *unflat_altfr)
{
    frame_T	*frp = wp->w_frame;

    // Put "wp"'s frame back where it was.
    ଯଦି (frp->fr_prev != ଖାଲି)
	frame_append(frp->fr_prev, frp);
    ଅନ୍ୟଥା
	frame_ଭର୍ତ୍ତି(frp->fr_next, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    ଯଦି (wp->w_vsep_width == 0
	    && frp->fr_parent->fr_layout == FR_ROW && frp->fr_prev != ଖାଲି)
	frame_set_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    ଯଦି (wp->w_status_height == 0
	    && frp->fr_parent->fr_layout == FR_COL && frp->fr_prev != ଖାଲି)
	frame_add_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    ଯଦି (dir == 'v')
    {
	frame_new_height(unflat_altfr, unflat_altfr->fr_height - frp->fr_height,
		unflat_altfr == frp->fr_next, FALSE, FALSE);
    }
    ଅନ୍ୟଥା ଯଦି (dir == 'h')
    {
	frame_new_width(unflat_altfr, unflat_altfr->fr_width - frp->fr_width,
		unflat_altfr == frp->fr_next, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    ଯଦି (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_parent);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row = topleft->w_winrow;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col = topleft->w_wincol;

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
    ସ୍ଥିର frame_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    frame_T	*other_fr, *target_fr;

    ଯଦି (tp == ଖାଲି ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	ଫେରନ୍ତୁ alt_tabpage()->tp_curwin->w_frame;

    frp = win->w_frame;

    ଯଦି (frp->fr_prev == ଖାଲି)
	ଫେରନ୍ତୁ frp->fr_next;
    ଯଦି (frp->fr_next == ଖାଲି)
	ଫେରନ୍ତୁ frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    // window
    target_fr = frp->fr_next;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    ଯଦି (frp->fr_parent != ଖାଲି && frp->fr_parent->fr_layout == FR_COL && p_sb)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    ଯଦି (frp->fr_parent != ଖାଲି && frp->fr_parent->fr_layout == FR_ROW && p_spr)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    ଯଦି (frp->fr_parent != ଖାଲି && frp->fr_parent->fr_layout == FR_ROW)
    {
	ଯଦି (frame_fixed_width(target_fr) && !frame_fixed_width(other_fr))
	    target_fr = other_fr;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (frame_fixed_height(target_fr) && !frame_fixed_height(other_fr))
	    target_fr = other_fr;
    }

    ଫେରନ୍ତୁ target_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    ସ୍ଥିର tabpage_T *
alt_tabpage(ଶୂନ୍ୟ)
{
    tabpage_T	*tp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forward;

    // Use the last accessed tab page, if possible.
    ଯଦି ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	ଫେରନ୍ତୁ lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_next != ଖାଲି &&
	    ((tcl_flags & TCL_ବାମ) == 0 || curtab == first_tabpage);

    ଯଦି (forward)
	tp = curtab->tp_next;
    ଅନ୍ୟଥା
	// Use the previous tab page.
	ପାଇଁ (tp = first_tabpage; tp->tp_next != curtab; tp = tp->tp_next)
	    ;

    ଫେରନ୍ତୁ tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    ସ୍ଥିର win_T *
frame2win(frame_T *frp)
{
    ଯେତେବେଳେ (frp->fr_win == ଖାଲି)
	frp = frp->fr_child;
    ଫେରନ୍ତୁ frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
frame_has_win(frame_T *frp, win_T *wp)
{
    frame_T	*p;

    ଯଦି (frp->fr_layout == FR_LEAF)
	ଫେରନ୍ତୁ frp->fr_win == wp;

    FOR_ALL_FRAMES(p, frp->fr_child)
	ଯଦି (frame_has_win(p, wp))
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_set_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
frame_new_height(
    frame_T	*topfrp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		topfirst,	// resize topmost contained frame first
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		set_ch)		// set 'cmdheight' to resize topframe
{
    frame_T	*frp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra_ଧାଡ଼ିଗୁଡ଼ିକ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		h;

    ଯଦି (topfrp->fr_parent == ଖାଲି && set_ch)
    {
	// topframe: update the command line height, with side effects.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_ch = MAX(min_set_ch, p_ch + topfrp->fr_height - height);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ch = min_set_ch;
	ଯଦି (new_ch != p_ch)
	    set_option_ମୂଲ୍ୟ((char_u *)"cmdheight", new_ch, ଖାଲି, 0);
	min_set_ch = save_ch;
	height = MIN(height, ROWS_AVAIL);
    }
    ଯଦି (topfrp->fr_win != ଖାଲି)
    {
	// Simple case: just one window.
	win_new_height(topfrp->fr_win,
				    height - topfrp->fr_win->w_status_height
					      - WINBAR_HEIGHT(topfrp->fr_win));
    }
    ଅନ୍ୟଥା ଯଦି (topfrp->fr_layout == FR_ROW)
    {
	କର
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_height(frp, height, topfirst, wfh, set_ch);
		ଯଦି (frp->fr_height > height)
		{
		    // Could not fit the windows, make the whole row higher.
		    height = frp->fr_height;
		    ଭାଙ୍ଗ;
		}
	    }
	}
	ଯେତେବେଳେ (frp != ଖାଲି);
    }
    ଅନ୍ୟଥା    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_child;
	ଯଦି (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    ଯେତେବେଳେ (frame_fixed_height(frp))
	    {
		frp = frp->fr_next;
		ଯଦି (frp == ଖାଲି)
		    ଫେରନ୍ତୁ;	    // no frame without 'wfh', give up
	    }
	ଯଦି (!topfirst)
	{
	    // Find the bottom frame of this column
	    ଯେତେବେଳେ (frp->fr_next != ଖାଲି)
		frp = frp->fr_next;
	    ଯଦି (wfh)
		// Advance back for frames with one window with 'wfh' set.
		ଯେତେବେଳେ (frame_fixed_height(frp))
		    frp = frp->fr_prev;
	}

	extra_ଧାଡ଼ିଗୁଡ଼ିକ = height - topfrp->fr_height;
	ଯଦି (extra_ଧାଡ଼ିଗୁଡ଼ିକ < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    ଯେତେବେଳେ (frp != ଖାଲି)
	    {
		h = frame_minheight(frp, ଖାଲି);
		ଯଦି (frp->fr_height + extra_ଧାଡ଼ିଗୁଡ଼ିକ < h)
		{
		    extra_ଧାଡ଼ିଗୁଡ଼ିକ += frp->fr_height - h;
		    frame_new_height(frp, h, topfirst, wfh, set_ch);
		}
		ଅନ୍ୟଥା
		{
		    frame_new_height(frp, frp->fr_height + extra_ଧାଡ଼ିଗୁଡ଼ିକ,
							topfirst, wfh, set_ch);
		    ଭାଙ୍ଗ;
		}
		ଯଦି (topfirst)
		{
		    କର
			frp = frp->fr_next;
		    ଯେତେବେଳେ (wfh && frp != ଖାଲି && frame_fixed_height(frp));
		}
		ଅନ୍ୟଥା
		{
		    କର
			frp = frp->fr_prev;
		    ଯେତେବେଳେ (wfh && frp != ଖାଲି && frame_fixed_height(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		ଯଦି (frp == ଖାଲି)
		    height -= extra_ଧାଡ଼ିଗୁଡ଼ିକ;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (extra_ଧାଡ଼ିଗୁଡ଼ିକ > 0)
	{
	    // increase height of bottom or top frame
	    frame_new_height(frp, frp->fr_height + extra_ଧାଡ଼ିଗୁଡ଼ିକ, topfirst, wfh,
									set_ch);
	}
    }
    topfrp->fr_height = height;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
frame_fixed_height(frame_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    ଯଦି (frp->fr_win != ଖାଲି)
	ଫେରନ୍ତୁ frp->fr_win->w_p_wfh;

    ଯଦି (frp->fr_layout == FR_ROW)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    ଯଦି (frame_fixed_height(frp))
		ଫେରନ୍ତୁ TRUE;
	ଫେରନ୍ତୁ FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	ଯଦି (!frame_fixed_height(frp))
	    ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
frame_fixed_width(frame_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    ଯଦି (frp->fr_win != ଖାଲି)
	ଫେରନ୍ତୁ frp->fr_win->w_p_wfw;

    ଯଦି (frp->fr_layout == FR_COL)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    ଯଦି (frame_fixed_width(frp))
		ଫେରନ୍ତୁ TRUE;
	ଫେରନ୍ତୁ FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	ଯଦି (!frame_fixed_width(frp))
	    ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    ସ୍ଥିର ଶୂନ୍ୟ
frame_add_statusline(frame_T *frp)
{
    win_T	*wp;

    ଯଦି (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	wp->w_status_height = statusline_height(wp);
    }
    ଅନ୍ୟଥା ଯଦି (frp->fr_layout == FR_ROW)
    {
	// Handle all the frames in the row.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_statusline(frp);
    }
    ଅନ୍ୟଥା // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	ପାଇଁ (frp = frp->fr_child; frp->fr_next != ଖାଲି; frp = frp->fr_next)
	    ;
	frame_add_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
frame_new_width(
    frame_T	*topfrp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		leftfirst,	// resize leftmost contained frame first
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    frame_T	*frp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra_cols;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w;
    win_T	*wp;

    ଯଦି (topfrp->fr_layout == FR_LEAF)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	ପାଇଁ (frp = topfrp; frp->fr_parent != ଖାଲି; frp = frp->fr_parent)
	    ଯଦି (frp->fr_parent->fr_layout == FR_ROW && frp->fr_next != ଖାଲି)
		ଭାଙ୍ଗ;
	ଯଦି (frp->fr_parent == ଖାଲି)
	    wp->w_vsep_width = 0;
	win_new_width(wp, width - wp->w_vsep_width);
    }
    ଅନ୍ୟଥା ଯଦି (topfrp->fr_layout == FR_COL)
    {
	କର
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_width(frp, width, leftfirst, wfw);
		ଯଦି (frp->fr_width > width)
		{
		    // Could not fit the windows, make whole column wider.
		    width = frp->fr_width;
		    ଭାଙ୍ଗ;
		}
	    }
	} ଯେତେବେଳେ (frp != ଖାଲି);
    }
    ଅନ୍ୟଥା    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_child;
	ଯଦି (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    ଯେତେବେଳେ (frame_fixed_width(frp))
	    {
		frp = frp->fr_next;
		ଯଦି (frp == ଖାଲି)
		    ଫେରନ୍ତୁ;	    // no frame without 'wfw', give up
	    }
	ଯଦି (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    ଯେତେବେଳେ (frp->fr_next != ଖାଲି)
		frp = frp->fr_next;
	    ଯଦି (wfw)
		// Advance back for frames with one window with 'wfw' set.
		ଯେତେବେଳେ (frame_fixed_width(frp))
		    frp = frp->fr_prev;
	}

	extra_cols = width - topfrp->fr_width;
	ଯଦି (extra_cols < 0)
	{
	    // reduce frame width, rightmost frame first
	    ଯେତେବେଳେ (frp != ଖାଲି)
	    {
		w = frame_minwidth(frp, ଖାଲି);
		ଯଦି (frp->fr_width + extra_cols < w)
		{
		    extra_cols += frp->fr_width - w;
		    frame_new_width(frp, w, leftfirst, wfw);
		}
		ଅନ୍ୟଥା
		{
		    frame_new_width(frp, frp->fr_width + extra_cols,
							      leftfirst, wfw);
		    ଭାଙ୍ଗ;
		}
		ଯଦି (leftfirst)
		{
		    କର
			frp = frp->fr_next;
		    ଯେତେବେଳେ (wfw && frp != ଖାଲି && frame_fixed_width(frp));
		}
		ଅନ୍ୟଥା
		{
		    କର
			frp = frp->fr_prev;
		    ଯେତେବେଳେ (wfw && frp != ଖାଲି && frame_fixed_width(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		ଯଦି (frp == ଖାଲି)
		    width -= extra_cols;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (extra_cols > 0)
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
    ସ୍ଥିର ଶୂନ୍ୟ
frame_set_vsep(frame_T *frp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା add)
{
    win_T	*wp;

    ଯଦି (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	ଯଦି (add && wp->w_vsep_width == 0)
	{
	    ଯଦି (wp->w_width > 0)	// don't make it negative
		win_new_width(wp, wp->w_width - 1);
	    wp->w_vsep_width = 1;
	}
	ଅନ୍ୟଥା ଯଦି (!add && wp->w_vsep_width == 1)
	{
	    win_new_width(wp, wp->w_width + 1);
	    wp->w_vsep_width = 0;
	}
    }
    ଅନ୍ୟଥା ଯଦି (frp->fr_layout == FR_COL)
    {
	// Handle all the frames in the column.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_set_vsep(frp, add);
    }
    ଅନ୍ୟଥା // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_child;
	ଯେତେବେଳେ (frp->fr_next != ଖାଲି)
	    frp = frp->fr_next;
	frame_set_vsep(frp, add);
    }
}

/*
 * Set frame width from the window it contains.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
frame_fix_width(win_T *wp)
{
    wp->w_frame->fr_width = wp->w_width + wp->w_vsep_width;
}

/*
 * Set frame height from the window it contains.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
frame_minheight(frame_T *topfrp, win_T *next_curwin)
{
    frame_T	*frp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		m;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    ଯଦି (topfrp->fr_win != ଖାଲି)
    {
	ଯଦି (topfrp->fr_win == next_curwin)
	    m = p_wh + topfrp->fr_win->w_status_height;
	ଅନ୍ୟଥା
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_status_height;
	    ଯଦି (topfrp->fr_win == curwin && next_curwin == ଖାଲି)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		ଯଦି (p_wmh == 0)
		    ++m;
		m += WINBAR_HEIGHT(curwin);
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (topfrp->fr_layout == FR_ROW)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minheight(frp, next_curwin);
	    ଯଦି (n > m)
		m = n;
	}
    }
    ଅନ୍ୟଥା
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minheight(frp, next_curwin);
    }

    ଫେରନ୍ତୁ m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
frame_wincount_in_height(frame_T *topfrp)
{
    frame_T	*frp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    ଯଦି (topfrp->fr_win != ଖାଲି)
	c = 1;
    ଅନ୍ୟଥା ଯଦି (topfrp->fr_layout == FR_ROW)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_wincount_in_height(frp);
	    ଯଦି (n > c)
		c = n;
	}
    }
    ଅନ୍ୟଥା
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    c += frame_wincount_in_height(frp);
    }

    ଫେରନ୍ତୁ c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
frame_minwidth(
    frame_T	*topfrp,
    win_T	*next_curwin)	// use p_wh and p_wiw for next_curwin
{
    frame_T	*frp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		m, n;

    ଯଦି (topfrp->fr_win != ଖାଲି)
    {
	ଯଦି (topfrp->fr_win == next_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_width;
	ଅନ୍ୟଥା
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_width;
	    // Current window is minimal one column wide
	    ଯଦି (p_wmw == 0 && topfrp->fr_win == curwin && next_curwin == ଖାଲି)
		++m;
	}
    }
    ଅନ୍ୟଥା ଯଦି (topfrp->fr_layout == FR_COL)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minwidth(frp, next_curwin);
	    ଯଦି (n > m)
		m = n;
	}
    }
    ଅନ୍ୟଥା
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minwidth(frp, next_curwin);
    }

    ଫେରନ୍ତୁ m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    ଶୂନ୍ୟ
close_others(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		message,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

    ଯଦି (one_window())
    {
	ଯଦି (message && !autocmd_busy)
	    ସନ୍ଦେଶ(_(m_onlyone));
	ଫେରନ୍ତୁ;
    }

    // Be very careful here: autocommands may change the window layout.
    ପାଇଁ (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_next;

	// autocommands messed this one up
	ଯଦି (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
	}

	ଯଦି (wp == curwin)		// don't close current window
	    ଜାରି;

	// autoccommands messed this one up
	ଯଦି (!buf_valid(wp->w_buffer) && win_valid(wp))
	{
	    wp->w_buffer = ଖାଲି;
	    win_close(wp, 0);
	    ଜାରି;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_buffer, forceit);
	ଯଦି (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    ଜାରି;
	}
	ଯଦି (!r)
	{
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ଯଦି (message && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_changed(wp->w_buffer, FALSE);
		ଯଦି (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    ଜାରି;
		}
	    }
	    ଯଦି (bufIsChanged(wp->w_buffer))
#endif
		ଜାରି;
	}
	win_close(wp, !buf_hide(wp->w_buffer) && !bufIsChanged(wp->w_buffer));
    }

    ଯଦି (message && !ONE_WINDOW)
	emsg(_(e_other_window_contains_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    ଶୂନ୍ୟ
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା command_frame_height = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    ଶୂନ୍ୟ
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_alloc_first(ଶୂନ୍ୟ)
{
    ଯଦି (win_alloc_firstwin(ଖାଲି) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    first_tabpage = alloc_tabpage();
    ଯଦି (first_tabpage == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    curtab = first_tabpage;
    unuse_tabpage(first_tabpage);

    ଫେରନ୍ତୁ OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(ଶୂନ୍ୟ)
{
    win_T *wp;

    wp = win_alloc(ଖାଲି, TRUE);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_frame(wp);
    ଫେରନ୍ତୁ wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    ଶୂନ୍ୟ
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(ଖାଲି, FALSE);
    ଯଦି (curwin == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (oldwin == ଖାଲି)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(ଖାଲି, ଖାଲି, 1L, BLN_LISTED);
	ଯଦି (curwin == ଖାଲି || curbuf == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	curwin->w_buffer = curbuf;
#ifdef FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#endif
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    ଅନ୍ୟଥା
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_frame(curwin);
    ଯଦି (curwin->w_frame == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    topframe = curwin->w_frame;
    topframe->fr_width = COLUMNS_WITHOUT_TPL();
    topframe->fr_height = Rows - p_ch;

    ଫେରନ୍ତୁ OK;
}

/*
 * Create a frame for window "wp".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
new_frame(win_T *wp)
{
    frame_T *frp = ALLOC_CLEAR_ONE(frame_T);

    wp->w_frame = frp;
    ଯଦି (frp == ଖାଲି)
	ଫେରନ୍ତୁ;
    frp->fr_layout = FR_LEAF;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    ଶୂନ୍ୟ
win_init_size(ଶୂନ୍ୟ)
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
    ସ୍ଥିର tabpage_T *
alloc_tabpage(ଶୂନ୍ୟ)
{
    tabpage_T	*tp;
#ifdef FEAT_GUI
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
#endif


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    ଯଦି (tp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

#ifdef FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    ଯଦି (tp->tp_vars == ଖାଲି)
    {
	vim_free(tp);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#endif

#ifdef FEAT_GUI
    ପାଇଁ (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#endif
#ifdef FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#endif
    tp->tp_ch_used = p_ch;

    ଫେରନ୍ତୁ tp;
}

    ଶୂନ୍ୟ
free_tabpage(tabpage_T *tp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

#ifdef FEAT_DIFF
    diff_clear(tp);
#endif
#ifdef FEAT_PROP_POPUP
    ଯେତେବେଳେ (tp->tp_first_popupwin != ଖାଲି)
	popup_close_tabpage(tp, tp->tp_first_popupwin->w_id, TRUE);
#endif
    ପାଇଁ (idx = 0; idx < SNAP_ଗଣନା; ++idx)
	clear_snapshot(tp, idx);
#ifdef FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#endif

    ଯଦି (tp == lastused_tabpage)
	lastused_tabpage = ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_new_tabpage(ପୂର୍ଣ୍ଣସଂଖ୍ୟା after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
#ଯଦି defined(FEAT_TABPANEL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_columns = COLUMNS_WITHOUT_TPL();
#endif

    ଯଦି (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (window_layout_locked(CMD_tabnew))
	ଫେରନ୍ତୁ FAIL;

    newtp = alloc_tabpage();
    ଯଦି (newtp == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // Remember the current windows in this Tab page.
    ଯଦି (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	ଫେରନ୍ତୁ FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == ଖାଲି)
				    ? ଖାଲି : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    ଯଦି (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	ଯଦି (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_next = first_tabpage;
	    first_tabpage = newtp;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		ପାଇଁ (tp = first_tabpage; tp->tp_next != ଖାଲି
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

#ଯଦି defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#endif
#ifdef FEAT_JOB_CHANNEL
	entering_window(curwin);
#endif
#ଯଦି defined(FEAT_TABPANEL)
	ଯଦି (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		ଯଦି (tp2 != curtab)
		{
		    frame_new_width(tp2->tp_topframe, w, FALSE, TRUE);
		    ଯଦି (!frame_check_width(tp2->tp_topframe, w))
			frame_new_width(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#endif
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, ଖାଲି, ଖାଲି, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, ଖାଲି, ଖାଲି, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
	ଫେରନ୍ତୁ OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_open_tabpage(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = (cmdmod.cmod_tab == 0)
				       ? postponed_split_tab : cmdmod.cmod_tab;

    ଯଦି (n == 0)
	ଫେରନ୍ତୁ FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_split_tab = 0;
    ଫେରନ୍ତୁ win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
make_tabpages(ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxcount)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = maxcount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;

    // Limit to 'tabpagemax' tabs.
    ଯଦି (ଗଣନା_2 > p_tpm)
	ଗଣନା_2 = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    ପାଇଁ (todo = ଗଣନା_2 - 1; todo > 0; --todo)
	ଯଦି (win_new_tabpage(0) == FAIL)
	    ଭାଙ୍ଗ;

    unblock_autocmds();

    // return actual number of tab pages
    ଫେରନ୍ତୁ (ଗଣନା_2 - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	ଯଦି (tp == tpc)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	ଯଦି (tp == tpc)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		ଯଦି (win_valid_any_tab(wp))
		    ଫେରନ୍ତୁ TRUE;
	    }
	    ଫେରନ୍ତୁ FALSE;
	}
    }
    // shouldn't happen
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    ଶୂନ୍ୟ
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    ଯଦି (tab == first_tabpage)
    {
	first_tabpage = tab->tp_next;
	ptp = first_tabpage;
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (ptp = first_tabpage; ptp != ଖାଲି && ptp->tp_next != tab;
							    ptp = ptp->tp_next)
	    ;
	ପ୍ରତିଜ୍ଞା(ptp != ଖାଲି);
	ptp->tp_next = tab->tp_next;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
find_tabpage(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    tabpage_T	*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 1;

    ଯଦି (n == 0)
	ଫେରନ୍ତୁ curtab;

    ପାଇଁ (tp = first_tabpage; tp != ଖାଲି && i != n; tp = tp->tp_next)
	++i;
    ଫେରନ୍ତୁ tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabpage_index(tabpage_T *ftp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 1;
    tabpage_T	*tp;

    ପାଇଁ (tp = first_tabpage; tp != ଖାଲି && tp != ftp; tp = tp->tp_next)
	++i;
    ଫେରନ୍ତୁ i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ifdef FEAT_JOB_CHANNEL
    leaving_window(curwin);
#endif
    reset_VIsual_and_resel();	// stop Visual mode
    ଯଦି (trigger_leave_autocmds)
    {
	ଯଦି (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ଖାଲି, ଖାଲି, FALSE, curbuf);
	    ଯଦି (curtab != tp)
		ଫେରନ୍ତୁ FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, ଖାଲି, ଖାଲି, FALSE, curbuf);
	ଯଦି (curtab != tp)
	    ଫେରନ୍ତୁ FAIL;
	apply_autocmds(EVENT_TABLEAVE, ଖାଲି, ଖାଲି, FALSE, curbuf);
	ଯଦି (curtab != tp)
	    ଫେରନ୍ତୁ FAIL;
    }

    reset_dragwin();
#ଯଦି defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    ଯଦି (gui.in_use)
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
    ଯଦି (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_width;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = ଖାଲି;
    lastwin = ଖାଲି;
    ଫେରନ୍ତୁ OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trigger_enter_autocmds,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trigger_leave_autocmds)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_off = tp->tp_firstwin->w_winrow;
    win_T	*next_prevwin = tp->tp_prevwin;
    tabpage_T	*last_tab = curtab;

    use_tabpage(tp);

    ଯଦି (p_ch != curtab->tp_ch_used)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_ch = curtab->tp_ch_used;
	curtab->tp_ch_used = p_ch;
	command_frame_height = FALSE;
	set_option_ମୂଲ୍ୟ((char_u *)"cmdheight", new_ch, ଖାଲି, 0);
	command_frame_height = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (ଶୂନ୍ୟ)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
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
    ଯଦି (curtab->tp_old_Rows != ROWS_AVAIL || (old_off != firstwin->w_winrow
#ifdef FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#endif
		))
	shell_new_rows();
    ଯଦି (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	ଯଦି (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_width;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	ଅନ୍ୟଥା
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = last_tab;

#ଯଦି defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    ଯଦି (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
	ଯଦି (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    ଶୂନ୍ୟ
goto_tabpage(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    tabpage_T	*tp = ଖାଲି;  // shut up compiler
    tabpage_T	*ttp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (text_locked())
    {
	// Not allowed when editing the command line.
	text_locked_ସନ୍ଦେଶ();
	ଫେରନ୍ତୁ;
    }

    // If there is only one it can't work.
    ଯଦି (first_tabpage->tp_next == ଖାଲି)
    {
	ଯଦି (n > 1)
	    beep_flush();
	ଫେରନ୍ତୁ;
    }

    ଯଦି (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	ଯଦି (curtab->tp_next == ଖାଲି)
	    tp = first_tabpage;
	ଅନ୍ୟଥା
	    tp = curtab->tp_next;
    }
    ଅନ୍ୟଥା ଯଦି (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	ପାଇଁ (i = n; i < 0; ++i)
	{
	    ପାଇଁ (tp = first_tabpage; tp->tp_next != ttp && tp->tp_next != ଖାଲି;
		    tp = tp->tp_next)
		;
	    ttp = tp;
	}
    }
    ଅନ୍ୟଥା ଯଦି (n == 9999)
    {
	// Go to last tab page.
	ପାଇଁ (tp = first_tabpage; tp->tp_next != ଖାଲି; tp = tp->tp_next)
	    ;
    }
    ଅନ୍ୟଥା
    {
	// Go to tab page "n".
	tp = find_tabpage(n);
	ଯଦି (tp == ଖାଲି)
	{
	    beep_flush();
	    ଫେରନ୍ତୁ;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ifdef FEAT_GUI_TABLINE
    ଯଦି (gui_use_tabline())
	gui_mch_set_curtab(tabpage_index(curtab));
#endif
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    ଶୂନ୍ୟ
goto_tabpage_tp(
    tabpage_T	*tp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trigger_enter_autocmds,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trigger_leave_autocmds)
{
    ଯଦି (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    set_keep_ସନ୍ଦେଶ(ଖାଲି, 0);

    skip_win_fix_scroll = TRUE;
    ଯଦି (tp != curtab && leave_tabpage(tp->tp_curwin->w_buffer,
					trigger_leave_autocmds) == OK)
    {
	ଯଦି (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	ଅନ୍ୟଥା
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
goto_tabpage_lastused(ଶୂନ୍ୟ)
{
    ଯଦି (!valid_tabpage(lastused_tabpage))
	ଫେରନ୍ତୁ FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    ଫେରନ୍ତୁ OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    ଶୂନ୍ୟ
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    ଯଦି (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ifdef FEAT_GUI_TABLINE
	ଯଦି (gui_use_tabline())
	    gui_mch_set_curtab(tabpage_index(curtab));
#endif
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    ଶୂନ୍ୟ
tabpage_move(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = 1;
    tabpage_T	*tp, *tp_dst;

    ଯଦି (first_tabpage->tp_next == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (tabpage_move_disallowed)
	ଫେରନ୍ତୁ;

    ପାଇଁ (tp = first_tabpage; tp->tp_next != ଖାଲି && n < nr; tp = tp->tp_next)
	++n;

    ଯଦି (tp == curtab || (nr > 0 && tp->tp_next != ଖାଲି
						    && tp->tp_next == curtab))
	ଫେରନ୍ତୁ;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    ଯଦି (curtab == first_tabpage)
	first_tabpage = curtab->tp_next;
    ଅନ୍ୟଥା
    {
	FOR_ALL_TABPAGES(tp)
	    ଯଦି (tp->tp_next == curtab)
		ଭାଙ୍ଗ;
	ଯଦି (tp == ଖାଲି)	// "cannot happen"
	    ଫେରନ୍ତୁ;
	tp->tp_next = curtab->tp_next;
    }

    // Re-insert it at the specified position.
    ଯଦି (nr <= 0)
    {
	curtab->tp_next = first_tabpage;
	first_tabpage = curtab;
    }
    ଅନ୍ୟଥା
    {
	curtab->tp_next = tp_dst->tp_next;
	tp_dst->tp_next = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#ଯଦି defined(FEAT_TABPANEL)
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
    ଶୂନ୍ୟ
win_goto(win_T *wp)
{
#ifdef FEAT_CONCEAL
    win_T	*owp = curwin;
#endif

#ifdef FEAT_PROP_POPUP
    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;
    ଯଦି (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_to_enter_popup_window));
	ଫେରନ୍ତୁ;
    }
#endif
    ଯଦି (text_or_buf_locked())
    {
	beep_flush();
	ଫେରନ୍ତୁ;
    }

    ଯଦି (wp->w_buffer != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    ଅନ୍ୟଥା ଯଦି (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    ଯଦି (!win_valid(wp))
	ଫେରନ୍ତୁ;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    win_enter(wp, TRUE);

#ifdef FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    ଯଦି (win_valid(owp) && owp->w_p_cole > 0 && !ସନ୍ଦେଶ_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    ଯଦି (curwin->w_p_cole > 0 && !ସନ୍ଦେଶ_scrolled)
	need_cursor_ଧାଡ଼ି_redraw = TRUE;
#endif
}

#ଯଦି defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_find_nr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ଯଦି (--winnr == 0)
	    ଭାଙ୍ଗ;
    ଫେରନ୍ତୁ wp;
}
#endif

#ଯଦି defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_find_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    ଯଦି (wp == win)
		ଫେରନ୍ତୁ tp;
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା up, long ଗଣନା_2)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    ଯଦି (popup_is_popup(wp))
	// popups don't have neighbors.
	ଫେରନ୍ତୁ ଖାଲି;
#endif
    foundfr = wp->w_frame;
    ଯେତେବେଳେ (ଗଣନା_2--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	ପାଇଁ (;;)
	{
	    ଯଦି (fr == tp->tp_topframe)
		ଯାଅ end;
	    ଯଦି (up)
		nfr = fr->fr_prev;
	    ଅନ୍ୟଥା
		nfr = fr->fr_next;
	    ଯଦି (fr->fr_parent->fr_layout == FR_COL && nfr != ଖାଲି)
		ଭାଙ୍ଗ;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	ପାଇଁ (;;)
	{
	    ଯଦି (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		ଭାଙ୍ଗ;
	    }
	    fr = nfr->fr_child;
	    ଯଦି (nfr->fr_layout == FR_ROW)
	    {
		// Find the frame at the cursor row.
		ଯେତେବେଳେ (fr->fr_next != ଖାଲି
			&& frame2win(fr)->w_wincol + fr->fr_width
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_next;
	    }
	    ଯଦି (nfr->fr_layout == FR_COL && up)
		ଯେତେବେଳେ (fr->fr_next != ଖାଲି)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    ଫେରନ୍ତୁ foundfr != ଖାଲି ? foundfr->fr_win : ଖାଲି;
}

/*
 * Move to window above or below "count" times.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_goto_ver(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		up,		// TRUE to go to win above
    long	ଗଣନା_2)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    ଯଦି (ERROR_IF_TERM_POPUP_WINDOW)
	ଫେରନ୍ତୁ;
#endif
    win = win_vert_neighbor(curtab, curwin, up, ଗଣନା_2);
    ଯଦି (win != ଖାଲି)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ, long ଗଣନା_2)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    ଯଦି (popup_is_popup(wp))
	// popups don't have neighbors.
	ଫେରନ୍ତୁ ଖାଲି;
#endif
    foundfr = wp->w_frame;
    ଯେତେବେଳେ (ଗଣନା_2--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	ପାଇଁ (;;)
	{
	    ଯଦି (fr == tp->tp_topframe)
		ଯାଅ end;
	    ଯଦି (ବାମ)
		nfr = fr->fr_prev;
	    ଅନ୍ୟଥା
		nfr = fr->fr_next;
	    ଯଦି (fr->fr_parent->fr_layout == FR_ROW && nfr != ଖାଲି)
		ଭାଙ୍ଗ;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	ପାଇଁ (;;)
	{
	    ଯଦି (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		ଭାଙ୍ଗ;
	    }
	    fr = nfr->fr_child;
	    ଯଦି (nfr->fr_layout == FR_COL)
	    {
		// Find the frame at the cursor row.
		ଯେତେବେଳେ (fr->fr_next != ଖାଲି
			&& frame2win(fr)->w_winrow + fr->fr_height
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_next;
	    }
	    ଯଦି (nfr->fr_layout == FR_ROW && ବାମ)
		ଯେତେବେଳେ (fr->fr_next != ଖାଲି)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    ଫେରନ୍ତୁ foundfr != ଖାଲି ? foundfr->fr_win : ଖାଲି;
}

/*
 * Move to left or right window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_goto_hor(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ବାମ,		// TRUE to go to left win
    long	ଗଣନା_2)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    ଯଦି (ERROR_IF_TERM_POPUP_WINDOW)
	ଫେରନ୍ତୁ;
#endif
    win = win_horz_neighbor(curtab, curwin, ବାମ, ଗଣନା_2);
    ଯଦି (win != ଖାଲି)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    ଶୂନ୍ୟ
win_enter(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା undo_sync)
{
    (ଶୂନ୍ୟ)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    ଶୂନ୍ୟ
win_fix_current_dir(ଶୂନ୍ୟ)
{
    ଯଦି (curwin->w_localdir != ଖାଲି || curtab->tp_localdir != ଖାଲି)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	ଯଦି (globaldir == ଖାଲି)
	{
	    char_u	cwd[MAXPATHL];

	    ଯଦି (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	ଯଦି (curwin->w_localdir != ଖାଲି)
	    dirname = curwin->w_localdir;
	ଅନ୍ୟଥା
	    dirname = curtab->tp_localdir;

	ଯଦି (mch_chdir((ଅକ୍ଷର *)dirname) == 0)
	{
	    last_chdir_reason = ଖାଲି;
	    shorten_fnames(TRUE);
	}
    }
    ଅନ୍ୟଥା ଯଦି (globaldir != ଖାଲି)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((ଅକ୍ଷର *)globaldir);
	VIM_CLEAR(globaldir);
	last_chdir_reason = ଖାଲି;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_enter_ext(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		other_buffer = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_decrement = FALSE;

    ଯଦି (wp == curwin && curwin_invalid == 0)	// nothing to do
	ଫେରନ୍ତୁ FALSE;

#ifdef FEAT_JOB_CHANNEL
    ଯଦି (curwin_invalid == 0)
	leaving_window(curwin);
#endif

    ଯଦି (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	ଯଦି (wp->w_buffer != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ଖାଲି, ଖାଲି, FALSE, curbuf);
	    other_buffer = TRUE;
	    ଯଦି (!win_valid(wp))
		ଫେରନ୍ତୁ FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, ଖାଲି, ଖାଲି, FALSE, curbuf);
	ଯଦି (!win_valid(wp))
	    ଫେରନ୍ତୁ FALSE;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	ଯଦି (aborting())
	    ଫେରନ୍ତୁ FALSE;
#endif
    }

    // sync undo before leaving the current buffer
    ଯଦି ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_buffer)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    ଯଦି (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    ଯଦି (wp->w_buffer != curbuf)
	buf_copy_options(wp->w_buffer, BCO_ENTER | BCO_NOHELP);
    ଯଦି (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_status = true;
    }
    curwin = wp;
    curbuf = wp->w_buffer;
    check_cursor();
    ଯଦି (!virtual_active())
	curwin->w_cursor.coladd = 0;
    ଯଦି (*p_spk == 'c')		// assume cursor position needs updating
	changed_ଧାଡ଼ି_abv_curs();
    ଅନ୍ୟଥା
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ifdef MESSAGE_QUEUE
    ଯଦି (flags & WEE_ALLOW_PARSE_MESSAGES)
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
    ଯଦି (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, ଖାଲି, ଖାଲି, FALSE, curbuf);
    ଯଦି (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
	ଯଦି (other_buffer)
	    apply_autocmds(EVENT_BUFENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_status = true;
#ifdef FEAT_TERMINAL
    ଯଦି (bt_terminal(curwin->w_buffer))
	// terminal is likely in another mode
	redraw_mode = TRUE;
#endif
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#ଯଦି defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    ଯଦି (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    ଯଦି (curwin->w_height < p_wh && !curwin->w_p_wfh
#ifdef FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#endif
	    )
	win_setheight((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_wh);
    ଅନ୍ୟଥା ଯଦି (curwin->w_height == 0)
	win_setheight(1);

    // set window width to desired minimal value
    ଯଦି (curwin->w_width < p_wiw && !curwin->w_p_wfw)
	win_setwidth((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    ଫେରନ୍ତୁ did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = ଖାଲି;

    ଯଦି (curwin->w_buffer == buf)
	wp = curwin;
    ଅନ୍ୟଥା
	FOR_ALL_WINDOWS(wp)
	    ଯଦି (wp->w_buffer == buf)
		ଭାଙ୍ଗ;
    ଯଦି (wp != ଖାଲି)
	win_enter(wp, FALSE);
    ଫେରନ୍ତୁ wp;
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

    ଯଦି (wp != ଖାଲି)
	ଫେରନ୍ତୁ wp;

    FOR_ALL_TABPAGES(tp)
	ଯଦି (tp != curtab)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		ଯଦି (wp->w_buffer == buf)
		    ଭାଙ୍ଗ;
	    ଯଦି (wp != ଖାଲି)
	    {
		goto_tabpage_win(tp, wp);
		ଯଦି (curwin != wp)
		    wp = ଖାଲି;	// something went wrong
		ଭାଙ୍ଗ;
	    }
	}
    ଫେରନ୍ତୁ wp;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_win_id = LOWEST_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    ସ୍ଥିର win_T *
win_alloc(win_T *after, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    ଯଦି (new_wp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (win_alloc_ଧାଡ଼ିଗୁଡ଼ିକ(new_wp) == FAIL)
    {
	vim_free(new_wp);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    new_wp->w_id = ++last_win_id;

#ifdef FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    ଯଦି (new_wp->w_vars == ଖାଲି)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	ଫେରନ୍ତୁ ଖାଲି;
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
    ଯଦି (!hidden)
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
    ଯଦି (gui.in_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ବାମ],
		SBAR_ବାମ, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ଡାହାଣ],
		SBAR_ଡାହାଣ, new_wp);
    }
#endif
#ifdef FEAT_FOLDING
    foldInitWin(new_wp);
#endif
    unblock_autocmds();
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    new_wp->w_next_match_id = 1000;  // up to 1000 can be picked by the user
#endif
    ଫେରନ୍ତୁ new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
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
    ଯଦି (wp->w_stl_click != ଖାଲି)
    {
	ପାଇଁ (i = 0; i < wp->w_stl_click_ଗଣନା; i++)
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

	ଯଦି (prevwin == wp)
	    prevwin = ଖାଲି;
	FOR_ALL_TABPAGES(ttp)
	    ଯଦି (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = ଖାଲି;
    }
    win_free_lsize(wp);

    ପାଇଁ (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    ଯଦି (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    ଯଦି (wip2 != wip && wip2->wi_win == ଖାଲି)
		    {
			ଯଦି (wip2->wi_next != ଖାଲି)
			    wip2->wi_next->wi_prev = wip2->wi_prev;
			ଯଦି (wip2->wi_prev == ଖାଲି)
			    buf->b_wininfo = wip2->wi_next;
			ଅନ୍ୟଥା
			    wip2->wi_prev->wi_next = wip2->wi_next;
			free_wininfo(wip2);
			ଭାଙ୍ଗ;
		    }

		wip->wi_win = ଖାଲି;
	    }

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    clear_matches(wp);
#endif

    free_jumplist(wp);

#ifdef FEAT_QUICKFIX
    qf_free_all(wp);
#endif

#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ବାମ]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ଡାହାଣ]);
    }
#endif // FEAT_GUI

#ifdef FEAT_MENU
    remove_winbar(wp);
#endif
#ifdef FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    ପାଇଁ (i = 0; i < 4; ++i)
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

    ଯଦି (win_valid_any_tab(wp))
	win_remove(wp, tp);
    ଯଦି (autocmd_busy)
    {
	wp->w_next = au_pending_free_win;
	au_pending_free_win = wp;
    }
    ଅନ୍ୟଥା
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_unlisted(win_T *wp)
{
    ଫେରନ୍ତୁ is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#ଯଦି defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    ଶୂନ୍ୟ
win_free_popup(win_T *win)
{
    ଯଦି (win->w_buffer != ଖାଲି)
    {
	ଯଦି (bt_popup(win->w_buffer))
	    win_close_buffer(win, DOBUF_WIPE_REUSE, FALSE);
	ଅନ୍ୟଥା
	    close_buffer(win, win->w_buffer, 0, FALSE, FALSE, TRUE);

	ଯଦି (win->w_buffer != ଖାଲି)
	    --win->w_buffer->b_nwindows;
    }
# ଯଦି defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    ଯଦି (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_frame);
    win_free(win, ଖାଲି);
}
#endif

/*
 * Append window "wp" in the window list after window "after".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    ଯଦି (after == ଖାଲି)	    // after NULL is in front of the first
	before = firstwin;
    ଅନ୍ୟଥା
	before = after->w_next;

    wp->w_next = before;
    wp->w_prev = after;
    ଯଦି (after == ଖାଲି)
	firstwin = wp;
    ଅନ୍ୟଥା
	after->w_next = wp;
    ଯଦି (before == ଖାଲି)
	lastwin = wp;
    ଅନ୍ୟଥା
	before->w_prev = wp;
}

/*
 * Remove a window from the window list.
 */
    ଶୂନ୍ୟ
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ଯଦି (wp->w_prev != ଖାଲି)
	wp->w_prev->w_next = wp->w_next;
    ଅନ୍ୟଥା ଯଦି (tp == ଖାଲି)
	firstwin = curtab->tp_firstwin = wp->w_next;
    ଅନ୍ୟଥା
	tp->tp_firstwin = wp->w_next;

    ଯଦି (wp->w_next != ଖାଲି)
	wp->w_next->w_prev = wp->w_prev;
    ଅନ୍ୟଥା ଯଦି (tp == ଖାଲି)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    ଅନ୍ୟଥା
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
frame_append(frame_T *after, frame_T *frp)
{
    frp->fr_next = after->fr_next;
    after->fr_next = frp;
    ଯଦି (frp->fr_next != ଖାଲି)
	frp->fr_next->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
frame_ଭର୍ତ୍ତି(frame_T *before, frame_T *frp)
{
    frp->fr_next = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    ଯଦି (frp->fr_prev != ଖାଲି)
	frp->fr_prev->fr_next = frp;
    ଅନ୍ୟଥା
	frp->fr_parent->fr_child = frp;
}

/*
 * Remove a frame from a frame list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
frame_remove(frame_T *frp)
{
    ଯଦି (frp->fr_prev != ଖାଲି)
	frp->fr_prev->fr_next = frp->fr_next;
    ଅନ୍ୟଥା
	frp->fr_parent->fr_child = frp->fr_next;
    ଯଦି (frp->fr_next != ଖାଲି)
	frp->fr_next->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_alloc_ଧାଡ଼ିଗୁଡ଼ିକ(win_T *wp)
{
    wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid = 0;
    wp->w_ଧାଡ଼ିଗୁଡ଼ିକ = ALLOC_CLEAR_MULT(wline_T, Rows);
    ଯଦି (wp->w_ଧାଡ଼ିଗୁଡ଼ିକ == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * free lsize arrays for a window
 */
    ଶୂନ୍ୟ
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    ଯଦି (wp != ଖାଲି)
	VIM_CLEAR(wp->w_ଧାଡ଼ିଗୁଡ଼ିକ);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    ଶୂନ୍ୟ
shell_new_rows(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		h = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ROWS_AVAIL;

    ଯଦି (firstwin == ଖାଲି)	// not initialized yet
	ଫେରନ୍ତୁ;
    ଯଦି (h < frame_minheight(topframe, ଖାଲି))
	h = frame_minheight(topframe, ଖାଲି);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    frame_new_height(topframe, h, FALSE, TRUE, FALSE);
    ଯଦି (!frame_check_height(topframe, h))
	frame_new_height(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_used = p_ch;

    ଯଦି (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#ଯଦି defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#ଯଦି 0
    // Disabled: don't want making the screen smaller make a window larger.
    ଯଦି (p_ea)
	win_equal(curwin, FALSE, 'v');
#endif
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    ଶୂନ୍ୟ
shell_new_columns(ଶୂନ୍ୟ)
{
    ଯଦି (firstwin == ଖାଲି)	// not initialized yet
	ଫେରନ୍ତୁ;

#ଯଦି defined(FEAT_TABPANEL)
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_tp_width;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_wincol;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_fr_width;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    frame_new_width(topframe, w, FALSE, TRUE);
    ଯଦି (!frame_check_width(topframe, w))
	frame_new_width(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#ଯଦି defined(FEAT_TABPANEL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tp_width = tabpanel_width();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tp_width_changed = tp_width != prev_tp_width;
    // tabpanel width changed
    ଯଦି (tp_width_changed && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    ଯଦି (tp_width_changed
	    || (tp_width > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_width != prev_fr_width)))
    {
	screen_fill(cmdline_row, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, 0, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns, ' ', ' ', 0);
    }
    prev_tp_width = tabpanel_width();
    prev_wincol = firstwin->w_wincol;
    prev_fr_width = topframe->fr_width;
#endif
    // Adjust offset for command line start column
    cmdline_col_off = firstwin->w_wincol;
    cmdline_width = topframe->fr_width;
    comp_col();
    ଯଦି (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#ଯଦି defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#ଯଦି 0
    // Disabled: don't want making the screen smaller make a window larger.
    ଯଦି (p_ea)
	win_equal(curwin, FALSE, 'h');
#endif
}

/*
 * Save the size of all windows in "gap".
 */
    ଶୂନ୍ୟ
win_size_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), 1);
    ଯଦି (ga_grow(gap, win_ଗଣନା() * 2 + 1) == FAIL)
	ଫେରନ୍ତୁ;

    // first entry is the total lines available for windows
    ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)gap->ga_data)[gap->ga_len++] = ROWS_AVAIL - last_stl_height(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)gap->ga_data)[gap->ga_len++] =
	    wp->w_width + wp->w_vsep_width;
	((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)gap->ga_data)[gap->ga_len++] = wp->w_height;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    ଶୂନ୍ୟ
win_size_restore(garray_T *gap)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;

    ଯଦି (win_ଗଣନା() * 2 + 1 == gap->ga_len
	    && ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)gap->ga_data)[0] == ROWS_AVAIL - last_stl_height(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	ପାଇଁ (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		frame_setwidth(wp->w_frame, ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)gap->ga_data)[i++]);
		win_setheight_win(((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)gap->ga_data)[i++], wp);
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
    ଶୂନ୍ୟ
win_comp_pos(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = tabline_height();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = TPL_LCOL();

    frame_comp_pos(topframe, &row, &col);
#ଯଦି defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
frame_comp_pos(frame_T *topfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *col)
{
    win_T	*wp;
    frame_T	*frp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		h;

    wp = topfrp->fr_win;
    ଯଦି (wp != ଖାଲି)
    {
	ଯଦି (wp->w_winrow != *row || wp->w_wincol != *col)
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
    ଅନ୍ୟଥା
    {
	startrow = *row;
	startcol = *col;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    ଯଦି (topfrp->fr_layout == FR_ROW)
		*row = startrow;	// all frames are at the same row
	    ଅନ୍ୟଥା
		*col = startcol;	// all frames are at the same col
	    frame_comp_pos(frp, row, col);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    ଶୂନ୍ୟ
win_ensure_size(ଶୂନ୍ୟ)
{
    ଯଦି (curwin->w_height == 0)
	win_setheight(1);
    ଯଦି (curwin->w_width == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    ଶୂନ୍ୟ
win_setheight(ପୂର୍ଣ୍ଣସଂଖ୍ୟା height)
{
    win_setheight_win(height, curwin);
#ଯଦି defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    ଶୂନ୍ୟ
win_setheight_win(ପୂର୍ଣ୍ଣସଂଖ୍ୟା height, win_T *win)
{
    ଯଦି (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	ଯଦି (height < p_wmh)
	    height = p_wmh;
	ଯଦି (height == 0)
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
    ସ୍ଥିର ଶୂନ୍ୟ
frame_setheight(frame_T *curfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room;		// total number of lines available
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		take;		// number of lines taken from other windows
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room_cmdline;	// lines available from cmdline
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		run;
    frame_T	*frp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		h;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room_reserved;

    // If the height already is the desired value, nothing to do.
    ଯଦି (curfrp->fr_height == height)
	ଫେରନ୍ତୁ;

    ଯଦି (curfrp->fr_parent == ଖାଲି)
    {
	// topframe: can only change the command line height
	ଯଦି (height > 0)
	    frame_new_height(curfrp, height, FALSE, FALSE, TRUE);
    }
    ଅନ୍ୟଥା ଯଦି (curfrp->fr_parent->fr_layout == FR_ROW)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = frame_minheight(curfrp->fr_parent, ଖାଲି);
	ଯଦି (height < h)
	    height = h;
	frame_setheight(curfrp->fr_parent, height);
    }
    ଅନ୍ୟଥା
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
	ପାଇଁ (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		ଯଦି (frp != curfrp
			&& frp->fr_win != ଖାଲି
			&& frp->fr_win->w_p_wfh)
		    room_reserved += frp->fr_height;
		room += frp->fr_height;
		ଯଦି (frp != curfrp)
		    room -= frame_minheight(frp, ଖାଲି);
	    }
	    ଯଦି (curfrp->fr_width != topframe->fr_width)
		room_cmdline = 0;
	    ଅନ୍ୟଥା
	    {
		room_cmdline = Rows - p_ch - (lastwin->w_winrow
						+ VISIBLE_HEIGHT(lastwin)
						+ lastwin->w_status_height);
		ଯଦି (room_cmdline < 0)
		    room_cmdline = 0;
	    }

	    ଯଦି (height <= room + room_cmdline)
		ଭାଙ୍ଗ;
	    ଯଦି (run == 2 || curfrp->fr_width == topframe->fr_width)
	    {
		height = room + room_cmdline;
		ଭାଙ୍ଗ;
	    }
	    frame_setheight(curfrp->fr_parent, height
		+ frame_minheight(curfrp->fr_parent, NOWIN) - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = height - curfrp->fr_height;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	ଯଦି (height > room + room_cmdline - room_reserved)
	    room_reserved = room + room_cmdline - height;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	ଯଦି (take < 0 && room - curfrp->fr_height <= room_reserved)
	    room_reserved = 0;

	ଯଦି (take > 0 && room_cmdline > 0)
	{
	    // use lines from cmdline first
	    ଯଦି (take < room_cmdline)
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
	ପାଇଁ (run = 0; run < 2; ++run)
	{
	    ଯଦି (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    ଅନ୍ୟଥା
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    ଯେତେବେଳେ (frp != ଖାଲି && take != 0)
	    {
		h = frame_minheight(frp, ଖାଲି);
		ଯଦି (room_reserved > 0
			&& frp->fr_win != ଖାଲି
			&& frp->fr_win->w_p_wfh)
		{
		    ଯଦି (room_reserved >= frp->fr_height)
			room_reserved -= frp->fr_height;
		    ଅନ୍ୟଥା
		    {
			ଯଦି (frp->fr_height - room_reserved > take)
			    room_reserved = frp->fr_height - take;
			take -= frp->fr_height - room_reserved;
			frame_new_height(frp, room_reserved, FALSE, FALSE,
									TRUE);
			room_reserved = 0;
		    }
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (frp->fr_height - take < h)
		    {
			take -= frp->fr_height - h;
			frame_new_height(frp, h, FALSE, FALSE, TRUE);
		    }
		    ଅନ୍ୟଥା
		    {
			frame_new_height(frp, frp->fr_height - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		ଯଦି (run == 0)
		    frp = frp->fr_next;
		ଅନ୍ୟଥା
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    ଶୂନ୍ୟ
win_setwidth(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
    win_setwidth_win(width, curwin);
}

    ଶୂନ୍ୟ
win_setwidth_win(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    ଯଦି (wp == curwin)
    {
	ଯଦି (width < p_wmw)
	    width = p_wmw;
	ଯଦି (width == 0)
	    width = 1;
    }
    ଅନ୍ୟଥା ଯଦି (width < 0)
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
    ସ୍ଥିର ଶୂନ୍ୟ
frame_setwidth(frame_T *curfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room;		// total number of lines available
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		take;		// number of lines taken from other windows
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		run;
    frame_T	*frp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room_reserved;

    // If the width already is the desired value, nothing to do.
    ଯଦି (curfrp->fr_width == width)
	ଫେରନ୍ତୁ;

    ଯଦି (curfrp->fr_parent == ଖାଲି)
	// topframe: can't change width
	ଫେରନ୍ତୁ;

    ଯଦି (curfrp->fr_parent->fr_layout == FR_COL)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = frame_minwidth(curfrp->fr_parent, ଖାଲି);
	ଯଦି (width < w)
	    width = w;
	frame_setwidth(curfrp->fr_parent, width);
    }
    ଅନ୍ୟଥା
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	ପାଇଁ (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		ଯଦି (frp != curfrp
			&& frp->fr_win != ଖାଲି
			&& frp->fr_win->w_p_wfw)
		    room_reserved += frp->fr_width;
		room += frp->fr_width;
		ଯଦି (frp != curfrp)
		    room -= frame_minwidth(frp, ଖାଲି);
	    }

	    ଯଦି (width <= room)
		ଭାଙ୍ଗ;
	    ଯଦି (run == 2 || curfrp->fr_height >= ROWS_AVAIL)
	    {
		width = room;
		ଭାଙ୍ଗ;
	    }
	    frame_setwidth(curfrp->fr_parent, width
		 + frame_minwidth(curfrp->fr_parent, NOWIN) - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = width - curfrp->fr_width;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	ଯଦି (width > room - room_reserved)
	    room_reserved = room - width;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	ଯଦି (take < 0 && room - curfrp->fr_width < room_reserved)
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
	ପାଇଁ (run = 0; run < 2; ++run)
	{
	    ଯଦି (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    ଅନ୍ୟଥା
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    ଯେତେବେଳେ (frp != ଖାଲି && take != 0)
	    {
		w = frame_minwidth(frp, ଖାଲି);
		ଯଦି (room_reserved > 0
			&& frp->fr_win != ଖାଲି
			&& frp->fr_win->w_p_wfw)
		{
		    ଯଦି (room_reserved >= frp->fr_width)
			room_reserved -= frp->fr_width;
		    ଅନ୍ୟଥା
		    {
			ଯଦି (frp->fr_width - room_reserved > take)
			    room_reserved = frp->fr_width - take;
			take -= frp->fr_width - room_reserved;
			frame_new_width(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (frp->fr_width - take < w)
		    {
			take -= frp->fr_width - w;
			frame_new_width(frp, w, FALSE, FALSE);
		    }
		    ଅନ୍ୟଥା
		    {
			frame_new_width(frp, frp->fr_width - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		ଯଦି (run == 0)
		    frp = frp->fr_next;
		ଅନ୍ୟଥା
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    ଶୂନ୍ୟ
win_setminheight(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		needed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    ଯେତେବେଳେ (p_wmh > 0)
    {
	room = Rows - p_ch;
	needed = min_rows_for_all_tabpages() - 1;  // 1 was added for the cmdline
	ଯଦି (room >= needed)
	    ଭାଙ୍ଗ;
	--p_wmh;
	ଯଦି (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    ଶୂନ୍ୟ
win_setminwidth(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		needed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    ଯେତେବେଳେ (p_wmw > 0)
    {
	room = topframe->fr_width;
	needed = frame_minwidth(topframe, ଖାଲି);
	ଯଦି (room >= needed)
	    ଭାଙ୍ଗ;
	--p_wmw;
	ଯଦି (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    ଶୂନ୍ୟ
win_drag_status_ଧାଡ଼ି(win_T *dragwin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		up;	// if TRUE, drag status line up, otherwise down
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    fr = dragwin->w_frame;
    curfr = fr;
    ଯଦି (fr != topframe)		// more than one window
    {
	fr = fr->fr_parent;
	// When the parent frame is not a column of frames, its parent should
	// be.
	ଯଦି (fr->fr_layout != FR_COL)
	{
	    curfr = fr;
	    ଯଦି (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_parent;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    ଯେତେବେଳେ (curfr != topframe && curfr->fr_next == ଖାଲି)
    {
	ଯଦି (fr != topframe)
	    fr = fr->fr_parent;
	curfr = fr;
	ଯଦି (fr != topframe)
	    fr = fr->fr_parent;
    }

    ଯଦି (offset < 0) // drag up
    {
	up = TRUE;
	offset = -offset;
	// sum up the room of the current frame and above it
	ଯଦି (fr == curfr)
	{
	    // only one window
	    room = fr->fr_height - frame_minheight(fr, ଖାଲି);
	}
	ଅନ୍ୟଥା
	{
	    room = 0;
	    ପାଇଁ (fr = fr->fr_child; ; fr = fr->fr_next)
	    {
		room += fr->fr_height - frame_minheight(fr, ଖାଲି);
		ଯଦି (fr == curfr)
		    ଭାଙ୍ଗ;
	    }
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    ଅନ୍ୟଥା    // drag down
    {
	up = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	room = Rows - cmdline_row;
	ଯଦି (curfr->fr_next == ଖାଲି)
	    --room;
	ଅନ୍ୟଥା
	    room -= p_ch;
	ଯଦି (room < 0)
	    room = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_height - frame_minheight(fr, ଖାଲି);
	fr = curfr;			// put fr at window that grows
    }

    ଯଦି (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    ଯଦି (offset <= 0)
	ଫେରନ୍ତୁ;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    ଯଦି (fr != ଖାଲି)
	frame_new_height(fr, fr->fr_height + offset, up, FALSE, TRUE);

    ଯଦି (up)
	fr = curfr;		// current frame gets smaller
    ଅନ୍ୟଥା
	fr = curfr->fr_next;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    ଯେତେବେଳେ (fr != ଖାଲି && offset > 0)
    {
	n = frame_minheight(fr, ଖାଲି);
	ଯଦି (fr->fr_height - offset <= n)
	{
	    offset -= fr->fr_height - n;
	    frame_new_height(fr, n, !up, FALSE, TRUE);
	}
	ଅନ୍ୟଥା
	{
	    frame_new_height(fr, fr->fr_height - offset, !up, FALSE, TRUE);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (up)
	    fr = fr->fr_prev;
	ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
win_drag_vsep_ଧାଡ଼ି(win_T *dragwin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ବାମ;	// if TRUE, drag separator line left, otherwise right
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    fr = dragwin->w_frame;
    ଯଦି (fr == topframe)		// only one window (cannot happen?)
	ଫେରନ୍ତୁ;
    curfr = fr;
    fr = fr->fr_parent;
    // When the parent frame is not a row of frames, its parent should be.
    ଯଦି (fr->fr_layout != FR_ROW)
    {
	ଯଦି (fr == topframe)	// only a column of windows (cannot happen?)
	    ଫେରନ୍ତୁ;
	curfr = fr;
	fr = fr->fr_parent;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    ଯେତେବେଳେ (curfr->fr_next == ଖାଲି)
    {
	ଯଦି (fr == topframe)
	    ଭାଙ୍ଗ;
	curfr = fr;
	fr = fr->fr_parent;
	ଯଦି (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_parent;
	}
    }

    ଯଦି (offset < 0) // drag left
    {
	ବାମ = TRUE;
	offset = -offset;
	// sum up the room of the current frame and left of it
	room = 0;
	ପାଇଁ (fr = fr->fr_child; ; fr = fr->fr_next)
	{
	    room += fr->fr_width - frame_minwidth(fr, ଖାଲି);
	    ଯଦି (fr == curfr)
		ଭାଙ୍ଗ;
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    ଅନ୍ୟଥା    // drag right
    {
	ବାମ = FALSE;
	// sum up the room of frames right of the current one
	room = 0;
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_width - frame_minwidth(fr, ଖାଲି);
	fr = curfr;			// put fr at window that grows
    }

    ଯଦି (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    ଯଦି (offset <= 0)		// No room at all, quit.
	ଫେରନ୍ତୁ;
    ଯଦି (fr == ଖାଲି)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	ଫେରନ୍ତୁ;

    // grow frame fr by offset lines
    frame_new_width(fr, fr->fr_width + offset, ବାମ, FALSE);

    // shrink other frames: current and at the left or at the right
    ଯଦି (ବାମ)
	fr = curfr;		// current frame gets smaller
    ଅନ୍ୟଥା
	fr = curfr->fr_next;	// next frame gets smaller

    ଯେତେବେଳେ (fr != ଖାଲି && offset > 0)
    {
	n = frame_minwidth(fr, ଖାଲି);
	ଯଦି (fr->fr_width - offset <= n)
	{
	    offset -= fr->fr_width - n;
	    frame_new_width(fr, n, !ବାମ, FALSE);
	}
	ଅନ୍ୟଥା
	{
	    frame_new_width(fr, fr->fr_width - offset, !ବାମ, FALSE);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (ବାମ)
	    fr = fr->fr_prev;
	ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
set_fraction(win_T *wp)
{
    ଯଦି (wp->w_height > 1)
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
    ସ୍ଥିର ଶୂନ୍ୟ
win_fix_scroll(ପୂର୍ଣ୍ଣସଂଖ୍ୟା resize)
{
    ଯଦି (*p_spk == 'c')
	ଫେରନ୍ତୁ;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	ଯଦି (wp->w_height != wp->w_prev_height)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    ଯଦି (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_height - wp->w_prev_height);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		ଯଦି (diff > 0)
		    cursor_down_inner(wp, diff);
		ଅନ୍ୟଥା
		    cursor_up_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_to_fraction(wp, wp->w_prev_height);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		ଯଦି (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    ଅନ୍ୟଥା ଯଦି (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    ଯଦି (!(get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL)))
	win_fix_cursor(FALSE);
    ଅନ୍ୟଥା ଯଦି (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_fix_cursor(ପୂର୍ଣ୍ଣସଂଖ୍ୟା normal)
{
    win_T	*wp = curwin;

    ଯଦି (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା < wp->w_height)
	ଫେରନ୍ତୁ;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    long so = MIN(wp->w_height / 2, get_scrolloff_ମୂଲ୍ୟ());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_down_inner(wp, so);
    linenr_T ଶୀର୍ଷ = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_up_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    ଯଦି (lnum > bot && (wp->w_botline - wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା) != 1)
	nlnum = bot;
    ଅନ୍ୟଥା ଯଦି (lnum < ଶୀର୍ଷ && wp->w_topline != 1)
	nlnum = (so == wp->w_height / 2) ? bot : ଶୀର୍ଷ;

    ଯଦି (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	ଯଦି (normal)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	ଅନ୍ୟଥା  // Scroll instead when not in normal mode.
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
    ଶୂନ୍ୟ
win_new_height(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_height = wp->w_height;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    ଯଦି (height < 0)
	height = 0;
    ଯଦି (wp->w_height == height)
	ଫେରନ୍ତୁ;	    // nothing to do

    ଯଦି (wp->w_height > 0)
    {
	ଯଦି (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	ଯଦି (wp->w_height != prev_height)
	    ଫେରନ୍ତୁ;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	ଯଦି (wp->w_wrow != wp->w_prev_fraction_row)
	    set_fraction(wp);
    }

    wp->w_height = height;
    wp->w_redr_status = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    ଯଦି (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	ଯଦି (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_to_fraction(wp, prev_height);
    }
}

    ଶୂନ୍ୟ
scroll_to_fraction(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_height)
{
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sline, ଧାଡ଼ି_size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height = wp->w_height;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    ଯଦି (height > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (height < wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	ଯଦି (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((long)wp->w_fraction * (long)height - 1L)
							       / FRACTION_MULT;
	ଧାଡ଼ି_size = plines_win_col(wp, lnum, (long)(wp->w_cursor.col)) - 1;
	sline = wp->w_wrow - ଧାଡ଼ି_size;

	ଯଦି (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows = plines_win(wp, lnum, FALSE);

	    ଯଦି (sline > wp->w_height - rows)
	    {
		sline = wp->w_height - rows;
		wp->w_wrow -= rows - ଧାଡ଼ି_size;
	    }
	}

	ଯଦି (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		want_row = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = ଧାଡ଼ି_size;
	    ଯଦି (wp->w_wrow >= wp->w_height
				       && (wp->w_width - win_col_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_width - win_col_off(wp);

		--wp->w_wrow;
		ଯେତେବେଳେ (wp->w_wrow > want_row)
		{
		    skipcol += wp->w_width - win_col_off(wp) + win_col_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (sline > 0)
	{
	    ଯେତେବେଳେ (sline > 0 && lnum > 1)
	    {
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, ଖାଲି, TRUE, ଖାଲି);
		ଯଦି (lnum == 1)
		{
		    // first line in buffer is folded
		    ଧାଡ଼ି_size = 1;
		    --sline;
		    ଭାଙ୍ଗ;
		}
#endif
		--lnum;
#ifdef FEAT_DIFF
		ଯଦି (lnum == wp->w_topline)
		    ଧାଡ଼ି_size = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		ଅନ୍ୟଥା
#endif
		    ଧାଡ଼ି_size = plines_win(wp, lnum, TRUE);
		sline -= ଧାଡ଼ି_size;
	    }

	    ଯଦି (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, ଖାଲି, &lnum, TRUE, ଖାଲି);
#endif
		lnum++;
		wp->w_wrow -= ଧାଡ଼ି_size + sline;
	    }
	    ଅନ୍ୟଥା ଯଦି (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	set_topline(wp, lnum);
    }

    ଯଦି (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    ଯଦି (prev_height > 0)
	wp->w_prev_fraction_row = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    ଶୂନ୍ୟ
win_new_width(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
    // Should we give an error if width < 0?
    wp->w_width = width < 0 ? 0 : width;
    wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid = 0;
    changed_ଧାଡ଼ି_abv_curs_win(wp);
    invalidate_botline_win(wp);

    ଯଦି (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_status = true;
}

    ଶୂନ୍ୟ
win_comp_scroll(win_T *wp)
{
#ଯଦି defined(FEAT_EVAL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_w_p_scr = wp->w_p_scr;
#endif

    wp->w_p_scr = ((unsigned)wp->w_height >> 1);
    ଯଦି (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#ଯଦି defined(FEAT_EVAL)
    ଯଦି (wp->w_p_scr != old_w_p_scr)
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
    ଶୂନ୍ୟ
command_height(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_p_ch = curtab->tp_ch_used;

    // Find bottom frame with width of screen.
    frame_T *frp = lastwin->w_frame;
    ଯେତେବେଳେ (frp->fr_width != topframe->fr_width && frp->fr_parent != ଖାଲି)
	frp = frp->fr_parent;

    // Avoid changing the height of a window with 'winfixheight' set.
    ଯେତେବେଳେ (frp->fr_prev != ଖାଲି && frp->fr_layout == FR_LEAF
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    ଯେତେବେଳେ (p_ch > old_p_ch && command_frame_height)
    {
	ଯଦି (frp == ଖାଲି)
	{
	    emsg(_(e_not_enough_room));
	    p_ch = old_p_ch;
	    ଭାଙ୍ଗ;
	}
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା h = MIN(p_ch - old_p_ch,
			frp->fr_height - frame_minheight(frp, ଖାଲି));
	frame_add_height(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    ଯଦି (p_ch < old_p_ch && command_frame_height && frp != ଖାଲି)
	frame_add_height(frp, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_row = Rows - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    ଯଦି (ସନ୍ଦେଶ_scrolled == 0 && full_screen)
    {
	screen_fill(cmdline_row, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, 0, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns, ' ', ' ', 0);
	ସନ୍ଦେଶ_row = cmdline_row;
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
    ସ୍ଥିର ଶୂନ୍ୟ
frame_add_height(frame_T *frp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    frame_new_height(frp, frp->fr_height + n, FALSE, FALSE, FALSE);
    ପାଇଁ (;;)
    {
	frp = frp->fr_parent;
	ଯଦି (frp == ଖାଲି)
	    ଭାଙ୍ଗ;
	frp->fr_height += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    ଶୂନ୍ୟ
last_status(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    last_status_rec(topframe, last_stl_height(morewin) > 0);
#ଯଦି defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
last_status_rec(frame_T *fr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା statusline)
{
    frame_T	*fp;
    win_T	*wp;

    ଯଦି (fr->fr_layout == FR_LEAF)
    {
	wp = fr->fr_win;
	ଯଦି (wp->w_status_height != 0 && !statusline)
	{
	    // remove status line
	    win_new_height(wp, wp->w_height + wp->w_status_height);
	    wp->w_status_height = 0;
	    comp_col();
	}
	ଅନ୍ୟଥା ଯଦି (wp->w_status_height == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    ଯେତେବେଳେ (fp->fr_height <= frame_minheight(fp, ଖାଲି))
	    {
		ଯଦି (fp == topframe)
		{
		    emsg(_(e_not_enough_room));
		    ଫେରନ୍ତୁ;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		ଯଦି (fp->fr_parent->fr_layout == FR_COL && fp->fr_prev != ଖାଲି)
		    fp = fp->fr_prev;
		ଅନ୍ୟଥା
		    fp = fp->fr_parent;
	    }
	    wp->w_status_height = statusline_height(wp);
	    ଯଦି (fp != fr)
	    {
		frame_new_height(fp, fp->fr_height - wp->w_status_height,
			FALSE, FALSE, FALSE);
		frame_fix_height(wp);
		win_comp_pos();
	    }
	    ଅନ୍ୟଥା
		win_new_height(wp, wp->w_height - wp->w_status_height);
	    comp_col();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	ଯଦି (abs(wp->w_height - wp->w_prev_height) == 1)
	    wp->w_prev_height = wp->w_height;
    }
    ଅନ୍ୟଥା ଯଦି (fr->fr_layout == FR_ROW)
    {
	// vertically split windows, set status line for each one
	ଯଦି (!statusline)
	{
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		last_status_rec(fp, statusline);
	}
	ଅନ୍ୟଥା
	{
	    frame_T	*fp2;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_stlh = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_row_height;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		ଯଦି (fp->fr_win != ଖାଲି && fp->fr_win->w_status_height == 0)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା h = statusline_height(fp->fr_win);
		    ଯଦି (h > max_stlh)
			max_stlh = h;
		}
	    ଯଦି (max_stlh == 0)
		ଫେରନ୍ତୁ;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    ଯେତେବେଳେ (fp2->fr_height - frame_minheight(fp2, ଖାଲି) < max_stlh)
	    {
		ଯଦି (fp2 == topframe)
		{
		    emsg(_(e_not_enough_room));
		    ଫେରନ୍ତୁ;
		}
		ଯଦି (fp2->fr_parent->fr_layout == FR_COL
						&& fp2->fr_prev != ଖାଲି)
		    fp2 = fp2->fr_prev;
		ଅନ୍ୟଥା
		    fp2 = fp2->fr_parent;
	    }

	    ଯଦି (fp2 != fr)
	    {
		frame_new_height(fp2, fp2->fr_height - max_stlh,
							FALSE, FALSE, FALSE);
		new_row_height = fr->fr_height + max_stlh;
	    }
	    ଅନ୍ୟଥା
		new_row_height = fr->fr_height;

	    // Set status and content heights for all leaves.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
	    {
		ଯଦି (fp->fr_win != ଖାଲି)
		{
		    wp = fp->fr_win;
		    ଯଦି (wp->w_status_height == 0)
		    {
			wp->w_status_height = statusline_height(wp);
			win_new_height(wp,
				  new_row_height - wp->w_status_height);
			frame_fix_height(wp);
			comp_col();
			redraw_all_later(UPD_SOME_VALID);
			ଯଦି (abs(wp->w_height - wp->w_prev_height) == 1)
			    wp->w_prev_height = wp->w_height;
		    }
		}
		ଅନ୍ୟଥା
		    last_status_rec(fp, statusline); // nested frames
	    }

	    fr->fr_height = new_row_height;
	    win_comp_pos();
	}
    }
    ଅନ୍ୟଥା
    {
	// horizontally split window, set status line for last one
	ପାଇଁ (fp = fr->fr_child; fp->fr_next != ଖାଲି; fp = fp->fr_next)
	    ;
	last_status_rec(fp, statusline);
    }
}

#ଯଦି defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    ଶୂନ୍ୟ
update_stl_rendered_height(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == ଖାଲି) ? p_stl : wp->w_p_stl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags = (wp == ଖାଲି) ? 0 : OPT_LOCAL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*height = (wp == ଖାଲି) ? &stl_rendered_height
						: &wp->w_stl_rendered_height;

    *height = get_stl_rendered_height(wp == ଖାଲି ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
frame_find_global_stlh_rec(frame_T *frp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h)
{
    ଯଦି (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	ଯଦି (wp->w_height > 0 && wp->w_status_height > 0
		&& *wp->w_p_stlo == NUL)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    ଯଦି (win_free_height - p_wmh < h)
		h = win_free_height - p_wmh;
	}
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (frp = frp->fr_child; frp != ଖାଲି; frp = frp->fr_next)
	    h = frame_find_global_stlh_rec(frp, h);
    }
    ଫେରନ୍ତୁ h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
frame_change_statusline_height_rec(frame_T *frp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା global_stlh)
{
    ଯଦି (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	ଯଦି (wp->w_height > 0 && wp->w_status_height > 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    wp->w_status_height = statusline_height(wp);
	    ଯଦି (wp->w_p_stlo == ଖାଲି || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		ଯଦି (wp->w_status_height > global_stlh)
		    wp->w_status_height = global_stlh;
	    }
	    ଯଦି (wp->w_status_height > win_free_height - p_wmh)
		wp->w_status_height = win_free_height - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	     saved_topfill = wp->w_topfill;
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
    ଅନ୍ୟଥା
    {
	ପାଇଁ (frp = frp->fr_child; frp != ଖାଲି; frp = frp->fr_next)
	    frame_change_statusline_height_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
frame_change_statusline_height(ଶୂନ୍ୟ)
{
    tabpage_T	*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = frame_find_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	frame_change_statusline_height_rec(tp->tp_topframe, global_stlh);
    comp_col();
    redraw_all_later(UPD_SOME_VALID);
    ଫେରନ୍ତୁ global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    ଶୂନ୍ୟ
set_stlo_mh(ପୂର୍ଣ୍ଣସଂଖ୍ୟା mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
statuslineopt_changed(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l_stlo_fh = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l_stlo_mh = 1;

    ଯଦି (stlopt != ଖାଲି)
	p = stlopt;
    ଅନ୍ୟଥା
	p = wp->w_p_stlo;

    ଯେତେବେଳେ (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	ଯଦି (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    ଯଦି (l_stlo_mh < 1)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	ଯଦି (*p != ',' && *p != NUL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (*p == ',')
	    ++p;
    }

    ଯଦି (wp == ଖାଲି)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    ଅନ୍ୟଥା
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
statusline_height(win_T *wp UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା stl_height = 1;
#ଯଦି defined(FEAT_STL_OPT)
    bool fixed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l_stl_rendered_height;

    ଯଦି (wp != ଖାଲି && *wp->w_p_stlo != NUL)
    {
	stl_height = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_height = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }
    ଅନ୍ୟଥା
    {
	stl_height = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_height = (wp != ଖାଲି && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    ଯଦି (!fixed)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_custom_stl = (*p_stl != NUL)
	    || (wp != ଖାଲି && *wp->w_p_stl != NUL);
	ଯଦି (has_custom_stl)
	    stl_height = MAX(1, MIN(l_stl_rendered_height, stl_height));
	ଅନ୍ୟଥା
	    stl_height = 1;
    }
#endif

    ଫେରନ୍ତୁ stl_height;
}

/*
 * Return the number of lines used by the tab page line.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabline_height(ଶୂନ୍ୟ)
{
#ifdef FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    ଯଦି (gui_use_tabline())
	ଫେରନ୍ତୁ 0;
#endif
    ବାଛ (p_stal)
    {
	ସ୍ଥିତି 0: ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି 1: ଫେରନ୍ତୁ (first_tabpage->tp_next == ଖାଲି) ? 0 : 1;
    }
    ଫେରନ୍ତୁ 1;
}

/*
 * Return the height of the last window's statusline.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
last_stl_height(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		morewin)	// pretend there are two or more windows
{
    ଫେରନ୍ତୁ (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_WINDOW)))
		? STATUS_HEIGHT : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
min_rows(ଶୂନ୍ୟ)
{
    ଯଦି (firstwin == ଖାଲି)	// not initialized yet
	ଫେରନ୍ତୁ MIN_ଧାଡ଼ିଗୁଡ଼ିକ;

    ଫେରନ୍ତୁ frame_minheight(curtab->tp_topframe, ଖାଲି) + tabline_height()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
min_rows_for_all_tabpages(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		total;
    tabpage_T	*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    ଯଦି (firstwin == ଖାଲି)	// not initialized yet
	ଫେରନ୍ତୁ MIN_ଧାଡ଼ିଗୁଡ଼ିକ;

    total = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = frame_minheight(tp->tp_topframe, ଖାଲି);
	ଯଦି (total < n)
	    total = n;
    }
    total += tabline_height();
    total += MIN_CMDHEIGHT;
    ଫେରନ୍ତୁ total;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
only_one_window(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    win_T	*wp;

#ଯଦି defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    ଯଦି (popup_is_popup(curwin))
	ଫେରନ୍ତୁ FALSE;
#endif

    // If there is another tab page there always is another window.
    ଯଦି (first_tabpage->tp_next != ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    FOR_ALL_WINDOWS(wp)
	ଯଦି (wp->w_buffer != ଖାଲି
		&& (!((bt_help(wp->w_buffer) && !bt_help(curbuf))
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++ଗଣନା_2;
    ଫେରନ୍ତୁ (ଗଣନା_2 <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
check_lnums_both(ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_curwin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ଯଦି ((do_curwin || wp != curwin) && wp->w_buffer == curbuf)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା need_adjust;

	    ଯଦି (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    ଯଦି (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    ଯଦି (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    ଯଦି (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    ଯଦି (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    ଶୂନ୍ୟ
check_lnums(ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    ଶୂନ୍ୟ
check_lnums_nested(ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    ଶୂନ୍ୟ
reset_lnums(ଶୂନ୍ୟ)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ଯଦି (wp->w_buffer == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    ଯଦି (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    ଯଦି (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    ଯଦି (wp->w_save_cursor.w_topline_save > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
make_snapshot(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    clear_snapshot(curtab, idx);
    ଯଦି (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
make_snapshot_rec(frame_T *fr, frame_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(frame_T);
    ଯଦି (*frp == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_width = fr->fr_width;
    (*frp)->fr_height = fr->fr_height;
    ଯଦି (fr->fr_next != ଖାଲି)
    {
	ଯଦି (make_snapshot_rec(fr->fr_next, &((*frp)->fr_next)) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (fr->fr_child != ଖାଲି)
    {
	ଯଦି (make_snapshot_rec(fr->fr_child, &((*frp)->fr_child)) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (fr->fr_layout == FR_LEAF && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    ଫେରନ୍ତୁ OK;
}

/*
 * Remove any existing snapshot.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_snapshot(tabpage_T *tp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = ଖାଲି;
}

    ସ୍ଥିର ଶୂନ୍ୟ
clear_snapshot_rec(frame_T *fr)
{
    ଯଦି (fr == ଖାଲି)
	ଫେରନ୍ତୁ;
    clear_snapshot_rec(fr->fr_next);
    clear_snapshot_rec(fr->fr_child);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    ସ୍ଥିର win_T *
get_snapshot_curwin_rec(frame_T *ft)
{
    win_T	*wp;

    ଯଦି (ft->fr_next != ଖାଲି)
    {
	ଯଦି ((wp = get_snapshot_curwin_rec(ft->fr_next)) != ଖାଲି)
	    ଫେରନ୍ତୁ wp;
    }
    ଯଦି (ft->fr_child != ଖାଲି)
    {
	ଯଦି ((wp = get_snapshot_curwin_rec(ft->fr_child)) != ଖାଲି)
	    ଫେରନ୍ତୁ wp;
    }

    ଫେରନ୍ତୁ ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    ସ୍ଥିର win_T *
get_snapshot_curwin(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (curtab->tp_snapshot[idx] == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    ଶୂନ୍ୟ
restore_snapshot(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		close_curwin)	    // closing current window
{
    win_T	*wp;

    ଯଦି (curtab->tp_snapshot[idx] != ଖାଲି
	    && curtab->tp_snapshot[idx]->fr_width == topframe->fr_width
	    && curtab->tp_snapshot[idx]->fr_height == topframe->fr_height
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	ଯଦି (wp != ଖାଲି && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_snapshot_rec(frame_T *sn, frame_T *fr)
{
    ଯଦି (sn->fr_layout != fr->fr_layout
	    || (sn->fr_next == ଖାଲି) != (fr->fr_next == ଖାଲି)
	    || (sn->fr_child == ଖାଲି) != (fr->fr_child == ଖାଲି)
	    || (sn->fr_next != ଖାଲି
		&& check_snapshot_rec(sn->fr_next, fr->fr_next) == FAIL)
	    || (sn->fr_child != ଖାଲି
		&& check_snapshot_rec(sn->fr_child, fr->fr_child) == FAIL)
	    || (sn->fr_win != ଖାଲି && !win_valid(sn->fr_win)))
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    ସ୍ଥିର win_T *
restore_snapshot_rec(frame_T *sn, frame_T *fr)
{
    win_T	*wp = ଖାଲି;
    win_T	*wp2;

    fr->fr_height = sn->fr_height;
    fr->fr_width = sn->fr_width;
    ଯଦି (fr->fr_layout == FR_LEAF)
    {
	frame_new_height(fr, fr->fr_height, FALSE, FALSE, FALSE);
	frame_new_width(fr, fr->fr_width, FALSE, FALSE);
	wp = sn->fr_win;
    }
    ଯଦି (sn->fr_next != ଖାଲି)
    {
	wp2 = restore_snapshot_rec(sn->fr_next, fr->fr_next);
	ଯଦି (wp2 != ଖାଲି)
	    wp = wp2;
    }
    ଯଦି (sn->fr_child != ଖାଲି)
    {
	wp2 = restore_snapshot_rec(sn->fr_child, fr->fr_child);
	ଯଦି (wp2 != ଖାଲି)
	    wp = wp2;
    }
    ଫେରନ୍ତୁ wp;
}

#ଯଦି defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_hasvertsplit(ଶୂନ୍ୟ)
{
    frame_T	*fr;

    ଯଦି (topframe->fr_layout == FR_ROW)
	ଫେରନ୍ତୁ TRUE;

    ଯଦି (topframe->fr_layout == FR_COL)
	FOR_ALL_FRAMES(fr, topframe->fr_child)
	    ଯଦି (fr->fr_layout == FR_ROW)
		ଫେରନ୍ତୁ TRUE;

    ଫେରନ୍ତୁ FALSE;
}
#endif

#ଯଦି defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_win_ସଂଖ୍ୟା(win_T *wp, win_T *first_win)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 1;
    win_T	*w;

    ପାଇଁ (w = first_win; w != ଖାଲି && w != wp; w = W_NEXT(w))
	++i;

    ଯଦି (w == ଖାଲି)
	ଫେରନ୍ତୁ 0;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ i;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_tab_ସଂଖ୍ୟା(tabpage_T *tp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 1;
    tabpage_T	*t;

    ପାଇଁ (t = first_tabpage; t != ଖାଲି && t != tp; t = t->tp_next)
	++i;

    ଯଦି (t == ଖାଲି)
	ଫେରନ୍ତୁ 0;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ i;
}
#endif

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
frame_check_height(frame_T *topfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height)
{
    frame_T *frp;

    ଯଦି (topfrp->fr_height != height)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (topfrp->fr_layout == FR_ROW)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    ଯଦି (frp->fr_height != height)
		ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
frame_check_width(frame_T *topfrp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
    frame_T *frp;

    ଯଦି (topfrp->fr_width != width)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (topfrp->fr_layout == FR_COL)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    ଯଦି (frp->fr_width != width)
		ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ TRUE;
}

#ଯଦି defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
int_cmp(ଅଚଳ ଶୂନ୍ୟ *pa, ଅଚଳ ଶୂନ୍ୟ *pb)
{
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା a = *(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)pa;
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = *(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)pb;
    ଯଦି (a > b)
	ଫେରନ୍ତୁ 1;
    ଯଦି (a < b)
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    ଅକ୍ଷର *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = empty_option;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tw;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		color_cols[256];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j = 0;

    ଯଦି (wp != ଖାଲି && wp->w_buffer == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;  // buffer was closed

    ଯଦି (cc != ଖାଲି)
	s = cc;
    ଅନ୍ୟଥା ଯଦି (wp != ଖାଲି)
	s = wp->w_p_cc;

    ଯଦି (wp != ଖାଲି)
	tw = wp->w_buffer->b_p_tw;
    ଅନ୍ୟଥା
	// buffer-local value not set, assume zero
	tw = 0;

    ଯେତେବେଳେ (*s != NUL && ଗଣନା_2 < 255)
    {
	ଯଦି (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    col = (*s == '-') ? -1 : 1;
	    ++s;
	    ଯଦି (!VIM_ISDIGIT(*s))
		ଫେରନ୍ତୁ e_invalid_argument;
	    col = col * getdigits(&s);
	    ଯଦି (tw == 0)
		ଯାଅ skip;  // 'textwidth' not set, skip this item
	    col += tw;
	    ଯଦି (col < 0)
		ଯାଅ skip;
	}
	ଅନ୍ୟଥା ଯଦି (VIM_ISDIGIT(*s))
	    col = getdigits(&s);
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ e_invalid_argument;
	color_cols[ଗଣନା_2++] = col - 1;  // 1-based to 0-based
skip:
	ଯଦି (*s == NUL)
	    ଭାଙ୍ଗ;
	ଯଦି (*s != ',')
	    ଫେରନ୍ତୁ e_invalid_argument;
	ଯଦି (*++s == NUL)
	    ଫେରନ୍ତୁ e_invalid_argument;  // illegal trailing comma as in "set cc=80,"
    }

    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;  // only parse "cc"

    vim_free(wp->w_p_cc_cols);
    ଯଦି (ଗଣନା_2 == 0)
	wp->w_p_cc_cols = ଖାଲି;
    ଅନ୍ୟଥା
    {
	wp->w_p_cc_cols = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଗଣନା_2 + 1);
	ଯଦି (wp->w_p_cc_cols != ଖାଲି)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_cols, ଗଣନା_2, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), int_cmp);

	    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
		// skip duplicates
		ଯଦି (j == 0 || wp->w_p_cc_cols[j - 1] != color_cols[i])
		    wp->w_p_cc_cols[j++] = color_cols[i];
	    wp->w_p_cc_cols[j] = -1;  // end marker
	}
    }

    ଫେରନ୍ତୁ ଖାଲି;  // no error
}
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_last_winid(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ last_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_locked(win_T *wp)
{
    ଫେରନ୍ତୁ wp->w_locked;
}
