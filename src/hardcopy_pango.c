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

#অন্তর্ভাৱ "vim.h"
#অন্তর্ভাৱ "version.h"

#ifdef FEAT_PRINT_PANGO

#অন্তর্ভাৱ "hardcopy.h"

#অন্তর্ভাৱ <cairo/cairo-ps.h>
#অন্তর্ভাৱ <cairo/cairo-pdf.h>
#অন্তর্ভাৱ <pango/pango.h>
#অন্তর্ভাৱ <pango/pangocairo.h>

#define PRINT_DPI 72.0

#define PRT_DRAW_BOLD 1
#define PRT_DRAW_ITALIC 2
#define PRT_DRAW_UNDERLINE 4

#define GET_Y(l) (pctx.শীৰ্ষ_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

প্রকারনির্দেশ গণনা
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
স্থির সংরচনা
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    পূর্ণাঙ্ক media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    দ্ৱিগুণ char_width;
    দ্ৱিগুণ char_ascent;
    দ্ৱিগুণ char_height;
    দ্ৱিগুণ সাৰি_spacing;
    দ্ৱিগুণ ul_pos;
    দ্ৱিগুণ ul_thickness;

    দ্ৱিগুণ page_width;
    দ্ৱিগুণ page_height;

    দ্ৱিগুণ বাম_margin;
    দ্ৱিগুণ ডান_margin;
    দ্ৱিগুণ শীৰ্ষ_margin;
    দ্ৱিগুণ bottom_margin;

    দ্ৱিগুণ সংখ্যা_width;

    // Current position to draw from
    দ্ৱিগুণ cur_x;
    পূর্ণাঙ্ক cur_সাৰি;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

স্থির ধ্রুৱ ৱর্ণ *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    স্থির দ্ৱিগুণ
to_device_units(পূর্ণাঙ্ক idx, দ্ৱিগুণ physsize, পূর্ণাঙ্ক def_সংখ্যা)
{
    ভিন্ন	ret;
    পূর্ণাঙ্ক		u;
    পূর্ণাঙ্ক		nr;

    u = prt_get_unit(idx);
    যদি (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_সংখ্যা;
    }
    অন্যথা
	nr = printer_opts[idx].সংখ্যা;

    ৱিকল্প (u)
    {
	পক্ষ PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    ভঙ্গ;
	পক্ষ PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    ভঙ্গ;
	পক্ষ PRT_UNIT_POINT:
	    ret = (দ্ৱিগুণ)nr;
	    ভঙ্গ;
	পক্ষ PRT_UNIT_PERC:
	স্ৱভাৱ:
	    ret = physsize * (nr / 100.0);
	    ভঙ্গ;
    }

    ঘূৰাই ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    স্থির শূন্য
get_page_margins(
	দ্ৱিগুণ	width,
	দ্ৱিগুণ	height,
	দ্ৱিগুণ	*বাম,
	দ্ৱিগুণ	*ডান,
	দ্ৱিগুণ	*শীৰ্ষ,
	দ্ৱিগুণ	*bottom)
{
    *বাম = to_device_units(OPT_PRINT_বাম, width, 10);
    *ডান = width - to_device_units(OPT_PRINT_ডান, width, 5);

    *শীৰ্ষ = to_device_units(OPT_PRINT_শীৰ্ষ, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    স্থির পূর্ণাঙ্ক
get_cpl(শূন্য)
{
    যদি (prt_use_সংখ্যা())
    {
	pctx.সংখ্যা_width = PRINT_সংখ্যা_WIDTH * pctx.char_width;
	pctx.বাম_margin += pctx.সংখ্যা_width;
    }
    অন্যথা
	pctx.সংখ্যা_width = 0.0;

    ঘূৰাই (পূর্ণাঙ্ক)((pctx.ডান_margin - pctx.বাম_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    স্থির পূর্ণাঙ্ক
get_lpp(শূন্য)
{
    পূর্ণাঙ্ক lpp;

    // Calculate lpp
    lpp = (পূর্ণাঙ্ক)((pctx.bottom_margin - pctx.শীৰ্ষ_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.শীৰ্ষ_margin += pctx.char_height * prt_header_height();

    ঘূৰাই lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    স্থির PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    যদি (name == রিক্ত)
	ঘূৰাই রিক্ত;

    font = pango_font_description_from_string((ধ্রুৱ ৱর্ণ *)name);
    যদি (font == রিক্ত)
    {
	semsg(_(e_unknown_printer_font_str), name);
	ঘূৰাই রিক্ত;
    }

    // Ensure a size is set
    যদি (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    ঘূৰাই font;
}

    পূর্ণাঙ্ক
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	পূর্ণাঙ্ক		forceit UNUSED)
{
    char_u	*paper_name;
    পূর্ণাঙ্ক		paper_strlen;
    bool	portrait;
    পূর্ণাঙ্ক		media;
    char_u	*filename = রিক্ত;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    যদি (gui.in_use && !forceit && psettings->outfile == রিক্ত)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	যদি (filename == রিক্ত)
	    ঘূৰাই FAIL;
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
	বাবে (media = 0; media < (পূর্ণাঙ্ক)PRT_MEDIASIZE_LEN; ++media)
	    যদি ((পূর্ণাঙ্ক)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		ভঙ্গ;
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
	    বাবে (পূর্ণাঙ্ক i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(print_formats); i++)
		যদি (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    ভঙ্গ;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    যদি (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    যদি (filename == রিক্ত)
    {
	// If the user didn't specify a file name, use a temp file.
	যদি (psettings->outfile == রিক্ত)
	{
	    filename = vim_tempname('p', TRUE);
	    যদি (filename == রিক্ত)
	    {
		emsg(_(e_cant_get_temp_file_name));
		ঘূৰাই FAIL;
	    }
	}
	অন্যথা
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    যদি (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ধ্রুৱ ৱর্ণ *)filename,
		pctx.page_width, pctx.page_height);
    অন্যথা
	pctx.surface = cairo_pdf_surface_create((ধ্রুৱ ৱর্ণ *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    যদি (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ঘূৰাই FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    যদি (pctx.font == রিক্ত)
    {
	mch_print_cleanup();
	ঘূৰাই FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	পূর্ণাঙ্ক		    ascent;
	পূর্ণাঙ্ক		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, রিক্ত);

	pctx.char_width =
	    (দ্ৱিগুণ)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (দ্ৱিগুণ)ascent / PANGO_SCALE;
	pctx.char_height = (দ্ৱিগুণ)(ascent + descent) / PANGO_SCALE;

	pctx.সাৰি_spacing = (দ্ৱিগুণ)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (দ্ৱিগুণ)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (দ্ৱিগুণ)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	দ্ৱিগুণ  বাম;
	দ্ৱিগুণ	ডান;
	দ্ৱিগুণ	শীৰ্ষ;
	দ্ৱিগুণ	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &বাম, &ডান, &শীৰ্ষ,
		&bottom);
	pctx.বাম_margin = বাম;
	pctx.ডান_margin = ডান;
	pctx.শীৰ্ষ_margin = শীৰ্ষ;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_সাৰি = get_cpl();
    psettings->সাৰিগুলি_per_page = get_lpp();

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

    ঘূৰাই OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    স্থির শূন্য
emit_dsc_comment(ৱর্ণ *comment, ৱর্ণ *fmt, ...)
{
    স্থির ৱর্ণ buf[257];

    va_list ap;
    পূর্ণাঙ্ক	    len = পরিমাণ(buf);
    পূর্ণাঙ্ক	    o;
    ৱর্ণ    *s = buf;

    যদি (fmt == রিক্ত)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	গচ্ছ exit;
    }
    অন্যথা
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    যদি (len <= 0)
	ঘূৰাই;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    পূর্ণাঙ্ক
mch_print_begin(prt_settings_T *psettings)
{
    ৱর্ণ user[256];

    pctx.cur_x = 0.0;
    pctx.cur_সাৰি = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    যদি (pctx.dialog)
	ঘূৰাই TRUE;
#endif

    যদি (!get_user_name((char_u *)user, পরিমাণ(user)))
	STRCPY(user, "Unknown");

    যদি (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (ৱর্ণ *)psettings->jobname);
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
	emit_dsc_comment("BeginDefaults", রিক্ত);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", রিক্ত);
    }
    অন্যথা যদি (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	যদি (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, রিক্ত);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (ৱর্ণ *)str);
	যদি (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	যদি (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, রিক্ত);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (ৱর্ণ *)str);
	যদি (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    ঘূৰাই TRUE;
}

    শূন্য
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    যদি (psettings->outfile == রিক্ত && !got_int && !psettings->user_abort
	    && pctx.filename != রিক্ত
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	বাৰ্তা(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	যদি (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	অন্যথা
	    বাৰ্তা(_("Print job sent."));
    }

    mch_print_cleanup();
}

    পূর্ণাঙ্ক
mch_print_end_page(শূন্য)
{
    cairo_show_page(pctx.cr);
    ঘূৰাই TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    পূর্ণাঙ্ক
mch_print_begin_page(char_u *str UNUSED)
{
    ঘূৰাই TRUE;
}

    পূর্ণাঙ্ক
mch_print_blank_page(শূন্য)
{
    mch_print_end_page();
    ঘূৰাই TRUE;
}

    শূন্য
mch_print_start_সাৰি(পূর্ণাঙ্ক margin, পূর্ণাঙ্ক page_সাৰি)
{
    pctx.cur_x = pctx.বাম_margin;
    যদি (margin)
	pctx.cur_x -= pctx.সংখ্যা_width;

    pctx.cur_সাৰি = page_সাৰি;
}

    স্থির শূন্য
long_to_rgb(long_u c, দ্ৱিগুণ *r, দ্ৱিগুণ *g, দ্ৱিগুণ *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    পূর্ণাঙ্ক
mch_print_text_out(char_u *textp, পূর্ণাঙ্ক len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    দ্ৱিগুণ		r, g, b;
    দ্ৱিগুণ		width = 0;
    দ্ৱিগুণ		next_x;

    // Always convert to UTF-8, Pango expects that
    যদি (output_conv.vc_type == CONV_NONE)
	str = textp;
    অন্যথা
	str = string_convert(&output_conv, textp, &len);

    যদি (str == রিক্ত)
	ঘূৰাই TRUE;

    attr_list = pango_attr_list_new();

#define ঢোকাও_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
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
	    (ধ্রুৱ ৱর্ণ *)str, 0, len, attr_list, রিক্ত);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    যেতিয়ালৈ (item_list != রিক্ত)
    {
	PangoItem   *item;
	দ্ৱিগুণ	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((ধ্রুৱ ৱর্ণ *)str + item->offset, item->length,
		(ধ্রুৱ ৱর্ণ *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (দ্ৱিগুণ)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	যদি (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_সাৰি) - pctx.সাৰি_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_সাৰি));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    যদি (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	দ্ৱিগুণ y = GET_TEXT_Y(pctx.cur_সাৰি) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_সাৰি_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_সাৰি_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    next_x = pctx.cur_x + pctx.char_width;

    যদি (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    ঘূৰাই (next_x > pctx.ডান_margin) &&
	((next_x - pctx.ডান_margin) > (pctx.ডান_margin * 1e-5));
}

    শূন্য
mch_print_set_font(পূর্ণাঙ্ক bold, পূর্ণাঙ্ক italic, পূর্ণাঙ্ক underline)
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
