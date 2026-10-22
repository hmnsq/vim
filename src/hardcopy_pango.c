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

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

#ifdef FEAT_PRINT_PANGO

#અન્તર્ભાવ "hardcopy.h"

#અન્તર્ભાવ <cairo/cairo-ps.h>
#અન્તર્ભાવ <cairo/cairo-pdf.h>
#અન્તર્ભાવ <pango/pango.h>
#અન્તર્ભાવ <pango/pangocairo.h>

#define PRINT_DPI 72.0

#define PRT_DRAW_BOLD 1
#define PRT_DRAW_ITALIC 2
#define PRT_DRAW_UNDERLINE 4

#define GET_Y(l) (pctx.ટોચ_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

પ્રકારવ્યાખ્યા ગણના
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
સ્થિર સંરચના
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    પૂર્ણાંક media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    દ્વિગુણ char_width;
    દ્વિગુણ char_ascent;
    દ્વિગુણ char_height;
    દ્વિગુણ લીટી_spacing;
    દ્વિગુણ ul_pos;
    દ્વિગુણ ul_thickness;

    દ્વિગુણ page_width;
    દ્વિગુણ page_height;

    દ્વિગુણ ડાબું_margin;
    દ્વિગુણ જમણું_margin;
    દ્વિગુણ ટોચ_margin;
    દ્વિગુણ bottom_margin;

    દ્વિગુણ સંખ્યા_width;

    // Current position to draw from
    દ્વિગુણ cur_x;
    પૂર્ણાંક cur_લીટી;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

સ્થિર અચલ અક્ષર *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    સ્થિર દ્વિગુણ
to_device_units(પૂર્ણાંક idx, દ્વિગુણ physsize, પૂર્ણાંક def_સંખ્યા)
{
    દશાંશ	ret;
    પૂર્ણાંક		u;
    પૂર્ણાંક		nr;

    u = prt_get_unit(idx);
    જો (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_સંખ્યા;
    }
    નહીંતર
	nr = printer_opts[idx].સંખ્યા;

    પસંદગી (u)
    {
	સ્થિતિ PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    તોડો;
	સ્થિતિ PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    તોડો;
	સ્થિતિ PRT_UNIT_POINT:
	    ret = (દ્વિગુણ)nr;
	    તોડો;
	સ્થિતિ PRT_UNIT_PERC:
	મૂળભૂત:
	    ret = physsize * (nr / 100.0);
	    તોડો;
    }

    પાછા ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    સ્થિર શૂન્ય
get_page_margins(
	દ્વિગુણ	width,
	દ્વિગુણ	height,
	દ્વિગુણ	*ડાબું,
	દ્વિગુણ	*જમણું,
	દ્વિગુણ	*ટોચ,
	દ્વિગુણ	*bottom)
{
    *ડાબું = to_device_units(OPT_PRINT_ડાબું, width, 10);
    *જમણું = width - to_device_units(OPT_PRINT_જમણું, width, 5);

    *ટોચ = to_device_units(OPT_PRINT_ટોચ, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    સ્થિર પૂર્ણાંક
get_cpl(શૂન્ય)
{
    જો (prt_use_સંખ્યા())
    {
	pctx.સંખ્યા_width = PRINT_સંખ્યા_WIDTH * pctx.char_width;
	pctx.ડાબું_margin += pctx.સંખ્યા_width;
    }
    નહીંતર
	pctx.સંખ્યા_width = 0.0;

    પાછા (પૂર્ણાંક)((pctx.જમણું_margin - pctx.ડાબું_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    સ્થિર પૂર્ણાંક
get_lpp(શૂન્ય)
{
    પૂર્ણાંક lpp;

    // Calculate lpp
    lpp = (પૂર્ણાંક)((pctx.bottom_margin - pctx.ટોચ_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.ટોચ_margin += pctx.char_height * prt_header_height();

    પાછા lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    સ્થિર PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    જો (name == ખાલી)
	પાછા ખાલી;

    font = pango_font_description_from_string((અચલ અક્ષર *)name);
    જો (font == ખાલી)
    {
	semsg(_(e_unknown_printer_font_str), name);
	પાછા ખાલી;
    }

    // Ensure a size is set
    જો (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    પાછા font;
}

    પૂર્ણાંક
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	પૂર્ણાંક		forceit UNUSED)
{
    char_u	*paper_name;
    પૂર્ણાંક		paper_strlen;
    bool	portrait;
    પૂર્ણાંક		media;
    char_u	*filename = ખાલી;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    જો (gui.in_use && !forceit && psettings->outfile == ખાલી)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	જો (filename == ખાલી)
	    પાછા FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    જો (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	જો (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
	}
	નહીંતર
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	માટે (media = 0; media < (પૂર્ણાંક)PRT_MEDIASIZE_LEN; ++media)
	    જો ((પૂર્ણાંક)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		તોડો;
	જો (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	જો (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	નહીંતર
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	જો (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    માટે (પૂર્ણાંક i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(print_formats); i++)
		જો (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    તોડો;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    જો (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    જો (filename == ખાલી)
    {
	// If the user didn't specify a file name, use a temp file.
	જો (psettings->outfile == ખાલી)
	{
	    filename = vim_tempname('p', TRUE);
	    જો (filename == ખાલી)
	    {
		emsg(_(e_cant_get_temp_file_name));
		પાછા FAIL;
	    }
	}
	નહીંતર
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    જો (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((અચલ અક્ષર *)filename,
		pctx.page_width, pctx.page_height);
    નહીંતર
	pctx.surface = cairo_pdf_surface_create((અચલ અક્ષર *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    જો (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	પાછા FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    જો (pctx.font == ખાલી)
    {
	mch_print_cleanup();
	પાછા FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	પૂર્ણાંક		    ascent;
	પૂર્ણાંક		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, ખાલી);

	pctx.char_width =
	    (દ્વિગુણ)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (દ્વિગુણ)ascent / PANGO_SCALE;
	pctx.char_height = (દ્વિગુણ)(ascent + descent) / PANGO_SCALE;

	pctx.લીટી_spacing = (દ્વિગુણ)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (દ્વિગુણ)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (દ્વિગુણ)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	દ્વિગુણ  ડાબું;
	દ્વિગુણ	જમણું;
	દ્વિગુણ	ટોચ;
	દ્વિગુણ	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &ડાબું, &જમણું, &ટોચ,
		&bottom);
	pctx.ડાબું_margin = ડાબું;
	pctx.જમણું_margin = જમણું;
	pctx.ટોચ_margin = ટોચ;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_લીટી = get_cpl();
    psettings->લીટીઓ_per_page = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ifdef USE_GTK4_PRINT_DIALOG
    જો (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	જો (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    જો (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    નહીંતર જો (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    પાછા OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    સ્થિર શૂન્ય
emit_dsc_comment(અક્ષર *comment, અક્ષર *fmt, ...)
{
    સ્થિર અક્ષર buf[257];

    va_list ap;
    પૂર્ણાંક	    len = કદ(buf);
    પૂર્ણાંક	    o;
    અક્ષર    *s = buf;

    જો (fmt == ખાલી)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	જાઓ exit;
    }
    નહીંતર
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    જો (len <= 0)
	પાછા;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    પૂર્ણાંક
mch_print_begin(prt_settings_T *psettings)
{
    અક્ષર user[256];

    pctx.cur_x = 0.0;
    pctx.cur_લીટી = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    જો (pctx.dialog)
	પાછા TRUE;
#endif

    જો (!get_user_name((char_u *)user, કદ(user)))
	STRCPY(user, "Unknown");

    જો (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (અક્ષર *)psettings->jobname);
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
	emit_dsc_comment("BeginDefaults", ખાલી);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", ખાલી);
    }
    નહીંતર જો (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	જો (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ખાલી);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (અક્ષર *)str);
	જો (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	જો (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ખાલી);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (અક્ષર *)str);
	જો (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    પાછા TRUE;
}

    શૂન્ય
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    જો (psettings->outfile == ખાલી && !got_int && !psettings->user_abort
	    && pctx.filename != ખાલી
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	સંદેશ(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	જો (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	નહીંતર
	    સંદેશ(_("Print job sent."));
    }

    mch_print_cleanup();
}

    પૂર્ણાંક
mch_print_end_page(શૂન્ય)
{
    cairo_show_page(pctx.cr);
    પાછા TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    પૂર્ણાંક
mch_print_begin_page(char_u *str UNUSED)
{
    પાછા TRUE;
}

    પૂર્ણાંક
mch_print_blank_page(શૂન્ય)
{
    mch_print_end_page();
    પાછા TRUE;
}

    શૂન્ય
mch_print_start_લીટી(પૂર્ણાંક margin, પૂર્ણાંક page_લીટી)
{
    pctx.cur_x = pctx.ડાબું_margin;
    જો (margin)
	pctx.cur_x -= pctx.સંખ્યા_width;

    pctx.cur_લીટી = page_લીટી;
}

    સ્થિર શૂન્ય
long_to_rgb(long_u c, દ્વિગુણ *r, દ્વિગુણ *g, દ્વિગુણ *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    પૂર્ણાંક
mch_print_text_out(char_u *textp, પૂર્ણાંક len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    દ્વિગુણ		r, g, b;
    દ્વિગુણ		width = 0;
    દ્વિગુણ		next_x;

    // Always convert to UTF-8, Pango expects that
    જો (output_conv.vc_type == CONV_NONE)
	str = textp;
    નહીંતર
	str = string_convert(&output_conv, textp, &len);

    જો (str == ખાલી)
	પાછા TRUE;

    attr_list = pango_attr_list_new();

#define ઉમેરો_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_ઉમેરો((AttrList), tmp_attr_);	    \
    }G_STMT_END

    // Underline attribute does not work with pango_itemize().
    જો ((pctx.draw_flags & PRT_DRAW_BOLD))
	ઉમેરો_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    જો (pctx.draw_flags & PRT_DRAW_ITALIC)
	ઉમેરો_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef ઉમેરો_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (અચલ અક્ષર *)str, 0, len, attr_list, ખાલી);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    જ્યારે (item_list != ખાલી)
    {
	PangoItem   *item;
	દ્વિગુણ	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((અચલ અક્ષર *)str + item->offset, item->length,
		(અચલ અક્ષર *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (દ્વિગુણ)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	જો (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_લીટી) - pctx.લીટી_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_લીટી));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    જો (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	દ્વિગુણ y = GET_TEXT_Y(pctx.cur_લીટી) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_લીટી_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_લીટી_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    next_x = pctx.cur_x + pctx.char_width;

    જો (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    પાછા (next_x > pctx.જમણું_margin) &&
	((next_x - pctx.જમણું_margin) > (pctx.જમણું_margin * 1e-5));
}

    શૂન્ય
mch_print_set_font(પૂર્ણાંક bold, પૂર્ણાંક italic, પૂર્ણાંક underline)
{
    pctx.draw_flags = 0;

    જો (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    જો (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    જો (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    શૂન્ય
mch_print_set_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    શૂન્ય
mch_print_set_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    શૂન્ય
mch_print_cleanup(શૂન્ય)
{
    g_clear_pointer(&pctx.cr, cairo_destroy);
    g_clear_pointer(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.text_context);
    g_clear_pointer(&pctx.font, pango_font_description_free);

#ifdef USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    જો (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
