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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_IMAGE)

నిర్మాణం
{
    స్థిరరాశి అక్షరం *name;

    నిర్మాణం
    {
	// Return OK on success and FAIL on failure
	పూర్ణాంకం (*init)(image_T *);
	శూన్య (*uninit)(image_T *);
    } image;

    నిర్మాణం
    {
	// Return OK on success and FAIL on failure
	పూర్ణాంకం (*init)(image_placement_T *);
	శూన్య (*uninit)(image_placement_T *);

	శూన్య (*draw)(image_placement_T *);
	శూన్య (*clear)(image_placement_T *);

	// If backend blits the image pixels to the screen. Images are not
	// treated as "objects".
	bool blit;
    } placement;
} image_backends[] = {
#నిర్వచితమైతే FEAT_IMAGE_GUI
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
#  లేకపోతే
	    .blit = true,
#  endif
# elif FEAT_GUI_MSWIN
	    .blit = true,
# లేకపోతే
	    .blit = false,
# endif
	}
    },
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE_KITTY
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
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE_SIXEL
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
#అయితే_ముగింపు
};

// Current image backend being used
స్థిర image_backend_T image_backend = IMAGE_BACKEND_NONE;

స్థిర image_T *images = ఖాళీ;
// Sorted from highest zindex to lowest zindex
స్థిర image_placement_T    *placements = ఖాళీ;
స్థిర పూర్ణాంకం		    n_placements = 0;

స్థిర పూర్ణాంకం జరుపు_శిఖరం = 0;
స్థిర పూర్ణాంకం జరుపు_bot = 0;
స్థిర పూర్ణాంకం జరుపు = 0;

// If > 0, then mark_dirty_region_for_images() will be a no-op
స్థిర పూర్ణాంకం freeze_mark_dirty = 0;

#నిర్వచించు FOR_ALL_IMAGES(v) కొరకు ((v) = images; (v) != ఖాళీ; (v) = (v)->తదుపరి)
#నిర్వచించు FOR_ALL_PLACEMENTS(v) \
    కొరకు ((v) = placements; (v) != ఖాళీ; (v) = (v)->తదుపరి)

#నిర్వచించు IMAGE_FUNC(b, n) (image_backends[b].image.n)
#నిర్వచించు PLACEMENT_FUNC(b, n) (image_backends[b].placement.n)

స్థిర శూన్య redraw_region(pixman_region32_t *region, bool now, bool restore);

    శూన్య
init_image_state(శూన్య)
{
    (శూన్య)update_image_backend();
}

    శూన్య
uninit_image_state(శూన్య)
{
#నిర్వచితమైతే FEAT_IMAGE_SIXEL
    sixel_uninit();
#అయితే_ముగింపు
}

/*
 * Return true if the there image backend is ready, otherwise emit error.
 */
    స్థిర bool
backend_అందుబాటు(bool సందేశం)
{
    ఒకవేళ (image_backend == IMAGE_BACKEND_NONE)
    {
	ఒకవేళ (సందేశం)
	    emsg(_(e_no_image_backend_అందుబాటు));
	తిరిగి false;
    }
    తిరిగి true;
}

/*
 * Create a new image using the given data (creates a copy). Returns NULL on
 * failure.
 */
    image_T *
image_new(uint8_t *దత్తాంశం, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు, image_format_T fmt)
{
    image_T	    *img;
    స్థిర int_u    id;

    ఒకవేళ (!backend_అందుబాటు(true))
	తిరిగి ఖాళీ;

    img = ALLOC_CLEAR_ONE(image_T);
    ఒకవేళ (img == ఖాళీ)
	తిరిగి ఖాళీ;

    img->దత్తాంశం = vim_memsave(దత్తాంశం, (size_t)వెడల్పు * ఎత్తు * fmt);
    ఒకవేళ (img->దత్తాంశం == ఖాళీ)
    {
	vim_free(img);
	తిరిగి ఖాళీ;
    }

    img->state = IMAGE_STATE_వ్యక్తిగత;
    img->వెడల్పు = వెడల్పు;
    img->ఎత్తు = ఎత్తు;
    img->fmt = fmt;

    // Mix in PID to prevent ID collisions when using kitty graphics protocol
    img->id = ((((int_u)mch_get_pid() & 0x7fff) + 1) << 16) | (id++ & 0xffff);
    img->refcount = 1;

    img->backend = image_backend;

    ఒకవేళ (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	vim_free(img->దత్తాంశం);
	vim_free(img);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (images != ఖాళీ)
	images->prev = img;
    img->తదుపరి = images;
    images = img;

    తిరిగి img;
}

    స్థిర శూన్య
image_free(image_T *img)
{
    ఒకవేళ (backend_అందుబాటు(false))
	IMAGE_FUNC(image_backend, uninit)(img);

    ఒకవేళ (img->prev != ఖాళీ)
	img->prev->తదుపరి = img->తదుపరి;
    ఒకవేళ (img->తదుపరి != ఖాళీ)
	img->తదుపరి->prev = img->prev;
    ఒకవేళ (images == img)
	images = img->తదుపరి;

    vim_free(img->దత్తాంశం);
    vim_free(img);
}

/*
 * Decrement reference count of an image. Be careful to not let the public API
 * be able to directly decrement the refcount of an image multiple times!
 */
    శూన్య
image_unref(image_T *img)
{
    ఒకవేళ (--img->refcount <= 0)
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
    తిరిగి img;
}

/*
 * Get image width and height in cells, rounding up so that every pixel is
 * covered.
 */
    శూన్య
image_get_cell_dimensions(image_T *img, పూర్ణాంకం *cw, పూర్ణాంకం *ch)
{
    పూర్ణాంకం w = PHY2LOG(img->వెడల్పు);
    పూర్ణాంకం h = PHY2LOG(img->ఎత్తు);

    *cw = (w + cell_వెడల్పు - 1) / cell_వెడల్పు;
    *ch = (h + cell_ఎత్తు - 1) / cell_ఎత్తు;
}

/*
 * Similar, but return the dimensions of all cells that are *fully* covered by
 * pixels. Pixels that are partially covered are not counted.
 */
    స్థిర శూన్య
image_get_cell_dimensions_min(image_T *img, పూర్ణాంకం *cw, పూర్ణాంకం *ch)
{
    పూర్ణాంకం w = PHY2LOG(img->వెడల్పు);
    పూర్ణాంకం h = PHY2LOG(img->ఎత్తు);

    *cw = w / cell_వెడల్పు;
    *ch = h / cell_ఎత్తు;
}

    శూన్య
image_get_dimensions(image_T *img, పూర్ణాంకం *w, పూర్ణాంకం *h)
{
    *w = img->వెడల్పు;
    *h = img->ఎత్తు;
}

/*
 * Replace the current image contents with "data". "data" should be the exact
 * same format, width, and height. Return OK on success and FAIL on failure.
 */
    పూర్ణాంకం
image_update(image_T *img, uint8_t *దత్తాంశం)
{
    పూర్ణాంకం w, h;

    ఒకవేళ (!backend_అందుబాటు(true) || img->దత్తాంశం == ఖాళీ)
	తిరిగి FAIL;

    image_get_dimensions(img, &w, &h);
    mch_memmove(img->దత్తాంశం, దత్తాంశం, (size_t)w * h * img->fmt);

    IMAGE_FUNC(image_backend, uninit)(img);

    ఒకవేళ (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	emsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం));
	VIM_CLEAR(img->దత్తాంశం);
	తిరిగి FAIL;
    }

    img->ver++;
    redraw_all_later(UPD_VALID);
    తిరిగి OK;
}

    స్థిర శూన్య
image_placement_unlink(image_placement_T *ఉంచు)
{
    ఒకవేళ (ఉంచు->prev != ఖాళీ)
	ఉంచు->prev->తదుపరి = ఉంచు->తదుపరి;
    ఒకవేళ (ఉంచు->తదుపరి != ఖాళీ)
	ఉంచు->తదుపరి->prev = ఉంచు->prev;
    ఒకవేళ (placements == ఉంచు)
	placements = ఉంచు->తదుపరి;
    n_placements--;
}

    స్థిర శూన్య
image_placement_link(image_placement_T *ఉంచు)
{
    image_placement_T *p = placements;
    image_placement_T *prev = ఖాళీ;

    // Add placement *before* the placement with the lower zindex.
    అయితే (p != ఖాళీ)
    {
	ఒకవేళ (ఉంచు->zindex > p->zindex)
	    ఆపు;
	లేకపోతే ఒకవేళ (ఉంచు->zindex == p->zindex)
	{
	    ఒకవేళ (ఉంచు->use > p->use)
		ఆపు;
	    ఒకవేళ (ఉంచు->gen < p->gen)
		ఆపు;
	}

	prev = p;
	p = p->తదుపరి;
    }

    ఉంచు->తదుపరి = p;
    ఉంచు->prev = prev;

    ఒకవేళ (p != ఖాళీ)
	p->prev = ఉంచు;

    ఒకవేళ (prev != ఖాళీ)
	prev->తదుపరి = ఉంచు;
    లేకపోతే
	placements = ఉంచు;

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
    image_placement_T	*ఉంచు;
    స్థిర int_u	id = 1; // Kitty placements id must be > 1

    ఒకవేళ (!backend_అందుబాటు(!quiet))
	తిరిగి ఖాళీ;

    ఉంచు = ALLOC_CLEAR_ONE(image_placement_T);
    ఒకవేళ (ఉంచు == ఖాళీ)
	తిరిగి ఖాళీ;

    ఉంచు->id = id;
    id += PLACEMENT_ID_INC; // Allocate 1000 free placement ids to be used to
			    // draw this image placement. Only relevant for
			    // kitty graphics protocol.
    ఉంచు->gen = gen;
    ఉంచు->use = use;

    ఉంచు->img = img;
    ఉంచు->flags = IMAGEPF_DIRTY;

    ఒకవేళ (img != ఖాళీ)
    {
	ఉంచు->img_ver = img->ver;
	ఉంచు->backend = image_backend;
	ఉంచు->cell_వెడల్పు = cell_వెడల్పు;
	ఉంచు->cell_ఎత్తు = cell_ఎత్తు;

	// Bounding box is in cells
	image_get_cell_dimensions(img,
		&ఉంచు->bounding_box.x2, &ఉంచు->bounding_box.y2);
	ఉంచు->crop_box.x2 = ఉంచు->bounding_box.x2;
	ఉంచు->crop_box.y2 = ఉంచు->bounding_box.y2;

	ఒకవేళ (PLACEMENT_FUNC(image_backend, init)(ఉంచు) == FAIL)
	{
	    id -= PLACEMENT_ID_INC;
	    vim_free(ఉంచు);
	    తిరిగి ఖాళీ;
	}
    }
    లేకపోతే
	ఉంచు->backend = IMAGE_BACKEND_NONE;

    image_placement_link(ఉంచు);

    తిరిగి ఉంచు;
}

    స్థిర శూన్య
image_placement_clear_int(image_placement_T *ఉంచు, bool now)
{
    ఒకవేళ (backend_అందుబాటు(false) && ఉంచు->img != ఖాళీ)
    {
	ఒకవేళ (ఉంచు->flags & IMAGEPF_VISIBLE_INIT
		&& PLACEMENT_FUNC(image_backend, blit))
	    redraw_region(&ఉంచు->visible_abs, now, true);
	PLACEMENT_FUNC(image_backend, clear)(ఉంచు);
	ఉంచు->flags |= IMAGEPF_HIDDEN;
	ఒకవేళ (!now && !updating_screen)
	    redraw_all_later(UPD_VALID);
    }
}

    శూన్య
image_placement_clear(image_placement_T *ఉంచు)
{
    image_placement_clear_int(ఉంచు, updating_screen);
}

    శూన్య
image_placement_free(image_placement_T *ఉంచు)
{
    ఒకవేళ (backend_అందుబాటు(false) && ఉంచు->backend_దత్తాంశం != ఖాళీ)
    {
	image_placement_clear(ఉంచు);
	PLACEMENT_FUNC(image_backend, uninit)(ఉంచు);
    }

    ఒకవేళ (ఉంచు->flags & IMAGEPF_VISIBLE_INIT)
    {
	pixman_region32_fini(&ఉంచు->visible);
	pixman_region32_fini(&ఉంచు->visible_abs);
    }
    image_placement_unlink(ఉంచు);

    ఒకవేళ (ఉంచు->img != ఖాళీ)
	image_unref(ఉంచు->img);
    vim_free(ఉంచు);
}

    శూన్య
image_placement_సమితి_zindex(image_placement_T *ఉంచు, పూర్ణాంకం zindex)
{
    // If image was hidden before, must dirty it so it is fully redrawn again.
    ఒకవేళ (ఉంచు->flags & IMAGEPF_HIDDEN)
	ఉంచు->flags |= IMAGEPF_DIRTY;
    ఉంచు->flags &= ~IMAGEPF_HIDDEN;

    ఒకవేళ (ఉంచు->zindex == zindex)
	తిరిగి;
    ఉంచు->zindex = zindex;
    ఉంచు->flags |= IMAGEPF_DIRTY;

    // Must re-add the placement back so it is in the correct order
    image_placement_unlink(ఉంచు);
    image_placement_link(ఉంచు);
}

/*
 * Make sure to also update the bounding box!
 */
    శూన్య
image_placement_సమితి_position(image_placement_T *ఉంచు, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    ఒకవేళ (ఉంచు->flags & IMAGEPF_HIDDEN)
	ఉంచు->flags |= IMAGEPF_DIRTY;
    ఉంచు->flags &= ~IMAGEPF_HIDDEN;

    ఒకవేళ (ఉంచు->అడ్డువరుస == అడ్డువరుస && ఉంచు->నిలువు == నిలువు)
	తిరిగి;
    ఉంచు->అడ్డువరుస = అడ్డువరుస;
    ఉంచు->నిలువు = నిలువు;
    ఉంచు->flags |= IMAGEPF_DIRTY;
}

    శూన్య
image_placement_సమితి_crop(image_placement_T *ఉంచు, పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h)
{
    ఒకవేళ (ఉంచు->flags & IMAGEPF_HIDDEN)
	ఉంచు->flags |= IMAGEPF_DIRTY;
    ఉంచు->flags &= ~IMAGEPF_HIDDEN;

    ఒకవేళ (ఉంచు->crop_box.x1 == x && ఉంచు->crop_box.y1 == y
	    && ఉంచు->crop_box.x2 == x + w && ఉంచు->crop_box.y2 == y + h)
	తిరిగి;

    ఉంచు->crop_box.x1 = x;
    ఉంచు->crop_box.y1 = y;

    ఉంచు->crop_box.x2 = x + w;
    ఉంచు->crop_box.y2 = y + h;
    ఉంచు->flags |= IMAGEPF_DIRTY;
}

    శూన్య
image_placement_సమితి_bounding_box(
	image_placement_T   *ఉంచు,
	పూర్ణాంకం		    అడ్డువరుస,
	పూర్ణాంకం		    నిలువు,
	పూర్ణాంకం		    అడ్డువరుస_ఎత్తు,
	పూర్ణాంకం		    నిలువు_వెడల్పు)
{
    ఒకవేళ (ఉంచు->flags & IMAGEPF_HIDDEN)
	ఉంచు->flags |= IMAGEPF_DIRTY;
    ఉంచు->flags &= ~IMAGEPF_HIDDEN;

    ఒకవేళ (ఉంచు->bounding_box.x1 == నిలువు && ఉంచు->bounding_box.y1 == అడ్డువరుస
	    && ఉంచు->bounding_box.x2 == నిలువు + నిలువు_వెడల్పు
	    && ఉంచు->bounding_box.y2 == అడ్డువరుస + అడ్డువరుస_ఎత్తు)
	తిరిగి;

    ఉంచు->bounding_box.x1 = నిలువు;
    ఉంచు->bounding_box.y1 = అడ్డువరుస;

    ఉంచు->bounding_box.x2 = నిలువు + నిలువు_వెడల్పు;
    ఉంచు->bounding_box.y2 = అడ్డువరుస + అడ్డువరుస_ఎత్తు;
    ఉంచు->flags |= IMAGEPF_DIRTY;
}

/*
 * Enable or disable the image placement. Unlike IMAGEPF_HIDDEN, this is not
 * reset across redraws.
 */
    శూన్య
image_placement_సమితి_enable(image_placement_T *ఉంచు, bool enable)
{
    bool old = ఉంచు->flags & IMAGEPF_ENABLED;

    ఒకవేళ (enable && !old)
	ఉంచు->flags |= IMAGEPF_ENABLED;
    లేకపోతే ఒకవేళ (!enable && old)
    {
	ఉంచు->flags &= ~IMAGEPF_ENABLED;
	image_placement_clear(ఉంచు);
    }
    లేకపోతే
	తిరిగి;

    ఉంచు->flags |= IMAGEPF_DIRTY;
}

/*
 * Note that the returned crop region (x, y, w, h) is in physical pixels if
 * "physical" is true. Not relevant when using terminal protocols (sixel, KGP).
 */
    శూన్య
image_placement_subrect(
	image_placement_T   *ఉంచు,
	pixman_box32_t	    rect,
	పూర్ణాంకం		    *అడ్డువరుస,
	పూర్ణాంకం		    *నిలువు,
	పూర్ణాంకం		    *x,
	పూర్ణాంకం		    *y,
	పూర్ణాంకం		    *w,
	పూర్ణాంకం		    *h,
	bool		    physical)
{
    పూర్ణాంకం iw, ih;

    image_get_dimensions(ఉంచు->img, &iw, &ih);

    *అడ్డువరుస = ఉంచు->అడ్డువరుస + rect.y1 + ఉంచు->అడ్డువరుస_off;
    *నిలువు = ఉంచు->నిలువు + rect.x1;

    *x = (rect.x1 + ఉంచు->crop_box.x1) * cell_వెడల్పు;
    *y = (rect.y1 + ఉంచు->crop_box.y1) * cell_ఎత్తు;
    *w = (rect.x2 - rect.x1) * cell_వెడల్పు;
    *h = (rect.y2 - rect.y1) * cell_ఎత్తు;

    ఒకవేళ (physical)
    {
	*x = LOG2PHY(*x);
	*y = LOG2PHY(*y);
	*w = LOG2PHY(*w);
	*h = LOG2PHY(*h);
    }
    లేకపోతే
    {
	iw = PHY2LOG(iw);
	ih = PHY2LOG(ih);
    }

    // Make that all the values are valid. "w" and "h" can especially be
    // invalid, because the image dimensions (in pixels) are converted to cells,
    // rounding *up*.
    *అడ్డువరుస = MIN(*అడ్డువరుస, అడ్డువరుసలు);
    *నిలువు = MIN(*నిలువు, Columns);

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
    స్థిర శూన్య
redraw_region(pixman_region32_t *region, bool now, bool restore)
{
    pixman_box32_t  *rects;
    పూర్ణాంకం		    n_rects;
    పూర్ణాంకం		    cur_అడ్డువరుస = screen_cur_అడ్డువరుస;
    పూర్ణాంకం		    cur_నిలువు = screen_cur_నిలువు;

    rects = pixman_region32_rectangles(region, &n_rects);
    ఒకవేళ (rects == ఖాళీ)
	తిరిగి;

    కొరకు (పూర్ణాంకం i = 0; i < n_rects; i++)
    {
	pixman_box32_t rect = rects[i];

	ఒకవేళ (now)
	{
	    // The region may be from before a resize, when the screen was
	    // bigger.
	    పూర్ణాంకం ఎత్తు = MIN(rect.y2, screen_అడ్డువరుసలు) - rect.y1;
	    పూర్ణాంకం వెడల్పు = MIN(rect.x2, screen_Columns) - rect.x1;

	    // Tip: if you want to debug stale pixels appearing, set "inverse"
	    // of screen_draw_rectangle() to TRUE.
	    ఒకవేళ (ఎత్తు > 0 && వెడల్పు > 0)
		screen_draw_rectangle(rect.y1, rect.x1, ఎత్తు, వెడల్పు,
			FALSE, TRUE);
	    కొనసాగు;
	}

	కొరకు (పూర్ణాంకం r = rect.y1; r < rect.y2; r++)
	{
	    ఒకవేళ (r >= screen_అడ్డువరుసలు)
		ఆపు;

	    కొరకు (పూర్ణాంకం c = rect.x1; c < rect.x2; c++)
	    {
		ఒకవేళ (c >= screen_Columns)
		    ఆపు;

		ScreenAttrs[పంక్తి_స్థానభ్రంశం[r] + c] = (sattr_T)-1;
	    }
	}
    }

    ఒకవేళ (restore)
	windgoto(cur_అడ్డువరుస, cur_నిలువు);
}

/*
 * Draw all image placements to the screen. This should be done after all text
 * have been drawn to the screen, and image placements positioned correctly.
 */
    శూన్య
draw_image_placements(శూన్య)
{
    image_placement_T	*ఉంచు;
    pixman_region32_t	వ్యవకలనం_region;    // In cells
    pixman_region32_t	dirty_region;	    // In cells
    image_placement_T	**pending_placements;
    పూర్ణాంకం			pending_len = -1;
    // Save current cursor position
    పూర్ణాంకం			cur_అడ్డువరుస = screen_cur_అడ్డువరుస;
    పూర్ణాంకం			cur_నిలువు = screen_cur_నిలువు;

    // Check if all placements have no backing image. If so, then do nothing
    FOR_ALL_PLACEMENTS(ఉంచు)
	ఒకవేళ (ఉంచు->img != ఖాళీ)
	{
	    pending_len = 0;
	    ఆపు;
	}

    ఒకవేళ (pending_len == -1)
	తిరిగి;

    ఒకవేళ (!backend_అందుబాటు(false))
	తిరిగి;

    ఒకవేళ (n_placements == 0)
	తిరిగి;

    pending_placements = ALLOC_CLEAR_MULT(image_placement_T *, n_placements);
    ఒకవేళ (pending_placements == ఖాళీ)
	తిరిగి;

    pixman_region32_init(&వ్యవకలనం_region);
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
    FOR_ALL_PLACEMENTS(ఉంచు)
    {
	image_T *img = ఉంచు->img;

	pixman_region32_t   image_region;
	pixman_region32_t   visible_region;

	పూర్ణాంకం x, y;

	ఒకవేళ (ఉంచు->flags & IMAGEPF_HIDDEN || !(ఉంచు->flags & IMAGEPF_ENABLED))
	    // Don't need to clear image placement for non-blit backends,
	    // because we call image_placement_clear() every time we don't want
	    // to see the image. Not sure of the side effects if we also clear
	    // it here...
	    కొనసాగు;

	ఒకవేళ (img != ఖాళీ)
	{
	    pixman_region32_t	visible_abs;
	    పూర్ణాంకం			crop_w, crop_h;
	    bool		cs_మారింది;
	    bool		need_redraw;

	    // Processing changed image data failed, skip this image
	    ఒకవేళ (ఉంచు->img->దత్తాంశం == ఖాళీ)
	    {
		image_placement_clear(ఉంచు);
		కొనసాగు;
	    }

	    // Only apply row shift if image is within the shifted region.
	    ఒకవేళ ((ఉంచు->bounding_box.y1 >= జరుపు_శిఖరం
			&& ఉంచు->bounding_box.y2 <= జరుపు_bot)
		    && ఉంచు->అడ్డువరుస_off != జరుపు)
	    {
		ఉంచు->అడ్డువరుస_off = జరుపు;
		ఉంచు->flags |= IMAGEPF_DIRTY;
	    }

	    x = ఉంచు->నిలువు;
	    y = ఉంచు->అడ్డువరుస + ఉంచు->అడ్డువరుస_off;

	    crop_w = ఉంచు->crop_box.x2 - ఉంచు->crop_box.x1;
	    crop_h = ఉంచు->crop_box.y2 - ఉంచు->crop_box.y1;

	    // Don't add "crop_box" x1 and y1, because "row" and "col" use the
	    // top left of the final cropped image.
	    pixman_region32_init_rect(&image_region, x, y, crop_w, crop_h);

	    pixman_region32_init(&visible_region);
	    pixman_region32_init(&visible_abs);

	    // Only use parts of the image region within the actual screen.
	    // "row_off" may offset the image slightly/completely off screen.
	    ఒకవేళ (!pixman_region32_intersect_rect(&image_region, &image_region,
			0, 0, Columns, అడ్డువరుసలు)
		    || !pixman_region32_వ్యవకలనం(&visible_region,
			&image_region, &వ్యవకలనం_region)
		    || !pixman_region32_copy(&visible_abs, &visible_region))
	    {
		pixman_region32_fini(&image_region);
		pixman_region32_fini(&visible_region);
		కొనసాగు;
	    }

	    pixman_region32_fini(&image_region);

	    // The visible region is in absolute coordinates, must convert it
	    // into image relative coordinates.
	    pixman_region32_translate(&visible_region, -x, -y);

	    cs_మారింది = ఉంచు->cell_వెడల్పు != cell_వెడల్పు
		|| ఉంచు->cell_ఎత్తు != cell_ఎత్తు;

	    // If cell size changed, then the visible regions are invalid pixel
	    // wise. This prevents backends such as sixel from using cached
	    // sequences.
	    ఒకవేళ (cs_మారింది)
	    {
		ఉంచు->cell_వెడల్పు = cell_వెడల్పు;
		ఉంచు->cell_ఎత్తు = cell_ఎత్తు;
		ఉంచు->flags |= IMAGEPF_FORCE;
	    }

	    // Only redraw the image if it has changed (or if we haven't drawn
	    // it yet). Or if image data has changed or cell size has changed
	    need_redraw = ఉంచు->flags & IMAGEPF_DIRTY || cs_మారింది
		||  (ఉంచు->flags & IMAGEPF_VISIBLE_INIT
			&& !pixman_region32_equal(&visible_region,
			    &ఉంచు->visible));

	    ఒకవేళ (ఉంచు->img_ver != img->ver)
	    {
		// Image data changed
		need_redraw = true;
		ఉంచు->img_ver = img->ver;

		image_placement_clear(ఉంచు);
		PLACEMENT_FUNC(image_backend, uninit)(ఉంచు);
		ఒకవేళ (PLACEMENT_FUNC(image_backend, init)(ఉంచు) == FAIL)
		{
		    emsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం));
		    image_unref(ఉంచు->img);
		    ఉంచు->img = ఖాళీ;
		    కొనసాగు;
		}
	    }

	    ఒకవేళ (need_redraw)
	    {
		ఒకవేళ (ఉంచు->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&ఉంచు->visible);
		ఉంచు->visible = visible_region;

		ఒకవేళ (PLACEMENT_FUNC(image_backend, blit))
		{
		    // Must redraw the stale regions that will not be composited
		    // over (for this specific image).
		    pixman_region32_t stale_region;

		    pixman_region32_init(&stale_region);

		    ఒకవేళ (ఉంచు->flags & IMAGEPF_VISIBLE_INIT)
		    {
			(శూన్య)pixman_region32_వ్యవకలనం(&stale_region,
				&ఉంచు->visible_abs, &visible_abs);

			// Since pixels may be translucent/transparent, we must
			// redraw the cells behind the image every time, so that
			// no stale pixels remain from the previous redraw.
			ఒకవేళ (img->fmt == IMAGE_FORMAT_RGBA)
			    (శూన్య)pixman_region32_union(&stale_region,
				    &stale_region, &ఉంచు->visible_abs);
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
		    ఒకవేళ (img->fmt != IMAGE_FORMAT_RGBA)
		    {
			పూర్ణాంకం		    min_w, min_h;
			పూర్ణాంకం		    rect_w, rect_h;
			pixman_region32_t   min_region;

			image_get_cell_dimensions_min(img, &min_w, &min_h);

			// Ensure "min_region" only covers the fully covered
			// cells of the cropped region.
			rect_w = min_w - ఉంచు->crop_box.x1;
			rect_h = min_h - ఉంచు->crop_box.y1;

			ఒకవేళ (rect_w > 0 && rect_h > 0)
			    // pixman_region32_init_rect() expects a non zero
			    // width and height (otherwise it outputs error
			    // message).
			    pixman_region32_init_rect(&min_region,
				    x, y, rect_w, rect_h);
			లేకపోతే
			    pixman_region32_init(&min_region);

			(శూన్య)pixman_region32_వ్యవకలనం(&min_region,
				&visible_abs, &min_region);
			(శూన్య)pixman_region32_union(&stale_region,
				&stale_region, &min_region);
			pixman_region32_fini(&min_region);
		    }

		    ఒకవేళ (ఉంచు->flags & IMAGEPF_VISIBLE_INIT)
			// Also subtract "subtract_region", so we don't
			// redundantly redraw cells that will have images
			// painted over them after. Make sure to do this before
			// we check partially covered cells, since
			// "subtract_region" may also have partially covered
			// cells.
			(శూన్య)pixman_region32_వ్యవకలనం(&stale_region,
				&stale_region, &వ్యవకలనం_region);

		    // Don't redraw now, defer it later
		    pixman_region32_union(&dirty_region,
			    &dirty_region, &stale_region);
		    pixman_region32_fini(&stale_region);
		}
		ఒకవేళ (ఉంచు->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&ఉంచు->visible_abs);
		ఉంచు->visible_abs = visible_abs;

		pending_placements[pending_len++] = ఉంచు;
		ఉంచు->flags |= IMAGEPF_VISIBLE_INIT;
		ఉంచు->flags &= ~IMAGEPF_DIRTY;
	    }
	    లేకపోతే
	    {
		pixman_region32_fini(&visible_region);
		pixman_region32_fini(&visible_abs);
	    }
	}

	// If this is last image, no need to union its bounds.
	ఒకవేళ (ఉంచు->తదుపరి == ఖాళీ)
	    ఆపు;

	pixman_region32_union_rect(&వ్యవకలనం_region, &వ్యవకలనం_region,
		ఉంచు->bounding_box.x1,
		ఉంచు->bounding_box.y1 + ఉంచు->అడ్డువరుస_off,
		ఉంచు->bounding_box.x2 - ఉంచు->bounding_box.x1,
		ఉంచు->bounding_box.y2 - ఉంచు->bounding_box.y1);
    }

    // Don't want to dirty images again redundantly
    freeze_mark_dirty++;
    redraw_region(&dirty_region, true, false);
    freeze_mark_dirty--;

    ఒకవేళ (pending_len > 0)
    {
#నిర్వచితమైతే FEAT_GUI
	// Make sure to flush any output! We write directly to the draw
	// area/whatever, meaning draw commands outputted before this (as escape
	// sequences), will still draw over the image surface if we don't flush
	// now.
	ఒకవేళ (gui.లోపల_use)
	    బహిర్గతం_flush();
#అయితే_ముగింపు

	కొరకు (పూర్ణాంకం i = 0; i < pending_len; i++)
	{
	    PLACEMENT_FUNC(image_backend, draw)(pending_placements[i]);
	    pending_placements[i]->flags &= ~IMAGEPF_FORCE;
	}
    }

    // Restore cursor position, however do not turn on the cursor, since that
    // causes it to flicker at the wrong position when using sixel. It will be
    // turned on later somewhere...
    windgoto(cur_అడ్డువరుస, cur_నిలువు);
    బహిర్గతం_flush();

    vim_free(pending_placements);
    pixman_region32_fini(&వ్యవకలనం_region);
    pixman_region32_fini(&dirty_region);
}

/*
 * Mark any images that touches the region dirty. Only used for backends that
 * blit pixels.
 */
    శూన్య
mark_dirty_region_for_images(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం అడ్డువరుస_ఎత్తు, పూర్ణాంకం నిలువు_వెడల్పు)
{
    image_placement_T	*ఉంచు;
    pixman_box32_t	rect;

    ఒకవేళ (freeze_mark_dirty > 0 || !backend_అందుబాటు(false)
	    || !PLACEMENT_FUNC(image_backend, blit) || n_placements == 0)
	తిరిగి;

    rect.x1 = నిలువు;
    rect.y1 = అడ్డువరుస;
    rect.x2 = నిలువు + నిలువు_వెడల్పు;
    rect.y2 = అడ్డువరుస + అడ్డువరుస_ఎత్తు;

    FOR_ALL_PLACEMENTS(ఉంచు)
    {
	ఒకవేళ (ఉంచు->flags & IMAGEPF_HIDDEN)
	    కొనసాగు;
	ఒకవేళ (pixman_region32_కలిగియుంది_rectangle(&ఉంచు->visible_abs, &rect)
		== PIXMAN_REGION_బహిర్గతం)
	    కొనసాగు;

	ఉంచు->flags |= IMAGEPF_DIRTY;
    }
}

    శూన్య
clear_all_image_placements(శూన్య)
{
    image_placement_T *ఉంచు;

    FOR_ALL_PLACEMENTS(ఉంచు)
	image_placement_clear(ఉంచు);
    ఒకవేళ (!updating_screen)
	redraw_all_later(UPD_VALID);
}

/*
 * Used for screen_ins_lines() and screen_del_lines(), because they directly
 * shift rows. Note that this does not queue a screen redraw.
 */
    శూన్య
జరుపు_image_placements(పూర్ణాంకం శిఖరం, పూర్ణాంకం bot, పూర్ణాంకం amount)
{
    జరుపు_శిఖరం = శిఖరం;
    జరుపు_bot = bot;
    జరుపు += amount;
}

/*
 * Called when screen is cleared, as update_screen() may go to clearing the
 * screen instead of inserting lines using screen_ins_lines().
 */
    శూన్య
జరుపు_reset_image_placements(శూన్య)
{
    జరుపు = 0;
}

    స్థిర image_T *
కనుగొను_image(int_u id)
{
    image_T *img;

    FOR_ALL_IMAGES(img)
	ఒకవేళ (img->state == IMAGE_STATE_ప్రజా && img->id == id)
	    తిరిగి img;
    తిరిగి ఖాళీ;
}

/*
 * Add an image using the given information in "dict". If "existing" is not
 * NULL, try updating it instead. If "find" is TRUE, then accept the "id" field.
 */
    image_T *
కూడు_image(dict_T *dict, image_T *existing, bool కనుగొను)
{
    dictitem_T	    *di;
    blob_T	    *దత్తాంశం;
    varnumber_T     w, h;
    varnumber_T     n_pixels;
    image_format_T  fmt;

    ఒకవేళ (కనుగొను && dict_has_కీలకం(dict, "id"))
    {
	image_T *img = కనుగొను_image((int_u)dict_get_సంఖ్య(dict, "id"));

	ఒకవేళ (img == ఖాళీ)
	    తిరిగి ఖాళీ;
	తిరిగి image_ref(img);
    }

    di = dict_కనుగొను(dict, (char_u *)"data", -1);
    w = dict_get_సంఖ్య(dict, "width");
    h = dict_get_సంఖ్య(dict, "height");

    ఒకవేళ (di == ఖాళీ || di->di_tv.v_type != VAR_BLOB || w <= 0 || h <= 0)
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి ఖాళీ;
    }

    // Check for overflow
    n_pixels = w * h;
    ఒకవేళ (w > గరిష్ఠ_పూర్ణాంకం || h > గరిష్ఠ_పూర్ణాంకం || n_pixels > గరిష్ఠ_పూర్ణాంకం)
    {
	emsg(_(e_invalid_image_dimensions));
	తిరిగి ఖాళీ;
    }

    దత్తాంశం = di->di_tv.vval.v_blob;

    ఒకవేళ (blob_len(దత్తాంశం) == n_pixels * 3)
	fmt = IMAGE_FORMAT_RGB;
    లేకపోతే ఒకవేళ (blob_len(దత్తాంశం) == n_pixels * 4)
	fmt = IMAGE_FORMAT_RGBA;
    లేకపోతే
    {
	semsg(_(e_invalid_విలువ_for_వాదం_str_str), "data",
		"data length must equal width*height*3 or width*height*4");
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (existing != ఖాళీ)
    {
	ఒకవేళ (existing->వెడల్పు == w && existing->ఎత్తు == h
		&& existing->fmt == fmt)
	{
	    ఒకవేళ (image_update(existing, దత్తాంశం->bv_ga.ga_దత్తాంశం) == FAIL)
		తిరిగి ఖాళీ;
	    తిరిగి existing;
	}
    }

    తిరిగి image_new(దత్తాంశం->bv_ga.ga_దత్తాంశం, w, h, fmt);
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
    పూర్ణాంకం		w, h;
    dictitem_T	*di;

    ఒకవేళ (img->దత్తాంశం == ఖాళీ)
	తిరిగి ఖాళీ;

    dict = dict_alloc();
    ఒకవేళ (dict == ఖాళీ)
	తిరిగి ఖాళీ;

    blob = blob_alloc();
    ఒకవేళ (blob == ఖాళీ)
    {
	dict_unref(dict);
	తిరిగి ఖాళీ;
    }

    image_get_dimensions(img, &w, &h);
    (శూన్య)ga_concat_బైట్లు(&blob->bv_ga, (అక్షరం *)img->దత్తాంశం,
	    (size_t)w * h * img->fmt);

    di = dictitem_alloc((char_u *)"data");

    ఒకవేళ (di != ఖాళీ)
    {
	di->di_tv.v_type = VAR_BLOB;
	di->di_tv.vval.v_blob = blob;
    }
    ఒకవేళ (di == ఖాళీ || dict_కూడు(dict, di) == FAIL)
    {
	ఒకవేళ (di != ఖాళీ)
	    dictitem_free(di);
	dict_unref(dict);
	// No need to unref blob, dictitem_free() will do that.
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (img->state == IMAGE_STATE_ప్రజా)
	dict_కూడు_సంఖ్య(dict, "id", img->id);
    dict_కూడు_సంఖ్య(dict, "width", img->వెడల్పు);
    dict_కూడు_సంఖ్య(dict, "height", img->ఎత్తు);
    dict_కూడు_సంఖ్య(dict, "alpha", img->fmt == IMAGE_FORMAT_RGBA);
    dict_కూడు_string(dict, "format",
	    (char_u *)(img->fmt == IMAGE_FORMAT_RGB ? "rgb" : "rgba"));

    blob->bv_refcount = 1;

    తిరిగి dict;
}

    స్థిర పూర్ణాంకం
సరిపోలిక_imageprotocol(image_backend_T *backend)
{
    పూర్ణాంకం		    len = (పూర్ణాంకం)STRLEN(p_ipc) + 1;
    char_u	    *buf = alloc(len);
    char_u	    *p = p_ipc;
    పూర్ణాంకం		    ret = FAIL;
    image_backend_T res = IMAGE_BACKEND_NONE;
    bool	    got = false;

    ఒకవేళ (buf == ఖాళీ)
	తిరిగి FAIL;

    అయితే (*p != NUL)
    {
	char_u		*colon;
	regmatch_T	regmatch;
	image_backend_T prot = IMAGE_BACKEND_NONE;

	// Isolate one comma separated item.
	(శూన్య)copy_option_part(&p, buf, len, ",");

	colon = vim_strchr(buf, ':');
	ఒకవేళ (colon == ఖాళీ || colon == buf || colon[1] == NUL)
	    వెళ్ళు నిష్క్రమించు;

	*colon = NUL;

	// Note: Keep this in sync with p_ipc_protocol_values.
	ఒకవేళ (STRCMP(colon + 1, "none") == 0)
	    prot = IMAGE_BACKEND_NONE;
	లేకపోతే ఒకవేళ (STRCMP(colon + 1, "kitty") == 0)
	{
#నిర్వచితమైతే FEAT_IMAGE_KITTY
	    prot = IMAGE_BACKEND_KITTY;
#అయితే_ముగింపు
	}
	లేకపోతే ఒకవేళ (STRCMP(colon + 1, "sixel") == 0)
	{
#నిర్వచితమైతే FEAT_IMAGE_SIXEL
	    prot = IMAGE_BACKEND_SIXEL;
#అయితే_ముగింపు
	}
	లేకపోతే
	    వెళ్ళు నిష్క్రమించు;

	ఒకవేళ (got)
	    కొనసాగు;

	CLEAR_క్షేత్రం(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);

	ఒకవేళ (regmatch.regprog == ఖాళీ)
	    వెళ్ళు నిష్క్రమించు;

	bool సరిపోలిక = T_NAME != ఖాళీ
	    && vim_regexec(&regmatch, T_NAME, (colnr_T)0);

	vim_regfree(regmatch.regprog);
	ఒకవేళ (సరిపోలిక)
	{
	    // Keep going to catch any errors
	    res = prot;
	    got = true;
	}
    }

    ret = OK;
    *backend = res;
నిష్క్రమించు:
    vim_free(buf);
    తిరిగి ret;
}

/*
 * Update the current image backend to use depending on 'imageprotocol' and if
 * GUI is being used. Returns OK on success and FAIL on failure.
 */
    పూర్ణాంకం
update_image_backend(శూన్య)
{
    image_backend_T new;

    image_T		*img;
    image_placement_T	*ఉంచు;

    // Always process 'imageprotocol' option to check for errors
    ఒకవేళ (సరిపోలిక_imageprotocol(&new) == FAIL)
	తిరిగి FAIL;

#నిర్వచితమైతే FEAT_IMAGE_GUI
    ఒకవేళ (gui.లోపల_use)
	new = IMAGE_BACKEND_GUI;
#అయితే_ముగింపు

    ఒకవేళ (image_backend == new)
	తిరిగి OK;

    // Must uninit the backends of every image/placement, then init the new
    // backend for each.
    FOR_ALL_IMAGES(img)
    {
	ఒకవేళ (image_backend != IMAGE_BACKEND_NONE && img->దత్తాంశం != ఖాళీ
		&& img->backend_దత్తాంశం != ఖాళీ)
	    IMAGE_FUNC(image_backend, uninit)(img);
	img->backend_దత్తాంశం = ఖాళీ;
	ఒకవేళ (new != IMAGE_BACKEND_NONE)
	{
	    ఒకవేళ (IMAGE_FUNC(new, init)(img) == FAIL)
		వెళ్ళు fail;
	    img->backend = new;
	}
    }
    FOR_ALL_PLACEMENTS(ఉంచు)
    {
	image_placement_clear(ఉంచు);
	ఒకవేళ (image_backend != IMAGE_BACKEND_NONE
		&& ఉంచు->img != ఖాళీ && ఉంచు->backend_దత్తాంశం != ఖాళీ)
	    PLACEMENT_FUNC(image_backend, uninit)(ఉంచు);
	ఉంచు->backend_దత్తాంశం = ఖాళీ;
	ఒకవేళ (new != IMAGE_BACKEND_NONE && ఉంచు->img != ఖాళీ)
	{
	    ఒకవేళ (PLACEMENT_FUNC(new, init)(ఉంచు) == FAIL)
		వెళ్ళు fail;
	    ఉంచు->backend = new;
	}
    }

    redraw_all_later(UPD_VALID);
    image_backend = new;

    ఒకవేళ (new == IMAGE_BACKEND_NONE)
	సమితి_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    లేకపోతే
	సమితి_vim_var_string(VV_IMAGEBACKEND,
		(char_u *)image_backends[image_backend].name, -1);

    తిరిగి OK;
fail:
    FOR_ALL_IMAGES(img)
	ఒకవేళ (img->backend != IMAGE_BACKEND_NONE)
	{
	    IMAGE_FUNC(img->backend, uninit)(img);
	    img->backend = IMAGE_BACKEND_NONE;
	}

    FOR_ALL_PLACEMENTS(ఉంచు)
    {
	ఒకవేళ (ఉంచు->img != ఖాళీ && ఉంచు->backend != IMAGE_BACKEND_NONE)
	{
	    image_placement_clear(ఉంచు);
	    PLACEMENT_FUNC(ఉంచు->backend, uninit)(ఉంచు);
	}
	ఉంచు->backend = IMAGE_BACKEND_NONE;
    }

    redraw_all_later(UPD_VALID);

    image_backend = IMAGE_BACKEND_NONE;
    సమితి_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    emsg(_(e_changing_image_backend_failed));
    తిరిగి FAIL;
}

    శూన్య
f_image_కూడు(typval_T *argvars, typval_T *rettv)
{
    dict_T  *dict;
    image_T *img;

    ఒకవేళ (check_for_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    dict = argvars[0].vval.v_dict;

    img = కూడు_image(dict, ఖాళీ, false);
    ఒకవేళ (img != ఖాళీ)
	img->state = IMAGE_STATE_ప్రజా;

    rettv->v_type = VAR_సంఖ్య;
    rettv->vval.v_సంఖ్య = img == ఖాళీ ? (varnumber_T)-1 : img->id;
}

    శూన్య
f_image_discard(typval_T *argvars, typval_T *rettv వాడని)
{
    image_T *img;

    ఒకవేళ (check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    img = కనుగొను_image((int_u)argvars[0].vval.v_సంఖ్య);

    ఒకవేళ (img != ఖాళీ)
    {
	img->state = IMAGE_STATE_వ్యక్తిగత;
	image_unref(img);
    }
    లేకపోతే
	semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_సంఖ్య);
}

    శూన్య
f_image_info(typval_T *argvars, typval_T *rettv)
{
    image_T *img;
    dict_T  *dict;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL
	    || check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_సంఖ్య)
    {
	img = కనుగొను_image((int_u)argvars[0].vval.v_సంఖ్య);
	ఒకవేళ (img == ఖాళీ)
	{
	    semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_సంఖ్య);
	    తిరిగి;
	}
	dict = get_image_info(img);
	ఒకవేళ (dict != ఖాళీ && list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	తిరిగి;
    }

    FOR_ALL_IMAGES(img)
    {
	ఒకవేళ (img->state == IMAGE_STATE_వ్యక్తిగత)
	    కొనసాగు;
	dict = get_image_info(img);
	ఒకవేళ (dict == ఖాళీ
		|| list_append_dict(rettv->vval.v_list, dict) == FAIL)
	{
	    dict_unref(dict);	// NULL-safe
	    తిరిగి;
	}
    }
}

#అయితే_ముగింపు // FEAT_IMAGE
