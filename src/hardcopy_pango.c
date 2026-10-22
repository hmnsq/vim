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

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

#ifdef FEAT_PRINT_PANGO

#अन्तर्भाव "hardcopy.h"

#अन्तर्भाव <cairo/cairo-ps.h>
#अन्तर्भाव <cairo/cairo-pdf.h>
#अन्तर्भाव <pango/pango.h>
#अन्तर्भाव <pango/pangocairo.h>

#define PRINT_DPI 72.0

#define PRT_DRAW_BOLD 1
#define PRT_DRAW_ITALIC 2
#define PRT_DRAW_UNDERLINE 4

#define GET_Y(l) (pctx.शिखरम्_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

प्रकारनिर्देश गणना
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
स्थिर संरचना
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    पूर्णाङ्क media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    द्विगुण char_width;
    द्विगुण char_ascent;
    द्विगुण char_height;
    द्विगुण पङ्क्तिः_spacing;
    द्विगुण ul_pos;
    द्विगुण ul_thickness;

    द्विगुण page_width;
    द्विगुण page_height;

    द्विगुण वामः_margin;
    द्विगुण दक्षिणः_margin;
    द्विगुण शिखरम्_margin;
    द्विगुण bottom_margin;

    द्विगुण सङ्ख्या_width;

    // Current position to draw from
    द्विगुण cur_x;
    पूर्णाङ्क cur_पङ्क्तिः;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

स्थिर ध्रुव वर्ण *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    स्थिर द्विगुण
to_device_units(पूर्णाङ्क idx, द्विगुण physsize, पूर्णाङ्क def_सङ्ख्या)
{
    भिन्न	ret;
    पूर्णाङ्क		u;
    पूर्णाङ्क		nr;

    u = prt_get_unit(idx);
    सचे (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_सङ्ख्या;
    }
    अञ्ञथा
	nr = printer_opts[idx].सङ्ख्या;

    विकल्प (u)
    {
	पक्ष PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    भङ्ग;
	पक्ष PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    भङ्ग;
	पक्ष PRT_UNIT_POINT:
	    ret = (द्विगुण)nr;
	    भङ्ग;
	पक्ष PRT_UNIT_PERC:
	स्वभाव:
	    ret = physsize * (nr / 100.0);
	    भङ्ग;
    }

    पटिनिवत्त ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    स्थिर शून्य
get_page_margins(
	द्विगुण	width,
	द्विगुण	height,
	द्विगुण	*वामः,
	द्विगुण	*दक्षिणः,
	द्विगुण	*शिखरम्,
	द्विगुण	*bottom)
{
    *वामः = to_device_units(OPT_PRINT_वामः, width, 10);
    *दक्षिणः = width - to_device_units(OPT_PRINT_दक्षिणः, width, 5);

    *शिखरम् = to_device_units(OPT_PRINT_शिखरम्, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    स्थिर पूर्णाङ्क
get_cpl(शून्य)
{
    सचे (prt_use_सङ्ख्या())
    {
	pctx.सङ्ख्या_width = PRINT_सङ्ख्या_WIDTH * pctx.char_width;
	pctx.वामः_margin += pctx.सङ्ख्या_width;
    }
    अञ्ञथा
	pctx.सङ्ख्या_width = 0.0;

    पटिनिवत्त (पूर्णाङ्क)((pctx.दक्षिणः_margin - pctx.वामः_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    स्थिर पूर्णाङ्क
get_lpp(शून्य)
{
    पूर्णाङ्क lpp;

    // Calculate lpp
    lpp = (पूर्णाङ्क)((pctx.bottom_margin - pctx.शिखरम्_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.शिखरम्_margin += pctx.char_height * prt_header_height();

    पटिनिवत्त lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    स्थिर PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    सचे (name == रिक्त)
	पटिनिवत्त रिक्त;

    font = pango_font_description_from_string((ध्रुव वर्ण *)name);
    सचे (font == रिक्त)
    {
	semsg(_(e_unknown_printer_font_str), name);
	पटिनिवत्त रिक्त;
    }

    // Ensure a size is set
    सचे (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    पटिनिवत्त font;
}

    पूर्णाङ्क
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	पूर्णाङ्क		forceit UNUSED)
{
    char_u	*paper_name;
    पूर्णाङ्क		paper_strlen;
    bool	portrait;
    पूर्णाङ्क		media;
    char_u	*filename = रिक्त;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    सचे (gui.in_use && !forceit && psettings->outfile == रिक्त)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	सचे (filename == रिक्त)
	    पटिनिवत्त FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    सचे (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	सचे (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
	}
	अञ्ञथा
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	अत्थाय (media = 0; media < (पूर्णाङ्क)PRT_MEDIASIZE_LEN; ++media)
	    सचे ((पूर्णाङ्क)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		भङ्ग;
	सचे (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	सचे (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	अञ्ञथा
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	सचे (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    अत्थाय (पूर्णाङ्क i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(print_formats); i++)
		सचे (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    भङ्ग;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    सचे (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    सचे (filename == रिक्त)
    {
	// If the user didn't specify a file name, use a temp file.
	सचे (psettings->outfile == रिक्त)
	{
	    filename = vim_tempname('p', TRUE);
	    सचे (filename == रिक्त)
	    {
		emsg(_(e_cant_get_temp_file_name));
		पटिनिवत्त FAIL;
	    }
	}
	अञ्ञथा
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    सचे (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ध्रुव वर्ण *)filename,
		pctx.page_width, pctx.page_height);
    अञ्ञथा
	pctx.surface = cairo_pdf_surface_create((ध्रुव वर्ण *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    सचे (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	पटिनिवत्त FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    सचे (pctx.font == रिक्त)
    {
	mch_print_cleanup();
	पटिनिवत्त FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	पूर्णाङ्क		    ascent;
	पूर्णाङ्क		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, रिक्त);

	pctx.char_width =
	    (द्विगुण)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (द्विगुण)ascent / PANGO_SCALE;
	pctx.char_height = (द्विगुण)(ascent + descent) / PANGO_SCALE;

	pctx.पङ्क्तिः_spacing = (द्विगुण)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (द्विगुण)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (द्विगुण)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	द्विगुण  वामः;
	द्विगुण	दक्षिणः;
	द्विगुण	शिखरम्;
	द्विगुण	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &वामः, &दक्षिणः, &शिखरम्,
		&bottom);
	pctx.वामः_margin = वामः;
	pctx.दक्षिणः_margin = दक्षिणः;
	pctx.शिखरम्_margin = शिखरम्;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_पङ्क्तिः = get_cpl();
    psettings->पङ्क्तयः_per_page = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ifdef USE_GTK4_PRINT_DIALOG
    सचे (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	सचे (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    सचे (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    अञ्ञथा सचे (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    पटिनिवत्त OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    स्थिर शून्य
emit_dsc_comment(वर्ण *comment, वर्ण *fmt, ...)
{
    स्थिर वर्ण buf[257];

    va_list ap;
    पूर्णाङ्क	    len = परिमाण(buf);
    पूर्णाङ्क	    o;
    वर्ण    *s = buf;

    सचे (fmt == रिक्त)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	गच्छ exit;
    }
    अञ्ञथा
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    सचे (len <= 0)
	पटिनिवत्त;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    पूर्णाङ्क
mch_print_begin(prt_settings_T *psettings)
{
    वर्ण user[256];

    pctx.cur_x = 0.0;
    pctx.cur_पङ्क्तिः = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    सचे (pctx.dialog)
	पटिनिवत्त TRUE;
#endif

    सचे (!get_user_name((char_u *)user, परिमाण(user)))
	STRCPY(user, "Unknown");

    सचे (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (वर्ण *)psettings->jobname);
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
	emit_dsc_comment("BeginDefaults", रिक्त);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", रिक्त);
    }
    अञ्ञथा सचे (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	सचे (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, रिक्त);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (वर्ण *)str);
	सचे (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	सचे (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, रिक्त);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (वर्ण *)str);
	सचे (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    पटिनिवत्त TRUE;
}

    शून्य
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    सचे (psettings->outfile == रिक्त && !got_int && !psettings->user_abort
	    && pctx.filename != रिक्त
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	सन्देशः(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	सचे (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	अञ्ञथा
	    सन्देशः(_("Print job sent."));
    }

    mch_print_cleanup();
}

    पूर्णाङ्क
mch_print_end_page(शून्य)
{
    cairo_show_page(pctx.cr);
    पटिनिवत्त TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    पूर्णाङ्क
mch_print_begin_page(char_u *str UNUSED)
{
    पटिनिवत्त TRUE;
}

    पूर्णाङ्क
mch_print_blank_page(शून्य)
{
    mch_print_end_page();
    पटिनिवत्त TRUE;
}

    शून्य
mch_print_start_पङ्क्तिः(पूर्णाङ्क margin, पूर्णाङ्क page_पङ्क्तिः)
{
    pctx.cur_x = pctx.वामः_margin;
    सचे (margin)
	pctx.cur_x -= pctx.सङ्ख्या_width;

    pctx.cur_पङ्क्तिः = page_पङ्क्तिः;
}

    स्थिर शून्य
long_to_rgb(long_u c, द्विगुण *r, द्विगुण *g, द्विगुण *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    पूर्णाङ्क
mch_print_text_out(char_u *textp, पूर्णाङ्क len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    द्विगुण		r, g, b;
    द्विगुण		width = 0;
    द्विगुण		next_x;

    // Always convert to UTF-8, Pango expects that
    सचे (output_conv.vc_type == CONV_NONE)
	str = textp;
    अञ्ञथा
	str = string_convert(&output_conv, textp, &len);

    सचे (str == रिक्त)
	पटिनिवत्त TRUE;

    attr_list = pango_attr_list_new();

#define निवेशय_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_निवेशय((AttrList), tmp_attr_);	    \
    }G_STMT_END

    // Underline attribute does not work with pango_itemize().
    सचे ((pctx.draw_flags & PRT_DRAW_BOLD))
	निवेशय_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    सचे (pctx.draw_flags & PRT_DRAW_ITALIC)
	निवेशय_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef निवेशय_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (ध्रुव वर्ण *)str, 0, len, attr_list, रिक्त);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    याव (item_list != रिक्त)
    {
	PangoItem   *item;
	द्विगुण	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((ध्रुव वर्ण *)str + item->offset, item->length,
		(ध्रुव वर्ण *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (द्विगुण)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	सचे (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_पङ्क्तिः) - pctx.पङ्क्तिः_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_पङ्क्तिः));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    सचे (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	द्विगुण y = GET_TEXT_Y(pctx.cur_पङ्क्तिः) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_पङ्क्तिः_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_पङ्क्तिः_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    next_x = pctx.cur_x + pctx.char_width;

    सचे (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    पटिनिवत्त (next_x > pctx.दक्षिणः_margin) &&
	((next_x - pctx.दक्षिणः_margin) > (pctx.दक्षिणः_margin * 1e-5));
}

    शून्य
mch_print_set_font(पूर्णाङ्क bold, पूर्णाङ्क italic, पूर्णाङ्क underline)
{
    pctx.draw_flags = 0;

    सचे (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    सचे (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    सचे (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    शून्य
mch_print_set_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    शून्य
mch_print_set_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    शून्य
mch_print_cleanup(शून्य)
{
    g_clear_pointer(&pctx.cr, cairo_destroy);
    g_clear_pointer(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.text_context);
    g_clear_pointer(&pctx.font, pango_font_description_free);

#ifdef USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    सचे (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
