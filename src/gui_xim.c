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

#અન્તર્ભાવ "vim.h"

#જો !defined(GTK_CHECK_VERSION)
# define GTK_CHECK_VERSION(a, b, c) 0
#endif

#જો defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# નહીંતર
#  include <gdk/gdkkeysyms.h>
# endif
# જો !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  નહીંતર
#   include <gdk/gdkx.h>
#  endif
# endif
#endif

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#જો defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
સ્થિર શૂન્ય xim_log(અક્ષર *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    સ્થિર શૂન્ય
xim_log(અક્ષર *s, ...)
{
    va_list arglist;
    સ્થિર FILE *fd = ખાલી;

    જો (fd == (FILE *)-1)
	પાછા;
    જો (fd == ખાલી)
    {
	fd = mch_fopen("xim.log", "w");
	જો (fd == ખાલી)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    પાછા;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#endif

#જો defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.in_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.in_use && *p_imsf != NUL)
#નહીંતર
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#endif

#જો defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
સ્થિર callback_T imaf_cb;	    // 'imactivatefunc' callback function
સ્થિર callback_T imsf_cb;	    // 'imstatusfunc' callback function

    અક્ષર *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    જો (option_set_callback_func(p_imaf, &imaf_cb) == FAIL)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    અક્ષર *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    જો (option_set_callback_func(p_imsf, &imsf_cb) == FAIL)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    સ્થિર શૂન્ય
call_imactivatefunc(પૂર્ણાંક active)
{
    typval_T argv[2];
    પૂર્ણાંક save_ચાવી_Typed = ચાવી_Typed;

    argv[0].v_type = VAR_સંખ્યા;
    argv[0].vval.v_સંખ્યા = active ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (શૂન્ય)call_callback_retnr(&imaf_cb, 1, argv);

    ચાવી_Typed = save_ચાવી_Typed;
}

    સ્થિર પૂર્ણાંક
call_imstatusfunc(શૂન્ય)
{
    પૂર્ણાંક is_active;
    પૂર્ણાંક save_ચાવી_Typed = ચાવી_Typed;

    // FIXME: Don't execute user function in unsafe situation.
    જો (exiting || is_autocmd_blocked())
	પાછા FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++સંદેશ_silent;
    is_active = call_callback_retnr(&imsf_cb, 0, ખાલી);
    --સંદેશ_silent;

    ચાવી_Typed = save_ચાવી_Typed;
    પાછા (is_active > 0);
}
#endif

#જો defined(EXITFREE)
    શૂન્ય
free_xim_stuff(શૂન્ય)
{
# જો defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#endif

#જો defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    પૂર્ણાંક
set_ref_in_im_funcs(પૂર્ણાંક copyID UNUSED)
{
    પૂર્ણાંક abort = FALSE;

# જો defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = set_ref_in_callback(&imaf_cb, copyID);
    abort = abort || set_ref_in_callback(&imsf_cb, copyID);
# endif

    પાછા abort;
}
#endif


#જો defined(FEAT_XIM)

# જો defined(FEAT_GUI_GTK)
સ્થિર પૂર્ણાંક xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    સ્થિર શૂન્ય
init_preedit_start_col(શૂન્ય)
{
    જો (State & MODE_CMDLINE)
	preedit_start_col = cmdline_getvcol_cursor();
    નહીંતર જો (curwin != ખાલી && curwin->w_buffer != ખાલી)
	getvcol(curwin, &curwin->w_cursor, &preedit_start_col, ખાલી, ખાલી, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_changed_while_preediting = curbuf->b_changed;
}

સ્થિર પૂર્ણાંક im_is_active	       = FALSE;	// IM is enabled for current mode
સ્થિર પૂર્ણાંક preedit_is_active   = FALSE;
સ્થિર પૂર્ણાંક im_preedit_cursor   = 0;	// cursor offset in characters
સ્થિર પૂર્ણાંક im_preedit_trailing = 0;	// number of characters after cursor

સ્થિર unsigned long im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_ચાવી_Void_પ્રતીક
#    define GDK_ચાવી_Void_પ્રતીક GDK_Void_પ્રતીક
#   endif
સ્થિર unsigned પૂર્ણાંક  im_activatekey_keyval = GDK_ચાવી_Void_પ્રતીક;
સ્થિર unsigned પૂર્ણાંક  im_activatekey_state  = 0;
#  endif

સ્થિર GtkWidget *preedit_window = ખાલી;
સ્થિર GtkWidget *preedit_label = ખાલી;
#  ifdef USE_GTK4
સ્થિર પૂર્ણાંક preedit_label_width = 0;	// last measured popover natural width
સ્થિર પૂર્ણાંક preedit_popover_height = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
સ્થિર GtkCssProvider *preedit_css_provider = ખાલી;
સ્થિર guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

સ્થિર શૂન્ય im_preedit_window_set_position(શૂન્ય);

    શૂન્ય
im_set_active(પૂર્ણાંક active)
{
    પૂર્ણાંક was_active;

    was_active = !!im_get_status();
    im_is_active = (active && !p_imdisable);

    જો (im_is_active != was_active)
	xim_reset();
}

    શૂન્ય
xim_set_focus(પૂર્ણાંક focus)
{
    જો (xic == ખાલી)
	પાછા;

    જો (focus)
	gtk_im_context_focus_in(xic);
    નહીંતર
	gtk_im_context_focus_out(xic);
}

    શૂન્ય
im_set_position(પૂર્ણાંક row, પૂર્ણાંક col)
{
    જો (xic == ખાલી)
	પાછા;

    GdkRectangle area;

    area.x = FILL_X(col);
    area.y = FILL_Y(row);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.width  = gui.char_width
		    * ((Screen_લીટીઓ != ખાલી && mb_lefthalve(row, col)) ? 2 : 1);
    area.height = gui.char_height;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    જો (p_imst == IM_OVER_THE_SPOT)
    {
	પૂર્ણાંક popover_h = preedit_popover_height;
	જો (popover_h <= 0)
	    popover_h = gui.char_height;
	જો (popover_h > area.height)
	    area.height = popover_h;
    }
#  endif

    gtk_im_context_set_cursor_location(xic, &area);

    જો (p_imst == IM_OVER_THE_SPOT)
	im_preedit_window_set_position();
}

#  જો 0 || defined(PROTO) // apparently only used in gui_x11.c
    શૂન્ય
xim_set_preedit(શૂન્ય)
{
    im_set_position(gui.row, gui.col);
}
#  endif

    સ્થિર શૂન્ય
im_add_to_આગત(char_u *str, પૂર્ણાંક len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    જો (આગત_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&આગત_conv, str, &len);
	g_return_if_fail(str != ખાલી);
    }

    add_to_આગત_buf_csi(str, len);

    જો (આગત_conv.vc_type != CONV_NONE)
	vim_free(str);

    જો (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    સ્થિર શૂન્ય
im_preedit_window_set_position(શૂન્ય)
{
    જો (preedit_window == ખાલી)
	પાછા;

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
    graphene_point_t pt_in, pt_out;

    pt_in.x = FILL_X(gui.col);
    pt_in.y = FILL_Y(gui.row);
    જો (!gtk_widget_compute_point(gui.drawarea, gui.mainwin,
						&pt_in, &pt_out))
    {
	pt_out.x = pt_in.x;
	pt_out.y = pt_in.y;
    }
    rect.x = (પૂર્ણાંક)pt_out.x;
    rect.y = (પૂર્ણાંક)pt_out.y;
    rect.width = preedit_label_width;
    rect.height = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    પૂર્ણાંક offset = gui.char_height - preedit_popover_height;
    gtk_popover_set_offset(GTK_POPOVER(preedit_window), 0, offset);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_set_halign(preedit_window, GTK_ALIGN_START);
    gtk_popover_set_pointing_to(GTK_POPOVER(preedit_window), &rect);
#  નહીંતર
    પૂર્ણાંક x, y, width, height;
    પૂર્ણાંક screen_x, screen_y, screen_width, screen_height;

    gui_gtk_get_screen_geom_of_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_width, &screen_height);
    gdk_window_get_origin(gtk_widget_get_window(gui.drawarea), &x, &y);
    gtk_window_get_size(GTK_WINDOW(preedit_window), &width, &height);
    x = x + FILL_X(gui.col);
    y = y + FILL_Y(gui.row);
    જો (x + width > screen_x + screen_width)
	x = screen_x + screen_width - width;
    જો (y + height > screen_y + screen_height)
	y = screen_y + screen_height - height;
    gtk_window_move(GTK_WINDOW(preedit_window), x, y);
#  endif
}

    સ્થિર શૂન્ય
im_preedit_window_open(શૂન્ય)
{
    અક્ષર *preedit_string;
#  જો !GTK_CHECK_VERSION(3,16,0)
    અક્ષર buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  જો GTK_CHECK_VERSION(3,0,0)
#   જો !GTK_CHECK_VERSION(3,16,0)
    GdkRGBA color;
#   endif
#  નહીંતર
    GdkColor color;
#  endif
    gint w, h;

    જો (preedit_window == ખાલી)
    {
#  ifdef USE_GTK4
	// GTK4: use GtkPopover (xdg_popup) so the compositor keeps keyboard
	// focus on the drawing area and does not disable text-input-v3.
	// See issue #20257.  Parent the popover to gui.mainwin (a real
	// container) rather than gui.drawarea: GtkDrawingArea is a leaf
	// widget and using it as a popover parent causes snapshot artifacts
	// (white-out) on the drawing area.
	preedit_window = gtk_popover_new();
	gtk_popover_set_autohide(GTK_POPOVER(preedit_window), FALSE);
	gtk_popover_set_has_arrow(GTK_POPOVER(preedit_window), FALSE);
	// Pin the popover below the anchor.  Together with anchor.h == 0 in
	// im_preedit_window_set_position() this lands the popover's top edge
	// exactly on FILL_Y(row), so the popover covers the cursor cell rather
	// than landing above it (the default GTK_POS_TOP would do that and
	// only flip to bottom when there is no room above, which is flaky).
	gtk_popover_set_position(GTK_POPOVER(preedit_window), GTK_POS_BOTTOM);
	gtk_widget_set_can_focus(preedit_window, FALSE);
	gtk_widget_set_focusable(preedit_window, FALSE);
	gtk_widget_set_can_target(preedit_window, FALSE);
	gtk_widget_set_parent(preedit_window, gui.mainwin);
#  નહીંતર
	preedit_window = gtk_window_new(GTK_WINDOW_POPUP);
	gtk_window_set_transient_for(GTK_WINDOW(preedit_window),
						     GTK_WINDOW(gui.mainwin));
#  endif
	preedit_label = gtk_label_new("");
	gtk_widget_set_name(preedit_label, "vim-gui-preedit-area");
#  ifdef USE_GTK4
	gtk_label_set_xalign(GTK_LABEL(preedit_label), 0.0);
	gtk_widget_set_halign(preedit_label, GTK_ALIGN_START);
	gtk_widget_set_valign(preedit_label, GTK_ALIGN_START);
	// Tag the popover with a CSS class so the minimal frame CSS below
	// (no border, no rounded corners, no shadow, no padding) applies
	// only to our preedit popover and not other popovers in the app.
	gtk_widget_add_css_class(preedit_window, "vim-preedit-popover");
	gtk_popover_set_child(GTK_POPOVER(preedit_window), preedit_label);
#  નહીંતર
	gtk_container_add(GTK_CONTAINER(preedit_window), preedit_label);
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
    જો (preedit_css_provider == ખાલી || preedit_css_cached_bg != gui.back_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	જો (preedit_css_provider != ખાલી)
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
		(unsigned long)((gui.back_pixel >> 16) & 0xff),
		(unsigned long)((gui.back_pixel >> 8) & 0xff),
		(unsigned long)(gui.back_pixel & 0xff));
	gtk_css_provider_load_from_string(preedit_css_provider, css);
	g_free(css);
	gtk_style_context_add_provider_for_display(display,
			    GTK_STYLE_PROVIDER(preedit_css_provider), G_MAXUINT);
	preedit_css_cached_bg = gui.back_pixel;
    }
#  endif

#  ifndef USE_GTK4
#   જો GTK_CHECK_VERSION(3,16,0)
    {
	GtkStyleContext * અચલ context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * અચલ provider = gtk_css_provider_new();
	gchar		   *css = ખાલી;
	અચલ અક્ષર * અચલ fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_size(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = ખાલી;

	જો (!pango_font_description_get_size_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * અચલ screen
		  = gdk_window_get_screen(gtk_widget_get_window(gui.mainwin));
	    અચલ gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	જો (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	નહીંતર
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != ખાલી ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.back_pixel >> 16) & 0xff,
		(gui.back_pixel >> 8) & 0xff,
		gui.back_pixel & 0xff);

	gtk_css_provider_load_from_data(provider, css, -1, ખાલી);
	gtk_style_context_add_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_VERSION(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, કદ(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_NORMAL, &color);

    vim_snprintf(buf, કદ(buf), "#%06X", gui.back_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_NORMAL,
								      &color);
#   નહીંતર
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, કદ(buf), "#%06X", (unsigned)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_NORMAL, &color);

    vim_snprintf(buf, કદ(buf), "#%06X", (unsigned)gui.back_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_window, GTK_STATE_NORMAL, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ખાલી);

    જો (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	જો (attr_list == ખાલી)
	    attr_list = pango_attr_list_new();

	જો (gui.norm_font != ખાલી)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->start_index = 0;
	    pa->end_index = G_MAXUINT;
	    pango_attr_list_ઉમેરો(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_ઉમેરો(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.back_pixel >> 16) & 0xff) * 257,
		((gui.back_pixel >> 8) & 0xff) * 257,
		(gui.back_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_ઉમેરો(attr_list, pa);
#  endif
	gtk_label_set_text(GTK_LABEL(preedit_label), preedit_string);
	gtk_label_set_attributes(GTK_LABEL(preedit_label), attr_list);

	layout = gtk_label_get_layout(GTK_LABEL(preedit_label));
	pango_layout_get_pixel_size(layout, &w, &h);
	h = MAX(h, gui.char_height);
#  ifdef USE_GTK4
	// Cache label/popover size derived from the Pango layout, which is
	// available without mapping the popover.  Using these for positioning
	// avoids calling gtk_popover_set_pointing_to() after popup, which
	// triggers a GDK "compositor doesn't support moving popups" warning
	// on compositors without xdg_popup.reposition (e.g. Weston).
	preedit_label_width = w;
	preedit_popover_height = h;

	// Report an enlarged cursor rectangle that covers both the cursor
	// cell and the preedit popover.  This way the IM (e.g. fcitx5)
	// places its candidate window below the popover instead of on top
	// of it.  Coordinates are in client-widget (drawarea) space.
	જો (xic != ખાલી)
	{
	    GdkRectangle area;

	    area.x = FILL_X(gui.col);
	    area.y = FILL_Y(gui.row);
	    area.width = preedit_label_width;
	    area.height = MAX(gui.char_height, preedit_popover_height);
	    gtk_im_context_set_cursor_location(xic, &area);
	}

	// Pop down before re-anchoring: on compositors that lack
	// xdg_popup.reposition (e.g. Weston), calling set_pointing_to() on
	// a mapped popover triggers GDK's remap fallback and a warning.
	// Doing the popdown ourselves makes the next popup land at the new
	// anchor cleanly without that warning.
	જો (gtk_widget_get_mapped(preedit_window))
	    gtk_popover_popdown(GTK_POPOVER(preedit_window));

	im_preedit_window_set_position();

	gtk_widget_queue_resize(preedit_window);
	gtk_popover_popup(GTK_POPOVER(preedit_window));
#  નહીંતર
	gtk_window_resize(GTK_WINDOW(preedit_window), w, h);
	gtk_widget_show_all(preedit_window);

	im_preedit_window_set_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    સ્થિર શૂન્ય
im_preedit_window_close(શૂન્ય)
{
    જો (preedit_window == ખાલી)
	પાછા;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_window));
#  નહીંતર
    gtk_widget_hide(preedit_window);
#  endif
}

    સ્થિર શૂન્ય
im_show_preedit(શૂન્ય)
{
    im_preedit_window_open();

    જો (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    સ્થિર શૂન્ય
im_delete_preedit(શૂન્ય)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    જો (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_window_close();
	પાછા;
    }

    જો (State & MODE_NORMAL
#  ifdef FEAT_TERMINAL
	    && !term_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	પાછા;
    }
    માટે (; im_preedit_cursor > 0; --im_preedit_cursor)
	add_to_આગત_buf(bskey, (પૂર્ણાંક)કદ(bskey));

    માટે (; im_preedit_trailing > 0; --im_preedit_trailing)
	add_to_આગત_buf(delkey, (પૂર્ણાંક)કદ(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    સ્થિર શૂન્ય
im_correct_cursor(પૂર્ણાંક અંક_move_back)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    જો (State & MODE_NORMAL)
	પાછા;
#  ifdef FEAT_RIGHTLEFT
    જો ((State & MODE_CMDLINE) == 0 && curwin != ખાલી && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    માટે (; અંક_move_back > 0; --અંક_move_back)
	add_to_આગત_buf(backkey, (પૂર્ણાંક)કદ(backkey));
}

સ્થિર પૂર્ણાંક xim_expected_char = NUL;
સ્થિર પૂર્ણાંક xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    સ્થિર શૂન્ય
im_show_info(શૂન્ય)
{
    પૂર્ણાંક	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    જો ((State & MODE_NORMAL) || (State & MODE_ઉમેરો))
	setcursor();
    out_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    સ્થિર શૂન્ય
im_commit_cb(GtkIMContext *context UNUSED,
	     અચલ gchar *str,
	     gpointer data UNUSED)
{
    પૂર્ણાંક		slen = (પૂર્ણાંક)STRLEN(str);
    પૂર્ણાંક		add_to_આગત = TRUE;
    પૂર્ણાંક		clen;
    પૂર્ણાંક		len = slen;
    પૂર્ણાંક		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    જો (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	જો (preedit_start_col == MAXCOL)
	{
	    init_preedit_start_col();
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
	જો (આગત_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&આગત_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != ખાલી);
	}
	નહીંતર
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	જો (આગત_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_start_col += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    જો (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	જો (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    add_to_આગત = FALSE;
	}
	નહીંતર
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    જો (add_to_આગત)
	im_add_to_આગત((char_u *)str, slen);

    જો (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	જો (!commit_with_preedit)
	    preedit_start_col = MAXCOL;

	// This flag is used in changed() at next call.
	xim_changed_while_preediting = TRUE;
    }

    જો (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    સ્થિર શૂન્ય
im_preedit_start_cb(GtkIMContext *context UNUSED, gpointer data UNUSED)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_start_cb()\n");
#  endif

    im_is_active = TRUE;
    preedit_is_active = TRUE;
    gui_update_cursor(TRUE, FALSE);
    im_show_info();
}

/*
 * Callback invoked after end to the preedit.
 */
    સ્થિર શૂન્ય
im_preedit_end_cb(GtkIMContext *context UNUSED, gpointer data UNUSED)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    જો (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;

#  જો 0
    // Removal of this line suggested by Takuhiro Nishioka.  Fixes that IM was
    // switched off unintentionally.  We now use preedit_is_active (added by
    // SungHyun Nam).
    im_is_active = FALSE;
#  endif
    preedit_is_active = FALSE;
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
    સ્થિર શૂન્ય
im_preedit_changed_cb(GtkIMContext *context, gpointer data UNUSED)
{
    અક્ષર    *preedit_string = ખાલી;
    પૂર્ણાંક	    cursor_index    = 0;
    પૂર્ણાંક	    અંક_move_back   = 0;
    char_u  *str;
    char_u  *p;
    પૂર્ણાંક	    i;

    જો (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ખાલી,
					  &cursor_index);
    નહીંતર
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ખાલી,
					  ખાલી);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != ખાલી); // just in case

    જો (p_imst == IM_OVER_THE_SPOT)
    {
	જો (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	નહીંતર
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    નહીંતર
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	જો (preedit_start_col == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_start_col();
	}
	નહીંતર જો (cursor_index == 0 && preedit_string[0] == '\0')
	{
	    xim_has_preediting = FALSE;

	    // If at the start position (after typing backspace)
	    // preedit_start_col must be reset.
	    preedit_start_col = MAXCOL;
	}

	im_delete_preedit();

	// Compute the end of the preediting area: "preedit_end_col".
	// According to the documentation of
	// gtk_im_context_get_preedit_string(), the cursor_pos output argument
	// returns the offset in bytes.  This is unfortunately not true -- real
	// life shows the offset is in characters, and the GTK+ source code
	// agrees with me.  Will file a bug later.
	જો (preedit_start_col != MAXCOL)
	    preedit_end_col = preedit_start_col;
	str = (char_u *)preedit_string;
	માટે (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    પૂર્ણાંક is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    જો (!is_composing || p_deco)
	    {
		જો (i < cursor_index)
		    ++im_preedit_cursor;
		નહીંતર
		    ++im_preedit_trailing;
	    }
	    જો (!is_composing && i >= cursor_index)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++અંક_move_back;
	    }
	    જો (preedit_start_col != MAXCOL)
		preedit_end_col += utf_ptr2cells(p);
	}

	જો (p > str)
	{
	    im_add_to_આગત(str, (પૂર્ણાંક)(p - str));
	    im_correct_cursor(અંક_move_back);
	}
    }

    g_free(preedit_string);

    જો (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    સ્થિર પૂર્ણાંક
translate_pango_attributes(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    પૂર્ણાંક		    char_attr = HL_NORMAL;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    જો (attr != ખાલી && ((PangoAttrInt *)attr)->કિંમત
						 != (પૂર્ણાંક)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_WEIGHT);
    જો (attr != ખાલી && ((PangoAttrInt *)attr)->કિંમત >= (પૂર્ણાંક)PANGO_WEIGHT_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    જો (attr != ખાલી && ((PangoAttrInt *)attr)->કિંમત
						   != (પૂર્ણાંક)PANGO_STYLE_NORMAL)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    જો (attr != ખાલી)
    {
	અચલ PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	જો ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    પાછા char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    પૂર્ણાંક
im_get_feedback_attr(પૂર્ણાંક col)
{
    અક્ષર	    *preedit_string = ખાલી;
    PangoAttrList   *attr_list	    = ખાલી;
    પૂર્ણાંક		    char_attr	    = -1;

    જો (xic == ખાલી)
	પાછા char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ખાલી);

    જો (preedit_string != ખાલી && attr_list != ખાલી)
    {
	પૂર્ણાંક idx;

	// Get the byte index as used by PangoAttrIterator
	માટે (idx = 0; col > 0 && preedit_string[idx] != '\0'; --col)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	જો (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    પૂર્ણાંક			start, end;

	    char_attr = HL_NORMAL;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    કરો
	    {
		pango_attr_iterator_range(iter, &start, &end);

		જો (idx >= start && idx < end)
		    char_attr |= translate_pango_attributes(iter);
	    }
	    જ્યારે (pango_attr_iterator_next(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    જો (attr_list != ખાલી)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    પાછા char_attr;
}

    શૂન્ય
xim_init(શૂન્ય)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != ખાલી);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_window(gui.drawarea) != ખાલી);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), ખાલી);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_changed_cb), ખાલી);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_start_cb), ખાલી);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_end_cb), ખાલી);

#  ifdef USE_GTK4
    gtk_im_context_set_client_widget(xic, gui.drawarea);

    // Seed the cursor location immediately.  GTK4's Wayland IM module
    // batches text-input-v3 state and sends it on the next commit cycle
    // (typically triggered by focus_in).  Without this seed the very
    // first commit would carry the protocol default (0, 0, 0, 0) for the
    // cursor rectangle, and the IM's candidate window for the very first
    // composition would appear at the top-left of the surface instead of
    // next to the cursor.
    im_set_position(gui.row, gui.col);
#  નહીંતર
    gtk_im_context_set_client_window(xic, gtk_widget_get_window(gui.drawarea));
#  endif
}

    શૂન્ય
im_shutdown(શૂન્ય)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    જો (xic != ખાલી)
    {
	gtk_im_context_focus_out(xic);
	g_object_unref(xic);
	xic = ખાલી;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    જો (preedit_window != ખાલી)
    {
	gtk_widget_unparent(preedit_window);
	preedit_window = ખાલી;
	preedit_label = ખાલી;
    }
    જો (preedit_css_provider != ખાલી)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = ખાલી;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_active = FALSE;
    im_commit_handler_id = 0;
    જો (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
/*
 * Convert the string argument to keyval and state for GdkEventKey.
 * If str is valid return TRUE, otherwise FALSE.
 *
 * See 'imactivatekey' for documentation of the format.
 */
    સ્થિર પૂર્ણાંક
im_string_to_keyval(અચલ અક્ષર *str, unsigned પૂર્ણાંક *keyval, unsigned પૂર્ણાંક *state)
{
    અચલ અક્ષર	    *mods_end;
    unsigned	    tmp_keyval;
    unsigned	    tmp_state = 0;

    mods_end = strrchr(str, '-');
    mods_end = (mods_end != ખાલી) ? mods_end + 1 : str;

    // Parse modifier keys
    જ્યારે (str < mods_end)
	પસંદગી (*str++)
	{
	    સ્થિતિ '-':							તોડો;
	    સ્થિતિ 'S': સ્થિતિ 's': tmp_state |= (unsigned)GDK_SHIFT_MASK;	તોડો;
	    સ્થિતિ 'L': સ્થિતિ 'l': tmp_state |= (unsigned)GDK_LOCK_MASK;	તોડો;
	    સ્થિતિ 'C': સ્થિતિ 'c': tmp_state |= (unsigned)GDK_CONTROL_MASK;તોડો;
	    સ્થિતિ '1':		tmp_state |= (unsigned)GDK_MOD1_MASK;	તોડો;
	    સ્થિતિ '2':		tmp_state |= (unsigned)GDK_MOD2_MASK;	તોડો;
	    સ્થિતિ '3':		tmp_state |= (unsigned)GDK_MOD3_MASK;	તોડો;
	    સ્થિતિ '4':		tmp_state |= (unsigned)GDK_MOD4_MASK;	તોડો;
	    સ્થિતિ '5':		tmp_state |= (unsigned)GDK_MOD5_MASK;	તોડો;
	    મૂળભૂત:
		પાછા FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    જો (tmp_keyval == 0 || tmp_keyval == GDK_Void_પ્રતીક)
	પાછા FALSE;

    જો (keyval != ખાલી)
	*keyval = tmp_keyval;
    જો (state != ખાલી)
	*state = tmp_state;

    પાછા TRUE;
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
    પૂર્ણાંક
im_xim_isvalid_imactivate(શૂન્ય)
{
    જો (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_Void_પ્રતીક;
	im_activatekey_state  = 0;
	પાછા TRUE;
    }

    પાછા im_string_to_keyval((અચલ અક્ષર *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    સ્થિર શૂન્ય
im_synthesize_keypress(unsigned પૂર્ણાંક keyval, unsigned પૂર્ણાંક state)
{
    Gdk_Event_ચાવી *event;

    event = (Gdk_Event_ચાવી *)gdk_event_new(GDK_ચાવી_PRESS);
    g_object_ref(gtk_widget_get_window(gui.drawarea));
					// unreffed by gdk_event_free()
    event->window = gtk_widget_get_window(gui.drawarea);
    event->send_event = TRUE;
    event->time = GDK_CURRENT_TIME;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysymToKeycode(GDK_WINDOW_XDISPLAY(event->window), (ચાવી_Sym)keyval);
    event->length = 0;
    event->string = ખાલી;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_ચાવી_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    શૂન્ય
xim_reset(શૂન્ય)
{
#  ifdef FEAT_EVAL
    જો (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_active);
    નહીંતર
#  endif
    જો (xic != ખાલી)
    {
	gtk_im_context_reset(xic);

	જો (p_imdisable)
	    im_shutdown();
	નહીંતર
	{
	    xim_set_focus(gui.in_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_set_focus(gui.in_focus);
#  નહીંતર
	    જો (im_activatekey_keyval != GDK_Void_પ્રતીક)
	    {
		જો (im_is_active)
		{
		    g_signal_handler_block(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    નહીંતર
	    {
		im_shutdown();
		xim_init();
		xim_set_focus(gui.in_focus);
	    }
#  endif
	}
    }

    જો (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    પૂર્ણાંક
xim_queue_ચાવી_press_event(Gdk_Event_ચાવી *event, પૂર્ણાંક down)
{
#   ifdef FEAT_GUI_GTK
    જો (event->state & GDK_SUPER_MASK) પાછા FALSE;
#   endif
    જો (down)
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
	પસંદગી (event->keyval)
	{
	    સ્થિતિ GDK_KP_Add:      xim_expected_char = '+';  તોડો;
	    સ્થિતિ GDK_KP_Subtract: xim_expected_char = '-';  તોડો;
	    સ્થિતિ GDK_KP_Divide:   xim_expected_char = '/';  તોડો;
	    સ્થિતિ GDK_KP_Multiply: xim_expected_char = '*';  તોડો;
	    સ્થિતિ GDK_KP_Decimal:  xim_expected_char = '.';  તોડો;
	    સ્થિતિ GDK_KP_Equal:    xim_expected_char = '=';  તોડો;
	    સ્થિતિ GDK_KP_0:	  xim_expected_char = '0';  તોડો;
	    સ્થિતિ GDK_KP_1:	  xim_expected_char = '1';  તોડો;
	    સ્થિતિ GDK_KP_2:	  xim_expected_char = '2';  તોડો;
	    સ્થિતિ GDK_KP_3:	  xim_expected_char = '3';  તોડો;
	    સ્થિતિ GDK_KP_4:	  xim_expected_char = '4';  તોડો;
	    સ્થિતિ GDK_KP_5:	  xim_expected_char = '5';  તોડો;
	    સ્થિતિ GDK_KP_6:	  xim_expected_char = '6';  તોડો;
	    સ્થિતિ GDK_KP_7:	  xim_expected_char = '7';  તોડો;
	    સ્થિતિ GDK_KP_8:	  xim_expected_char = '8';  તોડો;
	    સ્થિતિ GDK_KP_9:	  xim_expected_char = '9';  તોડો;
	    સ્થિતિ GDK_space:	  xim_expected_char = ' ';  તોડો;
	    મૂળભૂત:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    જો (xic != ખાલી && !p_imdisable
		    && (State & (MODE_ઉમેરો | MODE_CMDLINE
					      | MODE_NORMAL | MODE_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	જો (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    unsigned પૂર્ણાંક state_mask;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_mask  = im_activatekey_state;
	    state_mask |= ((પૂર્ણાંક)GDK_SHIFT_MASK | (પૂર્ણાંક)GDK_CONTROL_MASK
							| (પૂર્ણાંક)GDK_MOD1_MASK);

	    જો ((event->state & state_mask) != im_activatekey_state)
		પાછા FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    જો (event->type != GDK_ચાવી_PRESS)
		પાછા TRUE;

	    જો (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
	    {
		im_set_active(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= MODE_LANGMAP;
		જો (State & MODE_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~MODE_LANGMAP;
		}
		નહીંતર
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= MODE_LANGMAP;
		}
		પાછા TRUE;
	    }

	    પાછા gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	જો (im_activatekey_keyval == GDK_Void_પ્રતીક || im_is_active)
	{
	    પૂર્ણાંક imresult = gtk_im_context_filter_keypress(xic, event);

	    જો (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		જો (!imresult && preedit_start_col != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    પાછા FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    જો (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		પાછા FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    પાછા imresult;
	}
    }

    પાછા FALSE;
}
#  નહીંતર // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    પૂર્ણાંક
im_xim_isvalid_imactivate(શૂન્ય)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    પાછા p_imak[0] == NUL;
}

    પૂર્ણાંક
xim_queue_ચાવી_press_event(GdkEvent *event UNUSED, પૂર્ણાંક down UNUSED)
{
    પાછા FALSE;
}
#  endif // !USE_GTK4

    પૂર્ણાંક
im_get_status(શૂન્ય)
{
#  ifdef FEAT_EVAL
    જો (USE_IMSTATUSFUNC)
	પાછા call_imstatusfunc();
#  endif
    પાછા im_is_active;
}

    પૂર્ણાંક
preedit_get_status(શૂન્ય)
{
    પાછા preedit_is_active;
}

    પૂર્ણાંક
im_is_preediting(શૂન્ય)
{
    પાછા xim_has_preediting;
}

# નહીંતર // !FEAT_GUI_GTK

સ્થિર પૂર્ણાંક	xim_is_active = FALSE;  // XIM should be active in the current
					// mode
સ્થિર પૂર્ણાંક	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
સ્થિર XIMStyle	આગત_style;
સ્થિર પૂર્ણાંક	status_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    શૂન્ય
im_set_active(પૂર્ણાંક active_arg)
{
    પૂર્ણાંક active = active_arg;

    // If 'imdisable' is set, XIM is never active.
    જો (p_imdisable)
	active = FALSE;
    નહીંતર જો (આગત_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	active = TRUE;

#  જો defined(FEAT_EVAL)
    જો (USE_IMACTIVATEFUNC)
    {
	જો (active != im_get_status())
	{
	    call_imactivatefunc(active);
	    xim_has_focus = active;
	}
	પાછા;
    }
#  endif

    જો (xic == ખાલી)
	પાછા;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_active = active;
    xim_set_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    શૂન્ય
xim_set_focus(પૂર્ણાંક focus)
{
    જો (xic == ખાલી)
	પાછા;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    જો (focus && xim_is_active)
    {
	જો (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    નહીંતર
    {
	જો (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    શૂન્ય
im_set_position(પૂર્ણાંક row UNUSED, પૂર્ણાંક col UNUSED)
{
    xim_set_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    શૂન્ય
xim_set_preedit(શૂન્ય)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    પૂર્ણાંક લીટી_space;

    જો (xic == ખાલી)
	પાછા;

    xim_set_focus(TRUE);

    જો (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							ખાલી);
	XSetICValues(xic, XNPreeditAttributes, attr_list, ખાલી);
	XFree(attr_list);
	પાછા;
    }

    જો (આગત_style & XIMPreeditPosition)
    {
	જો (xim_fg_color == INVALCOLOR)
	{
	    xim_fg_color = gui.def_norm_pixel;
	    xim_bg_color = gui.def_back_pixel;
	}
	over_spot.x = TEXT_X(gui.col);
	over_spot.y = TEXT_Y(gui.row);
	spot_area.x = 0;
	spot_area.y = 0;
	spot_area.height = gui.char_height * Rows;
	spot_area.width  = gui.char_width * Columns;
	લીટી_space = gui.char_height;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, લીટી_space,
					ખાલી);
	જો (XSetICValues(xic, XNPreeditAttributes, attr_list, ખાલી))
	    emsg(_(e_cannot_set_ic_કિંમતો));
	XFree(attr_list);
    }
}

#  જો defined(FEAT_GUI_X11)
#   જો defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

સ્થિર પૂર્ણાંક xim_real_init(Window x11_window, Display *x11_display);


#   ifdef USE_X11R6_XIM
    સ્થિર શૂન્ય
xim_instantiate_cb(
    Display	*display,
    XPointer	client_data UNUSED,
    XPointer	call_data UNUSED)
{
    Window	x11_window;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_instantiate_cb()\n");
#    endif

    gui_get_x11_windis(&x11_window, &x11_display);
    જો (display != x11_display)
	પાછા;

    xim_real_init(x11_window, x11_display);
    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    જો (xic != ખાલી)
	XUnregisterIMInstantiateCallback(x11_display, ખાલી, ખાલી, ખાલી,
					 xim_instantiate_cb, ખાલી);
}

    સ્થિર શૂન્ય
xim_destroy_cb(
    XIM		im UNUSED,
    XPointer	client_data UNUSED,
    XPointer	call_data UNUSED)
{
    Window	x11_window;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_destroy_cb()\n");
#    endif
    gui_get_x11_windis(&x11_window, &x11_display);

    xic = ખાલી;
    status_area_enabled = FALSE;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, ખાલી, ખાલી, ખાલી,
				   xim_instantiate_cb, ખાલી);
}
#   endif

    શૂન્ય
xim_init(શૂન્ય)
{
    Window	x11_window;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_window, &x11_display);

    xic = ખાલી;

    જો (xim_real_init(x11_window, x11_display))
	પાછા;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, ખાલી, ખાલી, ખાલી,
				   xim_instantiate_cb, ખાલી);
#   endif
}

    સ્થિર પૂર્ણાંક
xim_real_init(Window x11_window, Display *x11_display)
{
    પૂર્ણાંક		i;
    અક્ષર	*p,
		*s,
		*ns,
		*end,
		tmp[1024];
#   define IMLEN_MAX 40
    અક્ષર	buf[IMLEN_MAX + 7];
    XIM		xim = ખાલી;
    XIMStyles	*xim_styles;
    XIMStyle	this_આગત_style = 0;
    Boolean	found;
    XPoint	over_spot;
    XVaNestedList preedit_list, status_list;

    આગત_style = 0;
    status_area_enabled = FALSE;

    જો (xic != ખાલી)
	પાછા FALSE;

    જો (gui.rsrc_આગત_method != ખાલી && *gui.rsrc_આગત_method != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_આગત_method,
	    કદ(tmp) - 1);
	માટે (ns = s = tmp; ns != ખાલી && *s != NUL;)
	{
	    s = (અક્ષર *)skipwhite((char_u *)s);
	    જો (*s == NUL)
		તોડો;
	    જો ((ns = end = strchr(s, ',')) == ખાલી)
		end = s + strlen(s);
	    જ્યારે (SAFE_isspace(end[-1]))
		end--;
	    *end = NUL;

	    જો (strlen(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		જો ((p = XSetLocaleModifiers(buf)) != ખાલી && *p != NUL
			&& (xim = XOpenIM(x11_display, ખાલી, ખાલી, ખાલી))
								      != ખાલી)
		    તોડો;
	    }

	    s = ns + 1;
	}
    }

    જો (xim == ખાલી && (p = XSetLocaleModifiers("")) != ખાલી && *p != NUL)
	xim = XOpenIM(x11_display, ખાલી, ખાલી, ખાલી);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    જો (xim == ખાલી && (p = XSetLocaleModifiers("@im=none")) != ખાલી
								 && *p != NUL)
	xim = XOpenIM(x11_display, ખાલી, ખાલી, ખાલી);

    જો (xim == ખાલી)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	જો (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_to_open_આગત_method));
	    verbose_leave();
	}
	પાછા FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_data = ખાલી;
	જો (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, ખાલી))
	    emsg(_(e_warning_could_not_set_destroy_callback_to_im));
    }
#   endif

    જો (XGetIMValues(xim, XNQuery_આગત_Style, &xim_styles, ખાલી) || !xim_styles)
    {
	emsg(_(e_આગત_method_doesnt_support_any_style));
	XCloseIM(xim);
	પાછા FALSE;
    }

    found = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    કદ(tmp) - 1);
    માટે (s = tmp; s && !found; )
    {
	જ્યારે (*s && SAFE_isspace(*s))
	    s++;
	જો (!*s)
	    તોડો;
	જો ((ns = end = strchr(s, ',')) != 0)
	    ns++;
	નહીંતર
	    end = s + strlen(s);
	જ્યારે (SAFE_isspace(*end))
	    end--;
	*end = '\0';

	જો (!strcmp(s, "OverTheSpot"))
	    this_આગત_style = (XIMPreeditPosition | XIMStatusArea);
	નહીંતર જો (!strcmp(s, "OffTheSpot"))
	    this_આગત_style = (XIMPreeditArea | XIMStatusArea);
	નહીંતર જો (!strcmp(s, "Root"))
	    this_આગત_style = (XIMPreeditNothing | XIMStatusNothing);

	માટે (i = 0; (unsigned short)i < xim_styles->ગણતરી_styles; i++)
	{
	    જો (this_આગત_style == xim_styles->supported_styles[i])
	    {
		found = True;
		તોડો;
	    }
	}
	જો (!found)
	    માટે (i = 0; (unsigned short)i < xim_styles->ગણતરી_styles; i++)
	    {
		જો ((xim_styles->supported_styles[i] & this_આગત_style)
			== (this_આગત_style & ~XIMStatusArea))
		{
		    this_આગત_style &= ~XIMStatusArea;
		    found = True;
		    તોડો;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    જો (!found)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	જો (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_આગત_method_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	પાછા FALSE;
    }

    over_spot.x = TEXT_X(gui.col);
    over_spot.y = TEXT_Y(gui.row);
    આગત_style = this_આગત_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    જો (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				ખાલી);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				ખાલી);
    }
    નહીંતર
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				ખાલી);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				ખાલી);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, આગત_style,
		    XNClientWindow, x11_window,
		    XNFocusWindow, gui.wid,
		    XNPreeditAttributes, preedit_list,
		    XNStatusAttributes, status_list,
		    ખાલી);
    XFree(status_list);
    XFree(preedit_list);
    જો (xic != ખાલી)
    {
	જો (આગત_style & XIMStatusArea)
	{
	    xim_set_status_area();
	    status_area_enabled = TRUE;
	}
	નહીંતર
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    નહીંતર
    {
	જો (!is_not_a_term())
	    emsg(_(e_failed_to_create_આગત_context));
	XCloseIM(xim);
	પાછા FALSE;
    }

    પાછા TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    પૂર્ણાંક
im_get_status(શૂન્ય)
{
#  ifdef FEAT_EVAL
    જો (USE_IMSTATUSFUNC)
	પાછા call_imstatusfunc();
#  endif
    પાછા xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# જો !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    શૂન્ય
xim_set_status_area(શૂન્ય)
{
    XVaNestedList preedit_list = 0, status_list = 0, list = 0;
    XRectangle pre_area, status_area;

    જો (xic == ખાલી)
	પાછા;

    જો (આગત_style & XIMStatusArea)
    {
	જો (આગત_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    status_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, ખાલી);
	    XGetICValues(xic, XNStatusAttributes, status_list, ખાલી);
	    XFree(status_list);

	    status_area.width = needed_rect->width;
	}
	નહીંતર
	    status_area.width = gui.char_width * Columns;

	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	જો (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	જો (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.height = gui.char_height;
	status_list = XVaCreateNestedList(0, XNArea, &status_area, ખાલી);
    }
    નહીંતર
    {
	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	જો (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	જો (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.width = 0;
	status_area.height = gui.char_height;
    }

    જો (આગત_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = status_area.x + status_area.width;
	pre_area.y = gui.char_height * Rows + gui.border_offset;
	pre_area.width = gui.char_width * Columns - pre_area.x;
	જો (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	જો (gui.menu_is_active)
	    pre_area.y += gui.menu_height;
#  endif
	pre_area.height = gui.char_height;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ખાલી);
    }
    નહીંતર જો (આગત_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.height = gui.char_height * Rows;
	pre_area.width = gui.char_width * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ખાલી);
    }

    જો (preedit_list && status_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   XNStatusAttributes, status_list, ખાલી);
    નહીંતર જો (preedit_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   ખાલી);
    નહીંતર જો (status_list)
	list = XVaCreateNestedList(0, XNStatusAttributes, status_list,
				   ખાલી);
    નહીંતર
	list = ખાલી;

    જો (list)
    {
	XSetICValues(xic, XNVaNestedList, list, ખાલી);
	XFree(list);
    }
    જો (status_list)
	XFree(status_list);
    જો (preedit_list)
	XFree(preedit_list);
}

    પૂર્ણાંક
xim_get_status_area_height(શૂન્ય)
{
    જો (status_area_enabled)
	પાછા gui.char_height;
    પાછા 0;
}
# endif

#નહીંતર // !defined(FEAT_XIM)

# જો defined(IME_WITHOUT_XIM) || defined(VIMDLL)
સ્થિર પૂર્ણાંક im_was_set_active = FALSE;

    પૂર્ણાંક
#  ifdef VIMDLL
mbyte_im_get_status(શૂન્ય)
#  નહીંતર
im_get_status(શૂન્ય)
#  endif
{
#  જો defined(FEAT_EVAL)
    જો (USE_IMSTATUSFUNC)
	પાછા call_imstatusfunc();
#  endif
    પાછા im_was_set_active;
}

    શૂન્ય
#  ifdef VIMDLL
mbyte_im_set_active(પૂર્ણાંક active_arg)
#  નહીંતર
im_set_active(પૂર્ણાંક active_arg)
#  endif
{
#  જો defined(FEAT_EVAL)
    પૂર્ણાંક	    active = !p_imdisable && active_arg;

    જો (USE_IMACTIVATEFUNC && active != im_get_status())
    {
	call_imactivatefunc(active);
	im_was_set_active = active;
    }
#  endif
}

#  જો defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    શૂન્ય
im_set_position(પૂર્ણાંક row UNUSED, પૂર્ણાંક col UNUSED)
{
}
#  endif
# endif

#endif // FEAT_XIM
