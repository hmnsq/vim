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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_IMAGE)

ਬਣਤਰ
{
    ਅਚਲ ਅੱਖਰ *name;

    ਬਣਤਰ
    {
	// Return OK on success and FAIL on failure
	ਪੂਰਨ (*init)(image_T *);
	ਖਾਲੀ (*uninit)(image_T *);
    } image;

    ਬਣਤਰ
    {
	// Return OK on success and FAIL on failure
	ਪੂਰਨ (*init)(image_placement_T *);
	ਖਾਲੀ (*uninit)(image_placement_T *);

	ਖਾਲੀ (*draw)(image_placement_T *);
	ਖਾਲੀ (*clear)(image_placement_T *);

	// If backend blits the image pixels to the screen. Images are not
	// treated as "objects".
	bool blit;
    } placement;
} image_backends[] = {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_GUI
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
#  ਨਹੀਂਤਾਂ
	    .blit = true,
#  endif
# elif FEAT_GUI_MSWIN
	    .blit = true,
# ਨਹੀਂਤਾਂ
	    .blit = false,
# endif
	}
    },
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_KITTY
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
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_SIXEL
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
#ਜੇ_ਅੰਤ
};

// Current image backend being used
ਸਥਿਰ image_backend_T image_backend = IMAGE_BACKEND_NONE;

ਸਥਿਰ image_T *images = ਨਿਰਾ;
// Sorted from highest zindex to lowest zindex
ਸਥਿਰ image_placement_T    *placements = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ		    n_placements = 0;

ਸਥਿਰ ਪੂਰਨ ਖਿਸਕਾਓ_ਸਿਖਰ = 0;
ਸਥਿਰ ਪੂਰਨ ਖਿਸਕਾਓ_bot = 0;
ਸਥਿਰ ਪੂਰਨ ਖਿਸਕਾਓ = 0;

// If > 0, then mark_dirty_region_for_images() will be a no-op
ਸਥਿਰ ਪੂਰਨ freeze_mark_dirty = 0;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_IMAGES(v) ਲਈ ((v) = images; (v) != ਨਿਰਾ; (v) = (v)->ਅਗਲਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_PLACEMENTS(v) \
    ਲਈ ((v) = placements; (v) != ਨਿਰਾ; (v) = (v)->ਅਗਲਾ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMAGE_FUNC(b, n) (image_backends[b].image.n)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLACEMENT_FUNC(b, n) (image_backends[b].placement.n)

ਸਥਿਰ ਖਾਲੀ redraw_region(pixman_region32_t *region, bool now, bool restore);

    ਖਾਲੀ
init_image_state(ਖਾਲੀ)
{
    (ਖਾਲੀ)update_image_backend();
}

    ਖਾਲੀ
uninit_image_state(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_SIXEL
    sixel_uninit();
#ਜੇ_ਅੰਤ
}

/*
 * Return true if the there image backend is ready, otherwise emit error.
 */
    ਸਥਿਰ bool
backend_ਉਪਲਬਧ(bool ਸੁਨੇਹਾ)
{
    ਜੇ (image_backend == IMAGE_BACKEND_NONE)
    {
	ਜੇ (ਸੁਨੇਹਾ)
	    emsg(_(e_no_image_backend_ਉਪਲਬਧ));
	ਵਾਪਸ false;
    }
    ਵਾਪਸ true;
}

/*
 * Create a new image using the given data (creates a copy). Returns NULL on
 * failure.
 */
    image_T *
image_new(uint8_t *ਅੰਕੜੇ, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ, image_format_T fmt)
{
    image_T	    *img;
    ਸਥਿਰ int_u    id;

    ਜੇ (!backend_ਉਪਲਬਧ(true))
	ਵਾਪਸ ਨਿਰਾ;

    img = ALLOC_CLEAR_ONE(image_T);
    ਜੇ (img == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    img->ਅੰਕੜੇ = vim_memsave(ਅੰਕੜੇ, (size_t)ਚੌੜਾਈ * ਉਚਾਈ * fmt);
    ਜੇ (img->ਅੰਕੜੇ == ਨਿਰਾ)
    {
	vim_free(img);
	ਵਾਪਸ ਨਿਰਾ;
    }

    img->state = IMAGE_STATE_ਨਿੱਜੀ;
    img->ਚੌੜਾਈ = ਚੌੜਾਈ;
    img->ਉਚਾਈ = ਉਚਾਈ;
    img->fmt = fmt;

    // Mix in PID to prevent ID collisions when using kitty graphics protocol
    img->id = ((((int_u)mch_get_pid() & 0x7fff) + 1) << 16) | (id++ & 0xffff);
    img->refcount = 1;

    img->backend = image_backend;

    ਜੇ (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	vim_free(img->ਅੰਕੜੇ);
	vim_free(img);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (images != ਨਿਰਾ)
	images->prev = img;
    img->ਅਗਲਾ = images;
    images = img;

    ਵਾਪਸ img;
}

    ਸਥਿਰ ਖਾਲੀ
image_free(image_T *img)
{
    ਜੇ (backend_ਉਪਲਬਧ(false))
	IMAGE_FUNC(image_backend, uninit)(img);

    ਜੇ (img->prev != ਨਿਰਾ)
	img->prev->ਅਗਲਾ = img->ਅਗਲਾ;
    ਜੇ (img->ਅਗਲਾ != ਨਿਰਾ)
	img->ਅਗਲਾ->prev = img->prev;
    ਜੇ (images == img)
	images = img->ਅਗਲਾ;

    vim_free(img->ਅੰਕੜੇ);
    vim_free(img);
}

/*
 * Decrement reference count of an image. Be careful to not let the public API
 * be able to directly decrement the refcount of an image multiple times!
 */
    ਖਾਲੀ
image_unref(image_T *img)
{
    ਜੇ (--img->refcount <= 0)
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
    ਵਾਪਸ img;
}

/*
 * Get image width and height in cells, rounding up so that every pixel is
 * covered.
 */
    ਖਾਲੀ
image_get_cell_dimensions(image_T *img, ਪੂਰਨ *cw, ਪੂਰਨ *ch)
{
    ਪੂਰਨ w = PHY2LOG(img->ਚੌੜਾਈ);
    ਪੂਰਨ h = PHY2LOG(img->ਉਚਾਈ);

    *cw = (w + cell_ਚੌੜਾਈ - 1) / cell_ਚੌੜਾਈ;
    *ch = (h + cell_ਉਚਾਈ - 1) / cell_ਉਚਾਈ;
}

/*
 * Similar, but return the dimensions of all cells that are *fully* covered by
 * pixels. Pixels that are partially covered are not counted.
 */
    ਸਥਿਰ ਖਾਲੀ
image_get_cell_dimensions_min(image_T *img, ਪੂਰਨ *cw, ਪੂਰਨ *ch)
{
    ਪੂਰਨ w = PHY2LOG(img->ਚੌੜਾਈ);
    ਪੂਰਨ h = PHY2LOG(img->ਉਚਾਈ);

    *cw = w / cell_ਚੌੜਾਈ;
    *ch = h / cell_ਉਚਾਈ;
}

    ਖਾਲੀ
image_get_dimensions(image_T *img, ਪੂਰਨ *w, ਪੂਰਨ *h)
{
    *w = img->ਚੌੜਾਈ;
    *h = img->ਉਚਾਈ;
}

/*
 * Replace the current image contents with "data". "data" should be the exact
 * same format, width, and height. Return OK on success and FAIL on failure.
 */
    ਪੂਰਨ
image_update(image_T *img, uint8_t *ਅੰਕੜੇ)
{
    ਪੂਰਨ w, h;

    ਜੇ (!backend_ਉਪਲਬਧ(true) || img->ਅੰਕੜੇ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    image_get_dimensions(img, &w, &h);
    mch_memmove(img->ਅੰਕੜੇ, ਅੰਕੜੇ, (size_t)w * h * img->fmt);

    IMAGE_FUNC(image_backend, uninit)(img);

    ਜੇ (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	emsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ));
	VIM_CLEAR(img->ਅੰਕੜੇ);
	ਵਾਪਸ FAIL;
    }

    img->ver++;
    redraw_all_later(UPD_VALID);
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਖਾਲੀ
image_placement_unlink(image_placement_T *ਰੱਖੋ)
{
    ਜੇ (ਰੱਖੋ->prev != ਨਿਰਾ)
	ਰੱਖੋ->prev->ਅਗਲਾ = ਰੱਖੋ->ਅਗਲਾ;
    ਜੇ (ਰੱਖੋ->ਅਗਲਾ != ਨਿਰਾ)
	ਰੱਖੋ->ਅਗਲਾ->prev = ਰੱਖੋ->prev;
    ਜੇ (placements == ਰੱਖੋ)
	placements = ਰੱਖੋ->ਅਗਲਾ;
    n_placements--;
}

    ਸਥਿਰ ਖਾਲੀ
image_placement_link(image_placement_T *ਰੱਖੋ)
{
    image_placement_T *p = placements;
    image_placement_T *prev = ਨਿਰਾ;

    // Add placement *before* the placement with the lower zindex.
    ਜਦੋਂ (p != ਨਿਰਾ)
    {
	ਜੇ (ਰੱਖੋ->zindex > p->zindex)
	    ਰੋਕੋ;
	ਨਹੀਂਤਾਂ ਜੇ (ਰੱਖੋ->zindex == p->zindex)
	{
	    ਜੇ (ਰੱਖੋ->use > p->use)
		ਰੋਕੋ;
	    ਜੇ (ਰੱਖੋ->gen < p->gen)
		ਰੋਕੋ;
	}

	prev = p;
	p = p->ਅਗਲਾ;
    }

    ਰੱਖੋ->ਅਗਲਾ = p;
    ਰੱਖੋ->prev = prev;

    ਜੇ (p != ਨਿਰਾ)
	p->prev = ਰੱਖੋ;

    ਜੇ (prev != ਨਿਰਾ)
	prev->ਅਗਲਾ = ਰੱਖੋ;
    ਨਹੀਂਤਾਂ
	placements = ਰੱਖੋ;

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
    image_placement_T	*ਰੱਖੋ;
    ਸਥਿਰ int_u	id = 1; // Kitty placements id must be > 1

    ਜੇ (!backend_ਉਪਲਬਧ(!quiet))
	ਵਾਪਸ ਨਿਰਾ;

    ਰੱਖੋ = ALLOC_CLEAR_ONE(image_placement_T);
    ਜੇ (ਰੱਖੋ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਰੱਖੋ->id = id;
    id += PLACEMENT_ID_INC; // Allocate 1000 free placement ids to be used to
			    // draw this image placement. Only relevant for
			    // kitty graphics protocol.
    ਰੱਖੋ->gen = gen;
    ਰੱਖੋ->use = use;

    ਰੱਖੋ->img = img;
    ਰੱਖੋ->flags = IMAGEPF_DIRTY;

    ਜੇ (img != ਨਿਰਾ)
    {
	ਰੱਖੋ->img_ver = img->ver;
	ਰੱਖੋ->backend = image_backend;
	ਰੱਖੋ->cell_ਚੌੜਾਈ = cell_ਚੌੜਾਈ;
	ਰੱਖੋ->cell_ਉਚਾਈ = cell_ਉਚਾਈ;

	// Bounding box is in cells
	image_get_cell_dimensions(img,
		&ਰੱਖੋ->bounding_box.x2, &ਰੱਖੋ->bounding_box.y2);
	ਰੱਖੋ->crop_box.x2 = ਰੱਖੋ->bounding_box.x2;
	ਰੱਖੋ->crop_box.y2 = ਰੱਖੋ->bounding_box.y2;

	ਜੇ (PLACEMENT_FUNC(image_backend, init)(ਰੱਖੋ) == FAIL)
	{
	    id -= PLACEMENT_ID_INC;
	    vim_free(ਰੱਖੋ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ
	ਰੱਖੋ->backend = IMAGE_BACKEND_NONE;

    image_placement_link(ਰੱਖੋ);

    ਵਾਪਸ ਰੱਖੋ;
}

    ਸਥਿਰ ਖਾਲੀ
image_placement_clear_int(image_placement_T *ਰੱਖੋ, bool now)
{
    ਜੇ (backend_ਉਪਲਬਧ(false) && ਰੱਖੋ->img != ਨਿਰਾ)
    {
	ਜੇ (ਰੱਖੋ->flags & IMAGEPF_VISIBLE_INIT
		&& PLACEMENT_FUNC(image_backend, blit))
	    redraw_region(&ਰੱਖੋ->visible_abs, now, true);
	PLACEMENT_FUNC(image_backend, clear)(ਰੱਖੋ);
	ਰੱਖੋ->flags |= IMAGEPF_HIDDEN;
	ਜੇ (!now && !updating_screen)
	    redraw_all_later(UPD_VALID);
    }
}

    ਖਾਲੀ
image_placement_clear(image_placement_T *ਰੱਖੋ)
{
    image_placement_clear_int(ਰੱਖੋ, updating_screen);
}

    ਖਾਲੀ
image_placement_free(image_placement_T *ਰੱਖੋ)
{
    ਜੇ (backend_ਉਪਲਬਧ(false) && ਰੱਖੋ->backend_ਅੰਕੜੇ != ਨਿਰਾ)
    {
	image_placement_clear(ਰੱਖੋ);
	PLACEMENT_FUNC(image_backend, uninit)(ਰੱਖੋ);
    }

    ਜੇ (ਰੱਖੋ->flags & IMAGEPF_VISIBLE_INIT)
    {
	pixman_region32_fini(&ਰੱਖੋ->visible);
	pixman_region32_fini(&ਰੱਖੋ->visible_abs);
    }
    image_placement_unlink(ਰੱਖੋ);

    ਜੇ (ਰੱਖੋ->img != ਨਿਰਾ)
	image_unref(ਰੱਖੋ->img);
    vim_free(ਰੱਖੋ);
}

    ਖਾਲੀ
image_placement_ਸੈੱਟ_zindex(image_placement_T *ਰੱਖੋ, ਪੂਰਨ zindex)
{
    // If image was hidden before, must dirty it so it is fully redrawn again.
    ਜੇ (ਰੱਖੋ->flags & IMAGEPF_HIDDEN)
	ਰੱਖੋ->flags |= IMAGEPF_DIRTY;
    ਰੱਖੋ->flags &= ~IMAGEPF_HIDDEN;

    ਜੇ (ਰੱਖੋ->zindex == zindex)
	ਵਾਪਸ;
    ਰੱਖੋ->zindex = zindex;
    ਰੱਖੋ->flags |= IMAGEPF_DIRTY;

    // Must re-add the placement back so it is in the correct order
    image_placement_unlink(ਰੱਖੋ);
    image_placement_link(ਰੱਖੋ);
}

/*
 * Make sure to also update the bounding box!
 */
    ਖਾਲੀ
image_placement_ਸੈੱਟ_position(image_placement_T *ਰੱਖੋ, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਜੇ (ਰੱਖੋ->flags & IMAGEPF_HIDDEN)
	ਰੱਖੋ->flags |= IMAGEPF_DIRTY;
    ਰੱਖੋ->flags &= ~IMAGEPF_HIDDEN;

    ਜੇ (ਰੱਖੋ->ਪੰਕਤੀ == ਪੰਕਤੀ && ਰੱਖੋ->ਖਾਨਾ == ਖਾਨਾ)
	ਵਾਪਸ;
    ਰੱਖੋ->ਪੰਕਤੀ = ਪੰਕਤੀ;
    ਰੱਖੋ->ਖਾਨਾ = ਖਾਨਾ;
    ਰੱਖੋ->flags |= IMAGEPF_DIRTY;
}

    ਖਾਲੀ
image_placement_ਸੈੱਟ_crop(image_placement_T *ਰੱਖੋ, ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h)
{
    ਜੇ (ਰੱਖੋ->flags & IMAGEPF_HIDDEN)
	ਰੱਖੋ->flags |= IMAGEPF_DIRTY;
    ਰੱਖੋ->flags &= ~IMAGEPF_HIDDEN;

    ਜੇ (ਰੱਖੋ->crop_box.x1 == x && ਰੱਖੋ->crop_box.y1 == y
	    && ਰੱਖੋ->crop_box.x2 == x + w && ਰੱਖੋ->crop_box.y2 == y + h)
	ਵਾਪਸ;

    ਰੱਖੋ->crop_box.x1 = x;
    ਰੱਖੋ->crop_box.y1 = y;

    ਰੱਖੋ->crop_box.x2 = x + w;
    ਰੱਖੋ->crop_box.y2 = y + h;
    ਰੱਖੋ->flags |= IMAGEPF_DIRTY;
}

    ਖਾਲੀ
image_placement_ਸੈੱਟ_bounding_box(
	image_placement_T   *ਰੱਖੋ,
	ਪੂਰਨ		    ਪੰਕਤੀ,
	ਪੂਰਨ		    ਖਾਨਾ,
	ਪੂਰਨ		    ਪੰਕਤੀ_ਉਚਾਈ,
	ਪੂਰਨ		    ਖਾਨਾ_ਚੌੜਾਈ)
{
    ਜੇ (ਰੱਖੋ->flags & IMAGEPF_HIDDEN)
	ਰੱਖੋ->flags |= IMAGEPF_DIRTY;
    ਰੱਖੋ->flags &= ~IMAGEPF_HIDDEN;

    ਜੇ (ਰੱਖੋ->bounding_box.x1 == ਖਾਨਾ && ਰੱਖੋ->bounding_box.y1 == ਪੰਕਤੀ
	    && ਰੱਖੋ->bounding_box.x2 == ਖਾਨਾ + ਖਾਨਾ_ਚੌੜਾਈ
	    && ਰੱਖੋ->bounding_box.y2 == ਪੰਕਤੀ + ਪੰਕਤੀ_ਉਚਾਈ)
	ਵਾਪਸ;

    ਰੱਖੋ->bounding_box.x1 = ਖਾਨਾ;
    ਰੱਖੋ->bounding_box.y1 = ਪੰਕਤੀ;

    ਰੱਖੋ->bounding_box.x2 = ਖਾਨਾ + ਖਾਨਾ_ਚੌੜਾਈ;
    ਰੱਖੋ->bounding_box.y2 = ਪੰਕਤੀ + ਪੰਕਤੀ_ਉਚਾਈ;
    ਰੱਖੋ->flags |= IMAGEPF_DIRTY;
}

/*
 * Enable or disable the image placement. Unlike IMAGEPF_HIDDEN, this is not
 * reset across redraws.
 */
    ਖਾਲੀ
image_placement_ਸੈੱਟ_enable(image_placement_T *ਰੱਖੋ, bool enable)
{
    bool old = ਰੱਖੋ->flags & IMAGEPF_ENABLED;

    ਜੇ (enable && !old)
	ਰੱਖੋ->flags |= IMAGEPF_ENABLED;
    ਨਹੀਂਤਾਂ ਜੇ (!enable && old)
    {
	ਰੱਖੋ->flags &= ~IMAGEPF_ENABLED;
	image_placement_clear(ਰੱਖੋ);
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ;

    ਰੱਖੋ->flags |= IMAGEPF_DIRTY;
}

/*
 * Note that the returned crop region (x, y, w, h) is in physical pixels if
 * "physical" is true. Not relevant when using terminal protocols (sixel, KGP).
 */
    ਖਾਲੀ
image_placement_subrect(
	image_placement_T   *ਰੱਖੋ,
	pixman_box32_t	    rect,
	ਪੂਰਨ		    *ਪੰਕਤੀ,
	ਪੂਰਨ		    *ਖਾਨਾ,
	ਪੂਰਨ		    *x,
	ਪੂਰਨ		    *y,
	ਪੂਰਨ		    *w,
	ਪੂਰਨ		    *h,
	bool		    physical)
{
    ਪੂਰਨ iw, ih;

    image_get_dimensions(ਰੱਖੋ->img, &iw, &ih);

    *ਪੰਕਤੀ = ਰੱਖੋ->ਪੰਕਤੀ + rect.y1 + ਰੱਖੋ->ਪੰਕਤੀ_off;
    *ਖਾਨਾ = ਰੱਖੋ->ਖਾਨਾ + rect.x1;

    *x = (rect.x1 + ਰੱਖੋ->crop_box.x1) * cell_ਚੌੜਾਈ;
    *y = (rect.y1 + ਰੱਖੋ->crop_box.y1) * cell_ਉਚਾਈ;
    *w = (rect.x2 - rect.x1) * cell_ਚੌੜਾਈ;
    *h = (rect.y2 - rect.y1) * cell_ਉਚਾਈ;

    ਜੇ (physical)
    {
	*x = LOG2PHY(*x);
	*y = LOG2PHY(*y);
	*w = LOG2PHY(*w);
	*h = LOG2PHY(*h);
    }
    ਨਹੀਂਤਾਂ
    {
	iw = PHY2LOG(iw);
	ih = PHY2LOG(ih);
    }

    // Make that all the values are valid. "w" and "h" can especially be
    // invalid, because the image dimensions (in pixels) are converted to cells,
    // rounding *up*.
    *ਪੰਕਤੀ = MIN(*ਪੰਕਤੀ, ਪੰਕਤੀਆਂ);
    *ਖਾਨਾ = MIN(*ਖਾਨਾ, Columns);

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
    ਸਥਿਰ ਖਾਲੀ
redraw_region(pixman_region32_t *region, bool now, bool restore)
{
    pixman_box32_t  *rects;
    ਪੂਰਨ		    n_rects;
    ਪੂਰਨ		    cur_ਪੰਕਤੀ = screen_cur_ਪੰਕਤੀ;
    ਪੂਰਨ		    cur_ਖਾਨਾ = screen_cur_ਖਾਨਾ;

    rects = pixman_region32_rectangles(region, &n_rects);
    ਜੇ (rects == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (ਪੂਰਨ i = 0; i < n_rects; i++)
    {
	pixman_box32_t rect = rects[i];

	ਜੇ (now)
	{
	    // The region may be from before a resize, when the screen was
	    // bigger.
	    ਪੂਰਨ ਉਚਾਈ = MIN(rect.y2, screen_ਪੰਕਤੀਆਂ) - rect.y1;
	    ਪੂਰਨ ਚੌੜਾਈ = MIN(rect.x2, screen_Columns) - rect.x1;

	    // Tip: if you want to debug stale pixels appearing, set "inverse"
	    // of screen_draw_rectangle() to TRUE.
	    ਜੇ (ਉਚਾਈ > 0 && ਚੌੜਾਈ > 0)
		screen_draw_rectangle(rect.y1, rect.x1, ਉਚਾਈ, ਚੌੜਾਈ,
			FALSE, TRUE);
	    ਜਾਰੀ;
	}

	ਲਈ (ਪੂਰਨ r = rect.y1; r < rect.y2; r++)
	{
	    ਜੇ (r >= screen_ਪੰਕਤੀਆਂ)
		ਰੋਕੋ;

	    ਲਈ (ਪੂਰਨ c = rect.x1; c < rect.x2; c++)
	    {
		ਜੇ (c >= screen_Columns)
		    ਰੋਕੋ;

		ScreenAttrs[ਸਤਰ_ਵਿੱਥ[r] + c] = (sattr_T)-1;
	    }
	}
    }

    ਜੇ (restore)
	windgoto(cur_ਪੰਕਤੀ, cur_ਖਾਨਾ);
}

/*
 * Draw all image placements to the screen. This should be done after all text
 * have been drawn to the screen, and image placements positioned correctly.
 */
    ਖਾਲੀ
draw_image_placements(ਖਾਲੀ)
{
    image_placement_T	*ਰੱਖੋ;
    pixman_region32_t	ਘਟਾਓ_region;    // In cells
    pixman_region32_t	dirty_region;	    // In cells
    image_placement_T	**pending_placements;
    ਪੂਰਨ			pending_len = -1;
    // Save current cursor position
    ਪੂਰਨ			cur_ਪੰਕਤੀ = screen_cur_ਪੰਕਤੀ;
    ਪੂਰਨ			cur_ਖਾਨਾ = screen_cur_ਖਾਨਾ;

    // Check if all placements have no backing image. If so, then do nothing
    FOR_ALL_PLACEMENTS(ਰੱਖੋ)
	ਜੇ (ਰੱਖੋ->img != ਨਿਰਾ)
	{
	    pending_len = 0;
	    ਰੋਕੋ;
	}

    ਜੇ (pending_len == -1)
	ਵਾਪਸ;

    ਜੇ (!backend_ਉਪਲਬਧ(false))
	ਵਾਪਸ;

    ਜੇ (n_placements == 0)
	ਵਾਪਸ;

    pending_placements = ALLOC_CLEAR_MULT(image_placement_T *, n_placements);
    ਜੇ (pending_placements == ਨਿਰਾ)
	ਵਾਪਸ;

    pixman_region32_init(&ਘਟਾਓ_region);
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
    FOR_ALL_PLACEMENTS(ਰੱਖੋ)
    {
	image_T *img = ਰੱਖੋ->img;

	pixman_region32_t   image_region;
	pixman_region32_t   visible_region;

	ਪੂਰਨ x, y;

	ਜੇ (ਰੱਖੋ->flags & IMAGEPF_HIDDEN || !(ਰੱਖੋ->flags & IMAGEPF_ENABLED))
	    // Don't need to clear image placement for non-blit backends,
	    // because we call image_placement_clear() every time we don't want
	    // to see the image. Not sure of the side effects if we also clear
	    // it here...
	    ਜਾਰੀ;

	ਜੇ (img != ਨਿਰਾ)
	{
	    pixman_region32_t	visible_abs;
	    ਪੂਰਨ			crop_w, crop_h;
	    bool		cs_ਬਦਲਿਆ;
	    bool		need_redraw;

	    // Processing changed image data failed, skip this image
	    ਜੇ (ਰੱਖੋ->img->ਅੰਕੜੇ == ਨਿਰਾ)
	    {
		image_placement_clear(ਰੱਖੋ);
		ਜਾਰੀ;
	    }

	    // Only apply row shift if image is within the shifted region.
	    ਜੇ ((ਰੱਖੋ->bounding_box.y1 >= ਖਿਸਕਾਓ_ਸਿਖਰ
			&& ਰੱਖੋ->bounding_box.y2 <= ਖਿਸਕਾਓ_bot)
		    && ਰੱਖੋ->ਪੰਕਤੀ_off != ਖਿਸਕਾਓ)
	    {
		ਰੱਖੋ->ਪੰਕਤੀ_off = ਖਿਸਕਾਓ;
		ਰੱਖੋ->flags |= IMAGEPF_DIRTY;
	    }

	    x = ਰੱਖੋ->ਖਾਨਾ;
	    y = ਰੱਖੋ->ਪੰਕਤੀ + ਰੱਖੋ->ਪੰਕਤੀ_off;

	    crop_w = ਰੱਖੋ->crop_box.x2 - ਰੱਖੋ->crop_box.x1;
	    crop_h = ਰੱਖੋ->crop_box.y2 - ਰੱਖੋ->crop_box.y1;

	    // Don't add "crop_box" x1 and y1, because "row" and "col" use the
	    // top left of the final cropped image.
	    pixman_region32_init_rect(&image_region, x, y, crop_w, crop_h);

	    pixman_region32_init(&visible_region);
	    pixman_region32_init(&visible_abs);

	    // Only use parts of the image region within the actual screen.
	    // "row_off" may offset the image slightly/completely off screen.
	    ਜੇ (!pixman_region32_intersect_rect(&image_region, &image_region,
			0, 0, Columns, ਪੰਕਤੀਆਂ)
		    || !pixman_region32_ਘਟਾਓ(&visible_region,
			&image_region, &ਘਟਾਓ_region)
		    || !pixman_region32_copy(&visible_abs, &visible_region))
	    {
		pixman_region32_fini(&image_region);
		pixman_region32_fini(&visible_region);
		ਜਾਰੀ;
	    }

	    pixman_region32_fini(&image_region);

	    // The visible region is in absolute coordinates, must convert it
	    // into image relative coordinates.
	    pixman_region32_translate(&visible_region, -x, -y);

	    cs_ਬਦਲਿਆ = ਰੱਖੋ->cell_ਚੌੜਾਈ != cell_ਚੌੜਾਈ
		|| ਰੱਖੋ->cell_ਉਚਾਈ != cell_ਉਚਾਈ;

	    // If cell size changed, then the visible regions are invalid pixel
	    // wise. This prevents backends such as sixel from using cached
	    // sequences.
	    ਜੇ (cs_ਬਦਲਿਆ)
	    {
		ਰੱਖੋ->cell_ਚੌੜਾਈ = cell_ਚੌੜਾਈ;
		ਰੱਖੋ->cell_ਉਚਾਈ = cell_ਉਚਾਈ;
		ਰੱਖੋ->flags |= IMAGEPF_FORCE;
	    }

	    // Only redraw the image if it has changed (or if we haven't drawn
	    // it yet). Or if image data has changed or cell size has changed
	    need_redraw = ਰੱਖੋ->flags & IMAGEPF_DIRTY || cs_ਬਦਲਿਆ
		||  (ਰੱਖੋ->flags & IMAGEPF_VISIBLE_INIT
			&& !pixman_region32_equal(&visible_region,
			    &ਰੱਖੋ->visible));

	    ਜੇ (ਰੱਖੋ->img_ver != img->ver)
	    {
		// Image data changed
		need_redraw = true;
		ਰੱਖੋ->img_ver = img->ver;

		image_placement_clear(ਰੱਖੋ);
		PLACEMENT_FUNC(image_backend, uninit)(ਰੱਖੋ);
		ਜੇ (PLACEMENT_FUNC(image_backend, init)(ਰੱਖੋ) == FAIL)
		{
		    emsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ));
		    image_unref(ਰੱਖੋ->img);
		    ਰੱਖੋ->img = ਨਿਰਾ;
		    ਜਾਰੀ;
		}
	    }

	    ਜੇ (need_redraw)
	    {
		ਜੇ (ਰੱਖੋ->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&ਰੱਖੋ->visible);
		ਰੱਖੋ->visible = visible_region;

		ਜੇ (PLACEMENT_FUNC(image_backend, blit))
		{
		    // Must redraw the stale regions that will not be composited
		    // over (for this specific image).
		    pixman_region32_t stale_region;

		    pixman_region32_init(&stale_region);

		    ਜੇ (ਰੱਖੋ->flags & IMAGEPF_VISIBLE_INIT)
		    {
			(ਖਾਲੀ)pixman_region32_ਘਟਾਓ(&stale_region,
				&ਰੱਖੋ->visible_abs, &visible_abs);

			// Since pixels may be translucent/transparent, we must
			// redraw the cells behind the image every time, so that
			// no stale pixels remain from the previous redraw.
			ਜੇ (img->fmt == IMAGE_FORMAT_RGBA)
			    (ਖਾਲੀ)pixman_region32_union(&stale_region,
				    &stale_region, &ਰੱਖੋ->visible_abs);
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
		    ਜੇ (img->fmt != IMAGE_FORMAT_RGBA)
		    {
			ਪੂਰਨ		    min_w, min_h;
			ਪੂਰਨ		    rect_w, rect_h;
			pixman_region32_t   min_region;

			image_get_cell_dimensions_min(img, &min_w, &min_h);

			// Ensure "min_region" only covers the fully covered
			// cells of the cropped region.
			rect_w = min_w - ਰੱਖੋ->crop_box.x1;
			rect_h = min_h - ਰੱਖੋ->crop_box.y1;

			ਜੇ (rect_w > 0 && rect_h > 0)
			    // pixman_region32_init_rect() expects a non zero
			    // width and height (otherwise it outputs error
			    // message).
			    pixman_region32_init_rect(&min_region,
				    x, y, rect_w, rect_h);
			ਨਹੀਂਤਾਂ
			    pixman_region32_init(&min_region);

			(ਖਾਲੀ)pixman_region32_ਘਟਾਓ(&min_region,
				&visible_abs, &min_region);
			(ਖਾਲੀ)pixman_region32_union(&stale_region,
				&stale_region, &min_region);
			pixman_region32_fini(&min_region);
		    }

		    ਜੇ (ਰੱਖੋ->flags & IMAGEPF_VISIBLE_INIT)
			// Also subtract "subtract_region", so we don't
			// redundantly redraw cells that will have images
			// painted over them after. Make sure to do this before
			// we check partially covered cells, since
			// "subtract_region" may also have partially covered
			// cells.
			(ਖਾਲੀ)pixman_region32_ਘਟਾਓ(&stale_region,
				&stale_region, &ਘਟਾਓ_region);

		    // Don't redraw now, defer it later
		    pixman_region32_union(&dirty_region,
			    &dirty_region, &stale_region);
		    pixman_region32_fini(&stale_region);
		}
		ਜੇ (ਰੱਖੋ->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&ਰੱਖੋ->visible_abs);
		ਰੱਖੋ->visible_abs = visible_abs;

		pending_placements[pending_len++] = ਰੱਖੋ;
		ਰੱਖੋ->flags |= IMAGEPF_VISIBLE_INIT;
		ਰੱਖੋ->flags &= ~IMAGEPF_DIRTY;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		pixman_region32_fini(&visible_region);
		pixman_region32_fini(&visible_abs);
	    }
	}

	// If this is last image, no need to union its bounds.
	ਜੇ (ਰੱਖੋ->ਅਗਲਾ == ਨਿਰਾ)
	    ਰੋਕੋ;

	pixman_region32_union_rect(&ਘਟਾਓ_region, &ਘਟਾਓ_region,
		ਰੱਖੋ->bounding_box.x1,
		ਰੱਖੋ->bounding_box.y1 + ਰੱਖੋ->ਪੰਕਤੀ_off,
		ਰੱਖੋ->bounding_box.x2 - ਰੱਖੋ->bounding_box.x1,
		ਰੱਖੋ->bounding_box.y2 - ਰੱਖੋ->bounding_box.y1);
    }

    // Don't want to dirty images again redundantly
    freeze_mark_dirty++;
    redraw_region(&dirty_region, true, false);
    freeze_mark_dirty--;

    ਜੇ (pending_len > 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	// Make sure to flush any output! We write directly to the draw
	// area/whatever, meaning draw commands outputted before this (as escape
	// sequences), will still draw over the image surface if we don't flush
	// now.
	ਜੇ (gui.ਵਿੱਚ_use)
	    ਬਾਹਰਲਾ_flush();
#ਜੇ_ਅੰਤ

	ਲਈ (ਪੂਰਨ i = 0; i < pending_len; i++)
	{
	    PLACEMENT_FUNC(image_backend, draw)(pending_placements[i]);
	    pending_placements[i]->flags &= ~IMAGEPF_FORCE;
	}
    }

    // Restore cursor position, however do not turn on the cursor, since that
    // causes it to flicker at the wrong position when using sixel. It will be
    // turned on later somewhere...
    windgoto(cur_ਪੰਕਤੀ, cur_ਖਾਨਾ);
    ਬਾਹਰਲਾ_flush();

    vim_free(pending_placements);
    pixman_region32_fini(&ਘਟਾਓ_region);
    pixman_region32_fini(&dirty_region);
}

/*
 * Mark any images that touches the region dirty. Only used for backends that
 * blit pixels.
 */
    ਖਾਲੀ
mark_dirty_region_for_images(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ ਪੰਕਤੀ_ਉਚਾਈ, ਪੂਰਨ ਖਾਨਾ_ਚੌੜਾਈ)
{
    image_placement_T	*ਰੱਖੋ;
    pixman_box32_t	rect;

    ਜੇ (freeze_mark_dirty > 0 || !backend_ਉਪਲਬਧ(false)
	    || !PLACEMENT_FUNC(image_backend, blit) || n_placements == 0)
	ਵਾਪਸ;

    rect.x1 = ਖਾਨਾ;
    rect.y1 = ਪੰਕਤੀ;
    rect.x2 = ਖਾਨਾ + ਖਾਨਾ_ਚੌੜਾਈ;
    rect.y2 = ਪੰਕਤੀ + ਪੰਕਤੀ_ਉਚਾਈ;

    FOR_ALL_PLACEMENTS(ਰੱਖੋ)
    {
	ਜੇ (ਰੱਖੋ->flags & IMAGEPF_HIDDEN)
	    ਜਾਰੀ;
	ਜੇ (pixman_region32_ਸ਼ਾਮਲ_ਹੈ_rectangle(&ਰੱਖੋ->visible_abs, &rect)
		== PIXMAN_REGION_ਬਾਹਰਲਾ)
	    ਜਾਰੀ;

	ਰੱਖੋ->flags |= IMAGEPF_DIRTY;
    }
}

    ਖਾਲੀ
clear_all_image_placements(ਖਾਲੀ)
{
    image_placement_T *ਰੱਖੋ;

    FOR_ALL_PLACEMENTS(ਰੱਖੋ)
	image_placement_clear(ਰੱਖੋ);
    ਜੇ (!updating_screen)
	redraw_all_later(UPD_VALID);
}

/*
 * Used for screen_ins_lines() and screen_del_lines(), because they directly
 * shift rows. Note that this does not queue a screen redraw.
 */
    ਖਾਲੀ
ਖਿਸਕਾਓ_image_placements(ਪੂਰਨ ਸਿਖਰ, ਪੂਰਨ bot, ਪੂਰਨ amount)
{
    ਖਿਸਕਾਓ_ਸਿਖਰ = ਸਿਖਰ;
    ਖਿਸਕਾਓ_bot = bot;
    ਖਿਸਕਾਓ += amount;
}

/*
 * Called when screen is cleared, as update_screen() may go to clearing the
 * screen instead of inserting lines using screen_ins_lines().
 */
    ਖਾਲੀ
ਖਿਸਕਾਓ_reset_image_placements(ਖਾਲੀ)
{
    ਖਿਸਕਾਓ = 0;
}

    ਸਥਿਰ image_T *
ਲੱਭੋ_image(int_u id)
{
    image_T *img;

    FOR_ALL_IMAGES(img)
	ਜੇ (img->state == IMAGE_STATE_ਜਨਤਕ && img->id == id)
	    ਵਾਪਸ img;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Add an image using the given information in "dict". If "existing" is not
 * NULL, try updating it instead. If "find" is TRUE, then accept the "id" field.
 */
    image_T *
ਜੋੜੋ_image(dict_T *dict, image_T *existing, bool ਲੱਭੋ)
{
    dictitem_T	    *di;
    blob_T	    *ਅੰਕੜੇ;
    varnumber_T     w, h;
    varnumber_T     n_pixels;
    image_format_T  fmt;

    ਜੇ (ਲੱਭੋ && dict_has_ਕੁੰਜੀ(dict, "id"))
    {
	image_T *img = ਲੱਭੋ_image((int_u)dict_get_ਸੰਖਿਆ(dict, "id"));

	ਜੇ (img == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	ਵਾਪਸ image_ref(img);
    }

    di = dict_ਲੱਭੋ(dict, (char_u *)"data", -1);
    w = dict_get_ਸੰਖਿਆ(dict, "width");
    h = dict_get_ਸੰਖਿਆ(dict, "height");

    ਜੇ (di == ਨਿਰਾ || di->di_tv.v_type != VAR_BLOB || w <= 0 || h <= 0)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Check for overflow
    n_pixels = w * h;
    ਜੇ (w > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ || h > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ || n_pixels > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
    {
	emsg(_(e_invalid_image_dimensions));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਅੰਕੜੇ = di->di_tv.vval.v_blob;

    ਜੇ (blob_len(ਅੰਕੜੇ) == n_pixels * 3)
	fmt = IMAGE_FORMAT_RGB;
    ਨਹੀਂਤਾਂ ਜੇ (blob_len(ਅੰਕੜੇ) == n_pixels * 4)
	fmt = IMAGE_FORMAT_RGBA;
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "data",
		"data length must equal width*height*3 or width*height*4");
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (existing != ਨਿਰਾ)
    {
	ਜੇ (existing->ਚੌੜਾਈ == w && existing->ਉਚਾਈ == h
		&& existing->fmt == fmt)
	{
	    ਜੇ (image_update(existing, ਅੰਕੜੇ->bv_ga.ga_ਅੰਕੜੇ) == FAIL)
		ਵਾਪਸ ਨਿਰਾ;
	    ਵਾਪਸ existing;
	}
    }

    ਵਾਪਸ image_new(ਅੰਕੜੇ->bv_ga.ga_ਅੰਕੜੇ, w, h, fmt);
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
    ਪੂਰਨ		w, h;
    dictitem_T	*di;

    ਜੇ (img->ਅੰਕੜੇ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    dict = dict_alloc();
    ਜੇ (dict == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    blob = blob_alloc();
    ਜੇ (blob == ਨਿਰਾ)
    {
	dict_unref(dict);
	ਵਾਪਸ ਨਿਰਾ;
    }

    image_get_dimensions(img, &w, &h);
    (ਖਾਲੀ)ga_concat_ਬਾਈਟਾਂ(&blob->bv_ga, (ਅੱਖਰ *)img->ਅੰਕੜੇ,
	    (size_t)w * h * img->fmt);

    di = dictitem_alloc((char_u *)"data");

    ਜੇ (di != ਨਿਰਾ)
    {
	di->di_tv.v_type = VAR_BLOB;
	di->di_tv.vval.v_blob = blob;
    }
    ਜੇ (di == ਨਿਰਾ || dict_ਜੋੜੋ(dict, di) == FAIL)
    {
	ਜੇ (di != ਨਿਰਾ)
	    dictitem_free(di);
	dict_unref(dict);
	// No need to unref blob, dictitem_free() will do that.
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (img->state == IMAGE_STATE_ਜਨਤਕ)
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "id", img->id);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "width", img->ਚੌੜਾਈ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "height", img->ਉਚਾਈ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "alpha", img->fmt == IMAGE_FORMAT_RGBA);
    dict_ਜੋੜੋ_string(dict, "format",
	    (char_u *)(img->fmt == IMAGE_FORMAT_RGB ? "rgb" : "rgba"));

    blob->bv_refcount = 1;

    ਵਾਪਸ dict;
}

    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_imageprotocol(image_backend_T *backend)
{
    ਪੂਰਨ		    len = (ਪੂਰਨ)STRLEN(p_ipc) + 1;
    char_u	    *buf = alloc(len);
    char_u	    *p = p_ipc;
    ਪੂਰਨ		    ret = FAIL;
    image_backend_T res = IMAGE_BACKEND_NONE;
    bool	    got = false;

    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜਦੋਂ (*p != NUL)
    {
	char_u		*colon;
	regmatch_T	regmatch;
	image_backend_T prot = IMAGE_BACKEND_NONE;

	// Isolate one comma separated item.
	(ਖਾਲੀ)copy_option_part(&p, buf, len, ",");

	colon = vim_strchr(buf, ':');
	ਜੇ (colon == ਨਿਰਾ || colon == buf || colon[1] == NUL)
	    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	*colon = NUL;

	// Note: Keep this in sync with p_ipc_protocol_values.
	ਜੇ (STRCMP(colon + 1, "none") == 0)
	    prot = IMAGE_BACKEND_NONE;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(colon + 1, "kitty") == 0)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_KITTY
	    prot = IMAGE_BACKEND_KITTY;
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(colon + 1, "sixel") == 0)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_SIXEL
	    prot = IMAGE_BACKEND_SIXEL;
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	ਜੇ (got)
	    ਜਾਰੀ;

	CLEAR_ਖੇਤਰ(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);

	ਜੇ (regmatch.regprog == ਨਿਰਾ)
	    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	bool ਮੇਲ = T_NAME != ਨਿਰਾ
	    && vim_regexec(&regmatch, T_NAME, (colnr_T)0);

	vim_regfree(regmatch.regprog);
	ਜੇ (ਮੇਲ)
	{
	    // Keep going to catch any errors
	    res = prot;
	    got = true;
	}
    }

    ret = OK;
    *backend = res;
ਬਾਹਰ_ਨਿਕਲੋ:
    vim_free(buf);
    ਵਾਪਸ ret;
}

/*
 * Update the current image backend to use depending on 'imageprotocol' and if
 * GUI is being used. Returns OK on success and FAIL on failure.
 */
    ਪੂਰਨ
update_image_backend(ਖਾਲੀ)
{
    image_backend_T new;

    image_T		*img;
    image_placement_T	*ਰੱਖੋ;

    // Always process 'imageprotocol' option to check for errors
    ਜੇ (ਮੇਲ_imageprotocol(&new) == FAIL)
	ਵਾਪਸ FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	new = IMAGE_BACKEND_GUI;
#ਜੇ_ਅੰਤ

    ਜੇ (image_backend == new)
	ਵਾਪਸ OK;

    // Must uninit the backends of every image/placement, then init the new
    // backend for each.
    FOR_ALL_IMAGES(img)
    {
	ਜੇ (image_backend != IMAGE_BACKEND_NONE && img->ਅੰਕੜੇ != ਨਿਰਾ
		&& img->backend_ਅੰਕੜੇ != ਨਿਰਾ)
	    IMAGE_FUNC(image_backend, uninit)(img);
	img->backend_ਅੰਕੜੇ = ਨਿਰਾ;
	ਜੇ (new != IMAGE_BACKEND_NONE)
	{
	    ਜੇ (IMAGE_FUNC(new, init)(img) == FAIL)
		ਜਾਓ fail;
	    img->backend = new;
	}
    }
    FOR_ALL_PLACEMENTS(ਰੱਖੋ)
    {
	image_placement_clear(ਰੱਖੋ);
	ਜੇ (image_backend != IMAGE_BACKEND_NONE
		&& ਰੱਖੋ->img != ਨਿਰਾ && ਰੱਖੋ->backend_ਅੰਕੜੇ != ਨਿਰਾ)
	    PLACEMENT_FUNC(image_backend, uninit)(ਰੱਖੋ);
	ਰੱਖੋ->backend_ਅੰਕੜੇ = ਨਿਰਾ;
	ਜੇ (new != IMAGE_BACKEND_NONE && ਰੱਖੋ->img != ਨਿਰਾ)
	{
	    ਜੇ (PLACEMENT_FUNC(new, init)(ਰੱਖੋ) == FAIL)
		ਜਾਓ fail;
	    ਰੱਖੋ->backend = new;
	}
    }

    redraw_all_later(UPD_VALID);
    image_backend = new;

    ਜੇ (new == IMAGE_BACKEND_NONE)
	ਸੈੱਟ_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    ਨਹੀਂਤਾਂ
	ਸੈੱਟ_vim_var_string(VV_IMAGEBACKEND,
		(char_u *)image_backends[image_backend].name, -1);

    ਵਾਪਸ OK;
fail:
    FOR_ALL_IMAGES(img)
	ਜੇ (img->backend != IMAGE_BACKEND_NONE)
	{
	    IMAGE_FUNC(img->backend, uninit)(img);
	    img->backend = IMAGE_BACKEND_NONE;
	}

    FOR_ALL_PLACEMENTS(ਰੱਖੋ)
    {
	ਜੇ (ਰੱਖੋ->img != ਨਿਰਾ && ਰੱਖੋ->backend != IMAGE_BACKEND_NONE)
	{
	    image_placement_clear(ਰੱਖੋ);
	    PLACEMENT_FUNC(ਰੱਖੋ->backend, uninit)(ਰੱਖੋ);
	}
	ਰੱਖੋ->backend = IMAGE_BACKEND_NONE;
    }

    redraw_all_later(UPD_VALID);

    image_backend = IMAGE_BACKEND_NONE;
    ਸੈੱਟ_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    emsg(_(e_changing_image_backend_failed));
    ਵਾਪਸ FAIL;
}

    ਖਾਲੀ
f_image_ਜੋੜੋ(typval_T *argvars, typval_T *rettv)
{
    dict_T  *dict;
    image_T *img;

    ਜੇ (check_for_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    dict = argvars[0].vval.v_dict;

    img = ਜੋੜੋ_image(dict, ਨਿਰਾ, false);
    ਜੇ (img != ਨਿਰਾ)
	img->state = IMAGE_STATE_ਜਨਤਕ;

    rettv->v_type = VAR_ਸੰਖਿਆ;
    rettv->vval.v_ਸੰਖਿਆ = img == ਨਿਰਾ ? (varnumber_T)-1 : img->id;
}

    ਖਾਲੀ
f_image_discard(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    image_T *img;

    ਜੇ (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    img = ਲੱਭੋ_image((int_u)argvars[0].vval.v_ਸੰਖਿਆ);

    ਜੇ (img != ਨਿਰਾ)
    {
	img->state = IMAGE_STATE_ਨਿੱਜੀ;
	image_unref(img);
    }
    ਨਹੀਂਤਾਂ
	semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_ਸੰਖਿਆ);
}

    ਖਾਲੀ
f_image_info(typval_T *argvars, typval_T *rettv)
{
    image_T *img;
    dict_T  *dict;

    ਜੇ (rettv_list_alloc(rettv) == FAIL
	    || check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_ਸੰਖਿਆ)
    {
	img = ਲੱਭੋ_image((int_u)argvars[0].vval.v_ਸੰਖਿਆ);
	ਜੇ (img == ਨਿਰਾ)
	{
	    semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_ਸੰਖਿਆ);
	    ਵਾਪਸ;
	}
	dict = get_image_info(img);
	ਜੇ (dict != ਨਿਰਾ && list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	ਵਾਪਸ;
    }

    FOR_ALL_IMAGES(img)
    {
	ਜੇ (img->state == IMAGE_STATE_ਨਿੱਜੀ)
	    ਜਾਰੀ;
	dict = get_image_info(img);
	ਜੇ (dict == ਨਿਰਾ
		|| list_append_dict(rettv->vval.v_list, dict) == FAIL)
	{
	    dict_unref(dict);	// NULL-safe
	    ਵਾਪਸ;
	}
    }
}

#ਜੇ_ਅੰਤ // FEAT_IMAGE
