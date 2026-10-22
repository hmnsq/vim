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

#অন্তর্ভাব "vim.h"

#যদি !defined(GTK_CHECK_VERSION)
# define GTK_CHECK_VERSION(a, b, c) 0
#endif

#যদি defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# অন্যথা
#  include <gdk/gdkkeysyms.h>
# endif
# যদি !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  অন্যথা
#   include <gdk/gdkx.h>
#  endif
# endif
#endif

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#যদি defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
স্থির শূন্য xim_log(অক্ষর *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    স্থির শূন্য
xim_log(অক্ষর *s, ...)
{
    va_list arglist;
    স্থির FILE *fd = ফাঁকা;

    যদি (fd == (FILE *)-1)
	ফেরত;
    যদি (fd == ফাঁকা)
    {
	fd = mch_fopen("xim.log", "w");
	যদি (fd == ফাঁকা)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    ফেরত;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#endif

#যদি defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.in_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.in_use && *p_imsf != NUL)
#অন্যথা
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#endif

#যদি defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
স্থির callback_T imaf_cb;	    // 'imactivatefunc' callback function
স্থির callback_T imsf_cb;	    // 'imstatusfunc' callback function

    অক্ষর *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    যদি (option_set_callback_func(p_imaf, &imaf_cb) == FAIL)
	ফেরত e_invalid_argument;

    ফেরত ফাঁকা;
}

    অক্ষর *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    যদি (option_set_callback_func(p_imsf, &imsf_cb) == FAIL)
	ফেরত e_invalid_argument;

    ফেরত ফাঁকা;
}

    স্থির শূন্য
call_imactivatefunc(পূর্ণসংখ্যা active)
{
    typval_T argv[2];
    পূর্ণসংখ্যা save_চাবি_Typed = চাবি_Typed;

    argv[0].v_type = VAR_সংখ্যা;
    argv[0].vval.v_সংখ্যা = active ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (শূন্য)call_callback_retnr(&imaf_cb, 1, argv);

    চাবি_Typed = save_চাবি_Typed;
}

    স্থির পূর্ণসংখ্যা
call_imstatusfunc(শূন্য)
{
    পূর্ণসংখ্যা is_active;
    পূর্ণসংখ্যা save_চাবি_Typed = চাবি_Typed;

    // FIXME: Don't execute user function in unsafe situation.
    যদি (exiting || is_autocmd_blocked())
	ফেরত FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++বার্তা_silent;
    is_active = call_callback_retnr(&imsf_cb, 0, ফাঁকা);
    --বার্তা_silent;

    চাবি_Typed = save_চাবি_Typed;
    ফেরত (is_active > 0);
}
#endif

#যদি defined(EXITFREE)
    শূন্য
free_xim_stuff(শূন্য)
{
# যদি defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    পূর্ণসংখ্যা
set_ref_in_im_funcs(পূর্ণসংখ্যা copyID UNUSED)
{
    পূর্ণসংখ্যা abort = FALSE;

# যদি defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = set_ref_in_callback(&imaf_cb, copyID);
    abort = abort || set_ref_in_callback(&imsf_cb, copyID);
# endif

    ফেরত abort;
}
#endif


#যদি defined(FEAT_XIM)

# যদি defined(FEAT_GUI_GTK)
স্থির পূর্ণসংখ্যা xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    স্থির শূন্য
init_preedit_start_col(শূন্য)
{
    যদি (State & MODE_CMDLINE)
	preedit_start_col = cmdline_getvcol_cursor();
    অন্যথা যদি (curwin != ফাঁকা && curwin->w_বাফার != ফাঁকা)
	getvcol(curwin, &curwin->w_cursor, &preedit_start_col, ফাঁকা, ফাঁকা, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_changed_while_preediting = curbuf->b_changed;
}

স্থির পূর্ণসংখ্যা im_is_active	       = FALSE;	// IM is enabled for current mode
স্থির পূর্ণসংখ্যা preedit_is_active   = FALSE;
স্থির পূর্ণসংখ্যা im_preedit_cursor   = 0;	// cursor offset in characters
স্থির পূর্ণসংখ্যা im_preedit_trailing = 0;	// number of characters after cursor

স্থির unsigned long im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_চাবি_Void_প্রতীক
#    define GDK_চাবি_Void_প্রতীক GDK_Void_প্রতীক
#   endif
স্থির unsigned পূর্ণসংখ্যা  im_activatekey_keyval = GDK_চাবি_Void_প্রতীক;
স্থির unsigned পূর্ণসংখ্যা  im_activatekey_state  = 0;
#  endif

স্থির GtkWidget *preedit_window = ফাঁকা;
স্থির GtkWidget *preedit_label = ফাঁকা;
#  ifdef USE_GTK4
স্থির পূর্ণসংখ্যা preedit_label_width = 0;	// last measured popover natural width
স্থির পূর্ণসংখ্যা preedit_popover_height = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
স্থির GtkCssProvider *preedit_css_provider = ফাঁকা;
স্থির guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

স্থির শূন্য im_preedit_window_set_position(শূন্য);

    শূন্য
im_set_active(পূর্ণসংখ্যা active)
{
    পূর্ণসংখ্যা was_active;

    was_active = !!im_get_status();
    im_is_active = (active && !p_imdisable);

    যদি (im_is_active != was_active)
	xim_reset();
}

    শূন্য
xim_set_focus(পূর্ণসংখ্যা focus)
{
    যদি (xic == ফাঁকা)
	ফেরত;

    যদি (focus)
	gtk_im_context_focus_in(xic);
    অন্যথা
	gtk_im_context_focus_out(xic);
}

    শূন্য
im_set_position(পূর্ণসংখ্যা row, পূর্ণসংখ্যা col)
{
    যদি (xic == ফাঁকা)
	ফেরত;

    GdkRectangle area;

    area.x = FILL_X(col);
    area.y = FILL_Y(row);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.width  = gui.char_width
		    * ((Screen_সারিগুলি != ফাঁকা && mb_lefthalve(row, col)) ? 2 : 1);
    area.height = gui.char_height;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    যদি (p_imst == IM_OVER_THE_SPOT)
    {
	পূর্ণসংখ্যা popover_h = preedit_popover_height;
	যদি (popover_h <= 0)
	    popover_h = gui.char_height;
	যদি (popover_h > area.height)
	    area.height = popover_h;
    }
#  endif

    gtk_im_context_set_cursor_location(xic, &area);

    যদি (p_imst == IM_OVER_THE_SPOT)
	im_preedit_window_set_position();
}

#  যদি 0 || defined(PROTO) // apparently only used in gui_x11.c
    শূন্য
xim_set_preedit(শূন্য)
{
    im_set_position(gui.row, gui.col);
}
#  endif

    স্থির শূন্য
im_add_to_প্রবেশ(char_u *str, পূর্ণসংখ্যা len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    যদি (প্রবেশ_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&প্রবেশ_conv, str, &len);
	g_return_if_fail(str != ফাঁকা);
    }

    add_to_প্রবেশ_buf_csi(str, len);

    যদি (প্রবেশ_conv.vc_type != CONV_NONE)
	vim_free(str);

    যদি (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    স্থির শূন্য
im_preedit_window_set_position(শূন্য)
{
    যদি (preedit_window == ফাঁকা)
	ফেরত;

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
    যদি (!gtk_widget_compute_point(gui.drawarea, gui.mainwin,
						&pt_in, &pt_out))
    {
	pt_out.x = pt_in.x;
	pt_out.y = pt_in.y;
    }
    rect.x = (পূর্ণসংখ্যা)pt_out.x;
    rect.y = (পূর্ণসংখ্যা)pt_out.y;
    rect.width = preedit_label_width;
    rect.height = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    পূর্ণসংখ্যা offset = gui.char_height - preedit_popover_height;
    gtk_popover_set_offset(GTK_POPOVER(preedit_window), 0, offset);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_set_halign(preedit_window, GTK_ALIGN_START);
    gtk_popover_set_pointing_to(GTK_POPOVER(preedit_window), &rect);
#  অন্যথা
    পূর্ণসংখ্যা x, y, width, height;
    পূর্ণসংখ্যা screen_x, screen_y, screen_width, screen_height;

    gui_gtk_get_screen_geom_of_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_width, &screen_height);
    gdk_window_get_origin(gtk_widget_get_window(gui.drawarea), &x, &y);
    gtk_window_get_আকার(GTK_WINDOW(preedit_window), &width, &height);
    x = x + FILL_X(gui.col);
    y = y + FILL_Y(gui.row);
    যদি (x + width > screen_x + screen_width)
	x = screen_x + screen_width - width;
    যদি (y + height > screen_y + screen_height)
	y = screen_y + screen_height - height;
    gtk_window_move(GTK_WINDOW(preedit_window), x, y);
#  endif
}

    স্থির শূন্য
im_preedit_window_open(শূন্য)
{
    অক্ষর *preedit_string;
#  যদি !GTK_CHECK_VERSION(3,16,0)
    অক্ষর buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  যদি GTK_CHECK_VERSION(3,0,0)
#   যদি !GTK_CHECK_VERSION(3,16,0)
    GdkRGBA color;
#   endif
#  অন্যথা
    GdkColor color;
#  endif
    gint w, h;

    যদি (preedit_window == ফাঁকা)
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
#  অন্যথা
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
#  অন্যথা
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
    যদি (preedit_css_provider == ফাঁকা || preedit_css_cached_bg != gui.back_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	যদি (preedit_css_provider != ফাঁকা)
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
#   যদি GTK_CHECK_VERSION(3,16,0)
    {
	GtkStyleContext * ধ্রুবক context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * ধ্রুবক provider = gtk_css_provider_new();
	gchar		   *css = ফাঁকা;
	ধ্রুবক অক্ষর * ধ্রুবক fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_আকার(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = ফাঁকা;

	যদি (!pango_font_description_get_আকার_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * ধ্রুবক screen
		  = gdk_window_get_screen(gtk_widget_get_window(gui.mainwin));
	    ধ্রুবক gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	যদি (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	অন্যথা
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != ফাঁকা ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.back_pixel >> 16) & 0xff,
		(gui.back_pixel >> 8) & 0xff,
		gui.back_pixel & 0xff);

	gtk_css_provider_load_from_তথ্য(provider, css, -1, ফাঁকা);
	gtk_style_context_add_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_VERSION(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, আকার(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_NORMAL, &color);

    vim_snprintf(buf, আকার(buf), "#%06X", gui.back_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_NORMAL,
								      &color);
#   অন্যথা
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, আকার(buf), "#%06X", (unsigned)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_NORMAL, &color);

    vim_snprintf(buf, আকার(buf), "#%06X", (unsigned)gui.back_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_window, GTK_STATE_NORMAL, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ফাঁকা);

    যদি (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	যদি (attr_list == ফাঁকা)
	    attr_list = pango_attr_list_new();

	যদি (gui.norm_font != ফাঁকা)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->start_সূচক = 0;
	    pa->end_সূচক = G_MAXUINT;
	    pango_attr_list_ঢোকাও(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->start_সূচক = 0;
	pa->end_সূচক = G_MAXUINT;
	pango_attr_list_ঢোকাও(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.back_pixel >> 16) & 0xff) * 257,
		((gui.back_pixel >> 8) & 0xff) * 257,
		(gui.back_pixel & 0xff) * 257);
	pa->start_সূচক = 0;
	pa->end_সূচক = G_MAXUINT;
	pango_attr_list_ঢোকাও(attr_list, pa);
#  endif
	gtk_label_set_text(GTK_LABEL(preedit_label), preedit_string);
	gtk_label_set_attributes(GTK_LABEL(preedit_label), attr_list);

	layout = gtk_label_get_layout(GTK_LABEL(preedit_label));
	pango_layout_get_pixel_আকার(layout, &w, &h);
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
	যদি (xic != ফাঁকা)
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
	যদি (gtk_widget_get_mapped(preedit_window))
	    gtk_popover_popdown(GTK_POPOVER(preedit_window));

	im_preedit_window_set_position();

	gtk_widget_queue_resize(preedit_window);
	gtk_popover_popup(GTK_POPOVER(preedit_window));
#  অন্যথা
	gtk_window_resize(GTK_WINDOW(preedit_window), w, h);
	gtk_widget_show_all(preedit_window);

	im_preedit_window_set_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    স্থির শূন্য
im_preedit_window_close(শূন্য)
{
    যদি (preedit_window == ফাঁকা)
	ফেরত;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_window));
#  অন্যথা
    gtk_widget_hide(preedit_window);
#  endif
}

    স্থির শূন্য
im_show_preedit(শূন্য)
{
    im_preedit_window_open();

    যদি (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    স্থির শূন্য
im_delete_preedit(শূন্য)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    যদি (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_window_close();
	ফেরত;
    }

    যদি (State & MODE_NORMAL
#  ifdef FEAT_TERMINAL
	    && !term_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	ফেরত;
    }
    জন্য (; im_preedit_cursor > 0; --im_preedit_cursor)
	add_to_প্রবেশ_buf(bskey, (পূর্ণসংখ্যা)আকার(bskey));

    জন্য (; im_preedit_trailing > 0; --im_preedit_trailing)
	add_to_প্রবেশ_buf(delkey, (পূর্ণসংখ্যা)আকার(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    স্থির শূন্য
im_correct_cursor(পূর্ণসংখ্যা অঙ্ক_move_back)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    যদি (State & MODE_NORMAL)
	ফেরত;
#  ifdef FEAT_RIGHTLEFT
    যদি ((State & MODE_CMDLINE) == 0 && curwin != ফাঁকা && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    জন্য (; অঙ্ক_move_back > 0; --অঙ্ক_move_back)
	add_to_প্রবেশ_buf(backkey, (পূর্ণসংখ্যা)আকার(backkey));
}

স্থির পূর্ণসংখ্যা xim_expected_char = NUL;
স্থির পূর্ণসংখ্যা xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    স্থির শূন্য
im_show_info(শূন্য)
{
    পূর্ণসংখ্যা	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    যদি ((State & MODE_NORMAL) || (State & MODE_ঢোকাও))
	setcursor();
    out_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    স্থির শূন্য
im_commit_cb(GtkIMContext *context UNUSED,
	     ধ্রুবক gchar *str,
	     gpointer তথ্য UNUSED)
{
    পূর্ণসংখ্যা		slen = (পূর্ণসংখ্যা)STRLEN(str);
    পূর্ণসংখ্যা		add_to_প্রবেশ = TRUE;
    পূর্ণসংখ্যা		clen;
    পূর্ণসংখ্যা		len = slen;
    পূর্ণসংখ্যা		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    যদি (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	যদি (preedit_start_col == MAXCOL)
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
	যদি (প্রবেশ_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&প্রবেশ_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != ফাঁকা);
	}
	অন্যথা
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	যদি (প্রবেশ_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_start_col += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    যদি (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	যদি (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    add_to_প্রবেশ = FALSE;
	}
	অন্যথা
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    যদি (add_to_প্রবেশ)
	im_add_to_প্রবেশ((char_u *)str, slen);

    যদি (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	যদি (!commit_with_preedit)
	    preedit_start_col = MAXCOL;

	// This flag is used in changed() at next call.
	xim_changed_while_preediting = TRUE;
    }

    যদি (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    স্থির শূন্য
im_preedit_start_cb(GtkIMContext *context UNUSED, gpointer তথ্য UNUSED)
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
    স্থির শূন্য
im_preedit_end_cb(GtkIMContext *context UNUSED, gpointer তথ্য UNUSED)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    যদি (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;

#  যদি 0
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
    স্থির শূন্য
im_preedit_changed_cb(GtkIMContext *context, gpointer তথ্য UNUSED)
{
    অক্ষর    *preedit_string = ফাঁকা;
    পূর্ণসংখ্যা	    cursor_সূচক    = 0;
    পূর্ণসংখ্যা	    অঙ্ক_move_back   = 0;
    char_u  *str;
    char_u  *p;
    পূর্ণসংখ্যা	    i;

    যদি (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ফাঁকা,
					  &cursor_সূচক);
    অন্যথা
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ফাঁকা,
					  ফাঁকা);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != ফাঁকা); // just in case

    যদি (p_imst == IM_OVER_THE_SPOT)
    {
	যদি (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	অন্যথা
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    অন্যথা
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	যদি (preedit_start_col == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_start_col();
	}
	অন্যথা যদি (cursor_সূচক == 0 && preedit_string[0] == '\0')
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
	যদি (preedit_start_col != MAXCOL)
	    preedit_end_col = preedit_start_col;
	str = (char_u *)preedit_string;
	জন্য (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    পূর্ণসংখ্যা is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    যদি (!is_composing || p_deco)
	    {
		যদি (i < cursor_সূচক)
		    ++im_preedit_cursor;
		অন্যথা
		    ++im_preedit_trailing;
	    }
	    যদি (!is_composing && i >= cursor_সূচক)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++অঙ্ক_move_back;
	    }
	    যদি (preedit_start_col != MAXCOL)
		preedit_end_col += utf_ptr2cells(p);
	}

	যদি (p > str)
	{
	    im_add_to_প্রবেশ(str, (পূর্ণসংখ্যা)(p - str));
	    im_correct_cursor(অঙ্ক_move_back);
	}
    }

    g_free(preedit_string);

    যদি (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    স্থির পূর্ণসংখ্যা
translate_pango_attributes(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    পূর্ণসংখ্যা		    char_attr = HL_NORMAL;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    যদি (attr != ফাঁকা && ((PangoAttrInt *)attr)->মান
						 != (পূর্ণসংখ্যা)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_WEIGHT);
    যদি (attr != ফাঁকা && ((PangoAttrInt *)attr)->মান >= (পূর্ণসংখ্যা)PANGO_WEIGHT_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    যদি (attr != ফাঁকা && ((PangoAttrInt *)attr)->মান
						   != (পূর্ণসংখ্যা)PANGO_STYLE_NORMAL)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    যদি (attr != ফাঁকা)
    {
	ধ্রুবক PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	যদি ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    ফেরত char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    পূর্ণসংখ্যা
im_get_feedback_attr(পূর্ণসংখ্যা col)
{
    অক্ষর	    *preedit_string = ফাঁকা;
    PangoAttrList   *attr_list	    = ফাঁকা;
    পূর্ণসংখ্যা		    char_attr	    = -1;

    যদি (xic == ফাঁকা)
	ফেরত char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ফাঁকা);

    যদি (preedit_string != ফাঁকা && attr_list != ফাঁকা)
    {
	পূর্ণসংখ্যা idx;

	// Get the byte index as used by PangoAttrIterator
	জন্য (idx = 0; col > 0 && preedit_string[idx] != '\0'; --col)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	যদি (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    পূর্ণসংখ্যা			start, end;

	    char_attr = HL_NORMAL;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    করো
	    {
		pango_attr_iterator_range(iter, &start, &end);

		যদি (idx >= start && idx < end)
		    char_attr |= translate_pango_attributes(iter);
	    }
	    যখন (pango_attr_iterator_পরবর্তী(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    যদি (attr_list != ফাঁকা)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    ফেরত char_attr;
}

    শূন্য
xim_init(শূন্য)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != ফাঁকা);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_window(gui.drawarea) != ফাঁকা);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), ফাঁকা);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_changed_cb), ফাঁকা);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_start_cb), ফাঁকা);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_end_cb), ফাঁকা);

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
#  অন্যথা
    gtk_im_context_set_client_window(xic, gtk_widget_get_window(gui.drawarea));
#  endif
}

    শূন্য
im_shutdown(শূন্য)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    যদি (xic != ফাঁকা)
    {
	gtk_im_context_focus_out(xic);
	g_object_unref(xic);
	xic = ফাঁকা;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    যদি (preedit_window != ফাঁকা)
    {
	gtk_widget_unparent(preedit_window);
	preedit_window = ফাঁকা;
	preedit_label = ফাঁকা;
    }
    যদি (preedit_css_provider != ফাঁকা)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = ফাঁকা;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_active = FALSE;
    im_commit_handler_id = 0;
    যদি (p_imst == IM_ON_THE_SPOT)
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
    স্থির পূর্ণসংখ্যা
im_string_to_keyval(ধ্রুবক অক্ষর *str, unsigned পূর্ণসংখ্যা *keyval, unsigned পূর্ণসংখ্যা *state)
{
    ধ্রুবক অক্ষর	    *mods_end;
    unsigned	    tmp_keyval;
    unsigned	    tmp_state = 0;

    mods_end = strrchr(str, '-');
    mods_end = (mods_end != ফাঁকা) ? mods_end + 1 : str;

    // Parse modifier keys
    যখন (str < mods_end)
	নির্বাচন (*str++)
	{
	    ক্ষেত্র '-':							বিরতি;
	    ক্ষেত্র 'S': ক্ষেত্র 's': tmp_state |= (unsigned)GDK_SHIFT_MASK;	বিরতি;
	    ক্ষেত্র 'L': ক্ষেত্র 'l': tmp_state |= (unsigned)GDK_LOCK_MASK;	বিরতি;
	    ক্ষেত্র 'C': ক্ষেত্র 'c': tmp_state |= (unsigned)GDK_CONTROL_MASK;বিরতি;
	    ক্ষেত্র '1':		tmp_state |= (unsigned)GDK_MOD1_MASK;	বিরতি;
	    ক্ষেত্র '2':		tmp_state |= (unsigned)GDK_MOD2_MASK;	বিরতি;
	    ক্ষেত্র '3':		tmp_state |= (unsigned)GDK_MOD3_MASK;	বিরতি;
	    ক্ষেত্র '4':		tmp_state |= (unsigned)GDK_MOD4_MASK;	বিরতি;
	    ক্ষেত্র '5':		tmp_state |= (unsigned)GDK_MOD5_MASK;	বিরতি;
	    পূর্বনির্ধারিত:
		ফেরত FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    যদি (tmp_keyval == 0 || tmp_keyval == GDK_Void_প্রতীক)
	ফেরত FALSE;

    যদি (keyval != ফাঁকা)
	*keyval = tmp_keyval;
    যদি (state != ফাঁকা)
	*state = tmp_state;

    ফেরত TRUE;
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
    পূর্ণসংখ্যা
im_xim_isvalid_imactivate(শূন্য)
{
    যদি (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_Void_প্রতীক;
	im_activatekey_state  = 0;
	ফেরত TRUE;
    }

    ফেরত im_string_to_keyval((ধ্রুবক অক্ষর *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    স্থির শূন্য
im_synthesize_keypress(unsigned পূর্ণসংখ্যা keyval, unsigned পূর্ণসংখ্যা state)
{
    Gdk_Event_চাবি *event;

    event = (Gdk_Event_চাবি *)gdk_event_new(GDK_চাবি_PRESS);
    g_object_ref(gtk_widget_get_window(gui.drawarea));
					// unreffed by gdk_event_free()
    event->window = gtk_widget_get_window(gui.drawarea);
    event->send_event = TRUE;
    event->time = GDK_CURRENT_TIME;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysymToKeycode(GDK_WINDOW_XDISPLAY(event->window), (চাবি_Sym)keyval);
    event->দৈর্ঘ্য = 0;
    event->string = ফাঁকা;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_চাবি_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    শূন্য
xim_reset(শূন্য)
{
#  ifdef FEAT_EVAL
    যদি (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_active);
    অন্যথা
#  endif
    যদি (xic != ফাঁকা)
    {
	gtk_im_context_reset(xic);

	যদি (p_imdisable)
	    im_shutdown();
	অন্যথা
	{
	    xim_set_focus(gui.in_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_set_focus(gui.in_focus);
#  অন্যথা
	    যদি (im_activatekey_keyval != GDK_Void_প্রতীক)
	    {
		যদি (im_is_active)
		{
		    g_signal_handler_block(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    অন্যথা
	    {
		im_shutdown();
		xim_init();
		xim_set_focus(gui.in_focus);
	    }
#  endif
	}
    }

    যদি (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    পূর্ণসংখ্যা
xim_queue_চাবি_press_event(Gdk_Event_চাবি *event, পূর্ণসংখ্যা down)
{
#   ifdef FEAT_GUI_GTK
    যদি (event->state & GDK_SUPER_MASK) ফেরত FALSE;
#   endif
    যদি (down)
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
	নির্বাচন (event->keyval)
	{
	    ক্ষেত্র GDK_KP_Add:      xim_expected_char = '+';  বিরতি;
	    ক্ষেত্র GDK_KP_Subtract: xim_expected_char = '-';  বিরতি;
	    ক্ষেত্র GDK_KP_Divide:   xim_expected_char = '/';  বিরতি;
	    ক্ষেত্র GDK_KP_Multiply: xim_expected_char = '*';  বিরতি;
	    ক্ষেত্র GDK_KP_Decimal:  xim_expected_char = '.';  বিরতি;
	    ক্ষেত্র GDK_KP_Equal:    xim_expected_char = '=';  বিরতি;
	    ক্ষেত্র GDK_KP_0:	  xim_expected_char = '0';  বিরতি;
	    ক্ষেত্র GDK_KP_1:	  xim_expected_char = '1';  বিরতি;
	    ক্ষেত্র GDK_KP_2:	  xim_expected_char = '2';  বিরতি;
	    ক্ষেত্র GDK_KP_3:	  xim_expected_char = '3';  বিরতি;
	    ক্ষেত্র GDK_KP_4:	  xim_expected_char = '4';  বিরতি;
	    ক্ষেত্র GDK_KP_5:	  xim_expected_char = '5';  বিরতি;
	    ক্ষেত্র GDK_KP_6:	  xim_expected_char = '6';  বিরতি;
	    ক্ষেত্র GDK_KP_7:	  xim_expected_char = '7';  বিরতি;
	    ক্ষেত্র GDK_KP_8:	  xim_expected_char = '8';  বিরতি;
	    ক্ষেত্র GDK_KP_9:	  xim_expected_char = '9';  বিরতি;
	    ক্ষেত্র GDK_space:	  xim_expected_char = ' ';  বিরতি;
	    পূর্বনির্ধারিত:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    যদি (xic != ফাঁকা && !p_imdisable
		    && (State & (MODE_ঢোকাও | MODE_CMDLINE
					      | MODE_NORMAL | MODE_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	যদি (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    unsigned পূর্ণসংখ্যা state_mask;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_mask  = im_activatekey_state;
	    state_mask |= ((পূর্ণসংখ্যা)GDK_SHIFT_MASK | (পূর্ণসংখ্যা)GDK_CONTROL_MASK
							| (পূর্ণসংখ্যা)GDK_MOD1_MASK);

	    যদি ((event->state & state_mask) != im_activatekey_state)
		ফেরত FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    যদি (event->type != GDK_চাবি_PRESS)
		ফেরত TRUE;

	    যদি (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
	    {
		im_set_active(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= MODE_LANGMAP;
		যদি (State & MODE_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~MODE_LANGMAP;
		}
		অন্যথা
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= MODE_LANGMAP;
		}
		ফেরত TRUE;
	    }

	    ফেরত gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	যদি (im_activatekey_keyval == GDK_Void_প্রতীক || im_is_active)
	{
	    পূর্ণসংখ্যা imresult = gtk_im_context_filter_keypress(xic, event);

	    যদি (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		যদি (!imresult && preedit_start_col != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    ফেরত FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    যদি (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		ফেরত FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    ফেরত imresult;
	}
    }

    ফেরত FALSE;
}
#  অন্যথা // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    পূর্ণসংখ্যা
im_xim_isvalid_imactivate(শূন্য)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    ফেরত p_imak[0] == NUL;
}

    পূর্ণসংখ্যা
xim_queue_চাবি_press_event(GdkEvent *event UNUSED, পূর্ণসংখ্যা down UNUSED)
{
    ফেরত FALSE;
}
#  endif // !USE_GTK4

    পূর্ণসংখ্যা
im_get_status(শূন্য)
{
#  ifdef FEAT_EVAL
    যদি (USE_IMSTATUSFUNC)
	ফেরত call_imstatusfunc();
#  endif
    ফেরত im_is_active;
}

    পূর্ণসংখ্যা
preedit_get_status(শূন্য)
{
    ফেরত preedit_is_active;
}

    পূর্ণসংখ্যা
im_is_preediting(শূন্য)
{
    ফেরত xim_has_preediting;
}

# অন্যথা // !FEAT_GUI_GTK

স্থির পূর্ণসংখ্যা	xim_is_active = FALSE;  // XIM should be active in the current
					// mode
স্থির পূর্ণসংখ্যা	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
স্থির XIMStyle	প্রবেশ_style;
স্থির পূর্ণসংখ্যা	status_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    শূন্য
im_set_active(পূর্ণসংখ্যা active_arg)
{
    পূর্ণসংখ্যা active = active_arg;

    // If 'imdisable' is set, XIM is never active.
    যদি (p_imdisable)
	active = FALSE;
    অন্যথা যদি (প্রবেশ_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	active = TRUE;

#  যদি defined(FEAT_EVAL)
    যদি (USE_IMACTIVATEFUNC)
    {
	যদি (active != im_get_status())
	{
	    call_imactivatefunc(active);
	    xim_has_focus = active;
	}
	ফেরত;
    }
#  endif

    যদি (xic == ফাঁকা)
	ফেরত;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_active = active;
    xim_set_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    শূন্য
xim_set_focus(পূর্ণসংখ্যা focus)
{
    যদি (xic == ফাঁকা)
	ফেরত;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    যদি (focus && xim_is_active)
    {
	যদি (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    অন্যথা
    {
	যদি (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    শূন্য
im_set_position(পূর্ণসংখ্যা row UNUSED, পূর্ণসংখ্যা col UNUSED)
{
    xim_set_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    শূন্য
xim_set_preedit(শূন্য)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    পূর্ণসংখ্যা সারি_space;

    যদি (xic == ফাঁকা)
	ফেরত;

    xim_set_focus(TRUE);

    যদি (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							ফাঁকা);
	XSetICValues(xic, XNPreeditAttributes, attr_list, ফাঁকা);
	XFree(attr_list);
	ফেরত;
    }

    যদি (প্রবেশ_style & XIMPreeditPosition)
    {
	যদি (xim_fg_color == INVALCOLOR)
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
	সারি_space = gui.char_height;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, সারি_space,
					ফাঁকা);
	যদি (XSetICValues(xic, XNPreeditAttributes, attr_list, ফাঁকা))
	    emsg(_(e_cannot_set_ic_মানসমূহ));
	XFree(attr_list);
    }
}

#  যদি defined(FEAT_GUI_X11)
#   যদি defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

স্থির পূর্ণসংখ্যা xim_real_init(Window x11_window, Display *x11_display);


#   ifdef USE_X11R6_XIM
    স্থির শূন্য
xim_instantiate_cb(
    Display	*display,
    XPointer	client_তথ্য_2 UNUSED,
    XPointer	call_তথ্য UNUSED)
{
    Window	x11_window;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_instantiate_cb()\n");
#    endif

    gui_get_x11_windis(&x11_window, &x11_display);
    যদি (display != x11_display)
	ফেরত;

    xim_real_init(x11_window, x11_display);
    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    যদি (xic != ফাঁকা)
	XUnregisterIMInstantiateCallback(x11_display, ফাঁকা, ফাঁকা, ফাঁকা,
					 xim_instantiate_cb, ফাঁকা);
}

    স্থির শূন্য
xim_destroy_cb(
    XIM		im UNUSED,
    XPointer	client_তথ্য_2 UNUSED,
    XPointer	call_তথ্য UNUSED)
{
    Window	x11_window;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_destroy_cb()\n");
#    endif
    gui_get_x11_windis(&x11_window, &x11_display);

    xic = ফাঁকা;
    status_area_enabled = FALSE;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, ফাঁকা, ফাঁকা, ফাঁকা,
				   xim_instantiate_cb, ফাঁকা);
}
#   endif

    শূন্য
xim_init(শূন্য)
{
    Window	x11_window;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_window, &x11_display);

    xic = ফাঁকা;

    যদি (xim_real_init(x11_window, x11_display))
	ফেরত;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, ফাঁকা, ফাঁকা, ফাঁকা,
				   xim_instantiate_cb, ফাঁকা);
#   endif
}

    স্থির পূর্ণসংখ্যা
xim_real_init(Window x11_window, Display *x11_display)
{
    পূর্ণসংখ্যা		i;
    অক্ষর	*p,
		*s,
		*ns,
		*end,
		tmp[1024];
#   define IMLEN_MAX 40
    অক্ষর	buf[IMLEN_MAX + 7];
    XIM		xim = ফাঁকা;
    XIMStyles	*xim_styles;
    XIMStyle	this_প্রবেশ_style = 0;
    Boolean	found;
    XPoint	over_spot;
    XVaNestedList preedit_list, status_list;

    প্রবেশ_style = 0;
    status_area_enabled = FALSE;

    যদি (xic != ফাঁকা)
	ফেরত FALSE;

    যদি (gui.rsrc_প্রবেশ_method != ফাঁকা && *gui.rsrc_প্রবেশ_method != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_প্রবেশ_method,
	    আকার(tmp) - 1);
	জন্য (ns = s = tmp; ns != ফাঁকা && *s != NUL;)
	{
	    s = (অক্ষর *)skipwhite((char_u *)s);
	    যদি (*s == NUL)
		বিরতি;
	    যদি ((ns = end = strchr(s, ',')) == ফাঁকা)
		end = s + strlen(s);
	    যখন (SAFE_isspace(end[-1]))
		end--;
	    *end = NUL;

	    যদি (strlen(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		যদি ((p = XSetLocaleModifiers(buf)) != ফাঁকা && *p != NUL
			&& (xim = XOpenIM(x11_display, ফাঁকা, ফাঁকা, ফাঁকা))
								      != ফাঁকা)
		    বিরতি;
	    }

	    s = ns + 1;
	}
    }

    যদি (xim == ফাঁকা && (p = XSetLocaleModifiers("")) != ফাঁকা && *p != NUL)
	xim = XOpenIM(x11_display, ফাঁকা, ফাঁকা, ফাঁকা);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    যদি (xim == ফাঁকা && (p = XSetLocaleModifiers("@im=none")) != ফাঁকা
								 && *p != NUL)
	xim = XOpenIM(x11_display, ফাঁকা, ফাঁকা, ফাঁকা);

    যদি (xim == ফাঁকা)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	যদি (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_to_open_প্রবেশ_method));
	    verbose_leave();
	}
	ফেরত FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_তথ্য_2 = ফাঁকা;
	যদি (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, ফাঁকা))
	    emsg(_(e_warning_could_not_set_destroy_callback_to_im));
    }
#   endif

    যদি (XGetIMValues(xim, XNQuery_প্রবেশ_Style, &xim_styles, ফাঁকা) || !xim_styles)
    {
	emsg(_(e_প্রবেশ_method_doesnt_support_any_style));
	XCloseIM(xim);
	ফেরত FALSE;
    }

    found = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    আকার(tmp) - 1);
    জন্য (s = tmp; s && !found; )
    {
	যখন (*s && SAFE_isspace(*s))
	    s++;
	যদি (!*s)
	    বিরতি;
	যদি ((ns = end = strchr(s, ',')) != 0)
	    ns++;
	অন্যথা
	    end = s + strlen(s);
	যখন (SAFE_isspace(*end))
	    end--;
	*end = '\0';

	যদি (!strcmp(s, "OverTheSpot"))
	    this_প্রবেশ_style = (XIMPreeditPosition | XIMStatusArea);
	অন্যথা যদি (!strcmp(s, "OffTheSpot"))
	    this_প্রবেশ_style = (XIMPreeditArea | XIMStatusArea);
	অন্যথা যদি (!strcmp(s, "Root"))
	    this_প্রবেশ_style = (XIMPreeditNothing | XIMStatusNothing);

	জন্য (i = 0; (unsigned short)i < xim_styles->গুনতি_styles; i++)
	{
	    যদি (this_প্রবেশ_style == xim_styles->supported_styles[i])
	    {
		found = True;
		বিরতি;
	    }
	}
	যদি (!found)
	    জন্য (i = 0; (unsigned short)i < xim_styles->গুনতি_styles; i++)
	    {
		যদি ((xim_styles->supported_styles[i] & this_প্রবেশ_style)
			== (this_প্রবেশ_style & ~XIMStatusArea))
		{
		    this_প্রবেশ_style &= ~XIMStatusArea;
		    found = True;
		    বিরতি;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    যদি (!found)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	যদি (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_প্রবেশ_method_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	ফেরত FALSE;
    }

    over_spot.x = TEXT_X(gui.col);
    over_spot.y = TEXT_Y(gui.row);
    প্রবেশ_style = this_প্রবেশ_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    যদি (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				ফাঁকা);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				ফাঁকা);
    }
    অন্যথা
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				ফাঁকা);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				ফাঁকা);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, প্রবেশ_style,
		    XNClientWindow, x11_window,
		    XNFocusWindow, gui.wid,
		    XNPreeditAttributes, preedit_list,
		    XNStatusAttributes, status_list,
		    ফাঁকা);
    XFree(status_list);
    XFree(preedit_list);
    যদি (xic != ফাঁকা)
    {
	যদি (প্রবেশ_style & XIMStatusArea)
	{
	    xim_set_status_area();
	    status_area_enabled = TRUE;
	}
	অন্যথা
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    অন্যথা
    {
	যদি (!is_not_a_term())
	    emsg(_(e_failed_to_create_প্রবেশ_context));
	XCloseIM(xim);
	ফেরত FALSE;
    }

    ফেরত TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    পূর্ণসংখ্যা
im_get_status(শূন্য)
{
#  ifdef FEAT_EVAL
    যদি (USE_IMSTATUSFUNC)
	ফেরত call_imstatusfunc();
#  endif
    ফেরত xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# যদি !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    শূন্য
xim_set_status_area(শূন্য)
{
    XVaNestedList preedit_list = 0, status_list = 0, list = 0;
    XRectangle pre_area, status_area;

    যদি (xic == ফাঁকা)
	ফেরত;

    যদি (প্রবেশ_style & XIMStatusArea)
    {
	যদি (প্রবেশ_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    status_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, ফাঁকা);
	    XGetICValues(xic, XNStatusAttributes, status_list, ফাঁকা);
	    XFree(status_list);

	    status_area.width = needed_rect->width;
	}
	অন্যথা
	    status_area.width = gui.char_width * Columns;

	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	যদি (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	যদি (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.height = gui.char_height;
	status_list = XVaCreateNestedList(0, XNArea, &status_area, ফাঁকা);
    }
    অন্যথা
    {
	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	যদি (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	যদি (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.width = 0;
	status_area.height = gui.char_height;
    }

    যদি (প্রবেশ_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = status_area.x + status_area.width;
	pre_area.y = gui.char_height * Rows + gui.border_offset;
	pre_area.width = gui.char_width * Columns - pre_area.x;
	যদি (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	যদি (gui.menu_is_active)
	    pre_area.y += gui.menu_height;
#  endif
	pre_area.height = gui.char_height;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ফাঁকা);
    }
    অন্যথা যদি (প্রবেশ_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.height = gui.char_height * Rows;
	pre_area.width = gui.char_width * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ফাঁকা);
    }

    যদি (preedit_list && status_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   XNStatusAttributes, status_list, ফাঁকা);
    অন্যথা যদি (preedit_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   ফাঁকা);
    অন্যথা যদি (status_list)
	list = XVaCreateNestedList(0, XNStatusAttributes, status_list,
				   ফাঁকা);
    অন্যথা
	list = ফাঁকা;

    যদি (list)
    {
	XSetICValues(xic, XNVaNestedList, list, ফাঁকা);
	XFree(list);
    }
    যদি (status_list)
	XFree(status_list);
    যদি (preedit_list)
	XFree(preedit_list);
}

    পূর্ণসংখ্যা
xim_get_status_area_height(শূন্য)
{
    যদি (status_area_enabled)
	ফেরত gui.char_height;
    ফেরত 0;
}
# endif

#অন্যথা // !defined(FEAT_XIM)

# যদি defined(IME_WITHOUT_XIM) || defined(VIMDLL)
স্থির পূর্ণসংখ্যা im_was_set_active = FALSE;

    পূর্ণসংখ্যা
#  ifdef VIMDLL
mbyte_im_get_status(শূন্য)
#  অন্যথা
im_get_status(শূন্য)
#  endif
{
#  যদি defined(FEAT_EVAL)
    যদি (USE_IMSTATUSFUNC)
	ফেরত call_imstatusfunc();
#  endif
    ফেরত im_was_set_active;
}

    শূন্য
#  ifdef VIMDLL
mbyte_im_set_active(পূর্ণসংখ্যা active_arg)
#  অন্যথা
im_set_active(পূর্ণসংখ্যা active_arg)
#  endif
{
#  যদি defined(FEAT_EVAL)
    পূর্ণসংখ্যা	    active = !p_imdisable && active_arg;

    যদি (USE_IMACTIVATEFUNC && active != im_get_status())
    {
	call_imactivatefunc(active);
	im_was_set_active = active;
    }
#  endif
}

#  যদি defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    শূন্য
im_set_position(পূর্ণসংখ্যা row UNUSED, পূর্ণসংখ্যা col UNUSED)
{
}
#  endif
# endif

#endif // FEAT_XIM
