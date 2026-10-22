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

#ضمن "vim.h"

#אם !defined(GTK_CHECK_VERSION)
# define GTK_CHECK_VERSION(a, b, c) 0
#endif

#אם defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# אחרת
#  include <gdk/gdkkeysyms.h>
# endif
# אם !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  אחרת
#   include <gdk/gdkx.h>
#  endif
# endif
#endif

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#אם defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
ساكن فراغ xim_log(حرف *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    ساكن فراغ
xim_log(حرف *s, ...)
{
    va_list arglist;
    ساكن FILE *fd = عدم;

    אם (fd == (FILE *)-1)
	החזר;
    אם (fd == عدم)
    {
	fd = mch_fopen("xim.log", "w");
	אם (fd == عدم)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    החזר;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#endif

#אם defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.in_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.in_use && *p_imsf != NUL)
#אחרת
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#endif

#אם defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
ساكن callback_T imaf_cb;	    // 'imactivatefunc' callback function
ساكن callback_T imsf_cb;	    // 'imstatusfunc' callback function

    حرف *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    אם (option_set_callback_func(p_imaf, &imaf_cb) == FAIL)
	החזר e_invalid_argument;

    החזר عدم;
}

    حرف *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    אם (option_set_callback_func(p_imsf, &imsf_cb) == FAIL)
	החזר e_invalid_argument;

    החזר عدم;
}

    ساكن فراغ
call_imactivatefunc(صحيح active)
{
    typval_T argv[2];
    صحيح save_KeyTyped = KeyTyped;

    argv[0].v_type = VAR_NUMBER;
    argv[0].vval.v_number = active ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (فراغ)call_callback_retnr(&imaf_cb, 1, argv);

    KeyTyped = save_KeyTyped;
}

    ساكن صحيح
call_imstatusfunc(فراغ)
{
    صحيح is_active;
    صحيح save_KeyTyped = KeyTyped;

    // FIXME: Don't execute user function in unsafe situation.
    אם (exiting || is_autocmd_blocked())
	החזר FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++msg_silent;
    is_active = call_callback_retnr(&imsf_cb, 0, عدم);
    --msg_silent;

    KeyTyped = save_KeyTyped;
    החזר (is_active > 0);
}
#endif

#אם defined(EXITFREE)
    فراغ
free_xim_stuff(فراغ)
{
# אם defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#endif

#אם defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    صحيح
set_ref_in_im_funcs(صحيح copyID UNUSED)
{
    صحيح abort = FALSE;

# אם defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = set_ref_in_callback(&imaf_cb, copyID);
    abort = abort || set_ref_in_callback(&imsf_cb, copyID);
# endif

    החזר abort;
}
#endif


#אם defined(FEAT_XIM)

# אם defined(FEAT_GUI_GTK)
ساكن صحيح xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    ساكن فراغ
init_preedit_start_col(فراغ)
{
    אם (State & MODE_CMDLINE)
	preedit_start_col = cmdline_getvcol_cursor();
    אחרת אם (curwin != عدم && curwin->w_buffer != عدم)
	getvcol(curwin, &curwin->w_cursor, &preedit_start_col, عدم, عدم, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_changed_while_preediting = curbuf->b_changed;
}

ساكن صحيح im_is_active	       = FALSE;	// IM is enabled for current mode
ساكن صحيح preedit_is_active   = FALSE;
ساكن صحيح im_preedit_cursor   = 0;	// cursor offset in characters
ساكن صحيح im_preedit_trailing = 0;	// number of characters after cursor

ساكن unsigned long im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_KEY_VoidSymbol
#    define GDK_KEY_VoidSymbol GDK_VoidSymbol
#   endif
ساكن unsigned صحيح  im_activatekey_keyval = GDK_KEY_VoidSymbol;
ساكن unsigned صحيح  im_activatekey_state  = 0;
#  endif

ساكن GtkWidget *preedit_window = عدم;
ساكن GtkWidget *preedit_label = عدم;
#  ifdef USE_GTK4
ساكن صحيح preedit_label_width = 0;	// last measured popover natural width
ساكن صحيح preedit_popover_height = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
ساكن GtkCssProvider *preedit_css_provider = عدم;
ساكن guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

ساكن فراغ im_preedit_window_set_position(فراغ);

    فراغ
im_set_active(صحيح active)
{
    صحيح was_active;

    was_active = !!im_get_status();
    im_is_active = (active && !p_imdisable);

    אם (im_is_active != was_active)
	xim_reset();
}

    فراغ
xim_set_focus(صحيح focus)
{
    אם (xic == عدم)
	החזר;

    אם (focus)
	gtk_im_context_focus_in(xic);
    אחרת
	gtk_im_context_focus_out(xic);
}

    فراغ
im_set_position(صحيح row, صحيح col)
{
    אם (xic == عدم)
	החזר;

    GdkRectangle area;

    area.x = FILL_X(col);
    area.y = FILL_Y(row);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.width  = gui.char_width
		    * ((ScreenLines != عدم && mb_lefthalve(row, col)) ? 2 : 1);
    area.height = gui.char_height;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    אם (p_imst == IM_OVER_THE_SPOT)
    {
	صحيح popover_h = preedit_popover_height;
	אם (popover_h <= 0)
	    popover_h = gui.char_height;
	אם (popover_h > area.height)
	    area.height = popover_h;
    }
#  endif

    gtk_im_context_set_cursor_location(xic, &area);

    אם (p_imst == IM_OVER_THE_SPOT)
	im_preedit_window_set_position();
}

#  אם 0 || defined(PROTO) // apparently only used in gui_x11.c
    فراغ
xim_set_preedit(فراغ)
{
    im_set_position(gui.row, gui.col);
}
#  endif

    ساكن فراغ
im_add_to_input(char_u *str, صحيح len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    אם (input_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&input_conv, str, &len);
	g_return_if_fail(str != عدم);
    }

    add_to_input_buf_csi(str, len);

    אם (input_conv.vc_type != CONV_NONE)
	vim_free(str);

    אם (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ساكن فراغ
im_preedit_window_set_position(فراغ)
{
    אם (preedit_window == عدم)
	החזר;

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
    אם (!gtk_widget_compute_point(gui.drawarea, gui.mainwin,
						&pt_in, &pt_out))
    {
	pt_out.x = pt_in.x;
	pt_out.y = pt_in.y;
    }
    rect.x = (صحيح)pt_out.x;
    rect.y = (صحيح)pt_out.y;
    rect.width = preedit_label_width;
    rect.height = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    صحيح offset = gui.char_height - preedit_popover_height;
    gtk_popover_set_offset(GTK_POPOVER(preedit_window), 0, offset);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_set_halign(preedit_window, GTK_ALIGN_START);
    gtk_popover_set_pointing_to(GTK_POPOVER(preedit_window), &rect);
#  אחרת
    صحيح x, y, width, height;
    صحيح screen_x, screen_y, screen_width, screen_height;

    gui_gtk_get_screen_geom_of_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_width, &screen_height);
    gdk_window_get_origin(gtk_widget_get_window(gui.drawarea), &x, &y);
    gtk_window_get_size(GTK_WINDOW(preedit_window), &width, &height);
    x = x + FILL_X(gui.col);
    y = y + FILL_Y(gui.row);
    אם (x + width > screen_x + screen_width)
	x = screen_x + screen_width - width;
    אם (y + height > screen_y + screen_height)
	y = screen_y + screen_height - height;
    gtk_window_move(GTK_WINDOW(preedit_window), x, y);
#  endif
}

    ساكن فراغ
im_preedit_window_open(فراغ)
{
    حرف *preedit_string;
#  אם !GTK_CHECK_VERSION(3,16,0)
    حرف buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  אם GTK_CHECK_VERSION(3,0,0)
#   אם !GTK_CHECK_VERSION(3,16,0)
    GdkRGBA color;
#   endif
#  אחרת
    GdkColor color;
#  endif
    gint w, h;

    אם (preedit_window == عدم)
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
#  אחרת
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
#  אחרת
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
    אם (preedit_css_provider == عدم || preedit_css_cached_bg != gui.back_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	אם (preedit_css_provider != عدم)
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
#   אם GTK_CHECK_VERSION(3,16,0)
    {
	GtkStyleContext * ثابت context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * ثابت provider = gtk_css_provider_new();
	gchar		   *css = عدم;
	ثابت حرف * ثابت fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_size(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = عدم;

	אם (!pango_font_description_get_size_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * ثابت screen
		  = gdk_window_get_screen(gtk_widget_get_window(gui.mainwin));
	    ثابت gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	אם (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	אחרת
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != عدم ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.back_pixel >> 16) & 0xff,
		(gui.back_pixel >> 8) & 0xff,
		gui.back_pixel & 0xff);

	gtk_css_provider_load_from_data(provider, css, -1, عدم);
	gtk_style_context_add_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_VERSION(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, حجم(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_NORMAL, &color);

    vim_snprintf(buf, حجم(buf), "#%06X", gui.back_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_NORMAL,
								      &color);
#   אחרת
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, حجم(buf), "#%06X", (unsigned)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_NORMAL, &color);

    vim_snprintf(buf, حجم(buf), "#%06X", (unsigned)gui.back_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_window, GTK_STATE_NORMAL, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, عدم);

    אם (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	אם (attr_list == عدم)
	    attr_list = pango_attr_list_new();

	אם (gui.norm_font != عدم)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->start_index = 0;
	    pa->end_index = G_MAXUINT;
	    pango_attr_list_insert(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_insert(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.back_pixel >> 16) & 0xff) * 257,
		((gui.back_pixel >> 8) & 0xff) * 257,
		(gui.back_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_insert(attr_list, pa);
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
	אם (xic != عدم)
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
	אם (gtk_widget_get_mapped(preedit_window))
	    gtk_popover_popdown(GTK_POPOVER(preedit_window));

	im_preedit_window_set_position();

	gtk_widget_queue_resize(preedit_window);
	gtk_popover_popup(GTK_POPOVER(preedit_window));
#  אחרת
	gtk_window_resize(GTK_WINDOW(preedit_window), w, h);
	gtk_widget_show_all(preedit_window);

	im_preedit_window_set_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    ساكن فراغ
im_preedit_window_close(فراغ)
{
    אם (preedit_window == عدم)
	החזר;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_window));
#  אחרת
    gtk_widget_hide(preedit_window);
#  endif
}

    ساكن فراغ
im_show_preedit(فراغ)
{
    im_preedit_window_open();

    אם (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ساكن فراغ
im_delete_preedit(فراغ)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    אם (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_window_close();
	החזר;
    }

    אם (State & MODE_NORMAL
#  ifdef FEAT_TERMINAL
	    && !term_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	החזר;
    }
    עבור (; im_preedit_cursor > 0; --im_preedit_cursor)
	add_to_input_buf(bskey, (صحيح)حجم(bskey));

    עבור (; im_preedit_trailing > 0; --im_preedit_trailing)
	add_to_input_buf(delkey, (صحيح)حجم(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    ساكن فراغ
im_correct_cursor(صحيح num_move_back)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    אם (State & MODE_NORMAL)
	החזר;
#  ifdef FEAT_RIGHTLEFT
    אם ((State & MODE_CMDLINE) == 0 && curwin != عدم && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    עבור (; num_move_back > 0; --num_move_back)
	add_to_input_buf(backkey, (صحيح)حجم(backkey));
}

ساكن صحيح xim_expected_char = NUL;
ساكن صحيح xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    ساكن فراغ
im_show_info(فراغ)
{
    صحيح	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    אם ((State & MODE_NORMAL) || (State & MODE_INSERT))
	setcursor();
    out_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    ساكن فراغ
im_commit_cb(GtkIMContext *context UNUSED,
	     ثابت gchar *str,
	     gpointer data UNUSED)
{
    صحيح		slen = (صحيح)STRLEN(str);
    صحيح		add_to_input = TRUE;
    صحيح		clen;
    صحيح		len = slen;
    صحيح		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    אם (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	אם (preedit_start_col == MAXCOL)
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
	אם (input_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&input_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != عدم);
	}
	אחרת
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	אם (input_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_start_col += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    אם (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	אם (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    add_to_input = FALSE;
	}
	אחרת
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    אם (add_to_input)
	im_add_to_input((char_u *)str, slen);

    אם (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	אם (!commit_with_preedit)
	    preedit_start_col = MAXCOL;

	// This flag is used in changed() at next call.
	xim_changed_while_preediting = TRUE;
    }

    אם (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    ساكن فراغ
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
    ساكن فراغ
im_preedit_end_cb(GtkIMContext *context UNUSED, gpointer data UNUSED)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    אם (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;

#  אם 0
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
    ساكن فراغ
im_preedit_changed_cb(GtkIMContext *context, gpointer data UNUSED)
{
    حرف    *preedit_string = عدم;
    صحيح	    cursor_index    = 0;
    صحيح	    num_move_back   = 0;
    char_u  *str;
    char_u  *p;
    صحيح	    i;

    אם (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, عدم,
					  &cursor_index);
    אחרת
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, عدم,
					  عدم);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != عدم); // just in case

    אם (p_imst == IM_OVER_THE_SPOT)
    {
	אם (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	אחרת
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    אחרת
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	אם (preedit_start_col == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_start_col();
	}
	אחרת אם (cursor_index == 0 && preedit_string[0] == '\0')
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
	אם (preedit_start_col != MAXCOL)
	    preedit_end_col = preedit_start_col;
	str = (char_u *)preedit_string;
	עבור (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    صحيح is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    אם (!is_composing || p_deco)
	    {
		אם (i < cursor_index)
		    ++im_preedit_cursor;
		אחרת
		    ++im_preedit_trailing;
	    }
	    אם (!is_composing && i >= cursor_index)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++num_move_back;
	    }
	    אם (preedit_start_col != MAXCOL)
		preedit_end_col += utf_ptr2cells(p);
	}

	אם (p > str)
	{
	    im_add_to_input(str, (صحيح)(p - str));
	    im_correct_cursor(num_move_back);
	}
    }

    g_free(preedit_string);

    אם (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    ساكن صحيح
translate_pango_attributes(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    صحيح		    char_attr = HL_NORMAL;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    אם (attr != عدم && ((PangoAttrInt *)attr)->value
						 != (صحيح)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_WEIGHT);
    אם (attr != عدم && ((PangoAttrInt *)attr)->value >= (صحيح)PANGO_WEIGHT_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    אם (attr != عدم && ((PangoAttrInt *)attr)->value
						   != (صحيح)PANGO_STYLE_NORMAL)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    אם (attr != عدم)
    {
	ثابت PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	אם ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    החזר char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    صحيح
im_get_feedback_attr(صحيح col)
{
    حرف	    *preedit_string = عدم;
    PangoAttrList   *attr_list	    = عدم;
    صحيح		    char_attr	    = -1;

    אם (xic == عدم)
	החזר char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, عدم);

    אם (preedit_string != عدم && attr_list != عدم)
    {
	صحيح idx;

	// Get the byte index as used by PangoAttrIterator
	עבור (idx = 0; col > 0 && preedit_string[idx] != '\0'; --col)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	אם (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    صحيح			start, end;

	    char_attr = HL_NORMAL;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    עשה
	    {
		pango_attr_iterator_range(iter, &start, &end);

		אם (idx >= start && idx < end)
		    char_attr |= translate_pango_attributes(iter);
	    }
	    כלעוד (pango_attr_iterator_next(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    אם (attr_list != عدم)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    החזר char_attr;
}

    فراغ
xim_init(فراغ)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != عدم);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_window(gui.drawarea) != عدم);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), عدم);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_changed_cb), عدم);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_start_cb), عدم);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_end_cb), عدم);

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
#  אחרת
    gtk_im_context_set_client_window(xic, gtk_widget_get_window(gui.drawarea));
#  endif
}

    فراغ
im_shutdown(فراغ)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    אם (xic != عدم)
    {
	gtk_im_context_focus_out(xic);
	g_object_unref(xic);
	xic = عدم;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    אם (preedit_window != عدم)
    {
	gtk_widget_unparent(preedit_window);
	preedit_window = عدم;
	preedit_label = عدم;
    }
    אם (preedit_css_provider != عدم)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = عدم;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_active = FALSE;
    im_commit_handler_id = 0;
    אם (p_imst == IM_ON_THE_SPOT)
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
    ساكن صحيح
im_string_to_keyval(ثابت حرف *str, unsigned صحيح *keyval, unsigned صحيح *state)
{
    ثابت حرف	    *mods_end;
    unsigned	    tmp_keyval;
    unsigned	    tmp_state = 0;

    mods_end = strrchr(str, '-');
    mods_end = (mods_end != عدم) ? mods_end + 1 : str;

    // Parse modifier keys
    כלעוד (str < mods_end)
	بدل (*str++)
	{
	    حالة '-':							اكسر;
	    حالة 'S': حالة 's': tmp_state |= (unsigned)GDK_SHIFT_MASK;	اكسر;
	    حالة 'L': حالة 'l': tmp_state |= (unsigned)GDK_LOCK_MASK;	اكسر;
	    حالة 'C': حالة 'c': tmp_state |= (unsigned)GDK_CONTROL_MASK;اكسر;
	    حالة '1':		tmp_state |= (unsigned)GDK_MOD1_MASK;	اكسر;
	    حالة '2':		tmp_state |= (unsigned)GDK_MOD2_MASK;	اكسر;
	    حالة '3':		tmp_state |= (unsigned)GDK_MOD3_MASK;	اكسر;
	    حالة '4':		tmp_state |= (unsigned)GDK_MOD4_MASK;	اكسر;
	    حالة '5':		tmp_state |= (unsigned)GDK_MOD5_MASK;	اكسر;
	    افتراضي:
		החזר FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    אם (tmp_keyval == 0 || tmp_keyval == GDK_VoidSymbol)
	החזר FALSE;

    אם (keyval != عدم)
	*keyval = tmp_keyval;
    אם (state != عدم)
	*state = tmp_state;

    החזר TRUE;
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
    صحيح
im_xim_isvalid_imactivate(فراغ)
{
    אם (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_VoidSymbol;
	im_activatekey_state  = 0;
	החזר TRUE;
    }

    החזר im_string_to_keyval((ثابت حرف *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    ساكن فراغ
im_synthesize_keypress(unsigned صحيح keyval, unsigned صحيح state)
{
    GdkEventKey *event;

    event = (GdkEventKey *)gdk_event_new(GDK_KEY_PRESS);
    g_object_ref(gtk_widget_get_window(gui.drawarea));
					// unreffed by gdk_event_free()
    event->window = gtk_widget_get_window(gui.drawarea);
    event->send_event = TRUE;
    event->time = GDK_CURRENT_TIME;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysymToKeycode(GDK_WINDOW_XDISPLAY(event->window), (KeySym)keyval);
    event->length = 0;
    event->string = عدم;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_KEY_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    فراغ
xim_reset(فراغ)
{
#  ifdef FEAT_EVAL
    אם (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_active);
    אחרת
#  endif
    אם (xic != عدم)
    {
	gtk_im_context_reset(xic);

	אם (p_imdisable)
	    im_shutdown();
	אחרת
	{
	    xim_set_focus(gui.in_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_set_focus(gui.in_focus);
#  אחרת
	    אם (im_activatekey_keyval != GDK_VoidSymbol)
	    {
		אם (im_is_active)
		{
		    g_signal_handler_block(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    אחרת
	    {
		im_shutdown();
		xim_init();
		xim_set_focus(gui.in_focus);
	    }
#  endif
	}
    }

    אם (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    صحيح
xim_queue_key_press_event(GdkEventKey *event, صحيح down)
{
#   ifdef FEAT_GUI_GTK
    אם (event->state & GDK_SUPER_MASK) החזר FALSE;
#   endif
    אם (down)
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
	بدل (event->keyval)
	{
	    حالة GDK_KP_Add:      xim_expected_char = '+';  اكسر;
	    حالة GDK_KP_Subtract: xim_expected_char = '-';  اكسر;
	    حالة GDK_KP_Divide:   xim_expected_char = '/';  اكسر;
	    حالة GDK_KP_Multiply: xim_expected_char = '*';  اكسر;
	    حالة GDK_KP_Decimal:  xim_expected_char = '.';  اكسر;
	    حالة GDK_KP_Equal:    xim_expected_char = '=';  اكسر;
	    حالة GDK_KP_0:	  xim_expected_char = '0';  اكسر;
	    حالة GDK_KP_1:	  xim_expected_char = '1';  اكسر;
	    حالة GDK_KP_2:	  xim_expected_char = '2';  اكسر;
	    حالة GDK_KP_3:	  xim_expected_char = '3';  اكسر;
	    حالة GDK_KP_4:	  xim_expected_char = '4';  اكسر;
	    حالة GDK_KP_5:	  xim_expected_char = '5';  اكسر;
	    حالة GDK_KP_6:	  xim_expected_char = '6';  اكسر;
	    حالة GDK_KP_7:	  xim_expected_char = '7';  اكسر;
	    حالة GDK_KP_8:	  xim_expected_char = '8';  اكسر;
	    حالة GDK_KP_9:	  xim_expected_char = '9';  اكسر;
	    حالة GDK_space:	  xim_expected_char = ' ';  اكسر;
	    افتراضي:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    אם (xic != عدم && !p_imdisable
		    && (State & (MODE_INSERT | MODE_CMDLINE
					      | MODE_NORMAL | MODE_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	אם (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    unsigned صحيح state_mask;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_mask  = im_activatekey_state;
	    state_mask |= ((صحيح)GDK_SHIFT_MASK | (صحيح)GDK_CONTROL_MASK
							| (صحيح)GDK_MOD1_MASK);

	    אם ((event->state & state_mask) != im_activatekey_state)
		החזר FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    אם (event->type != GDK_KEY_PRESS)
		החזר TRUE;

	    אם (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
	    {
		im_set_active(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= MODE_LANGMAP;
		אם (State & MODE_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~MODE_LANGMAP;
		}
		אחרת
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= MODE_LANGMAP;
		}
		החזר TRUE;
	    }

	    החזר gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	אם (im_activatekey_keyval == GDK_VoidSymbol || im_is_active)
	{
	    صحيح imresult = gtk_im_context_filter_keypress(xic, event);

	    אם (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		אם (!imresult && preedit_start_col != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    החזר FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    אם (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		החזר FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    החזר imresult;
	}
    }

    החזר FALSE;
}
#  אחרת // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    صحيح
im_xim_isvalid_imactivate(فراغ)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    החזר p_imak[0] == NUL;
}

    صحيح
xim_queue_key_press_event(GdkEvent *event UNUSED, صحيح down UNUSED)
{
    החזר FALSE;
}
#  endif // !USE_GTK4

    صحيح
im_get_status(فراغ)
{
#  ifdef FEAT_EVAL
    אם (USE_IMSTATUSFUNC)
	החזר call_imstatusfunc();
#  endif
    החזר im_is_active;
}

    صحيح
preedit_get_status(فراغ)
{
    החזר preedit_is_active;
}

    صحيح
im_is_preediting(فراغ)
{
    החזר xim_has_preediting;
}

# אחרת // !FEAT_GUI_GTK

ساكن صحيح	xim_is_active = FALSE;  // XIM should be active in the current
					// mode
ساكن صحيح	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
ساكن XIMStyle	input_style;
ساكن صحيح	status_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    فراغ
im_set_active(صحيح active_arg)
{
    صحيح active = active_arg;

    // If 'imdisable' is set, XIM is never active.
    אם (p_imdisable)
	active = FALSE;
    אחרת אם (input_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	active = TRUE;

#  אם defined(FEAT_EVAL)
    אם (USE_IMACTIVATEFUNC)
    {
	אם (active != im_get_status())
	{
	    call_imactivatefunc(active);
	    xim_has_focus = active;
	}
	החזר;
    }
#  endif

    אם (xic == عدم)
	החזר;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_active = active;
    xim_set_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    فراغ
xim_set_focus(صحيح focus)
{
    אם (xic == عدم)
	החזר;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    אם (focus && xim_is_active)
    {
	אם (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    אחרת
    {
	אם (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    فراغ
im_set_position(صحيح row UNUSED, صحيح col UNUSED)
{
    xim_set_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    فراغ
xim_set_preedit(فراغ)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    صحيح line_space;

    אם (xic == عدم)
	החזר;

    xim_set_focus(TRUE);

    אם (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							عدم);
	XSetICValues(xic, XNPreeditAttributes, attr_list, عدم);
	XFree(attr_list);
	החזר;
    }

    אם (input_style & XIMPreeditPosition)
    {
	אם (xim_fg_color == INVALCOLOR)
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
	line_space = gui.char_height;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, line_space,
					عدم);
	אם (XSetICValues(xic, XNPreeditAttributes, attr_list, عدم))
	    emsg(_(e_cannot_set_ic_values));
	XFree(attr_list);
    }
}

#  אם defined(FEAT_GUI_X11)
#   אם defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

ساكن صحيح xim_real_init(Window x11_window, Display *x11_display);


#   ifdef USE_X11R6_XIM
    ساكن فراغ
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
    אם (display != x11_display)
	החזר;

    xim_real_init(x11_window, x11_display);
    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    אם (xic != عدم)
	XUnregisterIMInstantiateCallback(x11_display, عدم, عدم, عدم,
					 xim_instantiate_cb, عدم);
}

    ساكن فراغ
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

    xic = عدم;
    status_area_enabled = FALSE;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, عدم, عدم, عدم,
				   xim_instantiate_cb, عدم);
}
#   endif

    فراغ
xim_init(فراغ)
{
    Window	x11_window;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_window, &x11_display);

    xic = عدم;

    אם (xim_real_init(x11_window, x11_display))
	החזר;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, عدم, عدم, عدم,
				   xim_instantiate_cb, عدم);
#   endif
}

    ساكن صحيح
xim_real_init(Window x11_window, Display *x11_display)
{
    صحيح		i;
    حرف	*p,
		*s,
		*ns,
		*end,
		tmp[1024];
#   define IMLEN_MAX 40
    حرف	buf[IMLEN_MAX + 7];
    XIM		xim = عدم;
    XIMStyles	*xim_styles;
    XIMStyle	this_input_style = 0;
    Boolean	found;
    XPoint	over_spot;
    XVaNestedList preedit_list, status_list;

    input_style = 0;
    status_area_enabled = FALSE;

    אם (xic != عدم)
	החזר FALSE;

    אם (gui.rsrc_input_method != عدم && *gui.rsrc_input_method != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_input_method,
	    حجم(tmp) - 1);
	עבור (ns = s = tmp; ns != عدم && *s != NUL;)
	{
	    s = (حرف *)skipwhite((char_u *)s);
	    אם (*s == NUL)
		اكسر;
	    אם ((ns = end = strchr(s, ',')) == عدم)
		end = s + strlen(s);
	    כלעוד (SAFE_isspace(end[-1]))
		end--;
	    *end = NUL;

	    אם (strlen(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		אם ((p = XSetLocaleModifiers(buf)) != عدم && *p != NUL
			&& (xim = XOpenIM(x11_display, عدم, عدم, عدم))
								      != عدم)
		    اكسر;
	    }

	    s = ns + 1;
	}
    }

    אם (xim == عدم && (p = XSetLocaleModifiers("")) != عدم && *p != NUL)
	xim = XOpenIM(x11_display, عدم, عدم, عدم);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    אם (xim == عدم && (p = XSetLocaleModifiers("@im=none")) != عدم
								 && *p != NUL)
	xim = XOpenIM(x11_display, عدم, عدم, عدم);

    אם (xim == عدم)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	אם (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_to_open_input_method));
	    verbose_leave();
	}
	החזר FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_data = عدم;
	אם (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, عدم))
	    emsg(_(e_warning_could_not_set_destroy_callback_to_im));
    }
#   endif

    אם (XGetIMValues(xim, XNQueryInputStyle, &xim_styles, عدم) || !xim_styles)
    {
	emsg(_(e_input_method_doesnt_support_any_style));
	XCloseIM(xim);
	החזר FALSE;
    }

    found = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    حجم(tmp) - 1);
    עבור (s = tmp; s && !found; )
    {
	כלעוד (*s && SAFE_isspace(*s))
	    s++;
	אם (!*s)
	    اكسر;
	אם ((ns = end = strchr(s, ',')) != 0)
	    ns++;
	אחרת
	    end = s + strlen(s);
	כלעוד (SAFE_isspace(*end))
	    end--;
	*end = '\0';

	אם (!strcmp(s, "OverTheSpot"))
	    this_input_style = (XIMPreeditPosition | XIMStatusArea);
	אחרת אם (!strcmp(s, "OffTheSpot"))
	    this_input_style = (XIMPreeditArea | XIMStatusArea);
	אחרת אם (!strcmp(s, "Root"))
	    this_input_style = (XIMPreeditNothing | XIMStatusNothing);

	עבור (i = 0; (unsigned short)i < xim_styles->count_styles; i++)
	{
	    אם (this_input_style == xim_styles->supported_styles[i])
	    {
		found = True;
		اكسر;
	    }
	}
	אם (!found)
	    עבור (i = 0; (unsigned short)i < xim_styles->count_styles; i++)
	    {
		אם ((xim_styles->supported_styles[i] & this_input_style)
			== (this_input_style & ~XIMStatusArea))
		{
		    this_input_style &= ~XIMStatusArea;
		    found = True;
		    اكسر;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    אם (!found)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	אם (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_input_method_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	החזר FALSE;
    }

    over_spot.x = TEXT_X(gui.col);
    over_spot.y = TEXT_Y(gui.row);
    input_style = this_input_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    אם (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				عدم);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				عدم);
    }
    אחרת
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				عدم);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				عدم);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, input_style,
		    XNClientWindow, x11_window,
		    XNFocusWindow, gui.wid,
		    XNPreeditAttributes, preedit_list,
		    XNStatusAttributes, status_list,
		    عدم);
    XFree(status_list);
    XFree(preedit_list);
    אם (xic != عدم)
    {
	אם (input_style & XIMStatusArea)
	{
	    xim_set_status_area();
	    status_area_enabled = TRUE;
	}
	אחרת
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    אחרת
    {
	אם (!is_not_a_term())
	    emsg(_(e_failed_to_create_input_context));
	XCloseIM(xim);
	החזר FALSE;
    }

    החזר TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    صحيح
im_get_status(فراغ)
{
#  ifdef FEAT_EVAL
    אם (USE_IMSTATUSFUNC)
	החזר call_imstatusfunc();
#  endif
    החזר xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# אם !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    فراغ
xim_set_status_area(فراغ)
{
    XVaNestedList preedit_list = 0, status_list = 0, list = 0;
    XRectangle pre_area, status_area;

    אם (xic == عدم)
	החזר;

    אם (input_style & XIMStatusArea)
    {
	אם (input_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    status_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, عدم);
	    XGetICValues(xic, XNStatusAttributes, status_list, عدم);
	    XFree(status_list);

	    status_area.width = needed_rect->width;
	}
	אחרת
	    status_area.width = gui.char_width * Columns;

	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	אם (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	אם (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.height = gui.char_height;
	status_list = XVaCreateNestedList(0, XNArea, &status_area, عدم);
    }
    אחרת
    {
	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	אם (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	אם (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.width = 0;
	status_area.height = gui.char_height;
    }

    אם (input_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = status_area.x + status_area.width;
	pre_area.y = gui.char_height * Rows + gui.border_offset;
	pre_area.width = gui.char_width * Columns - pre_area.x;
	אם (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	אם (gui.menu_is_active)
	    pre_area.y += gui.menu_height;
#  endif
	pre_area.height = gui.char_height;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, عدم);
    }
    אחרת אם (input_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.height = gui.char_height * Rows;
	pre_area.width = gui.char_width * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, عدم);
    }

    אם (preedit_list && status_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   XNStatusAttributes, status_list, عدم);
    אחרת אם (preedit_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   عدم);
    אחרת אם (status_list)
	list = XVaCreateNestedList(0, XNStatusAttributes, status_list,
				   عدم);
    אחרת
	list = عدم;

    אם (list)
    {
	XSetICValues(xic, XNVaNestedList, list, عدم);
	XFree(list);
    }
    אם (status_list)
	XFree(status_list);
    אם (preedit_list)
	XFree(preedit_list);
}

    صحيح
xim_get_status_area_height(فراغ)
{
    אם (status_area_enabled)
	החזר gui.char_height;
    החזר 0;
}
# endif

#אחרת // !defined(FEAT_XIM)

# אם defined(IME_WITHOUT_XIM) || defined(VIMDLL)
ساكن صحيح im_was_set_active = FALSE;

    صحيح
#  ifdef VIMDLL
mbyte_im_get_status(فراغ)
#  אחרת
im_get_status(فراغ)
#  endif
{
#  אם defined(FEAT_EVAL)
    אם (USE_IMSTATUSFUNC)
	החזר call_imstatusfunc();
#  endif
    החזר im_was_set_active;
}

    فراغ
#  ifdef VIMDLL
mbyte_im_set_active(صحيح active_arg)
#  אחרת
im_set_active(صحيح active_arg)
#  endif
{
#  אם defined(FEAT_EVAL)
    صحيح	    active = !p_imdisable && active_arg;

    אם (USE_IMACTIVATEFUNC && active != im_get_status())
    {
	call_imactivatefunc(active);
	im_was_set_active = active;
    }
#  endif
}

#  אם defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    فراغ
im_set_position(صحيح row UNUSED, صحيح col UNUSED)
{
}
#  endif
# endif

#endif // FEAT_XIM
