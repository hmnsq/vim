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
قسمتعریف ساخت
{
    GdkTexture	*texture;
    صحیح		چوڑائی;
    صحیح		اونچائی;
} DrawSign;
#اگر_ختم

/*
 * Temporary struct when drawing string representing visual attributes of a cell
 */
قسمتعریف ساخت
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
قسمتعریف ساخت
{
    صحیح refcount;

    uint32_t	fg_color;
    PangoFont	*font;
    // If "font" is NULL, "dsign" is used, otherwise "glyphs" is used.
    اتحاد
    {
	PangoGlyphInfo  *glyphs; // May be NULL
#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	DrawSign	*dsign;
#اگر_ختم
    };
    صحیح n_glyphs;
} DrawGlyphs;

قسمتعریف ساخت
{
    DrawGlyphs *dglyphs;
} DrawCell;

قسمتعریف شمار
{
    DRAW_LAYER_BACKGROUND,  // Background color, buf stores bg color
    DRAW_LAYER_متن,	    // Text glyphs + underline, buf stores fg color (for
			    // underline)
    DRAW_LAYER_SPECIAL,	    // Undercurls and strikethrough, buf store special
			    // color
    DRAW_LAYER_OVERLAY,	    // Invert blend, buf stores white color
    N_DRAW_LAYERS
} DrawLayerType;

قسمتعریف ساخت
{
    Gsk_Render_گرہ   *گرہ;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#تعریف_کرو UNDERC_انحراف(n_ستون_تعداد) (0)
#تعریف_کرو STRIKE_انحراف(n_ستون_تعداد) (n_ستون_تعداد)

قسمتعریف ساخت
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    صحیح		n_cells; // For convenience
    صحیح		صف;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_گرہ *underl_نقاب;
    Gsk_Render_گرہ *underc_نقاب;
    Gsk_Render_گرہ *strike_نقاب;
} Draw_صف;

#تعریف_کرو داخل_کرو_PANGO_ATTR(Attribute, AttrList, آغاز_2, انتہا_2) \
    کرو { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->آغاز_اشاریہ = (آغاز_2); \
	tmp_attr_->انتہا_اشاریہ = (انتہا_2); \
	pango_attr_list_داخل_کرو((AttrList), tmp_attr_); \
    } جب (FALSE)
#تعریف_کرو CELLS2PANGO(c) ((c) * gui.char_چوڑائی * PANGO_SCALE)

/*
 * Attributes of cursor
 */
قسمتعریف ساخت
{
    gboolean draw; // If cursor should be drawn
    صحیح چوڑائی;
    صحیح اونچائی;
    صحیح n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

ساخت _VimDrawArea
{
    GtkWidget والد;

    Draw_صف *صفیں_2;

    صحیح n_صفیں;
    صحیح	n_ستون_تعداد;

    صحیح	bleed_دایاں;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*گرہ_buf;

    DrawCursor cursor;

#اگر_تعریف_شدہ FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHash_جدول *external;
#اگر_ختم
};

ساکن خالی draw_صف_init(Draw_صف *drow, صحیح صف, صحیح ستون_تعداد);
ساکن خالی draw_صف_clear(Draw_صف *drow);
ساکن خالی draw_صف_dirty_layer(Draw_صف *drow, DrawLayerType dlayer_t);
ساکن خالی vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

ساکن GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    ساکن خالی
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    برائے (صحیح r = 0; r < self->n_صفیں; r++)
	draw_صف_clear(self->صفیں_2 + r);
    g_free(self->صفیں_2);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->گرہ_buf, TRUE);

#اگر_تعریف_شدہ FEAT_IMAGE_GUI
    g_hash_جدول_unref(self->external);
#اگر_ختم

    G_OBJECT_CLASS(vim_draw_area_والد_class)->finalize(obj);
}

    ساکن خالی
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_سیٹ_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    ساکن خالی
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_دایاں = -1;
#اگر_تعریف_شدہ FEAT_IMAGE_GUI
    self->external = g_hash_جدول_new_بھرا(g_براہ_راست_hash, g_براہ_راست_equal,
	    تہی, (GDestroyNotify)gsk_render_گرہ_unref);
#اگر_ختم
    self->glyph_buf = g_array_new(FALSE, FALSE, حجم(PangoGlyphInfo));
    self->گرہ_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_گرہ_unref);

}

    GtkWidget *
vim_draw_area_new(خالی)
{
    واپس g_object_new(VIM_TYPE_DRAW_AREA, تہی);
}

/*
 * Set the size of the draw area to "rows" and "cols".
 */
    خالی
vim_draw_area_سیٹ_سائز(VimDrawArea *self, صحیح صفیں_2, صحیح ستون_تعداد)
{
    اگر (self->صفیں_2 != تہی && self->n_صفیں == صفیں_2 && self->n_ستون_تعداد == ستون_تعداد)
	واپس;
    اگر (صفیں_2 == 0 || ستون_تعداد == 0)
	واپس;

    vim_draw_area_clear(self);

    برائے (صحیح r = 0; r < self->n_صفیں; r++)
	draw_صف_clear(self->صفیں_2 + r);
    self->صفیں_2 = g_realloc_n(self->صفیں_2, صفیں_2, حجم(Draw_صف));
    برائے (صحیح r = 0; r < صفیں_2; r++)
	draw_صف_init(self->صفیں_2 + r, r, ستون_تعداد);

    self->n_صفیں = صفیں_2;
    self->n_ستون_تعداد = ستون_تعداد;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    ساکن uint32_t
rgba_مقصد_u32(ثابت GdkRGBA *rgba)
{
    اتحاد { uint32_t val; uint8_t بائٹیں[4]; } color;

    color.بائٹیں[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.بائٹیں[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.بائٹیں[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.بائٹیں[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    واپس color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    ساکن GdkRGBA
u32_مقصد_rgba(uint32_t val)
{
    اتحاد { uint32_t val; uint8_t بائٹیں[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.بائٹیں[0] / 255.0;
    color.green = packed.بائٹیں[1] / 255.0;
    color.blue  = packed.بائٹیں[2] / 255.0;
    color.alpha = packed.بائٹیں[3] / 255.0;
    واپس color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    ساکن gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    اتحاد { uint32_t val; uint8_t بائٹیں[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.بائٹیں[0] << 16) |
	((guicolor_T)packed.بائٹیں[1] << 8)  |
	packed.بائٹیں[2];
    واپس res == rgb;
}

/*
 * Checks if a RGBA pixel buffer is fully transparent.
 */
    ساکن اندرونی gboolean
rgba_is_transparent(ثابت uint32_t *buf, صحیح n_pixels)
{
    // Could also compare 2 pixels at once using a 64 bit integer, but not sure
    // if that might cause alignment issues...
    برائے (صحیح i = 0; i < n_pixels; i++)
	اگر (buf[i] != 0)
	    واپس FALSE;
    واپس TRUE;
}

/*
 * Return TRUE if "glyph" has ink (not whitespace).
 */
    ساکن gboolean
glyph_has_ink(PangoFont *font, PangoGlyphInfo glyph)
{
    PangoRectangle ink;

    pango_font_get_glyph_extents (font, glyph.glyph, &ink, تہی);

    واپس ink.چوڑائی > 0 && ink.اونچائی > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    ساکن خالی
apply_wide_font_attr(char_u *s, صحیح len, PangoAttrList *attr_list)
{
    char_u  *آغاز = تہی;
    char_u  *p;
    صحیح	    uc;

    برائے (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	اگر (آغاز == تہی)
	{
	    اگر (uc >= 0x80 && utf_char2cells(uc) == 2)
		آغاز = p;
	}
	ورنہ اگر (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    داخل_کرو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, آغاز - s, p - s);
	    آغاز = تہی;
	}
    }

    اگر (آغاز != تہی)
	داخل_کرو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, آغاز - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    ساکن صحیح
گنتی_cluster_cells(
	char_u		    *s,
	Pango_شے	    *شے,
	PangoGlyphString    *glyphs,
	صحیح		    i,
	صحیح		    *cluster_چوڑائی,
	صحیح		    *آخری_glyph_rbearing)
{
    char_u  *p;
    صحیح	    اگلا;
    صحیح	    آغاز, انتہا;
    صحیح	    چوڑائی;
    صحیح	    uc;
    صحیح	    cellcount = 0;

    چوڑائی = glyphs->glyphs[i].geometry.چوڑائی;

    برائے (اگلا = i + 1; اگلا < glyphs->عدد_glyphs; ++اگلا)
    {
	اگر (glyphs->glyphs[اگلا].attr.is_cluster_آغاز)
	    توڑو;
	ورنہ اگر (glyphs->glyphs[اگلا].geometry.چوڑائی > چوڑائی)
	    چوڑائی = glyphs->glyphs[اگلا].geometry.چوڑائی;
    }

    آغاز = شے->انحراف + glyphs->log_clusters[i];
    انتہا   = شے->انحراف + ((اگلا < glyphs->عدد_glyphs) ?
	    glyphs->log_clusters[اگلا] : شے->لمبائی);

    برائے (p = s + آغاز; p < s + انتہا; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	اگر (uc < 0x80)
	    ++cellcount;
	ورنہ اگر (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    اگر (آخری_glyph_rbearing != تہی
	    && cellcount > 0 && اگلا == glyphs->عدد_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(شے->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, تہی);

	اگر (PANGO_RBEARING(ink_rect) > 0)
	    *آخری_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    اگر (cellcount > 0)
	*cluster_چوڑائی = چوڑائی;

    واپس cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    ساکن خالی
تیاری_zero_چوڑائی_cluster(
	Pango_شے	*شے,
	PangoGlyphInfo	*glyph,
	صحیح		آخری_cellcount,
	صحیح		آخری_cluster_چوڑائی,
	صحیح		آخری_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحیح		    چوڑائی;

    چوڑائی = آخری_cellcount * gui.char_چوڑائی * PANGO_SCALE;
    glyph->geometry.x_انحراف = -چوڑائی + MAX(0, چوڑائی - آخری_cluster_چوڑائی) / 2;
    glyph->geometry.چوڑائی = 0;

    pango_font_get_glyph_extents(شے->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    اگر (ink_rect.x < 0)
    {
	glyph->geometry.x_انحراف += آخری_glyph_rbearing;
	glyph->geometry.y_انحراف  = logical_rect.اونچائی
	    - (gui.char_اونچائی - p_linespace) * PANGO_SCALE;
    }
    ورنہ
	glyph->geometry.x_انحراف = -چوڑائی + MAX(0, چوڑائی - ink_rect.چوڑائی) / 2;
}

#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    ساکن DrawSign *
draw_sign_new(GdkTexture *texture, صحیح چوڑائی, صحیح اونچائی)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->چوڑائی = چوڑائی;
    dsign->اونچائی = اونچائی;

    واپس dsign;
}

    ساکن خالی
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#اگر_ختم

    ساکن خالی
draw_attr_init(
	DrawAttr	*dattr,
	ثابت GdkRGBA	*bg_color,
	ثابت GdkRGBA	*fg_color,
	ثابت GdkRGBA	*sp_color,
	صحیح		draw_flags)
{
    dattr->bg_color = rgba_مقصد_u32(bg_color);
    dattr->fg_color = rgba_مقصد_u32(fg_color);
    dattr->sp_color = rgba_مقصد_u32(sp_color);
    dattr->draw_flags = draw_flags & (DRAW_UNDERL | DRAW_UNDERC | DRAW_STRIKE);
}

/*
 * Apply the draw attr to the row for the given col/cell. Only dirty layers that
 * have actually changed.
 */
    ساکن خالی
draw_attr_apply(DrawAttr *dattr, صحیح ستون, Draw_صف *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + ستون;
    uint32_t	*متن_cell = drow->dlayers[DRAW_LAYER_متن].buf + ستون;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    صحیح		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	ستون + UNDERC_انحراف(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	ستون + STRIKE_انحراف(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    اگر (dattr != تہی)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.عقب_pixel, bg_color);
    }

    اگر (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_صف_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

	// If "bg_color" is same as global background color, then use
	// transparent color.
	اگر (bg_is_default)
	    *bg_cell = 0;
	ورنہ
	    *bg_cell = bg_color;
    }
    اگر ((draw_flags & DRAW_UNDERL && *متن_cell != fg_color)
	    || (!(draw_flags & DRAW_UNDERL) && *متن_cell != 0))
    {
	draw_صف_dirty_layer(drow, DRAW_LAYER_متن);
	اگر (draw_flags & DRAW_UNDERL)
	    *متن_cell = fg_color;
	ورنہ
	    *متن_cell = 0;
    }
    اگر ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_صف_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	اگر (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	ورنہ
	    *underc_cell = 0;
    }
    اگر ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_صف_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	اگر (draw_flags & DRAW_STRIKE)
	    *strike_cell = sp_color;
	ورنہ
	    *strike_cell = 0;
    }
}

    ساکن DrawGlyphs *
draw_glyphs_new(
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	صحیح		n_glyphs,
	uint32_t	fg_color)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->font = g_object_ref(font);
    dglyphs->glyphs = g_memdup2(glyphs, حجم(*glyphs) * n_glyphs);
    dglyphs->n_glyphs = n_glyphs;
    dglyphs->fg_color = fg_color;

    واپس dglyphs;
}

#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw glyphs that shows the given sign. Note that ownership of
 * "dsign" is taken.
 */
    ساکن DrawGlyphs *
draw_glyphs_new_sign(DrawSign *dsign)
{
    DrawGlyphs *dglyphs = g_new0(DrawGlyphs, 1);

    dglyphs->refcount = 1;
    dglyphs->dsign = dsign;

    واپس dglyphs;
}
#اگر_ختم

    ساکن DrawGlyphs *
draw_glyphs_ref(DrawGlyphs *dglyphs)
{
    dglyphs->refcount++;
    واپس dglyphs;
}

    ساکن خالی
draw_glyphs_unref(DrawGlyphs *dglyphs)
{
    اگر (--dglyphs->refcount <= 0)
    {
#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	اگر (dglyphs->font == تہی)
	    draw_sign_free(dglyphs->dsign);
	ورنہ
#اگر_ختم
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
    ساکن gboolean
draw_glyphs_same(DrawGlyphs *a, DrawGlyphs *b)
{
    اگر (a == تہی && b == تہی)
	واپس TRUE;
    اگر (a == تہی || b == تہی)
	واپس FALSE;
    اگر (a->fg_color != b->fg_color)
	واپس FALSE;
    اگر (a->n_glyphs != b->n_glyphs || a->font != b->font)
	واپس FALSE;
    برائے (صحیح i = 0; i < a->n_glyphs; i++)
	اگر (a->glyphs[i].glyph != b->glyphs[i].glyph
		|| a->glyphs[i].geometry.چوڑائی != b->glyphs[i].geometry.چوڑائی
		|| a->glyphs[i].geometry.x_انحراف != b->glyphs[i].geometry.x_انحراف
		|| a->glyphs[i].geometry.y_انحراف != b->glyphs[i].geometry.y_انحراف
		|| a->glyphs[i].attr.is_cluster_آغاز != b->glyphs[i].attr.is_cluster_آغاز)
	    واپس FALSE;
    واپس TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    ساکن Gsk_Render_گرہ *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	صحیح	    صف,
	VimDrawArea *da,
	صحیح	    off,
	gboolean    bleed)
{
    GBytes	    *بائٹیں;
    GdkTexture	    *texture;
    Gsk_Render_گرہ   *گرہ;
    صحیح		    اونچائی;

#اگر GTK_CHECK_نسخہ(4,24,0)
    اونچائی = gui.char_اونچائی;
#ورنہ
    اونچائی = gui.char_اونچائی + 1;
#اگر_ختم

    اگر (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		حجم(uint32_t) * da->n_ستون_تعداد);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_ستون_تعداد - 1)),
		حجم(uint32_t)); // Add bleed using last pixel of buffer
	بائٹیں = g_byte_array_free_مقصد_بائٹیں(arr);
    }
    ورنہ
	بائٹیں = g_بائٹیں_new(dlayer->buf + off, حجم(uint32_t) * da->n_ستون_تعداد);

    texture = gdk_حافظہ_texture_new(da->n_ستون_تعداد + bleed, 1,
	    GDK_حافظہ_R8_G8_B8_A8, بائٹیں, (da->n_ستون_تعداد + bleed) * 4);
    g_بائٹیں_unref(بائٹیں);

    // Scale texture to actual size
    گرہ = gsk_texture_scale_گرہ_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(صف),
		(da->n_ستون_تعداد + bleed) * gui.char_چوڑائی, gui.char_اونچائی + bleed),
	    GSK_SCALING_FILTER_قریب_ترین);
    اگر (bleed)
    {
	Gsk_Render_گرہ *new;

	new = gsk_clip_گرہ_new(گرہ,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(صف),
		    da->n_ستون_تعداد * gui.char_چوڑائی + da->bleed_دایاں, اونچائی));
	gsk_render_گرہ_unref(گرہ);
	گرہ = new;
    }
    g_object_unref(texture);
    واپس گرہ;
}

    ساکن خالی
draw_صف_init(Draw_صف *drow, صحیح صف, صحیح ستون_تعداد)
{
    drow->cells = g_malloc0_n(ستون_تعداد, حجم(DrawCell));
    drow->n_cells = ستون_تعداد;
    drow->صف = صف;
    drow->underl_نقاب = drow->underc_نقاب = drow->strike_نقاب = تہی;

    memset(drow->dlayers, 0, حجم(DrawLayer) * N_DRAW_LAYERS);
    برائے (صحیح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	اگر (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(ستون_تعداد * 2, 4);
	ورنہ
	    dlayer->buf = g_malloc0_n(ستون_تعداد, 4);
    }
}

    ساکن خالی
draw_صف_clear(Draw_صف *drow)
{
    g_free(drow->cells);
    برائے (صحیح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	اگر (dlayer->گرہ != تہی)
	    gsk_render_گرہ_unref(dlayer->گرہ);
	g_free(dlayer->buf);
    }
    g_clear_اشارہ_گر(&drow->underl_نقاب, gsk_render_گرہ_unref);
    g_clear_اشارہ_گر(&drow->underc_نقاب, gsk_render_گرہ_unref);
    g_clear_اشارہ_گر(&drow->strike_نقاب, gsk_render_گرہ_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    ساکن خالی
draw_صف_dirty_layer(Draw_صف *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_اشارہ_گر(&dlayer->گرہ, gsk_render_گرہ_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    ساکن خالی
draw_صف_uninvert_cell(Draw_صف *drow, صحیح ستون)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    اگر (buf[ستون] == 0xffffffff)
    {
	draw_صف_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[ستون] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    ساکن خالی
draw_صف_سیٹ_cell(Draw_صف *drow, صحیح ستون, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + ستون;

    اگر (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_صف_dirty_layer(drow, DRAW_LAYER_متن);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    اگر (dcell->dglyphs != dglyphs)
    {
	اگر (dcell->dglyphs != تہی)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == تہی ? تہی : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, ستون, drow);
    draw_صف_uninvert_cell(drow, ستون);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    ساکن خالی
draw_صف_fill(
	Draw_صف	    *drow,
	صحیح	    col1,
	صحیح	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    برائے (صحیح c = col1; c <= col2; c++)
	draw_صف_سیٹ_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    اگر (gui.cursor_صف == drow->صف && gui.cursor_ستون >= col1
	    && gui.cursor_ستون <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    ساکن خالی
draw_صف_move_مقصد(
	Draw_صف	    *dest_صف,
	Draw_صف	    *src_صف,
	صحیح	    col1,
	صحیح	    col2,
	VimDrawArea *da)
{
    صحیح move_سائز = (col2 - col1 + 1) * حجم(DrawCell);

    draw_صف_fill(dest_صف, col1, col2, تہی, تہی, &da->cursor);
    memmove(dest_صف->cells + col1, src_صف->cells + col1, move_سائز);
    // NULL the draw cells so we don't double unreference.
    memset(src_صف->cells + col1, 0, move_سائز);
    draw_صف_dirty_layer(src_صف, DRAW_LAYER_متن);

    // Move the layer buffers
    move_سائز = (col2 - col1 + 1) * 4;
    برائے (صحیح l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_صف->dlayers[l].buf;
	uint32_t *dest = dest_صف->dlayers[l].buf;

	اگر (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_انحراف(dest_صف->n_cells),
		    src + col1 + UNDERC_انحراف(src_صف->n_cells), move_سائز);
	    memmove(dest + col1 + STRIKE_انحراف(dest_صف->n_cells),
		    src + col1 + STRIKE_انحراف(src_صف->n_cells), move_سائز);
	}
	ورنہ
	    memmove(dest + col1, src + col1, move_سائز);
	draw_صف_dirty_layer(dest_صف, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    ساکن خالی
draw_صف_render_background(Draw_صف *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    اگر (!dlayer->dirty)
	واپس;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    اگر (rgba_is_transparent(dlayer->buf, da->n_ستون_تعداد))
	واپس;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->گرہ = draw_layer_get_texture(dlayer, drow->صف, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    ساکن خالی
draw_صف_ensure_decor(Draw_صف *drow, صحیح flags)
{
    اگر (flags & DRAW_UNDERL && drow->underl_نقاب == تہی)
	drow->underl_نقاب = gsk_color_گرہ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->صف + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    اگر (flags & DRAW_STRIKE && drow->strike_نقاب == تہی)
	drow->strike_نقاب = gsk_color_گرہ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->صف) + (صحیح)(gui.char_اونچائی / 2),
		    FILL_X(drow->n_cells), 1));

    اگر (flags & DRAW_UNDERC && drow->underc_نقاب == تہی)
    {
	ساکن ثابت صحیح val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	صحیح y = FILL_Y(drow->صف + 1) - 1;
	صحیح x_آغاز = FILL_X(0);
	صحیح x_انتہا = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_گرہ *بچہ = تہی;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#اگر GTK_CHECK_نسخہ(4, 14, 0)
	Gsk_راستہ_Builder	*builder;
	Gsk_راستہ		*راستہ;
	GskStroke       *stroke;
	Gsk_Render_گرہ	*color_گرہ;
	graphene_rect_t bounds;

	builder = gsk_راستہ_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_راستہ_builder_move_مقصد(builder, -1, y - val[7] + 0.5);
	gsk_راستہ_builder_سطر_مقصد(builder, 0, y - val[0] + 0.5);

	برائے (صحیح i = 1; i < 8; i++)
	    gsk_راستہ_builder_سطر_مقصد(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_راستہ_builder_سطر_مقصد(builder, 8, y - val[0] + 0.5);
	gsk_راستہ_builder_سطر_مقصد(builder, 9, y - val[1] + 0.5);

	راستہ = gsk_راستہ_builder_free_مقصد_راستہ(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_راستہ_get_stroke_bounds(راستہ, stroke, &bounds);
	color_گرہ = gsk_color_گرہ_new(&white_rgba, &bounds);
	بچہ = gsk_stroke_گرہ_new(color_گرہ, راستہ, stroke);

	gsk_stroke_free(stroke);
	gsk_راستہ_unref(راستہ);
	gsk_render_گرہ_unref(color_گرہ);
#ورنہ
	cairo_t *cr;

	بچہ = gsk_cairo_گرہ_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_گرہ_get_draw_context(بچہ);

	cairo_سیٹ_سطر_چوڑائی(cr, 1.0);
	cairo_سیٹ_ماخذ_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_مقصد(cr, -1, y - val[7] + 0.5);
	cairo_سطر_مقصد(cr, 0, y - val[0] + 0.5);

	برائے (صحیح i = 1; i < 8; ++i)
	    cairo_سطر_مقصد(cr, i, y - val[i] + 0.5);

	cairo_سطر_مقصد(cr, 8, y - val[0] + 0.5);
	cairo_سطر_مقصد(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#اگر_ختم
	drow->underc_نقاب = gsk_repeat_گرہ_new(
		&GRAPHENE_RECT_INIT(x_آغاز, y - 3, x_انتہا - x_آغاز, 5),
		بچہ, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_گرہ_unref(بچہ);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    ساکن خالی
draw_صف_render_متن(Draw_صف *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_متن;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = تہی;
    صحیح		آغاز_ستون = 0;
    صحیح		خالی_خانے_cells = 0;
    uint32_t	cur_fg = 0;
    صحیح		c;

    اگر (!dlayer->dirty)
	واپس;

    glyph_buf = da->glyph_buf;
    nodes = da->گرہ_buf;

    g_array_سیٹ_سائز(glyph_buf, 0);
    g_ptr_array_سیٹ_سائز(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#تعریف_کرو FLUSH_گرہ() \
    کرو { \
	اگر (cur_font != تہی) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_گرہ	*گرہ; \
	    GdkRGBA		fg_color = u32_مقصد_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->ڈیٹا; \
	    glyph_str.عدد_glyphs = glyph_buf->len; \
	    گرہ = gsk_متن_گرہ_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_نقطہ_INIT(متن_X(آغاز_ستون), \
			متن_Y(drow->صف))); \
	    اگر (گرہ != تہی) \
		g_ptr_array_جوڑو(nodes, گرہ); \
	    g_array_سیٹ_سائز(glyph_buf, 0); \
	    cur_font = تہی; \
	} \
    } جب (FALSE)

    برائے (c = 0; c < da->n_ستون_تعداد; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	صحیح	    پچھلا_len;

	اگر (dglyphs == تہی)
	{
	    خالی_خانے_cells++;
	    جاری;
	}
#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	ورنہ اگر (dglyphs->font == تہی)
	{
	    // Add sign icon
	    Gsk_Render_گرہ   *snode;
	    صحیح		    w;

	    FLUSH_گرہ();

	    snode = gsk_texture_scale_گرہ_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->صف),
			dglyphs->dsign->چوڑائی, dglyphs->dsign->اونچائی),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->چوڑائی + gui.char_چوڑائی - 1) / gui.char_چوڑائی;

	    اگر (snode != تہی)
		g_ptr_array_جوڑو(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    جاری;
	}
#اگر_ختم
	ورنہ اگر (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_گرہ();
	    cur_font = dglyphs->font;
	    آغاز_ستون = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	اگر (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    جاری;

	پچھلا_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	اگر (خالی_خانے_cells > 0)
	{
	    اگر (پچھلا_len > 0)
	    {
		// Spaces occurred between valid text: extend
		// previous character's advance
		PangoGlyphInfo *آخری = &g_array_اشاریہ(
			glyph_buf, PangoGlyphInfo,
			پچھلا_len - 1);
		آخری->geometry.چوڑائی += CELLS2PANGO(خالی_خانے_cells);
	    }
	    خالی_خانے_cells = 0; // Width consumed
	}
    }

    FLUSH_گرہ();

#تعریف_منسوخ FLUSH_گرہ

    اگر (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_گرہ *ماخذ;
	Gsk_Render_گرہ *گرہ;

	draw_صف_ensure_decor(drow, DRAW_UNDERL);
	اگر (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    ماخذ = draw_layer_get_texture(dlayer, drow->صف, da, 0, FALSE);
	    گرہ = gsk_نقاب_گرہ_new(ماخذ, drow->underl_نقاب,
		    GSK_نقاب_منوال_ALPHA);
	    gsk_render_گرہ_unref(ماخذ);
	    g_ptr_array_جوڑو(nodes, گرہ);
	}

	dlayer->گرہ = gsk_container_گرہ_new(
		(Gsk_Render_گرہ **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    ساکن خالی
draw_صف_render_special(Draw_صف *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_گرہ   *ماخذ;
    Gsk_Render_گرہ   *nodes[2];
    صحیح		    n_nodes = 0;

    اگر (!dlayer->dirty)
	واپس;

    draw_صف_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    اگر (!rgba_is_transparent(dlayer->buf + UNDERC_انحراف(drow->n_cells),
		drow->n_cells))
    {
	ماخذ = draw_layer_get_texture(dlayer, drow->صف, da,
		UNDERC_انحراف(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_نقاب_گرہ_new(ماخذ, drow->underc_نقاب,
		GSK_نقاب_منوال_ALPHA);
	gsk_render_گرہ_unref(ماخذ);
    }

    اگر (!rgba_is_transparent(dlayer->buf + STRIKE_انحراف(drow->n_cells),
		drow->n_cells))
    {
	ماخذ = draw_layer_get_texture(dlayer, drow->صف, da,
		STRIKE_انحراف(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_نقاب_گرہ_new(ماخذ, drow->strike_نقاب,
		GSK_نقاب_منوال_ALPHA);
	gsk_render_گرہ_unref(ماخذ);
    }

    اگر (n_nodes == 1)
    {
	dlayer->گرہ = nodes[0];
    }
    ورنہ اگر (n_nodes > 0)
    {
	dlayer->گرہ = gsk_container_گرہ_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	برائے (صحیح i = 0; i < (صحیح)ARRAY_لمبائی(nodes); i++)
	    gsk_render_گرہ_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    ساکن خالی
draw_صف_render_overlay(Draw_صف *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    اگر (!dlayer->dirty)
	واپس;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    اگر (rgba_is_transparent(dlayer->buf, da->n_ستون_تعداد))
	واپس;

    dlayer->گرہ = draw_layer_get_texture(dlayer, drow->صف, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    ساکن خالی
draw_صف_جوڑو_cluster(
	Draw_صف		*drow,
	DrawAttr	*dattr,
	صحیح		ستون,
	صحیح		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	صحیح		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = تہی;

    اگر (ستون + n_cells > da->n_ستون_تعداد)
	واپس;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    اگر (glyphs != تہی)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_صف_fill(drow, ستون, ستون + n_cells - 1, dglyphs, dattr, &da->cursor);

    اگر (dglyphs != تہی)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    ساکن صحیح
vim_draw_area_جوڑو_string(
	VimDrawArea *self,
	Draw_صف	    *drow,
	DrawAttr    *dattr,
	صحیح	    ستون,
	char_u	    *s,
	صحیح	    len,
	صحیح	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = تہی;
    صحیح			آغاز_idx = -1;
    GList		*شے_list;
    صحیح			عمود_انحراف = 0;
    صحیح			cluster_چوڑائی;
    صحیح			آخری_glyph_rbearing;
    صحیح			cells = 0;
    صحیح			i = 0;
    gboolean		has_unicode = FALSE;
    gboolean		needs_shaping = FALSE;

    // Scan the string, and find spans where ligatures should not happen. We
    // must ensure that attribute boundaries do not split a base character from
    // its combining marks, as Pango breaks items at attribute boundaries.
    جب (i < len)
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
	    want_disable = (آغاز_idx != -1);
	ورنہ
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
	    اگر (آغاز_idx == -1)
		آغاز_idx = i;
	}
	ورنہ اگر (آغاز_idx != -1)
	{
	    اگر (attr_list == تہی)
		attr_list = pango_attr_list_new();
	    داخل_کرو_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, آغاز_idx, i);
	    آغاز_idx = -1;
	}
	i += clen;
    }

    // Fast path for pure ASCII: use cached glyph table. Skip this path when
    // there are non-ascii characters in the string, font attributes, or if
    // there's a possible ligature.
    اگر (!(draw_flags & DRAW_ITALIC)
	    && !((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != تہی
	    && !has_unicode
	    && !needs_shaping)
    {
	برائے (i = 0; i < len; ++i)
	{
	    PangoGlyphInfo  glyph = gui.ascii_glyphs->glyphs[2 * s[i]];
	    gboolean	    has_ink;

	    has_ink = glyph_has_ink(gui.ascii_font, glyph);

	    draw_صف_جوڑو_cluster(drow, dattr, ستون + i, 1,
		    gui.ascii_font, has_ink ? &glyph : تہی, 1, self);
	}

	عمود_انحراف = len;
	جاؤ باہر_نکلو;
    }

    glyphs = pango_glyph_string_new();

    اگر (attr_list == تہی)
	attr_list = pango_attr_list_new();

    اگر (آغاز_idx != -1)
	داخل_کرو_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, آغاز_idx, len);

    cluster_چوڑائی = PANGO_SCALE * gui.char_چوڑائی;
    آخری_glyph_rbearing = PANGO_SCALE * gui.char_چوڑائی;

    // If 'guifontwide' is set then use that for double-width characters.
    اگر (gui.wide_font != تہی)
	apply_wide_font_attr(s, len, attr_list);

    اگر ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	داخل_کرو_PANGO_ATTR(pango_attr_وزن_new(PANGO_وزن_BOLD),
		attr_list, 0, len);
    اگر (draw_flags & DRAW_ITALIC)
	داخل_کرو_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    شے_list = pango_itemize(gui.متن_context,
	    (ثابت حرف *)s, 0, len, attr_list, تہی);

    جب (شے_list != تہی)
    {
	Pango_شے   *شے = شے_list->ڈیٹا;
	صحیح	    شے_cells = 0;
	صحیح	    cluster_آغاز_idx = 0;
	صحیح	    current_cluster_ستون = ستون + عمود_انحراف;
	صحیح	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	شے_list = g_list_delete_link(شے_list, شے_list);

	// Force LTR direction; Vim handles bidi on its own.
	شے->analysis.level = (شے->analysis.level + 1) & (~1U);

	pango_shape_بھرا((ثابت حرف *)s + شے->انحراف, شے->لمبائی,
		(ثابت حرف *)s, len, &شے->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	برائے (i = 0; i < glyphs->عدد_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    اگر (glyph->attr.is_cluster_آغاز)
	    {
		صحیح cellcount;

		cellcount = گنتی_cluster_cells(
			s, شے, glyphs, i, &cluster_چوڑائی,
			(شے_list != تہی) ? &آخری_glyph_rbearing : تہی);

		اگر (i > 0 && cellcount > 0)
		{
		    draw_صف_جوڑو_cluster(drow, dattr,
			    current_cluster_ستون, current_cluster_cells,
			    شے->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_آغاز_idx : تہی,
			    i - cluster_آغاز_idx, self);

		    current_cluster_ستون += current_cluster_cells;
		    cluster_آغاز_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		اگر (cellcount > 0)
		{
		    صحیح چوڑائی;

		    چوڑائی = cellcount * gui.char_چوڑائی * PANGO_SCALE;
		    glyph->geometry.x_انحراف +=
			MAX(0, چوڑائی - cluster_چوڑائی) / 2;
		    glyph->geometry.چوڑائی = چوڑائی;
		}
		ورنہ
		    تیاری_zero_چوڑائی_cluster(شے, glyph, cells,
			    cluster_چوڑائی,
			    آخری_glyph_rbearing);

		شے_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    ورنہ اگر (i > 0)
	    {
		صحیح چوڑائی;

		اگر (glyph->geometry.x_انحراف >= 0)
		{
		    glyphs->glyphs[i].geometry.چوڑائی =
			glyphs->glyphs[i - 1].geometry.چوڑائی;
		    glyphs->glyphs[i - 1].geometry.چوڑائی = 0;
		}
		چوڑائی = cells * gui.char_چوڑائی * PANGO_SCALE;
		glyph->geometry.x_انحراف +=
		    MAX(0, چوڑائی - cluster_چوڑائی) / 2;
	    }
	    ورنہ
		glyph->geometry.چوڑائی = 0;


	    اگر (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			شے->analysis.font, *glyph);
	}
	اگر (glyphs->عدد_glyphs > 0)
	    draw_صف_جوڑو_cluster(drow, dattr,
		    current_cluster_ستون, current_cluster_cells,
		    شے->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_آغاز_idx : تہی,
		    glyphs->عدد_glyphs - cluster_آغاز_idx, self);

	pango_شے_free(شے);
	عمود_انحراف += شے_cells;
    }

    pango_glyph_string_free(glyphs);

باہر_نکلو:
    اگر (attr_list != تہی)
	pango_attr_list_unref(attr_list);

    واپس عمود_انحراف;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    صحیح
vim_draw_area_draw_string(
	VimDrawArea *self,
	صحیح	    صف,
	صحیح	    ستون,
	char_u	    *s,
	صحیح	    len,
	صحیح	    flags)
{
    Draw_صف	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = تہی;
    صحیح		convlen;
    صحیح		n_cells ;

    اگر (unlikely(self->صفیں_2 == تہی
		|| صف >= self->n_صفیں
		|| ستون >= self->n_ستون_تعداد))
	واپس len;

    اگر (output_conv.vc_type != CONV_NONE)
    {
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	اگر (conv_buf != تہی)
	{
	    s = conv_buf;
	    len = convlen;
	}
    }

    // Safety check: pango crashes with invalid utf-8.
    اگر (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	واپس len;
    }

    drow = self->صفیں_2 + صف;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_جوڑو_string(self, drow, &dattr, ستون, s, len, flags);

    vim_free(conv_buf);

    واپس n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    خالی
vim_draw_area_clear_بلاک(
	VimDrawArea	*self,
	صحیح		row1,
	صحیح		col1,
	صحیح		row2,
	صحیح		col2)
{
    اگر (unlikely(self->صفیں_2 == تہی
		|| row1 >= self->n_صفیں
		|| col1 >= self->n_ستون_تعداد
		|| row2 >= self->n_صفیں
		|| col2 >= self->n_ستون_تعداد))
	واپس;

    برائے (صحیح r = row1; r <= row2; r++)
	draw_صف_fill(self->صفیں_2 + r, col1, col2, تہی, تہی, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    خالی
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_بلاک(self, 0, 0, self->n_صفیں - 1, self->n_ستون_تعداد - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    خالی
vim_draw_area_move_بلاک(
	VimDrawArea *self,
	صحیح	    مقصد,
	صحیح	    row1,
	صحیح	    row2,
	صحیح	    col1,
	صحیح	    col2)
{
    صحیح انحراف = row2 - row1;

    اگر (unlikely(self->صفیں_2 == تہی
		|| row1 >= self->n_صفیں
		|| row2 >= self->n_صفیں
		|| مقصد >= self->n_صفیں
		|| col1 >= self->n_ستون_تعداد
		|| col2 >= self->n_ستون_تعداد))
	واپس;

    أكد(row2 >= row1);
    أكد(col2 >= col1);
    أكد(row1 != مقصد);

    اگر (row1 > مقصد)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	برائے (صحیح o = 0; o <= انحراف; o++)
	    draw_صف_move_مقصد(self->صفیں_2 + مقصد + o, self->صفیں_2 + row1 + o,
		    col1, col2, self);
    }
    ورنہ
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	برائے (صحیح o = انحراف; o >= 0; o--)
	    اگر (مقصد + o >= self->n_صفیں)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_بلاک(self, row1 + o, col1, row1 + o, col2);
	    ورنہ
		draw_صف_move_مقصد(self->صفیں_2 + مقصد + o, self->صفیں_2 + row1 + o,
			col1, col2, self);
    }
}

/*
 * Set the cursor to the given width and height, use "gui.bgcolor" and
 * "gui.fgcolor" for the colors as well. If "w" and "h" is zero, then a full
 * block cursor is used, if "w" and "h" is -1 then a hollow cursor is used,
 * otherwise a part cursor with dimensions "w" and "h" is used.
 */
    خالی
vim_draw_area_سیٹ_cursor(VimDrawArea *self, صحیح w, صحیح h)
{
    self->cursor.draw = TRUE;
    self->cursor.چوڑائی = w;
    self->cursor.اونچائی = h;
    // Remember this now: the snapshot runs later, when the drawing position
    // has moved on.
    self->cursor.n_cells = 1 + mb_lefthalve(gui.cursor_صف, gui.cursor_ستون);
    self->cursor.bg_color = *gui.bgcolor;
    self->cursor.fg_color = *gui.fgcolor;
}

/*
 * Invert the rectangle in the draw area.
 */
    خالی
vim_draw_area_invert_بلاک(
	VimDrawArea	*self,
	صحیح		صف,
	صحیح		ستون,
	صحیح		nrows,
	صحیح		ncols)
{
    اگر (unlikely(self->صفیں_2 == تہی
		|| صف >= self->n_صفیں
		|| ستون >= self->n_ستون_تعداد
		|| صف + nrows - 1 >= self->n_صفیں
		|| ستون + ncols - 1 >= self->n_ستون_تعداد))
	واپس;

    برائے (صحیح r = صف; r < صف + nrows; r++)
    {
	Draw_صف	    *drow = self->صفیں_2 + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	برائے (صحیح c = 0; c < ncols; c++)
	    dlayer->buf[c + ستون] = ~dlayer->buf[c + ستون];
	draw_صف_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#اگر defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    خالی
vim_draw_area_جوڑو_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	صحیح	    صف,
	صحیح	    ستون,
	صحیح	    چوڑائی,
	صحیح	    اونچائی)
{
    Draw_صف	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحیح		cells;

    اگر (unlikely(self->صفیں_2 == تہی
		|| صف >= self->n_صفیں
		|| ستون >= self->n_ستون_تعداد))
	واپس;

    drow = self->صفیں_2 + صف;
    dsign = draw_sign_new(sign, چوڑائی, اونچائی);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (چوڑائی + gui.char_چوڑائی - 1) / gui.char_چوڑائی;

    draw_صف_fill(drow, ستون, ستون + cells - 1, dglyphs, تہی, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    خالی
vim_draw_area_جوڑو_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	صحیح		صف,
	صحیح		ستون,
	صحیح		چوڑائی,
	صحیح		اونچائی)
{
    Draw_صف	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحیح		cells;
    GdkTexture	*texture;
    GBytes	*بائٹیں;

    اگر (unlikely(self->صفیں_2 == تہی
		|| صف >= self->n_صفیں
		|| ستون >= self->n_ستون_تعداد))
	واپس;

    cairo_surface_flush(surf);
    بائٹیں = g_بائٹیں_new_with_free_func(cairo_image_surface_get_ڈیٹا(surf),
	    cairo_image_surface_get_اونچائی(surf)
	    * cairo_image_surface_get_stride(surf),
	    (GDestroyNotify)cairo_surface_destroy,
	    cairo_surface_reference(surf));

    texture = gdk_حافظہ_texture_new(cairo_image_surface_get_چوڑائی(surf),
	    cairo_image_surface_get_اونچائی(surf),
	    GDK_حافظہ_A8_R8_G8_B8_PREMULTIPLIED,
	    بائٹیں, cairo_image_surface_get_stride(surf));
    g_بائٹیں_unref(بائٹیں);

    drow = self->صفیں_2 + صف;
    dsign = draw_sign_new(texture, چوڑائی, اونچائی);
    g_object_unref(texture);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (چوڑائی + gui.char_چوڑائی - 1) / gui.char_چوڑائی;

    draw_صف_fill(drow, ستون, ستون + cells - 1, dglyphs, تہی, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    خالی
vim_draw_area_جوڑو_external(VimDrawArea *self, Gsk_Render_گرہ *گرہ)
{
    g_hash_جدول_جوڑو(self->external, gsk_render_گرہ_ref(گرہ));
}

    خالی
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_گرہ *گرہ)
{
    g_hash_جدول_remove(self->external, گرہ);
}

#اگر_ختم

/*
 * Add the cursor to the snapshot.
 */
    ساکن خالی
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_گرہ	*متن_گرہ,	// May be NULL
	Gsk_Render_گرہ	*special_گرہ,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    صحیح		    w = cursor->چوڑائی;
    صحیح		    h = cursor->اونچائی;
    graphene_rect_t rect;

    اگر (!cursor->draw)
	واپس;

    اگر (cursor->چوڑائی <= 0 && cursor->اونچائی <= 0)
    {
	w += gui.char_چوڑائی * cursor->n_cells;
	h = gui.char_اونچائی;
    }

    graphene_rect_init(&rect,
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    CURSOR_BAR_دایاں ? FILL_X(gui.cursor_ستون + 1) - w :
#اگر_ختم
	    FILL_X(gui.cursor_ستون),
	    FILL_Y(gui.cursor_صف) + gui.char_اونچائی - h, w, h);


    اگر (cursor->چوڑائی == -1 && cursor->اونچائی == -1)
    {
	// Create hollow cursor
	GskRoundedRect	    outline;
	ساکن ثابت اعشاری  border[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	ثابت GdkRGBA	    color[4] = {
	    cursor->bg_color, cursor->bg_color,
	    cursor->bg_color, cursor->bg_color
	} ;

	gsk_rounded_rect_init_from_rect(&outline, &rect, 0.0f);
	gtk_snapshot_append_border(snapshot, &outline, border, color);
    }
    ورنہ
    {
	// Create block style cursor, by masking the text + under decorations
	// with the fg color.
	gtk_snapshot_append_color(snapshot, &cursor->bg_color, &rect);

	gtk_snapshot_ڈالو_نقاب(snapshot, GSK_نقاب_منوال_ALPHA);

	// Used as an "outline" for the fg color.
	اگر (متن_گرہ != تہی)
	    gtk_snapshot_append_گرہ(snapshot, متن_گرہ);
	اگر (special_گرہ != تہی)
	    gtk_snapshot_append_گرہ(snapshot, special_گرہ);

	gtk_snapshot_نکالو(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_نکالو(snapshot);
    }
}

    ساکن خالی
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    صحیح		    اونچائی, چوڑائی;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = تہی;
    Gsk_Render_گرہ   *body_گرہ;
    Gsk_Render_گرہ   *invert_گرہ = تہی;
#اگر_تعریف_شدہ FEAT_IMAGE_GUI
    Gsk_Render_گرہ   *ext_گرہ;
    GHash_جدول_Iter  iter;
#اگر_ختم

    gui_mch_سیٹ_bg_color(gui.عقب_pixel);
    اونچائی = gtk_widget_get_اونچائی(widget) + gui.bleed_bot;
    چوڑائی = gtk_widget_get_چوڑائی(widget) + gui.bleed_دایاں;

    اگر (self->صفیں_2 == تہی)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, چوڑائی, اونچائی));
	واپس;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    اگر (self->bleed_دایاں == -1 || self->bleed_دایاں != gui.bleed_دایاں)
    {
	self->bleed_دایاں = gui.bleed_دایاں;
	برائے (صحیح r = 0; r < self->n_صفیں; r++)
	{
	    draw_صف_dirty_layer(self->صفیں_2 + r, DRAW_LAYER_BACKGROUND);
	    draw_صف_dirty_layer(self->صفیں_2 + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#اگر GTK_CHECK_نسخہ(4,24,0)
    gtk_snapshot_سیٹ_snap(body_snapshot, GSK_RECT_SNAP_باری);
#اگر_ختم

    برائے (صحیح r = 0; r < self->n_صفیں; r++)
    {
	Draw_صف *drow = self->صفیں_2 + r;

	draw_صف_render_background(drow, self);
	draw_صف_render_متن(drow, self);
	draw_صف_render_special(drow, self);
	draw_صف_render_overlay(drow, self);

	برائے (صحیح l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    اگر (dlayer->گرہ != تہی)
	    {
		اگر (l == DRAW_LAYER_OVERLAY)
		{
		    اگر (invert_snapshot == تہی)
		    {
			invert_snapshot = gtk_snapshot_new();
#اگر GTK_CHECK_نسخہ(4,24,0)
			gtk_snapshot_سیٹ_snap(invert_snapshot,
				GSK_RECT_SNAP_باری);
#اگر_ختم
		    }
		    gtk_snapshot_append_گرہ(invert_snapshot, dlayer->گرہ);
		}
		ورنہ
		    gtk_snapshot_append_گرہ(body_snapshot, dlayer->گرہ);
	    }
	}

	اگر (r == gui.cursor_صف)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_متن].گرہ,
		    drow->dlayers[DRAW_LAYER_SPECIAL].گرہ,
		    body_snapshot);
    }

    اگر (invert_snapshot != تہی)
	gtk_snapshot_ڈالو_blend(snapshot, GSK_BLEND_منوال_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, چوڑائی, اونچائی));
    body_گرہ = gtk_snapshot_free_مقصد_گرہ(body_snapshot);
    اگر (body_گرہ != تہی)
    {
	gtk_snapshot_append_گرہ(snapshot, body_گرہ);
	gsk_render_گرہ_unref(body_گرہ);
    }

#اگر_تعریف_شدہ FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_جدول_iter_init(&iter, self->external);

    جب (g_hash_جدول_iter_اگلا(&iter, (خالی **)&ext_گرہ, تہی))
	gtk_snapshot_append_گرہ(snapshot, ext_گرہ);
#اگر_ختم

    اگر (invert_snapshot != تہی)
    {
	gtk_snapshot_نکالو(snapshot);
	invert_گرہ = gtk_snapshot_free_مقصد_گرہ(invert_snapshot);
	// Not sure if it can be NULL but still check
	اگر (invert_گرہ != تہی)
	{
	    gtk_snapshot_append_گرہ(snapshot, invert_گرہ);
	    gsk_render_گرہ_unref(invert_گرہ);
	}
	gtk_snapshot_نکالو(snapshot);
    }
}
