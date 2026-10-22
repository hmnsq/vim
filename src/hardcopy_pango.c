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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINT_PANGO

#ಅನ್ತರ್ಭಾವ "hardcopy.h"

#ಅನ್ತರ್ಭಾವ <cairo/cairo-ps.h>
#ಅನ್ತರ್ಭಾವ <cairo/cairo-pdf.h>
#ಅನ್ತರ್ಭಾವ <pango/pango.h>
#ಅನ್ತರ್ಭಾವ <pango/pangocairo.h>

#ವ್ಯಾಖ್ಯಾನಿಸು PRINT_DPI 72.0

#ವ್ಯಾಖ್ಯಾನಿಸು PRT_DRAW_BOLD 1
#ವ್ಯಾಖ್ಯಾನಿಸು PRT_DRAW_ITALIC 2
#ವ್ಯಾಖ್ಯಾನಿಸು PRT_DRAW_UNDERLINE 4

#ವ್ಯಾಖ್ಯಾನಿಸು GET_Y(l) (pctx.ತುದಿ_margin + (l) * pctx.char_ಎತ್ತರ)
#ವ್ಯಾಖ್ಯಾನಿಸು GET_ಪಠ್ಯ_Y(l) (GET_Y(l) + pctx.char_ascent)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
ಸ್ಥಿರ ರಚನೆ
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *ಪಠ್ಯ_context;

    PangoFontDescription *font;

    char_u *filename;

    ಪೂರ್ಣಾಂಕ media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    ದ್ವಿಗುಣ char_ಅಗಲ;
    ದ್ವಿಗುಣ char_ascent;
    ದ್ವಿಗುಣ char_ಎತ್ತರ;
    ದ್ವಿಗುಣ ಸಾಲು_spacing;
    ದ್ವಿಗುಣ ul_pos;
    ದ್ವಿಗುಣ ul_thickness;

    ದ್ವಿಗುಣ ಪುಟ_ಅಗಲ;
    ದ್ವಿಗುಣ ಪುಟ_ಎತ್ತರ;

    ದ್ವಿಗುಣ ಎಡ_margin;
    ದ್ವಿಗುಣ ಬಲ_margin;
    ದ್ವಿಗುಣ ತುದಿ_margin;
    ದ್ವಿಗುಣ bottom_margin;

    ದ್ವಿಗುಣ ಸಂಖ್ಯೆ_ಅಗಲ;

    // Current position to draw from
    ದ್ವಿಗುಣ cur_x;
    ಪೂರ್ಣಾಂಕ cur_ಸಾಲು;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4_PRINT_DIALOG
    bool dialog;
#ಆದರೆ_ಅಂತ್ಯ
} pctx;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    ಸ್ಥಿರ ದ್ವಿಗುಣ
ಗಮ್ಯ_device_units(ಪೂರ್ಣಾಂಕ idx, ದ್ವಿಗುಣ physsize, ಪೂರ್ಣಾಂಕ def_ಸಂಖ್ಯೆ)
{
    ದಶಮಾಂಶ	ret;
    ಪೂರ್ಣಾಂಕ		u;
    ಪೂರ್ಣಾಂಕ		nr;

    u = prt_get_unit(idx);
    ಆದರೆ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ಸಂಖ್ಯೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	nr = printer_opts[idx].ಸಂಖ್ಯೆ;

    ಆಯ್ಕೆ (u)
    {
	ಸ್ಥಿತಿ PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRT_UNIT_ಬಿಂದು:
	    ret = (ದ್ವಿಗುಣ)nr;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRT_UNIT_PERC:
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ret = physsize * (nr / 100.0);
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ಪುಟ_margins(
	ದ್ವಿಗುಣ	ಅಗಲ,
	ದ್ವಿಗುಣ	ಎತ್ತರ,
	ದ್ವಿಗುಣ	*ಎಡ_2,
	ದ್ವಿಗುಣ	*ಬಲ_2,
	ದ್ವಿಗುಣ	*ತುದಿ,
	ದ್ವಿಗುಣ	*bottom)
{
    *ಎಡ_2 = ಗಮ್ಯ_device_units(OPT_PRINT_ಎಡ, ಅಗಲ, 10);
    *ಬಲ_2 = ಅಗಲ - ಗಮ್ಯ_device_units(OPT_PRINT_ಬಲ, ಅಗಲ, 5);

    *ತುದಿ = ಗಮ್ಯ_device_units(OPT_PRINT_ತುದಿ, ಎತ್ತರ, 5);
    *bottom = ಎತ್ತರ - ಗಮ್ಯ_device_units(OPT_PRINT_BOT, ಎತ್ತರ, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_cpl(ಶೂನ್ಯ)
{
    ಆದರೆ (prt_use_ಸಂಖ್ಯೆ())
    {
	pctx.ಸಂಖ್ಯೆ_ಅಗಲ = PRINT_ಸಂಖ್ಯೆ_ಅಗಲ * pctx.char_ಅಗಲ;
	pctx.ಎಡ_margin += pctx.ಸಂಖ್ಯೆ_ಅಗಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	pctx.ಸಂಖ್ಯೆ_ಅಗಲ = 0.0;

    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)((pctx.ಬಲ_margin - pctx.ಎಡ_margin) / pctx.char_ಅಗಲ);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_lpp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ lpp;

    // Calculate lpp
    lpp = (ಪೂರ್ಣಾಂಕ)((pctx.bottom_margin - pctx.ತುದಿ_margin) / pctx.char_ಎತ್ತರ);

    // Adjust top margin if there is a header
    pctx.ತುದಿ_margin += pctx.char_ಎತ್ತರ * prt_ಶೀರ್ಷಿಕೆ_ಎತ್ತರ();

    ಹಿಂತಿರುಗಿ lpp - prt_ಶೀರ್ಷಿಕೆ_ಎತ್ತರ();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    ಸ್ಥಿರ PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    font = pango_font_description_from_string((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name);
    ಆದರೆ (font == ಖಾಲಿ)
    {
	semsg(_(e_unknown_printer_font_str), name);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Ensure a size is set
    ಆದರೆ (pango_font_description_get_ಗಾತ್ರ(font) <= 0)
	pango_font_description_ಗಣ_ಗಾತ್ರ(font, 10 * PANGO_SCALE);

    ಹಿಂತಿರುಗಿ font;
}

    ಪೂರ್ಣಾಂಕ
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	ಪೂರ್ಣಾಂಕ		forceit ಬಳಸದ)
{
    char_u	*paper_name;
    ಪೂರ್ಣಾಂಕ		paper_strlen;
    bool	portrait;
    ಪೂರ್ಣಾಂಕ		media;
    char_u	*filename = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    ಆದರೆ (gui.ಒಳ_use && !forceit && psettings->outfile == ಖಾಲಿ)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.ಪುಟ_ಅಗಲ, &pctx.ಪುಟ_ಎತ್ತರ);

	ಆದರೆ (filename == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	dialog = true;
    }
#ಆದರೆ_ಅಂತ್ಯ

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4_PRINT_DIALOG
    ಆದರೆ (!dialog)
#ಆದರೆ_ಅಂತ್ಯ
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	ಆದರೆ (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].ಅಕ್ಷರಸರಣಿ_ಉದ್ದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	ಗಾಗಿ (media = 0; media < (ಪೂರ್ಣಾಂಕ)PRT_MEDIASIZE_LEN; ++media)
	    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	ಆದರೆ (portrait)
	{
	    pctx.ಪುಟ_ಅಗಲ = prt_mediasize[media].ಅಗಲ;
	    pctx.ಪುಟ_ಎತ್ತರ = prt_mediasize[media].ಎತ್ತರ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    pctx.ಪುಟ_ಅಗಲ = prt_mediasize[media].ಎತ್ತರ;
	    pctx.ಪುಟ_ಎತ್ತರ = prt_mediasize[media].ಅಗಲ;
	}

	ಆದರೆ (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(print_formats); i++)
		ಆದರೆ (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].ಅಕ್ಷರಸರಣಿ_ಉದ್ದ) == 0)
		{
		    pctx.format = i;
		    ನಿಲ್ಲಿಸು;
		}
	}
    }
    psettings->user_abort = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4_PRINT_DIALOG
    ಆದರೆ (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (filename == ಖಾಲಿ)
    {
	// If the user didn't specify a file name, use a temp file.
	ಆದರೆ (psettings->outfile == ಖಾಲಿ)
	{
	    filename = vim_tempname('p', TRUE);
	    ಆದರೆ (filename == ಖಾಲಿ)
	    {
		emsg(_(e_cant_get_ತಾತ್ಕಾಲಿಕ_file_name));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    ಆದರೆ (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)filename,
		pctx.ಪುಟ_ಅಗಲ, pctx.ಪುಟ_ಎತ್ತರ);
    ಇಲ್ಲದಿದ್ದರೆ
	pctx.surface = cairo_pdf_surface_create((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)filename,
		pctx.ಪುಟ_ಅಗಲ, pctx.ಪುಟ_ಎತ್ತರ);

    pctx.filename = (char_u *)filename;

    ಆದರೆ (cairo_surface_ಸ್ಥಿತಿ(pctx.surface) != CAIRO_ಸ್ಥಿತಿ_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ಹಿಂತಿರುಗಿ FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.ಪಠ್ಯ_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    ಆದರೆ (pctx.font == ಖಾಲಿ)
    {
	mch_print_cleanup();
	ಹಿಂತಿರುಗಿ FAIL;
    }

    pango_context_ಗಣ_font_description(pctx.ಪಠ್ಯ_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_ಗಣ_resolution(pctx.ಪಠ್ಯ_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	ಪೂರ್ಣಾಂಕ		    ascent;
	ಪೂರ್ಣಾಂಕ		    descent;

	layout = pango_layout_new(pctx.ಪಠ್ಯ_context);
	pango_layout_ಗಣ_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, ಖಾಲಿ);

	pctx.char_ಅಗಲ =
	    (ದ್ವಿಗುಣ)pango_font_metrics_get_approximate_char_ಅಗಲ(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (ದ್ವಿಗುಣ)ascent / PANGO_SCALE;
	pctx.char_ಎತ್ತರ = (ದ್ವಿಗುಣ)(ascent + descent) / PANGO_SCALE;

	pctx.ಸಾಲು_spacing = (ದ್ವಿಗುಣ)
	    (pango_font_metrics_get_ಎತ್ತರ(metrics) - (ascent + descent))
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
	ದ್ವಿಗುಣ  ಎಡ_2;
	ದ್ವಿಗುಣ	ಬಲ_2;
	ದ್ವಿಗುಣ	ತುದಿ;
	ದ್ವಿಗುಣ	bottom;

	get_ಪುಟ_margins(pctx.ಪುಟ_ಅಗಲ, pctx.ಪುಟ_ಎತ್ತರ, &ಎಡ_2, &ಬಲ_2, &ತುದಿ,
		&bottom);
	pctx.ಎಡ_margin = ಎಡ_2;
	pctx.ಬಲ_margin = ಬಲ_2;
	pctx.ತುದಿ_margin = ತುದಿ;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_ಪ್ರತಿ_ಸಾಲು = get_cpl();
    psettings->ಸಾಲುಗಳು_ಪ್ರತಿ_ಪುಟ = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4_PRINT_DIALOG
    ಆದರೆ (!dialog)
#ಆದರೆ_ಅಂತ್ಯ
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	ಆದರೆ (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    ಆದರೆ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
emit_dsc_comment(ಅಕ್ಷರ *comment, ಅಕ್ಷರ *fmt, ...)
{
    ಸ್ಥಿರ ಅಕ್ಷರ buf[257];

    va_list ap;
    ಪೂರ್ಣಾಂಕ	    len = ಗಾತ್ರ(buf);
    ಪೂರ್ಣಾಂಕ	    o;
    ಅಕ್ಷರ    *s = buf;

    ಆದರೆ (fmt == ಖಾಲಿ)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    ಆದರೆ (len <= 0)
	ಹಿಂತಿರುಗಿ;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

ನಿರ್ಗಮಿಸು:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    ಪೂರ್ಣಾಂಕ
mch_print_begin(prt_settings_T *psettings)
{
    ಅಕ್ಷರ user[256];

    pctx.cur_x = 0.0;
    pctx.cur_ಸಾಲು = 0;
    pctx.draw_flags = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4_PRINT_DIALOG
    ಆದರೆ (pctx.dialog)
	ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!get_user_name((char_u *)user, ಗಾತ್ರ(user)))
	STRCPY(user, "Unknown");

    ಆದರೆ (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (ಅಕ್ಷರ *)psettings->jobname);
	emit_dsc_comment("For", user);
	emit_dsc_comment("Orientation",
		pctx.portrait ? "Portrait" : "Landscape");
	emit_dsc_comment("PageOrder", "Ascend");

	emit_dsc_comment("DocumentMedia",
		"%s %.2lf %.2lf 0 () ()",
		prt_mediasize[pctx.media].name,
		prt_mediasize[pctx.media].ಅಗಲ,
		prt_mediasize[pctx.media].ಎತ್ತರ);

	emit_dsc_comment("Requirements", "%s%s %s %s",
		pctx.duplex ? "duplex" : "",
		pctx.tumble ? "(tumble)" : "",
		pctx.collate ? "collate" : "",
		psettings->do_syntax ? "color" : "");

	// In defaults section (defaults for every page)
	emit_dsc_comment("BeginDefaults", ಖಾಲಿ);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	ಆದರೆ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ಖಾಲಿ);

	cairo_pdf_surface_ಗಣ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (ಅಕ್ಷರ *)str);
	ಆದರೆ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	ಆದರೆ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ಖಾಲಿ);
	cairo_pdf_surface_ಗಣ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (ಅಕ್ಷರ *)str);
	ಆದರೆ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_ಗಣ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_ಆವೃತ್ತಿ_LONG);
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

    ಶೂನ್ಯ
mch_print_ಕೊನೆ(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_ಮುಕ್ತಾಯ(pctx.surface);

    ಆದರೆ (psettings->outfile == ಖಾಲಿ && !got_int && !psettings->user_abort
	    && pctx.filename != ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	ಸಂದೇಶ(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ಆದರೆ (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_ಗಮ್ಯ_print_postscript_file));
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ(_("Print job sent."));
    }

    mch_print_cleanup();
}

    ಪೂರ್ಣಾಂಕ
mch_print_ಕೊನೆ_ಪುಟ(ಶೂನ್ಯ)
{
    cairo_show_ಪುಟ(pctx.cr);
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    ಪೂರ್ಣಾಂಕ
mch_print_begin_ಪುಟ(char_u *str ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ TRUE;
}

    ಪೂರ್ಣಾಂಕ
mch_print_blank_ಪುಟ(ಶೂನ್ಯ)
{
    mch_print_ಕೊನೆ_ಪುಟ();
    ಹಿಂತಿರುಗಿ TRUE;
}

    ಶೂನ್ಯ
mch_print_ಆರಂಭ_ಸಾಲು(ಪೂರ್ಣಾಂಕ margin, ಪೂರ್ಣಾಂಕ ಪುಟ_ಸಾಲು)
{
    pctx.cur_x = pctx.ಎಡ_margin;
    ಆದರೆ (margin)
	pctx.cur_x -= pctx.ಸಂಖ್ಯೆ_ಅಗಲ;

    pctx.cur_ಸಾಲು = ಪುಟ_ಸಾಲು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
long_ಗಮ್ಯ_rgb(long_u c, ದ್ವಿಗುಣ *r, ದ್ವಿಗುಣ *g, ದ್ವಿಗುಣ *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    ಪೂರ್ಣಾಂಕ
mch_print_ಪಠ್ಯ_ಹೊರಹೋಗು(char_u *textp, ಪೂರ್ಣಾಂಕ len)
{
    PangoAttrList	*attr_list;
    GList		*ಅಂಶ_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    ದ್ವಿಗುಣ		r, g, b;
    ದ್ವಿಗುಣ		ಅಗಲ = 0;
    ದ್ವಿಗುಣ		ಮುಂದಿನ_x;

    // Always convert to UTF-8, Pango expects that
    ಆದರೆ (output_conv.vc_type == CONV_NONE)
	str = textp;
    ಇಲ್ಲದಿದ್ದರೆ
	str = string_convert(&output_conv, textp, &len);

    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    attr_list = pango_attr_list_new();

#ವ್ಯಾಖ್ಯಾನಿಸು ಸೇರಿಸು_PANGO_ATTR(Attribute, AttrList, ಆರಂಭ_2, ಕೊನೆ_2)  \
    G_STMT_ಆರಂಭ{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->ಆರಂಭ_ಸೂಚ್ಯಂಕ = (ಆರಂಭ_2);		    \
	tmp_attr_->ಕೊನೆ_ಸೂಚ್ಯಂಕ = (ಕೊನೆ_2);			    \
	pango_attr_list_ಸೇರಿಸು((AttrList), tmp_attr_);	    \
    }G_STMT_ಕೊನೆ

    // Underline attribute does not work with pango_itemize().
    ಆದರೆ ((pctx.draw_flags & PRT_DRAW_BOLD))
	ಸೇರಿಸು_PANGO_ATTR(pango_attr_ತೂಕ_new(PANGO_ತೂಕ_BOLD),
		attr_list, 0, len);
    ಆದರೆ (pctx.draw_flags & PRT_DRAW_ITALIC)
	ಸೇರಿಸು_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಸೇರಿಸು_PANGO_ATTR

    ಅಂಶ_list = pango_itemize(pctx.ಪಠ್ಯ_context,
	    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)str, 0, len, attr_list, ಖಾಲಿ);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    ಆಗ (ಅಂಶ_list != ಖಾಲಿ)
    {
	Pango_ಅಂಶ   *ಅಂಶ;
	ದ್ವಿಗುಣ	    o;

	ಅಂಶ = (Pango_ಅಂಶ *)ಅಂಶ_list->ದತ್ತಾಂಶ;
	ಅಂಶ_list = g_list_delete_link(ಅಂಶ_list, ಅಂಶ_list);

	pango_shape_ತುಂಬಿದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)str + ಅಂಶ->ಸ್ಥಾನಾಂತರ, ಅಂಶ->ಉದ್ದಳತೆ,
		(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)str, len, &ಅಂಶ->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (ದ್ವಿಗುಣ)pango_glyph_string_get_ಅಗಲ(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	ಆದರೆ (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_ಗಮ್ಯ_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_ಗಣ_ಮೂಲ_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + ಅಗಲ,
		    GET_Y(pctx.cur_ಸಾಲು) - pctx.ಸಾಲು_spacing, o, pctx.char_ಎತ್ತರ);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_ಗಮ್ಯ_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_ಗಣ_ಮೂಲ_rgb(pctx.cr, r, g, b);
	cairo_move_ಗಮ್ಯ(pctx.cr, pctx.cur_x + ಅಗಲ, GET_ಪಠ್ಯ_Y(pctx.cur_ಸಾಲು));
	pango_cairo_show_glyph_string(pctx.cr, ಅಂಶ->analysis.font, glyphs);

	pango_ಅಂಶ_free(ಅಂಶ);
	ಅಗಲ += o;
    }

    // Draw underline if needed
    ಆದರೆ (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	ದ್ವಿಗುಣ y = GET_ಪಠ್ಯ_Y(pctx.cur_ಸಾಲು) - pctx.ul_pos;

	long_ಗಮ್ಯ_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_ಗಣ_ಮೂಲ_rgb(pctx.cr, r, g, b);
	cairo_ಗಣ_ಸಾಲು_ಅಗಲ(pctx.cr, pctx.ul_thickness);
	cairo_move_ಗಮ್ಯ(pctx.cr, pctx.cur_x, y);
	cairo_ಸಾಲು_ಗಮ್ಯ(pctx.cr, pctx.cur_x + ಅಗಲ, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += ಅಗಲ;

    ಮುಂದಿನ_x = pctx.cur_x + pctx.char_ಅಗಲ;

    ಆದರೆ (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    ಹಿಂತಿರುಗಿ (ಮುಂದಿನ_x > pctx.ಬಲ_margin) &&
	((ಮುಂದಿನ_x - pctx.ಬಲ_margin) > (pctx.ಬಲ_margin * 1e-5));
}

    ಶೂನ್ಯ
mch_print_ಗಣ_font(ಪೂರ್ಣಾಂಕ bold, ಪೂರ್ಣಾಂಕ italic, ಪೂರ್ಣಾಂಕ underline)
{
    pctx.draw_flags = 0;

    ಆದರೆ (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    ಆದರೆ (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    ಆದರೆ (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    ಶೂನ್ಯ
mch_print_ಗಣ_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    ಶೂನ್ಯ
mch_print_ಗಣ_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    ಶೂನ್ಯ
mch_print_cleanup(ಶೂನ್ಯ)
{
    g_clear_ಸೂಚಕ(&pctx.cr, cairo_destroy);
    g_clear_ಸೂಚಕ(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.ಪಠ್ಯ_context);
    g_clear_ಸೂಚಕ(&pctx.font, pango_font_description_free);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    ಆದರೆ (pctx.dialog)
	gui_gtk4_print_ಮುಕ್ತಾಯ();
#ಆದರೆ_ಅಂತ್ಯ

    g_clear_ಸೂಚಕ(&pctx.filename, vim_free);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_PRINT_PANGO
