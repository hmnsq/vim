/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_da.h"


#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Used for sign icons and netbeans multisign indicator. It is in a DrawGlyphs
 * so that it can move/be manipulated with the text.
 */
تعريفنوع بنية
{
    GdkTexture	*texture;
    صحيح		width;
    صحيح		height;
} DrawSign;
#endif

/*
 * Temporary struct when drawing string representing visual attributes of a cell
 */
تعريفنوع بنية
{
    uint32_t	bg_color;
    uint32_t	fg_color;   // Only used for underlines
    uint32_t	sp_color;
    صحيح		draw_flags; // DRAW_* flags, only DRAW_UNDERL, DRAW_UNDERC, and
			    // DRAW_STRIKE are used.
} DrawAttr;

/*
 * Represents the glyphs of a single cell.
 */
تعريفنوع بنية
{
    صحيح refcount;

    uint32_t	fg_color;
    PangoFont	*font;
    // If "font" is NULL, "dsign" is used, otherwise "glyphs" is used.
    اتحاد
    {
	PangoGlyphInfo  *glyphs; // May be NULL
#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	DrawSign	*dsign;
#endif
    };
    صحيح n_glyphs;
} DrawGlyphs;

تعريفنوع بنية
{
    DrawGlyphs *dglyphs;
} DrawCell;

تعريفنوع تعداد
{
    DRAW_LAYER_BACKGROUND,  // Background color, buf stores bg color
    DRAW_LAYER_TEXT,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

تعريفنوع بنية
{
    Gsk_Render_گرێ   *گرێ;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#define UNDERC_OFFSET(n_cols) (0)
#define STRIKE_OFFSET(n_cols) (n_cols)

تعريفنوع بنية
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    صحيح		n_cells; // For convenience
    صحيح		row;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_گرێ *underl_mask;
    Gsk_Render_گرێ *underc_mask;
    Gsk_Render_گرێ *strike_mask;
} DrawRow;

#define زیادکردن_PANGO_ATTR(Attribute, AttrList, Start, End) \
    افعل { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->start_index = (Start); \
	tmp_attr_->end_index = (End); \
	pango_attr_list_زیادکردن((AttrList), tmp_attr_); \
    } طالما (FALSE)
#define CELLS2PANGO(c) ((c) * gui.char_width * PANGO_SCALE)

/*
 * Attributes of cursor
 */
تعريفنوع بنية
{
    gboolean draw; // If cursor should be drawn
    صحيح width;
    صحيح height;
    صحيح n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

بنية _VimDrawArea
{
    GtkWidget parent;

    DrawRow *rows;

    صحيح n_rows;
    صحيح	n_cols;

    صحيح	bleed_ڕاست;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*گرێ_buf;

    DrawCursor cursor;

#ifdef FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHashTable *external;
#endif
};

ساكن فراغ draw_row_init(DrawRow *drow, صحيح row, صحيح cols);
ساكن فراغ draw_row_clear(DrawRow *drow);
ساكن فراغ draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t);
ساكن فراغ vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

ساكن GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    ساكن فراغ
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    لكل (صحيح r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    g_free(self->rows);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->گرێ_buf, TRUE);

#ifdef FEAT_IMAGE_GUI
    g_hash_table_unref(self->external);
#endif

    G_OBJECT_CLASS(vim_draw_area_parent_class)->finalize(obj);
}

    ساكن فراغ
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    ساكن فراغ
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_ڕاست = -1;
#ifdef FEAT_IMAGE_GUI
    self->external = g_hash_table_new_full(g_direct_hash, g_direct_equal,
	    عدم, (GDestroyNotify)gsk_render_گرێ_unref);
#endif
    self->glyph_buf = g_array_new(FALSE, FALSE, حجم(PangoGlyphInfo));
    self->گرێ_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_گرێ_unref);

}

    GtkWidget *
vim_draw_area_new(فراغ)
{
    أرجع g_object_new(VIM_TYPE_DRAW_AREA, عدم);
}

/*
 * Set the size of the draw area to "rows" and "cols".
 */
    فراغ
vim_draw_area_set_size(VimDrawArea *self, صحيح rows, صحيح cols)
{
    إذا (self->rows != عدم && self->n_rows == rows && self->n_cols == cols)
	أرجع;
    إذا (rows == 0 || cols == 0)
	أرجع;

    vim_draw_area_clear(self);

    لكل (صحيح r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    self->rows = g_realloc_n(self->rows, rows, حجم(DrawRow));
    لكل (صحيح r = 0; r < rows; r++)
	draw_row_init(self->rows + r, r, cols);

    self->n_rows = rows;
    self->n_cols = cols;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    ساكن uint32_t
rgba_to_u32(ثابت GdkRGBA *rgba)
{
    اتحاد { uint32_t val; uint8_t bytes[4]; } color;

    color.bytes[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.bytes[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.bytes[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.bytes[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    أرجع color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    ساكن GdkRGBA
u32_to_rgba(uint32_t val)
{
    اتحاد { uint32_t val; uint8_t bytes[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.bytes[0] / 255.0;
    color.green = packed.bytes[1] / 255.0;
    color.blue  = packed.bytes[2] / 255.0;
    color.alpha = packed.bytes[3] / 255.0;
    أرجع color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    ساكن gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    اتحاد { uint32_t val; uint8_t bytes[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.bytes[0] << 16) |
	((guicolor_T)packed.bytes[1] << 8)  |
	packed.bytes[2];
    أرجع res == rgb;
}

/*
 * Checks if a RGBA pixel buffer is fully transparent.
 */
    ساكن مضمن gboolean
rgba_is_transparent(ثابت uint32_t *buf, صحيح n_pixels)
{
    // Could also compare 2 pixels at once using a 64 bit integer, but not sure
    // if that might cause alignment issues...
    لكل (صحيح i = 0; i < n_pixels; i++)
	إذا (buf[i] != 0)
	    أرجع FALSE;
    أرجع TRUE;
}

/*
 * Return TRUE if "glyph" has ink (not whitespace).
 */
    ساكن gboolean
glyph_has_ink(PangoFont *font, PangoGlyphInfo glyph)
{
    PangoRectangle ink;

    pango_font_get_glyph_extents (font, glyph.glyph, &ink, عدم);

    أرجع ink.width > 0 && ink.height > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    ساكن فراغ
apply_wide_font_attr(char_u *s, صحيح len, PangoAttrList *attr_list)
{
    char_u  *start = عدم;
    char_u  *p;
    صحيح	    uc;

    لكل (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	إذا (start == عدم)
	{
	    إذا (uc >= 0x80 && utf_char2cells(uc) == 2)
		start = p;
	}
	وإلا إذا (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    زیادکردن_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, start - s, p - s);
	    start = عدم;
	}
    }

    إذا (start != عدم)
	زیادکردن_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, start - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    ساكن صحيح
ژماردن_cluster_cells(
	char_u		    *s,
	PangoItem	    *item,
	PangoGlyphString    *glyphs,
	صحيح		    i,
	صحيح		    *cluster_width,
	صحيح		    *last_glyph_rbearing)
{
    char_u  *p;
    صحيح	    next;
    صحيح	    start, end;
    صحيح	    width;
    صحيح	    uc;
    صحيح	    cellcount = 0;

    width = glyphs->glyphs[i].geometry.width;

    لكل (next = i + 1; next < glyphs->ژمارە_glyphs; ++next)
    {
	إذا (glyphs->glyphs[next].attr.is_cluster_start)
	    اكسر;
	وإلا إذا (glyphs->glyphs[next].geometry.width > width)
	    width = glyphs->glyphs[next].geometry.width;
    }

    start = item->offset + glyphs->log_clusters[i];
    end   = item->offset + ((next < glyphs->ژمارە_glyphs) ?
	    glyphs->log_clusters[next] : item->length);

    لكل (p = s + start; p < s + end; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	إذا (uc < 0x80)
	    ++cellcount;
	وإلا إذا (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    إذا (last_glyph_rbearing != عدم
	    && cellcount > 0 && next == glyphs->ژمارە_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(item->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, عدم);

	إذا (PANGO_RBEARING(ink_rect) > 0)
	    *last_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    إذا (cellcount > 0)
	*cluster_width = width;

    أرجع cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    ساكن فراغ
setup_zero_width_cluster(
	PangoItem	*item,
	PangoGlyphInfo	*glyph,
	صحيح		last_cellcount,
	صحيح		last_cluster_width,
	صحيح		last_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحيح		    width;

    width = last_cellcount * gui.char_width * PANGO_SCALE;
    glyph->geometry.x_offset = -width + MAX(0, width - last_cluster_width) / 2;
    glyph->geometry.width = 0;

    pango_font_get_glyph_extents(item->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    إذا (ink_rect.x < 0)
    {
	glyph->geometry.x_offset += last_glyph_rbearing;
	glyph->geometry.y_offset  = logical_rect.height
	    - (gui.char_height - p_linespace) * PANGO_SCALE;
    }
    وإلا
	glyph->geometry.x_offset = -width + MAX(0, width - ink_rect.width) / 2;
}

#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    ساكن DrawSign *
draw_sign_new(GdkTexture *texture, صحيح width, صحيح height)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->width = width;
    dsign->height = height;

    أرجع dsign;
}

    ساكن فراغ
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#endif

    ساكن فراغ
draw_attr_init(
	DrawAttr	*dattr,
	ثابت GdkRGBA	*bg_color,
	ثابت GdkRGBA	*fg_color,
	ثابت GdkRGBA	*sp_color,
	صحيح		draw_flags)
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
    ساكن فراغ
draw_attr_apply(DrawAttr *dattr, صحيح col, DrawRow *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + col;
    uint32_t	*text_cell = drow->dlayers[DRAW_LAYER_TEXT].buf + col;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    صحيح		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + UNDERC_OFFSET(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + STRIKE_OFFSET(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    إذا (dattr != عدم)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.back_pixel, bg_color);
    }

    إذا (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	إذا (bg_is_default)
	    *bg_cell = 0;
	وإلا
	    *bg_cell = bg_color;
    }
    إذا ((draw_flags & DRAW_UNDERL && *text_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *text_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);
	إذا (draw_flags & DRAW_UNDERL)
	    *text_cell = fg_color;
	وإلا
	    *text_cell = 0;
    }
    إذا ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	إذا (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	وإلا
	    *underc_cell = 0;
    }
    إذا ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	إذا (draw_flags & DRAW_STRIKE)
	    *strike_cell = sp_color;
	وإلا
	    *strike_cell = 0;
    }
}

    ساكن DrawGlyphs *
draw_glyphs_new(
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	صحيح		n_glyphs,
	uint32_t	fg_color)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->font = g_object_ref(font);
    dglyphs->glyphs = g_memdup2(glyphs, حجم(*glyphs) * n_glyphs);
    dglyphs->n_glyphs = n_glyphs;
    dglyphs->fg_color = fg_color;

    أرجع dglyphs;
}

#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw glyphs that shows the given sign. Note that ownership of
 * "dsign" is taken.
 */
    ساكن DrawGlyphs *
draw_glyphs_new_sign(DrawSign *dsign)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->dsign = dsign;

    أرجع dglyphs;
}
#endif

    ساكن DrawGlyphs *
draw_glyphs_ref(DrawGlyphs *dglyphs)
{
    dglyphs->refcount++;
    أرجع dglyphs;
}

    ساكن فراغ
draw_glyphs_unref(DrawGlyphs *dglyphs)
{
    إذا (--dglyphs->refcount <= 0)
    {
#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	إذا (dglyphs->font == عدم)
	    draw_sign_free(dglyphs->dsign);
	وإلا
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
    ساكن gboolean
draw_glyphs_same(DrawGlyphs *a, DrawGlyphs *b)
{
    إذا (a == عدم && b == عدم)
	أرجع TRUE;
    إذا (a == عدم || b == عدم)
	أرجع FALSE;
    إذا (a->fg_color != b->fg_color)
	أرجع FALSE;
    إذا (a->n_glyphs != b->n_glyphs || a->font != b->font)
	أرجع FALSE;
    لكل (صحيح i = 0; i < a->n_glyphs; i++)
	إذا (a->glyphs[i].glyph != b->glyphs[i].glyph
		|| a->glyphs[i].geometry.width != b->glyphs[i].geometry.width
		|| a->glyphs[i].geometry.x_offset != b->glyphs[i].geometry.x_offset
		|| a->glyphs[i].geometry.y_offset != b->glyphs[i].geometry.y_offset
		|| a->glyphs[i].attr.is_cluster_start != b->glyphs[i].attr.is_cluster_start)
	    أرجع FALSE;
    أرجع TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    ساكن Gsk_Render_گرێ *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	صحيح	    row,
	VimDrawArea *da,
	صحيح	    off,
	gboolean    bleed)
{
    GBytes	    *bytes;
    GdkTexture	    *texture;
    Gsk_Render_گرێ   *گرێ;
    صحيح		    height;

#إذا GTK_CHECK_VERSION(4,24,0)
    height = gui.char_height;
#وإلا
    height = gui.char_height + 1;
#endif

    إذا (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		حجم(uint32_t) * da->n_cols);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_cols - 1)),
		حجم(uint32_t)); // Add bleed using last pixel of buffer
	bytes = g_byte_array_free_to_bytes(arr);
    }
    وإلا
	bytes = g_bytes_new(dlayer->buf + off, حجم(uint32_t) * da->n_cols);

    texture = gdk_memory_texture_new(da->n_cols + bleed, 1,
	    GDK_MEMORY_R8G8B8A8, bytes, (da->n_cols + bleed) * 4);
    g_bytes_unref(bytes);

    // Scale texture to actual size
    گرێ = gsk_texture_scale_گرێ_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		(da->n_cols + bleed) * gui.char_width, gui.char_height + bleed),
	    GSK_SCALING_FILTER_NEAREST);
    إذا (bleed)
    {
	Gsk_Render_گرێ *new;

	new = gsk_clip_گرێ_new(گرێ,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		    da->n_cols * gui.char_width + da->bleed_ڕاست, height));
	gsk_render_گرێ_unref(گرێ);
	گرێ = new;
    }
    g_object_unref(texture);
    أرجع گرێ;
}

    ساكن فراغ
draw_row_init(DrawRow *drow, صحيح row, صحيح cols)
{
    drow->cells = g_malloc0_n(cols, حجم(DrawCell));
    drow->n_cells = cols;
    drow->row = row;
    drow->underl_mask = drow->underc_mask = drow->strike_mask = عدم;

    memset(drow->dlayers, 0, حجم(DrawLayer) * N_DRAW_LAYERS);
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	إذا (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(cols * 2, 4);
	وإلا
	    dlayer->buf = g_malloc0_n(cols, 4);
    }
}

    ساكن فراغ
draw_row_clear(DrawRow *drow)
{
    g_free(drow->cells);
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	إذا (dlayer->گرێ != عدم)
	    gsk_render_گرێ_unref(dlayer->گرێ);
	g_free(dlayer->buf);
    }
    g_clear_pointer(&drow->underl_mask, gsk_render_گرێ_unref);
    g_clear_pointer(&drow->underc_mask, gsk_render_گرێ_unref);
    g_clear_pointer(&drow->strike_mask, gsk_render_گرێ_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    ساكن فراغ
draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_pointer(&dlayer->گرێ, gsk_render_گرێ_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    ساكن فراغ
draw_row_uninvert_cell(DrawRow *drow, صحيح col)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    إذا (buf[col] == 0xffffffff)
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[col] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    ساكن فراغ
draw_row_set_cell(DrawRow *drow, صحيح col, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + col;

    إذا (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    إذا (dcell->dglyphs != dglyphs)
    {
	إذا (dcell->dglyphs != عدم)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == عدم ? عدم : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, col, drow);
    draw_row_uninvert_cell(drow, col);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    ساكن فراغ
draw_row_fill(
	DrawRow	    *drow,
	صحيح	    col1,
	صحيح	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    لكل (صحيح c = col1; c <= col2; c++)
	draw_row_set_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    إذا (gui.cursor_row == drow->row && gui.cursor_col >= col1
	    && gui.cursor_col <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    ساكن فراغ
draw_row_move_to(
	DrawRow	    *dest_row,
	DrawRow	    *src_row,
	صحيح	    col1,
	صحيح	    col2,
	VimDrawArea *da)
{
    صحيح move_size = (col2 - col1 + 1) * حجم(DrawCell);

    draw_row_fill(dest_row, col1, col2, عدم, عدم, &da->cursor);
    memmove(dest_row->cells + col1, src_row->cells + col1, move_size);
    // NULL the draw cells so we don't double unreference.
    memset(src_row->cells + col1, 0, move_size);
    draw_row_dirty_layer(src_row, DRAW_LAYER_TEXT);

    // Move the layer buffers
    move_size = (col2 - col1 + 1) * 4;
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_row->dlayers[l].buf;
	uint32_t *dest = dest_row->dlayers[l].buf;

	إذا (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_OFFSET(dest_row->n_cells),
		    src + col1 + UNDERC_OFFSET(src_row->n_cells), move_size);
	    memmove(dest + col1 + STRIKE_OFFSET(dest_row->n_cells),
		    src + col1 + STRIKE_OFFSET(src_row->n_cells), move_size);
	}
	وإلا
	    memmove(dest + col1, src + col1, move_size);
	draw_row_dirty_layer(dest_row, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    ساكن فراغ
draw_row_render_background(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    إذا (!dlayer->dirty)
	أرجع;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    إذا (rgba_is_transparent(dlayer->buf, da->n_cols))
	أرجع;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->گرێ = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    ساكن فراغ
draw_row_ensure_decor(DrawRow *drow, صحيح flags)
{
    إذا (flags & DRAW_UNDERL && drow->underl_mask == عدم)
	drow->underl_mask = gsk_color_گرێ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->row + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    إذا (flags & DRAW_STRIKE && drow->strike_mask == عدم)
	drow->strike_mask = gsk_color_گرێ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->row) + (صحيح)(gui.char_height / 2),
		    FILL_X(drow->n_cells), 1));

    إذا (flags & DRAW_UNDERC && drow->underc_mask == عدم)
    {
	ساكن ثابت صحيح val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	صحيح y = FILL_Y(drow->row + 1) - 1;
	صحيح x_start = FILL_X(0);
	صحيح x_end = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_گرێ *child = عدم;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#إذا GTK_CHECK_VERSION(4, 14, 0)
	GskPathBuilder	*builder;
	GskPath		*path;
	GskStroke       *stroke;
	Gsk_Render_گرێ	*color_گرێ;
	graphene_rect_t bounds;

	builder = gsk_path_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_path_builder_move_to(builder, -1, y - val[7] + 0.5);
	gsk_path_builder_هێڵ_to(builder, 0, y - val[0] + 0.5);

	لكل (صحيح i = 1; i < 8; i++)
	    gsk_path_builder_هێڵ_to(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_path_builder_هێڵ_to(builder, 8, y - val[0] + 0.5);
	gsk_path_builder_هێڵ_to(builder, 9, y - val[1] + 0.5);

	path = gsk_path_builder_free_to_path(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_path_get_stroke_bounds(path, stroke, &bounds);
	color_گرێ = gsk_color_گرێ_new(&white_rgba, &bounds);
	child = gsk_stroke_گرێ_new(color_گرێ, path, stroke);

	gsk_stroke_free(stroke);
	gsk_path_unref(path);
	gsk_render_گرێ_unref(color_گرێ);
#وإلا
	cairo_t *cr;

	child = gsk_cairo_گرێ_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_گرێ_get_draw_context(child);

	cairo_set_هێڵ_width(cr, 1.0);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_to(cr, -1, y - val[7] + 0.5);
	cairo_هێڵ_to(cr, 0, y - val[0] + 0.5);

	لكل (صحيح i = 1; i < 8; ++i)
	    cairo_هێڵ_to(cr, i, y - val[i] + 0.5);

	cairo_هێڵ_to(cr, 8, y - val[0] + 0.5);
	cairo_هێڵ_to(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#endif
	drow->underc_mask = gsk_repeat_گرێ_new(
		&GRAPHENE_RECT_INIT(x_start, y - 3, x_end - x_start, 5),
		child, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_گرێ_unref(child);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    ساكن فراغ
draw_row_render_text(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_TEXT;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = عدم;
    صحيح		start_col = 0;
    صحيح		empty_cells = 0;
    uint32_t	cur_fg = 0;
    صحيح		c;

    إذا (!dlayer->dirty)
	أرجع;

    glyph_buf = da->glyph_buf;
    nodes = da->گرێ_buf;

    g_array_set_size(glyph_buf, 0);
    g_ptr_array_set_size(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#define FLUSH_گرێ() \
    افعل { \
	إذا (cur_font != عدم) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_گرێ	*گرێ; \
	    GdkRGBA		fg_color = u32_to_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->data; \
	    glyph_str.ژمارە_glyphs = glyph_buf->len; \
	    گرێ = gsk_text_گرێ_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_POINT_INIT(TEXT_X(start_col), \
			TEXT_Y(drow->row))); \
	    إذا (گرێ != عدم) \
		g_ptr_array_add(nodes, گرێ); \
	    g_array_set_size(glyph_buf, 0); \
	    cur_font = عدم; \
	} \
    } طالما (FALSE)

    لكل (c = 0; c < da->n_cols; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	صحيح	    prev_len;

	إذا (dglyphs == عدم)
	{
	    empty_cells++;
	    استمر;
	}
#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	وإلا إذا (dglyphs->font == عدم)
	{
	    // Add sign icon
	    Gsk_Render_گرێ   *snode;
	    صحيح		    w;

	    FLUSH_گرێ();

	    snode = gsk_texture_scale_گرێ_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->row),
			dglyphs->dsign->width, dglyphs->dsign->height),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->width + gui.char_width - 1) / gui.char_width;

	    إذا (snode != عدم)
		g_ptr_array_add(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    استمر;
	}
#endif
	وإلا إذا (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_گرێ();
	    cur_font = dglyphs->font;
	    start_col = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	إذا (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    استمر;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	إذا (empty_cells > 0)
	{
	    إذا (prev_len > 0)
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

    FLUSH_گرێ();

#undef FLUSH_گرێ

    إذا (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_گرێ *source;
	Gsk_Render_گرێ *گرێ;

	draw_row_ensure_decor(drow, DRAW_UNDERL);
	إذا (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    source = draw_layer_get_texture(dlayer, drow->row, da, 0, FALSE);
	    گرێ = gsk_mask_گرێ_new(source, drow->underl_mask,
		    GSK_MASK_MODE_ALPHA);
	    gsk_render_گرێ_unref(source);
	    g_ptr_array_add(nodes, گرێ);
	}

	dlayer->گرێ = gsk_container_گرێ_new(
		(Gsk_Render_گرێ **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    ساكن فراغ
draw_row_render_special(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_گرێ   *source;
    Gsk_Render_گرێ   *nodes[2];
    صحيح		    n_nodes = 0;

    إذا (!dlayer->dirty)
	أرجع;

    draw_row_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    إذا (!rgba_is_transparent(dlayer->buf + UNDERC_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		UNDERC_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_گرێ_new(source, drow->underc_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_گرێ_unref(source);
    }

    إذا (!rgba_is_transparent(dlayer->buf + STRIKE_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		STRIKE_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_گرێ_new(source, drow->strike_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_گرێ_unref(source);
    }

    إذا (n_nodes == 1)
    {
	dlayer->گرێ = nodes[0];
    }
    وإلا إذا (n_nodes > 0)
    {
	dlayer->گرێ = gsk_container_گرێ_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	لكل (صحيح i = 0; i < (صحيح)ARRAY_LENGTH(nodes); i++)
	    gsk_render_گرێ_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    ساكن فراغ
draw_row_render_overlay(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    إذا (!dlayer->dirty)
	أرجع;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    إذا (rgba_is_transparent(dlayer->buf, da->n_cols))
	أرجع;

    dlayer->گرێ = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    ساكن فراغ
draw_row_add_cluster(
	DrawRow		*drow,
	DrawAttr	*dattr,
	صحيح		col,
	صحيح		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	صحيح		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = عدم;

    إذا (col + n_cells > da->n_cols)
	أرجع;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    إذا (glyphs != عدم)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_row_fill(drow, col, col + n_cells - 1, dglyphs, dattr, &da->cursor);

    إذا (dglyphs != عدم)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    ساكن صحيح
vim_draw_area_add_string(
	VimDrawArea *self,
	DrawRow	    *drow,
	DrawAttr    *dattr,
	صحيح	    col,
	char_u	    *s,
	صحيح	    len,
	صحيح	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = عدم;
    صحيح			start_idx = -1;
    GList		*item_list;
    صحيح			column_offset = 0;
    صحيح			cluster_width;
    صحيح			last_glyph_rbearing;
    صحيح			cells = 0;
    صحيح			i = 0;
    gboolean		has_unicode = FALSE;
    gboolean		needs_shaping = FALSE;

    // Scan the string, and find spans where ligatures should not happen. We
    // must ensure that attribute boundaries do not split a base character from
    // its combining marks, as Pango breaks items at attribute boundaries.
    طالما (i < len)
    {
	صحيح	clen = 1;
	صحيح	is_composing = 0;
	bool	want_disable;

	إذا (s[i] >= 0x80)
	{
	    clen = utf_ptr2len(s + i);
	    is_composing = utf_iscomposing(utf_ptr2char(s + i));
	    has_unicode = TRUE;
	}

	إذا (is_composing)
	    // Do not change state mid-cluster. Inherit the previous character's
	    // state.
	    want_disable = (start_idx != -1);
	وإلا
	    // Base character: disable ligatures if it's an ASCII non-ligature
	    // char.
	    want_disable = (s[i] < 0x80 && !gui.ligatures_map[s[i]]);

	// If any base character actually wants ligature/contextual shaping,
	// the string must go through Pango's shaping engine -- the ASCII
	// fast path below draws cached glyphs one-by-one and can never
	// produce a ligature, regardless of 'guiligatures'.
	إذا (!is_composing && !want_disable)
	    needs_shaping = TRUE;

	إذا (want_disable)
	{
	    إذا (start_idx == -1)
		start_idx = i;
	}
	وإلا إذا (start_idx != -1)
	{
	    إذا (attr_list == عدم)
		attr_list = pango_attr_list_new();
	    زیادکردن_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, start_idx, i);
	    start_idx = -1;
	}
	i += clen;
    }

    // Fast path for pure ASCII: use cached glyph table. Skip this path when
    // there are non-ascii characters in the string, font attributes, or if
    // there's a possible ligature.
    إذا (!(draw_flags & DRAW_ITALIC)
	    && !((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != عدم
	    && !has_unicode
	    && !needs_shaping)
    {
	لكل (i = 0; i < len; ++i)
	{
	    PangoGlyphInfo  glyph = gui.ascii_glyphs->glyphs[2 * s[i]];
	    gboolean	    has_ink;

	    has_ink = glyph_has_ink(gui.ascii_font, glyph);

	    draw_row_add_cluster(drow, dattr, col + i, 1,
		    gui.ascii_font, has_ink ? &glyph : عدم, 1, self);
	}

	column_offset = len;
	اذهب exit;
    }

    glyphs = pango_glyph_string_new();

    إذا (attr_list == عدم)
	attr_list = pango_attr_list_new();

    إذا (start_idx != -1)
	زیادکردن_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, start_idx, len);

    cluster_width = PANGO_SCALE * gui.char_width;
    last_glyph_rbearing = PANGO_SCALE * gui.char_width;

    // If 'guifontwide' is set then use that for double-width characters.
    إذا (gui.wide_font != عدم)
	apply_wide_font_attr(s, len, attr_list);

    إذا ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	زیادکردن_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    إذا (draw_flags & DRAW_ITALIC)
	زیادکردن_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    item_list = pango_itemize(gui.text_context,
	    (ثابت حرف *)s, 0, len, attr_list, عدم);

    طالما (item_list != عدم)
    {
	PangoItem   *item = item_list->data;
	صحيح	    item_cells = 0;
	صحيح	    cluster_start_idx = 0;
	صحيح	    current_cluster_col = col + column_offset;
	صحيح	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	item_list = g_list_delete_link(item_list, item_list);

	// Force LTR direction; Vim handles bidi on its own.
	item->analysis.level = (item->analysis.level + 1) & (~1U);

	pango_shape_full((ثابت حرف *)s + item->offset, item->length,
		(ثابت حرف *)s, len, &item->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	لكل (i = 0; i < glyphs->ژمارە_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    إذا (glyph->attr.is_cluster_start)
	    {
		صحيح cellcount;

		cellcount = ژماردن_cluster_cells(
			s, item, glyphs, i, &cluster_width,
			(item_list != عدم) ? &last_glyph_rbearing : عدم);

		إذا (i > 0 && cellcount > 0)
		{
		    draw_row_add_cluster(drow, dattr,
			    current_cluster_col, current_cluster_cells,
			    item->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_start_idx : عدم,
			    i - cluster_start_idx, self);

		    current_cluster_col += current_cluster_cells;
		    cluster_start_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		إذا (cellcount > 0)
		{
		    صحيح width;

		    width = cellcount * gui.char_width * PANGO_SCALE;
		    glyph->geometry.x_offset +=
			MAX(0, width - cluster_width) / 2;
		    glyph->geometry.width = width;
		}
		وإلا
		    setup_zero_width_cluster(item, glyph, cells,
			    cluster_width,
			    last_glyph_rbearing);

		item_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    وإلا إذا (i > 0)
	    {
		صحيح width;

		إذا (glyph->geometry.x_offset >= 0)
		{
		    glyphs->glyphs[i].geometry.width =
			glyphs->glyphs[i - 1].geometry.width;
		    glyphs->glyphs[i - 1].geometry.width = 0;
		}
		width = cells * gui.char_width * PANGO_SCALE;
		glyph->geometry.x_offset +=
		    MAX(0, width - cluster_width) / 2;
	    }
	    وإلا
		glyph->geometry.width = 0;


	    إذا (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			item->analysis.font, *glyph);
	}
	إذا (glyphs->ژمارە_glyphs > 0)
	    draw_row_add_cluster(drow, dattr,
		    current_cluster_col, current_cluster_cells,
		    item->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_start_idx : عدم,
		    glyphs->ژمارە_glyphs - cluster_start_idx, self);

	pango_item_free(item);
	column_offset += item_cells;
    }

    pango_glyph_string_free(glyphs);

exit:
    إذا (attr_list != عدم)
	pango_attr_list_unref(attr_list);

    أرجع column_offset;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    صحيح
vim_draw_area_draw_string(
	VimDrawArea *self,
	صحيح	    row,
	صحيح	    col,
	char_u	    *s,
	صحيح	    len,
	صحيح	    flags)
{
    DrawRow	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = عدم;
    صحيح		convlen;
    صحيح		n_cells ;

    إذا (unlikely(self->rows == عدم
		|| row >= self->n_rows
		|| col >= self->n_cols))
	أرجع len;

    إذا (output_conv.vc_type != CONV_NONE)
    {
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	إذا (conv_buf != عدم)
	{
	    s = conv_buf;
	    len = convlen;
	}
    }

    // Safety check: pango crashes with invalid utf-8.
    إذا (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	أرجع len;
    }

    drow = self->rows + row;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_add_string(self, drow, &dattr, col, s, len, flags);

    vim_free(conv_buf);

    أرجع n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    فراغ
vim_draw_area_clear_block(
	VimDrawArea	*self,
	صحيح		row1,
	صحيح		col1,
	صحيح		row2,
	صحيح		col2)
{
    إذا (unlikely(self->rows == عدم
		|| row1 >= self->n_rows
		|| col1 >= self->n_cols
		|| row2 >= self->n_rows
		|| col2 >= self->n_cols))
	أرجع;

    لكل (صحيح r = row1; r <= row2; r++)
	draw_row_fill(self->rows + r, col1, col2, عدم, عدم, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    فراغ
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_block(self, 0, 0, self->n_rows - 1, self->n_cols - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    فراغ
vim_draw_area_move_block(
	VimDrawArea *self,
	صحيح	    to,
	صحيح	    row1,
	صحيح	    row2,
	صحيح	    col1,
	صحيح	    col2)
{
    صحيح offset = row2 - row1;

    إذا (unlikely(self->rows == عدم
		|| row1 >= self->n_rows
		|| row2 >= self->n_rows
		|| to >= self->n_rows
		|| col1 >= self->n_cols
		|| col2 >= self->n_cols))
	أرجع;

    أكد(row2 >= row1);
    أكد(col2 >= col1);
    أكد(row1 != to);

    إذا (row1 > to)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	لكل (صحيح o = 0; o <= offset; o++)
	    draw_row_move_to(self->rows + to + o, self->rows + row1 + o,
		    col1, col2, self);
    }
    وإلا
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	لكل (صحيح o = offset; o >= 0; o--)
	    إذا (to + o >= self->n_rows)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_block(self, row1 + o, col1, row1 + o, col2);
	    وإلا
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
    فراغ
vim_draw_area_set_cursor(VimDrawArea *self, صحيح w, صحيح h)
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
    فراغ
vim_draw_area_invert_block(
	VimDrawArea	*self,
	صحيح		row,
	صحيح		col,
	صحيح		nrows,
	صحيح		ncols)
{
    إذا (unlikely(self->rows == عدم
		|| row >= self->n_rows
		|| col >= self->n_cols
		|| row + nrows - 1 >= self->n_rows
		|| col + ncols - 1 >= self->n_cols))
	أرجع;

    لكل (صحيح r = row; r < row + nrows; r++)
    {
	DrawRow	    *drow = self->rows + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	لكل (صحيح c = 0; c < ncols; c++)
	    dlayer->buf[c + col] = ~dlayer->buf[c + col];
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#إذا defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    فراغ
vim_draw_area_add_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	صحيح	    row,
	صحيح	    col,
	صحيح	    width,
	صحيح	    height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحيح		cells;

    إذا (unlikely(self->rows == عدم
		|| row >= self->n_rows
		|| col >= self->n_cols))
	أرجع;

    drow = self->rows + row;
    dsign = draw_sign_new(sign, width, height);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (width + gui.char_width - 1) / gui.char_width;

    draw_row_fill(drow, col, col + cells - 1, dglyphs, عدم, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    فراغ
vim_draw_area_add_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	صحيح		row,
	صحيح		col,
	صحيح		width,
	صحيح		height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحيح		cells;
    GdkTexture	*texture;
    GBytes	*bytes;

    إذا (unlikely(self->rows == عدم
		|| row >= self->n_rows
		|| col >= self->n_cols))
	أرجع;

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

    draw_row_fill(drow, col, col + cells - 1, dglyphs, عدم, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    فراغ
vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_گرێ *گرێ)
{
    g_hash_table_add(self->external, gsk_render_گرێ_ref(گرێ));
}

    فراغ
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_گرێ *گرێ)
{
    g_hash_table_remove(self->external, گرێ);
}

#endif

/*
 * Add the cursor to the snapshot.
 */
    ساكن فراغ
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_گرێ	*text_گرێ,	// May be NULL
	Gsk_Render_گرێ	*special_گرێ,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    صحيح		    w = cursor->width;
    صحيح		    h = cursor->height;
    graphene_rect_t rect;

    إذا (!cursor->draw)
	أرجع;

    إذا (cursor->width <= 0 && cursor->height <= 0)
    {
	w += gui.char_width * cursor->n_cells;
	h = gui.char_height;
    }

    graphene_rect_init(&rect,
#ifdef FEAT_RIGHTLEFT
	    CURSOR_BAR_ڕاست ? FILL_X(gui.cursor_col + 1) - w :
#endif
	    FILL_X(gui.cursor_col),
	    FILL_Y(gui.cursor_row) + gui.char_height - h, w, h);


    إذا (cursor->width == -1 && cursor->height == -1)
    {
	// Create hollow cursor
	GskRoundedRect	    outline;
	ساكن ثابت عائم  border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	ثابت GdkRGBA	    color[4] = {
	    cursor->bg_color, cursor->bg_color,
	    cursor->bg_color, cursor->bg_color
	} ;

	gsk_rounded_rect_init_from_rect(&outline, &rect, 0.0f);
	gtk_snapshot_append_border(snapshot, &outline, border, color);
    }
    وإلا
    {
	// Create block style cursor, by masking the text + under decorations
	// with the fg color.
	gtk_snapshot_append_color(snapshot, &cursor->bg_color, &rect);

	gtk_snapshot_پاڵنان_mask(snapshot, GSK_MASK_MODE_ALPHA);

	// Used as an "outline" for the fg color.
	إذا (text_گرێ != عدم)
	    gtk_snapshot_append_گرێ(snapshot, text_گرێ);
	إذا (special_گرێ != عدم)
	    gtk_snapshot_append_گرێ(snapshot, special_گرێ);

	gtk_snapshot_دەرهێنان(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_دەرهێنان(snapshot);
    }
}

    ساكن فراغ
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    صحيح		    height, width;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = عدم;
    Gsk_Render_گرێ   *body_گرێ;
    Gsk_Render_گرێ   *invert_گرێ = عدم;
#ifdef FEAT_IMAGE_GUI
    Gsk_Render_گرێ   *ext_گرێ;
    GHashTableIter  iter;
#endif

    gui_mch_set_bg_color(gui.back_pixel);
    height = gtk_widget_get_height(widget) + gui.bleed_bot;
    width = gtk_widget_get_width(widget) + gui.bleed_ڕاست;

    إذا (self->rows == عدم)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, width, height));
	أرجع;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    إذا (self->bleed_ڕاست == -1 || self->bleed_ڕاست != gui.bleed_ڕاست)
    {
	self->bleed_ڕاست = gui.bleed_ڕاست;
	لكل (صحيح r = 0; r < self->n_rows; r++)
	{
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_BACKGROUND);
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#إذا GTK_CHECK_VERSION(4,24,0)
    gtk_snapshot_set_snap(body_snapshot, GSK_RECT_SNAP_ROUND);
#endif

    لكل (صحيح r = 0; r < self->n_rows; r++)
    {
	DrawRow *drow = self->rows + r;

	draw_row_render_background(drow, self);
	draw_row_render_text(drow, self);
	draw_row_render_special(drow, self);
	draw_row_render_overlay(drow, self);

	لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    إذا (dlayer->گرێ != عدم)
	    {
		إذا (l == DRAW_LAYER_OVERLAY)
		{
		    إذا (invert_snapshot == عدم)
		    {
			invert_snapshot = gtk_snapshot_new();
#إذا GTK_CHECK_VERSION(4,24,0)
			gtk_snapshot_set_snap(invert_snapshot,
				GSK_RECT_SNAP_ROUND);
#endif
		    }
		    gtk_snapshot_append_گرێ(invert_snapshot, dlayer->گرێ);
		}
		وإلا
		    gtk_snapshot_append_گرێ(body_snapshot, dlayer->گرێ);
	    }
	}

	إذا (r == gui.cursor_row)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_TEXT].گرێ,
		    drow->dlayers[DRAW_LAYER_SPECIAL].گرێ,
		    body_snapshot);
    }

    إذا (invert_snapshot != عدم)
	gtk_snapshot_پاڵنان_blend(snapshot, GSK_BLEND_MODE_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, width, height));
    body_گرێ = gtk_snapshot_free_to_گرێ(body_snapshot);
    إذا (body_گرێ != عدم)
    {
	gtk_snapshot_append_گرێ(snapshot, body_گرێ);
	gsk_render_گرێ_unref(body_گرێ);
    }

#ifdef FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_table_iter_init(&iter, self->external);

    طالما (g_hash_table_iter_next(&iter, (فراغ **)&ext_گرێ, عدم))
	gtk_snapshot_append_گرێ(snapshot, ext_گرێ);
#endif

    إذا (invert_snapshot != عدم)
    {
	gtk_snapshot_دەرهێنان(snapshot);
	invert_گرێ = gtk_snapshot_free_to_گرێ(invert_snapshot);
	// Not sure if it can be NULL but still check
	إذا (invert_گرێ != عدم)
	{
	    gtk_snapshot_append_گرێ(snapshot, invert_گرێ);
	    gsk_render_گرێ_unref(invert_گرێ);
	}
	gtk_snapshot_دەرهێنان(snapshot);
    }
}
