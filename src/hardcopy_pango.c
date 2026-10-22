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

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "version.h"

#ifdef FEAT_PRINT_PANGO

#അന്തര്ഭാവ "hardcopy.h"

#അന്തര്ഭാവ <cairo/cairo-ps.h>
#അന്തര്ഭാവ <cairo/cairo-pdf.h>
#അന്തര്ഭാവ <pango/pango.h>
#അന്തര്ഭാവ <pango/pangocairo.h>

#define PRINT_DPI 72.0

#define PRT_DRAW_BOLD 1
#define PRT_DRAW_ITALIC 2
#define PRT_DRAW_UNDERLINE 4

#define GET_Y(l) (pctx.മുകൾ_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

തരനിർവചനം എണ്ണൽ
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
സ്ഥിതം ഘടന
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    പൂർണ്ണസംഖ്യ media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    ഇരട്ട char_width;
    ഇരട്ട char_ascent;
    ഇരട്ട char_height;
    ഇരട്ട വരി_spacing;
    ഇരട്ട ul_pos;
    ഇരട്ട ul_thickness;

    ഇരട്ട page_width;
    ഇരട്ട page_height;

    ഇരട്ട ഇടത്_margin;
    ഇരട്ട വലത്_margin;
    ഇരട്ട മുകൾ_margin;
    ഇരട്ട bottom_margin;

    ഇരട്ട സംഖ്യ_width;

    // Current position to draw from
    ഇരട്ട cur_x;
    പൂർണ്ണസംഖ്യ cur_വരി;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

സ്ഥിതം സ്ഥിരം അക്ഷരം *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    സ്ഥിതം ഇരട്ട
to_device_units(പൂർണ്ണസംഖ്യ idx, ഇരട്ട physsize, പൂർണ്ണസംഖ്യ def_സംഖ്യ)
{
    ദശാംശം	ret;
    പൂർണ്ണസംഖ്യ		u;
    പൂർണ്ണസംഖ്യ		nr;

    u = prt_get_unit(idx);
    എങ്കിൽ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_സംഖ്യ;
    }
    അല്ലെങ്കിൽ
	nr = printer_opts[idx].സംഖ്യ;

    തിരഞ്ഞെടുപ്പ് (u)
    {
	അവസ്ഥ PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    നിർത്തുക;
	അവസ്ഥ PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    നിർത്തുക;
	അവസ്ഥ PRT_UNIT_POINT:
	    ret = (ഇരട്ട)nr;
	    നിർത്തുക;
	അവസ്ഥ PRT_UNIT_PERC:
	സ്ഥിരസ്ഥിതി:
	    ret = physsize * (nr / 100.0);
	    നിർത്തുക;
    }

    തിരികെ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    സ്ഥിതം ശൂന്യം
get_page_margins(
	ഇരട്ട	width,
	ഇരട്ട	height,
	ഇരട്ട	*ഇടത്,
	ഇരട്ട	*വലത്,
	ഇരട്ട	*മുകൾ,
	ഇരട്ട	*bottom)
{
    *ഇടത് = to_device_units(OPT_PRINT_ഇടത്, width, 10);
    *വലത് = width - to_device_units(OPT_PRINT_വലത്, width, 5);

    *മുകൾ = to_device_units(OPT_PRINT_മുകൾ, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_cpl(ശൂന്യം)
{
    എങ്കിൽ (prt_use_സംഖ്യ())
    {
	pctx.സംഖ്യ_width = PRINT_സംഖ്യ_WIDTH * pctx.char_width;
	pctx.ഇടത്_margin += pctx.സംഖ്യ_width;
    }
    അല്ലെങ്കിൽ
	pctx.സംഖ്യ_width = 0.0;

    തിരികെ (പൂർണ്ണസംഖ്യ)((pctx.വലത്_margin - pctx.ഇടത്_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_lpp(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ lpp;

    // Calculate lpp
    lpp = (പൂർണ്ണസംഖ്യ)((pctx.bottom_margin - pctx.മുകൾ_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.മുകൾ_margin += pctx.char_height * prt_header_height();

    തിരികെ lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    സ്ഥിതം PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ ശൂന്യ;

    font = pango_font_description_from_string((സ്ഥിരം അക്ഷരം *)name);
    എങ്കിൽ (font == ശൂന്യ)
    {
	semsg(_(e_unknown_printer_font_str), name);
	തിരികെ ശൂന്യ;
    }

    // Ensure a size is set
    എങ്കിൽ (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    തിരികെ font;
}

    പൂർണ്ണസംഖ്യ
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	പൂർണ്ണസംഖ്യ		forceit UNUSED)
{
    char_u	*paper_name;
    പൂർണ്ണസംഖ്യ		paper_strlen;
    bool	portrait;
    പൂർണ്ണസംഖ്യ		media;
    char_u	*filename = ശൂന്യ;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    എങ്കിൽ (gui.in_use && !forceit && psettings->outfile == ശൂന്യ)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	എങ്കിൽ (filename == ശൂന്യ)
	    തിരികെ FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    എങ്കിൽ (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	എങ്കിൽ (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
	}
	അല്ലെങ്കിൽ
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	വേണ്ടി (media = 0; media < (പൂർണ്ണസംഖ്യ)PRT_MEDIASIZE_LEN; ++media)
	    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		നിർത്തുക;
	എങ്കിൽ (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	എങ്കിൽ (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	അല്ലെങ്കിൽ
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	എങ്കിൽ (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(print_formats); i++)
		എങ്കിൽ (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    നിർത്തുക;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    എങ്കിൽ (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    എങ്കിൽ (filename == ശൂന്യ)
    {
	// If the user didn't specify a file name, use a temp file.
	എങ്കിൽ (psettings->outfile == ശൂന്യ)
	{
	    filename = vim_tempname('p', TRUE);
	    എങ്കിൽ (filename == ശൂന്യ)
	    {
		emsg(_(e_cant_get_temp_file_name));
		തിരികെ FAIL;
	    }
	}
	അല്ലെങ്കിൽ
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    എങ്കിൽ (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((സ്ഥിരം അക്ഷരം *)filename,
		pctx.page_width, pctx.page_height);
    അല്ലെങ്കിൽ
	pctx.surface = cairo_pdf_surface_create((സ്ഥിരം അക്ഷരം *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    എങ്കിൽ (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	തിരികെ FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    എങ്കിൽ (pctx.font == ശൂന്യ)
    {
	mch_print_cleanup();
	തിരികെ FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	പൂർണ്ണസംഖ്യ		    ascent;
	പൂർണ്ണസംഖ്യ		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, ശൂന്യ);

	pctx.char_width =
	    (ഇരട്ട)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (ഇരട്ട)ascent / PANGO_SCALE;
	pctx.char_height = (ഇരട്ട)(ascent + descent) / PANGO_SCALE;

	pctx.വരി_spacing = (ഇരട്ട)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (ഇരട്ട)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (ഇരട്ട)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	ഇരട്ട  ഇടത്;
	ഇരട്ട	വലത്;
	ഇരട്ട	മുകൾ;
	ഇരട്ട	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &ഇടത്, &വലത്, &മുകൾ,
		&bottom);
	pctx.ഇടത്_margin = ഇടത്;
	pctx.വലത്_margin = വലത്;
	pctx.മുകൾ_margin = മുകൾ;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_വരി = get_cpl();
    psettings->വരികൾ_per_page = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ifdef USE_GTK4_PRINT_DIALOG
    എങ്കിൽ (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	എങ്കിൽ (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    എങ്കിൽ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    തിരികെ OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    സ്ഥിതം ശൂന്യം
emit_dsc_comment(അക്ഷരം *comment, അക്ഷരം *fmt, ...)
{
    സ്ഥിതം അക്ഷരം buf[257];

    va_list ap;
    പൂർണ്ണസംഖ്യ	    len = വലുപ്പം(buf);
    പൂർണ്ണസംഖ്യ	    o;
    അക്ഷരം    *s = buf;

    എങ്കിൽ (fmt == ശൂന്യ)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	പോകുക exit;
    }
    അല്ലെങ്കിൽ
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    എങ്കിൽ (len <= 0)
	തിരികെ;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    പൂർണ്ണസംഖ്യ
mch_print_begin(prt_settings_T *psettings)
{
    അക്ഷരം user[256];

    pctx.cur_x = 0.0;
    pctx.cur_വരി = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    എങ്കിൽ (pctx.dialog)
	തിരികെ TRUE;
#endif

    എങ്കിൽ (!get_user_name((char_u *)user, വലുപ്പം(user)))
	STRCPY(user, "Unknown");

    എങ്കിൽ (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (അക്ഷരം *)psettings->jobname);
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
	emit_dsc_comment("BeginDefaults", ശൂന്യ);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", ശൂന്യ);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	എങ്കിൽ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ശൂന്യ);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (അക്ഷരം *)str);
	എങ്കിൽ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	എങ്കിൽ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ശൂന്യ);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (അക്ഷരം *)str);
	എങ്കിൽ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    തിരികെ TRUE;
}

    ശൂന്യം
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    എങ്കിൽ (psettings->outfile == ശൂന്യ && !got_int && !psettings->user_abort
	    && pctx.filename != ശൂന്യ
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	സന്ദേശം(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	എങ്കിൽ (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	അല്ലെങ്കിൽ
	    സന്ദേശം(_("Print job sent."));
    }

    mch_print_cleanup();
}

    പൂർണ്ണസംഖ്യ
mch_print_end_page(ശൂന്യം)
{
    cairo_show_page(pctx.cr);
    തിരികെ TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    പൂർണ്ണസംഖ്യ
mch_print_begin_page(char_u *str UNUSED)
{
    തിരികെ TRUE;
}

    പൂർണ്ണസംഖ്യ
mch_print_blank_page(ശൂന്യം)
{
    mch_print_end_page();
    തിരികെ TRUE;
}

    ശൂന്യം
mch_print_start_വരി(പൂർണ്ണസംഖ്യ margin, പൂർണ്ണസംഖ്യ page_വരി)
{
    pctx.cur_x = pctx.ഇടത്_margin;
    എങ്കിൽ (margin)
	pctx.cur_x -= pctx.സംഖ്യ_width;

    pctx.cur_വരി = page_വരി;
}

    സ്ഥിതം ശൂന്യം
long_to_rgb(long_u c, ഇരട്ട *r, ഇരട്ട *g, ഇരട്ട *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    പൂർണ്ണസംഖ്യ
mch_print_text_out(char_u *textp, പൂർണ്ണസംഖ്യ len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    ഇരട്ട		r, g, b;
    ഇരട്ട		width = 0;
    ഇരട്ട		next_x;

    // Always convert to UTF-8, Pango expects that
    എങ്കിൽ (output_conv.vc_type == CONV_NONE)
	str = textp;
    അല്ലെങ്കിൽ
	str = string_convert(&output_conv, textp, &len);

    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ TRUE;

    attr_list = pango_attr_list_new();

#define തിരുകുക_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_തിരുകുക((AttrList), tmp_attr_);	    \
    }G_STMT_END

    // Underline attribute does not work with pango_itemize().
    എങ്കിൽ ((pctx.draw_flags & PRT_DRAW_BOLD))
	തിരുകുക_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    എങ്കിൽ (pctx.draw_flags & PRT_DRAW_ITALIC)
	തിരുകുക_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef തിരുകുക_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (സ്ഥിരം അക്ഷരം *)str, 0, len, attr_list, ശൂന്യ);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    ആകുമ്പോൾ (item_list != ശൂന്യ)
    {
	PangoItem   *item;
	ഇരട്ട	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((സ്ഥിരം അക്ഷരം *)str + item->offset, item->length,
		(സ്ഥിരം അക്ഷരം *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (ഇരട്ട)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	എങ്കിൽ (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_വരി) - pctx.വരി_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_വരി));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    എങ്കിൽ (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	ഇരട്ട y = GET_TEXT_Y(pctx.cur_വരി) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_വരി_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_വരി_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    next_x = pctx.cur_x + pctx.char_width;

    എങ്കിൽ (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    തിരികെ (next_x > pctx.വലത്_margin) &&
	((next_x - pctx.വലത്_margin) > (pctx.വലത്_margin * 1e-5));
}

    ശൂന്യം
mch_print_set_font(പൂർണ്ണസംഖ്യ bold, പൂർണ്ണസംഖ്യ italic, പൂർണ്ണസംഖ്യ underline)
{
    pctx.draw_flags = 0;

    എങ്കിൽ (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    എങ്കിൽ (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    എങ്കിൽ (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    ശൂന്യം
mch_print_set_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    ശൂന്യം
mch_print_set_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    ശൂന്യം
mch_print_cleanup(ശൂന്യം)
{
    g_clear_pointer(&pctx.cr, cairo_destroy);
    g_clear_pointer(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.text_context);
    g_clear_pointer(&pctx.font, pango_font_description_free);

#ifdef USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    എങ്കിൽ (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
