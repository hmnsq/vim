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
    صحيح		پلنوالی;
    صحيح		لوړوالی;
} DrawSign;
#د_که_پای

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
#د_که_پای
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
    Gsk_Render_غوټه   *غوټه;
    gboolean	    dirty;

    // RGBA buffer, pixel size is number of cells in the row. Note that for the
    // special layer, it is twice the size, first part is for undercurl, second
    // part for strikethrough.
    uint32_t *buf;
} DrawLayer;

#تعریف_کړه UNDERC_دننه_واټن(n_ستون_شمېر) (0)
#تعریف_کړه STRIKE_دننه_واټن(n_ستون_شمېر) (n_ستون_شمېر)

تعريفنوع بنية
{
    DrawLayer	dlayers[N_DRAW_LAYERS];
    DrawCell	*cells;
    صحيح		n_cells; // For convenience
    صحيح		لیکه;	 // For convenience

    // Pre generated mask of of under decorations
    Gsk_Render_غوټه *underl_ماسک;
    Gsk_Render_غوټه *underc_ماسک;
    Gsk_Render_غوټه *strike_ماسک;
} Draw_لیکه;

#تعریف_کړه ورننباسه_PANGO_ATTR(Attribute, AttrList, پیل_2, پای_ټکی_2) \
    افعل { \
	PangoAttribute *tmp_attr_; \
	tmp_attr_ = (Attribute); \
	tmp_attr_->پیل_شاخص = (پیل_2); \
	tmp_attr_->پای_ټکی_شاخص = (پای_ټکی_2); \
	pango_attr_list_ورننباسه((AttrList), tmp_attr_); \
    } طالما (FALSE)
#تعریف_کړه CELLS2PANGO(c) ((c) * gui.char_پلنوالی * PANGO_SCALE)

/*
 * Attributes of cursor
 */
تعريفنوع بنية
{
    gboolean draw; // If cursor should be drawn
    صحيح پلنوالی;
    صحيح لوړوالی;
    صحيح n_cells; // Cells covered, 2 for a double width character
    GdkRGBA bg_color;
    GdkRGBA fg_color;
} DrawCursor;

بنية _VimDrawArea
{
    GtkWidget پلار;

    Draw_لیکه *لیکې_2;

    صحيح n_لیکې;
    صحيح	n_ستون_شمېر;

    صحيح	bleed_ښی;

    // Used for snapshot vfunc call when rendering text, kept around so there is
    // no need to allocate a new one each time.
    GArray	*glyph_buf;
    GPtrArray	*غوټه_buf;

    DrawCursor cursor;

#که_تعریف_شوی FEAT_IMAGE_GUI
    // Queue of external render nodes
    GHash_جدول *external;
#د_که_پای
};

ساكن فراغ draw_لیکه_init(Draw_لیکه *drow, صحيح لیکه, صحيح ستون_شمېر);
ساكن فراغ draw_لیکه_clear(Draw_لیکه *drow);
ساكن فراغ draw_لیکه_dirty_layer(Draw_لیکه *drow, DrawLayerType dlayer_t);
ساكن فراغ vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);

ساكن GdkRGBA white_rgba = {1, 1, 1, 1};

G_DEFINE_TYPE(VimDrawArea, vim_draw_area, GTK_TYPE_WIDGET)

    ساكن فراغ
vim_draw_area_finalize(GObject *obj)
{
    VimDrawArea *self = VIM_DRAW_AREA(obj);

    vim_draw_area_clear(self);
    لكل (صحيح r = 0; r < self->n_لیکې; r++)
	draw_لیکه_clear(self->لیکې_2 + r);
    g_free(self->لیکې_2);

    g_array_free(self->glyph_buf, TRUE);
    g_ptr_array_free(self->غوټه_buf, TRUE);

#که_تعریف_شوی FEAT_IMAGE_GUI
    g_hash_جدول_unref(self->external);
#د_که_پای

    G_OBJECT_CLASS(vim_draw_area_پلار_class)->finalize(obj);
}

    ساكن فراغ
vim_draw_area_class_init(VimDrawAreaClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_draw_area_snapshot;

    obj_class->finalize = vim_draw_area_finalize;

    // Add a layout manager so it can handle child popovers
    gtk_widget_class_سېټ_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

    ساكن فراغ
vim_draw_area_init(VimDrawArea *self)
{
    self->bleed_ښی = -1;
#که_تعریف_شوی FEAT_IMAGE_GUI
    self->external = g_hash_جدول_new_ډک(g_مستقیم_hash, g_مستقیم_equal,
	    عدم, (GDestroyNotify)gsk_render_غوټه_unref);
#د_که_پای
    self->glyph_buf = g_array_new(FALSE, FALSE, حجم(PangoGlyphInfo));
    self->غوټه_buf = g_ptr_array_new_with_free_func(
	    (GDestroyNotify)gsk_render_غوټه_unref);

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
vim_draw_area_سېټ_اندازه(VimDrawArea *self, صحيح لیکې_2, صحيح ستون_شمېر)
{
    إذا (self->لیکې_2 != عدم && self->n_لیکې == لیکې_2 && self->n_ستون_شمېر == ستون_شمېر)
	أرجع;
    إذا (لیکې_2 == 0 || ستون_شمېر == 0)
	أرجع;

    vim_draw_area_clear(self);

    لكل (صحيح r = 0; r < self->n_لیکې; r++)
	draw_لیکه_clear(self->لیکې_2 + r);
    self->لیکې_2 = g_realloc_n(self->لیکې_2, لیکې_2, حجم(Draw_لیکه));
    لكل (صحيح r = 0; r < لیکې_2; r++)
	draw_لیکه_init(self->لیکې_2 + r, r, ستون_شمېر);

    self->n_لیکې = لیکې_2;
    self->n_ستون_شمېر = ستون_شمېر;
}

/*
 * Convert the GdkRGBA to a packed uint32_t.
 */
    ساكن uint32_t
rgba_منزل_u32(ثابت GdkRGBA *rgba)
{
    اتحاد { uint32_t val; uint8_t بایټونه[4]; } color;

    color.بایټونه[0] = (uint8_t)(CLAMP(rgba->red,   0.0, 1.0) * 255.0 + 0.5);
    color.بایټونه[1] = (uint8_t)(CLAMP(rgba->green, 0.0, 1.0) * 255.0 + 0.5);
    color.بایټونه[2] = (uint8_t)(CLAMP(rgba->blue,  0.0, 1.0) * 255.0 + 0.5);
    color.بایټونه[3] = (uint8_t)(CLAMP(rgba->alpha, 0.0, 1.0) * 255.0 + 0.5);

    أرجع color.val;
}

/*
 * Convert the 32 bit RGBA value to a GdkRGBA
 */
    ساكن GdkRGBA
u32_منزل_rgba(uint32_t val)
{
    اتحاد { uint32_t val; uint8_t بایټونه[4]; } packed;
    GdkRGBA color;

    packed.val = val;
    color.red   = packed.بایټونه[0] / 255.0;
    color.green = packed.بایټونه[1] / 255.0;
    color.blue  = packed.بایټونه[2] / 255.0;
    color.alpha = packed.بایټونه[3] / 255.0;
    أرجع color;
}

/*
 * Return TRUE if "rgb" equals "rgba", ignoring the alpha channel.
 */
    ساكن gboolean
color_equal_u32(guicolor_T rgb, uint32_t rgba)
{
    اتحاد { uint32_t val; uint8_t بایټونه[4]; } packed;
    guicolor_T	res;

    packed.val = rgba;
    res = ((guicolor_T)packed.بایټونه[0] << 16) |
	((guicolor_T)packed.بایټونه[1] << 8)  |
	packed.بایټونه[2];
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

    أرجع ink.پلنوالی > 0 && ink.لوړوالی > 0;
}

/*
 * Apply the 'guifontwide' font to double-width characters in the string.
 */
    ساكن فراغ
apply_wide_font_attr(char_u *s, صحيح len, PangoAttrList *attr_list)
{
    char_u  *پیل = عدم;
    char_u  *p;
    صحيح	    uc;

    لكل (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	إذا (پیل == عدم)
	{
	    إذا (uc >= 0x80 && utf_char2cells(uc) == 2)
		پیل = p;
	}
	وإلا إذا (uc < 0x80
		|| (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    ورننباسه_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		    attr_list, پیل - s, p - s);
	    پیل = عدم;
	}
    }

    إذا (پیل != عدم)
	ورننباسه_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
		attr_list, پیل - s, len);
}

/*
 * Count the number of display cells occupied by a glyph cluster.
 */
    ساكن صحيح
شمېر_cluster_cells(
	char_u		    *s,
	Pango_توکی	    *توکی,
	PangoGlyphString    *glyphs,
	صحيح		    i,
	صحيح		    *cluster_پلنوالی,
	صحيح		    *وروستی_glyph_rbearing)
{
    char_u  *p;
    صحيح	    بل;
    صحيح	    پیل, پای_ټکی;
    صحيح	    پلنوالی;
    صحيح	    uc;
    صحيح	    cellcount = 0;

    پلنوالی = glyphs->glyphs[i].geometry.پلنوالی;

    لكل (بل = i + 1; بل < glyphs->عدد_glyphs; ++بل)
    {
	إذا (glyphs->glyphs[بل].attr.is_cluster_پیل)
	    اكسر;
	وإلا إذا (glyphs->glyphs[بل].geometry.پلنوالی > پلنوالی)
	    پلنوالی = glyphs->glyphs[بل].geometry.پلنوالی;
    }

    پیل = توکی->دننه_واټن + glyphs->log_clusters[i];
    پای_ټکی   = توکی->دننه_واټن + ((بل < glyphs->عدد_glyphs) ?
	    glyphs->log_clusters[بل] : توکی->اوږدتیا);

    لكل (p = s + پیل; p < s + پای_ټکی; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	إذا (uc < 0x80)
	    ++cellcount;
	وإلا إذا (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    إذا (وروستی_glyph_rbearing != عدم
	    && cellcount > 0 && بل == glyphs->عدد_glyphs)
    {
	PangoRectangle ink_rect;

	pango_font_get_glyph_extents(توکی->analysis.font,
		glyphs->glyphs[i].glyph,
		&ink_rect, عدم);

	إذا (PANGO_RBEARING(ink_rect) > 0)
	    *وروستی_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    إذا (cellcount > 0)
	*cluster_پلنوالی = پلنوالی;

    أرجع cellcount;
}

/*
 * Handle combining characters that form a zero-width cluster.
 */
    ساكن فراغ
تیاری_zero_پلنوالی_cluster(
	Pango_توکی	*توکی,
	PangoGlyphInfo	*glyph,
	صحيح		وروستی_cellcount,
	صحيح		وروستی_cluster_پلنوالی,
	صحيح		وروستی_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحيح		    پلنوالی;

    پلنوالی = وروستی_cellcount * gui.char_پلنوالی * PANGO_SCALE;
    glyph->geometry.x_دننه_واټن = -پلنوالی + MAX(0, پلنوالی - وروستی_cluster_پلنوالی) / 2;
    glyph->geometry.پلنوالی = 0;

    pango_font_get_glyph_extents(توکی->analysis.font,
	    glyph->glyph,
	    &ink_rect, &logical_rect);
    إذا (ink_rect.x < 0)
    {
	glyph->geometry.x_دننه_واټن += وروستی_glyph_rbearing;
	glyph->geometry.y_دننه_واټن  = logical_rect.لوړوالی
	    - (gui.char_لوړوالی - p_linespace) * PANGO_SCALE;
    }
    وإلا
	glyph->geometry.x_دننه_واټن = -پلنوالی + MAX(0, پلنوالی - ink_rect.پلنوالی) / 2;
}

#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
/*
 * Create a new draw sign icon with the given texture.
 */
    ساكن DrawSign *
draw_sign_new(GdkTexture *texture, صحيح پلنوالی, صحيح لوړوالی)
{
    DrawSign *dsign = g_new0(DrawSign, 1);

    dsign->texture = g_object_ref(texture);
    dsign->پلنوالی = پلنوالی;
    dsign->لوړوالی = لوړوالی;

    أرجع dsign;
}

    ساكن فراغ
draw_sign_free(DrawSign *dsign)
{
    g_object_unref(dsign->texture);
    g_free(dsign);
}
#د_که_پای

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
draw_attr_apply(DrawAttr *dattr, صحيح ستون, Draw_لیکه *drow)
{
    uint32_t	*bg_cell = drow->dlayers[DRAW_LAYER_BACKGROUND].buf + ستون;
    uint32_t	*متن_cell = drow->dlayers[DRAW_LAYER_متن].buf + ستون;
    uint32_t	*underc_cell;
    uint32_t	*strike_cell;
    uint32_t	fg_color = 0, bg_color = 0, sp_color = 0;
    صحيح		draw_flags = 0;
    gboolean	bg_is_default = TRUE;

    underc_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	ستون + UNDERC_دننه_واټن(drow->n_cells);
    strike_cell = drow->dlayers[DRAW_LAYER_SPECIAL].buf +
	ستون + STRIKE_دننه_واټن(drow->n_cells);

    // If "dattr" is NULL, then make the pixels transparent.
    إذا (dattr != عدم)
    {
	fg_color = dattr->fg_color;
	bg_color = dattr->bg_color;
	sp_color = dattr->sp_color;
	draw_flags = dattr->draw_flags;
	bg_is_default = color_equal_u32(gui.شا_pixel, bg_color);
    }

    إذا (*bg_cell != bg_color && !(*bg_cell == 0 && bg_is_default))
    {
	draw_لیکه_dirty_layer(drow, DRAW_LAYER_BACKGROUND);

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
	draw_لیکه_dirty_layer(drow, DRAW_LAYER_متن);
	إذا (draw_flags & DRAW_UNDERL)
	    *متن_cell = fg_color;
	وإلا
	    *متن_cell = 0;
    }
    إذا ((draw_flags & DRAW_UNDERC && *underc_cell != sp_color)
	    || (!(draw_flags & DRAW_UNDERC) && *underc_cell != 0))
    {
	draw_لیکه_dirty_layer(drow, DRAW_LAYER_SPECIAL);
	إذا (draw_flags & DRAW_UNDERC)
	    *underc_cell = sp_color;
	وإلا
	    *underc_cell = 0;
    }
    إذا ((draw_flags & DRAW_STRIKE && *strike_cell != sp_color)
	    || (!(draw_flags & DRAW_STRIKE) && *strike_cell != 0))
    {
	draw_لیکه_dirty_layer(drow, DRAW_LAYER_SPECIAL);
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
#د_که_پای

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
#د_که_پای
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
		|| a->glyphs[i].geometry.پلنوالی != b->glyphs[i].geometry.پلنوالی
		|| a->glyphs[i].geometry.x_دننه_واټن != b->glyphs[i].geometry.x_دننه_واټن
		|| a->glyphs[i].geometry.y_دننه_واټن != b->glyphs[i].geometry.y_دننه_واټن
		|| a->glyphs[i].attr.is_cluster_پیل != b->glyphs[i].attr.is_cluster_پیل)
	    أرجع FALSE;
    أرجع TRUE;
}

/*
 * Convert the buffer for the given layer to a texture node, scaled up to the
 * correct size. "off" is the pixel offset in the buffer to start from, and
 * "bleed" is TRUE, then expand the buffer by 1 cell (clipped by draw area
 * width), using the color of the rightmost cell.
 */
    ساكن Gsk_Render_غوټه *
draw_layer_get_texture(
	DrawLayer   *dlayer,
	صحيح	    لیکه,
	VimDrawArea *da,
	صحيح	    off,
	gboolean    bleed)
{
    GBytes	    *بایټونه;
    GdkTexture	    *texture;
    Gsk_Render_غوټه   *غوټه;
    صحيح		    لوړوالی;

#إذا GTK_CHECK_نسخه(4,24,0)
    لوړوالی = gui.char_لوړوالی;
#وإلا
    لوړوالی = gui.char_لوړوالی + 1;
#د_که_پای

    إذا (bleed)
    {
	GByteArray *arr = g_byte_array_new();

	g_byte_array_append(arr, (uint8_t *)(dlayer->buf + off),
		حجم(uint32_t) * da->n_ستون_شمېر);
	g_byte_array_append(arr,
		(uint8_t *)(dlayer->buf + off + (da->n_ستون_شمېر - 1)),
		حجم(uint32_t)); // Add bleed using last pixel of buffer
	بایټونه = g_byte_array_free_منزل_بایټونه(arr);
    }
    وإلا
	بایټونه = g_بایټونه_new(dlayer->buf + off, حجم(uint32_t) * da->n_ستون_شمېر);

    texture = gdk_حافظه_texture_new(da->n_ستون_شمېر + bleed, 1,
	    GDK_حافظه_R8_G8_B8_A8, بایټونه, (da->n_ستون_شمېر + bleed) * 4);
    g_بایټونه_unref(بایټونه);

    // Scale texture to actual size
    غوټه = gsk_texture_scale_غوټه_new(texture,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(لیکه),
		(da->n_ستون_شمېر + bleed) * gui.char_پلنوالی, gui.char_لوړوالی + bleed),
	    GSK_SCALING_FILTER_تر_ټولو_نږدې);
    إذا (bleed)
    {
	Gsk_Render_غوټه *new;

	new = gsk_clip_غوټه_new(غوټه,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(لیکه),
		    da->n_ستون_شمېر * gui.char_پلنوالی + da->bleed_ښی, لوړوالی));
	gsk_render_غوټه_unref(غوټه);
	غوټه = new;
    }
    g_object_unref(texture);
    أرجع غوټه;
}

    ساكن فراغ
draw_لیکه_init(Draw_لیکه *drow, صحيح لیکه, صحيح ستون_شمېر)
{
    drow->cells = g_malloc0_n(ستون_شمېر, حجم(DrawCell));
    drow->n_cells = ستون_شمېر;
    drow->لیکه = لیکه;
    drow->underl_ماسک = drow->underc_ماسک = drow->strike_ماسک = عدم;

    memset(drow->dlayers, 0, حجم(DrawLayer) * N_DRAW_LAYERS);
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	// Make the buffer transparent initially
	إذا (l == DRAW_LAYER_SPECIAL)
	    dlayer->buf = g_malloc0_n(ستون_شمېر * 2, 4);
	وإلا
	    dlayer->buf = g_malloc0_n(ستون_شمېر, 4);
    }
}

    ساكن فراغ
draw_لیکه_clear(Draw_لیکه *drow)
{
    g_free(drow->cells);
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	DrawLayer *dlayer = drow->dlayers + l;

	إذا (dlayer->غوټه != عدم)
	    gsk_render_غوټه_unref(dlayer->غوټه);
	g_free(dlayer->buf);
    }
    g_clear_اشاره_ګر(&drow->underl_ماسک, gsk_render_غوټه_unref);
    g_clear_اشاره_ګر(&drow->underc_ماسک, gsk_render_غوټه_unref);
    g_clear_اشاره_ګر(&drow->strike_ماسک, gsk_render_غوټه_unref);
}

/*
 * Dirty the given layer for the row so it is rendered again.
 */
    ساكن فراغ
draw_لیکه_dirty_layer(Draw_لیکه *drow, DrawLayerType dlayer_t)
{
    DrawLayer *dlayer = drow->dlayers + dlayer_t;

    dlayer->dirty = TRUE;
    g_clear_اشاره_ګر(&dlayer->غوټه, gsk_render_غوټه_unref);
}

/*
 * If the cell at "col" is inverted, then uninvert and set the layer as dirty.
 * Otherwise do nothing.
 */
    ساكن فراغ
draw_لیکه_uninvert_cell(Draw_لیکه *drow, صحيح ستون)
{
    uint32_t *buf = drow->dlayers[DRAW_LAYER_OVERLAY].buf;

    // If pixel is white, then it is inverted.
    إذا (buf[ستون] == 0xffffffff)
    {
	draw_لیکه_dirty_layer(drow, DRAW_LAYER_OVERLAY);
	buf[ستون] = 0;
    }
}

/*
 * Set the cell in the row to the glyphs and attrs (that represents the cell).
 * This will dirty the changed layers of the row.
 */
    ساكن فراغ
draw_لیکه_سېټ_cell(Draw_لیکه *drow, صحيح ستون, DrawGlyphs *dglyphs, DrawAttr *dattr)
{
    DrawCell *dcell = drow->cells + ستون;

    إذا (!draw_glyphs_same(dglyphs, dcell->dglyphs))
	draw_لیکه_dirty_layer(drow, DRAW_LAYER_متن);

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
    draw_لیکه_uninvert_cell(drow, ستون);
}

/*
 * Same as draw_row_set_cell(), but fills in the cells between "col1" and "col2"
 * (inclusive).
 */
    ساكن فراغ
draw_لیکه_fill(
	Draw_لیکه	    *drow,
	صحيح	    col1,
	صحيح	    col2,
	DrawGlyphs  *dglyphs,
	DrawAttr    *dattr,
	DrawCursor  *cursor)
{
    لكل (صحيح c = col1; c <= col2; c++)
	draw_لیکه_سېټ_cell(drow, c, dglyphs, dattr);

    // Clear cursor if its in the region
    إذا (gui.cursor_لیکه == drow->لیکه && gui.cursor_ستون >= col1
	    && gui.cursor_ستون <= col2)
	cursor->draw = FALSE;
}

/*
 * Move the cells between "col1" and "col2" from "src" to "dest", overwriting
 * the existing cells. This will clear the source cells.
 */
    ساكن فراغ
draw_لیکه_move_منزل(
	Draw_لیکه	    *dest_لیکه,
	Draw_لیکه	    *src_لیکه,
	صحيح	    col1,
	صحيح	    col2,
	VimDrawArea *da)
{
    صحيح move_اندازه = (col2 - col1 + 1) * حجم(DrawCell);

    draw_لیکه_fill(dest_لیکه, col1, col2, عدم, عدم, &da->cursor);
    memmove(dest_لیکه->cells + col1, src_لیکه->cells + col1, move_اندازه);
    // NULL the draw cells so we don't double unreference.
    memset(src_لیکه->cells + col1, 0, move_اندازه);
    draw_لیکه_dirty_layer(src_لیکه, DRAW_LAYER_متن);

    // Move the layer buffers
    move_اندازه = (col2 - col1 + 1) * 4;
    لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
    {
	uint32_t *src = src_لیکه->dlayers[l].buf;
	uint32_t *dest = dest_لیکه->dlayers[l].buf;

	إذا (l == DRAW_LAYER_SPECIAL)
	{
	    memmove(dest + col1 + UNDERC_دننه_واټن(dest_لیکه->n_cells),
		    src + col1 + UNDERC_دننه_واټن(src_لیکه->n_cells), move_اندازه);
	    memmove(dest + col1 + STRIKE_دننه_واټن(dest_لیکه->n_cells),
		    src + col1 + STRIKE_دننه_واټن(src_لیکه->n_cells), move_اندازه);
	}
	وإلا
	    memmove(dest + col1, src + col1, move_اندازه);
	draw_لیکه_dirty_layer(dest_لیکه, l);
    }
}

/*
 * If background layer is dirty, then render it.
 */
    ساكن فراغ
draw_لیکه_render_background(Draw_لیکه *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_BACKGROUND;

    إذا (!dlayer->dirty)
	أرجع;

    dlayer->dirty = FALSE;

    // If background is transparent, don't add a render node.
    إذا (rgba_is_transparent(dlayer->buf, da->n_ستون_شمېر))
	أرجع;

    // Add a 1 cell bleed, then use a clip node to clip it to the draw area
    // width. This just makes things like status bars look a bit nicer when draw
    // area size is not an exact multiple of the cell size.
    dlayer->غوټه = draw_layer_get_texture(dlayer, drow->لیکه, da, 0, TRUE);
}

/*
 * Ensure the given under decorations are generated.
 */
    ساكن فراغ
draw_لیکه_ensure_decor(Draw_لیکه *drow, صحيح flags)
{
    إذا (flags & DRAW_UNDERL && drow->underl_ماسک == عدم)
	drow->underl_ماسک = gsk_color_غوټه_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0), FILL_Y(drow->لیکه + 1) - 1,
		    FILL_X(drow->n_cells), 1));

    إذا (flags & DRAW_STRIKE && drow->strike_ماسک == عدم)
	drow->strike_ماسک = gsk_color_غوټه_new(&white_rgba,
		&GRAPHENE_RECT_INIT(FILL_X(0),
		    FILL_Y(drow->لیکه) + (صحيح)(gui.char_لوړوالی / 2),
		    FILL_X(drow->n_cells), 1));

    إذا (flags & DRAW_UNDERC && drow->underc_ماسک == عدم)
    {
	ساكن ثابت صحيح val[8] = {1, 0, 0, 0, 1, 2, 2, 2};

	صحيح y = FILL_Y(drow->لیکه + 1) - 1;
	صحيح x_پیل = FILL_X(0);
	صحيح x_پای_ټکی = FILL_X(drow->n_cells);

	// Instead of rendering the entire pattern, use a repeating node to
	// render a single cycle of the undercurl, taking advantage of the GPU
	// (if using opengl or vulkan renderer).
	Gsk_Render_غوټه *ماشوم = عدم;

	// GskPath was added in GSK 4.14, otherwise use Cairo
#إذا GTK_CHECK_نسخه(4, 14, 0)
	Gsk_لار_Builder	*builder;
	Gsk_لار		*لار;
	GskStroke       *stroke;
	Gsk_Render_غوټه	*color_غوټه;
	graphene_rect_t bounds;

	builder = gsk_لار_builder_new();

	// Start at X = -1 (val[7]) to ensure a fully formed stroke at X = 0
	gsk_لار_builder_move_منزل(builder, -1, y - val[7] + 0.5);
	gsk_لار_builder_کرښه_منزل(builder, 0, y - val[0] + 0.5);

	لكل (صحيح i = 1; i < 8; i++)
	    gsk_لار_builder_کرښه_منزل(builder, i, y - val[i] + 0.5);

	// Extend to X = 9 (val[1]) to ensure a fully formed stroke at X = 8
	gsk_لار_builder_کرښه_منزل(builder, 8, y - val[0] + 0.5);
	gsk_لار_builder_کرښه_منزل(builder, 9, y - val[1] + 0.5);

	لار = gsk_لار_builder_free_منزل_لار(builder);
	stroke = gsk_stroke_new(1.0);

	gsk_لار_get_stroke_bounds(لار, stroke, &bounds);
	color_غوټه = gsk_color_غوټه_new(&white_rgba, &bounds);
	ماشوم = gsk_stroke_غوټه_new(color_غوټه, لار, stroke);

	gsk_stroke_free(stroke);
	gsk_لار_unref(لار);
	gsk_render_غوټه_unref(color_غوټه);
#وإلا
	cairo_t *cr;

	ماشوم = gsk_cairo_غوټه_new(&GRAPHENE_RECT_INIT(-2, y - 4, 12, 7));
	cr = gsk_cairo_غوټه_get_draw_context(ماشوم);

	cairo_سېټ_کرښه_پلنوالی(cr, 1.0);
	cairo_سېټ_سرچینه_rgba(cr, 1.0, 1.0, 1.0, 1.0);

	cairo_move_منزل(cr, -1, y - val[7] + 0.5);
	cairo_کرښه_منزل(cr, 0, y - val[0] + 0.5);

	لكل (صحيح i = 1; i < 8; ++i)
	    cairo_کرښه_منزل(cr, i, y - val[i] + 0.5);

	cairo_کرښه_منزل(cr, 8, y - val[0] + 0.5);
	cairo_کرښه_منزل(cr, 9, y - val[1] + 0.5);

	cairo_stroke(cr);
	cairo_destroy(cr);
#د_که_پای
	drow->underc_ماسک = gsk_repeat_غوټه_new(
		&GRAPHENE_RECT_INIT(x_پیل, y - 3, x_پای_ټکی - x_پیل, 5),
		ماشوم, &GRAPHENE_RECT_INIT(0.0f, y - 3, 8.0f, 5.0f));
	gsk_render_غوټه_unref(ماشوم);
    }
}

/*
 * If the text layer of the draw row is dirty, then render it and set the render
 * node. Note that the render node may still be NULL if there is nothing to
 * display.
 */
    ساكن فراغ
draw_لیکه_render_متن(Draw_لیکه *drow, VimDrawArea *da)
{
    DrawLayer	*dlayer = drow->dlayers + DRAW_LAYER_متن;
    GArray	*glyph_buf;
    GPtrArray	*nodes;
    PangoFont	*cur_font = عدم;
    صحيح		پیل_ستون = 0;
    صحيح		تشه_cells = 0;
    uint32_t	cur_fg = 0;
    صحيح		c;

    إذا (!dlayer->dirty)
	أرجع;

    glyph_buf = da->glyph_buf;
    nodes = da->غوټه_buf;

    g_array_سېټ_اندازه(glyph_buf, 0);
    g_ptr_array_سېټ_اندازه(nodes, 0);

    // GskTextNode does not use the "log_clusters" array of PangoGlyphString, so
    // we don't need to set it.
#تعریف_کړه FLUSH_غوټه() \
    افعل { \
	إذا (cur_font != عدم) \
	{ \
	    PangoGlyphString	glyph_str; \
	    Gsk_Render_غوټه	*غوټه; \
	    GdkRGBA		fg_color = u32_منزل_rgba(cur_fg); \
	    glyph_str.glyphs = (PangoGlyphInfo *)glyph_buf->معلومات; \
	    glyph_str.عدد_glyphs = glyph_buf->len; \
	    غوټه = gsk_متن_غوټه_new(cur_font, &glyph_str, &fg_color, \
		    &GRAPHENE_ټکی_INIT(متن_X(پیل_ستون), \
			متن_Y(drow->لیکه))); \
	    إذا (غوټه != عدم) \
		g_ptr_array_جمع_کړه(nodes, غوټه); \
	    g_array_سېټ_اندازه(glyph_buf, 0); \
	    cur_font = عدم; \
	} \
    } طالما (FALSE)

    لكل (c = 0; c < da->n_ستون_شمېر; c++)
    {
	DrawCell    *dcell = drow->cells + c;
	DrawGlyphs  *dglyphs = dcell->dglyphs;
	صحيح	    prev_len;

	إذا (dglyphs == عدم)
	{
	    تشه_cells++;
	    استمر;
	}
#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_SIGN_ICONS)
	وإلا إذا (dglyphs->font == عدم)
	{
	    // Add sign icon
	    Gsk_Render_غوټه   *snode;
	    صحيح		    w;

	    FLUSH_غوټه();

	    snode = gsk_texture_scale_غوټه_new(dglyphs->dsign->texture,
		    &GRAPHENE_RECT_INIT(FILL_X(c), FILL_Y(drow->لیکه),
			dglyphs->dsign->پلنوالی, dglyphs->dsign->لوړوالی),
		    GSK_SCALING_FILTER_TRILINEAR);
	    w = (dglyphs->dsign->پلنوالی + gui.char_پلنوالی - 1) / gui.char_پلنوالی;

	    إذا (snode != عدم)
		g_ptr_array_جمع_کړه(nodes, snode);
	    // Skip the cells the draw sign covers.
	    c += w - 1; // Subtract one because we increment by one in the for
			// loop
	    استمر;
	}
#د_که_پای
	وإلا إذا (dglyphs->font != cur_font || cur_fg != dglyphs->fg_color)
	{
	    FLUSH_غوټه();
	    cur_font = dglyphs->font;
	    پیل_ستون = c;
	    cur_fg = dglyphs->fg_color;
	}

	// Don't want to render double width characters twice.
	إذا (c > 0 && drow->cells[c - 1].dglyphs == dglyphs)
	    استمر;

	prev_len = glyph_buf->len;
	g_array_append_vals(glyph_buf, dglyphs->glyphs, dglyphs->n_glyphs);

	// Inject the accumulated space width into the glyphs that
	// have ink.
	إذا (تشه_cells > 0)
	{
	    إذا (prev_len > 0)
	    {
		// Spaces occurred between valid text: extend
		// previous character's advance
		PangoGlyphInfo *وروستی = &g_array_شاخص(
			glyph_buf, PangoGlyphInfo,
			prev_len - 1);
		وروستی->geometry.پلنوالی += CELLS2PANGO(تشه_cells);
	    }
	    تشه_cells = 0; // Width consumed
	}
    }

    FLUSH_غوټه();

#تعریف_لغوه FLUSH_غوټه

    إذا (nodes->len > 0)
    {
	// Add underline mask (if needed).
	Gsk_Render_غوټه *سرچینه;
	Gsk_Render_غوټه *غوټه;

	draw_لیکه_ensure_decor(drow, DRAW_UNDERL);
	إذا (!rgba_is_transparent(dlayer->buf, drow->n_cells))
	{
	    سرچینه = draw_layer_get_texture(dlayer, drow->لیکه, da, 0, FALSE);
	    غوټه = gsk_ماسک_غوټه_new(سرچینه, drow->underl_ماسک,
		    GSK_ماسک_ډېرپېښ_ALPHA);
	    gsk_render_غوټه_unref(سرچینه);
	    g_ptr_array_جمع_کړه(nodes, غوټه);
	}

	dlayer->غوټه = gsk_container_غوټه_new(
		(Gsk_Render_غوټه **)nodes->pdata, nodes->len);
    }
    dlayer->dirty = FALSE;
}

/*
 * Render undercurl and strikethough, which use the special colour.
 */
    ساكن فراغ
draw_لیکه_render_special(Draw_لیکه *drow, VimDrawArea *da)
{
    DrawLayer	    *dlayer = drow->dlayers + DRAW_LAYER_SPECIAL;
    Gsk_Render_غوټه   *سرچینه;
    Gsk_Render_غوټه   *nodes[2];
    صحيح		    n_nodes = 0;

    إذا (!dlayer->dirty)
	أرجع;

    draw_لیکه_ensure_decor(drow, DRAW_UNDERC | DRAW_STRIKE);

    إذا (!rgba_is_transparent(dlayer->buf + UNDERC_دننه_واټن(drow->n_cells),
		drow->n_cells))
    {
	سرچینه = draw_layer_get_texture(dlayer, drow->لیکه, da,
		UNDERC_دننه_واټن(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_ماسک_غوټه_new(سرچینه, drow->underc_ماسک,
		GSK_ماسک_ډېرپېښ_ALPHA);
	gsk_render_غوټه_unref(سرچینه);
    }

    إذا (!rgba_is_transparent(dlayer->buf + STRIKE_دننه_واټن(drow->n_cells),
		drow->n_cells))
    {
	سرچینه = draw_layer_get_texture(dlayer, drow->لیکه, da,
		STRIKE_دننه_واټن(drow->n_cells), FALSE);
	nodes[n_nodes++] = gsk_ماسک_غوټه_new(سرچینه, drow->strike_ماسک,
		GSK_ماسک_ډېرپېښ_ALPHA);
	gsk_render_غوټه_unref(سرچینه);
    }

    إذا (n_nodes == 1)
    {
	dlayer->غوټه = nodes[0];
    }
    وإلا إذا (n_nodes > 0)
    {
	dlayer->غوټه = gsk_container_غوټه_new(nodes, 2);
	// gsk_container_node_new() takes its own ref
	لكل (صحيح i = 0; i < (صحيح)ARRAY_اوږدتیا(nodes); i++)
	    gsk_render_غوټه_unref(nodes[i]);
    }

    dlayer->dirty = FALSE;
}

/*
 * Render white color blocks used for blend to invert cells.
 */
    ساكن فراغ
draw_لیکه_render_overlay(Draw_لیکه *drow, VimDrawArea *da)
{
    DrawLayer *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

    إذا (!dlayer->dirty)
	أرجع;

    dlayer->dirty = FALSE;

    // If no invert regions, don't do anything.
    إذا (rgba_is_transparent(dlayer->buf, da->n_ستون_شمېر))
	أرجع;

    dlayer->غوټه = draw_layer_get_texture(dlayer, drow->لیکه, da, 0, TRUE);
}

/*
 * Add the glyph cluster to the raw area with given attributes and font.
 */
    ساكن فراغ
draw_لیکه_جمع_کړه_cluster(
	Draw_لیکه		*drow,
	DrawAttr	*dattr,
	صحيح		ستون,
	صحيح		n_cells,
	PangoFont	*font,
	PangoGlyphInfo	*glyphs,
	صحيح		n_glyphs,
	VimDrawArea	*da)
{
    DrawGlyphs *dglyphs = عدم;

    إذا (ستون + n_cells > da->n_ستون_شمېر)
	أرجع;

    // If there is no ink ("glyphs" is NULL), then just set the "dglyphs" of
    // each cell to NULL.
    إذا (glyphs != عدم)
	dglyphs = draw_glyphs_new(font, glyphs, n_glyphs, dattr->fg_color);

    draw_لیکه_fill(drow, ستون, ستون + n_cells - 1, dglyphs, dattr, &da->cursor);

    إذا (dglyphs != عدم)
	draw_glyphs_unref(dglyphs);
}

/*
 * Add the string to the draw area. This will handle the 'guiligatures' option.
 */
    ساكن صحيح
vim_draw_area_جمع_کړه_string(
	VimDrawArea *self,
	Draw_لیکه	    *drow,
	DrawAttr    *dattr,
	صحيح	    ستون,
	char_u	    *s,
	صحيح	    len,
	صحيح	    draw_flags)
{
    PangoGlyphString	*glyphs;
    PangoAttrList	*attr_list = عدم;
    صحيح			پیل_idx = -1;
    GList		*توکی_list;
    صحيح			ستنه_دننه_واټن = 0;
    صحيح			cluster_پلنوالی;
    صحيح			وروستی_glyph_rbearing;
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
	    want_disable = (پیل_idx != -1);
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
	    إذا (پیل_idx == -1)
		پیل_idx = i;
	}
	وإلا إذا (پیل_idx != -1)
	{
	    إذا (attr_list == عدم)
		attr_list = pango_attr_list_new();
	    ورننباسه_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		    attr_list, پیل_idx, i);
	    پیل_idx = -1;
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

	    draw_لیکه_جمع_کړه_cluster(drow, dattr, ستون + i, 1,
		    gui.ascii_font, has_ink ? &glyph : عدم, 1, self);
	}

	ستنه_دننه_واټن = len;
	اذهب وتل;
    }

    glyphs = pango_glyph_string_new();

    إذا (attr_list == عدم)
	attr_list = pango_attr_list_new();

    إذا (پیل_idx != -1)
	ورننباسه_PANGO_ATTR(pango_attr_font_features_new("liga 0, calt 0"),
		attr_list, پیل_idx, len);

    cluster_پلنوالی = PANGO_SCALE * gui.char_پلنوالی;
    وروستی_glyph_rbearing = PANGO_SCALE * gui.char_پلنوالی;

    // If 'guifontwide' is set then use that for double-width characters.
    إذا (gui.wide_font != عدم)
	apply_wide_font_attr(s, len, attr_list);

    إذا ((draw_flags & DRAW_BOLD) && gui.font_can_bold)
	ورننباسه_PANGO_ATTR(pango_attr_وزن_new(PANGO_وزن_BOLD),
		attr_list, 0, len);
    إذا (draw_flags & DRAW_ITALIC)
	ورننباسه_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
		attr_list, 0, len);

    توکی_list = pango_itemize(gui.متن_context,
	    (ثابت حرف *)s, 0, len, attr_list, عدم);

    طالما (توکی_list != عدم)
    {
	Pango_توکی   *توکی = توکی_list->معلومات;
	صحيح	    توکی_cells = 0;
	صحيح	    cluster_پیل_idx = 0;
	صحيح	    current_cluster_ستون = ستون + ستنه_دننه_واټن;
	صحيح	    current_cluster_cells = 0;
	gboolean    current_cluster_ink = FALSE;

	توکی_list = g_list_delete_link(توکی_list, توکی_list);

	// Force LTR direction; Vim handles bidi on its own.
	توکی->analysis.level = (توکی->analysis.level + 1) & (~1U);

	pango_shape_ډک((ثابت حرف *)s + توکی->دننه_واټن, توکی->اوږدتیا,
		(ثابت حرف *)s, len, &توکی->analysis, glyphs);

	// Fixed-width hack: assign a fixed width to each glyph based on the
	// number of cells it occupies, handling composing characters and
	// cluster boundaries properly.
	//
	// At the end of each cluster, add it to the draw area.
	لكل (i = 0; i < glyphs->عدد_glyphs; ++i)
	{
	    PangoGlyphInfo *glyph;

	    glyph = &glyphs->glyphs[i];

	    إذا (glyph->attr.is_cluster_پیل)
	    {
		صحيح cellcount;

		cellcount = شمېر_cluster_cells(
			s, توکی, glyphs, i, &cluster_پلنوالی,
			(توکی_list != عدم) ? &وروستی_glyph_rbearing : عدم);

		إذا (i > 0 && cellcount > 0)
		{
		    draw_لیکه_جمع_کړه_cluster(drow, dattr,
			    current_cluster_ستون, current_cluster_cells,
			    توکی->analysis.font,
			    current_cluster_ink ?
			    glyphs->glyphs + cluster_پیل_idx : عدم,
			    i - cluster_پیل_idx, self);

		    current_cluster_ستون += current_cluster_cells;
		    cluster_پیل_idx = i;
		    current_cluster_ink = FALSE;
		    current_cluster_cells = 0;
		}

		إذا (cellcount > 0)
		{
		    صحيح پلنوالی;

		    پلنوالی = cellcount * gui.char_پلنوالی * PANGO_SCALE;
		    glyph->geometry.x_دننه_واټن +=
			MAX(0, پلنوالی - cluster_پلنوالی) / 2;
		    glyph->geometry.پلنوالی = پلنوالی;
		}
		وإلا
		    تیاری_zero_پلنوالی_cluster(توکی, glyph, cells,
			    cluster_پلنوالی,
			    وروستی_glyph_rbearing);

		توکی_cells += cellcount;
		cells = cellcount;
		current_cluster_cells += cellcount;
	    }
	    وإلا إذا (i > 0)
	    {
		صحيح پلنوالی;

		إذا (glyph->geometry.x_دننه_واټن >= 0)
		{
		    glyphs->glyphs[i].geometry.پلنوالی =
			glyphs->glyphs[i - 1].geometry.پلنوالی;
		    glyphs->glyphs[i - 1].geometry.پلنوالی = 0;
		}
		پلنوالی = cells * gui.char_پلنوالی * PANGO_SCALE;
		glyph->geometry.x_دننه_واټن +=
		    MAX(0, پلنوالی - cluster_پلنوالی) / 2;
	    }
	    وإلا
		glyph->geometry.پلنوالی = 0;


	    إذا (!current_cluster_ink)
		current_cluster_ink = glyph_has_ink(
			توکی->analysis.font, *glyph);
	}
	إذا (glyphs->عدد_glyphs > 0)
	    draw_لیکه_جمع_کړه_cluster(drow, dattr,
		    current_cluster_ستون, current_cluster_cells,
		    توکی->analysis.font,
		    current_cluster_ink ?
		    glyphs->glyphs + cluster_پیل_idx : عدم,
		    glyphs->عدد_glyphs - cluster_پیل_idx, self);

	pango_توکی_free(توکی);
	ستنه_دننه_واټن += توکی_cells;
    }

    pango_glyph_string_free(glyphs);

وتل:
    إذا (attr_list != عدم)
	pango_attr_list_unref(attr_list);

    أرجع ستنه_دننه_واټن;
}

/*
 * Draw the given string at position (col, row) on the draw area.
 */
    صحيح
vim_draw_area_draw_string(
	VimDrawArea *self,
	صحيح	    لیکه,
	صحيح	    ستون,
	char_u	    *s,
	صحيح	    len,
	صحيح	    flags)
{
    Draw_لیکه	*drow;
    DrawAttr	dattr;
    char_u	*conv_buf = عدم;
    صحيح		convlen;
    صحيح		n_cells ;

    إذا (unlikely(self->لیکې_2 == عدم
		|| لیکه >= self->n_لیکې
		|| ستون >= self->n_ستون_شمېر))
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

    drow = self->لیکې_2 + لیکه;
    draw_attr_init(&dattr, gui.bgcolor, gui.fgcolor, gui.spcolor, flags);

    n_cells = vim_draw_area_جمع_کړه_string(self, drow, &dattr, ستون, s, len, flags);

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
    إذا (unlikely(self->لیکې_2 == عدم
		|| row1 >= self->n_لیکې
		|| col1 >= self->n_ستون_شمېر
		|| row2 >= self->n_لیکې
		|| col2 >= self->n_ستون_شمېر))
	أرجع;

    لكل (صحيح r = row1; r <= row2; r++)
	draw_لیکه_fill(self->لیکې_2 + r, col1, col2, عدم, عدم, &self->cursor);
}

/*
 * Clear out the entire draw area
 */
    فراغ
vim_draw_area_clear(VimDrawArea *self)
{
    vim_draw_area_clear_بلاک(self, 0, 0, self->n_لیکې - 1, self->n_ستون_شمېر - 1);
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
    صحيح دننه_واټن = row2 - row1;

    إذا (unlikely(self->لیکې_2 == عدم
		|| row1 >= self->n_لیکې
		|| row2 >= self->n_لیکې
		|| منزل >= self->n_لیکې
		|| col1 >= self->n_ستون_شمېر
		|| col2 >= self->n_ستون_شمېر))
	أرجع;

    أكد(row2 >= row1);
    أكد(col2 >= col1);
    أكد(row1 != منزل);

    إذا (row1 > منزل)
    {
	// "row1" is below "to", start moving rows starting at "row1". Rows are
	// being shifted upwards.
	لكل (صحيح o = 0; o <= دننه_واټن; o++)
	    draw_لیکه_move_منزل(self->لیکې_2 + منزل + o, self->لیکې_2 + row1 + o,
		    col1, col2, self);
    }
    وإلا
    {
	// "row1" is above "to", must start moving rows starting at "row2". Rows
	// are being shifted downwards.
	لكل (صحيح o = دننه_واټن; o >= 0; o--)
	    إذا (منزل + o >= self->n_لیکې)
		// "src_row" is being "moved" off the screen, no need to move
		// it physically.
		vim_draw_area_clear_بلاک(self, row1 + o, col1, row1 + o, col2);
	    وإلا
		draw_لیکه_move_منزل(self->لیکې_2 + منزل + o, self->لیکې_2 + row1 + o,
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
vim_draw_area_سېټ_cursor(VimDrawArea *self, صحيح w, صحيح h)
{
    self->cursor.draw = TRUE;
    self->cursor.پلنوالی = w;
    self->cursor.لوړوالی = h;
    // Remember this now: the snapshot runs later, when the drawing position
    // has moved on.
    self->cursor.n_cells = 1 + mb_lefthalve(gui.cursor_لیکه, gui.cursor_ستون);
    self->cursor.bg_color = *gui.bgcolor;
    self->cursor.fg_color = *gui.fgcolor;
}

/*
 * Invert the rectangle in the draw area.
 */
    فراغ
vim_draw_area_invert_بلاک(
	VimDrawArea	*self,
	صحيح		لیکه,
	صحيح		ستون,
	صحيح		nrows,
	صحيح		ncols)
{
    إذا (unlikely(self->لیکې_2 == عدم
		|| لیکه >= self->n_لیکې
		|| ستون >= self->n_ستون_شمېر
		|| لیکه + nrows - 1 >= self->n_لیکې
		|| ستون + ncols - 1 >= self->n_ستون_شمېر))
	أرجع;

    لكل (صحيح r = لیکه; r < لیکه + nrows; r++)
    {
	Draw_لیکه	    *drow = self->لیکې_2 + r;
	DrawLayer   *dlayer = drow->dlayers + DRAW_LAYER_OVERLAY;

	لكل (صحيح c = 0; c < ncols; c++)
	    dlayer->buf[c + ستون] = ~dlayer->buf[c + ستون];
	draw_لیکه_dirty_layer(drow, DRAW_LAYER_OVERLAY);
    }
}

#إذا defined(FEAT_SIGN_ICONS)
/*
 * Add a sign texture at the given row and column, and scale it to "width" and
 * "height".
 */
    فراغ
vim_draw_area_جمع_کړه_sign(
	VimDrawArea *self,
	GdkTexture  *sign,
	صحيح	    لیکه,
	صحيح	    ستون,
	صحيح	    پلنوالی,
	صحيح	    لوړوالی)
{
    Draw_لیکه	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحيح		cells;

    إذا (unlikely(self->لیکې_2 == عدم
		|| لیکه >= self->n_لیکې
		|| ستون >= self->n_ستون_شمېر))
	أرجع;

    drow = self->لیکې_2 + لیکه;
    dsign = draw_sign_new(sign, پلنوالی, لوړوالی);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (پلنوالی + gui.char_پلنوالی - 1) / gui.char_پلنوالی;

    draw_لیکه_fill(drow, ستون, ستون + cells - 1, dglyphs, عدم, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#د_که_پای

#که_تعریف_شوی FEAT_NETBEANS_INTG
/*
 * Add a multi sign indicator at the given position. "surf" should be in ARGB
 * format.
 */
    فراغ
vim_draw_area_جمع_کړه_multisign(
	VimDrawArea	*self,
	cairo_surface_t *surf,
	صحيح		لیکه,
	صحيح		ستون,
	صحيح		پلنوالی,
	صحيح		لوړوالی)
{
    Draw_لیکه	*drow;
    DrawSign	*dsign;
    DrawGlyphs	*dglyphs;
    صحيح		cells;
    GdkTexture	*texture;
    GBytes	*بایټونه;

    إذا (unlikely(self->لیکې_2 == عدم
		|| لیکه >= self->n_لیکې
		|| ستون >= self->n_ستون_شمېر))
	أرجع;

    cairo_surface_flush(surf);
    بایټونه = g_بایټونه_new_with_free_func(cairo_image_surface_get_معلومات(surf),
	    cairo_image_surface_get_لوړوالی(surf)
	    * cairo_image_surface_get_stride(surf),
	    (GDestroyNotify)cairo_surface_destroy,
	    cairo_surface_reference(surf));

    texture = gdk_حافظه_texture_new(cairo_image_surface_get_پلنوالی(surf),
	    cairo_image_surface_get_لوړوالی(surf),
	    GDK_حافظه_A8_R8_G8_B8_PREMULTIPLIED,
	    بایټونه, cairo_image_surface_get_stride(surf));
    g_بایټونه_unref(بایټونه);

    drow = self->لیکې_2 + لیکه;
    dsign = draw_sign_new(texture, پلنوالی, لوړوالی);
    g_object_unref(texture);
    dglyphs = draw_glyphs_new_sign(dsign);
    cells = (پلنوالی + gui.char_پلنوالی - 1) / gui.char_پلنوالی;

    draw_لیکه_fill(drow, ستون, ستون + cells - 1, dglyphs, عدم, &self->cursor);
    draw_glyphs_unref(dglyphs);
}
#د_که_پای

#که_تعریف_شوی FEAT_IMAGE_GUI

/*
 * Add an external node to be rendered, adding a new reference to the node.
 */
    فراغ
vim_draw_area_جمع_کړه_external(VimDrawArea *self, Gsk_Render_غوټه *غوټه)
{
    g_hash_جدول_جمع_کړه(self->external, gsk_render_غوټه_ref(غوټه));
}

    فراغ
vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_غوټه *غوټه)
{
    g_hash_جدول_remove(self->external, غوټه);
}

#د_که_پای

/*
 * Add the cursor to the snapshot.
 */
    ساكن فراغ
vim_draw_area_snapshot_cursor(
	VimDrawArea	*self,
	Gsk_Render_غوټه	*متن_غوټه,	// May be NULL
	Gsk_Render_غوټه	*special_غوټه,	// May be NULL
	GtkSnapshot	*snapshot)
{
    DrawCursor	    *cursor = &self->cursor;
    صحيح		    w = cursor->پلنوالی;
    صحيح		    h = cursor->لوړوالی;
    graphene_rect_t rect;

    إذا (!cursor->draw)
	أرجع;

    إذا (cursor->پلنوالی <= 0 && cursor->لوړوالی <= 0)
    {
	w += gui.char_پلنوالی * cursor->n_cells;
	h = gui.char_لوړوالی;
    }

    graphene_rect_init(&rect,
#که_تعریف_شوی FEAT_RIGHTLEFT
	    CURSOR_BAR_ښی ? FILL_X(gui.cursor_ستون + 1) - w :
#د_که_پای
	    FILL_X(gui.cursor_ستون),
	    FILL_Y(gui.cursor_لیکه) + gui.char_لوړوالی - h, w, h);


    إذا (cursor->پلنوالی == -1 && cursor->لوړوالی == -1)
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

	gtk_snapshot_ټېل_ماسک(snapshot, GSK_ماسک_ډېرپېښ_ALPHA);

	// Used as an "outline" for the fg color.
	إذا (متن_غوټه != عدم)
	    gtk_snapshot_append_غوټه(snapshot, متن_غوټه);
	إذا (special_غوټه != عدم)
	    gtk_snapshot_append_غوټه(snapshot, special_غوټه);

	gtk_snapshot_راوباسه(snapshot);

	gtk_snapshot_append_color(snapshot, &cursor->fg_color, &rect);
	gtk_snapshot_راوباسه(snapshot);
    }
}

    ساكن فراغ
vim_draw_area_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimDrawArea	    *self = VIM_DRAW_AREA(widget);
    صحيح		    لوړوالی, پلنوالی;
    GtkSnapshot	    *body_snapshot;
    GtkSnapshot	    *invert_snapshot = عدم;
    Gsk_Render_غوټه   *body_غوټه;
    Gsk_Render_غوټه   *invert_غوټه = عدم;
#که_تعریف_شوی FEAT_IMAGE_GUI
    Gsk_Render_غوټه   *ext_غوټه;
    GHash_جدول_Iter  iter;
#د_که_پای

    gui_mch_سېټ_bg_color(gui.شا_pixel);
    لوړوالی = gtk_widget_get_لوړوالی(widget) + gui.bleed_bot;
    پلنوالی = gtk_widget_get_پلنوالی(widget) + gui.bleed_ښی;

    إذا (self->لیکې_2 == عدم)
    {
	gtk_snapshot_append_color(snapshot, gui.bgcolor,
		&GRAPHENE_RECT_INIT(0, 0, پلنوالی, لوړوالی));
	أرجع;
    }

    // If number of pixels to bleed has changed, then dirty the background and
    // overlay layers of all rows.
    إذا (self->bleed_ښی == -1 || self->bleed_ښی != gui.bleed_ښی)
    {
	self->bleed_ښی = gui.bleed_ښی;
	لكل (صحيح r = 0; r < self->n_لیکې; r++)
	{
	    draw_لیکه_dirty_layer(self->لیکې_2 + r, DRAW_LAYER_BACKGROUND);
	    draw_لیکه_dirty_layer(self->لیکې_2 + r, DRAW_LAYER_OVERLAY);
	}
    }

    // First append everything that should be inverted to another snapshot, then
    // free that snapshot into a node so it can be blended (if needed).
    body_snapshot = gtk_snapshot_new();
#إذا GTK_CHECK_نسخه(4,24,0)
    gtk_snapshot_سېټ_snap(body_snapshot, GSK_RECT_SNAP_پړاو);
#د_که_پای

    لكل (صحيح r = 0; r < self->n_لیکې; r++)
    {
	Draw_لیکه *drow = self->لیکې_2 + r;

	draw_لیکه_render_background(drow, self);
	draw_لیکه_render_متن(drow, self);
	draw_لیکه_render_special(drow, self);
	draw_لیکه_render_overlay(drow, self);

	لكل (صحيح l = 0; l < N_DRAW_LAYERS; l++)
	{
	    DrawLayer *dlayer = drow->dlayers + l;

	    إذا (dlayer->غوټه != عدم)
	    {
		إذا (l == DRAW_LAYER_OVERLAY)
		{
		    إذا (invert_snapshot == عدم)
		    {
			invert_snapshot = gtk_snapshot_new();
#إذا GTK_CHECK_نسخه(4,24,0)
			gtk_snapshot_سېټ_snap(invert_snapshot,
				GSK_RECT_SNAP_پړاو);
#د_که_پای
		    }
		    gtk_snapshot_append_غوټه(invert_snapshot, dlayer->غوټه);
		}
		وإلا
		    gtk_snapshot_append_غوټه(body_snapshot, dlayer->غوټه);
	    }
	}

	إذا (r == gui.cursor_لیکه)
	    vim_draw_area_snapshot_cursor(self,
		    drow->dlayers[DRAW_LAYER_متن].غوټه,
		    drow->dlayers[DRAW_LAYER_SPECIAL].غوټه,
		    body_snapshot);
    }

    إذا (invert_snapshot != عدم)
	gtk_snapshot_ټېل_blend(snapshot, GSK_BLEND_ډېرپېښ_DIFFERENCE);

    gtk_snapshot_append_color(snapshot, gui.bgcolor,
	    &GRAPHENE_RECT_INIT(0, 0, پلنوالی, لوړوالی));
    body_غوټه = gtk_snapshot_free_منزل_غوټه(body_snapshot);
    إذا (body_غوټه != عدم)
    {
	gtk_snapshot_append_غوټه(snapshot, body_غوټه);
	gsk_render_غوټه_unref(body_غوټه);
    }

#که_تعریف_شوی FEAT_IMAGE_GUI
    // Allow external nodes to be inverted
    g_hash_جدول_iter_init(&iter, self->external);

    طالما (g_hash_جدول_iter_بل(&iter, (فراغ **)&ext_غوټه, عدم))
	gtk_snapshot_append_غوټه(snapshot, ext_غوټه);
#د_که_پای

    إذا (invert_snapshot != عدم)
    {
	gtk_snapshot_راوباسه(snapshot);
	invert_غوټه = gtk_snapshot_free_منزل_غوټه(invert_snapshot);
	// Not sure if it can be NULL but still check
	إذا (invert_غوټه != عدم)
	{
	    gtk_snapshot_append_غوټه(snapshot, invert_غوټه);
	    gsk_render_غوټه_unref(invert_غوټه);
	}
	gtk_snapshot_راوباسه(snapshot);
    }
}
