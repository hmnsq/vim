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
    صحيح		چوڑائی;
    صحيح		اچائی;
} DrawSign;
#جے_انت

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
#جے_انت
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
    Gsk_Render_گرہ   *گرہ;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#تعریف_کرو UNDERC_وتھ(n_خانے_گنتی) (0)
#تعریف_کرو STRIKE_وتھ(n_خانے_گنتی) (n_خانے_گنتی)

تعريفنوع بنية
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    صحيح		n_cells; // For convenience
    صحيح		صف;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_گرہ *underl_نقاب;
    Gsk_Render_گرہ *underc_نقاب;
    Gsk_Render_گرہ *strike_نقاب;
} Draw_صف;

#تعریف_کرو داخل_کرو_PANGO_ATTR(Attribute, AttrList, شروع_2, سرا_2) \
    افعل { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->شروع_اشاریہ = (شروع_2); \
	tmp_attr_->سرا_اشاریہ = (سرا_2); \
	pango_attr_list_داخل_کرو((AttrList), tmp_attr_); \
    } طالما (FALSE)
#تعریف_کرو CELLS2PANGO(c) ((c) * gui.char_چوڑائی * PANGO_SCALE)

/*
 * Attributes of cursor
 */
تعريفنوع بنية
{
    gboolean draw; // If cursor should be drawn
    صحيح چوڑائی;
    صحيح اچائی;
    صحيح n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

بنية _VimDrawArea
{
    GtkWidget ماپا;

    Draw_صف *صفاں_2;

    صحيح n_صفاں;
    صحيح	n_خانے_گنتی;

    صحيح	bleed_دایاں;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*گرہ_buf;

    DrawCursor cursor;

#جے_تعریف_شدہ FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHash_جدول *external;
#جے_انت
};

ساكن فراغ draw_صف_init(Draw_صف *drow, صحيح صف, صحيح خانے_گنتی);
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
    لكل (صحيح r = 0; r < self->n_صفاں; r++)
	draw_صف_clear(self->صفاں_2 + r);
    g_free(self->صفاں_2);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->گرہ_buf, TRUE);

#جے_تعریف_شدہ FEAT_IMAGE_GUI
    g_hash_جدول_unref(self->external);
#جے_انت

    G_OBJECT_CLASS(vim_draw_area_ماپا_class)->finalize(obj);
}

    ساكن فراغ
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_سیٹ_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    ساكن فراغ
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_دایاں = -1;
#جے_تعریف_شدہ FEAT_IMAGE_GUI
    self->external = g_hash_جدول_new_بھریا(g_سدھا_hash, g_سدھا_equal,
	    عدم, (GDestroyNotify)gsk_render_گرہ_unref);
#جے_انت
    self->glyph_buf = g_array_new(FALSE, FALSE, حجم(PangoGlyphInfo));
    self->گرہ_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_گرہ_unref);

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
vim_draw_area_سیٹ_ناپ(VimDrawArea *self, صحيح صفاں_2, صحيح خانے_گنتی)
{
    إذا (self->صفاں_2 != عدم && self->n_صفاں == صفاں_2 && self->n_خانے_گنتی == خانے_گنتی)
	أرجع;
    إذا (صفاں_2 == 0 || خانے_گنتی == 0)
	أرجع;

    vim_draw_area_clear(self);

    لكل (صحيح r = 0; r < self->n_صفاں; r++)
	draw_صف_clear(self->صفاں_2 + r);
    self->صفاں_2 = g_realloc_n(self->صفاں_2, صفاں_2, حجم(Draw_صف));
    لكل (صحيح r = 0; r < صفاں_2; r++)
	draw_صف_init(self->صفاں_2 + r, r, خانے_گنتی);

    self->n_صفاں = صفاں_2;
    self->n_خانے_گنتی = خانے_گنتی;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    ساكن uint32_t
rgba_منزل_u32(ثابت GdkRGBA *rgba)
{
    اتحاد { uint32_t val; uint8_t بائٹاں[4]; } color;

    color.بائٹاں[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.بائٹاں[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.بائٹاں[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.بائٹاں[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    أرجع color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    ساكن GdkRGBA
u32_منزل_rgba(uint32_t val)
{
    اتحاد { uint32_t val; uint8_t بائٹاں[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.بائٹاں[0] / 255.0;
    color.green = packed.بائٹاں[1] / 255.0;
    color.blue  = packed.بائٹاں[2] / 255.0;
    color.alpha = packed.بائٹاں[3] / 255.0;
    أرجع color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    ساكن gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    اتحاد { uint32_t val; uint8_t بائٹاں[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.بائٹاں[0] << 16) |
	((guicolor_T)packed.بائٹاں[1] << 8)  |
	packed.بائٹاں[2];
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

    أرجع ink.چوڑائی > 0 && ink.اچائی > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    ساكن فراغ
apply_wide_font_attr(char_u *s, صحيح len, PangoAttrList *attr_list)
{
    char_u  *شروع = عدم;
    char_u  *p;
    صحيح	    uc;

    لكل (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	إذا (شروع == عدم)
	{
	    إذا (uc >= 0x80 && utf_char2cells(uc) == 2)
		شروع = p;
	}
	وإلا إذا (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    داخل_کرو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, شروع - s, p - s);
	    شروع = عدم;
	}
    }

    إذا (شروع != عدم)
	داخل_کرو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, شروع - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    ساكن صحيح
گنتی_cluster_cells(
	char_u		    *s,
	Pango_شے	    *شے,
	PangoGlyphString    *glyphs,
	صحيح		    i,
	صحيح		    *cluster_چوڑائی,
	صحيح		    *آخری_glyph_rbearing)
{
    char_u  *p;
    صحيح	    اگلا;
    صحيح	    شروع, سرا;
    صحيح	    چوڑائی;
    صحيح	    uc;
    صحيح	    cellcount = 0;

    چوڑائی = glyphs->glyphs[i].geometry.چوڑائی;

    لكل (اگلا = i + 1; اگلا < glyphs->عدد_glyphs; ++اگلا)
    {
	إذا (glyphs->glyphs[اگلا].attr.is_cluster_شروع)
	    اكسر;
	وإلا إذا (glyphs->glyphs[اگلا].geometry.چوڑائی > چوڑائی)
	    چوڑائی = glyphs->glyphs[اگلا].geometry.چوڑائی;
    }

    شروع = شے->وتھ + glyphs->log_clusters[i];
    سرا   = شے->وتھ + ((اگلا < glyphs->عدد_glyphs) ?
	    glyphs->log_clusters[اگلا] : شے->لمائی);

    لكل (p = s + شروع; p < s + سرا; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	إذا (uc < 0x80)
	    ++cellcount;
	وإلا إذا (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    إذا (آخری_glyph_rbearing != عدم
	    && cellcount > 0 && اگلا == glyphs->عدد_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(شے->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, عدم);

	إذا (PANGO_RBEARING(ink_rect) > 0)
	    *آخری_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    إذا (cellcount > 0)
	*cluster_چوڑائی = چوڑائی;

    أرجع cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    ساكن فراغ
تیاری_zero_چوڑائی_cluster(
	Pango_شے	*شے,
	PangoGlyphInfo	*glyph,
	صحيح		آخری_cellcount,
	صحيح		آخری_cluster_چوڑائی,
	صحيح		آخری_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحيح		    چوڑائی;

    چوڑائی = آخری_cellcount * gui.char_چوڑائی * PANGO_SCALE;
    glyph->geometry.x_وتھ = -چوڑائی + MAX(0, چوڑائی - آخری_cluster_چوڑائی) / 2;
    glyph->geometry.چوڑائی = 0;

    pango_font_get_glyph_extents(شے->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    إذا (ink_rect.x < 0)
    {
	glyph->geometry.x_وتھ += آخری_glyph_rbearing;
	glyph->geometry.y_وتھ  = logical_rect.اچائی
	    - (gui.char_اچائی - p_linespace) * PANGO_SCALE;
    }
    وإلا
	glyph->geometry.x_وتھ = -چوڑائی + MAX(0, چوڑائی - ink_rect.چوڑائی) / 2;
}

#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    ساكن DrawSign *
draw_sign_new(GdkTexture *texture, صحيح چوڑائی, صحيح اچائی)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->چوڑائی = چوڑائی;
    dsign->اچائی = اچائی;

    أرجع dsign;
}

    ساكن فراغ
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#جے_انت

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
draw_attr_apply(DrawAttr *dattr, صحيح خانہ, Draw_صف *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + خانہ;
    uint32_t	*متن_cell = drow->dlayers[DRAW_LAYER_متن].buf + خانہ;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    صحيح		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	خانہ + UNDERC_وتھ(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	خانہ + STRIKE_وتھ(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    إذا (dattr != عدم)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.پٹھ_pixel, bg_color);
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
#جے_انت

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
#جے_انت
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
		|| a->glyphs[i].geometry.چوڑائی != b->glyphs[i].geometry.چوڑائی
		|| a->glyphs[i].geometry.x_وتھ != b->glyphs[i].geometry.x_وتھ
		|| a->glyphs[i].geometry.y_وتھ != b->glyphs[i].geometry.y_وتھ
		|| a->glyphs[i].attr.is_cluster_شروع != b->glyphs[i].attr.is_cluster_شروع)
	    أرجع FALSE;
    أرجع TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    ساكن Gsk_Render_گرہ *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	صحيح	    صف,
	VimDrawArea *da,
	صحيح	    off,
	gboolean    bleed)
{
    GBytes	    *بائٹاں;
    GdkTexture	    *texture;
    Gsk_Render_گرہ   *گرہ;
    صحيح		    اچائی;

#إذا GTK_CHECK_نسخہ(4,24,0)
    اچائی = gui.char_اچائی;
#وإلا
    اچائی = gui.char_اچائی + 1;
#جے_انت

    إذا (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		حجم(uint32_t) * da->n_خانے_گنتی);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_خانے_گنتی - 1)),
		حجم(uint32_t)); // Add bleed using last pixel of buffer
	بائٹاں = g_byte_array_free_منزل_بائٹاں(arr);
    }
    وإلا
	بائٹاں = g_بائٹاں_new(dlayer->buf + off, حجم(uint32_t) * da->n_خانے_گنتی);

    texture = gdk_حافظہ_texture_new(da->n_خانے_گنتی + bleed, 1,
	    GDK_حافظہ_R8_G8_B8_A8, بائٹاں, (da->n_خانے_گنتی + bleed) * 4);
    g_بائٹاں_unref(بائٹاں);

    // Scale texture to actual size
    گرہ = gsk_texture_scale_گرہ_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(صف),
		(da->n_خانے_گنتی + bleed) * gui.char_چوڑائی, gui.char_اچائی + bleed),
	    GSK_SCALING_FILTER_سبھ_توں_نیڑے);
    إذا (bleed)
    {
	Gsk_Render_گرہ *new;

	new = gsk_clip_گرہ_new(گرہ,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(صف),
		    da->n_خانے_گنتی * gui.char_چوڑائی + da->bleed_دایاں, اچائی));
	gsk_render_گرہ_unref(گرہ);
	گرہ = new;
    }
    g_object_unref(texture);
    أرجع گرہ;
}

    ساكن فراغ
draw_صف_init(Draw_صف *drow, صحيح صف, صحيح خانے_گنتی)
{
    drow->cells = g_malloc0_n(خانے_گنتی, حجم(DrawCell));
    drow->n_cells = خانے_گنتی;
    drow->صف = صف;
    drow->underl_نقاب = drow->underc_نقاب = drow->strike_نقاب = عدم;

    memset(drow->dlayers, 0, حجم(DrawLayer) * N_DRAW_LAYERS);
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	إذا (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(خانے_گنتی * 2, 4);
	وإلا
	    dlayer->buf = g_malloc0_n(خانے_گنتی, 4);
    }
}

    ساكن فراغ
draw_صف_clear(Draw_صف *drow)
{
    g_free(drow->cells);
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	إذا (dlayer->گرہ != عدم)
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
    ساكن فراغ
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
    ساكن فراغ
draw_صف_uninvert_cell(Draw_صف *drow, صحيح خانہ)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    إذا (buf[خانہ] == 0xffffffff)
    {
	draw_صف_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[خانہ] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    ساكن فراغ
draw_صف_سیٹ_cell(Draw_صف *drow, صحيح خانہ, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + خانہ;

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

    draw_attr_apply(dattr, خانہ, drow);
    draw_صف_uninvert_cell(drow, خانہ);
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
	draw_صف_سیٹ_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    إذا (gui.cursor_صف == drow->صف && gui.cursor_خانہ >= col1
	    && gui.cursor_خانہ <= col2)
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
    صحيح move_ناپ = (col2 - col1 + 1) * حجم(DrawCell);

    draw_صف_fill(dest_صف, col1, col2, عدم, عدم, &da->cursor);
    memmove(dest_صف->cells + col1, src_صف->cells + col1, move_ناپ);
    // NULL the draw cells so we don't double unreference.
    memset(src_صف->cells + col1, 0, move_ناپ);
    draw_صف_dirty_layer(src_صف, DRAW_LAYER_متن);

    // Move the layer buffers
    move_ناپ = (col2 - col1 + 1) * 4;
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_صف->dlayers[l].buf;
	uint32_t *dest = dest_صف->dlayers[l].buf;

	إذا (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_وتھ(dest_صف->n_cells),
		    src + col1 + UNDERC_وتھ(src_صف->n_cells), move_ناپ);
	    memmove(dest + col1 + STRIKE_وتھ(dest_صف->n_cells),
		    src + col1 + STRIKE_وتھ(src_صف->n_cells), move_ناپ);
	}
	وإلا
	    memmove(dest + col1, src + col1, move_ناپ);
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
    إذا (rgba_is_transparent(dlayer->buf, da->n_خانے_گنتی))
	أرجع;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->گرہ = draw_layer_get_texture(dlayer, drow->صف, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    ساكن فراغ
draw_صف_ensure_decor(Draw_صف *drow, صحيح flags)
{
    إذا (flags & DRAW_UNDERL && drow->underl_نقاب == عدم)
	drow->underl_نقاب = gsk_color_گرہ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->صف + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    إذا (flags & DRAW_STRIKE && drow->strike_نقاب == عدم)
	drow->strike_نقاب = gsk_color_گرہ_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->صف) + (صحيح)(gui.char_اچائی / 2),
		    FILL_X(drow->n_cells), 1));

    إذا (flags & DRAW_UNDERC && drow->underc_نقاب == عدم)
    {
	ساكن ثابت صحيح val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	صحيح y = FILL_Y(drow->صف + 1) - 1;
	صحيح x_شروع = FILL_X(0);
	صحيح x_سرا = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_گرہ *بچہ = عدم;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#إذا GTK_CHECK_نسخہ(4, 14, 0)
	Gsk_رستہ_Builder	*builder;
	Gsk_رستہ		*رستہ;
	GskStroke       *stroke;
	Gsk_Render_گرہ	*color_گرہ;
	graphene_rect_t bounds;

	builder = gsk_رستہ_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_رستہ_builder_move_منزل(builder, -1, y - val[7] + 0.5);
	gsk_رستہ_builder_سطر_منزل(builder, 0, y - val[0] + 0.5);

	لكل (صحيح i = 1; i < 8; i++)
	    gsk_رستہ_builder_سطر_منزل(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_رستہ_builder_سطر_منزل(builder, 8, y - val[0] + 0.5);
	gsk_رستہ_builder_سطر_منزل(builder, 9, y - val[1] + 0.5);

	رستہ = gsk_رستہ_builder_free_منزل_رستہ(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_رستہ_get_stroke_bounds(رستہ, stroke, &bounds);
	color_گرہ = gsk_color_گرہ_new(&white_rgba, &bounds);
	بچہ = gsk_stroke_گرہ_new(color_گرہ, رستہ, stroke);

	gsk_stroke_free(stroke);
	gsk_رستہ_unref(رستہ);
	gsk_render_گرہ_unref(color_گرہ);
#وإلا
	cairo_t *cr;

	بچہ = gsk_cairo_گرہ_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_گرہ_get_draw_context(بچہ);

	cairo_سیٹ_سطر_چوڑائی(cr, 1.0);
	cairo_سیٹ_سروت_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_منزل(cr, -1, y - val[7] + 0.5);
	cairo_سطر_منزل(cr, 0, y - val[0] + 0.5);

	لكل (صحيح i = 1; i < 8; ++i)
	    cairo_سطر_منزل(cr, i, y - val[i] + 0.5);

	cairo_سطر_منزل(cr, 8, y - val[0] + 0.5);
	cairo_سطر_منزل(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#جے_انت
	drow->underc_نقاب = gsk_repeat_گرہ_new(
		&GRAPHENE_RECT_INIT(x_شروع, y - 3, x_سرا - x_شروع, 5),
		بچہ, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_گرہ_unref(بچہ);
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
    صحيح		شروع_خانہ = 0;
    صحيح		خالی_تھاواں_cells = 0;
    uint32_t	cur_fg = 0;
    صحيح		c;

    إذا (!dlayer->dirty)
	أرجع;

    glyph_buf = da->glyph_buf;
    nodes = da->گرہ_buf;

    g_array_سیٹ_ناپ(glyph_buf, 0);
    g_ptr_array_سیٹ_ناپ(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#تعریف_کرو FLUSH_گرہ() \
    افعل { \
	إذا (cur_font != عدم) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_گرہ	*گرہ; \
	    GdkRGBA		fg_color = u32_منزل_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->انکڑے; \
	    glyph_str.عدد_glyphs = glyph_buf->len; \
	    گرہ = gsk_متن_گرہ_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_نقطہ_INIT(متن_X(شروع_خانہ), \
			متن_Y(drow->صف))); \
	    إذا (گرہ != عدم) \
		g_ptr_array_جوڑو(nodes, گرہ); \
	    g_array_سیٹ_ناپ(glyph_buf, 0); \
	    cur_font = عدم; \
	} \
    } طالما (FALSE)

    لكل (c = 0; c < da->n_خانے_گنتی; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	صحيح	    prev_len;

	إذا (dglyphs == عدم)
	{
	    خالی_تھاواں_cells++;
	    استمر;
	}
#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	وإلا إذا (dglyphs->font == عدم)
	{
	    // Add sign icon
	    Gsk_Render_گرہ   *snode;
	    صحيح		    w;

	    FLUSH_گرہ();

	    snode = gsk_texture_scale_گرہ_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->صف),
			dglyphs->dsign->چوڑائی, dglyphs->dsign->اچائی),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->چوڑائی + gui.char_چوڑائی - 1) / gui.char_چوڑائی;

	    إذا (snode != عدم)
		g_ptr_array_جوڑو(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    استمر;
	}
#جے_انت
	وإلا إذا (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_گرہ();
	    cur_font = dglyphs->font;
	    شروع_خانہ = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	إذا (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    استمر;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	إذا (خالی_تھاواں_cells > 0)
	{
	    إذا (prev_len > 0)
	    {
		// Spaces occurred between valid text: extend
		// previous character's advance
		PangoGlyphInfo *آخری = &g_array_اشاریہ(
			glyph_buf, PangoGlyphInfo,
			prev_len - 1);
		آخری->geometry.چوڑائی += CELLS2PANGO(خالی_تھاواں_cells);
	    }
	    خالی_تھاواں_cells = 0; // Width consumed
	}
    }

    FLUSH_گرہ();

#تعریف_رد FLUSH_گرہ

    إذا (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_گرہ *سروت;
	Gsk_Render_گرہ *گرہ;

	draw_صف_ensure_decor(drow, DRAW_UNDERL);
	إذا (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    سروت = draw_layer_get_texture(dlayer, drow->صف, da, 0, FALSE);
	    گرہ = gsk_نقاب_گرہ_new(سروت, drow->underl_نقاب,
		    GSK_نقاب_منوال_ALPHA);
	    gsk_render_گرہ_unref(سروت);
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
    ساكن فراغ
draw_صف_render_special(Draw_صف *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_گرہ   *سروت;
    Gsk_Render_گرہ   *nodes[2];
    صحيح		    n_nodes = 0;

    إذا (!dlayer->dirty)
	أرجع;

    draw_صف_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    إذا (!rgba_is_transparent(dlayer->buf + UNDERC_وتھ(drow->n_cells),
		drow->n_cells))
    {
	سروت = draw_layer_get_texture(dlayer, drow->صف, da,
		UNDERC_وتھ(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_نقاب_گرہ_new(سروت, drow->underc_نقاب,
		GSK_نقاب_منوال_ALPHA);
	gsk_render_گرہ_unref(سروت);
    }

    إذا (!rgba_is_transparent(dlayer->buf + STRIKE_وتھ(drow->n_cells),
		drow->n_cells))
    {
	سروت = draw_layer_get_texture(dlayer, drow->صف, da,
		STRIKE_وتھ(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_نقاب_گرہ_new(سروت, drow->strike_نقاب,
		GSK_نقاب_منوال_ALPHA);
	gsk_render_گرہ_unref(سروت);
    }

    إذا (n_nodes == 1)
    {
	dlayer->گرہ = nodes[0];
    }
    وإلا إذا (n_nodes > 0)
    {
	dlayer->گرہ = gsk_container_گرہ_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	لكل (صحيح i = 0; i < (صحيح)ARRAY_لمائی(nodes); i++)
	    gsk_render_گرہ_unref(nodes[i]);
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
    إذا (rgba_is_transparent(dlayer->buf, da->n_خانے_گنتی))
	أرجع;

    dlayer->گرہ = draw_layer_get_texture(dlayer, drow->صف, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    ساكن فراغ
draw_صف_جوڑو_cluster(
	Draw_صف		*drow,
	DrawAttr	*dattr,
	صحيح		خانہ,
	صحيح		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	صحيح		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = عدم;

    إذا (خانہ + n_cells > da->n_خانے_گنتی)
	أرجع;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    إذا (glyphs != عدم)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_صف_fill(drow, خانہ, خانہ + n_cells - 1, dglyphs, dattr, &da->cursor);

    إذا (dglyphs != عدم)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    ساكن صحيح
vim_draw_area_جوڑو_string(
	VimDrawArea *self,
	Draw_صف	    *drow,
	DrawAttr    *dattr,
	صحيح	    خانہ,
	char_u	    *s,
	صحيح	    len,
	صحيح	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = عدم;
    صحيح			شروع_idx = -1;
    GList		*شے_list;
    صحيح			کالم_وتھ = 0;
    صحيح			cluster_چوڑائی;
    صحيح			آخری_glyph_rbearing;
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
	    want_disable = (شروع_idx != -1);
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
	    إذا (شروع_idx == -1)
		شروع_idx = i;
	}
	وإلا إذا (شروع_idx != -1)
	{
	    إذا (attr_list == عدم)
		attr_list = pango_attr_list_new();
	    داخل_کرو_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, شروع_idx, i);
	    شروع_idx = -1;
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

	    draw_صف_جوڑو_cluster(drow, dattr, خانہ + i, 1,
		    gui.ascii_font, has_ink ? &glyph : عدم, 1, self);
	}

	کالم_وتھ = len;
	اذهب باہر_نکلو;
    }

    glyphs = pango_glyph_string_new();

    إذا (attr_list == عدم)
	attr_list = pango_attr_list_new();

    إذا (شروع_idx != -1)
	داخل_کرو_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, شروع_idx, len);

    cluster_چوڑائی = PANGO_SCALE * gui.char_چوڑائی;
    آخری_glyph_rbearing = PANGO_SCALE * gui.char_چوڑائی;

    // If 'guifontwide' is set then use that for double-width characters.
    إذا (gui.wide_font != عدم)
	apply_wide_font_attr(s, len, attr_list);

    إذا ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	داخل_کرو_PANGO_ATTR(pango_attr_بھار_new(PANGO_بھار_BOLD),
		attr_list, 0, len);
    إذا (draw_flags & DRAW_ITALIC)
	داخل_کرو_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    شے_list = pango_itemize(gui.متن_context,
	    (ثابت حرف *)s, 0, len, attr_list, عدم);

    طالما (شے_list != عدم)
    {
	Pango_شے   *شے = شے_list->انکڑے;
	صحيح	    شے_cells = 0;
	صحيح	    cluster_شروع_idx = 0;
	صحيح	    current_cluster_خانہ = خانہ + کالم_وتھ;
	صحيح	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	شے_list = g_list_delete_link(شے_list, شے_list);

	// Force LTR direction; Vim handles bidi on its own.
	شے->analysis.level = (شے->analysis.level + 1) & (~1U);

	pango_shape_بھریا((ثابت حرف *)s + شے->وتھ, شے->لمائی,
		(ثابت حرف *)s, len, &شے->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	لكل (i = 0; i < glyphs->عدد_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    إذا (glyph->attr.is_cluster_شروع)
	    {
		صحيح cellcount;

		cellcount = گنتی_cluster_cells(
			s, شے, glyphs, i, &cluster_چوڑائی,
			(شے_list != عدم) ? &آخری_glyph_rbearing : عدم);

		إذا (i > 0 && cellcount > 0)
		{
		    draw_صف_جوڑو_cluster(drow, dattr,
			    current_cluster_خانہ, current_cluster_cells,
			    شے->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_شروع_idx : عدم,
			    i - cluster_شروع_idx, self);

		    current_cluster_خانہ += current_cluster_cells;
		    cluster_شروع_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		إذا (cellcount > 0)
		{
		    صحيح چوڑائی;

		    چوڑائی = cellcount * gui.char_چوڑائی * PANGO_SCALE;
		    glyph->geometry.x_وتھ +=
			MAX(0, چوڑائی - cluster_چوڑائی) / 2;
		    glyph->geometry.چوڑائی = چوڑائی;
		}
		وإلا
		    تیاری_zero_چوڑائی_cluster(شے, glyph, cells,
			    cluster_چوڑائی,
			    آخری_glyph_rbearing);

		شے_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    وإلا إذا (i > 0)
	    {
		صحيح چوڑائی;

		إذا (glyph->geometry.x_وتھ >= 0)
		{
		    glyphs->glyphs[i].geometry.چوڑائی =
			glyphs->glyphs[i - 1].geometry.چوڑائی;
		    glyphs->glyphs[i - 1].geometry.چوڑائی = 0;
		}
		چوڑائی = cells * gui.char_چوڑائی * PANGO_SCALE;
		glyph->geometry.x_وتھ +=
		    MAX(0, چوڑائی - cluster_چوڑائی) / 2;
	    }
	    وإلا
		glyph->geometry.چوڑائی = 0;


	    إذا (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			شے->analysis.font, *glyph);
	}
	إذا (glyphs->عدد_glyphs > 0)
	    draw_صف_جوڑو_cluster(drow, dattr,
		    current_cluster_خانہ, current_cluster_cells,
		    شے->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_شروع_idx : عدم,
		    glyphs->عدد_glyphs - cluster_شروع_idx, self);

	pango_شے_free(شے);
	کالم_وتھ += شے_cells;
    }

    pango_glyph_string_free(glyphs);

باہر_نکلو:
    إذا (attr_list != عدم)
	pango_attr_list_unref(attr_list);

    أرجع کالم_وتھ;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    صحيح
vim_draw_area_draw_string(
	VimDrawArea *self,
	صحيح	    صف,
	صحيح	    خانہ,
	char_u	    *s,
	صحيح	    len,
	صحيح	    flags)
{
    Draw_صف	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = عدم;
    صحيح		convlen;
    صحيح		n_cells ;

    إذا (unlikely(self->صفاں_2 == عدم
		|| صف >= self->n_صفاں
		|| خانہ >= self->n_خانے_گنتی))
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

    drow = self->صفاں_2 + صف;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_جوڑو_string(self, drow, &dattr, خانہ, s, len, flags);

    vim_free(conv_buf);

    أرجع n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    فراغ
vim_draw_area_clear_بلاک(
	VimDrawArea	*self,
	صحيح		row1,
	صحيح		col1,
	صحيح		row2,
	صحيح		col2)
{
    إذا (unlikely(self->صفاں_2 == عدم
		|| row1 >= self->n_صفاں
		|| col1 >= self->n_خانے_گنتی
		|| row2 >= self->n_صفاں
		|| col2 >= self->n_خانے_گنتی))
	أرجع;

    لكل (صحيح r = row1; r <= row2; r++)
	draw_صف_fill(self->صفاں_2 + r, col1, col2, عدم, عدم, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    فراغ
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_بلاک(self, 0, 0, self->n_صفاں - 1, self->n_خانے_گنتی - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    فراغ
vim_draw_area_move_بلاک(
	VimDrawArea *self,
	صحيح	    منزل,
	صحيح	    row1,
	صحيح	    row2,
	صحيح	    col1,
	صحيح	    col2)
{
    صحيح وتھ = row2 - row1;

    إذا (unlikely(self->صفاں_2 == عدم
		|| row1 >= self->n_صفاں
		|| row2 >= self->n_صفاں
		|| منزل >= self->n_صفاں
		|| col1 >= self->n_خانے_گنتی
		|| col2 >= self->n_خانے_گنتی))
	أرجع;

    أكد(row2 >= row1);
    أكد(col2 >= col1);
    أكد(row1 != منزل);

    إذا (row1 > منزل)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	لكل (صحيح o = 0; o <= وتھ; o++)
	    draw_صف_move_منزل(self->صفاں_2 + منزل + o, self->صفاں_2 + row1 + o,
		    col1, col2, self);
    }
    وإلا
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	لكل (صحيح o = وتھ; o >= 0; o--)
	    إذا (منزل + o >= self->n_صفاں)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_بلاک(self, row1 + o, col1, row1 + o, col2);
	    وإلا
		draw_صف_move_منزل(self->صفاں_2 + منزل + o, self->صفاں_2 + row1 + o,
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
vim_draw_area_سیٹ_cursor(VimDrawArea *self, صحيح w, صحيح h)
{
    self->cursor.draw = TRUE;
    self->cursor.چوڑائی = w;
    self->cursor.اچائی = h;
    // Remember this now: the snapshot runs later, when the drawing position
    // has moved on.
    self->cursor.n_cells = 1 + mb_lefthalve(gui.cursor_صف, gui.cursor_خانہ);
    self->cursor.bg_color = *gui.bgcolor;
    self->cursor.fg_color = *gui.fgcolor;
}

/*
 * Invert the rectangle in the draw area.
 */
    فراغ
vim_draw_area_invert_بلاک(
	VimDrawArea	*self,
	صحيح		صف,
	صحيح		خانہ,
	صحيح		nrows,
	صحيح		ncols)
{
    إذا (unlikely(self->صفاں_2 == عدم
		|| صف >= self->n_صفاں
		|| خانہ >= self->n_خانے_گنتی
		|| صف + nrows - 1 >= self->n_صفاں
		|| خانہ + ncols - 1 >= self->n_خانے_گنتی))
	أرجع;

    لكل (صحيح r = صف; r < صف + nrows; r++)
    {
	Draw_صف	    *drow = self->صفاں_2 + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	لكل (صحيح c = 0; c < ncols; c++)
	    dlayer->buf[c + خانہ] = ~dlayer->buf[c + خانہ];
	draw_صف_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#إذا defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    فراغ
vim_draw_area_جوڑو_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	صحيح	    صف,
	صحيح	    خانہ,
	صحيح	    چوڑائی,
	صحيح	    اچائی)
{
    Draw_صف	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحيح		cells;

    إذا (unlikely(self->صفاں_2 == عدم
		|| صف >= self->n_صفاں
		|| خانہ >= self->n_خانے_گنتی))
	أرجع;

    drow = self->صفاں_2 + صف;
    dsign = draw_sign_new(sign, چوڑائی, اچائی);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (چوڑائی + gui.char_چوڑائی - 1) / gui.char_چوڑائی;

    draw_صف_fill(drow, خانہ, خانہ + cells - 1, dglyphs, عدم, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#جے_انت

#جے_تعریف_شدہ FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    فراغ
vim_draw_area_جوڑو_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	صحيح		صف,
	صحيح		خانہ,
	صحيح		چوڑائی,
	صحيح		اچائی)
{
    Draw_صف	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحيح		cells;
    GdkTexture	*texture;
    GBytes	*بائٹاں;

    إذا (unlikely(self->صفاں_2 == عدم
		|| صف >= self->n_صفاں
		|| خانہ >= self->n_خانے_گنتی))
	أرجع;

    cairo_surface_flush(surf);
    بائٹاں = g_بائٹاں_new_with_free_func(cairo_image_surface_get_انکڑے(surf),
	    cairo_image_surface_get_اچائی(surf)
	    * cairo_image_surface_get_stride(surf),
	    (GDestroyNotify)cairo_surface_destroy,
	    cairo_surface_reference(surf));

    texture = gdk_حافظہ_texture_new(cairo_image_surface_get_چوڑائی(surf),
	    cairo_image_surface_get_اچائی(surf),
	    GDK_حافظہ_A8_R8_G8_B8_PREMULTIPLIED,
	    بائٹاں, cairo_image_surface_get_stride(surf));
    g_بائٹاں_unref(بائٹاں);

    drow = self->صفاں_2 + صف;
    dsign = draw_sign_new(texture, چوڑائی, اچائی);
    g_object_unref(texture);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (چوڑائی + gui.char_چوڑائی - 1) / gui.char_چوڑائی;

    draw_صف_fill(drow, خانہ, خانہ + cells - 1, dglyphs, عدم, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#جے_انت

#جے_تعریف_شدہ FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    فراغ
vim_draw_area_جوڑو_external(VimDrawArea *self, Gsk_Render_گرہ *گرہ)
{
    g_hash_جدول_جوڑو(self->external, gsk_render_گرہ_ref(گرہ));
}

    فراغ
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_گرہ *گرہ)
{
    g_hash_جدول_remove(self->external, گرہ);
}

#جے_انت

/*
 * Add the cursor to the snapshot.
 */
    ساكن فراغ
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_گرہ	*متن_گرہ,	// May be NULL
	Gsk_Render_گرہ	*special_گرہ,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    صحيح		    w = cursor->چوڑائی;
    صحيح		    h = cursor->اچائی;
    graphene_rect_t rect;

    إذا (!cursor->draw)
	أرجع;

    إذا (cursor->چوڑائی <= 0 && cursor->اچائی <= 0)
    {
	w += gui.char_چوڑائی * cursor->n_cells;
	h = gui.char_اچائی;
    }

    graphene_rect_init(&rect,
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	    CURSOR_BAR_دایاں ? FILL_X(gui.cursor_خانہ + 1) - w :
#جے_انت
	    FILL_X(gui.cursor_خانہ),
	    FILL_Y(gui.cursor_صف) + gui.char_اچائی - h, w, h);


    إذا (cursor->چوڑائی == -1 && cursor->اچائی == -1)
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

	gtk_snapshot_ڈالو_نقاب(snapshot, GSK_نقاب_منوال_ALPHA);

	// Used as an "outline" for the fg color.
	إذا (متن_گرہ != عدم)
	    gtk_snapshot_append_گرہ(snapshot, متن_گرہ);
	إذا (special_گرہ != عدم)
	    gtk_snapshot_append_گرہ(snapshot, special_گرہ);

	gtk_snapshot_نکالو(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_نکالو(snapshot);
    }
}

    ساكن فراغ
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    صحيح		    اچائی, چوڑائی;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = عدم;
    Gsk_Render_گرہ   *body_گرہ;
    Gsk_Render_گرہ   *invert_گرہ = عدم;
#جے_تعریف_شدہ FEAT_IMAGE_GUI
    Gsk_Render_گرہ   *ext_گرہ;
    GHash_جدول_Iter  iter;
#جے_انت

    gui_mch_سیٹ_bg_color(gui.پٹھ_pixel);
    اچائی = gtk_widget_get_اچائی(widget) + gui.bleed_bot;
    چوڑائی = gtk_widget_get_چوڑائی(widget) + gui.bleed_دایاں;

    إذا (self->صفاں_2 == عدم)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, چوڑائی, اچائی));
	أرجع;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    إذا (self->bleed_دایاں == -1 || self->bleed_دایاں != gui.bleed_دایاں)
    {
	self->bleed_دایاں = gui.bleed_دایاں;
	لكل (صحيح r = 0; r < self->n_صفاں; r++)
	{
	    draw_صف_dirty_layer(self->صفاں_2 + r, DRAW_LAYER_BACKGROUND);
	    draw_صف_dirty_layer(self->صفاں_2 + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#إذا GTK_CHECK_نسخہ(4,24,0)
    gtk_snapshot_سیٹ_snap(body_snapshot, GSK_RECT_SNAP_گیڑا);
#جے_انت

    لكل (صحيح r = 0; r < self->n_صفاں; r++)
    {
	Draw_صف *drow = self->صفاں_2 + r;

	draw_صف_render_background(drow, self);
	draw_صف_render_متن(drow, self);
	draw_صف_render_special(drow, self);
	draw_صف_render_overlay(drow, self);

	لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    إذا (dlayer->گرہ != عدم)
	    {
		إذا (l == DRAW_LAYER_OVERLAY)
		{
		    إذا (invert_snapshot == عدم)
		    {
			invert_snapshot = gtk_snapshot_new();
#إذا GTK_CHECK_نسخہ(4,24,0)
			gtk_snapshot_سیٹ_snap(invert_snapshot,
				GSK_RECT_SNAP_گیڑا);
#جے_انت
		    }
		    gtk_snapshot_append_گرہ(invert_snapshot, dlayer->گرہ);
		}
		وإلا
		    gtk_snapshot_append_گرہ(body_snapshot, dlayer->گرہ);
	    }
	}

	إذا (r == gui.cursor_صف)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_متن].گرہ,
		    drow->dlayers[DRAW_LAYER_SPECIAL].گرہ,
		    body_snapshot);
    }

    إذا (invert_snapshot != عدم)
	gtk_snapshot_ڈالو_blend(snapshot, GSK_BLEND_منوال_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, چوڑائی, اچائی));
    body_گرہ = gtk_snapshot_free_منزل_گرہ(body_snapshot);
    إذا (body_گرہ != عدم)
    {
	gtk_snapshot_append_گرہ(snapshot, body_گرہ);
	gsk_render_گرہ_unref(body_گرہ);
    }

#جے_تعریف_شدہ FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_جدول_iter_init(&iter, self->external);

    طالما (g_hash_جدول_iter_اگلا(&iter, (فراغ **)&ext_گرہ, عدم))
	gtk_snapshot_append_گرہ(snapshot, ext_گرہ);
#جے_انت

    إذا (invert_snapshot != عدم)
    {
	gtk_snapshot_نکالو(snapshot);
	invert_گرہ = gtk_snapshot_free_منزل_گرہ(invert_snapshot);
	// Not sure if it can be NULL but still check
	إذا (invert_گرہ != عدم)
	{
	    gtk_snapshot_append_گرہ(snapshot, invert_گرہ);
	    gsk_render_گرہ_unref(invert_گرہ);
	}
	gtk_snapshot_نکالو(snapshot);
    }
}
