/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <gtk/gtk.h>
#ಅನ್ತರ್ಭಾವ "gui_gtk4_da.h"


#ಆದರೆ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Used for sign icons and netbeans multisign indicator. It is in a DrawGlyphs
 * so that it can move/be manipulated with the text.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    GdkTexture	*texture;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ;
} DrawSign;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Temporary struct when drawing string representing visual attributes of a cell
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    uint32_t	bg_color;
    uint32_t	fg_color;   // Only used for underlines
    uint32_t	sp_color;
    ಪೂರ್ಣಾಂಕ		draw_flags; // DRAW_* flags, only DRAW_UNDERL, DRAW_UNDERC, and
			    // DRAW_STRIKE are used.
} DrawAttr;

/*
 * Represents the glyphs of a single cell.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ refcount;

    uint32_t	fg_color;
    PangoFont	*font;
    // If "font" is NULL, "dsign" is used, otherwise "glyphs" is used.
    ಒಕ್ಕೂಟ
    {
	PangoGlyphInfo  *glyphs; // May be NULL
#ಆದರೆ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	DrawSign	*dsign;
#ಆದರೆ_ಅಂತ್ಯ
    };
    ಪೂರ್ಣಾಂಕ n_glyphs;
} DrawGlyphs;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    DrawGlyphs *dglyphs;
} DrawCell;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    DRAW_LAYER_BACKGROUND,  // Background color, buf stores bg color
    DRAW_LAYER_ಪಠ್ಯ,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Gsk_Render_ಗಂಟು   *ಗಂಟು;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#ವ್ಯಾಖ್ಯಾನಿಸು UNDERC_ಸ್ಥಾನಾಂತರ(n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) (0)
#ವ್ಯಾಖ್ಯಾನಿಸು STRIKE_ಸ್ಥಾನಾಂತರ(n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) (n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    ಪೂರ್ಣಾಂಕ		n_cells; // For convenience
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_ಗಂಟು *underl_ಮುಖವಾಡ;
    Gsk_Render_ಗಂಟು *underc_ಮುಖವಾಡ;
    Gsk_Render_ಗಂಟು *strike_ಮುಖವಾಡ;
} Draw_ಅಡ್ಡಸಾಲು;

#ವ್ಯಾಖ್ಯಾನಿಸು ಸೇರಿಸು_PANGO_ATTR(Attribute, AttrList, ಆರಂಭ_2, ಕೊನೆ_2) \
    ಮಾಡು { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->ಆರಂಭ_ಸೂಚ್ಯಂಕ = (ಆರಂಭ_2); \
	tmp_attr_->ಕೊನೆ_ಸೂಚ್ಯಂಕ = (ಕೊನೆ_2); \
	pango_attr_list_ಸೇರಿಸು((AttrList), tmp_attr_); \
    } ಆಗ (FALSE)
#ವ್ಯಾಖ್ಯಾನಿಸು CELLS2PANGO(c) ((c) * gui.char_ಅಗಲ * PANGO_SCALE)

/*
 * Attributes of cursor
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    gboolean draw; // If cursor should be drawn
    ಪೂರ್ಣಾಂಕ ಅಗಲ;
    ಪೂರ್ಣಾಂಕ ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

ರಚನೆ _VimDrawArea
{
    GtkWidget ಪೋಷಕ;

    Draw_ಅಡ್ಡಸಾಲು *ಅಡ್ಡಸಾಲುಗಳು_2;

    ಪೂರ್ಣಾಂಕ n_ಅಡ್ಡಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ	n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

    ಪೂರ್ಣಾಂಕ	bleed_ಬಲ;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*ಗಂಟು_buf;

    DrawCursor cursor;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHash_ಕೋಷ್ಟಕ *external;
#ಆದರೆ_ಅಂತ್ಯ
};

ಸ್ಥಿರ ಶೂನ್ಯ draw_ಅಡ್ಡಸಾಲು_init(Draw_ಅಡ್ಡಸಾಲು *drow, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
ಸ್ಥಿರ ಶೂನ್ಯ draw_ಅಡ್ಡಸಾಲು_clear(Draw_ಅಡ್ಡಸಾಲು *drow);
ಸ್ಥಿರ ಶೂನ್ಯ draw_ಅಡ್ಡಸಾಲು_dirty_layer(Draw_ಅಡ್ಡಸಾಲು *drow, DrawLayerType dlayer_t);
ಸ್ಥಿರ ಶೂನ್ಯ vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

ಸ್ಥಿರ GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = 0; r < self->n_ಅಡ್ಡಸಾಲುಗಳು; r++)
	draw_ಅಡ್ಡಸಾಲು_clear(self->ಅಡ್ಡಸಾಲುಗಳು_2 + r);
    g_free(self->ಅಡ್ಡಸಾಲುಗಳು_2);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->ಗಂಟು_buf, TRUE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_GUI
    g_hash_ಕೋಷ್ಟಕ_unref(self->external);
#ಆದರೆ_ಅಂತ್ಯ

    G_OBJECT_CLASS(vim_draw_area_ಪೋಷಕ_class)->finalize(obj);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_ಗಣ_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_ಬಲ = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_GUI
    self->external = g_hash_ಕೋಷ್ಟಕ_new_ತುಂಬಿದ(g_ನೇರ_hash, g_ನೇರ_equal,
	    ಖಾಲಿ, (GDestroyNotify)gsk_render_ಗಂಟು_unref);
#ಆದರೆ_ಅಂತ್ಯ
    self->glyph_buf = g_array_new(FALSE, FALSE, ಗಾತ್ರ(PangoGlyphInfo));
    self->ಗಂಟು_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_ಗಂಟು_unref);

}

    GtkWidget *
vim_draw_area_new(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ g_object_new(VIM_TYPE_DRAW_AREA, ಖಾಲಿ);
}

/*
 * Set the size of the draw area to "rows" and "cols".
 */
    ಶೂನ್ಯ
vim_draw_area_ಗಣ_ಗಾತ್ರ(VimDrawArea *self, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
    ಆದರೆ (self->ಅಡ್ಡಸಾಲುಗಳು_2 != ಖಾಲಿ && self->n_ಅಡ್ಡಸಾಲುಗಳು == ಅಡ್ಡಸಾಲುಗಳು_2 && self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು_2 == 0 || ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == 0)
	ಹಿಂತಿರುಗಿ;

    vim_draw_area_clear(self);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = 0; r < self->n_ಅಡ್ಡಸಾಲುಗಳು; r++)
	draw_ಅಡ್ಡಸಾಲು_clear(self->ಅಡ್ಡಸಾಲುಗಳು_2 + r);
    self->ಅಡ್ಡಸಾಲುಗಳು_2 = g_realloc_n(self->ಅಡ್ಡಸಾಲುಗಳು_2, ಅಡ್ಡಸಾಲುಗಳು_2, ಗಾತ್ರ(Draw_ಅಡ್ಡಸಾಲು));
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = 0; r < ಅಡ್ಡಸಾಲುಗಳು_2; r++)
	draw_ಅಡ್ಡಸಾಲು_init(self->ಅಡ್ಡಸಾಲುಗಳು_2 + r, r, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

    self->n_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು_2;
    self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    ಸ್ಥಿರ uint32_t
rgba_ಗಮ್ಯ_u32(ಸ್ಥಿರಾಂಕ GdkRGBA *rgba)
{
    ಒಕ್ಕೂಟ { uint32_t val; uint8_t ಬೈಟುಗಳು[4]; } color;

    color.ಬೈಟುಗಳು[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.ಬೈಟುಗಳು[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.ಬೈಟುಗಳು[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.ಬೈಟುಗಳು[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    ಹಿಂತಿರುಗಿ color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    ಸ್ಥಿರ GdkRGBA
u32_ಗಮ್ಯ_rgba(uint32_t val)
{
    ಒಕ್ಕೂಟ { uint32_t val; uint8_t ಬೈಟುಗಳು[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.ಬೈಟುಗಳು[0] / 255.0;
    color.green = packed.ಬೈಟುಗಳು[1] / 255.0;
    color.blue  = packed.ಬೈಟುಗಳು[2] / 255.0;
    color.alpha = packed.ಬೈಟುಗಳು[3] / 255.0;
    ಹಿಂತಿರುಗಿ color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    ಸ್ಥಿರ gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    ಒಕ್ಕೂಟ { uint32_t val; uint8_t ಬೈಟುಗಳು[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.ಬೈಟುಗಳು[0] << 16) |
	((guicolor_T)packed.ಬೈಟುಗಳು[1] << 8)  |
	packed.ಬೈಟುಗಳು[2];
    ಹಿಂತಿರುಗಿ res == rgb;
}

/*
 * Checks if a RGBA pixel buffer is fully transparent.
 */
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ gboolean
rgba_is_transparent(ಸ್ಥಿರಾಂಕ uint32_t *buf, ಪೂರ್ಣಾಂಕ n_pixels)
{
    // Could also compare 2 pixels at once using a 64 bit integer, but not sure
    // if that might cause alignment issues...
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_pixels; i++)
	ಆದರೆ (buf[i] != 0)
	    ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Return TRUE if "glyph" has ink (not whitespace).
 */
    ಸ್ಥಿರ gboolean
glyph_has_ink(PangoFont *font, PangoGlyphInfo glyph)
{
    PangoRectangle ink;

    pango_font_get_glyph_extents (font, glyph.glyph, &ink, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ ink.ಅಗಲ > 0 && ink.ಎತ್ತರ > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
apply_wide_font_attr(char_u *s, ಪೂರ್ಣಾಂಕ len, PangoAttrList *attr_list)
{
    char_u  *ಆರಂಭ = ಖಾಲಿ;
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    uc;

    ಗಾಗಿ (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	ಆದರೆ (ಆರಂಭ == ಖಾಲಿ)
	{
	    ಆದರೆ (uc >= 0x80 && utf_char2cells(uc) == 2)
		ಆರಂಭ = p;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    ಸೇರಿಸು_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, ಆರಂಭ - s, p - s);
	    ಆರಂಭ = ಖಾಲಿ;
	}
    }

    ಆದರೆ (ಆರಂಭ != ಖಾಲಿ)
	ಸೇರಿಸು_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, ಆರಂಭ - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಎಣಿಕೆ_cluster_cells(
	char_u		    *s,
	Pango_ಅಂಶ	    *ಅಂಶ,
	PangoGlyphString    *glyphs,
	ಪೂರ್ಣಾಂಕ		    i,
	ಪೂರ್ಣಾಂಕ		    *cluster_ಅಗಲ,
	ಪೂರ್ಣಾಂಕ		    *ಕೊನೆಯ_glyph_rbearing)
{
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ	    ಆರಂಭ, ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ	    ಅಗಲ;
    ಪೂರ್ಣಾಂಕ	    uc;
    ಪೂರ್ಣಾಂಕ	    cellcount = 0;

    ಅಗಲ = glyphs->glyphs[i].geometry.ಅಗಲ;

    ಗಾಗಿ (ಮುಂದಿನ = i + 1; ಮುಂದಿನ < glyphs->ಅಂಕೆ_glyphs; ++ಮುಂದಿನ)
    {
	ಆದರೆ (glyphs->glyphs[ಮುಂದಿನ].attr.is_cluster_ಆರಂಭ)
	    ನಿಲ್ಲಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (glyphs->glyphs[ಮುಂದಿನ].geometry.ಅಗಲ > ಅಗಲ)
	    ಅಗಲ = glyphs->glyphs[ಮುಂದಿನ].geometry.ಅಗಲ;
    }

    ಆರಂಭ = ಅಂಶ->ಸ್ಥಾನಾಂತರ + glyphs->log_clusters[i];
    ಕೊನೆ   = ಅಂಶ->ಸ್ಥಾನಾಂತರ + ((ಮುಂದಿನ < glyphs->ಅಂಕೆ_glyphs) ?
	    glyphs->log_clusters[ಮುಂದಿನ] : ಅಂಶ->ಉದ್ದಳತೆ);

    ಗಾಗಿ (p = s + ಆರಂಭ; p < s + ಕೊನೆ; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	ಆದರೆ (uc < 0x80)
	    ++cellcount;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    ಆದರೆ (ಕೊನೆಯ_glyph_rbearing != ಖಾಲಿ
	    && cellcount > 0 && ಮುಂದಿನ == glyphs->ಅಂಕೆ_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(ಅಂಶ->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, ಖಾಲಿ);

	ಆದರೆ (PANGO_RBEARING(ink_rect) > 0)
	    *ಕೊನೆಯ_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    ಆದರೆ (cellcount > 0)
	*cluster_ಅಗಲ = ಅಗಲ;

    ಹಿಂತಿರುಗಿ cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಿದ್ಧತೆ_zero_ಅಗಲ_cluster(
	Pango_ಅಂಶ	*ಅಂಶ,
	PangoGlyphInfo	*glyph,
	ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ_cellcount,
	ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ_cluster_ಅಗಲ,
	ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    ಪೂರ್ಣಾಂಕ		    ಅಗಲ;

    ಅಗಲ = ಕೊನೆಯ_cellcount * gui.char_ಅಗಲ * PANGO_SCALE;
    glyph->geometry.x_ಸ್ಥಾನಾಂತರ = -ಅಗಲ + MAX(0, ಅಗಲ - ಕೊನೆಯ_cluster_ಅಗಲ) / 2;
    glyph->geometry.ಅಗಲ = 0;

    pango_font_get_glyph_extents(ಅಂಶ->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    ಆದರೆ (ink_rect.x < 0)
    {
	glyph->geometry.x_ಸ್ಥಾನಾಂತರ += ಕೊನೆಯ_glyph_rbearing;
	glyph->geometry.y_ಸ್ಥಾನಾಂತರ  = logical_rect.ಎತ್ತರ
	    - (gui.char_ಎತ್ತರ - p_linespace) * PANGO_SCALE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	glyph->geometry.x_ಸ್ಥಾನಾಂತರ = -ಅಗಲ + MAX(0, ಅಗಲ - ink_rect.ಅಗಲ) / 2;
}

#ಆದರೆ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    ಸ್ಥಿರ DrawSign *
draw_sign_new(GdkTexture *texture, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->ಅಗಲ = ಅಗಲ;
    dsign->ಎತ್ತರ = ಎತ್ತರ;

    ಹಿಂತಿರುಗಿ dsign;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_attr_init(
	DrawAttr	*dattr,
	ಸ್ಥಿರಾಂಕ GdkRGBA	*bg_color,
	ಸ್ಥಿರಾಂಕ GdkRGBA	*fg_color,
	ಸ್ಥಿರಾಂಕ GdkRGBA	*sp_color,
	ಪೂರ್ಣಾಂಕ		draw_flags)
{
    dattr->bg_color = rgba_ಗಮ್ಯ_u32(bg_color);
    dattr->fg_color = rgba_ಗಮ್ಯ_u32(fg_color);
    dattr->sp_color = rgba_ಗಮ್ಯ_u32(sp_color);
    dattr->draw_flags = draw_flags & (DRAW_UNDERL | DRAW_UNDERC | DRAW_STRIKE);
}

/*
 * Apply the draw attr to the row for the given col/cell. Only dirty layers that
 * have actually changed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_attr_apply(DrawAttr *dattr, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, Draw_ಅಡ್ಡಸಾಲು *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + ಲಂಬಸಾಲು;
    uint32_t	*ಪಠ್ಯ_cell = drow->dlayers[DRAW_LAYER_ಪಠ್ಯ].buf + ಲಂಬಸಾಲು;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    ಪೂರ್ಣಾಂಕ		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	ಲಂಬಸಾಲು + UNDERC_ಸ್ಥಾನಾಂತರ(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	ಲಂಬಸಾಲು + STRIKE_ಸ್ಥಾನಾಂತರ(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    ಆದರೆ (dattr != ಖಾಲಿ)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.ಹಿಂಭಾಗ_pixel, bg_color);
    }

    ಆದರೆ (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_ಅಡ್ಡಸಾಲು_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	ಆದರೆ (bg_is_default)
	    *bg_cell = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    *bg_cell = bg_color;
    }
    ಆದರೆ ((draw_flags & DRAW_UNDERL && *ಪಠ್ಯ_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *ಪಠ್ಯ_cell != 0))
    {
	draw_ಅಡ್ಡಸಾಲು_dirty_layer(drow, DRAW_LAYER_ಪಠ್ಯ);
	ಆದರೆ (draw_flags & DRAW_UNDERL)
	    *ಪಠ್ಯ_cell = fg_color;
	ಇಲ್ಲದಿದ್ದರೆ
	    *ಪಠ್ಯ_cell = 0;
    }
    ಆದರೆ ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_ಅಡ್ಡಸಾಲು_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	ಆದರೆ (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	ಇಲ್ಲದಿದ್ದರೆ
	    *underc_cell = 0;
    }
    ಆದರೆ ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_ಅಡ್ಡಸಾಲು_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	ಆದರೆ (draw_flags & DRAW_STRIKE)
	    *strike_cell = sp_color;
	ಇಲ್ಲದಿದ್ದರೆ
	    *strike_cell = 0;
    }
}

    ಸ್ಥಿರ DrawGlyphs *
draw_glyphs_new(
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	ಪೂರ್ಣಾಂಕ		n_glyphs,
	uint32_t	fg_color)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->font = g_object_ref(font);
    dglyphs->glyphs = g_memdup2(glyphs, ಗಾತ್ರ(*glyphs) * n_glyphs);
    dglyphs->n_glyphs = n_glyphs;
    dglyphs->fg_color = fg_color;

    ಹಿಂತಿರುಗಿ dglyphs;
}

#ಆದರೆ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw glyphs that shows the given sign. Note that ownership of
 * "dsign" is taken.
 */
    ಸ್ಥಿರ DrawGlyphs *
draw_glyphs_new_sign(DrawSign *dsign)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->dsign = dsign;

    ಹಿಂತಿರುಗಿ dglyphs;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ DrawGlyphs *
draw_glyphs_ref(DrawGlyphs *dglyphs)
{
    dglyphs->refcount++;
    ಹಿಂತಿರುಗಿ dglyphs;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_glyphs_unref(DrawGlyphs *dglyphs)
{
    ಆದರೆ (--dglyphs->refcount <= 0)
    {
#ಆದರೆ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	ಆದರೆ (dglyphs->font == ಖಾಲಿ)
	    draw_sign_free(dglyphs->dsign);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
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
    ಸ್ಥಿರ gboolean
draw_glyphs_same(DrawGlyphs *a, DrawGlyphs *b)
{
    ಆದರೆ (a == ಖಾಲಿ && b == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (a == ಖಾಲಿ || b == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (a->fg_color != b->fg_color)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (a->n_glyphs != b->n_glyphs || a->font != b->font)
	ಹಿಂತಿರುಗಿ FALSE;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < a->n_glyphs; i++)
	ಆದರೆ (a->glyphs[i].glyph != b->glyphs[i].glyph
		|| a->glyphs[i].geometry.ಅಗಲ != b->glyphs[i].geometry.ಅಗಲ
		|| a->glyphs[i].geometry.x_ಸ್ಥಾನಾಂತರ != b->glyphs[i].geometry.x_ಸ್ಥಾನಾಂತರ
		|| a->glyphs[i].geometry.y_ಸ್ಥಾನಾಂತರ != b->glyphs[i].geometry.y_ಸ್ಥಾನಾಂತರ
		|| a->glyphs[i].attr.is_cluster_ಆರಂಭ != b->glyphs[i].attr.is_cluster_ಆರಂಭ)
	    ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    ಸ್ಥಿರ Gsk_Render_ಗಂಟು *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು,
	VimDrawArea *da,
	ಪೂರ್ಣಾಂಕ	    off,
	gboolean    bleed)
{
    GBytes	    *ಬೈಟುಗಳು;
    GdkTexture	    *texture;
    Gsk_Render_ಗಂಟು   *ಗಂಟು;
    ಪೂರ್ಣಾಂಕ		    ಎತ್ತರ;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(4,24,0)
    ಎತ್ತರ = gui.char_ಎತ್ತರ;
#ಇಲ್ಲದಿದ್ದರೆ
    ಎತ್ತರ = gui.char_ಎತ್ತರ + 1;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		ಗಾತ್ರ(uint32_t) * da->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1)),
		ಗಾತ್ರ(uint32_t)); // Add bleed using last pixel of buffer
	ಬೈಟುಗಳು = g_byte_array_free_ಗಮ್ಯ_ಬೈಟುಗಳು(arr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಬೈಟುಗಳು = g_ಬೈಟುಗಳು_new(dlayer->buf + off, ಗಾತ್ರ(uint32_t) * da->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

    texture = gdk_ಸ್ಮರಣೆ_texture_new(da->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + bleed, 1,
	    GDK_ಸ್ಮರಣೆ_R8_G8_B8_A8, ಬೈಟುಗಳು, (da->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + bleed) * 4);
    g_ಬೈಟುಗಳು_unref(ಬೈಟುಗಳು);

    // Scale texture to actual size
    ಗಂಟು = gsk_texture_scale_ಗಂಟು_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(ಅಡ್ಡಸಾಲು),
		(da->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + bleed) * gui.char_ಅಗಲ, gui.char_ಎತ್ತರ + bleed),
	    GSK_SCALING_FILTER_ಹತ್ತಿರದ);
    ಆದರೆ (bleed)
    {
	Gsk_Render_ಗಂಟು *new;

	new = gsk_clip_ಗಂಟು_new(ಗಂಟು,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(ಅಡ್ಡಸಾಲು),
		    da->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * gui.char_ಅಗಲ + da->bleed_ಬಲ, ಎತ್ತರ));
	gsk_render_ಗಂಟು_unref(ಗಂಟು);
	ಗಂಟು = new;
    }
    g_object_unref(texture);
    ಹಿಂತಿರುಗಿ ಗಂಟು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_init(Draw_ಅಡ್ಡಸಾಲು *drow, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
    drow->cells = g_malloc0_n(ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಗಾತ್ರ(DrawCell));
    drow->n_cells = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    drow->ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
    drow->underl_ಮುಖವಾಡ = drow->underc_ಮುಖವಾಡ = drow->strike_ಮುಖವಾಡ = ಖಾಲಿ;

    memset(drow->dlayers, 0, ಗಾತ್ರ(DrawLayer) * N_DRAW_LAYERS);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	ಆದರೆ (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * 2, 4);
	ಇಲ್ಲದಿದ್ದರೆ
	    dlayer->buf = g_malloc0_n(ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, 4);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_clear(Draw_ಅಡ್ಡಸಾಲು *drow)
{
    g_free(drow->cells);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	ಆದರೆ (dlayer->ಗಂಟು != ಖಾಲಿ)
	    gsk_render_ಗಂಟು_unref(dlayer->ಗಂಟು);
	g_free(dlayer->buf);
    }
    g_clear_ಸೂಚಕ(&drow->underl_ಮುಖವಾಡ, gsk_render_ಗಂಟು_unref);
    g_clear_ಸೂಚಕ(&drow->underc_ಮುಖವಾಡ, gsk_render_ಗಂಟು_unref);
    g_clear_ಸೂಚಕ(&drow->strike_ಮುಖವಾಡ, gsk_render_ಗಂಟು_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_dirty_layer(Draw_ಅಡ್ಡಸಾಲು *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_ಸೂಚಕ(&dlayer->ಗಂಟು, gsk_render_ಗಂಟು_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_uninvert_cell(Draw_ಅಡ್ಡಸಾಲು *drow, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    ಆದರೆ (buf[ಲಂಬಸಾಲು] == 0xffffffff)
    {
	draw_ಅಡ್ಡಸಾಲು_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[ಲಂಬಸಾಲು] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_ಗಣ_cell(Draw_ಅಡ್ಡಸಾಲು *drow, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + ಲಂಬಸಾಲು;

    ಆದರೆ (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_ಅಡ್ಡಸಾಲು_dirty_layer(drow, DRAW_LAYER_ಪಠ್ಯ);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    ಆದರೆ (dcell->dglyphs != dglyphs)
    {
	ಆದರೆ (dcell->dglyphs != ಖಾಲಿ)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == ಖಾಲಿ ? ಖಾಲಿ : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, ಲಂಬಸಾಲು, drow);
    draw_ಅಡ್ಡಸಾಲು_uninvert_cell(drow, ಲಂಬಸಾಲು);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_fill(
	Draw_ಅಡ್ಡಸಾಲು	    *drow,
	ಪೂರ್ಣಾಂಕ	    col1,
	ಪೂರ್ಣಾಂಕ	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ c = col1; c <= col2; c++)
	draw_ಅಡ್ಡಸಾಲು_ಗಣ_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    ಆದರೆ (gui.cursor_ಅಡ್ಡಸಾಲು == drow->ಅಡ್ಡಸಾಲು && gui.cursor_ಲಂಬಸಾಲು >= col1
	    && gui.cursor_ಲಂಬಸಾಲು <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_move_ಗಮ್ಯ(
	Draw_ಅಡ್ಡಸಾಲು	    *dest_ಅಡ್ಡಸಾಲು,
	Draw_ಅಡ್ಡಸಾಲು	    *src_ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	    col1,
	ಪೂರ್ಣಾಂಕ	    col2,
	VimDrawArea *da)
{
    ಪೂರ್ಣಾಂಕ move_ಗಾತ್ರ = (col2 - col1 + 1) * ಗಾತ್ರ(DrawCell);

    draw_ಅಡ್ಡಸಾಲು_fill(dest_ಅಡ್ಡಸಾಲು, col1, col2, ಖಾಲಿ, ಖಾಲಿ, &da->cursor);
    memmove(dest_ಅಡ್ಡಸಾಲು->cells + col1, src_ಅಡ್ಡಸಾಲು->cells + col1, move_ಗಾತ್ರ);
    // NULL the draw cells so we don't double unreference.
    memset(src_ಅಡ್ಡಸಾಲು->cells + col1, 0, move_ಗಾತ್ರ);
    draw_ಅಡ್ಡಸಾಲು_dirty_layer(src_ಅಡ್ಡಸಾಲು, DRAW_LAYER_ಪಠ್ಯ);

    // Move the layer buffers
    move_ಗಾತ್ರ = (col2 - col1 + 1) * 4;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_ಅಡ್ಡಸಾಲು->dlayers[l].buf;
	uint32_t *dest = dest_ಅಡ್ಡಸಾಲು->dlayers[l].buf;

	ಆದರೆ (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_ಸ್ಥಾನಾಂತರ(dest_ಅಡ್ಡಸಾಲು->n_cells),
		    src + col1 + UNDERC_ಸ್ಥಾನಾಂತರ(src_ಅಡ್ಡಸಾಲು->n_cells), move_ಗಾತ್ರ);
	    memmove(dest + col1 + STRIKE_ಸ್ಥಾನಾಂತರ(dest_ಅಡ್ಡಸಾಲು->n_cells),
		    src + col1 + STRIKE_ಸ್ಥಾನಾಂತರ(src_ಅಡ್ಡಸಾಲು->n_cells), move_ಗಾತ್ರ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    memmove(dest + col1, src + col1, move_ಗಾತ್ರ);
	draw_ಅಡ್ಡಸಾಲು_dirty_layer(dest_ಅಡ್ಡಸಾಲು, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_render_background(Draw_ಅಡ್ಡಸಾಲು *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    ಆದರೆ (!dlayer->dirty)
	ಹಿಂತಿರುಗಿ;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    ಆದರೆ (rgba_is_transparent(dlayer->buf, da->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
	ಹಿಂತಿರುಗಿ;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->ಗಂಟು = draw_layer_get_texture(dlayer, drow->ಅಡ್ಡಸಾಲು, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_ensure_decor(Draw_ಅಡ್ಡಸಾಲು *drow, ಪೂರ್ಣಾಂಕ flags)
{
    ಆದರೆ (flags & DRAW_UNDERL && drow->underl_ಮುಖವಾಡ == ಖಾಲಿ)
	drow->underl_ಮುಖವಾಡ = gsk_color_ಗಂಟು_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->ಅಡ್ಡಸಾಲು + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    ಆದರೆ (flags & DRAW_STRIKE && drow->strike_ಮುಖವಾಡ == ಖಾಲಿ)
	drow->strike_ಮುಖವಾಡ = gsk_color_ಗಂಟು_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->ಅಡ್ಡಸಾಲು) + (ಪೂರ್ಣಾಂಕ)(gui.char_ಎತ್ತರ / 2),
		    FILL_X(drow->n_cells), 1));

    ಆದರೆ (flags & DRAW_UNDERC && drow->underc_ಮುಖವಾಡ == ಖಾಲಿ)
    {
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	ಪೂರ್ಣಾಂಕ y = FILL_Y(drow->ಅಡ್ಡಸಾಲು + 1) - 1;
	ಪೂರ್ಣಾಂಕ x_ಆರಂಭ = FILL_X(0);
	ಪೂರ್ಣಾಂಕ x_ಕೊನೆ = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_ಗಂಟು *ಮಗು = ಖಾಲಿ;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(4, 14, 0)
	Gsk_ಮಾರ್ಗ_Builder	*builder;
	Gsk_ಮಾರ್ಗ		*ಮಾರ್ಗ;
	GskStroke       *stroke;
	Gsk_Render_ಗಂಟು	*color_ಗಂಟು;
	graphene_rect_t bounds;

	builder = gsk_ಮಾರ್ಗ_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_ಮಾರ್ಗ_builder_move_ಗಮ್ಯ(builder, -1, y - val[7] + 0.5);
	gsk_ಮಾರ್ಗ_builder_ಸಾಲು_ಗಮ್ಯ(builder, 0, y - val[0] + 0.5);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < 8; i++)
	    gsk_ಮಾರ್ಗ_builder_ಸಾಲು_ಗಮ್ಯ(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_ಮಾರ್ಗ_builder_ಸಾಲು_ಗಮ್ಯ(builder, 8, y - val[0] + 0.5);
	gsk_ಮಾರ್ಗ_builder_ಸಾಲು_ಗಮ್ಯ(builder, 9, y - val[1] + 0.5);

	ಮಾರ್ಗ = gsk_ಮಾರ್ಗ_builder_free_ಗಮ್ಯ_ಮಾರ್ಗ(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_ಮಾರ್ಗ_get_stroke_bounds(ಮಾರ್ಗ, stroke, &bounds);
	color_ಗಂಟು = gsk_color_ಗಂಟು_new(&white_rgba, &bounds);
	ಮಗು = gsk_stroke_ಗಂಟು_new(color_ಗಂಟು, ಮಾರ್ಗ, stroke);

	gsk_stroke_free(stroke);
	gsk_ಮಾರ್ಗ_unref(ಮಾರ್ಗ);
	gsk_render_ಗಂಟು_unref(color_ಗಂಟು);
#ಇಲ್ಲದಿದ್ದರೆ
	cairo_t *cr;

	ಮಗು = gsk_cairo_ಗಂಟು_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_ಗಂಟು_get_draw_context(ಮಗು);

	cairo_ಗಣ_ಸಾಲು_ಅಗಲ(cr, 1.0);
	cairo_ಗಣ_ಮೂಲ_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_ಗಮ್ಯ(cr, -1, y - val[7] + 0.5);
	cairo_ಸಾಲು_ಗಮ್ಯ(cr, 0, y - val[0] + 0.5);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < 8; ++i)
	    cairo_ಸಾಲು_ಗಮ್ಯ(cr, i, y - val[i] + 0.5);

	cairo_ಸಾಲು_ಗಮ್ಯ(cr, 8, y - val[0] + 0.5);
	cairo_ಸಾಲು_ಗಮ್ಯ(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#ಆದರೆ_ಅಂತ್ಯ
	drow->underc_ಮುಖವಾಡ = gsk_repeat_ಗಂಟು_new(
		&GRAPHENE_RECT_INIT(x_ಆರಂಭ, y - 3, x_ಕೊನೆ - x_ಆರಂಭ, 5),
		ಮಗು, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_ಗಂಟು_unref(ಮಗು);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_render_ಪಠ್ಯ(Draw_ಅಡ್ಡಸಾಲು *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_ಪಠ್ಯ;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_cells = 0;
    uint32_t	cur_fg = 0;
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (!dlayer->dirty)
	ಹಿಂತಿರುಗಿ;

    glyph_buf = da->glyph_buf;
    nodes = da->ಗಂಟು_buf;

    g_array_ಗಣ_ಗಾತ್ರ(glyph_buf, 0);
    g_ptr_array_ಗಣ_ಗಾತ್ರ(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#ವ್ಯಾಖ್ಯಾನಿಸು FLUSH_ಗಂಟು() \
    ಮಾಡು { \
	ಆದರೆ (cur_font != ಖಾಲಿ) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_ಗಂಟು	*ಗಂಟು; \
	    GdkRGBA		fg_color = u32_ಗಮ್ಯ_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->ದತ್ತಾಂಶ; \
	    glyph_str.ಅಂಕೆ_glyphs = glyph_buf->len; \
	    ಗಂಟು = gsk_ಪಠ್ಯ_ಗಂಟು_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_ಬಿಂದು_INIT(ಪಠ್ಯ_X(ಆರಂಭ_ಲಂಬಸಾಲು), \
			ಪಠ್ಯ_Y(drow->ಅಡ್ಡಸಾಲು))); \
	    ಆದರೆ (ಗಂಟು != ಖಾಲಿ) \
		g_ptr_array_ಕೂಡಿಸು(nodes, ಗಂಟು); \
	    g_array_ಗಣ_ಗಾತ್ರ(glyph_buf, 0); \
	    cur_font = ಖಾಲಿ; \
	} \
    } ಆಗ (FALSE)

    ಗಾಗಿ (c = 0; c < da->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	ಪೂರ್ಣಾಂಕ	    prev_len;

	ಆದರೆ (dglyphs == ಖಾಲಿ)
	{
	    ಖಾಲಿಗಳು_cells++;
	    ಮುಂದುವರಿ;
	}
#ಆದರೆ defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dglyphs->font == ಖಾಲಿ)
	{
	    // Add sign icon
	    Gsk_Render_ಗಂಟು   *snode;
	    ಪೂರ್ಣಾಂಕ		    w;

	    FLUSH_ಗಂಟು();

	    snode = gsk_texture_scale_ಗಂಟು_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->ಅಡ್ಡಸಾಲು),
			dglyphs->dsign->ಅಗಲ, dglyphs->dsign->ಎತ್ತರ),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->ಅಗಲ + gui.char_ಅಗಲ - 1) / gui.char_ಅಗಲ;

	    ಆದರೆ (snode != ಖಾಲಿ)
		g_ptr_array_ಕೂಡಿಸು(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    ಮುಂದುವರಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_ಗಂಟು();
	    cur_font = dglyphs->font;
	    ಆರಂಭ_ಲಂಬಸಾಲು = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	ಆದರೆ (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    ಮುಂದುವರಿ;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	ಆದರೆ (ಖಾಲಿಗಳು_cells > 0)
	{
	    ಆದರೆ (prev_len > 0)
	    {
		// Spaces occurred between valid text: extend
		// previous character's advance
		PangoGlyphInfo *ಕೊನೆಯ = &g_array_ಸೂಚ್ಯಂಕ(
			glyph_buf, PangoGlyphInfo,
			prev_len - 1);
		ಕೊನೆಯ->geometry.ಅಗಲ += CELLS2PANGO(ಖಾಲಿಗಳು_cells);
	    }
	    ಖಾಲಿಗಳು_cells = 0; // Width consumed
	}
    }

    FLUSH_ಗಂಟು();

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು FLUSH_ಗಂಟು

    ಆದರೆ (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_ಗಂಟು *ಮೂಲ;
	Gsk_Render_ಗಂಟು *ಗಂಟು;

	draw_ಅಡ್ಡಸಾಲು_ensure_decor(drow, DRAW_UNDERL);
	ಆದರೆ (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    ಮೂಲ = draw_layer_get_texture(dlayer, drow->ಅಡ್ಡಸಾಲು, da, 0, FALSE);
	    ಗಂಟು = gsk_ಮುಖವಾಡ_ಗಂಟು_new(ಮೂಲ, drow->underl_ಮುಖವಾಡ,
		    GSK_ಮುಖವಾಡ_ಬಹುಲಕ_ALPHA);
	    gsk_render_ಗಂಟು_unref(ಮೂಲ);
	    g_ptr_array_ಕೂಡಿಸು(nodes, ಗಂಟು);
	}

	dlayer->ಗಂಟು = gsk_container_ಗಂಟು_new(
		(Gsk_Render_ಗಂಟು **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_render_special(Draw_ಅಡ್ಡಸಾಲು *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_ಗಂಟು   *ಮೂಲ;
    Gsk_Render_ಗಂಟು   *nodes[2];
    ಪೂರ್ಣಾಂಕ		    n_nodes = 0;

    ಆದರೆ (!dlayer->dirty)
	ಹಿಂತಿರುಗಿ;

    draw_ಅಡ್ಡಸಾಲು_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    ಆದರೆ (!rgba_is_transparent(dlayer->buf + UNDERC_ಸ್ಥಾನಾಂತರ(drow->n_cells),
		drow->n_cells))
    {
	ಮೂಲ = draw_layer_get_texture(dlayer, drow->ಅಡ್ಡಸಾಲು, da,
		UNDERC_ಸ್ಥಾನಾಂತರ(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_ಮುಖವಾಡ_ಗಂಟು_new(ಮೂಲ, drow->underc_ಮುಖವಾಡ,
		GSK_ಮುಖವಾಡ_ಬಹುಲಕ_ALPHA);
	gsk_render_ಗಂಟು_unref(ಮೂಲ);
    }

    ಆದರೆ (!rgba_is_transparent(dlayer->buf + STRIKE_ಸ್ಥಾನಾಂತರ(drow->n_cells),
		drow->n_cells))
    {
	ಮೂಲ = draw_layer_get_texture(dlayer, drow->ಅಡ್ಡಸಾಲು, da,
		STRIKE_ಸ್ಥಾನಾಂತರ(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_ಮುಖವಾಡ_ಗಂಟು_new(ಮೂಲ, drow->strike_ಮುಖವಾಡ,
		GSK_ಮುಖವಾಡ_ಬಹುಲಕ_ALPHA);
	gsk_render_ಗಂಟು_unref(ಮೂಲ);
    }

    ಆದರೆ (n_nodes == 1)
    {
	dlayer->ಗಂಟು = nodes[0];
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n_nodes > 0)
    {
	dlayer->ಗಂಟು = gsk_container_ಗಂಟು_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(nodes); i++)
	    gsk_render_ಗಂಟು_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_render_overlay(Draw_ಅಡ್ಡಸಾಲು *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    ಆದರೆ (!dlayer->dirty)
	ಹಿಂತಿರುಗಿ;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    ಆದರೆ (rgba_is_transparent(dlayer->buf, da->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
	ಹಿಂತಿರುಗಿ;

    dlayer->ಗಂಟು = draw_layer_get_texture(dlayer, drow->ಅಡ್ಡಸಾಲು, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಅಡ್ಡಸಾಲು_ಕೂಡಿಸು_cluster(
	Draw_ಅಡ್ಡಸಾಲು		*drow,
	DrawAttr	*dattr,
	ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	ಪೂರ್ಣಾಂಕ		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = ಖಾಲಿ;

    ಆದರೆ (ಲಂಬಸಾಲು + n_cells > da->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	ಹಿಂತಿರುಗಿ;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    ಆದರೆ (glyphs != ಖಾಲಿ)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_ಅಡ್ಡಸಾಲು_fill(drow, ಲಂಬಸಾಲು, ಲಂಬಸಾಲು + n_cells - 1, dglyphs, dattr, &da->cursor);

    ಆದರೆ (dglyphs != ಖಾಲಿ)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vim_draw_area_ಕೂಡಿಸು_string(
	VimDrawArea *self,
	Draw_ಅಡ್ಡಸಾಲು	    *drow,
	DrawAttr    *dattr,
	ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು,
	char_u	    *s,
	ಪೂರ್ಣಾಂಕ	    len,
	ಪೂರ್ಣಾಂಕ	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			ಆರಂಭ_idx = -1;
    GList		*ಅಂಶ_list;
    ಪೂರ್ಣಾಂಕ			ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ = 0;
    ಪೂರ್ಣಾಂಕ			cluster_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ			ಕೊನೆಯ_glyph_rbearing;
    ಪೂರ್ಣಾಂಕ			cells = 0;
    ಪೂರ್ಣಾಂಕ			i = 0;
    gboolean		has_unicode = FALSE;
    gboolean		needs_shaping = FALSE;

    // Scan the string, and find spans where ligatures should not happen. We
    // must ensure that attribute boundaries do not split a base character from
    // its combining marks, as Pango breaks items at attribute boundaries.
    ಆಗ (i < len)
    {
	ಪೂರ್ಣಾಂಕ	clen = 1;
	ಪೂರ್ಣಾಂಕ	is_composing = 0;
	bool	want_disable;

	ಆದರೆ (s[i] >= 0x80)
	{
	    clen = utf_ptr2len(s + i);
	    is_composing = utf_iscomposing(utf_ptr2char(s + i));
	    has_unicode = TRUE;
	}

	ಆದರೆ (is_composing)
	    // Do not change state mid-cluster. Inherit the previous character's
	    // state.
	    want_disable = (ಆರಂಭ_idx != -1);
	ಇಲ್ಲದಿದ್ದರೆ
	    // Base character: disable ligatures if it's an ASCII non-ligature
	    // char.
	    want_disable = (s[i] < 0x80 && !gui.ligatures_map[s[i]]);

	// If any base character actually wants ligature/contextual shaping,
	// the string must go through Pango's shaping engine -- the ASCII
	// fast path below draws cached glyphs one-by-one and can never
	// produce a ligature, regardless of 'guiligatures'.
	ಆದರೆ (!is_composing && !want_disable)
	    needs_shaping = TRUE;

	ಆದರೆ (want_disable)
	{
	    ಆದರೆ (ಆರಂಭ_idx == -1)
		ಆರಂಭ_idx = i;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆರಂಭ_idx != -1)
	{
	    ಆದರೆ (attr_list == ಖಾಲಿ)
		attr_list = pango_attr_list_new();
	    ಸೇರಿಸು_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, ಆರಂಭ_idx, i);
	    ಆರಂಭ_idx = -1;
	}
	i += clen;
    }

    // Fast path for pure ASCII: use cached glyph table. Skip this path when
    // there are non-ascii characters in the string, font attributes, or if
    // there's a possible ligature.
    ಆದರೆ (!(draw_flags & DRAW_ITALIC)
	    && !((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != ಖಾಲಿ
	    && !has_unicode
	    && !needs_shaping)
    {
	ಗಾಗಿ (i = 0; i < len; ++i)
	{
	    PangoGlyphInfo  glyph = gui.ascii_glyphs->glyphs[2 * s[i]];
	    gboolean	    has_ink;

	    has_ink = glyph_has_ink(gui.ascii_font, glyph);

	    draw_ಅಡ್ಡಸಾಲು_ಕೂಡಿಸು_cluster(drow, dattr, ಲಂಬಸಾಲು + i, 1,
		    gui.ascii_font, has_ink ? &glyph : ಖಾಲಿ, 1, self);
	}

	ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ = len;
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }

    glyphs = pango_glyph_string_new();

    ಆದರೆ (attr_list == ಖಾಲಿ)
	attr_list = pango_attr_list_new();

    ಆದರೆ (ಆರಂಭ_idx != -1)
	ಸೇರಿಸು_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, ಆರಂಭ_idx, len);

    cluster_ಅಗಲ = PANGO_SCALE * gui.char_ಅಗಲ;
    ಕೊನೆಯ_glyph_rbearing = PANGO_SCALE * gui.char_ಅಗಲ;

    // If 'guifontwide' is set then use that for double-width characters.
    ಆದರೆ (gui.wide_font != ಖಾಲಿ)
	apply_wide_font_attr(s, len, attr_list);

    ಆದರೆ ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	ಸೇರಿಸು_PANGO_ATTR(pango_attr_ತೂಕ_new(PANGO_ತೂಕ_BOLD),
		attr_list, 0, len);
    ಆದರೆ (draw_flags & DRAW_ITALIC)
	ಸೇರಿಸು_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    ಅಂಶ_list = pango_itemize(gui.ಪಠ್ಯ_context,
	    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s, 0, len, attr_list, ಖಾಲಿ);

    ಆಗ (ಅಂಶ_list != ಖಾಲಿ)
    {
	Pango_ಅಂಶ   *ಅಂಶ = ಅಂಶ_list->ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ	    ಅಂಶ_cells = 0;
	ಪೂರ್ಣಾಂಕ	    cluster_ಆರಂಭ_idx = 0;
	ಪೂರ್ಣಾಂಕ	    current_cluster_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು + ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ;
	ಪೂರ್ಣಾಂಕ	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	ಅಂಶ_list = g_list_delete_link(ಅಂಶ_list, ಅಂಶ_list);

	// Force LTR direction; Vim handles bidi on its own.
	ಅಂಶ->analysis.level = (ಅಂಶ->analysis.level + 1) & (~1U);

	pango_shape_ತುಂಬಿದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s + ಅಂಶ->ಸ್ಥಾನಾಂತರ, ಅಂಶ->ಉದ್ದಳತೆ,
		(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s, len, &ಅಂಶ->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	ಗಾಗಿ (i = 0; i < glyphs->ಅಂಕೆ_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    ಆದರೆ (glyph->attr.is_cluster_ಆರಂಭ)
	    {
		ಪೂರ್ಣಾಂಕ cellcount;

		cellcount = ಎಣಿಕೆ_cluster_cells(
			s, ಅಂಶ, glyphs, i, &cluster_ಅಗಲ,
			(ಅಂಶ_list != ಖಾಲಿ) ? &ಕೊನೆಯ_glyph_rbearing : ಖಾಲಿ);

		ಆದರೆ (i > 0 && cellcount > 0)
		{
		    draw_ಅಡ್ಡಸಾಲು_ಕೂಡಿಸು_cluster(drow, dattr,
			    current_cluster_ಲಂಬಸಾಲು, current_cluster_cells,
			    ಅಂಶ->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_ಆರಂಭ_idx : ಖಾಲಿ,
			    i - cluster_ಆರಂಭ_idx, self);

		    current_cluster_ಲಂಬಸಾಲು += current_cluster_cells;
		    cluster_ಆರಂಭ_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		ಆದರೆ (cellcount > 0)
		{
		    ಪೂರ್ಣಾಂಕ ಅಗಲ;

		    ಅಗಲ = cellcount * gui.char_ಅಗಲ * PANGO_SCALE;
		    glyph->geometry.x_ಸ್ಥಾನಾಂತರ +=
			MAX(0, ಅಗಲ - cluster_ಅಗಲ) / 2;
		    glyph->geometry.ಅಗಲ = ಅಗಲ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಿದ್ಧತೆ_zero_ಅಗಲ_cluster(ಅಂಶ, glyph, cells,
			    cluster_ಅಗಲ,
			    ಕೊನೆಯ_glyph_rbearing);

		ಅಂಶ_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i > 0)
	    {
		ಪೂರ್ಣಾಂಕ ಅಗಲ;

		ಆದರೆ (glyph->geometry.x_ಸ್ಥಾನಾಂತರ >= 0)
		{
		    glyphs->glyphs[i].geometry.ಅಗಲ =
			glyphs->glyphs[i - 1].geometry.ಅಗಲ;
		    glyphs->glyphs[i - 1].geometry.ಅಗಲ = 0;
		}
		ಅಗಲ = cells * gui.char_ಅಗಲ * PANGO_SCALE;
		glyph->geometry.x_ಸ್ಥಾನಾಂತರ +=
		    MAX(0, ಅಗಲ - cluster_ಅಗಲ) / 2;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		glyph->geometry.ಅಗಲ = 0;


	    ಆದರೆ (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			ಅಂಶ->analysis.font, *glyph);
	}
	ಆದರೆ (glyphs->ಅಂಕೆ_glyphs > 0)
	    draw_ಅಡ್ಡಸಾಲು_ಕೂಡಿಸು_cluster(drow, dattr,
		    current_cluster_ಲಂಬಸಾಲು, current_cluster_cells,
		    ಅಂಶ->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_ಆರಂಭ_idx : ಖಾಲಿ,
		    glyphs->ಅಂಕೆ_glyphs - cluster_ಆರಂಭ_idx, self);

	pango_ಅಂಶ_free(ಅಂಶ);
	ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ += ಅಂಶ_cells;
    }

    pango_glyph_string_free(glyphs);

ನಿರ್ಗಮಿಸು:
    ಆದರೆ (attr_list != ಖಾಲಿ)
	pango_attr_list_unref(attr_list);

    ಹಿಂತಿರುಗಿ ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    ಪೂರ್ಣಾಂಕ
vim_draw_area_draw_string(
	VimDrawArea *self,
	ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು,
	char_u	    *s,
	ಪೂರ್ಣಾಂಕ	    len,
	ಪೂರ್ಣಾಂಕ	    flags)
{
    Draw_ಅಡ್ಡಸಾಲು	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		convlen;
    ಪೂರ್ಣಾಂಕ		n_cells ;

    ಆದರೆ (unlikely(self->ಅಡ್ಡಸಾಲುಗಳು_2 == ಖಾಲಿ
		|| ಅಡ್ಡಸಾಲು >= self->n_ಅಡ್ಡಸಾಲುಗಳು
		|| ಲಂಬಸಾಲು >= self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
	ಹಿಂತಿರುಗಿ len;

    ಆದರೆ (output_conv.vc_type != CONV_NONE)
    {
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	ಆದರೆ (conv_buf != ಖಾಲಿ)
	{
	    s = conv_buf;
	    len = convlen;
	}
    }

    // Safety check: pango crashes with invalid utf-8.
    ಆದರೆ (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	ಹಿಂತಿರುಗಿ len;
    }

    drow = self->ಅಡ್ಡಸಾಲುಗಳು_2 + ಅಡ್ಡಸಾಲು;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_ಕೂಡಿಸು_string(self, drow, &dattr, ಲಂಬಸಾಲು, s, len, flags);

    vim_free(conv_buf);

    ಹಿಂತಿರುಗಿ n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    ಶೂನ್ಯ
vim_draw_area_clear_ಖಂಡ(
	VimDrawArea	*self,
	ಪೂರ್ಣಾಂಕ		row1,
	ಪೂರ್ಣಾಂಕ		col1,
	ಪೂರ್ಣಾಂಕ		row2,
	ಪೂರ್ಣಾಂಕ		col2)
{
    ಆದರೆ (unlikely(self->ಅಡ್ಡಸಾಲುಗಳು_2 == ಖಾಲಿ
		|| row1 >= self->n_ಅಡ್ಡಸಾಲುಗಳು
		|| col1 >= self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ
		|| row2 >= self->n_ಅಡ್ಡಸಾಲುಗಳು
		|| col2 >= self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = row1; r <= row2; r++)
	draw_ಅಡ್ಡಸಾಲು_fill(self->ಅಡ್ಡಸಾಲುಗಳು_2 + r, col1, col2, ಖಾಲಿ, ಖಾಲಿ, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    ಶೂನ್ಯ
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_ಖಂಡ(self, 0, 0, self->n_ಅಡ್ಡಸಾಲುಗಳು - 1, self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    ಶೂನ್ಯ
vim_draw_area_move_ಖಂಡ(
	VimDrawArea *self,
	ಪೂರ್ಣಾಂಕ	    ಗಮ್ಯ,
	ಪೂರ್ಣಾಂಕ	    row1,
	ಪೂರ್ಣಾಂಕ	    row2,
	ಪೂರ್ಣಾಂಕ	    col1,
	ಪೂರ್ಣಾಂಕ	    col2)
{
    ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = row2 - row1;

    ಆದರೆ (unlikely(self->ಅಡ್ಡಸಾಲುಗಳು_2 == ಖಾಲಿ
		|| row1 >= self->n_ಅಡ್ಡಸಾಲುಗಳು
		|| row2 >= self->n_ಅಡ್ಡಸಾಲುಗಳು
		|| ಗಮ್ಯ >= self->n_ಅಡ್ಡಸಾಲುಗಳು
		|| col1 >= self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ
		|| col2 >= self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
	ಹಿಂತಿರುಗಿ;

    ಪ್ರತಿಜ್ಞಾ(row2 >= row1);
    ಪ್ರತಿಜ್ಞಾ(col2 >= col1);
    ಪ್ರತಿಜ್ಞಾ(row1 != ಗಮ್ಯ);

    ಆದರೆ (row1 > ಗಮ್ಯ)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ o = 0; o <= ಸ್ಥಾನಾಂತರ; o++)
	    draw_ಅಡ್ಡಸಾಲು_move_ಗಮ್ಯ(self->ಅಡ್ಡಸಾಲುಗಳು_2 + ಗಮ್ಯ + o, self->ಅಡ್ಡಸಾಲುಗಳು_2 + row1 + o,
		    col1, col2, self);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ o = ಸ್ಥಾನಾಂತರ; o >= 0; o--)
	    ಆದರೆ (ಗಮ್ಯ + o >= self->n_ಅಡ್ಡಸಾಲುಗಳು)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_ಖಂಡ(self, row1 + o, col1, row1 + o, col2);
	    ಇಲ್ಲದಿದ್ದರೆ
		draw_ಅಡ್ಡಸಾಲು_move_ಗಮ್ಯ(self->ಅಡ್ಡಸಾಲುಗಳು_2 + ಗಮ್ಯ + o, self->ಅಡ್ಡಸಾಲುಗಳು_2 + row1 + o,
			col1, col2, self);
    }
}

/*
 * Set the cursor to the given width and height, use "gui.bgcolor" and
 * "gui.fgcolor" for the colors as well. If "w" and "h" is zero, then a full
 * block cursor is used, if "w" and "h" is -1 then a hollow cursor is used,
 * otherwise a part cursor with dimensions "w" and "h" is used.
 */
    ಶೂನ್ಯ
vim_draw_area_ಗಣ_cursor(VimDrawArea *self, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    self->cursor.draw = TRUE;
    self->cursor.ಅಗಲ = w;
    self->cursor.ಎತ್ತರ = h;
    // Remember this now: the snapshot runs later, when the drawing position
    // has moved on.
    self->cursor.n_cells = 1 + mb_lefthalve(gui.cursor_ಅಡ್ಡಸಾಲು, gui.cursor_ಲಂಬಸಾಲು);
    self->cursor.bg_color = *gui.bgcolor;
    self->cursor.fg_color = *gui.fgcolor;
}

/*
 * Invert the rectangle in the draw area.
 */
    ಶೂನ್ಯ
vim_draw_area_invert_ಖಂಡ(
	VimDrawArea	*self,
	ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ		nrows,
	ಪೂರ್ಣಾಂಕ		ncols)
{
    ಆದರೆ (unlikely(self->ಅಡ್ಡಸಾಲುಗಳು_2 == ಖಾಲಿ
		|| ಅಡ್ಡಸಾಲು >= self->n_ಅಡ್ಡಸಾಲುಗಳು
		|| ಲಂಬಸಾಲು >= self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ
		|| ಅಡ್ಡಸಾಲು + nrows - 1 >= self->n_ಅಡ್ಡಸಾಲುಗಳು
		|| ಲಂಬಸಾಲು + ncols - 1 >= self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = ಅಡ್ಡಸಾಲು; r < ಅಡ್ಡಸಾಲು + nrows; r++)
    {
	Draw_ಅಡ್ಡಸಾಲು	    *drow = self->ಅಡ್ಡಸಾಲುಗಳು_2 + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ c = 0; c < ncols; c++)
	    dlayer->buf[c + ಲಂಬಸಾಲು] = ~dlayer->buf[c + ಲಂಬಸಾಲು];
	draw_ಅಡ್ಡಸಾಲು_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#ಆದರೆ defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    ಶೂನ್ಯ
vim_draw_area_ಕೂಡಿಸು_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ	    ಅಗಲ,
	ಪೂರ್ಣಾಂಕ	    ಎತ್ತರ)
{
    Draw_ಅಡ್ಡಸಾಲು	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    ಪೂರ್ಣಾಂಕ		cells;

    ಆದರೆ (unlikely(self->ಅಡ್ಡಸಾಲುಗಳು_2 == ಖಾಲಿ
		|| ಅಡ್ಡಸಾಲು >= self->n_ಅಡ್ಡಸಾಲುಗಳು
		|| ಲಂಬಸಾಲು >= self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
	ಹಿಂತಿರುಗಿ;

    drow = self->ಅಡ್ಡಸಾಲುಗಳು_2 + ಅಡ್ಡಸಾಲು;
    dsign = draw_sign_new(sign, ಅಗಲ, ಎತ್ತರ);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (ಅಗಲ + gui.char_ಅಗಲ - 1) / gui.char_ಅಗಲ;

    draw_ಅಡ್ಡಸಾಲು_fill(drow, ಲಂಬಸಾಲು, ಲಂಬಸಾಲು + cells - 1, dglyphs, ಖಾಲಿ, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    ಶೂನ್ಯ
vim_draw_area_ಕೂಡಿಸು_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ		ಅಗಲ,
	ಪೂರ್ಣಾಂಕ		ಎತ್ತರ)
{
    Draw_ಅಡ್ಡಸಾಲು	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    ಪೂರ್ಣಾಂಕ		cells;
    GdkTexture	*texture;
    GBytes	*ಬೈಟುಗಳು;

    ಆದರೆ (unlikely(self->ಅಡ್ಡಸಾಲುಗಳು_2 == ಖಾಲಿ
		|| ಅಡ್ಡಸಾಲು >= self->n_ಅಡ್ಡಸಾಲುಗಳು
		|| ಲಂಬಸಾಲು >= self->n_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
	ಹಿಂತಿರುಗಿ;

    cairo_surface_flush(surf);
    ಬೈಟುಗಳು = g_ಬೈಟುಗಳು_new_with_free_func(cairo_image_surface_get_ದತ್ತಾಂಶ(surf),
	    cairo_image_surface_get_ಎತ್ತರ(surf)
	    * cairo_image_surface_get_stride(surf),
	    (GDestroyNotify)cairo_surface_destroy,
	    cairo_surface_reference(surf));

    texture = gdk_ಸ್ಮರಣೆ_texture_new(cairo_image_surface_get_ಅಗಲ(surf),
	    cairo_image_surface_get_ಎತ್ತರ(surf),
	    GDK_ಸ್ಮರಣೆ_A8_R8_G8_B8_PREMULTIPLIED,
	    ಬೈಟುಗಳು, cairo_image_surface_get_stride(surf));
    g_ಬೈಟುಗಳು_unref(ಬೈಟುಗಳು);

    drow = self->ಅಡ್ಡಸಾಲುಗಳು_2 + ಅಡ್ಡಸಾಲು;
    dsign = draw_sign_new(texture, ಅಗಲ, ಎತ್ತರ);
    g_object_unref(texture);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (ಅಗಲ + gui.char_ಅಗಲ - 1) / gui.char_ಅಗಲ;

    draw_ಅಡ್ಡಸಾಲು_fill(drow, ಲಂಬಸಾಲು, ಲಂಬಸಾಲು + cells - 1, dglyphs, ಖಾಲಿ, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    ಶೂನ್ಯ
vim_draw_area_ಕೂಡಿಸು_external(VimDrawArea *self, Gsk_Render_ಗಂಟು *ಗಂಟು)
{
    g_hash_ಕೋಷ್ಟಕ_ಕೂಡಿಸು(self->external, gsk_render_ಗಂಟು_ref(ಗಂಟು));
}

    ಶೂನ್ಯ
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_ಗಂಟು *ಗಂಟು)
{
    g_hash_ಕೋಷ್ಟಕ_remove(self->external, ಗಂಟು);
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Add the cursor to the snapshot.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_ಗಂಟು	*ಪಠ್ಯ_ಗಂಟು,	// May be NULL
	Gsk_Render_ಗಂಟು	*special_ಗಂಟು,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    ಪೂರ್ಣಾಂಕ		    w = cursor->ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		    h = cursor->ಎತ್ತರ;
    graphene_rect_t rect;

    ಆದರೆ (!cursor->draw)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (cursor->ಅಗಲ <= 0 && cursor->ಎತ್ತರ <= 0)
    {
	w += gui.char_ಅಗಲ * cursor->n_cells;
	h = gui.char_ಎತ್ತರ;
    }

    graphene_rect_init(&rect,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    CURSOR_BAR_ಬಲ ? FILL_X(gui.cursor_ಲಂಬಸಾಲು + 1) - w :
#ಆದರೆ_ಅಂತ್ಯ
	    FILL_X(gui.cursor_ಲಂಬಸಾಲು),
	    FILL_Y(gui.cursor_ಅಡ್ಡಸಾಲು) + gui.char_ಎತ್ತರ - h, w, h);


    ಆದರೆ (cursor->ಅಗಲ == -1 && cursor->ಎತ್ತರ == -1)
    {
	// Create hollow cursor
	GskRoundedRect	    outline;
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ  border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	ಸ್ಥಿರಾಂಕ GdkRGBA	    color[4] = {
	    cursor->bg_color, cursor->bg_color,
	    cursor->bg_color, cursor->bg_color
	} ;

	gsk_rounded_rect_init_from_rect(&outline, &rect, 0.0f);
	gtk_snapshot_append_border(snapshot, &outline, border, color);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Create block style cursor, by masking the text + under decorations
	// with the fg color.
	gtk_snapshot_append_color(snapshot, &cursor->bg_color, &rect);

	gtk_snapshot_ತಳ್ಳು_ಮುಖವಾಡ(snapshot, GSK_ಮುಖವಾಡ_ಬಹುಲಕ_ALPHA);

	// Used as an "outline" for the fg color.
	ಆದರೆ (ಪಠ್ಯ_ಗಂಟು != ಖಾಲಿ)
	    gtk_snapshot_append_ಗಂಟು(snapshot, ಪಠ್ಯ_ಗಂಟು);
	ಆದರೆ (special_ಗಂಟು != ಖಾಲಿ)
	    gtk_snapshot_append_ಗಂಟು(snapshot, special_ಗಂಟು);

	gtk_snapshot_ತೆಗೆ(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_ತೆಗೆ(snapshot);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    ಪೂರ್ಣಾಂಕ		    ಎತ್ತರ, ಅಗಲ;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = ಖಾಲಿ;
    Gsk_Render_ಗಂಟು   *body_ಗಂಟು;
    Gsk_Render_ಗಂಟು   *invert_ಗಂಟು = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_GUI
    Gsk_Render_ಗಂಟು   *ext_ಗಂಟು;
    GHash_ಕೋಷ್ಟಕ_Iter  iter;
#ಆದರೆ_ಅಂತ್ಯ

    gui_mch_ಗಣ_bg_color(gui.ಹಿಂಭಾಗ_pixel);
    ಎತ್ತರ = gtk_widget_get_ಎತ್ತರ(widget) + gui.bleed_bot;
    ಅಗಲ = gtk_widget_get_ಅಗಲ(widget) + gui.bleed_ಬಲ;

    ಆದರೆ (self->ಅಡ್ಡಸಾಲುಗಳು_2 == ಖಾಲಿ)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, ಅಗಲ, ಎತ್ತರ));
	ಹಿಂತಿರುಗಿ;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    ಆದರೆ (self->bleed_ಬಲ == -1 || self->bleed_ಬಲ != gui.bleed_ಬಲ)
    {
	self->bleed_ಬಲ = gui.bleed_ಬಲ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = 0; r < self->n_ಅಡ್ಡಸಾಲುಗಳು; r++)
	{
	    draw_ಅಡ್ಡಸಾಲು_dirty_layer(self->ಅಡ್ಡಸಾಲುಗಳು_2 + r, DRAW_LAYER_BACKGROUND);
	    draw_ಅಡ್ಡಸಾಲು_dirty_layer(self->ಅಡ್ಡಸಾಲುಗಳು_2 + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(4,24,0)
    gtk_snapshot_ಗಣ_snap(body_snapshot, GSK_RECT_SNAP_ಸುತ್ತು);
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = 0; r < self->n_ಅಡ್ಡಸಾಲುಗಳು; r++)
    {
	Draw_ಅಡ್ಡಸಾಲು *drow = self->ಅಡ್ಡಸಾಲುಗಳು_2 + r;

	draw_ಅಡ್ಡಸಾಲು_render_background(drow, self);
	draw_ಅಡ್ಡಸಾಲು_render_ಪಠ್ಯ(drow, self);
	draw_ಅಡ್ಡಸಾಲು_render_special(drow, self);
	draw_ಅಡ್ಡಸಾಲು_render_overlay(drow, self);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    ಆದರೆ (dlayer->ಗಂಟು != ಖಾಲಿ)
	    {
		ಆದರೆ (l == DRAW_LAYER_OVERLAY)
		{
		    ಆದರೆ (invert_snapshot == ಖಾಲಿ)
		    {
			invert_snapshot = gtk_snapshot_new();
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(4,24,0)
			gtk_snapshot_ಗಣ_snap(invert_snapshot,
				GSK_RECT_SNAP_ಸುತ್ತು);
#ಆದರೆ_ಅಂತ್ಯ
		    }
		    gtk_snapshot_append_ಗಂಟು(invert_snapshot, dlayer->ಗಂಟು);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    gtk_snapshot_append_ಗಂಟು(body_snapshot, dlayer->ಗಂಟು);
	    }
	}

	ಆದರೆ (r == gui.cursor_ಅಡ್ಡಸಾಲು)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_ಪಠ್ಯ].ಗಂಟು,
		    drow->dlayers[DRAW_LAYER_SPECIAL].ಗಂಟು,
		    body_snapshot);
    }

    ಆದರೆ (invert_snapshot != ಖಾಲಿ)
	gtk_snapshot_ತಳ್ಳು_blend(snapshot, GSK_BLEND_ಬಹುಲಕ_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, ಅಗಲ, ಎತ್ತರ));
    body_ಗಂಟು = gtk_snapshot_free_ಗಮ್ಯ_ಗಂಟು(body_snapshot);
    ಆದರೆ (body_ಗಂಟು != ಖಾಲಿ)
    {
	gtk_snapshot_append_ಗಂಟು(snapshot, body_ಗಂಟು);
	gsk_render_ಗಂಟು_unref(body_ಗಂಟು);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_ಕೋಷ್ಟಕ_iter_init(&iter, self->external);

    ಆಗ (g_hash_ಕೋಷ್ಟಕ_iter_ಮುಂದಿನ(&iter, (ಶೂನ್ಯ **)&ext_ಗಂಟು, ಖಾಲಿ))
	gtk_snapshot_append_ಗಂಟು(snapshot, ext_ಗಂಟು);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (invert_snapshot != ಖಾಲಿ)
    {
	gtk_snapshot_ತೆಗೆ(snapshot);
	invert_ಗಂಟು = gtk_snapshot_free_ಗಮ್ಯ_ಗಂಟು(invert_snapshot);
	// Not sure if it can be NULL but still check
	ಆದರೆ (invert_ಗಂಟು != ಖಾಲಿ)
	{
	    gtk_snapshot_append_ಗಂಟು(snapshot, invert_ಗಂಟು);
	    gsk_render_ಗಂಟು_unref(invert_ಗಂಟು);
	}
	gtk_snapshot_ತೆಗೆ(snapshot);
    }
}
