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

#ضمن "vim.h"

#إذا defined(FEAT_IMAGE_SIXEL)

// Palette size cap (sixel allows up to 256 color registers; index 0 is
// reserved as a transparent key, so usable colors are 1..MAX_COLORS).
#define SIXEL_MAX_COLORS    255

/*
 * Working buffer for one band (6 pixel rows). Allocated as
 * width * (palette_size+1) bytes; bit p (0..5) marks pixel at row offset p.
 */
تعريفنوع بنية
{
    char_u  *bits;	// bitmask buffer, width * paletteSize bytes
    size_t   bits_len;	// allocated bytes in "bits"
    صحيح	    *seen;	// per-color "used in this band" flags
    صحيح	    *used;	// colors used in this band
    صحيح	    *xmin;	// leftmost x used by color in this band
    صحيح	    *xmax;	// rightmost x used by color in this band
    صحيح	     color_len;	// allocated length for per-color arrays
    صحيح	     seen_gen;	// generation counter to avoid memset
} sixel_band_T;

تعريفنوع بنية
{
    unsigned صحيح    *کلیلەکان;	// hash keys for dynamic palette lookup
    char_u	    *vals;	// hash values for dynamic palette lookup
    صحيح		     hash_cap;	// allocated slots in keys/vals
    char_u	    *idx;	// width * height paletted image
    size_t	     idx_len;	// allocated bytes in idx
    char_u	    *pal;	// dynamic palette storage (RGB triples)
    صحيح		     pal_len;	// allocated bytes in pal
    sixel_band_T     band;	// reusable band scratch
} sixel_state_T;

/*
 * A fully quantized (8bpp) image: one palette index per pixel, tightly
 * packed (stride == width), plus the palette that goes with it.
 */
تعريفنوع بنية
{
    char_u  *idx; // Is width*height bytes, indices 1..npal (0 = transparent)
    صحيح      width;
    صحيح      height;
    char_u  *pal; // Is npal*3 bytes, RGB triples
    صحيح      npal;
} sixel_image8_T;

/*
 * A view into a sixel_image8_T: an offset + stride,
 */
تعريفنوع بنية
{
    char_u  *idx; // Points at (x0,y0) inside the parent buffer
    صحيح      stride; // Row length of the *parent* image, not this view
    صحيح      width;
    صحيح      height;
    char_u  *pal;
    صحيح      npal;
} sixel_view_T;

تعريفنوع بنية
{
    // These are offsets relative to the top left of the image.
    صحيح	    row_off;
    صحيح	    col_off;
    char_u  *seq[2]; // Header + body
} sixel_chunk_T;

تعريفنوع بنية
{
    sixel_image8_T image8;
} image_sixel_T;

تعريفنوع بنية
{
    pixman_region32_t	visible_region; // Region used for the previous redraw
    bool		visible_init;
    sixel_chunk_T	*chunks;    // Cached sixel sequence (stored as multiple
				    // chunks).
    صحيح			n_chunks;
} image_placement_sixel_T;

ساكن char_u		sixel_fixed_palette[240 * 3];
ساكن char_u		sixel_rgb_cube_idx[256];
ساكن bool		sixel_fixed_tables_ready = false;
ساكن sixel_state_T	sixel_state;
ساكن bool		sent_decsdm = false;

/*
 * Release all module-level allocations cached by the sixel encoder.
 */
    فراغ
sixel_uninit(فراغ)
{
    sixel_band_T *band = &sixel_state.band;

    VIM_CLEAR(sixel_state.کلیلەکان);
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
    ساكن فراغ
sixel_init_fixed_tables(فراغ)
{
    صحيح	    r, g, b, gr, n;

    إذا (sixel_fixed_tables_ready)
	أرجع;

    n = 0;
    لكل (r = 0; r < 6; r++)
	لكل (g = 0; g < 6; g++)
	    لكل (b = 0; b < 6; b++)
	    {
		sixel_fixed_palette[n * 3]     = (char_u)(r * 51);
		sixel_fixed_palette[n * 3 + 1] = (char_u)(g * 51);
		sixel_fixed_palette[n * 3 + 2] = (char_u)(b * 51);
		n++;
	    }
    لكل (gr = 0; gr < 24; gr++)
    {
	صحيح v = 8 + gr * 10;

	sixel_fixed_palette[n * 3]     = (char_u)v;
	sixel_fixed_palette[n * 3 + 1] = (char_u)v;
	sixel_fixed_palette[n * 3 + 2] = (char_u)v;
	n++;
    }
    لكل (n = 0; n < 256; n++)
    {
	صحيح idx = n / 43;

	sixel_rgb_cube_idx[n] = (char_u)(idx > 5 ? 5 : idx);
    }
    sixel_fixed_tables_ready = true;
}

    ساكن صحيح
sixel_ensure_hash_capacity(صحيح max_colors, صحيح *cap_out)
{
    صحيح cap = 1024;

    طالما (cap < max_colors * 4)
	cap <<= 1;
    إذا (cap > sixel_state.hash_cap)
    {
	unsigned صحيح	*کلیلەکان;
	char_u		*vals;

	کلیلەکان = ALLOC_MULT(unsigned صحيح, cap);
	vals = ALLOC_MULT(char_u, cap);
	إذا (کلیلەکان == عدم || vals == عدم)
	{
	    vim_free(کلیلەکان);
	    vim_free(vals);
	    أرجع FAIL;
	}
	إذا (sixel_state.hash_cap > 0)
	{
	    mch_memmove(کلیلەکان, sixel_state.کلیلەکان,
		    (size_t)sixel_state.hash_cap * حجم(*کلیلەکان));
	    mch_memmove(vals, sixel_state.vals,
		    (size_t)sixel_state.hash_cap * حجم(*vals));
	}
	vim_free(sixel_state.کلیلەکان);
	vim_free(sixel_state.vals);
	sixel_state.کلیلەکان = کلیلەکان;
	sixel_state.vals = vals;
	sixel_state.hash_cap = cap;
    }
    vim_memset(sixel_state.کلیلەکان, 0, (size_t)cap * حجم(*sixel_state.کلیلەکان));
    *cap_out = cap;
    أرجع OK;
}

    ساكن صحيح
sixel_ensure_idx_capacity(size_t pixels)
{
    إذا (pixels > sixel_state.idx_len)
    {
	char_u *idx = vim_realloc(sixel_state.idx, pixels);

	إذا (idx == عدم)
	    أرجع FAIL;
	sixel_state.idx = idx;
	sixel_state.idx_len = pixels;
    }
    أرجع OK;
}

    ساكن صحيح
sixel_ensure_pal_capacity(صحيح max_colors)
{
    صحيح need = max_colors * 3;

    إذا (need > sixel_state.pal_len)
    {
	char_u *pal = vim_realloc(sixel_state.pal, (size_t)need);

	إذا (pal == عدم)
	    أرجع FAIL;
	sixel_state.pal = pal;
	sixel_state.pal_len = need;
    }
    أرجع OK;
}

    ساكن صحيح
sixel_ensure_band_capacity(صحيح width, صحيح npal)
{
    size_t	    bits_need = (size_t)width * (npal + 1);
    sixel_band_T    *band = &sixel_state.band;

    إذا (bits_need > band->bits_len)
    {
	char_u *bits = vim_realloc(band->bits, bits_need);

	إذا (bits == عدم)
	    أرجع FAIL;
	band->bits = bits;
	band->bits_len = bits_need;
    }
    إذا (npal + 1 > band->color_len)
    {
	صحيح *seen = ALLOC_MULT(صحيح, npal + 1);
	صحيح *used = ALLOC_MULT(صحيح, npal + 1);
	صحيح *xmin = ALLOC_MULT(صحيح, npal + 1);
	صحيح *xmax = ALLOC_MULT(صحيح, npal + 1);

	إذا (seen == عدم || used == عدم || xmin == عدم || xmax == عدم)
	{
	    vim_free(seen);
	    vim_free(used);
	    vim_free(xmin);
	    vim_free(xmax);
	    أرجع FAIL;
	}
	إذا (band->color_len > 0)
	{
	    mch_memmove(seen, band->seen,
		    (size_t)band->color_len * حجم(*seen));
	    mch_memmove(used, band->used,
		    (size_t)band->color_len * حجم(*used));
	    mch_memmove(xmin, band->xmin,
		    (size_t)band->color_len * حجم(*xmin));
	    mch_memmove(xmax, band->xmax,
		    (size_t)band->color_len * حجم(*xmax));
	}

	// seen[] is checked against the band's generation counter; if the
	// newly grown tail held garbage equal to the current generation, the
	// band loop would read uninitialised xmin/xmax.  Zero the new tail
	// (and the matching slots in used/xmin/xmax for good measure) so the
	// "first time this colour is touched" branch is always taken.
	{
	    size_t tail = (size_t)(npal + 1 - band->color_len);

	    vim_memset(seen + band->color_len, 0, tail * حجم(*seen));
	    vim_memset(used + band->color_len, 0, tail * حجم(*used));
	    vim_memset(xmin + band->color_len, 0, tail * حجم(*xmin));
	    vim_memset(xmax + band->color_len, 0, tail * حجم(*xmax));
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
    أرجع OK;
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
    ساكن صحيح
rgb_to_paletted_fast(
	char_u	*pixels,
	صحيح	 width,
	صحيح	 height,
	bool	 has_alpha,
	صحيح	 max_colors,
	char_u **pal_out,
	صحيح	*npal_out,
	char_u **idx_out)
{
    صحيح		     cap;
    صحيح		     mask;
    صحيح		     used = 0;
    unsigned صحيح    *کلیلەکان;
    char_u	    *vals;
    char_u	    *idx;
    char_u	    *pal;
    صحيح		     i, n;
    صحيح		     bpp = has_alpha ? 4 : 3;

    إذا (sixel_ensure_hash_capacity(max_colors, &cap) == FAIL
	    || sixel_ensure_idx_capacity((size_t)width * height) == FAIL
	    || sixel_ensure_pal_capacity(max_colors) == FAIL)
	أرجع FAIL;
    کلیلەکان = sixel_state.کلیلەکان;
    vals = sixel_state.vals;
    idx = sixel_state.idx;
    pal = sixel_state.pal;
    mask = cap - 1;

    n = width * height;
    لكل (i = 0; i < n; i++)
    {
	char_u		*p = pixels + (size_t)i * bpp;
	unsigned صحيح	 کلیل;
	unsigned صحيح	 h;
	صحيح		 slot;

	إذا (has_alpha && p[3] < 128)
	{
	    idx[i] = 0;	    // transparent -- terminal leaves cell as-is
	    استمر;
	}
	کلیل = ((unsigned صحيح)p[0] << 16)
	    | ((unsigned صحيح)p[1] << 8)
	    | (unsigned صحيح)p[2];
	h = کلیل * 2654435761u;
	slot = (صحيح)(h & mask);

	لكل (;;)
	{
	    إذا (کلیلەکان[slot] == 0)
	    {
		// new color
		إذا (used >= max_colors)
		    اذهب too_many;
		کلیلەکان[slot] = کلیل + 1;
		vals[slot] = (char_u)(used + 1);    // 1-based palette index
		pal[used * 3]	  = p[0];
		pal[used * 3 + 1] = p[1];
		pal[used * 3 + 2] = p[2];
		used++;
		idx[i] = vals[slot];
		اكسر;
	    }
	    إذا (کلیلەکان[slot] == کلیل + 1)
	    {
		idx[i] = vals[slot];
		اكسر;
	    }
	    slot = (slot + 1) & mask;
	}
    }
    *pal_out = pal;
    *npal_out = used;
    *idx_out = idx;
    أرجع OK;

too_many:
    أرجع FAIL;
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
    ساكن صحيح
rgb_to_paletted_fixed(
	char_u	*pixels,
	صحيح	width,
	صحيح	height,
	bool	has_alpha,
	char_u	**pal_out,
	صحيح	*npal_out,
	char_u	**idx_out)
{
    char_u  *idx;
    صحيح	     n;
    صحيح	     bpp = has_alpha ? 4 : 3;

    إذا (sixel_ensure_idx_capacity((size_t)width * height) == FAIL)
	أرجع FAIL;
    idx = sixel_state.idx;

    sixel_init_fixed_tables();

    // map every pixel to the nearest cube cell
    لكل (n = 0; n < width * height; n++)
    {
	char_u	*p = pixels + (size_t)n * bpp;
	صحيح	 ri, gi, bi;

	إذا (has_alpha && p[3] < 128)
	{
	    idx[n] = 0;
	    استمر;
	}
	ri = sixel_rgb_cube_idx[p[0]];
	gi = sixel_rgb_cube_idx[p[1]];
	bi = sixel_rgb_cube_idx[p[2]];
	idx[n] = (char_u)(ri * 36 + gi * 6 + bi + 1);	// 1-based
    }

    *pal_out = sixel_fixed_palette;
    *npal_out = 240;
    *idx_out = idx;
    أرجع OK;
}

    ساكن فراغ
sixel_image8_clear(sixel_image8_T *simg)
{
    vim_free(simg->idx);
    vim_free(simg->pal);
}

/*
 * Quantize an RGB(A) image to an 8bpp paletted image. Returns OK on success,
 * FAIL on invalid input or OOM.
 */
    ساكن صحيح
sixel_image_quantize(image_T *img, sixel_image8_T *out)
{
    char_u  *pal = عدم;
    char_u  *idx = عدم;
    صحيح	     npal = 0;

    إذا (rgb_to_paletted_fast(img->data, img->width, img->height,
		img->fmt == IMAGE_FORMAT_RGBA,
		SIXEL_MAX_COLORS, &pal, &npal, &idx) == FAIL)
    {
	إذا (rgb_to_paletted_fixed(img->data, img->width, img->height,
		    img->fmt == IMAGE_FORMAT_RGBA,
		    &pal, &npal, &idx) == FAIL)
	    أرجع FAIL;
    }

    out->idx = vim_memsave(idx, (size_t)img->width * img->height);
    out->width = img->width;
    out->height = img->height;
    out->pal = vim_memsave(pal, (size_t)npal * 3);
    out->npal = npal;

    إذا (out->pal == عدم || out->idx == عدم)
    {
	sixel_image8_clear(out);
	أرجع FAIL;
    }
    أرجع OK;
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
    ساكن صحيح
sixel_image8_crop(
	sixel_image8_T	*src,
	صحيح		 x,
	صحيح		 y,
	صحيح		 w,
	صحيح		 h,
	sixel_view_T	*out)
{
    صحيح x0, y0, x1, y1;   // clamped rectangle, half-open [x0,x1) x [y0,y1)

    إذا (src == عدم || out == عدم)
	أرجع FAIL;

    x0 = x < 0 ? 0 : x;
    y0 = y < 0 ? 0 : y;
    x1 = x + w;
    y1 = y + h;
    إذا (x1 > src->width)
	x1 = src->width;
    إذا (y1 > src->height)
	y1 = src->height;
    إذا (x1 < x0)
	x1 = x0;
    إذا (y1 < y0)
	y1 = y0;

    إذا (x1 <= x0 || y1 <= y0)
	أرجع FAIL;

    out->stride = src->width;
    out->idx    = (x1 > x0 && y1 > y0)
	? src->idx + (size_t)y0 * src->width + x0
	: عدم;
    out->width  = x1 - x0;
    out->height = y1 - y0;
    out->pal    = src->pal;
    out->npal   = src->npal;

    أرجع OK;
}

/*
 * Emit a run of `cnt` identical sixel data bytes (ch in 0..63) into gap.
 * Uses RLE form `!N{c}` when it shortens the output.
 */
    ساكن صحيح
emit_run(garray_T *gap, char_u ch, صحيح cnt)
{
    char_u  c = (char_u)(63 + ch);

    طالما (cnt > 255)
    {
	إذا (ga_concat_bytes(gap, "!255", 4) == FAIL
		|| ga_append(gap, c) == FAIL)
	    أرجع FAIL;
	cnt -= 255;
    }
    إذا (cnt <= 0)
	أرجع OK;
    إذا (cnt <= 3)
    {
	طالما (cnt-- > 0)
	    إذا (ga_append(gap, c) == FAIL)
		أرجع FAIL;
	أرجع OK;
    }
    إذا (ga_append(gap, '!') == FAIL
	    || ga_concat_int(gap, cnt) == FAIL
	    || ga_append(gap, c) == FAIL)
	أرجع FAIL;
    أرجع OK;
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
    ساكن صحيح
sixel_encode_view(sixel_view_T *view, char_u *seq[2])
{
    garray_T	    hdr;    // DCS introducer, raster attrs, used-color defs
    garray_T	    body;   // band data (colour selects + sixel bytes)
    sixel_band_T    *band_state = &sixel_state.band;
    char_u	    *idx;
    char_u	    *pal;
    صحيح		    npal;
    صحيح		    width, height;
    صحيح		    band, p, x, n;
    bool	    defined[256];   // register already defined in header

    idx = view->idx;
    pal = view->pal;
    npal = view->npal;
    width = view->width;
    height = view->height;

    ga_init2(&hdr, حجم(char_u), 256);
    ga_init2(&body, حجم(char_u), 4096);
    vim_memset(defined, 0, حجم(defined));

    // DECSIXEL Introducer + Raster Attributes "1;1;W;H.
    // P2=1 means pixel positions left unspecified by any colour register
    // keep their previous on-screen contents instead of being painted with
    // colour register 0.  That gives true transparency for RGBA images:
    // alpha < 128 pixels are emitted as palette index 0 (which we never
    // write to the bitmask), so the terminal leaves the popup's underlying
    // cell colour visible there -- no flatten, no colour-match drift.
    إذا (ga_concat_bytes(&hdr, "\033P0;1;8q\"1;1;", 13) == FAIL
	    || ga_concat_int(&hdr, width) == FAIL
	    || ga_append(&hdr, ';') == FAIL
	    || ga_concat_int(&hdr, height) == FAIL)
	اذهب fail;

    // bitmask buffer: (crop) width bytes per palette index, +1 for the
    // unused index 0.  Sized off the view's width, not the parent image's
    // width, so encoding a small crop out of a large quantized image only
    // needs scratch space proportional to the crop.
    إذا (sixel_ensure_band_capacity(width, npal) == FAIL)
	اذهب fail;

    لكل (band = 0; band < (height + 5) / 6; band++)
    {
	صحيح gen;
	صحيح last_was_cr = 0;
	صحيح used_ژماردن = 0;

	إذا (++band_state->seen_gen <= 0)
	{
	    vim_memset(band_state->seen, 0, (npal + 1) * حجم(صحيح));
	    band_state->seen_gen = 1;
	}
	gen = band_state->seen_gen;

	إذا (band > 0 && ga_append(&body, '-') == FAIL)
	    اذهب fail;

	// Fill the bitmask for this band.
	لكل (p = 0; p < 6; p++)
	{
	    صحيح y = band * 6 + p;
	    char_u  rowmask = (char_u)(1 << p);
	    char_u  *row;

	    إذا (y >= height)
		استمر;
	    // Use the *parent* stride to find the row -- "idx" already
	    // points at the crop's top-left corner, so only the row-to-row
	    // step needs to know about the parent's real width.
	    row = idx + (size_t)y * view->stride;
	    لكل (x = 0; x < width; x++)
	    {
		char_u  pix = row[x];

		إذا (pix == 0)
		    استمر;
		إذا (band_state->seen[pix] != gen)
		{
		    band_state->seen[pix] = gen;
		    band_state->used[used_ژماردن++] = pix;
		    band_state->xmin[pix] = x;
		    band_state->xmax[pix] = x;
		    vim_memset(band_state->bits + (size_t)pix * width, 0, width);
		}
		وإلا
		{
		    إذا (x < band_state->xmin[pix])
			band_state->xmin[pix] = x;
		    إذا (x > band_state->xmax[pix])
			band_state->xmax[pix] = x;
		}
		band_state->bits[(size_t)pix * width + x] |= rowmask;
	    }
	}

	لكل (n = 0; n < used_ژماردن; n++)
	{
	    صحيح	     pix = band_state->used[n];
	    char_u  *row;
	    char_u   ch0;
	    صحيح	     cnt;
	    صحيح	     start = band_state->xmin[pix];
	    صحيح	     end = band_state->xmax[pix];

	    // First time this colour is used in the subrect: add its
	    // definition "#N;2;R;G;B" (RGB scaled to 0..100) to the header.
	    // Colours the subrect never touches are never defined.  Round to
	    // nearest, not truncate, so the round-trip 8bit -> 0..100 -> 8bit
	    // stays within ~1 level instead of drifting up to 2-3 levels
	    // darker.  This matters when the popup blends RGBA alpha onto the
	    // terminal background: truncation made the flattened bg visibly
	    // darker than the surrounding terminal cells.
	    إذا (!defined[pix])
	    {
		صحيح r = (pal[(pix - 1) * 3]     * 100 + 127) / 255;
		صحيح g = (pal[(pix - 1) * 3 + 1] * 100 + 127) / 255;
		صحيح b = (pal[(pix - 1) * 3 + 2] * 100 + 127) / 255;

		defined[pix] = true;
		إذا (ga_append(&hdr, '#') == FAIL
			|| ga_concat_int(&hdr, pix) == FAIL
			|| ga_concat_bytes(&hdr, ";2;", 3) == FAIL
			|| ga_concat_int(&hdr, r) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, g) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, b) == FAIL)
		    اذهب fail;
	    }

	    إذا (last_was_cr && ga_append(&body, '$') == FAIL)
		اذهب fail;
	    إذا (ga_append(&body, '#') == FAIL
		    || ga_concat_int(&body, pix) == FAIL)
		اذهب fail;

	    row = band_state->bits + (size_t)pix * width;
	    إذا (start > 0 && emit_run(&body, 0, start) == FAIL)
		اذهب fail;
	    ch0 = row[start];
	    cnt = 1;
	    لكل (x = start + 1; x <= end; x++)
	    {
		char_u  ch = row[x];

		إذا (ch == ch0)
		{
		    cnt++;
		    استمر;
		}
		إذا (emit_run(&body, ch0, cnt) == FAIL)
		    اذهب fail;
		ch0 = ch;
		cnt = 1;
	    }
	    إذا (emit_run(&body, ch0, cnt) == FAIL)
		اذهب fail;
	    last_was_cr = 1;
	}
    }

    إذا (ga_append(&hdr, NUL) == FAIL
	    || ga_concat_bytes(&body, "\033\\", 3) == FAIL)
	اذهب fail;

    seq[0] = (char_u *)hdr.ga_data;
    seq[1] = (char_u *)body.ga_data;

    أرجع OK;

fail:
    ga_clear(&hdr);
    ga_clear(&body);
    أرجع FAIL;
}

    صحيح
image_sixel_init(image_T *img)
{
    image_sixel_T *ctx;

    ctx = ALLOC_CLEAR_ONE(image_sixel_T);
    إذا (ctx == عدم || sixel_image_quantize(img, &ctx->image8) == FAIL)
    {
	vim_free(ctx);
	أرجع FAIL;
    }

#إذا defined(FEAT_IMAGE_SIXEL) && defined(FEAT_TERMRESPONSE)
    إذا (!sent_decsdm)
    {
	// Disable sixel scrolling using DECSDM
	out_str((char_u *)"\033[?80l");
	sent_decsdm = true;
    }
#endif

    img->backend_data = ctx;
    أرجع OK;
}

    فراغ
image_sixel_uninit(image_T *img)
{
    image_sixel_T *ctx = img->backend_data;

    sixel_image8_clear(&ctx->image8);
    vim_free(img->backend_data);
}

    صحيح
image_placement_sixel_init(image_placement_T *place)
{
    image_placement_sixel_T *ctx = ALLOC_CLEAR_ONE(image_placement_sixel_T);

    إذا (ctx == عدم)
	أرجع FAIL;
    place->backend_data = ctx;
    أرجع OK;
}

    ساكن فراغ
clear_chunks(image_placement_sixel_T *ctx)
{
    إذا (ctx->chunks == عدم)
	أرجع;

    لكل (صحيح i = 0; i < ctx->n_chunks; i++)
    {
	vim_free(ctx->chunks[i].seq[0]);
	vim_free(ctx->chunks[i].seq[1]);
    }
    VIM_CLEAR(ctx->chunks);
    ctx->n_chunks = 0;
}

    فراغ
image_placement_sixel_uninit(image_placement_T *place)
{
    image_placement_sixel_T *ctx = place->backend_data;

    إذا (ctx->visible_init)
	pixman_region32_fini(&ctx->visible_region);
    clear_chunks(ctx);
    vim_free(ctx);
}

    فراغ
image_placement_sixel_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_sixel_T	    *ictx = img->backend_data;
    image_placement_sixel_T *ctx = place->backend_data;
    sixel_view_T	    view;
    pixman_box32_t	    *rects;
    صحيح			    n_rects;

    // Check if visible region is still the same, if so then use the cached
    // sixel sequences.
    إذا (!(place->flags & IMAGEPF_FORCE) && ctx->visible_init
	    && pixman_region32_equal(&ctx->visible_region, &place->visible))
    {
	لكل (صحيح i = 0; i < pixman_region32_n_rects(&ctx->visible_region); i++)
	{
	    sixel_chunk_T *chunk = ctx->chunks + i;

	    windgoto(place->row + place->row_off
		    + chunk->row_off, place->col + chunk->col_off);
	    إذا (chunk->seq[0] != عدم)
		out_str(chunk->seq[0]);
	    إذا (chunk->seq[1] != عدم)
		out_str(chunk->seq[1]);
	    screen_start();
	}
	أرجع;
    }

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    إذا (rects == عدم || n_rects == 0)
	أرجع;

    clear_chunks(ctx);
    ctx->chunks = ALLOC_CLEAR_MULT(sixel_chunk_T, n_rects);

    إذا (ctx->chunks == عدم)
	أرجع;

    لكل (صحيح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحيح		row, col;
	صحيح		x, y, w, h;
	sixel_chunk_T	*chunk = ctx->chunks + i;
	char_u		*seq[2];

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);

	إذا (sixel_image8_crop(&ictx->image8, x, y, w, h, &view) == FAIL
		|| sixel_encode_view(&view, seq) == FAIL)
	    استمر;

	windgoto(row, col);
	إذا (seq[0] != عدم)
	    out_str(seq[0]);
	إذا (seq[1] != عدم)
	    out_str(seq[1]);
	screen_start();

	chunk->row_off = rect.y1;
	chunk->col_off = rect.x1;
	chunk->seq[0] = seq[0];
	chunk->seq[1] = seq[1];
	ctx->n_chunks++;
    }

    إذا (ctx->visible_init)
	pixman_region32_clear(&ctx->visible_region);
    pixman_region32_copy(&ctx->visible_region, &place->visible);
    ctx->visible_init = true;
}

    فراغ
image_placement_sixel_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_SIXEL
