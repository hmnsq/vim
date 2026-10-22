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

#ضمن "vim.h"
#ضمن "version.h"

#اگر_تعریف_شده FEAT_PRINT_PANGO

#ضمن "hardcopy.h"

#ضمن <cairo/cairo-ps.h>
#ضمن <cairo/cairo-pdf.h>
#ضمن <pango/pango.h>
#ضمن <pango/pangocairo.h>

#تعریف_کن PRINT_DPI 72.0

#تعریف_کن PRT_DRAW_BOLD 1
#تعریف_کن PRT_DRAW_ITALIC 2
#تعریف_کن PRT_DRAW_UNDERLINE 4

#تعریف_کن GET_Y(l) (pctx.بالاترین_margin + (l) * pctx.char_ارتفاع)
#تعریف_کن GET_متن_Y(l) (GET_Y(l) + pctx.char_ascent)

تعريفنوع تعداد
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
ساكن بنية
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *متن_context;

    PangoFontDescription *font;

    char_u *filename;

    صحيح media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    مزدوج char_عرض;
    مزدوج char_ascent;
    مزدوج char_ارتفاع;
    مزدوج سطر_spacing;
    مزدوج ul_pos;
    مزدوج ul_thickness;

    مزدوج صفحه_عرض;
    مزدوج صفحه_ارتفاع;

    مزدوج چپ_margin;
    مزدوج راست_margin;
    مزدوج بالاترین_margin;
    مزدوج bottom_margin;

    مزدوج عدد_عرض;

    // Current position to draw from
    مزدوج cur_x;
    صحيح cur_سطر;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#اگر_تعریف_شده USE_GTK4_PRINT_DIALOG
    bool dialog;
#پایان_اگر
} pctx;

ساكن ثابت حرف *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    ساكن مزدوج
مقصد_device_units(صحيح idx, مزدوج physsize, صحيح def_عدد)
{
    عائم	ret;
    صحيح		u;
    صحيح		nr;

    u = prt_get_unit(idx);
    إذا (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_عدد;
    }
    وإلا
	nr = printer_opts[idx].عدد;

    بدل (u)
    {
	حالة PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    اكسر;
	حالة PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    اكسر;
	حالة PRT_UNIT_نقطه:
	    ret = (مزدوج)nr;
	    اكسر;
	حالة PRT_UNIT_PERC:
	افتراضي:
	    ret = physsize * (nr / 100.0);
	    اكسر;
    }

    أرجع ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ساكن فراغ
get_صفحه_margins(
	مزدوج	عرض,
	مزدوج	ارتفاع,
	مزدوج	*چپ_2,
	مزدوج	*راست_2,
	مزدوج	*بالاترین,
	مزدوج	*bottom)
{
    *چپ_2 = مقصد_device_units(OPT_PRINT_چپ, عرض, 10);
    *راست_2 = عرض - مقصد_device_units(OPT_PRINT_راست, عرض, 5);

    *بالاترین = مقصد_device_units(OPT_PRINT_بالاترین, ارتفاع, 5);
    *bottom = ارتفاع - مقصد_device_units(OPT_PRINT_BOT, ارتفاع, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    ساكن صحيح
get_cpl(فراغ)
{
    إذا (prt_use_عدد())
    {
	pctx.عدد_عرض = PRINT_عدد_عرض * pctx.char_عرض;
	pctx.چپ_margin += pctx.عدد_عرض;
    }
    وإلا
	pctx.عدد_عرض = 0.0;

    أرجع (صحيح)((pctx.راست_margin - pctx.چپ_margin) / pctx.char_عرض);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ساكن صحيح
get_lpp(فراغ)
{
    صحيح lpp;

    // Calculate lpp
    lpp = (صحيح)((pctx.bottom_margin - pctx.بالاترین_margin) / pctx.char_ارتفاع);

    // Adjust top margin if there is a header
    pctx.بالاترین_margin += pctx.char_ارتفاع * prt_سرآیند_ارتفاع();

    أرجع lpp - prt_سرآیند_ارتفاع();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    ساكن PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    إذا (name == عدم)
	أرجع عدم;

    font = pango_font_description_from_string((ثابت حرف *)name);
    إذا (font == عدم)
    {
	semsg(_(e_unknown_printer_font_str), name);
	أرجع عدم;
    }

    // Ensure a size is set
    إذا (pango_font_description_get_اندازه(font) <= 0)
	pango_font_description_مجموعه_اندازه(font, 10 * PANGO_SCALE);

    أرجع font;
}

    صحيح
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	صحيح		forceit استفادهنشده)
{
    char_u	*paper_name;
    صحيح		paper_strlen;
    bool	portrait;
    صحيح		media;
    char_u	*filename = عدم;
#اگر_تعریف_شده USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    إذا (gui.درون_use && !forceit && psettings->outfile == عدم)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.صفحه_عرض, &pctx.صفحه_ارتفاع);

	إذا (filename == عدم)
	    أرجع FAIL;
	dialog = true;
    }
#پایان_اگر

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#اگر_تعریف_شده USE_GTK4_PRINT_DIALOG
    إذا (!dialog)
#پایان_اگر
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	إذا (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].طول_رشته;
	}
	وإلا
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	لكل (media = 0; media < (صحيح)PRT_MEDIASIZE_LEN; ++media)
	    إذا ((صحيح)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		اكسر;
	إذا (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	إذا (portrait)
	{
	    pctx.صفحه_عرض = prt_mediasize[media].عرض;
	    pctx.صفحه_ارتفاع = prt_mediasize[media].ارتفاع;
	}
	وإلا
	{
	    pctx.صفحه_عرض = prt_mediasize[media].ارتفاع;
	    pctx.صفحه_ارتفاع = prt_mediasize[media].عرض;
	}

	إذا (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    لكل (صحيح i = 0; i < (صحيح)ARRAY_درازی(print_formats); i++)
		إذا (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].طول_رشته) == 0)
		{
		    pctx.format = i;
		    اكسر;
		}
	}
    }
    psettings->user_abort = FALSE;

#اگر_تعریف_شده USE_GTK4_PRINT_DIALOG
    إذا (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#پایان_اگر

    إذا (filename == عدم)
    {
	// If the user didn't specify a file name, use a temp file.
	إذا (psettings->outfile == عدم)
	{
	    filename = vim_tempname('p', TRUE);
	    إذا (filename == عدم)
	    {
		emsg(_(e_cant_get_موقت_file_name));
		أرجع FAIL;
	    }
	}
	وإلا
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    إذا (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ثابت حرف *)filename,
		pctx.صفحه_عرض, pctx.صفحه_ارتفاع);
    وإلا
	pctx.surface = cairo_pdf_surface_create((ثابت حرف *)filename,
		pctx.صفحه_عرض, pctx.صفحه_ارتفاع);

    pctx.filename = (char_u *)filename;

    إذا (cairo_surface_وضعیت(pctx.surface) != CAIRO_وضعیت_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	أرجع FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.متن_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    إذا (pctx.font == عدم)
    {
	mch_print_cleanup();
	أرجع FAIL;
    }

    pango_context_مجموعه_font_description(pctx.متن_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_مجموعه_resolution(pctx.متن_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	صحيح		    ascent;
	صحيح		    descent;

	layout = pango_layout_new(pctx.متن_context);
	pango_layout_مجموعه_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, عدم);

	pctx.char_عرض =
	    (مزدوج)pango_font_metrics_get_approximate_char_عرض(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (مزدوج)ascent / PANGO_SCALE;
	pctx.char_ارتفاع = (مزدوج)(ascent + descent) / PANGO_SCALE;

	pctx.سطر_spacing = (مزدوج)
	    (pango_font_metrics_get_ارتفاع(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (مزدوج)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (مزدوج)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	مزدوج  چپ_2;
	مزدوج	راست_2;
	مزدوج	بالاترین;
	مزدوج	bottom;

	get_صفحه_margins(pctx.صفحه_عرض, pctx.صفحه_ارتفاع, &چپ_2, &راست_2, &بالاترین,
		&bottom);
	pctx.چپ_margin = چپ_2;
	pctx.راست_margin = راست_2;
	pctx.بالاترین_margin = بالاترین;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_در_هر_سطر = get_cpl();
    psettings->سطرها_در_هر_صفحه = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#اگر_تعریف_شده USE_GTK4_PRINT_DIALOG
    إذا (!dialog)
#پایان_اگر
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	إذا (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    إذا (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    وإلا إذا (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#اگر_تعریف_شده USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#پایان_اگر

    أرجع OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    ساكن فراغ
emit_dsc_comment(حرف *comment, حرف *fmt, ...)
{
    ساكن حرف buf[257];

    va_list ap;
    صحيح	    len = حجم(buf);
    صحيح	    o;
    حرف    *s = buf;

    إذا (fmt == عدم)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	اذهب خروج;
    }
    وإلا
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    إذا (len <= 0)
	أرجع;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

خروج:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    صحيح
mch_print_begin(prt_settings_T *psettings)
{
    حرف user[256];

    pctx.cur_x = 0.0;
    pctx.cur_سطر = 0;
    pctx.draw_flags = 0;

#اگر_تعریف_شده USE_GTK4_PRINT_DIALOG
    إذا (pctx.dialog)
	أرجع TRUE;
#پایان_اگر

    إذا (!get_user_name((char_u *)user, حجم(user)))
	STRCPY(user, "Unknown");

    إذا (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (حرف *)psettings->jobname);
	emit_dsc_comment("For", user);
	emit_dsc_comment("Orientation",
		pctx.portrait ? "Portrait" : "Landscape");
	emit_dsc_comment("PageOrder", "Ascend");

	emit_dsc_comment("DocumentMedia",
		"%s %.2lf %.2lf 0 () ()",
		prt_mediasize[pctx.media].name,
		prt_mediasize[pctx.media].عرض,
		prt_mediasize[pctx.media].ارتفاع);

	emit_dsc_comment("Requirements", "%s%s %s %s",
		pctx.duplex ? "duplex" : "",
		pctx.tumble ? "(tumble)" : "",
		pctx.collate ? "collate" : "",
		psettings->do_syntax ? "color" : "");

	// In defaults section (defaults for every page)
	emit_dsc_comment("BeginDefaults", عدم);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", عدم);
    }
    وإلا إذا (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	إذا (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, عدم);

	cairo_pdf_surface_مجموعه_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (حرف *)str);
	إذا (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	إذا (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, عدم);
	cairo_pdf_surface_مجموعه_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (حرف *)str);
	إذا (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_مجموعه_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_نسخه_LONG);
    }

    أرجع TRUE;
}

    فراغ
mch_print_انتها(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_پایان(pctx.surface);

    إذا (psettings->outfile == عدم && !got_int && !psettings->user_abort
	    && pctx.filename != عدم
#اگر_تعریف_شده USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#پایان_اگر
       )
    {
	پیام(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	إذا (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_مقصد_print_postscript_file));
	وإلا
	    پیام(_("Print job sent."));
    }

    mch_print_cleanup();
}

    صحيح
mch_print_انتها_صفحه(فراغ)
{
    cairo_show_صفحه(pctx.cr);
    أرجع TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    صحيح
mch_print_begin_صفحه(char_u *str استفادهنشده)
{
    أرجع TRUE;
}

    صحيح
mch_print_blank_صفحه(فراغ)
{
    mch_print_انتها_صفحه();
    أرجع TRUE;
}

    فراغ
mch_print_شروع_سطر(صحيح margin, صحيح صفحه_سطر)
{
    pctx.cur_x = pctx.چپ_margin;
    إذا (margin)
	pctx.cur_x -= pctx.عدد_عرض;

    pctx.cur_سطر = صفحه_سطر;
}

    ساكن فراغ
long_مقصد_rgb(long_u c, مزدوج *r, مزدوج *g, مزدوج *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    صحيح
mch_print_متن_بیرون(char_u *textp, صحيح len)
{
    PangoAttrList	*attr_list;
    GList		*قلم_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    مزدوج		r, g, b;
    مزدوج		عرض = 0;
    مزدوج		بعدی_x;

    // Always convert to UTF-8, Pango expects that
    إذا (output_conv.vc_type == CONV_NONE)
	str = textp;
    وإلا
	str = string_convert(&output_conv, textp, &len);

    إذا (str == عدم)
	أرجع TRUE;

    attr_list = pango_attr_list_new();

#تعریف_کن درج_PANGO_ATTR(Attribute, AttrList, شروع_2, انتها_2)  \
    G_STMT_شروع{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->شروع_شاخص = (شروع_2);		    \
	tmp_attr_->انتها_شاخص = (انتها_2);			    \
	pango_attr_list_درج((AttrList), tmp_attr_);	    \
    }G_STMT_انتها

    // Underline attribute does not work with pango_itemize().
    إذا ((pctx.draw_flags & PRT_DRAW_BOLD))
	درج_PANGO_ATTR(pango_attr_وزن_new(PANGO_وزن_BOLD),
		attr_list, 0, len);
    إذا (pctx.draw_flags & PRT_DRAW_ITALIC)
	درج_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#لغو_تعریف درج_PANGO_ATTR

    قلم_list = pango_itemize(pctx.متن_context,
	    (ثابت حرف *)str, 0, len, attr_list, عدم);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    طالما (قلم_list != عدم)
    {
	Pango_قلم   *قلم;
	مزدوج	    o;

	قلم = (Pango_قلم *)قلم_list->داده;
	قلم_list = g_list_delete_link(قلم_list, قلم_list);

	pango_shape_پر((ثابت حرف *)str + قلم->جابه_جایی, قلم->درازی,
		(ثابت حرف *)str, len, &قلم->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (مزدوج)pango_glyph_string_get_عرض(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	إذا (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_مقصد_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_مجموعه_منبع_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + عرض,
		    GET_Y(pctx.cur_سطر) - pctx.سطر_spacing, o, pctx.char_ارتفاع);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_مقصد_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_مجموعه_منبع_rgb(pctx.cr, r, g, b);
	cairo_move_مقصد(pctx.cr, pctx.cur_x + عرض, GET_متن_Y(pctx.cur_سطر));
	pango_cairo_show_glyph_string(pctx.cr, قلم->analysis.font, glyphs);

	pango_قلم_free(قلم);
	عرض += o;
    }

    // Draw underline if needed
    إذا (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	مزدوج y = GET_متن_Y(pctx.cur_سطر) - pctx.ul_pos;

	long_مقصد_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_مجموعه_منبع_rgb(pctx.cr, r, g, b);
	cairo_مجموعه_سطر_عرض(pctx.cr, pctx.ul_thickness);
	cairo_move_مقصد(pctx.cr, pctx.cur_x, y);
	cairo_سطر_مقصد(pctx.cr, pctx.cur_x + عرض, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += عرض;

    بعدی_x = pctx.cur_x + pctx.char_عرض;

    إذا (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    أرجع (بعدی_x > pctx.راست_margin) &&
	((بعدی_x - pctx.راست_margin) > (pctx.راست_margin * 1e-5));
}

    فراغ
mch_print_مجموعه_font(صحيح bold, صحيح italic, صحيح underline)
{
    pctx.draw_flags = 0;

    إذا (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    إذا (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    إذا (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    فراغ
mch_print_مجموعه_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    فراغ
mch_print_مجموعه_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    فراغ
mch_print_cleanup(فراغ)
{
    g_clear_اشاره_گر(&pctx.cr, cairo_destroy);
    g_clear_اشاره_گر(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.متن_context);
    g_clear_اشاره_گر(&pctx.font, pango_font_description_free);

#اگر_تعریف_شده USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    إذا (pctx.dialog)
	gui_gtk4_print_پایان();
#پایان_اگر

    g_clear_اشاره_گر(&pctx.filename, vim_free);
}

#پایان_اگر // FEAT_PRINT_PANGO
