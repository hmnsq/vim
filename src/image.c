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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_IMAGE)

ರಚನೆ
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;

    ರಚನೆ
    {
	// Return OK on success and FAIL on failure
	ಪೂರ್ಣಾಂಕ (*init)(image_T *);
	ಶೂನ್ಯ (*uninit)(image_T *);
    } image;

    ರಚನೆ
    {
	// Return OK on success and FAIL on failure
	ಪೂರ್ಣಾಂಕ (*init)(image_placement_T *);
	ಶೂನ್ಯ (*uninit)(image_placement_T *);

	ಶೂನ್ಯ (*draw)(image_placement_T *);
	ಶೂನ್ಯ (*clear)(image_placement_T *);

	// If backend blits the image pixels to the screen. Images are not
	// treated as "objects".
	bool blit;
    } placement;
} image_backends[] = {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_GUI
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
#  ಇಲ್ಲದಿದ್ದರೆ
	    .blit = true,
#  endif
# elif FEAT_GUI_MSWIN
	    .blit = true,
# ಇಲ್ಲದಿದ್ದರೆ
	    .blit = false,
# endif
	}
    },
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_KITTY
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
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_SIXEL
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
#ಆದರೆ_ಅಂತ್ಯ
};

// Current image backend being used
ಸ್ಥಿರ image_backend_T image_backend = IMAGE_BACKEND_NONE;

ಸ್ಥಿರ image_T *images = ಖಾಲಿ;
// Sorted from highest zindex to lowest zindex
ಸ್ಥಿರ image_placement_T    *placements = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		    n_placements = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸರಿಸು_ತುದಿ = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸರಿಸು_bot = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸರಿಸು = 0;

// If > 0, then mark_dirty_region_for_images() will be a no-op
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ freeze_mark_dirty = 0;

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_IMAGES(v) ಗಾಗಿ ((v) = images; (v) != ಖಾಲಿ; (v) = (v)->ಮುಂದಿನ)
#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_PLACEMENTS(v) \
    ಗಾಗಿ ((v) = placements; (v) != ಖಾಲಿ; (v) = (v)->ಮುಂದಿನ)

#ವ್ಯಾಖ್ಯಾನಿಸು IMAGE_FUNC(b, n) (image_backends[b].image.n)
#ವ್ಯಾಖ್ಯಾನಿಸು PLACEMENT_FUNC(b, n) (image_backends[b].placement.n)

ಸ್ಥಿರ ಶೂನ್ಯ redraw_region(pixman_region32_t *region, bool now, bool restore);

    ಶೂನ್ಯ
init_image_state(ಶೂನ್ಯ)
{
    (ಶೂನ್ಯ)update_image_backend();
}

    ಶೂನ್ಯ
uninit_image_state(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_SIXEL
    sixel_uninit();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return true if the there image backend is ready, otherwise emit error.
 */
    ಸ್ಥಿರ bool
backend_ಲಭ್ಯ(bool ಸಂದೇಶ)
{
    ಆದರೆ (image_backend == IMAGE_BACKEND_NONE)
    {
	ಆದರೆ (ಸಂದೇಶ)
	    emsg(_(e_no_image_backend_ಲಭ್ಯ));
	ಹಿಂತಿರುಗಿ false;
    }
    ಹಿಂತಿರುಗಿ true;
}

/*
 * Create a new image using the given data (creates a copy). Returns NULL on
 * failure.
 */
    image_T *
image_new(uint8_t *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ, image_format_T fmt)
{
    image_T	    *img;
    ಸ್ಥಿರ int_u    id;

    ಆದರೆ (!backend_ಲಭ್ಯ(true))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    img = ALLOC_CLEAR_ONE(image_T);
    ಆದರೆ (img == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    img->ದತ್ತಾಂಶ = vim_memsave(ದತ್ತಾಂಶ, (size_t)ಅಗಲ * ಎತ್ತರ * fmt);
    ಆದರೆ (img->ದತ್ತಾಂಶ == ಖಾಲಿ)
    {
	vim_free(img);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    img->state = IMAGE_STATE_ಖಾಸಗಿ;
    img->ಅಗಲ = ಅಗಲ;
    img->ಎತ್ತರ = ಎತ್ತರ;
    img->fmt = fmt;

    // Mix in PID to prevent ID collisions when using kitty graphics protocol
    img->id = ((((int_u)mch_get_pid() & 0x7fff) + 1) << 16) | (id++ & 0xffff);
    img->refcount = 1;

    img->backend = image_backend;

    ಆದರೆ (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	vim_free(img->ದತ್ತಾಂಶ);
	vim_free(img);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (images != ಖಾಲಿ)
	images->prev = img;
    img->ಮುಂದಿನ = images;
    images = img;

    ಹಿಂತಿರುಗಿ img;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
image_free(image_T *img)
{
    ಆದರೆ (backend_ಲಭ್ಯ(false))
	IMAGE_FUNC(image_backend, uninit)(img);

    ಆದರೆ (img->prev != ಖಾಲಿ)
	img->prev->ಮುಂದಿನ = img->ಮುಂದಿನ;
    ಆದರೆ (img->ಮುಂದಿನ != ಖಾಲಿ)
	img->ಮುಂದಿನ->prev = img->prev;
    ಆದರೆ (images == img)
	images = img->ಮುಂದಿನ;

    vim_free(img->ದತ್ತಾಂಶ);
    vim_free(img);
}

/*
 * Decrement reference count of an image. Be careful to not let the public API
 * be able to directly decrement the refcount of an image multiple times!
 */
    ಶೂನ್ಯ
image_unref(image_T *img)
{
    ಆದರೆ (--img->refcount <= 0)
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
    ಹಿಂತಿರುಗಿ img;
}

/*
 * Get image width and height in cells, rounding up so that every pixel is
 * covered.
 */
    ಶೂನ್ಯ
image_get_cell_dimensions(image_T *img, ಪೂರ್ಣಾಂಕ *cw, ಪೂರ್ಣಾಂಕ *ch)
{
    ಪೂರ್ಣಾಂಕ w = PHY2LOG(img->ಅಗಲ);
    ಪೂರ್ಣಾಂಕ h = PHY2LOG(img->ಎತ್ತರ);

    *cw = (w + cell_ಅಗಲ - 1) / cell_ಅಗಲ;
    *ch = (h + cell_ಎತ್ತರ - 1) / cell_ಎತ್ತರ;
}

/*
 * Similar, but return the dimensions of all cells that are *fully* covered by
 * pixels. Pixels that are partially covered are not counted.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
image_get_cell_dimensions_min(image_T *img, ಪೂರ್ಣಾಂಕ *cw, ಪೂರ್ಣಾಂಕ *ch)
{
    ಪೂರ್ಣಾಂಕ w = PHY2LOG(img->ಅಗಲ);
    ಪೂರ್ಣಾಂಕ h = PHY2LOG(img->ಎತ್ತರ);

    *cw = w / cell_ಅಗಲ;
    *ch = h / cell_ಎತ್ತರ;
}

    ಶೂನ್ಯ
image_get_dimensions(image_T *img, ಪೂರ್ಣಾಂಕ *w, ಪೂರ್ಣಾಂಕ *h)
{
    *w = img->ಅಗಲ;
    *h = img->ಎತ್ತರ;
}

/*
 * Replace the current image contents with "data". "data" should be the exact
 * same format, width, and height. Return OK on success and FAIL on failure.
 */
    ಪೂರ್ಣಾಂಕ
image_update(image_T *img, uint8_t *ದತ್ತಾಂಶ)
{
    ಪೂರ್ಣಾಂಕ w, h;

    ಆದರೆ (!backend_ಲಭ್ಯ(true) || img->ದತ್ತಾಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    image_get_dimensions(img, &w, &h);
    mch_memmove(img->ದತ್ತಾಂಶ, ದತ್ತಾಂಶ, (size_t)w * h * img->fmt);

    IMAGE_FUNC(image_backend, uninit)(img);

    ಆದರೆ (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	emsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ));
	VIM_CLEAR(img->ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    img->ver++;
    redraw_all_later(UPD_VALID);
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
image_placement_unlink(image_placement_T *ಇರಿಸು)
{
    ಆದರೆ (ಇರಿಸು->prev != ಖಾಲಿ)
	ಇರಿಸು->prev->ಮುಂದಿನ = ಇರಿಸು->ಮುಂದಿನ;
    ಆದರೆ (ಇರಿಸು->ಮುಂದಿನ != ಖಾಲಿ)
	ಇರಿಸು->ಮುಂದಿನ->prev = ಇರಿಸು->prev;
    ಆದರೆ (placements == ಇರಿಸು)
	placements = ಇರಿಸು->ಮುಂದಿನ;
    n_placements--;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
image_placement_link(image_placement_T *ಇರಿಸು)
{
    image_placement_T *p = placements;
    image_placement_T *prev = ಖಾಲಿ;

    // Add placement *before* the placement with the lower zindex.
    ಆಗ (p != ಖಾಲಿ)
    {
	ಆದರೆ (ಇರಿಸು->zindex > p->zindex)
	    ನಿಲ್ಲಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಇರಿಸು->zindex == p->zindex)
	{
	    ಆದರೆ (ಇರಿಸು->use > p->use)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (ಇರಿಸು->gen < p->gen)
		ನಿಲ್ಲಿಸು;
	}

	prev = p;
	p = p->ಮುಂದಿನ;
    }

    ಇರಿಸು->ಮುಂದಿನ = p;
    ಇರಿಸು->prev = prev;

    ಆದರೆ (p != ಖಾಲಿ)
	p->prev = ಇರಿಸು;

    ಆದರೆ (prev != ಖಾಲಿ)
	prev->ಮುಂದಿನ = ಇರಿಸು;
    ಇಲ್ಲದಿದ್ದರೆ
	placements = ಇರಿಸು;

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
    image_placement_T	*ಇರಿಸು;
    ಸ್ಥಿರ int_u	id = 1; // Kitty placements id must be > 1

    ಆದರೆ (!backend_ಲಭ್ಯ(!quiet))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಇರಿಸು = ALLOC_CLEAR_ONE(image_placement_T);
    ಆದರೆ (ಇರಿಸು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಇರಿಸು->id = id;
    id += PLACEMENT_ID_INC; // Allocate 1000 free placement ids to be used to
			    // draw this image placement. Only relevant for
			    // kitty graphics protocol.
    ಇರಿಸು->gen = gen;
    ಇರಿಸು->use = use;

    ಇರಿಸು->img = img;
    ಇರಿಸು->flags = IMAGEPF_DIRTY;

    ಆದರೆ (img != ಖಾಲಿ)
    {
	ಇರಿಸು->img_ver = img->ver;
	ಇರಿಸು->backend = image_backend;
	ಇರಿಸು->cell_ಅಗಲ = cell_ಅಗಲ;
	ಇರಿಸು->cell_ಎತ್ತರ = cell_ಎತ್ತರ;

	// Bounding box is in cells
	image_get_cell_dimensions(img,
		&ಇರಿಸು->bounding_box.x2, &ಇರಿಸು->bounding_box.y2);
	ಇರಿಸು->crop_box.x2 = ಇರಿಸು->bounding_box.x2;
	ಇರಿಸು->crop_box.y2 = ಇರಿಸು->bounding_box.y2;

	ಆದರೆ (PLACEMENT_FUNC(image_backend, init)(ಇರಿಸು) == FAIL)
	{
	    id -= PLACEMENT_ID_INC;
	    vim_free(ಇರಿಸು);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಇರಿಸು->backend = IMAGE_BACKEND_NONE;

    image_placement_link(ಇರಿಸು);

    ಹಿಂತಿರುಗಿ ಇರಿಸು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
image_placement_clear_int(image_placement_T *ಇರಿಸು, bool now)
{
    ಆದರೆ (backend_ಲಭ್ಯ(false) && ಇರಿಸು->img != ಖಾಲಿ)
    {
	ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_VISIBLE_INIT
		&& PLACEMENT_FUNC(image_backend, blit))
	    redraw_region(&ಇರಿಸು->visible_abs, now, true);
	PLACEMENT_FUNC(image_backend, clear)(ಇರಿಸು);
	ಇರಿಸು->flags |= IMAGEPF_HIDDEN;
	ಆದರೆ (!now && !updating_screen)
	    redraw_all_later(UPD_VALID);
    }
}

    ಶೂನ್ಯ
image_placement_clear(image_placement_T *ಇರಿಸು)
{
    image_placement_clear_int(ಇರಿಸು, updating_screen);
}

    ಶೂನ್ಯ
image_placement_free(image_placement_T *ಇರಿಸು)
{
    ಆದರೆ (backend_ಲಭ್ಯ(false) && ಇರಿಸು->backend_ದತ್ತಾಂಶ != ಖಾಲಿ)
    {
	image_placement_clear(ಇರಿಸು);
	PLACEMENT_FUNC(image_backend, uninit)(ಇರಿಸು);
    }

    ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_VISIBLE_INIT)
    {
	pixman_region32_fini(&ಇರಿಸು->visible);
	pixman_region32_fini(&ಇರಿಸು->visible_abs);
    }
    image_placement_unlink(ಇರಿಸು);

    ಆದರೆ (ಇರಿಸು->img != ಖಾಲಿ)
	image_unref(ಇರಿಸು->img);
    vim_free(ಇರಿಸು);
}

    ಶೂನ್ಯ
image_placement_ಗಣ_zindex(image_placement_T *ಇರಿಸು, ಪೂರ್ಣಾಂಕ zindex)
{
    // If image was hidden before, must dirty it so it is fully redrawn again.
    ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_HIDDEN)
	ಇರಿಸು->flags |= IMAGEPF_DIRTY;
    ಇರಿಸು->flags &= ~IMAGEPF_HIDDEN;

    ಆದರೆ (ಇರಿಸು->zindex == zindex)
	ಹಿಂತಿರುಗಿ;
    ಇರಿಸು->zindex = zindex;
    ಇರಿಸು->flags |= IMAGEPF_DIRTY;

    // Must re-add the placement back so it is in the correct order
    image_placement_unlink(ಇರಿಸು);
    image_placement_link(ಇರಿಸು);
}

/*
 * Make sure to also update the bounding box!
 */
    ಶೂನ್ಯ
image_placement_ಗಣ_position(image_placement_T *ಇರಿಸು, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_HIDDEN)
	ಇರಿಸು->flags |= IMAGEPF_DIRTY;
    ಇರಿಸು->flags &= ~IMAGEPF_HIDDEN;

    ಆದರೆ (ಇರಿಸು->ಅಡ್ಡಸಾಲು == ಅಡ್ಡಸಾಲು && ಇರಿಸು->ಲಂಬಸಾಲು == ಲಂಬಸಾಲು)
	ಹಿಂತಿರುಗಿ;
    ಇರಿಸು->ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
    ಇರಿಸು->ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    ಇರಿಸು->flags |= IMAGEPF_DIRTY;
}

    ಶೂನ್ಯ
image_placement_ಗಣ_crop(image_placement_T *ಇರಿಸು, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_HIDDEN)
	ಇರಿಸು->flags |= IMAGEPF_DIRTY;
    ಇರಿಸು->flags &= ~IMAGEPF_HIDDEN;

    ಆದರೆ (ಇರಿಸು->crop_box.x1 == x && ಇರಿಸು->crop_box.y1 == y
	    && ಇರಿಸು->crop_box.x2 == x + w && ಇರಿಸು->crop_box.y2 == y + h)
	ಹಿಂತಿರುಗಿ;

    ಇರಿಸು->crop_box.x1 = x;
    ಇರಿಸು->crop_box.y1 = y;

    ಇರಿಸು->crop_box.x2 = x + w;
    ಇರಿಸು->crop_box.y2 = y + h;
    ಇರಿಸು->flags |= IMAGEPF_DIRTY;
}

    ಶೂನ್ಯ
image_placement_ಗಣ_bounding_box(
	image_placement_T   *ಇರಿಸು,
	ಪೂರ್ಣಾಂಕ		    ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ		    ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ		    ಅಡ್ಡಸಾಲು_ಎತ್ತರ,
	ಪೂರ್ಣಾಂಕ		    ಲಂಬಸಾಲು_ಅಗಲ)
{
    ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_HIDDEN)
	ಇರಿಸು->flags |= IMAGEPF_DIRTY;
    ಇರಿಸು->flags &= ~IMAGEPF_HIDDEN;

    ಆದರೆ (ಇರಿಸು->bounding_box.x1 == ಲಂಬಸಾಲು && ಇರಿಸು->bounding_box.y1 == ಅಡ್ಡಸಾಲು
	    && ಇರಿಸು->bounding_box.x2 == ಲಂಬಸಾಲು + ಲಂಬಸಾಲು_ಅಗಲ
	    && ಇರಿಸು->bounding_box.y2 == ಅಡ್ಡಸಾಲು + ಅಡ್ಡಸಾಲು_ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ;

    ಇರಿಸು->bounding_box.x1 = ಲಂಬಸಾಲು;
    ಇರಿಸು->bounding_box.y1 = ಅಡ್ಡಸಾಲು;

    ಇರಿಸು->bounding_box.x2 = ಲಂಬಸಾಲು + ಲಂಬಸಾಲು_ಅಗಲ;
    ಇರಿಸು->bounding_box.y2 = ಅಡ್ಡಸಾಲು + ಅಡ್ಡಸಾಲು_ಎತ್ತರ;
    ಇರಿಸು->flags |= IMAGEPF_DIRTY;
}

/*
 * Enable or disable the image placement. Unlike IMAGEPF_HIDDEN, this is not
 * reset across redraws.
 */
    ಶೂನ್ಯ
image_placement_ಗಣ_enable(image_placement_T *ಇರಿಸು, bool enable)
{
    bool old = ಇರಿಸು->flags & IMAGEPF_ENABLED;

    ಆದರೆ (enable && !old)
	ಇರಿಸು->flags |= IMAGEPF_ENABLED;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!enable && old)
    {
	ಇರಿಸು->flags &= ~IMAGEPF_ENABLED;
	image_placement_clear(ಇರಿಸು);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ;

    ಇರಿಸು->flags |= IMAGEPF_DIRTY;
}

/*
 * Note that the returned crop region (x, y, w, h) is in physical pixels if
 * "physical" is true. Not relevant when using terminal protocols (sixel, KGP).
 */
    ಶೂನ್ಯ
image_placement_subrect(
	image_placement_T   *ಇರಿಸು,
	pixman_box32_t	    rect,
	ಪೂರ್ಣಾಂಕ		    *ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ		    *ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ		    *x,
	ಪೂರ್ಣಾಂಕ		    *y,
	ಪೂರ್ಣಾಂಕ		    *w,
	ಪೂರ್ಣಾಂಕ		    *h,
	bool		    physical)
{
    ಪೂರ್ಣಾಂಕ iw, ih;

    image_get_dimensions(ಇರಿಸು->img, &iw, &ih);

    *ಅಡ್ಡಸಾಲು = ಇರಿಸು->ಅಡ್ಡಸಾಲು + rect.y1 + ಇರಿಸು->ಅಡ್ಡಸಾಲು_off;
    *ಲಂಬಸಾಲು = ಇರಿಸು->ಲಂಬಸಾಲು + rect.x1;

    *x = (rect.x1 + ಇರಿಸು->crop_box.x1) * cell_ಅಗಲ;
    *y = (rect.y1 + ಇರಿಸು->crop_box.y1) * cell_ಎತ್ತರ;
    *w = (rect.x2 - rect.x1) * cell_ಅಗಲ;
    *h = (rect.y2 - rect.y1) * cell_ಎತ್ತರ;

    ಆದರೆ (physical)
    {
	*x = LOG2PHY(*x);
	*y = LOG2PHY(*y);
	*w = LOG2PHY(*w);
	*h = LOG2PHY(*h);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	iw = PHY2LOG(iw);
	ih = PHY2LOG(ih);
    }

    // Make that all the values are valid. "w" and "h" can especially be
    // invalid, because the image dimensions (in pixels) are converted to cells,
    // rounding *up*.
    *ಅಡ್ಡಸಾಲು = MIN(*ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲುಗಳು);
    *ಲಂಬಸಾಲು = MIN(*ಲಂಬಸಾಲು, Columns);

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
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_region(pixman_region32_t *region, bool now, bool restore)
{
    pixman_box32_t  *rects;
    ಪೂರ್ಣಾಂಕ		    n_rects;
    ಪೂರ್ಣಾಂಕ		    cur_ಅಡ್ಡಸಾಲು = screen_cur_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		    cur_ಲಂಬಸಾಲು = screen_cur_ಲಂಬಸಾಲು;

    rects = pixman_region32_rectangles(region, &n_rects);
    ಆದರೆ (rects == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_rects; i++)
    {
	pixman_box32_t rect = rects[i];

	ಆದರೆ (now)
	{
	    // The region may be from before a resize, when the screen was
	    // bigger.
	    ಪೂರ್ಣಾಂಕ ಎತ್ತರ = MIN(rect.y2, screen_ಅಡ್ಡಸಾಲುಗಳು) - rect.y1;
	    ಪೂರ್ಣಾಂಕ ಅಗಲ = MIN(rect.x2, screen_Columns) - rect.x1;

	    // Tip: if you want to debug stale pixels appearing, set "inverse"
	    // of screen_draw_rectangle() to TRUE.
	    ಆದರೆ (ಎತ್ತರ > 0 && ಅಗಲ > 0)
		screen_draw_rectangle(rect.y1, rect.x1, ಎತ್ತರ, ಅಗಲ,
			FALSE, TRUE);
	    ಮುಂದುವರಿ;
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = rect.y1; r < rect.y2; r++)
	{
	    ಆದರೆ (r >= screen_ಅಡ್ಡಸಾಲುಗಳು)
		ನಿಲ್ಲಿಸು;

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ c = rect.x1; c < rect.x2; c++)
	    {
		ಆದರೆ (c >= screen_Columns)
		    ನಿಲ್ಲಿಸು;

		ScreenAttrs[ಸಾಲು_ಸ್ಥಾನಾಂತರ[r] + c] = (sattr_T)-1;
	    }
	}
    }

    ಆದರೆ (restore)
	windgoto(cur_ಅಡ್ಡಸಾಲು, cur_ಲಂಬಸಾಲು);
}

/*
 * Draw all image placements to the screen. This should be done after all text
 * have been drawn to the screen, and image placements positioned correctly.
 */
    ಶೂನ್ಯ
draw_image_placements(ಶೂನ್ಯ)
{
    image_placement_T	*ಇರಿಸು;
    pixman_region32_t	ಕಳೆ_region;    // In cells
    pixman_region32_t	dirty_region;	    // In cells
    image_placement_T	**pending_placements;
    ಪೂರ್ಣಾಂಕ			pending_len = -1;
    // Save current cursor position
    ಪೂರ್ಣಾಂಕ			cur_ಅಡ್ಡಸಾಲು = screen_cur_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ			cur_ಲಂಬಸಾಲು = screen_cur_ಲಂಬಸಾಲು;

    // Check if all placements have no backing image. If so, then do nothing
    FOR_ALL_PLACEMENTS(ಇರಿಸು)
	ಆದರೆ (ಇರಿಸು->img != ಖಾಲಿ)
	{
	    pending_len = 0;
	    ನಿಲ್ಲಿಸು;
	}

    ಆದರೆ (pending_len == -1)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!backend_ಲಭ್ಯ(false))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (n_placements == 0)
	ಹಿಂತಿರುಗಿ;

    pending_placements = ALLOC_CLEAR_MULT(image_placement_T *, n_placements);
    ಆದರೆ (pending_placements == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    pixman_region32_init(&ಕಳೆ_region);
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
    FOR_ALL_PLACEMENTS(ಇರಿಸು)
    {
	image_T *img = ಇರಿಸು->img;

	pixman_region32_t   image_region;
	pixman_region32_t   visible_region;

	ಪೂರ್ಣಾಂಕ x, y;

	ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_HIDDEN || !(ಇರಿಸು->flags & IMAGEPF_ENABLED))
	    // Don't need to clear image placement for non-blit backends,
	    // because we call image_placement_clear() every time we don't want
	    // to see the image. Not sure of the side effects if we also clear
	    // it here...
	    ಮುಂದುವರಿ;

	ಆದರೆ (img != ಖಾಲಿ)
	{
	    pixman_region32_t	visible_abs;
	    ಪೂರ್ಣಾಂಕ			crop_w, crop_h;
	    bool		cs_ಬದಲಾಯಿತು;
	    bool		need_redraw;

	    // Processing changed image data failed, skip this image
	    ಆದರೆ (ಇರಿಸು->img->ದತ್ತಾಂಶ == ಖಾಲಿ)
	    {
		image_placement_clear(ಇರಿಸು);
		ಮುಂದುವರಿ;
	    }

	    // Only apply row shift if image is within the shifted region.
	    ಆದರೆ ((ಇರಿಸು->bounding_box.y1 >= ಸರಿಸು_ತುದಿ
			&& ಇರಿಸು->bounding_box.y2 <= ಸರಿಸು_bot)
		    && ಇರಿಸು->ಅಡ್ಡಸಾಲು_off != ಸರಿಸು)
	    {
		ಇರಿಸು->ಅಡ್ಡಸಾಲು_off = ಸರಿಸು;
		ಇರಿಸು->flags |= IMAGEPF_DIRTY;
	    }

	    x = ಇರಿಸು->ಲಂಬಸಾಲು;
	    y = ಇರಿಸು->ಅಡ್ಡಸಾಲು + ಇರಿಸು->ಅಡ್ಡಸಾಲು_off;

	    crop_w = ಇರಿಸು->crop_box.x2 - ಇರಿಸು->crop_box.x1;
	    crop_h = ಇರಿಸು->crop_box.y2 - ಇರಿಸು->crop_box.y1;

	    // Don't add "crop_box" x1 and y1, because "row" and "col" use the
	    // top left of the final cropped image.
	    pixman_region32_init_rect(&image_region, x, y, crop_w, crop_h);

	    pixman_region32_init(&visible_region);
	    pixman_region32_init(&visible_abs);

	    // Only use parts of the image region within the actual screen.
	    // "row_off" may offset the image slightly/completely off screen.
	    ಆದರೆ (!pixman_region32_intersect_rect(&image_region, &image_region,
			0, 0, Columns, ಅಡ್ಡಸಾಲುಗಳು)
		    || !pixman_region32_ಕಳೆ(&visible_region,
			&image_region, &ಕಳೆ_region)
		    || !pixman_region32_copy(&visible_abs, &visible_region))
	    {
		pixman_region32_fini(&image_region);
		pixman_region32_fini(&visible_region);
		ಮುಂದುವರಿ;
	    }

	    pixman_region32_fini(&image_region);

	    // The visible region is in absolute coordinates, must convert it
	    // into image relative coordinates.
	    pixman_region32_translate(&visible_region, -x, -y);

	    cs_ಬದಲಾಯಿತು = ಇರಿಸು->cell_ಅಗಲ != cell_ಅಗಲ
		|| ಇರಿಸು->cell_ಎತ್ತರ != cell_ಎತ್ತರ;

	    // If cell size changed, then the visible regions are invalid pixel
	    // wise. This prevents backends such as sixel from using cached
	    // sequences.
	    ಆದರೆ (cs_ಬದಲಾಯಿತು)
	    {
		ಇರಿಸು->cell_ಅಗಲ = cell_ಅಗಲ;
		ಇರಿಸು->cell_ಎತ್ತರ = cell_ಎತ್ತರ;
		ಇರಿಸು->flags |= IMAGEPF_FORCE;
	    }

	    // Only redraw the image if it has changed (or if we haven't drawn
	    // it yet). Or if image data has changed or cell size has changed
	    need_redraw = ಇರಿಸು->flags & IMAGEPF_DIRTY || cs_ಬದಲಾಯಿತು
		||  (ಇರಿಸು->flags & IMAGEPF_VISIBLE_INIT
			&& !pixman_region32_equal(&visible_region,
			    &ಇರಿಸು->visible));

	    ಆದರೆ (ಇರಿಸು->img_ver != img->ver)
	    {
		// Image data changed
		need_redraw = true;
		ಇರಿಸು->img_ver = img->ver;

		image_placement_clear(ಇರಿಸು);
		PLACEMENT_FUNC(image_backend, uninit)(ಇರಿಸು);
		ಆದರೆ (PLACEMENT_FUNC(image_backend, init)(ಇರಿಸು) == FAIL)
		{
		    emsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ));
		    image_unref(ಇರಿಸು->img);
		    ಇರಿಸು->img = ಖಾಲಿ;
		    ಮುಂದುವರಿ;
		}
	    }

	    ಆದರೆ (need_redraw)
	    {
		ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&ಇರಿಸು->visible);
		ಇರಿಸು->visible = visible_region;

		ಆದರೆ (PLACEMENT_FUNC(image_backend, blit))
		{
		    // Must redraw the stale regions that will not be composited
		    // over (for this specific image).
		    pixman_region32_t stale_region;

		    pixman_region32_init(&stale_region);

		    ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_VISIBLE_INIT)
		    {
			(ಶೂನ್ಯ)pixman_region32_ಕಳೆ(&stale_region,
				&ಇರಿಸು->visible_abs, &visible_abs);

			// Since pixels may be translucent/transparent, we must
			// redraw the cells behind the image every time, so that
			// no stale pixels remain from the previous redraw.
			ಆದರೆ (img->fmt == IMAGE_FORMAT_RGBA)
			    (ಶೂನ್ಯ)pixman_region32_union(&stale_region,
				    &stale_region, &ಇರಿಸು->visible_abs);
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
		    ಆದರೆ (img->fmt != IMAGE_FORMAT_RGBA)
		    {
			ಪೂರ್ಣಾಂಕ		    min_w, min_h;
			ಪೂರ್ಣಾಂಕ		    rect_w, rect_h;
			pixman_region32_t   min_region;

			image_get_cell_dimensions_min(img, &min_w, &min_h);

			// Ensure "min_region" only covers the fully covered
			// cells of the cropped region.
			rect_w = min_w - ಇರಿಸು->crop_box.x1;
			rect_h = min_h - ಇರಿಸು->crop_box.y1;

			ಆದರೆ (rect_w > 0 && rect_h > 0)
			    // pixman_region32_init_rect() expects a non zero
			    // width and height (otherwise it outputs error
			    // message).
			    pixman_region32_init_rect(&min_region,
				    x, y, rect_w, rect_h);
			ಇಲ್ಲದಿದ್ದರೆ
			    pixman_region32_init(&min_region);

			(ಶೂನ್ಯ)pixman_region32_ಕಳೆ(&min_region,
				&visible_abs, &min_region);
			(ಶೂನ್ಯ)pixman_region32_union(&stale_region,
				&stale_region, &min_region);
			pixman_region32_fini(&min_region);
		    }

		    ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_VISIBLE_INIT)
			// Also subtract "subtract_region", so we don't
			// redundantly redraw cells that will have images
			// painted over them after. Make sure to do this before
			// we check partially covered cells, since
			// "subtract_region" may also have partially covered
			// cells.
			(ಶೂನ್ಯ)pixman_region32_ಕಳೆ(&stale_region,
				&stale_region, &ಕಳೆ_region);

		    // Don't redraw now, defer it later
		    pixman_region32_union(&dirty_region,
			    &dirty_region, &stale_region);
		    pixman_region32_fini(&stale_region);
		}
		ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&ಇರಿಸು->visible_abs);
		ಇರಿಸು->visible_abs = visible_abs;

		pending_placements[pending_len++] = ಇರಿಸು;
		ಇರಿಸು->flags |= IMAGEPF_VISIBLE_INIT;
		ಇರಿಸು->flags &= ~IMAGEPF_DIRTY;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		pixman_region32_fini(&visible_region);
		pixman_region32_fini(&visible_abs);
	    }
	}

	// If this is last image, no need to union its bounds.
	ಆದರೆ (ಇರಿಸು->ಮುಂದಿನ == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	pixman_region32_union_rect(&ಕಳೆ_region, &ಕಳೆ_region,
		ಇರಿಸು->bounding_box.x1,
		ಇರಿಸು->bounding_box.y1 + ಇರಿಸು->ಅಡ್ಡಸಾಲು_off,
		ಇರಿಸು->bounding_box.x2 - ಇರಿಸು->bounding_box.x1,
		ಇರಿಸು->bounding_box.y2 - ಇರಿಸು->bounding_box.y1);
    }

    // Don't want to dirty images again redundantly
    freeze_mark_dirty++;
    redraw_region(&dirty_region, true, false);
    freeze_mark_dirty--;

    ಆದರೆ (pending_len > 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	// Make sure to flush any output! We write directly to the draw
	// area/whatever, meaning draw commands outputted before this (as escape
	// sequences), will still draw over the image surface if we don't flush
	// now.
	ಆದರೆ (gui.ಒಳ_use)
	    ಹೊರಹೋಗು_flush();
#ಆದರೆ_ಅಂತ್ಯ

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < pending_len; i++)
	{
	    PLACEMENT_FUNC(image_backend, draw)(pending_placements[i]);
	    pending_placements[i]->flags &= ~IMAGEPF_FORCE;
	}
    }

    // Restore cursor position, however do not turn on the cursor, since that
    // causes it to flicker at the wrong position when using sixel. It will be
    // turned on later somewhere...
    windgoto(cur_ಅಡ್ಡಸಾಲು, cur_ಲಂಬಸಾಲು);
    ಹೊರಹೋಗು_flush();

    vim_free(pending_placements);
    pixman_region32_fini(&ಕಳೆ_region);
    pixman_region32_fini(&dirty_region);
}

/*
 * Mark any images that touches the region dirty. Only used for backends that
 * blit pixels.
 */
    ಶೂನ್ಯ
mark_dirty_region_for_images(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು_ಎತ್ತರ, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಅಗಲ)
{
    image_placement_T	*ಇರಿಸು;
    pixman_box32_t	rect;

    ಆದರೆ (freeze_mark_dirty > 0 || !backend_ಲಭ್ಯ(false)
	    || !PLACEMENT_FUNC(image_backend, blit) || n_placements == 0)
	ಹಿಂತಿರುಗಿ;

    rect.x1 = ಲಂಬಸಾಲು;
    rect.y1 = ಅಡ್ಡಸಾಲು;
    rect.x2 = ಲಂಬಸಾಲು + ಲಂಬಸಾಲು_ಅಗಲ;
    rect.y2 = ಅಡ್ಡಸಾಲು + ಅಡ್ಡಸಾಲು_ಎತ್ತರ;

    FOR_ALL_PLACEMENTS(ಇರಿಸು)
    {
	ಆದರೆ (ಇರಿಸು->flags & IMAGEPF_HIDDEN)
	    ಮುಂದುವರಿ;
	ಆದರೆ (pixman_region32_ಒಳಗೊಂಡಿದೆ_rectangle(&ಇರಿಸು->visible_abs, &rect)
		== PIXMAN_REGION_ಹೊರಹೋಗು)
	    ಮುಂದುವರಿ;

	ಇರಿಸು->flags |= IMAGEPF_DIRTY;
    }
}

    ಶೂನ್ಯ
clear_all_image_placements(ಶೂನ್ಯ)
{
    image_placement_T *ಇರಿಸು;

    FOR_ALL_PLACEMENTS(ಇರಿಸು)
	image_placement_clear(ಇರಿಸು);
    ಆದರೆ (!updating_screen)
	redraw_all_later(UPD_VALID);
}

/*
 * Used for screen_ins_lines() and screen_del_lines(), because they directly
 * shift rows. Note that this does not queue a screen redraw.
 */
    ಶೂನ್ಯ
ಸರಿಸು_image_placements(ಪೂರ್ಣಾಂಕ ತುದಿ, ಪೂರ್ಣಾಂಕ bot, ಪೂರ್ಣಾಂಕ amount)
{
    ಸರಿಸು_ತುದಿ = ತುದಿ;
    ಸರಿಸು_bot = bot;
    ಸರಿಸು += amount;
}

/*
 * Called when screen is cleared, as update_screen() may go to clearing the
 * screen instead of inserting lines using screen_ins_lines().
 */
    ಶೂನ್ಯ
ಸರಿಸು_reset_image_placements(ಶೂನ್ಯ)
{
    ಸರಿಸು = 0;
}

    ಸ್ಥಿರ image_T *
ಕಂಡುಹಿಡಿ_image(int_u id)
{
    image_T *img;

    FOR_ALL_IMAGES(img)
	ಆದರೆ (img->state == IMAGE_STATE_ಬಹಿರಂಗ && img->id == id)
	    ಹಿಂತಿರುಗಿ img;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Add an image using the given information in "dict". If "existing" is not
 * NULL, try updating it instead. If "find" is TRUE, then accept the "id" field.
 */
    image_T *
ಕೂಡಿಸು_image(dict_T *dict, image_T *existing, bool ಕಂಡುಹಿಡಿ)
{
    dictitem_T	    *di;
    blob_T	    *ದತ್ತಾಂಶ;
    varnumber_T     w, h;
    varnumber_T     n_pixels;
    image_format_T  fmt;

    ಆದರೆ (ಕಂಡುಹಿಡಿ && dict_has_ಕೀಲಿ(dict, "id"))
    {
	image_T *img = ಕಂಡುಹಿಡಿ_image((int_u)dict_get_ಸಂಖ್ಯೆ(dict, "id"));

	ಆದರೆ (img == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ image_ref(img);
    }

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"data", -1);
    w = dict_get_ಸಂಖ್ಯೆ(dict, "width");
    h = dict_get_ಸಂಖ್ಯೆ(dict, "height");

    ಆದರೆ (di == ಖಾಲಿ || di->di_tv.v_type != VAR_BLOB || w <= 0 || h <= 0)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Check for overflow
    n_pixels = w * h;
    ಆದರೆ (w > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ || h > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ || n_pixels > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
    {
	emsg(_(e_invalid_image_dimensions));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ದತ್ತಾಂಶ = di->di_tv.vval.v_blob;

    ಆದರೆ (blob_len(ದತ್ತಾಂಶ) == n_pixels * 3)
	fmt = IMAGE_FORMAT_RGB;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (blob_len(ದತ್ತಾಂಶ) == n_pixels * 4)
	fmt = IMAGE_FORMAT_RGBA;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "data",
		"data length must equal width*height*3 or width*height*4");
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (existing != ಖಾಲಿ)
    {
	ಆದರೆ (existing->ಅಗಲ == w && existing->ಎತ್ತರ == h
		&& existing->fmt == fmt)
	{
	    ಆದರೆ (image_update(existing, ದತ್ತಾಂಶ->bv_ga.ga_ದತ್ತಾಂಶ) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ existing;
	}
    }

    ಹಿಂತಿರುಗಿ image_new(ದತ್ತಾಂಶ->bv_ga.ga_ದತ್ತಾಂಶ, w, h, fmt);
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
    ಪೂರ್ಣಾಂಕ		w, h;
    dictitem_T	*di;

    ಆದರೆ (img->ದತ್ತಾಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    dict = dict_alloc();
    ಆದರೆ (dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    blob = blob_alloc();
    ಆದರೆ (blob == ಖಾಲಿ)
    {
	dict_unref(dict);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    image_get_dimensions(img, &w, &h);
    (ಶೂನ್ಯ)ga_concat_ಬೈಟುಗಳು(&blob->bv_ga, (ಅಕ್ಷರ *)img->ದತ್ತಾಂಶ,
	    (size_t)w * h * img->fmt);

    di = dictitem_alloc((char_u *)"data");

    ಆದರೆ (di != ಖಾಲಿ)
    {
	di->di_tv.v_type = VAR_BLOB;
	di->di_tv.vval.v_blob = blob;
    }
    ಆದರೆ (di == ಖಾಲಿ || dict_ಕೂಡಿಸು(dict, di) == FAIL)
    {
	ಆದರೆ (di != ಖಾಲಿ)
	    dictitem_free(di);
	dict_unref(dict);
	// No need to unref blob, dictitem_free() will do that.
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (img->state == IMAGE_STATE_ಬಹಿರಂಗ)
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "id", img->id);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "width", img->ಅಗಲ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "height", img->ಎತ್ತರ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "alpha", img->fmt == IMAGE_FORMAT_RGBA);
    dict_ಕೂಡಿಸು_string(dict, "format",
	    (char_u *)(img->fmt == IMAGE_FORMAT_RGB ? "rgb" : "rgba"));

    blob->bv_refcount = 1;

    ಹಿಂತಿರುಗಿ dict;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_imageprotocol(image_backend_T *backend)
{
    ಪೂರ್ಣಾಂಕ		    len = (ಪೂರ್ಣಾಂಕ)STRLEN(p_ipc) + 1;
    char_u	    *buf = alloc(len);
    char_u	    *p = p_ipc;
    ಪೂರ್ಣಾಂಕ		    ret = FAIL;
    image_backend_T res = IMAGE_BACKEND_NONE;
    bool	    got = false;

    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆಗ (*p != NUL)
    {
	char_u		*colon;
	regmatch_T	regmatch;
	image_backend_T prot = IMAGE_BACKEND_NONE;

	// Isolate one comma separated item.
	(ಶೂನ್ಯ)copy_option_part(&p, buf, len, ",");

	colon = vim_strchr(buf, ':');
	ಆದರೆ (colon == ಖಾಲಿ || colon == buf || colon[1] == NUL)
	    ಹೋಗು ನಿರ್ಗಮಿಸು;

	*colon = NUL;

	// Note: Keep this in sync with p_ipc_protocol_values.
	ಆದರೆ (STRCMP(colon + 1, "none") == 0)
	    prot = IMAGE_BACKEND_NONE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(colon + 1, "kitty") == 0)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_KITTY
	    prot = IMAGE_BACKEND_KITTY;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(colon + 1, "sixel") == 0)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_SIXEL
	    prot = IMAGE_BACKEND_SIXEL;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹೋಗು ನಿರ್ಗಮಿಸು;

	ಆದರೆ (got)
	    ಮುಂದುವರಿ;

	CLEAR_ಕ್ಷೇತ್ರ(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);

	ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	    ಹೋಗು ನಿರ್ಗಮಿಸು;

	bool ಹೊಂದಿಕೆ = T_NAME != ಖಾಲಿ
	    && vim_regexec(&regmatch, T_NAME, (colnr_T)0);

	vim_regfree(regmatch.regprog);
	ಆದರೆ (ಹೊಂದಿಕೆ)
	{
	    // Keep going to catch any errors
	    res = prot;
	    got = true;
	}
    }

    ret = OK;
    *backend = res;
ನಿರ್ಗಮಿಸು:
    vim_free(buf);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Update the current image backend to use depending on 'imageprotocol' and if
 * GUI is being used. Returns OK on success and FAIL on failure.
 */
    ಪೂರ್ಣಾಂಕ
update_image_backend(ಶೂನ್ಯ)
{
    image_backend_T new;

    image_T		*img;
    image_placement_T	*ಇರಿಸು;

    // Always process 'imageprotocol' option to check for errors
    ಆದರೆ (ಹೊಂದಿಕೆ_imageprotocol(&new) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_GUI
    ಆದರೆ (gui.ಒಳ_use)
	new = IMAGE_BACKEND_GUI;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (image_backend == new)
	ಹಿಂತಿರುಗಿ OK;

    // Must uninit the backends of every image/placement, then init the new
    // backend for each.
    FOR_ALL_IMAGES(img)
    {
	ಆದರೆ (image_backend != IMAGE_BACKEND_NONE && img->ದತ್ತಾಂಶ != ಖಾಲಿ
		&& img->backend_ದತ್ತಾಂಶ != ಖಾಲಿ)
	    IMAGE_FUNC(image_backend, uninit)(img);
	img->backend_ದತ್ತಾಂಶ = ಖಾಲಿ;
	ಆದರೆ (new != IMAGE_BACKEND_NONE)
	{
	    ಆದರೆ (IMAGE_FUNC(new, init)(img) == FAIL)
		ಹೋಗು fail;
	    img->backend = new;
	}
    }
    FOR_ALL_PLACEMENTS(ಇರಿಸು)
    {
	image_placement_clear(ಇರಿಸು);
	ಆದರೆ (image_backend != IMAGE_BACKEND_NONE
		&& ಇರಿಸು->img != ಖಾಲಿ && ಇರಿಸು->backend_ದತ್ತಾಂಶ != ಖಾಲಿ)
	    PLACEMENT_FUNC(image_backend, uninit)(ಇರಿಸು);
	ಇರಿಸು->backend_ದತ್ತಾಂಶ = ಖಾಲಿ;
	ಆದರೆ (new != IMAGE_BACKEND_NONE && ಇರಿಸು->img != ಖಾಲಿ)
	{
	    ಆದರೆ (PLACEMENT_FUNC(new, init)(ಇರಿಸು) == FAIL)
		ಹೋಗು fail;
	    ಇರಿಸು->backend = new;
	}
    }

    redraw_all_later(UPD_VALID);
    image_backend = new;

    ಆದರೆ (new == IMAGE_BACKEND_NONE)
	ಗಣ_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಣ_vim_var_string(VV_IMAGEBACKEND,
		(char_u *)image_backends[image_backend].name, -1);

    ಹಿಂತಿರುಗಿ OK;
fail:
    FOR_ALL_IMAGES(img)
	ಆದರೆ (img->backend != IMAGE_BACKEND_NONE)
	{
	    IMAGE_FUNC(img->backend, uninit)(img);
	    img->backend = IMAGE_BACKEND_NONE;
	}

    FOR_ALL_PLACEMENTS(ಇರಿಸು)
    {
	ಆದರೆ (ಇರಿಸು->img != ಖಾಲಿ && ಇರಿಸು->backend != IMAGE_BACKEND_NONE)
	{
	    image_placement_clear(ಇರಿಸು);
	    PLACEMENT_FUNC(ಇರಿಸು->backend, uninit)(ಇರಿಸು);
	}
	ಇರಿಸು->backend = IMAGE_BACKEND_NONE;
    }

    redraw_all_later(UPD_VALID);

    image_backend = IMAGE_BACKEND_NONE;
    ಗಣ_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    emsg(_(e_changing_image_backend_failed));
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಶೂನ್ಯ
f_image_ಕೂಡಿಸು(typval_T *argvars, typval_T *rettv)
{
    dict_T  *dict;
    image_T *img;

    ಆದರೆ (check_for_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    dict = argvars[0].vval.v_dict;

    img = ಕೂಡಿಸು_image(dict, ಖಾಲಿ, false);
    ಆದರೆ (img != ಖಾಲಿ)
	img->state = IMAGE_STATE_ಬಹಿರಂಗ;

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    rettv->vval.v_ಸಂಖ್ಯೆ = img == ಖಾಲಿ ? (varnumber_T)-1 : img->id;
}

    ಶೂನ್ಯ
f_image_discard(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    image_T *img;

    ಆದರೆ (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    img = ಕಂಡುಹಿಡಿ_image((int_u)argvars[0].vval.v_ಸಂಖ್ಯೆ);

    ಆದರೆ (img != ಖಾಲಿ)
    {
	img->state = IMAGE_STATE_ಖಾಸಗಿ;
	image_unref(img);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_ಸಂಖ್ಯೆ);
}

    ಶೂನ್ಯ
f_image_info(typval_T *argvars, typval_T *rettv)
{
    image_T *img;
    dict_T  *dict;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL
	    || check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_ಸಂಖ್ಯೆ)
    {
	img = ಕಂಡುಹಿಡಿ_image((int_u)argvars[0].vval.v_ಸಂಖ್ಯೆ);
	ಆದರೆ (img == ಖಾಲಿ)
	{
	    semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_ಸಂಖ್ಯೆ);
	    ಹಿಂತಿರುಗಿ;
	}
	dict = get_image_info(img);
	ಆದರೆ (dict != ಖಾಲಿ && list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	ಹಿಂತಿರುಗಿ;
    }

    FOR_ALL_IMAGES(img)
    {
	ಆದರೆ (img->state == IMAGE_STATE_ಖಾಸಗಿ)
	    ಮುಂದುವರಿ;
	dict = get_image_info(img);
	ಆದರೆ (dict == ಖಾಲಿ
		|| list_append_dict(rettv->vval.v_list, dict) == FAIL)
	{
	    dict_unref(dict);	// NULL-safe
	    ಹಿಂತಿರುಗಿ;
	}
    }
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_IMAGE
