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

#شامل "vim.h"

#اگر !defined(GTK_CHECK_نسخہ)
# define GTK_CHECK_نسخہ(a, b, c) 0
#اگر_ختم

#اگر defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_نسخہ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# ورنہ
#  include <gdk/gdkkeysyms.h>
# endif
# اگر !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  ورنہ
#   include <gdk/gdkx.h>
#  endif
# endif
#اگر_ختم

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#اگر defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
ساکن خالی xim_log(حرف *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    ساکن خالی
xim_log(حرف *s, ...)
{
    va_list arglist;
    ساکن FILE *fd = تہی;

    اگر (fd == (FILE *)-1)
	واپس;
    اگر (fd == تہی)
    {
	fd = mch_fopen("xim.log", "w");
	اگر (fd == تہی)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    واپس;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#اگر_ختم

#اگر defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.اندر_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.اندر_use && *p_imsf != NUL)
#ورنہ
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#اگر_ختم

#اگر defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
ساکن callback_T imaf_cb;	    // 'imactivatefunc' callback function
ساکن callback_T imsf_cb;	    // 'imstatusfunc' callback function

    حرف *
did_سیٹ_imactivatefunc(optset_T *دلائل غیر_مستعمل)
{
    اگر (option_سیٹ_callback_func(p_imaf, &imaf_cb) == FAIL)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    حرف *
did_سیٹ_imstatusfunc(optset_T *دلائل غیر_مستعمل)
{
    اگر (option_سیٹ_callback_func(p_imsf, &imsf_cb) == FAIL)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    ساکن خالی
call_imactivatefunc(صحیح فعال)
{
    typval_T argv[2];
    صحیح save_کلید_Typed = کلید_Typed;

    argv[0].v_type = VAR_نمبر;
    argv[0].vval.v_نمبر = فعال ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (خالی)call_callback_retnr(&imaf_cb, 1, argv);

    کلید_Typed = save_کلید_Typed;
}

    ساکن صحیح
call_imstatusfunc(خالی)
{
    صحیح is_فعال;
    صحیح save_کلید_Typed = کلید_Typed;

    // FIXME: Don't execute user function in unsafe situation.
    اگر (exiting || is_autocmd_blocked())
	واپس FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++پیغام_silent;
    is_فعال = call_callback_retnr(&imsf_cb, 0, تہی);
    --پیغام_silent;

    کلید_Typed = save_کلید_Typed;
    واپس (is_فعال > 0);
}
#اگر_ختم

#اگر defined(EXITFREE)
    خالی
free_xim_stuff(خالی)
{
# اگر defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    صحیح
سیٹ_ref_اندر_im_funcs(صحیح copyID غیر_مستعمل)
{
    صحیح abort = FALSE;

# اگر defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = سیٹ_ref_اندر_callback(&imaf_cb, copyID);
    abort = abort || سیٹ_ref_اندر_callback(&imsf_cb, copyID);
# endif

    واپس abort;
}
#اگر_ختم


#اگر defined(FEAT_XIM)

# اگر defined(FEAT_GUI_GTK)
ساکن صحیح xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    ساکن خالی
init_preedit_آغاز_ستون(خالی)
{
    اگر (State & منوال_CMDLINE)
	preedit_آغاز_ستون = cmdline_getvcol_cursor();
    ورنہ اگر (curwin != تہی && curwin->w_بفر != تہی)
	getvcol(curwin, &curwin->w_cursor, &preedit_آغاز_ستون, تہی, تہی, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_بدلا_while_preediting = curbuf->b_بدلا;
}

ساکن صحیح im_is_فعال	       = FALSE;	// IM is enabled for current mode
ساکن صحیح preedit_is_فعال   = FALSE;
ساکن صحیح im_preedit_cursor   = 0;	// cursor offset in characters
ساکن صحیح im_preedit_trailing = 0;	// number of characters after cursor

ساکن غیرعلامتی طویل im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_کلید_Void_علامت
#    define GDK_کلید_Void_علامت GDK_Void_علامت
#   endif
ساکن غیرعلامتی صحیح  im_activatekey_keyval = GDK_کلید_Void_علامت;
ساکن غیرعلامتی صحیح  im_activatekey_state  = 0;
#  endif

ساکن GtkWidget *preedit_کھڑکی = تہی;
ساکن GtkWidget *preedit_label = تہی;
#  ifdef USE_GTK4
ساکن صحیح preedit_label_چوڑائی = 0;	// last measured popover natural width
ساکن صحیح preedit_popover_اونچائی = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
ساکن GtkCssProvider *preedit_css_provider = تہی;
ساکن guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

ساکن خالی im_preedit_کھڑکی_سیٹ_position(خالی);

    خالی
im_سیٹ_فعال(صحیح فعال)
{
    صحیح was_فعال;

    was_فعال = !!im_get_حالت();
    im_is_فعال = (فعال && !p_imdisable);

    اگر (im_is_فعال != was_فعال)
	xim_reset();
}

    خالی
xim_سیٹ_focus(صحیح focus)
{
    اگر (xic == تہی)
	واپس;

    اگر (focus)
	gtk_im_context_focus_اندر(xic);
    ورنہ
	gtk_im_context_focus_برآمد(xic);
}

    خالی
im_سیٹ_position(صحیح صف, صحیح ستون)
{
    اگر (xic == تہی)
	واپس;

    GdkRectangle area;

    area.x = FILL_X(ستون);
    area.y = FILL_Y(صف);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.چوڑائی  = gui.char_چوڑائی
		    * ((Screen_سطریں != تہی && mb_lefthalve(صف, ستون)) ? 2 : 1);
    area.اونچائی = gui.char_اونچائی;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    اگر (p_imst == IM_OVER_THE_SPOT)
    {
	صحیح popover_h = preedit_popover_اونچائی;
	اگر (popover_h <= 0)
	    popover_h = gui.char_اونچائی;
	اگر (popover_h > area.اونچائی)
	    area.اونچائی = popover_h;
    }
#  endif

    gtk_im_context_سیٹ_cursor_location(xic, &area);

    اگر (p_imst == IM_OVER_THE_SPOT)
	im_preedit_کھڑکی_سیٹ_position();
}

#  اگر 0 || defined(PROTO) // apparently only used in gui_x11.c
    خالی
xim_سیٹ_preedit(خالی)
{
    im_سیٹ_position(gui.صف, gui.ستون);
}
#  endif

    ساکن خالی
im_جوڑو_مقصد_داخلہ(char_u *str, صحیح len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    اگر (داخلہ_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&داخلہ_conv, str, &len);
	g_return_if_fail(str != تہی);
    }

    جوڑو_مقصد_داخلہ_buf_csi(str, len);

    اگر (داخلہ_conv.vc_type != CONV_NONE)
	vim_free(str);

    اگر (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ساکن خالی
im_preedit_کھڑکی_سیٹ_position(خالی)
{
    اگر (preedit_کھڑکی == تہی)
	واپس;

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
    graphene_نقطہ_t pt_اندر, pt_برآمد;

    pt_اندر.x = FILL_X(gui.ستون);
    pt_اندر.y = FILL_Y(gui.صف);
    اگر (!gtk_widget_compute_نقطہ(gui.drawarea, gui.mainwin,
						&pt_اندر, &pt_برآمد))
    {
	pt_برآمد.x = pt_اندر.x;
	pt_برآمد.y = pt_اندر.y;
    }
    rect.x = (صحیح)pt_برآمد.x;
    rect.y = (صحیح)pt_برآمد.y;
    rect.چوڑائی = preedit_label_چوڑائی;
    rect.اونچائی = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    صحیح انحراف = gui.char_اونچائی - preedit_popover_اونچائی;
    gtk_popover_سیٹ_انحراف(GTK_POPOVER(preedit_کھڑکی), 0, انحراف);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_سیٹ_halign(preedit_کھڑکی, GTK_ALIGN_آغاز);
    gtk_popover_سیٹ_pointing_مقصد(GTK_POPOVER(preedit_کھڑکی), &rect);
#  ورنہ
    صحیح x, y, چوڑائی, اونچائی;
    صحیح screen_x, screen_y, screen_چوڑائی, screen_اونچائی;

    gui_gtk_get_screen_geom_کا_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_چوڑائی, &screen_اونچائی);
    gdk_کھڑکی_get_origin(gtk_widget_get_کھڑکی(gui.drawarea), &x, &y);
    gtk_کھڑکی_get_سائز(GTK_کھڑکی(preedit_کھڑکی), &چوڑائی, &اونچائی);
    x = x + FILL_X(gui.ستون);
    y = y + FILL_Y(gui.صف);
    اگر (x + چوڑائی > screen_x + screen_چوڑائی)
	x = screen_x + screen_چوڑائی - چوڑائی;
    اگر (y + اونچائی > screen_y + screen_اونچائی)
	y = screen_y + screen_اونچائی - اونچائی;
    gtk_کھڑکی_move(GTK_کھڑکی(preedit_کھڑکی), x, y);
#  endif
}

    ساکن خالی
im_preedit_کھڑکی_open(خالی)
{
    حرف *preedit_string;
#  اگر !GTK_CHECK_نسخہ(3,16,0)
    حرف buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  اگر GTK_CHECK_نسخہ(3,0,0)
#   اگر !GTK_CHECK_نسخہ(3,16,0)
    GdkRGBA color;
#   endif
#  ورنہ
    GdkColor color;
#  endif
    gint w, h;

    اگر (preedit_کھڑکی == تہی)
    {
#  ifdef USE_GTK4
	// GTK4: use GtkPopover (xdg_popup) so the compositor keeps keyboard
	// focus on the drawing area and does not disable text-input-v3.
	// See issue #20257.  Parent the popover to gui.mainwin (a real
	// container) rather than gui.drawarea: GtkDrawingArea is a leaf
	// widget and using it as a popover parent causes snapshot artifacts
	// (white-out) on the drawing area.
	preedit_کھڑکی = gtk_popover_new();
	gtk_popover_سیٹ_autohide(GTK_POPOVER(preedit_کھڑکی), FALSE);
	gtk_popover_سیٹ_has_arrow(GTK_POPOVER(preedit_کھڑکی), FALSE);
	// Pin the popover below the anchor.  Together with anchor.h == 0 in
	// im_preedit_window_set_position() this lands the popover's top edge
	// exactly on FILL_Y(row), so the popover covers the cursor cell rather
	// than landing above it (the default GTK_POS_TOP would do that and
	// only flip to bottom when there is no room above, which is flaky).
	gtk_popover_سیٹ_position(GTK_POPOVER(preedit_کھڑکی), GTK_POS_BOTTOM);
	gtk_widget_سیٹ_can_focus(preedit_کھڑکی, FALSE);
	gtk_widget_سیٹ_focusable(preedit_کھڑکی, FALSE);
	gtk_widget_سیٹ_can_ہدف(preedit_کھڑکی, FALSE);
	gtk_widget_سیٹ_والد(preedit_کھڑکی, gui.mainwin);
#  ورنہ
	preedit_کھڑکی = gtk_کھڑکی_new(GTK_کھڑکی_POPUP);
	gtk_کھڑکی_سیٹ_transient_for(GTK_کھڑکی(preedit_کھڑکی),
						     GTK_کھڑکی(gui.mainwin));
#  endif
	preedit_label = gtk_label_new("");
	gtk_widget_سیٹ_name(preedit_label, "vim-gui-preedit-area");
#  ifdef USE_GTK4
	gtk_label_سیٹ_xalign(GTK_LABEL(preedit_label), 0.0);
	gtk_widget_سیٹ_halign(preedit_label, GTK_ALIGN_آغاز);
	gtk_widget_سیٹ_valign(preedit_label, GTK_ALIGN_آغاز);
	// Tag the popover with a CSS class so the minimal frame CSS below
	// (no border, no rounded corners, no shadow, no padding) applies
	// only to our preedit popover and not other popovers in the app.
	gtk_widget_جوڑو_css_class(preedit_کھڑکی, "vim-preedit-popover");
	gtk_popover_سیٹ_بچہ(GTK_POPOVER(preedit_کھڑکی), preedit_label);
#  ورنہ
	gtk_container_جوڑو(GTK_CONTAINER(preedit_کھڑکی), preedit_label);
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
    اگر (preedit_css_provider == تہی || preedit_css_cached_bg != gui.عقب_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	اگر (preedit_css_provider != تہی)
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
		(غیرعلامتی طویل)((gui.عقب_pixel >> 16) & 0xff),
		(غیرعلامتی طویل)((gui.عقب_pixel >> 8) & 0xff),
		(غیرعلامتی طویل)(gui.عقب_pixel & 0xff));
	gtk_css_provider_بھرو_from_string(preedit_css_provider, css);
	g_free(css);
	gtk_style_context_جوڑو_provider_for_display(display,
			    GTK_STYLE_PROVIDER(preedit_css_provider), G_MAXUINT);
	preedit_css_cached_bg = gui.عقب_pixel;
    }
#  endif

#  ifndef USE_GTK4
#   اگر GTK_CHECK_نسخہ(3,16,0)
    {
	GtkStyleContext * ثابت context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * ثابت provider = gtk_css_provider_new();
	gchar		   *css = تہی;
	ثابت حرف * ثابت fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_سائز(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = تہی;

	اگر (!pango_font_description_get_سائز_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * ثابت screen
		  = gdk_کھڑکی_get_screen(gtk_widget_get_کھڑکی(gui.mainwin));
	    ثابت gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	اگر (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	ورنہ
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != تہی ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.عقب_pixel >> 16) & 0xff,
		(gui.عقب_pixel >> 8) & 0xff,
		gui.عقب_pixel & 0xff);

	gtk_css_provider_بھرو_from_ڈیٹا(provider, css, -1, تہی);
	gtk_style_context_جوڑو_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_نسخہ(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, حجم(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_معمول, &color);

    vim_snprintf(buf, حجم(buf), "#%06X", gui.عقب_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_معمول,
								      &color);
#   ورنہ
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, حجم(buf), "#%06X", (غیرعلامتی)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_معمول, &color);

    vim_snprintf(buf, حجم(buf), "#%06X", (غیرعلامتی)gui.عقب_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_کھڑکی, GTK_STATE_معمول, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, تہی);

    اگر (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	اگر (attr_list == تہی)
	    attr_list = pango_attr_list_new();

	اگر (gui.norm_font != تہی)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->آغاز_اشاریہ = 0;
	    pa->انتہا_اشاریہ = G_MAXUINT;
	    pango_attr_list_داخل_کرو(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->آغاز_اشاریہ = 0;
	pa->انتہا_اشاریہ = G_MAXUINT;
	pango_attr_list_داخل_کرو(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.عقب_pixel >> 16) & 0xff) * 257,
		((gui.عقب_pixel >> 8) & 0xff) * 257,
		(gui.عقب_pixel & 0xff) * 257);
	pa->آغاز_اشاریہ = 0;
	pa->انتہا_اشاریہ = G_MAXUINT;
	pango_attr_list_داخل_کرو(attr_list, pa);
#  endif
	gtk_label_سیٹ_متن(GTK_LABEL(preedit_label), preedit_string);
	gtk_label_سیٹ_اوصاف(GTK_LABEL(preedit_label), attr_list);

	layout = gtk_label_get_layout(GTK_LABEL(preedit_label));
	pango_layout_get_pixel_سائز(layout, &w, &h);
	h = MAX(h, gui.char_اونچائی);
#  ifdef USE_GTK4
	// Cache label/popover size derived from the Pango layout, which is
	// available without mapping the popover.  Using these for positioning
	// avoids calling gtk_popover_set_pointing_to() after popup, which
	// triggers a GDK "compositor doesn't support moving popups" warning
	// on compositors without xdg_popup.reposition (e.g. Weston).
	preedit_label_چوڑائی = w;
	preedit_popover_اونچائی = h;

	// Report an enlarged cursor rectangle that covers both the cursor
	// cell and the preedit popover.  This way the IM (e.g. fcitx5)
	// places its candidate window below the popover instead of on top
	// of it.  Coordinates are in client-widget (drawarea) space.
	اگر (xic != تہی)
	{
	    GdkRectangle area;

	    area.x = FILL_X(gui.ستون);
	    area.y = FILL_Y(gui.صف);
	    area.چوڑائی = preedit_label_چوڑائی;
	    area.اونچائی = MAX(gui.char_اونچائی, preedit_popover_اونچائی);
	    gtk_im_context_سیٹ_cursor_location(xic, &area);
	}

	// Pop down before re-anchoring: on compositors that lack
	// xdg_popup.reposition (e.g. Weston), calling set_pointing_to() on
	// a mapped popover triggers GDK's remap fallback and a warning.
	// Doing the popdown ourselves makes the next popup land at the new
	// anchor cleanly without that warning.
	اگر (gtk_widget_get_mapped(preedit_کھڑکی))
	    gtk_popover_popdown(GTK_POPOVER(preedit_کھڑکی));

	im_preedit_کھڑکی_سیٹ_position();

	gtk_widget_قطار_resize(preedit_کھڑکی);
	gtk_popover_popup(GTK_POPOVER(preedit_کھڑکی));
#  ورنہ
	gtk_کھڑکی_resize(GTK_کھڑکی(preedit_کھڑکی), w, h);
	gtk_widget_show_all(preedit_کھڑکی);

	im_preedit_کھڑکی_سیٹ_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    ساکن خالی
im_preedit_کھڑکی_close(خالی)
{
    اگر (preedit_کھڑکی == تہی)
	واپس;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_کھڑکی));
#  ورنہ
    gtk_widget_hide(preedit_کھڑکی);
#  endif
}

    ساکن خالی
im_show_preedit(خالی)
{
    im_preedit_کھڑکی_open();

    اگر (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ساکن خالی
im_delete_preedit(خالی)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    اگر (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_کھڑکی_close();
	واپس;
    }

    اگر (State & منوال_معمول
#  ifdef FEAT_TERMINAL
	    && !رکن_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	واپس;
    }
    برائے (; im_preedit_cursor > 0; --im_preedit_cursor)
	جوڑو_مقصد_داخلہ_buf(bskey, (صحیح)حجم(bskey));

    برائے (; im_preedit_trailing > 0; --im_preedit_trailing)
	جوڑو_مقصد_داخلہ_buf(delkey, (صحیح)حجم(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    ساکن خالی
im_correct_cursor(صحیح عدد_move_عقب)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    اگر (State & منوال_معمول)
	واپس;
#  ifdef FEAT_RIGHTLEFT
    اگر ((State & منوال_CMDLINE) == 0 && curwin != تہی && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    برائے (; عدد_move_عقب > 0; --عدد_move_عقب)
	جوڑو_مقصد_داخلہ_buf(backkey, (صحیح)حجم(backkey));
}

ساکن صحیح xim_expected_char = NUL;
ساکن صحیح xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    ساکن خالی
im_show_info(خالی)
{
    صحیح	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    اگر ((State & منوال_معمول) || (State & منوال_داخل_کرو))
	setcursor();
    برآمد_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    ساکن خالی
im_commit_cb(GtkIMContext *context غیر_مستعمل,
	     ثابت gchar *str,
	     gpointer ڈیٹا غیر_مستعمل)
{
    صحیح		slen = (صحیح)STRLEN(str);
    صحیح		جوڑو_مقصد_داخلہ = TRUE;
    صحیح		clen;
    صحیح		len = slen;
    صحیح		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    اگر (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	اگر (preedit_آغاز_ستون == MAXCOL)
	{
	    init_preedit_آغاز_ستون();
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
	اگر (داخلہ_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&داخلہ_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != تہی);
	}
	ورنہ
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	اگر (داخلہ_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_آغاز_ستون += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    اگر (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	اگر (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    جوڑو_مقصد_داخلہ = FALSE;
	}
	ورنہ
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    اگر (جوڑو_مقصد_داخلہ)
	im_جوڑو_مقصد_داخلہ((char_u *)str, slen);

    اگر (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	اگر (!commit_with_preedit)
	    preedit_آغاز_ستون = MAXCOL;

	// This flag is used in changed() at next call.
	xim_بدلا_while_preediting = TRUE;
    }

    اگر (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    ساکن خالی
im_preedit_آغاز_cb(GtkIMContext *context غیر_مستعمل, gpointer ڈیٹا غیر_مستعمل)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_start_cb()\n");
#  endif

    im_is_فعال = TRUE;
    preedit_is_فعال = TRUE;
    gui_update_cursor(TRUE, FALSE);
    im_show_info();
}

/*
 * Callback invoked after end to the preedit.
 */
    ساکن خالی
im_preedit_انتہا_cb(GtkIMContext *context غیر_مستعمل, gpointer ڈیٹا غیر_مستعمل)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    اگر (p_imst == IM_ON_THE_SPOT)
	preedit_آغاز_ستون = MAXCOL;
    xim_has_preediting = FALSE;

#  اگر 0
    // Removal of this line suggested by Takuhiro Nishioka.  Fixes that IM was
    // switched off unintentionally.  We now use preedit_is_active (added by
    // SungHyun Nam).
    im_is_فعال = FALSE;
#  endif
    preedit_is_فعال = FALSE;
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
    ساکن خالی
im_preedit_بدلا_cb(GtkIMContext *context, gpointer ڈیٹا غیر_مستعمل)
{
    حرف    *preedit_string = تہی;
    صحیح	    cursor_اشاریہ    = 0;
    صحیح	    عدد_move_عقب   = 0;
    char_u  *str;
    char_u  *p;
    صحیح	    i;

    اگر (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, تہی,
					  &cursor_اشاریہ);
    ورنہ
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, تہی,
					  تہی);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != تہی); // just in case

    اگر (p_imst == IM_OVER_THE_SPOT)
    {
	اگر (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	ورنہ
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    ورنہ
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	اگر (preedit_آغاز_ستون == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_آغاز_ستون();
	}
	ورنہ اگر (cursor_اشاریہ == 0 && preedit_string[0] == '\0')
	{
	    xim_has_preediting = FALSE;

	    // If at the start position (after typing backspace)
	    // preedit_start_col must be reset.
	    preedit_آغاز_ستون = MAXCOL;
	}

	im_delete_preedit();

	// Compute the end of the preediting area: "preedit_end_col".
	// According to the documentation of
	// gtk_im_context_get_preedit_string(), the cursor_pos output argument
	// returns the offset in bytes.  This is unfortunately not true -- real
	// life shows the offset is in characters, and the GTK+ source code
	// agrees with me.  Will file a bug later.
	اگر (preedit_آغاز_ستون != MAXCOL)
	    preedit_انتہا_ستون = preedit_آغاز_ستون;
	str = (char_u *)preedit_string;
	برائے (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    صحیح is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    اگر (!is_composing || p_deco)
	    {
		اگر (i < cursor_اشاریہ)
		    ++im_preedit_cursor;
		ورنہ
		    ++im_preedit_trailing;
	    }
	    اگر (!is_composing && i >= cursor_اشاریہ)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++عدد_move_عقب;
	    }
	    اگر (preedit_آغاز_ستون != MAXCOL)
		preedit_انتہا_ستون += utf_ptr2cells(p);
	}

	اگر (p > str)
	{
	    im_جوڑو_مقصد_داخلہ(str, (صحیح)(p - str));
	    im_correct_cursor(عدد_move_عقب);
	}
    }

    g_free(preedit_string);

    اگر (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    ساکن صحیح
translate_pango_اوصاف(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    صحیح		    char_attr = HL_معمول;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    اگر (attr != تہی && ((PangoAttrInt *)attr)->قدر
						 != (صحیح)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_وزن);
    اگر (attr != تہی && ((PangoAttrInt *)attr)->قدر >= (صحیح)PANGO_وزن_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    اگر (attr != تہی && ((PangoAttrInt *)attr)->قدر
						   != (صحیح)PANGO_STYLE_معمول)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    اگر (attr != تہی)
    {
	ثابت PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	اگر ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    واپس char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    صحیح
im_get_feedback_attr(صحیح ستون)
{
    حرف	    *preedit_string = تہی;
    PangoAttrList   *attr_list	    = تہی;
    صحیح		    char_attr	    = -1;

    اگر (xic == تہی)
	واپس char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, تہی);

    اگر (preedit_string != تہی && attr_list != تہی)
    {
	صحیح idx;

	// Get the byte index as used by PangoAttrIterator
	برائے (idx = 0; ستون > 0 && preedit_string[idx] != '\0'; --ستون)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	اگر (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    صحیح			آغاز, انتہا;

	    char_attr = HL_معمول;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    کرو
	    {
		pango_attr_iterator_range(iter, &آغاز, &انتہا);

		اگر (idx >= آغاز && idx < انتہا)
		    char_attr |= translate_pango_اوصاف(iter);
	    }
	    جب (pango_attr_iterator_اگلا(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    اگر (attr_list != تہی)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    واپس char_attr;
}

    خالی
xim_init(خالی)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != تہی);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_کھڑکی(gui.drawarea) != تہی);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), تہی);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_بدلا_cb), تہی);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_آغاز_cb), تہی);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_انتہا_cb), تہی);

#  ifdef USE_GTK4
    gtk_im_context_سیٹ_client_widget(xic, gui.drawarea);

    // Seed the cursor location immediately.  GTK4's Wayland IM module
    // batches text-input-v3 state and sends it on the next commit cycle
    // (typically triggered by focus_in).  Without this seed the very
    // first commit would carry the protocol default (0, 0, 0, 0) for the
    // cursor rectangle, and the IM's candidate window for the very first
    // composition would appear at the top-left of the surface instead of
    // next to the cursor.
    im_سیٹ_position(gui.صف, gui.ستون);
#  ورنہ
    gtk_im_context_سیٹ_client_کھڑکی(xic, gtk_widget_get_کھڑکی(gui.drawarea));
#  endif
}

    خالی
im_shutdown(خالی)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    اگر (xic != تہی)
    {
	gtk_im_context_focus_برآمد(xic);
	g_object_unref(xic);
	xic = تہی;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    اگر (preedit_کھڑکی != تہی)
    {
	gtk_widget_unparent(preedit_کھڑکی);
	preedit_کھڑکی = تہی;
	preedit_label = تہی;
    }
    اگر (preedit_css_provider != تہی)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = تہی;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_فعال = FALSE;
    im_commit_handler_id = 0;
    اگر (p_imst == IM_ON_THE_SPOT)
	preedit_آغاز_ستون = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
/*
 * Convert the string argument to keyval and state for GdkEventKey.
 * If str is valid return TRUE, otherwise FALSE.
 *
 * See 'imactivatekey' for documentation of the format.
 */
    ساکن صحیح
im_string_مقصد_keyval(ثابت حرف *str, غیرعلامتی صحیح *keyval, غیرعلامتی صحیح *state)
{
    ثابت حرف	    *mods_انتہا;
    غیرعلامتی	    tmp_keyval;
    غیرعلامتی	    tmp_state = 0;

    mods_انتہا = strrchr(str, '-');
    mods_انتہا = (mods_انتہا != تہی) ? mods_انتہا + 1 : str;

    // Parse modifier keys
    جب (str < mods_انتہا)
	انتخاب (*str++)
	{
	    حالت '-':							توڑو;
	    حالت 'S': حالت 's': tmp_state |= (غیرعلامتی)GDK_سرکاؤ_نقاب;	توڑو;
	    حالت 'L': حالت 'l': tmp_state |= (غیرعلامتی)GDK_تالا_نقاب;	توڑو;
	    حالت 'C': حالت 'c': tmp_state |= (غیرعلامتی)GDK_CONTROL_نقاب;توڑو;
	    حالت '1':		tmp_state |= (غیرعلامتی)GDK_MOD1_نقاب;	توڑو;
	    حالت '2':		tmp_state |= (غیرعلامتی)GDK_MOD2_نقاب;	توڑو;
	    حالت '3':		tmp_state |= (غیرعلامتی)GDK_MOD3_نقاب;	توڑو;
	    حالت '4':		tmp_state |= (غیرعلامتی)GDK_MOD4_نقاب;	توڑو;
	    حالت '5':		tmp_state |= (غیرعلامتی)GDK_MOD5_نقاب;	توڑو;
	    طےشدہ:
		واپس FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    اگر (tmp_keyval == 0 || tmp_keyval == GDK_Void_علامت)
	واپس FALSE;

    اگر (keyval != تہی)
	*keyval = tmp_keyval;
    اگر (state != تہی)
	*state = tmp_state;

    واپس TRUE;
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
    صحیح
im_xim_isvalid_imactivate(خالی)
{
    اگر (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_Void_علامت;
	im_activatekey_state  = 0;
	واپس TRUE;
    }

    واپس im_string_مقصد_keyval((ثابت حرف *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    ساکن خالی
im_synthesize_keypress(غیرعلامتی صحیح keyval, غیرعلامتی صحیح state)
{
    Gdk_Event_کلید *event;

    event = (Gdk_Event_کلید *)gdk_event_new(GDK_کلید_PRESS);
    g_object_ref(gtk_widget_get_کھڑکی(gui.drawarea));
					// unreffed by gdk_event_free()
    event->کھڑکی_2 = gtk_widget_get_کھڑکی(gui.drawarea);
    event->send_event = TRUE;
    event->وقت = GDK_CURRENT_وقت;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysym_مقصد_Keycode(GDK_کھڑکی_XDISPLAY(event->کھڑکی_2), (کلید_Sym)keyval);
    event->لمبائی = 0;
    event->string = تہی;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_کلید_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    خالی
xim_reset(خالی)
{
#  ifdef FEAT_EVAL
    اگر (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_فعال);
    ورنہ
#  endif
    اگر (xic != تہی)
    {
	gtk_im_context_reset(xic);

	اگر (p_imdisable)
	    im_shutdown();
	ورنہ
	{
	    xim_سیٹ_focus(gui.اندر_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_سیٹ_focus(gui.اندر_focus);
#  ورنہ
	    اگر (im_activatekey_keyval != GDK_Void_علامت)
	    {
		اگر (im_is_فعال)
		{
		    g_signal_handler_بلاک(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    ورنہ
	    {
		im_shutdown();
		xim_init();
		xim_سیٹ_focus(gui.اندر_focus);
	    }
#  endif
	}
    }

    اگر (p_imst == IM_ON_THE_SPOT)
	preedit_آغاز_ستون = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    صحیح
xim_قطار_کلید_press_event(Gdk_Event_کلید *event, صحیح نیچے_کو)
{
#   ifdef FEAT_GUI_GTK
    اگر (event->state & GDK_SUPER_نقاب) واپس FALSE;
#   endif
    اگر (نیچے_کو)
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
	انتخاب (event->keyval)
	{
	    حالت GDK_KP_جوڑو:      xim_expected_char = '+';  توڑو;
	    حالت GDK_KP_تفریق: xim_expected_char = '-';  توڑو;
	    حالت GDK_KP_Divide:   xim_expected_char = '/';  توڑو;
	    حالت GDK_KP_Multiply: xim_expected_char = '*';  توڑو;
	    حالت GDK_KP_Decimal:  xim_expected_char = '.';  توڑو;
	    حالت GDK_KP_Equal:    xim_expected_char = '=';  توڑو;
	    حالت GDK_KP_0:	  xim_expected_char = '0';  توڑو;
	    حالت GDK_KP_1:	  xim_expected_char = '1';  توڑو;
	    حالت GDK_KP_2:	  xim_expected_char = '2';  توڑو;
	    حالت GDK_KP_3:	  xim_expected_char = '3';  توڑو;
	    حالت GDK_KP_4:	  xim_expected_char = '4';  توڑو;
	    حالت GDK_KP_5:	  xim_expected_char = '5';  توڑو;
	    حالت GDK_KP_6:	  xim_expected_char = '6';  توڑو;
	    حالت GDK_KP_7:	  xim_expected_char = '7';  توڑو;
	    حالت GDK_KP_8:	  xim_expected_char = '8';  توڑو;
	    حالت GDK_KP_9:	  xim_expected_char = '9';  توڑو;
	    حالت GDK_space:	  xim_expected_char = ' ';  توڑو;
	    طےشدہ:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    اگر (xic != تہی && !p_imdisable
		    && (State & (منوال_داخل_کرو | منوال_CMDLINE
					      | منوال_معمول | منوال_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	اگر (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    غیرعلامتی صحیح state_نقاب;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_نقاب  = im_activatekey_state;
	    state_نقاب |= ((صحیح)GDK_سرکاؤ_نقاب | (صحیح)GDK_CONTROL_نقاب
							| (صحیح)GDK_MOD1_نقاب);

	    اگر ((event->state & state_نقاب) != im_activatekey_state)
		واپس FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    اگر (event->type != GDK_کلید_PRESS)
		واپس TRUE;

	    اگر (map_مقصد_exists_منوال((char_u *)"", منوال_LANGMAP, FALSE))
	    {
		im_سیٹ_فعال(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= منوال_LANGMAP;
		اگر (State & منوال_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~منوال_LANGMAP;
		}
		ورنہ
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= منوال_LANGMAP;
		}
		واپس TRUE;
	    }

	    واپس gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	اگر (im_activatekey_keyval == GDK_Void_علامت || im_is_فعال)
	{
	    صحیح imresult = gtk_im_context_filter_keypress(xic, event);

	    اگر (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		اگر (!imresult && preedit_آغاز_ستون != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    واپس FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    اگر (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		واپس FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    واپس imresult;
	}
    }

    واپس FALSE;
}
#  ورنہ // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    صحیح
im_xim_isvalid_imactivate(خالی)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    واپس p_imak[0] == NUL;
}

    صحیح
xim_قطار_کلید_press_event(GdkEvent *event غیر_مستعمل, صحیح نیچے_کو غیر_مستعمل)
{
    واپس FALSE;
}
#  endif // !USE_GTK4

    صحیح
im_get_حالت(خالی)
{
#  ifdef FEAT_EVAL
    اگر (USE_IMSTATUSFUNC)
	واپس call_imstatusfunc();
#  endif
    واپس im_is_فعال;
}

    صحیح
preedit_get_حالت(خالی)
{
    واپس preedit_is_فعال;
}

    صحیح
im_is_preediting(خالی)
{
    واپس xim_has_preediting;
}

# ورنہ // !FEAT_GUI_GTK

ساکن صحیح	xim_is_فعال = FALSE;  // XIM should be active in the current
					//منوال
ساکن صحیح	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
ساکن XIMStyle	داخلہ_style;
ساکن صحیح	حالت_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    خالی
im_سیٹ_فعال(صحیح فعال_arg)
{
    صحیح فعال = فعال_arg;

    // If 'imdisable' is set, XIM is never active.
    اگر (p_imdisable)
	فعال = FALSE;
    ورنہ اگر (داخلہ_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	فعال = TRUE;

#  اگر defined(FEAT_EVAL)
    اگر (USE_IMACTIVATEFUNC)
    {
	اگر (فعال != im_get_حالت())
	{
	    call_imactivatefunc(فعال);
	    xim_has_focus = فعال;
	}
	واپس;
    }
#  endif

    اگر (xic == تہی)
	واپس;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_فعال = فعال;
    xim_سیٹ_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    خالی
xim_سیٹ_focus(صحیح focus)
{
    اگر (xic == تہی)
	واپس;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    اگر (focus && xim_is_فعال)
    {
	اگر (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    ورنہ
    {
	اگر (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    خالی
im_سیٹ_position(صحیح صف غیر_مستعمل, صحیح ستون غیر_مستعمل)
{
    xim_سیٹ_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    خالی
xim_سیٹ_preedit(خالی)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    صحیح سطر_space;

    اگر (xic == تہی)
	واپس;

    xim_سیٹ_focus(TRUE);

    اگر (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							تہی);
	XSetICValues(xic, XNPreedit_اوصاف, attr_list, تہی);
	XFree(attr_list);
	واپس;
    }

    اگر (داخلہ_style & XIMPreeditPosition)
    {
	اگر (xim_fg_color == INVALCOLOR)
	{
	    xim_fg_color = gui.def_norm_pixel;
	    xim_bg_color = gui.def_عقب_pixel;
	}
	over_spot.x = متن_X(gui.ستون);
	over_spot.y = متن_Y(gui.صف);
	spot_area.x = 0;
	spot_area.y = 0;
	spot_area.اونچائی = gui.char_اونچائی * صفیں;
	spot_area.چوڑائی  = gui.char_چوڑائی * Columns;
	سطر_space = gui.char_اونچائی;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, سطر_space,
					تہی);
	اگر (XSetICValues(xic, XNPreedit_اوصاف, attr_list, تہی))
	    emsg(_(e_cannot_سیٹ_ic_اقدار));
	XFree(attr_list);
    }
}

#  اگر defined(FEAT_GUI_X11)
#   اگر defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

ساکن صحیح xim_real_init(کھڑکی x11_کھڑکی, Display *x11_display);


#   ifdef USE_X11R6_XIM
    ساکن خالی
xim_instantiate_cb(
    Display	*display,
    XPointer	client_ڈیٹا_2 غیر_مستعمل,
    XPointer	call_ڈیٹا غیر_مستعمل)
{
    کھڑکی	x11_کھڑکی;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_instantiate_cb()\n");
#    endif

    gui_get_x11_windis(&x11_کھڑکی, &x11_display);
    اگر (display != x11_display)
	واپس;

    xim_real_init(x11_کھڑکی, x11_display);
    gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    اگر (xic != تہی)
	XUnregisterIMInstantiateCallback(x11_display, تہی, تہی, تہی,
					 xim_instantiate_cb, تہی);
}

    ساکن خالی
xim_destroy_cb(
    XIM		im غیر_مستعمل,
    XPointer	client_ڈیٹا_2 غیر_مستعمل,
    XPointer	call_ڈیٹا غیر_مستعمل)
{
    کھڑکی	x11_کھڑکی;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_destroy_cb()\n");
#    endif
    gui_get_x11_windis(&x11_کھڑکی, &x11_display);

    xic = تہی;
    حالت_area_enabled = FALSE;

    gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, تہی, تہی, تہی,
				   xim_instantiate_cb, تہی);
}
#   endif

    خالی
xim_init(خالی)
{
    کھڑکی	x11_کھڑکی;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_کھڑکی, &x11_display);

    xic = تہی;

    اگر (xim_real_init(x11_کھڑکی, x11_display))
	واپس;

    gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, تہی, تہی, تہی,
				   xim_instantiate_cb, تہی);
#   endif
}

    ساکن صحیح
xim_real_init(کھڑکی x11_کھڑکی, Display *x11_display)
{
    صحیح		i;
    حرف	*p,
		*s,
		*ns,
		*انتہا,
		tmp[1024];
#   define IMLEN_MAX 40
    حرف	buf[IMLEN_MAX + 7];
    XIM		xim = تہی;
    XIMStyles	*xim_styles;
    XIMStyle	this_داخلہ_style = 0;
    Boolean	ملا;
    XPoint	over_spot;
    XVaNestedList preedit_list, حالت_list;

    داخلہ_style = 0;
    حالت_area_enabled = FALSE;

    اگر (xic != تہی)
	واپس FALSE;

    اگر (gui.rsrc_داخلہ_طریقہ != تہی && *gui.rsrc_داخلہ_طریقہ != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_داخلہ_طریقہ,
	    حجم(tmp) - 1);
	برائے (ns = s = tmp; ns != تہی && *s != NUL;)
	{
	    s = (حرف *)skipwhite((char_u *)s);
	    اگر (*s == NUL)
		توڑو;
	    اگر ((ns = انتہا = strchr(s, ',')) == تہی)
		انتہا = s + لڑی_لمبائی(s);
	    جب (محفوظ_isspace(انتہا[-1]))
		انتہا--;
	    *انتہا = NUL;

	    اگر (لڑی_لمبائی(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		اگر ((p = XSetLocaleModifiers(buf)) != تہی && *p != NUL
			&& (xim = XOpenIM(x11_display, تہی, تہی, تہی))
								      != تہی)
		    توڑو;
	    }

	    s = ns + 1;
	}
    }

    اگر (xim == تہی && (p = XSetLocaleModifiers("")) != تہی && *p != NUL)
	xim = XOpenIM(x11_display, تہی, تہی, تہی);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    اگر (xim == تہی && (p = XSetLocaleModifiers("@im=none")) != تہی
								 && *p != NUL)
	xim = XOpenIM(x11_display, تہی, تہی, تہی);

    اگر (xim == تہی)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_مقصد_open_داخلہ_طریقہ));
	    verbose_leave();
	}
	واپس FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_ڈیٹا_2 = تہی;
	اگر (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, تہی))
	    emsg(_(e_warning_could_not_سیٹ_destroy_callback_مقصد_im));
    }
#   endif

    اگر (XGetIMValues(xim, XNQuery_داخلہ_Style, &xim_styles, تہی) || !xim_styles)
    {
	emsg(_(e_داخلہ_طریقہ_doesnt_support_any_style));
	XCloseIM(xim);
	واپس FALSE;
    }

    ملا = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    حجم(tmp) - 1);
    برائے (s = tmp; s && !ملا; )
    {
	جب (*s && محفوظ_isspace(*s))
	    s++;
	اگر (!*s)
	    توڑو;
	اگر ((ns = انتہا = strchr(s, ',')) != 0)
	    ns++;
	ورنہ
	    انتہا = s + لڑی_لمبائی(s);
	جب (محفوظ_isspace(*انتہا))
	    انتہا--;
	*انتہا = '\0';

	اگر (!strcmp(s, "OverTheSpot"))
	    this_داخلہ_style = (XIMPreeditPosition | XIMStatusArea);
	ورنہ اگر (!strcmp(s, "OffTheSpot"))
	    this_داخلہ_style = (XIMPreeditArea | XIMStatusArea);
	ورنہ اگر (!strcmp(s, "Root"))
	    this_داخلہ_style = (XIMPreeditNothing | XIMStatusNothing);

	برائے (i = 0; (غیرعلامتی مختصر)i < xim_styles->گنتی_styles; i++)
	{
	    اگر (this_داخلہ_style == xim_styles->supported_styles[i])
	    {
		ملا = True;
		توڑو;
	    }
	}
	اگر (!ملا)
	    برائے (i = 0; (غیرعلامتی مختصر)i < xim_styles->گنتی_styles; i++)
	    {
		اگر ((xim_styles->supported_styles[i] & this_داخلہ_style)
			== (this_داخلہ_style & ~XIMStatusArea))
		{
		    this_داخلہ_style &= ~XIMStatusArea;
		    ملا = True;
		    توڑو;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    اگر (!ملا)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_داخلہ_طریقہ_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	واپس FALSE;
    }

    over_spot.x = متن_X(gui.ستون);
    over_spot.y = متن_Y(gui.صف);
    داخلہ_style = this_داخلہ_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    اگر (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_عقب_pixel,
				XNFont_سیٹ, (XFont_سیٹ)gui.fontset,
				تہی);
	حالت_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_عقب_pixel,
				XNFont_سیٹ, (XFont_سیٹ)gui.fontset,
				تہی);
    }
    ورنہ
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_عقب_pixel,
				تہی);
	حالت_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_عقب_pixel,
				تہی);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, داخلہ_style,
		    XNClient_کھڑکی, x11_کھڑکی,
		    XNFocus_کھڑکی, gui.wid,
		    XNPreedit_اوصاف, preedit_list,
		    XNStatus_اوصاف, حالت_list,
		    تہی);
    XFree(حالت_list);
    XFree(preedit_list);
    اگر (xic != تہی)
    {
	اگر (داخلہ_style & XIMStatusArea)
	{
	    xim_سیٹ_حالت_area();
	    حالت_area_enabled = TRUE;
	}
	ورنہ
	    gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    ورنہ
    {
	اگر (!is_not_a_رکن())
	    emsg(_(e_failed_مقصد_create_داخلہ_context));
	XCloseIM(xim);
	واپس FALSE;
    }

    واپس TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    صحیح
im_get_حالت(خالی)
{
#  ifdef FEAT_EVAL
    اگر (USE_IMSTATUSFUNC)
	واپس call_imstatusfunc();
#  endif
    واپس xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# اگر !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    خالی
xim_سیٹ_حالت_area(خالی)
{
    XVaNestedList preedit_list = 0, حالت_list = 0, list = 0;
    XRectangle pre_area, حالت_area;

    اگر (xic == تہی)
	واپس;

    اگر (داخلہ_style & XIMStatusArea)
    {
	اگر (داخلہ_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    حالت_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, تہی);
	    XGetICValues(xic, XNStatus_اوصاف, حالت_list, تہی);
	    XFree(حالت_list);

	    حالت_area.چوڑائی = needed_rect->چوڑائی;
	}
	ورنہ
	    حالت_area.چوڑائی = gui.char_چوڑائی * Columns;

	حالت_area.x = 0;
	حالت_area.y = gui.char_اونچائی * صفیں + gui.border_انحراف;
	اگر (gui.which_scrollbars[SBAR_BOTTOM])
	    حالت_area.y += gui.scrollbar_اونچائی;
#  ifdef FEAT_MENU
	اگر (gui.menu_is_فعال)
	    حالت_area.y += gui.menu_اونچائی;
#  endif
	حالت_area.اونچائی = gui.char_اونچائی;
	حالت_list = XVaCreateNestedList(0, XNArea, &حالت_area, تہی);
    }
    ورنہ
    {
	حالت_area.x = 0;
	حالت_area.y = gui.char_اونچائی * صفیں + gui.border_انحراف;
	اگر (gui.which_scrollbars[SBAR_BOTTOM])
	    حالت_area.y += gui.scrollbar_اونچائی;
#  ifdef FEAT_MENU
	اگر (gui.menu_is_فعال)
	    حالت_area.y += gui.menu_اونچائی;
#  endif
	حالت_area.چوڑائی = 0;
	حالت_area.اونچائی = gui.char_اونچائی;
    }

    اگر (داخلہ_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = حالت_area.x + حالت_area.چوڑائی;
	pre_area.y = gui.char_اونچائی * صفیں + gui.border_انحراف;
	pre_area.چوڑائی = gui.char_چوڑائی * Columns - pre_area.x;
	اگر (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_اونچائی;
#  ifdef FEAT_MENU
	اگر (gui.menu_is_فعال)
	    pre_area.y += gui.menu_اونچائی;
#  endif
	pre_area.اونچائی = gui.char_اونچائی;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, تہی);
    }
    ورنہ اگر (داخلہ_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.اونچائی = gui.char_اونچائی * صفیں;
	pre_area.چوڑائی = gui.char_چوڑائی * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, تہی);
    }

    اگر (preedit_list && حالت_list)
	list = XVaCreateNestedList(0, XNPreedit_اوصاف, preedit_list,
				   XNStatus_اوصاف, حالت_list, تہی);
    ورنہ اگر (preedit_list)
	list = XVaCreateNestedList(0, XNPreedit_اوصاف, preedit_list,
				   تہی);
    ورنہ اگر (حالت_list)
	list = XVaCreateNestedList(0, XNStatus_اوصاف, حالت_list,
				   تہی);
    ورنہ
	list = تہی;

    اگر (list)
    {
	XSetICValues(xic, XNVaNestedList, list, تہی);
	XFree(list);
    }
    اگر (حالت_list)
	XFree(حالت_list);
    اگر (preedit_list)
	XFree(preedit_list);
}

    صحیح
xim_get_حالت_area_اونچائی(خالی)
{
    اگر (حالت_area_enabled)
	واپس gui.char_اونچائی;
    واپس 0;
}
# endif

#ورنہ // !defined(FEAT_XIM)

# اگر defined(IME_WITHOUT_XIM) || defined(VIMDLL)
ساکن صحیح im_was_سیٹ_فعال = FALSE;

    صحیح
#  ifdef VIMDLL
mbyte_im_get_حالت(خالی)
#  ورنہ
im_get_حالت(خالی)
#  endif
{
#  اگر defined(FEAT_EVAL)
    اگر (USE_IMSTATUSFUNC)
	واپس call_imstatusfunc();
#  endif
    واپس im_was_سیٹ_فعال;
}

    خالی
#  ifdef VIMDLL
mbyte_im_سیٹ_فعال(صحیح فعال_arg)
#  ورنہ
im_سیٹ_فعال(صحیح فعال_arg)
#  endif
{
#  اگر defined(FEAT_EVAL)
    صحیح	    فعال = !p_imdisable && فعال_arg;

    اگر (USE_IMACTIVATEFUNC && فعال != im_get_حالت())
    {
	call_imactivatefunc(فعال);
	im_was_سیٹ_فعال = فعال;
    }
#  endif
}

#  اگر defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    خالی
im_سیٹ_position(صحیح صف غیر_مستعمل, صحیح ستون غیر_مستعمل)
{
}
#  endif
# endif

#اگر_ختم // FEAT_XIM
