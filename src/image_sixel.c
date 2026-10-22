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

#include "vim.h"

#ຖ້າ defined(FEAT_IMAGE_SIXEL)

// Palette size cap (sixel allows up to 256 color registers; index 0 is
// reserved as a transparent key, so usable colors are 1..MAX_COLORS).
#define SIXEL_MAX_COLORS    255

/*
 * Working buffer for one band (6 pixel rows). Allocated as
 * width * (palette_size+1) bytes; bit p (0..5) marks pixel at row offset p.
 */
typedef sa_nracanaa
{
    char_u  *bits;	// bitmask buffer, width * paletteSize bytes
    size_t   bits_len;	// allocated bytes in "bits"
    ຈຳນວນເຕັມ	    *seen;	// per-color "used in this band" flags
    ຈຳນວນເຕັມ	    *used;	// colors used in this band
    ຈຳນວນເຕັມ	    *xmin;	// leftmost x used by color in this band
    ຈຳນວນເຕັມ	    *xmax;	// rightmost x used by color in this band
    ຈຳນວນເຕັມ	     color_len;	// allocated length for per-color arrays
    ຈຳນວນເຕັມ	     seen_gen;	// generation counter to avoid memset
} sixel_band_T;

typedef sa_nracanaa
{
    unsigned ຈຳນວນເຕັມ    *keys;	// hash keys for dynamic palette lookup
    char_u	    *vals;	// hash values for dynamic palette lookup
    ຈຳນວນເຕັມ		     hash_cap;	// allocated slots in keys/vals
    char_u	    *idx;	// width * height paletted image
    size_t	     idx_len;	// allocated bytes in idx
    char_u	    *pal;	// dynamic palette storage (RGB triples)
    ຈຳນວນເຕັມ		     pal_len;	// allocated bytes in pal
    sixel_band_T     band;	// reusable band scratch
} sixel_state_T;

/*
 * A fully quantized (8bpp) image: one palette index per pixel, tightly
 * packed (stride == width), plus the palette that goes with it.
 */
typedef sa_nracanaa
{
    char_u  *idx; // Is width*height bytes, indices 1..npal (0 = transparent)
    ຈຳນວນເຕັມ      width;
    ຈຳນວນເຕັມ      height;
    char_u  *pal; // Is npal*3 bytes, RGB triples
    ຈຳນວນເຕັມ      npal;
} sixel_image8_T;

/*
 * A view into a sixel_image8_T: an offset + stride,
 */
typedef sa_nracanaa
{
    char_u  *idx; // Points at (x0,y0) inside the parent buffer
    ຈຳນວນເຕັມ      stride; // Row length of the *parent* image, not this view
    ຈຳນວນເຕັມ      width;
    ຈຳນວນເຕັມ      height;
    char_u  *pal;
    ຈຳນວນເຕັມ      npal;
} sixel_view_T;

typedef sa_nracanaa
{
    // These are offsets relative to the top left of the image.
    ຈຳນວນເຕັມ	    row_off;
    ຈຳນວນເຕັມ	    col_off;
    char_u  *seq[2]; // Header + body
} sixel_chunk_T;

typedef sa_nracanaa
{
    sixel_image8_T image8;
} image_sixel_T;

typedef sa_nracanaa
{
    pixman_region32_t	visible_region; // Region used for the previous redraw
    bool		visible_init;
    sixel_chunk_T	*chunks;    // Cached sixel sequence (stored as multiple
				    // chunks).
    ຈຳນວນເຕັມ			n_chunks;
} image_placement_sixel_T;

static char_u		sixel_fixed_palette[240 * 3];
static char_u		sixel_rgb_cube_idx[256];
static bool		sixel_fixed_tables_ready = false;
static sixel_state_T	sixel_state;
static bool		sent_decsdm = false;

/*
 * Release all module-level allocations cached by the sixel encoder.
 */
    void
sixel_uninit(void)
{
    sixel_band_T *band = &sixel_state.band;

    VIM_CLEAR(sixel_state.keys);
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
    static void
sixel_init_fixed_tables(void)
{
    ຈຳນວນເຕັມ	    r, g, b, gr, n;

    ຖ້າ (sixel_fixed_tables_ready)
	ສົ່ງຄືນ;

    n = 0;
    ສຳລັບ (r = 0; r < 6; r++)
	ສຳລັບ (g = 0; g < 6; g++)
	    ສຳລັບ (b = 0; b < 6; b++)
	    {
		sixel_fixed_palette[n * 3]     = (char_u)(r * 51);
		sixel_fixed_palette[n * 3 + 1] = (char_u)(g * 51);
		sixel_fixed_palette[n * 3 + 2] = (char_u)(b * 51);
		n++;
	    }
    ສຳລັບ (gr = 0; gr < 24; gr++)
    {
	ຈຳນວນເຕັມ v = 8 + gr * 10;

	sixel_fixed_palette[n * 3]     = (char_u)v;
	sixel_fixed_palette[n * 3 + 1] = (char_u)v;
	sixel_fixed_palette[n * 3 + 2] = (char_u)v;
	n++;
    }
    ສຳລັບ (n = 0; n < 256; n++)
    {
	ຈຳນວນເຕັມ idx = n / 43;

	sixel_rgb_cube_idx[n] = (char_u)(idx > 5 ? 5 : idx);
    }
    sixel_fixed_tables_ready = true;
}

    static ຈຳນວນເຕັມ
sixel_ensure_hash_capacity(ຈຳນວນເຕັມ max_colors, ຈຳນວນເຕັມ *cap_out)
{
    ຈຳນວນເຕັມ cap = 1024;

    ໃນຂະນະທີ່ (cap < max_colors * 4)
	cap <<= 1;
    ຖ້າ (cap > sixel_state.hash_cap)
    {
	unsigned ຈຳນວນເຕັມ	*keys;
	char_u		*vals;

	keys = ALLOC_MULT(unsigned ຈຳນວນເຕັມ, cap);
	vals = ALLOC_MULT(char_u, cap);
	ຖ້າ (keys == NULL || vals == NULL)
	{
	    vim_free(keys);
	    vim_free(vals);
	    ສົ່ງຄືນ FAIL;
	}
	ຖ້າ (sixel_state.hash_cap > 0)
	{
	    mch_memmove(keys, sixel_state.keys,
		    (size_t)sixel_state.hash_cap * parimaa_na(*keys));
	    mch_memmove(vals, sixel_state.vals,
		    (size_t)sixel_state.hash_cap * parimaa_na(*vals));
	}
	vim_free(sixel_state.keys);
	vim_free(sixel_state.vals);
	sixel_state.keys = keys;
	sixel_state.vals = vals;
	sixel_state.hash_cap = cap;
    }
    vim_memset(sixel_state.keys, 0, (size_t)cap * parimaa_na(*sixel_state.keys));
    *cap_out = cap;
    ສົ່ງຄືນ OK;
}

    static ຈຳນວນເຕັມ
sixel_ensure_idx_capacity(size_t pixels)
{
    ຖ້າ (pixels > sixel_state.idx_len)
    {
	char_u *idx = vim_realloc(sixel_state.idx, pixels);

	ຖ້າ (idx == NULL)
	    ສົ່ງຄືນ FAIL;
	sixel_state.idx = idx;
	sixel_state.idx_len = pixels;
    }
    ສົ່ງຄືນ OK;
}

    static ຈຳນວນເຕັມ
sixel_ensure_pal_capacity(ຈຳນວນເຕັມ max_colors)
{
    ຈຳນວນເຕັມ need = max_colors * 3;

    ຖ້າ (need > sixel_state.pal_len)
    {
	char_u *pal = vim_realloc(sixel_state.pal, (size_t)need);

	ຖ້າ (pal == NULL)
	    ສົ່ງຄືນ FAIL;
	sixel_state.pal = pal;
	sixel_state.pal_len = need;
    }
    ສົ່ງຄືນ OK;
}

    static ຈຳນວນເຕັມ
sixel_ensure_band_capacity(ຈຳນວນເຕັມ width, ຈຳນວນເຕັມ npal)
{
    size_t	    bits_need = (size_t)width * (npal + 1);
    sixel_band_T    *band = &sixel_state.band;

    ຖ້າ (bits_need > band->bits_len)
    {
	char_u *bits = vim_realloc(band->bits, bits_need);

	ຖ້າ (bits == NULL)
	    ສົ່ງຄືນ FAIL;
	band->bits = bits;
	band->bits_len = bits_need;
    }
    ຖ້າ (npal + 1 > band->color_len)
    {
	ຈຳນວນເຕັມ *seen = ALLOC_MULT(ຈຳນວນເຕັມ, npal + 1);
	ຈຳນວນເຕັມ *used = ALLOC_MULT(ຈຳນວນເຕັມ, npal + 1);
	ຈຳນວນເຕັມ *xmin = ALLOC_MULT(ຈຳນວນເຕັມ, npal + 1);
	ຈຳນວນເຕັມ *xmax = ALLOC_MULT(ຈຳນວນເຕັມ, npal + 1);

	ຖ້າ (seen == NULL || used == NULL || xmin == NULL || xmax == NULL)
	{
	    vim_free(seen);
	    vim_free(used);
	    vim_free(xmin);
	    vim_free(xmax);
	    ສົ່ງຄືນ FAIL;
	}
	ຖ້າ (band->color_len > 0)
	{
	    mch_memmove(seen, band->seen,
		    (size_t)band->color_len * parimaa_na(*seen));
	    mch_memmove(used, band->used,
		    (size_t)band->color_len * parimaa_na(*used));
	    mch_memmove(xmin, band->xmin,
		    (size_t)band->color_len * parimaa_na(*xmin));
	    mch_memmove(xmax, band->xmax,
		    (size_t)band->color_len * parimaa_na(*xmax));
	}

	// seen[] is checked against the band's generation counter; if the
	// newly grown tail held garbage equal to the current generation, the
	// band loop would read uninitialised xmin/xmax.  Zero the new tail
	// (and the matching slots in used/xmin/xmax for good measure) so the
	// "first time this colour is touched" branch is always taken.
	{
	    size_t tail = (size_t)(npal + 1 - band->color_len);

	    vim_memset(seen + band->color_len, 0, tail * parimaa_na(*seen));
	    vim_memset(used + band->color_len, 0, tail * parimaa_na(*used));
	    vim_memset(xmin + band->color_len, 0, tail * parimaa_na(*xmin));
	    vim_memset(xmax + band->color_len, 0, tail * parimaa_na(*xmax));
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
    ສົ່ງຄືນ OK;
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
    static ຈຳນວນເຕັມ
rgb_to_paletted_fast(
	char_u	*pixels,
	ຈຳນວນເຕັມ	 width,
	ຈຳນວນເຕັມ	 height,
	bool	 has_alpha,
	ຈຳນວນເຕັມ	 max_colors,
	char_u **pal_out,
	ຈຳນວນເຕັມ	*npal_out,
	char_u **idx_out)
{
    ຈຳນວນເຕັມ		     cap;
    ຈຳນວນເຕັມ		     mask;
    ຈຳນວນເຕັມ		     used = 0;
    unsigned ຈຳນວນເຕັມ    *keys;
    char_u	    *vals;
    char_u	    *idx;
    char_u	    *pal;
    ຈຳນວນເຕັມ		     i, n;
    ຈຳນວນເຕັມ		     bpp = has_alpha ? 4 : 3;

    ຖ້າ (sixel_ensure_hash_capacity(max_colors, &cap) == FAIL
	    || sixel_ensure_idx_capacity((size_t)width * height) == FAIL
	    || sixel_ensure_pal_capacity(max_colors) == FAIL)
	ສົ່ງຄືນ FAIL;
    keys = sixel_state.keys;
    vals = sixel_state.vals;
    idx = sixel_state.idx;
    pal = sixel_state.pal;
    mask = cap - 1;

    n = width * height;
    ສຳລັບ (i = 0; i < n; i++)
    {
	char_u		*p = pixels + (size_t)i * bpp;
	unsigned ຈຳນວນເຕັມ	 key;
	unsigned ຈຳນວນເຕັມ	 h;
	ຈຳນວນເຕັມ		 slot;

	ຖ້າ (has_alpha && p[3] < 128)
	{
	    idx[i] = 0;	    // transparent -- terminal leaves cell as-is
	    continue;
	}
	key = ((unsigned ຈຳນວນເຕັມ)p[0] << 16)
	    | ((unsigned ຈຳນວນເຕັມ)p[1] << 8)
	    | (unsigned ຈຳນວນເຕັມ)p[2];
	h = key * 2654435761u;
	slot = (ຈຳນວນເຕັມ)(h & mask);

	ສຳລັບ (;;)
	{
	    ຖ້າ (keys[slot] == 0)
	    {
		// new color
		ຖ້າ (used >= max_colors)
		    goto too_many;
		keys[slot] = key + 1;
		vals[slot] = (char_u)(used + 1);    // 1-based palette index
		pal[used * 3]	  = p[0];
		pal[used * 3 + 1] = p[1];
		pal[used * 3 + 2] = p[2];
		used++;
		idx[i] = vals[slot];
		bha_nga;
	    }
	    ຖ້າ (keys[slot] == key + 1)
	    {
		idx[i] = vals[slot];
		bha_nga;
	    }
	    slot = (slot + 1) & mask;
	}
    }
    *pal_out = pal;
    *npal_out = used;
    *idx_out = idx;
    ສົ່ງຄືນ OK;

too_many:
    ສົ່ງຄືນ FAIL;
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
    static ຈຳນວນເຕັມ
rgb_to_paletted_fixed(
	char_u	*pixels,
	ຈຳນວນເຕັມ	width,
	ຈຳນວນເຕັມ	height,
	bool	has_alpha,
	char_u	**pal_out,
	ຈຳນວນເຕັມ	*npal_out,
	char_u	**idx_out)
{
    char_u  *idx;
    ຈຳນວນເຕັມ	     n;
    ຈຳນວນເຕັມ	     bpp = has_alpha ? 4 : 3;

    ຖ້າ (sixel_ensure_idx_capacity((size_t)width * height) == FAIL)
	ສົ່ງຄືນ FAIL;
    idx = sixel_state.idx;

    sixel_init_fixed_tables();

    // map every pixel to the nearest cube cell
    ສຳລັບ (n = 0; n < width * height; n++)
    {
	char_u	*p = pixels + (size_t)n * bpp;
	ຈຳນວນເຕັມ	 ri, gi, bi;

	ຖ້າ (has_alpha && p[3] < 128)
	{
	    idx[n] = 0;
	    continue;
	}
	ri = sixel_rgb_cube_idx[p[0]];
	gi = sixel_rgb_cube_idx[p[1]];
	bi = sixel_rgb_cube_idx[p[2]];
	idx[n] = (char_u)(ri * 36 + gi * 6 + bi + 1);	// 1-based
    }

    *pal_out = sixel_fixed_palette;
    *npal_out = 240;
    *idx_out = idx;
    ສົ່ງຄືນ OK;
}

    static void
sixel_image8_clear(sixel_image8_T *simg)
{
    vim_free(simg->idx);
    vim_free(simg->pal);
}

/*
 * Quantize an RGB(A) image to an 8bpp paletted image. Returns OK on success,
 * FAIL on invalid input or OOM.
 */
    static ຈຳນວນເຕັມ
sixel_image_quantize(image_T *img, sixel_image8_T *out)
{
    char_u  *pal = NULL;
    char_u  *idx = NULL;
    ຈຳນວນເຕັມ	     npal = 0;

    ຖ້າ (rgb_to_paletted_fast(img->data, img->width, img->height,
		img->fmt == IMAGE_FORMAT_RGBA,
		SIXEL_MAX_COLORS, &pal, &npal, &idx) == FAIL)
    {
	ຖ້າ (rgb_to_paletted_fixed(img->data, img->width, img->height,
		    img->fmt == IMAGE_FORMAT_RGBA,
		    &pal, &npal, &idx) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }

    out->idx = vim_memsave(idx, (size_t)img->width * img->height);
    out->width = img->width;
    out->height = img->height;
    out->pal = vim_memsave(pal, (size_t)npal * 3);
    out->npal = npal;

    ຖ້າ (out->pal == NULL || out->idx == NULL)
    {
	sixel_image8_clear(out);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
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
    static ຈຳນວນເຕັມ
sixel_image8_crop(
	sixel_image8_T	*src,
	ຈຳນວນເຕັມ		 x,
	ຈຳນວນເຕັມ		 y,
	ຈຳນວນເຕັມ		 w,
	ຈຳນວນເຕັມ		 h,
	sixel_view_T	*out)
{
    ຈຳນວນເຕັມ x0, y0, x1, y1;   // clamped rectangle, half-open [x0,x1) x [y0,y1)

    ຖ້າ (src == NULL || out == NULL)
	ສົ່ງຄືນ FAIL;

    x0 = x < 0 ? 0 : x;
    y0 = y < 0 ? 0 : y;
    x1 = x + w;
    y1 = y + h;
    ຖ້າ (x1 > src->width)
	x1 = src->width;
    ຖ້າ (y1 > src->height)
	y1 = src->height;
    ຖ້າ (x1 < x0)
	x1 = x0;
    ຖ້າ (y1 < y0)
	y1 = y0;

    ຖ້າ (x1 <= x0 || y1 <= y0)
	ສົ່ງຄືນ FAIL;

    out->stride = src->width;
    out->idx    = (x1 > x0 && y1 > y0)
	? src->idx + (size_t)y0 * src->width + x0
	: NULL;
    out->width  = x1 - x0;
    out->height = y1 - y0;
    out->pal    = src->pal;
    out->npal   = src->npal;

    ສົ່ງຄືນ OK;
}

/*
 * Emit a run of `cnt` identical sixel data bytes (ch in 0..63) into gap.
 * Uses RLE form `!N{c}` when it shortens the output.
 */
    static ຈຳນວນເຕັມ
emit_run(garray_T *gap, char_u ch, ຈຳນວນເຕັມ cnt)
{
    char_u  c = (char_u)(63 + ch);

    ໃນຂະນະທີ່ (cnt > 255)
    {
	ຖ້າ (ga_concat_bytes(gap, "!255", 4) == FAIL
		|| ga_append(gap, c) == FAIL)
	    ສົ່ງຄືນ FAIL;
	cnt -= 255;
    }
    ຖ້າ (cnt <= 0)
	ສົ່ງຄືນ OK;
    ຖ້າ (cnt <= 3)
    {
	ໃນຂະນະທີ່ (cnt-- > 0)
	    ຖ້າ (ga_append(gap, c) == FAIL)
		ສົ່ງຄືນ FAIL;
	ສົ່ງຄືນ OK;
    }
    ຖ້າ (ga_append(gap, '!') == FAIL
	    || ga_concat_int(gap, cnt) == FAIL
	    || ga_append(gap, c) == FAIL)
	ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
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
    static ຈຳນວນເຕັມ
sixel_encode_view(sixel_view_T *view, char_u *seq[2])
{
    garray_T	    hdr;    // DCS introducer, raster attrs, used-color defs
    garray_T	    body;   // band data (colour selects + sixel bytes)
    sixel_band_T    *band_state = &sixel_state.band;
    char_u	    *idx;
    char_u	    *pal;
    ຈຳນວນເຕັມ		    npal;
    ຈຳນວນເຕັມ		    width, height;
    ຈຳນວນເຕັມ		    band, p, x, n;
    bool	    defined[256];   // register already defined in header

    idx = view->idx;
    pal = view->pal;
    npal = view->npal;
    width = view->width;
    height = view->height;

    ga_init2(&hdr, parimaa_na(char_u), 256);
    ga_init2(&body, parimaa_na(char_u), 4096);
    vim_memset(defined, 0, parimaa_na(defined));

    // DECSIXEL Introducer + Raster Attributes "1;1;W;H.
    // P2=1 means pixel positions left unspecified by any colour register
    // keep their previous on-screen contents instead of being painted with
    // colour register 0.  That gives true transparency for RGBA images:
    // alpha < 128 pixels are emitted as palette index 0 (which we never
    // write to the bitmask), so the terminal leaves the popup's underlying
    // cell colour visible there -- no flatten, no colour-match drift.
    ຖ້າ (ga_concat_bytes(&hdr, "\033P0;1;8q\"1;1;", 13) == FAIL
	    || ga_concat_int(&hdr, width) == FAIL
	    || ga_append(&hdr, ';') == FAIL
	    || ga_concat_int(&hdr, height) == FAIL)
	goto fail;

    // bitmask buffer: (crop) width bytes per palette index, +1 for the
    // unused index 0.  Sized off the view's width, not the parent image's
    // width, so encoding a small crop out of a large quantized image only
    // needs scratch space proportional to the crop.
    ຖ້າ (sixel_ensure_band_capacity(width, npal) == FAIL)
	goto fail;

    ສຳລັບ (band = 0; band < (height + 5) / 6; band++)
    {
	ຈຳນວນເຕັມ gen;
	ຈຳນວນເຕັມ last_was_cr = 0;
	ຈຳນວນເຕັມ used_count = 0;

	ຖ້າ (++band_state->seen_gen <= 0)
	{
	    vim_memset(band_state->seen, 0, (npal + 1) * parimaa_na(ຈຳນວນເຕັມ));
	    band_state->seen_gen = 1;
	}
	gen = band_state->seen_gen;

	ຖ້າ (band > 0 && ga_append(&body, '-') == FAIL)
	    goto fail;

	// Fill the bitmask for this band.
	ສຳລັບ (p = 0; p < 6; p++)
	{
	    ຈຳນວນເຕັມ y = band * 6 + p;
	    char_u  rowmask = (char_u)(1 << p);
	    char_u  *row;

	    ຖ້າ (y >= height)
		continue;
	    // Use the *parent* stride to find the row -- "idx" already
	    // points at the crop's top-left corner, so only the row-to-row
	    // step needs to know about the parent's real width.
	    row = idx + (size_t)y * view->stride;
	    ສຳລັບ (x = 0; x < width; x++)
	    {
		char_u  pix = row[x];

		ຖ້າ (pix == 0)
		    continue;
		ຖ້າ (band_state->seen[pix] != gen)
		{
		    band_state->seen[pix] = gen;
		    band_state->used[used_count++] = pix;
		    band_state->xmin[pix] = x;
		    band_state->xmax[pix] = x;
		    vim_memset(band_state->bits + (size_t)pix * width, 0, width);
		}
		ບໍ່ດັ່ງນັ້ນ
		{
		    ຖ້າ (x < band_state->xmin[pix])
			band_state->xmin[pix] = x;
		    ຖ້າ (x > band_state->xmax[pix])
			band_state->xmax[pix] = x;
		}
		band_state->bits[(size_t)pix * width + x] |= rowmask;
	    }
	}

	ສຳລັບ (n = 0; n < used_count; n++)
	{
	    ຈຳນວນເຕັມ	     pix = band_state->used[n];
	    char_u  *row;
	    char_u   ch0;
	    ຈຳນວນເຕັມ	     cnt;
	    ຈຳນວນເຕັມ	     start = band_state->xmin[pix];
	    ຈຳນວນເຕັມ	     end = band_state->xmax[pix];

	    // First time this colour is used in the subrect: add its
	    // definition "#N;2;R;G;B" (RGB scaled to 0..100) to the header.
	    // Colours the subrect never touches are never defined.  Round to
	    // nearest, not truncate, so the round-trip 8bit -> 0..100 -> 8bit
	    // stays within ~1 level instead of drifting up to 2-3 levels
	    // darker.  This matters when the popup blends RGBA alpha onto the
	    // terminal background: truncation made the flattened bg visibly
	    // darker than the surrounding terminal cells.
	    ຖ້າ (!defined[pix])
	    {
		ຈຳນວນເຕັມ r = (pal[(pix - 1) * 3]     * 100 + 127) / 255;
		ຈຳນວນເຕັມ g = (pal[(pix - 1) * 3 + 1] * 100 + 127) / 255;
		ຈຳນວນເຕັມ b = (pal[(pix - 1) * 3 + 2] * 100 + 127) / 255;

		defined[pix] = true;
		ຖ້າ (ga_append(&hdr, '#') == FAIL
			|| ga_concat_int(&hdr, pix) == FAIL
			|| ga_concat_bytes(&hdr, ";2;", 3) == FAIL
			|| ga_concat_int(&hdr, r) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, g) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, b) == FAIL)
		    goto fail;
	    }

	    ຖ້າ (last_was_cr && ga_append(&body, '$') == FAIL)
		goto fail;
	    ຖ້າ (ga_append(&body, '#') == FAIL
		    || ga_concat_int(&body, pix) == FAIL)
		goto fail;

	    row = band_state->bits + (size_t)pix * width;
	    ຖ້າ (start > 0 && emit_run(&body, 0, start) == FAIL)
		goto fail;
	    ch0 = row[start];
	    cnt = 1;
	    ສຳລັບ (x = start + 1; x <= end; x++)
	    {
		char_u  ch = row[x];

		ຖ້າ (ch == ch0)
		{
		    cnt++;
		    continue;
		}
		ຖ້າ (emit_run(&body, ch0, cnt) == FAIL)
		    goto fail;
		ch0 = ch;
		cnt = 1;
	    }
	    ຖ້າ (emit_run(&body, ch0, cnt) == FAIL)
		goto fail;
	    last_was_cr = 1;
	}
    }

    ຖ້າ (ga_append(&hdr, NUL) == FAIL
	    || ga_concat_bytes(&body, "\033\\", 3) == FAIL)
	goto fail;

    seq[0] = (char_u *)hdr.ga_data;
    seq[1] = (char_u *)body.ga_data;

    ສົ່ງຄືນ OK;

fail:
    ga_clear(&hdr);
    ga_clear(&body);
    ສົ່ງຄືນ FAIL;
}

    ຈຳນວນເຕັມ
image_sixel_init(image_T *img)
{
    image_sixel_T *ctx;

    ctx = ALLOC_CLEAR_ONE(image_sixel_T);
    ຖ້າ (ctx == NULL || sixel_image_quantize(img, &ctx->image8) == FAIL)
    {
	vim_free(ctx);
	ສົ່ງຄືນ FAIL;
    }

#ຖ້າ defined(FEAT_IMAGE_SIXEL) && defined(FEAT_TERMRESPONSE)
    ຖ້າ (!sent_decsdm)
    {
	// Disable sixel scrolling using DECSDM
	out_str((char_u *)"\033[?80l");
	sent_decsdm = true;
    }
#endif

    img->backend_data = ctx;
    ສົ່ງຄືນ OK;
}

    void
image_sixel_uninit(image_T *img)
{
    image_sixel_T *ctx = img->backend_data;

    sixel_image8_clear(&ctx->image8);
    vim_free(img->backend_data);
}

    ຈຳນວນເຕັມ
image_placement_sixel_init(image_placement_T *place)
{
    image_placement_sixel_T *ctx = ALLOC_CLEAR_ONE(image_placement_sixel_T);

    ຖ້າ (ctx == NULL)
	ສົ່ງຄືນ FAIL;
    place->backend_data = ctx;
    ສົ່ງຄືນ OK;
}

    static void
clear_chunks(image_placement_sixel_T *ctx)
{
    ຖ້າ (ctx->chunks == NULL)
	ສົ່ງຄືນ;

    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < ctx->n_chunks; i++)
    {
	vim_free(ctx->chunks[i].seq[0]);
	vim_free(ctx->chunks[i].seq[1]);
    }
    VIM_CLEAR(ctx->chunks);
    ctx->n_chunks = 0;
}

    void
image_placement_sixel_uninit(image_placement_T *place)
{
    image_placement_sixel_T *ctx = place->backend_data;

    ຖ້າ (ctx->visible_init)
	pixman_region32_fini(&ctx->visible_region);
    clear_chunks(ctx);
    vim_free(ctx);
}

    void
image_placement_sixel_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_sixel_T	    *ictx = img->backend_data;
    image_placement_sixel_T *ctx = place->backend_data;
    sixel_view_T	    view;
    pixman_box32_t	    *rects;
    ຈຳນວນເຕັມ			    n_rects;

    // Check if visible region is still the same, if so then use the cached
    // sixel sequences.
    ຖ້າ (!(place->flags & IMAGEPF_FORCE) && ctx->visible_init
	    && pixman_region32_equal(&ctx->visible_region, &place->visible))
    {
	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < pixman_region32_n_rects(&ctx->visible_region); i++)
	{
	    sixel_chunk_T *chunk = ctx->chunks + i;

	    windgoto(place->row + place->row_off
		    + chunk->row_off, place->col + chunk->col_off);
	    ຖ້າ (chunk->seq[0] != NULL)
		out_str(chunk->seq[0]);
	    ຖ້າ (chunk->seq[1] != NULL)
		out_str(chunk->seq[1]);
	    screen_start();
	}
	ສົ່ງຄືນ;
    }

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    ຖ້າ (rects == NULL || n_rects == 0)
	ສົ່ງຄືນ;

    clear_chunks(ctx);
    ctx->chunks = ALLOC_CLEAR_MULT(sixel_chunk_T, n_rects);

    ຖ້າ (ctx->chunks == NULL)
	ສົ່ງຄືນ;

    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ຈຳນວນເຕັມ		row, col;
	ຈຳນວນເຕັມ		x, y, w, h;
	sixel_chunk_T	*chunk = ctx->chunks + i;
	char_u		*seq[2];

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);

	ຖ້າ (sixel_image8_crop(&ictx->image8, x, y, w, h, &view) == FAIL
		|| sixel_encode_view(&view, seq) == FAIL)
	    continue;

	windgoto(row, col);
	ຖ້າ (seq[0] != NULL)
	    out_str(seq[0]);
	ຖ້າ (seq[1] != NULL)
	    out_str(seq[1]);
	screen_start();

	chunk->row_off = rect.y1;
	chunk->col_off = rect.x1;
	chunk->seq[0] = seq[0];
	chunk->seq[1] = seq[1];
	ctx->n_chunks++;
    }

    ຖ້າ (ctx->visible_init)
	pixman_region32_clear(&ctx->visible_region);
    pixman_region32_copy(&ctx->visible_region, &place->visible);
    ctx->visible_init = true;
}

    void
image_placement_sixel_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_SIXEL
