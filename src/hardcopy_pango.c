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

#شامل "vim.h"
#شامل "version.h"

#ifdef FEAT_PRINT_PANGO

#شامل "hardcopy.h"

#شامل <cairo/cairo-ps.h>
#شامل <cairo/cairo-pdf.h>
#شامل <pango/pango.h>
#شامل <pango/pangocairo.h>

#define PRINT_DPI 72.0

#define PRT_DRAW_BOLD 1
#define PRT_DRAW_ITALIC 2
#define PRT_DRAW_UNDERLINE 4

#define GET_Y(l) (pctx.بالاترین_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

تعریفنوع شمارش
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
ایستا ساختار
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    صحیح media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    دوگانه char_width;
    دوگانه char_ascent;
    دوگانه char_height;
    دوگانه سطر_spacing;
    دوگانه ul_pos;
    دوگانه ul_thickness;

    دوگانه page_width;
    دوگانه page_height;

    دوگانه چپ_margin;
    دوگانه راست_margin;
    دوگانه بالاترین_margin;
    دوگانه bottom_margin;

    دوگانه عدد_width;

    // Current position to draw from
    دوگانه cur_x;
    صحیح cur_سطر;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

ایستا ثابت نویسه *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    ایستا دوگانه
to_device_units(صحیح idx, دوگانه physsize, صحیح def_عدد)
{
    اعشاری	ret;
    صحیح		u;
    صحیح		nr;

    u = prt_get_unit(idx);
    اگر (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_عدد;
    }
    وگرنه
	nr = printer_opts[idx].عدد;

    انتخاب (u)
    {
	حالت PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    بشکن;
	حالت PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    بشکن;
	حالت PRT_UNIT_POINT:
	    ret = (دوگانه)nr;
	    بشکن;
	حالت PRT_UNIT_PERC:
	پیشفرض:
	    ret = physsize * (nr / 100.0);
	    بشکن;
    }

    بازگشت ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ایستا تهی
get_page_margins(
	دوگانه	width,
	دوگانه	height,
	دوگانه	*چپ,
	دوگانه	*راست,
	دوگانه	*بالاترین,
	دوگانه	*bottom)
{
    *چپ = to_device_units(OPT_PRINT_چپ, width, 10);
    *راست = width - to_device_units(OPT_PRINT_راست, width, 5);

    *بالاترین = to_device_units(OPT_PRINT_بالاترین, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    ایستا صحیح
get_cpl(تهی)
{
    اگر (prt_use_عدد())
    {
	pctx.عدد_width = PRINT_عدد_WIDTH * pctx.char_width;
	pctx.چپ_margin += pctx.عدد_width;
    }
    وگرنه
	pctx.عدد_width = 0.0;

    بازگشت (صحیح)((pctx.راست_margin - pctx.چپ_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ایستا صحیح
get_lpp(تهی)
{
    صحیح lpp;

    // Calculate lpp
    lpp = (صحیح)((pctx.bottom_margin - pctx.بالاترین_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.بالاترین_margin += pctx.char_height * prt_header_height();

    بازگشت lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    ایستا PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    اگر (name == پوچ)
	بازگشت پوچ;

    font = pango_font_description_from_string((ثابت نویسه *)name);
    اگر (font == پوچ)
    {
	semsg(_(e_unknown_printer_font_str), name);
	بازگشت پوچ;
    }

    // Ensure a size is set
    اگر (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    بازگشت font;
}

    صحیح
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	صحیح		forceit UNUSED)
{
    char_u	*paper_name;
    صحیح		paper_strlen;
    bool	portrait;
    صحیح		media;
    char_u	*filename = پوچ;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    اگر (gui.in_use && !forceit && psettings->outfile == پوچ)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	اگر (filename == پوچ)
	    بازگشت FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    اگر (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	اگر (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
	}
	وگرنه
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	برای (media = 0; media < (صحیح)PRT_MEDIASIZE_LEN; ++media)
	    اگر ((صحیح)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		بشکن;
	اگر (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	اگر (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	وگرنه
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	اگر (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    برای (صحیح i = 0; i < (صحیح)ARRAY_LENGTH(print_formats); i++)
		اگر (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    بشکن;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    اگر (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    اگر (filename == پوچ)
    {
	// If the user didn't specify a file name, use a temp file.
	اگر (psettings->outfile == پوچ)
	{
	    filename = vim_tempname('p', TRUE);
	    اگر (filename == پوچ)
	    {
		emsg(_(e_cant_get_temp_file_name));
		بازگشت FAIL;
	    }
	}
	وگرنه
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    اگر (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ثابت نویسه *)filename,
		pctx.page_width, pctx.page_height);
    وگرنه
	pctx.surface = cairo_pdf_surface_create((ثابت نویسه *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    اگر (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	بازگشت FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    اگر (pctx.font == پوچ)
    {
	mch_print_cleanup();
	بازگشت FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	صحیح		    ascent;
	صحیح		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, پوچ);

	pctx.char_width =
	    (دوگانه)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (دوگانه)ascent / PANGO_SCALE;
	pctx.char_height = (دوگانه)(ascent + descent) / PANGO_SCALE;

	pctx.سطر_spacing = (دوگانه)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (دوگانه)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (دوگانه)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	دوگانه  چپ;
	دوگانه	راست;
	دوگانه	بالاترین;
	دوگانه	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &چپ, &راست, &بالاترین,
		&bottom);
	pctx.چپ_margin = چپ;
	pctx.راست_margin = راست;
	pctx.بالاترین_margin = بالاترین;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_سطر = get_cpl();
    psettings->سطرها_per_page = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ifdef USE_GTK4_PRINT_DIALOG
    اگر (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	اگر (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    اگر (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    وگرنه اگر (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    بازگشت OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    ایستا تهی
emit_dsc_comment(نویسه *comment, نویسه *fmt, ...)
{
    ایستا نویسه buf[257];

    va_list ap;
    صحیح	    len = اندازه(buf);
    صحیح	    o;
    نویسه    *s = buf;

    اگر (fmt == پوچ)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	برو exit;
    }
    وگرنه
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    اگر (len <= 0)
	بازگشت;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    صحیح
mch_print_begin(prt_settings_T *psettings)
{
    نویسه user[256];

    pctx.cur_x = 0.0;
    pctx.cur_سطر = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    اگر (pctx.dialog)
	بازگشت TRUE;
#endif

    اگر (!get_user_name((char_u *)user, اندازه(user)))
	STRCPY(user, "Unknown");

    اگر (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (نویسه *)psettings->jobname);
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
	emit_dsc_comment("BeginDefaults", پوچ);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", پوچ);
    }
    وگرنه اگر (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	اگر (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, پوچ);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (نویسه *)str);
	اگر (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	اگر (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, پوچ);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (نویسه *)str);
	اگر (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    بازگشت TRUE;
}

    تهی
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    اگر (psettings->outfile == پوچ && !got_int && !psettings->user_abort
	    && pctx.filename != پوچ
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	پیام(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	اگر (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	وگرنه
	    پیام(_("Print job sent."));
    }

    mch_print_cleanup();
}

    صحیح
mch_print_end_page(تهی)
{
    cairo_show_page(pctx.cr);
    بازگشت TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    صحیح
mch_print_begin_page(char_u *str UNUSED)
{
    بازگشت TRUE;
}

    صحیح
mch_print_blank_page(تهی)
{
    mch_print_end_page();
    بازگشت TRUE;
}

    تهی
mch_print_start_سطر(صحیح margin, صحیح page_سطر)
{
    pctx.cur_x = pctx.چپ_margin;
    اگر (margin)
	pctx.cur_x -= pctx.عدد_width;

    pctx.cur_سطر = page_سطر;
}

    ایستا تهی
long_to_rgb(long_u c, دوگانه *r, دوگانه *g, دوگانه *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    صحیح
mch_print_text_out(char_u *textp, صحیح len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    دوگانه		r, g, b;
    دوگانه		width = 0;
    دوگانه		next_x;

    // Always convert to UTF-8, Pango expects that
    اگر (output_conv.vc_type == CONV_NONE)
	str = textp;
    وگرنه
	str = string_convert(&output_conv, textp, &len);

    اگر (str == پوچ)
	بازگشت TRUE;

    attr_list = pango_attr_list_new();

#define درج_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_درج((AttrList), tmp_attr_);	    \
    }G_STMT_END

    // Underline attribute does not work with pango_itemize().
    اگر ((pctx.draw_flags & PRT_DRAW_BOLD))
	درج_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    اگر (pctx.draw_flags & PRT_DRAW_ITALIC)
	درج_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef درج_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (ثابت نویسه *)str, 0, len, attr_list, پوچ);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    وقتی (item_list != پوچ)
    {
	PangoItem   *item;
	دوگانه	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((ثابت نویسه *)str + item->offset, item->length,
		(ثابت نویسه *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (دوگانه)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	اگر (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_سطر) - pctx.سطر_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_سطر));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    اگر (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	دوگانه y = GET_TEXT_Y(pctx.cur_سطر) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_سطر_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_سطر_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    next_x = pctx.cur_x + pctx.char_width;

    اگر (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    بازگشت (next_x > pctx.راست_margin) &&
	((next_x - pctx.راست_margin) > (pctx.راست_margin * 1e-5));
}

    تهی
mch_print_set_font(صحیح bold, صحیح italic, صحیح underline)
{
    pctx.draw_flags = 0;

    اگر (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    اگر (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    اگر (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    تهی
mch_print_set_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    تهی
mch_print_set_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    تهی
mch_print_cleanup(تهی)
{
    g_clear_pointer(&pctx.cr, cairo_destroy);
    g_clear_pointer(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.text_context);
    g_clear_pointer(&pctx.font, pango_font_description_free);

#ifdef USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    اگر (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
