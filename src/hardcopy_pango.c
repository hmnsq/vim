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

#include "vim.h"
#include "version.h"

#ifdef FEAT_PRINT_PANGO

#include "hardcopy.h"

#include <cairo/cairo-ps.h>
#include <cairo/cairo-pdf.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

#define PRINT_DPI 72.0

#define PRT_DRAW_BOLD 1
#define PRT_DRAW_ITALIC 2
#define PRT_DRAW_UNDERLINE 4

#define GET_Y(l) (pctx.top_margin + (l) * pctx.char_height)
#define GET_TEXT_Y(l) (GET_Y(l) + pctx.char_ascent)

typedef ga_nanaa
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
static sa_nracanaa
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    puur_naa_nka media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    dviguu_na char_width;
    dviguu_na char_ascent;
    dviguu_na char_height;
    dviguu_na line_spacing;
    dviguu_na ul_pos;
    dviguu_na ul_thickness;

    dviguu_na page_width;
    dviguu_na page_height;

    dviguu_na left_margin;
    dviguu_na right_margin;
    dviguu_na top_margin;
    dviguu_na bottom_margin;

    dviguu_na number_width;

    // Current position to draw from
    dviguu_na cur_x;
    puur_naa_nka cur_line;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

static const var_na *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    static dviguu_na
to_device_units(puur_naa_nka idx, dviguu_na physsize, puur_naa_nka def_number)
{
    float	ret;
    puur_naa_nka		u;
    puur_naa_nka		nr;

    u = prt_get_unit(idx);
    ᬬᬾᬦ᭄ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_number;
    }
    ᬮᬶᬬ
	nr = printer_opts[idx].number;

    switch (u)
    {
	pak_sa PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    bha_nga;
	pak_sa PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    bha_nga;
	pak_sa PRT_UNIT_POINT:
	    ret = (dviguu_na)nr;
	    bha_nga;
	pak_sa PRT_UNIT_PERC:
	default:
	    ret = physsize * (nr / 100.0);
	    bha_nga;
    }

    ᬯᬮᬶ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    static void
get_page_margins(
	dviguu_na	width,
	dviguu_na	height,
	dviguu_na	*left,
	dviguu_na	*right,
	dviguu_na	*top,
	dviguu_na	*bottom)
{
    *left = to_device_units(OPT_PRINT_LEFT, width, 10);
    *right = width - to_device_units(OPT_PRINT_RIGHT, width, 5);

    *top = to_device_units(OPT_PRINT_TOP, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    static puur_naa_nka
get_cpl(void)
{
    ᬬᬾᬦ᭄ (prt_use_number())
    {
	pctx.number_width = PRINT_NUMBER_WIDTH * pctx.char_width;
	pctx.left_margin += pctx.number_width;
    }
    ᬮᬶᬬ
	pctx.number_width = 0.0;

    ᬯᬮᬶ (puur_naa_nka)((pctx.right_margin - pctx.left_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    static puur_naa_nka
get_lpp(void)
{
    puur_naa_nka lpp;

    // Calculate lpp
    lpp = (puur_naa_nka)((pctx.bottom_margin - pctx.top_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.top_margin += pctx.char_height * prt_header_height();

    ᬯᬮᬶ lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    static PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    ᬬᬾᬦ᭄ (name == NULL)
	ᬯᬮᬶ NULL;

    font = pango_font_description_from_string((const var_na *)name);
    ᬬᬾᬦ᭄ (font == NULL)
    {
	semsg(_(e_unknown_printer_font_str), name);
	ᬯᬮᬶ NULL;
    }

    // Ensure a size is set
    ᬬᬾᬦ᭄ (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    ᬯᬮᬶ font;
}

    puur_naa_nka
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	puur_naa_nka		forceit UNUSED)
{
    char_u	*paper_name;
    puur_naa_nka		paper_strlen;
    bool	portrait;
    puur_naa_nka		media;
    char_u	*filename = NULL;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    ᬬᬾᬦ᭄ (gui.in_use && !forceit && psettings->outfile == NULL)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	ᬬᬾᬦ᭄ (filename == NULL)
	    ᬯᬮᬶ FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    ᬬᬾᬦ᭄ (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	ᬬᬾᬦ᭄ (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
	}
	ᬮᬶᬬ
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	for (media = 0; media < (puur_naa_nka)PRT_MEDIASIZE_LEN; ++media)
	    ᬬᬾᬦ᭄ ((puur_naa_nka)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		bha_nga;
	ᬬᬾᬦ᭄ (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	ᬬᬾᬦ᭄ (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	ᬮᬶᬬ
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	ᬬᬾᬦ᭄ (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    for (puur_naa_nka i = 0; i < (puur_naa_nka)ARRAY_LENGTH(print_formats); i++)
		ᬬᬾᬦ᭄ (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    bha_nga;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    ᬬᬾᬦ᭄ (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    ᬬᬾᬦ᭄ (filename == NULL)
    {
	// If the user didn't specify a file name, use a temp file.
	ᬬᬾᬦ᭄ (psettings->outfile == NULL)
	{
	    filename = vim_tempname('p', TRUE);
	    ᬬᬾᬦ᭄ (filename == NULL)
	    {
		emsg(_(e_cant_get_temp_file_name));
		ᬯᬮᬶ FAIL;
	    }
	}
	ᬮᬶᬬ
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    ᬬᬾᬦ᭄ (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((const var_na *)filename,
		pctx.page_width, pctx.page_height);
    ᬮᬶᬬ
	pctx.surface = cairo_pdf_surface_create((const var_na *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    ᬬᬾᬦ᭄ (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ᬯᬮᬶ FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    ᬬᬾᬦ᭄ (pctx.font == NULL)
    {
	mch_print_cleanup();
	ᬯᬮᬶ FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	puur_naa_nka		    ascent;
	puur_naa_nka		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, NULL);

	pctx.char_width =
	    (dviguu_na)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (dviguu_na)ascent / PANGO_SCALE;
	pctx.char_height = (dviguu_na)(ascent + descent) / PANGO_SCALE;

	pctx.line_spacing = (dviguu_na)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (dviguu_na)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (dviguu_na)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	dviguu_na  left;
	dviguu_na	right;
	dviguu_na	top;
	dviguu_na	bottom;

	get_page_margins(pctx.page_width, pctx.page_height, &left, &right, &top,
		&bottom);
	pctx.left_margin = left;
	pctx.right_margin = right;
	pctx.top_margin = top;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_per_line = get_cpl();
    psettings->lines_per_page = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#ifdef USE_GTK4_PRINT_DIALOG
    ᬬᬾᬦ᭄ (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	ᬬᬾᬦ᭄ (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    ᬬᬾᬦ᭄ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    ᬯᬮᬶ OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    static void
emit_dsc_comment(var_na *comment, var_na *fmt, ...)
{
    static var_na buf[257];

    va_list ap;
    puur_naa_nka	    len = parimaa_na(buf);
    puur_naa_nka	    o;
    var_na    *s = buf;

    ᬬᬾᬦ᭄ (fmt == NULL)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	goto exit;
    }
    ᬮᬶᬬ
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    ᬬᬾᬦ᭄ (len <= 0)
	ᬯᬮᬶ;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    puur_naa_nka
mch_print_begin(prt_settings_T *psettings)
{
    var_na user[256];

    pctx.cur_x = 0.0;
    pctx.cur_line = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    ᬬᬾᬦ᭄ (pctx.dialog)
	ᬯᬮᬶ TRUE;
#endif

    ᬬᬾᬦ᭄ (!get_user_name((char_u *)user, parimaa_na(user)))
	STRCPY(user, "Unknown");

    ᬬᬾᬦ᭄ (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (var_na *)psettings->jobname);
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
	emit_dsc_comment("BeginDefaults", NULL);
	emit_dsc_comment("PageMedia", prt_mediasize[pctx.media].name);
	emit_dsc_comment("EndDefaults", NULL);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	ᬬᬾᬦ᭄ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, NULL);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (var_na *)str);
	ᬬᬾᬦ᭄ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	ᬬᬾᬦ᭄ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, NULL);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (var_na *)str);
	ᬬᬾᬦ᭄ (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    ᬯᬮᬶ TRUE;
}

    void
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    ᬬᬾᬦ᭄ (psettings->outfile == NULL && !got_int && !psettings->user_abort
	    && pctx.filename != NULL
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	msg(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ᬬᬾᬦ᭄ (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	ᬮᬶᬬ
	    msg(_("Print job sent."));
    }

    mch_print_cleanup();
}

    puur_naa_nka
mch_print_end_page(void)
{
    cairo_show_page(pctx.cr);
    ᬯᬮᬶ TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    puur_naa_nka
mch_print_begin_page(char_u *str UNUSED)
{
    ᬯᬮᬶ TRUE;
}

    puur_naa_nka
mch_print_blank_page(void)
{
    mch_print_end_page();
    ᬯᬮᬶ TRUE;
}

    void
mch_print_start_line(puur_naa_nka margin, puur_naa_nka page_line)
{
    pctx.cur_x = pctx.left_margin;
    ᬬᬾᬦ᭄ (margin)
	pctx.cur_x -= pctx.number_width;

    pctx.cur_line = page_line;
}

    static void
long_to_rgb(long_u c, dviguu_na *r, dviguu_na *g, dviguu_na *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    puur_naa_nka
mch_print_text_out(char_u *textp, puur_naa_nka len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    dviguu_na		r, g, b;
    dviguu_na		width = 0;
    dviguu_na		next_x;

    // Always convert to UTF-8, Pango expects that
    ᬬᬾᬦ᭄ (output_conv.vc_type == CONV_NONE)
	str = textp;
    ᬮᬶᬬ
	str = string_convert(&output_conv, textp, &len);

    ᬬᬾᬦ᭄ (str == NULL)
	ᬯᬮᬶ TRUE;

    attr_list = pango_attr_list_new();

#define INSERT_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_insert((AttrList), tmp_attr_);	    \
    }G_STMT_END

    // Underline attribute does not work with pango_itemize().
    ᬬᬾᬦ᭄ ((pctx.draw_flags & PRT_DRAW_BOLD))
	INSERT_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    ᬬᬾᬦ᭄ (pctx.draw_flags & PRT_DRAW_ITALIC)
	INSERT_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef INSERT_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (const var_na *)str, 0, len, attr_list, NULL);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    ᬲᭂᬤᭂᬮ (item_list != NULL)
    {
	PangoItem   *item;
	dviguu_na	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((const var_na *)str + item->offset, item->length,
		(const var_na *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (dviguu_na)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	ᬬᬾᬦ᭄ (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_to_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_set_source_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + width,
		    GET_Y(pctx.cur_line) - pctx.line_spacing, o, pctx.char_height);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_move_to(pctx.cr, pctx.cur_x + width, GET_TEXT_Y(pctx.cur_line));
	pango_cairo_show_glyph_string(pctx.cr, item->analysis.font, glyphs);

	pango_item_free(item);
	width += o;
    }

    // Draw underline if needed
    ᬬᬾᬦ᭄ (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	dviguu_na y = GET_TEXT_Y(pctx.cur_line) - pctx.ul_pos;

	long_to_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_set_source_rgb(pctx.cr, r, g, b);
	cairo_set_line_width(pctx.cr, pctx.ul_thickness);
	cairo_move_to(pctx.cr, pctx.cur_x, y);
	cairo_line_to(pctx.cr, pctx.cur_x + width, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += width;

    next_x = pctx.cur_x + pctx.char_width;

    ᬬᬾᬦ᭄ (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    ᬯᬮᬶ (next_x > pctx.right_margin) &&
	((next_x - pctx.right_margin) > (pctx.right_margin * 1e-5));
}

    void
mch_print_set_font(puur_naa_nka bold, puur_naa_nka italic, puur_naa_nka underline)
{
    pctx.draw_flags = 0;

    ᬬᬾᬦ᭄ (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    ᬬᬾᬦ᭄ (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    ᬬᬾᬦ᭄ (underline)
	pctx.draw_flags |= PRT_DRAW_UNDERLINE;
}

    void
mch_print_set_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    void
mch_print_set_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    void
mch_print_cleanup(void)
{
    g_clear_pointer(&pctx.cr, cairo_destroy);
    g_clear_pointer(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.text_context);
    g_clear_pointer(&pctx.font, pango_font_description_free);

#ifdef USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    ᬬᬾᬦ᭄ (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
