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

#که_تعریف_شوی FEAT_PRINT_PANGO

#ضمن "hardcopy.h"

#ضمن <cairo/cairo-ps.h>
#ضمن <cairo/cairo-pdf.h>
#ضمن <pango/pango.h>
#ضمن <pango/pangocairo.h>

#تعریف_کړه PRINT_DPI 72.0

#تعریف_کړه PRT_DRAW_BOLD 1
#تعریف_کړه PRT_DRAW_ITALIC 2
#تعریف_کړه PRT_DRAW_UNDERLINE 4

#تعریف_کړه GET_Y(l) (pctx.سر_margin + (l) * pctx.char_لوړوالی)
#تعریف_کړه GET_متن_Y(l) (GET_Y(l) + pctx.char_ascent)

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

    مزدوج char_پلنوالی;
    مزدوج char_ascent;
    مزدوج char_لوړوالی;
    مزدوج کرښه_spacing;
    مزدوج ul_pos;
    مزدوج ul_thickness;

    مزدوج پاڼه_پلنوالی;
    مزدوج پاڼه_لوړوالی;

    مزدوج کیڼ_margin;
    مزدوج ښی_margin;
    مزدوج سر_margin;
    مزدوج bottom_margin;

    مزدوج شمېره_پلنوالی;

    // Current position to draw from
    مزدوج cur_x;
    صحيح cur_کرښه;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#که_تعریف_شوی USE_GTK4_PRINT_DIALOG
    bool dialog;
#د_که_پای
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
منزل_device_units(صحيح idx, مزدوج physsize, صحيح def_شمېره)
{
    عائم	ret;
    صحيح		u;
    صحيح		nr;

    u = prt_get_unit(idx);
    إذا (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_شمېره;
    }
    وإلا
	nr = printer_opts[idx].شمېره;

    بدل (u)
    {
	حالة PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    اكسر;
	حالة PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    اكسر;
	حالة PRT_UNIT_ټکی:
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
get_پاڼه_margins(
	مزدوج	پلنوالی,
	مزدوج	لوړوالی,
	مزدوج	*کیڼ_2,
	مزدوج	*ښی_2,
	مزدوج	*سر_2,
	مزدوج	*bottom)
{
    *کیڼ_2 = منزل_device_units(OPT_PRINT_کیڼ, پلنوالی, 10);
    *ښی_2 = پلنوالی - منزل_device_units(OPT_PRINT_ښی, پلنوالی, 5);

    *سر_2 = منزل_device_units(OPT_PRINT_سر, لوړوالی, 5);
    *bottom = لوړوالی - منزل_device_units(OPT_PRINT_BOT, لوړوالی, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    ساكن صحيح
get_cpl(فراغ)
{
    إذا (prt_use_شمېره())
    {
	pctx.شمېره_پلنوالی = PRINT_شمېره_پلنوالی * pctx.char_پلنوالی;
	pctx.کیڼ_margin += pctx.شمېره_پلنوالی;
    }
    وإلا
	pctx.شمېره_پلنوالی = 0.0;

    أرجع (صحيح)((pctx.ښی_margin - pctx.کیڼ_margin) / pctx.char_پلنوالی);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ساكن صحيح
get_lpp(فراغ)
{
    صحيح lpp;

    // Calculate lpp
    lpp = (صحيح)((pctx.bottom_margin - pctx.سر_margin) / pctx.char_لوړوالی);

    // Adjust top margin if there is a header
    pctx.سر_margin += pctx.char_لوړوالی * prt_سرلیک_لوړوالی();

    أرجع lpp - prt_سرلیک_لوړوالی();
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
	pango_font_description_سېټ_اندازه(font, 10 * PANGO_SCALE);

    أرجع font;
}

    صحيح
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	صحيح		forceit ناکارول_شوی)
{
    char_u	*paper_name;
    صحيح		paper_strlen;
    bool	portrait;
    صحيح		media;
    char_u	*filename = عدم;
#که_تعریف_شوی USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    إذا (gui.دننه_use && !forceit && psettings->outfile == عدم)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.پاڼه_پلنوالی, &pctx.پاڼه_لوړوالی);

	إذا (filename == عدم)
	    أرجع FAIL;
	dialog = true;
    }
#د_که_پای

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#که_تعریف_شوی USE_GTK4_PRINT_DIALOG
    إذا (!dialog)
#د_که_پای
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	إذا (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].مزي_اوږدوالی;
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
	    pctx.پاڼه_پلنوالی = prt_mediasize[media].پلنوالی;
	    pctx.پاڼه_لوړوالی = prt_mediasize[media].لوړوالی;
	}
	وإلا
	{
	    pctx.پاڼه_پلنوالی = prt_mediasize[media].لوړوالی;
	    pctx.پاڼه_لوړوالی = prt_mediasize[media].پلنوالی;
	}

	إذا (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    لكل (صحيح i = 0; i < (صحيح)ARRAY_اوږدتیا(print_formats); i++)
		إذا (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].مزي_اوږدوالی) == 0)
		{
		    pctx.format = i;
		    اكسر;
		}
	}
    }
    psettings->user_abort = FALSE;

#که_تعریف_شوی USE_GTK4_PRINT_DIALOG
    إذا (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#د_که_پای

    إذا (filename == عدم)
    {
	// If the user didn't specify a file name, use a temp file.
	إذا (psettings->outfile == عدم)
	{
	    filename = vim_tempname('p', TRUE);
	    إذا (filename == عدم)
	    {
		emsg(_(e_cant_get_لنډمهاله_file_name));
		أرجع FAIL;
	    }
	}
	وإلا
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    إذا (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ثابت حرف *)filename,
		pctx.پاڼه_پلنوالی, pctx.پاڼه_لوړوالی);
    وإلا
	pctx.surface = cairo_pdf_surface_create((ثابت حرف *)filename,
		pctx.پاڼه_پلنوالی, pctx.پاڼه_لوړوالی);

    pctx.filename = (char_u *)filename;

    إذا (cairo_surface_حالت(pctx.surface) != CAIRO_حالت_SUCCESS)
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

    pango_context_سېټ_font_description(pctx.متن_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_سېټ_resolution(pctx.متن_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	صحيح		    ascent;
	صحيح		    descent;

	layout = pango_layout_new(pctx.متن_context);
	pango_layout_سېټ_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, عدم);

	pctx.char_پلنوالی =
	    (مزدوج)pango_font_metrics_get_approximate_char_پلنوالی(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (مزدوج)ascent / PANGO_SCALE;
	pctx.char_لوړوالی = (مزدوج)(ascent + descent) / PANGO_SCALE;

	pctx.کرښه_spacing = (مزدوج)
	    (pango_font_metrics_get_لوړوالی(metrics) - (ascent + descent))
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
	مزدوج  کیڼ_2;
	مزدوج	ښی_2;
	مزدوج	سر_2;
	مزدوج	bottom;

	get_پاڼه_margins(pctx.پاڼه_پلنوالی, pctx.پاڼه_لوړوالی, &کیڼ_2, &ښی_2, &سر_2,
		&bottom);
	pctx.کیڼ_margin = کیڼ_2;
	pctx.ښی_margin = ښی_2;
	pctx.سر_margin = سر_2;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_فی_کرښه = get_cpl();
    psettings->کرښې_فی_پاڼه = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#که_تعریف_شوی USE_GTK4_PRINT_DIALOG
    إذا (!dialog)
#د_که_پای
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

#که_تعریف_شوی USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#د_که_پای

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
	اذهب وتل;
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

وتل:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    صحيح
mch_print_begin(prt_settings_T *psettings)
{
    حرف user[256];

    pctx.cur_x = 0.0;
    pctx.cur_کرښه = 0;
    pctx.draw_flags = 0;

#که_تعریف_شوی USE_GTK4_PRINT_DIALOG
    إذا (pctx.dialog)
	أرجع TRUE;
#د_که_پای

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
		prt_mediasize[pctx.media].پلنوالی,
		prt_mediasize[pctx.media].لوړوالی);

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

	cairo_pdf_surface_سېټ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (حرف *)str);
	إذا (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	إذا (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, عدم);
	cairo_pdf_surface_سېټ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (حرف *)str);
	إذا (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_سېټ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_نسخه_LONG);
    }

    أرجع TRUE;
}

    فراغ
mch_print_پای_ټکی(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_پای(pctx.surface);

    إذا (psettings->outfile == عدم && !got_int && !psettings->user_abort
	    && pctx.filename != عدم
#که_تعریف_شوی USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#د_که_پای
       )
    {
	پیغام(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	إذا (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_منزل_print_postscript_file));
	وإلا
	    پیغام(_("Print job sent."));
    }

    mch_print_cleanup();
}

    صحيح
mch_print_پای_ټکی_پاڼه(فراغ)
{
    cairo_show_پاڼه(pctx.cr);
    أرجع TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    صحيح
mch_print_begin_پاڼه(char_u *str ناکارول_شوی)
{
    أرجع TRUE;
}

    صحيح
mch_print_blank_پاڼه(فراغ)
{
    mch_print_پای_ټکی_پاڼه();
    أرجع TRUE;
}

    فراغ
mch_print_پیل_کرښه(صحيح margin, صحيح پاڼه_کرښه)
{
    pctx.cur_x = pctx.کیڼ_margin;
    إذا (margin)
	pctx.cur_x -= pctx.شمېره_پلنوالی;

    pctx.cur_کرښه = پاڼه_کرښه;
}

    ساكن فراغ
long_منزل_rgb(long_u c, مزدوج *r, مزدوج *g, مزدوج *b)
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
mch_print_متن_بهر(char_u *textp, صحيح len)
{
    PangoAttrList	*attr_list;
    GList		*توکی_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    مزدوج		r, g, b;
    مزدوج		پلنوالی = 0;
    مزدوج		بل_x;

    // Always convert to UTF-8, Pango expects that
    إذا (output_conv.vc_type == CONV_NONE)
	str = textp;
    وإلا
	str = string_convert(&output_conv, textp, &len);

    إذا (str == عدم)
	أرجع TRUE;

    attr_list = pango_attr_list_new();

#تعریف_کړه ورننباسه_PANGO_ATTR(Attribute, AttrList, پیل_2, پای_ټکی_2)  \
    G_STMT_پیل{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->پیل_شاخص = (پیل_2);		    \
	tmp_attr_->پای_ټکی_شاخص = (پای_ټکی_2);			    \
	pango_attr_list_ورننباسه((AttrList), tmp_attr_);	    \
    }G_STMT_پای_ټکی

    // Underline attribute does not work with pango_itemize().
    إذا ((pctx.draw_flags & PRT_DRAW_BOLD))
	ورننباسه_PANGO_ATTR(pango_attr_وزن_new(PANGO_وزن_BOLD),
		attr_list, 0, len);
    إذا (pctx.draw_flags & PRT_DRAW_ITALIC)
	ورننباسه_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#تعریف_لغوه ورننباسه_PANGO_ATTR

    توکی_list = pango_itemize(pctx.متن_context,
	    (ثابت حرف *)str, 0, len, attr_list, عدم);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    طالما (توکی_list != عدم)
    {
	Pango_توکی   *توکی;
	مزدوج	    o;

	توکی = (Pango_توکی *)توکی_list->معلومات;
	توکی_list = g_list_delete_link(توکی_list, توکی_list);

	pango_shape_ډک((ثابت حرف *)str + توکی->دننه_واټن, توکی->اوږدتیا,
		(ثابت حرف *)str, len, &توکی->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (مزدوج)pango_glyph_string_get_پلنوالی(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	إذا (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_منزل_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_سېټ_سرچینه_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + پلنوالی,
		    GET_Y(pctx.cur_کرښه) - pctx.کرښه_spacing, o, pctx.char_لوړوالی);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_منزل_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_سېټ_سرچینه_rgb(pctx.cr, r, g, b);
	cairo_move_منزل(pctx.cr, pctx.cur_x + پلنوالی, GET_متن_Y(pctx.cur_کرښه));
	pango_cairo_show_glyph_string(pctx.cr, توکی->analysis.font, glyphs);

	pango_توکی_free(توکی);
	پلنوالی += o;
    }

    // Draw underline if needed
    إذا (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	مزدوج y = GET_متن_Y(pctx.cur_کرښه) - pctx.ul_pos;

	long_منزل_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_سېټ_سرچینه_rgb(pctx.cr, r, g, b);
	cairo_سېټ_کرښه_پلنوالی(pctx.cr, pctx.ul_thickness);
	cairo_move_منزل(pctx.cr, pctx.cur_x, y);
	cairo_کرښه_منزل(pctx.cr, pctx.cur_x + پلنوالی, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += پلنوالی;

    بل_x = pctx.cur_x + pctx.char_پلنوالی;

    إذا (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    أرجع (بل_x > pctx.ښی_margin) &&
	((بل_x - pctx.ښی_margin) > (pctx.ښی_margin * 1e-5));
}

    فراغ
mch_print_سېټ_font(صحيح bold, صحيح italic, صحيح underline)
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
mch_print_سېټ_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    فراغ
mch_print_سېټ_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    فراغ
mch_print_cleanup(فراغ)
{
    g_clear_اشاره_ګر(&pctx.cr, cairo_destroy);
    g_clear_اشاره_ګر(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.متن_context);
    g_clear_اشاره_ګر(&pctx.font, pango_font_description_free);

#که_تعریف_شوی USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    إذا (pctx.dialog)
	gui_gtk4_print_پای();
#د_که_پای

    g_clear_اشاره_ګر(&pctx.filename, vim_free);
}

#د_که_پای // FEAT_PRINT_PANGO
