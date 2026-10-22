/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * gui_xim.c: functions for the X Input Method
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ !defined(GTK_CHECK_ਸੰਸਕਰਣ)
# define GTK_CHECK_ਸੰਸਕਰਣ(a, b, c) 0
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# ਨਹੀਂਤਾਂ
#  include <gdk/gdkkeysyms.h>
# endif
# ਜੇ !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  ਨਹੀਂਤਾਂ
#   include <gdk/gdkx.h>
#  endif
# endif
#ਜੇ_ਅੰਤ

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#ਜੇ defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
ਸਥਿਰ ਖਾਲੀ xim_log(ਅੱਖਰ *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    ਸਥਿਰ ਖਾਲੀ
xim_log(ਅੱਖਰ *s, ...)
{
    va_list arglist;
    ਸਥਿਰ FILE *fd = ਨਿਰਾ;

    ਜੇ (fd == (FILE *)-1)
	ਵਾਪਸ;
    ਜੇ (fd == ਨਿਰਾ)
    {
	fd = mch_fopen("xim.log", "w");
	ਜੇ (fd == ਨਿਰਾ)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    ਵਾਪਸ;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.ਵਿੱਚ_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.ਵਿੱਚ_use && *p_imsf != NUL)
#ਨਹੀਂਤਾਂ
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
ਸਥਿਰ callback_T imaf_cb;	    // 'imactivatefunc' callback function
ਸਥਿਰ callback_T imsf_cb;	    // 'imstatusfunc' callback function

    ਅੱਖਰ *
did_ਸੈੱਟ_imactivatefunc(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (option_ਸੈੱਟ_callback_func(p_imaf, &imaf_cb) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਅੱਖਰ *
did_ਸੈੱਟ_imstatusfunc(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (option_ਸੈੱਟ_callback_func(p_imsf, &imsf_cb) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
call_imactivatefunc(ਪੂਰਨ ਸਰਗਰਮ)
{
    typval_T argv[2];
    ਪੂਰਨ save_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;

    argv[0].v_type = VAR_ਸੰਖਿਆ;
    argv[0].vval.v_ਸੰਖਿਆ = ਸਰਗਰਮ ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (ਖਾਲੀ)call_callback_retnr(&imaf_cb, 1, argv);

    ਕੁੰਜੀ_Typed = save_ਕੁੰਜੀ_Typed;
}

    ਸਥਿਰ ਪੂਰਨ
call_imstatusfunc(ਖਾਲੀ)
{
    ਪੂਰਨ is_ਸਰਗਰਮ;
    ਪੂਰਨ save_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;

    // FIXME: Don't execute user function in unsafe situation.
    ਜੇ (exiting || is_autocmd_blocked())
	ਵਾਪਸ FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++ਸੁਨੇਹਾ_silent;
    is_ਸਰਗਰਮ = call_callback_retnr(&imsf_cb, 0, ਨਿਰਾ);
    --ਸੁਨੇਹਾ_silent;

    ਕੁੰਜੀ_Typed = save_ਕੁੰਜੀ_Typed;
    ਵਾਪਸ (is_ਸਰਗਰਮ > 0);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_xim_stuff(ਖਾਲੀ)
{
# ਜੇ defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_im_funcs(ਪੂਰਨ copyID ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ abort = FALSE;

# ਜੇ defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = ਸੈੱਟ_ref_ਵਿੱਚ_callback(&imaf_cb, copyID);
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&imsf_cb, copyID);
# endif

    ਵਾਪਸ abort;
}
#ਜੇ_ਅੰਤ


#ਜੇ defined(FEAT_XIM)

# ਜੇ defined(FEAT_GUI_GTK)
ਸਥਿਰ ਪੂਰਨ xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    ਸਥਿਰ ਖਾਲੀ
init_preedit_ਸ਼ੁਰੂ_ਖਾਨਾ(ਖਾਲੀ)
{
    ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	preedit_ਸ਼ੁਰੂ_ਖਾਨਾ = cmdline_getvcol_cursor();
    ਨਹੀਂਤਾਂ ਜੇ (curwin != ਨਿਰਾ && curwin->w_ਬਫਰ != ਨਿਰਾ)
	getvcol(curwin, &curwin->w_cursor, &preedit_ਸ਼ੁਰੂ_ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_ਬਦਲਿਆ_while_preediting = curbuf->b_ਬਦਲਿਆ;
}

ਸਥਿਰ ਪੂਰਨ im_is_ਸਰਗਰਮ	       = FALSE;	// IM is enabled for current mode
ਸਥਿਰ ਪੂਰਨ preedit_is_ਸਰਗਰਮ   = FALSE;
ਸਥਿਰ ਪੂਰਨ im_preedit_cursor   = 0;	// cursor offset in characters
ਸਥਿਰ ਪੂਰਨ im_preedit_trailing = 0;	// number of characters after cursor

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_ਕੁੰਜੀ_Void_ਚਿੰਨ੍ਹ
#    define GDK_ਕੁੰਜੀ_Void_ਚਿੰਨ੍ਹ GDK_Void_ਚਿੰਨ੍ਹ
#   endif
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  im_activatekey_keyval = GDK_ਕੁੰਜੀ_Void_ਚਿੰਨ੍ਹ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  im_activatekey_state  = 0;
#  endif

ਸਥਿਰ GtkWidget *preedit_ਖਿੜਕੀ = ਨਿਰਾ;
ਸਥਿਰ GtkWidget *preedit_label = ਨਿਰਾ;
#  ifdef USE_GTK4
ਸਥਿਰ ਪੂਰਨ preedit_label_ਚੌੜਾਈ = 0;	// last measured popover natural width
ਸਥਿਰ ਪੂਰਨ preedit_popover_ਉਚਾਈ = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
ਸਥਿਰ GtkCssProvider *preedit_css_provider = ਨਿਰਾ;
ਸਥਿਰ guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

ਸਥਿਰ ਖਾਲੀ im_preedit_ਖਿੜਕੀ_ਸੈੱਟ_position(ਖਾਲੀ);

    ਖਾਲੀ
im_ਸੈੱਟ_ਸਰਗਰਮ(ਪੂਰਨ ਸਰਗਰਮ)
{
    ਪੂਰਨ was_ਸਰਗਰਮ;

    was_ਸਰਗਰਮ = !!im_get_ਸਥਿਤੀ();
    im_is_ਸਰਗਰਮ = (ਸਰਗਰਮ && !p_imdisable);

    ਜੇ (im_is_ਸਰਗਰਮ != was_ਸਰਗਰਮ)
	xim_reset();
}

    ਖਾਲੀ
xim_ਸੈੱਟ_focus(ਪੂਰਨ focus)
{
    ਜੇ (xic == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (focus)
	gtk_im_context_focus_ਵਿੱਚ(xic);
    ਨਹੀਂਤਾਂ
	gtk_im_context_focus_ਬਾਹਰਲਾ(xic);
}

    ਖਾਲੀ
im_ਸੈੱਟ_position(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਜੇ (xic == ਨਿਰਾ)
	ਵਾਪਸ;

    GdkRectangle area;

    area.x = FILL_X(ਖਾਨਾ);
    area.y = FILL_Y(ਪੰਕਤੀ);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.ਚੌੜਾਈ  = gui.char_ਚੌੜਾਈ
		    * ((Screen_ਸਤਰਾਂ != ਨਿਰਾ && mb_lefthalve(ਪੰਕਤੀ, ਖਾਨਾ)) ? 2 : 1);
    area.ਉਚਾਈ = gui.char_ਉਚਾਈ;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    ਜੇ (p_imst == IM_OVER_THE_SPOT)
    {
	ਪੂਰਨ popover_h = preedit_popover_ਉਚਾਈ;
	ਜੇ (popover_h <= 0)
	    popover_h = gui.char_ਉਚਾਈ;
	ਜੇ (popover_h > area.ਉਚਾਈ)
	    area.ਉਚਾਈ = popover_h;
    }
#  endif

    gtk_im_context_ਸੈੱਟ_cursor_location(xic, &area);

    ਜੇ (p_imst == IM_OVER_THE_SPOT)
	im_preedit_ਖਿੜਕੀ_ਸੈੱਟ_position();
}

#  ਜੇ 0 || defined(PROTO) // apparently only used in gui_x11.c
    ਖਾਲੀ
xim_ਸੈੱਟ_preedit(ਖਾਲੀ)
{
    im_ਸੈੱਟ_position(gui.ਪੰਕਤੀ, gui.ਖਾਨਾ);
}
#  endif

    ਸਥਿਰ ਖਾਲੀ
im_ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼(char_u *str, ਪੂਰਨ len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    ਜੇ (ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&ਨਿਵੇਸ਼_conv, str, &len);
	g_return_if_fail(str != ਨਿਰਾ);
    }

    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf_csi(str, len);

    ਜੇ (ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	vim_free(str);

    ਜੇ (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ਸਥਿਰ ਖਾਲੀ
im_preedit_ਖਿੜਕੀ_ਸੈੱਟ_position(ਖਾਲੀ)
{
    ਜੇ (preedit_ਖਿੜਕੀ == ਨਿਰਾ)
	ਵਾਪਸ;

#  ifdef USE_GTK4
    // GTK4: The popover is parented to gui.mainwin (the toplevel) rather
    // than gui.drawarea, because GtkDrawingArea is a leaf widget that does
    // not support children and causes snapshot/redraw artifacts when used
    // as a popover parent.  Translate the cursor cell coordinates from
    // drawarea space to mainwin space, and use the cached label width as
    // the anchor rect width so the popover's left edge aligns with the
    // cursor cell:
    //   popup_origin.x = anchor.x + anchor.w/2 - popup_w/2
    // With anchor.w == label_w and popup_w ≈ label_w (after CSS zeroing
    // popover frame padding), popup_origin.x ≈ anchor.x.
    GdkRectangle rect;
    graphene_ਬਿੰਦੂ_t pt_ਵਿੱਚ, pt_ਬਾਹਰਲਾ;

    pt_ਵਿੱਚ.x = FILL_X(gui.ਖਾਨਾ);
    pt_ਵਿੱਚ.y = FILL_Y(gui.ਪੰਕਤੀ);
    ਜੇ (!gtk_widget_compute_ਬਿੰਦੂ(gui.drawarea, gui.mainwin,
						&pt_ਵਿੱਚ, &pt_ਬਾਹਰਲਾ))
    {
	pt_ਬਾਹਰਲਾ.x = pt_ਵਿੱਚ.x;
	pt_ਬਾਹਰਲਾ.y = pt_ਵਿੱਚ.y;
    }
    rect.x = (ਪੂਰਨ)pt_ਬਾਹਰਲਾ.x;
    rect.y = (ਪੂਰਨ)pt_ਬਾਹਰਲਾ.y;
    rect.ਚੌੜਾਈ = preedit_label_ਚੌੜਾਈ;
    rect.ਉਚਾਈ = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    ਪੂਰਨ ਵਿੱਥ = gui.char_ਉਚਾਈ - preedit_popover_ਉਚਾਈ;
    gtk_popover_ਸੈੱਟ_ਵਿੱਥ(GTK_POPOVER(preedit_ਖਿੜਕੀ), 0, ਵਿੱਥ);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_ਸੈੱਟ_halign(preedit_ਖਿੜਕੀ, GTK_ALIGN_ਸ਼ੁਰੂ);
    gtk_popover_ਸੈੱਟ_pointing_ਮੰਜ਼ਿਲ(GTK_POPOVER(preedit_ਖਿੜਕੀ), &rect);
#  ਨਹੀਂਤਾਂ
    ਪੂਰਨ x, y, ਚੌੜਾਈ, ਉਚਾਈ;
    ਪੂਰਨ screen_x, screen_y, screen_ਚੌੜਾਈ, screen_ਉਚਾਈ;

    gui_gtk_get_screen_geom_ਦਾ_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_ਚੌੜਾਈ, &screen_ਉਚਾਈ);
    gdk_ਖਿੜਕੀ_get_origin(gtk_widget_get_ਖਿੜਕੀ(gui.drawarea), &x, &y);
    gtk_ਖਿੜਕੀ_get_ਮਾਪ(GTK_ਖਿੜਕੀ(preedit_ਖਿੜਕੀ), &ਚੌੜਾਈ, &ਉਚਾਈ);
    x = x + FILL_X(gui.ਖਾਨਾ);
    y = y + FILL_Y(gui.ਪੰਕਤੀ);
    ਜੇ (x + ਚੌੜਾਈ > screen_x + screen_ਚੌੜਾਈ)
	x = screen_x + screen_ਚੌੜਾਈ - ਚੌੜਾਈ;
    ਜੇ (y + ਉਚਾਈ > screen_y + screen_ਉਚਾਈ)
	y = screen_y + screen_ਉਚਾਈ - ਉਚਾਈ;
    gtk_ਖਿੜਕੀ_move(GTK_ਖਿੜਕੀ(preedit_ਖਿੜਕੀ), x, y);
#  endif
}

    ਸਥਿਰ ਖਾਲੀ
im_preedit_ਖਿੜਕੀ_open(ਖਾਲੀ)
{
    ਅੱਖਰ *preedit_string;
#  ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,16,0)
    ਅੱਖਰ buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
#   ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,16,0)
    GdkRGBA color;
#   endif
#  ਨਹੀਂਤਾਂ
    GdkColor color;
#  endif
    gint w, h;

    ਜੇ (preedit_ਖਿੜਕੀ == ਨਿਰਾ)
    {
#  ifdef USE_GTK4
	// GTK4: use GtkPopover (xdg_popup) so the compositor keeps keyboard
	// focus on the drawing area and does not disable text-input-v3.
	// See issue #20257.  Parent the popover to gui.mainwin (a real
	// container) rather than gui.drawarea: GtkDrawingArea is a leaf
	// widget and using it as a popover parent causes snapshot artifacts
	// (white-out) on the drawing area.
	preedit_ਖਿੜਕੀ = gtk_popover_new();
	gtk_popover_ਸੈੱਟ_autohide(GTK_POPOVER(preedit_ਖਿੜਕੀ), FALSE);
	gtk_popover_ਸੈੱਟ_has_arrow(GTK_POPOVER(preedit_ਖਿੜਕੀ), FALSE);
	// Pin the popover below the anchor.  Together with anchor.h == 0 in
	// im_preedit_window_set_position() this lands the popover's top edge
	// exactly on FILL_Y(row), so the popover covers the cursor cell rather
	// than landing above it (the default GTK_POS_TOP would do that and
	// only flip to bottom when there is no room above, which is flaky).
	gtk_popover_ਸੈੱਟ_position(GTK_POPOVER(preedit_ਖਿੜਕੀ), GTK_POS_BOTTOM);
	gtk_widget_ਸੈੱਟ_can_focus(preedit_ਖਿੜਕੀ, FALSE);
	gtk_widget_ਸੈੱਟ_focusable(preedit_ਖਿੜਕੀ, FALSE);
	gtk_widget_ਸੈੱਟ_can_ਨਿਸ਼ਾਨਾ(preedit_ਖਿੜਕੀ, FALSE);
	gtk_widget_ਸੈੱਟ_ਮਾਪਾ(preedit_ਖਿੜਕੀ, gui.mainwin);
#  ਨਹੀਂਤਾਂ
	preedit_ਖਿੜਕੀ = gtk_ਖਿੜਕੀ_new(GTK_ਖਿੜਕੀ_POPUP);
	gtk_ਖਿੜਕੀ_ਸੈੱਟ_transient_for(GTK_ਖਿੜਕੀ(preedit_ਖਿੜਕੀ),
						     GTK_ਖਿੜਕੀ(gui.mainwin));
#  endif
	preedit_label = gtk_label_new("");
	gtk_widget_ਸੈੱਟ_name(preedit_label, "vim-gui-preedit-area");
#  ifdef USE_GTK4
	gtk_label_ਸੈੱਟ_xalign(GTK_LABEL(preedit_label), 0.0);
	gtk_widget_ਸੈੱਟ_halign(preedit_label, GTK_ALIGN_ਸ਼ੁਰੂ);
	gtk_widget_ਸੈੱਟ_valign(preedit_label, GTK_ALIGN_ਸ਼ੁਰੂ);
	// Tag the popover with a CSS class so the minimal frame CSS below
	// (no border, no rounded corners, no shadow, no padding) applies
	// only to our preedit popover and not other popovers in the app.
	gtk_widget_ਜੋੜੋ_css_class(preedit_ਖਿੜਕੀ, "vim-preedit-popover");
	gtk_popover_ਸੈੱਟ_ਬੱਚਾ(GTK_POPOVER(preedit_ਖਿੜਕੀ), preedit_label);
#  ਨਹੀਂਤਾਂ
	gtk_container_ਜੋੜੋ(GTK_CONTAINER(preedit_ਖਿੜਕੀ), preedit_label);
#  endif
    }

#  ifdef USE_GTK4
    // Build (or rebuild on background-colour change) the CSS that flattens
    // the popover frame and paints its background with gui.back_pixel.
    // Using a solid background instead of `background: transparent` makes
    // the popover surface fully cover the cursor cell even when the popover
    // surface is allocated a pixel or two larger than the glyph bounding
    // box (e.g. due to residual theme insets) -- otherwise the blinking
    // text cursor at the cell origin shows through the popover frame.
    ਜੇ (preedit_css_provider == ਨਿਰਾ || preedit_css_cached_bg != gui.ਪਿੱਠ_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	ਜੇ (preedit_css_provider != ਨਿਰਾ)
	{
	    gtk_style_context_remove_provider_for_display(display,
				    GTK_STYLE_PROVIDER(preedit_css_provider));
	    g_object_unref(preedit_css_provider);
	}
	preedit_css_provider = gtk_css_provider_new();
	css = g_strdup_printf(
		"popover.vim-preedit-popover,\n"
		"popover.vim-preedit-popover > contents,\n"
		"popover.vim-preedit-popover label {\n"
		"  border: none;\n"
		"  border-radius: 0;\n"
		"  box-shadow: none;\n"
		"  padding: 0;\n"
		"  margin: 0;\n"
		"  min-width: 0;\n"
		"  min-height: 0;\n"
		"  background-color: #%02lx%02lx%02lx;\n"
		"}\n"
		"popover.vim-preedit-popover > arrow {\n"
		"  background: transparent;\n"
		"  border: none;\n"
		"}\n",
		(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)((gui.ਪਿੱਠ_pixel >> 16) & 0xff),
		(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)((gui.ਪਿੱਠ_pixel >> 8) & 0xff),
		(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(gui.ਪਿੱਠ_pixel & 0xff));
	gtk_css_provider_ਲੱਦੋ_from_string(preedit_css_provider, css);
	g_free(css);
	gtk_style_context_ਜੋੜੋ_provider_for_display(display,
			    GTK_STYLE_PROVIDER(preedit_css_provider), G_MAXUINT);
	preedit_css_cached_bg = gui.ਪਿੱਠ_pixel;
    }
#  endif

#  ifndef USE_GTK4
#   ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,16,0)
    {
	GtkStyleContext * ਅਚਲ context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * ਅਚਲ provider = gtk_css_provider_new();
	gchar		   *css = ਨਿਰਾ;
	ਅਚਲ ਅੱਖਰ * ਅਚਲ fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_ਮਾਪ(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = ਨਿਰਾ;

	ਜੇ (!pango_font_description_get_ਮਾਪ_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * ਅਚਲ screen
		  = gdk_ਖਿੜਕੀ_get_screen(gtk_widget_get_ਖਿੜਕੀ(gui.mainwin));
	    ਅਚਲ gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	ਜੇ (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	ਨਹੀਂਤਾਂ
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != ਨਿਰਾ ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.ਪਿੱਠ_pixel >> 16) & 0xff,
		(gui.ਪਿੱਠ_pixel >> 8) & 0xff,
		gui.ਪਿੱਠ_pixel & 0xff);

	gtk_css_provider_ਲੱਦੋ_from_ਅੰਕੜੇ(provider, css, -1, ਨਿਰਾ);
	gtk_style_context_ਜੋੜੋ_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, ਆਕਾਰ(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_ਸਾਧਾਰਨ, &color);

    vim_snprintf(buf, ਆਕਾਰ(buf), "#%06X", gui.ਪਿੱਠ_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_ਸਾਧਾਰਨ,
								      &color);
#   ਨਹੀਂਤਾਂ
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, ਆਕਾਰ(buf), "#%06X", (ਬਿਨਾਨਿਸ਼ਾਨੀ)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_ਸਾਧਾਰਨ, &color);

    vim_snprintf(buf, ਆਕਾਰ(buf), "#%06X", (ਬਿਨਾਨਿਸ਼ਾਨੀ)gui.ਪਿੱਠ_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_ਖਿੜਕੀ, GTK_STATE_ਸਾਧਾਰਨ, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ਨਿਰਾ);

    ਜੇ (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	ਜੇ (attr_list == ਨਿਰਾ)
	    attr_list = pango_attr_list_new();

	ਜੇ (gui.norm_font != ਨਿਰਾ)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0;
	    pa->ਸਿਰਾ_ਸੂਚਕਾਂਕ = G_MAXUINT;
	    pango_attr_list_ਪਾਓ(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0;
	pa->ਸਿਰਾ_ਸੂਚਕਾਂਕ = G_MAXUINT;
	pango_attr_list_ਪਾਓ(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.ਪਿੱਠ_pixel >> 16) & 0xff) * 257,
		((gui.ਪਿੱਠ_pixel >> 8) & 0xff) * 257,
		(gui.ਪਿੱਠ_pixel & 0xff) * 257);
	pa->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0;
	pa->ਸਿਰਾ_ਸੂਚਕਾਂਕ = G_MAXUINT;
	pango_attr_list_ਪਾਓ(attr_list, pa);
#  endif
	gtk_label_ਸੈੱਟ_ਪਾਠ(GTK_LABEL(preedit_label), preedit_string);
	gtk_label_ਸੈੱਟ_ਗੁਣ(GTK_LABEL(preedit_label), attr_list);

	layout = gtk_label_get_layout(GTK_LABEL(preedit_label));
	pango_layout_get_pixel_ਮਾਪ(layout, &w, &h);
	h = MAX(h, gui.char_ਉਚਾਈ);
#  ifdef USE_GTK4
	// Cache label/popover size derived from the Pango layout, which is
	// available without mapping the popover.  Using these for positioning
	// avoids calling gtk_popover_set_pointing_to() after popup, which
	// triggers a GDK "compositor doesn't support moving popups" warning
	// on compositors without xdg_popup.reposition (e.g. Weston).
	preedit_label_ਚੌੜਾਈ = w;
	preedit_popover_ਉਚਾਈ = h;

	// Report an enlarged cursor rectangle that covers both the cursor
	// cell and the preedit popover.  This way the IM (e.g. fcitx5)
	// places its candidate window below the popover instead of on top
	// of it.  Coordinates are in client-widget (drawarea) space.
	ਜੇ (xic != ਨਿਰਾ)
	{
	    GdkRectangle area;

	    area.x = FILL_X(gui.ਖਾਨਾ);
	    area.y = FILL_Y(gui.ਪੰਕਤੀ);
	    area.ਚੌੜਾਈ = preedit_label_ਚੌੜਾਈ;
	    area.ਉਚਾਈ = MAX(gui.char_ਉਚਾਈ, preedit_popover_ਉਚਾਈ);
	    gtk_im_context_ਸੈੱਟ_cursor_location(xic, &area);
	}

	// Pop down before re-anchoring: on compositors that lack
	// xdg_popup.reposition (e.g. Weston), calling set_pointing_to() on
	// a mapped popover triggers GDK's remap fallback and a warning.
	// Doing the popdown ourselves makes the next popup land at the new
	// anchor cleanly without that warning.
	ਜੇ (gtk_widget_get_mapped(preedit_ਖਿੜਕੀ))
	    gtk_popover_popdown(GTK_POPOVER(preedit_ਖਿੜਕੀ));

	im_preedit_ਖਿੜਕੀ_ਸੈੱਟ_position();

	gtk_widget_ਕਤਾਰ_resize(preedit_ਖਿੜਕੀ);
	gtk_popover_popup(GTK_POPOVER(preedit_ਖਿੜਕੀ));
#  ਨਹੀਂਤਾਂ
	gtk_ਖਿੜਕੀ_resize(GTK_ਖਿੜਕੀ(preedit_ਖਿੜਕੀ), w, h);
	gtk_widget_show_all(preedit_ਖਿੜਕੀ);

	im_preedit_ਖਿੜਕੀ_ਸੈੱਟ_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    ਸਥਿਰ ਖਾਲੀ
im_preedit_ਖਿੜਕੀ_close(ਖਾਲੀ)
{
    ਜੇ (preedit_ਖਿੜਕੀ == ਨਿਰਾ)
	ਵਾਪਸ;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_ਖਿੜਕੀ));
#  ਨਹੀਂਤਾਂ
    gtk_widget_hide(preedit_ਖਿੜਕੀ);
#  endif
}

    ਸਥਿਰ ਖਾਲੀ
im_show_preedit(ਖਾਲੀ)
{
    im_preedit_ਖਿੜਕੀ_open();

    ਜੇ (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ਸਥਿਰ ਖਾਲੀ
im_delete_preedit(ਖਾਲੀ)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    ਜੇ (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_ਖਿੜਕੀ_close();
	ਵਾਪਸ;
    }

    ਜੇ (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ
#  ifdef FEAT_TERMINAL
	    && !ਪਦ_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	ਵਾਪਸ;
    }
    ਲਈ (; im_preedit_cursor > 0; --im_preedit_cursor)
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(bskey, (ਪੂਰਨ)ਆਕਾਰ(bskey));

    ਲਈ (; im_preedit_trailing > 0; --im_preedit_trailing)
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(delkey, (ਪੂਰਨ)ਆਕਾਰ(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    ਸਥਿਰ ਖਾਲੀ
im_correct_cursor(ਪੂਰਨ ਅੰਕ_move_ਪਿੱਠ)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    ਜੇ (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ)
	ਵਾਪਸ;
#  ifdef FEAT_RIGHTLEFT
    ਜੇ ((State & ਬਹੁਲਕ_CMDLINE) == 0 && curwin != ਨਿਰਾ && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    ਲਈ (; ਅੰਕ_move_ਪਿੱਠ > 0; --ਅੰਕ_move_ਪਿੱਠ)
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(backkey, (ਪੂਰਨ)ਆਕਾਰ(backkey));
}

ਸਥਿਰ ਪੂਰਨ xim_expected_char = NUL;
ਸਥਿਰ ਪੂਰਨ xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    ਸਥਿਰ ਖਾਲੀ
im_show_info(ਖਾਲੀ)
{
    ਪੂਰਨ	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    ਜੇ ((State & ਬਹੁਲਕ_ਸਾਧਾਰਨ) || (State & ਬਹੁਲਕ_ਪਾਓ))
	setcursor();
    ਬਾਹਰਲਾ_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
im_commit_cb(GtkIMContext *context ਅਣਵਰਤਿਆ,
	     ਅਚਲ gchar *str,
	     gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		slen = (ਪੂਰਨ)STRLEN(str);
    ਪੂਰਨ		ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼ = TRUE;
    ਪੂਰਨ		clen;
    ਪੂਰਨ		len = slen;
    ਪੂਰਨ		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    ਜੇ (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	ਜੇ (preedit_ਸ਼ੁਰੂ_ਖਾਨਾ == MAXCOL)
	{
	    init_preedit_ਸ਼ੁਰੂ_ਖਾਨਾ();
	    commit_with_preedit = FALSE;
	}

	// The thing which setting "preedit_start_col" to MAXCOL means that
	// "preedit_start_col" will be set forcedly when calling
	// preedit_changed_cb() next time.
	// "preedit_start_col" should not reset with MAXCOL on this part. Vim
	// is simulating the preediting by using add_to_input_str(). when
	// preedit begin immediately before committed, the typebuf is not
	// flushed to screen, then it can't get correct "preedit_start_col".
	// Thus, it should calculate the cells by adding cells of the committed
	// string.
	ਜੇ (ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&ਨਿਵੇਸ਼_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	ਜੇ (ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_ਸ਼ੁਰੂ_ਖਾਨਾ += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    ਜੇ (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	ਜੇ (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼ = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    ਜੇ (ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼)
	im_ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼((char_u *)str, slen);

    ਜੇ (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	ਜੇ (!commit_with_preedit)
	    preedit_ਸ਼ੁਰੂ_ਖਾਨਾ = MAXCOL;

	// This flag is used in changed() at next call.
	xim_ਬਦਲਿਆ_while_preediting = TRUE;
    }

    ਜੇ (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    ਸਥਿਰ ਖਾਲੀ
im_preedit_ਸ਼ੁਰੂ_cb(GtkIMContext *context ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_start_cb()\n");
#  endif

    im_is_ਸਰਗਰਮ = TRUE;
    preedit_is_ਸਰਗਰਮ = TRUE;
    gui_update_cursor(TRUE, FALSE);
    im_show_info();
}

/*
 * Callback invoked after end to the preedit.
 */
    ਸਥਿਰ ਖਾਲੀ
im_preedit_ਸਿਰਾ_cb(GtkIMContext *context ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    ਜੇ (p_imst == IM_ON_THE_SPOT)
	preedit_ਸ਼ੁਰੂ_ਖਾਨਾ = MAXCOL;
    xim_has_preediting = FALSE;

#  ਜੇ 0
    // Removal of this line suggested by Takuhiro Nishioka.  Fixes that IM was
    // switched off unintentionally.  We now use preedit_is_active (added by
    // SungHyun Nam).
    im_is_ਸਰਗਰਮ = FALSE;
#  endif
    preedit_is_ਸਰਗਰਮ = FALSE;
    gui_update_cursor(TRUE, FALSE);
    im_show_info();
}

/*
 * Callback invoked after changes to the preedit string.  If the preedit
 * string was empty before, remember the preedit start column so we know
 * where to apply feedback attributes.  Delete the previous preedit string
 * if there was one, save the new preedit cursor offset, and put the new
 * string into the input buffer.
 *
 * TODO: The pragmatic "put into input buffer" approach used here has
 *       several fundamental problems:
 *
 * - The characters in the preedit string are subject to remapping.
 *   That's broken, only the finally committed string should be remapped.
 *
 * - There is a race condition involved:  The retrieved value for the
 *   current cursor position will be wrong if any unprocessed characters
 *   are still queued in the input buffer.
 *
 * - Due to the lack of synchronization between the file buffer in memory
 *   and any typed characters, it's practically impossible to implement the
 *   "retrieve_surrounding" and "delete_surrounding" signals reliably.  IM
 *   modules for languages such as Thai are likely to rely on this feature
 *   for proper operation.
 *
 * Conclusions:  I think support for preediting needs to be moved to the
 * core parts of Vim.  Ideally, until it has been committed, the preediting
 * string should only be displayed and not affect the buffer content at all.
 * The question how to deal with the synchronization issue still remains.
 * Circumventing the input buffer is probably not desirable.  Anyway, I think
 * implementing "retrieve_surrounding" is the only hard problem.
 *
 * One way to solve all of this in a clean manner would be to queue all key
 * press/release events "as is" in the input buffer, and apply the IM filtering
 * at the receiving end of the queue.  This, however, would have a rather large
 * impact on the code base.  If there is an easy way to force processing of all
 * remaining input from within the "retrieve_surrounding" signal handler, this
 * might not be necessary.  Gotta ask on vim-dev for opinions.
 */
    ਸਥਿਰ ਖਾਲੀ
im_preedit_ਬਦਲਿਆ_cb(GtkIMContext *context, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ    *preedit_string = ਨਿਰਾ;
    ਪੂਰਨ	    cursor_ਸੂਚਕਾਂਕ    = 0;
    ਪੂਰਨ	    ਅੰਕ_move_ਪਿੱਠ   = 0;
    char_u  *str;
    char_u  *p;
    ਪੂਰਨ	    i;

    ਜੇ (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ਨਿਰਾ,
					  &cursor_ਸੂਚਕਾਂਕ);
    ਨਹੀਂਤਾਂ
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ਨਿਰਾ,
					  ਨਿਰਾ);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != ਨਿਰਾ); // just in case

    ਜੇ (p_imst == IM_OVER_THE_SPOT)
    {
	ਜੇ (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	ਨਹੀਂਤਾਂ
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	ਜੇ (preedit_ਸ਼ੁਰੂ_ਖਾਨਾ == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_ਸ਼ੁਰੂ_ਖਾਨਾ();
	}
	ਨਹੀਂਤਾਂ ਜੇ (cursor_ਸੂਚਕਾਂਕ == 0 && preedit_string[0] == '\0')
	{
	    xim_has_preediting = FALSE;

	    // If at the start position (after typing backspace)
	    // preedit_start_col must be reset.
	    preedit_ਸ਼ੁਰੂ_ਖਾਨਾ = MAXCOL;
	}

	im_delete_preedit();

	// Compute the end of the preediting area: "preedit_end_col".
	// According to the documentation of
	// gtk_im_context_get_preedit_string(), the cursor_pos output argument
	// returns the offset in bytes.  This is unfortunately not true -- real
	// life shows the offset is in characters, and the GTK+ source code
	// agrees with me.  Will file a bug later.
	ਜੇ (preedit_ਸ਼ੁਰੂ_ਖਾਨਾ != MAXCOL)
	    preedit_ਸਿਰਾ_ਖਾਨਾ = preedit_ਸ਼ੁਰੂ_ਖਾਨਾ;
	str = (char_u *)preedit_string;
	ਲਈ (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    ਪੂਰਨ is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    ਜੇ (!is_composing || p_deco)
	    {
		ਜੇ (i < cursor_ਸੂਚਕਾਂਕ)
		    ++im_preedit_cursor;
		ਨਹੀਂਤਾਂ
		    ++im_preedit_trailing;
	    }
	    ਜੇ (!is_composing && i >= cursor_ਸੂਚਕਾਂਕ)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++ਅੰਕ_move_ਪਿੱਠ;
	    }
	    ਜੇ (preedit_ਸ਼ੁਰੂ_ਖਾਨਾ != MAXCOL)
		preedit_ਸਿਰਾ_ਖਾਨਾ += utf_ptr2cells(p);
	}

	ਜੇ (p > str)
	{
	    im_ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼(str, (ਪੂਰਨ)(p - str));
	    im_correct_cursor(ਅੰਕ_move_ਪਿੱਠ);
	}
    }

    g_free(preedit_string);

    ਜੇ (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    ਸਥਿਰ ਪੂਰਨ
translate_pango_ਗੁਣ(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    ਪੂਰਨ		    char_attr = HL_ਸਾਧਾਰਨ;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    ਜੇ (attr != ਨਿਰਾ && ((PangoAttrInt *)attr)->ਮੁੱਲ
						 != (ਪੂਰਨ)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_ਭਾਰ);
    ਜੇ (attr != ਨਿਰਾ && ((PangoAttrInt *)attr)->ਮੁੱਲ >= (ਪੂਰਨ)PANGO_ਭਾਰ_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    ਜੇ (attr != ਨਿਰਾ && ((PangoAttrInt *)attr)->ਮੁੱਲ
						   != (ਪੂਰਨ)PANGO_STYLE_ਸਾਧਾਰਨ)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    ਜੇ (attr != ਨਿਰਾ)
    {
	ਅਚਲ PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	ਜੇ ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    ਵਾਪਸ char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    ਪੂਰਨ
im_get_feedback_attr(ਪੂਰਨ ਖਾਨਾ)
{
    ਅੱਖਰ	    *preedit_string = ਨਿਰਾ;
    PangoAttrList   *attr_list	    = ਨਿਰਾ;
    ਪੂਰਨ		    char_attr	    = -1;

    ਜੇ (xic == ਨਿਰਾ)
	ਵਾਪਸ char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ਨਿਰਾ);

    ਜੇ (preedit_string != ਨਿਰਾ && attr_list != ਨਿਰਾ)
    {
	ਪੂਰਨ idx;

	// Get the byte index as used by PangoAttrIterator
	ਲਈ (idx = 0; ਖਾਨਾ > 0 && preedit_string[idx] != '\0'; --ਖਾਨਾ)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	ਜੇ (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    ਪੂਰਨ			ਸ਼ੁਰੂ, ਸਿਰਾ;

	    char_attr = HL_ਸਾਧਾਰਨ;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    ਕਰੋ
	    {
		pango_attr_iterator_range(iter, &ਸ਼ੁਰੂ, &ਸਿਰਾ);

		ਜੇ (idx >= ਸ਼ੁਰੂ && idx < ਸਿਰਾ)
		    char_attr |= translate_pango_ਗੁਣ(iter);
	    }
	    ਜਦੋਂ (pango_attr_iterator_ਅਗਲਾ(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    ਜੇ (attr_list != ਨਿਰਾ)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    ਵਾਪਸ char_attr;
}

    ਖਾਲੀ
xim_init(ਖਾਲੀ)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != ਨਿਰਾ);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) != ਨਿਰਾ);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_ਬਦਲਿਆ_cb), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_ਸ਼ੁਰੂ_cb), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_ਸਿਰਾ_cb), ਨਿਰਾ);

#  ifdef USE_GTK4
    gtk_im_context_ਸੈੱਟ_client_widget(xic, gui.drawarea);

    // Seed the cursor location immediately.  GTK4's Wayland IM module
    // batches text-input-v3 state and sends it on the next commit cycle
    // (typically triggered by focus_in).  Without this seed the very
    // first commit would carry the protocol default (0, 0, 0, 0) for the
    // cursor rectangle, and the IM's candidate window for the very first
    // composition would appear at the top-left of the surface instead of
    // next to the cursor.
    im_ਸੈੱਟ_position(gui.ਪੰਕਤੀ, gui.ਖਾਨਾ);
#  ਨਹੀਂਤਾਂ
    gtk_im_context_ਸੈੱਟ_client_ਖਿੜਕੀ(xic, gtk_widget_get_ਖਿੜਕੀ(gui.drawarea));
#  endif
}

    ਖਾਲੀ
im_shutdown(ਖਾਲੀ)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    ਜੇ (xic != ਨਿਰਾ)
    {
	gtk_im_context_focus_ਬਾਹਰਲਾ(xic);
	g_object_unref(xic);
	xic = ਨਿਰਾ;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    ਜੇ (preedit_ਖਿੜਕੀ != ਨਿਰਾ)
    {
	gtk_widget_unparent(preedit_ਖਿੜਕੀ);
	preedit_ਖਿੜਕੀ = ਨਿਰਾ;
	preedit_label = ਨਿਰਾ;
    }
    ਜੇ (preedit_css_provider != ਨਿਰਾ)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = ਨਿਰਾ;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_ਸਰਗਰਮ = FALSE;
    im_commit_handler_id = 0;
    ਜੇ (p_imst == IM_ON_THE_SPOT)
	preedit_ਸ਼ੁਰੂ_ਖਾਨਾ = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
/*
 * Convert the string argument to keyval and state for GdkEventKey.
 * If str is valid return TRUE, otherwise FALSE.
 *
 * See 'imactivatekey' for documentation of the format.
 */
    ਸਥਿਰ ਪੂਰਨ
im_string_ਮੰਜ਼ਿਲ_keyval(ਅਚਲ ਅੱਖਰ *str, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *keyval, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *state)
{
    ਅਚਲ ਅੱਖਰ	    *mods_ਸਿਰਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	    tmp_keyval;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	    tmp_state = 0;

    mods_ਸਿਰਾ = strrchr(str, '-');
    mods_ਸਿਰਾ = (mods_ਸਿਰਾ != ਨਿਰਾ) ? mods_ਸਿਰਾ + 1 : str;

    // Parse modifier keys
    ਜਦੋਂ (str < mods_ਸਿਰਾ)
	ਚੋਣ (*str++)
	{
	    ਹਾਲਤ '-':							ਰੋਕੋ;
	    ਹਾਲਤ 'S': ਹਾਲਤ 's': tmp_state |= (ਬਿਨਾਨਿਸ਼ਾਨੀ)GDK_ਖਿਸਕਾਓ_ਨਕਾਬ;	ਰੋਕੋ;
	    ਹਾਲਤ 'L': ਹਾਲਤ 'l': tmp_state |= (ਬਿਨਾਨਿਸ਼ਾਨੀ)GDK_ਤਾਲਾ_ਨਕਾਬ;	ਰੋਕੋ;
	    ਹਾਲਤ 'C': ਹਾਲਤ 'c': tmp_state |= (ਬਿਨਾਨਿਸ਼ਾਨੀ)GDK_CONTROL_ਨਕਾਬ;ਰੋਕੋ;
	    ਹਾਲਤ '1':		tmp_state |= (ਬਿਨਾਨਿਸ਼ਾਨੀ)GDK_MOD1_ਨਕਾਬ;	ਰੋਕੋ;
	    ਹਾਲਤ '2':		tmp_state |= (ਬਿਨਾਨਿਸ਼ਾਨੀ)GDK_MOD2_ਨਕਾਬ;	ਰੋਕੋ;
	    ਹਾਲਤ '3':		tmp_state |= (ਬਿਨਾਨਿਸ਼ਾਨੀ)GDK_MOD3_ਨਕਾਬ;	ਰੋਕੋ;
	    ਹਾਲਤ '4':		tmp_state |= (ਬਿਨਾਨਿਸ਼ਾਨੀ)GDK_MOD4_ਨਕਾਬ;	ਰੋਕੋ;
	    ਹਾਲਤ '5':		tmp_state |= (ਬਿਨਾਨਿਸ਼ਾਨੀ)GDK_MOD5_ਨਕਾਬ;	ਰੋਕੋ;
	    ਮੂਲ:
		ਵਾਪਸ FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    ਜੇ (tmp_keyval == 0 || tmp_keyval == GDK_Void_ਚਿੰਨ੍ਹ)
	ਵਾਪਸ FALSE;

    ਜੇ (keyval != ਨਿਰਾ)
	*keyval = tmp_keyval;
    ਜੇ (state != ਨਿਰਾ)
	*state = tmp_state;

    ਵਾਪਸ TRUE;
}

/*
 * Return TRUE if p_imak is valid, otherwise FALSE.  As a special case, an
 * empty string is also regarded as valid.
 *
 * Note: The numerical key value of p_imak is cached if it was valid; thus
 * boldly assuming im_xim_isvalid_imactivate() will always be called whenever
 * 'imak' changes.  This is currently the case but not obvious -- should
 * probably rename the function for clarity.
 */
    ਪੂਰਨ
im_xim_isvalid_imactivate(ਖਾਲੀ)
{
    ਜੇ (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_Void_ਚਿੰਨ੍ਹ;
	im_activatekey_state  = 0;
	ਵਾਪਸ TRUE;
    }

    ਵਾਪਸ im_string_ਮੰਜ਼ਿਲ_keyval((ਅਚਲ ਅੱਖਰ *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    ਸਥਿਰ ਖਾਲੀ
im_synthesize_keypress(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ keyval, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ state)
{
    Gdk_Event_ਕੁੰਜੀ *event;

    event = (Gdk_Event_ਕੁੰਜੀ *)gdk_event_new(GDK_ਕੁੰਜੀ_PRESS);
    g_object_ref(gtk_widget_get_ਖਿੜਕੀ(gui.drawarea));
					// unreffed by gdk_event_free()
    event->ਖਿੜਕੀ_2 = gtk_widget_get_ਖਿੜਕੀ(gui.drawarea);
    event->send_event = TRUE;
    event->ਸਮਾਂ = GDK_CURRENT_ਸਮਾਂ;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysym_ਮੰਜ਼ਿਲ_Keycode(GDK_ਖਿੜਕੀ_XDISPLAY(event->ਖਿੜਕੀ_2), (ਕੁੰਜੀ_Sym)keyval);
    event->ਲੰਮਾਈ = 0;
    event->string = ਨਿਰਾ;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_ਕੁੰਜੀ_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    ਖਾਲੀ
xim_reset(ਖਾਲੀ)
{
#  ifdef FEAT_EVAL
    ਜੇ (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_ਸਰਗਰਮ);
    ਨਹੀਂਤਾਂ
#  endif
    ਜੇ (xic != ਨਿਰਾ)
    {
	gtk_im_context_reset(xic);

	ਜੇ (p_imdisable)
	    im_shutdown();
	ਨਹੀਂਤਾਂ
	{
	    xim_ਸੈੱਟ_focus(gui.ਵਿੱਚ_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_ਸੈੱਟ_focus(gui.ਵਿੱਚ_focus);
#  ਨਹੀਂਤਾਂ
	    ਜੇ (im_activatekey_keyval != GDK_Void_ਚਿੰਨ੍ਹ)
	    {
		ਜੇ (im_is_ਸਰਗਰਮ)
		{
		    g_signal_handler_ਖੰਡ(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		im_shutdown();
		xim_init();
		xim_ਸੈੱਟ_focus(gui.ਵਿੱਚ_focus);
	    }
#  endif
	}
    }

    ਜੇ (p_imst == IM_ON_THE_SPOT)
	preedit_ਸ਼ੁਰੂ_ਖਾਨਾ = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    ਪੂਰਨ
xim_ਕਤਾਰ_ਕੁੰਜੀ_press_event(Gdk_Event_ਕੁੰਜੀ *event, ਪੂਰਨ ਥੱਲੇ)
{
#   ifdef FEAT_GUI_GTK
    ਜੇ (event->state & GDK_SUPER_ਨਕਾਬ) ਵਾਪਸ FALSE;
#   endif
    ਜੇ (ਥੱਲੇ)
    {
	// Workaround GTK2 XIM 'feature' that always converts keypad keys to
	// chars., even when not part of an IM sequence (ref. feature of
	// gdk/gdkkeyuni.c).
	// Flag any keypad keys that might represent a single char.
	// If this (on its own - i.e., not part of an IM sequence) is
	// committed while we're processing one of these keys, we can ignore
	// that commit and go ahead & process it ourselves.  That way we can
	// still distinguish keypad keys for use in mappings.
	// Also add GDK_space to make <S-Space> work.
	ਚੋਣ (event->keyval)
	{
	    ਹਾਲਤ GDK_KP_ਜੋੜੋ:      xim_expected_char = '+';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_ਘਟਾਓ: xim_expected_char = '-';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_Divide:   xim_expected_char = '/';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_Multiply: xim_expected_char = '*';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_Decimal:  xim_expected_char = '.';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_Equal:    xim_expected_char = '=';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_0:	  xim_expected_char = '0';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_1:	  xim_expected_char = '1';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_2:	  xim_expected_char = '2';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_3:	  xim_expected_char = '3';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_4:	  xim_expected_char = '4';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_5:	  xim_expected_char = '5';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_6:	  xim_expected_char = '6';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_7:	  xim_expected_char = '7';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_8:	  xim_expected_char = '8';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_KP_9:	  xim_expected_char = '9';  ਰੋਕੋ;
	    ਹਾਲਤ GDK_space:	  xim_expected_char = ' ';  ਰੋਕੋ;
	    ਮੂਲ:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    ਜੇ (xic != ਨਿਰਾ && !p_imdisable
		    && (State & (ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE
					      | ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	ਜੇ (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ state_ਨਕਾਬ;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_ਨਕਾਬ  = im_activatekey_state;
	    state_ਨਕਾਬ |= ((ਪੂਰਨ)GDK_ਖਿਸਕਾਓ_ਨਕਾਬ | (ਪੂਰਨ)GDK_CONTROL_ਨਕਾਬ
							| (ਪੂਰਨ)GDK_MOD1_ਨਕਾਬ);

	    ਜੇ ((event->state & state_ਨਕਾਬ) != im_activatekey_state)
		ਵਾਪਸ FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    ਜੇ (event->type != GDK_ਕੁੰਜੀ_PRESS)
		ਵਾਪਸ TRUE;

	    ਜੇ (map_ਮੰਜ਼ਿਲ_exists_ਬਹੁਲਕ((char_u *)"", ਬਹੁਲਕ_LANGMAP, FALSE))
	    {
		im_ਸੈੱਟ_ਸਰਗਰਮ(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= ਬਹੁਲਕ_LANGMAP;
		ਜੇ (State & ਬਹੁਲਕ_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~ਬਹੁਲਕ_LANGMAP;
		}
		ਨਹੀਂਤਾਂ
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= ਬਹੁਲਕ_LANGMAP;
		}
		ਵਾਪਸ TRUE;
	    }

	    ਵਾਪਸ gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	ਜੇ (im_activatekey_keyval == GDK_Void_ਚਿੰਨ੍ਹ || im_is_ਸਰਗਰਮ)
	{
	    ਪੂਰਨ imresult = gtk_im_context_filter_keypress(xic, event);

	    ਜੇ (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		ਜੇ (!imresult && preedit_ਸ਼ੁਰੂ_ਖਾਨਾ != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    ਵਾਪਸ FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    ਜੇ (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		ਵਾਪਸ FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    ਵਾਪਸ imresult;
	}
    }

    ਵਾਪਸ FALSE;
}
#  ਨਹੀਂਤਾਂ // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    ਪੂਰਨ
im_xim_isvalid_imactivate(ਖਾਲੀ)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    ਵਾਪਸ p_imak[0] == NUL;
}

    ਪੂਰਨ
xim_ਕਤਾਰ_ਕੁੰਜੀ_press_event(GdkEvent *event ਅਣਵਰਤਿਆ, ਪੂਰਨ ਥੱਲੇ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ FALSE;
}
#  endif // !USE_GTK4

    ਪੂਰਨ
im_get_ਸਥਿਤੀ(ਖਾਲੀ)
{
#  ifdef FEAT_EVAL
    ਜੇ (USE_IMSTATUSFUNC)
	ਵਾਪਸ call_imstatusfunc();
#  endif
    ਵਾਪਸ im_is_ਸਰਗਰਮ;
}

    ਪੂਰਨ
preedit_get_ਸਥਿਤੀ(ਖਾਲੀ)
{
    ਵਾਪਸ preedit_is_ਸਰਗਰਮ;
}

    ਪੂਰਨ
im_is_preediting(ਖਾਲੀ)
{
    ਵਾਪਸ xim_has_preediting;
}

# ਨਹੀਂਤਾਂ // !FEAT_GUI_GTK

ਸਥਿਰ ਪੂਰਨ	xim_is_ਸਰਗਰਮ = FALSE;  // XIM should be active in the current
					//ਬਹੁਲਕ
ਸਥਿਰ ਪੂਰਨ	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
ਸਥਿਰ XIMStyle	ਨਿਵੇਸ਼_style;
ਸਥਿਰ ਪੂਰਨ	ਸਥਿਤੀ_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    ਖਾਲੀ
im_ਸੈੱਟ_ਸਰਗਰਮ(ਪੂਰਨ ਸਰਗਰਮ_arg)
{
    ਪੂਰਨ ਸਰਗਰਮ = ਸਰਗਰਮ_arg;

    // If 'imdisable' is set, XIM is never active.
    ਜੇ (p_imdisable)
	ਸਰਗਰਮ = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (ਨਿਵੇਸ਼_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	ਸਰਗਰਮ = TRUE;

#  ਜੇ defined(FEAT_EVAL)
    ਜੇ (USE_IMACTIVATEFUNC)
    {
	ਜੇ (ਸਰਗਰਮ != im_get_ਸਥਿਤੀ())
	{
	    call_imactivatefunc(ਸਰਗਰਮ);
	    xim_has_focus = ਸਰਗਰਮ;
	}
	ਵਾਪਸ;
    }
#  endif

    ਜੇ (xic == ਨਿਰਾ)
	ਵਾਪਸ;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_ਸਰਗਰਮ = ਸਰਗਰਮ;
    xim_ਸੈੱਟ_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    ਖਾਲੀ
xim_ਸੈੱਟ_focus(ਪੂਰਨ focus)
{
    ਜੇ (xic == ਨਿਰਾ)
	ਵਾਪਸ;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    ਜੇ (focus && xim_is_ਸਰਗਰਮ)
    {
	ਜੇ (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    ਖਾਲੀ
im_ਸੈੱਟ_position(ਪੂਰਨ ਪੰਕਤੀ ਅਣਵਰਤਿਆ, ਪੂਰਨ ਖਾਨਾ ਅਣਵਰਤਿਆ)
{
    xim_ਸੈੱਟ_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    ਖਾਲੀ
xim_ਸੈੱਟ_preedit(ਖਾਲੀ)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    ਪੂਰਨ ਸਤਰ_space;

    ਜੇ (xic == ਨਿਰਾ)
	ਵਾਪਸ;

    xim_ਸੈੱਟ_focus(TRUE);

    ਜੇ (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							ਨਿਰਾ);
	XSetICValues(xic, XNPreedit_ਗੁਣ, attr_list, ਨਿਰਾ);
	XFree(attr_list);
	ਵਾਪਸ;
    }

    ਜੇ (ਨਿਵੇਸ਼_style & XIMPreeditPosition)
    {
	ਜੇ (xim_fg_color == INVALCOLOR)
	{
	    xim_fg_color = gui.def_norm_pixel;
	    xim_bg_color = gui.def_ਪਿੱਠ_pixel;
	}
	over_spot.x = ਪਾਠ_X(gui.ਖਾਨਾ);
	over_spot.y = ਪਾਠ_Y(gui.ਪੰਕਤੀ);
	spot_area.x = 0;
	spot_area.y = 0;
	spot_area.ਉਚਾਈ = gui.char_ਉਚਾਈ * ਪੰਕਤੀਆਂ;
	spot_area.ਚੌੜਾਈ  = gui.char_ਚੌੜਾਈ * Columns;
	ਸਤਰ_space = gui.char_ਉਚਾਈ;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, ਸਤਰ_space,
					ਨਿਰਾ);
	ਜੇ (XSetICValues(xic, XNPreedit_ਗੁਣ, attr_list, ਨਿਰਾ))
	    emsg(_(e_cannot_ਸੈੱਟ_ic_ਮੁੱਲਾਂ));
	XFree(attr_list);
    }
}

#  ਜੇ defined(FEAT_GUI_X11)
#   ਜੇ defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

ਸਥਿਰ ਪੂਰਨ xim_real_init(ਖਿੜਕੀ x11_ਖਿੜਕੀ, Display *x11_display);


#   ifdef USE_X11R6_XIM
    ਸਥਿਰ ਖਾਲੀ
xim_instantiate_cb(
    Display	*display,
    XPointer	client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ,
    XPointer	call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਖਿੜਕੀ	x11_ਖਿੜਕੀ;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_instantiate_cb()\n");
#    endif

    gui_get_x11_windis(&x11_ਖਿੜਕੀ, &x11_display);
    ਜੇ (display != x11_display)
	ਵਾਪਸ;

    xim_real_init(x11_ਖਿੜਕੀ, x11_display);
    gui_ਸੈੱਟ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    ਜੇ (xic != ਨਿਰਾ)
	XUnregisterIMInstantiateCallback(x11_display, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
					 xim_instantiate_cb, ਨਿਰਾ);
}

    ਸਥਿਰ ਖਾਲੀ
xim_destroy_cb(
    XIM		im ਅਣਵਰਤਿਆ,
    XPointer	client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ,
    XPointer	call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਖਿੜਕੀ	x11_ਖਿੜਕੀ;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_destroy_cb()\n");
#    endif
    gui_get_x11_windis(&x11_ਖਿੜਕੀ, &x11_display);

    xic = ਨਿਰਾ;
    ਸਥਿਤੀ_area_enabled = FALSE;

    gui_ਸੈੱਟ_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
				   xim_instantiate_cb, ਨਿਰਾ);
}
#   endif

    ਖਾਲੀ
xim_init(ਖਾਲੀ)
{
    ਖਿੜਕੀ	x11_ਖਿੜਕੀ;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_ਖਿੜਕੀ, &x11_display);

    xic = ਨਿਰਾ;

    ਜੇ (xim_real_init(x11_ਖਿੜਕੀ, x11_display))
	ਵਾਪਸ;

    gui_ਸੈੱਟ_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
				   xim_instantiate_cb, ਨਿਰਾ);
#   endif
}

    ਸਥਿਰ ਪੂਰਨ
xim_real_init(ਖਿੜਕੀ x11_ਖਿੜਕੀ, Display *x11_display)
{
    ਪੂਰਨ		i;
    ਅੱਖਰ	*p,
		*s,
		*ns,
		*ਸਿਰਾ,
		tmp[1024];
#   define IMLEN_MAX 40
    ਅੱਖਰ	buf[IMLEN_MAX + 7];
    XIM		xim = ਨਿਰਾ;
    XIMStyles	*xim_styles;
    XIMStyle	this_ਨਿਵੇਸ਼_style = 0;
    Boolean	ਮਿਲਿਆ;
    XPoint	over_spot;
    XVaNestedList preedit_list, ਸਥਿਤੀ_list;

    ਨਿਵੇਸ਼_style = 0;
    ਸਥਿਤੀ_area_enabled = FALSE;

    ਜੇ (xic != ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਜੇ (gui.rsrc_ਨਿਵੇਸ਼_ਵਿਧੀ != ਨਿਰਾ && *gui.rsrc_ਨਿਵੇਸ਼_ਵਿਧੀ != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_ਨਿਵੇਸ਼_ਵਿਧੀ,
	    ਆਕਾਰ(tmp) - 1);
	ਲਈ (ns = s = tmp; ns != ਨਿਰਾ && *s != NUL;)
	{
	    s = (ਅੱਖਰ *)skipwhite((char_u *)s);
	    ਜੇ (*s == NUL)
		ਰੋਕੋ;
	    ਜੇ ((ns = ਸਿਰਾ = strchr(s, ',')) == ਨਿਰਾ)
		ਸਿਰਾ = s + ਲੜੀ_ਲੰਬਾਈ(s);
	    ਜਦੋਂ (ਸੁਰੱਖਿਅਤ_isspace(ਸਿਰਾ[-1]))
		ਸਿਰਾ--;
	    *ਸਿਰਾ = NUL;

	    ਜੇ (ਲੜੀ_ਲੰਬਾਈ(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		ਜੇ ((p = XSetLocaleModifiers(buf)) != ਨਿਰਾ && *p != NUL
			&& (xim = XOpenIM(x11_display, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ))
								      != ਨਿਰਾ)
		    ਰੋਕੋ;
	    }

	    s = ns + 1;
	}
    }

    ਜੇ (xim == ਨਿਰਾ && (p = XSetLocaleModifiers("")) != ਨਿਰਾ && *p != NUL)
	xim = XOpenIM(x11_display, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    ਜੇ (xim == ਨਿਰਾ && (p = XSetLocaleModifiers("@im=none")) != ਨਿਰਾ
								 && *p != NUL)
	xim = XOpenIM(x11_display, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);

    ਜੇ (xim == ਨਿਰਾ)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	ਜੇ (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_ਮੰਜ਼ਿਲ_open_ਨਿਵੇਸ਼_ਵਿਧੀ));
	    verbose_leave();
	}
	ਵਾਪਸ FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_ਅੰਕੜੇ_2 = ਨਿਰਾ;
	ਜੇ (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, ਨਿਰਾ))
	    emsg(_(e_warning_could_not_ਸੈੱਟ_destroy_callback_ਮੰਜ਼ਿਲ_im));
    }
#   endif

    ਜੇ (XGetIMValues(xim, XNQuery_ਨਿਵੇਸ਼_Style, &xim_styles, ਨਿਰਾ) || !xim_styles)
    {
	emsg(_(e_ਨਿਵੇਸ਼_ਵਿਧੀ_doesnt_support_any_style));
	XCloseIM(xim);
	ਵਾਪਸ FALSE;
    }

    ਮਿਲਿਆ = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    ਆਕਾਰ(tmp) - 1);
    ਲਈ (s = tmp; s && !ਮਿਲਿਆ; )
    {
	ਜਦੋਂ (*s && ਸੁਰੱਖਿਅਤ_isspace(*s))
	    s++;
	ਜੇ (!*s)
	    ਰੋਕੋ;
	ਜੇ ((ns = ਸਿਰਾ = strchr(s, ',')) != 0)
	    ns++;
	ਨਹੀਂਤਾਂ
	    ਸਿਰਾ = s + ਲੜੀ_ਲੰਬਾਈ(s);
	ਜਦੋਂ (ਸੁਰੱਖਿਅਤ_isspace(*ਸਿਰਾ))
	    ਸਿਰਾ--;
	*ਸਿਰਾ = '\0';

	ਜੇ (!strcmp(s, "OverTheSpot"))
	    this_ਨਿਵੇਸ਼_style = (XIMPreeditPosition | XIMStatusArea);
	ਨਹੀਂਤਾਂ ਜੇ (!strcmp(s, "OffTheSpot"))
	    this_ਨਿਵੇਸ਼_style = (XIMPreeditArea | XIMStatusArea);
	ਨਹੀਂਤਾਂ ਜੇ (!strcmp(s, "Root"))
	    this_ਨਿਵੇਸ਼_style = (XIMPreeditNothing | XIMStatusNothing);

	ਲਈ (i = 0; (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)i < xim_styles->ਗਿਣਤੀ_styles; i++)
	{
	    ਜੇ (this_ਨਿਵੇਸ਼_style == xim_styles->supported_styles[i])
	    {
		ਮਿਲਿਆ = True;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (!ਮਿਲਿਆ)
	    ਲਈ (i = 0; (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)i < xim_styles->ਗਿਣਤੀ_styles; i++)
	    {
		ਜੇ ((xim_styles->supported_styles[i] & this_ਨਿਵੇਸ਼_style)
			== (this_ਨਿਵੇਸ਼_style & ~XIMStatusArea))
		{
		    this_ਨਿਵੇਸ਼_style &= ~XIMStatusArea;
		    ਮਿਲਿਆ = True;
		    ਰੋਕੋ;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    ਜੇ (!ਮਿਲਿਆ)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	ਜੇ (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_ਨਿਵੇਸ਼_ਵਿਧੀ_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	ਵਾਪਸ FALSE;
    }

    over_spot.x = ਪਾਠ_X(gui.ਖਾਨਾ);
    over_spot.y = ਪਾਠ_Y(gui.ਪੰਕਤੀ);
    ਨਿਵੇਸ਼_style = this_ਨਿਵੇਸ਼_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    ਜੇ (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_ਪਿੱਠ_pixel,
				XNFont_ਸੈੱਟ, (XFont_ਸੈੱਟ)gui.fontset,
				ਨਿਰਾ);
	ਸਥਿਤੀ_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_ਪਿੱਠ_pixel,
				XNFont_ਸੈੱਟ, (XFont_ਸੈੱਟ)gui.fontset,
				ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_ਪਿੱਠ_pixel,
				ਨਿਰਾ);
	ਸਥਿਤੀ_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_ਪਿੱਠ_pixel,
				ਨਿਰਾ);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, ਨਿਵੇਸ਼_style,
		    XNClient_ਖਿੜਕੀ, x11_ਖਿੜਕੀ,
		    XNFocus_ਖਿੜਕੀ, gui.wid,
		    XNPreedit_ਗੁਣ, preedit_list,
		    XNStatus_ਗੁਣ, ਸਥਿਤੀ_list,
		    ਨਿਰਾ);
    XFree(ਸਥਿਤੀ_list);
    XFree(preedit_list);
    ਜੇ (xic != ਨਿਰਾ)
    {
	ਜੇ (ਨਿਵੇਸ਼_style & XIMStatusArea)
	{
	    xim_ਸੈੱਟ_ਸਥਿਤੀ_area();
	    ਸਥਿਤੀ_area_enabled = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    gui_ਸੈੱਟ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!is_not_a_ਪਦ())
	    emsg(_(e_failed_ਮੰਜ਼ਿਲ_create_ਨਿਵੇਸ਼_context));
	XCloseIM(xim);
	ਵਾਪਸ FALSE;
    }

    ਵਾਪਸ TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    ਪੂਰਨ
im_get_ਸਥਿਤੀ(ਖਾਲੀ)
{
#  ifdef FEAT_EVAL
    ਜੇ (USE_IMSTATUSFUNC)
	ਵਾਪਸ call_imstatusfunc();
#  endif
    ਵਾਪਸ xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# ਜੇ !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    ਖਾਲੀ
xim_ਸੈੱਟ_ਸਥਿਤੀ_area(ਖਾਲੀ)
{
    XVaNestedList preedit_list = 0, ਸਥਿਤੀ_list = 0, list = 0;
    XRectangle pre_area, ਸਥਿਤੀ_area;

    ਜੇ (xic == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (ਨਿਵੇਸ਼_style & XIMStatusArea)
    {
	ਜੇ (ਨਿਵੇਸ਼_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    ਸਥਿਤੀ_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, ਨਿਰਾ);
	    XGetICValues(xic, XNStatus_ਗੁਣ, ਸਥਿਤੀ_list, ਨਿਰਾ);
	    XFree(ਸਥਿਤੀ_list);

	    ਸਥਿਤੀ_area.ਚੌੜਾਈ = needed_rect->ਚੌੜਾਈ;
	}
	ਨਹੀਂਤਾਂ
	    ਸਥਿਤੀ_area.ਚੌੜਾਈ = gui.char_ਚੌੜਾਈ * Columns;

	ਸਥਿਤੀ_area.x = 0;
	ਸਥਿਤੀ_area.y = gui.char_ਉਚਾਈ * ਪੰਕਤੀਆਂ + gui.border_ਵਿੱਥ;
	ਜੇ (gui.which_scrollbars[SBAR_BOTTOM])
	    ਸਥਿਤੀ_area.y += gui.scrollbar_ਉਚਾਈ;
#  ifdef FEAT_MENU
	ਜੇ (gui.menu_is_ਸਰਗਰਮ)
	    ਸਥਿਤੀ_area.y += gui.menu_ਉਚਾਈ;
#  endif
	ਸਥਿਤੀ_area.ਉਚਾਈ = gui.char_ਉਚਾਈ;
	ਸਥਿਤੀ_list = XVaCreateNestedList(0, XNArea, &ਸਥਿਤੀ_area, ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਸਥਿਤੀ_area.x = 0;
	ਸਥਿਤੀ_area.y = gui.char_ਉਚਾਈ * ਪੰਕਤੀਆਂ + gui.border_ਵਿੱਥ;
	ਜੇ (gui.which_scrollbars[SBAR_BOTTOM])
	    ਸਥਿਤੀ_area.y += gui.scrollbar_ਉਚਾਈ;
#  ifdef FEAT_MENU
	ਜੇ (gui.menu_is_ਸਰਗਰਮ)
	    ਸਥਿਤੀ_area.y += gui.menu_ਉਚਾਈ;
#  endif
	ਸਥਿਤੀ_area.ਚੌੜਾਈ = 0;
	ਸਥਿਤੀ_area.ਉਚਾਈ = gui.char_ਉਚਾਈ;
    }

    ਜੇ (ਨਿਵੇਸ਼_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = ਸਥਿਤੀ_area.x + ਸਥਿਤੀ_area.ਚੌੜਾਈ;
	pre_area.y = gui.char_ਉਚਾਈ * ਪੰਕਤੀਆਂ + gui.border_ਵਿੱਥ;
	pre_area.ਚੌੜਾਈ = gui.char_ਚੌੜਾਈ * Columns - pre_area.x;
	ਜੇ (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_ਉਚਾਈ;
#  ifdef FEAT_MENU
	ਜੇ (gui.menu_is_ਸਰਗਰਮ)
	    pre_area.y += gui.menu_ਉਚਾਈ;
#  endif
	pre_area.ਉਚਾਈ = gui.char_ਉਚਾਈ;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਨਿਵੇਸ਼_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.ਉਚਾਈ = gui.char_ਉਚਾਈ * ਪੰਕਤੀਆਂ;
	pre_area.ਚੌੜਾਈ = gui.char_ਚੌੜਾਈ * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ਨਿਰਾ);
    }

    ਜੇ (preedit_list && ਸਥਿਤੀ_list)
	list = XVaCreateNestedList(0, XNPreedit_ਗੁਣ, preedit_list,
				   XNStatus_ਗੁਣ, ਸਥਿਤੀ_list, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ ਜੇ (preedit_list)
	list = XVaCreateNestedList(0, XNPreedit_ਗੁਣ, preedit_list,
				   ਨਿਰਾ);
    ਨਹੀਂਤਾਂ ਜੇ (ਸਥਿਤੀ_list)
	list = XVaCreateNestedList(0, XNStatus_ਗੁਣ, ਸਥਿਤੀ_list,
				   ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
	list = ਨਿਰਾ;

    ਜੇ (list)
    {
	XSetICValues(xic, XNVaNestedList, list, ਨਿਰਾ);
	XFree(list);
    }
    ਜੇ (ਸਥਿਤੀ_list)
	XFree(ਸਥਿਤੀ_list);
    ਜੇ (preedit_list)
	XFree(preedit_list);
}

    ਪੂਰਨ
xim_get_ਸਥਿਤੀ_area_ਉਚਾਈ(ਖਾਲੀ)
{
    ਜੇ (ਸਥਿਤੀ_area_enabled)
	ਵਾਪਸ gui.char_ਉਚਾਈ;
    ਵਾਪਸ 0;
}
# endif

#ਨਹੀਂਤਾਂ // !defined(FEAT_XIM)

# ਜੇ defined(IME_WITHOUT_XIM) || defined(VIMDLL)
ਸਥਿਰ ਪੂਰਨ im_was_ਸੈੱਟ_ਸਰਗਰਮ = FALSE;

    ਪੂਰਨ
#  ifdef VIMDLL
mbyte_im_get_ਸਥਿਤੀ(ਖਾਲੀ)
#  ਨਹੀਂਤਾਂ
im_get_ਸਥਿਤੀ(ਖਾਲੀ)
#  endif
{
#  ਜੇ defined(FEAT_EVAL)
    ਜੇ (USE_IMSTATUSFUNC)
	ਵਾਪਸ call_imstatusfunc();
#  endif
    ਵਾਪਸ im_was_ਸੈੱਟ_ਸਰਗਰਮ;
}

    ਖਾਲੀ
#  ifdef VIMDLL
mbyte_im_ਸੈੱਟ_ਸਰਗਰਮ(ਪੂਰਨ ਸਰਗਰਮ_arg)
#  ਨਹੀਂਤਾਂ
im_ਸੈੱਟ_ਸਰਗਰਮ(ਪੂਰਨ ਸਰਗਰਮ_arg)
#  endif
{
#  ਜੇ defined(FEAT_EVAL)
    ਪੂਰਨ	    ਸਰਗਰਮ = !p_imdisable && ਸਰਗਰਮ_arg;

    ਜੇ (USE_IMACTIVATEFUNC && ਸਰਗਰਮ != im_get_ਸਥਿਤੀ())
    {
	call_imactivatefunc(ਸਰਗਰਮ);
	im_was_ਸੈੱਟ_ਸਰਗਰਮ = ਸਰਗਰਮ;
    }
#  endif
}

#  ਜੇ defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    ਖਾਲੀ
im_ਸੈੱਟ_position(ਪੂਰਨ ਪੰਕਤੀ ਅਣਵਰਤਿਆ, ਪੂਰਨ ਖਾਨਾ ਅਣਵਰਤਿਆ)
{
}
#  endif
# endif

#ਜੇ_ਅੰਤ // FEAT_XIM
