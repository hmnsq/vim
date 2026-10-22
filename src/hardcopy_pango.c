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

#جيڪڏهن_وصف_ٿيل FEAT_PRINT_PANGO

#ضمن "hardcopy.h"

#ضمن <cairo/cairo-ps.h>
#ضمن <cairo/cairo-pdf.h>
#ضمن <pango/pango.h>
#ضمن <pango/pangocairo.h>

#وصف_ڪريو PRINT_DPI 72.0

#وصف_ڪريو PRT_DRAW_BOLD 1
#وصف_ڪريو PRT_DRAW_ITALIC 2
#وصف_ڪريو PRT_DRAW_UNDERLINE 4

#وصف_ڪريو GET_Y(l) (pctx.چوٽي_margin + (l) * pctx.char_اوچائي)
#وصف_ڪريو GET_متن_Y(l) (GET_Y(l) + pctx.char_ascent)

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
    PangoContext    *متن_context;

    PangoFontDescription *font;

    char_u *filename;

    صحيح media;
    bool portrait;
    bool collate;
    bool duplex;
    bool tumble; // Short edge duplex

    prt_format_T format;

    مزدوج char_ويڪر;
    مزدوج char_ascent;
    مزدوج char_اوچائي;
    مزدوج سٽ_spacing;
    مزدوج ul_pos;
    مزدوج ul_thickness;

    مزدوج صفحو_ويڪر;
    مزدوج صفحو_اوچائي;

    مزدوج کاٻو_margin;
    مزدوج ساڄو_margin;
    مزدوج چوٽي_margin;
    مزدوج bottom_margin;

    مزدوج عدد_ويڪر;

    // Current position to draw from
    مزدوج cur_x;
    صحيح cur_سٽ;
    long_u cur_fg;
    long_u cur_bg;

    char_u draw_flags;

#جيڪڏهن_وصف_ٿيل USE_GTK4_PRINT_DIALOG
    bool dialog;
#جيڪڏهن_پورو
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
منزل_device_units(صحيح idx, مزدوج physsize, صحيح def_عدد)
{
    عائم	ret;
    صحيح		u;
    صحيح		nr;

    u = prt_get_unit(idx);
    إذا (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_عدد;
    }
    وإلا
	nr = printer_opts[idx].عدد;

    بدل (u)
    {
	حالة PRT_UNIT_INCH:
	    ret = (nr * PRINT_DPI);
	    اكسر;
	حالة PRT_UNIT_MM:
	    ret = (nr * PRINT_DPI) / 25.4;
	    اكسر;
	حالة PRT_UNIT_نقطو:
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
get_صفحو_margins(
	مزدوج	ويڪر,
	مزدوج	اوچائي,
	مزدوج	*کاٻو,
	مزدوج	*ساڄو,
	مزدوج	*چوٽي,
	مزدوج	*bottom)
{
    *کاٻو = منزل_device_units(OPT_PRINT_کاٻو, ويڪر, 10);
    *ساڄو = ويڪر - منزل_device_units(OPT_PRINT_ساڄو, ويڪر, 5);

    *چوٽي = منزل_device_units(OPT_PRINT_چوٽي, اوچائي, 5);
    *bottom = اوچائي - منزل_device_units(OPT_PRINT_BOT, اوچائي, 5);
}

/*
 * Get characters per line (excluding margins).
 */
    ساكن صحيح
get_cpl(فراغ)
{
    إذا (prt_use_عدد())
    {
	pctx.عدد_ويڪر = PRINT_عدد_ويڪر * pctx.char_ويڪر;
	pctx.کاٻو_margin += pctx.عدد_ويڪر;
    }
    وإلا
	pctx.عدد_ويڪر = 0.0;

    أرجع (صحيح)((pctx.ساڄو_margin - pctx.کاٻو_margin) / pctx.char_ويڪر);
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ساكن صحيح
get_lpp(فراغ)
{
    صحيح lpp;

    // Calculate lpp
    lpp = (صحيح)((pctx.bottom_margin - pctx.چوٽي_margin) / pctx.char_اوچائي);

    // Adjust top margin if there is a header
    pctx.چوٽي_margin += pctx.char_اوچائي * prt_سرو_اوچائي();

    أرجع lpp - prt_سرو_اوچائي();
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
    إذا (pango_font_description_get_سائيز(font) <= 0)
	pango_font_description_سيٽ_سائيز(font, 10 * PANGO_SCALE);

    أرجع font;
}

    صحيح
mch_print_init(
	prt_settings_T  *psettings,
	char_u		*jobname,
	صحيح		forceit اڻ_استعمال_ٿيل)
{
    char_u	*paper_name;
    صحيح		paper_strlen;
    bool	portrait;
    صحيح		media;
    char_u	*filename = عدم;
#جيڪڏهن_وصف_ٿيل USE_GTK4_PRINT_DIALOG
    bool	dialog = false;

    إذا (gui.اندريون_use && !forceit && psettings->outfile == عدم)
    {
	filename = gui_gtk4_print_dialog(psettings, jobname,
		&pctx.صفحو_ويڪر, &pctx.صفحو_اوچائي);

	إذا (filename == عدم)
	    أرجع FAIL;
	dialog = true;
    }
#جيڪڏهن_پورو

    pctx.format = PRT_FORMAT_PS;

    // Let GTK handle stuff when using dialog. Not sure if our postscript DSC
    // comments can interfere with dialog settings, but don't add them...
#جيڪڏهن_وصف_ٿيل USE_GTK4_PRINT_DIALOG
    إذا (!dialog)
#جيڪڏهن_پورو
    {
	// Get paper type to use
	portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present ||
		TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
	pctx.portrait = portrait;
	إذا (printer_opts[OPT_PRINT_PAPER].present)
	{
	    paper_name = printer_opts[OPT_PRINT_PAPER].string;
	    paper_strlen = printer_opts[OPT_PRINT_PAPER].لڙي_ڊيگهه;
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
	    pctx.صفحو_ويڪر = prt_mediasize[media].ويڪر;
	    pctx.صفحو_اوچائي = prt_mediasize[media].اوچائي;
	}
	وإلا
	{
	    pctx.صفحو_ويڪر = prt_mediasize[media].اوچائي;
	    pctx.صفحو_اوچائي = prt_mediasize[media].ويڪر;
	}

	إذا (printer_opts[OPT_PRINT_FORMAT].present)
	{
	    لكل (صحيح i = 0; i < (صحيح)ARRAY_ڊيگهائي(print_formats); i++)
		إذا (STRNCMP(printer_opts[OPT_PRINT_FORMAT].string,
			    print_formats[i],
			    printer_opts[OPT_PRINT_FORMAT].لڙي_ڊيگهه) == 0)
		{
		    pctx.format = i;
		    اكسر;
		}
	}
    }
    psettings->user_abort = FALSE;

#جيڪڏهن_وصف_ٿيل USE_GTK4_PRINT_DIALOG
    إذا (dialog)
	// Always use PDF for dialog
	pctx.format = PRT_FORMAT_PDF;
#جيڪڏهن_پورو

    إذا (filename == عدم)
    {
	// If the user didn't specify a file name, use a temp file.
	إذا (psettings->outfile == عدم)
	{
	    filename = vim_tempname('p', TRUE);
	    إذا (filename == عدم)
	    {
		emsg(_(e_cant_get_عارضي_file_name));
		أرجع FAIL;
	    }
	}
	وإلا
	    filename = expand_env_save(psettings->outfile);
    }

    // If using dialog, then always use PDF
    إذا (pctx.format == PRT_FORMAT_PS)
	pctx.surface = cairo_ps_surface_create((ثابت حرف *)filename,
		pctx.صفحو_ويڪر, pctx.صفحو_اوچائي);
    وإلا
	pctx.surface = cairo_pdf_surface_create((ثابت حرف *)filename,
		pctx.صفحو_ويڪر, pctx.صفحو_اوچائي);

    pctx.filename = (char_u *)filename;

    إذا (cairo_surface_حالت(pctx.surface) != CAIRO_حالت_SUCCESS)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	أرجع FAIL;
    }

    pctx.cr = cairo_create(pctx.surface);
    pctx.متن_context = pango_cairo_create_context(pctx.cr);
    pctx.font = get_font(p_pfn);

    إذا (pctx.font == عدم)
    {
	mch_print_cleanup();
	أرجع FAIL;
    }

    pango_context_سيٽ_font_description(pctx.متن_context, pctx.font);
    // Pango DPI is 96, must adjust
    pango_cairo_context_سيٽ_resolution(pctx.متن_context, 72.0);

    // Get line height and char width
    {
	PangoLayout	    *layout;
	PangoFontMetrics    *metrics;
	صحيح		    ascent;
	صحيح		    descent;

	layout = pango_layout_new(pctx.متن_context);
	pango_layout_سيٽ_font_description(layout, pctx.font);

	metrics = pango_context_get_metrics(
		pango_layout_get_context(layout), pctx.font, عدم);

	pctx.char_ويڪر =
	    (مزدوج)pango_font_metrics_get_approximate_char_ويڪر(metrics)
	    / PANGO_SCALE;

	ascent = pango_font_metrics_get_ascent(metrics);
	descent = pango_font_metrics_get_descent(metrics);

	pctx.char_ascent = (مزدوج)ascent / PANGO_SCALE;
	pctx.char_اوچائي = (مزدوج)(ascent + descent) / PANGO_SCALE;

	pctx.سٽ_spacing = (مزدوج)
	    (pango_font_metrics_get_اوچائي(metrics) - (ascent + descent))
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
	مزدوج  کاٻو;
	مزدوج	ساڄو;
	مزدوج	چوٽي;
	مزدوج	bottom;

	get_صفحو_margins(pctx.صفحو_ويڪر, pctx.صفحو_اوچائي, &کاٻو, &ساڄو, &چوٽي,
		&bottom);
	pctx.کاٻو_margin = کاٻو;
	pctx.ساڄو_margin = ساڄو;
	pctx.چوٽي_margin = چوٽي;
	pctx.bottom_margin = bottom;
    }

    psettings->chars_في_سٽ = get_cpl();
    psettings->سٽون_في_صفحو = get_lpp();

    // Postscript handles collating pages, don't let Vim handle it. This means
    // we don't need to worry about the "jobsplit" value in 'printoptions'
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

#جيڪڏهن_وصف_ٿيل USE_GTK4_PRINT_DIALOG
    إذا (!dialog)
#جيڪڏهن_پورو
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

#جيڪڏهن_وصف_ٿيل USE_GTK4_PRINT_DIALOG
    pctx.dialog = dialog;
#جيڪڏهن_پورو

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
	اذهب ٻاهر_نڪرو;
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

ٻاهر_نڪرو:
    cairo_ps_surface_dsc_comment(pctx.surface, buf);
}

    صحيح
mch_print_begin(prt_settings_T *psettings)
{
    حرف user[256];

    pctx.cur_x = 0.0;
    pctx.cur_سٽ = 0;
    pctx.draw_flags = 0;

#جيڪڏهن_وصف_ٿيل USE_GTK4_PRINT_DIALOG
    إذا (pctx.dialog)
	أرجع TRUE;
#جيڪڏهن_پورو

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
		prt_mediasize[pctx.media].ويڪر,
		prt_mediasize[pctx.media].اوچائي);

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

	cairo_pdf_surface_سيٽ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_AUTHOR, (حرف *)str);
	إذا (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	str = psettings->jobname;
	إذا (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, عدم);
	cairo_pdf_surface_سيٽ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_TITLE, (حرف *)str);
	إذا (output_conv.vc_type != CONV_NONE)
	    vim_free(str);

	cairo_pdf_surface_سيٽ_metadata(pctx.surface,
		CAIRO_PDF_METADATA_CREATOR, VIM_نسخو_LONG);
    }

    أرجع TRUE;
}

    فراغ
mch_print_پڇاڙي(prt_settings_T *psettings)
{
    // Make sure surface flushes to file or stream
    cairo_surface_پڄاڻي(pctx.surface);

    إذا (psettings->outfile == عدم && !got_int && !psettings->user_abort
	    && pctx.filename != عدم
#جيڪڏهن_وصف_ٿيل USE_GTK4_PRINT_DIALOG
	    && !pctx.dialog
#جيڪڏهن_پورو
       )
    {
	نياپو(_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	إذا (eval_printexpr(pctx.filename, psettings->arguments) == FAIL)
	    emsg(_(e_failed_منزل_print_postscript_file));
	وإلا
	    نياپو(_("Print job sent."));
    }

    mch_print_cleanup();
}

    صحيح
mch_print_پڇاڙي_صفحو(فراغ)
{
    cairo_show_صفحو(pctx.cr);
    أرجع TRUE;
}

/*
 * Do nothing because we create a new page for the cairo surface at the end of
 * every page.
 */
    صحيح
mch_print_begin_صفحو(char_u *str اڻ_استعمال_ٿيل)
{
    أرجع TRUE;
}

    صحيح
mch_print_blank_صفحو(فراغ)
{
    mch_print_پڇاڙي_صفحو();
    أرجع TRUE;
}

    فراغ
mch_print_شروعات_سٽ(صحيح margin, صحيح صفحو_سٽ)
{
    pctx.cur_x = pctx.کاٻو_margin;
    إذا (margin)
	pctx.cur_x -= pctx.عدد_ويڪر;

    pctx.cur_سٽ = صفحو_سٽ;
}

    ساكن فراغ
long_منزل_rgb(long_u c, مزدوج *r, مزدوج *g, مزدوج *b)
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
mch_print_متن_ٻاهريون(char_u *textp, صحيح len)
{
    PangoAttrList	*attr_list;
    GList		*شي_list;
    PangoGlyphString	*glyphs;
    char_u		*str;
    مزدوج		r, g, b;
    مزدوج		ويڪر = 0;
    مزدوج		ايندڙ_x;

    // Always convert to UTF-8, Pango expects that
    إذا (output_conv.vc_type == CONV_NONE)
	str = textp;
    وإلا
	str = string_convert(&output_conv, textp, &len);

    إذا (str == عدم)
	أرجع TRUE;

    attr_list = pango_attr_list_new();

#وصف_ڪريو وجهو_PANGO_ATTR(Attribute, AttrList, شروعات_2, پڇاڙي_2)  \
    G_STMT_شروعات{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->شروعات_اشاريو = (شروعات_2);		    \
	tmp_attr_->پڇاڙي_اشاريو = (پڇاڙي_2);			    \
	pango_attr_list_وجهو((AttrList), tmp_attr_);	    \
    }G_STMT_پڇاڙي

    // Underline attribute does not work with pango_itemize().
    إذا ((pctx.draw_flags & PRT_DRAW_BOLD))
	وجهو_PANGO_ATTR(pango_attr_وزن_new(PANGO_وزن_BOLD),
		attr_list, 0, len);
    إذا (pctx.draw_flags & PRT_DRAW_ITALIC)
	وجهو_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

#وصف_رد وجهو_PANGO_ATTR

    شي_list = pango_itemize(pctx.متن_context,
	    (ثابت حرف *)str, 0, len, attr_list, عدم);
    pango_attr_list_unref(attr_list);

    glyphs = pango_glyph_string_new();

    // Loop is probably unnecessary, because "str" represents a single character.
    // Do it anyways to be sure.
    طالما (شي_list != عدم)
    {
	Pango_شي   *شي;
	مزدوج	    o;

	شي = (Pango_شي *)شي_list->معلومات;
	شي_list = g_list_delete_link(شي_list, شي_list);

	pango_shape_ڀريل((ثابت حرف *)str + شي->وٿي, شي->ڊيگهائي,
		(ثابت حرف *)str, len, &شي->analysis, glyphs);

	// Benefit of directly getting the width instead of deriving it from the
	// char width and number of cells, is that non-monospace fonts look
	// better.
	o = (مزدوج)pango_glyph_string_get_ويڪر(glyphs) / PANGO_SCALE;

	// Draw background color (only if different). Cannot batch bg color,
	// because then we would overwrite glyphs.
	إذا (pctx.cur_bg != PRCOLOR_WHITE)
	{
	    long_منزل_rgb(pctx.cur_bg, &r, &g, &b);
	    cairo_سيٽ_ذريعو_rgb(pctx.cr, r, g, b);
	    cairo_rectangle(pctx.cr, pctx.cur_x + ويڪر,
		    GET_Y(pctx.cur_سٽ) - pctx.سٽ_spacing, o, pctx.char_اوچائي);
	    cairo_fill(pctx.cr);
	}

	// Draw actual text
	long_منزل_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_سيٽ_ذريعو_rgb(pctx.cr, r, g, b);
	cairo_move_منزل(pctx.cr, pctx.cur_x + ويڪر, GET_متن_Y(pctx.cur_سٽ));
	pango_cairo_show_glyph_string(pctx.cr, شي->analysis.font, glyphs);

	pango_شي_free(شي);
	ويڪر += o;
    }

    // Draw underline if needed
    إذا (pctx.draw_flags & PRT_DRAW_UNDERLINE)
    {
	مزدوج y = GET_متن_Y(pctx.cur_سٽ) - pctx.ul_pos;

	long_منزل_rgb(pctx.cur_fg, &r, &g, &b);
	cairo_سيٽ_ذريعو_rgb(pctx.cr, r, g, b);
	cairo_سيٽ_سٽ_ويڪر(pctx.cr, pctx.ul_thickness);
	cairo_move_منزل(pctx.cr, pctx.cur_x, y);
	cairo_سٽ_منزل(pctx.cr, pctx.cur_x + ويڪر, y);
	cairo_stroke(pctx.cr);
    }

    pango_glyph_string_free(glyphs);

    pctx.cur_x += ويڪر;

    ايندڙ_x = pctx.cur_x + pctx.char_ويڪر;

    إذا (output_conv.vc_type != CONV_NONE)
	vim_free(str);

    // Use epsilon when comparing floating point
    أرجع (ايندڙ_x > pctx.ساڄو_margin) &&
	((ايندڙ_x - pctx.ساڄو_margin) > (pctx.ساڄو_margin * 1e-5));
}

    فراغ
mch_print_سيٽ_font(صحيح bold, صحيح italic, صحيح underline)
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
mch_print_سيٽ_bg(long_u bgcol)
{
    pctx.cur_bg = bgcol;
}

    فراغ
mch_print_سيٽ_fg(long_u fgcol)
{
    // Probably can just call cairo_set_source_rgba() here but defer it later to
    // be consistent with bg color.
    pctx.cur_fg = fgcol;
}

    فراغ
mch_print_cleanup(فراغ)
{
    g_clear_اشارو(&pctx.cr, cairo_destroy);
    g_clear_اشارو(&pctx.surface, cairo_surface_destroy);
    g_clear_object(&pctx.متن_context);
    g_clear_اشارو(&pctx.font, pango_font_description_free);

#جيڪڏهن_وصف_ٿيل USE_GTK4_PRINT_DIALOG
    // If using dialog, defer the temp file deletion later
    إذا (pctx.dialog)
	gui_gtk4_print_پڄاڻي();
#جيڪڏهن_پورو

    g_clear_اشارو(&pctx.filename, vim_free);
}

#جيڪڏهن_پورو // FEAT_PRINT_PANGO
