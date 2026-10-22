/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * sixel.c: RGB to DEC sixel encoder, by Yasuhiro Matsumoto.
 *
 * The encoder is intentionally self-contained (no libsixel dependency).
 * The fast path builds a palette on the fly from unique 24-bit colors and
 * fails over to a fixed 6x6x6 + grayscale 256-color palette when the input
 * has more colors than fit in the dynamic palette.
 *
 * Algorithm reference: github.com/mattn/go-sixel (sixel.go).
 *
 * Pipeline is split into three stages so a caller can quantize once and
 * cheaply re-encode arbitrary sub-rectangles without re-quantizing:
 *
 *   1. sixel_image_quantize()  RGB(A) -> 8bpp paletted image (sixel_image8_T)
 *   2. sixel_image8_crop()     8bpp image -> a stride-based view (sixel_view_T),
 *                              no pixel data is copied
 *   3. sixel_encode_view()     view -> sixel DCS byte sequence
 */

#अन्तर्भाव "vim.h"

#जर defined(FEAT_IMAGE_SIXEL)

// Palette size cap (sixel allows up to 256 color registers; index 0 is
// reserved as a transparent key, so usable colors are 1..MAX_COLORS).
#define SIXEL_MAX_COLORS    255

/*
 * Working buffer for one band (6 pixel rows). Allocated as
 * width * (palette_size+1) bytes; bit p (0..5) marks pixel at row offset p.
 */
प्रकारव्याख्या संरचना
{
    char_u  *bits;	// bitmask buffer, width * paletteSize bytes
    माप_प्रकार   bits_len;	// allocated bytes in "bits"
    पूर्णांक	    *seen;	// per-color "used in this band" flags
    पूर्णांक	    *used;	// colors used in this band
    पूर्णांक	    *xmin;	// leftmost x used by color in this band
    पूर्णांक	    *xmax;	// rightmost x used by color in this band
    पूर्णांक	     color_len;	// allocated length for per-color arrays
    पूर्णांक	     seen_gen;	// generation counter to avoid memset
} sixel_band_T;

प्रकारव्याख्या संरचना
{
    अचिन्हित पूर्णांक    *किल्ल्या;	// hash keys for dynamic palette lookup
    char_u	    *vals;	// hash values for dynamic palette lookup
    पूर्णांक		     hash_cap;	// allocated slots in keys/vals
    char_u	    *idx;	// width * height paletted image
    माप_प्रकार	     idx_len;	// allocated bytes in idx
    char_u	    *pal;	// dynamic palette storage (RGB triples)
    पूर्णांक		     pal_len;	// allocated bytes in pal
    sixel_band_T     band;	// reusable band scratch
} sixel_state_T;

/*
 * A fully quantized (8bpp) image: one palette index per pixel, tightly
 * packed (stride == width), plus the palette that goes with it.
 */
प्रकारव्याख्या संरचना
{
    char_u  *idx; // Is width*height bytes, indices 1..npal (0 = transparent)
    पूर्णांक      width;
    पूर्णांक      height;
    char_u  *pal; // Is npal*3 bytes, RGB triples
    पूर्णांक      npal;
} sixel_image8_T;

/*
 * A view into a sixel_image8_T: an offset + stride,
 */
प्रकारव्याख्या संरचना
{
    char_u  *idx; // Points at (x0,y0) inside the parent buffer
    पूर्णांक      stride; // Row length of the *parent* image, not this view
    पूर्णांक      width;
    पूर्णांक      height;
    char_u  *pal;
    पूर्णांक      npal;
} sixel_view_T;

प्रकारव्याख्या संरचना
{
    // These are offsets relative to the top left of the image.
    पूर्णांक	    row_off;
    पूर्णांक	    col_off;
    char_u  *seq[2]; // Header + body
} sixel_chunk_T;

प्रकारव्याख्या संरचना
{
    sixel_image8_T image8;
} image_sixel_T;

प्रकारव्याख्या संरचना
{
    pixman_region32_t	visible_region; // Region used for the previous redraw
    bool		visible_init;
    sixel_chunk_T	*chunks;    // Cached sixel sequence (stored as multiple
				    // chunks).
    पूर्णांक			n_chunks;
} image_placement_sixel_T;

स्थिर char_u		sixel_fixed_palette[240 * 3];
स्थिर char_u		sixel_rgb_cube_idx[256];
स्थिर bool		sixel_fixed_tables_ready = false;
स्थिर sixel_state_T	sixel_state;
स्थिर bool		sent_decsdm = false;

/*
 * Release all module-level allocations cached by the sixel encoder.
 */
    शून्य
sixel_uninit(शून्य)
{
    sixel_band_T *band = &sixel_state.band;

    VIM_CLEAR(sixel_state.किल्ल्या);
    VIM_CLEAR(sixel_state.vals);
    VIM_CLEAR(sixel_state.idx);
    VIM_CLEAR(sixel_state.pal);
    sixel_state.hash_cap = 0;
    sixel_state.idx_len = 0;
    sixel_state.pal_len = 0;

    VIM_CLEAR(band->bits);
    VIM_CLEAR(band->seen);
    VIM_CLEAR(band->used);
    VIM_CLEAR(band->xmin);
    VIM_CLEAR(band->xmax);
    band->bits_len = 0;
    band->color_len = 0;
    band->seen_gen = 0;
}

/*
 * Initialize the shared fixed sixel palette and RGB->cube lookup table.
 */
    स्थिर शून्य
sixel_init_fixed_tables(शून्य)
{
    पूर्णांक	    r, g, b, gr, n;

    जर (sixel_fixed_tables_ready)
	परत;

    n = 0;
    साठी (r = 0; r < 6; r++)
	साठी (g = 0; g < 6; g++)
	    साठी (b = 0; b < 6; b++)
	    {
		sixel_fixed_palette[n * 3]     = (char_u)(r * 51);
		sixel_fixed_palette[n * 3 + 1] = (char_u)(g * 51);
		sixel_fixed_palette[n * 3 + 2] = (char_u)(b * 51);
		n++;
	    }
    साठी (gr = 0; gr < 24; gr++)
    {
	पूर्णांक v = 8 + gr * 10;

	sixel_fixed_palette[n * 3]     = (char_u)v;
	sixel_fixed_palette[n * 3 + 1] = (char_u)v;
	sixel_fixed_palette[n * 3 + 2] = (char_u)v;
	n++;
    }
    साठी (n = 0; n < 256; n++)
    {
	पूर्णांक idx = n / 43;

	sixel_rgb_cube_idx[n] = (char_u)(idx > 5 ? 5 : idx);
    }
    sixel_fixed_tables_ready = true;
}

    स्थिर पूर्णांक
sixel_ensure_hash_capacity(पूर्णांक max_colors, पूर्णांक *cap_out)
{
    पूर्णांक cap = 1024;

    पर्यंत (cap < max_colors * 4)
	cap <<= 1;
    जर (cap > sixel_state.hash_cap)
    {
	अचिन्हित पूर्णांक	*किल्ल्या;
	char_u		*vals;

	किल्ल्या = ALLOC_MULT(अचिन्हित पूर्णांक, cap);
	vals = ALLOC_MULT(char_u, cap);
	जर (किल्ल्या == रिक्त || vals == रिक्त)
	{
	    vim_free(किल्ल्या);
	    vim_free(vals);
	    परत FAIL;
	}
	जर (sixel_state.hash_cap > 0)
	{
	    mch_memmove(किल्ल्या, sixel_state.किल्ल्या,
		    (माप_प्रकार)sixel_state.hash_cap * आकार(*किल्ल्या));
	    mch_memmove(vals, sixel_state.vals,
		    (माप_प्रकार)sixel_state.hash_cap * आकार(*vals));
	}
	vim_free(sixel_state.किल्ल्या);
	vim_free(sixel_state.vals);
	sixel_state.किल्ल्या = किल्ल्या;
	sixel_state.vals = vals;
	sixel_state.hash_cap = cap;
    }
    vim_memset(sixel_state.किल्ल्या, 0, (माप_प्रकार)cap * आकार(*sixel_state.किल्ल्या));
    *cap_out = cap;
    परत OK;
}

    स्थिर पूर्णांक
sixel_ensure_idx_capacity(माप_प्रकार pixels)
{
    जर (pixels > sixel_state.idx_len)
    {
	char_u *idx = vim_realloc(sixel_state.idx, pixels);

	जर (idx == रिक्त)
	    परत FAIL;
	sixel_state.idx = idx;
	sixel_state.idx_len = pixels;
    }
    परत OK;
}

    स्थिर पूर्णांक
sixel_ensure_pal_capacity(पूर्णांक max_colors)
{
    पूर्णांक need = max_colors * 3;

    जर (need > sixel_state.pal_len)
    {
	char_u *pal = vim_realloc(sixel_state.pal, (माप_प्रकार)need);

	जर (pal == रिक्त)
	    परत FAIL;
	sixel_state.pal = pal;
	sixel_state.pal_len = need;
    }
    परत OK;
}

    स्थिर पूर्णांक
sixel_ensure_band_capacity(पूर्णांक width, पूर्णांक npal)
{
    माप_प्रकार	    bits_need = (माप_प्रकार)width * (npal + 1);
    sixel_band_T    *band = &sixel_state.band;

    जर (bits_need > band->bits_len)
    {
	char_u *bits = vim_realloc(band->bits, bits_need);

	जर (bits == रिक्त)
	    परत FAIL;
	band->bits = bits;
	band->bits_len = bits_need;
    }
    जर (npal + 1 > band->color_len)
    {
	पूर्णांक *seen = ALLOC_MULT(पूर्णांक, npal + 1);
	पूर्णांक *used = ALLOC_MULT(पूर्णांक, npal + 1);
	पूर्णांक *xmin = ALLOC_MULT(पूर्णांक, npal + 1);
	पूर्णांक *xmax = ALLOC_MULT(पूर्णांक, npal + 1);

	जर (seen == रिक्त || used == रिक्त || xmin == रिक्त || xmax == रिक्त)
	{
	    vim_free(seen);
	    vim_free(used);
	    vim_free(xmin);
	    vim_free(xmax);
	    परत FAIL;
	}
	जर (band->color_len > 0)
	{
	    mch_memmove(seen, band->seen,
		    (माप_प्रकार)band->color_len * आकार(*seen));
	    mch_memmove(used, band->used,
		    (माप_प्रकार)band->color_len * आकार(*used));
	    mch_memmove(xmin, band->xmin,
		    (माप_प्रकार)band->color_len * आकार(*xmin));
	    mch_memmove(xmax, band->xmax,
		    (माप_प्रकार)band->color_len * आकार(*xmax));
	}

	// seen[] is checked against the band's generation counter; if the
	// newly grown tail held garbage equal to the current generation, the
	// band loop would read uninitialised xmin/xmax.  Zero the new tail
	// (and the matching slots in used/xmin/xmax for good measure) so the
	// "first time this colour is touched" branch is always taken.
	{
	    माप_प्रकार tail = (माप_प्रकार)(npal + 1 - band->color_len);

	    vim_memset(seen + band->color_len, 0, tail * आकार(*seen));
	    vim_memset(used + band->color_len, 0, tail * आकार(*used));
	    vim_memset(xmin + band->color_len, 0, tail * आकार(*xmin));
	    vim_memset(xmax + band->color_len, 0, tail * आकार(*xmax));
	}
	vim_free(band->seen);
	vim_free(band->used);
	vim_free(band->xmin);
	vim_free(band->xmax);
	band->seen = seen;
	band->used = used;
	band->xmin = xmin;
	band->xmax = xmax;
	band->color_len = npal + 1;
    }
    परत OK;
}

/*
 * Build a paletted image from an RGB / RGBA buffer using on-the-fly hashing. On
 * success: *pal_out receives a malloced array of (npal*3) bytes (R,G,B triples)
 * and *idx_out receives a malloced array of width*height bytes (indices
 * 1..npal; 0 is reserved as transparent).
 *
 * Handles both RGB (3 bytes/pixel, has_alpha == false) and RGBA (4 bytes/
 * pixel, has_alpha == true).  Sixel cannot represent partial transparency, so
 * RGBA pixels are split at half coverage: alpha < 128 pixels are mapped to
 * palette index 0 -- which the sixel emitter never writes to the bitmask,
 * leaving the cell's underlying terminal contents visible (transparency).
 * Rendering them opaque instead would show the image's anti-aliased edge fringe
 * (mostly-transparent, often light-colored pixels) as bright dots around the
 * image.  Pixels with alpha >= 128 are deduplicated by their R,G,B triple,
 * ignoring the alpha value.
 *
 * Returns OK on success, FAIL when colors exceed max_colors (caller may fall
 * back to a fixed palette) or on OOM.
 */
    स्थिर पूर्णांक
rgb_to_paletted_fast(
	char_u	*pixels,
	पूर्णांक	 width,
	पूर्णांक	 height,
	bool	 has_alpha,
	पूर्णांक	 max_colors,
	char_u **pal_out,
	पूर्णांक	*npal_out,
	char_u **idx_out)
{
    पूर्णांक		     cap;
    पूर्णांक		     mask;
    पूर्णांक		     used = 0;
    अचिन्हित पूर्णांक    *किल्ल्या;
    char_u	    *vals;
    char_u	    *idx;
    char_u	    *pal;
    पूर्णांक		     i, n;
    पूर्णांक		     bpp = has_alpha ? 4 : 3;

    जर (sixel_ensure_hash_capacity(max_colors, &cap) == FAIL
	    || sixel_ensure_idx_capacity((माप_प्रकार)width * height) == FAIL
	    || sixel_ensure_pal_capacity(max_colors) == FAIL)
	परत FAIL;
    किल्ल्या = sixel_state.किल्ल्या;
    vals = sixel_state.vals;
    idx = sixel_state.idx;
    pal = sixel_state.pal;
    mask = cap - 1;

    n = width * height;
    साठी (i = 0; i < n; i++)
    {
	char_u		*p = pixels + (माप_प्रकार)i * bpp;
	अचिन्हित पूर्णांक	 किल्ली;
	अचिन्हित पूर्णांक	 h;
	पूर्णांक		 slot;

	जर (has_alpha && p[3] < 128)
	{
	    idx[i] = 0;	    // transparent -- terminal leaves cell as-is
	    सुरूठेवा;
	}
	किल्ली = ((अचिन्हित पूर्णांक)p[0] << 16)
	    | ((अचिन्हित पूर्णांक)p[1] << 8)
	    | (अचिन्हित पूर्णांक)p[2];
	h = किल्ली * 2654435761u;
	slot = (पूर्णांक)(h & mask);

	साठी (;;)
	{
	    जर (किल्ल्या[slot] == 0)
	    {
		// new color
		जर (used >= max_colors)
		    जा too_many;
		किल्ल्या[slot] = किल्ली + 1;
		vals[slot] = (char_u)(used + 1);    // 1-based palette index
		pal[used * 3]	  = p[0];
		pal[used * 3 + 1] = p[1];
		pal[used * 3 + 2] = p[2];
		used++;
		idx[i] = vals[slot];
		थांबा;
	    }
	    जर (किल्ल्या[slot] == किल्ली + 1)
	    {
		idx[i] = vals[slot];
		थांबा;
	    }
	    slot = (slot + 1) & mask;
	}
    }
    *pal_out = pal;
    *npal_out = used;
    *idx_out = idx;
    परत OK;

too_many:
    परत FAIL;
}

/*
 * Fallback: quantize pixels to a fixed 6x6x6 RGB cube + 24-step grayscale.
 * Always succeeds; produces 240 palette entries.
 *
 * Handles both RGB (3 bytes/pixel, has_alpha == false) and RGBA (4 bytes/
 * pixel, has_alpha == true).  For RGBA, alpha < 128 pixels are mapped to
 * palette index 0 -- which the sixel emitter never writes to the bitmask,
 * leaving the cell's underlying terminal contents visible (transparency); see
 * rgb_to_paletted_fast() for why the cut is at half coverage.
 */
    स्थिर पूर्णांक
rgb_to_paletted_fixed(
	char_u	*pixels,
	पूर्णांक	width,
	पूर्णांक	height,
	bool	has_alpha,
	char_u	**pal_out,
	पूर्णांक	*npal_out,
	char_u	**idx_out)
{
    char_u  *idx;
    पूर्णांक	     n;
    पूर्णांक	     bpp = has_alpha ? 4 : 3;

    जर (sixel_ensure_idx_capacity((माप_प्रकार)width * height) == FAIL)
	परत FAIL;
    idx = sixel_state.idx;

    sixel_init_fixed_tables();

    // map every pixel to the nearest cube cell
    साठी (n = 0; n < width * height; n++)
    {
	char_u	*p = pixels + (माप_प्रकार)n * bpp;
	पूर्णांक	 ri, gi, bi;

	जर (has_alpha && p[3] < 128)
	{
	    idx[n] = 0;
	    सुरूठेवा;
	}
	ri = sixel_rgb_cube_idx[p[0]];
	gi = sixel_rgb_cube_idx[p[1]];
	bi = sixel_rgb_cube_idx[p[2]];
	idx[n] = (char_u)(ri * 36 + gi * 6 + bi + 1);	// 1-based
    }

    *pal_out = sixel_fixed_palette;
    *npal_out = 240;
    *idx_out = idx;
    परत OK;
}

    स्थिर शून्य
sixel_image8_clear(sixel_image8_T *simg)
{
    vim_free(simg->idx);
    vim_free(simg->pal);
}

/*
 * Quantize an RGB(A) image to an 8bpp paletted image. Returns OK on success,
 * FAIL on invalid input or OOM.
 */
    स्थिर पूर्णांक
sixel_image_quantize(image_T *img, sixel_image8_T *out)
{
    char_u  *pal = रिक्त;
    char_u  *idx = रिक्त;
    पूर्णांक	     npal = 0;

    जर (rgb_to_paletted_fast(img->data, img->width, img->height,
		img->fmt == IMAGE_FORMAT_RGBA,
		SIXEL_MAX_COLORS, &pal, &npal, &idx) == FAIL)
    {
	जर (rgb_to_paletted_fixed(img->data, img->width, img->height,
		    img->fmt == IMAGE_FORMAT_RGBA,
		    &pal, &npal, &idx) == FAIL)
	    परत FAIL;
    }

    out->idx = vim_memsave(idx, (माप_प्रकार)img->width * img->height);
    out->width = img->width;
    out->height = img->height;
    out->pal = vim_memsave(pal, (माप_प्रकार)npal * 3);
    out->npal = npal;

    जर (out->pal == रिक्त || out->idx == रिक्त)
    {
	sixel_image8_clear(out);
	परत FAIL;
    }
    परत OK;
}

/*
 * Build a view of a sub-rectangle of an already-quantized image. No pixel data
 * is copied; "out" just points into "src" with an offset and the parent's
 * stride.  x/y/w/h are clamped to src's bounds, so a partially or fully
 * out-of-range rectangle is silently shrunk rather than reading out of bounds;
 * a rectangle with no overlap at all yields a zero-sized view (width==0 or
 * height==0), which sixel_encode_view() rejects. Returns OK on success and FAIL
 * on failure.
 */
    स्थिर पूर्णांक
sixel_image8_crop(
	sixel_image8_T	*src,
	पूर्णांक		 x,
	पूर्णांक		 y,
	पूर्णांक		 w,
	पूर्णांक		 h,
	sixel_view_T	*out)
{
    पूर्णांक x0, y0, x1, y1;   // clamped rectangle, half-open [x0,x1) x [y0,y1)

    जर (src == रिक्त || out == रिक्त)
	परत FAIL;

    x0 = x < 0 ? 0 : x;
    y0 = y < 0 ? 0 : y;
    x1 = x + w;
    y1 = y + h;
    जर (x1 > src->width)
	x1 = src->width;
    जर (y1 > src->height)
	y1 = src->height;
    जर (x1 < x0)
	x1 = x0;
    जर (y1 < y0)
	y1 = y0;

    जर (x1 <= x0 || y1 <= y0)
	परत FAIL;

    out->stride = src->width;
    out->idx    = (x1 > x0 && y1 > y0)
	? src->idx + (माप_प्रकार)y0 * src->width + x0
	: रिक्त;
    out->width  = x1 - x0;
    out->height = y1 - y0;
    out->pal    = src->pal;
    out->npal   = src->npal;

    परत OK;
}

/*
 * Emit a run of `cnt` identical sixel data bytes (ch in 0..63) into gap.
 * Uses RLE form `!N{c}` when it shortens the output.
 */
    स्थिर पूर्णांक
emit_run(garray_T *gap, char_u ch, पूर्णांक cnt)
{
    char_u  c = (char_u)(63 + ch);

    पर्यंत (cnt > 255)
    {
	जर (ga_concat_bytes(gap, "!255", 4) == FAIL
		|| ga_append(gap, c) == FAIL)
	    परत FAIL;
	cnt -= 255;
    }
    जर (cnt <= 0)
	परत OK;
    जर (cnt <= 3)
    {
	पर्यंत (cnt-- > 0)
	    जर (ga_append(gap, c) == FAIL)
		परत FAIL;
	परत OK;
    }
    जर (ga_append(gap, '!') == FAIL
	    || ga_concat_int(gap, cnt) == FAIL
	    || ga_append(gap, c) == FAIL)
	परत FAIL;
    परत OK;
}

/*
 * Encode a view of an 8bpp paletted image into a sixel DCS sequence. "view" may
 * be a crop (view->stride >= view->width) or a full, uncropped image
 * (view->stride == view->width).
 *
 * Stores header string in "seq[0]" and body in "seq[1]", all malloced strings.
 * Note that check if each string is NULL! Probably can't happen but be safe.
 * Returns FAIL on failure.
 */
    स्थिर पूर्णांक
sixel_encode_view(sixel_view_T *view, char_u *seq[2])
{
    garray_T	    hdr;    // DCS introducer, raster attrs, used-color defs
    garray_T	    body;   // band data (colour selects + sixel bytes)
    sixel_band_T    *band_state = &sixel_state.band;
    char_u	    *idx;
    char_u	    *pal;
    पूर्णांक		    npal;
    पूर्णांक		    width, height;
    पूर्णांक		    band, p, x, n;
    bool	    defined[256];   // register already defined in header

    idx = view->idx;
    pal = view->pal;
    npal = view->npal;
    width = view->width;
    height = view->height;

    ga_init2(&hdr, आकार(char_u), 256);
    ga_init2(&body, आकार(char_u), 4096);
    vim_memset(defined, 0, आकार(defined));

    // DECSIXEL Introducer + Raster Attributes "1;1;W;H.
    // P2=1 means pixel positions left unspecified by any colour register
    // keep their previous on-screen contents instead of being painted with
    // colour register 0.  That gives true transparency for RGBA images:
    // alpha < 128 pixels are emitted as palette index 0 (which we never
    // write to the bitmask), so the terminal leaves the popup's underlying
    // cell colour visible there -- no flatten, no colour-match drift.
    जर (ga_concat_bytes(&hdr, "\033P0;1;8q\"1;1;", 13) == FAIL
	    || ga_concat_int(&hdr, width) == FAIL
	    || ga_append(&hdr, ';') == FAIL
	    || ga_concat_int(&hdr, height) == FAIL)
	जा fail;

    // bitmask buffer: (crop) width bytes per palette index, +1 for the
    // unused index 0.  Sized off the view's width, not the parent image's
    // width, so encoding a small crop out of a large quantized image only
    // needs scratch space proportional to the crop.
    जर (sixel_ensure_band_capacity(width, npal) == FAIL)
	जा fail;

    साठी (band = 0; band < (height + 5) / 6; band++)
    {
	पूर्णांक gen;
	पूर्णांक last_was_cr = 0;
	पूर्णांक used_मोजणी = 0;

	जर (++band_state->seen_gen <= 0)
	{
	    vim_memset(band_state->seen, 0, (npal + 1) * आकार(पूर्णांक));
	    band_state->seen_gen = 1;
	}
	gen = band_state->seen_gen;

	जर (band > 0 && ga_append(&body, '-') == FAIL)
	    जा fail;

	// Fill the bitmask for this band.
	साठी (p = 0; p < 6; p++)
	{
	    पूर्णांक y = band * 6 + p;
	    char_u  rowmask = (char_u)(1 << p);
	    char_u  *row;

	    जर (y >= height)
		सुरूठेवा;
	    // Use the *parent* stride to find the row -- "idx" already
	    // points at the crop's top-left corner, so only the row-to-row
	    // step needs to know about the parent's real width.
	    row = idx + (माप_प्रकार)y * view->stride;
	    साठी (x = 0; x < width; x++)
	    {
		char_u  pix = row[x];

		जर (pix == 0)
		    सुरूठेवा;
		जर (band_state->seen[pix] != gen)
		{
		    band_state->seen[pix] = gen;
		    band_state->used[used_मोजणी++] = pix;
		    band_state->xmin[pix] = x;
		    band_state->xmax[pix] = x;
		    vim_memset(band_state->bits + (माप_प्रकार)pix * width, 0, width);
		}
		अन्यथा
		{
		    जर (x < band_state->xmin[pix])
			band_state->xmin[pix] = x;
		    जर (x > band_state->xmax[pix])
			band_state->xmax[pix] = x;
		}
		band_state->bits[(माप_प्रकार)pix * width + x] |= rowmask;
	    }
	}

	साठी (n = 0; n < used_मोजणी; n++)
	{
	    पूर्णांक	     pix = band_state->used[n];
	    char_u  *row;
	    char_u   ch0;
	    पूर्णांक	     cnt;
	    पूर्णांक	     start = band_state->xmin[pix];
	    पूर्णांक	     end = band_state->xmax[pix];

	    // First time this colour is used in the subrect: add its
	    // definition "#N;2;R;G;B" (RGB scaled to 0..100) to the header.
	    // Colours the subrect never touches are never defined.  Round to
	    // nearest, not truncate, so the round-trip 8bit -> 0..100 -> 8bit
	    // stays within ~1 level instead of drifting up to 2-3 levels
	    // darker.  This matters when the popup blends RGBA alpha onto the
	    // terminal background: truncation made the flattened bg visibly
	    // darker than the surrounding terminal cells.
	    जर (!defined[pix])
	    {
		पूर्णांक r = (pal[(pix - 1) * 3]     * 100 + 127) / 255;
		पूर्णांक g = (pal[(pix - 1) * 3 + 1] * 100 + 127) / 255;
		पूर्णांक b = (pal[(pix - 1) * 3 + 2] * 100 + 127) / 255;

		defined[pix] = true;
		जर (ga_append(&hdr, '#') == FAIL
			|| ga_concat_int(&hdr, pix) == FAIL
			|| ga_concat_bytes(&hdr, ";2;", 3) == FAIL
			|| ga_concat_int(&hdr, r) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, g) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, b) == FAIL)
		    जा fail;
	    }

	    जर (last_was_cr && ga_append(&body, '$') == FAIL)
		जा fail;
	    जर (ga_append(&body, '#') == FAIL
		    || ga_concat_int(&body, pix) == FAIL)
		जा fail;

	    row = band_state->bits + (माप_प्रकार)pix * width;
	    जर (start > 0 && emit_run(&body, 0, start) == FAIL)
		जा fail;
	    ch0 = row[start];
	    cnt = 1;
	    साठी (x = start + 1; x <= end; x++)
	    {
		char_u  ch = row[x];

		जर (ch == ch0)
		{
		    cnt++;
		    सुरूठेवा;
		}
		जर (emit_run(&body, ch0, cnt) == FAIL)
		    जा fail;
		ch0 = ch;
		cnt = 1;
	    }
	    जर (emit_run(&body, ch0, cnt) == FAIL)
		जा fail;
	    last_was_cr = 1;
	}
    }

    जर (ga_append(&hdr, NUL) == FAIL
	    || ga_concat_bytes(&body, "\033\\", 3) == FAIL)
	जा fail;

    seq[0] = (char_u *)hdr.ga_data;
    seq[1] = (char_u *)body.ga_data;

    परत OK;

fail:
    ga_clear(&hdr);
    ga_clear(&body);
    परत FAIL;
}

    पूर्णांक
image_sixel_init(image_T *img)
{
    image_sixel_T *ctx;

    ctx = ALLOC_CLEAR_ONE(image_sixel_T);
    जर (ctx == रिक्त || sixel_image_quantize(img, &ctx->image8) == FAIL)
    {
	vim_free(ctx);
	परत FAIL;
    }

#जर defined(FEAT_IMAGE_SIXEL) && defined(FEAT_TERMRESPONSE)
    जर (!sent_decsdm)
    {
	// Disable sixel scrolling using DECSDM
	out_str((char_u *)"\033[?80l");
	sent_decsdm = true;
    }
#endif

    img->backend_data = ctx;
    परत OK;
}

    शून्य
image_sixel_uninit(image_T *img)
{
    image_sixel_T *ctx = img->backend_data;

    sixel_image8_clear(&ctx->image8);
    vim_free(img->backend_data);
}

    पूर्णांक
image_placement_sixel_init(image_placement_T *place)
{
    image_placement_sixel_T *ctx = ALLOC_CLEAR_ONE(image_placement_sixel_T);

    जर (ctx == रिक्त)
	परत FAIL;
    place->backend_data = ctx;
    परत OK;
}

    स्थिर शून्य
clear_chunks(image_placement_sixel_T *ctx)
{
    जर (ctx->chunks == रिक्त)
	परत;

    साठी (पूर्णांक i = 0; i < ctx->n_chunks; i++)
    {
	vim_free(ctx->chunks[i].seq[0]);
	vim_free(ctx->chunks[i].seq[1]);
    }
    VIM_CLEAR(ctx->chunks);
    ctx->n_chunks = 0;
}

    शून्य
image_placement_sixel_uninit(image_placement_T *place)
{
    image_placement_sixel_T *ctx = place->backend_data;

    जर (ctx->visible_init)
	pixman_region32_fini(&ctx->visible_region);
    clear_chunks(ctx);
    vim_free(ctx);
}

    शून्य
image_placement_sixel_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_sixel_T	    *ictx = img->backend_data;
    image_placement_sixel_T *ctx = place->backend_data;
    sixel_view_T	    view;
    pixman_box32_t	    *rects;
    पूर्णांक			    n_rects;

    // Check if visible region is still the same, if so then use the cached
    // sixel sequences.
    जर (!(place->flags & IMAGEPF_FORCE) && ctx->visible_init
	    && pixman_region32_equal(&ctx->visible_region, &place->visible))
    {
	साठी (पूर्णांक i = 0; i < pixman_region32_n_rects(&ctx->visible_region); i++)
	{
	    sixel_chunk_T *chunk = ctx->chunks + i;

	    windgoto(place->row + place->row_off
		    + chunk->row_off, place->col + chunk->col_off);
	    जर (chunk->seq[0] != रिक्त)
		out_str(chunk->seq[0]);
	    जर (chunk->seq[1] != रिक्त)
		out_str(chunk->seq[1]);
	    screen_start();
	}
	परत;
    }

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    जर (rects == रिक्त || n_rects == 0)
	परत;

    clear_chunks(ctx);
    ctx->chunks = ALLOC_CLEAR_MULT(sixel_chunk_T, n_rects);

    जर (ctx->chunks == रिक्त)
	परत;

    साठी (पूर्णांक i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	पूर्णांक		row, col;
	पूर्णांक		x, y, w, h;
	sixel_chunk_T	*chunk = ctx->chunks + i;
	char_u		*seq[2];

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);

	जर (sixel_image8_crop(&ictx->image8, x, y, w, h, &view) == FAIL
		|| sixel_encode_view(&view, seq) == FAIL)
	    सुरूठेवा;

	windgoto(row, col);
	जर (seq[0] != रिक्त)
	    out_str(seq[0]);
	जर (seq[1] != रिक्त)
	    out_str(seq[1]);
	screen_start();

	chunk->row_off = rect.y1;
	chunk->col_off = rect.x1;
	chunk->seq[0] = seq[0];
	chunk->seq[1] = seq[1];
	ctx->n_chunks++;
    }

    जर (ctx->visible_init)
	pixman_region32_clear(&ctx->visible_region);
    pixman_region32_copy(&ctx->visible_region, &place->visible);
    ctx->visible_init = true;
}

    शून्य
image_placement_sixel_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_SIXEL
