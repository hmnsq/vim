/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Abstracts each image backend into a common interface. This common interface
 * works around the idea of images being blitted to the screen, similar to how
 * characters are written to the terminal/GUI. For backends that do not natively
 * blit pixels ("images" are objects), the behaviour is emulated.
 */

#شامل "vim.h"

#اگر defined(FEAT_IMAGE)

ساخت
{
    ثابت حرف *name;

    ساخت
    {
	// Return OK on success and FAIL on failure
	صحیح (*init)(image_T *);
	خالی (*uninit)(image_T *);
    } image;

    ساخت
    {
	// Return OK on success and FAIL on failure
	صحیح (*init)(image_placement_T *);
	خالی (*uninit)(image_placement_T *);

	خالی (*draw)(image_placement_T *);
	خالی (*clear)(image_placement_T *);

	// If backend blits the image pixels to the screen. Images are not
	// treated as "objects".
	bool blit;
    } placement;
} image_backends[] = {
#اگر_تعریف_شدہ FEAT_IMAGE_GUI
    [IMAGE_BACKEND_GUI] = {
	.name = "gui",
	.image = {
	    .init = image_gui_init,
	    .uninit = image_gui_uninit
	},
	.placement = {
	    .init = image_placement_gui_init,
	    .uninit = image_placement_gui_uninit,
	    .draw = image_placement_gui_draw,
	    .clear = image_placement_gui_clear,
# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
	    .blit = false,
#  ورنہ
	    .blit = true,
#  endif
# elif FEAT_GUI_MSWIN
	    .blit = true,
# ورنہ
	    .blit = false,
# endif
	}
    },
#اگر_ختم
#اگر_تعریف_شدہ FEAT_IMAGE_KITTY
    [IMAGE_BACKEND_KITTY] = {
	.name = "kitty",
	.image = {
	    .init = image_kitty_init,
	    .uninit = image_kitty_uninit
	},
	.placement = {
	    .init = image_placement_kitty_init,
	    .uninit = image_placement_kitty_uninit,
	    .draw = image_placement_kitty_draw,
	    .clear = image_placement_kitty_clear,
	    .blit = false
	}
    },
#اگر_ختم
#اگر_تعریف_شدہ FEAT_IMAGE_SIXEL
    [IMAGE_BACKEND_SIXEL] = {
	.name = "sixel",
	.image = {
	    .init = image_sixel_init,
	    .uninit = image_sixel_uninit
	},
	.placement = {
	    .init = image_placement_sixel_init,
	    .uninit = image_placement_sixel_uninit,
	    .draw = image_placement_sixel_draw,
	    .clear = image_placement_sixel_clear,
	    .blit = true
	}
    }
#اگر_ختم
};

// Current image backend being used
ساکن image_backend_T image_backend = IMAGE_BACKEND_NONE;

ساکن image_T *images = تہی;
// Sorted from highest zindex to lowest zindex
ساکن image_placement_T    *placements = تہی;
ساکن صحیح		    n_placements = 0;

ساکن صحیح سرکاؤ_چوٹی = 0;
ساکن صحیح سرکاؤ_bot = 0;
ساکن صحیح سرکاؤ = 0;

// If > 0, then mark_dirty_region_for_images() will be a no-op
ساکن صحیح freeze_mark_dirty = 0;

#تعریف_کرو FOR_ALL_IMAGES(v) برائے ((v) = images; (v) != تہی; (v) = (v)->اگلا)
#تعریف_کرو FOR_ALL_PLACEMENTS(v) \
    برائے ((v) = placements; (v) != تہی; (v) = (v)->اگلا)

#تعریف_کرو IMAGE_FUNC(b, n) (image_backends[b].image.n)
#تعریف_کرو PLACEMENT_FUNC(b, n) (image_backends[b].placement.n)

ساکن خالی redraw_region(pixman_region32_t *region, bool now, bool restore);

    خالی
init_image_state(خالی)
{
    (خالی)update_image_backend();
}

    خالی
uninit_image_state(خالی)
{
#اگر_تعریف_شدہ FEAT_IMAGE_SIXEL
    sixel_uninit();
#اگر_ختم
}

/*
 * Return true if the there image backend is ready, otherwise emit error.
 */
    ساکن bool
backend_دستیاب(bool پیغام)
{
    اگر (image_backend == IMAGE_BACKEND_NONE)
    {
	اگر (پیغام)
	    emsg(_(e_no_image_backend_دستیاب));
	واپس false;
    }
    واپس true;
}

/*
 * Create a new image using the given data (creates a copy). Returns NULL on
 * failure.
 */
    image_T *
image_new(uint8_t *ڈیٹا, صحیح چوڑائی, صحیح اونچائی, image_format_T fmt)
{
    image_T	    *img;
    ساکن int_u    id;

    اگر (!backend_دستیاب(true))
	واپس تہی;

    img = ALLOC_CLEAR_ONE(image_T);
    اگر (img == تہی)
	واپس تہی;

    img->ڈیٹا = vim_memsave(ڈیٹا, (size_t)چوڑائی * اونچائی * fmt);
    اگر (img->ڈیٹا == تہی)
    {
	vim_free(img);
	واپس تہی;
    }

    img->state = IMAGE_STATE_نجی;
    img->چوڑائی = چوڑائی;
    img->اونچائی = اونچائی;
    img->fmt = fmt;

    // Mix in PID to prevent ID collisions when using kitty graphics protocol
    img->id = ((((int_u)mch_get_pid() & 0x7fff) + 1) << 16) | (id++ & 0xffff);
    img->refcount = 1;

    img->backend = image_backend;

    اگر (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	vim_free(img->ڈیٹا);
	vim_free(img);
	واپس تہی;
    }

    اگر (images != تہی)
	images->پچھلا = img;
    img->اگلا = images;
    images = img;

    واپس img;
}

    ساکن خالی
image_free(image_T *img)
{
    اگر (backend_دستیاب(false))
	IMAGE_FUNC(image_backend, uninit)(img);

    اگر (img->پچھلا != تہی)
	img->پچھلا->اگلا = img->اگلا;
    اگر (img->اگلا != تہی)
	img->اگلا->پچھلا = img->پچھلا;
    اگر (images == img)
	images = img->اگلا;

    vim_free(img->ڈیٹا);
    vim_free(img);
}

/*
 * Decrement reference count of an image. Be careful to not let the public API
 * be able to directly decrement the refcount of an image multiple times!
 */
    خالی
image_unref(image_T *img)
{
    اگر (--img->refcount <= 0)
	image_free(img);
}

/*
 * Increment reference count of an image. Each image placement should have its
 * own reference to the image.
 */
    image_T *
image_ref(image_T *img)
{
    img->refcount++;
    واپس img;
}

/*
 * Get image width and height in cells, rounding up so that every pixel is
 * covered.
 */
    خالی
image_get_cell_dimensions(image_T *img, صحیح *cw, صحیح *ch)
{
    صحیح w = PHY2LOG(img->چوڑائی);
    صحیح h = PHY2LOG(img->اونچائی);

    *cw = (w + cell_چوڑائی - 1) / cell_چوڑائی;
    *ch = (h + cell_اونچائی - 1) / cell_اونچائی;
}

/*
 * Similar, but return the dimensions of all cells that are *fully* covered by
 * pixels. Pixels that are partially covered are not counted.
 */
    ساکن خالی
image_get_cell_dimensions_min(image_T *img, صحیح *cw, صحیح *ch)
{
    صحیح w = PHY2LOG(img->چوڑائی);
    صحیح h = PHY2LOG(img->اونچائی);

    *cw = w / cell_چوڑائی;
    *ch = h / cell_اونچائی;
}

    خالی
image_get_dimensions(image_T *img, صحیح *w, صحیح *h)
{
    *w = img->چوڑائی;
    *h = img->اونچائی;
}

/*
 * Replace the current image contents with "data". "data" should be the exact
 * same format, width, and height. Return OK on success and FAIL on failure.
 */
    صحیح
image_update(image_T *img, uint8_t *ڈیٹا)
{
    صحیح w, h;

    اگر (!backend_دستیاب(true) || img->ڈیٹا == تہی)
	واپس FAIL;

    image_get_dimensions(img, &w, &h);
    mch_memmove(img->ڈیٹا, ڈیٹا, (size_t)w * h * img->fmt);

    IMAGE_FUNC(image_backend, uninit)(img);

    اگر (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	emsg(_(e_برآمد_کا_حافظہ));
	VIM_CLEAR(img->ڈیٹا);
	واپس FAIL;
    }

    img->ver++;
    redraw_all_later(UPD_VALID);
    واپس OK;
}

    ساکن خالی
image_placement_unlink(image_placement_T *رکھو)
{
    اگر (رکھو->پچھلا != تہی)
	رکھو->پچھلا->اگلا = رکھو->اگلا;
    اگر (رکھو->اگلا != تہی)
	رکھو->اگلا->پچھلا = رکھو->پچھلا;
    اگر (placements == رکھو)
	placements = رکھو->اگلا;
    n_placements--;
}

    ساکن خالی
image_placement_link(image_placement_T *رکھو)
{
    image_placement_T *p = placements;
    image_placement_T *پچھلا = تہی;

    // Add placement *before* the placement with the lower zindex.
    جب (p != تہی)
    {
	اگر (رکھو->zindex > p->zindex)
	    توڑو;
	ورنہ اگر (رکھو->zindex == p->zindex)
	{
	    اگر (رکھو->use > p->use)
		توڑو;
	    اگر (رکھو->gen < p->gen)
		توڑو;
	}

	پچھلا = p;
	p = p->اگلا;
    }

    رکھو->اگلا = p;
    رکھو->پچھلا = پچھلا;

    اگر (p != تہی)
	p->پچھلا = رکھو;

    اگر (پچھلا != تہی)
	پچھلا->اگلا = رکھو;
    ورنہ
	placements = رکھو;

    n_placements++;
}

/*
 * Create a new placement for the image, taking ownership of it. By default it
 * will be at the top left corner of the screen, no crop, zindex of 0, and the
 * bounding box will cover the entire image. "img" may be NULL to create a
 * placement with no backing image.
 *
 * "gen" is an arbitrary integer that is used to handle images with same zindex.
 * The image with the higher gen will be placed under others.
 *
 * "use" is the purpose of the image.
 *
 * Note that by default the image is disabled
 *
 * Returns NULL on failure.
 */
    image_placement_T *
image_placement_new(
	image_T			*img,
	int_u			gen,
	image_placement_use_T	use,
	bool			quiet)
{
    image_placement_T	*رکھو;
    ساکن int_u	id = 1; // Kitty placements id must be > 1

    اگر (!backend_دستیاب(!quiet))
	واپس تہی;

    رکھو = ALLOC_CLEAR_ONE(image_placement_T);
    اگر (رکھو == تہی)
	واپس تہی;

    رکھو->id = id;
    id += PLACEMENT_ID_INC; // Allocate 1000 free placement ids to be used to
			    // draw this image placement. Only relevant for
			    // kitty graphics protocol.
    رکھو->gen = gen;
    رکھو->use = use;

    رکھو->img = img;
    رکھو->flags = IMAGEPF_DIRTY;

    اگر (img != تہی)
    {
	رکھو->img_ver = img->ver;
	رکھو->backend = image_backend;
	رکھو->cell_چوڑائی = cell_چوڑائی;
	رکھو->cell_اونچائی = cell_اونچائی;

	// Bounding box is in cells
	image_get_cell_dimensions(img,
		&رکھو->bounding_box.x2, &رکھو->bounding_box.y2);
	رکھو->crop_box.x2 = رکھو->bounding_box.x2;
	رکھو->crop_box.y2 = رکھو->bounding_box.y2;

	اگر (PLACEMENT_FUNC(image_backend, init)(رکھو) == FAIL)
	{
	    id -= PLACEMENT_ID_INC;
	    vim_free(رکھو);
	    واپس تہی;
	}
    }
    ورنہ
	رکھو->backend = IMAGE_BACKEND_NONE;

    image_placement_link(رکھو);

    واپس رکھو;
}

    ساکن خالی
image_placement_clear_int(image_placement_T *رکھو, bool now)
{
    اگر (backend_دستیاب(false) && رکھو->img != تہی)
    {
	اگر (رکھو->flags & IMAGEPF_VISIBLE_INIT
		&& PLACEMENT_FUNC(image_backend, blit))
	    redraw_region(&رکھو->visible_abs, now, true);
	PLACEMENT_FUNC(image_backend, clear)(رکھو);
	رکھو->flags |= IMAGEPF_HIDDEN;
	اگر (!now && !updating_screen)
	    redraw_all_later(UPD_VALID);
    }
}

    خالی
image_placement_clear(image_placement_T *رکھو)
{
    image_placement_clear_int(رکھو, updating_screen);
}

    خالی
image_placement_free(image_placement_T *رکھو)
{
    اگر (backend_دستیاب(false) && رکھو->backend_ڈیٹا != تہی)
    {
	image_placement_clear(رکھو);
	PLACEMENT_FUNC(image_backend, uninit)(رکھو);
    }

    اگر (رکھو->flags & IMAGEPF_VISIBLE_INIT)
    {
	pixman_region32_fini(&رکھو->visible);
	pixman_region32_fini(&رکھو->visible_abs);
    }
    image_placement_unlink(رکھو);

    اگر (رکھو->img != تہی)
	image_unref(رکھو->img);
    vim_free(رکھو);
}

    خالی
image_placement_سیٹ_zindex(image_placement_T *رکھو, صحیح zindex)
{
    // If image was hidden before, must dirty it so it is fully redrawn again.
    اگر (رکھو->flags & IMAGEPF_HIDDEN)
	رکھو->flags |= IMAGEPF_DIRTY;
    رکھو->flags &= ~IMAGEPF_HIDDEN;

    اگر (رکھو->zindex == zindex)
	واپس;
    رکھو->zindex = zindex;
    رکھو->flags |= IMAGEPF_DIRTY;

    // Must re-add the placement back so it is in the correct order
    image_placement_unlink(رکھو);
    image_placement_link(رکھو);
}

/*
 * Make sure to also update the bounding box!
 */
    خالی
image_placement_سیٹ_position(image_placement_T *رکھو, صحیح صف, صحیح ستون)
{
    اگر (رکھو->flags & IMAGEPF_HIDDEN)
	رکھو->flags |= IMAGEPF_DIRTY;
    رکھو->flags &= ~IMAGEPF_HIDDEN;

    اگر (رکھو->صف == صف && رکھو->ستون == ستون)
	واپس;
    رکھو->صف = صف;
    رکھو->ستون = ستون;
    رکھو->flags |= IMAGEPF_DIRTY;
}

    خالی
image_placement_سیٹ_crop(image_placement_T *رکھو, صحیح x, صحیح y, صحیح w, صحیح h)
{
    اگر (رکھو->flags & IMAGEPF_HIDDEN)
	رکھو->flags |= IMAGEPF_DIRTY;
    رکھو->flags &= ~IMAGEPF_HIDDEN;

    اگر (رکھو->crop_box.x1 == x && رکھو->crop_box.y1 == y
	    && رکھو->crop_box.x2 == x + w && رکھو->crop_box.y2 == y + h)
	واپس;

    رکھو->crop_box.x1 = x;
    رکھو->crop_box.y1 = y;

    رکھو->crop_box.x2 = x + w;
    رکھو->crop_box.y2 = y + h;
    رکھو->flags |= IMAGEPF_DIRTY;
}

    خالی
image_placement_سیٹ_bounding_box(
	image_placement_T   *رکھو,
	صحیح		    صف,
	صحیح		    ستون,
	صحیح		    صف_اونچائی,
	صحیح		    ستون_چوڑائی)
{
    اگر (رکھو->flags & IMAGEPF_HIDDEN)
	رکھو->flags |= IMAGEPF_DIRTY;
    رکھو->flags &= ~IMAGEPF_HIDDEN;

    اگر (رکھو->bounding_box.x1 == ستون && رکھو->bounding_box.y1 == صف
	    && رکھو->bounding_box.x2 == ستون + ستون_چوڑائی
	    && رکھو->bounding_box.y2 == صف + صف_اونچائی)
	واپس;

    رکھو->bounding_box.x1 = ستون;
    رکھو->bounding_box.y1 = صف;

    رکھو->bounding_box.x2 = ستون + ستون_چوڑائی;
    رکھو->bounding_box.y2 = صف + صف_اونچائی;
    رکھو->flags |= IMAGEPF_DIRTY;
}

/*
 * Enable or disable the image placement. Unlike IMAGEPF_HIDDEN, this is not
 * reset across redraws.
 */
    خالی
image_placement_سیٹ_enable(image_placement_T *رکھو, bool enable)
{
    bool old = رکھو->flags & IMAGEPF_ENABLED;

    اگر (enable && !old)
	رکھو->flags |= IMAGEPF_ENABLED;
    ورنہ اگر (!enable && old)
    {
	رکھو->flags &= ~IMAGEPF_ENABLED;
	image_placement_clear(رکھو);
    }
    ورنہ
	واپس;

    رکھو->flags |= IMAGEPF_DIRTY;
}

/*
 * Note that the returned crop region (x, y, w, h) is in physical pixels if
 * "physical" is true. Not relevant when using terminal protocols (sixel, KGP).
 */
    خالی
image_placement_subrect(
	image_placement_T   *رکھو,
	pixman_box32_t	    rect,
	صحیح		    *صف,
	صحیح		    *ستون,
	صحیح		    *x,
	صحیح		    *y,
	صحیح		    *w,
	صحیح		    *h,
	bool		    physical)
{
    صحیح iw, ih;

    image_get_dimensions(رکھو->img, &iw, &ih);

    *صف = رکھو->صف + rect.y1 + رکھو->صف_off;
    *ستون = رکھو->ستون + rect.x1;

    *x = (rect.x1 + رکھو->crop_box.x1) * cell_چوڑائی;
    *y = (rect.y1 + رکھو->crop_box.y1) * cell_اونچائی;
    *w = (rect.x2 - rect.x1) * cell_چوڑائی;
    *h = (rect.y2 - rect.y1) * cell_اونچائی;

    اگر (physical)
    {
	*x = LOG2PHY(*x);
	*y = LOG2PHY(*y);
	*w = LOG2PHY(*w);
	*h = LOG2PHY(*h);
    }
    ورنہ
    {
	iw = PHY2LOG(iw);
	ih = PHY2LOG(ih);
    }

    // Make that all the values are valid. "w" and "h" can especially be
    // invalid, because the image dimensions (in pixels) are converted to cells,
    // rounding *up*.
    *صف = MIN(*صف, صفیں);
    *ستون = MIN(*ستون, Columns);

    *x = MIN(iw, MAX(*x, 0));
    *y = MIN(ih, MAX(*y, 0));
    *w = MIN(*w, iw - *x);
    *h = MIN(*h, ih - *y);
}

/*
 * Redraw the cells in the given region. This is only relevant for image
 * backends that blit pixels. If "now" is true, then draw the characters now
 * instead of deferring to next redraw. If "restore" is true, restore the cursor
 * position.
 */
    ساکن خالی
redraw_region(pixman_region32_t *region, bool now, bool restore)
{
    pixman_box32_t  *rects;
    صحیح		    n_rects;
    صحیح		    cur_صف = screen_cur_صف;
    صحیح		    cur_ستون = screen_cur_ستون;

    rects = pixman_region32_rectangles(region, &n_rects);
    اگر (rects == تہی)
	واپس;

    برائے (صحیح i = 0; i < n_rects; i++)
    {
	pixman_box32_t rect = rects[i];

	اگر (now)
	{
	    // The region may be from before a resize, when the screen was
	    // bigger.
	    صحیح اونچائی = MIN(rect.y2, screen_صفیں) - rect.y1;
	    صحیح چوڑائی = MIN(rect.x2, screen_Columns) - rect.x1;

	    // Tip: if you want to debug stale pixels appearing, set "inverse"
	    // of screen_draw_rectangle() to TRUE.
	    اگر (اونچائی > 0 && چوڑائی > 0)
		screen_draw_rectangle(rect.y1, rect.x1, اونچائی, چوڑائی,
			FALSE, TRUE);
	    جاری;
	}

	برائے (صحیح r = rect.y1; r < rect.y2; r++)
	{
	    اگر (r >= screen_صفیں)
		توڑو;

	    برائے (صحیح c = rect.x1; c < rect.x2; c++)
	    {
		اگر (c >= screen_Columns)
		    توڑو;

		ScreenAttrs[سطر_انحراف[r] + c] = (sattr_T)-1;
	    }
	}
    }

    اگر (restore)
	windgoto(cur_صف, cur_ستون);
}

/*
 * Draw all image placements to the screen. This should be done after all text
 * have been drawn to the screen, and image placements positioned correctly.
 */
    خالی
draw_image_placements(خالی)
{
    image_placement_T	*رکھو;
    pixman_region32_t	تفریق_region;    // In cells
    pixman_region32_t	dirty_region;	    // In cells
    image_placement_T	**pending_placements;
    صحیح			pending_len = -1;
    // Save current cursor position
    صحیح			cur_صف = screen_cur_صف;
    صحیح			cur_ستون = screen_cur_ستون;

    // Check if all placements have no backing image. If so, then do nothing
    FOR_ALL_PLACEMENTS(رکھو)
	اگر (رکھو->img != تہی)
	{
	    pending_len = 0;
	    توڑو;
	}

    اگر (pending_len == -1)
	واپس;

    اگر (!backend_دستیاب(false))
	واپس;

    اگر (n_placements == 0)
	واپس;

    pending_placements = ALLOC_CLEAR_MULT(image_placement_T *, n_placements);
    اگر (pending_placements == تہی)
	واپس;

    pixman_region32_init(&تفریق_region);
    pixman_region32_init(&dirty_region);

    cursor_off();

    // Go through each image placement, from highest to lowest zindex. For each
    // image, subtract the bounding boxes of the images with higher zindexes
    // from its own image region. The result is a region containing rectangles
    // that represent only the visible regions of the image that should be
    // drawn.
    //
    // We must do two passes, one to find what images need to be redrawn, and
    // also what regions are stale and redraw them. If we did everything in one
    // pass, stale regions of one image that overlap another image with a higher
    // index, would overwrite the overlapping image.
    FOR_ALL_PLACEMENTS(رکھو)
    {
	image_T *img = رکھو->img;

	pixman_region32_t   image_region;
	pixman_region32_t   visible_region;

	صحیح x, y;

	اگر (رکھو->flags & IMAGEPF_HIDDEN || !(رکھو->flags & IMAGEPF_ENABLED))
	    // Don't need to clear image placement for non-blit backends,
	    // because we call image_placement_clear() every time we don't want
	    // to see the image. Not sure of the side effects if we also clear
	    // it here...
	    جاری;

	اگر (img != تہی)
	{
	    pixman_region32_t	visible_abs;
	    صحیح			crop_w, crop_h;
	    bool		cs_بدلا;
	    bool		need_redraw;

	    // Processing changed image data failed, skip this image
	    اگر (رکھو->img->ڈیٹا == تہی)
	    {
		image_placement_clear(رکھو);
		جاری;
	    }

	    // Only apply row shift if image is within the shifted region.
	    اگر ((رکھو->bounding_box.y1 >= سرکاؤ_چوٹی
			&& رکھو->bounding_box.y2 <= سرکاؤ_bot)
		    && رکھو->صف_off != سرکاؤ)
	    {
		رکھو->صف_off = سرکاؤ;
		رکھو->flags |= IMAGEPF_DIRTY;
	    }

	    x = رکھو->ستون;
	    y = رکھو->صف + رکھو->صف_off;

	    crop_w = رکھو->crop_box.x2 - رکھو->crop_box.x1;
	    crop_h = رکھو->crop_box.y2 - رکھو->crop_box.y1;

	    // Don't add "crop_box" x1 and y1, because "row" and "col" use the
	    // top left of the final cropped image.
	    pixman_region32_init_rect(&image_region, x, y, crop_w, crop_h);

	    pixman_region32_init(&visible_region);
	    pixman_region32_init(&visible_abs);

	    // Only use parts of the image region within the actual screen.
	    // "row_off" may offset the image slightly/completely off screen.
	    اگر (!pixman_region32_intersect_rect(&image_region, &image_region,
			0, 0, Columns, صفیں)
		    || !pixman_region32_تفریق(&visible_region,
			&image_region, &تفریق_region)
		    || !pixman_region32_copy(&visible_abs, &visible_region))
	    {
		pixman_region32_fini(&image_region);
		pixman_region32_fini(&visible_region);
		جاری;
	    }

	    pixman_region32_fini(&image_region);

	    // The visible region is in absolute coordinates, must convert it
	    // into image relative coordinates.
	    pixman_region32_translate(&visible_region, -x, -y);

	    cs_بدلا = رکھو->cell_چوڑائی != cell_چوڑائی
		|| رکھو->cell_اونچائی != cell_اونچائی;

	    // If cell size changed, then the visible regions are invalid pixel
	    // wise. This prevents backends such as sixel from using cached
	    // sequences.
	    اگر (cs_بدلا)
	    {
		رکھو->cell_چوڑائی = cell_چوڑائی;
		رکھو->cell_اونچائی = cell_اونچائی;
		رکھو->flags |= IMAGEPF_FORCE;
	    }

	    // Only redraw the image if it has changed (or if we haven't drawn
	    // it yet). Or if image data has changed or cell size has changed
	    need_redraw = رکھو->flags & IMAGEPF_DIRTY || cs_بدلا
		||  (رکھو->flags & IMAGEPF_VISIBLE_INIT
			&& !pixman_region32_equal(&visible_region,
			    &رکھو->visible));

	    اگر (رکھو->img_ver != img->ver)
	    {
		// Image data changed
		need_redraw = true;
		رکھو->img_ver = img->ver;

		image_placement_clear(رکھو);
		PLACEMENT_FUNC(image_backend, uninit)(رکھو);
		اگر (PLACEMENT_FUNC(image_backend, init)(رکھو) == FAIL)
		{
		    emsg(_(e_برآمد_کا_حافظہ));
		    image_unref(رکھو->img);
		    رکھو->img = تہی;
		    جاری;
		}
	    }

	    اگر (need_redraw)
	    {
		اگر (رکھو->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&رکھو->visible);
		رکھو->visible = visible_region;

		اگر (PLACEMENT_FUNC(image_backend, blit))
		{
		    // Must redraw the stale regions that will not be composited
		    // over (for this specific image).
		    pixman_region32_t stale_region;

		    pixman_region32_init(&stale_region);

		    اگر (رکھو->flags & IMAGEPF_VISIBLE_INIT)
		    {
			(خالی)pixman_region32_تفریق(&stale_region,
				&رکھو->visible_abs, &visible_abs);

			// Since pixels may be translucent/transparent, we must
			// redraw the cells behind the image every time, so that
			// no stale pixels remain from the previous redraw.
			اگر (img->fmt == IMAGE_FORMAT_RGBA)
			    (خالی)pixman_region32_union(&stale_region,
				    &stale_region, &رکھو->visible_abs);
		    }

		    // The visible region is guaranteed to cover every single
		    // pixel. However when the visible region is converted to
		    // pixels, that means the resulting rectangles may be bigger
		    // than the image itself.
		    //
		    // We clamp the values in image_placement_subrect(), however
		    // that means partially covered cells will not be drawn over
		    // fully, and therefore could contain stale content. As
		    // such, subtract the minimum region from the visible region
		    // to get the resulting region containing partially covered
		    // cells that may have stale pixels still on them.
		    //
		    // Not needed for RGBA images, because we redraw the visible
		    // region every time anyways.
		    اگر (img->fmt != IMAGE_FORMAT_RGBA)
		    {
			صحیح		    min_w, min_h;
			صحیح		    rect_w, rect_h;
			pixman_region32_t   min_region;

			image_get_cell_dimensions_min(img, &min_w, &min_h);

			// Ensure "min_region" only covers the fully covered
			// cells of the cropped region.
			rect_w = min_w - رکھو->crop_box.x1;
			rect_h = min_h - رکھو->crop_box.y1;

			اگر (rect_w > 0 && rect_h > 0)
			    // pixman_region32_init_rect() expects a non zero
			    // width and height (otherwise it outputs error
			    // message).
			    pixman_region32_init_rect(&min_region,
				    x, y, rect_w, rect_h);
			ورنہ
			    pixman_region32_init(&min_region);

			(خالی)pixman_region32_تفریق(&min_region,
				&visible_abs, &min_region);
			(خالی)pixman_region32_union(&stale_region,
				&stale_region, &min_region);
			pixman_region32_fini(&min_region);
		    }

		    اگر (رکھو->flags & IMAGEPF_VISIBLE_INIT)
			// Also subtract "subtract_region", so we don't
			// redundantly redraw cells that will have images
			// painted over them after. Make sure to do this before
			// we check partially covered cells, since
			// "subtract_region" may also have partially covered
			// cells.
			(خالی)pixman_region32_تفریق(&stale_region,
				&stale_region, &تفریق_region);

		    // Don't redraw now, defer it later
		    pixman_region32_union(&dirty_region,
			    &dirty_region, &stale_region);
		    pixman_region32_fini(&stale_region);
		}
		اگر (رکھو->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&رکھو->visible_abs);
		رکھو->visible_abs = visible_abs;

		pending_placements[pending_len++] = رکھو;
		رکھو->flags |= IMAGEPF_VISIBLE_INIT;
		رکھو->flags &= ~IMAGEPF_DIRTY;
	    }
	    ورنہ
	    {
		pixman_region32_fini(&visible_region);
		pixman_region32_fini(&visible_abs);
	    }
	}

	// If this is last image, no need to union its bounds.
	اگر (رکھو->اگلا == تہی)
	    توڑو;

	pixman_region32_union_rect(&تفریق_region, &تفریق_region,
		رکھو->bounding_box.x1,
		رکھو->bounding_box.y1 + رکھو->صف_off,
		رکھو->bounding_box.x2 - رکھو->bounding_box.x1,
		رکھو->bounding_box.y2 - رکھو->bounding_box.y1);
    }

    // Don't want to dirty images again redundantly
    freeze_mark_dirty++;
    redraw_region(&dirty_region, true, false);
    freeze_mark_dirty--;

    اگر (pending_len > 0)
    {
#اگر_تعریف_شدہ FEAT_GUI
	// Make sure to flush any output! We write directly to the draw
	// area/whatever, meaning draw commands outputted before this (as escape
	// sequences), will still draw over the image surface if we don't flush
	// now.
	اگر (gui.اندر_use)
	    برآمد_flush();
#اگر_ختم

	برائے (صحیح i = 0; i < pending_len; i++)
	{
	    PLACEMENT_FUNC(image_backend, draw)(pending_placements[i]);
	    pending_placements[i]->flags &= ~IMAGEPF_FORCE;
	}
    }

    // Restore cursor position, however do not turn on the cursor, since that
    // causes it to flicker at the wrong position when using sixel. It will be
    // turned on later somewhere...
    windgoto(cur_صف, cur_ستون);
    برآمد_flush();

    vim_free(pending_placements);
    pixman_region32_fini(&تفریق_region);
    pixman_region32_fini(&dirty_region);
}

/*
 * Mark any images that touches the region dirty. Only used for backends that
 * blit pixels.
 */
    خالی
mark_dirty_region_for_images(صحیح صف, صحیح ستون, صحیح صف_اونچائی, صحیح ستون_چوڑائی)
{
    image_placement_T	*رکھو;
    pixman_box32_t	rect;

    اگر (freeze_mark_dirty > 0 || !backend_دستیاب(false)
	    || !PLACEMENT_FUNC(image_backend, blit) || n_placements == 0)
	واپس;

    rect.x1 = ستون;
    rect.y1 = صف;
    rect.x2 = ستون + ستون_چوڑائی;
    rect.y2 = صف + صف_اونچائی;

    FOR_ALL_PLACEMENTS(رکھو)
    {
	اگر (رکھو->flags & IMAGEPF_HIDDEN)
	    جاری;
	اگر (pixman_region32_شامل_ہے_rectangle(&رکھو->visible_abs, &rect)
		== PIXMAN_REGION_برآمد)
	    جاری;

	رکھو->flags |= IMAGEPF_DIRTY;
    }
}

    خالی
clear_all_image_placements(خالی)
{
    image_placement_T *رکھو;

    FOR_ALL_PLACEMENTS(رکھو)
	image_placement_clear(رکھو);
    اگر (!updating_screen)
	redraw_all_later(UPD_VALID);
}

/*
 * Used for screen_ins_lines() and screen_del_lines(), because they directly
 * shift rows. Note that this does not queue a screen redraw.
 */
    خالی
سرکاؤ_image_placements(صحیح چوٹی, صحیح bot, صحیح amount)
{
    سرکاؤ_چوٹی = چوٹی;
    سرکاؤ_bot = bot;
    سرکاؤ += amount;
}

/*
 * Called when screen is cleared, as update_screen() may go to clearing the
 * screen instead of inserting lines using screen_ins_lines().
 */
    خالی
سرکاؤ_reset_image_placements(خالی)
{
    سرکاؤ = 0;
}

    ساکن image_T *
ڈھونڈو_image(int_u id)
{
    image_T *img;

    FOR_ALL_IMAGES(img)
	اگر (img->state == IMAGE_STATE_عوامی && img->id == id)
	    واپس img;
    واپس تہی;
}

/*
 * Add an image using the given information in "dict". If "existing" is not
 * NULL, try updating it instead. If "find" is TRUE, then accept the "id" field.
 */
    image_T *
جوڑو_image(dict_T *dict, image_T *existing, bool ڈھونڈو)
{
    dictitem_T	    *di;
    blob_T	    *ڈیٹا;
    varnumber_T     w, h;
    varnumber_T     n_pixels;
    image_format_T  fmt;

    اگر (ڈھونڈو && dict_has_کلید(dict, "id"))
    {
	image_T *img = ڈھونڈو_image((int_u)dict_get_نمبر(dict, "id"));

	اگر (img == تہی)
	    واپس تہی;
	واپس image_ref(img);
    }

    di = dict_ڈھونڈو(dict, (char_u *)"data", -1);
    w = dict_get_نمبر(dict, "width");
    h = dict_get_نمبر(dict, "height");

    اگر (di == تہی || di->di_tv.v_type != VAR_BLOB || w <= 0 || h <= 0)
    {
	emsg(_(e_invalid_دلیل));
	واپس تہی;
    }

    // Check for overflow
    n_pixels = w * h;
    اگر (w > زیادہ_سے_زیادہ_عدد || h > زیادہ_سے_زیادہ_عدد || n_pixels > زیادہ_سے_زیادہ_عدد)
    {
	emsg(_(e_invalid_image_dimensions));
	واپس تہی;
    }

    ڈیٹا = di->di_tv.vval.v_blob;

    اگر (blob_len(ڈیٹا) == n_pixels * 3)
	fmt = IMAGE_FORMAT_RGB;
    ورنہ اگر (blob_len(ڈیٹا) == n_pixels * 4)
	fmt = IMAGE_FORMAT_RGBA;
    ورنہ
    {
	semsg(_(e_invalid_قدر_for_دلیل_str_str), "data",
		"data length must equal width*height*3 or width*height*4");
	واپس تہی;
    }

    اگر (existing != تہی)
    {
	اگر (existing->چوڑائی == w && existing->اونچائی == h
		&& existing->fmt == fmt)
	{
	    اگر (image_update(existing, ڈیٹا->bv_ga.ga_ڈیٹا) == FAIL)
		واپس تہی;
	    واپس existing;
	}
    }

    واپس image_new(ڈیٹا->bv_ga.ga_ڈیٹا, w, h, fmt);
}

/*
 * Return a dict containing information about the given dict. Note that
 * reference count of dict is not set. Returns NULL on failure.
 */
    dict_T *
get_image_info(image_T *img)
{
    dict_T	*dict;
    blob_T	*blob;
    صحیح		w, h;
    dictitem_T	*di;

    اگر (img->ڈیٹا == تہی)
	واپس تہی;

    dict = dict_alloc();
    اگر (dict == تہی)
	واپس تہی;

    blob = blob_alloc();
    اگر (blob == تہی)
    {
	dict_unref(dict);
	واپس تہی;
    }

    image_get_dimensions(img, &w, &h);
    (خالی)ga_concat_بائٹیں(&blob->bv_ga, (حرف *)img->ڈیٹا,
	    (size_t)w * h * img->fmt);

    di = dictitem_alloc((char_u *)"data");

    اگر (di != تہی)
    {
	di->di_tv.v_type = VAR_BLOB;
	di->di_tv.vval.v_blob = blob;
    }
    اگر (di == تہی || dict_جوڑو(dict, di) == FAIL)
    {
	اگر (di != تہی)
	    dictitem_free(di);
	dict_unref(dict);
	// No need to unref blob, dictitem_free() will do that.
	واپس تہی;
    }

    اگر (img->state == IMAGE_STATE_عوامی)
	dict_جوڑو_نمبر(dict, "id", img->id);
    dict_جوڑو_نمبر(dict, "width", img->چوڑائی);
    dict_جوڑو_نمبر(dict, "height", img->اونچائی);
    dict_جوڑو_نمبر(dict, "alpha", img->fmt == IMAGE_FORMAT_RGBA);
    dict_جوڑو_string(dict, "format",
	    (char_u *)(img->fmt == IMAGE_FORMAT_RGB ? "rgb" : "rgba"));

    blob->bv_refcount = 1;

    واپس dict;
}

    ساکن صحیح
مطابقت_imageprotocol(image_backend_T *backend)
{
    صحیح		    len = (صحیح)STRLEN(p_ipc) + 1;
    char_u	    *buf = alloc(len);
    char_u	    *p = p_ipc;
    صحیح		    ret = FAIL;
    image_backend_T res = IMAGE_BACKEND_NONE;
    bool	    got = false;

    اگر (buf == تہی)
	واپس FAIL;

    جب (*p != NUL)
    {
	char_u		*colon;
	regmatch_T	regmatch;
	image_backend_T prot = IMAGE_BACKEND_NONE;

	// Isolate one comma separated item.
	(خالی)copy_option_part(&p, buf, len, ",");

	colon = vim_strchr(buf, ':');
	اگر (colon == تہی || colon == buf || colon[1] == NUL)
	    جاؤ باہر_نکلو;

	*colon = NUL;

	// Note: Keep this in sync with p_ipc_protocol_values.
	اگر (STRCMP(colon + 1, "none") == 0)
	    prot = IMAGE_BACKEND_NONE;
	ورنہ اگر (STRCMP(colon + 1, "kitty") == 0)
	{
#اگر_تعریف_شدہ FEAT_IMAGE_KITTY
	    prot = IMAGE_BACKEND_KITTY;
#اگر_ختم
	}
	ورنہ اگر (STRCMP(colon + 1, "sixel") == 0)
	{
#اگر_تعریف_شدہ FEAT_IMAGE_SIXEL
	    prot = IMAGE_BACKEND_SIXEL;
#اگر_ختم
	}
	ورنہ
	    جاؤ باہر_نکلو;

	اگر (got)
	    جاری;

	CLEAR_میدان(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);

	اگر (regmatch.regprog == تہی)
	    جاؤ باہر_نکلو;

	bool مطابقت = T_NAME != تہی
	    && vim_regexec(&regmatch, T_NAME, (colnr_T)0);

	vim_regfree(regmatch.regprog);
	اگر (مطابقت)
	{
	    // Keep going to catch any errors
	    res = prot;
	    got = true;
	}
    }

    ret = OK;
    *backend = res;
باہر_نکلو:
    vim_free(buf);
    واپس ret;
}

/*
 * Update the current image backend to use depending on 'imageprotocol' and if
 * GUI is being used. Returns OK on success and FAIL on failure.
 */
    صحیح
update_image_backend(خالی)
{
    image_backend_T new;

    image_T		*img;
    image_placement_T	*رکھو;

    // Always process 'imageprotocol' option to check for errors
    اگر (مطابقت_imageprotocol(&new) == FAIL)
	واپس FAIL;

#اگر_تعریف_شدہ FEAT_IMAGE_GUI
    اگر (gui.اندر_use)
	new = IMAGE_BACKEND_GUI;
#اگر_ختم

    اگر (image_backend == new)
	واپس OK;

    // Must uninit the backends of every image/placement, then init the new
    // backend for each.
    FOR_ALL_IMAGES(img)
    {
	اگر (image_backend != IMAGE_BACKEND_NONE && img->ڈیٹا != تہی
		&& img->backend_ڈیٹا != تہی)
	    IMAGE_FUNC(image_backend, uninit)(img);
	img->backend_ڈیٹا = تہی;
	اگر (new != IMAGE_BACKEND_NONE)
	{
	    اگر (IMAGE_FUNC(new, init)(img) == FAIL)
		جاؤ fail;
	    img->backend = new;
	}
    }
    FOR_ALL_PLACEMENTS(رکھو)
    {
	image_placement_clear(رکھو);
	اگر (image_backend != IMAGE_BACKEND_NONE
		&& رکھو->img != تہی && رکھو->backend_ڈیٹا != تہی)
	    PLACEMENT_FUNC(image_backend, uninit)(رکھو);
	رکھو->backend_ڈیٹا = تہی;
	اگر (new != IMAGE_BACKEND_NONE && رکھو->img != تہی)
	{
	    اگر (PLACEMENT_FUNC(new, init)(رکھو) == FAIL)
		جاؤ fail;
	    رکھو->backend = new;
	}
    }

    redraw_all_later(UPD_VALID);
    image_backend = new;

    اگر (new == IMAGE_BACKEND_NONE)
	سیٹ_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    ورنہ
	سیٹ_vim_var_string(VV_IMAGEBACKEND,
		(char_u *)image_backends[image_backend].name, -1);

    واپس OK;
fail:
    FOR_ALL_IMAGES(img)
	اگر (img->backend != IMAGE_BACKEND_NONE)
	{
	    IMAGE_FUNC(img->backend, uninit)(img);
	    img->backend = IMAGE_BACKEND_NONE;
	}

    FOR_ALL_PLACEMENTS(رکھو)
    {
	اگر (رکھو->img != تہی && رکھو->backend != IMAGE_BACKEND_NONE)
	{
	    image_placement_clear(رکھو);
	    PLACEMENT_FUNC(رکھو->backend, uninit)(رکھو);
	}
	رکھو->backend = IMAGE_BACKEND_NONE;
    }

    redraw_all_later(UPD_VALID);

    image_backend = IMAGE_BACKEND_NONE;
    سیٹ_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    emsg(_(e_changing_image_backend_failed));
    واپس FAIL;
}

    خالی
f_image_جوڑو(typval_T *argvars, typval_T *rettv)
{
    dict_T  *dict;
    image_T *img;

    اگر (check_for_dict_arg(argvars, 0) == FAIL)
	واپس;

    dict = argvars[0].vval.v_dict;

    img = جوڑو_image(dict, تہی, false);
    اگر (img != تہی)
	img->state = IMAGE_STATE_عوامی;

    rettv->v_type = VAR_نمبر;
    rettv->vval.v_نمبر = img == تہی ? (varnumber_T)-1 : img->id;
}

    خالی
f_image_discard(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    image_T *img;

    اگر (check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    img = ڈھونڈو_image((int_u)argvars[0].vval.v_نمبر);

    اگر (img != تہی)
    {
	img->state = IMAGE_STATE_نجی;
	image_unref(img);
    }
    ورنہ
	semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_نمبر);
}

    خالی
f_image_info(typval_T *argvars, typval_T *rettv)
{
    image_T *img;
    dict_T  *dict;

    اگر (rettv_list_alloc(rettv) == FAIL
	    || check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_نمبر)
    {
	img = ڈھونڈو_image((int_u)argvars[0].vval.v_نمبر);
	اگر (img == تہی)
	{
	    semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_نمبر);
	    واپس;
	}
	dict = get_image_info(img);
	اگر (dict != تہی && list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	واپس;
    }

    FOR_ALL_IMAGES(img)
    {
	اگر (img->state == IMAGE_STATE_نجی)
	    جاری;
	dict = get_image_info(img);
	اگر (dict == تہی
		|| list_append_dict(rettv->vval.v_list, dict) == FAIL)
	{
	    dict_unref(dict);	// NULL-safe
	    واپس;
	}
    }
}

#اگر_ختم // FEAT_IMAGE
