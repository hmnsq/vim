/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <gtk/gtk.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "gui_gtk4_da.h"


#ਜੇ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Used for sign icons and netbeans multisign indicator. It is in a DrawGlyphs
 * so that it can move/be manipulated with the text.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    GdkTexture	*texture;
    ਪੂਰਨ		ਚੌੜਾਈ;
    ਪੂਰਨ		ਉਚਾਈ;
} DrawSign;
#ਜੇ_ਅੰਤ

/*
 * Temporary struct when drawing string representing visual attributes of a cell
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    uint32_t	bg_color;
    uint32_t	fg_color;   // Only used for underlines
    uint32_t	sp_color;
    ਪੂਰਨ		draw_flags; // DRAW_* flags, only DRAW_UNDERL, DRAW_UNDERC, and
			    // DRAW_STRIKE are used.
} DrawAttr;

/*
 * Represents the glyphs of a single cell.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ refcount;

    uint32_t	fg_color;
    PangoFont	*font;
    // If "font" is NULL, "dsign" is used, otherwise "glyphs" is used.
    ਸੰਘ
    {
	PangoGlyphInfo  *glyphs; // May be NULL
#ਜੇ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	DrawSign	*dsign;
#ਜੇ_ਅੰਤ
    };
    ਪੂਰਨ n_glyphs;
} DrawGlyphs;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    DrawGlyphs *dglyphs;
} DrawCell;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    DRAW_LAYER_BACKGROUND,  // Background color, buf stores bg color
    DRAW_LAYER_ਪਾਠ,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Gsk_Render_ਗੰਢ   *ਗੰਢ;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNDERC_ਵਿੱਥ(n_ਖਾਨੇ_ਗਿਣਤੀ) (0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRIKE_ਵਿੱਥ(n_ਖਾਨੇ_ਗਿਣਤੀ) (n_ਖਾਨੇ_ਗਿਣਤੀ)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    ਪੂਰਨ		n_cells; // For convenience
    ਪੂਰਨ		ਪੰਕਤੀ;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_ਗੰਢ *underl_ਨਕਾਬ;
    Gsk_Render_ਗੰਢ *underc_ਨਕਾਬ;
    Gsk_Render_ਗੰਢ *strike_ਨਕਾਬ;
} Draw_ਪੰਕਤੀ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਾਓ_PANGO_ATTR(Attribute, AttrList, ਸ਼ੁਰੂ_2, ਸਿਰਾ_2) \
    ਕਰੋ { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = (ਸ਼ੁਰੂ_2); \
	tmp_attr_->ਸਿਰਾ_ਸੂਚਕਾਂਕ = (ਸਿਰਾ_2); \
	pango_attr_list_ਪਾਓ((AttrList), tmp_attr_); \
    } ਜਦੋਂ (FALSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CELLS2PANGO(c) ((c) * gui.char_ਚੌੜਾਈ * PANGO_SCALE)

/*
 * Attributes of cursor
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    gboolean draw; // If cursor should be drawn
    ਪੂਰਨ ਚੌੜਾਈ;
    ਪੂਰਨ ਉਚਾਈ;
    ਪੂਰਨ n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

ਬਣਤਰ _VimDrawArea
{
    GtkWidget ਮਾਪਾ;

    Draw_ਪੰਕਤੀ *ਪੰਕਤੀਆਂ_2;

    ਪੂਰਨ n_ਪੰਕਤੀਆਂ;
    ਪੂਰਨ	n_ਖਾਨੇ_ਗਿਣਤੀ;

    ਪੂਰਨ	bleed_ਸੱਜਾ;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*ਗੰਢ_buf;

    DrawCursor cursor;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHash_ਸਾਰਣੀ *external;
#ਜੇ_ਅੰਤ
};

ਸਥਿਰ ਖਾਲੀ draw_ਪੰਕਤੀ_init(Draw_ਪੰਕਤੀ *drow, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ);
ਸਥਿਰ ਖਾਲੀ draw_ਪੰਕਤੀ_clear(Draw_ਪੰਕਤੀ *drow);
ਸਥਿਰ ਖਾਲੀ draw_ਪੰਕਤੀ_dirty_layer(Draw_ਪੰਕਤੀ *drow, DrawLayerType dlayer_t);
ਸਥਿਰ ਖਾਲੀ vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

ਸਥਿਰ GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    ਸਥਿਰ ਖਾਲੀ
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    ਲਈ (ਪੂਰਨ r = 0; r < self->n_ਪੰਕਤੀਆਂ; r++)
	draw_ਪੰਕਤੀ_clear(self->ਪੰਕਤੀਆਂ_2 + r);
    g_free(self->ਪੰਕਤੀਆਂ_2);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->ਗੰਢ_buf, TRUE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_GUI
    g_hash_ਸਾਰਣੀ_unref(self->external);
#ਜੇ_ਅੰਤ

    G_OBJECT_CLASS(vim_draw_area_ਮਾਪਾ_class)->finalize(obj);
}

    ਸਥਿਰ ਖਾਲੀ
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_ਸੈੱਟ_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    ਸਥਿਰ ਖਾਲੀ
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_ਸੱਜਾ = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_GUI
    self->external = g_hash_ਸਾਰਣੀ_new_ਭਰਿਆ(g_ਸਿੱਧਾ_hash, g_ਸਿੱਧਾ_equal,
	    ਨਿਰਾ, (GDestroyNotify)gsk_render_ਗੰਢ_unref);
#ਜੇ_ਅੰਤ
    self->glyph_buf = g_array_new(FALSE, FALSE, ਆਕਾਰ(PangoGlyphInfo));
    self->ਗੰਢ_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_ਗੰਢ_unref);

}

    GtkWidget *
vim_draw_area_new(ਖਾਲੀ)
{
    ਵਾਪਸ g_object_new(VIM_TYPE_DRAW_AREA, ਨਿਰਾ);
}

/*
 * Set the size of the draw area to "rows" and "cols".
 */
    ਖਾਲੀ
vim_draw_area_ਸੈੱਟ_ਮਾਪ(VimDrawArea *self, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
    ਜੇ (self->ਪੰਕਤੀਆਂ_2 != ਨਿਰਾ && self->n_ਪੰਕਤੀਆਂ == ਪੰਕਤੀਆਂ_2 && self->n_ਖਾਨੇ_ਗਿਣਤੀ == ਖਾਨੇ_ਗਿਣਤੀ)
	ਵਾਪਸ;
    ਜੇ (ਪੰਕਤੀਆਂ_2 == 0 || ਖਾਨੇ_ਗਿਣਤੀ == 0)
	ਵਾਪਸ;

    vim_draw_area_clear(self);

    ਲਈ (ਪੂਰਨ r = 0; r < self->n_ਪੰਕਤੀਆਂ; r++)
	draw_ਪੰਕਤੀ_clear(self->ਪੰਕਤੀਆਂ_2 + r);
    self->ਪੰਕਤੀਆਂ_2 = g_realloc_n(self->ਪੰਕਤੀਆਂ_2, ਪੰਕਤੀਆਂ_2, ਆਕਾਰ(Draw_ਪੰਕਤੀ));
    ਲਈ (ਪੂਰਨ r = 0; r < ਪੰਕਤੀਆਂ_2; r++)
	draw_ਪੰਕਤੀ_init(self->ਪੰਕਤੀਆਂ_2 + r, r, ਖਾਨੇ_ਗਿਣਤੀ);

    self->n_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ_2;
    self->n_ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    ਸਥਿਰ uint32_t
rgba_ਮੰਜ਼ਿਲ_u32(ਅਚਲ GdkRGBA *rgba)
{
    ਸੰਘ { uint32_t val; uint8_t ਬਾਈਟਾਂ[4]; } color;

    color.ਬਾਈਟਾਂ[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.ਬਾਈਟਾਂ[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.ਬਾਈਟਾਂ[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.ਬਾਈਟਾਂ[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    ਵਾਪਸ color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    ਸਥਿਰ GdkRGBA
u32_ਮੰਜ਼ਿਲ_rgba(uint32_t val)
{
    ਸੰਘ { uint32_t val; uint8_t ਬਾਈਟਾਂ[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.ਬਾਈਟਾਂ[0] / 255.0;
    color.green = packed.ਬਾਈਟਾਂ[1] / 255.0;
    color.blue  = packed.ਬਾਈਟਾਂ[2] / 255.0;
    color.alpha = packed.ਬਾਈਟਾਂ[3] / 255.0;
    ਵਾਪਸ color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    ਸਥਿਰ gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    ਸੰਘ { uint32_t val; uint8_t ਬਾਈਟਾਂ[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.ਬਾਈਟਾਂ[0] << 16) |
	((guicolor_T)packed.ਬਾਈਟਾਂ[1] << 8)  |
	packed.ਬਾਈਟਾਂ[2];
    ਵਾਪਸ res == rgb;
}

/*
 * Checks if a RGBA pixel buffer is fully transparent.
 */
    ਸਥਿਰ ਸਤਰਅੰਦਰ gboolean
rgba_is_transparent(ਅਚਲ uint32_t *buf, ਪੂਰਨ n_pixels)
{
    // Could also compare 2 pixels at once using a 64 bit integer, but not sure
    // if that might cause alignment issues...
    ਲਈ (ਪੂਰਨ i = 0; i < n_pixels; i++)
	ਜੇ (buf[i] != 0)
	    ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

/*
 * Return TRUE if "glyph" has ink (not whitespace).
 */
    ਸਥਿਰ gboolean
glyph_has_ink(PangoFont *font, PangoGlyphInfo glyph)
{
    PangoRectangle ink;

    pango_font_get_glyph_extents (font, glyph.glyph, &ink, ਨਿਰਾ);

    ਵਾਪਸ ink.ਚੌੜਾਈ > 0 && ink.ਉਚਾਈ > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    ਸਥਿਰ ਖਾਲੀ
apply_wide_font_attr(char_u *s, ਪੂਰਨ len, PangoAttrList *attr_list)
{
    char_u  *ਸ਼ੁਰੂ = ਨਿਰਾ;
    char_u  *p;
    ਪੂਰਨ	    uc;

    ਲਈ (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	ਜੇ (ਸ਼ੁਰੂ == ਨਿਰਾ)
	{
	    ਜੇ (uc >= 0x80 && utf_char2cells(uc) == 2)
		ਸ਼ੁਰੂ = p;
	}
	ਨਹੀਂਤਾਂ ਜੇ (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    ਪਾਓ_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, ਸ਼ੁਰੂ - s, p - s);
	    ਸ਼ੁਰੂ = ਨਿਰਾ;
	}
    }

    ਜੇ (ਸ਼ੁਰੂ != ਨਿਰਾ)
	ਪਾਓ_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, ਸ਼ੁਰੂ - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    ਸਥਿਰ ਪੂਰਨ
ਗਿਣਤੀ_cluster_cells(
	char_u		    *s,
	Pango_ਵਸਤੂ	    *ਵਸਤੂ,
	PangoGlyphString    *glyphs,
	ਪੂਰਨ		    i,
	ਪੂਰਨ		    *cluster_ਚੌੜਾਈ,
	ਪੂਰਨ		    *ਆਖਰੀ_glyph_rbearing)
{
    char_u  *p;
    ਪੂਰਨ	    ਅਗਲਾ;
    ਪੂਰਨ	    ਸ਼ੁਰੂ, ਸਿਰਾ;
    ਪੂਰਨ	    ਚੌੜਾਈ;
    ਪੂਰਨ	    uc;
    ਪੂਰਨ	    cellcount = 0;

    ਚੌੜਾਈ = glyphs->glyphs[i].geometry.ਚੌੜਾਈ;

    ਲਈ (ਅਗਲਾ = i + 1; ਅਗਲਾ < glyphs->ਅੰਕ_glyphs; ++ਅਗਲਾ)
    {
	ਜੇ (glyphs->glyphs[ਅਗਲਾ].attr.is_cluster_ਸ਼ੁਰੂ)
	    ਰੋਕੋ;
	ਨਹੀਂਤਾਂ ਜੇ (glyphs->glyphs[ਅਗਲਾ].geometry.ਚੌੜਾਈ > ਚੌੜਾਈ)
	    ਚੌੜਾਈ = glyphs->glyphs[ਅਗਲਾ].geometry.ਚੌੜਾਈ;
    }

    ਸ਼ੁਰੂ = ਵਸਤੂ->ਵਿੱਥ + glyphs->log_clusters[i];
    ਸਿਰਾ   = ਵਸਤੂ->ਵਿੱਥ + ((ਅਗਲਾ < glyphs->ਅੰਕ_glyphs) ?
	    glyphs->log_clusters[ਅਗਲਾ] : ਵਸਤੂ->ਲੰਮਾਈ);

    ਲਈ (p = s + ਸ਼ੁਰੂ; p < s + ਸਿਰਾ; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	ਜੇ (uc < 0x80)
	    ++cellcount;
	ਨਹੀਂਤਾਂ ਜੇ (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    ਜੇ (ਆਖਰੀ_glyph_rbearing != ਨਿਰਾ
	    && cellcount > 0 && ਅਗਲਾ == glyphs->ਅੰਕ_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(ਵਸਤੂ->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, ਨਿਰਾ);

	ਜੇ (PANGO_RBEARING(ink_rect) > 0)
	    *ਆਖਰੀ_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    ਜੇ (cellcount > 0)
	*cluster_ਚੌੜਾਈ = ਚੌੜਾਈ;

    ਵਾਪਸ cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    ਸਥਿਰ ਖਾਲੀ
ਤਿਆਰੀ_zero_ਚੌੜਾਈ_cluster(
	Pango_ਵਸਤੂ	*ਵਸਤੂ,
	PangoGlyphInfo	*glyph,
	ਪੂਰਨ		ਆਖਰੀ_cellcount,
	ਪੂਰਨ		ਆਖਰੀ_cluster_ਚੌੜਾਈ,
	ਪੂਰਨ		ਆਖਰੀ_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    ਪੂਰਨ		    ਚੌੜਾਈ;

    ਚੌੜਾਈ = ਆਖਰੀ_cellcount * gui.char_ਚੌੜਾਈ * PANGO_SCALE;
    glyph->geometry.x_ਵਿੱਥ = -ਚੌੜਾਈ + MAX(0, ਚੌੜਾਈ - ਆਖਰੀ_cluster_ਚੌੜਾਈ) / 2;
    glyph->geometry.ਚੌੜਾਈ = 0;

    pango_font_get_glyph_extents(ਵਸਤੂ->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    ਜੇ (ink_rect.x < 0)
    {
	glyph->geometry.x_ਵਿੱਥ += ਆਖਰੀ_glyph_rbearing;
	glyph->geometry.y_ਵਿੱਥ  = logical_rect.ਉਚਾਈ
	    - (gui.char_ਉਚਾਈ - p_linespace) * PANGO_SCALE;
    }
    ਨਹੀਂਤਾਂ
	glyph->geometry.x_ਵਿੱਥ = -ਚੌੜਾਈ + MAX(0, ਚੌੜਾਈ - ink_rect.ਚੌੜਾਈ) / 2;
}

#ਜੇ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    ਸਥਿਰ DrawSign *
draw_sign_new(GdkTexture *texture, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->ਚੌੜਾਈ = ਚੌੜਾਈ;
    dsign->ਉਚਾਈ = ਉਚਾਈ;

    ਵਾਪਸ dsign;
}

    ਸਥਿਰ ਖਾਲੀ
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
draw_attr_init(
	DrawAttr	*dattr,
	ਅਚਲ GdkRGBA	*bg_color,
	ਅਚਲ GdkRGBA	*fg_color,
	ਅਚਲ GdkRGBA	*sp_color,
	ਪੂਰਨ		draw_flags)
{
    dattr->bg_color = rgba_ਮੰਜ਼ਿਲ_u32(bg_color);
    dattr->fg_color = rgba_ਮੰਜ਼ਿਲ_u32(fg_color);
    dattr->sp_color = rgba_ਮੰਜ਼ਿਲ_u32(sp_color);
    dattr->draw_flags = draw_flags & (DRAW_UNDERL | DRAW_UNDERC | DRAW_STRIKE);
}

/*
 * Apply the draw attr to the row for the given col/cell. Only dirty layers that
 * have actually changed.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_attr_apply(DrawAttr *dattr, ਪੂਰਨ ਖਾਨਾ, Draw_ਪੰਕਤੀ *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + ਖਾਨਾ;
    uint32_t	*ਪਾਠ_cell = drow->dlayers[DRAW_LAYER_ਪਾਠ].buf + ਖਾਨਾ;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    ਪੂਰਨ		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	ਖਾਨਾ + UNDERC_ਵਿੱਥ(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	ਖਾਨਾ + STRIKE_ਵਿੱਥ(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    ਜੇ (dattr != ਨਿਰਾ)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.ਪਿੱਠ_pixel, bg_color);
    }

    ਜੇ (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_ਪੰਕਤੀ_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	ਜੇ (bg_is_default)
	    *bg_cell = 0;
	ਨਹੀਂਤਾਂ
	    *bg_cell = bg_color;
    }
    ਜੇ ((draw_flags & DRAW_UNDERL && *ਪਾਠ_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *ਪਾਠ_cell != 0))
    {
	draw_ਪੰਕਤੀ_dirty_layer(drow, DRAW_LAYER_ਪਾਠ);
	ਜੇ (draw_flags & DRAW_UNDERL)
	    *ਪਾਠ_cell = fg_color;
	ਨਹੀਂਤਾਂ
	    *ਪਾਠ_cell = 0;
    }
    ਜੇ ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_ਪੰਕਤੀ_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	ਜੇ (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	ਨਹੀਂਤਾਂ
	    *underc_cell = 0;
    }
    ਜੇ ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_ਪੰਕਤੀ_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	ਜੇ (draw_flags & DRAW_STRIKE)
	    *strike_cell = sp_color;
	ਨਹੀਂਤਾਂ
	    *strike_cell = 0;
    }
}

    ਸਥਿਰ DrawGlyphs *
draw_glyphs_new(
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	ਪੂਰਨ		n_glyphs,
	uint32_t	fg_color)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->font = g_object_ref(font);
    dglyphs->glyphs = g_memdup2(glyphs, ਆਕਾਰ(*glyphs) * n_glyphs);
    dglyphs->n_glyphs = n_glyphs;
    dglyphs->fg_color = fg_color;

    ਵਾਪਸ dglyphs;
}

#ਜੇ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw glyphs that shows the given sign. Note that ownership of
 * "dsign" is taken.
 */
    ਸਥਿਰ DrawGlyphs *
draw_glyphs_new_sign(DrawSign *dsign)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->dsign = dsign;

    ਵਾਪਸ dglyphs;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ DrawGlyphs *
draw_glyphs_ref(DrawGlyphs *dglyphs)
{
    dglyphs->refcount++;
    ਵਾਪਸ dglyphs;
}

    ਸਥਿਰ ਖਾਲੀ
draw_glyphs_unref(DrawGlyphs *dglyphs)
{
    ਜੇ (--dglyphs->refcount <= 0)
    {
#ਜੇ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	ਜੇ (dglyphs->font == ਨਿਰਾ)
	    draw_sign_free(dglyphs->dsign);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
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
    ਸਥਿਰ gboolean
draw_glyphs_same(DrawGlyphs *a, DrawGlyphs *b)
{
    ਜੇ (a == ਨਿਰਾ && b == ਨਿਰਾ)
	ਵਾਪਸ TRUE;
    ਜੇ (a == ਨਿਰਾ || b == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਜੇ (a->fg_color != b->fg_color)
	ਵਾਪਸ FALSE;
    ਜੇ (a->n_glyphs != b->n_glyphs || a->font != b->font)
	ਵਾਪਸ FALSE;
    ਲਈ (ਪੂਰਨ i = 0; i < a->n_glyphs; i++)
	ਜੇ (a->glyphs[i].glyph != b->glyphs[i].glyph
		|| a->glyphs[i].geometry.ਚੌੜਾਈ != b->glyphs[i].geometry.ਚੌੜਾਈ
		|| a->glyphs[i].geometry.x_ਵਿੱਥ != b->glyphs[i].geometry.x_ਵਿੱਥ
		|| a->glyphs[i].geometry.y_ਵਿੱਥ != b->glyphs[i].geometry.y_ਵਿੱਥ
		|| a->glyphs[i].attr.is_cluster_ਸ਼ੁਰੂ != b->glyphs[i].attr.is_cluster_ਸ਼ੁਰੂ)
	    ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    ਸਥਿਰ Gsk_Render_ਗੰਢ *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	ਪੂਰਨ	    ਪੰਕਤੀ,
	VimDrawArea *da,
	ਪੂਰਨ	    off,
	gboolean    bleed)
{
    GBytes	    *ਬਾਈਟਾਂ;
    GdkTexture	    *texture;
    Gsk_Render_ਗੰਢ   *ਗੰਢ;
    ਪੂਰਨ		    ਉਚਾਈ;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(4,24,0)
    ਉਚਾਈ = gui.char_ਉਚਾਈ;
#ਨਹੀਂਤਾਂ
    ਉਚਾਈ = gui.char_ਉਚਾਈ + 1;
#ਜੇ_ਅੰਤ

    ਜੇ (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		ਆਕਾਰ(uint32_t) * da->n_ਖਾਨੇ_ਗਿਣਤੀ);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_ਖਾਨੇ_ਗਿਣਤੀ - 1)),
		ਆਕਾਰ(uint32_t)); // Add bleed using last pixel of buffer
	ਬਾਈਟਾਂ = g_byte_array_free_ਮੰਜ਼ਿਲ_ਬਾਈਟਾਂ(arr);
    }
    ਨਹੀਂਤਾਂ
	ਬਾਈਟਾਂ = g_ਬਾਈਟਾਂ_new(dlayer->buf + off, ਆਕਾਰ(uint32_t) * da->n_ਖਾਨੇ_ਗਿਣਤੀ);

    texture = gdk_ਸਮ੍ਰਿਤੀ_texture_new(da->n_ਖਾਨੇ_ਗਿਣਤੀ + bleed, 1,
	    GDK_ਸਮ੍ਰਿਤੀ_R8_G8_B8_A8, ਬਾਈਟਾਂ, (da->n_ਖਾਨੇ_ਗਿਣਤੀ + bleed) * 4);
    g_ਬਾਈਟਾਂ_unref(ਬਾਈਟਾਂ);

    // Scale texture to actual size
    ਗੰਢ = gsk_texture_scale_ਗੰਢ_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(ਪੰਕਤੀ),
		(da->n_ਖਾਨੇ_ਗਿਣਤੀ + bleed) * gui.char_ਚੌੜਾਈ, gui.char_ਉਚਾਈ + bleed),
	    GSK_SCALING_FILTER_ਸਭ_ਤੋਂ_ਨੇੜੇ);
    ਜੇ (bleed)
    {
	Gsk_Render_ਗੰਢ *new;

	new = gsk_clip_ਗੰਢ_new(ਗੰਢ,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(ਪੰਕਤੀ),
		    da->n_ਖਾਨੇ_ਗਿਣਤੀ * gui.char_ਚੌੜਾਈ + da->bleed_ਸੱਜਾ, ਉਚਾਈ));
	gsk_render_ਗੰਢ_unref(ਗੰਢ);
	ਗੰਢ = new;
    }
    g_object_unref(texture);
    ਵਾਪਸ ਗੰਢ;
}

    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_init(Draw_ਪੰਕਤੀ *drow, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
    drow->cells = g_malloc0_n(ਖਾਨੇ_ਗਿਣਤੀ, ਆਕਾਰ(DrawCell));
    drow->n_cells = ਖਾਨੇ_ਗਿਣਤੀ;
    drow->ਪੰਕਤੀ = ਪੰਕਤੀ;
    drow->underl_ਨਕਾਬ = drow->underc_ਨਕਾਬ = drow->strike_ਨਕਾਬ = ਨਿਰਾ;

    memset(drow->dlayers, 0, ਆਕਾਰ(DrawLayer) * N_DRAW_LAYERS);
    ਲਈ (ਪੂਰਨ l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	ਜੇ (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(ਖਾਨੇ_ਗਿਣਤੀ * 2, 4);
	ਨਹੀਂਤਾਂ
	    dlayer->buf = g_malloc0_n(ਖਾਨੇ_ਗਿਣਤੀ, 4);
    }
}

    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_clear(Draw_ਪੰਕਤੀ *drow)
{
    g_free(drow->cells);
    ਲਈ (ਪੂਰਨ l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	ਜੇ (dlayer->ਗੰਢ != ਨਿਰਾ)
	    gsk_render_ਗੰਢ_unref(dlayer->ਗੰਢ);
	g_free(dlayer->buf);
    }
    g_clear_ਸੂਚਕ(&drow->underl_ਨਕਾਬ, gsk_render_ਗੰਢ_unref);
    g_clear_ਸੂਚਕ(&drow->underc_ਨਕਾਬ, gsk_render_ਗੰਢ_unref);
    g_clear_ਸੂਚਕ(&drow->strike_ਨਕਾਬ, gsk_render_ਗੰਢ_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_dirty_layer(Draw_ਪੰਕਤੀ *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_ਸੂਚਕ(&dlayer->ਗੰਢ, gsk_render_ਗੰਢ_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_uninvert_cell(Draw_ਪੰਕਤੀ *drow, ਪੂਰਨ ਖਾਨਾ)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    ਜੇ (buf[ਖਾਨਾ] == 0xffffffff)
    {
	draw_ਪੰਕਤੀ_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[ਖਾਨਾ] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_ਸੈੱਟ_cell(Draw_ਪੰਕਤੀ *drow, ਪੂਰਨ ਖਾਨਾ, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + ਖਾਨਾ;

    ਜੇ (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_ਪੰਕਤੀ_dirty_layer(drow, DRAW_LAYER_ਪਾਠ);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    ਜੇ (dcell->dglyphs != dglyphs)
    {
	ਜੇ (dcell->dglyphs != ਨਿਰਾ)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == ਨਿਰਾ ? ਨਿਰਾ : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, ਖਾਨਾ, drow);
    draw_ਪੰਕਤੀ_uninvert_cell(drow, ਖਾਨਾ);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_fill(
	Draw_ਪੰਕਤੀ	    *drow,
	ਪੂਰਨ	    col1,
	ਪੂਰਨ	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    ਲਈ (ਪੂਰਨ c = col1; c <= col2; c++)
	draw_ਪੰਕਤੀ_ਸੈੱਟ_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    ਜੇ (gui.cursor_ਪੰਕਤੀ == drow->ਪੰਕਤੀ && gui.cursor_ਖਾਨਾ >= col1
	    && gui.cursor_ਖਾਨਾ <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_move_ਮੰਜ਼ਿਲ(
	Draw_ਪੰਕਤੀ	    *dest_ਪੰਕਤੀ,
	Draw_ਪੰਕਤੀ	    *src_ਪੰਕਤੀ,
	ਪੂਰਨ	    col1,
	ਪੂਰਨ	    col2,
	VimDrawArea *da)
{
    ਪੂਰਨ move_ਮਾਪ = (col2 - col1 + 1) * ਆਕਾਰ(DrawCell);

    draw_ਪੰਕਤੀ_fill(dest_ਪੰਕਤੀ, col1, col2, ਨਿਰਾ, ਨਿਰਾ, &da->cursor);
    memmove(dest_ਪੰਕਤੀ->cells + col1, src_ਪੰਕਤੀ->cells + col1, move_ਮਾਪ);
    // NULL the draw cells so we don't double unreference.
    memset(src_ਪੰਕਤੀ->cells + col1, 0, move_ਮਾਪ);
    draw_ਪੰਕਤੀ_dirty_layer(src_ਪੰਕਤੀ, DRAW_LAYER_ਪਾਠ);

    // Move the layer buffers
    move_ਮਾਪ = (col2 - col1 + 1) * 4;
    ਲਈ (ਪੂਰਨ l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_ਪੰਕਤੀ->dlayers[l].buf;
	uint32_t *dest = dest_ਪੰਕਤੀ->dlayers[l].buf;

	ਜੇ (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_ਵਿੱਥ(dest_ਪੰਕਤੀ->n_cells),
		    src + col1 + UNDERC_ਵਿੱਥ(src_ਪੰਕਤੀ->n_cells), move_ਮਾਪ);
	    memmove(dest + col1 + STRIKE_ਵਿੱਥ(dest_ਪੰਕਤੀ->n_cells),
		    src + col1 + STRIKE_ਵਿੱਥ(src_ਪੰਕਤੀ->n_cells), move_ਮਾਪ);
	}
	ਨਹੀਂਤਾਂ
	    memmove(dest + col1, src + col1, move_ਮਾਪ);
	draw_ਪੰਕਤੀ_dirty_layer(dest_ਪੰਕਤੀ, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_render_background(Draw_ਪੰਕਤੀ *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    ਜੇ (!dlayer->dirty)
	ਵਾਪਸ;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    ਜੇ (rgba_is_transparent(dlayer->buf, da->n_ਖਾਨੇ_ਗਿਣਤੀ))
	ਵਾਪਸ;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->ਗੰਢ = draw_layer_get_texture(dlayer, drow->ਪੰਕਤੀ, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_ensure_decor(Draw_ਪੰਕਤੀ *drow, ਪੂਰਨ flags)
{
    ਜੇ (flags & DRAW_UNDERL && drow->underl_ਨਕਾਬ == ਨਿਰਾ)
	drow->underl_ਨਕਾਬ = gsk_color_ਗੰਢ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->ਪੰਕਤੀ + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    ਜੇ (flags & DRAW_STRIKE && drow->strike_ਨਕਾਬ == ਨਿਰਾ)
	drow->strike_ਨਕਾਬ = gsk_color_ਗੰਢ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->ਪੰਕਤੀ) + (ਪੂਰਨ)(gui.char_ਉਚਾਈ / 2),
		    FILL_X(drow->n_cells), 1));

    ਜੇ (flags & DRAW_UNDERC && drow->underc_ਨਕਾਬ == ਨਿਰਾ)
    {
	ਸਥਿਰ ਅਚਲ ਪੂਰਨ val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	ਪੂਰਨ y = FILL_Y(drow->ਪੰਕਤੀ + 1) - 1;
	ਪੂਰਨ x_ਸ਼ੁਰੂ = FILL_X(0);
	ਪੂਰਨ x_ਸਿਰਾ = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_ਗੰਢ *ਬੱਚਾ = ਨਿਰਾ;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(4, 14, 0)
	Gsk_ਰਸਤਾ_Builder	*builder;
	Gsk_ਰਸਤਾ		*ਰਸਤਾ;
	GskStroke       *stroke;
	Gsk_Render_ਗੰਢ	*color_ਗੰਢ;
	graphene_rect_t bounds;

	builder = gsk_ਰਸਤਾ_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_ਰਸਤਾ_builder_move_ਮੰਜ਼ਿਲ(builder, -1, y - val[7] + 0.5);
	gsk_ਰਸਤਾ_builder_ਸਤਰ_ਮੰਜ਼ਿਲ(builder, 0, y - val[0] + 0.5);

	ਲਈ (ਪੂਰਨ i = 1; i < 8; i++)
	    gsk_ਰਸਤਾ_builder_ਸਤਰ_ਮੰਜ਼ਿਲ(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_ਰਸਤਾ_builder_ਸਤਰ_ਮੰਜ਼ਿਲ(builder, 8, y - val[0] + 0.5);
	gsk_ਰਸਤਾ_builder_ਸਤਰ_ਮੰਜ਼ਿਲ(builder, 9, y - val[1] + 0.5);

	ਰਸਤਾ = gsk_ਰਸਤਾ_builder_free_ਮੰਜ਼ਿਲ_ਰਸਤਾ(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_ਰਸਤਾ_get_stroke_bounds(ਰਸਤਾ, stroke, &bounds);
	color_ਗੰਢ = gsk_color_ਗੰਢ_new(&white_rgba, &bounds);
	ਬੱਚਾ = gsk_stroke_ਗੰਢ_new(color_ਗੰਢ, ਰਸਤਾ, stroke);

	gsk_stroke_free(stroke);
	gsk_ਰਸਤਾ_unref(ਰਸਤਾ);
	gsk_render_ਗੰਢ_unref(color_ਗੰਢ);
#ਨਹੀਂਤਾਂ
	cairo_t *cr;

	ਬੱਚਾ = gsk_cairo_ਗੰਢ_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_ਗੰਢ_get_draw_context(ਬੱਚਾ);

	cairo_ਸੈੱਟ_ਸਤਰ_ਚੌੜਾਈ(cr, 1.0);
	cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_ਮੰਜ਼ਿਲ(cr, -1, y - val[7] + 0.5);
	cairo_ਸਤਰ_ਮੰਜ਼ਿਲ(cr, 0, y - val[0] + 0.5);

	ਲਈ (ਪੂਰਨ i = 1; i < 8; ++i)
	    cairo_ਸਤਰ_ਮੰਜ਼ਿਲ(cr, i, y - val[i] + 0.5);

	cairo_ਸਤਰ_ਮੰਜ਼ਿਲ(cr, 8, y - val[0] + 0.5);
	cairo_ਸਤਰ_ਮੰਜ਼ਿਲ(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#ਜੇ_ਅੰਤ
	drow->underc_ਨਕਾਬ = gsk_repeat_ਗੰਢ_new(
		&GRAPHENE_RECT_INIT(x_ਸ਼ੁਰੂ, y - 3, x_ਸਿਰਾ - x_ਸ਼ੁਰੂ, 5),
		ਬੱਚਾ, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_ਗੰਢ_unref(ਬੱਚਾ);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_render_ਪਾਠ(Draw_ਪੰਕਤੀ *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_ਪਾਠ;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = ਨਿਰਾ;
    ਪੂਰਨ		ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_cells = 0;
    uint32_t	cur_fg = 0;
    ਪੂਰਨ		c;

    ਜੇ (!dlayer->dirty)
	ਵਾਪਸ;

    glyph_buf = da->glyph_buf;
    nodes = da->ਗੰਢ_buf;

    g_array_ਸੈੱਟ_ਮਾਪ(glyph_buf, 0);
    g_ptr_array_ਸੈੱਟ_ਮਾਪ(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FLUSH_ਗੰਢ() \
    ਕਰੋ { \
	ਜੇ (cur_font != ਨਿਰਾ) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_ਗੰਢ	*ਗੰਢ; \
	    GdkRGBA		fg_color = u32_ਮੰਜ਼ਿਲ_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->ਅੰਕੜੇ; \
	    glyph_str.ਅੰਕ_glyphs = glyph_buf->len; \
	    ਗੰਢ = gsk_ਪਾਠ_ਗੰਢ_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_ਬਿੰਦੂ_INIT(ਪਾਠ_X(ਸ਼ੁਰੂ_ਖਾਨਾ), \
			ਪਾਠ_Y(drow->ਪੰਕਤੀ))); \
	    ਜੇ (ਗੰਢ != ਨਿਰਾ) \
		g_ptr_array_ਜੋੜੋ(nodes, ਗੰਢ); \
	    g_array_ਸੈੱਟ_ਮਾਪ(glyph_buf, 0); \
	    cur_font = ਨਿਰਾ; \
	} \
    } ਜਦੋਂ (FALSE)

    ਲਈ (c = 0; c < da->n_ਖਾਨੇ_ਗਿਣਤੀ; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	ਪੂਰਨ	    prev_len;

	ਜੇ (dglyphs == ਨਿਰਾ)
	{
	    ਖਾਲੀ_ਥਾਵਾਂ_cells++;
	    ਜਾਰੀ;
	}
#ਜੇ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	ਨਹੀਂਤਾਂ ਜੇ (dglyphs->font == ਨਿਰਾ)
	{
	    // Add sign icon
	    Gsk_Render_ਗੰਢ   *snode;
	    ਪੂਰਨ		    w;

	    FLUSH_ਗੰਢ();

	    snode = gsk_texture_scale_ਗੰਢ_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->ਪੰਕਤੀ),
			dglyphs->dsign->ਚੌੜਾਈ, dglyphs->dsign->ਉਚਾਈ),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->ਚੌੜਾਈ + gui.char_ਚੌੜਾਈ - 1) / gui.char_ਚੌੜਾਈ;

	    ਜੇ (snode != ਨਿਰਾ)
		g_ptr_array_ਜੋੜੋ(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    ਜਾਰੀ;
	}
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ ਜੇ (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_ਗੰਢ();
	    cur_font = dglyphs->font;
	    ਸ਼ੁਰੂ_ਖਾਨਾ = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	ਜੇ (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    ਜਾਰੀ;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_cells > 0)
	{
	    ਜੇ (prev_len > 0)
	    {
		// Spaces occurred between valid text: extend
		// previous character's advance
		PangoGlyphInfo *ਆਖਰੀ = &g_array_ਸੂਚਕਾਂਕ(
			glyph_buf, PangoGlyphInfo,
			prev_len - 1);
		ਆਖਰੀ->geometry.ਚੌੜਾਈ += CELLS2PANGO(ਖਾਲੀ_ਥਾਵਾਂ_cells);
	    }
	    ਖਾਲੀ_ਥਾਵਾਂ_cells = 0; // Width consumed
	}
    }

    FLUSH_ਗੰਢ();

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ FLUSH_ਗੰਢ

    ਜੇ (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_ਗੰਢ *ਸਰੋਤ;
	Gsk_Render_ਗੰਢ *ਗੰਢ;

	draw_ਪੰਕਤੀ_ensure_decor(drow, DRAW_UNDERL);
	ਜੇ (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    ਸਰੋਤ = draw_layer_get_texture(dlayer, drow->ਪੰਕਤੀ, da, 0, FALSE);
	    ਗੰਢ = gsk_ਨਕਾਬ_ਗੰਢ_new(ਸਰੋਤ, drow->underl_ਨਕਾਬ,
		    GSK_ਨਕਾਬ_ਬਹੁਲਕ_ALPHA);
	    gsk_render_ਗੰਢ_unref(ਸਰੋਤ);
	    g_ptr_array_ਜੋੜੋ(nodes, ਗੰਢ);
	}

	dlayer->ਗੰਢ = gsk_container_ਗੰਢ_new(
		(Gsk_Render_ਗੰਢ **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_render_special(Draw_ਪੰਕਤੀ *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_ਗੰਢ   *ਸਰੋਤ;
    Gsk_Render_ਗੰਢ   *nodes[2];
    ਪੂਰਨ		    n_nodes = 0;

    ਜੇ (!dlayer->dirty)
	ਵਾਪਸ;

    draw_ਪੰਕਤੀ_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    ਜੇ (!rgba_is_transparent(dlayer->buf + UNDERC_ਵਿੱਥ(drow->n_cells),
		drow->n_cells))
    {
	ਸਰੋਤ = draw_layer_get_texture(dlayer, drow->ਪੰਕਤੀ, da,
		UNDERC_ਵਿੱਥ(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_ਨਕਾਬ_ਗੰਢ_new(ਸਰੋਤ, drow->underc_ਨਕਾਬ,
		GSK_ਨਕਾਬ_ਬਹੁਲਕ_ALPHA);
	gsk_render_ਗੰਢ_unref(ਸਰੋਤ);
    }

    ਜੇ (!rgba_is_transparent(dlayer->buf + STRIKE_ਵਿੱਥ(drow->n_cells),
		drow->n_cells))
    {
	ਸਰੋਤ = draw_layer_get_texture(dlayer, drow->ਪੰਕਤੀ, da,
		STRIKE_ਵਿੱਥ(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_ਨਕਾਬ_ਗੰਢ_new(ਸਰੋਤ, drow->strike_ਨਕਾਬ,
		GSK_ਨਕਾਬ_ਬਹੁਲਕ_ALPHA);
	gsk_render_ਗੰਢ_unref(ਸਰੋਤ);
    }

    ਜੇ (n_nodes == 1)
    {
	dlayer->ਗੰਢ = nodes[0];
    }
    ਨਹੀਂਤਾਂ ਜੇ (n_nodes > 0)
    {
	dlayer->ਗੰਢ = gsk_container_ਗੰਢ_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(nodes); i++)
	    gsk_render_ਗੰਢ_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_render_overlay(Draw_ਪੰਕਤੀ *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    ਜੇ (!dlayer->dirty)
	ਵਾਪਸ;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    ਜੇ (rgba_is_transparent(dlayer->buf, da->n_ਖਾਨੇ_ਗਿਣਤੀ))
	ਵਾਪਸ;

    dlayer->ਗੰਢ = draw_layer_get_texture(dlayer, drow->ਪੰਕਤੀ, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_ਪੰਕਤੀ_ਜੋੜੋ_cluster(
	Draw_ਪੰਕਤੀ		*drow,
	DrawAttr	*dattr,
	ਪੂਰਨ		ਖਾਨਾ,
	ਪੂਰਨ		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	ਪੂਰਨ		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = ਨਿਰਾ;

    ਜੇ (ਖਾਨਾ + n_cells > da->n_ਖਾਨੇ_ਗਿਣਤੀ)
	ਵਾਪਸ;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    ਜੇ (glyphs != ਨਿਰਾ)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_ਪੰਕਤੀ_fill(drow, ਖਾਨਾ, ਖਾਨਾ + n_cells - 1, dglyphs, dattr, &da->cursor);

    ਜੇ (dglyphs != ਨਿਰਾ)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    ਸਥਿਰ ਪੂਰਨ
vim_draw_area_ਜੋੜੋ_string(
	VimDrawArea *self,
	Draw_ਪੰਕਤੀ	    *drow,
	DrawAttr    *dattr,
	ਪੂਰਨ	    ਖਾਨਾ,
	char_u	    *s,
	ਪੂਰਨ	    len,
	ਪੂਰਨ	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = ਨਿਰਾ;
    ਪੂਰਨ			ਸ਼ੁਰੂ_idx = -1;
    GList		*ਵਸਤੂ_list;
    ਪੂਰਨ			ਕਾਲਮ_ਵਿੱਥ = 0;
    ਪੂਰਨ			cluster_ਚੌੜਾਈ;
    ਪੂਰਨ			ਆਖਰੀ_glyph_rbearing;
    ਪੂਰਨ			cells = 0;
    ਪੂਰਨ			i = 0;
    gboolean		has_unicode = FALSE;
    gboolean		needs_shaping = FALSE;

    // Scan the string, and find spans where ligatures should not happen. We
    // must ensure that attribute boundaries do not split a base character from
    // its combining marks, as Pango breaks items at attribute boundaries.
    ਜਦੋਂ (i < len)
    {
	ਪੂਰਨ	clen = 1;
	ਪੂਰਨ	is_composing = 0;
	bool	want_disable;

	ਜੇ (s[i] >= 0x80)
	{
	    clen = utf_ptr2len(s + i);
	    is_composing = utf_iscomposing(utf_ptr2char(s + i));
	    has_unicode = TRUE;
	}

	ਜੇ (is_composing)
	    // Do not change state mid-cluster. Inherit the previous character's
	    // state.
	    want_disable = (ਸ਼ੁਰੂ_idx != -1);
	ਨਹੀਂਤਾਂ
	    // Base character: disable ligatures if it's an ASCII non-ligature
	    // char.
	    want_disable = (s[i] < 0x80 && !gui.ligatures_map[s[i]]);

	// If any base character actually wants ligature/contextual shaping,
	// the string must go through Pango's shaping engine -- the ASCII
	// fast path below draws cached glyphs one-by-one and can never
	// produce a ligature, regardless of 'guiligatures'.
	ਜੇ (!is_composing && !want_disable)
	    needs_shaping = TRUE;

	ਜੇ (want_disable)
	{
	    ਜੇ (ਸ਼ੁਰੂ_idx == -1)
		ਸ਼ੁਰੂ_idx = i;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਸ਼ੁਰੂ_idx != -1)
	{
	    ਜੇ (attr_list == ਨਿਰਾ)
		attr_list = pango_attr_list_new();
	    ਪਾਓ_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, ਸ਼ੁਰੂ_idx, i);
	    ਸ਼ੁਰੂ_idx = -1;
	}
	i += clen;
    }

    // Fast path for pure ASCII: use cached glyph table. Skip this path when
    // there are non-ascii characters in the string, font attributes, or if
    // there's a possible ligature.
    ਜੇ (!(draw_flags & DRAW_ITALIC)
	    && !((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != ਨਿਰਾ
	    && !has_unicode
	    && !needs_shaping)
    {
	ਲਈ (i = 0; i < len; ++i)
	{
	    PangoGlyphInfo  glyph = gui.ascii_glyphs->glyphs[2 * s[i]];
	    gboolean	    has_ink;

	    has_ink = glyph_has_ink(gui.ascii_font, glyph);

	    draw_ਪੰਕਤੀ_ਜੋੜੋ_cluster(drow, dattr, ਖਾਨਾ + i, 1,
		    gui.ascii_font, has_ink ? &glyph : ਨਿਰਾ, 1, self);
	}

	ਕਾਲਮ_ਵਿੱਥ = len;
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }

    glyphs = pango_glyph_string_new();

    ਜੇ (attr_list == ਨਿਰਾ)
	attr_list = pango_attr_list_new();

    ਜੇ (ਸ਼ੁਰੂ_idx != -1)
	ਪਾਓ_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, ਸ਼ੁਰੂ_idx, len);

    cluster_ਚੌੜਾਈ = PANGO_SCALE * gui.char_ਚੌੜਾਈ;
    ਆਖਰੀ_glyph_rbearing = PANGO_SCALE * gui.char_ਚੌੜਾਈ;

    // If 'guifontwide' is set then use that for double-width characters.
    ਜੇ (gui.wide_font != ਨਿਰਾ)
	apply_wide_font_attr(s, len, attr_list);

    ਜੇ ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	ਪਾਓ_PANGO_ATTR(pango_attr_ਭਾਰ_new(PANGO_ਭਾਰ_BOLD),
		attr_list, 0, len);
    ਜੇ (draw_flags & DRAW_ITALIC)
	ਪਾਓ_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    ਵਸਤੂ_list = pango_itemize(gui.ਪਾਠ_context,
	    (ਅਚਲ ਅੱਖਰ *)s, 0, len, attr_list, ਨਿਰਾ);

    ਜਦੋਂ (ਵਸਤੂ_list != ਨਿਰਾ)
    {
	Pango_ਵਸਤੂ   *ਵਸਤੂ = ਵਸਤੂ_list->ਅੰਕੜੇ;
	ਪੂਰਨ	    ਵਸਤੂ_cells = 0;
	ਪੂਰਨ	    cluster_ਸ਼ੁਰੂ_idx = 0;
	ਪੂਰਨ	    current_cluster_ਖਾਨਾ = ਖਾਨਾ + ਕਾਲਮ_ਵਿੱਥ;
	ਪੂਰਨ	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	ਵਸਤੂ_list = g_list_delete_link(ਵਸਤੂ_list, ਵਸਤੂ_list);

	// Force LTR direction; Vim handles bidi on its own.
	ਵਸਤੂ->analysis.level = (ਵਸਤੂ->analysis.level + 1) & (~1U);

	pango_shape_ਭਰਿਆ((ਅਚਲ ਅੱਖਰ *)s + ਵਸਤੂ->ਵਿੱਥ, ਵਸਤੂ->ਲੰਮਾਈ,
		(ਅਚਲ ਅੱਖਰ *)s, len, &ਵਸਤੂ->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	ਲਈ (i = 0; i < glyphs->ਅੰਕ_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    ਜੇ (glyph->attr.is_cluster_ਸ਼ੁਰੂ)
	    {
		ਪੂਰਨ cellcount;

		cellcount = ਗਿਣਤੀ_cluster_cells(
			s, ਵਸਤੂ, glyphs, i, &cluster_ਚੌੜਾਈ,
			(ਵਸਤੂ_list != ਨਿਰਾ) ? &ਆਖਰੀ_glyph_rbearing : ਨਿਰਾ);

		ਜੇ (i > 0 && cellcount > 0)
		{
		    draw_ਪੰਕਤੀ_ਜੋੜੋ_cluster(drow, dattr,
			    current_cluster_ਖਾਨਾ, current_cluster_cells,
			    ਵਸਤੂ->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_ਸ਼ੁਰੂ_idx : ਨਿਰਾ,
			    i - cluster_ਸ਼ੁਰੂ_idx, self);

		    current_cluster_ਖਾਨਾ += current_cluster_cells;
		    cluster_ਸ਼ੁਰੂ_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		ਜੇ (cellcount > 0)
		{
		    ਪੂਰਨ ਚੌੜਾਈ;

		    ਚੌੜਾਈ = cellcount * gui.char_ਚੌੜਾਈ * PANGO_SCALE;
		    glyph->geometry.x_ਵਿੱਥ +=
			MAX(0, ਚੌੜਾਈ - cluster_ਚੌੜਾਈ) / 2;
		    glyph->geometry.ਚੌੜਾਈ = ਚੌੜਾਈ;
		}
		ਨਹੀਂਤਾਂ
		    ਤਿਆਰੀ_zero_ਚੌੜਾਈ_cluster(ਵਸਤੂ, glyph, cells,
			    cluster_ਚੌੜਾਈ,
			    ਆਖਰੀ_glyph_rbearing);

		ਵਸਤੂ_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (i > 0)
	    {
		ਪੂਰਨ ਚੌੜਾਈ;

		ਜੇ (glyph->geometry.x_ਵਿੱਥ >= 0)
		{
		    glyphs->glyphs[i].geometry.ਚੌੜਾਈ =
			glyphs->glyphs[i - 1].geometry.ਚੌੜਾਈ;
		    glyphs->glyphs[i - 1].geometry.ਚੌੜਾਈ = 0;
		}
		ਚੌੜਾਈ = cells * gui.char_ਚੌੜਾਈ * PANGO_SCALE;
		glyph->geometry.x_ਵਿੱਥ +=
		    MAX(0, ਚੌੜਾਈ - cluster_ਚੌੜਾਈ) / 2;
	    }
	    ਨਹੀਂਤਾਂ
		glyph->geometry.ਚੌੜਾਈ = 0;


	    ਜੇ (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			ਵਸਤੂ->analysis.font, *glyph);
	}
	ਜੇ (glyphs->ਅੰਕ_glyphs > 0)
	    draw_ਪੰਕਤੀ_ਜੋੜੋ_cluster(drow, dattr,
		    current_cluster_ਖਾਨਾ, current_cluster_cells,
		    ਵਸਤੂ->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_ਸ਼ੁਰੂ_idx : ਨਿਰਾ,
		    glyphs->ਅੰਕ_glyphs - cluster_ਸ਼ੁਰੂ_idx, self);

	pango_ਵਸਤੂ_free(ਵਸਤੂ);
	ਕਾਲਮ_ਵਿੱਥ += ਵਸਤੂ_cells;
    }

    pango_glyph_string_free(glyphs);

ਬਾਹਰ_ਨਿਕਲੋ:
    ਜੇ (attr_list != ਨਿਰਾ)
	pango_attr_list_unref(attr_list);

    ਵਾਪਸ ਕਾਲਮ_ਵਿੱਥ;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    ਪੂਰਨ
vim_draw_area_draw_string(
	VimDrawArea *self,
	ਪੂਰਨ	    ਪੰਕਤੀ,
	ਪੂਰਨ	    ਖਾਨਾ,
	char_u	    *s,
	ਪੂਰਨ	    len,
	ਪੂਰਨ	    flags)
{
    Draw_ਪੰਕਤੀ	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = ਨਿਰਾ;
    ਪੂਰਨ		convlen;
    ਪੂਰਨ		n_cells ;

    ਜੇ (unlikely(self->ਪੰਕਤੀਆਂ_2 == ਨਿਰਾ
		|| ਪੰਕਤੀ >= self->n_ਪੰਕਤੀਆਂ
		|| ਖਾਨਾ >= self->n_ਖਾਨੇ_ਗਿਣਤੀ))
	ਵਾਪਸ len;

    ਜੇ (output_conv.vc_type != CONV_NONE)
    {
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	ਜੇ (conv_buf != ਨਿਰਾ)
	{
	    s = conv_buf;
	    len = convlen;
	}
    }

    // Safety check: pango crashes with invalid utf-8.
    ਜੇ (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	ਵਾਪਸ len;
    }

    drow = self->ਪੰਕਤੀਆਂ_2 + ਪੰਕਤੀ;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_ਜੋੜੋ_string(self, drow, &dattr, ਖਾਨਾ, s, len, flags);

    vim_free(conv_buf);

    ਵਾਪਸ n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    ਖਾਲੀ
vim_draw_area_clear_ਖੰਡ(
	VimDrawArea	*self,
	ਪੂਰਨ		row1,
	ਪੂਰਨ		col1,
	ਪੂਰਨ		row2,
	ਪੂਰਨ		col2)
{
    ਜੇ (unlikely(self->ਪੰਕਤੀਆਂ_2 == ਨਿਰਾ
		|| row1 >= self->n_ਪੰਕਤੀਆਂ
		|| col1 >= self->n_ਖਾਨੇ_ਗਿਣਤੀ
		|| row2 >= self->n_ਪੰਕਤੀਆਂ
		|| col2 >= self->n_ਖਾਨੇ_ਗਿਣਤੀ))
	ਵਾਪਸ;

    ਲਈ (ਪੂਰਨ r = row1; r <= row2; r++)
	draw_ਪੰਕਤੀ_fill(self->ਪੰਕਤੀਆਂ_2 + r, col1, col2, ਨਿਰਾ, ਨਿਰਾ, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    ਖਾਲੀ
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_ਖੰਡ(self, 0, 0, self->n_ਪੰਕਤੀਆਂ - 1, self->n_ਖਾਨੇ_ਗਿਣਤੀ - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    ਖਾਲੀ
vim_draw_area_move_ਖੰਡ(
	VimDrawArea *self,
	ਪੂਰਨ	    ਮੰਜ਼ਿਲ,
	ਪੂਰਨ	    row1,
	ਪੂਰਨ	    row2,
	ਪੂਰਨ	    col1,
	ਪੂਰਨ	    col2)
{
    ਪੂਰਨ ਵਿੱਥ = row2 - row1;

    ਜੇ (unlikely(self->ਪੰਕਤੀਆਂ_2 == ਨਿਰਾ
		|| row1 >= self->n_ਪੰਕਤੀਆਂ
		|| row2 >= self->n_ਪੰਕਤੀਆਂ
		|| ਮੰਜ਼ਿਲ >= self->n_ਪੰਕਤੀਆਂ
		|| col1 >= self->n_ਖਾਨੇ_ਗਿਣਤੀ
		|| col2 >= self->n_ਖਾਨੇ_ਗਿਣਤੀ))
	ਵਾਪਸ;

    ਪ੍ਰਤਿਜ੍ਞਾ(row2 >= row1);
    ਪ੍ਰਤਿਜ੍ਞਾ(col2 >= col1);
    ਪ੍ਰਤਿਜ੍ਞਾ(row1 != ਮੰਜ਼ਿਲ);

    ਜੇ (row1 > ਮੰਜ਼ਿਲ)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	ਲਈ (ਪੂਰਨ o = 0; o <= ਵਿੱਥ; o++)
	    draw_ਪੰਕਤੀ_move_ਮੰਜ਼ਿਲ(self->ਪੰਕਤੀਆਂ_2 + ਮੰਜ਼ਿਲ + o, self->ਪੰਕਤੀਆਂ_2 + row1 + o,
		    col1, col2, self);
    }
    ਨਹੀਂਤਾਂ
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	ਲਈ (ਪੂਰਨ o = ਵਿੱਥ; o >= 0; o--)
	    ਜੇ (ਮੰਜ਼ਿਲ + o >= self->n_ਪੰਕਤੀਆਂ)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_ਖੰਡ(self, row1 + o, col1, row1 + o, col2);
	    ਨਹੀਂਤਾਂ
		draw_ਪੰਕਤੀ_move_ਮੰਜ਼ਿਲ(self->ਪੰਕਤੀਆਂ_2 + ਮੰਜ਼ਿਲ + o, self->ਪੰਕਤੀਆਂ_2 + row1 + o,
			col1, col2, self);
    }
}

/*
 * Set the cursor to the given width and height, use "gui.bgcolor" and
 * "gui.fgcolor" for the colors as well. If "w" and "h" is zero, then a full
 * block cursor is used, if "w" and "h" is -1 then a hollow cursor is used,
 * otherwise a part cursor with dimensions "w" and "h" is used.
 */
    ਖਾਲੀ
vim_draw_area_ਸੈੱਟ_cursor(VimDrawArea *self, ਪੂਰਨ w, ਪੂਰਨ h)
{
    self->cursor.draw = TRUE;
    self->cursor.ਚੌੜਾਈ = w;
    self->cursor.ਉਚਾਈ = h;
    // Remember this now: the snapshot runs later, when the drawing position
    // has moved on.
    self->cursor.n_cells = 1 + mb_lefthalve(gui.cursor_ਪੰਕਤੀ, gui.cursor_ਖਾਨਾ);
    self->cursor.bg_color = *gui.bgcolor;
    self->cursor.fg_color = *gui.fgcolor;
}

/*
 * Invert the rectangle in the draw area.
 */
    ਖਾਲੀ
vim_draw_area_invert_ਖੰਡ(
	VimDrawArea	*self,
	ਪੂਰਨ		ਪੰਕਤੀ,
	ਪੂਰਨ		ਖਾਨਾ,
	ਪੂਰਨ		nrows,
	ਪੂਰਨ		ncols)
{
    ਜੇ (unlikely(self->ਪੰਕਤੀਆਂ_2 == ਨਿਰਾ
		|| ਪੰਕਤੀ >= self->n_ਪੰਕਤੀਆਂ
		|| ਖਾਨਾ >= self->n_ਖਾਨੇ_ਗਿਣਤੀ
		|| ਪੰਕਤੀ + nrows - 1 >= self->n_ਪੰਕਤੀਆਂ
		|| ਖਾਨਾ + ncols - 1 >= self->n_ਖਾਨੇ_ਗਿਣਤੀ))
	ਵਾਪਸ;

    ਲਈ (ਪੂਰਨ r = ਪੰਕਤੀ; r < ਪੰਕਤੀ + nrows; r++)
    {
	Draw_ਪੰਕਤੀ	    *drow = self->ਪੰਕਤੀਆਂ_2 + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	ਲਈ (ਪੂਰਨ c = 0; c < ncols; c++)
	    dlayer->buf[c + ਖਾਨਾ] = ~dlayer->buf[c + ਖਾਨਾ];
	draw_ਪੰਕਤੀ_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#ਜੇ defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    ਖਾਲੀ
vim_draw_area_ਜੋੜੋ_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	ਪੂਰਨ	    ਪੰਕਤੀ,
	ਪੂਰਨ	    ਖਾਨਾ,
	ਪੂਰਨ	    ਚੌੜਾਈ,
	ਪੂਰਨ	    ਉਚਾਈ)
{
    Draw_ਪੰਕਤੀ	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    ਪੂਰਨ		cells;

    ਜੇ (unlikely(self->ਪੰਕਤੀਆਂ_2 == ਨਿਰਾ
		|| ਪੰਕਤੀ >= self->n_ਪੰਕਤੀਆਂ
		|| ਖਾਨਾ >= self->n_ਖਾਨੇ_ਗਿਣਤੀ))
	ਵਾਪਸ;

    drow = self->ਪੰਕਤੀਆਂ_2 + ਪੰਕਤੀ;
    dsign = draw_sign_new(sign, ਚੌੜਾਈ, ਉਚਾਈ);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (ਚੌੜਾਈ + gui.char_ਚੌੜਾਈ - 1) / gui.char_ਚੌੜਾਈ;

    draw_ਪੰਕਤੀ_fill(drow, ਖਾਨਾ, ਖਾਨਾ + cells - 1, dglyphs, ਨਿਰਾ, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    ਖਾਲੀ
vim_draw_area_ਜੋੜੋ_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	ਪੂਰਨ		ਪੰਕਤੀ,
	ਪੂਰਨ		ਖਾਨਾ,
	ਪੂਰਨ		ਚੌੜਾਈ,
	ਪੂਰਨ		ਉਚਾਈ)
{
    Draw_ਪੰਕਤੀ	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    ਪੂਰਨ		cells;
    GdkTexture	*texture;
    GBytes	*ਬਾਈਟਾਂ;

    ਜੇ (unlikely(self->ਪੰਕਤੀਆਂ_2 == ਨਿਰਾ
		|| ਪੰਕਤੀ >= self->n_ਪੰਕਤੀਆਂ
		|| ਖਾਨਾ >= self->n_ਖਾਨੇ_ਗਿਣਤੀ))
	ਵਾਪਸ;

    cairo_surface_flush(surf);
    ਬਾਈਟਾਂ = g_ਬਾਈਟਾਂ_new_with_free_func(cairo_image_surface_get_ਅੰਕੜੇ(surf),
	    cairo_image_surface_get_ਉਚਾਈ(surf)
	    * cairo_image_surface_get_stride(surf),
	    (GDestroyNotify)cairo_surface_destroy,
	    cairo_surface_reference(surf));

    texture = gdk_ਸਮ੍ਰਿਤੀ_texture_new(cairo_image_surface_get_ਚੌੜਾਈ(surf),
	    cairo_image_surface_get_ਉਚਾਈ(surf),
	    GDK_ਸਮ੍ਰਿਤੀ_A8_R8_G8_B8_PREMULTIPLIED,
	    ਬਾਈਟਾਂ, cairo_image_surface_get_stride(surf));
    g_ਬਾਈਟਾਂ_unref(ਬਾਈਟਾਂ);

    drow = self->ਪੰਕਤੀਆਂ_2 + ਪੰਕਤੀ;
    dsign = draw_sign_new(texture, ਚੌੜਾਈ, ਉਚਾਈ);
    g_object_unref(texture);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (ਚੌੜਾਈ + gui.char_ਚੌੜਾਈ - 1) / gui.char_ਚੌੜਾਈ;

    draw_ਪੰਕਤੀ_fill(drow, ਖਾਨਾ, ਖਾਨਾ + cells - 1, dglyphs, ਨਿਰਾ, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    ਖਾਲੀ
vim_draw_area_ਜੋੜੋ_external(VimDrawArea *self, Gsk_Render_ਗੰਢ *ਗੰਢ)
{
    g_hash_ਸਾਰਣੀ_ਜੋੜੋ(self->external, gsk_render_ਗੰਢ_ref(ਗੰਢ));
}

    ਖਾਲੀ
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_ਗੰਢ *ਗੰਢ)
{
    g_hash_ਸਾਰਣੀ_remove(self->external, ਗੰਢ);
}

#ਜੇ_ਅੰਤ

/*
 * Add the cursor to the snapshot.
 */
    ਸਥਿਰ ਖਾਲੀ
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_ਗੰਢ	*ਪਾਠ_ਗੰਢ,	// May be NULL
	Gsk_Render_ਗੰਢ	*special_ਗੰਢ,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    ਪੂਰਨ		    w = cursor->ਚੌੜਾਈ;
    ਪੂਰਨ		    h = cursor->ਉਚਾਈ;
    graphene_rect_t rect;

    ਜੇ (!cursor->draw)
	ਵਾਪਸ;

    ਜੇ (cursor->ਚੌੜਾਈ <= 0 && cursor->ਉਚਾਈ <= 0)
    {
	w += gui.char_ਚੌੜਾਈ * cursor->n_cells;
	h = gui.char_ਉਚਾਈ;
    }

    graphene_rect_init(&rect,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    CURSOR_BAR_ਸੱਜਾ ? FILL_X(gui.cursor_ਖਾਨਾ + 1) - w :
#ਜੇ_ਅੰਤ
	    FILL_X(gui.cursor_ਖਾਨਾ),
	    FILL_Y(gui.cursor_ਪੰਕਤੀ) + gui.char_ਉਚਾਈ - h, w, h);


    ਜੇ (cursor->ਚੌੜਾਈ == -1 && cursor->ਉਚਾਈ == -1)
    {
	// Create hollow cursor
	GskRoundedRect	    outline;
	ਸਥਿਰ ਅਚਲ ਦਸ਼ਮਲਵ  border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	ਅਚਲ GdkRGBA	    color[4] = {
	    cursor->bg_color, cursor->bg_color,
	    cursor->bg_color, cursor->bg_color
	} ;

	gsk_rounded_rect_init_from_rect(&outline, &rect, 0.0f);
	gtk_snapshot_append_border(snapshot, &outline, border, color);
    }
    ਨਹੀਂਤਾਂ
    {
	// Create block style cursor, by masking the text + under decorations
	// with the fg color.
	gtk_snapshot_append_color(snapshot, &cursor->bg_color, &rect);

	gtk_snapshot_ਧੱਕੋ_ਨਕਾਬ(snapshot, GSK_ਨਕਾਬ_ਬਹੁਲਕ_ALPHA);

	// Used as an "outline" for the fg color.
	ਜੇ (ਪਾਠ_ਗੰਢ != ਨਿਰਾ)
	    gtk_snapshot_append_ਗੰਢ(snapshot, ਪਾਠ_ਗੰਢ);
	ਜੇ (special_ਗੰਢ != ਨਿਰਾ)
	    gtk_snapshot_append_ਗੰਢ(snapshot, special_ਗੰਢ);

	gtk_snapshot_ਕੱਢੋ(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_ਕੱਢੋ(snapshot);
    }
}

    ਸਥਿਰ ਖਾਲੀ
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    ਪੂਰਨ		    ਉਚਾਈ, ਚੌੜਾਈ;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = ਨਿਰਾ;
    Gsk_Render_ਗੰਢ   *body_ਗੰਢ;
    Gsk_Render_ਗੰਢ   *invert_ਗੰਢ = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_GUI
    Gsk_Render_ਗੰਢ   *ext_ਗੰਢ;
    GHash_ਸਾਰਣੀ_Iter  iter;
#ਜੇ_ਅੰਤ

    gui_mch_ਸੈੱਟ_bg_color(gui.ਪਿੱਠ_pixel);
    ਉਚਾਈ = gtk_widget_get_ਉਚਾਈ(widget) + gui.bleed_bot;
    ਚੌੜਾਈ = gtk_widget_get_ਚੌੜਾਈ(widget) + gui.bleed_ਸੱਜਾ;

    ਜੇ (self->ਪੰਕਤੀਆਂ_2 == ਨਿਰਾ)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, ਚੌੜਾਈ, ਉਚਾਈ));
	ਵਾਪਸ;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    ਜੇ (self->bleed_ਸੱਜਾ == -1 || self->bleed_ਸੱਜਾ != gui.bleed_ਸੱਜਾ)
    {
	self->bleed_ਸੱਜਾ = gui.bleed_ਸੱਜਾ;
	ਲਈ (ਪੂਰਨ r = 0; r < self->n_ਪੰਕਤੀਆਂ; r++)
	{
	    draw_ਪੰਕਤੀ_dirty_layer(self->ਪੰਕਤੀਆਂ_2 + r, DRAW_LAYER_BACKGROUND);
	    draw_ਪੰਕਤੀ_dirty_layer(self->ਪੰਕਤੀਆਂ_2 + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(4,24,0)
    gtk_snapshot_ਸੈੱਟ_snap(body_snapshot, GSK_RECT_SNAP_ਗੇੜਾ);
#ਜੇ_ਅੰਤ

    ਲਈ (ਪੂਰਨ r = 0; r < self->n_ਪੰਕਤੀਆਂ; r++)
    {
	Draw_ਪੰਕਤੀ *drow = self->ਪੰਕਤੀਆਂ_2 + r;

	draw_ਪੰਕਤੀ_render_background(drow, self);
	draw_ਪੰਕਤੀ_render_ਪਾਠ(drow, self);
	draw_ਪੰਕਤੀ_render_special(drow, self);
	draw_ਪੰਕਤੀ_render_overlay(drow, self);

	ਲਈ (ਪੂਰਨ l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    ਜੇ (dlayer->ਗੰਢ != ਨਿਰਾ)
	    {
		ਜੇ (l == DRAW_LAYER_OVERLAY)
		{
		    ਜੇ (invert_snapshot == ਨਿਰਾ)
		    {
			invert_snapshot = gtk_snapshot_new();
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(4,24,0)
			gtk_snapshot_ਸੈੱਟ_snap(invert_snapshot,
				GSK_RECT_SNAP_ਗੇੜਾ);
#ਜੇ_ਅੰਤ
		    }
		    gtk_snapshot_append_ਗੰਢ(invert_snapshot, dlayer->ਗੰਢ);
		}
		ਨਹੀਂਤਾਂ
		    gtk_snapshot_append_ਗੰਢ(body_snapshot, dlayer->ਗੰਢ);
	    }
	}

	ਜੇ (r == gui.cursor_ਪੰਕਤੀ)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_ਪਾਠ].ਗੰਢ,
		    drow->dlayers[DRAW_LAYER_SPECIAL].ਗੰਢ,
		    body_snapshot);
    }

    ਜੇ (invert_snapshot != ਨਿਰਾ)
	gtk_snapshot_ਧੱਕੋ_blend(snapshot, GSK_BLEND_ਬਹੁਲਕ_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, ਚੌੜਾਈ, ਉਚਾਈ));
    body_ਗੰਢ = gtk_snapshot_free_ਮੰਜ਼ਿਲ_ਗੰਢ(body_snapshot);
    ਜੇ (body_ਗੰਢ != ਨਿਰਾ)
    {
	gtk_snapshot_append_ਗੰਢ(snapshot, body_ਗੰਢ);
	gsk_render_ਗੰਢ_unref(body_ਗੰਢ);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_ਸਾਰਣੀ_iter_init(&iter, self->external);

    ਜਦੋਂ (g_hash_ਸਾਰਣੀ_iter_ਅਗਲਾ(&iter, (ਖਾਲੀ **)&ext_ਗੰਢ, ਨਿਰਾ))
	gtk_snapshot_append_ਗੰਢ(snapshot, ext_ਗੰਢ);
#ਜੇ_ਅੰਤ

    ਜੇ (invert_snapshot != ਨਿਰਾ)
    {
	gtk_snapshot_ਕੱਢੋ(snapshot);
	invert_ਗੰਢ = gtk_snapshot_free_ਮੰਜ਼ਿਲ_ਗੰਢ(invert_snapshot);
	// Not sure if it can be NULL but still check
	ਜੇ (invert_ਗੰਢ != ਨਿਰਾ)
	{
	    gtk_snapshot_append_ਗੰਢ(snapshot, invert_ਗੰਢ);
	    gsk_render_ਗੰਢ_unref(invert_ਗੰਢ);
	}
	gtk_snapshot_ਕੱਢੋ(snapshot);
    }
}
