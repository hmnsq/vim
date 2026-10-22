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

typedef enum
{
    PRT_FORMAT_PS,  // Postscript
    PRT_FORMAT_PDF, // PDF
} prt_format_T;

/*
 * Global state for printing
 */
static struct
{
    // Cairo surface to draw on. Note that surface is scaled in points (1/72 of
    // an inch), for all formats.
    cairo_surface_t *surface;
    cairo_t	    *cr;
    PangoContext    *text_context;

    PangoFontDescription *font;

    char_u *filename;

    整数 media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    double char_width;
    double char_ascent;
    double char_height;
    double line_spacing;
    double ul_pos;
    double ul_thickness;

    double page_width;
    double page_height;

    double left_margin;
    double right_margin;
    double top_margin;
    double bottom_margin;

    double number_width;

    // Current position to draw from
    double cur_x;
    整数 cur_line;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#ifdef USE_GTK4_PRINT_DIALOG
    bool dialog;
#endif
} pctx;

static const char *print_formats[] = {
    [PRT_FORMAT_PS] = "postscript",
    [PRT_FORMAT_PDF] = "pdf",
};

/*
 * Convert the margin specified in "idx" to device units. "physsize" is the size
 * (width or height).
 */
    static double
to_device_units(整数 idx, double physsize, 整数 def_number)
{
    float	ret;
    整数		u;
    整数		nr;

    u = prt_get_unit(idx);
    如果 (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_number;
    }
    否则
	nr = printer_opts[idx].number;

    switch (u)
    {
	case PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    break;
	case PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    break;
	case PRT_UNIT_POINT:
	    ret = (double)nr;
	    break;
	case PRT_UNIT_PERC:
	default:
	    ret = physsize * (nr / 100.0);
	    break;
    }

    返回 ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    static void
get_page_margins(
	double	width,
	double	height,
	double	*left,
	double	*right,
	double	*top,
	double	*bottom)
{
    *left = to_device_units(OPT_PRINT_LEFT, width, 10);
    *right = width - to_device_units(OPT_PRINT_RIGHT, width, 5);

    *top = to_device_units(OPT_PRINT_TOP, height, 5);
    *bottom = height - to_device_units(OPT_PRINT_BOT, height, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    static 整数
get_cpl(void)
{
    如果 (prt_use_number())
    {
	pctx.number_width = PRINT_NUMBER_WIDTH * pctx.char_width;
	pctx.left_margin += pctx.number_width;
    }
    否则
	pctx.number_width = 0.0;

    返回 (整数)((pctx.right_margin - pctx.left_margin) / pctx.char_width);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    static 整数
get_lpp(void)
{
    整数 lpp;

    // Calculate lpp
    lpp = (整数)((pctx.bottom_margin - pctx.top_margin) / pctx.char_height);

    // Adjust top margin if there is a header
    pctx.top_margin += pctx.char_height * prt_header_height();

    返回 lpp - prt_header_height();
}

/*
 * Get the font description from the given name. Returns NULL on failure.
 */
    static PangoFontDescription *
get_font(char_u *name)
{
    PangoFontDescription *font;

    如果 (name == NULL)
	返回 NULL;

    font = pango_font_description_from_string((const char *)name);
    如果 (font == NULL)
    {
	semsg(_(e_unknown_printer_font_str), name);
	返回 NULL;
    }

    // Ensure a size is set
    如果 (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    返回 font;
}

    整数
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	整数		forceit UNUSED)
{
    char_u	*paper_name;
    整数		paper_strlen;
    bool	portrait;
    整数		media;
    char_u	*filename = NULL;
#ifdef USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    如果 (gui.in_use && !forceit && psettings->outfile == NULL)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.page_width, &pctx.page_height);

	如果 (filename == NULL)
	    返回 FAIL;
	dialog = true;
    }
#endif

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#ifdef USE_GTK4_PRINT_DIALOG
    如果 (!dialog)
#endif
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	如果 (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
	}
	否则
	{
	    paper_name = (char_u *)"A4";
	    paper_strlen = 2;
	}

	// Set width and height using given paper type
	为 (media = 0; media < (整数)PRT_MEDIASIZE_LEN; ++media)
	    如果 ((整数)STRLEN(prt_mediasize[media].name) == paper_strlen
		    && STRNICMP(prt_mediasize[media].name,
			paper_name, paper_strlen) == 0)
		break;
	如果 (media == PRT_MEDIASIZE_LEN)
	    media = 0;
	pctx.media = media;

	如果 (portrait)
	{
	    pctx.page_width = prt_mediasize[media].width;
	    pctx.page_height = prt_mediasize[media].height;
	}
	否则
	{
	    pctx.page_width = prt_mediasize[media].height;
	    pctx.page_height = prt_mediasize[media].width;
	}

	如果 (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    为 (整数 i = 0; i < (整数)ARRAY_LENGTH(print_formats); i++)
		如果 (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].strlen) == 0)
		{
		    pctx.format = i;
		    break;
		}
	}
    }
    psettings->user_abort = FALSE;

#ifdef USE_GTK4_PRINT_DIALOG
    如果 (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#endif

    如果 (filename == NULL)
    {
	// If the user didn't specify a file name, use a temp file.
	如果 (psettings->outfile == NULL)
	{
	    filename = vim_tempname('p', TRUE);
	    如果 (filename == NULL)
	    {
		emsg(_(e_cant_get_temp_file_name));
		返回 FAIL;
	    }
	}
	否则
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    如果 (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((const char *)filename,
		pctx.page_width, pctx.page_height);
    否则
	pctx.surface = cairo_pdf_surface_create((const char *)filename,
		pctx.page_width, pctx.page_height);

    pctx.filename = (char_u *)filename;

    如果 (cairo_surface_status(pctx.surface) != CAIRO_STATUS_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	返回 FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.text_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    如果 (pctx.font == NULL)
    {
	mch_print_cleanup();
	返回 FAIL;
    }

    pango_context_set_font_description(pctx.text_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_set_resolution(pctx.text_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	整数		    ascent;
	整数		    descent;

	layout = pango_layout_new(pctx.text_context);
	pango_layout_set_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, NULL);

	pctx.char_width =
	    (double)pango_font_metrics_get_approximate_char_width(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (double)ascent / PANGO_SCALE;
	pctx.char_height = (double)(ascent + descent) / PANGO_SCALE;

	pctx.line_spacing = (double)
	    (pango_font_metrics_get_height(metrics) - (ascent + descent))
	    / PANGO_SCALE;

	pctx.ul_pos =
	    (double)pango_font_metrics_get_underline_position(metrics)
	    / PANGO_SCALE;
	pctx.ul_thickness =
	    (double)pango_font_metrics_get_underline_thickness(metrics)
	    / PANGO_SCALE;

	pango_font_metrics_unref(metrics);
	g_object_unref(layout);
    }

    // Calculate margins
    {
	double  left;
	double	right;
	double	top;
	double	bottom;

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
    如果 (!dialog)
#endif
    {
	pctx.collate = (!printer_opts[OPT_PRINT_COLLATE].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');

	pctx.duplex = TRUE;
	pctx.tumble = FALSE;
	psettings->duplex = TRUE;
	如果 (printer_opts[OPT_PRINT_DUPLEX].present)
	{
	    如果 (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	    {
		pctx.duplex = FALSE;
		psettings->duplex = 0;
	    }
	    否则 如果 (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short",
			5) == 0)
		pctx.tumble = TRUE;
	}
    }

#ifdef USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#endif

    返回 OK;
}

/*
 * Emit a DSC comment, "fmt" may be NULL to indicate no arguments.
 */
    static void
emit_dsc_comment(char *comment, char *fmt, ...)
{
    static char buf[257];

    va_list ap;
    整数	    len = sizeof(buf);
    整数	    o;
    char    *s = buf;

    如果 (fmt == NULL)
    {
	o = vim_snprintf(s, len, "%%%%%s", comment);
	goto exit;
    }
    否则
	o = vim_snprintf(s, len, "%%%%%s: ", comment);

    len -= o;
    s += o;
    如果 (len <= 0)
	返回;

    va_start(ap, fmt);
    vim_vsnprintf(s, len, fmt, ap);
    va_end(ap);

exit:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    整数
mch_print_begin(prt_settings_T *psettings)
{
    char user[256];

    pctx.cur_x = 0.0;
    pctx.cur_line = 0;
    pctx.draw_flags = 0;

#ifdef USE_GTK4_PRINT_DIALOG
    如果 (pctx.dialog)
	返回 TRUE;
#endif

    如果 (!get_user_name((char_u *)user, sizeof(user)))
	STRCPY(user, "Unknown");

    如果 (pctx.format == PRT_FORMAT_PS)
    {
	emit_dsc_comment("Title", (char *)psettings->jobname);
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
    否则 如果 (pctx.format == PRT_FORMAT_PDF)
    {
	char_u *str = (char_u *)user;

	如果 (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, NULL);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (char *)str);
	如果 (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	如果 (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, NULL);
	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (char *)str);
	如果 (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_set_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_VERSION_LONG);
    }

    返回 TRUE;
}

    void
mch_print_end(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_finish(pctx.surface);

    如果 (psettings->outfile == NULL && !got_int && !psettings->user_abort
	    && pctx.filename != NULL
#ifdef USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#endif
       )
    {
	msg(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	如果 (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	否则
	    msg(_("Print job sent."));
    }

    mch_print_cleanup();
}

    整数
mch_print_end_page(void)
{
    cairo_show_page(pctx.cr);
    返回 TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    整数
mch_print_begin_page(char_u *str UNUSED)
{
    返回 TRUE;
}

    整数
mch_print_blank_page(void)
{
    mch_print_end_page();
    返回 TRUE;
}

    void
mch_print_start_line(整数 margin, 整数 page_line)
{
    pctx.cur_x = pctx.left_margin;
    如果 (margin)
	pctx.cur_x -= pctx.number_width;

    pctx.cur_line = page_line;
}

    static void
long_to_rgb(long_u c, double *r, double *g, double *b)
{
    *r = ((c >> 16) & 0xFF) / 255.0;
    *g = ((c >> 8) & 0xFF) / 255.0;
    *b = ((c >> 0) & 0xFF) / 255.0;
}

/*
 * Don't use PangoLayout for rendering the text, so that we can handle the
 * positioning and sizing ourselves.
 */
    整数
mch_print_text_out(char_u *textp, 整数 len)
{
    PangoAttrList	*attr_list;
    GList		*item_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    double		r, g, b;
    double		width = 0;
    double		next_x;

    // Always convert to UTF-8, Pango expects that
    如果 (output_conv.vc_type == CONV_NONE)
	str = textp;
    否则
	str = string_convert(&output_conv, textp, &len);

    如果 (str == NULL)
	返回 TRUE;

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
    如果 ((pctx.draw_flags & PRT_DRAW_BOLD))
	INSERT_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    如果 (pctx.draw_flags & PRT_DRAW_ITALIC)
	INSERT_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#undef INSERT_PANGO_ATTR

    item_list = pango_itemize(pctx.text_context,
	    (const char *)str, 0, len, attr_list, NULL);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    当 (item_list != NULL)
    {
	PangoItem   *item;
	double	    o;

	item = (PangoItem *)item_list->data;
	item_list = g_list_delete_link(item_list, item_list);

	pango_shape_full((const char *)str + item->offset, item->length,
		(const char *)str, len, &item->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (double)pango_glyph_string_get_width(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	如果 (pctx.cur_bg != PRCOLOR_WHITE)
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
    如果 (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	double y = GET_TEXT_Y(pctx.cur_line) - pctx.ul_pos;

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

    如果 (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    返回 (next_x > pctx.right_margin) &&
	((next_x - pctx.right_margin) > (pctx.right_margin * 1e-5));
}

    void
mch_print_set_font(整数 bold, 整数 italic, 整数 underline)
{
    pctx.draw_flags = 0;

    如果 (bold)
	pctx.draw_flags |= PRT_DRAW_BOLD;
    如果 (italic)
	pctx.draw_flags |= PRT_DRAW_ITALIC;
    如果 (underline)
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
    如果 (pctx.dialog)
	gui_gtk4_print_finish();
#endif

    g_clear_pointer(&pctx.filename, vim_free);
}

#endif // FEAT_PRINT_PANGO
