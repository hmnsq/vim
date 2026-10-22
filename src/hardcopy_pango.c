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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

#ifdef FEAT_PRINT_PANGO

#ಅನ್ತರ್ಭಾವ "hardcopy.h"

#ಅನ್ತರ್ಭಾವ <cairo/cairo-ps.h>
#ಅನ್ತರ್ಭಾವ <cairo/cairo-pdf.h>
#ಅನ್ತರ್ಭಾವ <pango/pango.h>
#ಅನ್ತರ್ಭಾವ <pango/pangocairo.h>

#define PRINT_DPI 72.0

#define PRT_DRAW_BOLD 1
#define PRT_DRAW_ITALIC 2
#define PRT_DRAW_UNDERLINE 4

#define GET_Y(l) (pctx.ತುದಿ_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
ಸ್ಥಿರ ಸಂರಚನಾ
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    ಪೂರ್ಣಾಙ್ಕ media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    ದ್ವಿಗುಣ char_width;
    ದ್ವಿಗುಣ char_ascent;
    ದ್ವಿಗುಣ char_height;
    ದ್ವಿಗುಣ ಸಾಲು_spacing;
    ದ್ವಿಗುಣ ul_pos;
    ದ್ವಿಗುಣ ul_thickness;

    ದ್ವಿಗುಣ page_width;
    ದ್ವಿಗುಣ page_height;

    ದ್ವಿಗುಣ ಎಡ_margin;
    ದ್ವಿಗುಣ ಬಲ_margin;
    ದ್ವಿಗುಣ ತುದಿ_margin;
    ದ್ವಿಗುಣ bottom_margin;

    ದ್ವಿಗುಣ ಸಂಖ್ಯೆ_width;

    // Current position to draw from
    ದ್ವಿಗುಣ cur_x;
    ಪೂರ್ಣಾಙ್ಕ cur_ಸಾಲು;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    ಸ್ಥಿರ ದ್ವಿಗುಣ
to_device_units(ಪೂರ್ಣಾಙ್ಕ idx, ದ್ವಿಗುಣ physsize, ಪೂರ್ಣಾಙ್ಕ def_ಸಂಖ್ಯೆ)
{
    ಭಿನ್ನ	ret;
    ಪೂರ್ಣಾಙ್ಕ		u;
    ಪೂರ್ಣಾಙ್ಕ		nr;

    u = prt_get_unit(idx);
    ಯದಿ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ಸಂಖ್ಯೆ;
    }
    ಅನ್ಯಥಾ
	nr = printer_opts[idx].ಸಂಖ್ಯೆ;

    ವಿಕಲ್ಪ (u)
    {
	ಪಕ್ಷ PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PRT_UNIT_POINT:
	    ret = (ದ್ವಿಗುಣ)nr;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PRT_UNIT_PERC:
	ಸ್ವಭಾವ:
	    ret = physsize * (nr / 100.0);
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_page_margins(
	ದ್ವಿಗುಣ	width,
	ದ್ವಿಗುಣ	height,
	ದ್ವಿಗುಣ	*ಎಡ,
	ದ್ವಿಗುಣ	*ಬಲ,
	ದ್ವಿಗುಣ	*ತುದಿ,
	ದ್ವಿಗುಣ	*bottom)
{
    *ಎಡ = to_device_units(OPT_PRINT_ಎಡ, width, 10);
    *ಬಲ = width - to_device_units(OPT_PRINT_ಬಲ, width, 5);

    *ತುದಿ = to_device_units(OPT_PRINT_ತುದಿ, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_cpl(ಶೂನ್ಯ)
{
    ಯದಿ (prt_use_ಸಂಖ್ಯೆ())
    {
	pctx.ಸಂಖ್ಯೆ_width = PRINT_ಸಂಖ್ಯೆ_WIDTH * pctx.char_width;
	pctx.ಎಡ_margin += pctx.ಸಂಖ್ಯೆ_width;
    }
    ಅನ್ಯಥಾ
	pctx.ಸಂಖ್ಯೆ_width = 0.0;

    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)((pctx.ಬಲ_margin - pctx.ಎಡ_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_lpp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ lpp;

    // Calculate lpp
    lpp = (ಪೂರ್ಣಾಙ್ಕ)((pctx.bottom_margin - pctx.ತುದಿ_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.ತುದಿ_margin += pctx.char_height * prt_header_height();

    ಪ್ರತ್ಯಾಗಮ lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    ಸ್ಥಿರ PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    font = pango_font_description_from_string((ಧ್ರುವ ವರ್ಣ *)name);
    ಯದಿ (font == ರಿಕ್ತ)
    {
	semsg(_(e_unknown_printer_font_str), name);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Ensure a size is set
    ಯದಿ (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    ಪ್ರತ್ಯಾಗಮ font;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	ಪೂರ್ಣಾಙ್ಕ		forceit UNUSED)
{
    char_u	*paper_name;
    ಪೂರ್ಣಾಙ್ಕ		paper_strlen;
    bool	portrait;
    ಪೂರ್ಣಾಙ್ಕ		media;
    char_u	*filename = ರಿಕ್ತ;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    ಯದಿ (gui.in_use && !forceit && psettings->outfile == ರಿಕ್ತ)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	ಯದಿ (filename == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    ಯದಿ (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	ಯದಿ (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
	}
	ಅನ್ಯಥಾ
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	ಪ್ರತಿ (media = 0; media < (ಪೂರ್ಣಾಙ್ಕ)PRT_MEDIASIZE_LEN; ++media)
	    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		ಭಙ್ಗ;
	ಯದಿ (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	ಯದಿ (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	ಅನ್ಯಥಾ
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	ಯದಿ (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(print_formats); i++)
		ಯದಿ (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    ಭಙ್ಗ;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    ಯದಿ (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    ಯದಿ (filename == ರಿಕ್ತ)
    {
	// If the user didn't specify a file name, use a temp file.
	ಯದಿ (psettings->outfile == ರಿಕ್ತ)
	{
	    filename = vim_tempname('p', TRUE);
	    ಯದಿ (filename == ರಿಕ್ತ)
	    {
		emsg(_(e_cant_get_temp_file_name));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	ಅನ್ಯಥಾ
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    ಯದಿ (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ಧ್ರುವ ವರ್ಣ *)filename,
		pctx.page_width, pctx.page_height);
    ಅನ್ಯಥಾ
	pctx.surface = cairo_pdf_surface_create((ಧ್ರುವ ವರ್ಣ *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    ಯದಿ (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    ಯದಿ (pctx.font == ರಿಕ್ತ)
    {
	mch_print_cleanup();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	ಪೂರ್ಣಾಙ್ಕ		    ascent;
	ಪೂರ್ಣಾಙ್ಕ		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, ರಿಕ್ತ);

	pctx.char_width =
	    (ದ್ವಿಗುಣ)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (ದ್ವಿಗುಣ)ascent / PANGO_SCALE;
	pctx.char_height = (ದ್ವಿಗುಣ)(ascent + descent) / PANGO_SCALE;

	pctx.ಸಾಲು_spacing = (ದ್ವಿಗುಣ)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (ದ್ವಿಗುಣ)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (ದ್ವಿಗುಣ)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	ದ್ವಿಗುಣ  ಎಡ;
	ದ್ವಿಗುಣ	ಬಲ;
	ದ್ವಿಗುಣ	ತುದಿ;
	ದ್ವಿಗುಣ	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &ಎಡ, &ಬಲ, &ತುದಿ,
		&bottom);
	pctx.ಎಡ_margin = ಎಡ;
	pctx.ಬಲ_margin = ಬಲ;
	pctx.ತುದಿ_margin = ತುದಿ;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_ಸಾಲು = get_cpl();
    psettings->ಸಾಲುಗಳು_per_page = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ifdef USE_GTK4_PRINT_DIALOG
    ಯದಿ (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	ಯದಿ (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    ಯದಿ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
emit_dsc_comment(ವರ್ಣ *comment, ವರ್ಣ *fmt, ...)
{
    ಸ್ಥಿರ ವರ್ಣ buf[257];

    va_list ap;
    ಪೂರ್ಣಾಙ್ಕ	    len = ಪರಿಮಾಣ(buf);
    ಪೂರ್ಣಾಙ್ಕ	    o;
    ವರ್ಣ    *s = buf;

    ಯದಿ (fmt == ರಿಕ್ತ)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	ಗಚ್ಛ exit;
    }
    ಅನ್ಯಥಾ
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    ಯದಿ (len <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_begin(prt_settings_T *psettings)
{
    ವರ್ಣ user[256];

    pctx.cur_x = 0.0;
    pctx.cur_ಸಾಲು = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    ಯದಿ (pctx.dialog)
	ಪ್ರತ್ಯಾಗಮ TRUE;
#endif

    ಯದಿ (!get_user_name((char_u *)user, ಪರಿಮಾಣ(user)))
	STRCPY(user, "Unknown");

    ಯದಿ (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (ವರ್ಣ *)psettings->jobname);
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
	emit_dsc_comment("BeginDefaults", ರಿಕ್ತ);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ ಯದಿ (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	ಯದಿ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ರಿಕ್ತ);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (ವರ್ಣ *)str);
	ಯದಿ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	ಯದಿ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ರಿಕ್ತ);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (ವರ್ಣ *)str);
	ಯದಿ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಶೂನ್ಯ
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    ಯದಿ (psettings->outfile == ರಿಕ್ತ && !got_int && !psettings->user_abort
	    && pctx.filename != ರಿಕ್ತ
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	ಸಂದೇಶ(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ಯದಿ (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ(_("Print job sent."));
    }

    mch_print_cleanup();
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_end_page(ಶೂನ್ಯ)
{
    cairo_show_page(pctx.cr);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_print_begin_page(char_u *str UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_blank_page(ಶೂನ್ಯ)
{
    mch_print_end_page();
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಶೂನ್ಯ
mch_print_start_ಸಾಲು(ಪೂರ್ಣಾಙ್ಕ margin, ಪೂರ್ಣಾಙ್ಕ page_ಸಾಲು)
{
    pctx.cur_x = pctx.ಎಡ_margin;
    ಯದಿ (margin)
	pctx.cur_x -= pctx.ಸಂಖ್ಯೆ_width;

    pctx.cur_ಸಾಲು = page_ಸಾಲು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
long_to_rgb(long_u c, ದ್ವಿಗುಣ *r, ದ್ವಿಗುಣ *g, ದ್ವಿಗುಣ *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_print_text_out(char_u *textp, ಪೂರ್ಣಾಙ್ಕ len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    ದ್ವಿಗುಣ		r, g, b;
    ದ್ವಿಗುಣ		width = 0;
    ದ್ವಿಗುಣ		next_x;

    // Always convert to UTF-8, Pango expects that
    ಯದಿ (output_conv.vc_type == CONV_NONE)
	str = textp;
    ಅನ್ಯಥಾ
	str = string_convert(&output_conv, textp, &len);

    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    attr_list = pango_attr_list_new();

#define ಸೇರಿಸು_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_ಸೇರಿಸು((AttrList), tmp_attr_);	    \
    }G_STMT_END

    // Underline attribute does not work with pango_itemize().
    ಯದಿ ((pctx.draw_flags & PRT_DRAW_BOLD))
	ಸೇರಿಸು_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    ಯದಿ (pctx.draw_flags & PRT_DRAW_ITALIC)
	ಸೇರಿಸು_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef ಸೇರಿಸು_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (ಧ್ರುವ ವರ್ಣ *)str, 0, len, attr_list, ರಿಕ್ತ);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    ಯಾವತ್ (item_list != ರಿಕ್ತ)
    {
	PangoItem   *item;
	ದ್ವಿಗುಣ	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((ಧ್ರುವ ವರ್ಣ *)str + item->offset, item->length,
		(ಧ್ರುವ ವರ್ಣ *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (ದ್ವಿಗುಣ)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	ಯದಿ (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_ಸಾಲು) - pctx.ಸಾಲು_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_ಸಾಲು));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    ಯದಿ (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	ದ್ವಿಗುಣ y = GET_TEXT_Y(pctx.cur_ಸಾಲು) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_ಸಾಲು_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_ಸಾಲು_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    next_x = pctx.cur_x + pctx.char_width;

    ಯದಿ (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    ಪ್ರತ್ಯಾಗಮ (next_x > pctx.ಬಲ_margin) &&
	((next_x - pctx.ಬಲ_margin) > (pctx.ಬಲ_margin * 1e-5));
}

    ಶೂನ್ಯ
mch_print_set_font(ಪೂರ್ಣಾಙ್ಕ bold, ಪೂರ್ಣಾಙ್ಕ italic, ಪೂರ್ಣಾಙ್ಕ underline)
{
    pctx.draw_flags = 0;

    ಯದಿ (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    ಯದಿ (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    ಯದಿ (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    ಶೂನ್ಯ
mch_print_set_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    ಶೂನ್ಯ
mch_print_set_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    ಶೂನ್ಯ
mch_print_cleanup(ಶೂನ್ಯ)
{
    g_clear_pointer(&pctx.cr, cairo_destroy);
    g_clear_pointer(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.text_context);
    g_clear_pointer(&pctx.font, pango_font_description_free);

#ifdef USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    ಯದಿ (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
