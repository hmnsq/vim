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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINT_PANGO

#ਅਨ੍ਤਰ੍ਭਾਵ "hardcopy.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <cairo/cairo-ps.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <cairo/cairo-pdf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <pango/pango.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <pango/pangocairo.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRINT_DPI 72.0

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRT_DRAW_BOLD 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRT_DRAW_ITALIC 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRT_DRAW_UNDERLINE 4

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_Y(l) (pctx.ਸਿਖਰ_margin + (l) * pctx.char_ਉਚਾਈ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_ਪਾਠ_Y(l) (GET_Y(l) + pctx.char_ascent)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
ਸਥਿਰ ਬਣਤਰ
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *ਪਾਠ_context;

    PangoFontDescription *font;

    char_u *filename;

    ਪੂਰਨ media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    ਦੁੱਗਣਾ char_ਚੌੜਾਈ;
    ਦੁੱਗਣਾ char_ascent;
    ਦੁੱਗਣਾ char_ਉਚਾਈ;
    ਦੁੱਗਣਾ ਸਤਰ_spacing;
    ਦੁੱਗਣਾ ul_pos;
    ਦੁੱਗਣਾ ul_thickness;

    ਦੁੱਗਣਾ ਪੰਨਾ_ਚੌੜਾਈ;
    ਦੁੱਗਣਾ ਪੰਨਾ_ਉਚਾਈ;

    ਦੁੱਗਣਾ ਖੱਬਾ_margin;
    ਦੁੱਗਣਾ ਸੱਜਾ_margin;
    ਦੁੱਗਣਾ ਸਿਖਰ_margin;
    ਦੁੱਗਣਾ bottom_margin;

    ਦੁੱਗਣਾ ਸੰਖਿਆ_ਚੌੜਾਈ;

    // Current position to draw from
    ਦੁੱਗਣਾ cur_x;
    ਪੂਰਨ cur_ਸਤਰ;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4_PRINT_DIALOG
    bool dialog;
#ਜੇ_ਅੰਤ
} pctx;

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    ਸਥਿਰ ਦੁੱਗਣਾ
ਮੰਜ਼ਿਲ_device_units(ਪੂਰਨ idx, ਦੁੱਗਣਾ physsize, ਪੂਰਨ def_ਸੰਖਿਆ)
{
    ਦਸ਼ਮਲਵ	ret;
    ਪੂਰਨ		u;
    ਪੂਰਨ		nr;

    u = prt_get_unit(idx);
    ਜੇ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ਸੰਖਿਆ;
    }
    ਨਹੀਂਤਾਂ
	nr = printer_opts[idx].ਸੰਖਿਆ;

    ਚੋਣ (u)
    {
	ਹਾਲਤ PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    ਰੋਕੋ;
	ਹਾਲਤ PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    ਰੋਕੋ;
	ਹਾਲਤ PRT_UNIT_ਬਿੰਦੂ:
	    ret = (ਦੁੱਗਣਾ)nr;
	    ਰੋਕੋ;
	ਹਾਲਤ PRT_UNIT_PERC:
	ਮੂਲ:
	    ret = physsize * (nr / 100.0);
	    ਰੋਕੋ;
    }

    ਵਾਪਸ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਪੰਨਾ_margins(
	ਦੁੱਗਣਾ	ਚੌੜਾਈ,
	ਦੁੱਗਣਾ	ਉਚਾਈ,
	ਦੁੱਗਣਾ	*ਖੱਬਾ,
	ਦੁੱਗਣਾ	*ਸੱਜਾ,
	ਦੁੱਗਣਾ	*ਸਿਖਰ,
	ਦੁੱਗਣਾ	*bottom)
{
    *ਖੱਬਾ = ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_ਖੱਬਾ, ਚੌੜਾਈ, 10);
    *ਸੱਜਾ = ਚੌੜਾਈ - ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_ਸੱਜਾ, ਚੌੜਾਈ, 5);

    *ਸਿਖਰ = ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_ਸਿਖਰ, ਉਚਾਈ, 5);
    *bottom = ਉਚਾਈ - ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_BOT, ਉਚਾਈ, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    ਸਥਿਰ ਪੂਰਨ
get_cpl(ਖਾਲੀ)
{
    ਜੇ (prt_use_ਸੰਖਿਆ())
    {
	pctx.ਸੰਖਿਆ_ਚੌੜਾਈ = PRINT_ਸੰਖਿਆ_ਚੌੜਾਈ * pctx.char_ਚੌੜਾਈ;
	pctx.ਖੱਬਾ_margin += pctx.ਸੰਖਿਆ_ਚੌੜਾਈ;
    }
    ਨਹੀਂਤਾਂ
	pctx.ਸੰਖਿਆ_ਚੌੜਾਈ = 0.0;

    ਵਾਪਸ (ਪੂਰਨ)((pctx.ਸੱਜਾ_margin - pctx.ਖੱਬਾ_margin) / pctx.char_ਚੌੜਾਈ);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ਸਥਿਰ ਪੂਰਨ
get_lpp(ਖਾਲੀ)
{
    ਪੂਰਨ lpp;

    // Calculate lpp
    lpp = (ਪੂਰਨ)((pctx.bottom_margin - pctx.ਸਿਖਰ_margin) / pctx.char_ਉਚਾਈ);

    // Adjust top margin if there is a header
    pctx.ਸਿਖਰ_margin += pctx.char_ਉਚਾਈ * prt_ਸਿਰਲੇਖ_ਉਚਾਈ();

    ਵਾਪਸ lpp - prt_ਸਿਰਲੇਖ_ਉਚਾਈ();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    ਸਥਿਰ PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    font = pango_font_description_from_string((ਅਚਲ ਅੱਖਰ *)name);
    ਜੇ (font == ਨਿਰਾ)
    {
	semsg(_(e_unknown_printer_font_str), name);
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Ensure a size is set
    ਜੇ (pango_font_description_get_ਮਾਪ(font) <= 0)
	pango_font_description_ਸੈੱਟ_ਮਾਪ(font, 10 * PANGO_SCALE);

    ਵਾਪਸ font;
}

    ਪੂਰਨ
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	ਪੂਰਨ		forceit ਅਣਵਰਤਿਆ)
{
    char_u	*paper_name;
    ਪੂਰਨ		paper_strlen;
    bool	portrait;
    ਪੂਰਨ		media;
    char_u	*filename = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    ਜੇ (gui.ਵਿੱਚ_use && !forceit && psettings->outfile == ਨਿਰਾ)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.ਪੰਨਾ_ਚੌੜਾਈ, &pctx.ਪੰਨਾ_ਉਚਾਈ);

	ਜੇ (filename == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	dialog = true;
    }
#ਜੇ_ਅੰਤ

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4_PRINT_DIALOG
    ਜੇ (!dialog)
#ਜੇ_ਅੰਤ
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	ਜੇ (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].ਲੜੀ_ਲੰਬਾਈ;
	}
	ਨਹੀਂਤਾਂ
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	ਲਈ (media = 0; media < (ਪੂਰਨ)PRT_MEDIASIZE_LEN; ++media)
	    ਜੇ ((ਪੂਰਨ)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		ਰੋਕੋ;
	ਜੇ (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	ਜੇ (portrait)
	{
	    pctx.ਪੰਨਾ_ਚੌੜਾਈ = prt_mediasize[media].ਚੌੜਾਈ;
	    pctx.ਪੰਨਾ_ਉਚਾਈ = prt_mediasize[media].ਉਚਾਈ;
	}
	ਨਹੀਂਤਾਂ
	{
	    pctx.ਪੰਨਾ_ਚੌੜਾਈ = prt_mediasize[media].ਉਚਾਈ;
	    pctx.ਪੰਨਾ_ਉਚਾਈ = prt_mediasize[media].ਚੌੜਾਈ;
	}

	ਜੇ (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(print_formats); i++)
		ਜੇ (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].ਲੜੀ_ਲੰਬਾਈ) == 0)
		{
		    pctx.format = i;
		    ਰੋਕੋ;
		}
	}
    }
    psettings->user_abort = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4_PRINT_DIALOG
    ਜੇ (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#ਜੇ_ਅੰਤ

    ਜੇ (filename == ਨਿਰਾ)
    {
	// If the user didn't specify a file name, use a temp file.
	ਜੇ (psettings->outfile == ਨਿਰਾ)
	{
	    filename = vim_tempname('p', TRUE);
	    ਜੇ (filename == ਨਿਰਾ)
	    {
		emsg(_(e_cant_get_ਆਰਜ਼ੀ_file_name));
		ਵਾਪਸ FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    ਜੇ (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ਅਚਲ ਅੱਖਰ *)filename,
		pctx.ਪੰਨਾ_ਚੌੜਾਈ, pctx.ਪੰਨਾ_ਉਚਾਈ);
    ਨਹੀਂਤਾਂ
	pctx.surface = cairo_pdf_surface_create((ਅਚਲ ਅੱਖਰ *)filename,
		pctx.ਪੰਨਾ_ਚੌੜਾਈ, pctx.ਪੰਨਾ_ਉਚਾਈ);

    pctx.filename = (char_u *)filename;

    ਜੇ (cairo_surface_ਸਥਿਤੀ(pctx.surface) != CAIRO_ਸਥਿਤੀ_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ਵਾਪਸ FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.ਪਾਠ_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    ਜੇ (pctx.font == ਨਿਰਾ)
    {
	mch_print_cleanup();
	ਵਾਪਸ FAIL;
    }

    pango_context_ਸੈੱਟ_font_description(pctx.ਪਾਠ_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_ਸੈੱਟ_resolution(pctx.ਪਾਠ_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	ਪੂਰਨ		    ascent;
	ਪੂਰਨ		    descent;

	layout = pango_layout_new(pctx.ਪਾਠ_context);
	pango_layout_ਸੈੱਟ_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, ਨਿਰਾ);

	pctx.char_ਚੌੜਾਈ =
	    (ਦੁੱਗਣਾ)pango_font_metrics_get_approximate_char_ਚੌੜਾਈ(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (ਦੁੱਗਣਾ)ascent / PANGO_SCALE;
	pctx.char_ਉਚਾਈ = (ਦੁੱਗਣਾ)(ascent + descent) / PANGO_SCALE;

	pctx.ਸਤਰ_spacing = (ਦੁੱਗਣਾ)
	    (pango_font_metrics_get_ਉਚਾਈ(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (ਦੁੱਗਣਾ)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (ਦੁੱਗਣਾ)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	ਦੁੱਗਣਾ  ਖੱਬਾ;
	ਦੁੱਗਣਾ	ਸੱਜਾ;
	ਦੁੱਗਣਾ	ਸਿਖਰ;
	ਦੁੱਗਣਾ	bottom;

	get_ਪੰਨਾ_margins(pctx.ਪੰਨਾ_ਚੌੜਾਈ, pctx.ਪੰਨਾ_ਉਚਾਈ, &ਖੱਬਾ, &ਸੱਜਾ, &ਸਿਖਰ,
		&bottom);
	pctx.ਖੱਬਾ_margin = ਖੱਬਾ;
	pctx.ਸੱਜਾ_margin = ਸੱਜਾ;
	pctx.ਸਿਖਰ_margin = ਸਿਖਰ;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_ਪ੍ਰਤੀ_ਸਤਰ = get_cpl();
    psettings->ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਪੰਨਾ = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4_PRINT_DIALOG
    ਜੇ (!dialog)
#ਜੇ_ਅੰਤ
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	ਜੇ (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    ਜੇ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    ਸਥਿਰ ਖਾਲੀ
emit_dsc_comment(ਅੱਖਰ *comment, ਅੱਖਰ *fmt, ...)
{
    ਸਥਿਰ ਅੱਖਰ buf[257];

    va_list ap;
    ਪੂਰਨ	    len = ਆਕਾਰ(buf);
    ਪੂਰਨ	    o;
    ਅੱਖਰ    *s = buf;

    ਜੇ (fmt == ਨਿਰਾ)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }
    ਨਹੀਂਤਾਂ
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    ਜੇ (len <= 0)
	ਵਾਪਸ;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

ਬਾਹਰ_ਨਿਕਲੋ:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    ਪੂਰਨ
mch_print_begin(prt_settings_T *psettings)
{
    ਅੱਖਰ user[256];

    pctx.cur_x = 0.0;
    pctx.cur_ਸਤਰ = 0;
    pctx.draw_flags = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4_PRINT_DIALOG
    ਜੇ (pctx.dialog)
	ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ

    ਜੇ (!get_user_name((char_u *)user, ਆਕਾਰ(user)))
	STRCPY(user, "Unknown");

    ਜੇ (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (ਅੱਖਰ *)psettings->jobname);
	emit_dsc_comment("For", user);
	emit_dsc_comment("Orientation",
		pctx.portrait ? "Portrait" : "Landscape");
	emit_dsc_comment("PageOrder", "Ascend");

	emit_dsc_comment("DocumentMedia",
		"%s %.2lf %.2lf 0 () ()",
		prt_mediasize[pctx.media].name,
		prt_mediasize[pctx.media].ਚੌੜਾਈ,
		prt_mediasize[pctx.media].ਉਚਾਈ);

	emit_dsc_comment("Requirements", "%s%s %s %s",
		pctx.duplex ? "duplex" : "",
		pctx.tumble ? "(tumble)" : "",
		pctx.collate ? "collate" : "",
		psettings->do_syntax ? "color" : "");

	// In defaults section (defaults for every page)
	emit_dsc_comment("BeginDefaults", ਨਿਰਾ);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	ਜੇ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ਨਿਰਾ);

	cairo_pdf_surface_ਸੈੱਟ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (ਅੱਖਰ *)str);
	ਜੇ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	ਜੇ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ਨਿਰਾ);
	cairo_pdf_surface_ਸੈੱਟ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (ਅੱਖਰ *)str);
	ਜੇ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_ਸੈੱਟ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_ਸੰਸਕਰਣ_LONG);
    }

    ਵਾਪਸ TRUE;
}

    ਖਾਲੀ
mch_print_ਸਿਰਾ(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_ਅੰਤ(pctx.surface);

    ਜੇ (psettings->outfile == ਨਿਰਾ && !got_int && !psettings->user_abort
	    && pctx.filename != ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#ਜੇ_ਅੰਤ
       )
    {
	ਸੁਨੇਹਾ(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ਜੇ (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_ਮੰਜ਼ਿਲ_print_postscript_file));
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ(_("Print job sent."));
    }

    mch_print_cleanup();
}

    ਪੂਰਨ
mch_print_ਸਿਰਾ_ਪੰਨਾ(ਖਾਲੀ)
{
    cairo_show_ਪੰਨਾ(pctx.cr);
    ਵਾਪਸ TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    ਪੂਰਨ
mch_print_begin_ਪੰਨਾ(char_u *str ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ TRUE;
}

    ਪੂਰਨ
mch_print_blank_ਪੰਨਾ(ਖਾਲੀ)
{
    mch_print_ਸਿਰਾ_ਪੰਨਾ();
    ਵਾਪਸ TRUE;
}

    ਖਾਲੀ
mch_print_ਸ਼ੁਰੂ_ਸਤਰ(ਪੂਰਨ margin, ਪੂਰਨ ਪੰਨਾ_ਸਤਰ)
{
    pctx.cur_x = pctx.ਖੱਬਾ_margin;
    ਜੇ (margin)
	pctx.cur_x -= pctx.ਸੰਖਿਆ_ਚੌੜਾਈ;

    pctx.cur_ਸਤਰ = ਪੰਨਾ_ਸਤਰ;
}

    ਸਥਿਰ ਖਾਲੀ
long_ਮੰਜ਼ਿਲ_rgb(long_u c, ਦੁੱਗਣਾ *r, ਦੁੱਗਣਾ *g, ਦੁੱਗਣਾ *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    ਪੂਰਨ
mch_print_ਪਾਠ_ਬਾਹਰਲਾ(char_u *textp, ਪੂਰਨ len)
{
    PangoAttrList	*attr_list;
    GList		*ਵਸਤੂ_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    ਦੁੱਗਣਾ		r, g, b;
    ਦੁੱਗਣਾ		ਚੌੜਾਈ = 0;
    ਦੁੱਗਣਾ		ਅਗਲਾ_x;

    // Always convert to UTF-8, Pango expects that
    ਜੇ (output_conv.vc_type == CONV_NONE)
	str = textp;
    ਨਹੀਂਤਾਂ
	str = string_convert(&output_conv, textp, &len);

    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    attr_list = pango_attr_list_new();

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਾਓ_PANGO_ATTR(Attribute, AttrList, ਸ਼ੁਰੂ_2, ਸਿਰਾ_2)  \
    G_STMT_ਸ਼ੁਰੂ{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = (ਸ਼ੁਰੂ_2);		    \
	tmp_attr_->ਸਿਰਾ_ਸੂਚਕਾਂਕ = (ਸਿਰਾ_2);			    \
	pango_attr_list_ਪਾਓ((AttrList), tmp_attr_);	    \
    }G_STMT_ਸਿਰਾ

    // Underline attribute does not work with pango_itemize().
    ਜੇ ((pctx.draw_flags & PRT_DRAW_BOLD))
	ਪਾਓ_PANGO_ATTR(pango_attr_ਭਾਰ_new(PANGO_ਭਾਰ_BOLD),
		attr_list, 0, len);
    ਜੇ (pctx.draw_flags & PRT_DRAW_ITALIC)
	ਪਾਓ_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਪਾਓ_PANGO_ATTR

    ਵਸਤੂ_list = pango_itemize(pctx.ਪਾਠ_context,
	    (ਅਚਲ ਅੱਖਰ *)str, 0, len, attr_list, ਨਿਰਾ);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    ਜਦੋਂ (ਵਸਤੂ_list != ਨਿਰਾ)
    {
	Pango_ਵਸਤੂ   *ਵਸਤੂ;
	ਦੁੱਗਣਾ	    o;

	ਵਸਤੂ = (Pango_ਵਸਤੂ *)ਵਸਤੂ_list->ਅੰਕੜੇ;
	ਵਸਤੂ_list = g_list_delete_link(ਵਸਤੂ_list, ਵਸਤੂ_list);

	pango_shape_ਭਰਿਆ((ਅਚਲ ਅੱਖਰ *)str + ਵਸਤੂ->ਵਿੱਥ, ਵਸਤੂ->ਲੰਮਾਈ,
		(ਅਚਲ ਅੱਖਰ *)str, len, &ਵਸਤੂ->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (ਦੁੱਗਣਾ)pango_glyph_string_get_ਚੌੜਾਈ(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	ਜੇ (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_ਮੰਜ਼ਿਲ_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_ਸੈੱਟ_ਸਰੋਤ_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + ਚੌੜਾਈ,
		    GET_Y(pctx.cur_ਸਤਰ) - pctx.ਸਤਰ_spacing, o, pctx.char_ਉਚਾਈ);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_ਮੰਜ਼ਿਲ_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_ਸੈੱਟ_ਸਰੋਤ_rgb(pctx.cr, r, g, b);
	cairo_move_ਮੰਜ਼ਿਲ(pctx.cr, pctx.cur_x + ਚੌੜਾਈ, GET_ਪਾਠ_Y(pctx.cur_ਸਤਰ));
	pango_cairo_show_glyph_string(pctx.cr, ਵਸਤੂ->analysis.font, glyphs);

	pango_ਵਸਤੂ_free(ਵਸਤੂ);
	ਚੌੜਾਈ += o;
    }

    // Draw underline if needed
    ਜੇ (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	ਦੁੱਗਣਾ y = GET_ਪਾਠ_Y(pctx.cur_ਸਤਰ) - pctx.ul_pos;

	long_ਮੰਜ਼ਿਲ_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_ਸੈੱਟ_ਸਰੋਤ_rgb(pctx.cr, r, g, b);
	cairo_ਸੈੱਟ_ਸਤਰ_ਚੌੜਾਈ(pctx.cr, pctx.ul_thickness);
	cairo_move_ਮੰਜ਼ਿਲ(pctx.cr, pctx.cur_x, y);
	cairo_ਸਤਰ_ਮੰਜ਼ਿਲ(pctx.cr, pctx.cur_x + ਚੌੜਾਈ, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += ਚੌੜਾਈ;

    ਅਗਲਾ_x = pctx.cur_x + pctx.char_ਚੌੜਾਈ;

    ਜੇ (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    ਵਾਪਸ (ਅਗਲਾ_x > pctx.ਸੱਜਾ_margin) &&
	((ਅਗਲਾ_x - pctx.ਸੱਜਾ_margin) > (pctx.ਸੱਜਾ_margin * 1e-5));
}

    ਖਾਲੀ
mch_print_ਸੈੱਟ_font(ਪੂਰਨ bold, ਪੂਰਨ italic, ਪੂਰਨ underline)
{
    pctx.draw_flags = 0;

    ਜੇ (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    ਜੇ (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    ਜੇ (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    ਖਾਲੀ
mch_print_ਸੈੱਟ_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    ਖਾਲੀ
mch_print_ਸੈੱਟ_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    ਖਾਲੀ
mch_print_cleanup(ਖਾਲੀ)
{
    g_clear_ਸੂਚਕ(&pctx.cr, cairo_destroy);
    g_clear_ਸੂਚਕ(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.ਪਾਠ_context);
    g_clear_ਸੂਚਕ(&pctx.font, pango_font_description_free);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    ਜੇ (pctx.dialog)
	gui_gtk4_print_ਅੰਤ();
#ਜੇ_ਅੰਤ

    g_clear_ਸੂਚਕ(&pctx.filename, vim_free);
}

#ਜੇ_ਅੰਤ // FEAT_PRINT_PANGO
