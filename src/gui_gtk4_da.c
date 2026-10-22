/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#அந்தர்பாவ "vim.h"

#அந்தர்பாவ <gtk/gtk.h>
#அந்தர்பாவ "gui_gtk4_da.h"


#எனில் defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Used for sign icons and netbeans multisign indicator. It is in a DrawGlyphs
 * so that it can move/be manipulated with the text.
 */
வகைவரை கட்டமைப்பு
{
    GdkTexture	*texture;
    முழு		width;
    முழு		height;
} DrawSign;
#endif

/*
 * Temporary struct when drawing string representing visual attributes of a cell
 */
வகைவரை கட்டமைப்பு
{
    uint32_t	bg_color;
    uint32_t	fg_color;   // Only used for underlines
    uint32_t	sp_color;
    முழு		draw_flags; // DRAW_* flags, only DRAW_UNDERL, DRAW_UNDERC, and
			    // DRAW_STRIKE are used.
} DrawAttr;

/*
 * Represents the glyphs of a single cell.
 */
வகைவரை கட்டமைப்பு
{
    முழு refcount;

    uint32_t	fg_color;
    PangoFont	*font;
    // If "font" is NULL, "dsign" is used, otherwise "glyphs" is used.
    ஒன்றிப்பு
    {
	PangoGlyphInfo  *glyphs; // May be NULL
#எனில் defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	DrawSign	*dsign;
#endif
    };
    முழு n_glyphs;
} DrawGlyphs;

வகைவரை கட்டமைப்பு
{
    DrawGlyphs *dglyphs;
} DrawCell;

வகைவரை எண்ணிக்கை
{
    DRAW_LAYER_BACKGROUND,  // Background color, buf stores bg color
    DRAW_LAYER_TEXT,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

வகைவரை கட்டமைப்பு
{
    Gsk_Render_முனை   *முனை;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#define UNDERC_OFFSET(n_cols) (0)
#define STRIKE_OFFSET(n_cols) (n_cols)

வகைவரை கட்டமைப்பு
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    முழு		n_cells; // For convenience
    முழு		row;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_முனை *underl_mask;
    Gsk_Render_முனை *underc_mask;
    Gsk_Render_முனை *strike_mask;
} DrawRow;

#define செருகு_PANGO_ATTR(Attribute, AttrList, Start, End) \
    செய் { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->start_index = (Start); \
	tmp_attr_->end_index = (End); \
	pango_attr_list_செருகு((AttrList), tmp_attr_); \
    } போது (FALSE)
#define CELLS2PANGO(c) ((c) * gui.char_width * PANGO_SCALE)

/*
 * Attributes of cursor
 */
வகைவரை கட்டமைப்பு
{
    gboolean draw; // If cursor should be drawn
    முழு width;
    முழு height;
    முழு n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

கட்டமைப்பு _VimDrawArea
{
    GtkWidget parent;

    DrawRow *rows;

    முழு n_rows;
    முழு	n_cols;

    முழு	bleed_வலது;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*முனை_buf;

    DrawCursor cursor;

#ifdef FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHashTable *external;
#endif
};

நிலையான வெறுமை draw_row_init(DrawRow *drow, முழு row, முழு cols);
நிலையான வெறுமை draw_row_clear(DrawRow *drow);
நிலையான வெறுமை draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t);
நிலையான வெறுமை vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

நிலையான GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    நிலையான வெறுமை
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    க்கு (முழு r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    g_free(self->rows);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->முனை_buf, TRUE);

#ifdef FEAT_IMAGE_GUI
    g_hash_table_unref(self->external);
#endif

    G_OBJECT_CLASS(vim_draw_area_parent_class)->finalize(obj);
}

    நிலையான வெறுமை
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    நிலையான வெறுமை
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_வலது = -1;
#ifdef FEAT_IMAGE_GUI
    self->external = g_hash_table_new_full(g_direct_hash, g_direct_equal,
	    வெற்று, (GDestroyNotify)gsk_render_முனை_unref);
#endif
    self->glyph_buf = g_array_new(FALSE, FALSE, அளவு(PangoGlyphInfo));
    self->முனை_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_முனை_unref);

}

    GtkWidget *
vim_draw_area_new(வெறுமை)
{
    திரும்பு g_object_new(VIM_TYPE_DRAW_AREA, வெற்று);
}

/*
 * Set the size of the draw area to "rows" and "cols".
 */
    வெறுமை
vim_draw_area_set_size(VimDrawArea *self, முழு rows, முழு cols)
{
    எனில் (self->rows != வெற்று && self->n_rows == rows && self->n_cols == cols)
	திரும்பு;
    எனில் (rows == 0 || cols == 0)
	திரும்பு;

    vim_draw_area_clear(self);

    க்கு (முழு r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    self->rows = g_realloc_n(self->rows, rows, அளவு(DrawRow));
    க்கு (முழு r = 0; r < rows; r++)
	draw_row_init(self->rows + r, r, cols);

    self->n_rows = rows;
    self->n_cols = cols;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    நிலையான uint32_t
rgba_to_u32(மாறிலி GdkRGBA *rgba)
{
    ஒன்றிப்பு { uint32_t val; uint8_t bytes[4]; } color;

    color.bytes[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.bytes[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.bytes[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.bytes[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    திரும்பு color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    நிலையான GdkRGBA
u32_to_rgba(uint32_t val)
{
    ஒன்றிப்பு { uint32_t val; uint8_t bytes[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.bytes[0] / 255.0;
    color.green = packed.bytes[1] / 255.0;
    color.blue  = packed.bytes[2] / 255.0;
    color.alpha = packed.bytes[3] / 255.0;
    திரும்பு color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    நிலையான gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    ஒன்றிப்பு { uint32_t val; uint8_t bytes[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.bytes[0] << 16) |
	((guicolor_T)packed.bytes[1] << 8)  |
	packed.bytes[2];
    திரும்பு res == rgb;
}

/*
 * Checks if a RGBA pixel buffer is fully transparent.
 */
    நிலையான உள்வரி gboolean
rgba_is_transparent(மாறிலி uint32_t *buf, முழு n_pixels)
{
    // Could also compare 2 pixels at once using a 64 bit integer, but not sure
    // if that might cause alignment issues...
    க்கு (முழு i = 0; i < n_pixels; i++)
	எனில் (buf[i] != 0)
	    திரும்பு FALSE;
    திரும்பு TRUE;
}

/*
 * Return TRUE if "glyph" has ink (not whitespace).
 */
    நிலையான gboolean
glyph_has_ink(PangoFont *font, PangoGlyphInfo glyph)
{
    PangoRectangle ink;

    pango_font_get_glyph_extents (font, glyph.glyph, &ink, வெற்று);

    திரும்பு ink.width > 0 && ink.height > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    நிலையான வெறுமை
apply_wide_font_attr(char_u *s, முழு len, PangoAttrList *attr_list)
{
    char_u  *start = வெற்று;
    char_u  *p;
    முழு	    uc;

    க்கு (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	எனில் (start == வெற்று)
	{
	    எனில் (uc >= 0x80 && utf_char2cells(uc) == 2)
		start = p;
	}
	இல்லையெனில் எனில் (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    செருகு_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, start - s, p - s);
	    start = வெற்று;
	}
    }

    எனில் (start != வெற்று)
	செருகு_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, start - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    நிலையான முழு
மொத்தம்_cluster_cells(
	char_u		    *s,
	PangoItem	    *item,
	PangoGlyphString    *glyphs,
	முழு		    i,
	முழு		    *cluster_width,
	முழு		    *last_glyph_rbearing)
{
    char_u  *p;
    முழு	    next;
    முழு	    start, end;
    முழு	    width;
    முழு	    uc;
    முழு	    cellcount = 0;

    width = glyphs->glyphs[i].geometry.width;

    க்கு (next = i + 1; next < glyphs->இலக்கம்_glyphs; ++next)
    {
	எனில் (glyphs->glyphs[next].attr.is_cluster_start)
	    நிறுத்து;
	இல்லையெனில் எனில் (glyphs->glyphs[next].geometry.width > width)
	    width = glyphs->glyphs[next].geometry.width;
    }

    start = item->offset + glyphs->log_clusters[i];
    end   = item->offset + ((next < glyphs->இலக்கம்_glyphs) ?
	    glyphs->log_clusters[next] : item->length);

    க்கு (p = s + start; p < s + end; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	எனில் (uc < 0x80)
	    ++cellcount;
	இல்லையெனில் எனில் (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    எனில் (last_glyph_rbearing != வெற்று
	    && cellcount > 0 && next == glyphs->இலக்கம்_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(item->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, வெற்று);

	எனில் (PANGO_RBEARING(ink_rect) > 0)
	    *last_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    எனில் (cellcount > 0)
	*cluster_width = width;

    திரும்பு cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    நிலையான வெறுமை
setup_zero_width_cluster(
	PangoItem	*item,
	PangoGlyphInfo	*glyph,
	முழு		last_cellcount,
	முழு		last_cluster_width,
	முழு		last_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    முழு		    width;

    width = last_cellcount * gui.char_width * PANGO_SCALE;
    glyph->geometry.x_offset = -width + MAX(0, width - last_cluster_width) / 2;
    glyph->geometry.width = 0;

    pango_font_get_glyph_extents(item->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    எனில் (ink_rect.x < 0)
    {
	glyph->geometry.x_offset += last_glyph_rbearing;
	glyph->geometry.y_offset  = logical_rect.height
	    - (gui.char_height - p_linespace) * PANGO_SCALE;
    }
    இல்லையெனில்
	glyph->geometry.x_offset = -width + MAX(0, width - ink_rect.width) / 2;
}

#எனில் defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    நிலையான DrawSign *
draw_sign_new(GdkTexture *texture, முழு width, முழு height)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->width = width;
    dsign->height = height;

    திரும்பு dsign;
}

    நிலையான வெறுமை
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#endif

    நிலையான வெறுமை
draw_attr_init(
	DrawAttr	*dattr,
	மாறிலி GdkRGBA	*bg_color,
	மாறிலி GdkRGBA	*fg_color,
	மாறிலி GdkRGBA	*sp_color,
	முழு		draw_flags)
{
    dattr->bg_color = rgba_to_u32(bg_color);
    dattr->fg_color = rgba_to_u32(fg_color);
    dattr->sp_color = rgba_to_u32(sp_color);
    dattr->draw_flags = draw_flags & (DRAW_UNDERL | DRAW_UNDERC | DRAW_STRIKE);
}

/*
 * Apply the draw attr to the row for the given col/cell. Only dirty layers that
 * have actually changed.
 */
    நிலையான வெறுமை
draw_attr_apply(DrawAttr *dattr, முழு col, DrawRow *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + col;
    uint32_t	*text_cell = drow->dlayers[DRAW_LAYER_TEXT].buf + col;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    முழு		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + UNDERC_OFFSET(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + STRIKE_OFFSET(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    எனில் (dattr != வெற்று)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.back_pixel, bg_color);
    }

    எனில் (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	எனில் (bg_is_default)
	    *bg_cell = 0;
	இல்லையெனில்
	    *bg_cell = bg_color;
    }
    எனில் ((draw_flags & DRAW_UNDERL && *text_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *text_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);
	எனில் (draw_flags & DRAW_UNDERL)
	    *text_cell = fg_color;
	இல்லையெனில்
	    *text_cell = 0;
    }
    எனில் ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	எனில் (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	இல்லையெனில்
	    *underc_cell = 0;
    }
    எனில் ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	எனில் (draw_flags & DRAW_STRIKE)
	    *strike_cell = sp_color;
	இல்லையெனில்
	    *strike_cell = 0;
    }
}

    நிலையான DrawGlyphs *
draw_glyphs_new(
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	முழு		n_glyphs,
	uint32_t	fg_color)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->font = g_object_ref(font);
    dglyphs->glyphs = g_memdup2(glyphs, அளவு(*glyphs) * n_glyphs);
    dglyphs->n_glyphs = n_glyphs;
    dglyphs->fg_color = fg_color;

    திரும்பு dglyphs;
}

#எனில் defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw glyphs that shows the given sign. Note that ownership of
 * "dsign" is taken.
 */
    நிலையான DrawGlyphs *
draw_glyphs_new_sign(DrawSign *dsign)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->dsign = dsign;

    திரும்பு dglyphs;
}
#endif

    நிலையான DrawGlyphs *
draw_glyphs_ref(DrawGlyphs *dglyphs)
{
    dglyphs->refcount++;
    திரும்பு dglyphs;
}

    நிலையான வெறுமை
draw_glyphs_unref(DrawGlyphs *dglyphs)
{
    எனில் (--dglyphs->refcount <= 0)
    {
#எனில் defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	எனில் (dglyphs->font == வெற்று)
	    draw_sign_free(dglyphs->dsign);
	இல்லையெனில்
#endif
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
    நிலையான gboolean
draw_glyphs_same(DrawGlyphs *a, DrawGlyphs *b)
{
    எனில் (a == வெற்று && b == வெற்று)
	திரும்பு TRUE;
    எனில் (a == வெற்று || b == வெற்று)
	திரும்பு FALSE;
    எனில் (a->fg_color != b->fg_color)
	திரும்பு FALSE;
    எனில் (a->n_glyphs != b->n_glyphs || a->font != b->font)
	திரும்பு FALSE;
    க்கு (முழு i = 0; i < a->n_glyphs; i++)
	எனில் (a->glyphs[i].glyph != b->glyphs[i].glyph
		|| a->glyphs[i].geometry.width != b->glyphs[i].geometry.width
		|| a->glyphs[i].geometry.x_offset != b->glyphs[i].geometry.x_offset
		|| a->glyphs[i].geometry.y_offset != b->glyphs[i].geometry.y_offset
		|| a->glyphs[i].attr.is_cluster_start != b->glyphs[i].attr.is_cluster_start)
	    திரும்பு FALSE;
    திரும்பு TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    நிலையான Gsk_Render_முனை *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	முழு	    row,
	VimDrawArea *da,
	முழு	    off,
	gboolean    bleed)
{
    GBytes	    *bytes;
    GdkTexture	    *texture;
    Gsk_Render_முனை   *முனை;
    முழு		    height;

#எனில் GTK_CHECK_VERSION(4,24,0)
    height = gui.char_height;
#இல்லையெனில்
    height = gui.char_height + 1;
#endif

    எனில் (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		அளவு(uint32_t) * da->n_cols);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_cols - 1)),
		அளவு(uint32_t)); // Add bleed using last pixel of buffer
	bytes = g_byte_array_free_to_bytes(arr);
    }
    இல்லையெனில்
	bytes = g_bytes_new(dlayer->buf + off, அளவு(uint32_t) * da->n_cols);

    texture = gdk_memory_texture_new(da->n_cols + bleed, 1,
	    GDK_MEMORY_R8G8B8A8, bytes, (da->n_cols + bleed) * 4);
    g_bytes_unref(bytes);

    // Scale texture to actual size
    முனை = gsk_texture_scale_முனை_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		(da->n_cols + bleed) * gui.char_width, gui.char_height + bleed),
	    GSK_SCALING_FILTER_NEAREST);
    எனில் (bleed)
    {
	Gsk_Render_முனை *new;

	new = gsk_clip_முனை_new(முனை,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		    da->n_cols * gui.char_width + da->bleed_வலது, height));
	gsk_render_முனை_unref(முனை);
	முனை = new;
    }
    g_object_unref(texture);
    திரும்பு முனை;
}

    நிலையான வெறுமை
draw_row_init(DrawRow *drow, முழு row, முழு cols)
{
    drow->cells = g_malloc0_n(cols, அளவு(DrawCell));
    drow->n_cells = cols;
    drow->row = row;
    drow->underl_mask = drow->underc_mask = drow->strike_mask = வெற்று;

    memset(drow->dlayers, 0, அளவு(DrawLayer) * N_DRAW_LAYERS);
    க்கு (முழு l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	எனில் (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(cols * 2, 4);
	இல்லையெனில்
	    dlayer->buf = g_malloc0_n(cols, 4);
    }
}

    நிலையான வெறுமை
draw_row_clear(DrawRow *drow)
{
    g_free(drow->cells);
    க்கு (முழு l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	எனில் (dlayer->முனை != வெற்று)
	    gsk_render_முனை_unref(dlayer->முனை);
	g_free(dlayer->buf);
    }
    g_clear_pointer(&drow->underl_mask, gsk_render_முனை_unref);
    g_clear_pointer(&drow->underc_mask, gsk_render_முனை_unref);
    g_clear_pointer(&drow->strike_mask, gsk_render_முனை_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    நிலையான வெறுமை
draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_pointer(&dlayer->முனை, gsk_render_முனை_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    நிலையான வெறுமை
draw_row_uninvert_cell(DrawRow *drow, முழு col)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    எனில் (buf[col] == 0xffffffff)
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[col] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    நிலையான வெறுமை
draw_row_set_cell(DrawRow *drow, முழு col, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + col;

    எனில் (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    எனில் (dcell->dglyphs != dglyphs)
    {
	எனில் (dcell->dglyphs != வெற்று)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == வெற்று ? வெற்று : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, col, drow);
    draw_row_uninvert_cell(drow, col);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    நிலையான வெறுமை
draw_row_fill(
	DrawRow	    *drow,
	முழு	    col1,
	முழு	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    க்கு (முழு c = col1; c <= col2; c++)
	draw_row_set_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    எனில் (gui.cursor_row == drow->row && gui.cursor_col >= col1
	    && gui.cursor_col <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    நிலையான வெறுமை
draw_row_move_to(
	DrawRow	    *dest_row,
	DrawRow	    *src_row,
	முழு	    col1,
	முழு	    col2,
	VimDrawArea *da)
{
    முழு move_size = (col2 - col1 + 1) * அளவு(DrawCell);

    draw_row_fill(dest_row, col1, col2, வெற்று, வெற்று, &da->cursor);
    memmove(dest_row->cells + col1, src_row->cells + col1, move_size);
    // NULL the draw cells so we don't double unreference.
    memset(src_row->cells + col1, 0, move_size);
    draw_row_dirty_layer(src_row, DRAW_LAYER_TEXT);

    // Move the layer buffers
    move_size = (col2 - col1 + 1) * 4;
    க்கு (முழு l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_row->dlayers[l].buf;
	uint32_t *dest = dest_row->dlayers[l].buf;

	எனில் (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_OFFSET(dest_row->n_cells),
		    src + col1 + UNDERC_OFFSET(src_row->n_cells), move_size);
	    memmove(dest + col1 + STRIKE_OFFSET(dest_row->n_cells),
		    src + col1 + STRIKE_OFFSET(src_row->n_cells), move_size);
	}
	இல்லையெனில்
	    memmove(dest + col1, src + col1, move_size);
	draw_row_dirty_layer(dest_row, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    நிலையான வெறுமை
draw_row_render_background(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    எனில் (!dlayer->dirty)
	திரும்பு;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    எனில் (rgba_is_transparent(dlayer->buf, da->n_cols))
	திரும்பு;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->முனை = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    நிலையான வெறுமை
draw_row_ensure_decor(DrawRow *drow, முழு flags)
{
    எனில் (flags & DRAW_UNDERL && drow->underl_mask == வெற்று)
	drow->underl_mask = gsk_color_முனை_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->row + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    எனில் (flags & DRAW_STRIKE && drow->strike_mask == வெற்று)
	drow->strike_mask = gsk_color_முனை_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->row) + (முழு)(gui.char_height / 2),
		    FILL_X(drow->n_cells), 1));

    எனில் (flags & DRAW_UNDERC && drow->underc_mask == வெற்று)
    {
	நிலையான மாறிலி முழு val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	முழு y = FILL_Y(drow->row + 1) - 1;
	முழு x_start = FILL_X(0);
	முழு x_end = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_முனை *child = வெற்று;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#எனில் GTK_CHECK_VERSION(4, 14, 0)
	GskPathBuilder	*builder;
	GskPath		*path;
	GskStroke       *stroke;
	Gsk_Render_முனை	*color_முனை;
	graphene_rect_t bounds;

	builder = gsk_path_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_path_builder_move_to(builder, -1, y - val[7] + 0.5);
	gsk_path_builder_வரி_to(builder, 0, y - val[0] + 0.5);

	க்கு (முழு i = 1; i < 8; i++)
	    gsk_path_builder_வரி_to(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_path_builder_வரி_to(builder, 8, y - val[0] + 0.5);
	gsk_path_builder_வரி_to(builder, 9, y - val[1] + 0.5);

	path = gsk_path_builder_free_to_path(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_path_get_stroke_bounds(path, stroke, &bounds);
	color_முனை = gsk_color_முனை_new(&white_rgba, &bounds);
	child = gsk_stroke_முனை_new(color_முனை, path, stroke);

	gsk_stroke_free(stroke);
	gsk_path_unref(path);
	gsk_render_முனை_unref(color_முனை);
#இல்லையெனில்
	cairo_t *cr;

	child = gsk_cairo_முனை_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_முனை_get_draw_context(child);

	cairo_set_வரி_width(cr, 1.0);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_to(cr, -1, y - val[7] + 0.5);
	cairo_வரி_to(cr, 0, y - val[0] + 0.5);

	க்கு (முழு i = 1; i < 8; ++i)
	    cairo_வரி_to(cr, i, y - val[i] + 0.5);

	cairo_வரி_to(cr, 8, y - val[0] + 0.5);
	cairo_வரி_to(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#endif
	drow->underc_mask = gsk_repeat_முனை_new(
		&GRAPHENE_RECT_INIT(x_start, y - 3, x_end - x_start, 5),
		child, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_முனை_unref(child);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    நிலையான வெறுமை
draw_row_render_text(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_TEXT;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = வெற்று;
    முழு		start_col = 0;
    முழு		empty_cells = 0;
    uint32_t	cur_fg = 0;
    முழு		c;

    எனில் (!dlayer->dirty)
	திரும்பு;

    glyph_buf = da->glyph_buf;
    nodes = da->முனை_buf;

    g_array_set_size(glyph_buf, 0);
    g_ptr_array_set_size(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#define FLUSH_முனை() \
    செய் { \
	எனில் (cur_font != வெற்று) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_முனை	*முனை; \
	    GdkRGBA		fg_color = u32_to_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->data; \
	    glyph_str.இலக்கம்_glyphs = glyph_buf->len; \
	    முனை = gsk_text_முனை_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_POINT_INIT(TEXT_X(start_col), \
			TEXT_Y(drow->row))); \
	    எனில் (முனை != வெற்று) \
		g_ptr_array_add(nodes, முனை); \
	    g_array_set_size(glyph_buf, 0); \
	    cur_font = வெற்று; \
	} \
    } போது (FALSE)

    க்கு (c = 0; c < da->n_cols; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	முழு	    prev_len;

	எனில் (dglyphs == வெற்று)
	{
	    empty_cells++;
	    தொடர்;
	}
#எனில் defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	இல்லையெனில் எனில் (dglyphs->font == வெற்று)
	{
	    // Add sign icon
	    Gsk_Render_முனை   *snode;
	    முழு		    w;

	    FLUSH_முனை();

	    snode = gsk_texture_scale_முனை_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->row),
			dglyphs->dsign->width, dglyphs->dsign->height),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->width + gui.char_width - 1) / gui.char_width;

	    எனில் (snode != வெற்று)
		g_ptr_array_add(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    தொடர்;
	}
#endif
	இல்லையெனில் எனில் (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_முனை();
	    cur_font = dglyphs->font;
	    start_col = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	எனில் (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    தொடர்;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	எனில் (empty_cells > 0)
	{
	    எனில் (prev_len > 0)
	    {
		// Spaces occurred between valid text: extend
		// previous character's advance
		PangoGlyphInfo *last = &g_array_index(
			glyph_buf, PangoGlyphInfo,
			prev_len - 1);
		last->geometry.width += CELLS2PANGO(empty_cells);
	    }
	    empty_cells = 0; // Width consumed
	}
    }

    FLUSH_முனை();

#undef FLUSH_முனை

    எனில் (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_முனை *source;
	Gsk_Render_முனை *முனை;

	draw_row_ensure_decor(drow, DRAW_UNDERL);
	எனில் (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    source = draw_layer_get_texture(dlayer, drow->row, da, 0, FALSE);
	    முனை = gsk_mask_முனை_new(source, drow->underl_mask,
		    GSK_MASK_MODE_ALPHA);
	    gsk_render_முனை_unref(source);
	    g_ptr_array_add(nodes, முனை);
	}

	dlayer->முனை = gsk_container_முனை_new(
		(Gsk_Render_முனை **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    நிலையான வெறுமை
draw_row_render_special(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_முனை   *source;
    Gsk_Render_முனை   *nodes[2];
    முழு		    n_nodes = 0;

    எனில் (!dlayer->dirty)
	திரும்பு;

    draw_row_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    எனில் (!rgba_is_transparent(dlayer->buf + UNDERC_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		UNDERC_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_முனை_new(source, drow->underc_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_முனை_unref(source);
    }

    எனில் (!rgba_is_transparent(dlayer->buf + STRIKE_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		STRIKE_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_முனை_new(source, drow->strike_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_முனை_unref(source);
    }

    எனில் (n_nodes == 1)
    {
	dlayer->முனை = nodes[0];
    }
    இல்லையெனில் எனில் (n_nodes > 0)
    {
	dlayer->முனை = gsk_container_முனை_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	க்கு (முழு i = 0; i < (முழு)ARRAY_LENGTH(nodes); i++)
	    gsk_render_முனை_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    நிலையான வெறுமை
draw_row_render_overlay(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    எனில் (!dlayer->dirty)
	திரும்பு;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    எனில் (rgba_is_transparent(dlayer->buf, da->n_cols))
	திரும்பு;

    dlayer->முனை = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    நிலையான வெறுமை
draw_row_add_cluster(
	DrawRow		*drow,
	DrawAttr	*dattr,
	முழு		col,
	முழு		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	முழு		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = வெற்று;

    எனில் (col + n_cells > da->n_cols)
	திரும்பு;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    எனில் (glyphs != வெற்று)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_row_fill(drow, col, col + n_cells - 1, dglyphs, dattr, &da->cursor);

    எனில் (dglyphs != வெற்று)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    நிலையான முழு
vim_draw_area_add_string(
	VimDrawArea *self,
	DrawRow	    *drow,
	DrawAttr    *dattr,
	முழு	    col,
	char_u	    *s,
	முழு	    len,
	முழு	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = வெற்று;
    முழு			start_idx = -1;
    GList		*item_list;
    முழு			column_offset = 0;
    முழு			cluster_width;
    முழு			last_glyph_rbearing;
    முழு			cells = 0;
    முழு			i = 0;
    gboolean		has_unicode = FALSE;
    gboolean		needs_shaping = FALSE;

    // Scan the string, and find spans where ligatures should not happen. We
    // must ensure that attribute boundaries do not split a base character from
    // its combining marks, as Pango breaks items at attribute boundaries.
    போது (i < len)
    {
	முழு	clen = 1;
	முழு	is_composing = 0;
	bool	want_disable;

	எனில் (s[i] >= 0x80)
	{
	    clen = utf_ptr2len(s + i);
	    is_composing = utf_iscomposing(utf_ptr2char(s + i));
	    has_unicode = TRUE;
	}

	எனில் (is_composing)
	    // Do not change state mid-cluster. Inherit the previous character's
	    // state.
	    want_disable = (start_idx != -1);
	இல்லையெனில்
	    // Base character: disable ligatures if it's an ASCII non-ligature
	    // char.
	    want_disable = (s[i] < 0x80 && !gui.ligatures_map[s[i]]);

	// If any base character actually wants ligature/contextual shaping,
	// the string must go through Pango's shaping engine -- the ASCII
	// fast path below draws cached glyphs one-by-one and can never
	// produce a ligature, regardless of 'guiligatures'.
	எனில் (!is_composing && !want_disable)
	    needs_shaping = TRUE;

	எனில் (want_disable)
	{
	    எனில் (start_idx == -1)
		start_idx = i;
	}
	இல்லையெனில் எனில் (start_idx != -1)
	{
	    எனில் (attr_list == வெற்று)
		attr_list = pango_attr_list_new();
	    செருகு_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, start_idx, i);
	    start_idx = -1;
	}
	i += clen;
    }

    // Fast path for pure ASCII: use cached glyph table. Skip this path when
    // there are non-ascii characters in the string, font attributes, or if
    // there's a possible ligature.
    எனில் (!(draw_flags & DRAW_ITALIC)
	    && !((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != வெற்று
	    && !has_unicode
	    && !needs_shaping)
    {
	க்கு (i = 0; i < len; ++i)
	{
	    PangoGlyphInfo  glyph = gui.ascii_glyphs->glyphs[2 * s[i]];
	    gboolean	    has_ink;

	    has_ink = glyph_has_ink(gui.ascii_font, glyph);

	    draw_row_add_cluster(drow, dattr, col + i, 1,
		    gui.ascii_font, has_ink ? &glyph : வெற்று, 1, self);
	}

	column_offset = len;
	செல் exit;
    }

    glyphs = pango_glyph_string_new();

    எனில் (attr_list == வெற்று)
	attr_list = pango_attr_list_new();

    எனில் (start_idx != -1)
	செருகு_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, start_idx, len);

    cluster_width = PANGO_SCALE * gui.char_width;
    last_glyph_rbearing = PANGO_SCALE * gui.char_width;

    // If 'guifontwide' is set then use that for double-width characters.
    எனில் (gui.wide_font != வெற்று)
	apply_wide_font_attr(s, len, attr_list);

    எனில் ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	செருகு_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    எனில் (draw_flags & DRAW_ITALIC)
	செருகு_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    item_list = pango_itemize(gui.text_context,
	    (மாறிலி எழுத்து *)s, 0, len, attr_list, வெற்று);

    போது (item_list != வெற்று)
    {
	PangoItem   *item = item_list->data;
	முழு	    item_cells = 0;
	முழு	    cluster_start_idx = 0;
	முழு	    current_cluster_col = col + column_offset;
	முழு	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	item_list = g_list_delete_link(item_list, item_list);

	// Force LTR direction; Vim handles bidi on its own.
	item->analysis.level = (item->analysis.level + 1) & (~1U);

	pango_shape_full((மாறிலி எழுத்து *)s + item->offset, item->length,
		(மாறிலி எழுத்து *)s, len, &item->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	க்கு (i = 0; i < glyphs->இலக்கம்_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    எனில் (glyph->attr.is_cluster_start)
	    {
		முழு cellcount;

		cellcount = மொத்தம்_cluster_cells(
			s, item, glyphs, i, &cluster_width,
			(item_list != வெற்று) ? &last_glyph_rbearing : வெற்று);

		எனில் (i > 0 && cellcount > 0)
		{
		    draw_row_add_cluster(drow, dattr,
			    current_cluster_col, current_cluster_cells,
			    item->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_start_idx : வெற்று,
			    i - cluster_start_idx, self);

		    current_cluster_col += current_cluster_cells;
		    cluster_start_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		எனில் (cellcount > 0)
		{
		    முழு width;

		    width = cellcount * gui.char_width * PANGO_SCALE;
		    glyph->geometry.x_offset +=
			MAX(0, width - cluster_width) / 2;
		    glyph->geometry.width = width;
		}
		இல்லையெனில்
		    setup_zero_width_cluster(item, glyph, cells,
			    cluster_width,
			    last_glyph_rbearing);

		item_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    இல்லையெனில் எனில் (i > 0)
	    {
		முழு width;

		எனில் (glyph->geometry.x_offset >= 0)
		{
		    glyphs->glyphs[i].geometry.width =
			glyphs->glyphs[i - 1].geometry.width;
		    glyphs->glyphs[i - 1].geometry.width = 0;
		}
		width = cells * gui.char_width * PANGO_SCALE;
		glyph->geometry.x_offset +=
		    MAX(0, width - cluster_width) / 2;
	    }
	    இல்லையெனில்
		glyph->geometry.width = 0;


	    எனில் (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			item->analysis.font, *glyph);
	}
	எனில் (glyphs->இலக்கம்_glyphs > 0)
	    draw_row_add_cluster(drow, dattr,
		    current_cluster_col, current_cluster_cells,
		    item->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_start_idx : வெற்று,
		    glyphs->இலக்கம்_glyphs - cluster_start_idx, self);

	pango_item_free(item);
	column_offset += item_cells;
    }

    pango_glyph_string_free(glyphs);

exit:
    எனில் (attr_list != வெற்று)
	pango_attr_list_unref(attr_list);

    திரும்பு column_offset;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    முழு
vim_draw_area_draw_string(
	VimDrawArea *self,
	முழு	    row,
	முழு	    col,
	char_u	    *s,
	முழு	    len,
	முழு	    flags)
{
    DrawRow	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = வெற்று;
    முழு		convlen;
    முழு		n_cells ;

    எனில் (unlikely(self->rows == வெற்று
		|| row >= self->n_rows
		|| col >= self->n_cols))
	திரும்பு len;

    எனில் (output_conv.vc_type != CONV_NONE)
    {
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	எனில் (conv_buf != வெற்று)
	{
	    s = conv_buf;
	    len = convlen;
	}
    }

    // Safety check: pango crashes with invalid utf-8.
    எனில் (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	திரும்பு len;
    }

    drow = self->rows + row;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_add_string(self, drow, &dattr, col, s, len, flags);

    vim_free(conv_buf);

    திரும்பு n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    வெறுமை
vim_draw_area_clear_block(
	VimDrawArea	*self,
	முழு		row1,
	முழு		col1,
	முழு		row2,
	முழு		col2)
{
    எனில் (unlikely(self->rows == வெற்று
		|| row1 >= self->n_rows
		|| col1 >= self->n_cols
		|| row2 >= self->n_rows
		|| col2 >= self->n_cols))
	திரும்பு;

    க்கு (முழு r = row1; r <= row2; r++)
	draw_row_fill(self->rows + r, col1, col2, வெற்று, வெற்று, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    வெறுமை
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_block(self, 0, 0, self->n_rows - 1, self->n_cols - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    வெறுமை
vim_draw_area_move_block(
	VimDrawArea *self,
	முழு	    to,
	முழு	    row1,
	முழு	    row2,
	முழு	    col1,
	முழு	    col2)
{
    முழு offset = row2 - row1;

    எனில் (unlikely(self->rows == வெற்று
		|| row1 >= self->n_rows
		|| row2 >= self->n_rows
		|| to >= self->n_rows
		|| col1 >= self->n_cols
		|| col2 >= self->n_cols))
	திரும்பு;

    ப்ரதிஜ்ஞா(row2 >= row1);
    ப்ரதிஜ்ஞா(col2 >= col1);
    ப்ரதிஜ்ஞா(row1 != to);

    எனில் (row1 > to)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	க்கு (முழு o = 0; o <= offset; o++)
	    draw_row_move_to(self->rows + to + o, self->rows + row1 + o,
		    col1, col2, self);
    }
    இல்லையெனில்
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	க்கு (முழு o = offset; o >= 0; o--)
	    எனில் (to + o >= self->n_rows)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_block(self, row1 + o, col1, row1 + o, col2);
	    இல்லையெனில்
		draw_row_move_to(self->rows + to + o, self->rows + row1 + o,
			col1, col2, self);
    }
}

/*
 * Set the cursor to the given width and height, use "gui.bgcolor" and
 * "gui.fgcolor" for the colors as well. If "w" and "h" is zero, then a full
 * block cursor is used, if "w" and "h" is -1 then a hollow cursor is used,
 * otherwise a part cursor with dimensions "w" and "h" is used.
 */
    வெறுமை
vim_draw_area_set_cursor(VimDrawArea *self, முழு w, முழு h)
{
    self->cursor.draw = TRUE;
    self->cursor.width = w;
    self->cursor.height = h;
    // Remember this now: the snapshot runs later, when the drawing position
    // has moved on.
    self->cursor.n_cells = 1 + mb_lefthalve(gui.cursor_row, gui.cursor_col);
    self->cursor.bg_color = *gui.bgcolor;
    self->cursor.fg_color = *gui.fgcolor;
}

/*
 * Invert the rectangle in the draw area.
 */
    வெறுமை
vim_draw_area_invert_block(
	VimDrawArea	*self,
	முழு		row,
	முழு		col,
	முழு		nrows,
	முழு		ncols)
{
    எனில் (unlikely(self->rows == வெற்று
		|| row >= self->n_rows
		|| col >= self->n_cols
		|| row + nrows - 1 >= self->n_rows
		|| col + ncols - 1 >= self->n_cols))
	திரும்பு;

    க்கு (முழு r = row; r < row + nrows; r++)
    {
	DrawRow	    *drow = self->rows + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	க்கு (முழு c = 0; c < ncols; c++)
	    dlayer->buf[c + col] = ~dlayer->buf[c + col];
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#எனில் defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    வெறுமை
vim_draw_area_add_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	முழு	    row,
	முழு	    col,
	முழு	    width,
	முழு	    height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    முழு		cells;

    எனில் (unlikely(self->rows == வெற்று
		|| row >= self->n_rows
		|| col >= self->n_cols))
	திரும்பு;

    drow = self->rows + row;
    dsign = draw_sign_new(sign, width, height);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (width + gui.char_width - 1) / gui.char_width;

    draw_row_fill(drow, col, col + cells - 1, dglyphs, வெற்று, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    வெறுமை
vim_draw_area_add_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	முழு		row,
	முழு		col,
	முழு		width,
	முழு		height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    முழு		cells;
    GdkTexture	*texture;
    GBytes	*bytes;

    எனில் (unlikely(self->rows == வெற்று
		|| row >= self->n_rows
		|| col >= self->n_cols))
	திரும்பு;

    cairo_surface_flush(surf);
    bytes = g_bytes_new_with_free_func(cairo_image_surface_get_data(surf),
	    cairo_image_surface_get_height(surf)
	    * cairo_image_surface_get_stride(surf),
	    (GDestroyNotify)cairo_surface_destroy,
	    cairo_surface_reference(surf));

    texture = gdk_memory_texture_new(cairo_image_surface_get_width(surf),
	    cairo_image_surface_get_height(surf),
	    GDK_MEMORY_A8R8G8B8_PREMULTIPLIED,
	    bytes, cairo_image_surface_get_stride(surf));
    g_bytes_unref(bytes);

    drow = self->rows + row;
    dsign = draw_sign_new(texture, width, height);
    g_object_unref(texture);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (width + gui.char_width - 1) / gui.char_width;

    draw_row_fill(drow, col, col + cells - 1, dglyphs, வெற்று, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    வெறுமை
vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_முனை *முனை)
{
    g_hash_table_add(self->external, gsk_render_முனை_ref(முனை));
}

    வெறுமை
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_முனை *முனை)
{
    g_hash_table_remove(self->external, முனை);
}

#endif

/*
 * Add the cursor to the snapshot.
 */
    நிலையான வெறுமை
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_முனை	*text_முனை,	// May be NULL
	Gsk_Render_முனை	*special_முனை,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    முழு		    w = cursor->width;
    முழு		    h = cursor->height;
    graphene_rect_t rect;

    எனில் (!cursor->draw)
	திரும்பு;

    எனில் (cursor->width <= 0 && cursor->height <= 0)
    {
	w += gui.char_width * cursor->n_cells;
	h = gui.char_height;
    }

    graphene_rect_init(&rect,
#ifdef FEAT_RIGHTLEFT
	    CURSOR_BAR_வலது ? FILL_X(gui.cursor_col + 1) - w :
#endif
	    FILL_X(gui.cursor_col),
	    FILL_Y(gui.cursor_row) + gui.char_height - h, w, h);


    எனில் (cursor->width == -1 && cursor->height == -1)
    {
	// Create hollow cursor
	GskRoundedRect	    outline;
	நிலையான மாறிலி தசம  border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	மாறிலி GdkRGBA	    color[4] = {
	    cursor->bg_color, cursor->bg_color,
	    cursor->bg_color, cursor->bg_color
	} ;

	gsk_rounded_rect_init_from_rect(&outline, &rect, 0.0f);
	gtk_snapshot_append_border(snapshot, &outline, border, color);
    }
    இல்லையெனில்
    {
	// Create block style cursor, by masking the text + under decorations
	// with the fg color.
	gtk_snapshot_append_color(snapshot, &cursor->bg_color, &rect);

	gtk_snapshot_தள்ளு_mask(snapshot, GSK_MASK_MODE_ALPHA);

	// Used as an "outline" for the fg color.
	எனில் (text_முனை != வெற்று)
	    gtk_snapshot_append_முனை(snapshot, text_முனை);
	எனில் (special_முனை != வெற்று)
	    gtk_snapshot_append_முனை(snapshot, special_முனை);

	gtk_snapshot_எடு(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_எடு(snapshot);
    }
}

    நிலையான வெறுமை
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    முழு		    height, width;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = வெற்று;
    Gsk_Render_முனை   *body_முனை;
    Gsk_Render_முனை   *invert_முனை = வெற்று;
#ifdef FEAT_IMAGE_GUI
    Gsk_Render_முனை   *ext_முனை;
    GHashTableIter  iter;
#endif

    gui_mch_set_bg_color(gui.back_pixel);
    height = gtk_widget_get_height(widget) + gui.bleed_bot;
    width = gtk_widget_get_width(widget) + gui.bleed_வலது;

    எனில் (self->rows == வெற்று)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, width, height));
	திரும்பு;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    எனில் (self->bleed_வலது == -1 || self->bleed_வலது != gui.bleed_வலது)
    {
	self->bleed_வலது = gui.bleed_வலது;
	க்கு (முழு r = 0; r < self->n_rows; r++)
	{
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_BACKGROUND);
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#எனில் GTK_CHECK_VERSION(4,24,0)
    gtk_snapshot_set_snap(body_snapshot, GSK_RECT_SNAP_ROUND);
#endif

    க்கு (முழு r = 0; r < self->n_rows; r++)
    {
	DrawRow *drow = self->rows + r;

	draw_row_render_background(drow, self);
	draw_row_render_text(drow, self);
	draw_row_render_special(drow, self);
	draw_row_render_overlay(drow, self);

	க்கு (முழு l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    எனில் (dlayer->முனை != வெற்று)
	    {
		எனில் (l == DRAW_LAYER_OVERLAY)
		{
		    எனில் (invert_snapshot == வெற்று)
		    {
			invert_snapshot = gtk_snapshot_new();
#எனில் GTK_CHECK_VERSION(4,24,0)
			gtk_snapshot_set_snap(invert_snapshot,
				GSK_RECT_SNAP_ROUND);
#endif
		    }
		    gtk_snapshot_append_முனை(invert_snapshot, dlayer->முனை);
		}
		இல்லையெனில்
		    gtk_snapshot_append_முனை(body_snapshot, dlayer->முனை);
	    }
	}

	எனில் (r == gui.cursor_row)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_TEXT].முனை,
		    drow->dlayers[DRAW_LAYER_SPECIAL].முனை,
		    body_snapshot);
    }

    எனில் (invert_snapshot != வெற்று)
	gtk_snapshot_தள்ளு_blend(snapshot, GSK_BLEND_MODE_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, width, height));
    body_முனை = gtk_snapshot_free_to_முனை(body_snapshot);
    எனில் (body_முனை != வெற்று)
    {
	gtk_snapshot_append_முனை(snapshot, body_முனை);
	gsk_render_முனை_unref(body_முனை);
    }

#ifdef FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_table_iter_init(&iter, self->external);

    போது (g_hash_table_iter_next(&iter, (வெறுமை **)&ext_முனை, வெற்று))
	gtk_snapshot_append_முனை(snapshot, ext_முனை);
#endif

    எனில் (invert_snapshot != வெற்று)
    {
	gtk_snapshot_எடு(snapshot);
	invert_முனை = gtk_snapshot_free_to_முனை(invert_snapshot);
	// Not sure if it can be NULL but still check
	எனில் (invert_முனை != வெற்று)
	{
	    gtk_snapshot_append_முனை(snapshot, invert_முனை);
	    gsk_render_முனை_unref(invert_முனை);
	}
	gtk_snapshot_எடு(snapshot);
    }
}
