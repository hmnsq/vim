/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"

#include <gtk/gtk.h>
#include "gui_gtk4_da.h"


#ᨀᨘ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Used for sign icons and netbeans multisign indicator. It is in a DrawGlyphs
 * so that it can move/be manipulated with the text.
 */
typedef sa_nracanaa
{
    GdkTexture	*texture;
    puur_naa_nka		width;
    puur_naa_nka		height;
} DrawSign;
#endif

/*
 * Temporary struct when drawing string representing visual attributes of a cell
 */
typedef sa_nracanaa
{
    uint32_t	bg_color;
    uint32_t	fg_color;   // Only used for underlines
    uint32_t	sp_color;
    puur_naa_nka		draw_flags; // DRAW_* flags, only DRAW_UNDERL, DRAW_UNDERC, and
			    // DRAW_STRIKE are used.
} DrawAttr;

/*
 * Represents the glyphs of a single cell.
 */
typedef sa_nracanaa
{
    puur_naa_nka refcount;

    uint32_t	fg_color;
    PangoFont	*font;
    // If "font" is NULL, "dsign" is used, otherwise "glyphs" is used.
    sa_ngha
    {
	PangoGlyphInfo  *glyphs; // May be NULL
#ᨀᨘ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	DrawSign	*dsign;
#endif
    };
    puur_naa_nka n_glyphs;
} DrawGlyphs;

typedef sa_nracanaa
{
    DrawGlyphs *dglyphs;
} DrawCell;

typedef ga_nanaa
{
    DRAW_LAYER_BACKGROUND,  // Background color, buf stores bg color
    DRAW_LAYER_TEXT,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

typedef sa_nracanaa
{
    GskRenderNode   *node;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#define UNDERC_OFFSET(n_cols) (0)
#define STRIKE_OFFSET(n_cols) (n_cols)

typedef sa_nracanaa
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    puur_naa_nka		n_cells; // For convenience
    puur_naa_nka		row;	 // For convenience

    // Pre generated mask of of under decorations
    GskRenderNode *underl_mask;
    GskRenderNode *underc_mask;
    GskRenderNode *strike_mask;
} DrawRow;

#define INSERT_PANGO_ATTR(Attribute, AttrList, Start, End) \
    do { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->start_index = (Start); \
	tmp_attr_->end_index = (End); \
	pango_attr_list_insert((AttrList), tmp_attr_); \
    } ᨀᨛ (FALSE)
#define CELLS2PANGO(c) ((c) * gui.char_width * PANGO_SCALE)

/*
 * Attributes of cursor
 */
typedef sa_nracanaa
{
    gboolean draw; // If cursor should be drawn
    puur_naa_nka width;
    puur_naa_nka height;
    puur_naa_nka n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

sa_nracanaa _VimDrawArea
{
    GtkWidget parent;

    DrawRow *rows;

    puur_naa_nka n_rows;
    puur_naa_nka	n_cols;

    puur_naa_nka	bleed_right;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*node_buf;

    DrawCursor cursor;

#ifdef FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHashTable *external;
#endif
};

static void draw_row_init(DrawRow *drow, puur_naa_nka row, puur_naa_nka cols);
static void draw_row_clear(DrawRow *drow);
static void draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t);
static void vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

static GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    static void
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    for (puur_naa_nka r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    g_free(self->rows);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->node_buf, TRUE);

#ifdef FEAT_IMAGE_GUI
    g_hash_table_unref(self->external);
#endif

    G_OBJECT_CLASS(vim_draw_area_parent_class)->finalize(obj);
}

    static void
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    static void
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_right = -1;
#ifdef FEAT_IMAGE_GUI
    self->external = g_hash_table_new_full(g_direct_hash, g_direct_equal,
	    NULL, (GDestroyNotify)gsk_render_node_unref);
#endif
    self->glyph_buf = g_array_new(FALSE, FALSE, parimaa_na(PangoGlyphInfo));
    self->node_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_node_unref);

}

    GtkWidget *
vim_draw_area_new(void)
{
    ᨀᨘᨒ g_object_new(VIM_TYPE_DRAW_AREA, NULL);
}

/*
 * Set the size of the draw area to "rows" and "cols".
 */
    void
vim_draw_area_set_size(VimDrawArea *self, puur_naa_nka rows, puur_naa_nka cols)
{
    ᨀᨘ (self->rows != NULL && self->n_rows == rows && self->n_cols == cols)
	ᨀᨘᨒ;
    ᨀᨘ (rows == 0 || cols == 0)
	ᨀᨘᨒ;

    vim_draw_area_clear(self);

    for (puur_naa_nka r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    self->rows = g_realloc_n(self->rows, rows, parimaa_na(DrawRow));
    for (puur_naa_nka r = 0; r < rows; r++)
	draw_row_init(self->rows + r, r, cols);

    self->n_rows = rows;
    self->n_cols = cols;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    static uint32_t
rgba_to_u32(const GdkRGBA *rgba)
{
    sa_ngha { uint32_t val; uint8_t bytes[4]; } color;

    color.bytes[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.bytes[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.bytes[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.bytes[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    ᨀᨘᨒ color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    static GdkRGBA
u32_to_rgba(uint32_t val)
{
    sa_ngha { uint32_t val; uint8_t bytes[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.bytes[0] / 255.0;
    color.green = packed.bytes[1] / 255.0;
    color.blue  = packed.bytes[2] / 255.0;
    color.alpha = packed.bytes[3] / 255.0;
    ᨀᨘᨒ color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    static gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    sa_ngha { uint32_t val; uint8_t bytes[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.bytes[0] << 16) |
	((guicolor_T)packed.bytes[1] << 8)  |
	packed.bytes[2];
    ᨀᨘᨒ res == rgb;
}

/*
 * Checks if a RGBA pixel buffer is fully transparent.
 */
    static anta_hstha gboolean
rgba_is_transparent(const uint32_t *buf, puur_naa_nka n_pixels)
{
    // Could also compare 2 pixels at once using a 64 bit integer, but not sure
    // if that might cause alignment issues...
    for (puur_naa_nka i = 0; i < n_pixels; i++)
	ᨀᨘ (buf[i] != 0)
	    ᨀᨘᨒ FALSE;
    ᨀᨘᨒ TRUE;
}

/*
 * Return TRUE if "glyph" has ink (not whitespace).
 */
    static gboolean
glyph_has_ink(PangoFont *font, PangoGlyphInfo glyph)
{
    PangoRectangle ink;

    pango_font_get_glyph_extents (font, glyph.glyph, &ink, NULL);

    ᨀᨘᨒ ink.width > 0 && ink.height > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    static void
apply_wide_font_attr(char_u *s, puur_naa_nka len, PangoAttrList *attr_list)
{
    char_u  *start = NULL;
    char_u  *p;
    puur_naa_nka	    uc;

    for (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	ᨀᨘ (start == NULL)
	{
	    ᨀᨘ (uc >= 0x80 && utf_char2cells(uc) == 2)
		start = p;
	}
	ᨀᨘᨁ ᨀᨘ (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    INSERT_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, start - s, p - s);
	    start = NULL;
	}
    }

    ᨀᨘ (start != NULL)
	INSERT_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, start - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    static puur_naa_nka
count_cluster_cells(
	char_u		    *s,
	PangoItem	    *item,
	PangoGlyphString    *glyphs,
	puur_naa_nka		    i,
	puur_naa_nka		    *cluster_width,
	puur_naa_nka		    *last_glyph_rbearing)
{
    char_u  *p;
    puur_naa_nka	    next;
    puur_naa_nka	    start, end;
    puur_naa_nka	    width;
    puur_naa_nka	    uc;
    puur_naa_nka	    cellcount = 0;

    width = glyphs->glyphs[i].geometry.width;

    for (next = i + 1; next < glyphs->num_glyphs; ++next)
    {
	ᨀᨘ (glyphs->glyphs[next].attr.is_cluster_start)
	    bha_nga;
	ᨀᨘᨁ ᨀᨘ (glyphs->glyphs[next].geometry.width > width)
	    width = glyphs->glyphs[next].geometry.width;
    }

    start = item->offset + glyphs->log_clusters[i];
    end   = item->offset + ((next < glyphs->num_glyphs) ?
	    glyphs->log_clusters[next] : item->length);

    for (p = s + start; p < s + end; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	ᨀᨘ (uc < 0x80)
	    ++cellcount;
	ᨀᨘᨁ ᨀᨘ (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    ᨀᨘ (last_glyph_rbearing != NULL
	    && cellcount > 0 && next == glyphs->num_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(item->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, NULL);

	ᨀᨘ (PANGO_RBEARING(ink_rect) > 0)
	    *last_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    ᨀᨘ (cellcount > 0)
	*cluster_width = width;

    ᨀᨘᨒ cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    static void
setup_zero_width_cluster(
	PangoItem	*item,
	PangoGlyphInfo	*glyph,
	puur_naa_nka		last_cellcount,
	puur_naa_nka		last_cluster_width,
	puur_naa_nka		last_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    puur_naa_nka		    width;

    width = last_cellcount * gui.char_width * PANGO_SCALE;
    glyph->geometry.x_offset = -width + MAX(0, width - last_cluster_width) / 2;
    glyph->geometry.width = 0;

    pango_font_get_glyph_extents(item->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    ᨀᨘ (ink_rect.x < 0)
    {
	glyph->geometry.x_offset += last_glyph_rbearing;
	glyph->geometry.y_offset  = logical_rect.height
	    - (gui.char_height - p_linespace) * PANGO_SCALE;
    }
    ᨀᨘᨁ
	glyph->geometry.x_offset = -width + MAX(0, width - ink_rect.width) / 2;
}

#ᨀᨘ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    static DrawSign *
draw_sign_new(GdkTexture *texture, puur_naa_nka width, puur_naa_nka height)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->width = width;
    dsign->height = height;

    ᨀᨘᨒ dsign;
}

    static void
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#endif

    static void
draw_attr_init(
	DrawAttr	*dattr,
	const GdkRGBA	*bg_color,
	const GdkRGBA	*fg_color,
	const GdkRGBA	*sp_color,
	puur_naa_nka		draw_flags)
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
    static void
draw_attr_apply(DrawAttr *dattr, puur_naa_nka col, DrawRow *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + col;
    uint32_t	*text_cell = drow->dlayers[DRAW_LAYER_TEXT].buf + col;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    puur_naa_nka		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + UNDERC_OFFSET(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + STRIKE_OFFSET(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    ᨀᨘ (dattr != NULL)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.back_pixel, bg_color);
    }

    ᨀᨘ (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	ᨀᨘ (bg_is_default)
	    *bg_cell = 0;
	ᨀᨘᨁ
	    *bg_cell = bg_color;
    }
    ᨀᨘ ((draw_flags & DRAW_UNDERL && *text_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *text_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);
	ᨀᨘ (draw_flags & DRAW_UNDERL)
	    *text_cell = fg_color;
	ᨀᨘᨁ
	    *text_cell = 0;
    }
    ᨀᨘ ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	ᨀᨘ (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	ᨀᨘᨁ
	    *underc_cell = 0;
    }
    ᨀᨘ ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	ᨀᨘ (draw_flags & DRAW_STRIKE)
	    *strike_cell = sp_color;
	ᨀᨘᨁ
	    *strike_cell = 0;
    }
}

    static DrawGlyphs *
draw_glyphs_new(
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	puur_naa_nka		n_glyphs,
	uint32_t	fg_color)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->font = g_object_ref(font);
    dglyphs->glyphs = g_memdup2(glyphs, parimaa_na(*glyphs) * n_glyphs);
    dglyphs->n_glyphs = n_glyphs;
    dglyphs->fg_color = fg_color;

    ᨀᨘᨒ dglyphs;
}

#ᨀᨘ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw glyphs that shows the given sign. Note that ownership of
 * "dsign" is taken.
 */
    static DrawGlyphs *
draw_glyphs_new_sign(DrawSign *dsign)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->dsign = dsign;

    ᨀᨘᨒ dglyphs;
}
#endif

    static DrawGlyphs *
draw_glyphs_ref(DrawGlyphs *dglyphs)
{
    dglyphs->refcount++;
    ᨀᨘᨒ dglyphs;
}

    static void
draw_glyphs_unref(DrawGlyphs *dglyphs)
{
    ᨀᨘ (--dglyphs->refcount <= 0)
    {
#ᨀᨘ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	ᨀᨘ (dglyphs->font == NULL)
	    draw_sign_free(dglyphs->dsign);
	ᨀᨘᨁ
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
    static gboolean
draw_glyphs_same(DrawGlyphs *a, DrawGlyphs *b)
{
    ᨀᨘ (a == NULL && b == NULL)
	ᨀᨘᨒ TRUE;
    ᨀᨘ (a == NULL || b == NULL)
	ᨀᨘᨒ FALSE;
    ᨀᨘ (a->fg_color != b->fg_color)
	ᨀᨘᨒ FALSE;
    ᨀᨘ (a->n_glyphs != b->n_glyphs || a->font != b->font)
	ᨀᨘᨒ FALSE;
    for (puur_naa_nka i = 0; i < a->n_glyphs; i++)
	ᨀᨘ (a->glyphs[i].glyph != b->glyphs[i].glyph
		|| a->glyphs[i].geometry.width != b->glyphs[i].geometry.width
		|| a->glyphs[i].geometry.x_offset != b->glyphs[i].geometry.x_offset
		|| a->glyphs[i].geometry.y_offset != b->glyphs[i].geometry.y_offset
		|| a->glyphs[i].attr.is_cluster_start != b->glyphs[i].attr.is_cluster_start)
	    ᨀᨘᨒ FALSE;
    ᨀᨘᨒ TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    static GskRenderNode *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	puur_naa_nka	    row,
	VimDrawArea *da,
	puur_naa_nka	    off,
	gboolean    bleed)
{
    GBytes	    *bytes;
    GdkTexture	    *texture;
    GskRenderNode   *node;
    puur_naa_nka		    height;

#ᨀᨘ GTK_CHECK_VERSION(4,24,0)
    height = gui.char_height;
#ᨀᨘᨁ
    height = gui.char_height + 1;
#endif

    ᨀᨘ (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		parimaa_na(uint32_t) * da->n_cols);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_cols - 1)),
		parimaa_na(uint32_t)); // Add bleed using last pixel of buffer
	bytes = g_byte_array_free_to_bytes(arr);
    }
    ᨀᨘᨁ
	bytes = g_bytes_new(dlayer->buf + off, parimaa_na(uint32_t) * da->n_cols);

    texture = gdk_memory_texture_new(da->n_cols + bleed, 1,
	    GDK_MEMORY_R8G8B8A8, bytes, (da->n_cols + bleed) * 4);
    g_bytes_unref(bytes);

    // Scale texture to actual size
    node = gsk_texture_scale_node_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		(da->n_cols + bleed) * gui.char_width, gui.char_height + bleed),
	    GSK_SCALING_FILTER_NEAREST);
    ᨀᨘ (bleed)
    {
	GskRenderNode *new;

	new = gsk_clip_node_new(node,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		    da->n_cols * gui.char_width + da->bleed_right, height));
	gsk_render_node_unref(node);
	node = new;
    }
    g_object_unref(texture);
    ᨀᨘᨒ node;
}

    static void
draw_row_init(DrawRow *drow, puur_naa_nka row, puur_naa_nka cols)
{
    drow->cells = g_malloc0_n(cols, parimaa_na(DrawCell));
    drow->n_cells = cols;
    drow->row = row;
    drow->underl_mask = drow->underc_mask = drow->strike_mask = NULL;

    memset(drow->dlayers, 0, parimaa_na(DrawLayer) * N_DRAW_LAYERS);
    for (puur_naa_nka l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	ᨀᨘ (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(cols * 2, 4);
	ᨀᨘᨁ
	    dlayer->buf = g_malloc0_n(cols, 4);
    }
}

    static void
draw_row_clear(DrawRow *drow)
{
    g_free(drow->cells);
    for (puur_naa_nka l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	ᨀᨘ (dlayer->node != NULL)
	    gsk_render_node_unref(dlayer->node);
	g_free(dlayer->buf);
    }
    g_clear_pointer(&drow->underl_mask, gsk_render_node_unref);
    g_clear_pointer(&drow->underc_mask, gsk_render_node_unref);
    g_clear_pointer(&drow->strike_mask, gsk_render_node_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    static void
draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_pointer(&dlayer->node, gsk_render_node_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    static void
draw_row_uninvert_cell(DrawRow *drow, puur_naa_nka col)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    ᨀᨘ (buf[col] == 0xffffffff)
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[col] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    static void
draw_row_set_cell(DrawRow *drow, puur_naa_nka col, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + col;

    ᨀᨘ (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    ᨀᨘ (dcell->dglyphs != dglyphs)
    {
	ᨀᨘ (dcell->dglyphs != NULL)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == NULL ? NULL : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, col, drow);
    draw_row_uninvert_cell(drow, col);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    static void
draw_row_fill(
	DrawRow	    *drow,
	puur_naa_nka	    col1,
	puur_naa_nka	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    for (puur_naa_nka c = col1; c <= col2; c++)
	draw_row_set_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    ᨀᨘ (gui.cursor_row == drow->row && gui.cursor_col >= col1
	    && gui.cursor_col <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    static void
draw_row_move_to(
	DrawRow	    *dest_row,
	DrawRow	    *src_row,
	puur_naa_nka	    col1,
	puur_naa_nka	    col2,
	VimDrawArea *da)
{
    puur_naa_nka move_size = (col2 - col1 + 1) * parimaa_na(DrawCell);

    draw_row_fill(dest_row, col1, col2, NULL, NULL, &da->cursor);
    memmove(dest_row->cells + col1, src_row->cells + col1, move_size);
    // NULL the draw cells so we don't double unreference.
    memset(src_row->cells + col1, 0, move_size);
    draw_row_dirty_layer(src_row, DRAW_LAYER_TEXT);

    // Move the layer buffers
    move_size = (col2 - col1 + 1) * 4;
    for (puur_naa_nka l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_row->dlayers[l].buf;
	uint32_t *dest = dest_row->dlayers[l].buf;

	ᨀᨘ (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_OFFSET(dest_row->n_cells),
		    src + col1 + UNDERC_OFFSET(src_row->n_cells), move_size);
	    memmove(dest + col1 + STRIKE_OFFSET(dest_row->n_cells),
		    src + col1 + STRIKE_OFFSET(src_row->n_cells), move_size);
	}
	ᨀᨘᨁ
	    memmove(dest + col1, src + col1, move_size);
	draw_row_dirty_layer(dest_row, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    static void
draw_row_render_background(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    ᨀᨘ (!dlayer->dirty)
	ᨀᨘᨒ;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    ᨀᨘ (rgba_is_transparent(dlayer->buf, da->n_cols))
	ᨀᨘᨒ;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->node = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    static void
draw_row_ensure_decor(DrawRow *drow, puur_naa_nka flags)
{
    ᨀᨘ (flags & DRAW_UNDERL && drow->underl_mask == NULL)
	drow->underl_mask = gsk_color_node_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->row + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    ᨀᨘ (flags & DRAW_STRIKE && drow->strike_mask == NULL)
	drow->strike_mask = gsk_color_node_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->row) + (puur_naa_nka)(gui.char_height / 2),
		    FILL_X(drow->n_cells), 1));

    ᨀᨘ (flags & DRAW_UNDERC && drow->underc_mask == NULL)
    {
	static const puur_naa_nka val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	puur_naa_nka y = FILL_Y(drow->row + 1) - 1;
	puur_naa_nka x_start = FILL_X(0);
	puur_naa_nka x_end = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	GskRenderNode *child = NULL;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#ᨀᨘ GTK_CHECK_VERSION(4, 14, 0)
	GskPathBuilder	*builder;
	GskPath		*path;
	GskStroke       *stroke;
	GskRenderNode	*color_node;
	graphene_rect_t bounds;

	builder = gsk_path_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_path_builder_move_to(builder, -1, y - val[7] + 0.5);
	gsk_path_builder_line_to(builder, 0, y - val[0] + 0.5);

	for (puur_naa_nka i = 1; i < 8; i++)
	    gsk_path_builder_line_to(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_path_builder_line_to(builder, 8, y - val[0] + 0.5);
	gsk_path_builder_line_to(builder, 9, y - val[1] + 0.5);

	path = gsk_path_builder_free_to_path(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_path_get_stroke_bounds(path, stroke, &bounds);
	color_node = gsk_color_node_new(&white_rgba, &bounds);
	child = gsk_stroke_node_new(color_node, path, stroke);

	gsk_stroke_free(stroke);
	gsk_path_unref(path);
	gsk_render_node_unref(color_node);
#ᨀᨘᨁ
	cairo_t *cr;

	child = gsk_cairo_node_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_node_get_draw_context(child);

	cairo_set_line_width(cr, 1.0);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_to(cr, -1, y - val[7] + 0.5);
	cairo_line_to(cr, 0, y - val[0] + 0.5);

	for (puur_naa_nka i = 1; i < 8; ++i)
	    cairo_line_to(cr, i, y - val[i] + 0.5);

	cairo_line_to(cr, 8, y - val[0] + 0.5);
	cairo_line_to(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#endif
	drow->underc_mask = gsk_repeat_node_new(
		&GRAPHENE_RECT_INIT(x_start, y - 3, x_end - x_start, 5),
		child, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_node_unref(child);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    static void
draw_row_render_text(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_TEXT;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = NULL;
    puur_naa_nka		start_col = 0;
    puur_naa_nka		empty_cells = 0;
    uint32_t	cur_fg = 0;
    puur_naa_nka		c;

    ᨀᨘ (!dlayer->dirty)
	ᨀᨘᨒ;

    glyph_buf = da->glyph_buf;
    nodes = da->node_buf;

    g_array_set_size(glyph_buf, 0);
    g_ptr_array_set_size(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#define FLUSH_NODE() \
    do { \
	ᨀᨘ (cur_font != NULL) \
	{ \
	    PangoGlyphString	glyph_str; \
	    GskRenderNode	*node; \
	    GdkRGBA		fg_color = u32_to_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->data; \
	    glyph_str.num_glyphs = glyph_buf->len; \
	    node = gsk_text_node_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_POINT_INIT(TEXT_X(start_col), \
			TEXT_Y(drow->row))); \
	    ᨀᨘ (node != NULL) \
		g_ptr_array_add(nodes, node); \
	    g_array_set_size(glyph_buf, 0); \
	    cur_font = NULL; \
	} \
    } ᨀᨛ (FALSE)

    for (c = 0; c < da->n_cols; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	puur_naa_nka	    prev_len;

	ᨀᨘ (dglyphs == NULL)
	{
	    empty_cells++;
	    continue;
	}
#ᨀᨘ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	ᨀᨘᨁ ᨀᨘ (dglyphs->font == NULL)
	{
	    // Add sign icon
	    GskRenderNode   *snode;
	    puur_naa_nka		    w;

	    FLUSH_NODE();

	    snode = gsk_texture_scale_node_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->row),
			dglyphs->dsign->width, dglyphs->dsign->height),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->width + gui.char_width - 1) / gui.char_width;

	    ᨀᨘ (snode != NULL)
		g_ptr_array_add(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    continue;
	}
#endif
	ᨀᨘᨁ ᨀᨘ (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_NODE();
	    cur_font = dglyphs->font;
	    start_col = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	ᨀᨘ (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    continue;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	ᨀᨘ (empty_cells > 0)
	{
	    ᨀᨘ (prev_len > 0)
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

    FLUSH_NODE();

#undef FLUSH_NODE

    ᨀᨘ (nodes->len > 0)
    {
	// Add underline mask (if needed).
	GskRenderNode *source;
	GskRenderNode *node;

	draw_row_ensure_decor(drow, DRAW_UNDERL);
	ᨀᨘ (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    source = draw_layer_get_texture(dlayer, drow->row, da, 0, FALSE);
	    node = gsk_mask_node_new(source, drow->underl_mask,
		    GSK_MASK_MODE_ALPHA);
	    gsk_render_node_unref(source);
	    g_ptr_array_add(nodes, node);
	}

	dlayer->node = gsk_container_node_new(
		(GskRenderNode **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    static void
draw_row_render_special(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    GskRenderNode   *source;
    GskRenderNode   *nodes[2];
    puur_naa_nka		    n_nodes = 0;

    ᨀᨘ (!dlayer->dirty)
	ᨀᨘᨒ;

    draw_row_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    ᨀᨘ (!rgba_is_transparent(dlayer->buf + UNDERC_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		UNDERC_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_node_new(source, drow->underc_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_node_unref(source);
    }

    ᨀᨘ (!rgba_is_transparent(dlayer->buf + STRIKE_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		STRIKE_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_node_new(source, drow->strike_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_node_unref(source);
    }

    ᨀᨘ (n_nodes == 1)
    {
	dlayer->node = nodes[0];
    }
    ᨀᨘᨁ ᨀᨘ (n_nodes > 0)
    {
	dlayer->node = gsk_container_node_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	for (puur_naa_nka i = 0; i < (puur_naa_nka)ARRAY_LENGTH(nodes); i++)
	    gsk_render_node_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    static void
draw_row_render_overlay(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    ᨀᨘ (!dlayer->dirty)
	ᨀᨘᨒ;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    ᨀᨘ (rgba_is_transparent(dlayer->buf, da->n_cols))
	ᨀᨘᨒ;

    dlayer->node = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    static void
draw_row_add_cluster(
	DrawRow		*drow,
	DrawAttr	*dattr,
	puur_naa_nka		col,
	puur_naa_nka		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	puur_naa_nka		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = NULL;

    ᨀᨘ (col + n_cells > da->n_cols)
	ᨀᨘᨒ;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    ᨀᨘ (glyphs != NULL)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_row_fill(drow, col, col + n_cells - 1, dglyphs, dattr, &da->cursor);

    ᨀᨘ (dglyphs != NULL)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    static puur_naa_nka
vim_draw_area_add_string(
	VimDrawArea *self,
	DrawRow	    *drow,
	DrawAttr    *dattr,
	puur_naa_nka	    col,
	char_u	    *s,
	puur_naa_nka	    len,
	puur_naa_nka	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = NULL;
    puur_naa_nka			start_idx = -1;
    GList		*item_list;
    puur_naa_nka			column_offset = 0;
    puur_naa_nka			cluster_width;
    puur_naa_nka			last_glyph_rbearing;
    puur_naa_nka			cells = 0;
    puur_naa_nka			i = 0;
    gboolean		has_unicode = FALSE;
    gboolean		needs_shaping = FALSE;

    // Scan the string, and find spans where ligatures should not happen. We
    // must ensure that attribute boundaries do not split a base character from
    // its combining marks, as Pango breaks items at attribute boundaries.
    ᨀᨛ (i < len)
    {
	puur_naa_nka	clen = 1;
	puur_naa_nka	is_composing = 0;
	bool	want_disable;

	ᨀᨘ (s[i] >= 0x80)
	{
	    clen = utf_ptr2len(s + i);
	    is_composing = utf_iscomposing(utf_ptr2char(s + i));
	    has_unicode = TRUE;
	}

	ᨀᨘ (is_composing)
	    // Do not change state mid-cluster. Inherit the previous character's
	    // state.
	    want_disable = (start_idx != -1);
	ᨀᨘᨁ
	    // Base character: disable ligatures if it's an ASCII non-ligature
	    // char.
	    want_disable = (s[i] < 0x80 && !gui.ligatures_map[s[i]]);

	// If any base character actually wants ligature/contextual shaping,
	// the string must go through Pango's shaping engine -- the ASCII
	// fast path below draws cached glyphs one-by-one and can never
	// produce a ligature, regardless of 'guiligatures'.
	ᨀᨘ (!is_composing && !want_disable)
	    needs_shaping = TRUE;

	ᨀᨘ (want_disable)
	{
	    ᨀᨘ (start_idx == -1)
		start_idx = i;
	}
	ᨀᨘᨁ ᨀᨘ (start_idx != -1)
	{
	    ᨀᨘ (attr_list == NULL)
		attr_list = pango_attr_list_new();
	    INSERT_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, start_idx, i);
	    start_idx = -1;
	}
	i += clen;
    }

    // Fast path for pure ASCII: use cached glyph table. Skip this path when
    // there are non-ascii characters in the string, font attributes, or if
    // there's a possible ligature.
    ᨀᨘ (!(draw_flags & DRAW_ITALIC)
	    && !((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != NULL
	    && !has_unicode
	    && !needs_shaping)
    {
	for (i = 0; i < len; ++i)
	{
	    PangoGlyphInfo  glyph = gui.ascii_glyphs->glyphs[2 * s[i]];
	    gboolean	    has_ink;

	    has_ink = glyph_has_ink(gui.ascii_font, glyph);

	    draw_row_add_cluster(drow, dattr, col + i, 1,
		    gui.ascii_font, has_ink ? &glyph : NULL, 1, self);
	}

	column_offset = len;
	goto exit;
    }

    glyphs = pango_glyph_string_new();

    ᨀᨘ (attr_list == NULL)
	attr_list = pango_attr_list_new();

    ᨀᨘ (start_idx != -1)
	INSERT_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, start_idx, len);

    cluster_width = PANGO_SCALE * gui.char_width;
    last_glyph_rbearing = PANGO_SCALE * gui.char_width;

    // If 'guifontwide' is set then use that for double-width characters.
    ᨀᨘ (gui.wide_font != NULL)
	apply_wide_font_attr(s, len, attr_list);

    ᨀᨘ ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	INSERT_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    ᨀᨘ (draw_flags & DRAW_ITALIC)
	INSERT_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    item_list = pango_itemize(gui.text_context,
	    (const var_na *)s, 0, len, attr_list, NULL);

    ᨀᨛ (item_list != NULL)
    {
	PangoItem   *item = item_list->data;
	puur_naa_nka	    item_cells = 0;
	puur_naa_nka	    cluster_start_idx = 0;
	puur_naa_nka	    current_cluster_col = col + column_offset;
	puur_naa_nka	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	item_list = g_list_delete_link(item_list, item_list);

	// Force LTR direction; Vim handles bidi on its own.
	item->analysis.level = (item->analysis.level + 1) & (~1U);

	pango_shape_full((const var_na *)s + item->offset, item->length,
		(const var_na *)s, len, &item->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	for (i = 0; i < glyphs->num_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    ᨀᨘ (glyph->attr.is_cluster_start)
	    {
		puur_naa_nka cellcount;

		cellcount = count_cluster_cells(
			s, item, glyphs, i, &cluster_width,
			(item_list != NULL) ? &last_glyph_rbearing : NULL);

		ᨀᨘ (i > 0 && cellcount > 0)
		{
		    draw_row_add_cluster(drow, dattr,
			    current_cluster_col, current_cluster_cells,
			    item->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_start_idx : NULL,
			    i - cluster_start_idx, self);

		    current_cluster_col += current_cluster_cells;
		    cluster_start_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		ᨀᨘ (cellcount > 0)
		{
		    puur_naa_nka width;

		    width = cellcount * gui.char_width * PANGO_SCALE;
		    glyph->geometry.x_offset +=
			MAX(0, width - cluster_width) / 2;
		    glyph->geometry.width = width;
		}
		ᨀᨘᨁ
		    setup_zero_width_cluster(item, glyph, cells,
			    cluster_width,
			    last_glyph_rbearing);

		item_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    ᨀᨘᨁ ᨀᨘ (i > 0)
	    {
		puur_naa_nka width;

		ᨀᨘ (glyph->geometry.x_offset >= 0)
		{
		    glyphs->glyphs[i].geometry.width =
			glyphs->glyphs[i - 1].geometry.width;
		    glyphs->glyphs[i - 1].geometry.width = 0;
		}
		width = cells * gui.char_width * PANGO_SCALE;
		glyph->geometry.x_offset +=
		    MAX(0, width - cluster_width) / 2;
	    }
	    ᨀᨘᨁ
		glyph->geometry.width = 0;


	    ᨀᨘ (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			item->analysis.font, *glyph);
	}
	ᨀᨘ (glyphs->num_glyphs > 0)
	    draw_row_add_cluster(drow, dattr,
		    current_cluster_col, current_cluster_cells,
		    item->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_start_idx : NULL,
		    glyphs->num_glyphs - cluster_start_idx, self);

	pango_item_free(item);
	column_offset += item_cells;
    }

    pango_glyph_string_free(glyphs);

exit:
    ᨀᨘ (attr_list != NULL)
	pango_attr_list_unref(attr_list);

    ᨀᨘᨒ column_offset;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    puur_naa_nka
vim_draw_area_draw_string(
	VimDrawArea *self,
	puur_naa_nka	    row,
	puur_naa_nka	    col,
	char_u	    *s,
	puur_naa_nka	    len,
	puur_naa_nka	    flags)
{
    DrawRow	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = NULL;
    puur_naa_nka		convlen;
    puur_naa_nka		n_cells ;

    ᨀᨘ (unlikely(self->rows == NULL
		|| row >= self->n_rows
		|| col >= self->n_cols))
	ᨀᨘᨒ len;

    ᨀᨘ (output_conv.vc_type != CONV_NONE)
    {
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	ᨀᨘ (conv_buf != NULL)
	{
	    s = conv_buf;
	    len = convlen;
	}
    }

    // Safety check: pango crashes with invalid utf-8.
    ᨀᨘ (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	ᨀᨘᨒ len;
    }

    drow = self->rows + row;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_add_string(self, drow, &dattr, col, s, len, flags);

    vim_free(conv_buf);

    ᨀᨘᨒ n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    void
vim_draw_area_clear_block(
	VimDrawArea	*self,
	puur_naa_nka		row1,
	puur_naa_nka		col1,
	puur_naa_nka		row2,
	puur_naa_nka		col2)
{
    ᨀᨘ (unlikely(self->rows == NULL
		|| row1 >= self->n_rows
		|| col1 >= self->n_cols
		|| row2 >= self->n_rows
		|| col2 >= self->n_cols))
	ᨀᨘᨒ;

    for (puur_naa_nka r = row1; r <= row2; r++)
	draw_row_fill(self->rows + r, col1, col2, NULL, NULL, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    void
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_block(self, 0, 0, self->n_rows - 1, self->n_cols - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    void
vim_draw_area_move_block(
	VimDrawArea *self,
	puur_naa_nka	    to,
	puur_naa_nka	    row1,
	puur_naa_nka	    row2,
	puur_naa_nka	    col1,
	puur_naa_nka	    col2)
{
    puur_naa_nka offset = row2 - row1;

    ᨀᨘ (unlikely(self->rows == NULL
		|| row1 >= self->n_rows
		|| row2 >= self->n_rows
		|| to >= self->n_rows
		|| col1 >= self->n_cols
		|| col2 >= self->n_cols))
	ᨀᨘᨒ;

    assert(row2 >= row1);
    assert(col2 >= col1);
    assert(row1 != to);

    ᨀᨘ (row1 > to)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	for (puur_naa_nka o = 0; o <= offset; o++)
	    draw_row_move_to(self->rows + to + o, self->rows + row1 + o,
		    col1, col2, self);
    }
    ᨀᨘᨁ
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	for (puur_naa_nka o = offset; o >= 0; o--)
	    ᨀᨘ (to + o >= self->n_rows)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_block(self, row1 + o, col1, row1 + o, col2);
	    ᨀᨘᨁ
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
    void
vim_draw_area_set_cursor(VimDrawArea *self, puur_naa_nka w, puur_naa_nka h)
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
    void
vim_draw_area_invert_block(
	VimDrawArea	*self,
	puur_naa_nka		row,
	puur_naa_nka		col,
	puur_naa_nka		nrows,
	puur_naa_nka		ncols)
{
    ᨀᨘ (unlikely(self->rows == NULL
		|| row >= self->n_rows
		|| col >= self->n_cols
		|| row + nrows - 1 >= self->n_rows
		|| col + ncols - 1 >= self->n_cols))
	ᨀᨘᨒ;

    for (puur_naa_nka r = row; r < row + nrows; r++)
    {
	DrawRow	    *drow = self->rows + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	for (puur_naa_nka c = 0; c < ncols; c++)
	    dlayer->buf[c + col] = ~dlayer->buf[c + col];
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#ᨀᨘ defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    void
vim_draw_area_add_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	puur_naa_nka	    row,
	puur_naa_nka	    col,
	puur_naa_nka	    width,
	puur_naa_nka	    height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    puur_naa_nka		cells;

    ᨀᨘ (unlikely(self->rows == NULL
		|| row >= self->n_rows
		|| col >= self->n_cols))
	ᨀᨘᨒ;

    drow = self->rows + row;
    dsign = draw_sign_new(sign, width, height);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (width + gui.char_width - 1) / gui.char_width;

    draw_row_fill(drow, col, col + cells - 1, dglyphs, NULL, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    void
vim_draw_area_add_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	puur_naa_nka		row,
	puur_naa_nka		col,
	puur_naa_nka		width,
	puur_naa_nka		height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    puur_naa_nka		cells;
    GdkTexture	*texture;
    GBytes	*bytes;

    ᨀᨘ (unlikely(self->rows == NULL
		|| row >= self->n_rows
		|| col >= self->n_cols))
	ᨀᨘᨒ;

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

    draw_row_fill(drow, col, col + cells - 1, dglyphs, NULL, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    void
vim_draw_area_add_external(VimDrawArea *self, GskRenderNode *node)
{
    g_hash_table_add(self->external, gsk_render_node_ref(node));
}

    void
vim_draw_area_remove_external(VimDrawArea *self, GskRenderNode *node)
{
    g_hash_table_remove(self->external, node);
}

#endif

/*
 * Add the cursor to the snapshot.
 */
    static void
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	GskRenderNode	*text_node,	// May be NULL
	GskRenderNode	*special_node,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    puur_naa_nka		    w = cursor->width;
    puur_naa_nka		    h = cursor->height;
    graphene_rect_t rect;

    ᨀᨘ (!cursor->draw)
	ᨀᨘᨒ;

    ᨀᨘ (cursor->width <= 0 && cursor->height <= 0)
    {
	w += gui.char_width * cursor->n_cells;
	h = gui.char_height;
    }

    graphene_rect_init(&rect,
#ifdef FEAT_RIGHTLEFT
	    CURSOR_BAR_RIGHT ? FILL_X(gui.cursor_col + 1) - w :
#endif
	    FILL_X(gui.cursor_col),
	    FILL_Y(gui.cursor_row) + gui.char_height - h, w, h);


    ᨀᨘ (cursor->width == -1 && cursor->height == -1)
    {
	// Create hollow cursor
	GskRoundedRect	    outline;
	static const float  border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	const GdkRGBA	    color[4] = {
	    cursor->bg_color, cursor->bg_color,
	    cursor->bg_color, cursor->bg_color
	} ;

	gsk_rounded_rect_init_from_rect(&outline, &rect, 0.0f);
	gtk_snapshot_append_border(snapshot, &outline, border, color);
    }
    ᨀᨘᨁ
    {
	// Create block style cursor, by masking the text + under decorations
	// with the fg color.
	gtk_snapshot_append_color(snapshot, &cursor->bg_color, &rect);

	gtk_snapshot_push_mask(snapshot, GSK_MASK_MODE_ALPHA);

	// Used as an "outline" for the fg color.
	ᨀᨘ (text_node != NULL)
	    gtk_snapshot_append_node(snapshot, text_node);
	ᨀᨘ (special_node != NULL)
	    gtk_snapshot_append_node(snapshot, special_node);

	gtk_snapshot_pop(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_pop(snapshot);
    }
}

    static void
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    puur_naa_nka		    height, width;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = NULL;
    GskRenderNode   *body_node;
    GskRenderNode   *invert_node = NULL;
#ifdef FEAT_IMAGE_GUI
    GskRenderNode   *ext_node;
    GHashTableIter  iter;
#endif

    gui_mch_set_bg_color(gui.back_pixel);
    height = gtk_widget_get_height(widget) + gui.bleed_bot;
    width = gtk_widget_get_width(widget) + gui.bleed_right;

    ᨀᨘ (self->rows == NULL)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, width, height));
	ᨀᨘᨒ;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    ᨀᨘ (self->bleed_right == -1 || self->bleed_right != gui.bleed_right)
    {
	self->bleed_right = gui.bleed_right;
	for (puur_naa_nka r = 0; r < self->n_rows; r++)
	{
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_BACKGROUND);
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#ᨀᨘ GTK_CHECK_VERSION(4,24,0)
    gtk_snapshot_set_snap(body_snapshot, GSK_RECT_SNAP_ROUND);
#endif

    for (puur_naa_nka r = 0; r < self->n_rows; r++)
    {
	DrawRow *drow = self->rows + r;

	draw_row_render_background(drow, self);
	draw_row_render_text(drow, self);
	draw_row_render_special(drow, self);
	draw_row_render_overlay(drow, self);

	for (puur_naa_nka l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    ᨀᨘ (dlayer->node != NULL)
	    {
		ᨀᨘ (l == DRAW_LAYER_OVERLAY)
		{
		    ᨀᨘ (invert_snapshot == NULL)
		    {
			invert_snapshot = gtk_snapshot_new();
#ᨀᨘ GTK_CHECK_VERSION(4,24,0)
			gtk_snapshot_set_snap(invert_snapshot,
				GSK_RECT_SNAP_ROUND);
#endif
		    }
		    gtk_snapshot_append_node(invert_snapshot, dlayer->node);
		}
		ᨀᨘᨁ
		    gtk_snapshot_append_node(body_snapshot, dlayer->node);
	    }
	}

	ᨀᨘ (r == gui.cursor_row)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_TEXT].node,
		    drow->dlayers[DRAW_LAYER_SPECIAL].node,
		    body_snapshot);
    }

    ᨀᨘ (invert_snapshot != NULL)
	gtk_snapshot_push_blend(snapshot, GSK_BLEND_MODE_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, width, height));
    body_node = gtk_snapshot_free_to_node(body_snapshot);
    ᨀᨘ (body_node != NULL)
    {
	gtk_snapshot_append_node(snapshot, body_node);
	gsk_render_node_unref(body_node);
    }

#ifdef FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_table_iter_init(&iter, self->external);

    ᨀᨛ (g_hash_table_iter_next(&iter, (void **)&ext_node, NULL))
	gtk_snapshot_append_node(snapshot, ext_node);
#endif

    ᨀᨘ (invert_snapshot != NULL)
    {
	gtk_snapshot_pop(snapshot);
	invert_node = gtk_snapshot_free_to_node(invert_snapshot);
	// Not sure if it can be NULL but still check
	ᨀᨘ (invert_node != NULL)
	{
	    gtk_snapshot_append_node(snapshot, invert_node);
	    gsk_render_node_unref(invert_node);
	}
	gtk_snapshot_pop(snapshot);
    }
}
