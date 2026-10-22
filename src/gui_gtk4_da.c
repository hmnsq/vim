/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#അന്തര്ഭാവ "vim.h"

#അന്തര്ഭാവ <gtk/gtk.h>
#അന്തര്ഭാവ "gui_gtk4_da.h"


#എങ്കിൽ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Used for sign icons and netbeans multisign indicator. It is in a DrawGlyphs
 * so that it can move/be manipulated with the text.
 */
തരനിർവചനം ഘടന
{
    GdkTexture	*texture;
    പൂർണ്ണസംഖ്യ		width;
    പൂർണ്ണസംഖ്യ		height;
} DrawSign;
#endif

/*
 * Temporary struct when drawing string representing visual attributes of a cell
 */
തരനിർവചനം ഘടന
{
    uint32_t	bg_color;
    uint32_t	fg_color;   // Only used for underlines
    uint32_t	sp_color;
    പൂർണ്ണസംഖ്യ		draw_flags; // DRAW_* flags, only DRAW_UNDERL, DRAW_UNDERC, and
			    // DRAW_STRIKE are used.
} DrawAttr;

/*
 * Represents the glyphs of a single cell.
 */
തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ refcount;

    uint32_t	fg_color;
    PangoFont	*font;
    // If "font" is NULL, "dsign" is used, otherwise "glyphs" is used.
    സംഘം
    {
	PangoGlyphInfo  *glyphs; // May be NULL
#എങ്കിൽ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	DrawSign	*dsign;
#endif
    };
    പൂർണ്ണസംഖ്യ n_glyphs;
} DrawGlyphs;

തരനിർവചനം ഘടന
{
    DrawGlyphs *dglyphs;
} DrawCell;

തരനിർവചനം എണ്ണൽ
{
    DRAW_LAYER_BACKGROUND,  // Background color, buf stores bg color
    DRAW_LAYER_TEXT,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

തരനിർവചനം ഘടന
{
    Gsk_Render_കണ്ണി   *കണ്ണി;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#define UNDERC_OFFSET(n_cols) (0)
#define STRIKE_OFFSET(n_cols) (n_cols)

തരനിർവചനം ഘടന
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    പൂർണ്ണസംഖ്യ		n_cells; // For convenience
    പൂർണ്ണസംഖ്യ		row;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_കണ്ണി *underl_mask;
    Gsk_Render_കണ്ണി *underc_mask;
    Gsk_Render_കണ്ണി *strike_mask;
} DrawRow;

#define തിരുകുക_PANGO_ATTR(Attribute, AttrList, Start, End) \
    ചെയ്യുക { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->start_index = (Start); \
	tmp_attr_->end_index = (End); \
	pango_attr_list_തിരുകുക((AttrList), tmp_attr_); \
    } ആകുമ്പോൾ (FALSE)
#define CELLS2PANGO(c) ((c) * gui.char_width * PANGO_SCALE)

/*
 * Attributes of cursor
 */
തരനിർവചനം ഘടന
{
    gboolean draw; // If cursor should be drawn
    പൂർണ്ണസംഖ്യ width;
    പൂർണ്ണസംഖ്യ height;
    പൂർണ്ണസംഖ്യ n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

ഘടന _VimDrawArea
{
    GtkWidget parent;

    DrawRow *rows;

    പൂർണ്ണസംഖ്യ n_rows;
    പൂർണ്ണസംഖ്യ	n_cols;

    പൂർണ്ണസംഖ്യ	bleed_വലത്;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*കണ്ണി_buf;

    DrawCursor cursor;

#ifdef FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHashTable *external;
#endif
};

സ്ഥിതം ശൂന്യം draw_row_init(DrawRow *drow, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ cols);
സ്ഥിതം ശൂന്യം draw_row_clear(DrawRow *drow);
സ്ഥിതം ശൂന്യം draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t);
സ്ഥിതം ശൂന്യം vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

സ്ഥിതം GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    സ്ഥിതം ശൂന്യം
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    വേണ്ടി (പൂർണ്ണസംഖ്യ r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    g_free(self->rows);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->കണ്ണി_buf, TRUE);

#ifdef FEAT_IMAGE_GUI
    g_hash_table_unref(self->external);
#endif

    G_OBJECT_CLASS(vim_draw_area_parent_class)->finalize(obj);
}

    സ്ഥിതം ശൂന്യം
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    സ്ഥിതം ശൂന്യം
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_വലത് = -1;
#ifdef FEAT_IMAGE_GUI
    self->external = g_hash_table_new_full(g_direct_hash, g_direct_equal,
	    ശൂന്യ, (GDestroyNotify)gsk_render_കണ്ണി_unref);
#endif
    self->glyph_buf = g_array_new(FALSE, FALSE, വലുപ്പം(PangoGlyphInfo));
    self->കണ്ണി_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_കണ്ണി_unref);

}

    GtkWidget *
vim_draw_area_new(ശൂന്യം)
{
    തിരികെ g_object_new(VIM_TYPE_DRAW_AREA, ശൂന്യ);
}

/*
 * Set the size of the draw area to "rows" and "cols".
 */
    ശൂന്യം
vim_draw_area_set_size(VimDrawArea *self, പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols)
{
    എങ്കിൽ (self->rows != ശൂന്യ && self->n_rows == rows && self->n_cols == cols)
	തിരികെ;
    എങ്കിൽ (rows == 0 || cols == 0)
	തിരികെ;

    vim_draw_area_clear(self);

    വേണ്ടി (പൂർണ്ണസംഖ്യ r = 0; r < self->n_rows; r++)
	draw_row_clear(self->rows + r);
    self->rows = g_realloc_n(self->rows, rows, വലുപ്പം(DrawRow));
    വേണ്ടി (പൂർണ്ണസംഖ്യ r = 0; r < rows; r++)
	draw_row_init(self->rows + r, r, cols);

    self->n_rows = rows;
    self->n_cols = cols;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    സ്ഥിതം uint32_t
rgba_to_u32(സ്ഥിരം GdkRGBA *rgba)
{
    സംഘം { uint32_t val; uint8_t bytes[4]; } color;

    color.bytes[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.bytes[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.bytes[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.bytes[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    തിരികെ color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    സ്ഥിതം GdkRGBA
u32_to_rgba(uint32_t val)
{
    സംഘം { uint32_t val; uint8_t bytes[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.bytes[0] / 255.0;
    color.green = packed.bytes[1] / 255.0;
    color.blue  = packed.bytes[2] / 255.0;
    color.alpha = packed.bytes[3] / 255.0;
    തിരികെ color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    സ്ഥിതം gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    സംഘം { uint32_t val; uint8_t bytes[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.bytes[0] << 16) |
	((guicolor_T)packed.bytes[1] << 8)  |
	packed.bytes[2];
    തിരികെ res == rgb;
}

/*
 * Checks if a RGBA pixel buffer is fully transparent.
 */
    സ്ഥിതം വരിയിൽ gboolean
rgba_is_transparent(സ്ഥിരം uint32_t *buf, പൂർണ്ണസംഖ്യ n_pixels)
{
    // Could also compare 2 pixels at once using a 64 bit integer, but not sure
    // if that might cause alignment issues...
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < n_pixels; i++)
	എങ്കിൽ (buf[i] != 0)
	    തിരികെ FALSE;
    തിരികെ TRUE;
}

/*
 * Return TRUE if "glyph" has ink (not whitespace).
 */
    സ്ഥിതം gboolean
glyph_has_ink(PangoFont *font, PangoGlyphInfo glyph)
{
    PangoRectangle ink;

    pango_font_get_glyph_extents (font, glyph.glyph, &ink, ശൂന്യ);

    തിരികെ ink.width > 0 && ink.height > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    സ്ഥിതം ശൂന്യം
apply_wide_font_attr(char_u *s, പൂർണ്ണസംഖ്യ len, PangoAttrList *attr_list)
{
    char_u  *start = ശൂന്യ;
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    uc;

    വേണ്ടി (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	എങ്കിൽ (start == ശൂന്യ)
	{
	    എങ്കിൽ (uc >= 0x80 && utf_char2cells(uc) == 2)
		start = p;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    തിരുകുക_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, start - s, p - s);
	    start = ശൂന്യ;
	}
    }

    എങ്കിൽ (start != ശൂന്യ)
	തിരുകുക_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, start - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
എണ്ണം_cluster_cells(
	char_u		    *s,
	PangoItem	    *item,
	PangoGlyphString    *glyphs,
	പൂർണ്ണസംഖ്യ		    i,
	പൂർണ്ണസംഖ്യ		    *cluster_width,
	പൂർണ്ണസംഖ്യ		    *last_glyph_rbearing)
{
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    next;
    പൂർണ്ണസംഖ്യ	    start, end;
    പൂർണ്ണസംഖ്യ	    width;
    പൂർണ്ണസംഖ്യ	    uc;
    പൂർണ്ണസംഖ്യ	    cellcount = 0;

    width = glyphs->glyphs[i].geometry.width;

    വേണ്ടി (next = i + 1; next < glyphs->അക്കം_glyphs; ++next)
    {
	എങ്കിൽ (glyphs->glyphs[next].attr.is_cluster_start)
	    നിർത്തുക;
	അല്ലെങ്കിൽ എങ്കിൽ (glyphs->glyphs[next].geometry.width > width)
	    width = glyphs->glyphs[next].geometry.width;
    }

    start = item->offset + glyphs->log_clusters[i];
    end   = item->offset + ((next < glyphs->അക്കം_glyphs) ?
	    glyphs->log_clusters[next] : item->length);

    വേണ്ടി (p = s + start; p < s + end; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	എങ്കിൽ (uc < 0x80)
	    ++cellcount;
	അല്ലെങ്കിൽ എങ്കിൽ (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    എങ്കിൽ (last_glyph_rbearing != ശൂന്യ
	    && cellcount > 0 && next == glyphs->അക്കം_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(item->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, ശൂന്യ);

	എങ്കിൽ (PANGO_RBEARING(ink_rect) > 0)
	    *last_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    എങ്കിൽ (cellcount > 0)
	*cluster_width = width;

    തിരികെ cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    സ്ഥിതം ശൂന്യം
setup_zero_width_cluster(
	PangoItem	*item,
	PangoGlyphInfo	*glyph,
	പൂർണ്ണസംഖ്യ		last_cellcount,
	പൂർണ്ണസംഖ്യ		last_cluster_width,
	പൂർണ്ണസംഖ്യ		last_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    പൂർണ്ണസംഖ്യ		    width;

    width = last_cellcount * gui.char_width * PANGO_SCALE;
    glyph->geometry.x_offset = -width + MAX(0, width - last_cluster_width) / 2;
    glyph->geometry.width = 0;

    pango_font_get_glyph_extents(item->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    എങ്കിൽ (ink_rect.x < 0)
    {
	glyph->geometry.x_offset += last_glyph_rbearing;
	glyph->geometry.y_offset  = logical_rect.height
	    - (gui.char_height - p_linespace) * PANGO_SCALE;
    }
    അല്ലെങ്കിൽ
	glyph->geometry.x_offset = -width + MAX(0, width - ink_rect.width) / 2;
}

#എങ്കിൽ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    സ്ഥിതം DrawSign *
draw_sign_new(GdkTexture *texture, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->width = width;
    dsign->height = height;

    തിരികെ dsign;
}

    സ്ഥിതം ശൂന്യം
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#endif

    സ്ഥിതം ശൂന്യം
draw_attr_init(
	DrawAttr	*dattr,
	സ്ഥിരം GdkRGBA	*bg_color,
	സ്ഥിരം GdkRGBA	*fg_color,
	സ്ഥിരം GdkRGBA	*sp_color,
	പൂർണ്ണസംഖ്യ		draw_flags)
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
    സ്ഥിതം ശൂന്യം
draw_attr_apply(DrawAttr *dattr, പൂർണ്ണസംഖ്യ col, DrawRow *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + col;
    uint32_t	*text_cell = drow->dlayers[DRAW_LAYER_TEXT].buf + col;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    പൂർണ്ണസംഖ്യ		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + UNDERC_OFFSET(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	col + STRIKE_OFFSET(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    എങ്കിൽ (dattr != ശൂന്യ)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.back_pixel, bg_color);
    }

    എങ്കിൽ (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	എങ്കിൽ (bg_is_default)
	    *bg_cell = 0;
	അല്ലെങ്കിൽ
	    *bg_cell = bg_color;
    }
    എങ്കിൽ ((draw_flags & DRAW_UNDERL && *text_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *text_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);
	എങ്കിൽ (draw_flags & DRAW_UNDERL)
	    *text_cell = fg_color;
	അല്ലെങ്കിൽ
	    *text_cell = 0;
    }
    എങ്കിൽ ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	എങ്കിൽ (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	അല്ലെങ്കിൽ
	    *underc_cell = 0;
    }
    എങ്കിൽ ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	എങ്കിൽ (draw_flags & DRAW_STRIKE)
	    *strike_cell = sp_color;
	അല്ലെങ്കിൽ
	    *strike_cell = 0;
    }
}

    സ്ഥിതം DrawGlyphs *
draw_glyphs_new(
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	പൂർണ്ണസംഖ്യ		n_glyphs,
	uint32_t	fg_color)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->font = g_object_ref(font);
    dglyphs->glyphs = g_memdup2(glyphs, വലുപ്പം(*glyphs) * n_glyphs);
    dglyphs->n_glyphs = n_glyphs;
    dglyphs->fg_color = fg_color;

    തിരികെ dglyphs;
}

#എങ്കിൽ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw glyphs that shows the given sign. Note that ownership of
 * "dsign" is taken.
 */
    സ്ഥിതം DrawGlyphs *
draw_glyphs_new_sign(DrawSign *dsign)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->dsign = dsign;

    തിരികെ dglyphs;
}
#endif

    സ്ഥിതം DrawGlyphs *
draw_glyphs_ref(DrawGlyphs *dglyphs)
{
    dglyphs->refcount++;
    തിരികെ dglyphs;
}

    സ്ഥിതം ശൂന്യം
draw_glyphs_unref(DrawGlyphs *dglyphs)
{
    എങ്കിൽ (--dglyphs->refcount <= 0)
    {
#എങ്കിൽ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	എങ്കിൽ (dglyphs->font == ശൂന്യ)
	    draw_sign_free(dglyphs->dsign);
	അല്ലെങ്കിൽ
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
    സ്ഥിതം gboolean
draw_glyphs_same(DrawGlyphs *a, DrawGlyphs *b)
{
    എങ്കിൽ (a == ശൂന്യ && b == ശൂന്യ)
	തിരികെ TRUE;
    എങ്കിൽ (a == ശൂന്യ || b == ശൂന്യ)
	തിരികെ FALSE;
    എങ്കിൽ (a->fg_color != b->fg_color)
	തിരികെ FALSE;
    എങ്കിൽ (a->n_glyphs != b->n_glyphs || a->font != b->font)
	തിരികെ FALSE;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < a->n_glyphs; i++)
	എങ്കിൽ (a->glyphs[i].glyph != b->glyphs[i].glyph
		|| a->glyphs[i].geometry.width != b->glyphs[i].geometry.width
		|| a->glyphs[i].geometry.x_offset != b->glyphs[i].geometry.x_offset
		|| a->glyphs[i].geometry.y_offset != b->glyphs[i].geometry.y_offset
		|| a->glyphs[i].attr.is_cluster_start != b->glyphs[i].attr.is_cluster_start)
	    തിരികെ FALSE;
    തിരികെ TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    സ്ഥിതം Gsk_Render_കണ്ണി *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	പൂർണ്ണസംഖ്യ	    row,
	VimDrawArea *da,
	പൂർണ്ണസംഖ്യ	    off,
	gboolean    bleed)
{
    GBytes	    *bytes;
    GdkTexture	    *texture;
    Gsk_Render_കണ്ണി   *കണ്ണി;
    പൂർണ്ണസംഖ്യ		    height;

#എങ്കിൽ GTK_CHECK_VERSION(4,24,0)
    height = gui.char_height;
#അല്ലെങ്കിൽ
    height = gui.char_height + 1;
#endif

    എങ്കിൽ (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		വലുപ്പം(uint32_t) * da->n_cols);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_cols - 1)),
		വലുപ്പം(uint32_t)); // Add bleed using last pixel of buffer
	bytes = g_byte_array_free_to_bytes(arr);
    }
    അല്ലെങ്കിൽ
	bytes = g_bytes_new(dlayer->buf + off, വലുപ്പം(uint32_t) * da->n_cols);

    texture = gdk_memory_texture_new(da->n_cols + bleed, 1,
	    GDK_MEMORY_R8G8B8A8, bytes, (da->n_cols + bleed) * 4);
    g_bytes_unref(bytes);

    // Scale texture to actual size
    കണ്ണി = gsk_texture_scale_കണ്ണി_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		(da->n_cols + bleed) * gui.char_width, gui.char_height + bleed),
	    GSK_SCALING_FILTER_NEAREST);
    എങ്കിൽ (bleed)
    {
	Gsk_Render_കണ്ണി *new;

	new = gsk_clip_കണ്ണി_new(കണ്ണി,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(row),
		    da->n_cols * gui.char_width + da->bleed_വലത്, height));
	gsk_render_കണ്ണി_unref(കണ്ണി);
	കണ്ണി = new;
    }
    g_object_unref(texture);
    തിരികെ കണ്ണി;
}

    സ്ഥിതം ശൂന്യം
draw_row_init(DrawRow *drow, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ cols)
{
    drow->cells = g_malloc0_n(cols, വലുപ്പം(DrawCell));
    drow->n_cells = cols;
    drow->row = row;
    drow->underl_mask = drow->underc_mask = drow->strike_mask = ശൂന്യ;

    memset(drow->dlayers, 0, വലുപ്പം(DrawLayer) * N_DRAW_LAYERS);
    വേണ്ടി (പൂർണ്ണസംഖ്യ l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	എങ്കിൽ (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(cols * 2, 4);
	അല്ലെങ്കിൽ
	    dlayer->buf = g_malloc0_n(cols, 4);
    }
}

    സ്ഥിതം ശൂന്യം
draw_row_clear(DrawRow *drow)
{
    g_free(drow->cells);
    വേണ്ടി (പൂർണ്ണസംഖ്യ l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	എങ്കിൽ (dlayer->കണ്ണി != ശൂന്യ)
	    gsk_render_കണ്ണി_unref(dlayer->കണ്ണി);
	g_free(dlayer->buf);
    }
    g_clear_pointer(&drow->underl_mask, gsk_render_കണ്ണി_unref);
    g_clear_pointer(&drow->underc_mask, gsk_render_കണ്ണി_unref);
    g_clear_pointer(&drow->strike_mask, gsk_render_കണ്ണി_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    സ്ഥിതം ശൂന്യം
draw_row_dirty_layer(DrawRow *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_pointer(&dlayer->കണ്ണി, gsk_render_കണ്ണി_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    സ്ഥിതം ശൂന്യം
draw_row_uninvert_cell(DrawRow *drow, പൂർണ്ണസംഖ്യ col)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    എങ്കിൽ (buf[col] == 0xffffffff)
    {
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[col] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    സ്ഥിതം ശൂന്യം
draw_row_set_cell(DrawRow *drow, പൂർണ്ണസംഖ്യ col, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + col;

    എങ്കിൽ (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_row_dirty_layer(drow, DRAW_LAYER_TEXT);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    എങ്കിൽ (dcell->dglyphs != dglyphs)
    {
	എങ്കിൽ (dcell->dglyphs != ശൂന്യ)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == ശൂന്യ ? ശൂന്യ : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, col, drow);
    draw_row_uninvert_cell(drow, col);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    സ്ഥിതം ശൂന്യം
draw_row_fill(
	DrawRow	    *drow,
	പൂർണ്ണസംഖ്യ	    col1,
	പൂർണ്ണസംഖ്യ	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ c = col1; c <= col2; c++)
	draw_row_set_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    എങ്കിൽ (gui.cursor_row == drow->row && gui.cursor_col >= col1
	    && gui.cursor_col <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    സ്ഥിതം ശൂന്യം
draw_row_move_to(
	DrawRow	    *dest_row,
	DrawRow	    *src_row,
	പൂർണ്ണസംഖ്യ	    col1,
	പൂർണ്ണസംഖ്യ	    col2,
	VimDrawArea *da)
{
    പൂർണ്ണസംഖ്യ move_size = (col2 - col1 + 1) * വലുപ്പം(DrawCell);

    draw_row_fill(dest_row, col1, col2, ശൂന്യ, ശൂന്യ, &da->cursor);
    memmove(dest_row->cells + col1, src_row->cells + col1, move_size);
    // NULL the draw cells so we don't double unreference.
    memset(src_row->cells + col1, 0, move_size);
    draw_row_dirty_layer(src_row, DRAW_LAYER_TEXT);

    // Move the layer buffers
    move_size = (col2 - col1 + 1) * 4;
    വേണ്ടി (പൂർണ്ണസംഖ്യ l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_row->dlayers[l].buf;
	uint32_t *dest = dest_row->dlayers[l].buf;

	എങ്കിൽ (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_OFFSET(dest_row->n_cells),
		    src + col1 + UNDERC_OFFSET(src_row->n_cells), move_size);
	    memmove(dest + col1 + STRIKE_OFFSET(dest_row->n_cells),
		    src + col1 + STRIKE_OFFSET(src_row->n_cells), move_size);
	}
	അല്ലെങ്കിൽ
	    memmove(dest + col1, src + col1, move_size);
	draw_row_dirty_layer(dest_row, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    സ്ഥിതം ശൂന്യം
draw_row_render_background(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    എങ്കിൽ (!dlayer->dirty)
	തിരികെ;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    എങ്കിൽ (rgba_is_transparent(dlayer->buf, da->n_cols))
	തിരികെ;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->കണ്ണി = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    സ്ഥിതം ശൂന്യം
draw_row_ensure_decor(DrawRow *drow, പൂർണ്ണസംഖ്യ flags)
{
    എങ്കിൽ (flags & DRAW_UNDERL && drow->underl_mask == ശൂന്യ)
	drow->underl_mask = gsk_color_കണ്ണി_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->row + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    എങ്കിൽ (flags & DRAW_STRIKE && drow->strike_mask == ശൂന്യ)
	drow->strike_mask = gsk_color_കണ്ണി_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->row) + (പൂർണ്ണസംഖ്യ)(gui.char_height / 2),
		    FILL_X(drow->n_cells), 1));

    എങ്കിൽ (flags & DRAW_UNDERC && drow->underc_mask == ശൂന്യ)
    {
	സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	പൂർണ്ണസംഖ്യ y = FILL_Y(drow->row + 1) - 1;
	പൂർണ്ണസംഖ്യ x_start = FILL_X(0);
	പൂർണ്ണസംഖ്യ x_end = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_കണ്ണി *child = ശൂന്യ;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#എങ്കിൽ GTK_CHECK_VERSION(4, 14, 0)
	GskPathBuilder	*builder;
	GskPath		*path;
	GskStroke       *stroke;
	Gsk_Render_കണ്ണി	*color_കണ്ണി;
	graphene_rect_t bounds;

	builder = gsk_path_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_path_builder_move_to(builder, -1, y - val[7] + 0.5);
	gsk_path_builder_വരി_to(builder, 0, y - val[0] + 0.5);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < 8; i++)
	    gsk_path_builder_വരി_to(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_path_builder_വരി_to(builder, 8, y - val[0] + 0.5);
	gsk_path_builder_വരി_to(builder, 9, y - val[1] + 0.5);

	path = gsk_path_builder_free_to_path(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_path_get_stroke_bounds(path, stroke, &bounds);
	color_കണ്ണി = gsk_color_കണ്ണി_new(&white_rgba, &bounds);
	child = gsk_stroke_കണ്ണി_new(color_കണ്ണി, path, stroke);

	gsk_stroke_free(stroke);
	gsk_path_unref(path);
	gsk_render_കണ്ണി_unref(color_കണ്ണി);
#അല്ലെങ്കിൽ
	cairo_t *cr;

	child = gsk_cairo_കണ്ണി_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_കണ്ണി_get_draw_context(child);

	cairo_set_വരി_width(cr, 1.0);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_to(cr, -1, y - val[7] + 0.5);
	cairo_വരി_to(cr, 0, y - val[0] + 0.5);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < 8; ++i)
	    cairo_വരി_to(cr, i, y - val[i] + 0.5);

	cairo_വരി_to(cr, 8, y - val[0] + 0.5);
	cairo_വരി_to(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#endif
	drow->underc_mask = gsk_repeat_കണ്ണി_new(
		&GRAPHENE_RECT_INIT(x_start, y - 3, x_end - x_start, 5),
		child, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_കണ്ണി_unref(child);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    സ്ഥിതം ശൂന്യം
draw_row_render_text(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_TEXT;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		start_col = 0;
    പൂർണ്ണസംഖ്യ		empty_cells = 0;
    uint32_t	cur_fg = 0;
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (!dlayer->dirty)
	തിരികെ;

    glyph_buf = da->glyph_buf;
    nodes = da->കണ്ണി_buf;

    g_array_set_size(glyph_buf, 0);
    g_ptr_array_set_size(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#define FLUSH_കണ്ണി() \
    ചെയ്യുക { \
	എങ്കിൽ (cur_font != ശൂന്യ) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_കണ്ണി	*കണ്ണി; \
	    GdkRGBA		fg_color = u32_to_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->data; \
	    glyph_str.അക്കം_glyphs = glyph_buf->len; \
	    കണ്ണി = gsk_text_കണ്ണി_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_POINT_INIT(TEXT_X(start_col), \
			TEXT_Y(drow->row))); \
	    എങ്കിൽ (കണ്ണി != ശൂന്യ) \
		g_ptr_array_add(nodes, കണ്ണി); \
	    g_array_set_size(glyph_buf, 0); \
	    cur_font = ശൂന്യ; \
	} \
    } ആകുമ്പോൾ (FALSE)

    വേണ്ടി (c = 0; c < da->n_cols; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	പൂർണ്ണസംഖ്യ	    prev_len;

	എങ്കിൽ (dglyphs == ശൂന്യ)
	{
	    empty_cells++;
	    തുടരുക;
	}
#എങ്കിൽ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	അല്ലെങ്കിൽ എങ്കിൽ (dglyphs->font == ശൂന്യ)
	{
	    // Add sign icon
	    Gsk_Render_കണ്ണി   *snode;
	    പൂർണ്ണസംഖ്യ		    w;

	    FLUSH_കണ്ണി();

	    snode = gsk_texture_scale_കണ്ണി_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->row),
			dglyphs->dsign->width, dglyphs->dsign->height),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->width + gui.char_width - 1) / gui.char_width;

	    എങ്കിൽ (snode != ശൂന്യ)
		g_ptr_array_add(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    തുടരുക;
	}
#endif
	അല്ലെങ്കിൽ എങ്കിൽ (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_കണ്ണി();
	    cur_font = dglyphs->font;
	    start_col = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	എങ്കിൽ (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    തുടരുക;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	എങ്കിൽ (empty_cells > 0)
	{
	    എങ്കിൽ (prev_len > 0)
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

    FLUSH_കണ്ണി();

#undef FLUSH_കണ്ണി

    എങ്കിൽ (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_കണ്ണി *source;
	Gsk_Render_കണ്ണി *കണ്ണി;

	draw_row_ensure_decor(drow, DRAW_UNDERL);
	എങ്കിൽ (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    source = draw_layer_get_texture(dlayer, drow->row, da, 0, FALSE);
	    കണ്ണി = gsk_mask_കണ്ണി_new(source, drow->underl_mask,
		    GSK_MASK_MODE_ALPHA);
	    gsk_render_കണ്ണി_unref(source);
	    g_ptr_array_add(nodes, കണ്ണി);
	}

	dlayer->കണ്ണി = gsk_container_കണ്ണി_new(
		(Gsk_Render_കണ്ണി **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    സ്ഥിതം ശൂന്യം
draw_row_render_special(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_കണ്ണി   *source;
    Gsk_Render_കണ്ണി   *nodes[2];
    പൂർണ്ണസംഖ്യ		    n_nodes = 0;

    എങ്കിൽ (!dlayer->dirty)
	തിരികെ;

    draw_row_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    എങ്കിൽ (!rgba_is_transparent(dlayer->buf + UNDERC_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		UNDERC_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_കണ്ണി_new(source, drow->underc_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_കണ്ണി_unref(source);
    }

    എങ്കിൽ (!rgba_is_transparent(dlayer->buf + STRIKE_OFFSET(drow->n_cells),
		drow->n_cells))
    {
	source = draw_layer_get_texture(dlayer, drow->row, da,
		STRIKE_OFFSET(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_mask_കണ്ണി_new(source, drow->strike_mask,
		GSK_MASK_MODE_ALPHA);
	gsk_render_കണ്ണി_unref(source);
    }

    എങ്കിൽ (n_nodes == 1)
    {
	dlayer->കണ്ണി = nodes[0];
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n_nodes > 0)
    {
	dlayer->കണ്ണി = gsk_container_കണ്ണി_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(nodes); i++)
	    gsk_render_കണ്ണി_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    സ്ഥിതം ശൂന്യം
draw_row_render_overlay(DrawRow *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    എങ്കിൽ (!dlayer->dirty)
	തിരികെ;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    എങ്കിൽ (rgba_is_transparent(dlayer->buf, da->n_cols))
	തിരികെ;

    dlayer->കണ്ണി = draw_layer_get_texture(dlayer, drow->row, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    സ്ഥിതം ശൂന്യം
draw_row_add_cluster(
	DrawRow		*drow,
	DrawAttr	*dattr,
	പൂർണ്ണസംഖ്യ		col,
	പൂർണ്ണസംഖ്യ		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	പൂർണ്ണസംഖ്യ		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = ശൂന്യ;

    എങ്കിൽ (col + n_cells > da->n_cols)
	തിരികെ;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    എങ്കിൽ (glyphs != ശൂന്യ)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_row_fill(drow, col, col + n_cells - 1, dglyphs, dattr, &da->cursor);

    എങ്കിൽ (dglyphs != ശൂന്യ)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vim_draw_area_add_string(
	VimDrawArea *self,
	DrawRow	    *drow,
	DrawAttr    *dattr,
	പൂർണ്ണസംഖ്യ	    col,
	char_u	    *s,
	പൂർണ്ണസംഖ്യ	    len,
	പൂർണ്ണസംഖ്യ	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			start_idx = -1;
    GList		*item_list;
    പൂർണ്ണസംഖ്യ			column_offset = 0;
    പൂർണ്ണസംഖ്യ			cluster_width;
    പൂർണ്ണസംഖ്യ			last_glyph_rbearing;
    പൂർണ്ണസംഖ്യ			cells = 0;
    പൂർണ്ണസംഖ്യ			i = 0;
    gboolean		has_unicode = FALSE;
    gboolean		needs_shaping = FALSE;

    // Scan the string, and find spans where ligatures should not happen. We
    // must ensure that attribute boundaries do not split a base character from
    // its combining marks, as Pango breaks items at attribute boundaries.
    ആകുമ്പോൾ (i < len)
    {
	പൂർണ്ണസംഖ്യ	clen = 1;
	പൂർണ്ണസംഖ്യ	is_composing = 0;
	bool	want_disable;

	എങ്കിൽ (s[i] >= 0x80)
	{
	    clen = utf_ptr2len(s + i);
	    is_composing = utf_iscomposing(utf_ptr2char(s + i));
	    has_unicode = TRUE;
	}

	എങ്കിൽ (is_composing)
	    // Do not change state mid-cluster. Inherit the previous character's
	    // state.
	    want_disable = (start_idx != -1);
	അല്ലെങ്കിൽ
	    // Base character: disable ligatures if it's an ASCII non-ligature
	    // char.
	    want_disable = (s[i] < 0x80 && !gui.ligatures_map[s[i]]);

	// If any base character actually wants ligature/contextual shaping,
	// the string must go through Pango's shaping engine -- the ASCII
	// fast path below draws cached glyphs one-by-one and can never
	// produce a ligature, regardless of 'guiligatures'.
	എങ്കിൽ (!is_composing && !want_disable)
	    needs_shaping = TRUE;

	എങ്കിൽ (want_disable)
	{
	    എങ്കിൽ (start_idx == -1)
		start_idx = i;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (start_idx != -1)
	{
	    എങ്കിൽ (attr_list == ശൂന്യ)
		attr_list = pango_attr_list_new();
	    തിരുകുക_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, start_idx, i);
	    start_idx = -1;
	}
	i += clen;
    }

    // Fast path for pure ASCII: use cached glyph table. Skip this path when
    // there are non-ascii characters in the string, font attributes, or if
    // there's a possible ligature.
    എങ്കിൽ (!(draw_flags & DRAW_ITALIC)
	    && !((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != ശൂന്യ
	    && !has_unicode
	    && !needs_shaping)
    {
	വേണ്ടി (i = 0; i < len; ++i)
	{
	    PangoGlyphInfo  glyph = gui.ascii_glyphs->glyphs[2 * s[i]];
	    gboolean	    has_ink;

	    has_ink = glyph_has_ink(gui.ascii_font, glyph);

	    draw_row_add_cluster(drow, dattr, col + i, 1,
		    gui.ascii_font, has_ink ? &glyph : ശൂന്യ, 1, self);
	}

	column_offset = len;
	പോകുക exit;
    }

    glyphs = pango_glyph_string_new();

    എങ്കിൽ (attr_list == ശൂന്യ)
	attr_list = pango_attr_list_new();

    എങ്കിൽ (start_idx != -1)
	തിരുകുക_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, start_idx, len);

    cluster_width = PANGO_SCALE * gui.char_width;
    last_glyph_rbearing = PANGO_SCALE * gui.char_width;

    // If 'guifontwide' is set then use that for double-width characters.
    എങ്കിൽ (gui.wide_font != ശൂന്യ)
	apply_wide_font_attr(s, len, attr_list);

    എങ്കിൽ ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	തിരുകുക_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
		attr_list, 0, len);
    എങ്കിൽ (draw_flags & DRAW_ITALIC)
	തിരുകുക_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    item_list = pango_itemize(gui.text_context,
	    (സ്ഥിരം അക്ഷരം *)s, 0, len, attr_list, ശൂന്യ);

    ആകുമ്പോൾ (item_list != ശൂന്യ)
    {
	PangoItem   *item = item_list->data;
	പൂർണ്ണസംഖ്യ	    item_cells = 0;
	പൂർണ്ണസംഖ്യ	    cluster_start_idx = 0;
	പൂർണ്ണസംഖ്യ	    current_cluster_col = col + column_offset;
	പൂർണ്ണസംഖ്യ	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	item_list = g_list_delete_link(item_list, item_list);

	// Force LTR direction; Vim handles bidi on its own.
	item->analysis.level = (item->analysis.level + 1) & (~1U);

	pango_shape_full((സ്ഥിരം അക്ഷരം *)s + item->offset, item->length,
		(സ്ഥിരം അക്ഷരം *)s, len, &item->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	വേണ്ടി (i = 0; i < glyphs->അക്കം_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    എങ്കിൽ (glyph->attr.is_cluster_start)
	    {
		പൂർണ്ണസംഖ്യ cellcount;

		cellcount = എണ്ണം_cluster_cells(
			s, item, glyphs, i, &cluster_width,
			(item_list != ശൂന്യ) ? &last_glyph_rbearing : ശൂന്യ);

		എങ്കിൽ (i > 0 && cellcount > 0)
		{
		    draw_row_add_cluster(drow, dattr,
			    current_cluster_col, current_cluster_cells,
			    item->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_start_idx : ശൂന്യ,
			    i - cluster_start_idx, self);

		    current_cluster_col += current_cluster_cells;
		    cluster_start_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		എങ്കിൽ (cellcount > 0)
		{
		    പൂർണ്ണസംഖ്യ width;

		    width = cellcount * gui.char_width * PANGO_SCALE;
		    glyph->geometry.x_offset +=
			MAX(0, width - cluster_width) / 2;
		    glyph->geometry.width = width;
		}
		അല്ലെങ്കിൽ
		    setup_zero_width_cluster(item, glyph, cells,
			    cluster_width,
			    last_glyph_rbearing);

		item_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (i > 0)
	    {
		പൂർണ്ണസംഖ്യ width;

		എങ്കിൽ (glyph->geometry.x_offset >= 0)
		{
		    glyphs->glyphs[i].geometry.width =
			glyphs->glyphs[i - 1].geometry.width;
		    glyphs->glyphs[i - 1].geometry.width = 0;
		}
		width = cells * gui.char_width * PANGO_SCALE;
		glyph->geometry.x_offset +=
		    MAX(0, width - cluster_width) / 2;
	    }
	    അല്ലെങ്കിൽ
		glyph->geometry.width = 0;


	    എങ്കിൽ (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			item->analysis.font, *glyph);
	}
	എങ്കിൽ (glyphs->അക്കം_glyphs > 0)
	    draw_row_add_cluster(drow, dattr,
		    current_cluster_col, current_cluster_cells,
		    item->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_start_idx : ശൂന്യ,
		    glyphs->അക്കം_glyphs - cluster_start_idx, self);

	pango_item_free(item);
	column_offset += item_cells;
    }

    pango_glyph_string_free(glyphs);

exit:
    എങ്കിൽ (attr_list != ശൂന്യ)
	pango_attr_list_unref(attr_list);

    തിരികെ column_offset;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    പൂർണ്ണസംഖ്യ
vim_draw_area_draw_string(
	VimDrawArea *self,
	പൂർണ്ണസംഖ്യ	    row,
	പൂർണ്ണസംഖ്യ	    col,
	char_u	    *s,
	പൂർണ്ണസംഖ്യ	    len,
	പൂർണ്ണസംഖ്യ	    flags)
{
    DrawRow	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		convlen;
    പൂർണ്ണസംഖ്യ		n_cells ;

    എങ്കിൽ (unlikely(self->rows == ശൂന്യ
		|| row >= self->n_rows
		|| col >= self->n_cols))
	തിരികെ len;

    എങ്കിൽ (output_conv.vc_type != CONV_NONE)
    {
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	എങ്കിൽ (conv_buf != ശൂന്യ)
	{
	    s = conv_buf;
	    len = convlen;
	}
    }

    // Safety check: pango crashes with invalid utf-8.
    എങ്കിൽ (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	തിരികെ len;
    }

    drow = self->rows + row;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_add_string(self, drow, &dattr, col, s, len, flags);

    vim_free(conv_buf);

    തിരികെ n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    ശൂന്യം
vim_draw_area_clear_block(
	VimDrawArea	*self,
	പൂർണ്ണസംഖ്യ		row1,
	പൂർണ്ണസംഖ്യ		col1,
	പൂർണ്ണസംഖ്യ		row2,
	പൂർണ്ണസംഖ്യ		col2)
{
    എങ്കിൽ (unlikely(self->rows == ശൂന്യ
		|| row1 >= self->n_rows
		|| col1 >= self->n_cols
		|| row2 >= self->n_rows
		|| col2 >= self->n_cols))
	തിരികെ;

    വേണ്ടി (പൂർണ്ണസംഖ്യ r = row1; r <= row2; r++)
	draw_row_fill(self->rows + r, col1, col2, ശൂന്യ, ശൂന്യ, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    ശൂന്യം
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_block(self, 0, 0, self->n_rows - 1, self->n_cols - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    ശൂന്യം
vim_draw_area_move_block(
	VimDrawArea *self,
	പൂർണ്ണസംഖ്യ	    to,
	പൂർണ്ണസംഖ്യ	    row1,
	പൂർണ്ണസംഖ്യ	    row2,
	പൂർണ്ണസംഖ്യ	    col1,
	പൂർണ്ണസംഖ്യ	    col2)
{
    പൂർണ്ണസംഖ്യ offset = row2 - row1;

    എങ്കിൽ (unlikely(self->rows == ശൂന്യ
		|| row1 >= self->n_rows
		|| row2 >= self->n_rows
		|| to >= self->n_rows
		|| col1 >= self->n_cols
		|| col2 >= self->n_cols))
	തിരികെ;

    പ്രതിജ്ഞാ(row2 >= row1);
    പ്രതിജ്ഞാ(col2 >= col1);
    പ്രതിജ്ഞാ(row1 != to);

    എങ്കിൽ (row1 > to)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	വേണ്ടി (പൂർണ്ണസംഖ്യ o = 0; o <= offset; o++)
	    draw_row_move_to(self->rows + to + o, self->rows + row1 + o,
		    col1, col2, self);
    }
    അല്ലെങ്കിൽ
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	വേണ്ടി (പൂർണ്ണസംഖ്യ o = offset; o >= 0; o--)
	    എങ്കിൽ (to + o >= self->n_rows)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_block(self, row1 + o, col1, row1 + o, col2);
	    അല്ലെങ്കിൽ
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
    ശൂന്യം
vim_draw_area_set_cursor(VimDrawArea *self, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h)
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
    ശൂന്യം
vim_draw_area_invert_block(
	VimDrawArea	*self,
	പൂർണ്ണസംഖ്യ		row,
	പൂർണ്ണസംഖ്യ		col,
	പൂർണ്ണസംഖ്യ		nrows,
	പൂർണ്ണസംഖ്യ		ncols)
{
    എങ്കിൽ (unlikely(self->rows == ശൂന്യ
		|| row >= self->n_rows
		|| col >= self->n_cols
		|| row + nrows - 1 >= self->n_rows
		|| col + ncols - 1 >= self->n_cols))
	തിരികെ;

    വേണ്ടി (പൂർണ്ണസംഖ്യ r = row; r < row + nrows; r++)
    {
	DrawRow	    *drow = self->rows + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	വേണ്ടി (പൂർണ്ണസംഖ്യ c = 0; c < ncols; c++)
	    dlayer->buf[c + col] = ~dlayer->buf[c + col];
	draw_row_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#എങ്കിൽ defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    ശൂന്യം
vim_draw_area_add_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	പൂർണ്ണസംഖ്യ	    row,
	പൂർണ്ണസംഖ്യ	    col,
	പൂർണ്ണസംഖ്യ	    width,
	പൂർണ്ണസംഖ്യ	    height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    പൂർണ്ണസംഖ്യ		cells;

    എങ്കിൽ (unlikely(self->rows == ശൂന്യ
		|| row >= self->n_rows
		|| col >= self->n_cols))
	തിരികെ;

    drow = self->rows + row;
    dsign = draw_sign_new(sign, width, height);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (width + gui.char_width - 1) / gui.char_width;

    draw_row_fill(drow, col, col + cells - 1, dglyphs, ശൂന്യ, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    ശൂന്യം
vim_draw_area_add_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	പൂർണ്ണസംഖ്യ		row,
	പൂർണ്ണസംഖ്യ		col,
	പൂർണ്ണസംഖ്യ		width,
	പൂർണ്ണസംഖ്യ		height)
{
    DrawRow	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    പൂർണ്ണസംഖ്യ		cells;
    GdkTexture	*texture;
    GBytes	*bytes;

    എങ്കിൽ (unlikely(self->rows == ശൂന്യ
		|| row >= self->n_rows
		|| col >= self->n_cols))
	തിരികെ;

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

    draw_row_fill(drow, col, col + cells - 1, dglyphs, ശൂന്യ, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#endif

#ifdef FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    ശൂന്യം
vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_കണ്ണി *കണ്ണി)
{
    g_hash_table_add(self->external, gsk_render_കണ്ണി_ref(കണ്ണി));
}

    ശൂന്യം
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_കണ്ണി *കണ്ണി)
{
    g_hash_table_remove(self->external, കണ്ണി);
}

#endif

/*
 * Add the cursor to the snapshot.
 */
    സ്ഥിതം ശൂന്യം
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_കണ്ണി	*text_കണ്ണി,	// May be NULL
	Gsk_Render_കണ്ണി	*special_കണ്ണി,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    പൂർണ്ണസംഖ്യ		    w = cursor->width;
    പൂർണ്ണസംഖ്യ		    h = cursor->height;
    graphene_rect_t rect;

    എങ്കിൽ (!cursor->draw)
	തിരികെ;

    എങ്കിൽ (cursor->width <= 0 && cursor->height <= 0)
    {
	w += gui.char_width * cursor->n_cells;
	h = gui.char_height;
    }

    graphene_rect_init(&rect,
#ifdef FEAT_RIGHTLEFT
	    CURSOR_BAR_വലത് ? FILL_X(gui.cursor_col + 1) - w :
#endif
	    FILL_X(gui.cursor_col),
	    FILL_Y(gui.cursor_row) + gui.char_height - h, w, h);


    എങ്കിൽ (cursor->width == -1 && cursor->height == -1)
    {
	// Create hollow cursor
	GskRoundedRect	    outline;
	സ്ഥിതം സ്ഥിരം ദശാംശം  border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	സ്ഥിരം GdkRGBA	    color[4] = {
	    cursor->bg_color, cursor->bg_color,
	    cursor->bg_color, cursor->bg_color
	} ;

	gsk_rounded_rect_init_from_rect(&outline, &rect, 0.0f);
	gtk_snapshot_append_border(snapshot, &outline, border, color);
    }
    അല്ലെങ്കിൽ
    {
	// Create block style cursor, by masking the text + under decorations
	// with the fg color.
	gtk_snapshot_append_color(snapshot, &cursor->bg_color, &rect);

	gtk_snapshot_തള്ളുക_mask(snapshot, GSK_MASK_MODE_ALPHA);

	// Used as an "outline" for the fg color.
	എങ്കിൽ (text_കണ്ണി != ശൂന്യ)
	    gtk_snapshot_append_കണ്ണി(snapshot, text_കണ്ണി);
	എങ്കിൽ (special_കണ്ണി != ശൂന്യ)
	    gtk_snapshot_append_കണ്ണി(snapshot, special_കണ്ണി);

	gtk_snapshot_എടുക്കുക(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_എടുക്കുക(snapshot);
    }
}

    സ്ഥിതം ശൂന്യം
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    പൂർണ്ണസംഖ്യ		    height, width;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = ശൂന്യ;
    Gsk_Render_കണ്ണി   *body_കണ്ണി;
    Gsk_Render_കണ്ണി   *invert_കണ്ണി = ശൂന്യ;
#ifdef FEAT_IMAGE_GUI
    Gsk_Render_കണ്ണി   *ext_കണ്ണി;
    GHashTableIter  iter;
#endif

    gui_mch_set_bg_color(gui.back_pixel);
    height = gtk_widget_get_height(widget) + gui.bleed_bot;
    width = gtk_widget_get_width(widget) + gui.bleed_വലത്;

    എങ്കിൽ (self->rows == ശൂന്യ)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, width, height));
	തിരികെ;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    എങ്കിൽ (self->bleed_വലത് == -1 || self->bleed_വലത് != gui.bleed_വലത്)
    {
	self->bleed_വലത് = gui.bleed_വലത്;
	വേണ്ടി (പൂർണ്ണസംഖ്യ r = 0; r < self->n_rows; r++)
	{
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_BACKGROUND);
	    draw_row_dirty_layer(self->rows + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#എങ്കിൽ GTK_CHECK_VERSION(4,24,0)
    gtk_snapshot_set_snap(body_snapshot, GSK_RECT_SNAP_ROUND);
#endif

    വേണ്ടി (പൂർണ്ണസംഖ്യ r = 0; r < self->n_rows; r++)
    {
	DrawRow *drow = self->rows + r;

	draw_row_render_background(drow, self);
	draw_row_render_text(drow, self);
	draw_row_render_special(drow, self);
	draw_row_render_overlay(drow, self);

	വേണ്ടി (പൂർണ്ണസംഖ്യ l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    എങ്കിൽ (dlayer->കണ്ണി != ശൂന്യ)
	    {
		എങ്കിൽ (l == DRAW_LAYER_OVERLAY)
		{
		    എങ്കിൽ (invert_snapshot == ശൂന്യ)
		    {
			invert_snapshot = gtk_snapshot_new();
#എങ്കിൽ GTK_CHECK_VERSION(4,24,0)
			gtk_snapshot_set_snap(invert_snapshot,
				GSK_RECT_SNAP_ROUND);
#endif
		    }
		    gtk_snapshot_append_കണ്ണി(invert_snapshot, dlayer->കണ്ണി);
		}
		അല്ലെങ്കിൽ
		    gtk_snapshot_append_കണ്ണി(body_snapshot, dlayer->കണ്ണി);
	    }
	}

	എങ്കിൽ (r == gui.cursor_row)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_TEXT].കണ്ണി,
		    drow->dlayers[DRAW_LAYER_SPECIAL].കണ്ണി,
		    body_snapshot);
    }

    എങ്കിൽ (invert_snapshot != ശൂന്യ)
	gtk_snapshot_തള്ളുക_blend(snapshot, GSK_BLEND_MODE_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, width, height));
    body_കണ്ണി = gtk_snapshot_free_to_കണ്ണി(body_snapshot);
    എങ്കിൽ (body_കണ്ണി != ശൂന്യ)
    {
	gtk_snapshot_append_കണ്ണി(snapshot, body_കണ്ണി);
	gsk_render_കണ്ണി_unref(body_കണ്ണി);
    }

#ifdef FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_table_iter_init(&iter, self->external);

    ആകുമ്പോൾ (g_hash_table_iter_next(&iter, (ശൂന്യം **)&ext_കണ്ണി, ശൂന്യ))
	gtk_snapshot_append_കണ്ണി(snapshot, ext_കണ്ണി);
#endif

    എങ്കിൽ (invert_snapshot != ശൂന്യ)
    {
	gtk_snapshot_എടുക്കുക(snapshot);
	invert_കണ്ണി = gtk_snapshot_free_to_കണ്ണി(invert_snapshot);
	// Not sure if it can be NULL but still check
	എങ്കിൽ (invert_കണ്ണി != ശൂന്യ)
	{
	    gtk_snapshot_append_കണ്ണി(snapshot, invert_കണ്ണി);
	    gsk_render_കണ്ണി_unref(invert_കണ്ണി);
	}
	gtk_snapshot_എടുക്കുക(snapshot);
    }
}
