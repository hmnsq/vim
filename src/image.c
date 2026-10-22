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

#만약 defined(FEAT_IMAGE)

struct
{
    const char *name;

    struct
    {
	// Return OK on success and FAIL on failure
	정수 (*init)(image_T *);
	void (*uninit)(image_T *);
    } image;

    struct
    {
	// Return OK on success and FAIL on failure
	정수 (*init)(image_placement_T *);
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
#  아니면
	    .blit = true,
#  endif
# elif FEAT_GUI_MSWIN
	    .blit = true,
# 아니면
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
static 정수		    n_placements = 0;

static 정수 shift_top = 0;
static 정수 shift_bot = 0;
static 정수 shift = 0;

// If > 0, then mark_dirty_region_for_images() will be a no-op
static 정수 freeze_mark_dirty = 0;

#define FOR_ALL_IMAGES(v) 위해 ((v) = images; (v) != NULL; (v) = (v)->next)
#define FOR_ALL_PLACEMENTS(v) \
    위해 ((v) = placements; (v) != NULL; (v) = (v)->next)

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
    만약 (image_backend == IMAGE_BACKEND_NONE)
    {
	만약 (msg)
	    emsg(_(e_no_image_backend_available));
	반환 false;
    }
    반환 true;
}

/*
 * Create a new image using the given data (creates a copy). Returns NULL on
 * failure.
 */
    image_T *
image_new(uint8_t *data, 정수 width, 정수 height, image_format_T fmt)
{
    image_T	    *img;
    static int_u    id;

    만약 (!backend_available(true))
	반환 NULL;

    img = ALLOC_CLEAR_ONE(image_T);
    만약 (img == NULL)
	반환 NULL;

    img->data = vim_memsave(data, (size_t)width * height * fmt);
    만약 (img->data == NULL)
    {
	vim_free(img);
	반환 NULL;
    }

    img->state = IMAGE_STATE_PRIVATE;
    img->width = width;
    img->height = height;
    img->fmt = fmt;

    // Mix in PID to prevent ID collisions when using kitty graphics protocol
    img->id = ((((int_u)mch_get_pid() & 0x7fff) + 1) << 16) | (id++ & 0xffff);
    img->refcount = 1;

    img->backend = image_backend;

    만약 (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	vim_free(img->data);
	vim_free(img);
	반환 NULL;
    }

    만약 (images != NULL)
	images->prev = img;
    img->next = images;
    images = img;

    반환 img;
}

    static void
image_free(image_T *img)
{
    만약 (backend_available(false))
	IMAGE_FUNC(image_backend, uninit)(img);

    만약 (img->prev != NULL)
	img->prev->next = img->next;
    만약 (img->next != NULL)
	img->next->prev = img->prev;
    만약 (images == img)
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
    만약 (--img->refcount <= 0)
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
    반환 img;
}

/*
 * Get image width and height in cells, rounding up so that every pixel is
 * covered.
 */
    void
image_get_cell_dimensions(image_T *img, 정수 *cw, 정수 *ch)
{
    정수 w = PHY2LOG(img->width);
    정수 h = PHY2LOG(img->height);

    *cw = (w + cell_width - 1) / cell_width;
    *ch = (h + cell_height - 1) / cell_height;
}

/*
 * Similar, but return the dimensions of all cells that are *fully* covered by
 * pixels. Pixels that are partially covered are not counted.
 */
    static void
image_get_cell_dimensions_min(image_T *img, 정수 *cw, 정수 *ch)
{
    정수 w = PHY2LOG(img->width);
    정수 h = PHY2LOG(img->height);

    *cw = w / cell_width;
    *ch = h / cell_height;
}

    void
image_get_dimensions(image_T *img, 정수 *w, 정수 *h)
{
    *w = img->width;
    *h = img->height;
}

/*
 * Replace the current image contents with "data". "data" should be the exact
 * same format, width, and height. Return OK on success and FAIL on failure.
 */
    정수
image_update(image_T *img, uint8_t *data)
{
    정수 w, h;

    만약 (!backend_available(true) || img->data == NULL)
	반환 FAIL;

    image_get_dimensions(img, &w, &h);
    mch_memmove(img->data, data, (size_t)w * h * img->fmt);

    IMAGE_FUNC(image_backend, uninit)(img);

    만약 (IMAGE_FUNC(image_backend, init)(img) == FAIL)
    {
	emsg(_(e_out_of_memory));
	VIM_CLEAR(img->data);
	반환 FAIL;
    }

    img->ver++;
    redraw_all_later(UPD_VALID);
    반환 OK;
}

    static void
image_placement_unlink(image_placement_T *place)
{
    만약 (place->prev != NULL)
	place->prev->next = place->next;
    만약 (place->next != NULL)
	place->next->prev = place->prev;
    만약 (placements == place)
	placements = place->next;
    n_placements--;
}

    static void
image_placement_link(image_placement_T *place)
{
    image_placement_T *p = placements;
    image_placement_T *prev = NULL;

    // Add placement *before* the placement with the lower zindex.
    동안 (p != NULL)
    {
	만약 (place->zindex > p->zindex)
	    break;
	아니면 만약 (place->zindex == p->zindex)
	{
	    만약 (place->use > p->use)
		break;
	    만약 (place->gen < p->gen)
		break;
	}

	prev = p;
	p = p->next;
    }

    place->next = p;
    place->prev = prev;

    만약 (p != NULL)
	p->prev = place;

    만약 (prev != NULL)
	prev->next = place;
    아니면
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

    만약 (!backend_available(!quiet))
	반환 NULL;

    place = ALLOC_CLEAR_ONE(image_placement_T);
    만약 (place == NULL)
	반환 NULL;

    place->id = id;
    id += PLACEMENT_ID_INC; // Allocate 1000 free placement ids to be used to
			    // draw this image placement. Only relevant for
			    // kitty graphics protocol.
    place->gen = gen;
    place->use = use;

    place->img = img;
    place->flags = IMAGEPF_DIRTY;

    만약 (img != NULL)
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

	만약 (PLACEMENT_FUNC(image_backend, init)(place) == FAIL)
	{
	    id -= PLACEMENT_ID_INC;
	    vim_free(place);
	    반환 NULL;
	}
    }
    아니면
	place->backend = IMAGE_BACKEND_NONE;

    image_placement_link(place);

    반환 place;
}

    static void
image_placement_clear_int(image_placement_T *place, bool now)
{
    만약 (backend_available(false) && place->img != NULL)
    {
	만약 (place->flags & IMAGEPF_VISIBLE_INIT
		&& PLACEMENT_FUNC(image_backend, blit))
	    redraw_region(&place->visible_abs, now, true);
	PLACEMENT_FUNC(image_backend, clear)(place);
	place->flags |= IMAGEPF_HIDDEN;
	만약 (!now && !updating_screen)
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
    만약 (backend_available(false) && place->backend_data != NULL)
    {
	image_placement_clear(place);
	PLACEMENT_FUNC(image_backend, uninit)(place);
    }

    만약 (place->flags & IMAGEPF_VISIBLE_INIT)
    {
	pixman_region32_fini(&place->visible);
	pixman_region32_fini(&place->visible_abs);
    }
    image_placement_unlink(place);

    만약 (place->img != NULL)
	image_unref(place->img);
    vim_free(place);
}

    void
image_placement_set_zindex(image_placement_T *place, 정수 zindex)
{
    // If image was hidden before, must dirty it so it is fully redrawn again.
    만약 (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    만약 (place->zindex == zindex)
	반환;
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
image_placement_set_position(image_placement_T *place, 정수 row, 정수 col)
{
    만약 (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    만약 (place->row == row && place->col == col)
	반환;
    place->row = row;
    place->col = col;
    place->flags |= IMAGEPF_DIRTY;
}

    void
image_placement_set_crop(image_placement_T *place, 정수 x, 정수 y, 정수 w, 정수 h)
{
    만약 (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    만약 (place->crop_box.x1 == x && place->crop_box.y1 == y
	    && place->crop_box.x2 == x + w && place->crop_box.y2 == y + h)
	반환;

    place->crop_box.x1 = x;
    place->crop_box.y1 = y;

    place->crop_box.x2 = x + w;
    place->crop_box.y2 = y + h;
    place->flags |= IMAGEPF_DIRTY;
}

    void
image_placement_set_bounding_box(
	image_placement_T   *place,
	정수		    row,
	정수		    col,
	정수		    row_height,
	정수		    col_width)
{
    만약 (place->flags & IMAGEPF_HIDDEN)
	place->flags |= IMAGEPF_DIRTY;
    place->flags &= ~IMAGEPF_HIDDEN;

    만약 (place->bounding_box.x1 == col && place->bounding_box.y1 == row
	    && place->bounding_box.x2 == col + col_width
	    && place->bounding_box.y2 == row + row_height)
	반환;

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

    만약 (enable && !old)
	place->flags |= IMAGEPF_ENABLED;
    아니면 만약 (!enable && old)
    {
	place->flags &= ~IMAGEPF_ENABLED;
	image_placement_clear(place);
    }
    아니면
	반환;

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
	정수		    *row,
	정수		    *col,
	정수		    *x,
	정수		    *y,
	정수		    *w,
	정수		    *h,
	bool		    physical)
{
    정수 iw, ih;

    image_get_dimensions(place->img, &iw, &ih);

    *row = place->row + rect.y1 + place->row_off;
    *col = place->col + rect.x1;

    *x = (rect.x1 + place->crop_box.x1) * cell_width;
    *y = (rect.y1 + place->crop_box.y1) * cell_height;
    *w = (rect.x2 - rect.x1) * cell_width;
    *h = (rect.y2 - rect.y1) * cell_height;

    만약 (physical)
    {
	*x = LOG2PHY(*x);
	*y = LOG2PHY(*y);
	*w = LOG2PHY(*w);
	*h = LOG2PHY(*h);
    }
    아니면
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
    정수		    n_rects;
    정수		    cur_row = screen_cur_row;
    정수		    cur_col = screen_cur_col;

    rects = pixman_region32_rectangles(region, &n_rects);
    만약 (rects == NULL)
	반환;

    위해 (정수 i = 0; i < n_rects; i++)
    {
	pixman_box32_t rect = rects[i];

	만약 (now)
	{
	    // The region may be from before a resize, when the screen was
	    // bigger.
	    정수 height = MIN(rect.y2, screen_Rows) - rect.y1;
	    정수 width = MIN(rect.x2, screen_Columns) - rect.x1;

	    // Tip: if you want to debug stale pixels appearing, set "inverse"
	    // of screen_draw_rectangle() to TRUE.
	    만약 (height > 0 && width > 0)
		screen_draw_rectangle(rect.y1, rect.x1, height, width,
			FALSE, TRUE);
	    continue;
	}

	위해 (정수 r = rect.y1; r < rect.y2; r++)
	{
	    만약 (r >= screen_Rows)
		break;

	    위해 (정수 c = rect.x1; c < rect.x2; c++)
	    {
		만약 (c >= screen_Columns)
		    break;

		ScreenAttrs[LineOffset[r] + c] = (sattr_T)-1;
	    }
	}
    }

    만약 (restore)
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
    정수			pending_len = -1;
    // Save current cursor position
    정수			cur_row = screen_cur_row;
    정수			cur_col = screen_cur_col;

    // Check if all placements have no backing image. If so, then do nothing
    FOR_ALL_PLACEMENTS(place)
	만약 (place->img != NULL)
	{
	    pending_len = 0;
	    break;
	}

    만약 (pending_len == -1)
	반환;

    만약 (!backend_available(false))
	반환;

    만약 (n_placements == 0)
	반환;

    pending_placements = ALLOC_CLEAR_MULT(image_placement_T *, n_placements);
    만약 (pending_placements == NULL)
	반환;

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

	정수 x, y;

	만약 (place->flags & IMAGEPF_HIDDEN || !(place->flags & IMAGEPF_ENABLED))
	    // Don't need to clear image placement for non-blit backends,
	    // because we call image_placement_clear() every time we don't want
	    // to see the image. Not sure of the side effects if we also clear
	    // it here...
	    continue;

	만약 (img != NULL)
	{
	    pixman_region32_t	visible_abs;
	    정수			crop_w, crop_h;
	    bool		cs_changed;
	    bool		need_redraw;

	    // Processing changed image data failed, skip this image
	    만약 (place->img->data == NULL)
	    {
		image_placement_clear(place);
		continue;
	    }

	    // Only apply row shift if image is within the shifted region.
	    만약 ((place->bounding_box.y1 >= shift_top
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
	    만약 (!pixman_region32_intersect_rect(&image_region, &image_region,
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
	    만약 (cs_changed)
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

	    만약 (place->img_ver != img->ver)
	    {
		// Image data changed
		need_redraw = true;
		place->img_ver = img->ver;

		image_placement_clear(place);
		PLACEMENT_FUNC(image_backend, uninit)(place);
		만약 (PLACEMENT_FUNC(image_backend, init)(place) == FAIL)
		{
		    emsg(_(e_out_of_memory));
		    image_unref(place->img);
		    place->img = NULL;
		    continue;
		}
	    }

	    만약 (need_redraw)
	    {
		만약 (place->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&place->visible);
		place->visible = visible_region;

		만약 (PLACEMENT_FUNC(image_backend, blit))
		{
		    // Must redraw the stale regions that will not be composited
		    // over (for this specific image).
		    pixman_region32_t stale_region;

		    pixman_region32_init(&stale_region);

		    만약 (place->flags & IMAGEPF_VISIBLE_INIT)
		    {
			(void)pixman_region32_subtract(&stale_region,
				&place->visible_abs, &visible_abs);

			// Since pixels may be translucent/transparent, we must
			// redraw the cells behind the image every time, so that
			// no stale pixels remain from the previous redraw.
			만약 (img->fmt == IMAGE_FORMAT_RGBA)
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
		    만약 (img->fmt != IMAGE_FORMAT_RGBA)
		    {
			정수		    min_w, min_h;
			정수		    rect_w, rect_h;
			pixman_region32_t   min_region;

			image_get_cell_dimensions_min(img, &min_w, &min_h);

			// Ensure "min_region" only covers the fully covered
			// cells of the cropped region.
			rect_w = min_w - place->crop_box.x1;
			rect_h = min_h - place->crop_box.y1;

			만약 (rect_w > 0 && rect_h > 0)
			    // pixman_region32_init_rect() expects a non zero
			    // width and height (otherwise it outputs error
			    // message).
			    pixman_region32_init_rect(&min_region,
				    x, y, rect_w, rect_h);
			아니면
			    pixman_region32_init(&min_region);

			(void)pixman_region32_subtract(&min_region,
				&visible_abs, &min_region);
			(void)pixman_region32_union(&stale_region,
				&stale_region, &min_region);
			pixman_region32_fini(&min_region);
		    }

		    만약 (place->flags & IMAGEPF_VISIBLE_INIT)
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
		만약 (place->flags & IMAGEPF_VISIBLE_INIT)
		    pixman_region32_fini(&place->visible_abs);
		place->visible_abs = visible_abs;

		pending_placements[pending_len++] = place;
		place->flags |= IMAGEPF_VISIBLE_INIT;
		place->flags &= ~IMAGEPF_DIRTY;
	    }
	    아니면
	    {
		pixman_region32_fini(&visible_region);
		pixman_region32_fini(&visible_abs);
	    }
	}

	// If this is last image, no need to union its bounds.
	만약 (place->next == NULL)
	    break;

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

    만약 (pending_len > 0)
    {
#ifdef FEAT_GUI
	// Make sure to flush any output! We write directly to the draw
	// area/whatever, meaning draw commands outputted before this (as escape
	// sequences), will still draw over the image surface if we don't flush
	// now.
	만약 (gui.in_use)
	    out_flush();
#endif

	위해 (정수 i = 0; i < pending_len; i++)
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
mark_dirty_region_for_images(정수 row, 정수 col, 정수 row_height, 정수 col_width)
{
    image_placement_T	*place;
    pixman_box32_t	rect;

    만약 (freeze_mark_dirty > 0 || !backend_available(false)
	    || !PLACEMENT_FUNC(image_backend, blit) || n_placements == 0)
	반환;

    rect.x1 = col;
    rect.y1 = row;
    rect.x2 = col + col_width;
    rect.y2 = row + row_height;

    FOR_ALL_PLACEMENTS(place)
    {
	만약 (place->flags & IMAGEPF_HIDDEN)
	    continue;
	만약 (pixman_region32_contains_rectangle(&place->visible_abs, &rect)
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
    만약 (!updating_screen)
	redraw_all_later(UPD_VALID);
}

/*
 * Used for screen_ins_lines() and screen_del_lines(), because they directly
 * shift rows. Note that this does not queue a screen redraw.
 */
    void
shift_image_placements(정수 top, 정수 bot, 정수 amount)
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
	만약 (img->state == IMAGE_STATE_PUBLIC && img->id == id)
	    반환 img;
    반환 NULL;
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

    만약 (find && dict_has_key(dict, "id"))
    {
	image_T *img = find_image((int_u)dict_get_number(dict, "id"));

	만약 (img == NULL)
	    반환 NULL;
	반환 image_ref(img);
    }

    di = dict_find(dict, (char_u *)"data", -1);
    w = dict_get_number(dict, "width");
    h = dict_get_number(dict, "height");

    만약 (di == NULL || di->di_tv.v_type != VAR_BLOB || w <= 0 || h <= 0)
    {
	emsg(_(e_invalid_argument));
	반환 NULL;
    }

    // Check for overflow
    n_pixels = w * h;
    만약 (w > INT_MAX || h > INT_MAX || n_pixels > INT_MAX)
    {
	emsg(_(e_invalid_image_dimensions));
	반환 NULL;
    }

    data = di->di_tv.vval.v_blob;

    만약 (blob_len(data) == n_pixels * 3)
	fmt = IMAGE_FORMAT_RGB;
    아니면 만약 (blob_len(data) == n_pixels * 4)
	fmt = IMAGE_FORMAT_RGBA;
    아니면
    {
	semsg(_(e_invalid_value_for_argument_str_str), "data",
		"data length must equal width*height*3 or width*height*4");
	반환 NULL;
    }

    만약 (existing != NULL)
    {
	만약 (existing->width == w && existing->height == h
		&& existing->fmt == fmt)
	{
	    만약 (image_update(existing, data->bv_ga.ga_data) == FAIL)
		반환 NULL;
	    반환 existing;
	}
    }

    반환 image_new(data->bv_ga.ga_data, w, h, fmt);
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
    정수		w, h;
    dictitem_T	*di;

    만약 (img->data == NULL)
	반환 NULL;

    dict = dict_alloc();
    만약 (dict == NULL)
	반환 NULL;

    blob = blob_alloc();
    만약 (blob == NULL)
    {
	dict_unref(dict);
	반환 NULL;
    }

    image_get_dimensions(img, &w, &h);
    (void)ga_concat_bytes(&blob->bv_ga, (char *)img->data,
	    (size_t)w * h * img->fmt);

    di = dictitem_alloc((char_u *)"data");

    만약 (di != NULL)
    {
	di->di_tv.v_type = VAR_BLOB;
	di->di_tv.vval.v_blob = blob;
    }
    만약 (di == NULL || dict_add(dict, di) == FAIL)
    {
	만약 (di != NULL)
	    dictitem_free(di);
	dict_unref(dict);
	// No need to unref blob, dictitem_free() will do that.
	반환 NULL;
    }

    만약 (img->state == IMAGE_STATE_PUBLIC)
	dict_add_number(dict, "id", img->id);
    dict_add_number(dict, "width", img->width);
    dict_add_number(dict, "height", img->height);
    dict_add_number(dict, "alpha", img->fmt == IMAGE_FORMAT_RGBA);
    dict_add_string(dict, "format",
	    (char_u *)(img->fmt == IMAGE_FORMAT_RGB ? "rgb" : "rgba"));

    blob->bv_refcount = 1;

    반환 dict;
}

    static 정수
match_imageprotocol(image_backend_T *backend)
{
    정수		    len = (정수)STRLEN(p_ipc) + 1;
    char_u	    *buf = alloc(len);
    char_u	    *p = p_ipc;
    정수		    ret = FAIL;
    image_backend_T res = IMAGE_BACKEND_NONE;
    bool	    got = false;

    만약 (buf == NULL)
	반환 FAIL;

    동안 (*p != NUL)
    {
	char_u		*colon;
	regmatch_T	regmatch;
	image_backend_T prot = IMAGE_BACKEND_NONE;

	// Isolate one comma separated item.
	(void)copy_option_part(&p, buf, len, ",");

	colon = vim_strchr(buf, ':');
	만약 (colon == NULL || colon == buf || colon[1] == NUL)
	    goto exit;

	*colon = NUL;

	// Note: Keep this in sync with p_ipc_protocol_values.
	만약 (STRCMP(colon + 1, "none") == 0)
	    prot = IMAGE_BACKEND_NONE;
	아니면 만약 (STRCMP(colon + 1, "kitty") == 0)
	{
#ifdef FEAT_IMAGE_KITTY
	    prot = IMAGE_BACKEND_KITTY;
#endif
	}
	아니면 만약 (STRCMP(colon + 1, "sixel") == 0)
	{
#ifdef FEAT_IMAGE_SIXEL
	    prot = IMAGE_BACKEND_SIXEL;
#endif
	}
	아니면
	    goto exit;

	만약 (got)
	    continue;

	CLEAR_FIELD(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);

	만약 (regmatch.regprog == NULL)
	    goto exit;

	bool match = T_NAME != NULL
	    && vim_regexec(&regmatch, T_NAME, (colnr_T)0);

	vim_regfree(regmatch.regprog);
	만약 (match)
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
    반환 ret;
}

/*
 * Update the current image backend to use depending on 'imageprotocol' and if
 * GUI is being used. Returns OK on success and FAIL on failure.
 */
    정수
update_image_backend(void)
{
    image_backend_T new;

    image_T		*img;
    image_placement_T	*place;

    // Always process 'imageprotocol' option to check for errors
    만약 (match_imageprotocol(&new) == FAIL)
	반환 FAIL;

#ifdef FEAT_IMAGE_GUI
    만약 (gui.in_use)
	new = IMAGE_BACKEND_GUI;
#endif

    만약 (image_backend == new)
	반환 OK;

    // Must uninit the backends of every image/placement, then init the new
    // backend for each.
    FOR_ALL_IMAGES(img)
    {
	만약 (image_backend != IMAGE_BACKEND_NONE && img->data != NULL
		&& img->backend_data != NULL)
	    IMAGE_FUNC(image_backend, uninit)(img);
	img->backend_data = NULL;
	만약 (new != IMAGE_BACKEND_NONE)
	{
	    만약 (IMAGE_FUNC(new, init)(img) == FAIL)
		goto fail;
	    img->backend = new;
	}
    }
    FOR_ALL_PLACEMENTS(place)
    {
	image_placement_clear(place);
	만약 (image_backend != IMAGE_BACKEND_NONE
		&& place->img != NULL && place->backend_data != NULL)
	    PLACEMENT_FUNC(image_backend, uninit)(place);
	place->backend_data = NULL;
	만약 (new != IMAGE_BACKEND_NONE && place->img != NULL)
	{
	    만약 (PLACEMENT_FUNC(new, init)(place) == FAIL)
		goto fail;
	    place->backend = new;
	}
    }

    redraw_all_later(UPD_VALID);
    image_backend = new;

    만약 (new == IMAGE_BACKEND_NONE)
	set_vim_var_string(VV_IMAGEBACKEND, (char_u *)"none", -1);
    아니면
	set_vim_var_string(VV_IMAGEBACKEND,
		(char_u *)image_backends[image_backend].name, -1);

    반환 OK;
fail:
    FOR_ALL_IMAGES(img)
	만약 (img->backend != IMAGE_BACKEND_NONE)
	{
	    IMAGE_FUNC(img->backend, uninit)(img);
	    img->backend = IMAGE_BACKEND_NONE;
	}

    FOR_ALL_PLACEMENTS(place)
    {
	만약 (place->img != NULL && place->backend != IMAGE_BACKEND_NONE)
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
    반환 FAIL;
}

    void
f_image_add(typval_T *argvars, typval_T *rettv)
{
    dict_T  *dict;
    image_T *img;

    만약 (check_for_dict_arg(argvars, 0) == FAIL)
	반환;

    dict = argvars[0].vval.v_dict;

    img = add_image(dict, NULL, false);
    만약 (img != NULL)
	img->state = IMAGE_STATE_PUBLIC;

    rettv->v_type = VAR_NUMBER;
    rettv->vval.v_number = img == NULL ? (varnumber_T)-1 : img->id;
}

    void
f_image_discard(typval_T *argvars, typval_T *rettv UNUSED)
{
    image_T *img;

    만약 (check_for_number_arg(argvars, 0) == FAIL)
	반환;

    img = find_image((int_u)argvars[0].vval.v_number);

    만약 (img != NULL)
    {
	img->state = IMAGE_STATE_PRIVATE;
	image_unref(img);
    }
    아니면
	semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_number);
}

    void
f_image_info(typval_T *argvars, typval_T *rettv)
{
    image_T *img;
    dict_T  *dict;

    만약 (rettv_list_alloc(rettv) == FAIL
	    || check_for_opt_number_arg(argvars, 0) == FAIL)
	반환;

    만약 (argvars[0].v_type == VAR_NUMBER)
    {
	img = find_image((int_u)argvars[0].vval.v_number);
	만약 (img == NULL)
	{
	    semsg(_(e_image_id_nr_does_not_exist), argvars[0].vval.v_number);
	    반환;
	}
	dict = get_image_info(img);
	만약 (dict != NULL && list_append_dict(rettv->vval.v_list, dict) == FAIL)
	    dict_unref(dict);
	반환;
    }

    FOR_ALL_IMAGES(img)
    {
	만약 (img->state == IMAGE_STATE_PRIVATE)
	    continue;
	dict = get_image_info(img);
	만약 (dict == NULL
		|| list_append_dict(rettv->vval.v_list, dict) == FAIL)
	{
	    dict_unref(dict);	// NULL-safe
	    반환;
	}
    }
}

#endif // FEAT_IMAGE
