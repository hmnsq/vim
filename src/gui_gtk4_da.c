/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

#شامل <gtk/gtk.h>
#شامل "gui_gtk4_da.h"


#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Used for sign icons and netbeans multisign indicator. It is in a DrawGlyphs
 * so that it can move/be manipulated with the text.
 */
تعریفنوع ساختار
{
    GdkTexture	*texture;
    صحیح		width;
    صحیح		height;
} DrawSign;
#endif

/*
 * Temporary struct when drawing string representing visual attributes of a cell
 */
تعریفنوع ساختار
{
    uint32_t	bg_color;
    uint32_t	fg_color;   // Only used for underlines
    uint32_t	sp_color;
    صحیح		draw_flags; // DRAW_* flags, only DRAW_UNDERL, DRAW_UNDERC, and
			    // DRAW_STRIKE are used.
} DrawAttr;

/*
 * Represents the glyphs of a single cell.
 */
تعریفنوع ساختار
{
    صحیح refcount;

    uint32_t	fg_color;
    PangoFont	*font;
    // If "font" is NULL, "dsign" is used, otherwise "glyphs" is used.
    اجتماع
    {
	PangoGlyphInfo  *glyphs; // May be NULL
#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	DrawSign	*dsign;
#endif
    };
    صحیح n_glyphs;
} DrawGlyphs;

تعریفنوع ساختار
{
    DrawGlyphs *dglyphs;
} DrawCell;

تعریفنوع شمارش
{
    DRAW_LAYER_BACKGROUND,  // Background color, buf stores bg color
    DRAW_LAYER_TEXT,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

تعریفنوع ساختار
{
    Gsk_Render_گره   *گره;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#define UNDERC_OFFSET(n_cols) (0)
#define STRIKE_OFFSET(n_cols) (n_cols)

تعریفنوع ساختار
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    صحیح		n_cells; // For convenience
    صحیح		row;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_گره *underl_mask;
    Gsk_Render_گره *underc_mask;
    Gsk_Render_گره *strike_mask;
} DrawRow;

#define درج_PANGO_ATTR(Attribute, AttrList, Start, End) \
    انجامبده { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->start_index = (Start); \
	tmp_attr_->end_index = (End); \
	pango_attr_list_درج((AttrList), tmp_attr_); \
    } وقتی (FALSE)
#define CELLS2PANGO(c) ((c) * gui.char_width * PANGO_SCALE)

/*
 * Attributes of cursor
 */
تعریفنوع ساختار
{
    gboolean draw; // If cursor should be drawn
    صحیح width;
    صحیح height;
    صحیح n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

ساختار _VimDrawArea
{
    GtkWidget parent;

    DrawRow *rows;

    صحیح n_rows;
    صحیح	n_cols;

    صحیح	bleed_راست;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*گره_buf;

    DrawCursor cursor;

#ifdef FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHashTable *external;
#endif
};

ایستا تهی draw_row_init(DrawRow *drow, صحیح row, صحیح cols);
ایستا تهی draw_row_clear(DrawRow *drow);
ایستا تهی draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t);
ایستا تهی vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

ایستا GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    ایستا تهی
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    برای (صحیح r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    g_free(self->rows);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->گره_buf, TRUE);

#ifdef FEAT_IMAGE_GUI
    g_hash_table_unref(self->external);
#endif

    G_OBJECT_CLASS(vim_draw_area_parent_class)->finalize(obj);
}

    ایستا تهی
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    ایستا تهی
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_راست = -1;
#ifdef FEAT_IMAGE_GUI
    self->external = g_hash_table_new_full(g_direct_hash, g_direct_equal,
	    پوچ, (GDestroyNotify)gsk_render_گره_unref);
#endif
    self->glyph_buf = g_array_new(FALSE, FALSE, اندازه(PangoGlyphInfo));
    self->گره_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_گره_unref);

}

    GtkWidget *
vim_draw_area_new(تهی)
{
    بازگشت g_object_new(VIM_TYPE_DRAW_AREA, پوچ);
}

/*
 * Set the size of the draw area to "rows" and "cols".
 */
    تهی
vim_draw_area_set_size(VimDrawArea *self, صحیح rows, صحیح cols)
{
    اگر (self->rows != پوچ && self->n_rows == rows && self->n_cols == cols)
	بازگشت;
    اگر (rows == 0 || cols == 0)
	بازگشت;

    vim_draw_area_clear(self);

    برای (صحیح r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    self->rows = g_realloc_n(self->rows, rows, اندازه(DrawRow));
    برای (صحیح r = 0; r < rows; r++)
	draw_row_init(self->rows + r, r, cols);

    self->n_rows = rows;
    self->n_cols = cols;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    ایستا uint32_t
rgba_to_u32(ثابت GdkRGBA *rgba)
{
    اجتماع { uint32_t val; uint8_t bytes[4]; } color;

    color.bytes[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.bytes[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.bytes[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.bytes[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    بازگشت color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    ایستا GdkRGBA
u32_to_rgba(uint32_t val)
{
    اجتماع { uint32_t val; uint8_t bytes[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.bytes[0] / 255.0;
    color.green = packed.bytes[1] / 255.0;
    color.blue  = packed.bytes[2] / 255.0;
    color.alpha = packed.bytes[3] / 255.0;
    بازگشت color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    ایستا gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    اجتماع { uint32_t val; uint8_t bytes[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.bytes[0] << 16) |
	((guicolor_T)packed.bytes[1] << 8)  |
	packed.bytes[2];
    بازگشت res == rgb;
}

/*
 * Checks if a RGBA pixel buffer is fully transparent.
 */
    ایستا درونخطی gboolean
rgba_is_transparent(ثابت uint32_t *buf, صحیح n_pixels)
{
    // Could also compare 2 pixels at once using a 64 bit integer, but not sure
    // if that might cause alignment issues...
    برای (صحیح i = 0; i < n_pixels; i++)
	اگر (buf[i] != 0)
	    بازگشت FALSE;
    بازگشت TRUE;
}

/*
 * Return TRUE if "glyph" has ink (not whitespace).
 */
    ایستا gboolean
glyph_has_ink(PangoFont *font, PangoGlyphInfo glyph)
{
    PangoRectangle ink;

    pango_font_get_glyph_extents (font, glyph.glyph, &ink, پوچ);

    بازگشت ink.width > 0 && ink.height > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    ایستا تهی
apply_wide_font_attr(char_u *s, صحیح len, PangoAttrList *attr_list)
{
    char_u  *start = پوچ;
    char_u  *p;
    صحیح	    uc;

    برای (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	اگر (start == پوچ)
	{
	    اگر (uc >= 0x80 && utf_char2cells(uc) == 2)
		start = p;
	}
	وگرنه اگر (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    درج_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, start - s, p - s);
	    start = پوچ;
	}
    }

    اگر (start != پوچ)
	درج_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, start - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    ایستا صحیح
شمار_cluster_cells(
	char_u		    *s,
	PangoItem	    *item,
	PangoGlyphString    *glyphs,
	صحیح		    i,
	صحیح		    *cluster_width,
	صحیح		    *last_glyph_rbearing)
{
    char_u  *p;
    صحیح	    next;
    صحیح	    start, end;
    صحیح	    width;
    صحیح	    uc;
    صحیح	    cellcount = 0;

    width = glyphs->glyphs[i].geometry.width;

    برای (next = i + 1; next < glyphs->رقم_glyphs; ++next)
    {
	اگر (glyphs->glyphs[next].attr.is_cluster_start)
	    بشکن;
	وگرنه اگر (glyphs->glyphs[next].geometry.width > width)
	    width = glyphs->glyphs[next].geometry.width;
    }

    start = item->offset + glyphs->log_clusters[i];
    end   = item->offset + ((next < glyphs->رقم_glyphs) ?
	    glyphs->log_clusters[next] : item->length);

    برای (p = s + start; p < s + end; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	اگر (uc < 0x80)
	    ++cellcount;
	وگرنه اگر (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    اگر (last_glyph_rbearing != پوچ
	    && cellcount > 0 && next == glyphs->رقم_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(item->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, پوچ);

	اگر (PANGO_RBEARING(ink_rect) > 0)
	    *last_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    اگر (cellcount > 0)
	*cluster_width = width;

    بازگشت cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    ایستا تهی
setup_zero_width_cluster(
	PangoItem	*item,
	PangoGlyphInfo	*glyph,
	صحیح		last_cellcount,
	صحیح		last_cluster_width,
	صحیح		last_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحیح		    width;

    width = last_cellcount * gui.char_width * PANGO_SCALE;
    glyph->geometry.x_offset = -width + MAX(0, width - last_cluster_width) / 2;
    glyph->geometry.width = 0;

    pango_font_get_glyph_extents(item->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    اگر (ink_rect.x < 0)
    {
	glyph->geometry.x_offset += last_glyph_rbearing;
	glyph->geometry.y_offset  = logical_rect.height
	    - (gui.char_height - p_linespace) * PANGO_SCALE;
    }
    وگرنه
	glyph->geometry.x_offset = -width + MAX(0, width - ink_rect.width) / 2;
}

#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    ایستا DrawSign *
draw_sign_new(GdkTexture *texture, صحیح width, صحیح height)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->width = width;
    dsign->height = height;

    بازگشت dsign;
}

    ایستا تهی
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#endif

    ایستا تهی
draw_attr_init(
	DrawAttr	*dattr,
	ثابت GdkRGBA	*bg_color,
	ثابت GdkRGBA	*fg_color,
	ثابت GdkRGBA	*sp_color,
	صحیح		draw_flags)
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
    ایستا تهی
draw_attr_apply(DrawAttr *dattr, صحیح col, DrawRow *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + col;
    uint32_t	*text_cell = drow->dlayers[DRAW_LAYER_TEXT].buf + col;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    صحیح		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + UNDERC_OFFSET(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + STRIKE_OFFSET(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    اگر (dattr != پوچ)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.back_pixel, bg_color);
    }

    اگر (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	اگر (bg_is_default)
	    *bg_cell = 0;
	وگرنه
	    *bg_cell = bg_color;
    }
    اگر ((draw_flags & DRAW_UNDERL && *text_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *text_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);
	اگر (draw_flags & DRAW_UNDERL)
	    *text_cell = fg_color;
	وگرنه
	    *text_cell = 0;
    }
    اگر ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	اگر (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	وگرنه
	    *underc_cell = 0;
    }
    اگر ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	اگر (draw_flags & DRAW_STRIKE)
	    *strike_cell = sp_color;
	وگرنه
	    *strike_cell = 0;
    }
}

    ایستا DrawGlyphs *
draw_glyphs_new(
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	صحیح		n_glyphs,
	uint32_t	fg_color)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->font = g_object_ref(font);
    dglyphs->glyphs = g_memdup2(glyphs, اندازه(*glyphs) * n_glyphs);
    dglyphs->n_glyphs = n_glyphs;
    dglyphs->fg_color = fg_color;

    بازگشت dglyphs;
}

#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw glyphs that shows the given sign. Note that ownership of
 * "dsign" is taken.
 */
    ایستا DrawGlyphs *
draw_glyphs_new_sign(DrawSign *dsign)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->dsign = dsign;

    بازگشت dglyphs;
}
#endif

    ایستا DrawGlyphs *
draw_glyphs_ref(DrawGlyphs *dglyphs)
{
    dglyphs->refcount++;
    بازگشت dglyphs;
}

    ایستا تهی
draw_glyphs_unref(DrawGlyphs *dglyphs)
{
    اگر (--dglyphs->refcount <= 0)
    {
#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	اگر (dglyphs->font == پوچ)
	    draw_sign_free(dglyphs->dsign);
	وگرنه
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
    ایستا gboolean
draw_glyphs_same(DrawGlyphs *a, DrawGlyphs *b)
{
    اگر (a == پوچ && b == پوچ)
	بازگشت TRUE;
    اگر (a == پوچ || b == پوچ)
	بازگشت FALSE;
    اگر (a->fg_color != b->fg_color)
	بازگشت FALSE;
    اگر (a->n_glyphs != b->n_glyphs || a->font != b->font)
	بازگشت FALSE;
    برای (صحیح i = 0; i < a->n_glyphs; i++)
	اگر (a->glyphs[i].glyph != b->glyphs[i].glyph
		|| a->glyphs[i].geometry.width != b->glyphs[i].geometry.width
		|| a->glyphs[i].geometry.x_offset != b->glyphs[i].geometry.x_offset
		|| a->glyphs[i].geometry.y_offset != b->glyphs[i].geometry.y_offset
		|| a->glyphs[i].attr.is_cluster_start != b->glyphs[i].attr.is_cluster_start)
	    بازگشت FALSE;
    بازگشت TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    ایستا Gsk_Render_گره *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	صحیح	    row,
	VimDrawArea *da,
	صحیح	    off,
	gboolean    bleed)
{
    GBytes	    *bytes;
    GdkTexture	    *texture;
    Gsk_Render_گره   *گره;
    صحیح		    height;

#اگر GTK_CHECK_VERSION(4,24,0)
    height = gui.char_height;
#وگرنه
    height = gui.char_height + 1;
#endif

    اگر (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		اندازه(uint32_t) * da->n_cols);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_cols - 1)),
		اندازه(uint32_t)); // Add bleed using last pixel of buffer
	bytes = g_byte_array_free_to_bytes(arr);
    }
    وگرنه
	bytes = g_bytes_new(dlayer->buf + off, اندازه(uint32_t) * da->n_cols);

    texture = gdk_memory_texture_new(da->n_cols + bleed, 1,
	    GDK_MEMORY_R8G8B8A8, bytes, (da->n_cols + bleed) * 4);
    g_bytes_unref(bytes);

    // Scale texture to actual size
    گره = gsk_texture_scale_گره_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		(da->n_cols + bleed) * gui.char_width, gui.char_height + bleed),
	    GSK_SCALING_FILTER_NEAREST);
    اگر (bleed)
    {
	Gsk_Render_گره *new;

	new = gsk_clip_گره_new(گره,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		    da->n_cols * gui.char_width + da->bleed_راست, height));
	gsk_render_گره_unref(گره);
	گره = new;
    }
    g_object_unref(texture);
    بازگشت گره;
}

    ایستا تهی
draw_row_init(DrawRow *drow, صحیح row, صحیح cols)
{
    drow->cells = g_malloc0_n(cols, اندازه(DrawCell));
    drow->n_cells = cols;
    drow->row = row;
    drow->underl_mask = drow->underc_mask = drow->strike_mask = پوچ;

    memset(drow->dlayers, 0, اندازه(DrawLayer) * N_DRAW_LAYERS);
    برای (صحیح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	اگر (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(cols * 2, 4);
	وگرنه
	    dlayer->buf = g_malloc0_n(cols, 4);
    }
}

    ایستا تهی
draw_row_clear(DrawRow *drow)
{
    g_free(drow->cells);
    برای (صحیح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	اگر (dlayer->گره != پوچ)
	    gsk_render_گره_unref(dlayer->گره);
	g_free(dlayer->buf);
    }
    g_clear_pointer(&drow->underl_mask, gsk_render_گره_unref);
    g_clear_pointer(&drow->underc_mask, gsk_render_گره_unref);
    g_clear_pointer(&drow->strike_mask, gsk_render_گره_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    ایستا تهی
draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_pointer(&dlayer->گره, gsk_render_گره_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    ایستا تهی
draw_row_uninvert_cell(DrawRow *drow, صحیح col)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    اگر (buf[col] == 0xffffffff)
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[col] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    ایستا تهی
draw_row_set_cell(DrawRow *drow, صحیح col, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + col;

    اگر (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    اگر (dcell->dglyphs != dglyphs)
    {
	اگر (dcell->dglyphs != پوچ)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == پوچ ? پوچ : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, col, drow);
    draw_row_uninvert_cell(drow, col);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    ایستا تهی
draw_row_fill(
	DrawRow	    *drow,
	صحیح	    col1,
	صحیح	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    برای (صحیح c = col1; c <= col2; c++)
	draw_row_set_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    اگر (gui.cursor_row == drow->row && gui.cursor_col >= col1
	    && gui.cursor_col <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    ایستا تهی
draw_row_move_to(
	DrawRow	    *dest_row,
	DrawRow	    *src_row,
	صحیح	    col1,
	صحیح	    col2,
	VimDrawArea *da)
{
    صحیح move_size = (col2 - col1 + 1) * اندازه(DrawCell);

    draw_row_fill(dest_row, col1, col2, پوچ, پوچ, &da->cursor);
    memmove(dest_row->cells + col1, src_row->cells + col1, move_size);
    // NULL the draw cells so we don't double unreference.
    memset(src_row->cells + col1, 0, move_size);
    draw_row_dirty_layer(src_row, DRAW_LAYER_TEXT);

    // Move the layer buffers
    move_size = (col2 - col1 + 1) * 4;
    برای (صحیح l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_row->dlayers[l].buf;
	uint32_t *dest = dest_row->dlayers[l].buf;

	اگر (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_OFFSET(dest_row->n_cells),
		    src + col1 + UNDERC_OFFSET(src_row->n_cells), move_size);
	    memmove(dest + col1 + STRIKE_OFFSET(dest_row->n_cells),
		    src + col1 + STRIKE_OFFSET(src_row->n_cells), move_size);
	}
	وگرنه
	    memmove(dest + col1, src + col1, move_size);
	draw_row_dirty_layer(dest_row, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    ایستا تهی
draw_row_render_background(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    اگر (!dlayer->dirty)
	بازگشت;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    اگر (rgba_is_transparent(dlayer->buf, da->n_cols))
	بازگشت;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->گره = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    ایستا تهی
draw_row_ensure_decor(DrawRow *drow, صحیح flags)
{
    اگر (flags & DRAW_UNDERL && drow->underl_mask == پوچ)
	drow->underl_mask = gsk_color_گره_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->row + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    اگر (flags & DRAW_STRIKE && drow->strike_mask == پوچ)
	drow->strike_mask = gsk_color_گره_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->row) + (صحیح)(gui.char_height / 2),
		    FILL_X(drow->n_cells), 1));

    اگر (flags & DRAW_UNDERC && drow->underc_mask == پوچ)
    {
	ایستا ثابت صحیح val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	صحیح y = FILL_Y(drow->row + 1) - 1;
	صحیح x_start = FILL_X(0);
	صحیح x_end = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_گره *child = پوچ;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#اگر GTK_CHECK_VERSION(4, 14, 0)
	GskPathBuilder	*builder;
	GskPath		*path;
	GskStroke       *stroke;
	Gsk_Render_گره	*color_گره;
	graphene_rect_t bounds;

	builder = gsk_path_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_path_builder_move_to(builder, -1, y - val[7] + 0.5);
	gsk_path_builder_سطر_to(builder, 0, y - val[0] + 0.5);

	برای (صحیح i = 1; i < 8; i++)
	    gsk_path_builder_سطر_to(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_path_builder_سطر_to(builder, 8, y - val[0] + 0.5);
	gsk_path_builder_سطر_to(builder, 9, y - val[1] + 0.5);

	path = gsk_path_builder_free_to_path(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_path_get_stroke_bounds(path, stroke, &bounds);
	color_گره = gsk_color_گره_new(&white_rgba, &bounds);
	child = gsk_stroke_گره_new(color_گره, path, stroke);

	gsk_stroke_free(stroke);
	gsk_path_unref(path);
	gsk_render_گره_unref(color_گره);
#وگرنه
	cairo_t *cr;

	child = gsk_cairo_گره_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_گره_get_draw_context(child);

	cairo_set_سطر_width(cr, 1.0);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_to(cr, -1, y - val[7] + 0.5);
	cairo_سطر_to(cr, 0, y - val[0] + 0.5);

	برای (صحیح i = 1; i < 8; ++i)
	    cairo_سطر_to(cr, i, y - val[i] + 0.5);

	cairo_سطر_to(cr, 8, y - val[0] + 0.5);
	cairo_سطر_to(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#endif
	drow->underc_mask = gsk_repeat_گره_new(
		&GRAPHENE_RECT_INIT(x_start, y - 3, x_end - x_start, 5),
		child, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_گره_unref(child);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    ایستا تهی
draw_row_render_text(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_TEXT;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = پوچ;
    صحیح		start_col = 0;
    صحیح		empty_cells = 0;
    uint32_t	cur_fg = 0;
    صحیح		c;

    اگر (!dlayer->dirty)
	بازگشت;

    glyph_buf = da->glyph_buf;
    nodes = da->گره_buf;

    g_array_set_size(glyph_buf, 0);
    g_ptr_array_set_size(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#define FLUSH_گره() \
    انجامبده { \
	اگر (cur_font != پوچ) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_گره	*گره; \
	    GdkRGBA		fg_color = u32_to_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->data; \
	    glyph_str.رقم_glyphs = glyph_buf->len; \
	    گره = gsk_text_گره_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_POINT_INIT(TEXT_X(start_col), \
			TEXT_Y(drow->row))); \
	    اگر (گره != پوچ) \
		g_ptr_array_add(nodes, گره); \
	    g_array_set_size(glyph_buf, 0); \
	    cur_font = پوچ; \
	} \
    } وقتی (FALSE)

    برای (c = 0; c < da->n_cols; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	صحیح	    prev_len;

	اگر (dglyphs == پوچ)
	{
	    empty_cells++;
	    ادامه;
	}
#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	وگرنه اگر (dglyphs->font == پوچ)
	{
	    // Add sign icon
	    Gsk_Render_گره   *snode;
	    صحیح		    w;

	    FLUSH_گره();

	    snode = gsk_texture_scale_گره_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->row),
			dglyphs->dsign->width, dglyphs->dsign->height),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->width + gui.char_width - 1) / gui.char_width;

	    اگر (snode != پوچ)
		g_ptr_array_add(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    ادامه;
	}
#endif
	وگرنه اگر (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_گره();
	    cur_font = dglyphs->font;
	    start_col = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	اگر (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    ادامه;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	اگر (empty_cells > 0)
	{
	    اگر (prev_len > 0)
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

    FLUSH_گره();

#undef FLUSH_گره

    اگر (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_گره *source;
	Gsk_Render_گره *گره;

	draw_row_ensure_decor(drow, DRAW_UNDERL);
	اگر (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    source = draw_layer_get_texture(dlayer, drow->row, da, 0, FALSE);
	    گره = gsk_mask_گره_new(source, drow->underl_mask,
		    GSK_MASK_MODE_ALPHA);
	    gsk_render_گره_unref(source);
	    g_ptr_array_add(nodes, گره);
	}

	dlayer->گره = gsk_container_گره_new(
		(Gsk_Render_گره **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    ایستا تهی
draw_row_render_special(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_گره   *source;
    Gsk_Render_گره   *nodes[2];
    صحیح		    n_nodes = 0;

    اگر (!dlayer->dirty)
	بازگشت;

    draw_row_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    اگر (!rgba_is_transparent(dlayer->buf + UNDERC_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		UNDERC_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_گره_new(source, drow->underc_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_گره_unref(source);
    }

    اگر (!rgba_is_transparent(dlayer->buf + STRIKE_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		STRIKE_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_گره_new(source, drow->strike_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_گره_unref(source);
    }

    اگر (n_nodes == 1)
    {
	dlayer->گره = nodes[0];
    }
    وگرنه اگر (n_nodes > 0)
    {
	dlayer->گره = gsk_container_گره_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	برای (صحیح i = 0; i < (صحیح)ARRAY_LENGTH(nodes); i++)
	    gsk_render_گره_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    ایستا تهی
draw_row_render_overlay(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    اگر (!dlayer->dirty)
	بازگشت;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    اگر (rgba_is_transparent(dlayer->buf, da->n_cols))
	بازگشت;

    dlayer->گره = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    ایستا تهی
draw_row_add_cluster(
	DrawRow		*drow,
	DrawAttr	*dattr,
	صحیح		col,
	صحیح		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	صحیح		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = پوچ;

    اگر (col + n_cells > da->n_cols)
	بازگشت;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    اگر (glyphs != پوچ)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_row_fill(drow, col, col + n_cells - 1, dglyphs, dattr, &da->cursor);

    اگر (dglyphs != پوچ)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    ایستا صحیح
vim_draw_area_add_string(
	VimDrawArea *self,
	DrawRow	    *drow,
	DrawAttr    *dattr,
	صحیح	    col,
	char_u	    *s,
	صحیح	    len,
	صحیح	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = پوچ;
    صحیح			start_idx = -1;
    GList		*item_list;
    صحیح			column_offset = 0;
    صحیح			cluster_width;
    صحیح			last_glyph_rbearing;
    صحیح			cells = 0;
    صحیح			i = 0;
    gboolean		has_unicode = FALSE;
    gboolean		needs_shaping = FALSE;

    // Scan the string, and find spans where ligatures should not happen. We
    // must ensure that attribute boundaries do not split a base character from
    // its combining marks, as Pango breaks items at attribute boundaries.
    وقتی (i < len)
    {
	صحیح	clen = 1;
	صحیح	is_composing = 0;
	bool	want_disable;

	اگر (s[i] >= 0x80)
	{
	    clen = utf_ptr2len(s + i);
	    is_composing = utf_iscomposing(utf_ptr2char(s + i));
	    has_unicode = TRUE;
	}

	اگر (is_composing)
	    // Do not change state mid-cluster. Inherit the previous character's
	    // state.
	    want_disable = (start_idx != -1);
	وگرنه
	    // Base character: disable ligatures if it's an ASCII non-ligature
	    // char.
	    want_disable = (s[i] < 0x80 && !gui.ligatures_map[s[i]]);

	// If any base character actually wants ligature/contextual shaping,
	// the string must go through Pango's shaping engine -- the ASCII
	// fast path below draws cached glyphs one-by-one and can never
	// produce a ligature, regardless of 'guiligatures'.
	اگر (!is_composing && !want_disable)
	    needs_shaping = TRUE;

	اگر (want_disable)
	{
	    اگر (start_idx == -1)
		start_idx = i;
	}
	وگرنه اگر (start_idx != -1)
	{
	    اگر (attr_list == پوچ)
		attr_list = pango_attr_list_new();
	    درج_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, start_idx, i);
	    start_idx = -1;
	}
	i += clen;
    }

    // Fast path for pure ASCII: use cached glyph table. Skip this path when
    // there are non-ascii characters in the string, font attributes, or if
    // there's a possible ligature.
    اگر (!(draw_flags & DRAW_ITALIC)
	    && !((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != پوچ
	    && !has_unicode
	    && !needs_shaping)
    {
	برای (i = 0; i < len; ++i)
	{
	    PangoGlyphInfo  glyph = gui.ascii_glyphs->glyphs[2 * s[i]];
	    gboolean	    has_ink;

	    has_ink = glyph_has_ink(gui.ascii_font, glyph);

	    draw_row_add_cluster(drow, dattr, col + i, 1,
		    gui.ascii_font, has_ink ? &glyph : پوچ, 1, self);
	}

	column_offset = len;
	برو exit;
    }

    glyphs = pango_glyph_string_new();

    اگر (attr_list == پوچ)
	attr_list = pango_attr_list_new();

    اگر (start_idx != -1)
	درج_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, start_idx, len);

    cluster_width = PANGO_SCALE * gui.char_width;
    last_glyph_rbearing = PANGO_SCALE * gui.char_width;

    // If 'guifontwide' is set then use that for double-width characters.
    اگر (gui.wide_font != پوچ)
	apply_wide_font_attr(s, len, attr_list);

    اگر ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	درج_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    اگر (draw_flags & DRAW_ITALIC)
	درج_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    item_list = pango_itemize(gui.text_context,
	    (ثابت نویسه *)s, 0, len, attr_list, پوچ);

    وقتی (item_list != پوچ)
    {
	PangoItem   *item = item_list->data;
	صحیح	    item_cells = 0;
	صحیح	    cluster_start_idx = 0;
	صحیح	    current_cluster_col = col + column_offset;
	صحیح	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	item_list = g_list_delete_link(item_list, item_list);

	// Force LTR direction; Vim handles bidi on its own.
	item->analysis.level = (item->analysis.level + 1) & (~1U);

	pango_shape_full((ثابت نویسه *)s + item->offset, item->length,
		(ثابت نویسه *)s, len, &item->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	برای (i = 0; i < glyphs->رقم_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    اگر (glyph->attr.is_cluster_start)
	    {
		صحیح cellcount;

		cellcount = شمار_cluster_cells(
			s, item, glyphs, i, &cluster_width,
			(item_list != پوچ) ? &last_glyph_rbearing : پوچ);

		اگر (i > 0 && cellcount > 0)
		{
		    draw_row_add_cluster(drow, dattr,
			    current_cluster_col, current_cluster_cells,
			    item->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_start_idx : پوچ,
			    i - cluster_start_idx, self);

		    current_cluster_col += current_cluster_cells;
		    cluster_start_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		اگر (cellcount > 0)
		{
		    صحیح width;

		    width = cellcount * gui.char_width * PANGO_SCALE;
		    glyph->geometry.x_offset +=
			MAX(0, width - cluster_width) / 2;
		    glyph->geometry.width = width;
		}
		وگرنه
		    setup_zero_width_cluster(item, glyph, cells,
			    cluster_width,
			    last_glyph_rbearing);

		item_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    وگرنه اگر (i > 0)
	    {
		صحیح width;

		اگر (glyph->geometry.x_offset >= 0)
		{
		    glyphs->glyphs[i].geometry.width =
			glyphs->glyphs[i - 1].geometry.width;
		    glyphs->glyphs[i - 1].geometry.width = 0;
		}
		width = cells * gui.char_width * PANGO_SCALE;
		glyph->geometry.x_offset +=
		    MAX(0, width - cluster_width) / 2;
	    }
	    وگرنه
		glyph->geometry.width = 0;


	    اگر (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			item->analysis.font, *glyph);
	}
	اگر (glyphs->رقم_glyphs > 0)
	    draw_row_add_cluster(drow, dattr,
		    current_cluster_col, current_cluster_cells,
		    item->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_start_idx : پوچ,
		    glyphs->رقم_glyphs - cluster_start_idx, self);

	pango_item_free(item);
	column_offset += item_cells;
    }

    pango_glyph_string_free(glyphs);

exit:
    اگر (attr_list != پوچ)
	pango_attr_list_unref(attr_list);

    بازگشت column_offset;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    صحیح
vim_draw_area_draw_string(
	VimDrawArea *self,
	صحیح	    row,
	صحیح	    col,
	char_u	    *s,
	صحیح	    len,
	صحیح	    flags)
{
    DrawRow	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = پوچ;
    صحیح		convlen;
    صحیح		n_cells ;

    اگر (unlikely(self->rows == پوچ
		|| row >= self->n_rows
		|| col >= self->n_cols))
	بازگشت len;

    اگر (output_conv.vc_type != CONV_NONE)
    {
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	اگر (conv_buf != پوچ)
	{
	    s = conv_buf;
	    len = convlen;
	}
    }

    // Safety check: pango crashes with invalid utf-8.
    اگر (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	بازگشت len;
    }

    drow = self->rows + row;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_add_string(self, drow, &dattr, col, s, len, flags);

    vim_free(conv_buf);

    بازگشت n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    تهی
vim_draw_area_clear_block(
	VimDrawArea	*self,
	صحیح		row1,
	صحیح		col1,
	صحیح		row2,
	صحیح		col2)
{
    اگر (unlikely(self->rows == پوچ
		|| row1 >= self->n_rows
		|| col1 >= self->n_cols
		|| row2 >= self->n_rows
		|| col2 >= self->n_cols))
	بازگشت;

    برای (صحیح r = row1; r <= row2; r++)
	draw_row_fill(self->rows + r, col1, col2, پوچ, پوچ, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    تهی
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_block(self, 0, 0, self->n_rows - 1, self->n_cols - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    تهی
vim_draw_area_move_block(
	VimDrawArea *self,
	صحیح	    to,
	صحیح	    row1,
	صحیح	    row2,
	صحیح	    col1,
	صحیح	    col2)
{
    صحیح offset = row2 - row1;

    اگر (unlikely(self->rows == پوچ
		|| row1 >= self->n_rows
		|| row2 >= self->n_rows
		|| to >= self->n_rows
		|| col1 >= self->n_cols
		|| col2 >= self->n_cols))
	بازگشت;

    أكد(row2 >= row1);
    أكد(col2 >= col1);
    أكد(row1 != to);

    اگر (row1 > to)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	برای (صحیح o = 0; o <= offset; o++)
	    draw_row_move_to(self->rows + to + o, self->rows + row1 + o,
		    col1, col2, self);
    }
    وگرنه
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	برای (صحیح o = offset; o >= 0; o--)
	    اگر (to + o >= self->n_rows)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_block(self, row1 + o, col1, row1 + o, col2);
	    وگرنه
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
    تهی
vim_draw_area_set_cursor(VimDrawArea *self, صحیح w, صحیح h)
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
    تهی
vim_draw_area_invert_block(
	VimDrawArea	*self,
	صحیح		row,
	صحیح		col,
	صحیح		nrows,
	صحیح		ncols)
{
    اگر (unlikely(self->rows == پوچ
		|| row >= self->n_rows
		|| col >= self->n_cols
		|| row + nrows - 1 >= self->n_rows
		|| col + ncols - 1 >= self->n_cols))
	بازگشت;

    برای (صحیح r = row; r < row + nrows; r++)
    {
	DrawRow	    *drow = self->rows + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	برای (صحیح c = 0; c < ncols; c++)
	    dlayer->buf[c + col] = ~dlayer->buf[c + col];
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#اگر defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    تهی
vim_draw_area_add_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	صحیح	    row,
	صحیح	    col,
	صحیح	    width,
	صحیح	    height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحیح		cells;

    اگر (unlikely(self->rows == پوچ
		|| row >= self->n_rows
		|| col >= self->n_cols))
	بازگشت;

    drow = self->rows + row;
    dsign = draw_sign_new(sign, width, height);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (width + gui.char_width - 1) / gui.char_width;

    draw_row_fill(drow, col, col + cells - 1, dglyphs, پوچ, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    تهی
vim_draw_area_add_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	صحیح		row,
	صحیح		col,
	صحیح		width,
	صحیح		height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحیح		cells;
    GdkTexture	*texture;
    GBytes	*bytes;

    اگر (unlikely(self->rows == پوچ
		|| row >= self->n_rows
		|| col >= self->n_cols))
	بازگشت;

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

    draw_row_fill(drow, col, col + cells - 1, dglyphs, پوچ, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    تهی
vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_گره *گره)
{
    g_hash_table_add(self->external, gsk_render_گره_ref(گره));
}

    تهی
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_گره *گره)
{
    g_hash_table_remove(self->external, گره);
}

#endif

/*
 * Add the cursor to the snapshot.
 */
    ایستا تهی
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_گره	*text_گره,	// May be NULL
	Gsk_Render_گره	*special_گره,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    صحیح		    w = cursor->width;
    صحیح		    h = cursor->height;
    graphene_rect_t rect;

    اگر (!cursor->draw)
	بازگشت;

    اگر (cursor->width <= 0 && cursor->height <= 0)
    {
	w += gui.char_width * cursor->n_cells;
	h = gui.char_height;
    }

    graphene_rect_init(&rect,
#ifdef FEAT_RIGHTLEFT
	    CURSOR_BAR_راست ? FILL_X(gui.cursor_col + 1) - w :
#endif
	    FILL_X(gui.cursor_col),
	    FILL_Y(gui.cursor_row) + gui.char_height - h, w, h);


    اگر (cursor->width == -1 && cursor->height == -1)
    {
	// Create hollow cursor
	GskRoundedRect	    outline;
	ایستا ثابت اعشاری  border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	ثابت GdkRGBA	    color[4] = {
	    cursor->bg_color, cursor->bg_color,
	    cursor->bg_color, cursor->bg_color
	} ;

	gsk_rounded_rect_init_from_rect(&outline, &rect, 0.0f);
	gtk_snapshot_append_border(snapshot, &outline, border, color);
    }
    وگرنه
    {
	// Create block style cursor, by masking the text + under decorations
	// with the fg color.
	gtk_snapshot_append_color(snapshot, &cursor->bg_color, &rect);

	gtk_snapshot_افزودن_mask(snapshot, GSK_MASK_MODE_ALPHA);

	// Used as an "outline" for the fg color.
	اگر (text_گره != پوچ)
	    gtk_snapshot_append_گره(snapshot, text_گره);
	اگر (special_گره != پوچ)
	    gtk_snapshot_append_گره(snapshot, special_گره);

	gtk_snapshot_برداشت(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_برداشت(snapshot);
    }
}

    ایستا تهی
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    صحیح		    height, width;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = پوچ;
    Gsk_Render_گره   *body_گره;
    Gsk_Render_گره   *invert_گره = پوچ;
#ifdef FEAT_IMAGE_GUI
    Gsk_Render_گره   *ext_گره;
    GHashTableIter  iter;
#endif

    gui_mch_set_bg_color(gui.back_pixel);
    height = gtk_widget_get_height(widget) + gui.bleed_bot;
    width = gtk_widget_get_width(widget) + gui.bleed_راست;

    اگر (self->rows == پوچ)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, width, height));
	بازگشت;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    اگر (self->bleed_راست == -1 || self->bleed_راست != gui.bleed_راست)
    {
	self->bleed_راست = gui.bleed_راست;
	برای (صحیح r = 0; r < self->n_rows; r++)
	{
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_BACKGROUND);
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#اگر GTK_CHECK_VERSION(4,24,0)
    gtk_snapshot_set_snap(body_snapshot, GSK_RECT_SNAP_ROUND);
#endif

    برای (صحیح r = 0; r < self->n_rows; r++)
    {
	DrawRow *drow = self->rows + r;

	draw_row_render_background(drow, self);
	draw_row_render_text(drow, self);
	draw_row_render_special(drow, self);
	draw_row_render_overlay(drow, self);

	برای (صحیح l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    اگر (dlayer->گره != پوچ)
	    {
		اگر (l == DRAW_LAYER_OVERLAY)
		{
		    اگر (invert_snapshot == پوچ)
		    {
			invert_snapshot = gtk_snapshot_new();
#اگر GTK_CHECK_VERSION(4,24,0)
			gtk_snapshot_set_snap(invert_snapshot,
				GSK_RECT_SNAP_ROUND);
#endif
		    }
		    gtk_snapshot_append_گره(invert_snapshot, dlayer->گره);
		}
		وگرنه
		    gtk_snapshot_append_گره(body_snapshot, dlayer->گره);
	    }
	}

	اگر (r == gui.cursor_row)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_TEXT].گره,
		    drow->dlayers[DRAW_LAYER_SPECIAL].گره,
		    body_snapshot);
    }

    اگر (invert_snapshot != پوچ)
	gtk_snapshot_افزودن_blend(snapshot, GSK_BLEND_MODE_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, width, height));
    body_گره = gtk_snapshot_free_to_گره(body_snapshot);
    اگر (body_گره != پوچ)
    {
	gtk_snapshot_append_گره(snapshot, body_گره);
	gsk_render_گره_unref(body_گره);
    }

#ifdef FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_table_iter_init(&iter, self->external);

    وقتی (g_hash_table_iter_next(&iter, (تهی **)&ext_گره, پوچ))
	gtk_snapshot_append_گره(snapshot, ext_گره);
#endif

    اگر (invert_snapshot != پوچ)
    {
	gtk_snapshot_برداشت(snapshot);
	invert_گره = gtk_snapshot_free_to_گره(invert_snapshot);
	// Not sure if it can be NULL but still check
	اگر (invert_گره != پوچ)
	{
	    gtk_snapshot_append_گره(snapshot, invert_گره);
	    gsk_render_گره_unref(invert_گره);
	}
	gtk_snapshot_برداشت(snapshot);
    }
}
