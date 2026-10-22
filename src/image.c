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

#include "vim.h"

#ຖ້າ defined(FEAT_IMAGE)

sa_nracanaa
{
    const var_na *name;

    sa_nracanaa
    {
	// Return OK on success and FAIL on failure
	ຈຳນວນເຕັມ (*init)(image_T *);
	void (*uninit)(image_T *);
    } image;

    sa_nracanaa
    {
	// Return OK on success and FAIL on failure
	ຈຳນວນເຕັມ (*init)(image_placement_T *);
	void (*uninit)(image_placement_T *);

	void (*draw)(image_placement_T *);
	void (*clear)(image_placement_T *);

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
#  ບໍ່ດັ່ງນັ້ນ
	    .blit = true,
#  endif
# elif FEAT_GUI_MSWIN
	    .blit = true,
# ບໍ່ດັ່ງນັ້ນ
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
static image_backend_T image_backend = IMAGE_BACKEND_NONE;

static image_T *images = NULL;
// Sorted from highest zindex to lowest zindex
static image_placement_T    *placements = NULL;
static ຈຳນວນເຕັມ		    n_placements = 0;

static ຈຳນວນເຕັມ shift_top = 0;
static ຈຳນວນເຕັມ shift_bot = 0;
static ຈຳນວນເຕັມ shift = 0;

// If > 0, then mark_dirty_region_for_images() will be a no-op
static ຈຳນວນເຕັມ freeze_mark_dirty = 0;

#define FOR_ALL_IMAGES(v) ສຳລັບ ((v) = images; (v) != NULL; (v) = (v)->next)
#define FOR_ALL_PLACEMENTS(v) \
    ສຳລັບ ((v) = placements; (v) != NULL; (v) = (v)->next)

#define IMAGE_FUNC(b, n) (image_backends[b].image.n)
#define PLACEMENT_FUNC(b, n) (image_backends[b].placement.n)

static void redraw_region(pixman_region32_t *region, bool now, bool restore);

    void
init_image_state(void)
{
    (void)update_image_backend();
}

    void
uninit_image_state(void)
{
#ifdef FEAT_IMAGE_SIXEL
    sixel_uninit();
#endif
}

/*
 * Return true if the there image backend is ready, otherwise emit error.
 */
    static bool
backend_available(bool msg)
{
    ຖ້າ (image_backend == IMAGE_BACKEND_NONE)
    {
	ຖ້າ (msg)
	    emsg(_(e_no_image_backend_available));
	ສົ່ງຄືນ false;
    }
    ສົ່ງຄືນ true;
}

/*
 * Create a new image using the given data (creates a copy). Returns NULL on
 * failure.
 */
    image_T *
image_new(uint8_t *data, ຈຳນວນເຕັມ width, ຈຳນວນເຕັມ height, image_format_T fmt)
{
    image_T	    *img;
    static int_u    id;

    ຖ້າ (!backend_available(true))
	ສົ່ງຄືນ NULL;

    img = ALLOC_CLEAR_ONE(image_T);
    ຖ້າ (img == NULL)
	ສົ່ງຄືນ NULL;

    img->data = vim_memsave(data, (size_t)width * height * fmt);
    ຖ້າ (img->data == NULL)
    {
	vim_free(img);
	ສົ່ງຄືນ NULL;
    }

    img->state = IMAGE_STATE_PRIVATE;
    img->width = width;
    img->height = height;
    img->fmt = fmt;

    // Mix in PID to prevent ID collisions when using kitty graphics protocol
    img->id = ((((int_u)mch_get_pid() & 0x7fff) + 1) << 16) | (id++ & 0xffff);
    img->refcount = 1;

    img->backend = image_backend;

    ຖ້າ (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	vim_free(img->data);
	vim_free(img);
	ສົ່ງຄືນ NULL;
    }

    ຖ້າ (images != NULL)
	images->prev = img;
    img->next = images;
    images = img;

    ສົ່ງຄືນ img;
}

    static void
image_free(image_T *img)
{
    ຖ້າ (backend_available(false))
	IMAGE_FUNC(image_backend, uninit)(img);

    ຖ້າ (img->prev != NULL)
	img->prev->next = img->next;
    ຖ້າ (img->next != NULL)
	img->next->prev = img->prev;
    ຖ້າ (images == img)
	images = img->next;

    vim_free(img->data);
    vim_free(img);
}

/*
 * Decrement reference count of an image. Be careful to not let the public API
 * be able to directly decrement the refcount of an image multiple times!
 */
    void
image_unref(image_T *img)
{
    ຖ້າ (--img->refcount <= 0)
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
    ສົ່ງຄືນ img;
}

/*
 * Get image width and height in cells, rounding up so that every pixel is
 * covered.
 */
    void
image_get_cell_dimensions(image_T *img, ຈຳນວນເຕັມ *cw, ຈຳນວນເຕັມ *ch)
{
    ຈຳນວນເຕັມ w = PHY2LOG(img->width);
    ຈຳນວນເຕັມ h = PHY2LOG(img->height);

    *cw = (w + cell_width - 1) / cell_width;
    *ch = (h + cell_height - 1) / cell_height;
}

/*
 * Similar, but return the dimensions of all cells that are *fully* covered by
 * pixels. Pixels that are partially covered are not counted.
 */
    static void
image_get_cell_dimensions_min(image_T *img, ຈຳນວນເຕັມ *cw, ຈຳນວນເຕັມ *ch)
{
    ຈຳນວນເຕັມ w = PHY2LOG(img->width);
    ຈຳນວນເຕັມ h = PHY2LOG(img->height);

    *cw = w / cell_width;
    *ch = h / cell_height;
}

    void
image_get_dimensions(image_T *img, ຈຳນວນເຕັມ *w, ຈຳນວນເຕັມ *h)
{
    *w = img->width;
    *h = img->height;
}

/*
 * Replace the current image contents with "data". "data" should be the exact
 * same format, width, and height. Return OK on success and FAIL on failure.
 */
    ຈຳນວນເຕັມ
image_update(image_T *img, uint8_t *data)
{
    ຈຳນວນເຕັມ w, h;

    ຖ້າ (!backend_available(true) || img->data == NULL)
	ສົ່ງຄືນ FAIL;

    image_get_dimensions(img, &w, &h);
    mch_memmove(img->data, data, (size_t)w * h * img->fmt);

    IMAGE_FUNC(image_backend, uninit)(img);

    ຖ້າ (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	emsg(_(e_out_of_memory));
	VIM_CLEAR(img->data);
	ສົ່ງຄືນ FAIL;
    }

    img->ver++;
    redraw_all_later(UPD_VALID);
    ສົ່ງຄືນ OK;
}

    static void
image_placement_unlink(image_placement_T *place)
{
    ຖ້າ (place->prev != NULL)
	place->prev->next = place->next;
    ຖ້າ (place->next != NULL)
	place->next->prev = place->prev;
    ຖ້າ (placements == place)
	placements = place->next;
    n_placements--;
}

    static void
image_placement_link(image_placement_T *place)
{
    image_placement_T *p = placements;
    image_placement_T *prev = NULL;

    // Add placement *before* the placement with the lower zindex.
    ໃນຂະນະທີ່ (p != NULL)
    {
	ຖ້າ (place->zindex > p->zindex)
	    bha_nga;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (place->zindex == p->zindex)
	{
	    ຖ້າ (place->use > p->use)
		bha_nga;
	    ຖ້າ (place->gen < p->gen)
		bha_nga;
	}

	prev = p;
	p = p->next;
    }

    place->next = p;
    place->prev = prev;

    ຖ້າ (p != NULL)
	p->prev = place;

    ຖ້າ (prev != NULL)
	prev->next = place;
    ບໍ່ດັ່ງນັ້ນ
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
    static int_u	id = 1; // Kitty placements id must be > 1

    ຖ້າ (!backend_available(!quiet))
	ສົ່ງຄືນ NULL;

    place = ALLOC_CLEAR_ONE(image_placement_T);
    ຖ້າ (place == NULL)
	ສົ່ງຄືນ NULL;

    place->id = id;
    id += PLACEMENT_ID_INC; // Allocate 1000 free placement ids to be used to
			    // draw this image placement. Only relevant for
			    // kitty graphics protocol.
    place->gen = gen;
    place->use = use;

    place->img = img;
    place->flags = IMAGEPF_DIRTY;

    ຖ້າ (img != NULL)
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

	ຖ້າ (PLACEMENT_FUNC(image_backend, init)(place) == FAIL)
	{
	    id -= PLACEMENT_ID_INC;
	    vim_free(place);
	    ສົ່ງຄືນ NULL;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
	place->backend = IMAGE_BACKEND_NONE;

    image_placement_link(place);

    ສົ່ງຄືນ place;
}

    static void
image_placement_clear_int(image_placement_T *place, bool now)
{
    ຖ້າ (backend_available(false) && place->img != NULL)
    {
	ຖ້າ (place->flags & IMAGEPF_VISIBLE_INIT
		&& PLACEMENT_FUNC(image_backend, blit))
	    redraw_region(&place->visible_abs, now, true);
	PLACEMENT_FUNC(image_backend, clear)(place);
	place->flags |= IMAGEPF_HIDDEN;
	ຖ້າ (!now && !updating_screen)
	    redraw_all_later(UPD_VALID);
    }
}

    void
image_placement_clear(image_placement_T *place)
{
    image_placement_clear_int(place, updating_screen);
}

    void
image_placement_free(image_placement_T *place)
{
    ຖ້າ (backend_available(false) && place->backend_data != NULL)
    {
	image_placement_clear(place);
	PLACEMENT_FUNC(image_backend, uninit)(place);
    }

    ຖ້າ (place->flags & IMAGEPF_VISIBLE_INIT)
    {
	pixman_region32_fini(&place->visible);
	pixman_region32_fini(&place->visible_abs);
    }
    image_placement_unlink(place);

    ຖ້າ (place->img != NULL)
	image_unref(place->img);
    vim_free(place);
}

    void
image_placement_set_zindex(image_placement_T *place, ຈຳນວນເຕັມ zindex)
{
    // If image was hidden before, must dirty it so it is fully redrawn again.
    ຖ້າ (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    ຖ້າ (place->zindex == zindex)
	ສົ່ງຄືນ;
    place->zindex = zindex;
    place->flags |= IMAGEPF_DIRTY;

    // Must re-add the placement back so it is in the correct order
    image_placement_unlink(place);
    image_placement_link(place);
}

/*
 * Make sure to also update the bounding box!
 */
    void
image_placement_set_position(image_placement_T *place, ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ col)
{
    ຖ້າ (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    ຖ້າ (place->row == row && place->col == col)
	ສົ່ງຄືນ;
    place->row = row;
    place->col = col;
    place->flags |= IMAGEPF_DIRTY;
}

    void
image_placement_set_crop(image_placement_T *place, ຈຳນວນເຕັມ x, ຈຳນວນເຕັມ y, ຈຳນວນເຕັມ w, ຈຳນວນເຕັມ h)
{
    ຖ້າ (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    ຖ້າ (place->crop_box.x1 == x && place->crop_box.y1 == y
	    && place->crop_box.x2 == x + w && place->crop_box.y2 == y + h)
	ສົ່ງຄືນ;

    place->crop_box.x1 = x;
    place->crop_box.y1 = y;

    place->crop_box.x2 = x + w;
    place->crop_box.y2 = y + h;
    place->flags |= IMAGEPF_DIRTY;
}

    void
image_placement_set_bounding_box(
	image_placement_T   *place,
	ຈຳນວນເຕັມ		    row,
	ຈຳນວນເຕັມ		    col,
	ຈຳນວນເຕັມ		    row_height,
	ຈຳນວນເຕັມ		    col_width)
{
    ຖ້າ (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    ຖ້າ (place->bounding_box.x1 == col && place->bounding_box.y1 == row
	    && place->bounding_box.x2 == col + col_width
	    && place->bounding_box.y2 == row + row_height)
	ສົ່ງຄືນ;

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
    void
image_placement_set_enable(image_placement_T *place, bool enable)
{
    bool old = place->flags & IMAGEPF_ENABLED;

    ຖ້າ (enable && !old)
	place->flags |= IMAGEPF_ENABLED;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (!enable && old)
    {
	place->flags &= ~IMAGEPF_ENABLED;
	image_placement_clear(place);
    }
    ບໍ່ດັ່ງນັ້ນ
	ສົ່ງຄືນ;

    place->flags |= IMAGEPF_DIRTY;
}

/*
 * Note that the returned crop region (x, y, w, h) is in physical pixels if
 * "physical" is true. Not relevant when using terminal protocols (sixel, KGP).
 */
    void
image_placement_subrect(
	image_placement_T   *place,
	pixman_box32_t	    rect,
	ຈຳນວນເຕັມ		    *row,
	ຈຳນວນເຕັມ		    *col,
	ຈຳນວນເຕັມ		    *x,
	ຈຳນວນເຕັມ		    *y,
	ຈຳນວນເຕັມ		    *w,
	ຈຳນວນເຕັມ		    *h,
	bool		    physical)
{
    ຈຳນວນເຕັມ iw, ih;

    image_get_dimensions(place->img, &iw, &ih);

    *row = place->row + rect.y1 + place->row_off;
    *col = place->col + rect.x1;

    *x = (rect.x1 + place->crop_box.x1) * cell_width;
    *y = (rect.y1 + place->crop_box.y1) * cell_height;
    *w = (rect.x2 - rect.x1) * cell_width;
    *h = (rect.y2 - rect.y1) * cell_height;

    ຖ້າ (physical)
    {
	*x = LOG2PHY(*x);
	*y = LOG2PHY(*y);
	*w = LOG2PHY(*w);
	*h = LOG2PHY(*h);
    }
    ບໍ່ດັ່ງນັ້ນ
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
    static void
redraw_region(pixman_region32_t *region, bool now, bool restore)
{
    pixman_box32_t  *rects;
    ຈຳນວນເຕັມ		    n_rects;
    ຈຳນວນເຕັມ		    cur_row = screen_cur_row;
    ຈຳນວນເຕັມ		    cur_col = screen_cur_col;

    rects = pixman_region32_rectangles(region, &n_rects);
    ຖ້າ (rects == NULL)
	ສົ່ງຄືນ;

    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < n_rects; i++)
    {
	pixman_box32_t rect = rects[i];

	ຖ້າ (now)
	{
	    // The region may be from before a resize, when the screen was
	    // bigger.
	    ຈຳນວນເຕັມ height = MIN(rect.y2, screen_Rows) - rect.y1;
	    ຈຳນວນເຕັມ width = MIN(rect.x2, screen_Columns) - rect.x1;

	    // Tip: if you want to debug stale pixels appearing, set "inverse"
	    // of screen_draw_rectangle() to TRUE.
	    ຖ້າ (height > 0 && width > 0)
		screen_draw_rectangle(rect.y1, rect.x1, height, width,
			FALSE, TRUE);
	    continue;
	}

	ສຳລັບ (ຈຳນວນເຕັມ r = rect.y1; r < rect.y2; r++)
	{
	    ຖ້າ (r >= screen_Rows)
		bha_nga;

	    ສຳລັບ (ຈຳນວນເຕັມ c = rect.x1; c < rect.x2; c++)
	    {
		ຖ້າ (c >= screen_Columns)
		    bha_nga;

		ScreenAttrs[LineOffset[r] + c] = (sattr_T)-1;
	    }
	}
    }

    ຖ້າ (restore)
	windgoto(cur_row, cur_col);
}

/*
 * Draw all image placements to the screen. This should be done after all text
 * have been drawn to the screen, and image placements positioned correctly.
 */
    void
draw_image_placements(void)
{
    image_placement_T	*place;
    pixman_region32_t	subtract_region;    // In cells
    pixman_region32_t	dirty_region;	    // In cells
    image_placement_T	**pending_placements;
    ຈຳນວນເຕັມ			pending_len = -1;
    // Save current cursor position
    ຈຳນວນເຕັມ			cur_row = screen_cur_row;
    ຈຳນວນເຕັມ			cur_col = screen_cur_col;

    // Check if all placements have no backing image. If so, then do nothing
    FOR_ALL_PLACEMENTS(place)
	ຖ້າ (place->img != NULL)
	{
	    pending_len = 0;
	    bha_nga;
	}

    ຖ້າ (pending_len == -1)
	ສົ່ງຄືນ;

    ຖ້າ (!backend_available(false))
	ສົ່ງຄືນ;

    ຖ້າ (n_placements == 0)
	ສົ່ງຄືນ;

    pending_placements = ALLOC_CLEAR_MULT(image_placement_T *, n_placements);
    ຖ້າ (pending_placements == NULL)
	ສົ່ງຄືນ;

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

	ຈຳນວນເຕັມ x, y;

	ຖ້າ (place->flags & IMAGEPF_HIDDEN || !(place->flags & IMAGEPF_ENABLED))
	    // Don't need to clear image placement for non-blit backends,
	    // because we call image_placement_clear() every time we don't want
	    // to see the image. Not sure of the side effects if we also clear
	    // it here...
	    continue;

	ຖ້າ (img != NULL)
	{
	    pixman_region32_t	visible_abs;
	    ຈຳນວນເຕັມ			crop_w, crop_h;
	    bool		cs_changed;
	    bool		need_redraw;

	    // Processing changed image data failed, skip this image
	    ຖ້າ (place->img->data == NULL)
	    {
		image_placement_clear(place);
		continue;
	    }

	    // Only apply row shift if image is within the shifted region.
	    ຖ້າ ((place->bounding_box.y1 >= shift_top
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
	    ຖ້າ (!pixman_region32_intersect_rect(&image_region, &image_region,
			0, 0, Columns, Rows)
		    || !pixman_region32_subtract(&visible_region,
			&image_region, &subtract_region)
		    || !pixman_region32_copy(&visible_abs, &visible_region))
	    {
		pixman_region32_fini(&image_region);
		pixman_region32_fini(&visible_region);
		continue;
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
	    ຖ້າ (cs_changed)
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

	    ຖ້າ (place->img_ver != img->ver)
	    {
		// Image data changed
		need_redraw = true;
		place->img_ver = img->ver;

		image_placement_clear(place);
		PLACEMENT_FUNC(image_backend, uninit)(place);
		ຖ້າ (PLACEMENT_FUNC(image_backend, init)(place) == FAIL)
		{
		    emsg(_(e_out_of_memory));
		    image_unref(place->img);
		    place->img = NULL;
		    continue;
		}
	    }

	    ຖ້າ (need_redraw)
	    {
		ຖ້າ (place->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&place->visible);
		place->visible = visible_region;

		ຖ້າ (PLACEMENT_FUNC(image_backend, blit))
		{
		    // Must redraw the stale regions that will not be composited
		    // over (for this specific image).
		    pixman_region32_t stale_region;

		    pixman_region32_init(&stale_region);

		    ຖ້າ (place->flags & IMAGEPF_VISIBLE_INIT)
		    {
			(void)pixman_region32_subtract(&stale_region,
				&place->visible_abs, &visible_abs);

			// Since pixels may be translucent/transparent, we must
			// redraw the cells behind the image every time, so that
			// no stale pixels remain from the previous redraw.
			ຖ້າ (img->fmt == IMAGE_FORMAT_RGBA)
			    (void)pixman_region32_union(&stale_region,
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
		    ຖ້າ (img->fmt != IMAGE_FORMAT_RGBA)
		    {
			ຈຳນວນເຕັມ		    min_w, min_h;
			ຈຳນວນເຕັມ		    rect_w, rect_h;
			pixman_region32_t   min_region;

			image_get_cell_dimensions_min(img, &min_w, &min_h);

			// Ensure "min_region" only covers the fully covered
			// cells of the cropped region.
			rect_w = min_w - place->crop_box.x1;
			rect_h = min_h - place->crop_box.y1;

			ຖ້າ (rect_w > 0 && rect_h > 0)
			    // pixman_region32_init_rect() expects a non zero
			    // width and height (otherwise it outputs error
			    // message).
			    pixman_region32_init_rect(&min_region,
				    x, y, rect_w, rect_h);
			ບໍ່ດັ່ງນັ້ນ
			    pixman_region32_init(&min_region);

			(void)pixman_region32_subtract(&min_region,
				&visible_abs, &min_region);
			(void)pixman_region32_union(&stale_region,
				&stale_region, &min_region);
			pixman_region32_fini(&min_region);
		    }

		    ຖ້າ (place->flags & IMAGEPF_VISIBLE_INIT)
			// Also subtract "subtract_region", so we don't
			// redundantly redraw cells that will have images
			// painted over them after. Make sure to do this before
			// we check partially covered cells, since
			// "subtract_region" may also have partially covered
			// cells.
			(void)pixman_region32_subtract(&stale_region,
				&stale_region, &subtract_region);

		    // Don't redraw now, defer it later
		    pixman_region32_union(&dirty_region,
			    &dirty_region, &stale_region);
		    pixman_region32_fini(&stale_region);
		}
		ຖ້າ (place->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&place->visible_abs);
		place->visible_abs = visible_abs;

		pending_placements[pending_len++] = place;
		place->flags |= IMAGEPF_VISIBLE_INIT;
		place->flags &= ~IMAGEPF_DIRTY;
	    }
	    ບໍ່ດັ່ງນັ້ນ
	    {
		pixman_region32_fini(&visible_region);
		pixman_region32_fini(&visible_abs);
	    }
	}

	// If this is last image, no need to union its bounds.
	ຖ້າ (place->next == NULL)
	    bha_nga;

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

    ຖ້າ (pending_len > 0)
    {
#ifdef FEAT_GUI
	// Make sure to flush any output! We write directly to the draw
	// area/whatever, meaning draw commands outputted before this (as escape
	// sequences), will still draw over the image surface if we don't flush
	// now.
	ຖ້າ (gui.in_use)
	    out_flush();
#endif

	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < pending_len; i++)
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
    void
mark_dirty_region_for_images(ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ col, ຈຳນວນເຕັມ row_height, ຈຳນວນເຕັມ col_width)
{
    image_placement_T	*place;
    pixman_box32_t	rect;

    ຖ້າ (freeze_mark_dirty > 0 || !backend_available(false)
	    || !PLACEMENT_FUNC(image_backend, blit) || n_placements == 0)
	ສົ່ງຄືນ;

    rect.x1 = col;
    rect.y1 = row;
    rect.x2 = col + col_width;
    rect.y2 = row + row_height;

    FOR_ALL_PLACEMENTS(place)
    {
	ຖ້າ (place->flags & IMAGEPF_HIDDEN)
	    continue;
	ຖ້າ (pixman_region32_contains_rectangle(&place->visible_abs, &rect)
		== PIXMAN_REGION_OUT)
	    continue;

	place->flags |= IMAGEPF_DIRTY;
    }
}

    void
clear_all_image_placements(void)
{
    image_placement_T *place;

    FOR_ALL_PLACEMENTS(place)
	image_placement_clear(place);
    ຖ້າ (!updating_screen)
	redraw_all_later(UPD_VALID);
}

/*
 * Used for screen_ins_lines() and screen_del_lines(), because they directly
 * shift rows. Note that this does not queue a screen redraw.
 */
    void
shift_image_placements(ຈຳນວນເຕັມ top, ຈຳນວນເຕັມ bot, ຈຳນວນເຕັມ amount)
{
    shift_top = top;
    shift_bot = bot;
    shift += amount;
}

/*
 * Called when screen is cleared, as update_screen() may go to clearing the
 * screen instead of inserting lines using screen_ins_lines().
 */
    void
shift_reset_image_placements(void)
{
    shift = 0;
}

    static image_T *
find_image(int_u id)
{
    image_T *img;

    FOR_ALL_IMAGES(img)
	ຖ້າ (img->state == IMAGE_STATE_PUBLIC && img->id == id)
	    ສົ່ງຄືນ img;
    ສົ່ງຄືນ NULL;
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

    ຖ້າ (find && dict_has_key(dict, "id"))
    {
	image_T *img = find_image((int_u)dict_get_number(dict, "id"));

	ຖ້າ (img == NULL)
	    ສົ່ງຄືນ NULL;
	ສົ່ງຄືນ image_ref(img);
    }

    di = dict_find(dict, (char_u *)"data", -1);
    w = dict_get_number(dict, "width");
    h = dict_get_number(dict, "height");

    ຖ້າ (di == NULL || di->di_tv.v_type != VAR_BLOB || w <= 0 || h <= 0)
    {
	emsg(_(e_invalid_argument));
	ສົ່ງຄືນ NULL;
    }

    // Check for overflow
    n_pixels = w * h;
    ຖ້າ (w > INT_MAX || h > INT_MAX || n_pixels > INT_MAX)
    {
	emsg(_(e_invalid_image_dimensions));
	ສົ່ງຄືນ NULL;
    }

    data = di->di_tv.vval.v_blob;

    ຖ້າ (blob_len(data) == n_pixels * 3)
	fmt = IMAGE_FORMAT_RGB;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (blob_len(data) == n_pixels * 4)
	fmt = IMAGE_FORMAT_RGBA;
    ບໍ່ດັ່ງນັ້ນ
    {
	semsg(_(e_invalid_value_for_argument_str_str), "data",
		"data length must equal width*height*3 or width*height*4");
	ສົ່ງຄືນ NULL;
    }

    ຖ້າ (existing != NULL)
    {
	ຖ້າ (existing->width == w && existing->height == h
		&& existing->fmt == fmt)
	{
	    ຖ້າ (image_update(existing, data->bv_ga.ga_data) == FAIL)
		ສົ່ງຄືນ NULL;
	    ສົ່ງຄືນ existing;
	}
    }

    ສົ່ງຄືນ image_new(data->bv_ga.ga_data, w, h, fmt);
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
    ຈຳນວນເຕັມ		w, h;
    dictitem_T	*di;

    ຖ້າ (img->data == NULL)
	ສົ່ງຄືນ NULL;

    dict = dict_alloc();
    ຖ້າ (dict == NULL)
	ສົ່ງຄືນ NULL;

    blob = blob_alloc();
    ຖ້າ (blob == NULL)
    {
	dict_unref(dict);
	ສົ່ງຄືນ NULL;
    }

    image_get_dimensions(img, &w, &h);
    (void)ga_concat_bytes(&blob->bv_ga, (var_na *)img->data,
	    (size_t)w * h * img->fmt);

    di = dictitem_alloc((char_u *)"data");

    ຖ້າ (di != NULL)
    {
	di->di_tv.v_type = VAR_BLOB;
	di->di_tv.vval.v_blob = blob;
    }
    ຖ້າ (di == NULL || dict_add(dict, di) == FAIL)
    {
	ຖ້າ (di != NULL)
	    dictitem_free(di);
	dict_unref(dict);
	// No need to unref blob, dictitem_free() will do that.
	ສົ່ງຄືນ NULL;
    }

    ຖ້າ (img->state == IMAGE_STATE_PUBLIC)
	dict_add_number(dict, "id", img->id);
    dict_add_number(dict, "width", img->width);
    dict_add_number(dict, "height", img->height);
    dict_add_number(dict, "alpha", img->fmt == IMAGE_FORMAT_RGBA);
    dict_add_string(dict, "format",
	    (char_u *)(img->fmt == IMAGE_FORMAT_RGB ? "rgb" : "rgba"));

    blob->bv_refcount = 1;

    ສົ່ງຄືນ dict;
}

    static ຈຳນວນເຕັມ
match_imageprotocol(image_backend_T *backend)
{
    ຈຳນວນເຕັມ		    len = (ຈຳນວນເຕັມ)STRLEN(p_ipc) + 1;
    char_u	    *buf = alloc(len);
    char_u	    *p = p_ipc;
    ຈຳນວນເຕັມ		    ret = FAIL;
    image_backend_T res = IMAGE_BACKEND_NONE;
    bool	    got = false;

    ຖ້າ (buf == NULL)
	ສົ່ງຄືນ FAIL;

    ໃນຂະນະທີ່ (*p != NUL)
    {
	char_u		*colon;
	regmatch_T	regmatch;
	image_backend_T prot = IMAGE_BACKEND_NONE;

	// Isolate one comma separated item.
	(void)copy_option_part(&p, buf, len, ",");

	colon = vim_strchr(buf, ':');
	ຖ້າ (colon == NULL || colon == buf || colon[1] == NUL)
	    goto exit;

	*colon = NUL;

	// Note: Keep this in sync with p_ipc_protocol_values.
	ຖ້າ (STRCMP(colon + 1, "none") == 0)
	    prot = IMAGE_BACKEND_NONE;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRCMP(colon + 1, "kitty") == 0)
	{
#ifdef FEAT_IMAGE_KITTY
	    prot = IMAGE_BACKEND_KITTY;
#endif
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRCMP(colon + 1, "sixel") == 0)
	{
#ifdef FEAT_IMAGE_SIXEL
	    prot = IMAGE_BACKEND_SIXEL;
#endif
	}
	ບໍ່ດັ່ງນັ້ນ
	    goto exit;

	ຖ້າ (got)
	    continue;

	CLEAR_FIELD(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);

	ຖ້າ (regmatch.regprog == NULL)
	    goto exit;

	bool match = T_NAME != NULL
	    && vim_regexec(&regmatch, T_NAME, (colnr_T)0);

	vim_regfree(regmatch.regprog);
	ຖ້າ (match)
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
    ສົ່ງຄືນ ret;
}

/*
 * Update the current image backend to use depending on 'imageprotocol' and if
 * GUI is being used. Returns OK on success and FAIL on failure.
 */
    ຈຳນວນເຕັມ
update_image_backend(void)
{
    image_backend_T new;

    image_T		*img;
    image_placement_T	*place;

    // Always process 'imageprotocol' option to check for errors
    ຖ້າ (match_imageprotocol(&new) == FAIL)
	ສົ່ງຄືນ FAIL;

#ifdef FEAT_IMAGE_GUI
    ຖ້າ (gui.in_use)
	new = IMAGE_BACKEND_GUI;
#endif

    ຖ້າ (image_backend == new)
	ສົ່ງຄືນ OK;

    // Must uninit the backends of every image/placement, then init the new
    // backend for each.
    FOR_ALL_IMAGES(img)
    {
	ຖ້າ (image_backend != IMAGE_BACKEND_NONE && img->data != NULL
		&& img->backend_data != NULL)
	    IMAGE_FUNC(image_backend, uninit)(img);
	img->backend_data = NULL;
	ຖ້າ (new != IMAGE_BACKEND_NONE)
	{
	    ຖ້າ (IMAGE_FUNC(new, init)(img) == FAIL)
		goto fail;
	    img->backend = new;
	}
    }
    FOR_ALL_PLACEMENTS(place)
    {
	image_placement_clear(place);
	ຖ້າ (image_backend != IMAGE_BACKEND_NONE
		&& place->img != NULL && place->backend_data != NULL)
	    PLACEMENT_FUNC(image_backend, uninit)(place);
	place->backend_data = NULL;
	ຖ້າ (new != IMAGE_BACKEND_NONE && place->img != NULL)
	{
	    ຖ້າ (PLACEMENT_FUNC(new, init)(place) == FAIL)
		goto fail;
	    place->backend = new;
	}
    }

    redraw_all_later(UPD_VALID);
    image_backend = new;

    ຖ້າ (new == IMAGE_BACKEND_NONE)
	set_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    ບໍ່ດັ່ງນັ້ນ
	set_vim_var_string(VV_IMAGEBACKEND,
		(char_u *)image_backends[image_backend].name, -1);

    ສົ່ງຄືນ OK;
fail:
    FOR_ALL_IMAGES(img)
	ຖ້າ (img->backend != IMAGE_BACKEND_NONE)
	{
	    IMAGE_FUNC(img->backend, uninit)(img);
	    img->backend = IMAGE_BACKEND_NONE;
	}

    FOR_ALL_PLACEMENTS(place)
    {
	ຖ້າ (place->img != NULL && place->backend != IMAGE_BACKEND_NONE)
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
    ສົ່ງຄືນ FAIL;
}

    void
f_image_add(typval_T *argvars, typval_T *rettv)
{
    dict_T  *dict;
    image_T *img;

    ຖ້າ (check_for_dict_arg(argvars, 0) == FAIL)
	ສົ່ງຄືນ;

    dict = argvars[0].vval.v_dict;

    img = add_image(dict, NULL, false);
    ຖ້າ (img != NULL)
	img->state = IMAGE_STATE_PUBLIC;

    rettv->v_type = VAR_NUMBER;
    rettv->vval.v_number = img == NULL ? (varnumber_T)-1 : img->id;
}

    void
f_image_discard(typval_T *argvars, typval_T *rettv UNUSED)
{
    image_T *img;

    ຖ້າ (check_for_number_arg(argvars, 0) == FAIL)
	ສົ່ງຄືນ;

    img = find_image((int_u)argvars[0].vval.v_number);

    ຖ້າ (img != NULL)
    {
	img->state = IMAGE_STATE_PRIVATE;
	image_unref(img);
    }
    ບໍ່ດັ່ງນັ້ນ
	semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_number);
}

    void
f_image_info(typval_T *argvars, typval_T *rettv)
{
    image_T *img;
    dict_T  *dict;

    ຖ້າ (rettv_list_alloc(rettv) == FAIL
	    || check_for_opt_number_arg(argvars, 0) == FAIL)
	ສົ່ງຄືນ;

    ຖ້າ (argvars[0].v_type == VAR_NUMBER)
    {
	img = find_image((int_u)argvars[0].vval.v_number);
	ຖ້າ (img == NULL)
	{
	    semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_number);
	    ສົ່ງຄືນ;
	}
	dict = get_image_info(img);
	ຖ້າ (dict != NULL && list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	ສົ່ງຄືນ;
    }

    FOR_ALL_IMAGES(img)
    {
	ຖ້າ (img->state == IMAGE_STATE_PRIVATE)
	    continue;
	dict = get_image_info(img);
	ຖ້າ (dict == NULL
		|| list_append_dict(rettv->vval.v_list, dict) == FAIL)
	{
	    dict_unref(dict);	// NULL-safe
	    ສົ່ງຄືນ;
	}
    }
}

#endif // FEAT_IMAGE
