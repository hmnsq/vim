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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

#ifdef FEAT_PRINT_PANGO

#ଅନ୍ତର୍ଭାଵ "hardcopy.h"

#ଅନ୍ତର୍ଭାଵ <cairo/cairo-ps.h>
#ଅନ୍ତର୍ଭାଵ <cairo/cairo-pdf.h>
#ଅନ୍ତର୍ଭାଵ <pango/pango.h>
#ଅନ୍ତର୍ଭାଵ <pango/pangocairo.h>

#define PRINT_DPI 72.0

#define PRT_DRAW_BOLD 1
#define PRT_DRAW_ITALIC 2
#define PRT_DRAW_UNDERLINE 4

#define GET_Y(l) (pctx.ଶୀର୍ଷ_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
ସ୍ଥିର ସଂରଚନା
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    ଦ୍ୱିଗୁଣ char_width;
    ଦ୍ୱିଗୁଣ char_ascent;
    ଦ୍ୱିଗୁଣ char_height;
    ଦ୍ୱିଗୁଣ ଧାଡ଼ି_spacing;
    ଦ୍ୱିଗୁଣ ul_pos;
    ଦ୍ୱିଗୁଣ ul_thickness;

    ଦ୍ୱିଗୁଣ page_width;
    ଦ୍ୱିଗୁଣ page_height;

    ଦ୍ୱିଗୁଣ ବାମ_margin;
    ଦ୍ୱିଗୁଣ ଡାହାଣ_margin;
    ଦ୍ୱିଗୁଣ ଶୀର୍ଷ_margin;
    ଦ୍ୱିଗୁଣ bottom_margin;

    ଦ୍ୱିଗୁଣ ସଂଖ୍ୟା_width;

    // Current position to draw from
    ଦ୍ୱିଗୁଣ cur_x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_ଧାଡ଼ି;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    ସ୍ଥିର ଦ୍ୱିଗୁଣ
to_device_units(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ଦ୍ୱିଗୁଣ physsize, ପୂର୍ଣ୍ଣସଂଖ୍ୟା def_ସଂଖ୍ୟା)
{
    ଦଶମିକ	ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		u;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;

    u = prt_get_unit(idx);
    ଯଦି (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ସଂଖ୍ୟା;
    }
    ଅନ୍ୟଥା
	nr = printer_opts[idx].ସଂଖ୍ୟା;

    ବାଛ (u)
    {
	ସ୍ଥିତି PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PRT_UNIT_POINT:
	    ret = (ଦ୍ୱିଗୁଣ)nr;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PRT_UNIT_PERC:
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ret = physsize * (nr / 100.0);
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_page_margins(
	ଦ୍ୱିଗୁଣ	width,
	ଦ୍ୱିଗୁଣ	height,
	ଦ୍ୱିଗୁଣ	*ବାମ,
	ଦ୍ୱିଗୁଣ	*ଡାହାଣ,
	ଦ୍ୱିଗୁଣ	*ଶୀର୍ଷ,
	ଦ୍ୱିଗୁଣ	*bottom)
{
    *ବାମ = to_device_units(OPT_PRINT_ବାମ, width, 10);
    *ଡାହାଣ = width - to_device_units(OPT_PRINT_ଡାହାଣ, width, 5);

    *ଶୀର୍ଷ = to_device_units(OPT_PRINT_ଶୀର୍ଷ, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_cpl(ଶୂନ୍ୟ)
{
    ଯଦି (prt_use_ସଂଖ୍ୟା())
    {
	pctx.ସଂଖ୍ୟା_width = PRINT_ସଂଖ୍ୟା_WIDTH * pctx.char_width;
	pctx.ବାମ_margin += pctx.ସଂଖ୍ୟା_width;
    }
    ଅନ୍ୟଥା
	pctx.ସଂଖ୍ୟା_width = 0.0;

    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((pctx.ଡାହାଣ_margin - pctx.ବାମ_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lpp(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lpp;

    // Calculate lpp
    lpp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((pctx.bottom_margin - pctx.ଶୀର୍ଷ_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.ଶୀର୍ଷ_margin += pctx.char_height * prt_header_height();

    ଫେରନ୍ତୁ lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    ସ୍ଥିର PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    font = pango_font_description_from_string((ଅଚଳ ଅକ୍ଷର *)name);
    ଯଦି (font == ଖାଲି)
    {
	semsg(_(e_unknown_printer_font_str), name);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Ensure a size is set
    ଯଦି (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    ଫେରନ୍ତୁ font;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit UNUSED)
{
    char_u	*paper_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		paper_strlen;
    bool	portrait;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		media;
    char_u	*filename = ଖାଲି;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    ଯଦି (gui.in_use && !forceit && psettings->outfile == ଖାଲି)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	ଯଦି (filename == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    ଯଦି (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	ଯଦି (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
	}
	ଅନ୍ୟଥା
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	ପାଇଁ (media = 0; media < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)PRT_MEDIASIZE_LEN; ++media)
	    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		ଭାଙ୍ଗ;
	ଯଦି (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	ଯଦି (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	ଅନ୍ୟଥା
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	ଯଦି (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(print_formats); i++)
		ଯଦି (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    ଭାଙ୍ଗ;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    ଯଦି (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    ଯଦି (filename == ଖାଲି)
    {
	// If the user didn't specify a file name, use a temp file.
	ଯଦି (psettings->outfile == ଖାଲି)
	{
	    filename = vim_tempname('p', TRUE);
	    ଯଦି (filename == ଖାଲି)
	    {
		emsg(_(e_cant_get_temp_file_name));
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଅନ୍ୟଥା
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    ଯଦି (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ଅଚଳ ଅକ୍ଷର *)filename,
		pctx.page_width, pctx.page_height);
    ଅନ୍ୟଥା
	pctx.surface = cairo_pdf_surface_create((ଅଚଳ ଅକ୍ଷର *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    ଯଦି (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ଫେରନ୍ତୁ FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    ଯଦି (pctx.font == ଖାଲି)
    {
	mch_print_cleanup();
	ଫେରନ୍ତୁ FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ascent;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, ଖାଲି);

	pctx.char_width =
	    (ଦ୍ୱିଗୁଣ)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (ଦ୍ୱିଗୁଣ)ascent / PANGO_SCALE;
	pctx.char_height = (ଦ୍ୱିଗୁଣ)(ascent + descent) / PANGO_SCALE;

	pctx.ଧାଡ଼ି_spacing = (ଦ୍ୱିଗୁଣ)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (ଦ୍ୱିଗୁଣ)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (ଦ୍ୱିଗୁଣ)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	ଦ୍ୱିଗୁଣ  ବାମ;
	ଦ୍ୱିଗୁଣ	ଡାହାଣ;
	ଦ୍ୱିଗୁଣ	ଶୀର୍ଷ;
	ଦ୍ୱିଗୁଣ	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &ବାମ, &ଡାହାଣ, &ଶୀର୍ଷ,
		&bottom);
	pctx.ବାମ_margin = ବାମ;
	pctx.ଡାହାଣ_margin = ଡାହାଣ;
	pctx.ଶୀର୍ଷ_margin = ଶୀର୍ଷ;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_ଧାଡ଼ି = get_cpl();
    psettings->ଧାଡ଼ିଗୁଡ଼ିକ_per_page = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ifdef USE_GTK4_PRINT_DIALOG
    ଯଦି (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	ଯଦି (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    ଯଦି (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    ଫେରନ୍ତୁ OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
emit_dsc_comment(ଅକ୍ଷର *comment, ଅକ୍ଷର *fmt, ...)
{
    ସ୍ଥିର ଅକ୍ଷର buf[257];

    va_list ap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = ଆକାର(buf);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    o;
    ଅକ୍ଷର    *s = buf;

    ଯଦି (fmt == ଖାଲି)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	ଯାଅ exit;
    }
    ଅନ୍ୟଥା
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    ଯଦି (len <= 0)
	ଫେରନ୍ତୁ;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_begin(prt_settings_T *psettings)
{
    ଅକ୍ଷର user[256];

    pctx.cur_x = 0.0;
    pctx.cur_ଧାଡ଼ି = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    ଯଦି (pctx.dialog)
	ଫେରନ୍ତୁ TRUE;
#endif

    ଯଦି (!get_user_name((char_u *)user, ଆକାର(user)))
	STRCPY(user, "Unknown");

    ଯଦି (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (ଅକ୍ଷର *)psettings->jobname);
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
	emit_dsc_comment("BeginDefaults", ଖାଲି);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", ଖାଲି);
    }
    ଅନ୍ୟଥା ଯଦି (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	ଯଦି (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ଖାଲି);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (ଅକ୍ଷର *)str);
	ଯଦି (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	ଯଦି (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ଖାଲି);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (ଅକ୍ଷର *)str);
	ଯଦି (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    ଫେରନ୍ତୁ TRUE;
}

    ଶୂନ୍ୟ
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    ଯଦି (psettings->outfile == ଖାଲି && !got_int && !psettings->user_abort
	    && pctx.filename != ଖାଲି
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	ସନ୍ଦେଶ(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ଯଦି (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ(_("Print job sent."));
    }

    mch_print_cleanup();
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_end_page(ଶୂନ୍ୟ)
{
    cairo_show_page(pctx.cr);
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_begin_page(char_u *str UNUSED)
{
    ଫେରନ୍ତୁ TRUE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_blank_page(ଶୂନ୍ୟ)
{
    mch_print_end_page();
    ଫେରନ୍ତୁ TRUE;
}

    ଶୂନ୍ୟ
mch_print_start_ଧାଡ଼ି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା margin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା page_ଧାଡ଼ି)
{
    pctx.cur_x = pctx.ବାମ_margin;
    ଯଦି (margin)
	pctx.cur_x -= pctx.ସଂଖ୍ୟା_width;

    pctx.cur_ଧାଡ଼ି = page_ଧାଡ଼ି;
}

    ସ୍ଥିର ଶୂନ୍ୟ
long_to_rgb(long_u c, ଦ୍ୱିଗୁଣ *r, ଦ୍ୱିଗୁଣ *g, ଦ୍ୱିଗୁଣ *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_text_out(char_u *textp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    ଦ୍ୱିଗୁଣ		r, g, b;
    ଦ୍ୱିଗୁଣ		width = 0;
    ଦ୍ୱିଗୁଣ		next_x;

    // Always convert to UTF-8, Pango expects that
    ଯଦି (output_conv.vc_type == CONV_NONE)
	str = textp;
    ଅନ୍ୟଥା
	str = string_convert(&output_conv, textp, &len);

    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ TRUE;

    attr_list = pango_attr_list_new();

#define ଭର୍ତ୍ତି_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_ଭର୍ତ୍ତି((AttrList), tmp_attr_);	    \
    }G_STMT_END

    // Underline attribute does not work with pango_itemize().
    ଯଦି ((pctx.draw_flags & PRT_DRAW_BOLD))
	ଭର୍ତ୍ତି_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    ଯଦି (pctx.draw_flags & PRT_DRAW_ITALIC)
	ଭର୍ତ୍ତି_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef ଭର୍ତ୍ତି_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (ଅଚଳ ଅକ୍ଷର *)str, 0, len, attr_list, ଖାଲି);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    ଯେତେବେଳେ (item_list != ଖାଲି)
    {
	PangoItem   *item;
	ଦ୍ୱିଗୁଣ	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((ଅଚଳ ଅକ୍ଷର *)str + item->offset, item->length,
		(ଅଚଳ ଅକ୍ଷର *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (ଦ୍ୱିଗୁଣ)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	ଯଦି (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_ଧାଡ଼ି) - pctx.ଧାଡ଼ି_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_ଧାଡ଼ି));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    ଯଦି (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	ଦ୍ୱିଗୁଣ y = GET_TEXT_Y(pctx.cur_ଧାଡ଼ି) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_ଧାଡ଼ି_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_ଧାଡ଼ି_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    next_x = pctx.cur_x + pctx.char_width;

    ଯଦି (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    ଫେରନ୍ତୁ (next_x > pctx.ଡାହାଣ_margin) &&
	((next_x - pctx.ଡାହାଣ_margin) > (pctx.ଡାହାଣ_margin * 1e-5));
}

    ଶୂନ୍ୟ
mch_print_set_font(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold, ପୂର୍ଣ୍ଣସଂଖ୍ୟା italic, ପୂର୍ଣ୍ଣସଂଖ୍ୟା underline)
{
    pctx.draw_flags = 0;

    ଯଦି (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    ଯଦି (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    ଯଦି (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    ଶୂନ୍ୟ
mch_print_set_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    ଶୂନ୍ୟ
mch_print_set_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    ଶୂନ୍ୟ
mch_print_cleanup(ଶୂନ୍ୟ)
{
    g_clear_pointer(&pctx.cr, cairo_destroy);
    g_clear_pointer(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.text_context);
    g_clear_pointer(&pctx.font, pango_font_description_free);

#ifdef USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    ଯଦି (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
