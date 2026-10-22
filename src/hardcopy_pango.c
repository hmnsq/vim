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

#ضمن "vim.h"
#ضمن "version.h"

#ifdef FEAT_PRINT_PANGO

#ضمن "hardcopy.h"

#ضمن <cairo/cairo-ps.h>
#ضمن <cairo/cairo-pdf.h>
#ضمن <pango/pango.h>
#ضمن <pango/pangocairo.h>

#define PRINT_DPI 72.0

#define PRT_DRAW_BOLD 1
#define PRT_DRAW_ITALIC 2
#define PRT_DRAW_UNDERLINE 4

#define GET_Y(l) (pctx.لووتکە_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

تعريفنوع تعداد
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
ساكن بنية
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    صحيح media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    مزدوج char_width;
    مزدوج char_ascent;
    مزدوج char_height;
    مزدوج هێڵ_spacing;
    مزدوج ul_pos;
    مزدوج ul_thickness;

    مزدوج page_width;
    مزدوج page_height;

    مزدوج چەپ_margin;
    مزدوج ڕاست_margin;
    مزدوج لووتکە_margin;
    مزدوج bottom_margin;

    مزدوج ژمار_width;

    // Current position to draw from
    مزدوج cur_x;
    صحيح cur_هێڵ;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

ساكن ثابت حرف *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    ساكن مزدوج
to_device_units(صحيح idx, مزدوج physsize, صحيح def_ژمار)
{
    عائم	ret;
    صحيح		u;
    صحيح		nr;

    u = prt_get_unit(idx);
    إذا (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ژمار;
    }
    وإلا
	nr = printer_opts[idx].ژمار;

    بدل (u)
    {
	حالة PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    اكسر;
	حالة PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    اكسر;
	حالة PRT_UNIT_POINT:
	    ret = (مزدوج)nr;
	    اكسر;
	حالة PRT_UNIT_PERC:
	افتراضي:
	    ret = physsize * (nr / 100.0);
	    اكسر;
    }

    أرجع ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ساكن فراغ
get_page_margins(
	مزدوج	width,
	مزدوج	height,
	مزدوج	*چەپ,
	مزدوج	*ڕاست,
	مزدوج	*لووتکە,
	مزدوج	*bottom)
{
    *چەپ = to_device_units(OPT_PRINT_چەپ, width, 10);
    *ڕاست = width - to_device_units(OPT_PRINT_ڕاست, width, 5);

    *لووتکە = to_device_units(OPT_PRINT_لووتکە, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    ساكن صحيح
get_cpl(فراغ)
{
    إذا (prt_use_ژمار())
    {
	pctx.ژمار_width = PRINT_ژمار_WIDTH * pctx.char_width;
	pctx.چەپ_margin += pctx.ژمار_width;
    }
    وإلا
	pctx.ژمار_width = 0.0;

    أرجع (صحيح)((pctx.ڕاست_margin - pctx.چەپ_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ساكن صحيح
get_lpp(فراغ)
{
    صحيح lpp;

    // Calculate lpp
    lpp = (صحيح)((pctx.bottom_margin - pctx.لووتکە_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.لووتکە_margin += pctx.char_height * prt_header_height();

    أرجع lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    ساكن PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    إذا (name == عدم)
	أرجع عدم;

    font = pango_font_description_from_string((ثابت حرف *)name);
    إذا (font == عدم)
    {
	semsg(_(e_unknown_printer_font_str), name);
	أرجع عدم;
    }

    // Ensure a size is set
    إذا (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    أرجع font;
}

    صحيح
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	صحيح		forceit UNUSED)
{
    char_u	*paper_name;
    صحيح		paper_strlen;
    bool	portrait;
    صحيح		media;
    char_u	*filename = عدم;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    إذا (gui.in_use && !forceit && psettings->outfile == عدم)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	إذا (filename == عدم)
	    أرجع FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    إذا (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	إذا (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
	}
	وإلا
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	لكل (media = 0; media < (صحيح)PRT_MEDIASIZE_LEN; ++media)
	    إذا ((صحيح)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		اكسر;
	إذا (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	إذا (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	وإلا
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	إذا (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    لكل (صحيح i = 0; i < (صحيح)ARRAY_LENGTH(print_formats); i++)
		إذا (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    اكسر;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    إذا (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    إذا (filename == عدم)
    {
	// If the user didn't specify a file name, use a temp file.
	إذا (psettings->outfile == عدم)
	{
	    filename = vim_tempname('p', TRUE);
	    إذا (filename == عدم)
	    {
		emsg(_(e_cant_get_temp_file_name));
		أرجع FAIL;
	    }
	}
	وإلا
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    إذا (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ثابت حرف *)filename,
		pctx.page_width, pctx.page_height);
    وإلا
	pctx.surface = cairo_pdf_surface_create((ثابت حرف *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    إذا (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	أرجع FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    إذا (pctx.font == عدم)
    {
	mch_print_cleanup();
	أرجع FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	صحيح		    ascent;
	صحيح		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, عدم);

	pctx.char_width =
	    (مزدوج)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (مزدوج)ascent / PANGO_SCALE;
	pctx.char_height = (مزدوج)(ascent + descent) / PANGO_SCALE;

	pctx.هێڵ_spacing = (مزدوج)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (مزدوج)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (مزدوج)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	مزدوج  چەپ;
	مزدوج	ڕاست;
	مزدوج	لووتکە;
	مزدوج	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &چەپ, &ڕاست, &لووتکە,
		&bottom);
	pctx.چەپ_margin = چەپ;
	pctx.ڕاست_margin = ڕاست;
	pctx.لووتکە_margin = لووتکە;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_هێڵ = get_cpl();
    psettings->هێڵەکان_per_page = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ifdef USE_GTK4_PRINT_DIALOG
    إذا (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	إذا (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    إذا (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    وإلا إذا (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    أرجع OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    ساكن فراغ
emit_dsc_comment(حرف *comment, حرف *fmt, ...)
{
    ساكن حرف buf[257];

    va_list ap;
    صحيح	    len = حجم(buf);
    صحيح	    o;
    حرف    *s = buf;

    إذا (fmt == عدم)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	اذهب exit;
    }
    وإلا
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    إذا (len <= 0)
	أرجع;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    صحيح
mch_print_begin(prt_settings_T *psettings)
{
    حرف user[256];

    pctx.cur_x = 0.0;
    pctx.cur_هێڵ = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    إذا (pctx.dialog)
	أرجع TRUE;
#endif

    إذا (!get_user_name((char_u *)user, حجم(user)))
	STRCPY(user, "Unknown");

    إذا (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (حرف *)psettings->jobname);
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
	emit_dsc_comment("BeginDefaults", عدم);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", عدم);
    }
    وإلا إذا (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	إذا (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, عدم);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (حرف *)str);
	إذا (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	إذا (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, عدم);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (حرف *)str);
	إذا (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    أرجع TRUE;
}

    فراغ
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    إذا (psettings->outfile == عدم && !got_int && !psettings->user_abort
	    && pctx.filename != عدم
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	پەیام(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	إذا (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	وإلا
	    پەیام(_("Print job sent."));
    }

    mch_print_cleanup();
}

    صحيح
mch_print_end_page(فراغ)
{
    cairo_show_page(pctx.cr);
    أرجع TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    صحيح
mch_print_begin_page(char_u *str UNUSED)
{
    أرجع TRUE;
}

    صحيح
mch_print_blank_page(فراغ)
{
    mch_print_end_page();
    أرجع TRUE;
}

    فراغ
mch_print_start_هێڵ(صحيح margin, صحيح page_هێڵ)
{
    pctx.cur_x = pctx.چەپ_margin;
    إذا (margin)
	pctx.cur_x -= pctx.ژمار_width;

    pctx.cur_هێڵ = page_هێڵ;
}

    ساكن فراغ
long_to_rgb(long_u c, مزدوج *r, مزدوج *g, مزدوج *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    صحيح
mch_print_text_out(char_u *textp, صحيح len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    مزدوج		r, g, b;
    مزدوج		width = 0;
    مزدوج		next_x;

    // Always convert to UTF-8, Pango expects that
    إذا (output_conv.vc_type == CONV_NONE)
	str = textp;
    وإلا
	str = string_convert(&output_conv, textp, &len);

    إذا (str == عدم)
	أرجع TRUE;

    attr_list = pango_attr_list_new();

#define زیادکردن_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_زیادکردن((AttrList), tmp_attr_);	    \
    }G_STMT_END

    // Underline attribute does not work with pango_itemize().
    إذا ((pctx.draw_flags & PRT_DRAW_BOLD))
	زیادکردن_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    إذا (pctx.draw_flags & PRT_DRAW_ITALIC)
	زیادکردن_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef زیادکردن_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (ثابت حرف *)str, 0, len, attr_list, عدم);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    طالما (item_list != عدم)
    {
	PangoItem   *item;
	مزدوج	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((ثابت حرف *)str + item->offset, item->length,
		(ثابت حرف *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (مزدوج)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	إذا (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_هێڵ) - pctx.هێڵ_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_هێڵ));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    إذا (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	مزدوج y = GET_TEXT_Y(pctx.cur_هێڵ) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_هێڵ_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_هێڵ_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    next_x = pctx.cur_x + pctx.char_width;

    إذا (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    أرجع (next_x > pctx.ڕاست_margin) &&
	((next_x - pctx.ڕاست_margin) > (pctx.ڕاست_margin * 1e-5));
}

    فراغ
mch_print_set_font(صحيح bold, صحيح italic, صحيح underline)
{
    pctx.draw_flags = 0;

    إذا (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    إذا (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    إذا (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    فراغ
mch_print_set_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    فراغ
mch_print_set_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    فراغ
mch_print_cleanup(فراغ)
{
    g_clear_pointer(&pctx.cr, cairo_destroy);
    g_clear_pointer(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.text_context);
    g_clear_pointer(&pctx.font, pango_font_description_free);

#ifdef USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    إذا (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
