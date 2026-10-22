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
    صحيح		عرض;
    صحيح		ارتفاع;
} DrawSign;
#پایان_اگر

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
#پایان_اگر
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
    Gsk_Render_گره   *گره;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#تعریف_کن UNDERC_جابه_جایی(n_تعداد_ستون) (0)
#تعریف_کن STRIKE_جابه_جایی(n_تعداد_ستون) (n_تعداد_ستون)

تعريفنوع بنية
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    صحيح		n_cells; // For convenience
    صحيح		ردیف;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_گره *underl_ماسک;
    Gsk_Render_گره *underc_ماسک;
    Gsk_Render_گره *strike_ماسک;
} Draw_ردیف;

#تعریف_کن درج_PANGO_ATTR(Attribute, AttrList, شروع_2, انتها_2) \
    افعل { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->شروع_شاخص = (شروع_2); \
	tmp_attr_->انتها_شاخص = (انتها_2); \
	pango_attr_list_درج((AttrList), tmp_attr_); \
    } طالما (FALSE)
#تعریف_کن CELLS2PANGO(c) ((c) * gui.char_عرض * PANGO_SCALE)

/*
 * Attributes of cursor
 */
تعريفنوع بنية
{
    gboolean draw; // If cursor should be drawn
    صحيح عرض;
    صحيح ارتفاع;
    صحيح n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

بنية _VimDrawArea
{
    GtkWidget والد;

    Draw_ردیف *ردیفها_2;

    صحيح n_ردیفها;
    صحيح	n_تعداد_ستون;

    صحيح	bleed_راست;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*گره_buf;

    DrawCursor cursor;

#اگر_تعریف_شده FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHash_جدول *external;
#پایان_اگر
};

ساكن فراغ draw_ردیف_init(Draw_ردیف *drow, صحيح ردیف, صحيح تعداد_ستون);
ساكن فراغ draw_ردیف_clear(Draw_ردیف *drow);
ساكن فراغ draw_ردیف_dirty_layer(Draw_ردیف *drow, DrawLayerType dlayer_t);
ساكن فراغ vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

ساكن GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    ساكن فراغ
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    لكل (صحيح r = 0; r < self->n_ردیفها; r++)
	draw_ردیف_clear(self->ردیفها_2 + r);
    g_free(self->ردیفها_2);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->گره_buf, TRUE);

#اگر_تعریف_شده FEAT_IMAGE_GUI
    g_hash_جدول_unref(self->external);
#پایان_اگر

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
    gtk_widget_class_مجموعه_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    ساكن فراغ
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_راست = -1;
#اگر_تعریف_شده FEAT_IMAGE_GUI
    self->external = g_hash_جدول_new_پر(g_مستقیم_hash, g_مستقیم_equal,
	    عدم, (GDestroyNotify)gsk_render_گره_unref);
#پایان_اگر
    self->glyph_buf = g_array_new(FALSE, FALSE, حجم(PangoGlyphInfo));
    self->گره_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_گره_unref);

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
vim_draw_area_مجموعه_اندازه(VimDrawArea *self, صحيح ردیفها_2, صحيح تعداد_ستون)
{
    إذا (self->ردیفها_2 != عدم && self->n_ردیفها == ردیفها_2 && self->n_تعداد_ستون == تعداد_ستون)
	أرجع;
    إذا (ردیفها_2 == 0 || تعداد_ستون == 0)
	أرجع;

    vim_draw_area_clear(self);

    لكل (صحيح r = 0; r < self->n_ردیفها; r++)
	draw_ردیف_clear(self->ردیفها_2 + r);
    self->ردیفها_2 = g_realloc_n(self->ردیفها_2, ردیفها_2, حجم(Draw_ردیف));
    لكل (صحيح r = 0; r < ردیفها_2; r++)
	draw_ردیف_init(self->ردیفها_2 + r, r, تعداد_ستون);

    self->n_ردیفها = ردیفها_2;
    self->n_تعداد_ستون = تعداد_ستون;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    ساكن uint32_t
rgba_مقصد_u32(ثابت GdkRGBA *rgba)
{
    اتحاد { uint32_t val; uint8_t بایتها[4]; } color;

    color.بایتها[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.بایتها[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.بایتها[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.بایتها[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    أرجع color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    ساكن GdkRGBA
u32_مقصد_rgba(uint32_t val)
{
    اتحاد { uint32_t val; uint8_t بایتها[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.بایتها[0] / 255.0;
    color.green = packed.بایتها[1] / 255.0;
    color.blue  = packed.بایتها[2] / 255.0;
    color.alpha = packed.بایتها[3] / 255.0;
    أرجع color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    ساكن gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    اتحاد { uint32_t val; uint8_t بایتها[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.بایتها[0] << 16) |
	((guicolor_T)packed.بایتها[1] << 8)  |
	packed.بایتها[2];
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

    أرجع ink.عرض > 0 && ink.ارتفاع > 0;
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
	    درج_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, شروع - s, p - s);
	    شروع = عدم;
	}
    }

    إذا (شروع != عدم)
	درج_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, شروع - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    ساكن صحيح
شمار_cluster_cells(
	char_u		    *s,
	Pango_قلم	    *قلم,
	PangoGlyphString    *glyphs,
	صحيح		    i,
	صحيح		    *cluster_عرض,
	صحيح		    *آخرین_glyph_rbearing)
{
    char_u  *p;
    صحيح	    بعدی;
    صحيح	    شروع, انتها;
    صحيح	    عرض;
    صحيح	    uc;
    صحيح	    cellcount = 0;

    عرض = glyphs->glyphs[i].geometry.عرض;

    لكل (بعدی = i + 1; بعدی < glyphs->رقم_glyphs; ++بعدی)
    {
	إذا (glyphs->glyphs[بعدی].attr.is_cluster_شروع)
	    اكسر;
	وإلا إذا (glyphs->glyphs[بعدی].geometry.عرض > عرض)
	    عرض = glyphs->glyphs[بعدی].geometry.عرض;
    }

    شروع = قلم->جابه_جایی + glyphs->log_clusters[i];
    انتها   = قلم->جابه_جایی + ((بعدی < glyphs->رقم_glyphs) ?
	    glyphs->log_clusters[بعدی] : قلم->درازی);

    لكل (p = s + شروع; p < s + انتها; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	إذا (uc < 0x80)
	    ++cellcount;
	وإلا إذا (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    إذا (آخرین_glyph_rbearing != عدم
	    && cellcount > 0 && بعدی == glyphs->رقم_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(قلم->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, عدم);

	إذا (PANGO_RBEARING(ink_rect) > 0)
	    *آخرین_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    إذا (cellcount > 0)
	*cluster_عرض = عرض;

    أرجع cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    ساكن فراغ
راهاندازی_zero_عرض_cluster(
	Pango_قلم	*قلم,
	PangoGlyphInfo	*glyph,
	صحيح		آخرین_cellcount,
	صحيح		آخرین_cluster_عرض,
	صحيح		آخرین_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحيح		    عرض;

    عرض = آخرین_cellcount * gui.char_عرض * PANGO_SCALE;
    glyph->geometry.x_جابه_جایی = -عرض + MAX(0, عرض - آخرین_cluster_عرض) / 2;
    glyph->geometry.عرض = 0;

    pango_font_get_glyph_extents(قلم->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    إذا (ink_rect.x < 0)
    {
	glyph->geometry.x_جابه_جایی += آخرین_glyph_rbearing;
	glyph->geometry.y_جابه_جایی  = logical_rect.ارتفاع
	    - (gui.char_ارتفاع - p_linespace) * PANGO_SCALE;
    }
    وإلا
	glyph->geometry.x_جابه_جایی = -عرض + MAX(0, عرض - ink_rect.عرض) / 2;
}

#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    ساكن DrawSign *
draw_sign_new(GdkTexture *texture, صحيح عرض, صحيح ارتفاع)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->عرض = عرض;
    dsign->ارتفاع = ارتفاع;

    أرجع dsign;
}

    ساكن فراغ
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#پایان_اگر

    ساكن فراغ
draw_attr_init(
	DrawAttr	*dattr,
	ثابت GdkRGBA	*bg_color,
	ثابت GdkRGBA	*fg_color,
	ثابت GdkRGBA	*sp_color,
	صحيح		draw_flags)
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
    ساكن فراغ
draw_attr_apply(DrawAttr *dattr, صحيح ستون, Draw_ردیف *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + ستون;
    uint32_t	*متن_cell = drow->dlayers[DRAW_LAYER_متن].buf + ستون;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    صحيح		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	ستون + UNDERC_جابه_جایی(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	ستون + STRIKE_جابه_جایی(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    إذا (dattr != عدم)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.پشت_pixel, bg_color);
    }

    إذا (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_ردیف_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

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
	draw_ردیف_dirty_layer(drow, DRAW_LAYER_متن);
	إذا (draw_flags & DRAW_UNDERL)
	    *متن_cell = fg_color;
	وإلا
	    *متن_cell = 0;
    }
    إذا ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_ردیف_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	إذا (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	وإلا
	    *underc_cell = 0;
    }
    إذا ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_ردیف_dirty_layer(drow, DRAW_LAYER_SPECIAL);
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
#پایان_اگر

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
#پایان_اگر
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
		|| a->glyphs[i].geometry.عرض != b->glyphs[i].geometry.عرض
		|| a->glyphs[i].geometry.x_جابه_جایی != b->glyphs[i].geometry.x_جابه_جایی
		|| a->glyphs[i].geometry.y_جابه_جایی != b->glyphs[i].geometry.y_جابه_جایی
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
    ساكن Gsk_Render_گره *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	صحيح	    ردیف,
	VimDrawArea *da,
	صحيح	    off,
	gboolean    bleed)
{
    GBytes	    *بایتها;
    GdkTexture	    *texture;
    Gsk_Render_گره   *گره;
    صحيح		    ارتفاع;

#إذا GTK_CHECK_نسخه(4,24,0)
    ارتفاع = gui.char_ارتفاع;
#وإلا
    ارتفاع = gui.char_ارتفاع + 1;
#پایان_اگر

    إذا (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		حجم(uint32_t) * da->n_تعداد_ستون);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_تعداد_ستون - 1)),
		حجم(uint32_t)); // Add bleed using last pixel of buffer
	بایتها = g_byte_array_free_مقصد_بایتها(arr);
    }
    وإلا
	بایتها = g_بایتها_new(dlayer->buf + off, حجم(uint32_t) * da->n_تعداد_ستون);

    texture = gdk_حافظه_texture_new(da->n_تعداد_ستون + bleed, 1,
	    GDK_حافظه_R8_G8_B8_A8, بایتها, (da->n_تعداد_ستون + bleed) * 4);
    g_بایتها_unref(بایتها);

    // Scale texture to actual size
    گره = gsk_texture_scale_گره_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(ردیف),
		(da->n_تعداد_ستون + bleed) * gui.char_عرض, gui.char_ارتفاع + bleed),
	    GSK_SCALING_FILTER_نزدیکترین);
    إذا (bleed)
    {
	Gsk_Render_گره *new;

	new = gsk_clip_گره_new(گره,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(ردیف),
		    da->n_تعداد_ستون * gui.char_عرض + da->bleed_راست, ارتفاع));
	gsk_render_گره_unref(گره);
	گره = new;
    }
    g_object_unref(texture);
    أرجع گره;
}

    ساكن فراغ
draw_ردیف_init(Draw_ردیف *drow, صحيح ردیف, صحيح تعداد_ستون)
{
    drow->cells = g_malloc0_n(تعداد_ستون, حجم(DrawCell));
    drow->n_cells = تعداد_ستون;
    drow->ردیف = ردیف;
    drow->underl_ماسک = drow->underc_ماسک = drow->strike_ماسک = عدم;

    memset(drow->dlayers, 0, حجم(DrawLayer) * N_DRAW_LAYERS);
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	إذا (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(تعداد_ستون * 2, 4);
	وإلا
	    dlayer->buf = g_malloc0_n(تعداد_ستون, 4);
    }
}

    ساكن فراغ
draw_ردیف_clear(Draw_ردیف *drow)
{
    g_free(drow->cells);
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	إذا (dlayer->گره != عدم)
	    gsk_render_گره_unref(dlayer->گره);
	g_free(dlayer->buf);
    }
    g_clear_اشاره_گر(&drow->underl_ماسک, gsk_render_گره_unref);
    g_clear_اشاره_گر(&drow->underc_ماسک, gsk_render_گره_unref);
    g_clear_اشاره_گر(&drow->strike_ماسک, gsk_render_گره_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    ساكن فراغ
draw_ردیف_dirty_layer(Draw_ردیف *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_اشاره_گر(&dlayer->گره, gsk_render_گره_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    ساكن فراغ
draw_ردیف_uninvert_cell(Draw_ردیف *drow, صحيح ستون)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    إذا (buf[ستون] == 0xffffffff)
    {
	draw_ردیف_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[ستون] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    ساكن فراغ
draw_ردیف_مجموعه_cell(Draw_ردیف *drow, صحيح ستون, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + ستون;

    إذا (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_ردیف_dirty_layer(drow, DRAW_LAYER_متن);

    // When drawing double width characters in the snapshot vfunc, pointer
    // equality of the DrawGlyphs is used. Must make sure to always keep the
    // pointer in sync, even if glyphs are technically the "same".
    إذا (dcell->dglyphs != dglyphs)
    {
	إذا (dcell->dglyphs != عدم)
	    draw_glyphs_unref(dcell->dglyphs);
	dcell->dglyphs = dglyphs == عدم ? عدم : draw_glyphs_ref(dglyphs);
    }

    draw_attr_apply(dattr, ستون, drow);
    draw_ردیف_uninvert_cell(drow, ستون);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    ساكن فراغ
draw_ردیف_fill(
	Draw_ردیف	    *drow,
	صحيح	    col1,
	صحيح	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    لكل (صحيح c = col1; c <= col2; c++)
	draw_ردیف_مجموعه_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    إذا (gui.cursor_ردیف == drow->ردیف && gui.cursor_ستون >= col1
	    && gui.cursor_ستون <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    ساكن فراغ
draw_ردیف_move_مقصد(
	Draw_ردیف	    *dest_ردیف,
	Draw_ردیف	    *src_ردیف,
	صحيح	    col1,
	صحيح	    col2,
	VimDrawArea *da)
{
    صحيح move_اندازه = (col2 - col1 + 1) * حجم(DrawCell);

    draw_ردیف_fill(dest_ردیف, col1, col2, عدم, عدم, &da->cursor);
    memmove(dest_ردیف->cells + col1, src_ردیف->cells + col1, move_اندازه);
    // NULL the draw cells so we don't double unreference.
    memset(src_ردیف->cells + col1, 0, move_اندازه);
    draw_ردیف_dirty_layer(src_ردیف, DRAW_LAYER_متن);

    // Move the layer buffers
    move_اندازه = (col2 - col1 + 1) * 4;
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_ردیف->dlayers[l].buf;
	uint32_t *dest = dest_ردیف->dlayers[l].buf;

	إذا (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_جابه_جایی(dest_ردیف->n_cells),
		    src + col1 + UNDERC_جابه_جایی(src_ردیف->n_cells), move_اندازه);
	    memmove(dest + col1 + STRIKE_جابه_جایی(dest_ردیف->n_cells),
		    src + col1 + STRIKE_جابه_جایی(src_ردیف->n_cells), move_اندازه);
	}
	وإلا
	    memmove(dest + col1, src + col1, move_اندازه);
	draw_ردیف_dirty_layer(dest_ردیف, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    ساكن فراغ
draw_ردیف_render_background(Draw_ردیف *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    إذا (!dlayer->dirty)
	أرجع;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    إذا (rgba_is_transparent(dlayer->buf, da->n_تعداد_ستون))
	أرجع;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->گره = draw_layer_get_texture(dlayer, drow->ردیف, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    ساكن فراغ
draw_ردیف_ensure_decor(Draw_ردیف *drow, صحيح flags)
{
    إذا (flags & DRAW_UNDERL && drow->underl_ماسک == عدم)
	drow->underl_ماسک = gsk_color_گره_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->ردیف + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    إذا (flags & DRAW_STRIKE && drow->strike_ماسک == عدم)
	drow->strike_ماسک = gsk_color_گره_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->ردیف) + (صحيح)(gui.char_ارتفاع / 2),
		    FILL_X(drow->n_cells), 1));

    إذا (flags & DRAW_UNDERC && drow->underc_ماسک == عدم)
    {
	ساكن ثابت صحيح val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	صحيح y = FILL_Y(drow->ردیف + 1) - 1;
	صحيح x_شروع = FILL_X(0);
	صحيح x_انتها = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_گره *فرزند = عدم;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#إذا GTK_CHECK_نسخه(4, 14, 0)
	Gsk_مسیر_Builder	*builder;
	Gsk_مسیر		*مسیر;
	GskStroke       *stroke;
	Gsk_Render_گره	*color_گره;
	graphene_rect_t bounds;

	builder = gsk_مسیر_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_مسیر_builder_move_مقصد(builder, -1, y - val[7] + 0.5);
	gsk_مسیر_builder_سطر_مقصد(builder, 0, y - val[0] + 0.5);

	لكل (صحيح i = 1; i < 8; i++)
	    gsk_مسیر_builder_سطر_مقصد(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_مسیر_builder_سطر_مقصد(builder, 8, y - val[0] + 0.5);
	gsk_مسیر_builder_سطر_مقصد(builder, 9, y - val[1] + 0.5);

	مسیر = gsk_مسیر_builder_free_مقصد_مسیر(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_مسیر_get_stroke_bounds(مسیر, stroke, &bounds);
	color_گره = gsk_color_گره_new(&white_rgba, &bounds);
	فرزند = gsk_stroke_گره_new(color_گره, مسیر, stroke);

	gsk_stroke_free(stroke);
	gsk_مسیر_unref(مسیر);
	gsk_render_گره_unref(color_گره);
#وإلا
	cairo_t *cr;

	فرزند = gsk_cairo_گره_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_گره_get_draw_context(فرزند);

	cairo_مجموعه_سطر_عرض(cr, 1.0);
	cairo_مجموعه_منبع_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_مقصد(cr, -1, y - val[7] + 0.5);
	cairo_سطر_مقصد(cr, 0, y - val[0] + 0.5);

	لكل (صحيح i = 1; i < 8; ++i)
	    cairo_سطر_مقصد(cr, i, y - val[i] + 0.5);

	cairo_سطر_مقصد(cr, 8, y - val[0] + 0.5);
	cairo_سطر_مقصد(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#پایان_اگر
	drow->underc_ماسک = gsk_repeat_گره_new(
		&GRAPHENE_RECT_INIT(x_شروع, y - 3, x_انتها - x_شروع, 5),
		فرزند, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_گره_unref(فرزند);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    ساكن فراغ
draw_ردیف_render_متن(Draw_ردیف *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_متن;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = عدم;
    صحيح		شروع_ستون = 0;
    صحيح		خالی_cells = 0;
    uint32_t	cur_fg = 0;
    صحيح		c;

    إذا (!dlayer->dirty)
	أرجع;

    glyph_buf = da->glyph_buf;
    nodes = da->گره_buf;

    g_array_مجموعه_اندازه(glyph_buf, 0);
    g_ptr_array_مجموعه_اندازه(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#تعریف_کن FLUSH_گره() \
    افعل { \
	إذا (cur_font != عدم) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_گره	*گره; \
	    GdkRGBA		fg_color = u32_مقصد_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->داده; \
	    glyph_str.رقم_glyphs = glyph_buf->len; \
	    گره = gsk_متن_گره_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_نقطه_INIT(متن_X(شروع_ستون), \
			متن_Y(drow->ردیف))); \
	    إذا (گره != عدم) \
		g_ptr_array_جمع_کن(nodes, گره); \
	    g_array_مجموعه_اندازه(glyph_buf, 0); \
	    cur_font = عدم; \
	} \
    } طالما (FALSE)

    لكل (c = 0; c < da->n_تعداد_ستون; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	صحيح	    prev_len;

	إذا (dglyphs == عدم)
	{
	    خالی_cells++;
	    استمر;
	}
#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	وإلا إذا (dglyphs->font == عدم)
	{
	    // Add sign icon
	    Gsk_Render_گره   *snode;
	    صحيح		    w;

	    FLUSH_گره();

	    snode = gsk_texture_scale_گره_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->ردیف),
			dglyphs->dsign->عرض, dglyphs->dsign->ارتفاع),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->عرض + gui.char_عرض - 1) / gui.char_عرض;

	    إذا (snode != عدم)
		g_ptr_array_جمع_کن(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    استمر;
	}
#پایان_اگر
	وإلا إذا (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_گره();
	    cur_font = dglyphs->font;
	    شروع_ستون = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	إذا (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    استمر;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	إذا (خالی_cells > 0)
	{
	    إذا (prev_len > 0)
	    {
		// Spaces occurred between valid text: extend
		// previous character's advance
		PangoGlyphInfo *آخرین = &g_array_شاخص(
			glyph_buf, PangoGlyphInfo,
			prev_len - 1);
		آخرین->geometry.عرض += CELLS2PANGO(خالی_cells);
	    }
	    خالی_cells = 0; // Width consumed
	}
    }

    FLUSH_گره();

#لغو_تعریف FLUSH_گره

    إذا (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_گره *منبع;
	Gsk_Render_گره *گره;

	draw_ردیف_ensure_decor(drow, DRAW_UNDERL);
	إذا (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    منبع = draw_layer_get_texture(dlayer, drow->ردیف, da, 0, FALSE);
	    گره = gsk_ماسک_گره_new(منبع, drow->underl_ماسک,
		    GSK_ماسک_نما_ALPHA);
	    gsk_render_گره_unref(منبع);
	    g_ptr_array_جمع_کن(nodes, گره);
	}

	dlayer->گره = gsk_container_گره_new(
		(Gsk_Render_گره **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    ساكن فراغ
draw_ردیف_render_special(Draw_ردیف *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_گره   *منبع;
    Gsk_Render_گره   *nodes[2];
    صحيح		    n_nodes = 0;

    إذا (!dlayer->dirty)
	أرجع;

    draw_ردیف_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    إذا (!rgba_is_transparent(dlayer->buf + UNDERC_جابه_جایی(drow->n_cells),
		drow->n_cells))
    {
	منبع = draw_layer_get_texture(dlayer, drow->ردیف, da,
		UNDERC_جابه_جایی(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_ماسک_گره_new(منبع, drow->underc_ماسک,
		GSK_ماسک_نما_ALPHA);
	gsk_render_گره_unref(منبع);
    }

    إذا (!rgba_is_transparent(dlayer->buf + STRIKE_جابه_جایی(drow->n_cells),
		drow->n_cells))
    {
	منبع = draw_layer_get_texture(dlayer, drow->ردیف, da,
		STRIKE_جابه_جایی(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_ماسک_گره_new(منبع, drow->strike_ماسک,
		GSK_ماسک_نما_ALPHA);
	gsk_render_گره_unref(منبع);
    }

    إذا (n_nodes == 1)
    {
	dlayer->گره = nodes[0];
    }
    وإلا إذا (n_nodes > 0)
    {
	dlayer->گره = gsk_container_گره_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	لكل (صحيح i = 0; i < (صحيح)ARRAY_درازی(nodes); i++)
	    gsk_render_گره_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    ساكن فراغ
draw_ردیف_render_overlay(Draw_ردیف *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    إذا (!dlayer->dirty)
	أرجع;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    إذا (rgba_is_transparent(dlayer->buf, da->n_تعداد_ستون))
	أرجع;

    dlayer->گره = draw_layer_get_texture(dlayer, drow->ردیف, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    ساكن فراغ
draw_ردیف_جمع_کن_cluster(
	Draw_ردیف		*drow,
	DrawAttr	*dattr,
	صحيح		ستون,
	صحيح		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	صحيح		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = عدم;

    إذا (ستون + n_cells > da->n_تعداد_ستون)
	أرجع;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    إذا (glyphs != عدم)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_ردیف_fill(drow, ستون, ستون + n_cells - 1, dglyphs, dattr, &da->cursor);

    إذا (dglyphs != عدم)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    ساكن صحيح
vim_draw_area_جمع_کن_string(
	VimDrawArea *self,
	Draw_ردیف	    *drow,
	DrawAttr    *dattr,
	صحيح	    ستون,
	char_u	    *s,
	صحيح	    len,
	صحيح	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = عدم;
    صحيح			شروع_idx = -1;
    GList		*قلم_list;
    صحيح			ستونی_جابه_جایی = 0;
    صحيح			cluster_عرض;
    صحيح			آخرین_glyph_rbearing;
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
	    درج_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
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

	    draw_ردیف_جمع_کن_cluster(drow, dattr, ستون + i, 1,
		    gui.ascii_font, has_ink ? &glyph : عدم, 1, self);
	}

	ستونی_جابه_جایی = len;
	اذهب خروج;
    }

    glyphs = pango_glyph_string_new();

    إذا (attr_list == عدم)
	attr_list = pango_attr_list_new();

    إذا (شروع_idx != -1)
	درج_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, شروع_idx, len);

    cluster_عرض = PANGO_SCALE * gui.char_عرض;
    آخرین_glyph_rbearing = PANGO_SCALE * gui.char_عرض;

    // If 'guifontwide' is set then use that for double-width characters.
    إذا (gui.wide_font != عدم)
	apply_wide_font_attr(s, len, attr_list);

    إذا ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	درج_PANGO_ATTR(pango_attr_وزن_new(PANGO_وزن_BOLD),
		attr_list, 0, len);
    إذا (draw_flags & DRAW_ITALIC)
	درج_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    قلم_list = pango_itemize(gui.متن_context,
	    (ثابت حرف *)s, 0, len, attr_list, عدم);

    طالما (قلم_list != عدم)
    {
	Pango_قلم   *قلم = قلم_list->داده;
	صحيح	    قلم_cells = 0;
	صحيح	    cluster_شروع_idx = 0;
	صحيح	    current_cluster_ستون = ستون + ستونی_جابه_جایی;
	صحيح	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	قلم_list = g_list_delete_link(قلم_list, قلم_list);

	// Force LTR direction; Vim handles bidi on its own.
	قلم->analysis.level = (قلم->analysis.level + 1) & (~1U);

	pango_shape_پر((ثابت حرف *)s + قلم->جابه_جایی, قلم->درازی,
		(ثابت حرف *)s, len, &قلم->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	لكل (i = 0; i < glyphs->رقم_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    إذا (glyph->attr.is_cluster_شروع)
	    {
		صحيح cellcount;

		cellcount = شمار_cluster_cells(
			s, قلم, glyphs, i, &cluster_عرض,
			(قلم_list != عدم) ? &آخرین_glyph_rbearing : عدم);

		إذا (i > 0 && cellcount > 0)
		{
		    draw_ردیف_جمع_کن_cluster(drow, dattr,
			    current_cluster_ستون, current_cluster_cells,
			    قلم->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_شروع_idx : عدم,
			    i - cluster_شروع_idx, self);

		    current_cluster_ستون += current_cluster_cells;
		    cluster_شروع_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		إذا (cellcount > 0)
		{
		    صحيح عرض;

		    عرض = cellcount * gui.char_عرض * PANGO_SCALE;
		    glyph->geometry.x_جابه_جایی +=
			MAX(0, عرض - cluster_عرض) / 2;
		    glyph->geometry.عرض = عرض;
		}
		وإلا
		    راهاندازی_zero_عرض_cluster(قلم, glyph, cells,
			    cluster_عرض,
			    آخرین_glyph_rbearing);

		قلم_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    وإلا إذا (i > 0)
	    {
		صحيح عرض;

		إذا (glyph->geometry.x_جابه_جایی >= 0)
		{
		    glyphs->glyphs[i].geometry.عرض =
			glyphs->glyphs[i - 1].geometry.عرض;
		    glyphs->glyphs[i - 1].geometry.عرض = 0;
		}
		عرض = cells * gui.char_عرض * PANGO_SCALE;
		glyph->geometry.x_جابه_جایی +=
		    MAX(0, عرض - cluster_عرض) / 2;
	    }
	    وإلا
		glyph->geometry.عرض = 0;


	    إذا (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			قلم->analysis.font, *glyph);
	}
	إذا (glyphs->رقم_glyphs > 0)
	    draw_ردیف_جمع_کن_cluster(drow, dattr,
		    current_cluster_ستون, current_cluster_cells,
		    قلم->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_شروع_idx : عدم,
		    glyphs->رقم_glyphs - cluster_شروع_idx, self);

	pango_قلم_free(قلم);
	ستونی_جابه_جایی += قلم_cells;
    }

    pango_glyph_string_free(glyphs);

خروج:
    إذا (attr_list != عدم)
	pango_attr_list_unref(attr_list);

    أرجع ستونی_جابه_جایی;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    صحيح
vim_draw_area_draw_string(
	VimDrawArea *self,
	صحيح	    ردیف,
	صحيح	    ستون,
	char_u	    *s,
	صحيح	    len,
	صحيح	    flags)
{
    Draw_ردیف	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = عدم;
    صحيح		convlen;
    صحيح		n_cells ;

    إذا (unlikely(self->ردیفها_2 == عدم
		|| ردیف >= self->n_ردیفها
		|| ستون >= self->n_تعداد_ستون))
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

    drow = self->ردیفها_2 + ردیف;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_جمع_کن_string(self, drow, &dattr, ستون, s, len, flags);

    vim_free(conv_buf);

    أرجع n_cells;
}

/*
 * Clear out the block with the given bounds (inclusive).
 */
    فراغ
vim_draw_area_clear_بلوک(
	VimDrawArea	*self,
	صحيح		row1,
	صحيح		col1,
	صحيح		row2,
	صحيح		col2)
{
    إذا (unlikely(self->ردیفها_2 == عدم
		|| row1 >= self->n_ردیفها
		|| col1 >= self->n_تعداد_ستون
		|| row2 >= self->n_ردیفها
		|| col2 >= self->n_تعداد_ستون))
	أرجع;

    لكل (صحيح r = row1; r <= row2; r++)
	draw_ردیف_fill(self->ردیفها_2 + r, col1, col2, عدم, عدم, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    فراغ
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_بلوک(self, 0, 0, self->n_ردیفها - 1, self->n_تعداد_ستون - 1);
}

/*
 * Move the given rows between "row1" and "row2", within the column "col1" and
 * "col2" (making a rectangle region), to the row "to". The previous region that
 * was moved is cleared.
 */
    فراغ
vim_draw_area_move_بلوک(
	VimDrawArea *self,
	صحيح	    مقصد,
	صحيح	    row1,
	صحيح	    row2,
	صحيح	    col1,
	صحيح	    col2)
{
    صحيح جابه_جایی = row2 - row1;

    إذا (unlikely(self->ردیفها_2 == عدم
		|| row1 >= self->n_ردیفها
		|| row2 >= self->n_ردیفها
		|| مقصد >= self->n_ردیفها
		|| col1 >= self->n_تعداد_ستون
		|| col2 >= self->n_تعداد_ستون))
	أرجع;

    أكد(row2 >= row1);
    أكد(col2 >= col1);
    أكد(row1 != مقصد);

    إذا (row1 > مقصد)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	لكل (صحيح o = 0; o <= جابه_جایی; o++)
	    draw_ردیف_move_مقصد(self->ردیفها_2 + مقصد + o, self->ردیفها_2 + row1 + o,
		    col1, col2, self);
    }
    وإلا
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	لكل (صحيح o = جابه_جایی; o >= 0; o--)
	    إذا (مقصد + o >= self->n_ردیفها)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_بلوک(self, row1 + o, col1, row1 + o, col2);
	    وإلا
		draw_ردیف_move_مقصد(self->ردیفها_2 + مقصد + o, self->ردیفها_2 + row1 + o,
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
vim_draw_area_مجموعه_cursor(VimDrawArea *self, صحيح w, صحيح h)
{
    self->cursor.draw = TRUE;
    self->cursor.عرض = w;
    self->cursor.ارتفاع = h;
    // Remember this now: the snapshot runs later, when the drawing position
    // has moved on.
    self->cursor.n_cells = 1 + mb_lefthalve(gui.cursor_ردیف, gui.cursor_ستون);
    self->cursor.bg_color = *gui.bgcolor;
    self->cursor.fg_color = *gui.fgcolor;
}

/*
 * Invert the rectangle in the draw area.
 */
    فراغ
vim_draw_area_invert_بلوک(
	VimDrawArea	*self,
	صحيح		ردیف,
	صحيح		ستون,
	صحيح		nrows,
	صحيح		ncols)
{
    إذا (unlikely(self->ردیفها_2 == عدم
		|| ردیف >= self->n_ردیفها
		|| ستون >= self->n_تعداد_ستون
		|| ردیف + nrows - 1 >= self->n_ردیفها
		|| ستون + ncols - 1 >= self->n_تعداد_ستون))
	أرجع;

    لكل (صحيح r = ردیف; r < ردیف + nrows; r++)
    {
	Draw_ردیف	    *drow = self->ردیفها_2 + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	لكل (صحيح c = 0; c < ncols; c++)
	    dlayer->buf[c + ستون] = ~dlayer->buf[c + ستون];
	draw_ردیف_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#إذا defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    فراغ
vim_draw_area_جمع_کن_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	صحيح	    ردیف,
	صحيح	    ستون,
	صحيح	    عرض,
	صحيح	    ارتفاع)
{
    Draw_ردیف	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحيح		cells;

    إذا (unlikely(self->ردیفها_2 == عدم
		|| ردیف >= self->n_ردیفها
		|| ستون >= self->n_تعداد_ستون))
	أرجع;

    drow = self->ردیفها_2 + ردیف;
    dsign = draw_sign_new(sign, عرض, ارتفاع);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (عرض + gui.char_عرض - 1) / gui.char_عرض;

    draw_ردیف_fill(drow, ستون, ستون + cells - 1, dglyphs, عدم, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#پایان_اگر

#اگر_تعریف_شده FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    فراغ
vim_draw_area_جمع_کن_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	صحيح		ردیف,
	صحيح		ستون,
	صحيح		عرض,
	صحيح		ارتفاع)
{
    Draw_ردیف	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحيح		cells;
    GdkTexture	*texture;
    GBytes	*بایتها;

    إذا (unlikely(self->ردیفها_2 == عدم
		|| ردیف >= self->n_ردیفها
		|| ستون >= self->n_تعداد_ستون))
	أرجع;

    cairo_surface_flush(surf);
    بایتها = g_بایتها_new_with_free_func(cairo_image_surface_get_داده(surf),
	    cairo_image_surface_get_ارتفاع(surf)
	    * cairo_image_surface_get_stride(surf),
	    (GDestroyNotify)cairo_surface_destroy,
	    cairo_surface_reference(surf));

    texture = gdk_حافظه_texture_new(cairo_image_surface_get_عرض(surf),
	    cairo_image_surface_get_ارتفاع(surf),
	    GDK_حافظه_A8_R8_G8_B8_PREMULTIPLIED,
	    بایتها, cairo_image_surface_get_stride(surf));
    g_بایتها_unref(بایتها);

    drow = self->ردیفها_2 + ردیف;
    dsign = draw_sign_new(texture, عرض, ارتفاع);
    g_object_unref(texture);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (عرض + gui.char_عرض - 1) / gui.char_عرض;

    draw_ردیف_fill(drow, ستون, ستون + cells - 1, dglyphs, عدم, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#پایان_اگر

#اگر_تعریف_شده FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    فراغ
vim_draw_area_جمع_کن_external(VimDrawArea *self, Gsk_Render_گره *گره)
{
    g_hash_جدول_جمع_کن(self->external, gsk_render_گره_ref(گره));
}

    فراغ
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_گره *گره)
{
    g_hash_جدول_remove(self->external, گره);
}

#پایان_اگر

/*
 * Add the cursor to the snapshot.
 */
    ساكن فراغ
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_گره	*متن_گره,	// May be NULL
	Gsk_Render_گره	*special_گره,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    صحيح		    w = cursor->عرض;
    صحيح		    h = cursor->ارتفاع;
    graphene_rect_t rect;

    إذا (!cursor->draw)
	أرجع;

    إذا (cursor->عرض <= 0 && cursor->ارتفاع <= 0)
    {
	w += gui.char_عرض * cursor->n_cells;
	h = gui.char_ارتفاع;
    }

    graphene_rect_init(&rect,
#اگر_تعریف_شده FEAT_RIGHTLEFT
	    CURSOR_BAR_راست ? FILL_X(gui.cursor_ستون + 1) - w :
#پایان_اگر
	    FILL_X(gui.cursor_ستون),
	    FILL_Y(gui.cursor_ردیف) + gui.char_ارتفاع - h, w, h);


    إذا (cursor->عرض == -1 && cursor->ارتفاع == -1)
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

	gtk_snapshot_افزودن_ماسک(snapshot, GSK_ماسک_نما_ALPHA);

	// Used as an "outline" for the fg color.
	إذا (متن_گره != عدم)
	    gtk_snapshot_append_گره(snapshot, متن_گره);
	إذا (special_گره != عدم)
	    gtk_snapshot_append_گره(snapshot, special_گره);

	gtk_snapshot_برداشت(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_برداشت(snapshot);
    }
}

    ساكن فراغ
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    صحيح		    ارتفاع, عرض;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = عدم;
    Gsk_Render_گره   *body_گره;
    Gsk_Render_گره   *invert_گره = عدم;
#اگر_تعریف_شده FEAT_IMAGE_GUI
    Gsk_Render_گره   *ext_گره;
    GHash_جدول_Iter  iter;
#پایان_اگر

    gui_mch_مجموعه_bg_color(gui.پشت_pixel);
    ارتفاع = gtk_widget_get_ارتفاع(widget) + gui.bleed_bot;
    عرض = gtk_widget_get_عرض(widget) + gui.bleed_راست;

    إذا (self->ردیفها_2 == عدم)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, عرض, ارتفاع));
	أرجع;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    إذا (self->bleed_راست == -1 || self->bleed_راست != gui.bleed_راست)
    {
	self->bleed_راست = gui.bleed_راست;
	لكل (صحيح r = 0; r < self->n_ردیفها; r++)
	{
	    draw_ردیف_dirty_layer(self->ردیفها_2 + r, DRAW_LAYER_BACKGROUND);
	    draw_ردیف_dirty_layer(self->ردیفها_2 + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#إذا GTK_CHECK_نسخه(4,24,0)
    gtk_snapshot_مجموعه_snap(body_snapshot, GSK_RECT_SNAP_دور);
#پایان_اگر

    لكل (صحيح r = 0; r < self->n_ردیفها; r++)
    {
	Draw_ردیف *drow = self->ردیفها_2 + r;

	draw_ردیف_render_background(drow, self);
	draw_ردیف_render_متن(drow, self);
	draw_ردیف_render_special(drow, self);
	draw_ردیف_render_overlay(drow, self);

	لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    إذا (dlayer->گره != عدم)
	    {
		إذا (l == DRAW_LAYER_OVERLAY)
		{
		    إذا (invert_snapshot == عدم)
		    {
			invert_snapshot = gtk_snapshot_new();
#إذا GTK_CHECK_نسخه(4,24,0)
			gtk_snapshot_مجموعه_snap(invert_snapshot,
				GSK_RECT_SNAP_دور);
#پایان_اگر
		    }
		    gtk_snapshot_append_گره(invert_snapshot, dlayer->گره);
		}
		وإلا
		    gtk_snapshot_append_گره(body_snapshot, dlayer->گره);
	    }
	}

	إذا (r == gui.cursor_ردیف)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_متن].گره,
		    drow->dlayers[DRAW_LAYER_SPECIAL].گره,
		    body_snapshot);
    }

    إذا (invert_snapshot != عدم)
	gtk_snapshot_افزودن_blend(snapshot, GSK_BLEND_نما_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, عرض, ارتفاع));
    body_گره = gtk_snapshot_free_مقصد_گره(body_snapshot);
    إذا (body_گره != عدم)
    {
	gtk_snapshot_append_گره(snapshot, body_گره);
	gsk_render_گره_unref(body_گره);
    }

#اگر_تعریف_شده FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_جدول_iter_init(&iter, self->external);

    طالما (g_hash_جدول_iter_بعدی(&iter, (فراغ **)&ext_گره, عدم))
	gtk_snapshot_append_گره(snapshot, ext_گره);
#پایان_اگر

    إذا (invert_snapshot != عدم)
    {
	gtk_snapshot_برداشت(snapshot);
	invert_گره = gtk_snapshot_free_مقصد_گره(invert_snapshot);
	// Not sure if it can be NULL but still check
	إذا (invert_گره != عدم)
	{
	    gtk_snapshot_append_گره(snapshot, invert_گره);
	    gsk_render_گره_unref(invert_گره);
	}
	gtk_snapshot_برداشت(snapshot);
    }
}
