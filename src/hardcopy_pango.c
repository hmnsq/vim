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

#define GET_Y(l) (pctx.शीर्ष_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

प्रकारव्याख्या गणना
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

    पूर्णांक media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    द्विगुण char_width;
    द्विगुण char_ascent;
    द्विगुण char_height;
    द्विगुण ओळ_spacing;
    द्विगुण ul_pos;
    द्विगुण ul_thickness;

    द्विगुण page_width;
    द्विगुण page_height;

    द्विगुण डावा_margin;
    द्विगुण उजवा_margin;
    द्विगुण शीर्ष_margin;
    द्विगुण bottom_margin;

    द्विगुण संख्या_width;

    // Current position to draw from
    द्विगुण cur_x;
    पूर्णांक cur_ओळ;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

स्थिर स्थिरांक अक्षर *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    स्थिर द्विगुण
to_device_units(पूर्णांक idx, द्विगुण physsize, पूर्णांक def_संख्या)
{
    दशांश	ret;
    पूर्णांक		u;
    पूर्णांक		nr;

    u = prt_get_unit(idx);
    जर (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_संख्या;
    }
    अन्यथा
	nr = printer_opts[idx].संख्या;

    निवड (u)
    {
	स्थिती PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    थांबा;
	स्थिती PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    थांबा;
	स्थिती PRT_UNIT_POINT:
	    ret = (द्विगुण)nr;
	    थांबा;
	स्थिती PRT_UNIT_PERC:
	पूर्वनिर्धारित:
	    ret = physsize * (nr / 100.0);
	    थांबा;
    }

    परत ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    स्थिर शून्य
get_page_margins(
	द्विगुण	width,
	द्विगुण	height,
	द्विगुण	*डावा,
	द्विगुण	*उजवा,
	द्विगुण	*शीर्ष,
	द्विगुण	*bottom)
{
    *डावा = to_device_units(OPT_PRINT_डावा, width, 10);
    *उजवा = width - to_device_units(OPT_PRINT_उजवा, width, 5);

    *शीर्ष = to_device_units(OPT_PRINT_शीर्ष, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    स्थिर पूर्णांक
get_cpl(शून्य)
{
    जर (prt_use_संख्या())
    {
	pctx.संख्या_width = PRINT_संख्या_WIDTH * pctx.char_width;
	pctx.डावा_margin += pctx.संख्या_width;
    }
    अन्यथा
	pctx.संख्या_width = 0.0;

    परत (पूर्णांक)((pctx.उजवा_margin - pctx.डावा_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    स्थिर पूर्णांक
get_lpp(शून्य)
{
    पूर्णांक lpp;

    // Calculate lpp
    lpp = (पूर्णांक)((pctx.bottom_margin - pctx.शीर्ष_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.शीर्ष_margin += pctx.char_height * prt_header_height();

    परत lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    स्थिर PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    जर (name == रिक्त)
	परत रिक्त;

    font = pango_font_description_from_string((स्थिरांक अक्षर *)name);
    जर (font == रिक्त)
    {
	semsg(_(e_unknown_printer_font_str), name);
	परत रिक्त;
    }

    // Ensure a size is set
    जर (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    परत font;
}

    पूर्णांक
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	पूर्णांक		forceit UNUSED)
{
    char_u	*paper_name;
    पूर्णांक		paper_strlen;
    bool	portrait;
    पूर्णांक		media;
    char_u	*filename = रिक्त;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    जर (gui.in_use && !forceit && psettings->outfile == रिक्त)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	जर (filename == रिक्त)
	    परत FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    जर (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	जर (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].म_माप;
	}
	अन्यथा
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	साठी (media = 0; media < (पूर्णांक)PRT_MEDIASIZE_LEN; ++media)
	    जर ((पूर्णांक)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		थांबा;
	जर (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	जर (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	अन्यथा
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	जर (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    साठी (पूर्णांक i = 0; i < (पूर्णांक)ARRAY_LENGTH(print_formats); i++)
		जर (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].म_माप) == 0)
		{
		    pctx.format = i;
		    थांबा;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    जर (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    जर (filename == रिक्त)
    {
	// If the user didn't specify a file name, use a temp file.
	जर (psettings->outfile == रिक्त)
	{
	    filename = vim_tempname('p', TRUE);
	    जर (filename == रिक्त)
	    {
		emsg(_(e_cant_get_temp_file_name));
		परत FAIL;
	    }
	}
	अन्यथा
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    जर (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((स्थिरांक अक्षर *)filename,
		pctx.page_width, pctx.page_height);
    अन्यथा
	pctx.surface = cairo_pdf_surface_create((स्थिरांक अक्षर *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    जर (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	परत FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    जर (pctx.font == रिक्त)
    {
	mch_print_cleanup();
	परत FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	पूर्णांक		    ascent;
	पूर्णांक		    descent;

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

	pctx.ओळ_spacing = (द्विगुण)
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
	द्विगुण  डावा;
	द्विगुण	उजवा;
	द्विगुण	शीर्ष;
	द्विगुण	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &डावा, &उजवा, &शीर्ष,
		&bottom);
	pctx.डावा_margin = डावा;
	pctx.उजवा_margin = उजवा;
	pctx.शीर्ष_margin = शीर्ष;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_ओळ = get_cpl();
    psettings->ओळी_per_page = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ifdef USE_GTK4_PRINT_DIALOG
    जर (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	जर (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    जर (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    अन्यथा जर (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    परत OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    स्थिर शून्य
emit_dsc_comment(अक्षर *comment, अक्षर *fmt, ...)
{
    स्थिर अक्षर buf[257];

    बहु_सूची ap;
    पूर्णांक	    len = आकार(buf);
    पूर्णांक	    o;
    अक्षर    *s = buf;

    जर (fmt == रिक्त)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	जा निकास;
    }
    अन्यथा
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    जर (len <= 0)
	परत;

    बहु_शुरू(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    बहु_पूर्ण(ap);

निकास:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    पूर्णांक
mch_print_begin(prt_settings_T *psettings)
{
    अक्षर user[256];

    pctx.cur_x = 0.0;
    pctx.cur_ओळ = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    जर (pctx.dialog)
	परत TRUE;
#endif

    जर (!get_user_name((char_u *)user, आकार(user)))
	STRCPY(user, "Unknown");

    जर (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (अक्षर *)psettings->jobname);
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
    अन्यथा जर (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	जर (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, रिक्त);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (अक्षर *)str);
	जर (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	जर (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, रिक्त);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (अक्षर *)str);
	जर (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    परत TRUE;
}

    शून्य
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    जर (psettings->outfile == रिक्त && !got_int && !psettings->user_abort
	    && pctx.filename != रिक्त
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	संदेश(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	जर (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	अन्यथा
	    संदेश(_("Print job sent."));
    }

    mch_print_cleanup();
}

    पूर्णांक
mch_print_end_page(शून्य)
{
    cairo_show_page(pctx.cr);
    परत TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    पूर्णांक
mch_print_begin_page(char_u *str UNUSED)
{
    परत TRUE;
}

    पूर्णांक
mch_print_blank_page(शून्य)
{
    mch_print_end_page();
    परत TRUE;
}

    शून्य
mch_print_start_ओळ(पूर्णांक margin, पूर्णांक page_ओळ)
{
    pctx.cur_x = pctx.डावा_margin;
    जर (margin)
	pctx.cur_x -= pctx.संख्या_width;

    pctx.cur_ओळ = page_ओळ;
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
    पूर्णांक
mch_print_text_out(char_u *textp, पूर्णांक len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    द्विगुण		r, g, b;
    द्विगुण		width = 0;
    द्विगुण		next_x;

    // Always convert to UTF-8, Pango expects that
    जर (output_conv.vc_type == CONV_NONE)
	str = textp;
    अन्यथा
	str = string_convert(&output_conv, textp, &len);

    जर (str == रिक्त)
	परत TRUE;

    attr_list = pango_attr_list_new();

#define घाला_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_घाला((AttrList), tmp_attr_);	    \
    }G_STMT_END

    // Underline attribute does not work with pango_itemize().
    जर ((pctx.draw_flags & PRT_DRAW_BOLD))
	घाला_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    जर (pctx.draw_flags & PRT_DRAW_ITALIC)
	घाला_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef घाला_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (स्थिरांक अक्षर *)str, 0, len, attr_list, रिक्त);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    पर्यंत (item_list != रिक्त)
    {
	PangoItem   *item;
	द्विगुण	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((स्थिरांक अक्षर *)str + item->offset, item->length,
		(स्थिरांक अक्षर *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (द्विगुण)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	जर (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_ओळ) - pctx.ओळ_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_ओळ));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    जर (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	द्विगुण y = GET_TEXT_Y(pctx.cur_ओळ) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_ओळ_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_ओळ_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    next_x = pctx.cur_x + pctx.char_width;

    जर (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    परत (next_x > pctx.उजवा_margin) &&
	((next_x - pctx.उजवा_margin) > (pctx.उजवा_margin * 1e-5));
}

    शून्य
mch_print_set_font(पूर्णांक bold, पूर्णांक italic, पूर्णांक underline)
{
    pctx.draw_flags = 0;

    जर (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    जर (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    जर (underline)
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
    जर (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
