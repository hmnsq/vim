/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#અન્તર્ભાવ "vim.h"

#અન્તર્ભાવ <gtk/gtk.h>
#અન્તર્ભાવ "gui_gtk4_da.h"


#જો defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Used for sign icons and netbeans multisign indicator. It is in a DrawGlyphs
 * so that it can move/be manipulated with the text.
 */
પ્રકારવ્યાખ્યા સંરચના
{
    GdkTexture	*texture;
    પૂર્ણાંક		width;
    પૂર્ણાંક		height;
} DrawSign;
#endif

/*
 * Temporary struct when drawing string representing visual attributes of a cell
 */
પ્રકારવ્યાખ્યા સંરચના
{
    uint32_t	bg_color;
    uint32_t	fg_color;   // Only used for underlines
    uint32_t	sp_color;
    પૂર્ણાંક		draw_flags; // DRAW_* flags, only DRAW_UNDERL, DRAW_UNDERC, and
			    // DRAW_STRIKE are used.
} DrawAttr;

/*
 * Represents the glyphs of a single cell.
 */
પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક refcount;

    uint32_t	fg_color;
    PangoFont	*font;
    // If "font" is NULL, "dsign" is used, otherwise "glyphs" is used.
    સંઘ
    {
	PangoGlyphInfo  *glyphs; // May be NULL
#જો defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	DrawSign	*dsign;
#endif
    };
    પૂર્ણાંક n_glyphs;
} DrawGlyphs;

પ્રકારવ્યાખ્યા સંરચના
{
    DrawGlyphs *dglyphs;
} DrawCell;

પ્રકારવ્યાખ્યા ગણના
{
    DRAW_LAYER_BACKGROUND,  // Background color, buf stores bg color
    DRAW_LAYER_TEXT,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

પ્રકારવ્યાખ્યા સંરચના
{
    Gsk_Render_ગાંઠ   *ગાંઠ;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#define UNDERC_OFFSET(n_cols) (0)
#define STRIKE_OFFSET(n_cols) (n_cols)

પ્રકારવ્યાખ્યા સંરચના
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    પૂર્ણાંક		n_cells; // For convenience
    પૂર્ણાંક		row;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_ગાંઠ *underl_mask;
    Gsk_Render_ગાંઠ *underc_mask;
    Gsk_Render_ગાંઠ *strike_mask;
} DrawRow;

#define ઉમેરો_PANGO_ATTR(Attribute, AttrList, Start, End) \
    કરો { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->start_index = (Start); \
	tmp_attr_->end_index = (End); \
	pango_attr_list_ઉમેરો((AttrList), tmp_attr_); \
    } જ્યારે (FALSE)
#define CELLS2PANGO(c) ((c) * gui.char_width * PANGO_SCALE)

/*
 * Attributes of cursor
 */
પ્રકારવ્યાખ્યા સંરચના
{
    gboolean draw; // If cursor should be drawn
    પૂર્ણાંક width;
    પૂર્ણાંક height;
    પૂર્ણાંક n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

સંરચના _VimDrawArea
{
    GtkWidget parent;

    DrawRow *rows;

    પૂર્ણાંક n_rows;
    પૂર્ણાંક	n_cols;

    પૂર્ણાંક	bleed_જમણું;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*ગાંઠ_buf;

    DrawCursor cursor;

#ifdef FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHashTable *external;
#endif
};

સ્થિર શૂન્ય draw_row_init(DrawRow *drow, પૂર્ણાંક row, પૂર્ણાંક cols);
સ્થિર શૂન્ય draw_row_clear(DrawRow *drow);
સ્થિર શૂન્ય draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t);
સ્થિર શૂન્ય vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

સ્થિર GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    સ્થિર શૂન્ય
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    માટે (પૂર્ણાંક r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    g_free(self->rows);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->ગાંઠ_buf, TRUE);

#ifdef FEAT_IMAGE_GUI
    g_hash_table_unref(self->external);
#endif

    G_OBJECT_CLASS(vim_draw_area_parent_class)->finalize(obj);
}

    સ્થિર શૂન્ય
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    સ્થિર શૂન્ય
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_જમણું = -1;
#ifdef FEAT_IMAGE_GUI
    self->external = g_hash_table_new_full(g_direct_hash, g_direct_equal,
	    ખાલી, (GDestroyNotify)gsk_render_ગાંઠ_unref);
#endif
    self->glyph_buf = g_array_new(FALSE, FALSE, કદ(PangoGlyphInfo));
    self->ગાંઠ_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_ગાંઠ_unref);

}

    GtkWidget *
vim_draw_area_new(શૂન્ય)
{
    પાછા g_object_new(VIM_TYPE_DRAW_AREA, ખાલી);
}

/*
 * Set the size of the draw area to "rows" and "cols".
 */
    શૂન્ય
vim_draw_area_set_size(VimDrawArea *self, પૂર્ણાંક rows, પૂર્ણાંક cols)
{
    જો (self->rows != ખાલી && self->n_rows == rows && self->n_cols == cols)
	પાછા;
    જો (rows == 0 || cols == 0)
	પાછા;

    vim_draw_area_clear(self);

    માટે (પૂર્ણાંક r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    self->rows = g_realloc_n(self->rows, rows, કદ(DrawRow));
    માટે (પૂર્ણાંક r = 0; r < rows; r++)
	draw_row_init(self->rows + r, r, cols);

    self->n_rows = rows;
    self->n_cols = cols;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    સ્થિર uint32_t
rgba_to_u32(અચલ GdkRGBA *rgba)
{
    સંઘ { uint32_t val; uint8_t bytes[4]; } color;

    color.bytes[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.bytes[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.bytes[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.bytes[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    પાછા color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    સ્થિર GdkRGBA
u32_to_rgba(uint32_t val)
{
    સંઘ { uint32_t val; uint8_t bytes[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.bytes[0] / 255.0;
    color.green = packed.bytes[1] / 255.0;
    color.blue  = packed.bytes[2] / 255.0;
    color.alpha = packed.bytes[3] / 255.0;
    પાછા color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    સ્થિર gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    સંઘ { uint32_t val; uint8_t bytes[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.bytes[0] << 16) |
	((guicolor_T)packed.bytes[1] << 8)  |
	packed.bytes[2];
    પાછા res == rgb;
}

/*
 * Checks if a RGBA pixel buffer is fully transparent.
 */
    સ્થિર અંતઃરેખ gboolean
rgba_is_transparent(અચલ uint32_t *buf, પૂર્ણાંક n_pixels)
{
    // Could also compare 2 pixels at once using a 64 bit integer, but not sure
    // if that might cause alignment issues...
    માટે (પૂર્ણાંક i = 0; i < n_pixels; i++)
	જો (buf[i] != 0)
	    પાછા FALSE;
    પાછા TRUE;
}

/*
 * Return TRUE if "glyph" has ink (not whitespace).
 */
    સ્થિર gboolean
glyph_has_ink(PangoFont *font, PangoGlyphInfo glyph)
{
    PangoRectangle ink;

    pango_font_get_glyph_extents (font, glyph.glyph, &ink, ખાલી);

    પાછા ink.width > 0 && ink.height > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    સ્થિર શૂન્ય
apply_wide_font_attr(char_u *s, પૂર્ણાંક len, PangoAttrList *attr_list)
{
    char_u  *start = ખાલી;
    char_u  *p;
    પૂર્ણાંક	    uc;

    માટે (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	જો (start == ખાલી)
	{
	    જો (uc >= 0x80 && utf_char2cells(uc) == 2)
		start = p;
	}
	નહીંતર જો (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    ઉમેરો_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, start - s, p - s);
	    start = ખાલી;
	}
    }

    જો (start != ખાલી)
	ઉમેરો_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, start - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    સ્થિર પૂર્ણાંક
ગણતરી_cluster_cells(
	char_u		    *s,
	PangoItem	    *item,
	PangoGlyphString    *glyphs,
	પૂર્ણાંક		    i,
	પૂર્ણાંક		    *cluster_width,
	પૂર્ણાંક		    *last_glyph_rbearing)
{
    char_u  *p;
    પૂર્ણાંક	    next;
    પૂર્ણાંક	    start, end;
    પૂર્ણાંક	    width;
    પૂર્ણાંક	    uc;
    પૂર્ણાંક	    cellcount = 0;

    width = glyphs->glyphs[i].geometry.width;

    માટે (next = i + 1; next < glyphs->અંક_glyphs; ++next)
    {
	જો (glyphs->glyphs[next].attr.is_cluster_start)
	    તોડો;
	નહીંતર જો (glyphs->glyphs[next].geometry.width > width)
	    width = glyphs->glyphs[next].geometry.width;
    }

    start = item->offset + glyphs->log_clusters[i];
    end   = item->offset + ((next < glyphs->અંક_glyphs) ?
	    glyphs->log_clusters[next] : item->length);

    માટે (p = s + start; p < s + end; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	જો (uc < 0x80)
	    ++cellcount;
	નહીંતર જો (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    જો (last_glyph_rbearing != ખાલી
	    && cellcount > 0 && next == glyphs->અંક_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(item->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, ખાલી);

	જો (PANGO_RBEARING(ink_rect) > 0)
	    *last_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    જો (cellcount > 0)
	*cluster_width = width;

    પાછા cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    સ્થિર શૂન્ય
setup_zero_width_cluster(
	PangoItem	*item,
	PangoGlyphInfo	*glyph,
	પૂર્ણાંક		last_cellcount,
	પૂર્ણાંક		last_cluster_width,
	પૂર્ણાંક		last_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    પૂર્ણાંક		    width;

    width = last_cellcount * gui.char_width * PANGO_SCALE;
    glyph->geometry.x_offset = -width + MAX(0, width - last_cluster_width) / 2;
    glyph->geometry.width = 0;

    pango_font_get_glyph_extents(item->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    જો (ink_rect.x < 0)
    {
	glyph->geometry.x_offset += last_glyph_rbearing;
	glyph->geometry.y_offset  = logical_rect.height
	    - (gui.char_height - p_linespace) * PANGO_SCALE;
    }
    નહીંતર
	glyph->geometry.x_offset = -width + MAX(0, width - ink_rect.width) / 2;
}

#જો defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    સ્થિર DrawSign *
draw_sign_new(GdkTexture *texture, પૂર્ણાંક width, પૂર્ણાંક height)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->width = width;
    dsign->height = height;

    પાછા dsign;
}

    સ્થિર શૂન્ય
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#endif

    સ્થિર શૂન્ય
draw_attr_init(
	DrawAttr	*dattr,
	અચલ GdkRGBA	*bg_color,
	અચલ GdkRGBA	*fg_color,
	અચલ GdkRGBA	*sp_color,
	પૂર્ણાંક		draw_flags)
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
    સ્થિર શૂન્ય
draw_attr_apply(DrawAttr *dattr, પૂર્ણાંક col, DrawRow *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + col;
    uint32_t	*text_cell = drow->dlayers[DRAW_LAYER_TEXT].buf + col;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    પૂર્ણાંક		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + UNDERC_OFFSET(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + STRIKE_OFFSET(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    જો (dattr != ખાલી)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.back_pixel, bg_color);
    }

    જો (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	જો (bg_is_default)
	    *bg_cell = 0;
	નહીંતર
	    *bg_cell = bg_color;
    }
    જો ((draw_flags & DRAW_UNDERL && *text_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *text_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);
	જો (draw_flags & DRAW_UNDERL)
	    *text_cell = fg_color;
	નહીંતર
	    *text_cell = 0;
    }
    જો ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	જો (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	નહીંતર
	    *underc_cell = 0;
    }
    જો ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	જો (draw_flags & DRAW_STRIKE)
	    *strike_cell = sp_color;
	નહીંતર
	    *strike_cell = 0;
    }
}

    સ્થિર DrawGlyphs *
draw_glyphs_new(
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	પૂર્ણાંક		n_glyphs,
	uint32_t	fg_color)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->font = g_object_ref(font);
    dglyphs->glyphs = g_memdup2(glyphs, કદ(*glyphs) * n_glyphs);
    dglyphs->n_glyphs = n_glyphs;
    dglyphs->fg_color = fg_color;

    પાછા dglyphs;
}

#જો defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw glyphs that shows the given sign. Note that ownership of
 * "dsign" is taken.
 */
    સ્થિર DrawGlyphs *
draw_glyphs_new_sign(DrawSign *dsign)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->dsign = dsign;

    પાછા dglyphs;
}
#endif

    સ્થિર DrawGlyphs *
draw_glyphs_ref(DrawGlyphs *dglyphs)
{
    dglyphs->refcount++;
    પાછા dglyphs;
}

    સ્થિર શૂન્ય
draw_glyphs_unref(DrawGlyphs *dglyphs)
{
    જો (--dglyphs->refcount <= 0)
    {
#જો defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	જો (dglyphs->font == ખાલી)
	    draw_sign_free(dglyphs->dsign);
	નહીંતર
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
    સ્થિર gboolean
draw_glyphs_same(DrawGlyphs *a, DrawGlyphs *b)
{
    જો (a == ખાલી && b == ખાલી)
	પાછા TRUE;
    જો (a == ખાલી || b == ખાલી)
	પાછા FALSE;
    જો (a->fg_color != b->fg_color)
	પાછા FALSE;
    જો (a->n_glyphs != b->n_glyphs || a->font != b->font)
	પાછા FALSE;
    માટે (પૂર્ણાંક i = 0; i < a->n_glyphs; i++)
	જો (a->glyphs[i].glyph != b->glyphs[i].glyph
		|| a->glyphs[i].geometry.width != b->glyphs[i].geometry.width
		|| a->glyphs[i].geometry.x_offset != b->glyphs[i].geometry.x_offset
		|| a->glyphs[i].geometry.y_offset != b->glyphs[i].geometry.y_offset
		|| a->glyphs[i].attr.is_cluster_start != b->glyphs[i].attr.is_cluster_start)
	    પાછા FALSE;
    પાછા TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    સ્થિર Gsk_Render_ગાંઠ *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	પૂર્ણાંક	    row,
	VimDrawArea *da,
	પૂર્ણાંક	    off,
	gboolean    bleed)
{
    GBytes	    *bytes;
    GdkTexture	    *texture;
    Gsk_Render_ગાંઠ   *ગાંઠ;
    પૂર્ણાંક		    height;

#જો GTK_CHECK_VERSION(4,24,0)
    height = gui.char_height;
#નહીંતર
    height = gui.char_height + 1;
#endif

    જો (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		કદ(uint32_t) * da->n_cols);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_cols - 1)),
		કદ(uint32_t)); // Add bleed using last pixel of buffer
	bytes = g_byte_array_free_to_bytes(arr);
    }
    નહીંતર
	bytes = g_bytes_new(dlayer->buf + off, કદ(uint32_t) * da->n_cols);

    texture = gdk_memory_texture_new(da->n_cols + bleed, 1,
	    GDK_MEMORY_R8G8B8A8, bytes, (da->n_cols + bleed) * 4);
    g_bytes_unref(bytes);

    // Scale texture to actual size
    ગાંઠ = gsk_texture_scale_ગાંઠ_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		(da->n_cols + bleed) * gui.char_width, gui.char_height + bleed),
	    GSK_SCALING_FILTER_NEAREST);
    જો (bleed)
    {
	Gsk_Render_ગાંઠ *new;

	new = gsk_clip_ગાંઠ_new(ગાંઠ,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		    da->n_cols * gui.char_width + da->bleed_જમણું, height));
	gsk_render_ગાંઠ_unref(ગાંઠ);
	ગાંઠ = new;
    }
    g_object_unref(texture);
    પાછા ગાંઠ;
}

    સ્થિર શૂન્ય
draw_row_init(DrawRow *drow, પૂર્ણાંક row, પૂર્ણાંક cols)
{
    drow->cells = g_malloc0_n(cols, કદ(DrawCell));
    drow->n_cells = cols;
    drow->row = row;
    drow->underl_mask = drow->underc_mask = drow->strike_mask = ખાલી;

    memset(drow->dlayers, 0, કદ(DrawLayer) * N_DRAW_LAYERS);
    માટે (પૂર્ણાંક l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	જો (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(cols * 2, 4);
	નહીંતર
	    dlayer->buf = g_malloc0_n(cols, 4);
    }
}

    સ્થિર શૂન્ય
draw_row_clear(DrawRow *drow)
{
    g_free(drow->cells);
    માટે (પૂર્ણાંક l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	જો (dlayer->ગાંઠ != ખાલી)
	    gsk_render_ગાંઠ_unref(dlayer->ગાંઠ);
	g_free(dlayer->buf);
    }
    g_clear_pointer(&drow->underl_mask, gsk_render_ગાંઠ_unref);
    g_clear_pointer(&drow->underc_mask, gsk_render_ગાંઠ_unref);
    g_clear_pointer(&drow->strike_mask, gsk_render_ગાંઠ_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    સ્થિર શૂન્ય
draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_pointer(&dlayer->ગાંઠ, gsk_render_ગાંઠ_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    સ્થિર શૂન્ય
draw_row_uninvert_cell(DrawRow *drow, પૂર્ણાંક col)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    જો (buf[col] == 0xffffffff)
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[col] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    સ્થિર શૂન્ય
draw_row_set_cell(DrawRow *drow, પૂર્ણાંક col, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + col;

    જો (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    જો (dcell->dglyphs != dglyphs)
    {
	જો (dcell->dglyphs != ખાલી)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == ખાલી ? ખાલી : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, col, drow);
    draw_row_uninvert_cell(drow, col);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    સ્થિર શૂન્ય
draw_row_fill(
	DrawRow	    *drow,
	પૂર્ણાંક	    col1,
	પૂર્ણાંક	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    માટે (પૂર્ણાંક c = col1; c <= col2; c++)
	draw_row_set_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    જો (gui.cursor_row == drow->row && gui.cursor_col >= col1
	    && gui.cursor_col <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    સ્થિર શૂન્ય
draw_row_move_to(
	DrawRow	    *dest_row,
	DrawRow	    *src_row,
	પૂર્ણાંક	    col1,
	પૂર્ણાંક	    col2,
	VimDrawArea *da)
{
    પૂર્ણાંક move_size = (col2 - col1 + 1) * કદ(DrawCell);

    draw_row_fill(dest_row, col1, col2, ખાલી, ખાલી, &da->cursor);
    memmove(dest_row->cells + col1, src_row->cells + col1, move_size);
    // NULL the draw cells so we don't double unreference.
    memset(src_row->cells + col1, 0, move_size);
    draw_row_dirty_layer(src_row, DRAW_LAYER_TEXT);

    // Move the layer buffers
    move_size = (col2 - col1 + 1) * 4;
    માટે (પૂર્ણાંક l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_row->dlayers[l].buf;
	uint32_t *dest = dest_row->dlayers[l].buf;

	જો (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_OFFSET(dest_row->n_cells),
		    src + col1 + UNDERC_OFFSET(src_row->n_cells), move_size);
	    memmove(dest + col1 + STRIKE_OFFSET(dest_row->n_cells),
		    src + col1 + STRIKE_OFFSET(src_row->n_cells), move_size);
	}
	નહીંતર
	    memmove(dest + col1, src + col1, move_size);
	draw_row_dirty_layer(dest_row, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    સ્થિર શૂન્ય
draw_row_render_background(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    જો (!dlayer->dirty)
	પાછા;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    જો (rgba_is_transparent(dlayer->buf, da->n_cols))
	પાછા;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->ગાંઠ = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    સ્થિર શૂન્ય
draw_row_ensure_decor(DrawRow *drow, પૂર્ણાંક flags)
{
    જો (flags & DRAW_UNDERL && drow->underl_mask == ખાલી)
	drow->underl_mask = gsk_color_ગાંઠ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->row + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    જો (flags & DRAW_STRIKE && drow->strike_mask == ખાલી)
	drow->strike_mask = gsk_color_ગાંઠ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->row) + (પૂર્ણાંક)(gui.char_height / 2),
		    FILL_X(drow->n_cells), 1));

    જો (flags & DRAW_UNDERC && drow->underc_mask == ખાલી)
    {
	સ્થિર અચલ પૂર્ણાંક val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	પૂર્ણાંક y = FILL_Y(drow->row + 1) - 1;
	પૂર્ણાંક x_start = FILL_X(0);
	પૂર્ણાંક x_end = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_ગાંઠ *child = ખાલી;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#જો GTK_CHECK_VERSION(4, 14, 0)
	GskPathBuilder	*builder;
	GskPath		*path;
	GskStroke       *stroke;
	Gsk_Render_ગાંઠ	*color_ગાંઠ;
	graphene_rect_t bounds;

	builder = gsk_path_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_path_builder_move_to(builder, -1, y - val[7] + 0.5);
	gsk_path_builder_લીટી_to(builder, 0, y - val[0] + 0.5);

	માટે (પૂર્ણાંક i = 1; i < 8; i++)
	    gsk_path_builder_લીટી_to(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_path_builder_લીટી_to(builder, 8, y - val[0] + 0.5);
	gsk_path_builder_લીટી_to(builder, 9, y - val[1] + 0.5);

	path = gsk_path_builder_free_to_path(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_path_get_stroke_bounds(path, stroke, &bounds);
	color_ગાંઠ = gsk_color_ગાંઠ_new(&white_rgba, &bounds);
	child = gsk_stroke_ગાંઠ_new(color_ગાંઠ, path, stroke);

	gsk_stroke_free(stroke);
	gsk_path_unref(path);
	gsk_render_ગાંઠ_unref(color_ગાંઠ);
#નહીંતર
	cairo_t *cr;

	child = gsk_cairo_ગાંઠ_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_ગાંઠ_get_draw_context(child);

	cairo_set_લીટી_width(cr, 1.0);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_to(cr, -1, y - val[7] + 0.5);
	cairo_લીટી_to(cr, 0, y - val[0] + 0.5);

	માટે (પૂર્ણાંક i = 1; i < 8; ++i)
	    cairo_લીટી_to(cr, i, y - val[i] + 0.5);

	cairo_લીટી_to(cr, 8, y - val[0] + 0.5);
	cairo_લીટી_to(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#endif
	drow->underc_mask = gsk_repeat_ગાંઠ_new(
		&GRAPHENE_RECT_INIT(x_start, y - 3, x_end - x_start, 5),
		child, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_ગાંઠ_unref(child);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    સ્થિર શૂન્ય
draw_row_render_text(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_TEXT;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = ખાલી;
    પૂર્ણાંક		start_col = 0;
    પૂર્ણાંક		empty_cells = 0;
    uint32_t	cur_fg = 0;
    પૂર્ણાંક		c;

    જો (!dlayer->dirty)
	પાછા;

    glyph_buf = da->glyph_buf;
    nodes = da->ગાંઠ_buf;

    g_array_set_size(glyph_buf, 0);
    g_ptr_array_set_size(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#define FLUSH_ગાંઠ() \
    કરો { \
	જો (cur_font != ખાલી) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_ગાંઠ	*ગાંઠ; \
	    GdkRGBA		fg_color = u32_to_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->data; \
	    glyph_str.અંક_glyphs = glyph_buf->len; \
	    ગાંઠ = gsk_text_ગાંઠ_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_POINT_INIT(TEXT_X(start_col), \
			TEXT_Y(drow->row))); \
	    જો (ગાંઠ != ખાલી) \
		g_ptr_array_add(nodes, ગાંઠ); \
	    g_array_set_size(glyph_buf, 0); \
	    cur_font = ખાલી; \
	} \
    } જ્યારે (FALSE)

    માટે (c = 0; c < da->n_cols; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	પૂર્ણાંક	    prev_len;

	જો (dglyphs == ખાલી)
	{
	    empty_cells++;
	    ચાલુ;
	}
#જો defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	નહીંતર જો (dglyphs->font == ખાલી)
	{
	    // Add sign icon
	    Gsk_Render_ગાંઠ   *snode;
	    પૂર્ણાંક		    w;

	    FLUSH_ગાંઠ();

	    snode = gsk_texture_scale_ગાંઠ_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->row),
			dglyphs->dsign->width, dglyphs->dsign->height),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->width + gui.char_width - 1) / gui.char_width;

	    જો (snode != ખાલી)
		g_ptr_array_add(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    ચાલુ;
	}
#endif
	નહીંતર જો (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_ગાંઠ();
	    cur_font = dglyphs->font;
	    start_col = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	જો (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    ચાલુ;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	જો (empty_cells > 0)
	{
	    જો (prev_len > 0)
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

    FLUSH_ગાંઠ();

#undef FLUSH_ગાંઠ

    જો (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_ગાંઠ *source;
	Gsk_Render_ગાંઠ *ગાંઠ;

	draw_row_ensure_decor(drow, DRAW_UNDERL);
	જો (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    source = draw_layer_get_texture(dlayer, drow->row, da, 0, FALSE);
	    ગાંઠ = gsk_mask_ગાંઠ_new(source, drow->underl_mask,
		    GSK_MASK_MODE_ALPHA);
	    gsk_render_ગાંઠ_unref(source);
	    g_ptr_array_add(nodes, ગાંઠ);
	}

	dlayer->ગાંઠ = gsk_container_ગાંઠ_new(
		(Gsk_Render_ગાંઠ **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    સ્થિર શૂન્ય
draw_row_render_special(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_ગાંઠ   *source;
    Gsk_Render_ગાંઠ   *nodes[2];
    પૂર્ણાંક		    n_nodes = 0;

    જો (!dlayer->dirty)
	પાછા;

    draw_row_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    જો (!rgba_is_transparent(dlayer->buf + UNDERC_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		UNDERC_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_ગાંઠ_new(source, drow->underc_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_ગાંઠ_unref(source);
    }

    જો (!rgba_is_transparent(dlayer->buf + STRIKE_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		STRIKE_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_ગાંઠ_new(source, drow->strike_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_ગાંઠ_unref(source);
    }

    જો (n_nodes == 1)
    {
	dlayer->ગાંઠ = nodes[0];
    }
    નહીંતર જો (n_nodes > 0)
    {
	dlayer->ગાંઠ = gsk_container_ગાંઠ_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	માટે (પૂર્ણાંક i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(nodes); i++)
	    gsk_render_ગાંઠ_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    સ્થિર શૂન્ય
draw_row_render_overlay(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    જો (!dlayer->dirty)
	પાછા;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    જો (rgba_is_transparent(dlayer->buf, da->n_cols))
	પાછા;

    dlayer->ગાંઠ = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    સ્થિર શૂન્ય
draw_row_add_cluster(
	DrawRow		*drow,
	DrawAttr	*dattr,
	પૂર્ણાંક		col,
	પૂર્ણાંક		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	પૂર્ણાંક		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = ખાલી;

    જો (col + n_cells > da->n_cols)
	પાછા;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    જો (glyphs != ખાલી)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_row_fill(drow, col, col + n_cells - 1, dglyphs, dattr, &da->cursor);

    જો (dglyphs != ખાલી)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    સ્થિર પૂર્ણાંક
vim_draw_area_add_string(
	VimDrawArea *self,
	DrawRow	    *drow,
	DrawAttr    *dattr,
	પૂર્ણાંક	    col,
	char_u	    *s,
	પૂર્ણાંક	    len,
	પૂર્ણાંક	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = ખાલી;
    પૂર્ણાંક			start_idx = -1;
    GList		*item_list;
    પૂર્ણાંક			column_offset = 0;
    પૂર્ણાંક			cluster_width;
    પૂર્ણાંક			last_glyph_rbearing;
    પૂર્ણાંક			cells = 0;
    પૂર્ણાંક			i = 0;
    gboolean		has_unicode = FALSE;
    gboolean		needs_shaping = FALSE;

    // Scan the string, and find spans where ligatures should not happen. We
    // must ensure that attribute boundaries do not split a base character from
    // its combining marks, as Pango breaks items at attribute boundaries.
    જ્યારે (i < len)
    {
	પૂર્ણાંક	clen = 1;
	પૂર્ણાંક	is_composing = 0;
	bool	want_disable;

	જો (s[i] >= 0x80)
	{
	    clen = utf_ptr2len(s + i);
	    is_composing = utf_iscomposing(utf_ptr2char(s + i));
	    has_unicode = TRUE;
	}

	જો (is_composing)
	    // Do not change state mid-cluster. Inherit the previous character's
	    // state.
	    want_disable = (start_idx != -1);
	નહીંતર
	    // Base character: disable ligatures if it's an ASCII non-ligature
	    // char.
	    want_disable = (s[i] < 0x80 && !gui.ligatures_map[s[i]]);

	// If any base character actually wants ligature/contextual shaping,
	// the string must go through Pango's shaping engine -- the ASCII
	// fast path below draws cached glyphs one-by-one and can never
	// produce a ligature, regardless of 'guiligatures'.
	જો (!is_composing && !want_disable)
	    needs_shaping = TRUE;

	જો (want_disable)
	{
	    જો (start_idx == -1)
		start_idx = i;
	}
	નહીંતર જો (start_idx != -1)
	{
	    જો (attr_list == ખાલી)
		attr_list = pango_attr_list_new();
	    ઉમેરો_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, start_idx, i);
	    start_idx = -1;
	}
	i += clen;
    }

    // Fast path for pure ASCII: use cached glyph table. Skip this path when
    // there are non-ascii characters in the string, font attributes, or if
    // there's a possible ligature.
    જો (!(draw_flags & DRAW_ITALIC)
	    && !((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != ખાલી
	    && !has_unicode
	    && !needs_shaping)
    {
	માટે (i = 0; i < len; ++i)
	{
	    PangoGlyphInfo  glyph = gui.ascii_glyphs->glyphs[2 * s[i]];
	    gboolean	    has_ink;

	    has_ink = glyph_has_ink(gui.ascii_font, glyph);

	    draw_row_add_cluster(drow, dattr, col + i, 1,
		    gui.ascii_font, has_ink ? &glyph : ખાલી, 1, self);
	}

	column_offset = len;
	જાઓ exit;
    }

    glyphs = pango_glyph_string_new();

    જો (attr_list == ખાલી)
	attr_list = pango_attr_list_new();

    જો (start_idx != -1)
	ઉમેરો_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, start_idx, len);

    cluster_width = PANGO_SCALE * gui.char_width;
    last_glyph_rbearing = PANGO_SCALE * gui.char_width;

    // If 'guifontwide' is set then use that for double-width characters.
    જો (gui.wide_font != ખાલી)
	apply_wide_font_attr(s, len, attr_list);

    જો ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	ઉમેરો_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    જો (draw_flags & DRAW_ITALIC)
	ઉમેરો_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    item_list = pango_itemize(gui.text_context,
	    (અચલ અક્ષર *)s, 0, len, attr_list, ખાલી);

    જ્યારે (item_list != ખાલી)
    {
	PangoItem   *item = item_list->data;
	પૂર્ણાંક	    item_cells = 0;
	પૂર્ણાંક	    cluster_start_idx = 0;
	પૂર્ણાંક	    current_cluster_col = col + column_offset;
	પૂર્ણાંક	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	item_list = g_list_delete_link(item_list, item_list);

	// Force LTR direction; Vim handles bidi on its own.
	item->analysis.level = (item->analysis.level + 1) & (~1U);

	pango_shape_full((અચલ અક્ષર *)s + item->offset, item->length,
		(અચલ અક્ષર *)s, len, &item->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	માટે (i = 0; i < glyphs->અંક_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    જો (glyph->attr.is_cluster_start)
	    {
		પૂર્ણાંક cellcount;

		cellcount = ગણતરી_cluster_cells(
			s, item, glyphs, i, &cluster_width,
			(item_list != ખાલી) ? &last_glyph_rbearing : ખાલી);

		જો (i > 0 && cellcount > 0)
		{
		    draw_row_add_cluster(drow, dattr,
			    current_cluster_col, current_cluster_cells,
			    item->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_start_idx : ખાલી,
			    i - cluster_start_idx, self);

		    current_cluster_col += current_cluster_cells;
		    cluster_start_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		જો (cellcount > 0)
		{
		    પૂર્ણાંક width;

		    width = cellcount * gui.char_width * PANGO_SCALE;
		    glyph->geometry.x_offset +=
			MAX(0, width - cluster_width) / 2;
		    glyph->geometry.width = width;
		}
		નહીંતર
		    setup_zero_width_cluster(item, glyph, cells,
			    cluster_width,
			    last_glyph_rbearing);

		item_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    નહીંતર જો (i > 0)
	    {
		પૂર્ણાંક width;

		જો (glyph->geometry.x_offset >= 0)
		{
		    glyphs->glyphs[i].geometry.width =
			glyphs->glyphs[i - 1].geometry.width;
		    glyphs->glyphs[i - 1].geometry.width = 0;
		}
		width = cells * gui.char_width * PANGO_SCALE;
		glyph->geometry.x_offset +=
		    MAX(0, width - cluster_width) / 2;
	    }
	    નહીંતર
		glyph->geometry.width = 0;


	    જો (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			item->analysis.font, *glyph);
	}
	જો (glyphs->અંક_glyphs > 0)
	    draw_row_add_cluster(drow, dattr,
		    current_cluster_col, current_cluster_cells,
		    item->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_start_idx : ખાલી,
		    glyphs->અંક_glyphs - cluster_start_idx, self);

	pango_item_free(item);
	column_offset += item_cells;
    }

    pango_glyph_string_free(glyphs);

exit:
    જો (attr_list != ખાલી)
	pango_attr_list_unref(attr_list);

    પાછા column_offset;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    પૂર્ણાંક
vim_draw_area_draw_string(
	VimDrawArea *self,
	પૂર્ણાંક	    row,
	પૂર્ણાંક	    col,
	char_u	    *s,
	પૂર્ણાંક	    len,
	પૂર્ણાંક	    flags)
{
    DrawRow	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = ખાલી;
    પૂર્ણાંક		convlen;
    પૂર્ણાંક		n_cells ;

    જો (unlikely(self->rows == ખાલી
		|| row >= self->n_rows
		|| col >= self->n_cols))
	પાછા len;

    જો (output_conv.vc_type != CONV_NONE)
    {
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	જો (conv_buf != ખાલી)
	{
	    s = conv_buf;
	    len = convlen;
	}
    }

    // Safety check: pango crashes with invalid utf-8.
    જો (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	પાછા len;
    }

    drow = self->rows + row;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_add_string(self, drow, &dattr, col, s, len, flags);

    vim_free(conv_buf);

    પાછા n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    શૂન્ય
vim_draw_area_clear_block(
	VimDrawArea	*self,
	પૂર્ણાંક		row1,
	પૂર્ણાંક		col1,
	પૂર્ણાંક		row2,
	પૂર્ણાંક		col2)
{
    જો (unlikely(self->rows == ખાલી
		|| row1 >= self->n_rows
		|| col1 >= self->n_cols
		|| row2 >= self->n_rows
		|| col2 >= self->n_cols))
	પાછા;

    માટે (પૂર્ણાંક r = row1; r <= row2; r++)
	draw_row_fill(self->rows + r, col1, col2, ખાલી, ખાલી, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    શૂન્ય
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_block(self, 0, 0, self->n_rows - 1, self->n_cols - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    શૂન્ય
vim_draw_area_move_block(
	VimDrawArea *self,
	પૂર્ણાંક	    to,
	પૂર્ણાંક	    row1,
	પૂર્ણાંક	    row2,
	પૂર્ણાંક	    col1,
	પૂર્ણાંક	    col2)
{
    પૂર્ણાંક offset = row2 - row1;

    જો (unlikely(self->rows == ખાલી
		|| row1 >= self->n_rows
		|| row2 >= self->n_rows
		|| to >= self->n_rows
		|| col1 >= self->n_cols
		|| col2 >= self->n_cols))
	પાછા;

    પ્રતિજ્ઞા(row2 >= row1);
    પ્રતિજ્ઞા(col2 >= col1);
    પ્રતિજ્ઞા(row1 != to);

    જો (row1 > to)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	માટે (પૂર્ણાંક o = 0; o <= offset; o++)
	    draw_row_move_to(self->rows + to + o, self->rows + row1 + o,
		    col1, col2, self);
    }
    નહીંતર
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	માટે (પૂર્ણાંક o = offset; o >= 0; o--)
	    જો (to + o >= self->n_rows)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_block(self, row1 + o, col1, row1 + o, col2);
	    નહીંતર
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
    શૂન્ય
vim_draw_area_set_cursor(VimDrawArea *self, પૂર્ણાંક w, પૂર્ણાંક h)
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
    શૂન્ય
vim_draw_area_invert_block(
	VimDrawArea	*self,
	પૂર્ણાંક		row,
	પૂર્ણાંક		col,
	પૂર્ણાંક		nrows,
	પૂર્ણાંક		ncols)
{
    જો (unlikely(self->rows == ખાલી
		|| row >= self->n_rows
		|| col >= self->n_cols
		|| row + nrows - 1 >= self->n_rows
		|| col + ncols - 1 >= self->n_cols))
	પાછા;

    માટે (પૂર્ણાંક r = row; r < row + nrows; r++)
    {
	DrawRow	    *drow = self->rows + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	માટે (પૂર્ણાંક c = 0; c < ncols; c++)
	    dlayer->buf[c + col] = ~dlayer->buf[c + col];
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#જો defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    શૂન્ય
vim_draw_area_add_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	પૂર્ણાંક	    row,
	પૂર્ણાંક	    col,
	પૂર્ણાંક	    width,
	પૂર્ણાંક	    height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    પૂર્ણાંક		cells;

    જો (unlikely(self->rows == ખાલી
		|| row >= self->n_rows
		|| col >= self->n_cols))
	પાછા;

    drow = self->rows + row;
    dsign = draw_sign_new(sign, width, height);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (width + gui.char_width - 1) / gui.char_width;

    draw_row_fill(drow, col, col + cells - 1, dglyphs, ખાલી, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    શૂન્ય
vim_draw_area_add_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	પૂર્ણાંક		row,
	પૂર્ણાંક		col,
	પૂર્ણાંક		width,
	પૂર્ણાંક		height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    પૂર્ણાંક		cells;
    GdkTexture	*texture;
    GBytes	*bytes;

    જો (unlikely(self->rows == ખાલી
		|| row >= self->n_rows
		|| col >= self->n_cols))
	પાછા;

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

    draw_row_fill(drow, col, col + cells - 1, dglyphs, ખાલી, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    શૂન્ય
vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_ગાંઠ *ગાંઠ)
{
    g_hash_table_add(self->external, gsk_render_ગાંઠ_ref(ગાંઠ));
}

    શૂન્ય
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_ગાંઠ *ગાંઠ)
{
    g_hash_table_remove(self->external, ગાંઠ);
}

#endif

/*
 * Add the cursor to the snapshot.
 */
    સ્થિર શૂન્ય
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_ગાંઠ	*text_ગાંઠ,	// May be NULL
	Gsk_Render_ગાંઠ	*special_ગાંઠ,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    પૂર્ણાંક		    w = cursor->width;
    પૂર્ણાંક		    h = cursor->height;
    graphene_rect_t rect;

    જો (!cursor->draw)
	પાછા;

    જો (cursor->width <= 0 && cursor->height <= 0)
    {
	w += gui.char_width * cursor->n_cells;
	h = gui.char_height;
    }

    graphene_rect_init(&rect,
#ifdef FEAT_RIGHTLEFT
	    CURSOR_BAR_જમણું ? FILL_X(gui.cursor_col + 1) - w :
#endif
	    FILL_X(gui.cursor_col),
	    FILL_Y(gui.cursor_row) + gui.char_height - h, w, h);


    જો (cursor->width == -1 && cursor->height == -1)
    {
	// Create hollow cursor
	GskRoundedRect	    outline;
	સ્થિર અચલ દશાંશ  border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	અચલ GdkRGBA	    color[4] = {
	    cursor->bg_color, cursor->bg_color,
	    cursor->bg_color, cursor->bg_color
	} ;

	gsk_rounded_rect_init_from_rect(&outline, &rect, 0.0f);
	gtk_snapshot_append_border(snapshot, &outline, border, color);
    }
    નહીંતર
    {
	// Create block style cursor, by masking the text + under decorations
	// with the fg color.
	gtk_snapshot_append_color(snapshot, &cursor->bg_color, &rect);

	gtk_snapshot_ધકેલો_mask(snapshot, GSK_MASK_MODE_ALPHA);

	// Used as an "outline" for the fg color.
	જો (text_ગાંઠ != ખાલી)
	    gtk_snapshot_append_ગાંઠ(snapshot, text_ગાંઠ);
	જો (special_ગાંઠ != ખાલી)
	    gtk_snapshot_append_ગાંઠ(snapshot, special_ગાંઠ);

	gtk_snapshot_કાઢો(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_કાઢો(snapshot);
    }
}

    સ્થિર શૂન્ય
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    પૂર્ણાંક		    height, width;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = ખાલી;
    Gsk_Render_ગાંઠ   *body_ગાંઠ;
    Gsk_Render_ગાંઠ   *invert_ગાંઠ = ખાલી;
#ifdef FEAT_IMAGE_GUI
    Gsk_Render_ગાંઠ   *ext_ગાંઠ;
    GHashTableIter  iter;
#endif

    gui_mch_set_bg_color(gui.back_pixel);
    height = gtk_widget_get_height(widget) + gui.bleed_bot;
    width = gtk_widget_get_width(widget) + gui.bleed_જમણું;

    જો (self->rows == ખાલી)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, width, height));
	પાછા;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    જો (self->bleed_જમણું == -1 || self->bleed_જમણું != gui.bleed_જમણું)
    {
	self->bleed_જમણું = gui.bleed_જમણું;
	માટે (પૂર્ણાંક r = 0; r < self->n_rows; r++)
	{
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_BACKGROUND);
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#જો GTK_CHECK_VERSION(4,24,0)
    gtk_snapshot_set_snap(body_snapshot, GSK_RECT_SNAP_ROUND);
#endif

    માટે (પૂર્ણાંક r = 0; r < self->n_rows; r++)
    {
	DrawRow *drow = self->rows + r;

	draw_row_render_background(drow, self);
	draw_row_render_text(drow, self);
	draw_row_render_special(drow, self);
	draw_row_render_overlay(drow, self);

	માટે (પૂર્ણાંક l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    જો (dlayer->ગાંઠ != ખાલી)
	    {
		જો (l == DRAW_LAYER_OVERLAY)
		{
		    જો (invert_snapshot == ખાલી)
		    {
			invert_snapshot = gtk_snapshot_new();
#જો GTK_CHECK_VERSION(4,24,0)
			gtk_snapshot_set_snap(invert_snapshot,
				GSK_RECT_SNAP_ROUND);
#endif
		    }
		    gtk_snapshot_append_ગાંઠ(invert_snapshot, dlayer->ગાંઠ);
		}
		નહીંતર
		    gtk_snapshot_append_ગાંઠ(body_snapshot, dlayer->ગાંઠ);
	    }
	}

	જો (r == gui.cursor_row)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_TEXT].ગાંઠ,
		    drow->dlayers[DRAW_LAYER_SPECIAL].ગાંઠ,
		    body_snapshot);
    }

    જો (invert_snapshot != ખાલી)
	gtk_snapshot_ધકેલો_blend(snapshot, GSK_BLEND_MODE_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, width, height));
    body_ગાંઠ = gtk_snapshot_free_to_ગાંઠ(body_snapshot);
    જો (body_ગાંઠ != ખાલી)
    {
	gtk_snapshot_append_ગાંઠ(snapshot, body_ગાંઠ);
	gsk_render_ગાંઠ_unref(body_ગાંઠ);
    }

#ifdef FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_table_iter_init(&iter, self->external);

    જ્યારે (g_hash_table_iter_next(&iter, (શૂન્ય **)&ext_ગાંઠ, ખાલી))
	gtk_snapshot_append_ગાંઠ(snapshot, ext_ગાંઠ);
#endif

    જો (invert_snapshot != ખાલી)
    {
	gtk_snapshot_કાઢો(snapshot);
	invert_ગાંઠ = gtk_snapshot_free_to_ગાંઠ(invert_snapshot);
	// Not sure if it can be NULL but still check
	જો (invert_ગાંઠ != ખાલી)
	{
	    gtk_snapshot_append_ગાંઠ(snapshot, invert_ગાંઠ);
	    gsk_render_ગાંઠ_unref(invert_ગાંઠ);
	}
	gtk_snapshot_કાઢો(snapshot);
    }
}
