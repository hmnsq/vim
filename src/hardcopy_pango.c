/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * See https://www.cairographics.org/documentation/using_the_postscript_surface/
 * for information about Cairo's postscript backend.
 *
 * Related:
 * https://www.cairographics.org/manual/cairo-PDF-Surfaces.html#cairo-pdf-surface-set-size
 */

#شامل "vim.h"
#شامل "version.h"

#اگر_تعریف_شدہ FEAT_PRINT_PANGO

#شامل "hardcopy.h"

#شامل <cairo/cairo-ps.h>
#شامل <cairo/cairo-pdf.h>
#شامل <pango/pango.h>
#شامل <pango/pangocairo.h>

#تعریف_کرو PRINT_DPI 72.0

#تعریف_کرو PRT_DRAW_BOLD 1
#تعریف_کرو PRT_DRAW_ITALIC 2
#تعریف_کرو PRT_DRAW_UNDERLINE 4

#تعریف_کرو GET_Y(l) (pctx.چوٹی_margin + (l) * pctx.char_اونچائی)
#تعریف_کرو GET_متن_Y(l) (GET_Y(l) + pctx.char_ascent)

قسمتعریف شمار
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
ساکن ساخت
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *متن_context;

    PangoFontDescription *font;

    char_u *filename;

    صحیح media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    دوہرا char_چوڑائی;
    دوہرا char_ascent;
    دوہرا char_اونچائی;
    دوہرا سطر_spacing;
    دوہرا ul_pos;
    دوہرا ul_thickness;

    دوہرا صفحہ_چوڑائی;
    دوہرا صفحہ_اونچائی;

    دوہرا بایاں_margin;
    دوہرا دایاں_margin;
    دوہرا چوٹی_margin;
    دوہرا bottom_margin;

    دوہرا نمبر_چوڑائی;

    // Current position to draw from
    دوہرا cur_x;
    صحیح cur_سطر;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#اگر_تعریف_شدہ USE_GTK4_PRINT_DIALOG
    bool dialog;
#اگر_ختم
} pctx;

ساکن ثابت حرف *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    ساکن دوہرا
مقصد_device_units(صحیح idx, دوہرا physsize, صحیح def_نمبر)
{
    اعشاری	ret;
    صحیح		u;
    صحیح		nr;

    u = prt_get_unit(idx);
    اگر (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_نمبر;
    }
    ورنہ
	nr = printer_opts[idx].نمبر;

    انتخاب (u)
    {
	حالت PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    توڑو;
	حالت PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    توڑو;
	حالت PRT_UNIT_نقطہ:
	    ret = (دوہرا)nr;
	    توڑو;
	حالت PRT_UNIT_PERC:
	طےشدہ:
	    ret = physsize * (nr / 100.0);
	    توڑو;
    }

    واپس ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ساکن خالی
get_صفحہ_margins(
	دوہرا	چوڑائی,
	دوہرا	اونچائی,
	دوہرا	*بایاں,
	دوہرا	*دایاں,
	دوہرا	*چوٹی,
	دوہرا	*bottom)
{
    *بایاں = مقصد_device_units(OPT_PRINT_بایاں, چوڑائی, 10);
    *دایاں = چوڑائی - مقصد_device_units(OPT_PRINT_دایاں, چوڑائی, 5);

    *چوٹی = مقصد_device_units(OPT_PRINT_چوٹی, اونچائی, 5);
    *bottom = اونچائی - مقصد_device_units(OPT_PRINT_BOT, اونچائی, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    ساکن صحیح
get_cpl(خالی)
{
    اگر (prt_use_نمبر())
    {
	pctx.نمبر_چوڑائی = PRINT_نمبر_چوڑائی * pctx.char_چوڑائی;
	pctx.بایاں_margin += pctx.نمبر_چوڑائی;
    }
    ورنہ
	pctx.نمبر_چوڑائی = 0.0;

    واپس (صحیح)((pctx.دایاں_margin - pctx.بایاں_margin) / pctx.char_چوڑائی);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ساکن صحیح
get_lpp(خالی)
{
    صحیح lpp;

    // Calculate lpp
    lpp = (صحیح)((pctx.bottom_margin - pctx.چوٹی_margin) / pctx.char_اونچائی);

    // Adjust top margin if there is a header
    pctx.چوٹی_margin += pctx.char_اونچائی * prt_سرخی_اونچائی();

    واپس lpp - prt_سرخی_اونچائی();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    ساکن PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    اگر (name == تہی)
	واپس تہی;

    font = pango_font_description_from_string((ثابت حرف *)name);
    اگر (font == تہی)
    {
	semsg(_(e_unknown_printer_font_str), name);
	واپس تہی;
    }

    // Ensure a size is set
    اگر (pango_font_description_get_سائز(font) <= 0)
	pango_font_description_سیٹ_سائز(font, 10 * PANGO_SCALE);

    واپس font;
}

    صحیح
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	صحیح		forceit غیر_مستعمل)
{
    char_u	*paper_name;
    صحیح		paper_strlen;
    bool	portrait;
    صحیح		media;
    char_u	*filename = تہی;
#اگر_تعریف_شدہ USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    اگر (gui.اندر_use && !forceit && psettings->outfile == تہی)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.صفحہ_چوڑائی, &pctx.صفحہ_اونچائی);

	اگر (filename == تہی)
	    واپس FAIL;
	dialog = true;
    }
#اگر_ختم

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#اگر_تعریف_شدہ USE_GTK4_PRINT_DIALOG
    اگر (!dialog)
#اگر_ختم
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	اگر (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].لڑی_لمبائی;
	}
	ورنہ
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	برائے (media = 0; media < (صحیح)PRT_MEDIASIZE_LEN; ++media)
	    اگر ((صحیح)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		توڑو;
	اگر (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	اگر (portrait)
	{
	    pctx.صفحہ_چوڑائی = prt_mediasize[media].چوڑائی;
	    pctx.صفحہ_اونچائی = prt_mediasize[media].اونچائی;
	}
	ورنہ
	{
	    pctx.صفحہ_چوڑائی = prt_mediasize[media].اونچائی;
	    pctx.صفحہ_اونچائی = prt_mediasize[media].چوڑائی;
	}

	اگر (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    برائے (صحیح i = 0; i < (صحیح)ARRAY_لمبائی(print_formats); i++)
		اگر (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].لڑی_لمبائی) == 0)
		{
		    pctx.format = i;
		    توڑو;
		}
	}
    }
    psettings->user_abort = FALSE;

#اگر_تعریف_شدہ USE_GTK4_PRINT_DIALOG
    اگر (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#اگر_ختم

    اگر (filename == تہی)
    {
	// If the user didn't specify a file name, use a temp file.
	اگر (psettings->outfile == تہی)
	{
	    filename = vim_tempname('p', TRUE);
	    اگر (filename == تہی)
	    {
		emsg(_(e_cant_get_عارضی_file_name));
		واپس FAIL;
	    }
	}
	ورنہ
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    اگر (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ثابت حرف *)filename,
		pctx.صفحہ_چوڑائی, pctx.صفحہ_اونچائی);
    ورنہ
	pctx.surface = cairo_pdf_surface_create((ثابت حرف *)filename,
		pctx.صفحہ_چوڑائی, pctx.صفحہ_اونچائی);

    pctx.filename = (char_u *)filename;

    اگر (cairo_surface_حالت(pctx.surface) != CAIRO_حالت_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	واپس FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.متن_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    اگر (pctx.font == تہی)
    {
	mch_print_cleanup();
	واپس FAIL;
    }

    pango_context_سیٹ_font_description(pctx.متن_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_سیٹ_resolution(pctx.متن_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	صحیح		    ascent;
	صحیح		    descent;

	layout = pango_layout_new(pctx.متن_context);
	pango_layout_سیٹ_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, تہی);

	pctx.char_چوڑائی =
	    (دوہرا)pango_font_metrics_get_approximate_char_چوڑائی(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (دوہرا)ascent / PANGO_SCALE;
	pctx.char_اونچائی = (دوہرا)(ascent + descent) / PANGO_SCALE;

	pctx.سطر_spacing = (دوہرا)
	    (pango_font_metrics_get_اونچائی(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (دوہرا)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (دوہرا)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	دوہرا  بایاں;
	دوہرا	دایاں;
	دوہرا	چوٹی;
	دوہرا	bottom;

	get_صفحہ_margins(pctx.صفحہ_چوڑائی, pctx.صفحہ_اونچائی, &بایاں, &دایاں, &چوٹی,
		&bottom);
	pctx.بایاں_margin = بایاں;
	pctx.دایاں_margin = دایاں;
	pctx.چوٹی_margin = چوٹی;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_فی_سطر = get_cpl();
    psettings->سطریں_فی_صفحہ = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#اگر_تعریف_شدہ USE_GTK4_PRINT_DIALOG
    اگر (!dialog)
#اگر_ختم
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	اگر (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    اگر (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    ورنہ اگر (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#اگر_تعریف_شدہ USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#اگر_ختم

    واپس OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    ساکن خالی
emit_dsc_comment(حرف *comment, حرف *fmt, ...)
{
    ساکن حرف buf[257];

    va_list ap;
    صحیح	    len = حجم(buf);
    صحیح	    o;
    حرف    *s = buf;

    اگر (fmt == تہی)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	جاؤ باہر_نکلو;
    }
    ورنہ
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    اگر (len <= 0)
	واپس;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

باہر_نکلو:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    صحیح
mch_print_begin(prt_settings_T *psettings)
{
    حرف user[256];

    pctx.cur_x = 0.0;
    pctx.cur_سطر = 0;
    pctx.draw_flags = 0;

#اگر_تعریف_شدہ USE_GTK4_PRINT_DIALOG
    اگر (pctx.dialog)
	واپس TRUE;
#اگر_ختم

    اگر (!get_user_name((char_u *)user, حجم(user)))
	STRCPY(user, "Unknown");

    اگر (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (حرف *)psettings->jobname);
	emit_dsc_comment("For", user);
	emit_dsc_comment("Orientation",
		pctx.portrait ? "Portrait" : "Landscape");
	emit_dsc_comment("PageOrder", "Ascend");

	emit_dsc_comment("DocumentMedia",
		"%s %.2lf %.2lf 0 () ()",
		prt_mediasize[pctx.media].name,
		prt_mediasize[pctx.media].چوڑائی,
		prt_mediasize[pctx.media].اونچائی);

	emit_dsc_comment("Requirements", "%s%s %s %s",
		pctx.duplex ? "duplex" : "",
		pctx.tumble ? "(tumble)" : "",
		pctx.collate ? "collate" : "",
		psettings->do_syntax ? "color" : "");

	// In defaults section (defaults for every page)
	emit_dsc_comment("BeginDefaults", تہی);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", تہی);
    }
    ورنہ اگر (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	اگر (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, تہی);

	cairo_pdf_surface_سیٹ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (حرف *)str);
	اگر (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	اگر (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, تہی);
	cairo_pdf_surface_سیٹ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (حرف *)str);
	اگر (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_سیٹ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_نسخہ_LONG);
    }

    واپس TRUE;
}

    خالی
mch_print_انتہا(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_اختتام(pctx.surface);

    اگر (psettings->outfile == تہی && !got_int && !psettings->user_abort
	    && pctx.filename != تہی
#اگر_تعریف_شدہ USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#اگر_ختم
       )
    {
	پیغام(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	اگر (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_مقصد_print_postscript_file));
	ورنہ
	    پیغام(_("Print job sent."));
    }

    mch_print_cleanup();
}

    صحیح
mch_print_انتہا_صفحہ(خالی)
{
    cairo_show_صفحہ(pctx.cr);
    واپس TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    صحیح
mch_print_begin_صفحہ(char_u *str غیر_مستعمل)
{
    واپس TRUE;
}

    صحیح
mch_print_blank_صفحہ(خالی)
{
    mch_print_انتہا_صفحہ();
    واپس TRUE;
}

    خالی
mch_print_آغاز_سطر(صحیح margin, صحیح صفحہ_سطر)
{
    pctx.cur_x = pctx.بایاں_margin;
    اگر (margin)
	pctx.cur_x -= pctx.نمبر_چوڑائی;

    pctx.cur_سطر = صفحہ_سطر;
}

    ساکن خالی
long_مقصد_rgb(long_u c, دوہرا *r, دوہرا *g, دوہرا *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    صحیح
mch_print_متن_برآمد(char_u *textp, صحیح len)
{
    PangoAttrList	*attr_list;
    GList		*شے_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    دوہرا		r, g, b;
    دوہرا		چوڑائی = 0;
    دوہرا		اگلا_x;

    // Always convert to UTF-8, Pango expects that
    اگر (output_conv.vc_type == CONV_NONE)
	str = textp;
    ورنہ
	str = string_convert(&output_conv, textp, &len);

    اگر (str == تہی)
	واپس TRUE;

    attr_list = pango_attr_list_new();

#تعریف_کرو داخل_کرو_PANGO_ATTR(Attribute, AttrList, آغاز_2, انتہا_2)  \
    G_STMT_آغاز{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->آغاز_اشاریہ = (آغاز_2);		    \
	tmp_attr_->انتہا_اشاریہ = (انتہا_2);			    \
	pango_attr_list_داخل_کرو((AttrList), tmp_attr_);	    \
    }G_STMT_انتہا

    // Underline attribute does not work with pango_itemize().
    اگر ((pctx.draw_flags & PRT_DRAW_BOLD))
	داخل_کرو_PANGO_ATTR(pango_attr_وزن_new(PANGO_وزن_BOLD),
		attr_list, 0, len);
    اگر (pctx.draw_flags & PRT_DRAW_ITALIC)
	داخل_کرو_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#تعریف_منسوخ داخل_کرو_PANGO_ATTR

    شے_list = pango_itemize(pctx.متن_context,
	    (ثابت حرف *)str, 0, len, attr_list, تہی);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    جب (شے_list != تہی)
    {
	Pango_شے   *شے;
	دوہرا	    o;

	شے = (Pango_شے *)شے_list->ڈیٹا;
	شے_list = g_list_delete_link(شے_list, شے_list);

	pango_shape_بھرا((ثابت حرف *)str + شے->انحراف, شے->لمبائی,
		(ثابت حرف *)str, len, &شے->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (دوہرا)pango_glyph_string_get_چوڑائی(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	اگر (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_مقصد_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_سیٹ_ماخذ_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + چوڑائی,
		    GET_Y(pctx.cur_سطر) - pctx.سطر_spacing, o, pctx.char_اونچائی);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_مقصد_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_سیٹ_ماخذ_rgb(pctx.cr, r, g, b);
	cairo_move_مقصد(pctx.cr, pctx.cur_x + چوڑائی, GET_متن_Y(pctx.cur_سطر));
	pango_cairo_show_glyph_string(pctx.cr, شے->analysis.font, glyphs);

	pango_شے_free(شے);
	چوڑائی += o;
    }

    // Draw underline if needed
    اگر (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	دوہرا y = GET_متن_Y(pctx.cur_سطر) - pctx.ul_pos;

	long_مقصد_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_سیٹ_ماخذ_rgb(pctx.cr, r, g, b);
	cairo_سیٹ_سطر_چوڑائی(pctx.cr, pctx.ul_thickness);
	cairo_move_مقصد(pctx.cr, pctx.cur_x, y);
	cairo_سطر_مقصد(pctx.cr, pctx.cur_x + چوڑائی, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += چوڑائی;

    اگلا_x = pctx.cur_x + pctx.char_چوڑائی;

    اگر (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    واپس (اگلا_x > pctx.دایاں_margin) &&
	((اگلا_x - pctx.دایاں_margin) > (pctx.دایاں_margin * 1e-5));
}

    خالی
mch_print_سیٹ_font(صحیح bold, صحیح italic, صحیح underline)
{
    pctx.draw_flags = 0;

    اگر (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    اگر (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    اگر (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    خالی
mch_print_سیٹ_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    خالی
mch_print_سیٹ_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    خالی
mch_print_cleanup(خالی)
{
    g_clear_اشارہ_گر(&pctx.cr, cairo_destroy);
    g_clear_اشارہ_گر(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.متن_context);
    g_clear_اشارہ_گر(&pctx.font, pango_font_description_free);

#اگر_تعریف_شدہ USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    اگر (pctx.dialog)
	gui_gtk4_print_اختتام();
#اگر_ختم

    g_clear_اشارہ_گر(&pctx.filename, vim_free);
}

#اگر_ختم // FEAT_PRINT_PANGO
