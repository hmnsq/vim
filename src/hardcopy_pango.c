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

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "version.h"

#ifdef FEAT_PRINT_PANGO

#অন্তর্ভাব "hardcopy.h"

#অন্তর্ভাব <cairo/cairo-ps.h>
#অন্তর্ভাব <cairo/cairo-pdf.h>
#অন্তর্ভাব <pango/pango.h>
#অন্তর্ভাব <pango/pangocairo.h>

#define PRINT_DPI 72.0

#define PRT_DRAW_BOLD 1
#define PRT_DRAW_ITALIC 2
#define PRT_DRAW_UNDERLINE 4

#define GET_Y(l) (pctx.শীর্ষ_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

প্রকারলেখ গণনা
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
স্থির কাঠামো
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    পূর্ণসংখ্যা media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    দ্বিগুণ char_width;
    দ্বিগুণ char_ascent;
    দ্বিগুণ char_height;
    দ্বিগুণ সারি_spacing;
    দ্বিগুণ ul_pos;
    দ্বিগুণ ul_thickness;

    দ্বিগুণ page_width;
    দ্বিগুণ page_height;

    দ্বিগুণ বাম_margin;
    দ্বিগুণ ডান_margin;
    দ্বিগুণ শীর্ষ_margin;
    দ্বিগুণ bottom_margin;

    দ্বিগুণ সংখ্যা_width;

    // Current position to draw from
    দ্বিগুণ cur_x;
    পূর্ণসংখ্যা cur_সারি;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

স্থির ধ্রুবক অক্ষর *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    স্থির দ্বিগুণ
to_device_units(পূর্ণসংখ্যা idx, দ্বিগুণ physsize, পূর্ণসংখ্যা def_সংখ্যা)
{
    দশমিক	ret;
    পূর্ণসংখ্যা		u;
    পূর্ণসংখ্যা		nr;

    u = prt_get_unit(idx);
    যদি (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_সংখ্যা;
    }
    অন্যথা
	nr = printer_opts[idx].সংখ্যা;

    নির্বাচন (u)
    {
	ক্ষেত্র PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    বিরতি;
	ক্ষেত্র PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    বিরতি;
	ক্ষেত্র PRT_UNIT_POINT:
	    ret = (দ্বিগুণ)nr;
	    বিরতি;
	ক্ষেত্র PRT_UNIT_PERC:
	পূর্বনির্ধারিত:
	    ret = physsize * (nr / 100.0);
	    বিরতি;
    }

    ফেরত ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    স্থির শূন্য
get_page_margins(
	দ্বিগুণ	width,
	দ্বিগুণ	height,
	দ্বিগুণ	*বাম,
	দ্বিগুণ	*ডান,
	দ্বিগুণ	*শীর্ষ_2,
	দ্বিগুণ	*bottom)
{
    *বাম = to_device_units(OPT_PRINT_বাম, width, 10);
    *ডান = width - to_device_units(OPT_PRINT_ডান, width, 5);

    *শীর্ষ_2 = to_device_units(OPT_PRINT_শীর্ষ, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    স্থির পূর্ণসংখ্যা
get_cpl(শূন্য)
{
    যদি (prt_use_সংখ্যা())
    {
	pctx.সংখ্যা_width = PRINT_সংখ্যা_WIDTH * pctx.char_width;
	pctx.বাম_margin += pctx.সংখ্যা_width;
    }
    অন্যথা
	pctx.সংখ্যা_width = 0.0;

    ফেরত (পূর্ণসংখ্যা)((pctx.ডান_margin - pctx.বাম_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    স্থির পূর্ণসংখ্যা
get_lpp(শূন্য)
{
    পূর্ণসংখ্যা lpp;

    // Calculate lpp
    lpp = (পূর্ণসংখ্যা)((pctx.bottom_margin - pctx.শীর্ষ_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.শীর্ষ_margin += pctx.char_height * prt_header_height();

    ফেরত lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    স্থির PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    যদি (name == ফাঁকা)
	ফেরত ফাঁকা;

    font = pango_font_description_from_string((ধ্রুবক অক্ষর *)name);
    যদি (font == ফাঁকা)
    {
	semsg(_(e_unknown_printer_font_str), name);
	ফেরত ফাঁকা;
    }

    // Ensure a size is set
    যদি (pango_font_description_get_আকার(font) <= 0)
	pango_font_description_set_আকার(font, 10 * PANGO_SCALE);

    ফেরত font;
}

    পূর্ণসংখ্যা
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	পূর্ণসংখ্যা		forceit UNUSED)
{
    char_u	*paper_name;
    পূর্ণসংখ্যা		paper_strlen;
    bool	portrait;
    পূর্ণসংখ্যা		media;
    char_u	*filename = ফাঁকা;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    যদি (gui.in_use && !forceit && psettings->outfile == ফাঁকা)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	যদি (filename == ফাঁকা)
	    ফেরত FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    যদি (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	যদি (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
	}
	অন্যথা
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	জন্য (media = 0; media < (পূর্ণসংখ্যা)PRT_MEDIASIZE_LEN; ++media)
	    যদি ((পূর্ণসংখ্যা)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		বিরতি;
	যদি (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	যদি (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	অন্যথা
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	যদি (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    জন্য (পূর্ণসংখ্যা i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(print_formats); i++)
		যদি (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    বিরতি;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    যদি (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    যদি (filename == ফাঁকা)
    {
	// If the user didn't specify a file name, use a temp file.
	যদি (psettings->outfile == ফাঁকা)
	{
	    filename = vim_tempname('p', TRUE);
	    যদি (filename == ফাঁকা)
	    {
		emsg(_(e_cant_get_অস্থায়ী_file_name));
		ফেরত FAIL;
	    }
	}
	অন্যথা
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    যদি (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ধ্রুবক অক্ষর *)filename,
		pctx.page_width, pctx.page_height);
    অন্যথা
	pctx.surface = cairo_pdf_surface_create((ধ্রুবক অক্ষর *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    যদি (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ফেরত FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    যদি (pctx.font == ফাঁকা)
    {
	mch_print_cleanup();
	ফেরত FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	পূর্ণসংখ্যা		    ascent;
	পূর্ণসংখ্যা		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, ফাঁকা);

	pctx.char_width =
	    (দ্বিগুণ)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (দ্বিগুণ)ascent / PANGO_SCALE;
	pctx.char_height = (দ্বিগুণ)(ascent + descent) / PANGO_SCALE;

	pctx.সারি_spacing = (দ্বিগুণ)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (দ্বিগুণ)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (দ্বিগুণ)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	দ্বিগুণ  বাম;
	দ্বিগুণ	ডান;
	দ্বিগুণ	শীর্ষ_2;
	দ্বিগুণ	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &বাম, &ডান, &শীর্ষ_2,
		&bottom);
	pctx.বাম_margin = বাম;
	pctx.ডান_margin = ডান;
	pctx.শীর্ষ_margin = শীর্ষ_2;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_সারি = get_cpl();
    psettings->সারিগুলি_per_page = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ifdef USE_GTK4_PRINT_DIALOG
    যদি (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	যদি (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    যদি (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    অন্যথা যদি (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    ফেরত OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    স্থির শূন্য
emit_dsc_comment(অক্ষর *comment, অক্ষর *fmt, ...)
{
    স্থির অক্ষর buf[257];

    va_list ap;
    পূর্ণসংখ্যা	    len = আকার(buf);
    পূর্ণসংখ্যা	    o;
    অক্ষর    *s = buf;

    যদি (fmt == ফাঁকা)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	যাও exit;
    }
    অন্যথা
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    যদি (len <= 0)
	ফেরত;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    পূর্ণসংখ্যা
mch_print_begin(prt_settings_T *psettings)
{
    অক্ষর user[256];

    pctx.cur_x = 0.0;
    pctx.cur_সারি = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    যদি (pctx.dialog)
	ফেরত TRUE;
#endif

    যদি (!get_user_name((char_u *)user, আকার(user)))
	STRCPY(user, "Unknown");

    যদি (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (অক্ষর *)psettings->jobname);
	emit_dsc_comment("For", user);
	emit_dsc_comment("Orientation",
		pctx.portrait ? "Portrait" : "Landscape");
	emit_dsc_comment("PageOrder", "Ascend");

	emit_dsc_comment("DocumentMedia",
		"%s %.2lf %.2lf 0 () ()",
		prt_mediasize[pctx.media].name,
		prt_mediasize[pctx.media].width,
		prt_mediasize[pctx.media].height);

	emit_dsc_comment("Requirements", "%s%s %s %s",
		pctx.duplex ? "duplex" : "",
		pctx.tumble ? "(tumble)" : "",
		pctx.collate ? "collate" : "",
		psettings->do_syntax ? "color" : "");

	// In defaults section (defaults for every page)
	emit_dsc_comment("BeginDefaults", ফাঁকা);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", ফাঁকা);
    }
    অন্যথা যদি (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	যদি (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ফাঁকা);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (অক্ষর *)str);
	যদি (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	যদি (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ফাঁকা);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (অক্ষর *)str);
	যদি (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    ফেরত TRUE;
}

    শূন্য
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    যদি (psettings->outfile == ফাঁকা && !got_int && !psettings->user_abort
	    && pctx.filename != ফাঁকা
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	বার্তা(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	যদি (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	অন্যথা
	    বার্তা(_("Print job sent."));
    }

    mch_print_cleanup();
}

    পূর্ণসংখ্যা
mch_print_end_page(শূন্য)
{
    cairo_show_page(pctx.cr);
    ফেরত TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    পূর্ণসংখ্যা
mch_print_begin_page(char_u *str UNUSED)
{
    ফেরত TRUE;
}

    পূর্ণসংখ্যা
mch_print_blank_page(শূন্য)
{
    mch_print_end_page();
    ফেরত TRUE;
}

    শূন্য
mch_print_start_সারি(পূর্ণসংখ্যা margin, পূর্ণসংখ্যা page_সারি)
{
    pctx.cur_x = pctx.বাম_margin;
    যদি (margin)
	pctx.cur_x -= pctx.সংখ্যা_width;

    pctx.cur_সারি = page_সারি;
}

    স্থির শূন্য
long_to_rgb(long_u c, দ্বিগুণ *r, দ্বিগুণ *g, দ্বিগুণ *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    পূর্ণসংখ্যা
mch_print_text_out(char_u *textp, পূর্ণসংখ্যা len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    দ্বিগুণ		r, g, b;
    দ্বিগুণ		width = 0;
    দ্বিগুণ		পরবর্তী_x;

    // Always convert to UTF-8, Pango expects that
    যদি (output_conv.vc_type == CONV_NONE)
	str = textp;
    অন্যথা
	str = string_convert(&output_conv, textp, &len);

    যদি (str == ফাঁকা)
	ফেরত TRUE;

    attr_list = pango_attr_list_new();

#define ঢোকাও_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_সূচক = (Start);		    \
	tmp_attr_->end_সূচক = (End);			    \
	pango_attr_list_ঢোকাও((AttrList), tmp_attr_);	    \
    }G_STMT_END

    // Underline attribute does not work with pango_itemize().
    যদি ((pctx.draw_flags & PRT_DRAW_BOLD))
	ঢোকাও_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    যদি (pctx.draw_flags & PRT_DRAW_ITALIC)
	ঢোকাও_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef ঢোকাও_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (ধ্রুবক অক্ষর *)str, 0, len, attr_list, ফাঁকা);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    যখন (item_list != ফাঁকা)
    {
	PangoItem   *item;
	দ্বিগুণ	    o;

	item = (PangoItem *)item_list->তথ্য;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((ধ্রুবক অক্ষর *)str + item->offset, item->দৈর্ঘ্য,
		(ধ্রুবক অক্ষর *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (দ্বিগুণ)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	যদি (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_সারি) - pctx.সারি_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_সারি));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    যদি (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	দ্বিগুণ y = GET_TEXT_Y(pctx.cur_সারি) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_সারি_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_সারি_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    পরবর্তী_x = pctx.cur_x + pctx.char_width;

    যদি (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    ফেরত (পরবর্তী_x > pctx.ডান_margin) &&
	((পরবর্তী_x - pctx.ডান_margin) > (pctx.ডান_margin * 1e-5));
}

    শূন্য
mch_print_set_font(পূর্ণসংখ্যা bold, পূর্ণসংখ্যা italic, পূর্ণসংখ্যা underline)
{
    pctx.draw_flags = 0;

    যদি (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    যদি (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    যদি (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    শূন্য
mch_print_set_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    শূন্য
mch_print_set_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    শূন্য
mch_print_cleanup(শূন্য)
{
    g_clear_pointer(&pctx.cr, cairo_destroy);
    g_clear_pointer(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.text_context);
    g_clear_pointer(&pctx.font, pango_font_description_free);

#ifdef USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    যদি (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
