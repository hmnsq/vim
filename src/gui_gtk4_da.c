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
    صحيح		ويڪر;
    صحيح		اوچائي;
} DrawSign;
#جيڪڏهن_پورو

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
#جيڪڏهن_پورو
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
    DRAW_LAYER_متن,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

تعريفنوع بنية
{
    Gsk_Render_ڳنڍ   *ڳنڍ;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#وصف_ڪريو UNDERC_وٿي(n_کانن_ڳڻپ) (0)
#وصف_ڪريو STRIKE_وٿي(n_کانن_ڳڻپ) (n_کانن_ڳڻپ)

تعريفنوع بنية
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    صحيح		n_cells; // For convenience
    صحيح		صف;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_ڳنڍ *underl_نقاب;
    Gsk_Render_ڳنڍ *underc_نقاب;
    Gsk_Render_ڳنڍ *strike_نقاب;
} Draw_صف;

#وصف_ڪريو وجهو_PANGO_ATTR(Attribute, AttrList, شروعات_2, پڇاڙي_2) \
    افعل { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->شروعات_اشاريو = (شروعات_2); \
	tmp_attr_->پڇاڙي_اشاريو = (پڇاڙي_2); \
	pango_attr_list_وجهو((AttrList), tmp_attr_); \
    } طالما (FALSE)
#وصف_ڪريو CELLS2PANGO(c) ((c) * gui.char_ويڪر * PANGO_SCALE)

/*
 * Attributes of cursor
 */
تعريفنوع بنية
{
    gboolean draw; // If cursor should be drawn
    صحيح ويڪر;
    صحيح اوچائي;
    صحيح n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

بنية _VimDrawArea
{
    GtkWidget والد;

    Draw_صف *صفون_2;

    صحيح n_صفون;
    صحيح	n_کانن_ڳڻپ;

    صحيح	bleed_ساڄو;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*ڳنڍ_buf;

    DrawCursor cursor;

#جيڪڏهن_وصف_ٿيل FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHash_جدول *external;
#جيڪڏهن_پورو
};

ساكن فراغ draw_صف_init(Draw_صف *drow, صحيح صف, صحيح کانن_ڳڻپ);
ساكن فراغ draw_صف_clear(Draw_صف *drow);
ساكن فراغ draw_صف_dirty_layer(Draw_صف *drow, DrawLayerType dlayer_t);
ساكن فراغ vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

ساكن GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    ساكن فراغ
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    لكل (صحيح r = 0; r < self->n_صفون; r++)
	draw_صف_clear(self->صفون_2 + r);
    g_free(self->صفون_2);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->ڳنڍ_buf, TRUE);

#جيڪڏهن_وصف_ٿيل FEAT_IMAGE_GUI
    g_hash_جدول_unref(self->external);
#جيڪڏهن_پورو

    G_OBJECT_CLASS(vim_draw_area_والد_class)->finalize(obj);
}

    ساكن فراغ
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_سيٽ_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    ساكن فراغ
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_ساڄو = -1;
#جيڪڏهن_وصف_ٿيل FEAT_IMAGE_GUI
    self->external = g_hash_جدول_new_ڀريل(g_سڌو_hash, g_سڌو_equal,
	    عدم, (GDestroyNotify)gsk_render_ڳنڍ_unref);
#جيڪڏهن_پورو
    self->glyph_buf = g_array_new(FALSE, FALSE, حجم(PangoGlyphInfo));
    self->ڳنڍ_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_ڳنڍ_unref);

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
vim_draw_area_سيٽ_سائيز(VimDrawArea *self, صحيح صفون_2, صحيح کانن_ڳڻپ)
{
    إذا (self->صفون_2 != عدم && self->n_صفون == صفون_2 && self->n_کانن_ڳڻپ == کانن_ڳڻپ)
	أرجع;
    إذا (صفون_2 == 0 || کانن_ڳڻپ == 0)
	أرجع;

    vim_draw_area_clear(self);

    لكل (صحيح r = 0; r < self->n_صفون; r++)
	draw_صف_clear(self->صفون_2 + r);
    self->صفون_2 = g_realloc_n(self->صفون_2, صفون_2, حجم(Draw_صف));
    لكل (صحيح r = 0; r < صفون_2; r++)
	draw_صف_init(self->صفون_2 + r, r, کانن_ڳڻپ);

    self->n_صفون = صفون_2;
    self->n_کانن_ڳڻپ = کانن_ڳڻپ;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    ساكن uint32_t
rgba_منزل_u32(ثابت GdkRGBA *rgba)
{
    اتحاد { uint32_t val; uint8_t بائيٽون[4]; } color;

    color.بائيٽون[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.بائيٽون[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.بائيٽون[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.بائيٽون[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    أرجع color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    ساكن GdkRGBA
u32_منزل_rgba(uint32_t val)
{
    اتحاد { uint32_t val; uint8_t بائيٽون[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.بائيٽون[0] / 255.0;
    color.green = packed.بائيٽون[1] / 255.0;
    color.blue  = packed.بائيٽون[2] / 255.0;
    color.alpha = packed.بائيٽون[3] / 255.0;
    أرجع color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    ساكن gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    اتحاد { uint32_t val; uint8_t بائيٽون[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.بائيٽون[0] << 16) |
	((guicolor_T)packed.بائيٽون[1] << 8)  |
	packed.بائيٽون[2];
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

    أرجع ink.ويڪر > 0 && ink.اوچائي > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    ساكن فراغ
apply_wide_font_attr(char_u *s, صحيح len, PangoAttrList *attr_list)
{
    char_u  *شروعات = عدم;
    char_u  *p;
    صحيح	    uc;

    لكل (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	إذا (شروعات == عدم)
	{
	    إذا (uc >= 0x80 && utf_char2cells(uc) == 2)
		شروعات = p;
	}
	وإلا إذا (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    وجهو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, شروعات - s, p - s);
	    شروعات = عدم;
	}
    }

    إذا (شروعات != عدم)
	وجهو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, شروعات - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    ساكن صحيح
ڳڻپ_cluster_cells(
	char_u		    *s,
	Pango_شي	    *شي,
	PangoGlyphString    *glyphs,
	صحيح		    i,
	صحيح		    *cluster_ويڪر,
	صحيح		    *آخري_glyph_rbearing)
{
    char_u  *p;
    صحيح	    ايندڙ;
    صحيح	    شروعات, پڇاڙي;
    صحيح	    ويڪر;
    صحيح	    uc;
    صحيح	    cellcount = 0;

    ويڪر = glyphs->glyphs[i].geometry.ويڪر;

    لكل (ايندڙ = i + 1; ايندڙ < glyphs->انگ_glyphs; ++ايندڙ)
    {
	إذا (glyphs->glyphs[ايندڙ].attr.is_cluster_شروعات)
	    اكسر;
	وإلا إذا (glyphs->glyphs[ايندڙ].geometry.ويڪر > ويڪر)
	    ويڪر = glyphs->glyphs[ايندڙ].geometry.ويڪر;
    }

    شروعات = شي->وٿي + glyphs->log_clusters[i];
    پڇاڙي   = شي->وٿي + ((ايندڙ < glyphs->انگ_glyphs) ?
	    glyphs->log_clusters[ايندڙ] : شي->ڊيگهائي);

    لكل (p = s + شروعات; p < s + پڇاڙي; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	إذا (uc < 0x80)
	    ++cellcount;
	وإلا إذا (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    إذا (آخري_glyph_rbearing != عدم
	    && cellcount > 0 && ايندڙ == glyphs->انگ_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(شي->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, عدم);

	إذا (PANGO_RBEARING(ink_rect) > 0)
	    *آخري_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    إذا (cellcount > 0)
	*cluster_ويڪر = ويڪر;

    أرجع cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    ساكن فراغ
تياري_zero_ويڪر_cluster(
	Pango_شي	*شي,
	PangoGlyphInfo	*glyph,
	صحيح		آخري_cellcount,
	صحيح		آخري_cluster_ويڪر,
	صحيح		آخري_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحيح		    ويڪر;

    ويڪر = آخري_cellcount * gui.char_ويڪر * PANGO_SCALE;
    glyph->geometry.x_وٿي = -ويڪر + MAX(0, ويڪر - آخري_cluster_ويڪر) / 2;
    glyph->geometry.ويڪر = 0;

    pango_font_get_glyph_extents(شي->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    إذا (ink_rect.x < 0)
    {
	glyph->geometry.x_وٿي += آخري_glyph_rbearing;
	glyph->geometry.y_وٿي  = logical_rect.اوچائي
	    - (gui.char_اوچائي - p_linespace) * PANGO_SCALE;
    }
    وإلا
	glyph->geometry.x_وٿي = -ويڪر + MAX(0, ويڪر - ink_rect.ويڪر) / 2;
}

#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    ساكن DrawSign *
draw_sign_new(GdkTexture *texture, صحيح ويڪر, صحيح اوچائي)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->ويڪر = ويڪر;
    dsign->اوچائي = اوچائي;

    أرجع dsign;
}

    ساكن فراغ
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#جيڪڏهن_پورو

    ساكن فراغ
draw_attr_init(
	DrawAttr	*dattr,
	ثابت GdkRGBA	*bg_color,
	ثابت GdkRGBA	*fg_color,
	ثابت GdkRGBA	*sp_color,
	صحيح		draw_flags)
{
    dattr->bg_color = rgba_منزل_u32(bg_color);
    dattr->fg_color = rgba_منزل_u32(fg_color);
    dattr->sp_color = rgba_منزل_u32(sp_color);
    dattr->draw_flags = draw_flags & (DRAW_UNDERL | DRAW_UNDERC | DRAW_STRIKE);
}

/*
 * Apply the draw attr to the row for the given col/cell. Only dirty layers that
 * have actually changed.
 */
    ساكن فراغ
draw_attr_apply(DrawAttr *dattr, صحيح کانو, Draw_صف *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + کانو;
    uint32_t	*متن_cell = drow->dlayers[DRAW_LAYER_متن].buf + کانو;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    صحيح		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	کانو + UNDERC_وٿي(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	کانو + STRIKE_وٿي(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    إذا (dattr != عدم)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.پٺ_pixel, bg_color);
    }

    إذا (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_صف_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	إذا (bg_is_default)
	    *bg_cell = 0;
	وإلا
	    *bg_cell = bg_color;
    }
    إذا ((draw_flags & DRAW_UNDERL && *متن_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *متن_cell != 0))
    {
	draw_صف_dirty_layer(drow, DRAW_LAYER_متن);
	إذا (draw_flags & DRAW_UNDERL)
	    *متن_cell = fg_color;
	وإلا
	    *متن_cell = 0;
    }
    إذا ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_صف_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	إذا (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	وإلا
	    *underc_cell = 0;
    }
    إذا ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_صف_dirty_layer(drow, DRAW_LAYER_SPECIAL);
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
#جيڪڏهن_پورو

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
#جيڪڏهن_پورو
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
		|| a->glyphs[i].geometry.ويڪر != b->glyphs[i].geometry.ويڪر
		|| a->glyphs[i].geometry.x_وٿي != b->glyphs[i].geometry.x_وٿي
		|| a->glyphs[i].geometry.y_وٿي != b->glyphs[i].geometry.y_وٿي
		|| a->glyphs[i].attr.is_cluster_شروعات != b->glyphs[i].attr.is_cluster_شروعات)
	    أرجع FALSE;
    أرجع TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    ساكن Gsk_Render_ڳنڍ *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	صحيح	    صف,
	VimDrawArea *da,
	صحيح	    off,
	gboolean    bleed)
{
    GBytes	    *بائيٽون;
    GdkTexture	    *texture;
    Gsk_Render_ڳنڍ   *ڳنڍ;
    صحيح		    اوچائي;

#إذا GTK_CHECK_نسخو(4,24,0)
    اوچائي = gui.char_اوچائي;
#وإلا
    اوچائي = gui.char_اوچائي + 1;
#جيڪڏهن_پورو

    إذا (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		حجم(uint32_t) * da->n_کانن_ڳڻپ);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_کانن_ڳڻپ - 1)),
		حجم(uint32_t)); // Add bleed using last pixel of buffer
	بائيٽون = g_byte_array_free_منزل_بائيٽون(arr);
    }
    وإلا
	بائيٽون = g_بائيٽون_new(dlayer->buf + off, حجم(uint32_t) * da->n_کانن_ڳڻپ);

    texture = gdk_يادگيري_texture_new(da->n_کانن_ڳڻپ + bleed, 1,
	    GDK_يادگيري_R8_G8_B8_A8, بائيٽون, (da->n_کانن_ڳڻپ + bleed) * 4);
    g_بائيٽون_unref(بائيٽون);

    // Scale texture to actual size
    ڳنڍ = gsk_texture_scale_ڳنڍ_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(صف),
		(da->n_کانن_ڳڻپ + bleed) * gui.char_ويڪر, gui.char_اوچائي + bleed),
	    GSK_SCALING_FILTER_سڀ_کان_ويجهو);
    إذا (bleed)
    {
	Gsk_Render_ڳنڍ *new;

	new = gsk_clip_ڳنڍ_new(ڳنڍ,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(صف),
		    da->n_کانن_ڳڻپ * gui.char_ويڪر + da->bleed_ساڄو, اوچائي));
	gsk_render_ڳنڍ_unref(ڳنڍ);
	ڳنڍ = new;
    }
    g_object_unref(texture);
    أرجع ڳنڍ;
}

    ساكن فراغ
draw_صف_init(Draw_صف *drow, صحيح صف, صحيح کانن_ڳڻپ)
{
    drow->cells = g_malloc0_n(کانن_ڳڻپ, حجم(DrawCell));
    drow->n_cells = کانن_ڳڻپ;
    drow->صف = صف;
    drow->underl_نقاب = drow->underc_نقاب = drow->strike_نقاب = عدم;

    memset(drow->dlayers, 0, حجم(DrawLayer) * N_DRAW_LAYERS);
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	إذا (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(کانن_ڳڻپ * 2, 4);
	وإلا
	    dlayer->buf = g_malloc0_n(کانن_ڳڻپ, 4);
    }
}

    ساكن فراغ
draw_صف_clear(Draw_صف *drow)
{
    g_free(drow->cells);
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	إذا (dlayer->ڳنڍ != عدم)
	    gsk_render_ڳنڍ_unref(dlayer->ڳنڍ);
	g_free(dlayer->buf);
    }
    g_clear_اشارو(&drow->underl_نقاب, gsk_render_ڳنڍ_unref);
    g_clear_اشارو(&drow->underc_نقاب, gsk_render_ڳنڍ_unref);
    g_clear_اشارو(&drow->strike_نقاب, gsk_render_ڳنڍ_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    ساكن فراغ
draw_صف_dirty_layer(Draw_صف *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_اشارو(&dlayer->ڳنڍ, gsk_render_ڳنڍ_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    ساكن فراغ
draw_صف_uninvert_cell(Draw_صف *drow, صحيح کانو)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    إذا (buf[کانو] == 0xffffffff)
    {
	draw_صف_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[کانو] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    ساكن فراغ
draw_صف_سيٽ_cell(Draw_صف *drow, صحيح کانو, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + کانو;

    إذا (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_صف_dirty_layer(drow, DRAW_LAYER_متن);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    إذا (dcell->dglyphs != dglyphs)
    {
	إذا (dcell->dglyphs != عدم)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == عدم ? عدم : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, کانو, drow);
    draw_صف_uninvert_cell(drow, کانو);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    ساكن فراغ
draw_صف_fill(
	Draw_صف	    *drow,
	صحيح	    col1,
	صحيح	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    لكل (صحيح c = col1; c <= col2; c++)
	draw_صف_سيٽ_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    إذا (gui.cursor_صف == drow->صف && gui.cursor_کانو >= col1
	    && gui.cursor_کانو <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    ساكن فراغ
draw_صف_move_منزل(
	Draw_صف	    *dest_صف,
	Draw_صف	    *src_صف,
	صحيح	    col1,
	صحيح	    col2,
	VimDrawArea *da)
{
    صحيح move_سائيز = (col2 - col1 + 1) * حجم(DrawCell);

    draw_صف_fill(dest_صف, col1, col2, عدم, عدم, &da->cursor);
    memmove(dest_صف->cells + col1, src_صف->cells + col1, move_سائيز);
    // NULL the draw cells so we don't double unreference.
    memset(src_صف->cells + col1, 0, move_سائيز);
    draw_صف_dirty_layer(src_صف, DRAW_LAYER_متن);

    // Move the layer buffers
    move_سائيز = (col2 - col1 + 1) * 4;
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_صف->dlayers[l].buf;
	uint32_t *dest = dest_صف->dlayers[l].buf;

	إذا (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_وٿي(dest_صف->n_cells),
		    src + col1 + UNDERC_وٿي(src_صف->n_cells), move_سائيز);
	    memmove(dest + col1 + STRIKE_وٿي(dest_صف->n_cells),
		    src + col1 + STRIKE_وٿي(src_صف->n_cells), move_سائيز);
	}
	وإلا
	    memmove(dest + col1, src + col1, move_سائيز);
	draw_صف_dirty_layer(dest_صف, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    ساكن فراغ
draw_صف_render_background(Draw_صف *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    إذا (!dlayer->dirty)
	أرجع;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    إذا (rgba_is_transparent(dlayer->buf, da->n_کانن_ڳڻپ))
	أرجع;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->ڳنڍ = draw_layer_get_texture(dlayer, drow->صف, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    ساكن فراغ
draw_صف_ensure_decor(Draw_صف *drow, صحيح flags)
{
    إذا (flags & DRAW_UNDERL && drow->underl_نقاب == عدم)
	drow->underl_نقاب = gsk_color_ڳنڍ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->صف + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    إذا (flags & DRAW_STRIKE && drow->strike_نقاب == عدم)
	drow->strike_نقاب = gsk_color_ڳنڍ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->صف) + (صحيح)(gui.char_اوچائي / 2),
		    FILL_X(drow->n_cells), 1));

    إذا (flags & DRAW_UNDERC && drow->underc_نقاب == عدم)
    {
	ساكن ثابت صحيح val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	صحيح y = FILL_Y(drow->صف + 1) - 1;
	صحيح x_شروعات = FILL_X(0);
	صحيح x_پڇاڙي = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_ڳنڍ *ٻار = عدم;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#إذا GTK_CHECK_نسخو(4, 14, 0)
	Gsk_رستو_Builder	*builder;
	Gsk_رستو		*رستو;
	GskStroke       *stroke;
	Gsk_Render_ڳنڍ	*color_ڳنڍ;
	graphene_rect_t bounds;

	builder = gsk_رستو_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_رستو_builder_move_منزل(builder, -1, y - val[7] + 0.5);
	gsk_رستو_builder_سٽ_منزل(builder, 0, y - val[0] + 0.5);

	لكل (صحيح i = 1; i < 8; i++)
	    gsk_رستو_builder_سٽ_منزل(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_رستو_builder_سٽ_منزل(builder, 8, y - val[0] + 0.5);
	gsk_رستو_builder_سٽ_منزل(builder, 9, y - val[1] + 0.5);

	رستو = gsk_رستو_builder_free_منزل_رستو(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_رستو_get_stroke_bounds(رستو, stroke, &bounds);
	color_ڳنڍ = gsk_color_ڳنڍ_new(&white_rgba, &bounds);
	ٻار = gsk_stroke_ڳنڍ_new(color_ڳنڍ, رستو, stroke);

	gsk_stroke_free(stroke);
	gsk_رستو_unref(رستو);
	gsk_render_ڳنڍ_unref(color_ڳنڍ);
#وإلا
	cairo_t *cr;

	ٻار = gsk_cairo_ڳنڍ_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_ڳنڍ_get_draw_context(ٻار);

	cairo_سيٽ_سٽ_ويڪر(cr, 1.0);
	cairo_سيٽ_ذريعو_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_منزل(cr, -1, y - val[7] + 0.5);
	cairo_سٽ_منزل(cr, 0, y - val[0] + 0.5);

	لكل (صحيح i = 1; i < 8; ++i)
	    cairo_سٽ_منزل(cr, i, y - val[i] + 0.5);

	cairo_سٽ_منزل(cr, 8, y - val[0] + 0.5);
	cairo_سٽ_منزل(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#جيڪڏهن_پورو
	drow->underc_نقاب = gsk_repeat_ڳنڍ_new(
		&GRAPHENE_RECT_INIT(x_شروعات, y - 3, x_پڇاڙي - x_شروعات, 5),
		ٻار, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_ڳنڍ_unref(ٻار);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    ساكن فراغ
draw_صف_render_متن(Draw_صف *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_متن;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = عدم;
    صحيح		شروعات_کانو = 0;
    صحيح		خالي_جايون_cells = 0;
    uint32_t	cur_fg = 0;
    صحيح		c;

    إذا (!dlayer->dirty)
	أرجع;

    glyph_buf = da->glyph_buf;
    nodes = da->ڳنڍ_buf;

    g_array_سيٽ_سائيز(glyph_buf, 0);
    g_ptr_array_سيٽ_سائيز(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#وصف_ڪريو FLUSH_ڳنڍ() \
    افعل { \
	إذا (cur_font != عدم) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_ڳنڍ	*ڳنڍ; \
	    GdkRGBA		fg_color = u32_منزل_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->معلومات; \
	    glyph_str.انگ_glyphs = glyph_buf->len; \
	    ڳنڍ = gsk_متن_ڳنڍ_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_نقطو_INIT(متن_X(شروعات_کانو), \
			متن_Y(drow->صف))); \
	    إذا (ڳنڍ != عدم) \
		g_ptr_array_جوڙيو(nodes, ڳنڍ); \
	    g_array_سيٽ_سائيز(glyph_buf, 0); \
	    cur_font = عدم; \
	} \
    } طالما (FALSE)

    لكل (c = 0; c < da->n_کانن_ڳڻپ; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	صحيح	    prev_len;

	إذا (dglyphs == عدم)
	{
	    خالي_جايون_cells++;
	    استمر;
	}
#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	وإلا إذا (dglyphs->font == عدم)
	{
	    // Add sign icon
	    Gsk_Render_ڳنڍ   *snode;
	    صحيح		    w;

	    FLUSH_ڳنڍ();

	    snode = gsk_texture_scale_ڳنڍ_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->صف),
			dglyphs->dsign->ويڪر, dglyphs->dsign->اوچائي),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->ويڪر + gui.char_ويڪر - 1) / gui.char_ويڪر;

	    إذا (snode != عدم)
		g_ptr_array_جوڙيو(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    استمر;
	}
#جيڪڏهن_پورو
	وإلا إذا (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_ڳنڍ();
	    cur_font = dglyphs->font;
	    شروعات_کانو = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	إذا (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    استمر;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	إذا (خالي_جايون_cells > 0)
	{
	    إذا (prev_len > 0)
	    {
		// Spaces occurred between valid text: extend
		// previous character's advance
		PangoGlyphInfo *آخري = &g_array_اشاريو(
			glyph_buf, PangoGlyphInfo,
			prev_len - 1);
		آخري->geometry.ويڪر += CELLS2PANGO(خالي_جايون_cells);
	    }
	    خالي_جايون_cells = 0; // Width consumed
	}
    }

    FLUSH_ڳنڍ();

#وصف_رد FLUSH_ڳنڍ

    إذا (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_ڳنڍ *ذريعو;
	Gsk_Render_ڳنڍ *ڳنڍ;

	draw_صف_ensure_decor(drow, DRAW_UNDERL);
	إذا (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    ذريعو = draw_layer_get_texture(dlayer, drow->صف, da, 0, FALSE);
	    ڳنڍ = gsk_نقاب_ڳنڍ_new(ذريعو, drow->underl_نقاب,
		    GSK_نقاب_گهڻائي_ALPHA);
	    gsk_render_ڳنڍ_unref(ذريعو);
	    g_ptr_array_جوڙيو(nodes, ڳنڍ);
	}

	dlayer->ڳنڍ = gsk_container_ڳنڍ_new(
		(Gsk_Render_ڳنڍ **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    ساكن فراغ
draw_صف_render_special(Draw_صف *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_ڳنڍ   *ذريعو;
    Gsk_Render_ڳنڍ   *nodes[2];
    صحيح		    n_nodes = 0;

    إذا (!dlayer->dirty)
	أرجع;

    draw_صف_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    إذا (!rgba_is_transparent(dlayer->buf + UNDERC_وٿي(drow->n_cells),
		drow->n_cells))
    {
	ذريعو = draw_layer_get_texture(dlayer, drow->صف, da,
		UNDERC_وٿي(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_نقاب_ڳنڍ_new(ذريعو, drow->underc_نقاب,
		GSK_نقاب_گهڻائي_ALPHA);
	gsk_render_ڳنڍ_unref(ذريعو);
    }

    إذا (!rgba_is_transparent(dlayer->buf + STRIKE_وٿي(drow->n_cells),
		drow->n_cells))
    {
	ذريعو = draw_layer_get_texture(dlayer, drow->صف, da,
		STRIKE_وٿي(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_نقاب_ڳنڍ_new(ذريعو, drow->strike_نقاب,
		GSK_نقاب_گهڻائي_ALPHA);
	gsk_render_ڳنڍ_unref(ذريعو);
    }

    إذا (n_nodes == 1)
    {
	dlayer->ڳنڍ = nodes[0];
    }
    وإلا إذا (n_nodes > 0)
    {
	dlayer->ڳنڍ = gsk_container_ڳنڍ_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	لكل (صحيح i = 0; i < (صحيح)ARRAY_ڊيگهائي(nodes); i++)
	    gsk_render_ڳنڍ_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    ساكن فراغ
draw_صف_render_overlay(Draw_صف *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    إذا (!dlayer->dirty)
	أرجع;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    إذا (rgba_is_transparent(dlayer->buf, da->n_کانن_ڳڻپ))
	أرجع;

    dlayer->ڳنڍ = draw_layer_get_texture(dlayer, drow->صف, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    ساكن فراغ
draw_صف_جوڙيو_cluster(
	Draw_صف		*drow,
	DrawAttr	*dattr,
	صحيح		کانو,
	صحيح		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	صحيح		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = عدم;

    إذا (کانو + n_cells > da->n_کانن_ڳڻپ)
	أرجع;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    إذا (glyphs != عدم)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_صف_fill(drow, کانو, کانو + n_cells - 1, dglyphs, dattr, &da->cursor);

    إذا (dglyphs != عدم)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    ساكن صحيح
vim_draw_area_جوڙيو_string(
	VimDrawArea *self,
	Draw_صف	    *drow,
	DrawAttr    *dattr,
	صحيح	    کانو,
	char_u	    *s,
	صحيح	    len,
	صحيح	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = عدم;
    صحيح			شروعات_idx = -1;
    GList		*شي_list;
    صحيح			ڪالم_وٿي = 0;
    صحيح			cluster_ويڪر;
    صحيح			آخري_glyph_rbearing;
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
	    want_disable = (شروعات_idx != -1);
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
	    إذا (شروعات_idx == -1)
		شروعات_idx = i;
	}
	وإلا إذا (شروعات_idx != -1)
	{
	    إذا (attr_list == عدم)
		attr_list = pango_attr_list_new();
	    وجهو_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, شروعات_idx, i);
	    شروعات_idx = -1;
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

	    draw_صف_جوڙيو_cluster(drow, dattr, کانو + i, 1,
		    gui.ascii_font, has_ink ? &glyph : عدم, 1, self);
	}

	ڪالم_وٿي = len;
	اذهب ٻاهر_نڪرو;
    }

    glyphs = pango_glyph_string_new();

    إذا (attr_list == عدم)
	attr_list = pango_attr_list_new();

    إذا (شروعات_idx != -1)
	وجهو_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, شروعات_idx, len);

    cluster_ويڪر = PANGO_SCALE * gui.char_ويڪر;
    آخري_glyph_rbearing = PANGO_SCALE * gui.char_ويڪر;

    // If 'guifontwide' is set then use that for double-width characters.
    إذا (gui.wide_font != عدم)
	apply_wide_font_attr(s, len, attr_list);

    إذا ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	وجهو_PANGO_ATTR(pango_attr_وزن_new(PANGO_وزن_BOLD),
		attr_list, 0, len);
    إذا (draw_flags & DRAW_ITALIC)
	وجهو_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    شي_list = pango_itemize(gui.متن_context,
	    (ثابت حرف *)s, 0, len, attr_list, عدم);

    طالما (شي_list != عدم)
    {
	Pango_شي   *شي = شي_list->معلومات;
	صحيح	    شي_cells = 0;
	صحيح	    cluster_شروعات_idx = 0;
	صحيح	    current_cluster_کانو = کانو + ڪالم_وٿي;
	صحيح	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	شي_list = g_list_delete_link(شي_list, شي_list);

	// Force LTR direction; Vim handles bidi on its own.
	شي->analysis.level = (شي->analysis.level + 1) & (~1U);

	pango_shape_ڀريل((ثابت حرف *)s + شي->وٿي, شي->ڊيگهائي,
		(ثابت حرف *)s, len, &شي->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	لكل (i = 0; i < glyphs->انگ_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    إذا (glyph->attr.is_cluster_شروعات)
	    {
		صحيح cellcount;

		cellcount = ڳڻپ_cluster_cells(
			s, شي, glyphs, i, &cluster_ويڪر,
			(شي_list != عدم) ? &آخري_glyph_rbearing : عدم);

		إذا (i > 0 && cellcount > 0)
		{
		    draw_صف_جوڙيو_cluster(drow, dattr,
			    current_cluster_کانو, current_cluster_cells,
			    شي->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_شروعات_idx : عدم,
			    i - cluster_شروعات_idx, self);

		    current_cluster_کانو += current_cluster_cells;
		    cluster_شروعات_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		إذا (cellcount > 0)
		{
		    صحيح ويڪر;

		    ويڪر = cellcount * gui.char_ويڪر * PANGO_SCALE;
		    glyph->geometry.x_وٿي +=
			MAX(0, ويڪر - cluster_ويڪر) / 2;
		    glyph->geometry.ويڪر = ويڪر;
		}
		وإلا
		    تياري_zero_ويڪر_cluster(شي, glyph, cells,
			    cluster_ويڪر,
			    آخري_glyph_rbearing);

		شي_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    وإلا إذا (i > 0)
	    {
		صحيح ويڪر;

		إذا (glyph->geometry.x_وٿي >= 0)
		{
		    glyphs->glyphs[i].geometry.ويڪر =
			glyphs->glyphs[i - 1].geometry.ويڪر;
		    glyphs->glyphs[i - 1].geometry.ويڪر = 0;
		}
		ويڪر = cells * gui.char_ويڪر * PANGO_SCALE;
		glyph->geometry.x_وٿي +=
		    MAX(0, ويڪر - cluster_ويڪر) / 2;
	    }
	    وإلا
		glyph->geometry.ويڪر = 0;


	    إذا (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			شي->analysis.font, *glyph);
	}
	إذا (glyphs->انگ_glyphs > 0)
	    draw_صف_جوڙيو_cluster(drow, dattr,
		    current_cluster_کانو, current_cluster_cells,
		    شي->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_شروعات_idx : عدم,
		    glyphs->انگ_glyphs - cluster_شروعات_idx, self);

	pango_شي_free(شي);
	ڪالم_وٿي += شي_cells;
    }

    pango_glyph_string_free(glyphs);

ٻاهر_نڪرو:
    إذا (attr_list != عدم)
	pango_attr_list_unref(attr_list);

    أرجع ڪالم_وٿي;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    صحيح
vim_draw_area_draw_string(
	VimDrawArea *self,
	صحيح	    صف,
	صحيح	    کانو,
	char_u	    *s,
	صحيح	    len,
	صحيح	    flags)
{
    Draw_صف	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = عدم;
    صحيح		convlen;
    صحيح		n_cells ;

    إذا (unlikely(self->صفون_2 == عدم
		|| صف >= self->n_صفون
		|| کانو >= self->n_کانن_ڳڻپ))
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

    drow = self->صفون_2 + صف;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_جوڙيو_string(self, drow, &dattr, کانو, s, len, flags);

    vim_free(conv_buf);

    أرجع n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    فراغ
vim_draw_area_clear_بلاڪ(
	VimDrawArea	*self,
	صحيح		row1,
	صحيح		col1,
	صحيح		row2,
	صحيح		col2)
{
    إذا (unlikely(self->صفون_2 == عدم
		|| row1 >= self->n_صفون
		|| col1 >= self->n_کانن_ڳڻپ
		|| row2 >= self->n_صفون
		|| col2 >= self->n_کانن_ڳڻپ))
	أرجع;

    لكل (صحيح r = row1; r <= row2; r++)
	draw_صف_fill(self->صفون_2 + r, col1, col2, عدم, عدم, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    فراغ
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_بلاڪ(self, 0, 0, self->n_صفون - 1, self->n_کانن_ڳڻپ - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    فراغ
vim_draw_area_move_بلاڪ(
	VimDrawArea *self,
	صحيح	    منزل,
	صحيح	    row1,
	صحيح	    row2,
	صحيح	    col1,
	صحيح	    col2)
{
    صحيح وٿي = row2 - row1;

    إذا (unlikely(self->صفون_2 == عدم
		|| row1 >= self->n_صفون
		|| row2 >= self->n_صفون
		|| منزل >= self->n_صفون
		|| col1 >= self->n_کانن_ڳڻپ
		|| col2 >= self->n_کانن_ڳڻپ))
	أرجع;

    أكد(row2 >= row1);
    أكد(col2 >= col1);
    أكد(row1 != منزل);

    إذا (row1 > منزل)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	لكل (صحيح o = 0; o <= وٿي; o++)
	    draw_صف_move_منزل(self->صفون_2 + منزل + o, self->صفون_2 + row1 + o,
		    col1, col2, self);
    }
    وإلا
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	لكل (صحيح o = وٿي; o >= 0; o--)
	    إذا (منزل + o >= self->n_صفون)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_بلاڪ(self, row1 + o, col1, row1 + o, col2);
	    وإلا
		draw_صف_move_منزل(self->صفون_2 + منزل + o, self->صفون_2 + row1 + o,
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
vim_draw_area_سيٽ_cursor(VimDrawArea *self, صحيح w, صحيح h)
{
    self->cursor.draw = TRUE;
    self->cursor.ويڪر = w;
    self->cursor.اوچائي = h;
    // Remember this now: the snapshot runs later, when the drawing position
    // has moved on.
    self->cursor.n_cells = 1 + mb_lefthalve(gui.cursor_صف, gui.cursor_کانو);
    self->cursor.bg_color = *gui.bgcolor;
    self->cursor.fg_color = *gui.fgcolor;
}

/*
 * Invert the rectangle in the draw area.
 */
    فراغ
vim_draw_area_invert_بلاڪ(
	VimDrawArea	*self,
	صحيح		صف,
	صحيح		کانو,
	صحيح		nrows,
	صحيح		ncols)
{
    إذا (unlikely(self->صفون_2 == عدم
		|| صف >= self->n_صفون
		|| کانو >= self->n_کانن_ڳڻپ
		|| صف + nrows - 1 >= self->n_صفون
		|| کانو + ncols - 1 >= self->n_کانن_ڳڻپ))
	أرجع;

    لكل (صحيح r = صف; r < صف + nrows; r++)
    {
	Draw_صف	    *drow = self->صفون_2 + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	لكل (صحيح c = 0; c < ncols; c++)
	    dlayer->buf[c + کانو] = ~dlayer->buf[c + کانو];
	draw_صف_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#إذا defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    فراغ
vim_draw_area_جوڙيو_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	صحيح	    صف,
	صحيح	    کانو,
	صحيح	    ويڪر,
	صحيح	    اوچائي)
{
    Draw_صف	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحيح		cells;

    إذا (unlikely(self->صفون_2 == عدم
		|| صف >= self->n_صفون
		|| کانو >= self->n_کانن_ڳڻپ))
	أرجع;

    drow = self->صفون_2 + صف;
    dsign = draw_sign_new(sign, ويڪر, اوچائي);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (ويڪر + gui.char_ويڪر - 1) / gui.char_ويڪر;

    draw_صف_fill(drow, کانو, کانو + cells - 1, dglyphs, عدم, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    فراغ
vim_draw_area_جوڙيو_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	صحيح		صف,
	صحيح		کانو,
	صحيح		ويڪر,
	صحيح		اوچائي)
{
    Draw_صف	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحيح		cells;
    GdkTexture	*texture;
    GBytes	*بائيٽون;

    إذا (unlikely(self->صفون_2 == عدم
		|| صف >= self->n_صفون
		|| کانو >= self->n_کانن_ڳڻپ))
	أرجع;

    cairo_surface_flush(surf);
    بائيٽون = g_بائيٽون_new_with_free_func(cairo_image_surface_get_معلومات(surf),
	    cairo_image_surface_get_اوچائي(surf)
	    * cairo_image_surface_get_stride(surf),
	    (GDestroyNotify)cairo_surface_destroy,
	    cairo_surface_reference(surf));

    texture = gdk_يادگيري_texture_new(cairo_image_surface_get_ويڪر(surf),
	    cairo_image_surface_get_اوچائي(surf),
	    GDK_يادگيري_A8_R8_G8_B8_PREMULTIPLIED,
	    بائيٽون, cairo_image_surface_get_stride(surf));
    g_بائيٽون_unref(بائيٽون);

    drow = self->صفون_2 + صف;
    dsign = draw_sign_new(texture, ويڪر, اوچائي);
    g_object_unref(texture);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (ويڪر + gui.char_ويڪر - 1) / gui.char_ويڪر;

    draw_صف_fill(drow, کانو, کانو + cells - 1, dglyphs, عدم, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    فراغ
vim_draw_area_جوڙيو_external(VimDrawArea *self, Gsk_Render_ڳنڍ *ڳنڍ)
{
    g_hash_جدول_جوڙيو(self->external, gsk_render_ڳنڍ_ref(ڳنڍ));
}

    فراغ
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_ڳنڍ *ڳنڍ)
{
    g_hash_جدول_remove(self->external, ڳنڍ);
}

#جيڪڏهن_پورو

/*
 * Add the cursor to the snapshot.
 */
    ساكن فراغ
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_ڳنڍ	*متن_ڳنڍ,	// May be NULL
	Gsk_Render_ڳنڍ	*special_ڳنڍ,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    صحيح		    w = cursor->ويڪر;
    صحيح		    h = cursor->اوچائي;
    graphene_rect_t rect;

    إذا (!cursor->draw)
	أرجع;

    إذا (cursor->ويڪر <= 0 && cursor->اوچائي <= 0)
    {
	w += gui.char_ويڪر * cursor->n_cells;
	h = gui.char_اوچائي;
    }

    graphene_rect_init(&rect,
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	    CURSOR_BAR_ساڄو ? FILL_X(gui.cursor_کانو + 1) - w :
#جيڪڏهن_پورو
	    FILL_X(gui.cursor_کانو),
	    FILL_Y(gui.cursor_صف) + gui.char_اوچائي - h, w, h);


    إذا (cursor->ويڪر == -1 && cursor->اوچائي == -1)
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

	gtk_snapshot_ڌڪو_نقاب(snapshot, GSK_نقاب_گهڻائي_ALPHA);

	// Used as an "outline" for the fg color.
	إذا (متن_ڳنڍ != عدم)
	    gtk_snapshot_append_ڳنڍ(snapshot, متن_ڳنڍ);
	إذا (special_ڳنڍ != عدم)
	    gtk_snapshot_append_ڳنڍ(snapshot, special_ڳنڍ);

	gtk_snapshot_ڪڍو(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_ڪڍو(snapshot);
    }
}

    ساكن فراغ
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    صحيح		    اوچائي, ويڪر;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = عدم;
    Gsk_Render_ڳنڍ   *body_ڳنڍ;
    Gsk_Render_ڳنڍ   *invert_ڳنڍ = عدم;
#جيڪڏهن_وصف_ٿيل FEAT_IMAGE_GUI
    Gsk_Render_ڳنڍ   *ext_ڳنڍ;
    GHash_جدول_Iter  iter;
#جيڪڏهن_پورو

    gui_mch_سيٽ_bg_color(gui.پٺ_pixel);
    اوچائي = gtk_widget_get_اوچائي(widget) + gui.bleed_bot;
    ويڪر = gtk_widget_get_ويڪر(widget) + gui.bleed_ساڄو;

    إذا (self->صفون_2 == عدم)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, ويڪر, اوچائي));
	أرجع;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    إذا (self->bleed_ساڄو == -1 || self->bleed_ساڄو != gui.bleed_ساڄو)
    {
	self->bleed_ساڄو = gui.bleed_ساڄو;
	لكل (صحيح r = 0; r < self->n_صفون; r++)
	{
	    draw_صف_dirty_layer(self->صفون_2 + r, DRAW_LAYER_BACKGROUND);
	    draw_صف_dirty_layer(self->صفون_2 + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#إذا GTK_CHECK_نسخو(4,24,0)
    gtk_snapshot_سيٽ_snap(body_snapshot, GSK_RECT_SNAP_ڦيرو);
#جيڪڏهن_پورو

    لكل (صحيح r = 0; r < self->n_صفون; r++)
    {
	Draw_صف *drow = self->صفون_2 + r;

	draw_صف_render_background(drow, self);
	draw_صف_render_متن(drow, self);
	draw_صف_render_special(drow, self);
	draw_صف_render_overlay(drow, self);

	لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    إذا (dlayer->ڳنڍ != عدم)
	    {
		إذا (l == DRAW_LAYER_OVERLAY)
		{
		    إذا (invert_snapshot == عدم)
		    {
			invert_snapshot = gtk_snapshot_new();
#إذا GTK_CHECK_نسخو(4,24,0)
			gtk_snapshot_سيٽ_snap(invert_snapshot,
				GSK_RECT_SNAP_ڦيرو);
#جيڪڏهن_پورو
		    }
		    gtk_snapshot_append_ڳنڍ(invert_snapshot, dlayer->ڳنڍ);
		}
		وإلا
		    gtk_snapshot_append_ڳنڍ(body_snapshot, dlayer->ڳنڍ);
	    }
	}

	إذا (r == gui.cursor_صف)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_متن].ڳنڍ,
		    drow->dlayers[DRAW_LAYER_SPECIAL].ڳنڍ,
		    body_snapshot);
    }

    إذا (invert_snapshot != عدم)
	gtk_snapshot_ڌڪو_blend(snapshot, GSK_BLEND_گهڻائي_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, ويڪر, اوچائي));
    body_ڳنڍ = gtk_snapshot_free_منزل_ڳنڍ(body_snapshot);
    إذا (body_ڳنڍ != عدم)
    {
	gtk_snapshot_append_ڳنڍ(snapshot, body_ڳنڍ);
	gsk_render_ڳنڍ_unref(body_ڳنڍ);
    }

#جيڪڏهن_وصف_ٿيل FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_جدول_iter_init(&iter, self->external);

    طالما (g_hash_جدول_iter_ايندڙ(&iter, (فراغ **)&ext_ڳنڍ, عدم))
	gtk_snapshot_append_ڳنڍ(snapshot, ext_ڳنڍ);
#جيڪڏهن_پورو

    إذا (invert_snapshot != عدم)
    {
	gtk_snapshot_ڪڍو(snapshot);
	invert_ڳنڍ = gtk_snapshot_free_منزل_ڳنڍ(invert_snapshot);
	// Not sure if it can be NULL but still check
	إذا (invert_ڳنڍ != عدم)
	{
	    gtk_snapshot_append_ڳنڍ(snapshot, invert_ڳنڍ);
	    gsk_render_ڳنڍ_unref(invert_ڳنڍ);
	}
	gtk_snapshot_ڪڍو(snapshot);
    }
}
