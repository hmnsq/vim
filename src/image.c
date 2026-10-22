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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_IMAGE)

সংরচনা
{
    ধ্রুৱ ৱর্ণ *name;

    সংরচনা
    {
	// Return OK on success and FAIL on failure
	পূর্ণাঙ্ক (*init)(image_T *);
	শূন্য (*uninit)(image_T *);
    } image;

    সংরচনা
    {
	// Return OK on success and FAIL on failure
	পূর্ণাঙ্ক (*init)(image_placement_T *);
	শূন্য (*uninit)(image_placement_T *);

	শূন্য (*draw)(image_placement_T *);
	শূন্য (*clear)(image_placement_T *);

	// If backend blits the image pixels to the screen. Images are not
	// treated as "objects".
	bool blit;
    } placement;
} image_backends[] = {
#ifdef FEAT_IMAGE_GUI
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
#  অন্যথা
	    .blit = true,
#  endif
# elif FEAT_GUI_MSWIN
	    .blit = true,
# অন্যথা
	    .blit = false,
# endif
	}
    },
#endif
#ifdef FEAT_IMAGE_KITTY
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
#endif
#ifdef FEAT_IMAGE_SIXEL
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
#endif
};

// Current image backend being used
স্থির image_backend_T image_backend = IMAGE_BACKEND_NONE;

স্থির image_T *images = রিক্ত;
// Sorted from highest zindex to lowest zindex
স্থির image_placement_T    *placements = রিক্ত;
স্থির পূর্ণাঙ্ক		    n_placements = 0;

স্থির পূর্ণাঙ্ক shift_শীৰ্ষ = 0;
স্থির পূর্ণাঙ্ক shift_bot = 0;
স্থির পূর্ণাঙ্ক shift = 0;

// If > 0, then mark_dirty_region_for_images() will be a no-op
স্থির পূর্ণাঙ্ক freeze_mark_dirty = 0;

#define FOR_ALL_IMAGES(v) বাবে ((v) = images; (v) != রিক্ত; (v) = (v)->next)
#define FOR_ALL_PLACEMENTS(v) \
    বাবে ((v) = placements; (v) != রিক্ত; (v) = (v)->next)

#define IMAGE_FUNC(b, n) (image_backends[b].image.n)
#define PLACEMENT_FUNC(b, n) (image_backends[b].placement.n)

স্থির শূন্য redraw_region(pixman_region32_t *region, bool now, bool restore);

    শূন্য
init_image_state(শূন্য)
{
    (শূন্য)update_image_backend();
}

    শূন্য
uninit_image_state(শূন্য)
{
#ifdef FEAT_IMAGE_SIXEL
    sixel_uninit();
#endif
}

/*
 * Return true if the there image backend is ready, otherwise emit error.
 */
    স্থির bool
backend_available(bool বাৰ্তা)
{
    যদি (image_backend == IMAGE_BACKEND_NONE)
    {
	যদি (বাৰ্তা)
	    emsg(_(e_no_image_backend_available));
	ঘূৰাই false;
    }
    ঘূৰাই true;
}

/*
 * Create a new image using the given data (creates a copy). Returns NULL on
 * failure.
 */
    image_T *
image_new(uint8_t *data, পূর্ণাঙ্ক width, পূর্ণাঙ্ক height, image_format_T fmt)
{
    image_T	    *img;
    স্থির int_u    id;

    যদি (!backend_available(true))
	ঘূৰাই রিক্ত;

    img = ALLOC_CLEAR_ONE(image_T);
    যদি (img == রিক্ত)
	ঘূৰাই রিক্ত;

    img->data = vim_memsave(data, (size_t)width * height * fmt);
    যদি (img->data == রিক্ত)
    {
	vim_free(img);
	ঘূৰাই রিক্ত;
    }

    img->state = IMAGE_STATE_PRIVATE;
    img->width = width;
    img->height = height;
    img->fmt = fmt;

    // Mix in PID to prevent ID collisions when using kitty graphics protocol
    img->id = ((((int_u)mch_get_pid() & 0x7fff) + 1) << 16) | (id++ & 0xffff);
    img->refcount = 1;

    img->backend = image_backend;

    যদি (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	vim_free(img->data);
	vim_free(img);
	ঘূৰাই রিক্ত;
    }

    যদি (images != রিক্ত)
	images->prev = img;
    img->next = images;
    images = img;

    ঘূৰাই img;
}

    স্থির শূন্য
image_free(image_T *img)
{
    যদি (backend_available(false))
	IMAGE_FUNC(image_backend, uninit)(img);

    যদি (img->prev != রিক্ত)
	img->prev->next = img->next;
    যদি (img->next != রিক্ত)
	img->next->prev = img->prev;
    যদি (images == img)
	images = img->next;

    vim_free(img->data);
    vim_free(img);
}

/*
 * Decrement reference count of an image. Be careful to not let the public API
 * be able to directly decrement the refcount of an image multiple times!
 */
    শূন্য
image_unref(image_T *img)
{
    যদি (--img->refcount <= 0)
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
    ঘূৰাই img;
}

/*
 * Get image width and height in cells, rounding up so that every pixel is
 * covered.
 */
    শূন্য
image_get_cell_dimensions(image_T *img, পূর্ণাঙ্ক *cw, পূর্ণাঙ্ক *ch)
{
    পূর্ণাঙ্ক w = PHY2LOG(img->width);
    পূর্ণাঙ্ক h = PHY2LOG(img->height);

    *cw = (w + cell_width - 1) / cell_width;
    *ch = (h + cell_height - 1) / cell_height;
}

/*
 * Similar, but return the dimensions of all cells that are *fully* covered by
 * pixels. Pixels that are partially covered are not counted.
 */
    স্থির শূন্য
image_get_cell_dimensions_min(image_T *img, পূর্ণাঙ্ক *cw, পূর্ণাঙ্ক *ch)
{
    পূর্ণাঙ্ক w = PHY2LOG(img->width);
    পূর্ণাঙ্ক h = PHY2LOG(img->height);

    *cw = w / cell_width;
    *ch = h / cell_height;
}

    শূন্য
image_get_dimensions(image_T *img, পূর্ণাঙ্ক *w, পূর্ণাঙ্ক *h)
{
    *w = img->width;
    *h = img->height;
}

/*
 * Replace the current image contents with "data". "data" should be the exact
 * same format, width, and height. Return OK on success and FAIL on failure.
 */
    পূর্ণাঙ্ক
image_update(image_T *img, uint8_t *data)
{
    পূর্ণাঙ্ক w, h;

    যদি (!backend_available(true) || img->data == রিক্ত)
	ঘূৰাই FAIL;

    image_get_dimensions(img, &w, &h);
    mch_memmove(img->data, data, (size_t)w * h * img->fmt);

    IMAGE_FUNC(image_backend, uninit)(img);

    যদি (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	emsg(_(e_out_of_memory));
	VIM_CLEAR(img->data);
	ঘূৰাই FAIL;
    }

    img->ver++;
    redraw_all_later(UPD_VALID);
    ঘূৰাই OK;
}

    স্থির শূন্য
image_placement_unlink(image_placement_T *place)
{
    যদি (place->prev != রিক্ত)
	place->prev->next = place->next;
    যদি (place->next != রিক্ত)
	place->next->prev = place->prev;
    যদি (placements == place)
	placements = place->next;
    n_placements--;
}

    স্থির শূন্য
image_placement_link(image_placement_T *place)
{
    image_placement_T *p = placements;
    image_placement_T *prev = রিক্ত;

    // Add placement *before* the placement with the lower zindex.
    যেতিয়ালৈ (p != রিক্ত)
    {
	যদি (place->zindex > p->zindex)
	    ভঙ্গ;
	অন্যথা যদি (place->zindex == p->zindex)
	{
	    যদি (place->use > p->use)
		ভঙ্গ;
	    যদি (place->gen < p->gen)
		ভঙ্গ;
	}

	prev = p;
	p = p->next;
    }

    place->next = p;
    place->prev = prev;

    যদি (p != রিক্ত)
	p->prev = place;

    যদি (prev != রিক্ত)
	prev->next = place;
    অন্যথা
	placements = place;

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
    image_placement_T	*place;
    স্থির int_u	id = 1; // Kitty placements id must be > 1

    যদি (!backend_available(!quiet))
	ঘূৰাই রিক্ত;

    place = ALLOC_CLEAR_ONE(image_placement_T);
    যদি (place == রিক্ত)
	ঘূৰাই রিক্ত;

    place->id = id;
    id += PLACEMENT_ID_INC; // Allocate 1000 free placement ids to be used to
			    // draw this image placement. Only relevant for
			    // kitty graphics protocol.
    place->gen = gen;
    place->use = use;

    place->img = img;
    place->flags = IMAGEPF_DIRTY;

    যদি (img != রিক্ত)
    {
	place->img_ver = img->ver;
	place->backend = image_backend;
	place->cell_width = cell_width;
	place->cell_height = cell_height;

	// Bounding box is in cells
	image_get_cell_dimensions(img,
		&place->bounding_box.x2, &place->bounding_box.y2);
	place->crop_box.x2 = place->bounding_box.x2;
	place->crop_box.y2 = place->bounding_box.y2;

	যদি (PLACEMENT_FUNC(image_backend, init)(place) == FAIL)
	{
	    id -= PLACEMENT_ID_INC;
	    vim_free(place);
	    ঘূৰাই রিক্ত;
	}
    }
    অন্যথা
	place->backend = IMAGE_BACKEND_NONE;

    image_placement_link(place);

    ঘূৰাই place;
}

    স্থির শূন্য
image_placement_clear_int(image_placement_T *place, bool now)
{
    যদি (backend_available(false) && place->img != রিক্ত)
    {
	যদি (place->flags & IMAGEPF_VISIBLE_INIT
		&& PLACEMENT_FUNC(image_backend, blit))
	    redraw_region(&place->visible_abs, now, true);
	PLACEMENT_FUNC(image_backend, clear)(place);
	place->flags |= IMAGEPF_HIDDEN;
	যদি (!now && !updating_screen)
	    redraw_all_later(UPD_VALID);
    }
}

    শূন্য
image_placement_clear(image_placement_T *place)
{
    image_placement_clear_int(place, updating_screen);
}

    শূন্য
image_placement_free(image_placement_T *place)
{
    যদি (backend_available(false) && place->backend_data != রিক্ত)
    {
	image_placement_clear(place);
	PLACEMENT_FUNC(image_backend, uninit)(place);
    }

    যদি (place->flags & IMAGEPF_VISIBLE_INIT)
    {
	pixman_region32_fini(&place->visible);
	pixman_region32_fini(&place->visible_abs);
    }
    image_placement_unlink(place);

    যদি (place->img != রিক্ত)
	image_unref(place->img);
    vim_free(place);
}

    শূন্য
image_placement_set_zindex(image_placement_T *place, পূর্ণাঙ্ক zindex)
{
    // If image was hidden before, must dirty it so it is fully redrawn again.
    যদি (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    যদি (place->zindex == zindex)
	ঘূৰাই;
    place->zindex = zindex;
    place->flags |= IMAGEPF_DIRTY;

    // Must re-add the placement back so it is in the correct order
    image_placement_unlink(place);
    image_placement_link(place);
}

/*
 * Make sure to also update the bounding box!
 */
    শূন্য
image_placement_set_position(image_placement_T *place, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    যদি (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    যদি (place->row == row && place->col == col)
	ঘূৰাই;
    place->row = row;
    place->col = col;
    place->flags |= IMAGEPF_DIRTY;
}

    শূন্য
image_placement_set_crop(image_placement_T *place, পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, পূর্ণাঙ্ক w, পূর্ণাঙ্ক h)
{
    যদি (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    যদি (place->crop_box.x1 == x && place->crop_box.y1 == y
	    && place->crop_box.x2 == x + w && place->crop_box.y2 == y + h)
	ঘূৰাই;

    place->crop_box.x1 = x;
    place->crop_box.y1 = y;

    place->crop_box.x2 = x + w;
    place->crop_box.y2 = y + h;
    place->flags |= IMAGEPF_DIRTY;
}

    শূন্য
image_placement_set_bounding_box(
	image_placement_T   *place,
	পূর্ণাঙ্ক		    row,
	পূর্ণাঙ্ক		    col,
	পূর্ণাঙ্ক		    row_height,
	পূর্ণাঙ্ক		    col_width)
{
    যদি (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    যদি (place->bounding_box.x1 == col && place->bounding_box.y1 == row
	    && place->bounding_box.x2 == col + col_width
	    && place->bounding_box.y2 == row + row_height)
	ঘূৰাই;

    place->bounding_box.x1 = col;
    place->bounding_box.y1 = row;

    place->bounding_box.x2 = col + col_width;
    place->bounding_box.y2 = row + row_height;
    place->flags |= IMAGEPF_DIRTY;
}

/*
 * Enable or disable the image placement. Unlike IMAGEPF_HIDDEN, this is not
 * reset across redraws.
 */
    শূন্য
image_placement_set_enable(image_placement_T *place, bool enable)
{
    bool old = place->flags & IMAGEPF_ENABLED;

    যদি (enable && !old)
	place->flags |= IMAGEPF_ENABLED;
    অন্যথা যদি (!enable && old)
    {
	place->flags &= ~IMAGEPF_ENABLED;
	image_placement_clear(place);
    }
    অন্যথা
	ঘূৰাই;

    place->flags |= IMAGEPF_DIRTY;
}

/*
 * Note that the returned crop region (x, y, w, h) is in physical pixels if
 * "physical" is true. Not relevant when using terminal protocols (sixel, KGP).
 */
    শূন্য
image_placement_subrect(
	image_placement_T   *place,
	pixman_box32_t	    rect,
	পূর্ণাঙ্ক		    *row,
	পূর্ণাঙ্ক		    *col,
	পূর্ণাঙ্ক		    *x,
	পূর্ণাঙ্ক		    *y,
	পূর্ণাঙ্ক		    *w,
	পূর্ণাঙ্ক		    *h,
	bool		    physical)
{
    পূর্ণাঙ্ক iw, ih;

    image_get_dimensions(place->img, &iw, &ih);

    *row = place->row + rect.y1 + place->row_off;
    *col = place->col + rect.x1;

    *x = (rect.x1 + place->crop_box.x1) * cell_width;
    *y = (rect.y1 + place->crop_box.y1) * cell_height;
    *w = (rect.x2 - rect.x1) * cell_width;
    *h = (rect.y2 - rect.y1) * cell_height;

    যদি (physical)
    {
	*x = LOG2PHY(*x);
	*y = LOG2PHY(*y);
	*w = LOG2PHY(*w);
	*h = LOG2PHY(*h);
    }
    অন্যথা
    {
	iw = PHY2LOG(iw);
	ih = PHY2LOG(ih);
    }

    // Make that all the values are valid. "w" and "h" can especially be
    // invalid, because the image dimensions (in pixels) are converted to cells,
    // rounding *up*.
    *row = MIN(*row, Rows);
    *col = MIN(*col, Columns);

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
    স্থির শূন্য
redraw_region(pixman_region32_t *region, bool now, bool restore)
{
    pixman_box32_t  *rects;
    পূর্ণাঙ্ক		    n_rects;
    পূর্ণাঙ্ক		    cur_row = screen_cur_row;
    পূর্ণাঙ্ক		    cur_col = screen_cur_col;

    rects = pixman_region32_rectangles(region, &n_rects);
    যদি (rects == রিক্ত)
	ঘূৰাই;

    বাবে (পূর্ণাঙ্ক i = 0; i < n_rects; i++)
    {
	pixman_box32_t rect = rects[i];

	যদি (now)
	{
	    // The region may be from before a resize, when the screen was
	    // bigger.
	    পূর্ণাঙ্ক height = MIN(rect.y2, screen_Rows) - rect.y1;
	    পূর্ণাঙ্ক width = MIN(rect.x2, screen_Columns) - rect.x1;

	    // Tip: if you want to debug stale pixels appearing, set "inverse"
	    // of screen_draw_rectangle() to TRUE.
	    যদি (height > 0 && width > 0)
		screen_draw_rectangle(rect.y1, rect.x1, height, width,
			FALSE, TRUE);
	    অনুৱর্ত;
	}

	বাবে (পূর্ণাঙ্ক r = rect.y1; r < rect.y2; r++)
	{
	    যদি (r >= screen_Rows)
		ভঙ্গ;

	    বাবে (পূর্ণাঙ্ক c = rect.x1; c < rect.x2; c++)
	    {
		যদি (c >= screen_Columns)
		    ভঙ্গ;

		ScreenAttrs[সাৰি_Offset[r] + c] = (sattr_T)-1;
	    }
	}
    }

    যদি (restore)
	windgoto(cur_row, cur_col);
}

/*
 * Draw all image placements to the screen. This should be done after all text
 * have been drawn to the screen, and image placements positioned correctly.
 */
    শূন্য
draw_image_placements(শূন্য)
{
    image_placement_T	*place;
    pixman_region32_t	subtract_region;    // In cells
    pixman_region32_t	dirty_region;	    // In cells
    image_placement_T	**pending_placements;
    পূর্ণাঙ্ক			pending_len = -1;
    // Save current cursor position
    পূর্ণাঙ্ক			cur_row = screen_cur_row;
    পূর্ণাঙ্ক			cur_col = screen_cur_col;

    // Check if all placements have no backing image. If so, then do nothing
    FOR_ALL_PLACEMENTS(place)
	যদি (place->img != রিক্ত)
	{
	    pending_len = 0;
	    ভঙ্গ;
	}

    যদি (pending_len == -1)
	ঘূৰাই;

    যদি (!backend_available(false))
	ঘূৰাই;

    যদি (n_placements == 0)
	ঘূৰাই;

    pending_placements = ALLOC_CLEAR_MULT(image_placement_T *, n_placements);
    যদি (pending_placements == রিক্ত)
	ঘূৰাই;

    pixman_region32_init(&subtract_region);
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
    FOR_ALL_PLACEMENTS(place)
    {
	image_T *img = place->img;

	pixman_region32_t   image_region;
	pixman_region32_t   visible_region;

	পূর্ণাঙ্ক x, y;

	যদি (place->flags & IMAGEPF_HIDDEN || !(place->flags & IMAGEPF_ENABLED))
	    // Don't need to clear image placement for non-blit backends,
	    // because we call image_placement_clear() every time we don't want
	    // to see the image. Not sure of the side effects if we also clear
	    // it here...
	    অনুৱর্ত;

	যদি (img != রিক্ত)
	{
	    pixman_region32_t	visible_abs;
	    পূর্ণাঙ্ক			crop_w, crop_h;
	    bool		cs_changed;
	    bool		need_redraw;

	    // Processing changed image data failed, skip this image
	    যদি (place->img->data == রিক্ত)
	    {
		image_placement_clear(place);
		অনুৱর্ত;
	    }

	    // Only apply row shift if image is within the shifted region.
	    যদি ((place->bounding_box.y1 >= shift_শীৰ্ষ
			&& place->bounding_box.y2 <= shift_bot)
		    && place->row_off != shift)
	    {
		place->row_off = shift;
		place->flags |= IMAGEPF_DIRTY;
	    }

	    x = place->col;
	    y = place->row + place->row_off;

	    crop_w = place->crop_box.x2 - place->crop_box.x1;
	    crop_h = place->crop_box.y2 - place->crop_box.y1;

	    // Don't add "crop_box" x1 and y1, because "row" and "col" use the
	    // top left of the final cropped image.
	    pixman_region32_init_rect(&image_region, x, y, crop_w, crop_h);

	    pixman_region32_init(&visible_region);
	    pixman_region32_init(&visible_abs);

	    // Only use parts of the image region within the actual screen.
	    // "row_off" may offset the image slightly/completely off screen.
	    যদি (!pixman_region32_intersect_rect(&image_region, &image_region,
			0, 0, Columns, Rows)
		    || !pixman_region32_subtract(&visible_region,
			&image_region, &subtract_region)
		    || !pixman_region32_copy(&visible_abs, &visible_region))
	    {
		pixman_region32_fini(&image_region);
		pixman_region32_fini(&visible_region);
		অনুৱর্ত;
	    }

	    pixman_region32_fini(&image_region);

	    // The visible region is in absolute coordinates, must convert it
	    // into image relative coordinates.
	    pixman_region32_translate(&visible_region, -x, -y);

	    cs_changed = place->cell_width != cell_width
		|| place->cell_height != cell_height;

	    // If cell size changed, then the visible regions are invalid pixel
	    // wise. This prevents backends such as sixel from using cached
	    // sequences.
	    যদি (cs_changed)
	    {
		place->cell_width = cell_width;
		place->cell_height = cell_height;
		place->flags |= IMAGEPF_FORCE;
	    }

	    // Only redraw the image if it has changed (or if we haven't drawn
	    // it yet). Or if image data has changed or cell size has changed
	    need_redraw = place->flags & IMAGEPF_DIRTY || cs_changed
		||  (place->flags & IMAGEPF_VISIBLE_INIT
			&& !pixman_region32_equal(&visible_region,
			    &place->visible));

	    যদি (place->img_ver != img->ver)
	    {
		// Image data changed
		need_redraw = true;
		place->img_ver = img->ver;

		image_placement_clear(place);
		PLACEMENT_FUNC(image_backend, uninit)(place);
		যদি (PLACEMENT_FUNC(image_backend, init)(place) == FAIL)
		{
		    emsg(_(e_out_of_memory));
		    image_unref(place->img);
		    place->img = রিক্ত;
		    অনুৱর্ত;
		}
	    }

	    যদি (need_redraw)
	    {
		যদি (place->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&place->visible);
		place->visible = visible_region;

		যদি (PLACEMENT_FUNC(image_backend, blit))
		{
		    // Must redraw the stale regions that will not be composited
		    // over (for this specific image).
		    pixman_region32_t stale_region;

		    pixman_region32_init(&stale_region);

		    যদি (place->flags & IMAGEPF_VISIBLE_INIT)
		    {
			(শূন্য)pixman_region32_subtract(&stale_region,
				&place->visible_abs, &visible_abs);

			// Since pixels may be translucent/transparent, we must
			// redraw the cells behind the image every time, so that
			// no stale pixels remain from the previous redraw.
			যদি (img->fmt == IMAGE_FORMAT_RGBA)
			    (শূন্য)pixman_region32_union(&stale_region,
				    &stale_region, &place->visible_abs);
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
		    যদি (img->fmt != IMAGE_FORMAT_RGBA)
		    {
			পূর্ণাঙ্ক		    min_w, min_h;
			পূর্ণাঙ্ক		    rect_w, rect_h;
			pixman_region32_t   min_region;

			image_get_cell_dimensions_min(img, &min_w, &min_h);

			// Ensure "min_region" only covers the fully covered
			// cells of the cropped region.
			rect_w = min_w - place->crop_box.x1;
			rect_h = min_h - place->crop_box.y1;

			যদি (rect_w > 0 && rect_h > 0)
			    // pixman_region32_init_rect() expects a non zero
			    // width and height (otherwise it outputs error
			    // message).
			    pixman_region32_init_rect(&min_region,
				    x, y, rect_w, rect_h);
			অন্যথা
			    pixman_region32_init(&min_region);

			(শূন্য)pixman_region32_subtract(&min_region,
				&visible_abs, &min_region);
			(শূন্য)pixman_region32_union(&stale_region,
				&stale_region, &min_region);
			pixman_region32_fini(&min_region);
		    }

		    যদি (place->flags & IMAGEPF_VISIBLE_INIT)
			// Also subtract "subtract_region", so we don't
			// redundantly redraw cells that will have images
			// painted over them after. Make sure to do this before
			// we check partially covered cells, since
			// "subtract_region" may also have partially covered
			// cells.
			(শূন্য)pixman_region32_subtract(&stale_region,
				&stale_region, &subtract_region);

		    // Don't redraw now, defer it later
		    pixman_region32_union(&dirty_region,
			    &dirty_region, &stale_region);
		    pixman_region32_fini(&stale_region);
		}
		যদি (place->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&place->visible_abs);
		place->visible_abs = visible_abs;

		pending_placements[pending_len++] = place;
		place->flags |= IMAGEPF_VISIBLE_INIT;
		place->flags &= ~IMAGEPF_DIRTY;
	    }
	    অন্যথা
	    {
		pixman_region32_fini(&visible_region);
		pixman_region32_fini(&visible_abs);
	    }
	}

	// If this is last image, no need to union its bounds.
	যদি (place->next == রিক্ত)
	    ভঙ্গ;

	pixman_region32_union_rect(&subtract_region, &subtract_region,
		place->bounding_box.x1,
		place->bounding_box.y1 + place->row_off,
		place->bounding_box.x2 - place->bounding_box.x1,
		place->bounding_box.y2 - place->bounding_box.y1);
    }

    // Don't want to dirty images again redundantly
    freeze_mark_dirty++;
    redraw_region(&dirty_region, true, false);
    freeze_mark_dirty--;

    যদি (pending_len > 0)
    {
#ifdef FEAT_GUI
	// Make sure to flush any output! We write directly to the draw
	// area/whatever, meaning draw commands outputted before this (as escape
	// sequences), will still draw over the image surface if we don't flush
	// now.
	যদি (gui.in_use)
	    out_flush();
#endif

	বাবে (পূর্ণাঙ্ক i = 0; i < pending_len; i++)
	{
	    PLACEMENT_FUNC(image_backend, draw)(pending_placements[i]);
	    pending_placements[i]->flags &= ~IMAGEPF_FORCE;
	}
    }

    // Restore cursor position, however do not turn on the cursor, since that
    // causes it to flicker at the wrong position when using sixel. It will be
    // turned on later somewhere...
    windgoto(cur_row, cur_col);
    out_flush();

    vim_free(pending_placements);
    pixman_region32_fini(&subtract_region);
    pixman_region32_fini(&dirty_region);
}

/*
 * Mark any images that touches the region dirty. Only used for backends that
 * blit pixels.
 */
    শূন্য
mark_dirty_region_for_images(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, পূর্ণাঙ্ক row_height, পূর্ণাঙ্ক col_width)
{
    image_placement_T	*place;
    pixman_box32_t	rect;

    যদি (freeze_mark_dirty > 0 || !backend_available(false)
	    || !PLACEMENT_FUNC(image_backend, blit) || n_placements == 0)
	ঘূৰাই;

    rect.x1 = col;
    rect.y1 = row;
    rect.x2 = col + col_width;
    rect.y2 = row + row_height;

    FOR_ALL_PLACEMENTS(place)
    {
	যদি (place->flags & IMAGEPF_HIDDEN)
	    অনুৱর্ত;
	যদি (pixman_region32_contains_rectangle(&place->visible_abs, &rect)
		== PIXMAN_REGION_OUT)
	    অনুৱর্ত;

	place->flags |= IMAGEPF_DIRTY;
    }
}

    শূন্য
clear_all_image_placements(শূন্য)
{
    image_placement_T *place;

    FOR_ALL_PLACEMENTS(place)
	image_placement_clear(place);
    যদি (!updating_screen)
	redraw_all_later(UPD_VALID);
}

/*
 * Used for screen_ins_lines() and screen_del_lines(), because they directly
 * shift rows. Note that this does not queue a screen redraw.
 */
    শূন্য
shift_image_placements(পূর্ণাঙ্ক শীৰ্ষ, পূর্ণাঙ্ক bot, পূর্ণাঙ্ক amount)
{
    shift_শীৰ্ষ = শীৰ্ষ;
    shift_bot = bot;
    shift += amount;
}

/*
 * Called when screen is cleared, as update_screen() may go to clearing the
 * screen instead of inserting lines using screen_ins_lines().
 */
    শূন্য
shift_reset_image_placements(শূন্য)
{
    shift = 0;
}

    স্থির image_T *
find_image(int_u id)
{
    image_T *img;

    FOR_ALL_IMAGES(img)
	যদি (img->state == IMAGE_STATE_PUBLIC && img->id == id)
	    ঘূৰাই img;
    ঘূৰাই রিক্ত;
}

/*
 * Add an image using the given information in "dict". If "existing" is not
 * NULL, try updating it instead. If "find" is TRUE, then accept the "id" field.
 */
    image_T *
add_image(dict_T *dict, image_T *existing, bool find)
{
    dictitem_T	    *di;
    blob_T	    *data;
    varnumber_T     w, h;
    varnumber_T     n_pixels;
    image_format_T  fmt;

    যদি (find && dict_has_চাবি(dict, "id"))
    {
	image_T *img = find_image((int_u)dict_get_সংখ্যা(dict, "id"));

	যদি (img == রিক্ত)
	    ঘূৰাই রিক্ত;
	ঘূৰাই image_ref(img);
    }

    di = dict_find(dict, (char_u *)"data", -1);
    w = dict_get_সংখ্যা(dict, "width");
    h = dict_get_সংখ্যা(dict, "height");

    যদি (di == রিক্ত || di->di_tv.v_type != VAR_BLOB || w <= 0 || h <= 0)
    {
	emsg(_(e_invalid_argument));
	ঘূৰাই রিক্ত;
    }

    // Check for overflow
    n_pixels = w * h;
    যদি (w > INT_MAX || h > INT_MAX || n_pixels > INT_MAX)
    {
	emsg(_(e_invalid_image_dimensions));
	ঘূৰাই রিক্ত;
    }

    data = di->di_tv.vval.v_blob;

    যদি (blob_len(data) == n_pixels * 3)
	fmt = IMAGE_FORMAT_RGB;
    অন্যথা যদি (blob_len(data) == n_pixels * 4)
	fmt = IMAGE_FORMAT_RGBA;
    অন্যথা
    {
	semsg(_(e_invalid_মান_for_argument_str_str), "data",
		"data length must equal width*height*3 or width*height*4");
	ঘূৰাই রিক্ত;
    }

    যদি (existing != রিক্ত)
    {
	যদি (existing->width == w && existing->height == h
		&& existing->fmt == fmt)
	{
	    যদি (image_update(existing, data->bv_ga.ga_data) == FAIL)
		ঘূৰাই রিক্ত;
	    ঘূৰাই existing;
	}
    }

    ঘূৰাই image_new(data->bv_ga.ga_data, w, h, fmt);
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
    পূর্ণাঙ্ক		w, h;
    dictitem_T	*di;

    যদি (img->data == রিক্ত)
	ঘূৰাই রিক্ত;

    dict = dict_alloc();
    যদি (dict == রিক্ত)
	ঘূৰাই রিক্ত;

    blob = blob_alloc();
    যদি (blob == রিক্ত)
    {
	dict_unref(dict);
	ঘূৰাই রিক্ত;
    }

    image_get_dimensions(img, &w, &h);
    (শূন্য)ga_concat_bytes(&blob->bv_ga, (ৱর্ণ *)img->data,
	    (size_t)w * h * img->fmt);

    di = dictitem_alloc((char_u *)"data");

    যদি (di != রিক্ত)
    {
	di->di_tv.v_type = VAR_BLOB;
	di->di_tv.vval.v_blob = blob;
    }
    যদি (di == রিক্ত || dict_add(dict, di) == FAIL)
    {
	যদি (di != রিক্ত)
	    dictitem_free(di);
	dict_unref(dict);
	// No need to unref blob, dictitem_free() will do that.
	ঘূৰাই রিক্ত;
    }

    যদি (img->state == IMAGE_STATE_PUBLIC)
	dict_add_সংখ্যা(dict, "id", img->id);
    dict_add_সংখ্যা(dict, "width", img->width);
    dict_add_সংখ্যা(dict, "height", img->height);
    dict_add_সংখ্যা(dict, "alpha", img->fmt == IMAGE_FORMAT_RGBA);
    dict_add_string(dict, "format",
	    (char_u *)(img->fmt == IMAGE_FORMAT_RGB ? "rgb" : "rgba"));

    blob->bv_refcount = 1;

    ঘূৰাই dict;
}

    স্থির পূর্ণাঙ্ক
match_imageprotocol(image_backend_T *backend)
{
    পূর্ণাঙ্ক		    len = (পূর্ণাঙ্ক)STRLEN(p_ipc) + 1;
    char_u	    *buf = alloc(len);
    char_u	    *p = p_ipc;
    পূর্ণাঙ্ক		    ret = FAIL;
    image_backend_T res = IMAGE_BACKEND_NONE;
    bool	    got = false;

    যদি (buf == রিক্ত)
	ঘূৰাই FAIL;

    যেতিয়ালৈ (*p != NUL)
    {
	char_u		*colon;
	regmatch_T	regmatch;
	image_backend_T prot = IMAGE_BACKEND_NONE;

	// Isolate one comma separated item.
	(শূন্য)copy_option_part(&p, buf, len, ",");

	colon = vim_strchr(buf, ':');
	যদি (colon == রিক্ত || colon == buf || colon[1] == NUL)
	    গচ্ছ exit;

	*colon = NUL;

	// Note: Keep this in sync with p_ipc_protocol_values.
	যদি (STRCMP(colon + 1, "none") == 0)
	    prot = IMAGE_BACKEND_NONE;
	অন্যথা যদি (STRCMP(colon + 1, "kitty") == 0)
	{
#ifdef FEAT_IMAGE_KITTY
	    prot = IMAGE_BACKEND_KITTY;
#endif
	}
	অন্যথা যদি (STRCMP(colon + 1, "sixel") == 0)
	{
#ifdef FEAT_IMAGE_SIXEL
	    prot = IMAGE_BACKEND_SIXEL;
#endif
	}
	অন্যথা
	    গচ্ছ exit;

	যদি (got)
	    অনুৱর্ত;

	CLEAR_FIELD(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);

	যদি (regmatch.regprog == রিক্ত)
	    গচ্ছ exit;

	bool match = T_NAME != রিক্ত
	    && vim_regexec(&regmatch, T_NAME, (colnr_T)0);

	vim_regfree(regmatch.regprog);
	যদি (match)
	{
	    // Keep going to catch any errors
	    res = prot;
	    got = true;
	}
    }

    ret = OK;
    *backend = res;
exit:
    vim_free(buf);
    ঘূৰাই ret;
}

/*
 * Update the current image backend to use depending on 'imageprotocol' and if
 * GUI is being used. Returns OK on success and FAIL on failure.
 */
    পূর্ণাঙ্ক
update_image_backend(শূন্য)
{
    image_backend_T new;

    image_T		*img;
    image_placement_T	*place;

    // Always process 'imageprotocol' option to check for errors
    যদি (match_imageprotocol(&new) == FAIL)
	ঘূৰাই FAIL;

#ifdef FEAT_IMAGE_GUI
    যদি (gui.in_use)
	new = IMAGE_BACKEND_GUI;
#endif

    যদি (image_backend == new)
	ঘূৰাই OK;

    // Must uninit the backends of every image/placement, then init the new
    // backend for each.
    FOR_ALL_IMAGES(img)
    {
	যদি (image_backend != IMAGE_BACKEND_NONE && img->data != রিক্ত
		&& img->backend_data != রিক্ত)
	    IMAGE_FUNC(image_backend, uninit)(img);
	img->backend_data = রিক্ত;
	যদি (new != IMAGE_BACKEND_NONE)
	{
	    যদি (IMAGE_FUNC(new, init)(img) == FAIL)
		গচ্ছ fail;
	    img->backend = new;
	}
    }
    FOR_ALL_PLACEMENTS(place)
    {
	image_placement_clear(place);
	যদি (image_backend != IMAGE_BACKEND_NONE
		&& place->img != রিক্ত && place->backend_data != রিক্ত)
	    PLACEMENT_FUNC(image_backend, uninit)(place);
	place->backend_data = রিক্ত;
	যদি (new != IMAGE_BACKEND_NONE && place->img != রিক্ত)
	{
	    যদি (PLACEMENT_FUNC(new, init)(place) == FAIL)
		গচ্ছ fail;
	    place->backend = new;
	}
    }

    redraw_all_later(UPD_VALID);
    image_backend = new;

    যদি (new == IMAGE_BACKEND_NONE)
	set_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    অন্যথা
	set_vim_var_string(VV_IMAGEBACKEND,
		(char_u *)image_backends[image_backend].name, -1);

    ঘূৰাই OK;
fail:
    FOR_ALL_IMAGES(img)
	যদি (img->backend != IMAGE_BACKEND_NONE)
	{
	    IMAGE_FUNC(img->backend, uninit)(img);
	    img->backend = IMAGE_BACKEND_NONE;
	}

    FOR_ALL_PLACEMENTS(place)
    {
	যদি (place->img != রিক্ত && place->backend != IMAGE_BACKEND_NONE)
	{
	    image_placement_clear(place);
	    PLACEMENT_FUNC(place->backend, uninit)(place);
	}
	place->backend = IMAGE_BACKEND_NONE;
    }

    redraw_all_later(UPD_VALID);

    image_backend = IMAGE_BACKEND_NONE;
    set_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    emsg(_(e_changing_image_backend_failed));
    ঘূৰাই FAIL;
}

    শূন্য
f_image_add(typval_T *argvars, typval_T *rettv)
{
    dict_T  *dict;
    image_T *img;

    যদি (check_for_dict_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    dict = argvars[0].vval.v_dict;

    img = add_image(dict, রিক্ত, false);
    যদি (img != রিক্ত)
	img->state = IMAGE_STATE_PUBLIC;

    rettv->v_type = VAR_সংখ্যা;
    rettv->vval.v_সংখ্যা = img == রিক্ত ? (varnumber_T)-1 : img->id;
}

    শূন্য
f_image_discard(typval_T *argvars, typval_T *rettv UNUSED)
{
    image_T *img;

    যদি (check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    img = find_image((int_u)argvars[0].vval.v_সংখ্যা);

    যদি (img != রিক্ত)
    {
	img->state = IMAGE_STATE_PRIVATE;
	image_unref(img);
    }
    অন্যথা
	semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_সংখ্যা);
}

    শূন্য
f_image_info(typval_T *argvars, typval_T *rettv)
{
    image_T *img;
    dict_T  *dict;

    যদি (rettv_list_alloc(rettv) == FAIL
	    || check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    যদি (argvars[0].v_type == VAR_সংখ্যা)
    {
	img = find_image((int_u)argvars[0].vval.v_সংখ্যা);
	যদি (img == রিক্ত)
	{
	    semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_সংখ্যা);
	    ঘূৰাই;
	}
	dict = get_image_info(img);
	যদি (dict != রিক্ত && list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	ঘূৰাই;
    }

    FOR_ALL_IMAGES(img)
    {
	যদি (img->state == IMAGE_STATE_PRIVATE)
	    অনুৱর্ত;
	dict = get_image_info(img);
	যদি (dict == রিক্ত
		|| list_append_dict(rettv->vval.v_list, dict) == FAIL)
	{
	    dict_unref(dict);	// NULL-safe
	    ঘূৰাই;
	}
    }
}

#endif // FEAT_IMAGE
