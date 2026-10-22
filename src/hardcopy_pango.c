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

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "version.h"

#ifdef FEAT_PRINT_PANGO

#அந்தர்பாவ "hardcopy.h"

#அந்தர்பாவ <cairo/cairo-ps.h>
#அந்தர்பாவ <cairo/cairo-pdf.h>
#அந்தர்பாவ <pango/pango.h>
#அந்தர்பாவ <pango/pangocairo.h>

#define PRINT_DPI 72.0

#define PRT_DRAW_BOLD 1
#define PRT_DRAW_ITALIC 2
#define PRT_DRAW_UNDERLINE 4

#define GET_Y(l) (pctx.உச்சி_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

வகைவரை எண்ணிக்கை
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
நிலையான கட்டமைப்பு
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    முழு media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    இரட்டை char_width;
    இரட்டை char_ascent;
    இரட்டை char_height;
    இரட்டை வரி_spacing;
    இரட்டை ul_pos;
    இரட்டை ul_thickness;

    இரட்டை page_width;
    இரட்டை page_height;

    இரட்டை இடது_margin;
    இரட்டை வலது_margin;
    இரட்டை உச்சி_margin;
    இரட்டை bottom_margin;

    இரட்டை எண்_width;

    // Current position to draw from
    இரட்டை cur_x;
    முழு cur_வரி;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

நிலையான மாறிலி எழுத்து *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    நிலையான இரட்டை
to_device_units(முழு idx, இரட்டை physsize, முழு def_எண்)
{
    தசம	ret;
    முழு		u;
    முழு		nr;

    u = prt_get_unit(idx);
    எனில் (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_எண்;
    }
    இல்லையெனில்
	nr = printer_opts[idx].எண்;

    தேர்வு (u)
    {
	நிலை PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    நிறுத்து;
	நிலை PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    நிறுத்து;
	நிலை PRT_UNIT_POINT:
	    ret = (இரட்டை)nr;
	    நிறுத்து;
	நிலை PRT_UNIT_PERC:
	இயல்பு:
	    ret = physsize * (nr / 100.0);
	    நிறுத்து;
    }

    திரும்பு ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    நிலையான வெறுமை
get_page_margins(
	இரட்டை	width,
	இரட்டை	height,
	இரட்டை	*இடது,
	இரட்டை	*வலது,
	இரட்டை	*உச்சி,
	இரட்டை	*bottom)
{
    *இடது = to_device_units(OPT_PRINT_இடது, width, 10);
    *வலது = width - to_device_units(OPT_PRINT_வலது, width, 5);

    *உச்சி = to_device_units(OPT_PRINT_உச்சி, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    நிலையான முழு
get_cpl(வெறுமை)
{
    எனில் (prt_use_எண்())
    {
	pctx.எண்_width = PRINT_எண்_WIDTH * pctx.char_width;
	pctx.இடது_margin += pctx.எண்_width;
    }
    இல்லையெனில்
	pctx.எண்_width = 0.0;

    திரும்பு (முழு)((pctx.வலது_margin - pctx.இடது_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    நிலையான முழு
get_lpp(வெறுமை)
{
    முழு lpp;

    // Calculate lpp
    lpp = (முழு)((pctx.bottom_margin - pctx.உச்சி_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.உச்சி_margin += pctx.char_height * prt_header_height();

    திரும்பு lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    நிலையான PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    எனில் (name == வெற்று)
	திரும்பு வெற்று;

    font = pango_font_description_from_string((மாறிலி எழுத்து *)name);
    எனில் (font == வெற்று)
    {
	semsg(_(e_unknown_printer_font_str), name);
	திரும்பு வெற்று;
    }

    // Ensure a size is set
    எனில் (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    திரும்பு font;
}

    முழு
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	முழு		forceit UNUSED)
{
    char_u	*paper_name;
    முழு		paper_strlen;
    bool	portrait;
    முழு		media;
    char_u	*filename = வெற்று;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    எனில் (gui.in_use && !forceit && psettings->outfile == வெற்று)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	எனில் (filename == வெற்று)
	    திரும்பு FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    எனில் (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	எனில் (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
	}
	இல்லையெனில்
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	க்கு (media = 0; media < (முழு)PRT_MEDIASIZE_LEN; ++media)
	    எனில் ((முழு)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		நிறுத்து;
	எனில் (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	எனில் (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	இல்லையெனில்
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	எனில் (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    க்கு (முழு i = 0; i < (முழு)ARRAY_LENGTH(print_formats); i++)
		எனில் (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    நிறுத்து;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    எனில் (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    எனில் (filename == வெற்று)
    {
	// If the user didn't specify a file name, use a temp file.
	எனில் (psettings->outfile == வெற்று)
	{
	    filename = vim_tempname('p', TRUE);
	    எனில் (filename == வெற்று)
	    {
		emsg(_(e_cant_get_temp_file_name));
		திரும்பு FAIL;
	    }
	}
	இல்லையெனில்
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    எனில் (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((மாறிலி எழுத்து *)filename,
		pctx.page_width, pctx.page_height);
    இல்லையெனில்
	pctx.surface = cairo_pdf_surface_create((மாறிலி எழுத்து *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    எனில் (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	திரும்பு FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    எனில் (pctx.font == வெற்று)
    {
	mch_print_cleanup();
	திரும்பு FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	முழு		    ascent;
	முழு		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, வெற்று);

	pctx.char_width =
	    (இரட்டை)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (இரட்டை)ascent / PANGO_SCALE;
	pctx.char_height = (இரட்டை)(ascent + descent) / PANGO_SCALE;

	pctx.வரி_spacing = (இரட்டை)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (இரட்டை)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (இரட்டை)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	இரட்டை  இடது;
	இரட்டை	வலது;
	இரட்டை	உச்சி;
	இரட்டை	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &இடது, &வலது, &உச்சி,
		&bottom);
	pctx.இடது_margin = இடது;
	pctx.வலது_margin = வலது;
	pctx.உச்சி_margin = உச்சி;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_வரி = get_cpl();
    psettings->வரிகள்_per_page = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ifdef USE_GTK4_PRINT_DIALOG
    எனில் (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	எனில் (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    எனில் (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    இல்லையெனில் எனில் (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    திரும்பு OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    நிலையான வெறுமை
emit_dsc_comment(எழுத்து *comment, எழுத்து *fmt, ...)
{
    நிலையான எழுத்து buf[257];

    va_list ap;
    முழு	    len = அளவு(buf);
    முழு	    o;
    எழுத்து    *s = buf;

    எனில் (fmt == வெற்று)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	செல் exit;
    }
    இல்லையெனில்
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    எனில் (len <= 0)
	திரும்பு;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    முழு
mch_print_begin(prt_settings_T *psettings)
{
    எழுத்து user[256];

    pctx.cur_x = 0.0;
    pctx.cur_வரி = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    எனில் (pctx.dialog)
	திரும்பு TRUE;
#endif

    எனில் (!get_user_name((char_u *)user, அளவு(user)))
	STRCPY(user, "Unknown");

    எனில் (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (எழுத்து *)psettings->jobname);
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
	emit_dsc_comment("BeginDefaults", வெற்று);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", வெற்று);
    }
    இல்லையெனில் எனில் (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	எனில் (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, வெற்று);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (எழுத்து *)str);
	எனில் (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	எனில் (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, வெற்று);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (எழுத்து *)str);
	எனில் (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    திரும்பு TRUE;
}

    வெறுமை
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    எனில் (psettings->outfile == வெற்று && !got_int && !psettings->user_abort
	    && pctx.filename != வெற்று
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	செய்தி(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	எனில் (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	இல்லையெனில்
	    செய்தி(_("Print job sent."));
    }

    mch_print_cleanup();
}

    முழு
mch_print_end_page(வெறுமை)
{
    cairo_show_page(pctx.cr);
    திரும்பு TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    முழு
mch_print_begin_page(char_u *str UNUSED)
{
    திரும்பு TRUE;
}

    முழு
mch_print_blank_page(வெறுமை)
{
    mch_print_end_page();
    திரும்பு TRUE;
}

    வெறுமை
mch_print_start_வரி(முழு margin, முழு page_வரி)
{
    pctx.cur_x = pctx.இடது_margin;
    எனில் (margin)
	pctx.cur_x -= pctx.எண்_width;

    pctx.cur_வரி = page_வரி;
}

    நிலையான வெறுமை
long_to_rgb(long_u c, இரட்டை *r, இரட்டை *g, இரட்டை *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    முழு
mch_print_text_out(char_u *textp, முழு len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    இரட்டை		r, g, b;
    இரட்டை		width = 0;
    இரட்டை		next_x;

    // Always convert to UTF-8, Pango expects that
    எனில் (output_conv.vc_type == CONV_NONE)
	str = textp;
    இல்லையெனில்
	str = string_convert(&output_conv, textp, &len);

    எனில் (str == வெற்று)
	திரும்பு TRUE;

    attr_list = pango_attr_list_new();

#define செருகு_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_செருகு((AttrList), tmp_attr_);	    \
    }G_STMT_END

    // Underline attribute does not work with pango_itemize().
    எனில் ((pctx.draw_flags & PRT_DRAW_BOLD))
	செருகு_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    எனில் (pctx.draw_flags & PRT_DRAW_ITALIC)
	செருகு_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef செருகு_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (மாறிலி எழுத்து *)str, 0, len, attr_list, வெற்று);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    போது (item_list != வெற்று)
    {
	PangoItem   *item;
	இரட்டை	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((மாறிலி எழுத்து *)str + item->offset, item->length,
		(மாறிலி எழுத்து *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (இரட்டை)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	எனில் (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_வரி) - pctx.வரி_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_வரி));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    எனில் (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	இரட்டை y = GET_TEXT_Y(pctx.cur_வரி) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_வரி_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_வரி_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    next_x = pctx.cur_x + pctx.char_width;

    எனில் (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    திரும்பு (next_x > pctx.வலது_margin) &&
	((next_x - pctx.வலது_margin) > (pctx.வலது_margin * 1e-5));
}

    வெறுமை
mch_print_set_font(முழு bold, முழு italic, முழு underline)
{
    pctx.draw_flags = 0;

    எனில் (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    எனில் (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    எனில் (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    வெறுமை
mch_print_set_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    வெறுமை
mch_print_set_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    வெறுமை
mch_print_cleanup(வெறுமை)
{
    g_clear_pointer(&pctx.cr, cairo_destroy);
    g_clear_pointer(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.text_context);
    g_clear_pointer(&pctx.font, pango_font_description_free);

#ifdef USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    எனில் (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
