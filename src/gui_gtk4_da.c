/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#అన్తర్భావ "vim.h"

#అన్తర్భావ <gtk/gtk.h>
#అన్తర్భావ "gui_gtk4_da.h"


#ఒకవేళ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Used for sign icons and netbeans multisign indicator. It is in a DrawGlyphs
 * so that it can move/be manipulated with the text.
 */
రకనిర్వచనం నిర్మాణం
{
    GdkTexture	*texture;
    పూర్ణాంకం		వెడల్పు;
    పూర్ణాంకం		ఎత్తు;
} DrawSign;
#అయితే_ముగింపు

/*
 * Temporary struct when drawing string representing visual attributes of a cell
 */
రకనిర్వచనం నిర్మాణం
{
    uint32_t	bg_color;
    uint32_t	fg_color;   // Only used for underlines
    uint32_t	sp_color;
    పూర్ణాంకం		draw_flags; // DRAW_* flags, only DRAW_UNDERL, DRAW_UNDERC, and
			    // DRAW_STRIKE are used.
} DrawAttr;

/*
 * Represents the glyphs of a single cell.
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం refcount;

    uint32_t	fg_color;
    PangoFont	*font;
    // If "font" is NULL, "dsign" is used, otherwise "glyphs" is used.
    సంఘం
    {
	PangoGlyphInfo  *glyphs; // May be NULL
#ఒకవేళ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	DrawSign	*dsign;
#అయితే_ముగింపు
    };
    పూర్ణాంకం n_glyphs;
} DrawGlyphs;

రకనిర్వచనం నిర్మాణం
{
    DrawGlyphs *dglyphs;
} DrawCell;

రకనిర్వచనం గణన
{
    DRAW_LAYER_BACKGROUND,  // Background color, buf stores bg color
    DRAW_LAYER_పాఠ్యం,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

రకనిర్వచనం నిర్మాణం
{
    Gsk_Render_కణుపు   *కణుపు;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#నిర్వచించు UNDERC_స్థానభ్రంశం(n_నిలువుల_సంఖ్య) (0)
#నిర్వచించు STRIKE_స్థానభ్రంశం(n_నిలువుల_సంఖ్య) (n_నిలువుల_సంఖ్య)

రకనిర్వచనం నిర్మాణం
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    పూర్ణాంకం		n_cells; // For convenience
    పూర్ణాంకం		అడ్డువరుస;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_కణుపు *underl_ముసుగు;
    Gsk_Render_కణుపు *underc_ముసుగు;
    Gsk_Render_కణుపు *strike_ముసుగు;
} Draw_అడ్డువరుస;

#నిర్వచించు చొప్పించు_PANGO_ATTR(Attribute, AttrList, ప్రారంభం_2, చివర_2) \
    చేయి { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->ప్రారంభం_సూచీ = (ప్రారంభం_2); \
	tmp_attr_->చివర_సూచీ = (చివర_2); \
	pango_attr_list_చొప్పించు((AttrList), tmp_attr_); \
    } అయితే (FALSE)
#నిర్వచించు CELLS2PANGO(c) ((c) * gui.char_వెడల్పు * PANGO_SCALE)

/*
 * Attributes of cursor
 */
రకనిర్వచనం నిర్మాణం
{
    gboolean draw; // If cursor should be drawn
    పూర్ణాంకం వెడల్పు;
    పూర్ణాంకం ఎత్తు;
    పూర్ణాంకం n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

నిర్మాణం _VimDrawArea
{
    GtkWidget మాతృ;

    Draw_అడ్డువరుస *అడ్డువరుసలు_2;

    పూర్ణాంకం n_అడ్డువరుసలు;
    పూర్ణాంకం	n_నిలువుల_సంఖ్య;

    పూర్ణాంకం	bleed_కుడి;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*కణుపు_buf;

    DrawCursor cursor;

#నిర్వచితమైతే FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHash_పట్టిక *external;
#అయితే_ముగింపు
};

స్థిర శూన్య draw_అడ్డువరుస_init(Draw_అడ్డువరుస *drow, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువుల_సంఖ్య);
స్థిర శూన్య draw_అడ్డువరుస_clear(Draw_అడ్డువరుస *drow);
స్థిర శూన్య draw_అడ్డువరుస_dirty_layer(Draw_అడ్డువరుస *drow, DrawLayerType dlayer_t);
స్థిర శూన్య vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

స్థిర GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    స్థిర శూన్య
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    కొరకు (పూర్ణాంకం r = 0; r < self->n_అడ్డువరుసలు; r++)
	draw_అడ్డువరుస_clear(self->అడ్డువరుసలు_2 + r);
    g_free(self->అడ్డువరుసలు_2);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->కణుపు_buf, TRUE);

#నిర్వచితమైతే FEAT_IMAGE_GUI
    g_hash_పట్టిక_unref(self->external);
#అయితే_ముగింపు

    G_OBJECT_CLASS(vim_draw_area_మాతృ_class)->finalize(obj);
}

    స్థిర శూన్య
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_సమితి_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    స్థిర శూన్య
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_కుడి = -1;
#నిర్వచితమైతే FEAT_IMAGE_GUI
    self->external = g_hash_పట్టిక_new_నిండిన(g_ప్రత్యక్ష_hash, g_ప్రత్యక్ష_equal,
	    ఖాళీ, (GDestroyNotify)gsk_render_కణుపు_unref);
#అయితే_ముగింపు
    self->glyph_buf = g_array_new(FALSE, FALSE, పరిమాణం(PangoGlyphInfo));
    self->కణుపు_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_కణుపు_unref);

}

    GtkWidget *
vim_draw_area_new(శూన్య)
{
    తిరిగి g_object_new(VIM_TYPE_DRAW_AREA, ఖాళీ);
}

/*
 * Set the size of the draw area to "rows" and "cols".
 */
    శూన్య
vim_draw_area_సమితి_కొలత(VimDrawArea *self, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య)
{
    ఒకవేళ (self->అడ్డువరుసలు_2 != ఖాళీ && self->n_అడ్డువరుసలు == అడ్డువరుసలు_2 && self->n_నిలువుల_సంఖ్య == నిలువుల_సంఖ్య)
	తిరిగి;
    ఒకవేళ (అడ్డువరుసలు_2 == 0 || నిలువుల_సంఖ్య == 0)
	తిరిగి;

    vim_draw_area_clear(self);

    కొరకు (పూర్ణాంకం r = 0; r < self->n_అడ్డువరుసలు; r++)
	draw_అడ్డువరుస_clear(self->అడ్డువరుసలు_2 + r);
    self->అడ్డువరుసలు_2 = g_realloc_n(self->అడ్డువరుసలు_2, అడ్డువరుసలు_2, పరిమాణం(Draw_అడ్డువరుస));
    కొరకు (పూర్ణాంకం r = 0; r < అడ్డువరుసలు_2; r++)
	draw_అడ్డువరుస_init(self->అడ్డువరుసలు_2 + r, r, నిలువుల_సంఖ్య);

    self->n_అడ్డువరుసలు = అడ్డువరుసలు_2;
    self->n_నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    స్థిర uint32_t
rgba_గమ్యం_u32(స్థిరరాశి GdkRGBA *rgba)
{
    సంఘం { uint32_t val; uint8_t బైట్లు[4]; } color;

    color.బైట్లు[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.బైట్లు[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.బైట్లు[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.బైట్లు[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    తిరిగి color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    స్థిర GdkRGBA
u32_గమ్యం_rgba(uint32_t val)
{
    సంఘం { uint32_t val; uint8_t బైట్లు[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.బైట్లు[0] / 255.0;
    color.green = packed.బైట్లు[1] / 255.0;
    color.blue  = packed.బైట్లు[2] / 255.0;
    color.alpha = packed.బైట్లు[3] / 255.0;
    తిరిగి color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    స్థిర gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    సంఘం { uint32_t val; uint8_t బైట్లు[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.బైట్లు[0] << 16) |
	((guicolor_T)packed.బైట్లు[1] << 8)  |
	packed.బైట్లు[2];
    తిరిగి res == rgb;
}

/*
 * Checks if a RGBA pixel buffer is fully transparent.
 */
    స్థిర అంతఃవరుస gboolean
rgba_is_transparent(స్థిరరాశి uint32_t *buf, పూర్ణాంకం n_pixels)
{
    // Could also compare 2 pixels at once using a 64 bit integer, but not sure
    // if that might cause alignment issues...
    కొరకు (పూర్ణాంకం i = 0; i < n_pixels; i++)
	ఒకవేళ (buf[i] != 0)
	    తిరిగి FALSE;
    తిరిగి TRUE;
}

/*
 * Return TRUE if "glyph" has ink (not whitespace).
 */
    స్థిర gboolean
glyph_has_ink(PangoFont *font, PangoGlyphInfo glyph)
{
    PangoRectangle ink;

    pango_font_get_glyph_extents (font, glyph.glyph, &ink, ఖాళీ);

    తిరిగి ink.వెడల్పు > 0 && ink.ఎత్తు > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    స్థిర శూన్య
apply_wide_font_attr(char_u *s, పూర్ణాంకం len, PangoAttrList *attr_list)
{
    char_u  *ప్రారంభం = ఖాళీ;
    char_u  *p;
    పూర్ణాంకం	    uc;

    కొరకు (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	ఒకవేళ (ప్రారంభం == ఖాళీ)
	{
	    ఒకవేళ (uc >= 0x80 && utf_char2cells(uc) == 2)
		ప్రారంభం = p;
	}
	లేకపోతే ఒకవేళ (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    చొప్పించు_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, ప్రారంభం - s, p - s);
	    ప్రారంభం = ఖాళీ;
	}
    }

    ఒకవేళ (ప్రారంభం != ఖాళీ)
	చొప్పించు_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, ప్రారంభం - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    స్థిర పూర్ణాంకం
లెక్క_cluster_cells(
	char_u		    *s,
	Pango_అంశం	    *అంశం,
	PangoGlyphString    *glyphs,
	పూర్ణాంకం		    i,
	పూర్ణాంకం		    *cluster_వెడల్పు,
	పూర్ణాంకం		    *చివరి_glyph_rbearing)
{
    char_u  *p;
    పూర్ణాంకం	    తదుపరి;
    పూర్ణాంకం	    ప్రారంభం, చివర;
    పూర్ణాంకం	    వెడల్పు;
    పూర్ణాంకం	    uc;
    పూర్ణాంకం	    cellcount = 0;

    వెడల్పు = glyphs->glyphs[i].geometry.వెడల్పు;

    కొరకు (తదుపరి = i + 1; తదుపరి < glyphs->అంకె_glyphs; ++తదుపరి)
    {
	ఒకవేళ (glyphs->glyphs[తదుపరి].attr.is_cluster_ప్రారంభం)
	    ఆపు;
	లేకపోతే ఒకవేళ (glyphs->glyphs[తదుపరి].geometry.వెడల్పు > వెడల్పు)
	    వెడల్పు = glyphs->glyphs[తదుపరి].geometry.వెడల్పు;
    }

    ప్రారంభం = అంశం->స్థానభ్రంశం + glyphs->log_clusters[i];
    చివర   = అంశం->స్థానభ్రంశం + ((తదుపరి < glyphs->అంకె_glyphs) ?
	    glyphs->log_clusters[తదుపరి] : అంశం->నిడివి);

    కొరకు (p = s + ప్రారంభం; p < s + చివర; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	ఒకవేళ (uc < 0x80)
	    ++cellcount;
	లేకపోతే ఒకవేళ (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    ఒకవేళ (చివరి_glyph_rbearing != ఖాళీ
	    && cellcount > 0 && తదుపరి == glyphs->అంకె_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(అంశం->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, ఖాళీ);

	ఒకవేళ (PANGO_RBEARING(ink_rect) > 0)
	    *చివరి_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    ఒకవేళ (cellcount > 0)
	*cluster_వెడల్పు = వెడల్పు;

    తిరిగి cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    స్థిర శూన్య
అమరిక_zero_వెడల్పు_cluster(
	Pango_అంశం	*అంశం,
	PangoGlyphInfo	*glyph,
	పూర్ణాంకం		చివరి_cellcount,
	పూర్ణాంకం		చివరి_cluster_వెడల్పు,
	పూర్ణాంకం		చివరి_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    పూర్ణాంకం		    వెడల్పు;

    వెడల్పు = చివరి_cellcount * gui.char_వెడల్పు * PANGO_SCALE;
    glyph->geometry.x_స్థానభ్రంశం = -వెడల్పు + MAX(0, వెడల్పు - చివరి_cluster_వెడల్పు) / 2;
    glyph->geometry.వెడల్పు = 0;

    pango_font_get_glyph_extents(అంశం->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    ఒకవేళ (ink_rect.x < 0)
    {
	glyph->geometry.x_స్థానభ్రంశం += చివరి_glyph_rbearing;
	glyph->geometry.y_స్థానభ్రంశం  = logical_rect.ఎత్తు
	    - (gui.char_ఎత్తు - p_linespace) * PANGO_SCALE;
    }
    లేకపోతే
	glyph->geometry.x_స్థానభ్రంశం = -వెడల్పు + MAX(0, వెడల్పు - ink_rect.వెడల్పు) / 2;
}

#ఒకవేళ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    స్థిర DrawSign *
draw_sign_new(GdkTexture *texture, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->వెడల్పు = వెడల్పు;
    dsign->ఎత్తు = ఎత్తు;

    తిరిగి dsign;
}

    స్థిర శూన్య
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#అయితే_ముగింపు

    స్థిర శూన్య
draw_attr_init(
	DrawAttr	*dattr,
	స్థిరరాశి GdkRGBA	*bg_color,
	స్థిరరాశి GdkRGBA	*fg_color,
	స్థిరరాశి GdkRGBA	*sp_color,
	పూర్ణాంకం		draw_flags)
{
    dattr->bg_color = rgba_గమ్యం_u32(bg_color);
    dattr->fg_color = rgba_గమ్యం_u32(fg_color);
    dattr->sp_color = rgba_గమ్యం_u32(sp_color);
    dattr->draw_flags = draw_flags & (DRAW_UNDERL | DRAW_UNDERC | DRAW_STRIKE);
}

/*
 * Apply the draw attr to the row for the given col/cell. Only dirty layers that
 * have actually changed.
 */
    స్థిర శూన్య
draw_attr_apply(DrawAttr *dattr, పూర్ణాంకం నిలువు, Draw_అడ్డువరుస *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + నిలువు;
    uint32_t	*పాఠ్యం_cell = drow->dlayers[DRAW_LAYER_పాఠ్యం].buf + నిలువు;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    పూర్ణాంకం		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	నిలువు + UNDERC_స్థానభ్రంశం(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	నిలువు + STRIKE_స్థానభ్రంశం(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    ఒకవేళ (dattr != ఖాళీ)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.వెనుక_pixel, bg_color);
    }

    ఒకవేళ (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_అడ్డువరుస_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	ఒకవేళ (bg_is_default)
	    *bg_cell = 0;
	లేకపోతే
	    *bg_cell = bg_color;
    }
    ఒకవేళ ((draw_flags & DRAW_UNDERL && *పాఠ్యం_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *పాఠ్యం_cell != 0))
    {
	draw_అడ్డువరుస_dirty_layer(drow, DRAW_LAYER_పాఠ్యం);
	ఒకవేళ (draw_flags & DRAW_UNDERL)
	    *పాఠ్యం_cell = fg_color;
	లేకపోతే
	    *పాఠ్యం_cell = 0;
    }
    ఒకవేళ ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_అడ్డువరుస_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	ఒకవేళ (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	లేకపోతే
	    *underc_cell = 0;
    }
    ఒకవేళ ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_అడ్డువరుస_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	ఒకవేళ (draw_flags & DRAW_STRIKE)
	    *strike_cell = sp_color;
	లేకపోతే
	    *strike_cell = 0;
    }
}

    స్థిర DrawGlyphs *
draw_glyphs_new(
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	పూర్ణాంకం		n_glyphs,
	uint32_t	fg_color)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->font = g_object_ref(font);
    dglyphs->glyphs = g_memdup2(glyphs, పరిమాణం(*glyphs) * n_glyphs);
    dglyphs->n_glyphs = n_glyphs;
    dglyphs->fg_color = fg_color;

    తిరిగి dglyphs;
}

#ఒకవేళ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw glyphs that shows the given sign. Note that ownership of
 * "dsign" is taken.
 */
    స్థిర DrawGlyphs *
draw_glyphs_new_sign(DrawSign *dsign)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->dsign = dsign;

    తిరిగి dglyphs;
}
#అయితే_ముగింపు

    స్థిర DrawGlyphs *
draw_glyphs_ref(DrawGlyphs *dglyphs)
{
    dglyphs->refcount++;
    తిరిగి dglyphs;
}

    స్థిర శూన్య
draw_glyphs_unref(DrawGlyphs *dglyphs)
{
    ఒకవేళ (--dglyphs->refcount <= 0)
    {
#ఒకవేళ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	ఒకవేళ (dglyphs->font == ఖాళీ)
	    draw_sign_free(dglyphs->dsign);
	లేకపోతే
#అయితే_ముగింపు
	{
	    g_free(dglyphs->glyphs);
	    g_object_unref(dglyphs->font);
	}
	g_free(dglyphs);
    }
}

/*
 * Return TRUE if "a" and "b" are same. If either are NULL, return FALSE, if
 * both are NULL, return TRUE.
 */
    స్థిర gboolean
draw_glyphs_same(DrawGlyphs *a, DrawGlyphs *b)
{
    ఒకవేళ (a == ఖాళీ && b == ఖాళీ)
	తిరిగి TRUE;
    ఒకవేళ (a == ఖాళీ || b == ఖాళీ)
	తిరిగి FALSE;
    ఒకవేళ (a->fg_color != b->fg_color)
	తిరిగి FALSE;
    ఒకవేళ (a->n_glyphs != b->n_glyphs || a->font != b->font)
	తిరిగి FALSE;
    కొరకు (పూర్ణాంకం i = 0; i < a->n_glyphs; i++)
	ఒకవేళ (a->glyphs[i].glyph != b->glyphs[i].glyph
		|| a->glyphs[i].geometry.వెడల్పు != b->glyphs[i].geometry.వెడల్పు
		|| a->glyphs[i].geometry.x_స్థానభ్రంశం != b->glyphs[i].geometry.x_స్థానభ్రంశం
		|| a->glyphs[i].geometry.y_స్థానభ్రంశం != b->glyphs[i].geometry.y_స్థానభ్రంశం
		|| a->glyphs[i].attr.is_cluster_ప్రారంభం != b->glyphs[i].attr.is_cluster_ప్రారంభం)
	    తిరిగి FALSE;
    తిరిగి TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    స్థిర Gsk_Render_కణుపు *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	పూర్ణాంకం	    అడ్డువరుస,
	VimDrawArea *da,
	పూర్ణాంకం	    off,
	gboolean    bleed)
{
    GBytes	    *బైట్లు;
    GdkTexture	    *texture;
    Gsk_Render_కణుపు   *కణుపు;
    పూర్ణాంకం		    ఎత్తు;

#ఒకవేళ GTK_CHECK_సంస్కరణ(4,24,0)
    ఎత్తు = gui.char_ఎత్తు;
#లేకపోతే
    ఎత్తు = gui.char_ఎత్తు + 1;
#అయితే_ముగింపు

    ఒకవేళ (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		పరిమాణం(uint32_t) * da->n_నిలువుల_సంఖ్య);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_నిలువుల_సంఖ్య - 1)),
		పరిమాణం(uint32_t)); // Add bleed using last pixel of buffer
	బైట్లు = g_byte_array_free_గమ్యం_బైట్లు(arr);
    }
    లేకపోతే
	బైట్లు = g_బైట్లు_new(dlayer->buf + off, పరిమాణం(uint32_t) * da->n_నిలువుల_సంఖ్య);

    texture = gdk_జ్ఞాపకం_texture_new(da->n_నిలువుల_సంఖ్య + bleed, 1,
	    GDK_జ్ఞాపకం_R8_G8_B8_A8, బైట్లు, (da->n_నిలువుల_సంఖ్య + bleed) * 4);
    g_బైట్లు_unref(బైట్లు);

    // Scale texture to actual size
    కణుపు = gsk_texture_scale_కణుపు_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(అడ్డువరుస),
		(da->n_నిలువుల_సంఖ్య + bleed) * gui.char_వెడల్పు, gui.char_ఎత్తు + bleed),
	    GSK_SCALING_FILTER_సమీప);
    ఒకవేళ (bleed)
    {
	Gsk_Render_కణుపు *new;

	new = gsk_clip_కణుపు_new(కణుపు,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(అడ్డువరుస),
		    da->n_నిలువుల_సంఖ్య * gui.char_వెడల్పు + da->bleed_కుడి, ఎత్తు));
	gsk_render_కణుపు_unref(కణుపు);
	కణుపు = new;
    }
    g_object_unref(texture);
    తిరిగి కణుపు;
}

    స్థిర శూన్య
draw_అడ్డువరుస_init(Draw_అడ్డువరుస *drow, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువుల_సంఖ్య)
{
    drow->cells = g_malloc0_n(నిలువుల_సంఖ్య, పరిమాణం(DrawCell));
    drow->n_cells = నిలువుల_సంఖ్య;
    drow->అడ్డువరుస = అడ్డువరుస;
    drow->underl_ముసుగు = drow->underc_ముసుగు = drow->strike_ముసుగు = ఖాళీ;

    memset(drow->dlayers, 0, పరిమాణం(DrawLayer) * N_DRAW_LAYERS);
    కొరకు (పూర్ణాంకం l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	ఒకవేళ (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(నిలువుల_సంఖ్య * 2, 4);
	లేకపోతే
	    dlayer->buf = g_malloc0_n(నిలువుల_సంఖ్య, 4);
    }
}

    స్థిర శూన్య
draw_అడ్డువరుస_clear(Draw_అడ్డువరుస *drow)
{
    g_free(drow->cells);
    కొరకు (పూర్ణాంకం l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	ఒకవేళ (dlayer->కణుపు != ఖాళీ)
	    gsk_render_కణుపు_unref(dlayer->కణుపు);
	g_free(dlayer->buf);
    }
    g_clear_సూచిక(&drow->underl_ముసుగు, gsk_render_కణుపు_unref);
    g_clear_సూచిక(&drow->underc_ముసుగు, gsk_render_కణుపు_unref);
    g_clear_సూచిక(&drow->strike_ముసుగు, gsk_render_కణుపు_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    స్థిర శూన్య
draw_అడ్డువరుస_dirty_layer(Draw_అడ్డువరుస *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_సూచిక(&dlayer->కణుపు, gsk_render_కణుపు_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    స్థిర శూన్య
draw_అడ్డువరుస_uninvert_cell(Draw_అడ్డువరుస *drow, పూర్ణాంకం నిలువు)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    ఒకవేళ (buf[నిలువు] == 0xffffffff)
    {
	draw_అడ్డువరుస_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[నిలువు] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    స్థిర శూన్య
draw_అడ్డువరుస_సమితి_cell(Draw_అడ్డువరుస *drow, పూర్ణాంకం నిలువు, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + నిలువు;

    ఒకవేళ (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_అడ్డువరుస_dirty_layer(drow, DRAW_LAYER_పాఠ్యం);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    ఒకవేళ (dcell->dglyphs != dglyphs)
    {
	ఒకవేళ (dcell->dglyphs != ఖాళీ)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == ఖాళీ ? ఖాళీ : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, నిలువు, drow);
    draw_అడ్డువరుస_uninvert_cell(drow, నిలువు);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    స్థిర శూన్య
draw_అడ్డువరుస_fill(
	Draw_అడ్డువరుస	    *drow,
	పూర్ణాంకం	    col1,
	పూర్ణాంకం	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    కొరకు (పూర్ణాంకం c = col1; c <= col2; c++)
	draw_అడ్డువరుస_సమితి_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    ఒకవేళ (gui.cursor_అడ్డువరుస == drow->అడ్డువరుస && gui.cursor_నిలువు >= col1
	    && gui.cursor_నిలువు <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    స్థిర శూన్య
draw_అడ్డువరుస_move_గమ్యం(
	Draw_అడ్డువరుస	    *dest_అడ్డువరుస,
	Draw_అడ్డువరుస	    *src_అడ్డువరుస,
	పూర్ణాంకం	    col1,
	పూర్ణాంకం	    col2,
	VimDrawArea *da)
{
    పూర్ణాంకం move_కొలత = (col2 - col1 + 1) * పరిమాణం(DrawCell);

    draw_అడ్డువరుస_fill(dest_అడ్డువరుస, col1, col2, ఖాళీ, ఖాళీ, &da->cursor);
    memmove(dest_అడ్డువరుస->cells + col1, src_అడ్డువరుస->cells + col1, move_కొలత);
    // NULL the draw cells so we don't double unreference.
    memset(src_అడ్డువరుస->cells + col1, 0, move_కొలత);
    draw_అడ్డువరుస_dirty_layer(src_అడ్డువరుస, DRAW_LAYER_పాఠ్యం);

    // Move the layer buffers
    move_కొలత = (col2 - col1 + 1) * 4;
    కొరకు (పూర్ణాంకం l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_అడ్డువరుస->dlayers[l].buf;
	uint32_t *dest = dest_అడ్డువరుస->dlayers[l].buf;

	ఒకవేళ (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_స్థానభ్రంశం(dest_అడ్డువరుస->n_cells),
		    src + col1 + UNDERC_స్థానభ్రంశం(src_అడ్డువరుస->n_cells), move_కొలత);
	    memmove(dest + col1 + STRIKE_స్థానభ్రంశం(dest_అడ్డువరుస->n_cells),
		    src + col1 + STRIKE_స్థానభ్రంశం(src_అడ్డువరుస->n_cells), move_కొలత);
	}
	లేకపోతే
	    memmove(dest + col1, src + col1, move_కొలత);
	draw_అడ్డువరుస_dirty_layer(dest_అడ్డువరుస, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    స్థిర శూన్య
draw_అడ్డువరుస_render_background(Draw_అడ్డువరుస *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    ఒకవేళ (!dlayer->dirty)
	తిరిగి;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    ఒకవేళ (rgba_is_transparent(dlayer->buf, da->n_నిలువుల_సంఖ్య))
	తిరిగి;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->కణుపు = draw_layer_get_texture(dlayer, drow->అడ్డువరుస, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    స్థిర శూన్య
draw_అడ్డువరుస_ensure_decor(Draw_అడ్డువరుస *drow, పూర్ణాంకం flags)
{
    ఒకవేళ (flags & DRAW_UNDERL && drow->underl_ముసుగు == ఖాళీ)
	drow->underl_ముసుగు = gsk_color_కణుపు_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->అడ్డువరుస + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    ఒకవేళ (flags & DRAW_STRIKE && drow->strike_ముసుగు == ఖాళీ)
	drow->strike_ముసుగు = gsk_color_కణుపు_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->అడ్డువరుస) + (పూర్ణాంకం)(gui.char_ఎత్తు / 2),
		    FILL_X(drow->n_cells), 1));

    ఒకవేళ (flags & DRAW_UNDERC && drow->underc_ముసుగు == ఖాళీ)
    {
	స్థిర స్థిరరాశి పూర్ణాంకం val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	పూర్ణాంకం y = FILL_Y(drow->అడ్డువరుస + 1) - 1;
	పూర్ణాంకం x_ప్రారంభం = FILL_X(0);
	పూర్ణాంకం x_చివర = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_కణుపు *బిడ్డ = ఖాళీ;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#ఒకవేళ GTK_CHECK_సంస్కరణ(4, 14, 0)
	Gsk_మార్గం_Builder	*builder;
	Gsk_మార్గం		*మార్గం;
	GskStroke       *stroke;
	Gsk_Render_కణుపు	*color_కణుపు;
	graphene_rect_t bounds;

	builder = gsk_మార్గం_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_మార్గం_builder_move_గమ్యం(builder, -1, y - val[7] + 0.5);
	gsk_మార్గం_builder_పంక్తి_గమ్యం(builder, 0, y - val[0] + 0.5);

	కొరకు (పూర్ణాంకం i = 1; i < 8; i++)
	    gsk_మార్గం_builder_పంక్తి_గమ్యం(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_మార్గం_builder_పంక్తి_గమ్యం(builder, 8, y - val[0] + 0.5);
	gsk_మార్గం_builder_పంక్తి_గమ్యం(builder, 9, y - val[1] + 0.5);

	మార్గం = gsk_మార్గం_builder_free_గమ్యం_మార్గం(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_మార్గం_get_stroke_bounds(మార్గం, stroke, &bounds);
	color_కణుపు = gsk_color_కణుపు_new(&white_rgba, &bounds);
	బిడ్డ = gsk_stroke_కణుపు_new(color_కణుపు, మార్గం, stroke);

	gsk_stroke_free(stroke);
	gsk_మార్గం_unref(మార్గం);
	gsk_render_కణుపు_unref(color_కణుపు);
#లేకపోతే
	cairo_t *cr;

	బిడ్డ = gsk_cairo_కణుపు_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_కణుపు_get_draw_context(బిడ్డ);

	cairo_సమితి_పంక్తి_వెడల్పు(cr, 1.0);
	cairo_సమితి_ఆధారం_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_గమ్యం(cr, -1, y - val[7] + 0.5);
	cairo_పంక్తి_గమ్యం(cr, 0, y - val[0] + 0.5);

	కొరకు (పూర్ణాంకం i = 1; i < 8; ++i)
	    cairo_పంక్తి_గమ్యం(cr, i, y - val[i] + 0.5);

	cairo_పంక్తి_గమ్యం(cr, 8, y - val[0] + 0.5);
	cairo_పంక్తి_గమ్యం(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#అయితే_ముగింపు
	drow->underc_ముసుగు = gsk_repeat_కణుపు_new(
		&GRAPHENE_RECT_INIT(x_ప్రారంభం, y - 3, x_చివర - x_ప్రారంభం, 5),
		బిడ్డ, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_కణుపు_unref(బిడ్డ);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    స్థిర శూన్య
draw_అడ్డువరుస_render_పాఠ్యం(Draw_అడ్డువరుస *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_పాఠ్యం;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = ఖాళీ;
    పూర్ణాంకం		ప్రారంభం_నిలువు = 0;
    పూర్ణాంకం		ఖాళీలు_cells = 0;
    uint32_t	cur_fg = 0;
    పూర్ణాంకం		c;

    ఒకవేళ (!dlayer->dirty)
	తిరిగి;

    glyph_buf = da->glyph_buf;
    nodes = da->కణుపు_buf;

    g_array_సమితి_కొలత(glyph_buf, 0);
    g_ptr_array_సమితి_కొలత(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#నిర్వచించు FLUSH_కణుపు() \
    చేయి { \
	ఒకవేళ (cur_font != ఖాళీ) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_కణుపు	*కణుపు; \
	    GdkRGBA		fg_color = u32_గమ్యం_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->దత్తాంశం; \
	    glyph_str.అంకె_glyphs = glyph_buf->len; \
	    కణుపు = gsk_పాఠ్యం_కణుపు_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_బిందువు_INIT(పాఠ్యం_X(ప్రారంభం_నిలువు), \
			పాఠ్యం_Y(drow->అడ్డువరుస))); \
	    ఒకవేళ (కణుపు != ఖాళీ) \
		g_ptr_array_కూడు(nodes, కణుపు); \
	    g_array_సమితి_కొలత(glyph_buf, 0); \
	    cur_font = ఖాళీ; \
	} \
    } అయితే (FALSE)

    కొరకు (c = 0; c < da->n_నిలువుల_సంఖ్య; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	పూర్ణాంకం	    prev_len;

	ఒకవేళ (dglyphs == ఖాళీ)
	{
	    ఖాళీలు_cells++;
	    కొనసాగు;
	}
#ఒకవేళ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	లేకపోతే ఒకవేళ (dglyphs->font == ఖాళీ)
	{
	    // Add sign icon
	    Gsk_Render_కణుపు   *snode;
	    పూర్ణాంకం		    w;

	    FLUSH_కణుపు();

	    snode = gsk_texture_scale_కణుపు_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->అడ్డువరుస),
			dglyphs->dsign->వెడల్పు, dglyphs->dsign->ఎత్తు),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->వెడల్పు + gui.char_వెడల్పు - 1) / gui.char_వెడల్పు;

	    ఒకవేళ (snode != ఖాళీ)
		g_ptr_array_కూడు(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    కొనసాగు;
	}
#అయితే_ముగింపు
	లేకపోతే ఒకవేళ (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_కణుపు();
	    cur_font = dglyphs->font;
	    ప్రారంభం_నిలువు = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	ఒకవేళ (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    కొనసాగు;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	ఒకవేళ (ఖాళీలు_cells > 0)
	{
	    ఒకవేళ (prev_len > 0)
	    {
		// Spaces occurred between valid text: extend
		// previous character's advance
		PangoGlyphInfo *చివరి = &g_array_సూచీ(
			glyph_buf, PangoGlyphInfo,
			prev_len - 1);
		చివరి->geometry.వెడల్పు += CELLS2PANGO(ఖాళీలు_cells);
	    }
	    ఖాళీలు_cells = 0; // Width consumed
	}
    }

    FLUSH_కణుపు();

#నిర్వచనం_రద్దు FLUSH_కణుపు

    ఒకవేళ (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_కణుపు *ఆధారం;
	Gsk_Render_కణుపు *కణుపు;

	draw_అడ్డువరుస_ensure_decor(drow, DRAW_UNDERL);
	ఒకవేళ (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    ఆధారం = draw_layer_get_texture(dlayer, drow->అడ్డువరుస, da, 0, FALSE);
	    కణుపు = gsk_ముసుగు_కణుపు_new(ఆధారం, drow->underl_ముసుగు,
		    GSK_ముసుగు_బహుళకం_ALPHA);
	    gsk_render_కణుపు_unref(ఆధారం);
	    g_ptr_array_కూడు(nodes, కణుపు);
	}

	dlayer->కణుపు = gsk_container_కణుపు_new(
		(Gsk_Render_కణుపు **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    స్థిర శూన్య
draw_అడ్డువరుస_render_special(Draw_అడ్డువరుస *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_కణుపు   *ఆధారం;
    Gsk_Render_కణుపు   *nodes[2];
    పూర్ణాంకం		    n_nodes = 0;

    ఒకవేళ (!dlayer->dirty)
	తిరిగి;

    draw_అడ్డువరుస_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    ఒకవేళ (!rgba_is_transparent(dlayer->buf + UNDERC_స్థానభ్రంశం(drow->n_cells),
		drow->n_cells))
    {
	ఆధారం = draw_layer_get_texture(dlayer, drow->అడ్డువరుస, da,
		UNDERC_స్థానభ్రంశం(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_ముసుగు_కణుపు_new(ఆధారం, drow->underc_ముసుగు,
		GSK_ముసుగు_బహుళకం_ALPHA);
	gsk_render_కణుపు_unref(ఆధారం);
    }

    ఒకవేళ (!rgba_is_transparent(dlayer->buf + STRIKE_స్థానభ్రంశం(drow->n_cells),
		drow->n_cells))
    {
	ఆధారం = draw_layer_get_texture(dlayer, drow->అడ్డువరుస, da,
		STRIKE_స్థానభ్రంశం(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_ముసుగు_కణుపు_new(ఆధారం, drow->strike_ముసుగు,
		GSK_ముసుగు_బహుళకం_ALPHA);
	gsk_render_కణుపు_unref(ఆధారం);
    }

    ఒకవేళ (n_nodes == 1)
    {
	dlayer->కణుపు = nodes[0];
    }
    లేకపోతే ఒకవేళ (n_nodes > 0)
    {
	dlayer->కణుపు = gsk_container_కణుపు_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	కొరకు (పూర్ణాంకం i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(nodes); i++)
	    gsk_render_కణుపు_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    స్థిర శూన్య
draw_అడ్డువరుస_render_overlay(Draw_అడ్డువరుస *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    ఒకవేళ (!dlayer->dirty)
	తిరిగి;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    ఒకవేళ (rgba_is_transparent(dlayer->buf, da->n_నిలువుల_సంఖ్య))
	తిరిగి;

    dlayer->కణుపు = draw_layer_get_texture(dlayer, drow->అడ్డువరుస, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    స్థిర శూన్య
draw_అడ్డువరుస_కూడు_cluster(
	Draw_అడ్డువరుస		*drow,
	DrawAttr	*dattr,
	పూర్ణాంకం		నిలువు,
	పూర్ణాంకం		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	పూర్ణాంకం		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = ఖాళీ;

    ఒకవేళ (నిలువు + n_cells > da->n_నిలువుల_సంఖ్య)
	తిరిగి;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    ఒకవేళ (glyphs != ఖాళీ)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_అడ్డువరుస_fill(drow, నిలువు, నిలువు + n_cells - 1, dglyphs, dattr, &da->cursor);

    ఒకవేళ (dglyphs != ఖాళీ)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    స్థిర పూర్ణాంకం
vim_draw_area_కూడు_string(
	VimDrawArea *self,
	Draw_అడ్డువరుస	    *drow,
	DrawAttr    *dattr,
	పూర్ణాంకం	    నిలువు,
	char_u	    *s,
	పూర్ణాంకం	    len,
	పూర్ణాంకం	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = ఖాళీ;
    పూర్ణాంకం			ప్రారంభం_idx = -1;
    GList		*అంశం_list;
    పూర్ణాంకం			స్తంభం_స్థానభ్రంశం = 0;
    పూర్ణాంకం			cluster_వెడల్పు;
    పూర్ణాంకం			చివరి_glyph_rbearing;
    పూర్ణాంకం			cells = 0;
    పూర్ణాంకం			i = 0;
    gboolean		has_unicode = FALSE;
    gboolean		needs_shaping = FALSE;

    // Scan the string, and find spans where ligatures should not happen. We
    // must ensure that attribute boundaries do not split a base character from
    // its combining marks, as Pango breaks items at attribute boundaries.
    అయితే (i < len)
    {
	పూర్ణాంకం	clen = 1;
	పూర్ణాంకం	is_composing = 0;
	bool	want_disable;

	ఒకవేళ (s[i] >= 0x80)
	{
	    clen = utf_ptr2len(s + i);
	    is_composing = utf_iscomposing(utf_ptr2char(s + i));
	    has_unicode = TRUE;
	}

	ఒకవేళ (is_composing)
	    // Do not change state mid-cluster. Inherit the previous character's
	    // state.
	    want_disable = (ప్రారంభం_idx != -1);
	లేకపోతే
	    // Base character: disable ligatures if it's an ASCII non-ligature
	    // char.
	    want_disable = (s[i] < 0x80 && !gui.ligatures_map[s[i]]);

	// If any base character actually wants ligature/contextual shaping,
	// the string must go through Pango's shaping engine -- the ASCII
	// fast path below draws cached glyphs one-by-one and can never
	// produce a ligature, regardless of 'guiligatures'.
	ఒకవేళ (!is_composing && !want_disable)
	    needs_shaping = TRUE;

	ఒకవేళ (want_disable)
	{
	    ఒకవేళ (ప్రారంభం_idx == -1)
		ప్రారంభం_idx = i;
	}
	లేకపోతే ఒకవేళ (ప్రారంభం_idx != -1)
	{
	    ఒకవేళ (attr_list == ఖాళీ)
		attr_list = pango_attr_list_new();
	    చొప్పించు_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, ప్రారంభం_idx, i);
	    ప్రారంభం_idx = -1;
	}
	i += clen;
    }

    // Fast path for pure ASCII: use cached glyph table. Skip this path when
    // there are non-ascii characters in the string, font attributes, or if
    // there's a possible ligature.
    ఒకవేళ (!(draw_flags & DRAW_ITALIC)
	    && !((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != ఖాళీ
	    && !has_unicode
	    && !needs_shaping)
    {
	కొరకు (i = 0; i < len; ++i)
	{
	    PangoGlyphInfo  glyph = gui.ascii_glyphs->glyphs[2 * s[i]];
	    gboolean	    has_ink;

	    has_ink = glyph_has_ink(gui.ascii_font, glyph);

	    draw_అడ్డువరుస_కూడు_cluster(drow, dattr, నిలువు + i, 1,
		    gui.ascii_font, has_ink ? &glyph : ఖాళీ, 1, self);
	}

	స్తంభం_స్థానభ్రంశం = len;
	వెళ్ళు నిష్క్రమించు;
    }

    glyphs = pango_glyph_string_new();

    ఒకవేళ (attr_list == ఖాళీ)
	attr_list = pango_attr_list_new();

    ఒకవేళ (ప్రారంభం_idx != -1)
	చొప్పించు_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, ప్రారంభం_idx, len);

    cluster_వెడల్పు = PANGO_SCALE * gui.char_వెడల్పు;
    చివరి_glyph_rbearing = PANGO_SCALE * gui.char_వెడల్పు;

    // If 'guifontwide' is set then use that for double-width characters.
    ఒకవేళ (gui.wide_font != ఖాళీ)
	apply_wide_font_attr(s, len, attr_list);

    ఒకవేళ ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	చొప్పించు_PANGO_ATTR(pango_attr_బరువు_new(PANGO_బరువు_BOLD),
		attr_list, 0, len);
    ఒకవేళ (draw_flags & DRAW_ITALIC)
	చొప్పించు_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    అంశం_list = pango_itemize(gui.పాఠ్యం_context,
	    (స్థిరరాశి అక్షరం *)s, 0, len, attr_list, ఖాళీ);

    అయితే (అంశం_list != ఖాళీ)
    {
	Pango_అంశం   *అంశం = అంశం_list->దత్తాంశం;
	పూర్ణాంకం	    అంశం_cells = 0;
	పూర్ణాంకం	    cluster_ప్రారంభం_idx = 0;
	పూర్ణాంకం	    current_cluster_నిలువు = నిలువు + స్తంభం_స్థానభ్రంశం;
	పూర్ణాంకం	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	అంశం_list = g_list_delete_link(అంశం_list, అంశం_list);

	// Force LTR direction; Vim handles bidi on its own.
	అంశం->analysis.level = (అంశం->analysis.level + 1) & (~1U);

	pango_shape_నిండిన((స్థిరరాశి అక్షరం *)s + అంశం->స్థానభ్రంశం, అంశం->నిడివి,
		(స్థిరరాశి అక్షరం *)s, len, &అంశం->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	కొరకు (i = 0; i < glyphs->అంకె_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    ఒకవేళ (glyph->attr.is_cluster_ప్రారంభం)
	    {
		పూర్ణాంకం cellcount;

		cellcount = లెక్క_cluster_cells(
			s, అంశం, glyphs, i, &cluster_వెడల్పు,
			(అంశం_list != ఖాళీ) ? &చివరి_glyph_rbearing : ఖాళీ);

		ఒకవేళ (i > 0 && cellcount > 0)
		{
		    draw_అడ్డువరుస_కూడు_cluster(drow, dattr,
			    current_cluster_నిలువు, current_cluster_cells,
			    అంశం->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_ప్రారంభం_idx : ఖాళీ,
			    i - cluster_ప్రారంభం_idx, self);

		    current_cluster_నిలువు += current_cluster_cells;
		    cluster_ప్రారంభం_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		ఒకవేళ (cellcount > 0)
		{
		    పూర్ణాంకం వెడల్పు;

		    వెడల్పు = cellcount * gui.char_వెడల్పు * PANGO_SCALE;
		    glyph->geometry.x_స్థానభ్రంశం +=
			MAX(0, వెడల్పు - cluster_వెడల్పు) / 2;
		    glyph->geometry.వెడల్పు = వెడల్పు;
		}
		లేకపోతే
		    అమరిక_zero_వెడల్పు_cluster(అంశం, glyph, cells,
			    cluster_వెడల్పు,
			    చివరి_glyph_rbearing);

		అంశం_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    లేకపోతే ఒకవేళ (i > 0)
	    {
		పూర్ణాంకం వెడల్పు;

		ఒకవేళ (glyph->geometry.x_స్థానభ్రంశం >= 0)
		{
		    glyphs->glyphs[i].geometry.వెడల్పు =
			glyphs->glyphs[i - 1].geometry.వెడల్పు;
		    glyphs->glyphs[i - 1].geometry.వెడల్పు = 0;
		}
		వెడల్పు = cells * gui.char_వెడల్పు * PANGO_SCALE;
		glyph->geometry.x_స్థానభ్రంశం +=
		    MAX(0, వెడల్పు - cluster_వెడల్పు) / 2;
	    }
	    లేకపోతే
		glyph->geometry.వెడల్పు = 0;


	    ఒకవేళ (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			అంశం->analysis.font, *glyph);
	}
	ఒకవేళ (glyphs->అంకె_glyphs > 0)
	    draw_అడ్డువరుస_కూడు_cluster(drow, dattr,
		    current_cluster_నిలువు, current_cluster_cells,
		    అంశం->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_ప్రారంభం_idx : ఖాళీ,
		    glyphs->అంకె_glyphs - cluster_ప్రారంభం_idx, self);

	pango_అంశం_free(అంశం);
	స్తంభం_స్థానభ్రంశం += అంశం_cells;
    }

    pango_glyph_string_free(glyphs);

నిష్క్రమించు:
    ఒకవేళ (attr_list != ఖాళీ)
	pango_attr_list_unref(attr_list);

    తిరిగి స్తంభం_స్థానభ్రంశం;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    పూర్ణాంకం
vim_draw_area_draw_string(
	VimDrawArea *self,
	పూర్ణాంకం	    అడ్డువరుస,
	పూర్ణాంకం	    నిలువు,
	char_u	    *s,
	పూర్ణాంకం	    len,
	పూర్ణాంకం	    flags)
{
    Draw_అడ్డువరుస	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = ఖాళీ;
    పూర్ణాంకం		convlen;
    పూర్ణాంకం		n_cells ;

    ఒకవేళ (unlikely(self->అడ్డువరుసలు_2 == ఖాళీ
		|| అడ్డువరుస >= self->n_అడ్డువరుసలు
		|| నిలువు >= self->n_నిలువుల_సంఖ్య))
	తిరిగి len;

    ఒకవేళ (output_conv.vc_type != CONV_NONE)
    {
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	ఒకవేళ (conv_buf != ఖాళీ)
	{
	    s = conv_buf;
	    len = convlen;
	}
    }

    // Safety check: pango crashes with invalid utf-8.
    ఒకవేళ (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	తిరిగి len;
    }

    drow = self->అడ్డువరుసలు_2 + అడ్డువరుస;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_కూడు_string(self, drow, &dattr, నిలువు, s, len, flags);

    vim_free(conv_buf);

    తిరిగి n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    శూన్య
vim_draw_area_clear_ఖండం(
	VimDrawArea	*self,
	పూర్ణాంకం		row1,
	పూర్ణాంకం		col1,
	పూర్ణాంకం		row2,
	పూర్ణాంకం		col2)
{
    ఒకవేళ (unlikely(self->అడ్డువరుసలు_2 == ఖాళీ
		|| row1 >= self->n_అడ్డువరుసలు
		|| col1 >= self->n_నిలువుల_సంఖ్య
		|| row2 >= self->n_అడ్డువరుసలు
		|| col2 >= self->n_నిలువుల_సంఖ్య))
	తిరిగి;

    కొరకు (పూర్ణాంకం r = row1; r <= row2; r++)
	draw_అడ్డువరుస_fill(self->అడ్డువరుసలు_2 + r, col1, col2, ఖాళీ, ఖాళీ, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    శూన్య
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_ఖండం(self, 0, 0, self->n_అడ్డువరుసలు - 1, self->n_నిలువుల_సంఖ్య - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    శూన్య
vim_draw_area_move_ఖండం(
	VimDrawArea *self,
	పూర్ణాంకం	    గమ్యం,
	పూర్ణాంకం	    row1,
	పూర్ణాంకం	    row2,
	పూర్ణాంకం	    col1,
	పూర్ణాంకం	    col2)
{
    పూర్ణాంకం స్థానభ్రంశం = row2 - row1;

    ఒకవేళ (unlikely(self->అడ్డువరుసలు_2 == ఖాళీ
		|| row1 >= self->n_అడ్డువరుసలు
		|| row2 >= self->n_అడ్డువరుసలు
		|| గమ్యం >= self->n_అడ్డువరుసలు
		|| col1 >= self->n_నిలువుల_సంఖ్య
		|| col2 >= self->n_నిలువుల_సంఖ్య))
	తిరిగి;

    ప్రతిజ్ఞా(row2 >= row1);
    ప్రతిజ్ఞా(col2 >= col1);
    ప్రతిజ్ఞా(row1 != గమ్యం);

    ఒకవేళ (row1 > గమ్యం)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	కొరకు (పూర్ణాంకం o = 0; o <= స్థానభ్రంశం; o++)
	    draw_అడ్డువరుస_move_గమ్యం(self->అడ్డువరుసలు_2 + గమ్యం + o, self->అడ్డువరుసలు_2 + row1 + o,
		    col1, col2, self);
    }
    లేకపోతే
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	కొరకు (పూర్ణాంకం o = స్థానభ్రంశం; o >= 0; o--)
	    ఒకవేళ (గమ్యం + o >= self->n_అడ్డువరుసలు)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_ఖండం(self, row1 + o, col1, row1 + o, col2);
	    లేకపోతే
		draw_అడ్డువరుస_move_గమ్యం(self->అడ్డువరుసలు_2 + గమ్యం + o, self->అడ్డువరుసలు_2 + row1 + o,
			col1, col2, self);
    }
}

/*
 * Set the cursor to the given width and height, use "gui.bgcolor" and
 * "gui.fgcolor" for the colors as well. If "w" and "h" is zero, then a full
 * block cursor is used, if "w" and "h" is -1 then a hollow cursor is used,
 * otherwise a part cursor with dimensions "w" and "h" is used.
 */
    శూన్య
vim_draw_area_సమితి_cursor(VimDrawArea *self, పూర్ణాంకం w, పూర్ణాంకం h)
{
    self->cursor.draw = TRUE;
    self->cursor.వెడల్పు = w;
    self->cursor.ఎత్తు = h;
    // Remember this now: the snapshot runs later, when the drawing position
    // has moved on.
    self->cursor.n_cells = 1 + mb_lefthalve(gui.cursor_అడ్డువరుస, gui.cursor_నిలువు);
    self->cursor.bg_color = *gui.bgcolor;
    self->cursor.fg_color = *gui.fgcolor;
}

/*
 * Invert the rectangle in the draw area.
 */
    శూన్య
vim_draw_area_invert_ఖండం(
	VimDrawArea	*self,
	పూర్ణాంకం		అడ్డువరుస,
	పూర్ణాంకం		నిలువు,
	పూర్ణాంకం		nrows,
	పూర్ణాంకం		ncols)
{
    ఒకవేళ (unlikely(self->అడ్డువరుసలు_2 == ఖాళీ
		|| అడ్డువరుస >= self->n_అడ్డువరుసలు
		|| నిలువు >= self->n_నిలువుల_సంఖ్య
		|| అడ్డువరుస + nrows - 1 >= self->n_అడ్డువరుసలు
		|| నిలువు + ncols - 1 >= self->n_నిలువుల_సంఖ్య))
	తిరిగి;

    కొరకు (పూర్ణాంకం r = అడ్డువరుస; r < అడ్డువరుస + nrows; r++)
    {
	Draw_అడ్డువరుస	    *drow = self->అడ్డువరుసలు_2 + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	కొరకు (పూర్ణాంకం c = 0; c < ncols; c++)
	    dlayer->buf[c + నిలువు] = ~dlayer->buf[c + నిలువు];
	draw_అడ్డువరుస_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#ఒకవేళ defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    శూన్య
vim_draw_area_కూడు_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	పూర్ణాంకం	    అడ్డువరుస,
	పూర్ణాంకం	    నిలువు,
	పూర్ణాంకం	    వెడల్పు,
	పూర్ణాంకం	    ఎత్తు)
{
    Draw_అడ్డువరుస	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    పూర్ణాంకం		cells;

    ఒకవేళ (unlikely(self->అడ్డువరుసలు_2 == ఖాళీ
		|| అడ్డువరుస >= self->n_అడ్డువరుసలు
		|| నిలువు >= self->n_నిలువుల_సంఖ్య))
	తిరిగి;

    drow = self->అడ్డువరుసలు_2 + అడ్డువరుస;
    dsign = draw_sign_new(sign, వెడల్పు, ఎత్తు);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (వెడల్పు + gui.char_వెడల్పు - 1) / gui.char_వెడల్పు;

    draw_అడ్డువరుస_fill(drow, నిలువు, నిలువు + cells - 1, dglyphs, ఖాళీ, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    శూన్య
vim_draw_area_కూడు_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	పూర్ణాంకం		అడ్డువరుస,
	పూర్ణాంకం		నిలువు,
	పూర్ణాంకం		వెడల్పు,
	పూర్ణాంకం		ఎత్తు)
{
    Draw_అడ్డువరుస	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    పూర్ణాంకం		cells;
    GdkTexture	*texture;
    GBytes	*బైట్లు;

    ఒకవేళ (unlikely(self->అడ్డువరుసలు_2 == ఖాళీ
		|| అడ్డువరుస >= self->n_అడ్డువరుసలు
		|| నిలువు >= self->n_నిలువుల_సంఖ్య))
	తిరిగి;

    cairo_surface_flush(surf);
    బైట్లు = g_బైట్లు_new_with_free_func(cairo_image_surface_get_దత్తాంశం(surf),
	    cairo_image_surface_get_ఎత్తు(surf)
	    * cairo_image_surface_get_stride(surf),
	    (GDestroyNotify)cairo_surface_destroy,
	    cairo_surface_reference(surf));

    texture = gdk_జ్ఞాపకం_texture_new(cairo_image_surface_get_వెడల్పు(surf),
	    cairo_image_surface_get_ఎత్తు(surf),
	    GDK_జ్ఞాపకం_A8_R8_G8_B8_PREMULTIPLIED,
	    బైట్లు, cairo_image_surface_get_stride(surf));
    g_బైట్లు_unref(బైట్లు);

    drow = self->అడ్డువరుసలు_2 + అడ్డువరుస;
    dsign = draw_sign_new(texture, వెడల్పు, ఎత్తు);
    g_object_unref(texture);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (వెడల్పు + gui.char_వెడల్పు - 1) / gui.char_వెడల్పు;

    draw_అడ్డువరుస_fill(drow, నిలువు, నిలువు + cells - 1, dglyphs, ఖాళీ, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    శూన్య
vim_draw_area_కూడు_external(VimDrawArea *self, Gsk_Render_కణుపు *కణుపు)
{
    g_hash_పట్టిక_కూడు(self->external, gsk_render_కణుపు_ref(కణుపు));
}

    శూన్య
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_కణుపు *కణుపు)
{
    g_hash_పట్టిక_remove(self->external, కణుపు);
}

#అయితే_ముగింపు

/*
 * Add the cursor to the snapshot.
 */
    స్థిర శూన్య
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_కణుపు	*పాఠ్యం_కణుపు,	// May be NULL
	Gsk_Render_కణుపు	*special_కణుపు,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    పూర్ణాంకం		    w = cursor->వెడల్పు;
    పూర్ణాంకం		    h = cursor->ఎత్తు;
    graphene_rect_t rect;

    ఒకవేళ (!cursor->draw)
	తిరిగి;

    ఒకవేళ (cursor->వెడల్పు <= 0 && cursor->ఎత్తు <= 0)
    {
	w += gui.char_వెడల్పు * cursor->n_cells;
	h = gui.char_ఎత్తు;
    }

    graphene_rect_init(&rect,
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    CURSOR_BAR_కుడి ? FILL_X(gui.cursor_నిలువు + 1) - w :
#అయితే_ముగింపు
	    FILL_X(gui.cursor_నిలువు),
	    FILL_Y(gui.cursor_అడ్డువరుస) + gui.char_ఎత్తు - h, w, h);


    ఒకవేళ (cursor->వెడల్పు == -1 && cursor->ఎత్తు == -1)
    {
	// Create hollow cursor
	GskRoundedRect	    outline;
	స్థిర స్థిరరాశి దశాంశం  border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	స్థిరరాశి GdkRGBA	    color[4] = {
	    cursor->bg_color, cursor->bg_color,
	    cursor->bg_color, cursor->bg_color
	} ;

	gsk_rounded_rect_init_from_rect(&outline, &rect, 0.0f);
	gtk_snapshot_append_border(snapshot, &outline, border, color);
    }
    లేకపోతే
    {
	// Create block style cursor, by masking the text + under decorations
	// with the fg color.
	gtk_snapshot_append_color(snapshot, &cursor->bg_color, &rect);

	gtk_snapshot_నెట్టు_ముసుగు(snapshot, GSK_ముసుగు_బహుళకం_ALPHA);

	// Used as an "outline" for the fg color.
	ఒకవేళ (పాఠ్యం_కణుపు != ఖాళీ)
	    gtk_snapshot_append_కణుపు(snapshot, పాఠ్యం_కణుపు);
	ఒకవేళ (special_కణుపు != ఖాళీ)
	    gtk_snapshot_append_కణుపు(snapshot, special_కణుపు);

	gtk_snapshot_తీయి(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_తీయి(snapshot);
    }
}

    స్థిర శూన్య
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    పూర్ణాంకం		    ఎత్తు, వెడల్పు;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = ఖాళీ;
    Gsk_Render_కణుపు   *body_కణుపు;
    Gsk_Render_కణుపు   *invert_కణుపు = ఖాళీ;
#నిర్వచితమైతే FEAT_IMAGE_GUI
    Gsk_Render_కణుపు   *ext_కణుపు;
    GHash_పట్టిక_Iter  iter;
#అయితే_ముగింపు

    gui_mch_సమితి_bg_color(gui.వెనుక_pixel);
    ఎత్తు = gtk_widget_get_ఎత్తు(widget) + gui.bleed_bot;
    వెడల్పు = gtk_widget_get_వెడల్పు(widget) + gui.bleed_కుడి;

    ఒకవేళ (self->అడ్డువరుసలు_2 == ఖాళీ)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, వెడల్పు, ఎత్తు));
	తిరిగి;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    ఒకవేళ (self->bleed_కుడి == -1 || self->bleed_కుడి != gui.bleed_కుడి)
    {
	self->bleed_కుడి = gui.bleed_కుడి;
	కొరకు (పూర్ణాంకం r = 0; r < self->n_అడ్డువరుసలు; r++)
	{
	    draw_అడ్డువరుస_dirty_layer(self->అడ్డువరుసలు_2 + r, DRAW_LAYER_BACKGROUND);
	    draw_అడ్డువరుస_dirty_layer(self->అడ్డువరుసలు_2 + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#ఒకవేళ GTK_CHECK_సంస్కరణ(4,24,0)
    gtk_snapshot_సమితి_snap(body_snapshot, GSK_RECT_SNAP_విడత);
#అయితే_ముగింపు

    కొరకు (పూర్ణాంకం r = 0; r < self->n_అడ్డువరుసలు; r++)
    {
	Draw_అడ్డువరుస *drow = self->అడ్డువరుసలు_2 + r;

	draw_అడ్డువరుస_render_background(drow, self);
	draw_అడ్డువరుస_render_పాఠ్యం(drow, self);
	draw_అడ్డువరుస_render_special(drow, self);
	draw_అడ్డువరుస_render_overlay(drow, self);

	కొరకు (పూర్ణాంకం l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    ఒకవేళ (dlayer->కణుపు != ఖాళీ)
	    {
		ఒకవేళ (l == DRAW_LAYER_OVERLAY)
		{
		    ఒకవేళ (invert_snapshot == ఖాళీ)
		    {
			invert_snapshot = gtk_snapshot_new();
#ఒకవేళ GTK_CHECK_సంస్కరణ(4,24,0)
			gtk_snapshot_సమితి_snap(invert_snapshot,
				GSK_RECT_SNAP_విడత);
#అయితే_ముగింపు
		    }
		    gtk_snapshot_append_కణుపు(invert_snapshot, dlayer->కణుపు);
		}
		లేకపోతే
		    gtk_snapshot_append_కణుపు(body_snapshot, dlayer->కణుపు);
	    }
	}

	ఒకవేళ (r == gui.cursor_అడ్డువరుస)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_పాఠ్యం].కణుపు,
		    drow->dlayers[DRAW_LAYER_SPECIAL].కణుపు,
		    body_snapshot);
    }

    ఒకవేళ (invert_snapshot != ఖాళీ)
	gtk_snapshot_నెట్టు_blend(snapshot, GSK_BLEND_బహుళకం_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, వెడల్పు, ఎత్తు));
    body_కణుపు = gtk_snapshot_free_గమ్యం_కణుపు(body_snapshot);
    ఒకవేళ (body_కణుపు != ఖాళీ)
    {
	gtk_snapshot_append_కణుపు(snapshot, body_కణుపు);
	gsk_render_కణుపు_unref(body_కణుపు);
    }

#నిర్వచితమైతే FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_పట్టిక_iter_init(&iter, self->external);

    అయితే (g_hash_పట్టిక_iter_తదుపరి(&iter, (శూన్య **)&ext_కణుపు, ఖాళీ))
	gtk_snapshot_append_కణుపు(snapshot, ext_కణుపు);
#అయితే_ముగింపు

    ఒకవేళ (invert_snapshot != ఖాళీ)
    {
	gtk_snapshot_తీయి(snapshot);
	invert_కణుపు = gtk_snapshot_free_గమ్యం_కణుపు(invert_snapshot);
	// Not sure if it can be NULL but still check
	ఒకవేళ (invert_కణుపు != ఖాళీ)
	{
	    gtk_snapshot_append_కణుపు(snapshot, invert_కణుపు);
	    gsk_render_కణుపు_unref(invert_కణుపు);
	}
	gtk_snapshot_తీయి(snapshot);
    }
}
