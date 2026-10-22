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

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

#నిర్వచితమైతే FEAT_PRINT_PANGO

#అన్తర్భావ "hardcopy.h"

#అన్తర్భావ <cairo/cairo-ps.h>
#అన్తర్భావ <cairo/cairo-pdf.h>
#అన్తర్భావ <pango/pango.h>
#అన్తర్భావ <pango/pangocairo.h>

#నిర్వచించు PRINT_DPI 72.0

#నిర్వచించు PRT_DRAW_BOLD 1
#నిర్వచించు PRT_DRAW_ITALIC 2
#నిర్వచించు PRT_DRAW_UNDERLINE 4

#నిర్వచించు GET_Y(l) (pctx.శిఖరం_margin + (l) * pctx.char_ఎత్తు)
#నిర్వచించు GET_పాఠ్యం_Y(l) (GET_Y(l) + pctx.char_ascent)

రకనిర్వచనం గణన
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
స్థిర నిర్మాణం
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *పాఠ్యం_context;

    PangoFontDescription *font;

    char_u *filename;

    పూర్ణాంకం media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    ద్విగుణ char_వెడల్పు;
    ద్విగుణ char_ascent;
    ద్విగుణ char_ఎత్తు;
    ద్విగుణ పంక్తి_spacing;
    ద్విగుణ ul_pos;
    ద్విగుణ ul_thickness;

    ద్విగుణ పుట_వెడల్పు;
    ద్విగుణ పుట_ఎత్తు;

    ద్విగుణ ఎడమ_margin;
    ద్విగుణ కుడి_margin;
    ద్విగుణ శిఖరం_margin;
    ద్విగుణ bottom_margin;

    ద్విగుణ సంఖ్య_వెడల్పు;

    // Current position to draw from
    ద్విగుణ cur_x;
    పూర్ణాంకం cur_పంక్తి;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#నిర్వచితమైతే USE_GTK4_PRINT_DIALOG
    bool dialog;
#అయితే_ముగింపు
} pctx;

స్థిర స్థిరరాశి అక్షరం *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    స్థిర ద్విగుణ
గమ్యం_device_units(పూర్ణాంకం idx, ద్విగుణ physsize, పూర్ణాంకం def_సంఖ్య)
{
    దశాంశం	ret;
    పూర్ణాంకం		u;
    పూర్ణాంకం		nr;

    u = prt_get_unit(idx);
    ఒకవేళ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_సంఖ్య;
    }
    లేకపోతే
	nr = printer_opts[idx].సంఖ్య;

    ఎంపిక (u)
    {
	స్థితి PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    ఆపు;
	స్థితి PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    ఆపు;
	స్థితి PRT_UNIT_బిందువు:
	    ret = (ద్విగుణ)nr;
	    ఆపు;
	స్థితి PRT_UNIT_PERC:
	అప్రమేయ:
	    ret = physsize * (nr / 100.0);
	    ఆపు;
    }

    తిరిగి ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    స్థిర శూన్య
get_పుట_margins(
	ద్విగుణ	వెడల్పు,
	ద్విగుణ	ఎత్తు,
	ద్విగుణ	*ఎడమ_2,
	ద్విగుణ	*కుడి_2,
	ద్విగుణ	*శిఖరం,
	ద్విగుణ	*bottom)
{
    *ఎడమ_2 = గమ్యం_device_units(OPT_PRINT_ఎడమ, వెడల్పు, 10);
    *కుడి_2 = వెడల్పు - గమ్యం_device_units(OPT_PRINT_కుడి, వెడల్పు, 5);

    *శిఖరం = గమ్యం_device_units(OPT_PRINT_శిఖరం, ఎత్తు, 5);
    *bottom = ఎత్తు - గమ్యం_device_units(OPT_PRINT_BOT, ఎత్తు, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    స్థిర పూర్ణాంకం
get_cpl(శూన్య)
{
    ఒకవేళ (prt_use_సంఖ్య())
    {
	pctx.సంఖ్య_వెడల్పు = PRINT_సంఖ్య_వెడల్పు * pctx.char_వెడల్పు;
	pctx.ఎడమ_margin += pctx.సంఖ్య_వెడల్పు;
    }
    లేకపోతే
	pctx.సంఖ్య_వెడల్పు = 0.0;

    తిరిగి (పూర్ణాంకం)((pctx.కుడి_margin - pctx.ఎడమ_margin) / pctx.char_వెడల్పు);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    స్థిర పూర్ణాంకం
get_lpp(శూన్య)
{
    పూర్ణాంకం lpp;

    // Calculate lpp
    lpp = (పూర్ణాంకం)((pctx.bottom_margin - pctx.శిఖరం_margin) / pctx.char_ఎత్తు);

    // Adjust top margin if there is a header
    pctx.శిఖరం_margin += pctx.char_ఎత్తు * prt_శీర్షిక_ఎత్తు();

    తిరిగి lpp - prt_శీర్షిక_ఎత్తు();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    స్థిర PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    ఒకవేళ (name == ఖాళీ)
	తిరిగి ఖాళీ;

    font = pango_font_description_from_string((స్థిరరాశి అక్షరం *)name);
    ఒకవేళ (font == ఖాళీ)
    {
	semsg(_(e_unknown_printer_font_str), name);
	తిరిగి ఖాళీ;
    }

    // Ensure a size is set
    ఒకవేళ (pango_font_description_get_కొలత(font) <= 0)
	pango_font_description_సమితి_కొలత(font, 10 * PANGO_SCALE);

    తిరిగి font;
}

    పూర్ణాంకం
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	పూర్ణాంకం		forceit వాడని)
{
    char_u	*paper_name;
    పూర్ణాంకం		paper_strlen;
    bool	portrait;
    పూర్ణాంకం		media;
    char_u	*filename = ఖాళీ;
#నిర్వచితమైతే USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    ఒకవేళ (gui.లోపల_use && !forceit && psettings->outfile == ఖాళీ)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.పుట_వెడల్పు, &pctx.పుట_ఎత్తు);

	ఒకవేళ (filename == ఖాళీ)
	    తిరిగి FAIL;
	dialog = true;
    }
#అయితే_ముగింపు

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#నిర్వచితమైతే USE_GTK4_PRINT_DIALOG
    ఒకవేళ (!dialog)
#అయితే_ముగింపు
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	ఒకవేళ (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].అక్షరశ్రేణి_పొడవు;
	}
	లేకపోతే
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	కొరకు (media = 0; media < (పూర్ణాంకం)PRT_MEDIASIZE_LEN; ++media)
	    ఒకవేళ ((పూర్ణాంకం)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		ఆపు;
	ఒకవేళ (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	ఒకవేళ (portrait)
	{
	    pctx.పుట_వెడల్పు = prt_mediasize[media].వెడల్పు;
	    pctx.పుట_ఎత్తు = prt_mediasize[media].ఎత్తు;
	}
	లేకపోతే
	{
	    pctx.పుట_వెడల్పు = prt_mediasize[media].ఎత్తు;
	    pctx.పుట_ఎత్తు = prt_mediasize[media].వెడల్పు;
	}

	ఒకవేళ (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    కొరకు (పూర్ణాంకం i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(print_formats); i++)
		ఒకవేళ (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].అక్షరశ్రేణి_పొడవు) == 0)
		{
		    pctx.format = i;
		    ఆపు;
		}
	}
    }
    psettings->user_abort = FALSE;

#నిర్వచితమైతే USE_GTK4_PRINT_DIALOG
    ఒకవేళ (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#అయితే_ముగింపు

    ఒకవేళ (filename == ఖాళీ)
    {
	// If the user didn't specify a file name, use a temp file.
	ఒకవేళ (psettings->outfile == ఖాళీ)
	{
	    filename = vim_tempname('p', TRUE);
	    ఒకవేళ (filename == ఖాళీ)
	    {
		emsg(_(e_cant_get_తాత్కాలిక_file_name));
		తిరిగి FAIL;
	    }
	}
	లేకపోతే
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    ఒకవేళ (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((స్థిరరాశి అక్షరం *)filename,
		pctx.పుట_వెడల్పు, pctx.పుట_ఎత్తు);
    లేకపోతే
	pctx.surface = cairo_pdf_surface_create((స్థిరరాశి అక్షరం *)filename,
		pctx.పుట_వెడల్పు, pctx.పుట_ఎత్తు);

    pctx.filename = (char_u *)filename;

    ఒకవేళ (cairo_surface_స్థితి(pctx.surface) != CAIRO_స్థితి_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	తిరిగి FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.పాఠ్యం_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    ఒకవేళ (pctx.font == ఖాళీ)
    {
	mch_print_cleanup();
	తిరిగి FAIL;
    }

    pango_context_సమితి_font_description(pctx.పాఠ్యం_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_సమితి_resolution(pctx.పాఠ్యం_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	పూర్ణాంకం		    ascent;
	పూర్ణాంకం		    descent;

	layout = pango_layout_new(pctx.పాఠ్యం_context);
	pango_layout_సమితి_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, ఖాళీ);

	pctx.char_వెడల్పు =
	    (ద్విగుణ)pango_font_metrics_get_approximate_char_వెడల్పు(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (ద్విగుణ)ascent / PANGO_SCALE;
	pctx.char_ఎత్తు = (ద్విగుణ)(ascent + descent) / PANGO_SCALE;

	pctx.పంక్తి_spacing = (ద్విగుణ)
	    (pango_font_metrics_get_ఎత్తు(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (ద్విగుణ)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (ద్విగుణ)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	ద్విగుణ  ఎడమ_2;
	ద్విగుణ	కుడి_2;
	ద్విగుణ	శిఖరం;
	ద్విగుణ	bottom;

	get_పుట_margins(pctx.పుట_వెడల్పు, pctx.పుట_ఎత్తు, &ఎడమ_2, &కుడి_2, &శిఖరం,
		&bottom);
	pctx.ఎడమ_margin = ఎడమ_2;
	pctx.కుడి_margin = కుడి_2;
	pctx.శిఖరం_margin = శిఖరం;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_చొప్పున_పంక్తి = get_cpl();
    psettings->పంక్తులు_చొప్పున_పుట = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#నిర్వచితమైతే USE_GTK4_PRINT_DIALOG
    ఒకవేళ (!dialog)
#అయితే_ముగింపు
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	ఒకవేళ (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    ఒకవేళ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    లేకపోతే ఒకవేళ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#నిర్వచితమైతే USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#అయితే_ముగింపు

    తిరిగి OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    స్థిర శూన్య
emit_dsc_comment(అక్షరం *comment, అక్షరం *fmt, ...)
{
    స్థిర అక్షరం buf[257];

    va_list ap;
    పూర్ణాంకం	    len = పరిమాణం(buf);
    పూర్ణాంకం	    o;
    అక్షరం    *s = buf;

    ఒకవేళ (fmt == ఖాళీ)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	వెళ్ళు నిష్క్రమించు;
    }
    లేకపోతే
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    ఒకవేళ (len <= 0)
	తిరిగి;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

నిష్క్రమించు:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    పూర్ణాంకం
mch_print_begin(prt_settings_T *psettings)
{
    అక్షరం user[256];

    pctx.cur_x = 0.0;
    pctx.cur_పంక్తి = 0;
    pctx.draw_flags = 0;

#నిర్వచితమైతే USE_GTK4_PRINT_DIALOG
    ఒకవేళ (pctx.dialog)
	తిరిగి TRUE;
#అయితే_ముగింపు

    ఒకవేళ (!get_user_name((char_u *)user, పరిమాణం(user)))
	STRCPY(user, "Unknown");

    ఒకవేళ (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (అక్షరం *)psettings->jobname);
	emit_dsc_comment("For", user);
	emit_dsc_comment("Orientation",
		pctx.portrait ? "Portrait" : "Landscape");
	emit_dsc_comment("PageOrder", "Ascend");

	emit_dsc_comment("DocumentMedia",
		"%s %.2lf %.2lf 0 () ()",
		prt_mediasize[pctx.media].name,
		prt_mediasize[pctx.media].వెడల్పు,
		prt_mediasize[pctx.media].ఎత్తు);

	emit_dsc_comment("Requirements", "%s%s %s %s",
		pctx.duplex ? "duplex" : "",
		pctx.tumble ? "(tumble)" : "",
		pctx.collate ? "collate" : "",
		psettings->do_syntax ? "color" : "");

	// In defaults section (defaults for every page)
	emit_dsc_comment("BeginDefaults", ఖాళీ);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", ఖాళీ);
    }
    లేకపోతే ఒకవేళ (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	ఒకవేళ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ఖాళీ);

	cairo_pdf_surface_సమితి_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (అక్షరం *)str);
	ఒకవేళ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	ఒకవేళ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ఖాళీ);
	cairo_pdf_surface_సమితి_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (అక్షరం *)str);
	ఒకవేళ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_సమితి_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_సంస్కరణ_LONG);
    }

    తిరిగి TRUE;
}

    శూన్య
mch_print_చివర(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_ముగింపు(pctx.surface);

    ఒకవేళ (psettings->outfile == ఖాళీ && !got_int && !psettings->user_abort
	    && pctx.filename != ఖాళీ
#నిర్వచితమైతే USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#అయితే_ముగింపు
       )
    {
	సందేశం(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ఒకవేళ (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_గమ్యం_print_postscript_file));
	లేకపోతే
	    సందేశం(_("Print job sent."));
    }

    mch_print_cleanup();
}

    పూర్ణాంకం
mch_print_చివర_పుట(శూన్య)
{
    cairo_show_పుట(pctx.cr);
    తిరిగి TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    పూర్ణాంకం
mch_print_begin_పుట(char_u *str వాడని)
{
    తిరిగి TRUE;
}

    పూర్ణాంకం
mch_print_blank_పుట(శూన్య)
{
    mch_print_చివర_పుట();
    తిరిగి TRUE;
}

    శూన్య
mch_print_ప్రారంభం_పంక్తి(పూర్ణాంకం margin, పూర్ణాంకం పుట_పంక్తి)
{
    pctx.cur_x = pctx.ఎడమ_margin;
    ఒకవేళ (margin)
	pctx.cur_x -= pctx.సంఖ్య_వెడల్పు;

    pctx.cur_పంక్తి = పుట_పంక్తి;
}

    స్థిర శూన్య
long_గమ్యం_rgb(long_u c, ద్విగుణ *r, ద్విగుణ *g, ద్విగుణ *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    పూర్ణాంకం
mch_print_పాఠ్యం_బహిర్గతం(char_u *textp, పూర్ణాంకం len)
{
    PangoAttrList	*attr_list;
    GList		*అంశం_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    ద్విగుణ		r, g, b;
    ద్విగుణ		వెడల్పు = 0;
    ద్విగుణ		తదుపరి_x;

    // Always convert to UTF-8, Pango expects that
    ఒకవేళ (output_conv.vc_type == CONV_NONE)
	str = textp;
    లేకపోతే
	str = string_convert(&output_conv, textp, &len);

    ఒకవేళ (str == ఖాళీ)
	తిరిగి TRUE;

    attr_list = pango_attr_list_new();

#నిర్వచించు చొప్పించు_PANGO_ATTR(Attribute, AttrList, ప్రారంభం_2, చివర_2)  \
    G_STMT_ప్రారంభం{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->ప్రారంభం_సూచీ = (ప్రారంభం_2);		    \
	tmp_attr_->చివర_సూచీ = (చివర_2);			    \
	pango_attr_list_చొప్పించు((AttrList), tmp_attr_);	    \
    }G_STMT_చివర

    // Underline attribute does not work with pango_itemize().
    ఒకవేళ ((pctx.draw_flags & PRT_DRAW_BOLD))
	చొప్పించు_PANGO_ATTR(pango_attr_బరువు_new(PANGO_బరువు_BOLD),
		attr_list, 0, len);
    ఒకవేళ (pctx.draw_flags & PRT_DRAW_ITALIC)
	చొప్పించు_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#నిర్వచనం_రద్దు చొప్పించు_PANGO_ATTR

    అంశం_list = pango_itemize(pctx.పాఠ్యం_context,
	    (స్థిరరాశి అక్షరం *)str, 0, len, attr_list, ఖాళీ);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    అయితే (అంశం_list != ఖాళీ)
    {
	Pango_అంశం   *అంశం;
	ద్విగుణ	    o;

	అంశం = (Pango_అంశం *)అంశం_list->దత్తాంశం;
	అంశం_list = g_list_delete_link(అంశం_list, అంశం_list);

	pango_shape_నిండిన((స్థిరరాశి అక్షరం *)str + అంశం->స్థానభ్రంశం, అంశం->నిడివి,
		(స్థిరరాశి అక్షరం *)str, len, &అంశం->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (ద్విగుణ)pango_glyph_string_get_వెడల్పు(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	ఒకవేళ (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_గమ్యం_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_సమితి_ఆధారం_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + వెడల్పు,
		    GET_Y(pctx.cur_పంక్తి) - pctx.పంక్తి_spacing, o, pctx.char_ఎత్తు);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_గమ్యం_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_సమితి_ఆధారం_rgb(pctx.cr, r, g, b);
	cairo_move_గమ్యం(pctx.cr, pctx.cur_x + వెడల్పు, GET_పాఠ్యం_Y(pctx.cur_పంక్తి));
	pango_cairo_show_glyph_string(pctx.cr, అంశం->analysis.font, glyphs);

	pango_అంశం_free(అంశం);
	వెడల్పు += o;
    }

    // Draw underline if needed
    ఒకవేళ (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	ద్విగుణ y = GET_పాఠ్యం_Y(pctx.cur_పంక్తి) - pctx.ul_pos;

	long_గమ్యం_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_సమితి_ఆధారం_rgb(pctx.cr, r, g, b);
	cairo_సమితి_పంక్తి_వెడల్పు(pctx.cr, pctx.ul_thickness);
	cairo_move_గమ్యం(pctx.cr, pctx.cur_x, y);
	cairo_పంక్తి_గమ్యం(pctx.cr, pctx.cur_x + వెడల్పు, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += వెడల్పు;

    తదుపరి_x = pctx.cur_x + pctx.char_వెడల్పు;

    ఒకవేళ (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    తిరిగి (తదుపరి_x > pctx.కుడి_margin) &&
	((తదుపరి_x - pctx.కుడి_margin) > (pctx.కుడి_margin * 1e-5));
}

    శూన్య
mch_print_సమితి_font(పూర్ణాంకం bold, పూర్ణాంకం italic, పూర్ణాంకం underline)
{
    pctx.draw_flags = 0;

    ఒకవేళ (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    ఒకవేళ (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    ఒకవేళ (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    శూన్య
mch_print_సమితి_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    శూన్య
mch_print_సమితి_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    శూన్య
mch_print_cleanup(శూన్య)
{
    g_clear_సూచిక(&pctx.cr, cairo_destroy);
    g_clear_సూచిక(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.పాఠ్యం_context);
    g_clear_సూచిక(&pctx.font, pango_font_description_free);

#నిర్వచితమైతే USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    ఒకవేళ (pctx.dialog)
	gui_gtk4_print_ముగింపు();
#అయితే_ముగింపు

    g_clear_సూచిక(&pctx.filename, vim_free);
}

#అయితే_ముగింపు // FEAT_PRINT_PANGO
