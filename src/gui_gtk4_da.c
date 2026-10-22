/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#অন্তর্ভাৱ "vim.h"

#অন্তর্ভাৱ <gtk/gtk.h>
#অন্তর্ভাৱ "gui_gtk4_da.h"


#যদি defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Used for sign icons and netbeans multisign indicator. It is in a DrawGlyphs
 * so that it can move/be manipulated with the text.
 */
প্রকারনির্দেশ সংরচনা
{
    GdkTexture	*texture;
    পূর্ণাঙ্ক		width;
    পূর্ণাঙ্ক		height;
} DrawSign;
#endif

/*
 * Temporary struct when drawing string representing visual attributes of a cell
 */
প্রকারনির্দেশ সংরচনা
{
    uint32_t	bg_color;
    uint32_t	fg_color;   // Only used for underlines
    uint32_t	sp_color;
    পূর্ণাঙ্ক		draw_flags; // DRAW_* flags, only DRAW_UNDERL, DRAW_UNDERC, and
			    // DRAW_STRIKE are used.
} DrawAttr;

/*
 * Represents the glyphs of a single cell.
 */
প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক refcount;

    uint32_t	fg_color;
    PangoFont	*font;
    // If "font" is NULL, "dsign" is used, otherwise "glyphs" is used.
    সঙ্ঘ
    {
	PangoGlyphInfo  *glyphs; // May be NULL
#যদি defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	DrawSign	*dsign;
#endif
    };
    পূর্ণাঙ্ক n_glyphs;
} DrawGlyphs;

প্রকারনির্দেশ সংরচনা
{
    DrawGlyphs *dglyphs;
} DrawCell;

প্রকারনির্দেশ গণনা
{
    DRAW_LAYER_BACKGROUND,  // Background color, buf stores bg color
    DRAW_LAYER_TEXT,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

প্রকারনির্দেশ সংরচনা
{
    Gsk_Render_গ্ৰন্থি   *গ্ৰন্থি;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#define UNDERC_OFFSET(n_cols) (0)
#define STRIKE_OFFSET(n_cols) (n_cols)

প্রকারনির্দেশ সংরচনা
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    পূর্ণাঙ্ক		n_cells; // For convenience
    পূর্ণাঙ্ক		row;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_গ্ৰন্থি *underl_mask;
    Gsk_Render_গ্ৰন্থি *underc_mask;
    Gsk_Render_গ্ৰন্থি *strike_mask;
} DrawRow;

#define ঢোকাও_PANGO_ATTR(Attribute, AttrList, Start, End) \
    কৰা { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->start_index = (Start); \
	tmp_attr_->end_index = (End); \
	pango_attr_list_ঢোকাও((AttrList), tmp_attr_); \
    } যেতিয়ালৈ (FALSE)
#define CELLS2PANGO(c) ((c) * gui.char_width * PANGO_SCALE)

/*
 * Attributes of cursor
 */
প্রকারনির্দেশ সংরচনা
{
    gboolean draw; // If cursor should be drawn
    পূর্ণাঙ্ক width;
    পূর্ণাঙ্ক height;
    পূর্ণাঙ্ক n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

সংরচনা _VimDrawArea
{
    GtkWidget parent;

    DrawRow *rows;

    পূর্ণাঙ্ক n_rows;
    পূর্ণাঙ্ক	n_cols;

    পূর্ণাঙ্ক	bleed_ডান;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*গ্ৰন্থি_buf;

    DrawCursor cursor;

#ifdef FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHashTable *external;
#endif
};

স্থির শূন্য draw_row_init(DrawRow *drow, পূর্ণাঙ্ক row, পূর্ণাঙ্ক cols);
স্থির শূন্য draw_row_clear(DrawRow *drow);
স্থির শূন্য draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t);
স্থির শূন্য vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

স্থির GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    স্থির শূন্য
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    বাবে (পূর্ণাঙ্ক r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    g_free(self->rows);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->গ্ৰন্থি_buf, TRUE);

#ifdef FEAT_IMAGE_GUI
    g_hash_table_unref(self->external);
#endif

    G_OBJECT_CLASS(vim_draw_area_parent_class)->finalize(obj);
}

    স্থির শূন্য
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    স্থির শূন্য
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_ডান = -1;
#ifdef FEAT_IMAGE_GUI
    self->external = g_hash_table_new_full(g_direct_hash, g_direct_equal,
	    রিক্ত, (GDestroyNotify)gsk_render_গ্ৰন্থি_unref);
#endif
    self->glyph_buf = g_array_new(FALSE, FALSE, পরিমাণ(PangoGlyphInfo));
    self->গ্ৰন্থি_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_গ্ৰন্থি_unref);

}

    GtkWidget *
vim_draw_area_new(শূন্য)
{
    ঘূৰাই g_object_new(VIM_TYPE_DRAW_AREA, রিক্ত);
}

/*
 * Set the size of the draw area to "rows" and "cols".
 */
    শূন্য
vim_draw_area_set_size(VimDrawArea *self, পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols)
{
    যদি (self->rows != রিক্ত && self->n_rows == rows && self->n_cols == cols)
	ঘূৰাই;
    যদি (rows == 0 || cols == 0)
	ঘূৰাই;

    vim_draw_area_clear(self);

    বাবে (পূর্ণাঙ্ক r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    self->rows = g_realloc_n(self->rows, rows, পরিমাণ(DrawRow));
    বাবে (পূর্ণাঙ্ক r = 0; r < rows; r++)
	draw_row_init(self->rows + r, r, cols);

    self->n_rows = rows;
    self->n_cols = cols;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    স্থির uint32_t
rgba_to_u32(ধ্রুৱ GdkRGBA *rgba)
{
    সঙ্ঘ { uint32_t val; uint8_t bytes[4]; } color;

    color.bytes[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.bytes[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.bytes[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.bytes[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    ঘূৰাই color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    স্থির GdkRGBA
u32_to_rgba(uint32_t val)
{
    সঙ্ঘ { uint32_t val; uint8_t bytes[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.bytes[0] / 255.0;
    color.green = packed.bytes[1] / 255.0;
    color.blue  = packed.bytes[2] / 255.0;
    color.alpha = packed.bytes[3] / 255.0;
    ঘূৰাই color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    স্থির gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    সঙ্ঘ { uint32_t val; uint8_t bytes[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.bytes[0] << 16) |
	((guicolor_T)packed.bytes[1] << 8)  |
	packed.bytes[2];
    ঘূৰাই res == rgb;
}

/*
 * Checks if a RGBA pixel buffer is fully transparent.
 */
    স্থির অন্তঃস্থ gboolean
rgba_is_transparent(ধ্রুৱ uint32_t *buf, পূর্ণাঙ্ক n_pixels)
{
    // Could also compare 2 pixels at once using a 64 bit integer, but not sure
    // if that might cause alignment issues...
    বাবে (পূর্ণাঙ্ক i = 0; i < n_pixels; i++)
	যদি (buf[i] != 0)
	    ঘূৰাই FALSE;
    ঘূৰাই TRUE;
}

/*
 * Return TRUE if "glyph" has ink (not whitespace).
 */
    স্থির gboolean
glyph_has_ink(PangoFont *font, PangoGlyphInfo glyph)
{
    PangoRectangle ink;

    pango_font_get_glyph_extents (font, glyph.glyph, &ink, রিক্ত);

    ঘূৰাই ink.width > 0 && ink.height > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    স্থির শূন্য
apply_wide_font_attr(char_u *s, পূর্ণাঙ্ক len, PangoAttrList *attr_list)
{
    char_u  *start = রিক্ত;
    char_u  *p;
    পূর্ণাঙ্ক	    uc;

    বাবে (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	যদি (start == রিক্ত)
	{
	    যদি (uc >= 0x80 && utf_char2cells(uc) == 2)
		start = p;
	}
	অন্যথা যদি (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    ঢোকাও_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, start - s, p - s);
	    start = রিক্ত;
	}
    }

    যদি (start != রিক্ত)
	ঢোকাও_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, start - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    স্থির পূর্ণাঙ্ক
গুনতি_cluster_cells(
	char_u		    *s,
	PangoItem	    *item,
	PangoGlyphString    *glyphs,
	পূর্ণাঙ্ক		    i,
	পূর্ণাঙ্ক		    *cluster_width,
	পূর্ণাঙ্ক		    *last_glyph_rbearing)
{
    char_u  *p;
    পূর্ণাঙ্ক	    next;
    পূর্ণাঙ্ক	    start, end;
    পূর্ণাঙ্ক	    width;
    পূর্ণাঙ্ক	    uc;
    পূর্ণাঙ্ক	    cellcount = 0;

    width = glyphs->glyphs[i].geometry.width;

    বাবে (next = i + 1; next < glyphs->অঙ্ক_glyphs; ++next)
    {
	যদি (glyphs->glyphs[next].attr.is_cluster_start)
	    ভঙ্গ;
	অন্যথা যদি (glyphs->glyphs[next].geometry.width > width)
	    width = glyphs->glyphs[next].geometry.width;
    }

    start = item->offset + glyphs->log_clusters[i];
    end   = item->offset + ((next < glyphs->অঙ্ক_glyphs) ?
	    glyphs->log_clusters[next] : item->length);

    বাবে (p = s + start; p < s + end; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	যদি (uc < 0x80)
	    ++cellcount;
	অন্যথা যদি (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    যদি (last_glyph_rbearing != রিক্ত
	    && cellcount > 0 && next == glyphs->অঙ্ক_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(item->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, রিক্ত);

	যদি (PANGO_RBEARING(ink_rect) > 0)
	    *last_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    যদি (cellcount > 0)
	*cluster_width = width;

    ঘূৰাই cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    স্থির শূন্য
setup_zero_width_cluster(
	PangoItem	*item,
	PangoGlyphInfo	*glyph,
	পূর্ণাঙ্ক		last_cellcount,
	পূর্ণাঙ্ক		last_cluster_width,
	পূর্ণাঙ্ক		last_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    পূর্ণাঙ্ক		    width;

    width = last_cellcount * gui.char_width * PANGO_SCALE;
    glyph->geometry.x_offset = -width + MAX(0, width - last_cluster_width) / 2;
    glyph->geometry.width = 0;

    pango_font_get_glyph_extents(item->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    যদি (ink_rect.x < 0)
    {
	glyph->geometry.x_offset += last_glyph_rbearing;
	glyph->geometry.y_offset  = logical_rect.height
	    - (gui.char_height - p_linespace) * PANGO_SCALE;
    }
    অন্যথা
	glyph->geometry.x_offset = -width + MAX(0, width - ink_rect.width) / 2;
}

#যদি defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    স্থির DrawSign *
draw_sign_new(GdkTexture *texture, পূর্ণাঙ্ক width, পূর্ণাঙ্ক height)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->width = width;
    dsign->height = height;

    ঘূৰাই dsign;
}

    স্থির শূন্য
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#endif

    স্থির শূন্য
draw_attr_init(
	DrawAttr	*dattr,
	ধ্রুৱ GdkRGBA	*bg_color,
	ধ্রুৱ GdkRGBA	*fg_color,
	ধ্রুৱ GdkRGBA	*sp_color,
	পূর্ণাঙ্ক		draw_flags)
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
    স্থির শূন্য
draw_attr_apply(DrawAttr *dattr, পূর্ণাঙ্ক col, DrawRow *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + col;
    uint32_t	*text_cell = drow->dlayers[DRAW_LAYER_TEXT].buf + col;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    পূর্ণাঙ্ক		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + UNDERC_OFFSET(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + STRIKE_OFFSET(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    যদি (dattr != রিক্ত)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.back_pixel, bg_color);
    }

    যদি (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	যদি (bg_is_default)
	    *bg_cell = 0;
	অন্যথা
	    *bg_cell = bg_color;
    }
    যদি ((draw_flags & DRAW_UNDERL && *text_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *text_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);
	যদি (draw_flags & DRAW_UNDERL)
	    *text_cell = fg_color;
	অন্যথা
	    *text_cell = 0;
    }
    যদি ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	যদি (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	অন্যথা
	    *underc_cell = 0;
    }
    যদি ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	যদি (draw_flags & DRAW_STRIKE)
	    *strike_cell = sp_color;
	অন্যথা
	    *strike_cell = 0;
    }
}

    স্থির DrawGlyphs *
draw_glyphs_new(
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	পূর্ণাঙ্ক		n_glyphs,
	uint32_t	fg_color)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->font = g_object_ref(font);
    dglyphs->glyphs = g_memdup2(glyphs, পরিমাণ(*glyphs) * n_glyphs);
    dglyphs->n_glyphs = n_glyphs;
    dglyphs->fg_color = fg_color;

    ঘূৰাই dglyphs;
}

#যদি defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw glyphs that shows the given sign. Note that ownership of
 * "dsign" is taken.
 */
    স্থির DrawGlyphs *
draw_glyphs_new_sign(DrawSign *dsign)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->dsign = dsign;

    ঘূৰাই dglyphs;
}
#endif

    স্থির DrawGlyphs *
draw_glyphs_ref(DrawGlyphs *dglyphs)
{
    dglyphs->refcount++;
    ঘূৰাই dglyphs;
}

    স্থির শূন্য
draw_glyphs_unref(DrawGlyphs *dglyphs)
{
    যদি (--dglyphs->refcount <= 0)
    {
#যদি defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	যদি (dglyphs->font == রিক্ত)
	    draw_sign_free(dglyphs->dsign);
	অন্যথা
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
    স্থির gboolean
draw_glyphs_same(DrawGlyphs *a, DrawGlyphs *b)
{
    যদি (a == রিক্ত && b == রিক্ত)
	ঘূৰাই TRUE;
    যদি (a == রিক্ত || b == রিক্ত)
	ঘূৰাই FALSE;
    যদি (a->fg_color != b->fg_color)
	ঘূৰাই FALSE;
    যদি (a->n_glyphs != b->n_glyphs || a->font != b->font)
	ঘূৰাই FALSE;
    বাবে (পূর্ণাঙ্ক i = 0; i < a->n_glyphs; i++)
	যদি (a->glyphs[i].glyph != b->glyphs[i].glyph
		|| a->glyphs[i].geometry.width != b->glyphs[i].geometry.width
		|| a->glyphs[i].geometry.x_offset != b->glyphs[i].geometry.x_offset
		|| a->glyphs[i].geometry.y_offset != b->glyphs[i].geometry.y_offset
		|| a->glyphs[i].attr.is_cluster_start != b->glyphs[i].attr.is_cluster_start)
	    ঘূৰাই FALSE;
    ঘূৰাই TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    স্থির Gsk_Render_গ্ৰন্থি *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	পূর্ণাঙ্ক	    row,
	VimDrawArea *da,
	পূর্ণাঙ্ক	    off,
	gboolean    bleed)
{
    GBytes	    *bytes;
    GdkTexture	    *texture;
    Gsk_Render_গ্ৰন্থি   *গ্ৰন্থি;
    পূর্ণাঙ্ক		    height;

#যদি GTK_CHECK_VERSION(4,24,0)
    height = gui.char_height;
#অন্যথা
    height = gui.char_height + 1;
#endif

    যদি (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		পরিমাণ(uint32_t) * da->n_cols);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_cols - 1)),
		পরিমাণ(uint32_t)); // Add bleed using last pixel of buffer
	bytes = g_byte_array_free_to_bytes(arr);
    }
    অন্যথা
	bytes = g_bytes_new(dlayer->buf + off, পরিমাণ(uint32_t) * da->n_cols);

    texture = gdk_memory_texture_new(da->n_cols + bleed, 1,
	    GDK_MEMORY_R8G8B8A8, bytes, (da->n_cols + bleed) * 4);
    g_bytes_unref(bytes);

    // Scale texture to actual size
    গ্ৰন্থি = gsk_texture_scale_গ্ৰন্থি_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		(da->n_cols + bleed) * gui.char_width, gui.char_height + bleed),
	    GSK_SCALING_FILTER_NEAREST);
    যদি (bleed)
    {
	Gsk_Render_গ্ৰন্থি *new;

	new = gsk_clip_গ্ৰন্থি_new(গ্ৰন্থি,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		    da->n_cols * gui.char_width + da->bleed_ডান, height));
	gsk_render_গ্ৰন্থি_unref(গ্ৰন্থি);
	গ্ৰন্থি = new;
    }
    g_object_unref(texture);
    ঘূৰাই গ্ৰন্থি;
}

    স্থির শূন্য
draw_row_init(DrawRow *drow, পূর্ণাঙ্ক row, পূর্ণাঙ্ক cols)
{
    drow->cells = g_malloc0_n(cols, পরিমাণ(DrawCell));
    drow->n_cells = cols;
    drow->row = row;
    drow->underl_mask = drow->underc_mask = drow->strike_mask = রিক্ত;

    memset(drow->dlayers, 0, পরিমাণ(DrawLayer) * N_DRAW_LAYERS);
    বাবে (পূর্ণাঙ্ক l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	যদি (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(cols * 2, 4);
	অন্যথা
	    dlayer->buf = g_malloc0_n(cols, 4);
    }
}

    স্থির শূন্য
draw_row_clear(DrawRow *drow)
{
    g_free(drow->cells);
    বাবে (পূর্ণাঙ্ক l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	যদি (dlayer->গ্ৰন্থি != রিক্ত)
	    gsk_render_গ্ৰন্থি_unref(dlayer->গ্ৰন্থি);
	g_free(dlayer->buf);
    }
    g_clear_pointer(&drow->underl_mask, gsk_render_গ্ৰন্থি_unref);
    g_clear_pointer(&drow->underc_mask, gsk_render_গ্ৰন্থি_unref);
    g_clear_pointer(&drow->strike_mask, gsk_render_গ্ৰন্থি_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    স্থির শূন্য
draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_pointer(&dlayer->গ্ৰন্থি, gsk_render_গ্ৰন্থি_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    স্থির শূন্য
draw_row_uninvert_cell(DrawRow *drow, পূর্ণাঙ্ক col)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    যদি (buf[col] == 0xffffffff)
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[col] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    স্থির শূন্য
draw_row_set_cell(DrawRow *drow, পূর্ণাঙ্ক col, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + col;

    যদি (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    যদি (dcell->dglyphs != dglyphs)
    {
	যদি (dcell->dglyphs != রিক্ত)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == রিক্ত ? রিক্ত : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, col, drow);
    draw_row_uninvert_cell(drow, col);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    স্থির শূন্য
draw_row_fill(
	DrawRow	    *drow,
	পূর্ণাঙ্ক	    col1,
	পূর্ণাঙ্ক	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    বাবে (পূর্ণাঙ্ক c = col1; c <= col2; c++)
	draw_row_set_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    যদি (gui.cursor_row == drow->row && gui.cursor_col >= col1
	    && gui.cursor_col <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    স্থির শূন্য
draw_row_move_to(
	DrawRow	    *dest_row,
	DrawRow	    *src_row,
	পূর্ণাঙ্ক	    col1,
	পূর্ণাঙ্ক	    col2,
	VimDrawArea *da)
{
    পূর্ণাঙ্ক move_size = (col2 - col1 + 1) * পরিমাণ(DrawCell);

    draw_row_fill(dest_row, col1, col2, রিক্ত, রিক্ত, &da->cursor);
    memmove(dest_row->cells + col1, src_row->cells + col1, move_size);
    // NULL the draw cells so we don't double unreference.
    memset(src_row->cells + col1, 0, move_size);
    draw_row_dirty_layer(src_row, DRAW_LAYER_TEXT);

    // Move the layer buffers
    move_size = (col2 - col1 + 1) * 4;
    বাবে (পূর্ণাঙ্ক l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_row->dlayers[l].buf;
	uint32_t *dest = dest_row->dlayers[l].buf;

	যদি (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_OFFSET(dest_row->n_cells),
		    src + col1 + UNDERC_OFFSET(src_row->n_cells), move_size);
	    memmove(dest + col1 + STRIKE_OFFSET(dest_row->n_cells),
		    src + col1 + STRIKE_OFFSET(src_row->n_cells), move_size);
	}
	অন্যথা
	    memmove(dest + col1, src + col1, move_size);
	draw_row_dirty_layer(dest_row, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    স্থির শূন্য
draw_row_render_background(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    যদি (!dlayer->dirty)
	ঘূৰাই;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    যদি (rgba_is_transparent(dlayer->buf, da->n_cols))
	ঘূৰাই;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->গ্ৰন্থি = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    স্থির শূন্য
draw_row_ensure_decor(DrawRow *drow, পূর্ণাঙ্ক flags)
{
    যদি (flags & DRAW_UNDERL && drow->underl_mask == রিক্ত)
	drow->underl_mask = gsk_color_গ্ৰন্থি_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->row + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    যদি (flags & DRAW_STRIKE && drow->strike_mask == রিক্ত)
	drow->strike_mask = gsk_color_গ্ৰন্থি_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->row) + (পূর্ণাঙ্ক)(gui.char_height / 2),
		    FILL_X(drow->n_cells), 1));

    যদি (flags & DRAW_UNDERC && drow->underc_mask == রিক্ত)
    {
	স্থির ধ্রুৱ পূর্ণাঙ্ক val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	পূর্ণাঙ্ক y = FILL_Y(drow->row + 1) - 1;
	পূর্ণাঙ্ক x_start = FILL_X(0);
	পূর্ণাঙ্ক x_end = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_গ্ৰন্থি *child = রিক্ত;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#যদি GTK_CHECK_VERSION(4, 14, 0)
	GskPathBuilder	*builder;
	GskPath		*path;
	GskStroke       *stroke;
	Gsk_Render_গ্ৰন্থি	*color_গ্ৰন্থি;
	graphene_rect_t bounds;

	builder = gsk_path_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_path_builder_move_to(builder, -1, y - val[7] + 0.5);
	gsk_path_builder_সাৰি_to(builder, 0, y - val[0] + 0.5);

	বাবে (পূর্ণাঙ্ক i = 1; i < 8; i++)
	    gsk_path_builder_সাৰি_to(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_path_builder_সাৰি_to(builder, 8, y - val[0] + 0.5);
	gsk_path_builder_সাৰি_to(builder, 9, y - val[1] + 0.5);

	path = gsk_path_builder_free_to_path(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_path_get_stroke_bounds(path, stroke, &bounds);
	color_গ্ৰন্থি = gsk_color_গ্ৰন্থি_new(&white_rgba, &bounds);
	child = gsk_stroke_গ্ৰন্থি_new(color_গ্ৰন্থি, path, stroke);

	gsk_stroke_free(stroke);
	gsk_path_unref(path);
	gsk_render_গ্ৰন্থি_unref(color_গ্ৰন্থি);
#অন্যথা
	cairo_t *cr;

	child = gsk_cairo_গ্ৰন্থি_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_গ্ৰন্থি_get_draw_context(child);

	cairo_set_সাৰি_width(cr, 1.0);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_to(cr, -1, y - val[7] + 0.5);
	cairo_সাৰি_to(cr, 0, y - val[0] + 0.5);

	বাবে (পূর্ণাঙ্ক i = 1; i < 8; ++i)
	    cairo_সাৰি_to(cr, i, y - val[i] + 0.5);

	cairo_সাৰি_to(cr, 8, y - val[0] + 0.5);
	cairo_সাৰি_to(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#endif
	drow->underc_mask = gsk_repeat_গ্ৰন্থি_new(
		&GRAPHENE_RECT_INIT(x_start, y - 3, x_end - x_start, 5),
		child, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_গ্ৰন্থি_unref(child);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    স্থির শূন্য
draw_row_render_text(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_TEXT;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = রিক্ত;
    পূর্ণাঙ্ক		start_col = 0;
    পূর্ণাঙ্ক		empty_cells = 0;
    uint32_t	cur_fg = 0;
    পূর্ণাঙ্ক		c;

    যদি (!dlayer->dirty)
	ঘূৰাই;

    glyph_buf = da->glyph_buf;
    nodes = da->গ্ৰন্থি_buf;

    g_array_set_size(glyph_buf, 0);
    g_ptr_array_set_size(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#define FLUSH_গ্ৰন্থি() \
    কৰা { \
	যদি (cur_font != রিক্ত) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_গ্ৰন্থি	*গ্ৰন্থি; \
	    GdkRGBA		fg_color = u32_to_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->data; \
	    glyph_str.অঙ্ক_glyphs = glyph_buf->len; \
	    গ্ৰন্থি = gsk_text_গ্ৰন্থি_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_POINT_INIT(TEXT_X(start_col), \
			TEXT_Y(drow->row))); \
	    যদি (গ্ৰন্থি != রিক্ত) \
		g_ptr_array_add(nodes, গ্ৰন্থি); \
	    g_array_set_size(glyph_buf, 0); \
	    cur_font = রিক্ত; \
	} \
    } যেতিয়ালৈ (FALSE)

    বাবে (c = 0; c < da->n_cols; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	পূর্ণাঙ্ক	    prev_len;

	যদি (dglyphs == রিক্ত)
	{
	    empty_cells++;
	    অনুৱর্ত;
	}
#যদি defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	অন্যথা যদি (dglyphs->font == রিক্ত)
	{
	    // Add sign icon
	    Gsk_Render_গ্ৰন্থি   *snode;
	    পূর্ণাঙ্ক		    w;

	    FLUSH_গ্ৰন্থি();

	    snode = gsk_texture_scale_গ্ৰন্থি_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->row),
			dglyphs->dsign->width, dglyphs->dsign->height),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->width + gui.char_width - 1) / gui.char_width;

	    যদি (snode != রিক্ত)
		g_ptr_array_add(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    অনুৱর্ত;
	}
#endif
	অন্যথা যদি (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_গ্ৰন্থি();
	    cur_font = dglyphs->font;
	    start_col = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	যদি (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    অনুৱর্ত;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	যদি (empty_cells > 0)
	{
	    যদি (prev_len > 0)
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

    FLUSH_গ্ৰন্থি();

#undef FLUSH_গ্ৰন্থি

    যদি (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_গ্ৰন্থি *source;
	Gsk_Render_গ্ৰন্থি *গ্ৰন্থি;

	draw_row_ensure_decor(drow, DRAW_UNDERL);
	যদি (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    source = draw_layer_get_texture(dlayer, drow->row, da, 0, FALSE);
	    গ্ৰন্থি = gsk_mask_গ্ৰন্থি_new(source, drow->underl_mask,
		    GSK_MASK_MODE_ALPHA);
	    gsk_render_গ্ৰন্থি_unref(source);
	    g_ptr_array_add(nodes, গ্ৰন্থি);
	}

	dlayer->গ্ৰন্থি = gsk_container_গ্ৰন্থি_new(
		(Gsk_Render_গ্ৰন্থি **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    স্থির শূন্য
draw_row_render_special(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_গ্ৰন্থি   *source;
    Gsk_Render_গ্ৰন্থি   *nodes[2];
    পূর্ণাঙ্ক		    n_nodes = 0;

    যদি (!dlayer->dirty)
	ঘূৰাই;

    draw_row_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    যদি (!rgba_is_transparent(dlayer->buf + UNDERC_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		UNDERC_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_গ্ৰন্থি_new(source, drow->underc_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_গ্ৰন্থি_unref(source);
    }

    যদি (!rgba_is_transparent(dlayer->buf + STRIKE_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		STRIKE_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_গ্ৰন্থি_new(source, drow->strike_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_গ্ৰন্থি_unref(source);
    }

    যদি (n_nodes == 1)
    {
	dlayer->গ্ৰন্থি = nodes[0];
    }
    অন্যথা যদি (n_nodes > 0)
    {
	dlayer->গ্ৰন্থি = gsk_container_গ্ৰন্থি_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	বাবে (পূর্ণাঙ্ক i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(nodes); i++)
	    gsk_render_গ্ৰন্থি_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    স্থির শূন্য
draw_row_render_overlay(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    যদি (!dlayer->dirty)
	ঘূৰাই;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    যদি (rgba_is_transparent(dlayer->buf, da->n_cols))
	ঘূৰাই;

    dlayer->গ্ৰন্থি = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    স্থির শূন্য
draw_row_add_cluster(
	DrawRow		*drow,
	DrawAttr	*dattr,
	পূর্ণাঙ্ক		col,
	পূর্ণাঙ্ক		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	পূর্ণাঙ্ক		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = রিক্ত;

    যদি (col + n_cells > da->n_cols)
	ঘূৰাই;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    যদি (glyphs != রিক্ত)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_row_fill(drow, col, col + n_cells - 1, dglyphs, dattr, &da->cursor);

    যদি (dglyphs != রিক্ত)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    স্থির পূর্ণাঙ্ক
vim_draw_area_add_string(
	VimDrawArea *self,
	DrawRow	    *drow,
	DrawAttr    *dattr,
	পূর্ণাঙ্ক	    col,
	char_u	    *s,
	পূর্ণাঙ্ক	    len,
	পূর্ণাঙ্ক	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = রিক্ত;
    পূর্ণাঙ্ক			start_idx = -1;
    GList		*item_list;
    পূর্ণাঙ্ক			column_offset = 0;
    পূর্ণাঙ্ক			cluster_width;
    পূর্ণাঙ্ক			last_glyph_rbearing;
    পূর্ণাঙ্ক			cells = 0;
    পূর্ণাঙ্ক			i = 0;
    gboolean		has_unicode = FALSE;
    gboolean		needs_shaping = FALSE;

    // Scan the string, and find spans where ligatures should not happen. We
    // must ensure that attribute boundaries do not split a base character from
    // its combining marks, as Pango breaks items at attribute boundaries.
    যেতিয়ালৈ (i < len)
    {
	পূর্ণাঙ্ক	clen = 1;
	পূর্ণাঙ্ক	is_composing = 0;
	bool	want_disable;

	যদি (s[i] >= 0x80)
	{
	    clen = utf_ptr2len(s + i);
	    is_composing = utf_iscomposing(utf_ptr2char(s + i));
	    has_unicode = TRUE;
	}

	যদি (is_composing)
	    // Do not change state mid-cluster. Inherit the previous character's
	    // state.
	    want_disable = (start_idx != -1);
	অন্যথা
	    // Base character: disable ligatures if it's an ASCII non-ligature
	    // char.
	    want_disable = (s[i] < 0x80 && !gui.ligatures_map[s[i]]);

	// If any base character actually wants ligature/contextual shaping,
	// the string must go through Pango's shaping engine -- the ASCII
	// fast path below draws cached glyphs one-by-one and can never
	// produce a ligature, regardless of 'guiligatures'.
	যদি (!is_composing && !want_disable)
	    needs_shaping = TRUE;

	যদি (want_disable)
	{
	    যদি (start_idx == -1)
		start_idx = i;
	}
	অন্যথা যদি (start_idx != -1)
	{
	    যদি (attr_list == রিক্ত)
		attr_list = pango_attr_list_new();
	    ঢোকাও_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, start_idx, i);
	    start_idx = -1;
	}
	i += clen;
    }

    // Fast path for pure ASCII: use cached glyph table. Skip this path when
    // there are non-ascii characters in the string, font attributes, or if
    // there's a possible ligature.
    যদি (!(draw_flags & DRAW_ITALIC)
	    && !((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != রিক্ত
	    && !has_unicode
	    && !needs_shaping)
    {
	বাবে (i = 0; i < len; ++i)
	{
	    PangoGlyphInfo  glyph = gui.ascii_glyphs->glyphs[2 * s[i]];
	    gboolean	    has_ink;

	    has_ink = glyph_has_ink(gui.ascii_font, glyph);

	    draw_row_add_cluster(drow, dattr, col + i, 1,
		    gui.ascii_font, has_ink ? &glyph : রিক্ত, 1, self);
	}

	column_offset = len;
	গচ্ছ exit;
    }

    glyphs = pango_glyph_string_new();

    যদি (attr_list == রিক্ত)
	attr_list = pango_attr_list_new();

    যদি (start_idx != -1)
	ঢোকাও_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, start_idx, len);

    cluster_width = PANGO_SCALE * gui.char_width;
    last_glyph_rbearing = PANGO_SCALE * gui.char_width;

    // If 'guifontwide' is set then use that for double-width characters.
    যদি (gui.wide_font != রিক্ত)
	apply_wide_font_attr(s, len, attr_list);

    যদি ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	ঢোকাও_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    যদি (draw_flags & DRAW_ITALIC)
	ঢোকাও_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    item_list = pango_itemize(gui.text_context,
	    (ধ্রুৱ ৱর্ণ *)s, 0, len, attr_list, রিক্ত);

    যেতিয়ালৈ (item_list != রিক্ত)
    {
	PangoItem   *item = item_list->data;
	পূর্ণাঙ্ক	    item_cells = 0;
	পূর্ণাঙ্ক	    cluster_start_idx = 0;
	পূর্ণাঙ্ক	    current_cluster_col = col + column_offset;
	পূর্ণাঙ্ক	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	item_list = g_list_delete_link(item_list, item_list);

	// Force LTR direction; Vim handles bidi on its own.
	item->analysis.level = (item->analysis.level + 1) & (~1U);

	pango_shape_full((ধ্রুৱ ৱর্ণ *)s + item->offset, item->length,
		(ধ্রুৱ ৱর্ণ *)s, len, &item->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	বাবে (i = 0; i < glyphs->অঙ্ক_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    যদি (glyph->attr.is_cluster_start)
	    {
		পূর্ণাঙ্ক cellcount;

		cellcount = গুনতি_cluster_cells(
			s, item, glyphs, i, &cluster_width,
			(item_list != রিক্ত) ? &last_glyph_rbearing : রিক্ত);

		যদি (i > 0 && cellcount > 0)
		{
		    draw_row_add_cluster(drow, dattr,
			    current_cluster_col, current_cluster_cells,
			    item->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_start_idx : রিক্ত,
			    i - cluster_start_idx, self);

		    current_cluster_col += current_cluster_cells;
		    cluster_start_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		যদি (cellcount > 0)
		{
		    পূর্ণাঙ্ক width;

		    width = cellcount * gui.char_width * PANGO_SCALE;
		    glyph->geometry.x_offset +=
			MAX(0, width - cluster_width) / 2;
		    glyph->geometry.width = width;
		}
		অন্যথা
		    setup_zero_width_cluster(item, glyph, cells,
			    cluster_width,
			    last_glyph_rbearing);

		item_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    অন্যথা যদি (i > 0)
	    {
		পূর্ণাঙ্ক width;

		যদি (glyph->geometry.x_offset >= 0)
		{
		    glyphs->glyphs[i].geometry.width =
			glyphs->glyphs[i - 1].geometry.width;
		    glyphs->glyphs[i - 1].geometry.width = 0;
		}
		width = cells * gui.char_width * PANGO_SCALE;
		glyph->geometry.x_offset +=
		    MAX(0, width - cluster_width) / 2;
	    }
	    অন্যথা
		glyph->geometry.width = 0;


	    যদি (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			item->analysis.font, *glyph);
	}
	যদি (glyphs->অঙ্ক_glyphs > 0)
	    draw_row_add_cluster(drow, dattr,
		    current_cluster_col, current_cluster_cells,
		    item->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_start_idx : রিক্ত,
		    glyphs->অঙ্ক_glyphs - cluster_start_idx, self);

	pango_item_free(item);
	column_offset += item_cells;
    }

    pango_glyph_string_free(glyphs);

exit:
    যদি (attr_list != রিক্ত)
	pango_attr_list_unref(attr_list);

    ঘূৰাই column_offset;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    পূর্ণাঙ্ক
vim_draw_area_draw_string(
	VimDrawArea *self,
	পূর্ণাঙ্ক	    row,
	পূর্ণাঙ্ক	    col,
	char_u	    *s,
	পূর্ণাঙ্ক	    len,
	পূর্ণাঙ্ক	    flags)
{
    DrawRow	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = রিক্ত;
    পূর্ণাঙ্ক		convlen;
    পূর্ণাঙ্ক		n_cells ;

    যদি (unlikely(self->rows == রিক্ত
		|| row >= self->n_rows
		|| col >= self->n_cols))
	ঘূৰাই len;

    যদি (output_conv.vc_type != CONV_NONE)
    {
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	যদি (conv_buf != রিক্ত)
	{
	    s = conv_buf;
	    len = convlen;
	}
    }

    // Safety check: pango crashes with invalid utf-8.
    যদি (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	ঘূৰাই len;
    }

    drow = self->rows + row;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_add_string(self, drow, &dattr, col, s, len, flags);

    vim_free(conv_buf);

    ঘূৰাই n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    শূন্য
vim_draw_area_clear_block(
	VimDrawArea	*self,
	পূর্ণাঙ্ক		row1,
	পূর্ণাঙ্ক		col1,
	পূর্ণাঙ্ক		row2,
	পূর্ণাঙ্ক		col2)
{
    যদি (unlikely(self->rows == রিক্ত
		|| row1 >= self->n_rows
		|| col1 >= self->n_cols
		|| row2 >= self->n_rows
		|| col2 >= self->n_cols))
	ঘূৰাই;

    বাবে (পূর্ণাঙ্ক r = row1; r <= row2; r++)
	draw_row_fill(self->rows + r, col1, col2, রিক্ত, রিক্ত, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    শূন্য
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_block(self, 0, 0, self->n_rows - 1, self->n_cols - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    শূন্য
vim_draw_area_move_block(
	VimDrawArea *self,
	পূর্ণাঙ্ক	    to,
	পূর্ণাঙ্ক	    row1,
	পূর্ণাঙ্ক	    row2,
	পূর্ণাঙ্ক	    col1,
	পূর্ণাঙ্ক	    col2)
{
    পূর্ণাঙ্ক offset = row2 - row1;

    যদি (unlikely(self->rows == রিক্ত
		|| row1 >= self->n_rows
		|| row2 >= self->n_rows
		|| to >= self->n_rows
		|| col1 >= self->n_cols
		|| col2 >= self->n_cols))
	ঘূৰাই;

    প্রতিজ্ঞা(row2 >= row1);
    প্রতিজ্ঞা(col2 >= col1);
    প্রতিজ্ঞা(row1 != to);

    যদি (row1 > to)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	বাবে (পূর্ণাঙ্ক o = 0; o <= offset; o++)
	    draw_row_move_to(self->rows + to + o, self->rows + row1 + o,
		    col1, col2, self);
    }
    অন্যথা
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	বাবে (পূর্ণাঙ্ক o = offset; o >= 0; o--)
	    যদি (to + o >= self->n_rows)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_block(self, row1 + o, col1, row1 + o, col2);
	    অন্যথা
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
    শূন্য
vim_draw_area_set_cursor(VimDrawArea *self, পূর্ণাঙ্ক w, পূর্ণাঙ্ক h)
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
    শূন্য
vim_draw_area_invert_block(
	VimDrawArea	*self,
	পূর্ণাঙ্ক		row,
	পূর্ণাঙ্ক		col,
	পূর্ণাঙ্ক		nrows,
	পূর্ণাঙ্ক		ncols)
{
    যদি (unlikely(self->rows == রিক্ত
		|| row >= self->n_rows
		|| col >= self->n_cols
		|| row + nrows - 1 >= self->n_rows
		|| col + ncols - 1 >= self->n_cols))
	ঘূৰাই;

    বাবে (পূর্ণাঙ্ক r = row; r < row + nrows; r++)
    {
	DrawRow	    *drow = self->rows + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	বাবে (পূর্ণাঙ্ক c = 0; c < ncols; c++)
	    dlayer->buf[c + col] = ~dlayer->buf[c + col];
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#যদি defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    শূন্য
vim_draw_area_add_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	পূর্ণাঙ্ক	    row,
	পূর্ণাঙ্ক	    col,
	পূর্ণাঙ্ক	    width,
	পূর্ণাঙ্ক	    height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    পূর্ণাঙ্ক		cells;

    যদি (unlikely(self->rows == রিক্ত
		|| row >= self->n_rows
		|| col >= self->n_cols))
	ঘূৰাই;

    drow = self->rows + row;
    dsign = draw_sign_new(sign, width, height);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (width + gui.char_width - 1) / gui.char_width;

    draw_row_fill(drow, col, col + cells - 1, dglyphs, রিক্ত, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    শূন্য
vim_draw_area_add_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	পূর্ণাঙ্ক		row,
	পূর্ণাঙ্ক		col,
	পূর্ণাঙ্ক		width,
	পূর্ণাঙ্ক		height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    পূর্ণাঙ্ক		cells;
    GdkTexture	*texture;
    GBytes	*bytes;

    যদি (unlikely(self->rows == রিক্ত
		|| row >= self->n_rows
		|| col >= self->n_cols))
	ঘূৰাই;

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

    draw_row_fill(drow, col, col + cells - 1, dglyphs, রিক্ত, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    শূন্য
vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_গ্ৰন্থি *গ্ৰন্থি)
{
    g_hash_table_add(self->external, gsk_render_গ্ৰন্থি_ref(গ্ৰন্থি));
}

    শূন্য
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_গ্ৰন্থি *গ্ৰন্থি)
{
    g_hash_table_remove(self->external, গ্ৰন্থি);
}

#endif

/*
 * Add the cursor to the snapshot.
 */
    স্থির শূন্য
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_গ্ৰন্থি	*text_গ্ৰন্থি,	// May be NULL
	Gsk_Render_গ্ৰন্থি	*special_গ্ৰন্থি,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    পূর্ণাঙ্ক		    w = cursor->width;
    পূর্ণাঙ্ক		    h = cursor->height;
    graphene_rect_t rect;

    যদি (!cursor->draw)
	ঘূৰাই;

    যদি (cursor->width <= 0 && cursor->height <= 0)
    {
	w += gui.char_width * cursor->n_cells;
	h = gui.char_height;
    }

    graphene_rect_init(&rect,
#ifdef FEAT_RIGHTLEFT
	    CURSOR_BAR_ডান ? FILL_X(gui.cursor_col + 1) - w :
#endif
	    FILL_X(gui.cursor_col),
	    FILL_Y(gui.cursor_row) + gui.char_height - h, w, h);


    যদি (cursor->width == -1 && cursor->height == -1)
    {
	// Create hollow cursor
	GskRoundedRect	    outline;
	স্থির ধ্রুৱ ভিন্ন  border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	ধ্রুৱ GdkRGBA	    color[4] = {
	    cursor->bg_color, cursor->bg_color,
	    cursor->bg_color, cursor->bg_color
	} ;

	gsk_rounded_rect_init_from_rect(&outline, &rect, 0.0f);
	gtk_snapshot_append_border(snapshot, &outline, border, color);
    }
    অন্যথা
    {
	// Create block style cursor, by masking the text + under decorations
	// with the fg color.
	gtk_snapshot_append_color(snapshot, &cursor->bg_color, &rect);

	gtk_snapshot_ঠেলো_mask(snapshot, GSK_MASK_MODE_ALPHA);

	// Used as an "outline" for the fg color.
	যদি (text_গ্ৰন্থি != রিক্ত)
	    gtk_snapshot_append_গ্ৰন্থি(snapshot, text_গ্ৰন্থি);
	যদি (special_গ্ৰন্থি != রিক্ত)
	    gtk_snapshot_append_গ্ৰন্থি(snapshot, special_গ্ৰন্থি);

	gtk_snapshot_বেৰ_কৰো(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_বেৰ_কৰো(snapshot);
    }
}

    স্থির শূন্য
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    পূর্ণাঙ্ক		    height, width;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = রিক্ত;
    Gsk_Render_গ্ৰন্থি   *body_গ্ৰন্থি;
    Gsk_Render_গ্ৰন্থি   *invert_গ্ৰন্থি = রিক্ত;
#ifdef FEAT_IMAGE_GUI
    Gsk_Render_গ্ৰন্থি   *ext_গ্ৰন্থি;
    GHashTableIter  iter;
#endif

    gui_mch_set_bg_color(gui.back_pixel);
    height = gtk_widget_get_height(widget) + gui.bleed_bot;
    width = gtk_widget_get_width(widget) + gui.bleed_ডান;

    যদি (self->rows == রিক্ত)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, width, height));
	ঘূৰাই;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    যদি (self->bleed_ডান == -1 || self->bleed_ডান != gui.bleed_ডান)
    {
	self->bleed_ডান = gui.bleed_ডান;
	বাবে (পূর্ণাঙ্ক r = 0; r < self->n_rows; r++)
	{
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_BACKGROUND);
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#যদি GTK_CHECK_VERSION(4,24,0)
    gtk_snapshot_set_snap(body_snapshot, GSK_RECT_SNAP_ROUND);
#endif

    বাবে (পূর্ণাঙ্ক r = 0; r < self->n_rows; r++)
    {
	DrawRow *drow = self->rows + r;

	draw_row_render_background(drow, self);
	draw_row_render_text(drow, self);
	draw_row_render_special(drow, self);
	draw_row_render_overlay(drow, self);

	বাবে (পূর্ণাঙ্ক l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    যদি (dlayer->গ্ৰন্থি != রিক্ত)
	    {
		যদি (l == DRAW_LAYER_OVERLAY)
		{
		    যদি (invert_snapshot == রিক্ত)
		    {
			invert_snapshot = gtk_snapshot_new();
#যদি GTK_CHECK_VERSION(4,24,0)
			gtk_snapshot_set_snap(invert_snapshot,
				GSK_RECT_SNAP_ROUND);
#endif
		    }
		    gtk_snapshot_append_গ্ৰন্থি(invert_snapshot, dlayer->গ্ৰন্থি);
		}
		অন্যথা
		    gtk_snapshot_append_গ্ৰন্থি(body_snapshot, dlayer->গ্ৰন্থি);
	    }
	}

	যদি (r == gui.cursor_row)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_TEXT].গ্ৰন্থি,
		    drow->dlayers[DRAW_LAYER_SPECIAL].গ্ৰন্থি,
		    body_snapshot);
    }

    যদি (invert_snapshot != রিক্ত)
	gtk_snapshot_ঠেলো_blend(snapshot, GSK_BLEND_MODE_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, width, height));
    body_গ্ৰন্থি = gtk_snapshot_free_to_গ্ৰন্থি(body_snapshot);
    যদি (body_গ্ৰন্থি != রিক্ত)
    {
	gtk_snapshot_append_গ্ৰন্থি(snapshot, body_গ্ৰন্থি);
	gsk_render_গ্ৰন্থি_unref(body_গ্ৰন্থি);
    }

#ifdef FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_table_iter_init(&iter, self->external);

    যেতিয়ালৈ (g_hash_table_iter_next(&iter, (শূন্য **)&ext_গ্ৰন্থি, রিক্ত))
	gtk_snapshot_append_গ্ৰন্থি(snapshot, ext_গ্ৰন্থি);
#endif

    যদি (invert_snapshot != রিক্ত)
    {
	gtk_snapshot_বেৰ_কৰো(snapshot);
	invert_গ্ৰন্থি = gtk_snapshot_free_to_গ্ৰন্থি(invert_snapshot);
	// Not sure if it can be NULL but still check
	যদি (invert_গ্ৰন্থি != রিক্ত)
	{
	    gtk_snapshot_append_গ্ৰন্থি(snapshot, invert_গ্ৰন্থি);
	    gsk_render_গ্ৰন্থি_unref(invert_গ্ৰন্থি);
	}
	gtk_snapshot_বেৰ_কৰো(snapshot);
    }
}
