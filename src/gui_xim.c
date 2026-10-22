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

#అన్తర్భావ "vim.h"

#ఒకవేళ !defined(GTK_CHECK_సంస్కరణ)
# define GTK_CHECK_సంస్కరణ(a, b, c) 0
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_సంస్కరణ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# లేకపోతే
#  include <gdk/gdkkeysyms.h>
# endif
# ఒకవేళ !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  లేకపోతే
#   include <gdk/gdkx.h>
#  endif
# endif
#అయితే_ముగింపు

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#ఒకవేళ defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
స్థిర శూన్య xim_log(అక్షరం *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    స్థిర శూన్య
xim_log(అక్షరం *s, ...)
{
    va_list arglist;
    స్థిర FILE *fd = ఖాళీ;

    ఒకవేళ (fd == (FILE *)-1)
	తిరిగి;
    ఒకవేళ (fd == ఖాళీ)
    {
	fd = mch_fopen("xim.log", "w");
	ఒకవేళ (fd == ఖాళీ)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    తిరిగి;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.లోపల_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.లోపల_use && *p_imsf != NUL)
#లేకపోతే
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
స్థిర callback_T imaf_cb;	    // 'imactivatefunc' callback function
స్థిర callback_T imsf_cb;	    // 'imstatusfunc' callback function

    అక్షరం *
did_సమితి_imactivatefunc(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (option_సమితి_callback_func(p_imaf, &imaf_cb) == FAIL)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    అక్షరం *
did_సమితి_imstatusfunc(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (option_సమితి_callback_func(p_imsf, &imsf_cb) == FAIL)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    స్థిర శూన్య
call_imactivatefunc(పూర్ణాంకం చురుకైన)
{
    typval_T argv[2];
    పూర్ణాంకం save_కీలకం_Typed = కీలకం_Typed;

    argv[0].v_type = VAR_సంఖ్య;
    argv[0].vval.v_సంఖ్య = చురుకైన ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (శూన్య)call_callback_retnr(&imaf_cb, 1, argv);

    కీలకం_Typed = save_కీలకం_Typed;
}

    స్థిర పూర్ణాంకం
call_imstatusfunc(శూన్య)
{
    పూర్ణాంకం is_చురుకైన;
    పూర్ణాంకం save_కీలకం_Typed = కీలకం_Typed;

    // FIXME: Don't execute user function in unsafe situation.
    ఒకవేళ (exiting || is_autocmd_blocked())
	తిరిగి FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++సందేశం_silent;
    is_చురుకైన = call_callback_retnr(&imsf_cb, 0, ఖాళీ);
    --సందేశం_silent;

    కీలకం_Typed = save_కీలకం_Typed;
    తిరిగి (is_చురుకైన > 0);
}
#అయితే_ముగింపు

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_xim_stuff(శూన్య)
{
# ఒకవేళ defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_im_funcs(పూర్ణాంకం copyID వాడని)
{
    పూర్ణాంకం abort = FALSE;

# ఒకవేళ defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = సమితి_ref_లోపల_callback(&imaf_cb, copyID);
    abort = abort || సమితి_ref_లోపల_callback(&imsf_cb, copyID);
# endif

    తిరిగి abort;
}
#అయితే_ముగింపు


#ఒకవేళ defined(FEAT_XIM)

# ఒకవేళ defined(FEAT_GUI_GTK)
స్థిర పూర్ణాంకం xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    స్థిర శూన్య
init_preedit_ప్రారంభం_నిలువు(శూన్య)
{
    ఒకవేళ (State & బహుళకం_CMDLINE)
	preedit_ప్రారంభం_నిలువు = cmdline_getvcol_cursor();
    లేకపోతే ఒకవేళ (curwin != ఖాళీ && curwin->w_బఫరు != ఖాళీ)
	getvcol(curwin, &curwin->w_cursor, &preedit_ప్రారంభం_నిలువు, ఖాళీ, ఖాళీ, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_మారింది_while_preediting = curbuf->b_మారింది;
}

స్థిర పూర్ణాంకం im_is_చురుకైన	       = FALSE;	// IM is enabled for current mode
స్థిర పూర్ణాంకం preedit_is_చురుకైన   = FALSE;
స్థిర పూర్ణాంకం im_preedit_cursor   = 0;	// cursor offset in characters
స్థిర పూర్ణాంకం im_preedit_trailing = 0;	// number of characters after cursor

స్థిర గుర్తులేని దీర్ఘ im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_కీలకం_Void_చిహ్నం
#    define GDK_కీలకం_Void_చిహ్నం GDK_Void_చిహ్నం
#   endif
స్థిర గుర్తులేని పూర్ణాంకం  im_activatekey_keyval = GDK_కీలకం_Void_చిహ్నం;
స్థిర గుర్తులేని పూర్ణాంకం  im_activatekey_state  = 0;
#  endif

స్థిర GtkWidget *preedit_కిటికీ = ఖాళీ;
స్థిర GtkWidget *preedit_label = ఖాళీ;
#  ifdef USE_GTK4
స్థిర పూర్ణాంకం preedit_label_వెడల్పు = 0;	// last measured popover natural width
స్థిర పూర్ణాంకం preedit_popover_ఎత్తు = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
స్థిర GtkCssProvider *preedit_css_provider = ఖాళీ;
స్థిర guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

స్థిర శూన్య im_preedit_కిటికీ_సమితి_position(శూన్య);

    శూన్య
im_సమితి_చురుకైన(పూర్ణాంకం చురుకైన)
{
    పూర్ణాంకం was_చురుకైన;

    was_చురుకైన = !!im_get_స్థితి();
    im_is_చురుకైన = (చురుకైన && !p_imdisable);

    ఒకవేళ (im_is_చురుకైన != was_చురుకైన)
	xim_reset();
}

    శూన్య
xim_సమితి_focus(పూర్ణాంకం focus)
{
    ఒకవేళ (xic == ఖాళీ)
	తిరిగి;

    ఒకవేళ (focus)
	gtk_im_context_focus_లోపల(xic);
    లేకపోతే
	gtk_im_context_focus_బహిర్గతం(xic);
}

    శూన్య
im_సమితి_position(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    ఒకవేళ (xic == ఖాళీ)
	తిరిగి;

    GdkRectangle area;

    area.x = FILL_X(నిలువు);
    area.y = FILL_Y(అడ్డువరుస);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.వెడల్పు  = gui.char_వెడల్పు
		    * ((Screen_పంక్తులు != ఖాళీ && mb_lefthalve(అడ్డువరుస, నిలువు)) ? 2 : 1);
    area.ఎత్తు = gui.char_ఎత్తు;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    ఒకవేళ (p_imst == IM_OVER_THE_SPOT)
    {
	పూర్ణాంకం popover_h = preedit_popover_ఎత్తు;
	ఒకవేళ (popover_h <= 0)
	    popover_h = gui.char_ఎత్తు;
	ఒకవేళ (popover_h > area.ఎత్తు)
	    area.ఎత్తు = popover_h;
    }
#  endif

    gtk_im_context_సమితి_cursor_location(xic, &area);

    ఒకవేళ (p_imst == IM_OVER_THE_SPOT)
	im_preedit_కిటికీ_సమితి_position();
}

#  ఒకవేళ 0 || defined(PROTO) // apparently only used in gui_x11.c
    శూన్య
xim_సమితి_preedit(శూన్య)
{
    im_సమితి_position(gui.అడ్డువరుస, gui.నిలువు);
}
#  endif

    స్థిర శూన్య
im_కూడు_గమ్యం_ఆదానం(char_u *str, పూర్ణాంకం len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    ఒకవేళ (ఆదానం_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&ఆదానం_conv, str, &len);
	g_return_if_fail(str != ఖాళీ);
    }

    కూడు_గమ్యం_ఆదానం_buf_csi(str, len);

    ఒకవేళ (ఆదానం_conv.vc_type != CONV_NONE)
	vim_free(str);

    ఒకవేళ (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    స్థిర శూన్య
im_preedit_కిటికీ_సమితి_position(శూన్య)
{
    ఒకవేళ (preedit_కిటికీ == ఖాళీ)
	తిరిగి;

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
    graphene_బిందువు_t pt_లోపల, pt_బహిర్గతం;

    pt_లోపల.x = FILL_X(gui.నిలువు);
    pt_లోపల.y = FILL_Y(gui.అడ్డువరుస);
    ఒకవేళ (!gtk_widget_compute_బిందువు(gui.drawarea, gui.mainwin,
						&pt_లోపల, &pt_బహిర్గతం))
    {
	pt_బహిర్గతం.x = pt_లోపల.x;
	pt_బహిర్గతం.y = pt_లోపల.y;
    }
    rect.x = (పూర్ణాంకం)pt_బహిర్గతం.x;
    rect.y = (పూర్ణాంకం)pt_బహిర్గతం.y;
    rect.వెడల్పు = preedit_label_వెడల్పు;
    rect.ఎత్తు = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    పూర్ణాంకం స్థానభ్రంశం = gui.char_ఎత్తు - preedit_popover_ఎత్తు;
    gtk_popover_సమితి_స్థానభ్రంశం(GTK_POPOVER(preedit_కిటికీ), 0, స్థానభ్రంశం);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_సమితి_halign(preedit_కిటికీ, GTK_ALIGN_ప్రారంభం);
    gtk_popover_సమితి_pointing_గమ్యం(GTK_POPOVER(preedit_కిటికీ), &rect);
#  లేకపోతే
    పూర్ణాంకం x, y, వెడల్పు, ఎత్తు;
    పూర్ణాంకం screen_x, screen_y, screen_వెడల్పు, screen_ఎత్తు;

    gui_gtk_get_screen_geom_యొక్క_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_వెడల్పు, &screen_ఎత్తు);
    gdk_కిటికీ_get_origin(gtk_widget_get_కిటికీ(gui.drawarea), &x, &y);
    gtk_కిటికీ_get_కొలత(GTK_కిటికీ(preedit_కిటికీ), &వెడల్పు, &ఎత్తు);
    x = x + FILL_X(gui.నిలువు);
    y = y + FILL_Y(gui.అడ్డువరుస);
    ఒకవేళ (x + వెడల్పు > screen_x + screen_వెడల్పు)
	x = screen_x + screen_వెడల్పు - వెడల్పు;
    ఒకవేళ (y + ఎత్తు > screen_y + screen_ఎత్తు)
	y = screen_y + screen_ఎత్తు - ఎత్తు;
    gtk_కిటికీ_move(GTK_కిటికీ(preedit_కిటికీ), x, y);
#  endif
}

    స్థిర శూన్య
im_preedit_కిటికీ_open(శూన్య)
{
    అక్షరం *preedit_string;
#  ఒకవేళ !GTK_CHECK_సంస్కరణ(3,16,0)
    అక్షరం buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
#   ఒకవేళ !GTK_CHECK_సంస్కరణ(3,16,0)
    GdkRGBA color;
#   endif
#  లేకపోతే
    GdkColor color;
#  endif
    gint w, h;

    ఒకవేళ (preedit_కిటికీ == ఖాళీ)
    {
#  ifdef USE_GTK4
	// GTK4: use GtkPopover (xdg_popup) so the compositor keeps keyboard
	// focus on the drawing area and does not disable text-input-v3.
	// See issue #20257.  Parent the popover to gui.mainwin (a real
	// container) rather than gui.drawarea: GtkDrawingArea is a leaf
	// widget and using it as a popover parent causes snapshot artifacts
	// (white-out) on the drawing area.
	preedit_కిటికీ = gtk_popover_new();
	gtk_popover_సమితి_autohide(GTK_POPOVER(preedit_కిటికీ), FALSE);
	gtk_popover_సమితి_has_arrow(GTK_POPOVER(preedit_కిటికీ), FALSE);
	// Pin the popover below the anchor.  Together with anchor.h == 0 in
	// im_preedit_window_set_position() this lands the popover's top edge
	// exactly on FILL_Y(row), so the popover covers the cursor cell rather
	// than landing above it (the default GTK_POS_TOP would do that and
	// only flip to bottom when there is no room above, which is flaky).
	gtk_popover_సమితి_position(GTK_POPOVER(preedit_కిటికీ), GTK_POS_BOTTOM);
	gtk_widget_సమితి_can_focus(preedit_కిటికీ, FALSE);
	gtk_widget_సమితి_focusable(preedit_కిటికీ, FALSE);
	gtk_widget_సమితి_can_లక్ష్యం(preedit_కిటికీ, FALSE);
	gtk_widget_సమితి_మాతృ(preedit_కిటికీ, gui.mainwin);
#  లేకపోతే
	preedit_కిటికీ = gtk_కిటికీ_new(GTK_కిటికీ_POPUP);
	gtk_కిటికీ_సమితి_transient_for(GTK_కిటికీ(preedit_కిటికీ),
						     GTK_కిటికీ(gui.mainwin));
#  endif
	preedit_label = gtk_label_new("");
	gtk_widget_సమితి_name(preedit_label, "vim-gui-preedit-area");
#  ifdef USE_GTK4
	gtk_label_సమితి_xalign(GTK_LABEL(preedit_label), 0.0);
	gtk_widget_సమితి_halign(preedit_label, GTK_ALIGN_ప్రారంభం);
	gtk_widget_సమితి_valign(preedit_label, GTK_ALIGN_ప్రారంభం);
	// Tag the popover with a CSS class so the minimal frame CSS below
	// (no border, no rounded corners, no shadow, no padding) applies
	// only to our preedit popover and not other popovers in the app.
	gtk_widget_కూడు_css_class(preedit_కిటికీ, "vim-preedit-popover");
	gtk_popover_సమితి_బిడ్డ(GTK_POPOVER(preedit_కిటికీ), preedit_label);
#  లేకపోతే
	gtk_container_కూడు(GTK_CONTAINER(preedit_కిటికీ), preedit_label);
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
    ఒకవేళ (preedit_css_provider == ఖాళీ || preedit_css_cached_bg != gui.వెనుక_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	ఒకవేళ (preedit_css_provider != ఖాళీ)
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
		(గుర్తులేని దీర్ఘ)((gui.వెనుక_pixel >> 16) & 0xff),
		(గుర్తులేని దీర్ఘ)((gui.వెనుక_pixel >> 8) & 0xff),
		(గుర్తులేని దీర్ఘ)(gui.వెనుక_pixel & 0xff));
	gtk_css_provider_ఎక్కించు_from_string(preedit_css_provider, css);
	g_free(css);
	gtk_style_context_కూడు_provider_for_display(display,
			    GTK_STYLE_PROVIDER(preedit_css_provider), G_MAXUINT);
	preedit_css_cached_bg = gui.వెనుక_pixel;
    }
#  endif

#  ifndef USE_GTK4
#   ఒకవేళ GTK_CHECK_సంస్కరణ(3,16,0)
    {
	GtkStyleContext * స్థిరరాశి context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * స్థిరరాశి provider = gtk_css_provider_new();
	gchar		   *css = ఖాళీ;
	స్థిరరాశి అక్షరం * స్థిరరాశి fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_కొలత(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = ఖాళీ;

	ఒకవేళ (!pango_font_description_get_కొలత_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * స్థిరరాశి screen
		  = gdk_కిటికీ_get_screen(gtk_widget_get_కిటికీ(gui.mainwin));
	    స్థిరరాశి gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	ఒకవేళ (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	లేకపోతే
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != ఖాళీ ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.వెనుక_pixel >> 16) & 0xff,
		(gui.వెనుక_pixel >> 8) & 0xff,
		gui.వెనుక_pixel & 0xff);

	gtk_css_provider_ఎక్కించు_from_దత్తాంశం(provider, css, -1, ఖాళీ);
	gtk_style_context_కూడు_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_సంస్కరణ(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, పరిమాణం(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_సాధారణ, &color);

    vim_snprintf(buf, పరిమాణం(buf), "#%06X", gui.వెనుక_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_సాధారణ,
								      &color);
#   లేకపోతే
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, పరిమాణం(buf), "#%06X", (గుర్తులేని)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_సాధారణ, &color);

    vim_snprintf(buf, పరిమాణం(buf), "#%06X", (గుర్తులేని)gui.వెనుక_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_కిటికీ, GTK_STATE_సాధారణ, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ఖాళీ);

    ఒకవేళ (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	ఒకవేళ (attr_list == ఖాళీ)
	    attr_list = pango_attr_list_new();

	ఒకవేళ (gui.norm_font != ఖాళీ)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->ప్రారంభం_సూచీ = 0;
	    pa->చివర_సూచీ = G_MAXUINT;
	    pango_attr_list_చొప్పించు(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->ప్రారంభం_సూచీ = 0;
	pa->చివర_సూచీ = G_MAXUINT;
	pango_attr_list_చొప్పించు(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.వెనుక_pixel >> 16) & 0xff) * 257,
		((gui.వెనుక_pixel >> 8) & 0xff) * 257,
		(gui.వెనుక_pixel & 0xff) * 257);
	pa->ప్రారంభం_సూచీ = 0;
	pa->చివర_సూచీ = G_MAXUINT;
	pango_attr_list_చొప్పించు(attr_list, pa);
#  endif
	gtk_label_సమితి_పాఠ్యం(GTK_LABEL(preedit_label), preedit_string);
	gtk_label_సమితి_లక్షణాలు(GTK_LABEL(preedit_label), attr_list);

	layout = gtk_label_get_layout(GTK_LABEL(preedit_label));
	pango_layout_get_pixel_కొలత(layout, &w, &h);
	h = MAX(h, gui.char_ఎత్తు);
#  ifdef USE_GTK4
	// Cache label/popover size derived from the Pango layout, which is
	// available without mapping the popover.  Using these for positioning
	// avoids calling gtk_popover_set_pointing_to() after popup, which
	// triggers a GDK "compositor doesn't support moving popups" warning
	// on compositors without xdg_popup.reposition (e.g. Weston).
	preedit_label_వెడల్పు = w;
	preedit_popover_ఎత్తు = h;

	// Report an enlarged cursor rectangle that covers both the cursor
	// cell and the preedit popover.  This way the IM (e.g. fcitx5)
	// places its candidate window below the popover instead of on top
	// of it.  Coordinates are in client-widget (drawarea) space.
	ఒకవేళ (xic != ఖాళీ)
	{
	    GdkRectangle area;

	    area.x = FILL_X(gui.నిలువు);
	    area.y = FILL_Y(gui.అడ్డువరుస);
	    area.వెడల్పు = preedit_label_వెడల్పు;
	    area.ఎత్తు = MAX(gui.char_ఎత్తు, preedit_popover_ఎత్తు);
	    gtk_im_context_సమితి_cursor_location(xic, &area);
	}

	// Pop down before re-anchoring: on compositors that lack
	// xdg_popup.reposition (e.g. Weston), calling set_pointing_to() on
	// a mapped popover triggers GDK's remap fallback and a warning.
	// Doing the popdown ourselves makes the next popup land at the new
	// anchor cleanly without that warning.
	ఒకవేళ (gtk_widget_get_mapped(preedit_కిటికీ))
	    gtk_popover_popdown(GTK_POPOVER(preedit_కిటికీ));

	im_preedit_కిటికీ_సమితి_position();

	gtk_widget_వరుస_resize(preedit_కిటికీ);
	gtk_popover_popup(GTK_POPOVER(preedit_కిటికీ));
#  లేకపోతే
	gtk_కిటికీ_resize(GTK_కిటికీ(preedit_కిటికీ), w, h);
	gtk_widget_show_all(preedit_కిటికీ);

	im_preedit_కిటికీ_సమితి_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    స్థిర శూన్య
im_preedit_కిటికీ_close(శూన్య)
{
    ఒకవేళ (preedit_కిటికీ == ఖాళీ)
	తిరిగి;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_కిటికీ));
#  లేకపోతే
    gtk_widget_hide(preedit_కిటికీ);
#  endif
}

    స్థిర శూన్య
im_show_preedit(శూన్య)
{
    im_preedit_కిటికీ_open();

    ఒకవేళ (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    స్థిర శూన్య
im_delete_preedit(శూన్య)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    ఒకవేళ (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_కిటికీ_close();
	తిరిగి;
    }

    ఒకవేళ (State & బహుళకం_సాధారణ
#  ifdef FEAT_TERMINAL
	    && !పదం_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	తిరిగి;
    }
    కొరకు (; im_preedit_cursor > 0; --im_preedit_cursor)
	కూడు_గమ్యం_ఆదానం_buf(bskey, (పూర్ణాంకం)పరిమాణం(bskey));

    కొరకు (; im_preedit_trailing > 0; --im_preedit_trailing)
	కూడు_గమ్యం_ఆదానం_buf(delkey, (పూర్ణాంకం)పరిమాణం(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    స్థిర శూన్య
im_correct_cursor(పూర్ణాంకం అంకె_move_వెనుక)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    ఒకవేళ (State & బహుళకం_సాధారణ)
	తిరిగి;
#  ifdef FEAT_RIGHTLEFT
    ఒకవేళ ((State & బహుళకం_CMDLINE) == 0 && curwin != ఖాళీ && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    కొరకు (; అంకె_move_వెనుక > 0; --అంకె_move_వెనుక)
	కూడు_గమ్యం_ఆదానం_buf(backkey, (పూర్ణాంకం)పరిమాణం(backkey));
}

స్థిర పూర్ణాంకం xim_expected_char = NUL;
స్థిర పూర్ణాంకం xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    స్థిర శూన్య
im_show_info(శూన్య)
{
    పూర్ణాంకం	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    ఒకవేళ ((State & బహుళకం_సాధారణ) || (State & బహుళకం_చొప్పించు))
	setcursor();
    బహిర్గతం_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    స్థిర శూన్య
im_commit_cb(GtkIMContext *context వాడని,
	     స్థిరరాశి gchar *str,
	     gpointer దత్తాంశం వాడని)
{
    పూర్ణాంకం		slen = (పూర్ణాంకం)STRLEN(str);
    పూర్ణాంకం		కూడు_గమ్యం_ఆదానం = TRUE;
    పూర్ణాంకం		clen;
    పూర్ణాంకం		len = slen;
    పూర్ణాంకం		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    ఒకవేళ (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	ఒకవేళ (preedit_ప్రారంభం_నిలువు == MAXCOL)
	{
	    init_preedit_ప్రారంభం_నిలువు();
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
	ఒకవేళ (ఆదానం_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&ఆదానం_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != ఖాళీ);
	}
	లేకపోతే
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	ఒకవేళ (ఆదానం_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_ప్రారంభం_నిలువు += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    ఒకవేళ (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	ఒకవేళ (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    కూడు_గమ్యం_ఆదానం = FALSE;
	}
	లేకపోతే
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    ఒకవేళ (కూడు_గమ్యం_ఆదానం)
	im_కూడు_గమ్యం_ఆదానం((char_u *)str, slen);

    ఒకవేళ (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	ఒకవేళ (!commit_with_preedit)
	    preedit_ప్రారంభం_నిలువు = MAXCOL;

	// This flag is used in changed() at next call.
	xim_మారింది_while_preediting = TRUE;
    }

    ఒకవేళ (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    స్థిర శూన్య
im_preedit_ప్రారంభం_cb(GtkIMContext *context వాడని, gpointer దత్తాంశం వాడని)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_start_cb()\n");
#  endif

    im_is_చురుకైన = TRUE;
    preedit_is_చురుకైన = TRUE;
    gui_update_cursor(TRUE, FALSE);
    im_show_info();
}

/*
 * Callback invoked after end to the preedit.
 */
    స్థిర శూన్య
im_preedit_చివర_cb(GtkIMContext *context వాడని, gpointer దత్తాంశం వాడని)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    ఒకవేళ (p_imst == IM_ON_THE_SPOT)
	preedit_ప్రారంభం_నిలువు = MAXCOL;
    xim_has_preediting = FALSE;

#  ఒకవేళ 0
    // Removal of this line suggested by Takuhiro Nishioka.  Fixes that IM was
    // switched off unintentionally.  We now use preedit_is_active (added by
    // SungHyun Nam).
    im_is_చురుకైన = FALSE;
#  endif
    preedit_is_చురుకైన = FALSE;
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
    స్థిర శూన్య
im_preedit_మారింది_cb(GtkIMContext *context, gpointer దత్తాంశం వాడని)
{
    అక్షరం    *preedit_string = ఖాళీ;
    పూర్ణాంకం	    cursor_సూచీ    = 0;
    పూర్ణాంకం	    అంకె_move_వెనుక   = 0;
    char_u  *str;
    char_u  *p;
    పూర్ణాంకం	    i;

    ఒకవేళ (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ఖాళీ,
					  &cursor_సూచీ);
    లేకపోతే
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ఖాళీ,
					  ఖాళీ);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != ఖాళీ); // just in case

    ఒకవేళ (p_imst == IM_OVER_THE_SPOT)
    {
	ఒకవేళ (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	లేకపోతే
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    లేకపోతే
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	ఒకవేళ (preedit_ప్రారంభం_నిలువు == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_ప్రారంభం_నిలువు();
	}
	లేకపోతే ఒకవేళ (cursor_సూచీ == 0 && preedit_string[0] == '\0')
	{
	    xim_has_preediting = FALSE;

	    // If at the start position (after typing backspace)
	    // preedit_start_col must be reset.
	    preedit_ప్రారంభం_నిలువు = MAXCOL;
	}

	im_delete_preedit();

	// Compute the end of the preediting area: "preedit_end_col".
	// According to the documentation of
	// gtk_im_context_get_preedit_string(), the cursor_pos output argument
	// returns the offset in bytes.  This is unfortunately not true -- real
	// life shows the offset is in characters, and the GTK+ source code
	// agrees with me.  Will file a bug later.
	ఒకవేళ (preedit_ప్రారంభం_నిలువు != MAXCOL)
	    preedit_చివర_నిలువు = preedit_ప్రారంభం_నిలువు;
	str = (char_u *)preedit_string;
	కొరకు (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    పూర్ణాంకం is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    ఒకవేళ (!is_composing || p_deco)
	    {
		ఒకవేళ (i < cursor_సూచీ)
		    ++im_preedit_cursor;
		లేకపోతే
		    ++im_preedit_trailing;
	    }
	    ఒకవేళ (!is_composing && i >= cursor_సూచీ)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++అంకె_move_వెనుక;
	    }
	    ఒకవేళ (preedit_ప్రారంభం_నిలువు != MAXCOL)
		preedit_చివర_నిలువు += utf_ptr2cells(p);
	}

	ఒకవేళ (p > str)
	{
	    im_కూడు_గమ్యం_ఆదానం(str, (పూర్ణాంకం)(p - str));
	    im_correct_cursor(అంకె_move_వెనుక);
	}
    }

    g_free(preedit_string);

    ఒకవేళ (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    స్థిర పూర్ణాంకం
translate_pango_లక్షణాలు(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    పూర్ణాంకం		    char_attr = HL_సాధారణ;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    ఒకవేళ (attr != ఖాళీ && ((PangoAttrInt *)attr)->విలువ
						 != (పూర్ణాంకం)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_బరువు);
    ఒకవేళ (attr != ఖాళీ && ((PangoAttrInt *)attr)->విలువ >= (పూర్ణాంకం)PANGO_బరువు_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    ఒకవేళ (attr != ఖాళీ && ((PangoAttrInt *)attr)->విలువ
						   != (పూర్ణాంకం)PANGO_STYLE_సాధారణ)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    ఒకవేళ (attr != ఖాళీ)
    {
	స్థిరరాశి PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	ఒకవేళ ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    తిరిగి char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    పూర్ణాంకం
im_get_feedback_attr(పూర్ణాంకం నిలువు)
{
    అక్షరం	    *preedit_string = ఖాళీ;
    PangoAttrList   *attr_list	    = ఖాళీ;
    పూర్ణాంకం		    char_attr	    = -1;

    ఒకవేళ (xic == ఖాళీ)
	తిరిగి char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ఖాళీ);

    ఒకవేళ (preedit_string != ఖాళీ && attr_list != ఖాళీ)
    {
	పూర్ణాంకం idx;

	// Get the byte index as used by PangoAttrIterator
	కొరకు (idx = 0; నిలువు > 0 && preedit_string[idx] != '\0'; --నిలువు)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	ఒకవేళ (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    పూర్ణాంకం			ప్రారంభం, చివర;

	    char_attr = HL_సాధారణ;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    చేయి
	    {
		pango_attr_iterator_range(iter, &ప్రారంభం, &చివర);

		ఒకవేళ (idx >= ప్రారంభం && idx < చివర)
		    char_attr |= translate_pango_లక్షణాలు(iter);
	    }
	    అయితే (pango_attr_iterator_తదుపరి(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    ఒకవేళ (attr_list != ఖాళీ)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    తిరిగి char_attr;
}

    శూన్య
xim_init(శూన్య)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != ఖాళీ);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_కిటికీ(gui.drawarea) != ఖాళీ);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), ఖాళీ);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_మారింది_cb), ఖాళీ);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_ప్రారంభం_cb), ఖాళీ);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_చివర_cb), ఖాళీ);

#  ifdef USE_GTK4
    gtk_im_context_సమితి_client_widget(xic, gui.drawarea);

    // Seed the cursor location immediately.  GTK4's Wayland IM module
    // batches text-input-v3 state and sends it on the next commit cycle
    // (typically triggered by focus_in).  Without this seed the very
    // first commit would carry the protocol default (0, 0, 0, 0) for the
    // cursor rectangle, and the IM's candidate window for the very first
    // composition would appear at the top-left of the surface instead of
    // next to the cursor.
    im_సమితి_position(gui.అడ్డువరుస, gui.నిలువు);
#  లేకపోతే
    gtk_im_context_సమితి_client_కిటికీ(xic, gtk_widget_get_కిటికీ(gui.drawarea));
#  endif
}

    శూన్య
im_shutdown(శూన్య)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    ఒకవేళ (xic != ఖాళీ)
    {
	gtk_im_context_focus_బహిర్గతం(xic);
	g_object_unref(xic);
	xic = ఖాళీ;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    ఒకవేళ (preedit_కిటికీ != ఖాళీ)
    {
	gtk_widget_unparent(preedit_కిటికీ);
	preedit_కిటికీ = ఖాళీ;
	preedit_label = ఖాళీ;
    }
    ఒకవేళ (preedit_css_provider != ఖాళీ)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = ఖాళీ;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_చురుకైన = FALSE;
    im_commit_handler_id = 0;
    ఒకవేళ (p_imst == IM_ON_THE_SPOT)
	preedit_ప్రారంభం_నిలువు = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
/*
 * Convert the string argument to keyval and state for GdkEventKey.
 * If str is valid return TRUE, otherwise FALSE.
 *
 * See 'imactivatekey' for documentation of the format.
 */
    స్థిర పూర్ణాంకం
im_string_గమ్యం_keyval(స్థిరరాశి అక్షరం *str, గుర్తులేని పూర్ణాంకం *keyval, గుర్తులేని పూర్ణాంకం *state)
{
    స్థిరరాశి అక్షరం	    *mods_చివర;
    గుర్తులేని	    tmp_keyval;
    గుర్తులేని	    tmp_state = 0;

    mods_చివర = strrchr(str, '-');
    mods_చివర = (mods_చివర != ఖాళీ) ? mods_చివర + 1 : str;

    // Parse modifier keys
    అయితే (str < mods_చివర)
	ఎంపిక (*str++)
	{
	    స్థితి '-':							ఆపు;
	    స్థితి 'S': స్థితి 's': tmp_state |= (గుర్తులేని)GDK_జరుపు_ముసుగు;	ఆపు;
	    స్థితి 'L': స్థితి 'l': tmp_state |= (గుర్తులేని)GDK_తాళం_ముసుగు;	ఆపు;
	    స్థితి 'C': స్థితి 'c': tmp_state |= (గుర్తులేని)GDK_CONTROL_ముసుగు;ఆపు;
	    స్థితి '1':		tmp_state |= (గుర్తులేని)GDK_MOD1_ముసుగు;	ఆపు;
	    స్థితి '2':		tmp_state |= (గుర్తులేని)GDK_MOD2_ముసుగు;	ఆపు;
	    స్థితి '3':		tmp_state |= (గుర్తులేని)GDK_MOD3_ముసుగు;	ఆపు;
	    స్థితి '4':		tmp_state |= (గుర్తులేని)GDK_MOD4_ముసుగు;	ఆపు;
	    స్థితి '5':		tmp_state |= (గుర్తులేని)GDK_MOD5_ముసుగు;	ఆపు;
	    అప్రమేయ:
		తిరిగి FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    ఒకవేళ (tmp_keyval == 0 || tmp_keyval == GDK_Void_చిహ్నం)
	తిరిగి FALSE;

    ఒకవేళ (keyval != ఖాళీ)
	*keyval = tmp_keyval;
    ఒకవేళ (state != ఖాళీ)
	*state = tmp_state;

    తిరిగి TRUE;
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
    పూర్ణాంకం
im_xim_isvalid_imactivate(శూన్య)
{
    ఒకవేళ (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_Void_చిహ్నం;
	im_activatekey_state  = 0;
	తిరిగి TRUE;
    }

    తిరిగి im_string_గమ్యం_keyval((స్థిరరాశి అక్షరం *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    స్థిర శూన్య
im_synthesize_keypress(గుర్తులేని పూర్ణాంకం keyval, గుర్తులేని పూర్ణాంకం state)
{
    Gdk_Event_కీలకం *event;

    event = (Gdk_Event_కీలకం *)gdk_event_new(GDK_కీలకం_PRESS);
    g_object_ref(gtk_widget_get_కిటికీ(gui.drawarea));
					// unreffed by gdk_event_free()
    event->కిటికీ_2 = gtk_widget_get_కిటికీ(gui.drawarea);
    event->send_event = TRUE;
    event->సమయం = GDK_CURRENT_సమయం;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysym_గమ్యం_Keycode(GDK_కిటికీ_XDISPLAY(event->కిటికీ_2), (కీలకం_Sym)keyval);
    event->నిడివి = 0;
    event->string = ఖాళీ;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_కీలకం_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    శూన్య
xim_reset(శూన్య)
{
#  ifdef FEAT_EVAL
    ఒకవేళ (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_చురుకైన);
    లేకపోతే
#  endif
    ఒకవేళ (xic != ఖాళీ)
    {
	gtk_im_context_reset(xic);

	ఒకవేళ (p_imdisable)
	    im_shutdown();
	లేకపోతే
	{
	    xim_సమితి_focus(gui.లోపల_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_సమితి_focus(gui.లోపల_focus);
#  లేకపోతే
	    ఒకవేళ (im_activatekey_keyval != GDK_Void_చిహ్నం)
	    {
		ఒకవేళ (im_is_చురుకైన)
		{
		    g_signal_handler_ఖండం(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    లేకపోతే
	    {
		im_shutdown();
		xim_init();
		xim_సమితి_focus(gui.లోపల_focus);
	    }
#  endif
	}
    }

    ఒకవేళ (p_imst == IM_ON_THE_SPOT)
	preedit_ప్రారంభం_నిలువు = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    పూర్ణాంకం
xim_వరుస_కీలకం_press_event(Gdk_Event_కీలకం *event, పూర్ణాంకం కిందికి)
{
#   ifdef FEAT_GUI_GTK
    ఒకవేళ (event->state & GDK_SUPER_ముసుగు) తిరిగి FALSE;
#   endif
    ఒకవేళ (కిందికి)
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
	ఎంపిక (event->keyval)
	{
	    స్థితి GDK_KP_కూడు:      xim_expected_char = '+';  ఆపు;
	    స్థితి GDK_KP_వ్యవకలనం: xim_expected_char = '-';  ఆపు;
	    స్థితి GDK_KP_Divide:   xim_expected_char = '/';  ఆపు;
	    స్థితి GDK_KP_Multiply: xim_expected_char = '*';  ఆపు;
	    స్థితి GDK_KP_Decimal:  xim_expected_char = '.';  ఆపు;
	    స్థితి GDK_KP_Equal:    xim_expected_char = '=';  ఆపు;
	    స్థితి GDK_KP_0:	  xim_expected_char = '0';  ఆపు;
	    స్థితి GDK_KP_1:	  xim_expected_char = '1';  ఆపు;
	    స్థితి GDK_KP_2:	  xim_expected_char = '2';  ఆపు;
	    స్థితి GDK_KP_3:	  xim_expected_char = '3';  ఆపు;
	    స్థితి GDK_KP_4:	  xim_expected_char = '4';  ఆపు;
	    స్థితి GDK_KP_5:	  xim_expected_char = '5';  ఆపు;
	    స్థితి GDK_KP_6:	  xim_expected_char = '6';  ఆపు;
	    స్థితి GDK_KP_7:	  xim_expected_char = '7';  ఆపు;
	    స్థితి GDK_KP_8:	  xim_expected_char = '8';  ఆపు;
	    స్థితి GDK_KP_9:	  xim_expected_char = '9';  ఆపు;
	    స్థితి GDK_space:	  xim_expected_char = ' ';  ఆపు;
	    అప్రమేయ:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    ఒకవేళ (xic != ఖాళీ && !p_imdisable
		    && (State & (బహుళకం_చొప్పించు | బహుళకం_CMDLINE
					      | బహుళకం_సాధారణ | బహుళకం_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	ఒకవేళ (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    గుర్తులేని పూర్ణాంకం state_ముసుగు;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_ముసుగు  = im_activatekey_state;
	    state_ముసుగు |= ((పూర్ణాంకం)GDK_జరుపు_ముసుగు | (పూర్ణాంకం)GDK_CONTROL_ముసుగు
							| (పూర్ణాంకం)GDK_MOD1_ముసుగు);

	    ఒకవేళ ((event->state & state_ముసుగు) != im_activatekey_state)
		తిరిగి FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    ఒకవేళ (event->type != GDK_కీలకం_PRESS)
		తిరిగి TRUE;

	    ఒకవేళ (map_గమ్యం_exists_బహుళకం((char_u *)"", బహుళకం_LANGMAP, FALSE))
	    {
		im_సమితి_చురుకైన(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= బహుళకం_LANGMAP;
		ఒకవేళ (State & బహుళకం_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~బహుళకం_LANGMAP;
		}
		లేకపోతే
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= బహుళకం_LANGMAP;
		}
		తిరిగి TRUE;
	    }

	    తిరిగి gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	ఒకవేళ (im_activatekey_keyval == GDK_Void_చిహ్నం || im_is_చురుకైన)
	{
	    పూర్ణాంకం imresult = gtk_im_context_filter_keypress(xic, event);

	    ఒకవేళ (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		ఒకవేళ (!imresult && preedit_ప్రారంభం_నిలువు != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    తిరిగి FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    ఒకవేళ (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		తిరిగి FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    తిరిగి imresult;
	}
    }

    తిరిగి FALSE;
}
#  లేకపోతే // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    పూర్ణాంకం
im_xim_isvalid_imactivate(శూన్య)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    తిరిగి p_imak[0] == NUL;
}

    పూర్ణాంకం
xim_వరుస_కీలకం_press_event(GdkEvent *event వాడని, పూర్ణాంకం కిందికి వాడని)
{
    తిరిగి FALSE;
}
#  endif // !USE_GTK4

    పూర్ణాంకం
im_get_స్థితి(శూన్య)
{
#  ifdef FEAT_EVAL
    ఒకవేళ (USE_IMSTATUSFUNC)
	తిరిగి call_imstatusfunc();
#  endif
    తిరిగి im_is_చురుకైన;
}

    పూర్ణాంకం
preedit_get_స్థితి(శూన్య)
{
    తిరిగి preedit_is_చురుకైన;
}

    పూర్ణాంకం
im_is_preediting(శూన్య)
{
    తిరిగి xim_has_preediting;
}

# లేకపోతే // !FEAT_GUI_GTK

స్థిర పూర్ణాంకం	xim_is_చురుకైన = FALSE;  // XIM should be active in the current
					//బహుళకం
స్థిర పూర్ణాంకం	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
స్థిర XIMStyle	ఆదానం_style;
స్థిర పూర్ణాంకం	స్థితి_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    శూన్య
im_సమితి_చురుకైన(పూర్ణాంకం చురుకైన_arg)
{
    పూర్ణాంకం చురుకైన = చురుకైన_arg;

    // If 'imdisable' is set, XIM is never active.
    ఒకవేళ (p_imdisable)
	చురుకైన = FALSE;
    లేకపోతే ఒకవేళ (ఆదానం_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	చురుకైన = TRUE;

#  ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (USE_IMACTIVATEFUNC)
    {
	ఒకవేళ (చురుకైన != im_get_స్థితి())
	{
	    call_imactivatefunc(చురుకైన);
	    xim_has_focus = చురుకైన;
	}
	తిరిగి;
    }
#  endif

    ఒకవేళ (xic == ఖాళీ)
	తిరిగి;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_చురుకైన = చురుకైన;
    xim_సమితి_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    శూన్య
xim_సమితి_focus(పూర్ణాంకం focus)
{
    ఒకవేళ (xic == ఖాళీ)
	తిరిగి;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    ఒకవేళ (focus && xim_is_చురుకైన)
    {
	ఒకవేళ (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    లేకపోతే
    {
	ఒకవేళ (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    శూన్య
im_సమితి_position(పూర్ణాంకం అడ్డువరుస వాడని, పూర్ణాంకం నిలువు వాడని)
{
    xim_సమితి_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    శూన్య
xim_సమితి_preedit(శూన్య)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    పూర్ణాంకం పంక్తి_space;

    ఒకవేళ (xic == ఖాళీ)
	తిరిగి;

    xim_సమితి_focus(TRUE);

    ఒకవేళ (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							ఖాళీ);
	XSetICValues(xic, XNPreedit_లక్షణాలు, attr_list, ఖాళీ);
	XFree(attr_list);
	తిరిగి;
    }

    ఒకవేళ (ఆదానం_style & XIMPreeditPosition)
    {
	ఒకవేళ (xim_fg_color == INVALCOLOR)
	{
	    xim_fg_color = gui.def_norm_pixel;
	    xim_bg_color = gui.def_వెనుక_pixel;
	}
	over_spot.x = పాఠ్యం_X(gui.నిలువు);
	over_spot.y = పాఠ్యం_Y(gui.అడ్డువరుస);
	spot_area.x = 0;
	spot_area.y = 0;
	spot_area.ఎత్తు = gui.char_ఎత్తు * అడ్డువరుసలు;
	spot_area.వెడల్పు  = gui.char_వెడల్పు * Columns;
	పంక్తి_space = gui.char_ఎత్తు;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, పంక్తి_space,
					ఖాళీ);
	ఒకవేళ (XSetICValues(xic, XNPreedit_లక్షణాలు, attr_list, ఖాళీ))
	    emsg(_(e_cannot_సమితి_ic_విలువలు));
	XFree(attr_list);
    }
}

#  ఒకవేళ defined(FEAT_GUI_X11)
#   ఒకవేళ defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

స్థిర పూర్ణాంకం xim_real_init(కిటికీ x11_కిటికీ, Display *x11_display);


#   ifdef USE_X11R6_XIM
    స్థిర శూన్య
xim_instantiate_cb(
    Display	*display,
    XPointer	client_దత్తాంశం_2 వాడని,
    XPointer	call_దత్తాంశం వాడని)
{
    కిటికీ	x11_కిటికీ;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_instantiate_cb()\n");
#    endif

    gui_get_x11_windis(&x11_కిటికీ, &x11_display);
    ఒకవేళ (display != x11_display)
	తిరిగి;

    xim_real_init(x11_కిటికీ, x11_display);
    gui_సమితి_shellsize(FALSE, FALSE, RESIZE_BOTH);
    ఒకవేళ (xic != ఖాళీ)
	XUnregisterIMInstantiateCallback(x11_display, ఖాళీ, ఖాళీ, ఖాళీ,
					 xim_instantiate_cb, ఖాళీ);
}

    స్థిర శూన్య
xim_destroy_cb(
    XIM		im వాడని,
    XPointer	client_దత్తాంశం_2 వాడని,
    XPointer	call_దత్తాంశం వాడని)
{
    కిటికీ	x11_కిటికీ;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_destroy_cb()\n");
#    endif
    gui_get_x11_windis(&x11_కిటికీ, &x11_display);

    xic = ఖాళీ;
    స్థితి_area_enabled = FALSE;

    gui_సమితి_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, ఖాళీ, ఖాళీ, ఖాళీ,
				   xim_instantiate_cb, ఖాళీ);
}
#   endif

    శూన్య
xim_init(శూన్య)
{
    కిటికీ	x11_కిటికీ;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_కిటికీ, &x11_display);

    xic = ఖాళీ;

    ఒకవేళ (xim_real_init(x11_కిటికీ, x11_display))
	తిరిగి;

    gui_సమితి_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, ఖాళీ, ఖాళీ, ఖాళీ,
				   xim_instantiate_cb, ఖాళీ);
#   endif
}

    స్థిర పూర్ణాంకం
xim_real_init(కిటికీ x11_కిటికీ, Display *x11_display)
{
    పూర్ణాంకం		i;
    అక్షరం	*p,
		*s,
		*ns,
		*చివర,
		tmp[1024];
#   define IMLEN_MAX 40
    అక్షరం	buf[IMLEN_MAX + 7];
    XIM		xim = ఖాళీ;
    XIMStyles	*xim_styles;
    XIMStyle	this_ఆదానం_style = 0;
    Boolean	దొరికింది;
    XPoint	over_spot;
    XVaNestedList preedit_list, స్థితి_list;

    ఆదానం_style = 0;
    స్థితి_area_enabled = FALSE;

    ఒకవేళ (xic != ఖాళీ)
	తిరిగి FALSE;

    ఒకవేళ (gui.rsrc_ఆదానం_పద్ధతి != ఖాళీ && *gui.rsrc_ఆదానం_పద్ధతి != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_ఆదానం_పద్ధతి,
	    పరిమాణం(tmp) - 1);
	కొరకు (ns = s = tmp; ns != ఖాళీ && *s != NUL;)
	{
	    s = (అక్షరం *)skipwhite((char_u *)s);
	    ఒకవేళ (*s == NUL)
		ఆపు;
	    ఒకవేళ ((ns = చివర = strchr(s, ',')) == ఖాళీ)
		చివర = s + అక్షరశ్రేణి_పొడవు(s);
	    అయితే (సురక్షిత_isspace(చివర[-1]))
		చివర--;
	    *చివర = NUL;

	    ఒకవేళ (అక్షరశ్రేణి_పొడవు(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		ఒకవేళ ((p = XSetLocaleModifiers(buf)) != ఖాళీ && *p != NUL
			&& (xim = XOpenIM(x11_display, ఖాళీ, ఖాళీ, ఖాళీ))
								      != ఖాళీ)
		    ఆపు;
	    }

	    s = ns + 1;
	}
    }

    ఒకవేళ (xim == ఖాళీ && (p = XSetLocaleModifiers("")) != ఖాళీ && *p != NUL)
	xim = XOpenIM(x11_display, ఖాళీ, ఖాళీ, ఖాళీ);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    ఒకవేళ (xim == ఖాళీ && (p = XSetLocaleModifiers("@im=none")) != ఖాళీ
								 && *p != NUL)
	xim = XOpenIM(x11_display, ఖాళీ, ఖాళీ, ఖాళీ);

    ఒకవేళ (xim == ఖాళీ)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	ఒకవేళ (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_గమ్యం_open_ఆదానం_పద్ధతి));
	    verbose_leave();
	}
	తిరిగి FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_దత్తాంశం_2 = ఖాళీ;
	ఒకవేళ (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, ఖాళీ))
	    emsg(_(e_warning_could_not_సమితి_destroy_callback_గమ్యం_im));
    }
#   endif

    ఒకవేళ (XGetIMValues(xim, XNQuery_ఆదానం_Style, &xim_styles, ఖాళీ) || !xim_styles)
    {
	emsg(_(e_ఆదానం_పద్ధతి_doesnt_support_any_style));
	XCloseIM(xim);
	తిరిగి FALSE;
    }

    దొరికింది = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    పరిమాణం(tmp) - 1);
    కొరకు (s = tmp; s && !దొరికింది; )
    {
	అయితే (*s && సురక్షిత_isspace(*s))
	    s++;
	ఒకవేళ (!*s)
	    ఆపు;
	ఒకవేళ ((ns = చివర = strchr(s, ',')) != 0)
	    ns++;
	లేకపోతే
	    చివర = s + అక్షరశ్రేణి_పొడవు(s);
	అయితే (సురక్షిత_isspace(*చివర))
	    చివర--;
	*చివర = '\0';

	ఒకవేళ (!strcmp(s, "OverTheSpot"))
	    this_ఆదానం_style = (XIMPreeditPosition | XIMStatusArea);
	లేకపోతే ఒకవేళ (!strcmp(s, "OffTheSpot"))
	    this_ఆదానం_style = (XIMPreeditArea | XIMStatusArea);
	లేకపోతే ఒకవేళ (!strcmp(s, "Root"))
	    this_ఆదానం_style = (XIMPreeditNothing | XIMStatusNothing);

	కొరకు (i = 0; (గుర్తులేని పొట్టి)i < xim_styles->లెక్క_styles; i++)
	{
	    ఒకవేళ (this_ఆదానం_style == xim_styles->supported_styles[i])
	    {
		దొరికింది = True;
		ఆపు;
	    }
	}
	ఒకవేళ (!దొరికింది)
	    కొరకు (i = 0; (గుర్తులేని పొట్టి)i < xim_styles->లెక్క_styles; i++)
	    {
		ఒకవేళ ((xim_styles->supported_styles[i] & this_ఆదానం_style)
			== (this_ఆదానం_style & ~XIMStatusArea))
		{
		    this_ఆదానం_style &= ~XIMStatusArea;
		    దొరికింది = True;
		    ఆపు;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    ఒకవేళ (!దొరికింది)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	ఒకవేళ (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_ఆదానం_పద్ధతి_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	తిరిగి FALSE;
    }

    over_spot.x = పాఠ్యం_X(gui.నిలువు);
    over_spot.y = పాఠ్యం_Y(gui.అడ్డువరుస);
    ఆదానం_style = this_ఆదానం_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    ఒకవేళ (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_వెనుక_pixel,
				XNFont_సమితి, (XFont_సమితి)gui.fontset,
				ఖాళీ);
	స్థితి_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_వెనుక_pixel,
				XNFont_సమితి, (XFont_సమితి)gui.fontset,
				ఖాళీ);
    }
    లేకపోతే
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_వెనుక_pixel,
				ఖాళీ);
	స్థితి_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_వెనుక_pixel,
				ఖాళీ);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, ఆదానం_style,
		    XNClient_కిటికీ, x11_కిటికీ,
		    XNFocus_కిటికీ, gui.wid,
		    XNPreedit_లక్షణాలు, preedit_list,
		    XNStatus_లక్షణాలు, స్థితి_list,
		    ఖాళీ);
    XFree(స్థితి_list);
    XFree(preedit_list);
    ఒకవేళ (xic != ఖాళీ)
    {
	ఒకవేళ (ఆదానం_style & XIMStatusArea)
	{
	    xim_సమితి_స్థితి_area();
	    స్థితి_area_enabled = TRUE;
	}
	లేకపోతే
	    gui_సమితి_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    లేకపోతే
    {
	ఒకవేళ (!is_not_a_పదం())
	    emsg(_(e_failed_గమ్యం_create_ఆదానం_context));
	XCloseIM(xim);
	తిరిగి FALSE;
    }

    తిరిగి TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    పూర్ణాంకం
im_get_స్థితి(శూన్య)
{
#  ifdef FEAT_EVAL
    ఒకవేళ (USE_IMSTATUSFUNC)
	తిరిగి call_imstatusfunc();
#  endif
    తిరిగి xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# ఒకవేళ !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    శూన్య
xim_సమితి_స్థితి_area(శూన్య)
{
    XVaNestedList preedit_list = 0, స్థితి_list = 0, list = 0;
    XRectangle pre_area, స్థితి_area;

    ఒకవేళ (xic == ఖాళీ)
	తిరిగి;

    ఒకవేళ (ఆదానం_style & XIMStatusArea)
    {
	ఒకవేళ (ఆదానం_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    స్థితి_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, ఖాళీ);
	    XGetICValues(xic, XNStatus_లక్షణాలు, స్థితి_list, ఖాళీ);
	    XFree(స్థితి_list);

	    స్థితి_area.వెడల్పు = needed_rect->వెడల్పు;
	}
	లేకపోతే
	    స్థితి_area.వెడల్పు = gui.char_వెడల్పు * Columns;

	స్థితి_area.x = 0;
	స్థితి_area.y = gui.char_ఎత్తు * అడ్డువరుసలు + gui.border_స్థానభ్రంశం;
	ఒకవేళ (gui.which_scrollbars[SBAR_BOTTOM])
	    స్థితి_area.y += gui.scrollbar_ఎత్తు;
#  ifdef FEAT_MENU
	ఒకవేళ (gui.menu_is_చురుకైన)
	    స్థితి_area.y += gui.menu_ఎత్తు;
#  endif
	స్థితి_area.ఎత్తు = gui.char_ఎత్తు;
	స్థితి_list = XVaCreateNestedList(0, XNArea, &స్థితి_area, ఖాళీ);
    }
    లేకపోతే
    {
	స్థితి_area.x = 0;
	స్థితి_area.y = gui.char_ఎత్తు * అడ్డువరుసలు + gui.border_స్థానభ్రంశం;
	ఒకవేళ (gui.which_scrollbars[SBAR_BOTTOM])
	    స్థితి_area.y += gui.scrollbar_ఎత్తు;
#  ifdef FEAT_MENU
	ఒకవేళ (gui.menu_is_చురుకైన)
	    స్థితి_area.y += gui.menu_ఎత్తు;
#  endif
	స్థితి_area.వెడల్పు = 0;
	స్థితి_area.ఎత్తు = gui.char_ఎత్తు;
    }

    ఒకవేళ (ఆదానం_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = స్థితి_area.x + స్థితి_area.వెడల్పు;
	pre_area.y = gui.char_ఎత్తు * అడ్డువరుసలు + gui.border_స్థానభ్రంశం;
	pre_area.వెడల్పు = gui.char_వెడల్పు * Columns - pre_area.x;
	ఒకవేళ (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_ఎత్తు;
#  ifdef FEAT_MENU
	ఒకవేళ (gui.menu_is_చురుకైన)
	    pre_area.y += gui.menu_ఎత్తు;
#  endif
	pre_area.ఎత్తు = gui.char_ఎత్తు;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ఖాళీ);
    }
    లేకపోతే ఒకవేళ (ఆదానం_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.ఎత్తు = gui.char_ఎత్తు * అడ్డువరుసలు;
	pre_area.వెడల్పు = gui.char_వెడల్పు * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ఖాళీ);
    }

    ఒకవేళ (preedit_list && స్థితి_list)
	list = XVaCreateNestedList(0, XNPreedit_లక్షణాలు, preedit_list,
				   XNStatus_లక్షణాలు, స్థితి_list, ఖాళీ);
    లేకపోతే ఒకవేళ (preedit_list)
	list = XVaCreateNestedList(0, XNPreedit_లక్షణాలు, preedit_list,
				   ఖాళీ);
    లేకపోతే ఒకవేళ (స్థితి_list)
	list = XVaCreateNestedList(0, XNStatus_లక్షణాలు, స్థితి_list,
				   ఖాళీ);
    లేకపోతే
	list = ఖాళీ;

    ఒకవేళ (list)
    {
	XSetICValues(xic, XNVaNestedList, list, ఖాళీ);
	XFree(list);
    }
    ఒకవేళ (స్థితి_list)
	XFree(స్థితి_list);
    ఒకవేళ (preedit_list)
	XFree(preedit_list);
}

    పూర్ణాంకం
xim_get_స్థితి_area_ఎత్తు(శూన్య)
{
    ఒకవేళ (స్థితి_area_enabled)
	తిరిగి gui.char_ఎత్తు;
    తిరిగి 0;
}
# endif

#లేకపోతే // !defined(FEAT_XIM)

# ఒకవేళ defined(IME_WITHOUT_XIM) || defined(VIMDLL)
స్థిర పూర్ణాంకం im_was_సమితి_చురుకైన = FALSE;

    పూర్ణాంకం
#  ifdef VIMDLL
mbyte_im_get_స్థితి(శూన్య)
#  లేకపోతే
im_get_స్థితి(శూన్య)
#  endif
{
#  ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (USE_IMSTATUSFUNC)
	తిరిగి call_imstatusfunc();
#  endif
    తిరిగి im_was_సమితి_చురుకైన;
}

    శూన్య
#  ifdef VIMDLL
mbyte_im_సమితి_చురుకైన(పూర్ణాంకం చురుకైన_arg)
#  లేకపోతే
im_సమితి_చురుకైన(పూర్ణాంకం చురుకైన_arg)
#  endif
{
#  ఒకవేళ defined(FEAT_EVAL)
    పూర్ణాంకం	    చురుకైన = !p_imdisable && చురుకైన_arg;

    ఒకవేళ (USE_IMACTIVATEFUNC && చురుకైన != im_get_స్థితి())
    {
	call_imactivatefunc(చురుకైన);
	im_was_సమితి_చురుకైన = చురుకైన;
    }
#  endif
}

#  ఒకవేళ defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    శూన్య
im_సమితి_position(పూర్ణాంకం అడ్డువరుస వాడని, పూర్ణాంకం నిలువు వాడని)
{
}
#  endif
# endif

#అయితే_ముగింపు // FEAT_XIM
