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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_IMAGE_SIXEL)

// Palette size cap (sixel allows up to 256 color registers; index 0 is
// reserved as a transparent key, so usable colors are 1..MAX_COLORS).
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIXEL_MAX_COLORS    255

/*
 * Working buffer for one band (6 pixel rows). Allocated as
 * width * (palette_size+1) bytes; bit p (0..5) marks pixel at row offset p.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u  *ਬਿੱਟ;	// bitmask buffer, width * paletteSize bytes
    size_t   ਬਿੱਟ_len;	// allocated bytes in "bits"
    ਪੂਰਨ	    *ਵੇਖਿਆ;	// per-color "used in this band" flags
    ਪੂਰਨ	    *ਵਰਤਿਆ;	// colors used in this band
    ਪੂਰਨ	    *xmin;	// leftmost x used by color in this band
    ਪੂਰਨ	    *xmax;	// rightmost x used by color in this band
    ਪੂਰਨ	     color_len;	// allocated length for per-color arrays
    ਪੂਰਨ	     ਵੇਖਿਆ_gen;	// generation counter to avoid memset
} sixel_band_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    *ਕੁੰਜੀਆਂ;	// hash keys for dynamic palette lookup
    char_u	    *vals;	// hash values for dynamic palette lookup
    ਪੂਰਨ		     hash_cap;	// allocated slots in keys/vals
    char_u	    *idx;	// width * height paletted image
    size_t	     idx_len;	// allocated bytes in idx
    char_u	    *pal;	// dynamic palette storage (RGB triples)
    ਪੂਰਨ		     pal_len;	// allocated bytes in pal
    sixel_band_T     band;	// reusable band scratch
} sixel_state_T;

/*
 * A fully quantized (8bpp) image: one palette index per pixel, tightly
 * packed (stride == width), plus the palette that goes with it.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u  *idx; // Is width*height bytes, indices 1..npal (0 = transparent)
    ਪੂਰਨ      ਚੌੜਾਈ;
    ਪੂਰਨ      ਉਚਾਈ;
    char_u  *pal; // Is npal*3 bytes, RGB triples
    ਪੂਰਨ      npal;
} sixel_image8_T;

/*
 * A view into a sixel_image8_T: an offset + stride,
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u  *idx; // Points at (x0,y0) inside the parent buffer
    ਪੂਰਨ      stride; // Row length of the *parent* image, not this view
    ਪੂਰਨ      ਚੌੜਾਈ;
    ਪੂਰਨ      ਉਚਾਈ;
    char_u  *pal;
    ਪੂਰਨ      npal;
} sixel_view_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    // These are offsets relative to the top left of the image.
    ਪੂਰਨ	    ਪੰਕਤੀ_off;
    ਪੂਰਨ	    ਖਾਨਾ_off;
    char_u  *seq[2]; // Header + body
} sixel_chunk_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    sixel_image8_T image8;
} image_sixel_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    pixman_region32_t	visible_region; // Region used for the previous redraw
    bool		visible_init;
    sixel_chunk_T	*chunks;    // Cached sixel sequence (stored as multiple
				    // chunks).
    ਪੂਰਨ			n_chunks;
} image_placement_sixel_T;

ਸਥਿਰ char_u		sixel_fixed_palette[240 * 3];
ਸਥਿਰ char_u		sixel_rgb_cube_idx[256];
ਸਥਿਰ bool		sixel_fixed_tables_ready = false;
ਸਥਿਰ sixel_state_T	sixel_state;
ਸਥਿਰ bool		ਭੇਜਿਆ_decsdm = false;

/*
 * Release all module-level allocations cached by the sixel encoder.
 */
    ਖਾਲੀ
sixel_uninit(ਖਾਲੀ)
{
    sixel_band_T *band = &sixel_state.band;

    VIM_CLEAR(sixel_state.ਕੁੰਜੀਆਂ);
    VIM_CLEAR(sixel_state.vals);
    VIM_CLEAR(sixel_state.idx);
    VIM_CLEAR(sixel_state.pal);
    sixel_state.hash_cap = 0;
    sixel_state.idx_len = 0;
    sixel_state.pal_len = 0;

    VIM_CLEAR(band->ਬਿੱਟ);
    VIM_CLEAR(band->ਵੇਖਿਆ);
    VIM_CLEAR(band->ਵਰਤਿਆ);
    VIM_CLEAR(band->xmin);
    VIM_CLEAR(band->xmax);
    band->ਬਿੱਟ_len = 0;
    band->color_len = 0;
    band->ਵੇਖਿਆ_gen = 0;
}

/*
 * Initialize the shared fixed sixel palette and RGB->cube lookup table.
 */
    ਸਥਿਰ ਖਾਲੀ
sixel_init_fixed_tables(ਖਾਲੀ)
{
    ਪੂਰਨ	    r, g, b, gr, n;

    ਜੇ (sixel_fixed_tables_ready)
	ਵਾਪਸ;

    n = 0;
    ਲਈ (r = 0; r < 6; r++)
	ਲਈ (g = 0; g < 6; g++)
	    ਲਈ (b = 0; b < 6; b++)
	    {
		sixel_fixed_palette[n * 3]     = (char_u)(r * 51);
		sixel_fixed_palette[n * 3 + 1] = (char_u)(g * 51);
		sixel_fixed_palette[n * 3 + 2] = (char_u)(b * 51);
		n++;
	    }
    ਲਈ (gr = 0; gr < 24; gr++)
    {
	ਪੂਰਨ v = 8 + gr * 10;

	sixel_fixed_palette[n * 3]     = (char_u)v;
	sixel_fixed_palette[n * 3 + 1] = (char_u)v;
	sixel_fixed_palette[n * 3 + 2] = (char_u)v;
	n++;
    }
    ਲਈ (n = 0; n < 256; n++)
    {
	ਪੂਰਨ idx = n / 43;

	sixel_rgb_cube_idx[n] = (char_u)(idx > 5 ? 5 : idx);
    }
    sixel_fixed_tables_ready = true;
}

    ਸਥਿਰ ਪੂਰਨ
sixel_ensure_hash_ਸਮਰੱਥਾ(ਪੂਰਨ max_colors, ਪੂਰਨ *cap_ਬਾਹਰਲਾ)
{
    ਪੂਰਨ cap = 1024;

    ਜਦੋਂ (cap < max_colors * 4)
	cap <<= 1;
    ਜੇ (cap > sixel_state.hash_cap)
    {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	*ਕੁੰਜੀਆਂ;
	char_u		*vals;

	ਕੁੰਜੀਆਂ = ALLOC_MULT(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, cap);
	vals = ALLOC_MULT(char_u, cap);
	ਜੇ (ਕੁੰਜੀਆਂ == ਨਿਰਾ || vals == ਨਿਰਾ)
	{
	    vim_free(ਕੁੰਜੀਆਂ);
	    vim_free(vals);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (sixel_state.hash_cap > 0)
	{
	    mch_memmove(ਕੁੰਜੀਆਂ, sixel_state.ਕੁੰਜੀਆਂ,
		    (size_t)sixel_state.hash_cap * ਆਕਾਰ(*ਕੁੰਜੀਆਂ));
	    mch_memmove(vals, sixel_state.vals,
		    (size_t)sixel_state.hash_cap * ਆਕਾਰ(*vals));
	}
	vim_free(sixel_state.ਕੁੰਜੀਆਂ);
	vim_free(sixel_state.vals);
	sixel_state.ਕੁੰਜੀਆਂ = ਕੁੰਜੀਆਂ;
	sixel_state.vals = vals;
	sixel_state.hash_cap = cap;
    }
    vim_memset(sixel_state.ਕੁੰਜੀਆਂ, 0, (size_t)cap * ਆਕਾਰ(*sixel_state.ਕੁੰਜੀਆਂ));
    *cap_ਬਾਹਰਲਾ = cap;
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
sixel_ensure_idx_ਸਮਰੱਥਾ(size_t pixels)
{
    ਜੇ (pixels > sixel_state.idx_len)
    {
	char_u *idx = vim_realloc(sixel_state.idx, pixels);

	ਜੇ (idx == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	sixel_state.idx = idx;
	sixel_state.idx_len = pixels;
    }
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
sixel_ensure_pal_ਸਮਰੱਥਾ(ਪੂਰਨ max_colors)
{
    ਪੂਰਨ need = max_colors * 3;

    ਜੇ (need > sixel_state.pal_len)
    {
	char_u *pal = vim_realloc(sixel_state.pal, (size_t)need);

	ਜੇ (pal == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	sixel_state.pal = pal;
	sixel_state.pal_len = need;
    }
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
sixel_ensure_band_ਸਮਰੱਥਾ(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ npal)
{
    size_t	    ਬਿੱਟ_need = (size_t)ਚੌੜਾਈ * (npal + 1);
    sixel_band_T    *band = &sixel_state.band;

    ਜੇ (ਬਿੱਟ_need > band->ਬਿੱਟ_len)
    {
	char_u *ਬਿੱਟ = vim_realloc(band->ਬਿੱਟ, ਬਿੱਟ_need);

	ਜੇ (ਬਿੱਟ == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	band->ਬਿੱਟ = ਬਿੱਟ;
	band->ਬਿੱਟ_len = ਬਿੱਟ_need;
    }
    ਜੇ (npal + 1 > band->color_len)
    {
	ਪੂਰਨ *ਵੇਖਿਆ = ALLOC_MULT(ਪੂਰਨ, npal + 1);
	ਪੂਰਨ *ਵਰਤਿਆ = ALLOC_MULT(ਪੂਰਨ, npal + 1);
	ਪੂਰਨ *xmin = ALLOC_MULT(ਪੂਰਨ, npal + 1);
	ਪੂਰਨ *xmax = ALLOC_MULT(ਪੂਰਨ, npal + 1);

	ਜੇ (ਵੇਖਿਆ == ਨਿਰਾ || ਵਰਤਿਆ == ਨਿਰਾ || xmin == ਨਿਰਾ || xmax == ਨਿਰਾ)
	{
	    vim_free(ਵੇਖਿਆ);
	    vim_free(ਵਰਤਿਆ);
	    vim_free(xmin);
	    vim_free(xmax);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (band->color_len > 0)
	{
	    mch_memmove(ਵੇਖਿਆ, band->ਵੇਖਿਆ,
		    (size_t)band->color_len * ਆਕਾਰ(*ਵੇਖਿਆ));
	    mch_memmove(ਵਰਤਿਆ, band->ਵਰਤਿਆ,
		    (size_t)band->color_len * ਆਕਾਰ(*ਵਰਤਿਆ));
	    mch_memmove(xmin, band->xmin,
		    (size_t)band->color_len * ਆਕਾਰ(*xmin));
	    mch_memmove(xmax, band->xmax,
		    (size_t)band->color_len * ਆਕਾਰ(*xmax));
	}

	// seen[] is checked against the band's generation counter; if the
	// newly grown tail held garbage equal to the current generation, the
	// band loop would read uninitialised xmin/xmax.  Zero the new tail
	// (and the matching slots in used/xmin/xmax for good measure) so the
	// "first time this colour is touched" branch is always taken.
	{
	    size_t ਪੂਛ = (size_t)(npal + 1 - band->color_len);

	    vim_memset(ਵੇਖਿਆ + band->color_len, 0, ਪੂਛ * ਆਕਾਰ(*ਵੇਖਿਆ));
	    vim_memset(ਵਰਤਿਆ + band->color_len, 0, ਪੂਛ * ਆਕਾਰ(*ਵਰਤਿਆ));
	    vim_memset(xmin + band->color_len, 0, ਪੂਛ * ਆਕਾਰ(*xmin));
	    vim_memset(xmax + band->color_len, 0, ਪੂਛ * ਆਕਾਰ(*xmax));
	}
	vim_free(band->ਵੇਖਿਆ);
	vim_free(band->ਵਰਤਿਆ);
	vim_free(band->xmin);
	vim_free(band->xmax);
	band->ਵੇਖਿਆ = ਵੇਖਿਆ;
	band->ਵਰਤਿਆ = ਵਰਤਿਆ;
	band->xmin = xmin;
	band->xmax = xmax;
	band->color_len = npal + 1;
    }
    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
rgb_ਮੰਜ਼ਿਲ_paletted_fast(
	char_u	*pixels,
	ਪੂਰਨ	 ਚੌੜਾਈ,
	ਪੂਰਨ	 ਉਚਾਈ,
	bool	 has_alpha,
	ਪੂਰਨ	 max_colors,
	char_u **pal_ਬਾਹਰਲਾ,
	ਪੂਰਨ	*npal_ਬਾਹਰਲਾ,
	char_u **idx_ਬਾਹਰਲਾ)
{
    ਪੂਰਨ		     cap;
    ਪੂਰਨ		     ਨਕਾਬ;
    ਪੂਰਨ		     ਵਰਤਿਆ = 0;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    *ਕੁੰਜੀਆਂ;
    char_u	    *vals;
    char_u	    *idx;
    char_u	    *pal;
    ਪੂਰਨ		     i, n;
    ਪੂਰਨ		     bpp = has_alpha ? 4 : 3;

    ਜੇ (sixel_ensure_hash_ਸਮਰੱਥਾ(max_colors, &cap) == FAIL
	    || sixel_ensure_idx_ਸਮਰੱਥਾ((size_t)ਚੌੜਾਈ * ਉਚਾਈ) == FAIL
	    || sixel_ensure_pal_ਸਮਰੱਥਾ(max_colors) == FAIL)
	ਵਾਪਸ FAIL;
    ਕੁੰਜੀਆਂ = sixel_state.ਕੁੰਜੀਆਂ;
    vals = sixel_state.vals;
    idx = sixel_state.idx;
    pal = sixel_state.pal;
    ਨਕਾਬ = cap - 1;

    n = ਚੌੜਾਈ * ਉਚਾਈ;
    ਲਈ (i = 0; i < n; i++)
    {
	char_u		*p = pixels + (size_t)i * bpp;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	 ਕੁੰਜੀ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	 h;
	ਪੂਰਨ		 ਥਾਂ;

	ਜੇ (has_alpha && p[3] < 128)
	{
	    idx[i] = 0;	    // transparent -- terminal leaves cell as-is
	    ਜਾਰੀ;
	}
	ਕੁੰਜੀ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)p[0] << 16)
	    | ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)p[1] << 8)
	    | (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)p[2];
	h = ਕੁੰਜੀ * 2654435761u;
	ਥਾਂ = (ਪੂਰਨ)(h & ਨਕਾਬ);

	ਲਈ (;;)
	{
	    ਜੇ (ਕੁੰਜੀਆਂ[ਥਾਂ] == 0)
	    {
		// new color
		ਜੇ (ਵਰਤਿਆ >= max_colors)
		    ਜਾਓ too_many;
		ਕੁੰਜੀਆਂ[ਥਾਂ] = ਕੁੰਜੀ + 1;
		vals[ਥਾਂ] = (char_u)(ਵਰਤਿਆ + 1);    // 1-based palette index
		pal[ਵਰਤਿਆ * 3]	  = p[0];
		pal[ਵਰਤਿਆ * 3 + 1] = p[1];
		pal[ਵਰਤਿਆ * 3 + 2] = p[2];
		ਵਰਤਿਆ++;
		idx[i] = vals[ਥਾਂ];
		ਰੋਕੋ;
	    }
	    ਜੇ (ਕੁੰਜੀਆਂ[ਥਾਂ] == ਕੁੰਜੀ + 1)
	    {
		idx[i] = vals[ਥਾਂ];
		ਰੋਕੋ;
	    }
	    ਥਾਂ = (ਥਾਂ + 1) & ਨਕਾਬ;
	}
    }
    *pal_ਬਾਹਰਲਾ = pal;
    *npal_ਬਾਹਰਲਾ = ਵਰਤਿਆ;
    *idx_ਬਾਹਰਲਾ = idx;
    ਵਾਪਸ OK;

too_many:
    ਵਾਪਸ FAIL;
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
    ਸਥਿਰ ਪੂਰਨ
rgb_ਮੰਜ਼ਿਲ_paletted_fixed(
	char_u	*pixels,
	ਪੂਰਨ	ਚੌੜਾਈ,
	ਪੂਰਨ	ਉਚਾਈ,
	bool	has_alpha,
	char_u	**pal_ਬਾਹਰਲਾ,
	ਪੂਰਨ	*npal_ਬਾਹਰਲਾ,
	char_u	**idx_ਬਾਹਰਲਾ)
{
    char_u  *idx;
    ਪੂਰਨ	     n;
    ਪੂਰਨ	     bpp = has_alpha ? 4 : 3;

    ਜੇ (sixel_ensure_idx_ਸਮਰੱਥਾ((size_t)ਚੌੜਾਈ * ਉਚਾਈ) == FAIL)
	ਵਾਪਸ FAIL;
    idx = sixel_state.idx;

    sixel_init_fixed_tables();

    // map every pixel to the nearest cube cell
    ਲਈ (n = 0; n < ਚੌੜਾਈ * ਉਚਾਈ; n++)
    {
	char_u	*p = pixels + (size_t)n * bpp;
	ਪੂਰਨ	 ri, gi, bi;

	ਜੇ (has_alpha && p[3] < 128)
	{
	    idx[n] = 0;
	    ਜਾਰੀ;
	}
	ri = sixel_rgb_cube_idx[p[0]];
	gi = sixel_rgb_cube_idx[p[1]];
	bi = sixel_rgb_cube_idx[p[2]];
	idx[n] = (char_u)(ri * 36 + gi * 6 + bi + 1);	// 1-based
    }

    *pal_ਬਾਹਰਲਾ = sixel_fixed_palette;
    *npal_ਬਾਹਰਲਾ = 240;
    *idx_ਬਾਹਰਲਾ = idx;
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਖਾਲੀ
sixel_image8_clear(sixel_image8_T *simg)
{
    vim_free(simg->idx);
    vim_free(simg->pal);
}

/*
 * Quantize an RGB(A) image to an 8bpp paletted image. Returns OK on success,
 * FAIL on invalid input or OOM.
 */
    ਸਥਿਰ ਪੂਰਨ
sixel_image_quantize(image_T *img, sixel_image8_T *ਬਾਹਰਲਾ)
{
    char_u  *pal = ਨਿਰਾ;
    char_u  *idx = ਨਿਰਾ;
    ਪੂਰਨ	     npal = 0;

    ਜੇ (rgb_ਮੰਜ਼ਿਲ_paletted_fast(img->ਅੰਕੜੇ, img->ਚੌੜਾਈ, img->ਉਚਾਈ,
		img->fmt == IMAGE_FORMAT_RGBA,
		SIXEL_MAX_COLORS, &pal, &npal, &idx) == FAIL)
    {
	ਜੇ (rgb_ਮੰਜ਼ਿਲ_paletted_fixed(img->ਅੰਕੜੇ, img->ਚੌੜਾਈ, img->ਉਚਾਈ,
		    img->fmt == IMAGE_FORMAT_RGBA,
		    &pal, &npal, &idx) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਬਾਹਰਲਾ->idx = vim_memsave(idx, (size_t)img->ਚੌੜਾਈ * img->ਉਚਾਈ);
    ਬਾਹਰਲਾ->ਚੌੜਾਈ = img->ਚੌੜਾਈ;
    ਬਾਹਰਲਾ->ਉਚਾਈ = img->ਉਚਾਈ;
    ਬਾਹਰਲਾ->pal = vim_memsave(pal, (size_t)npal * 3);
    ਬਾਹਰਲਾ->npal = npal;

    ਜੇ (ਬਾਹਰਲਾ->pal == ਨਿਰਾ || ਬਾਹਰਲਾ->idx == ਨਿਰਾ)
    {
	sixel_image8_clear(ਬਾਹਰਲਾ);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
sixel_image8_crop(
	sixel_image8_T	*src,
	ਪੂਰਨ		 x,
	ਪੂਰਨ		 y,
	ਪੂਰਨ		 w,
	ਪੂਰਨ		 h,
	sixel_view_T	*ਬਾਹਰਲਾ)
{
    ਪੂਰਨ x0, y0, x1, y1;   // clamped rectangle, half-open [x0,x1) x [y0,y1)

    ਜੇ (src == ਨਿਰਾ || ਬਾਹਰਲਾ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    x0 = x < 0 ? 0 : x;
    y0 = y < 0 ? 0 : y;
    x1 = x + w;
    y1 = y + h;
    ਜੇ (x1 > src->ਚੌੜਾਈ)
	x1 = src->ਚੌੜਾਈ;
    ਜੇ (y1 > src->ਉਚਾਈ)
	y1 = src->ਉਚਾਈ;
    ਜੇ (x1 < x0)
	x1 = x0;
    ਜੇ (y1 < y0)
	y1 = y0;

    ਜੇ (x1 <= x0 || y1 <= y0)
	ਵਾਪਸ FAIL;

    ਬਾਹਰਲਾ->stride = src->ਚੌੜਾਈ;
    ਬਾਹਰਲਾ->idx    = (x1 > x0 && y1 > y0)
	? src->idx + (size_t)y0 * src->ਚੌੜਾਈ + x0
	: ਨਿਰਾ;
    ਬਾਹਰਲਾ->ਚੌੜਾਈ  = x1 - x0;
    ਬਾਹਰਲਾ->ਉਚਾਈ = y1 - y0;
    ਬਾਹਰਲਾ->pal    = src->pal;
    ਬਾਹਰਲਾ->npal   = src->npal;

    ਵਾਪਸ OK;
}

/*
 * Emit a run of `cnt` identical sixel data bytes (ch in 0..63) into gap.
 * Uses RLE form `!N{c}` when it shortens the output.
 */
    ਸਥਿਰ ਪੂਰਨ
emit_ਲੜੀਵਾਰ(garray_T *gap, char_u ch, ਪੂਰਨ cnt)
{
    char_u  c = (char_u)(63 + ch);

    ਜਦੋਂ (cnt > 255)
    {
	ਜੇ (ga_concat_ਬਾਈਟਾਂ(gap, "!255", 4) == FAIL
		|| ga_append(gap, c) == FAIL)
	    ਵਾਪਸ FAIL;
	cnt -= 255;
    }
    ਜੇ (cnt <= 0)
	ਵਾਪਸ OK;
    ਜੇ (cnt <= 3)
    {
	ਜਦੋਂ (cnt-- > 0)
	    ਜੇ (ga_append(gap, c) == FAIL)
		ਵਾਪਸ FAIL;
	ਵਾਪਸ OK;
    }
    ਜੇ (ga_append(gap, '!') == FAIL
	    || ga_concat_int(gap, cnt) == FAIL
	    || ga_append(gap, c) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
sixel_encode_view(sixel_view_T *view, char_u *seq[2])
{
    garray_T	    hdr;    // DCS introducer, raster attrs, used-color defs
    garray_T	    body;   // band data (colour selects + sixel bytes)
    sixel_band_T    *band_state = &sixel_state.band;
    char_u	    *idx;
    char_u	    *pal;
    ਪੂਰਨ		    npal;
    ਪੂਰਨ		    ਚੌੜਾਈ, ਉਚਾਈ;
    ਪੂਰਨ		    band, p, x, n;
    bool	    defined[256];   // register already defined in header

    idx = view->idx;
    pal = view->pal;
    npal = view->npal;
    ਚੌੜਾਈ = view->ਚੌੜਾਈ;
    ਉਚਾਈ = view->ਉਚਾਈ;

    ga_init2(&hdr, ਆਕਾਰ(char_u), 256);
    ga_init2(&body, ਆਕਾਰ(char_u), 4096);
    vim_memset(defined, 0, ਆਕਾਰ(defined));

    // DECSIXEL Introducer + Raster Attributes "1;1;W;H.
    // P2=1 means pixel positions left unspecified by any colour register
    // keep their previous on-screen contents instead of being painted with
    // colour register 0.  That gives true transparency for RGBA images:
    // alpha < 128 pixels are emitted as palette index 0 (which we never
    // write to the bitmask), so the terminal leaves the popup's underlying
    // cell colour visible there -- no flatten, no colour-match drift.
    ਜੇ (ga_concat_ਬਾਈਟਾਂ(&hdr, "\033P0;1;8q\"1;1;", 13) == FAIL
	    || ga_concat_int(&hdr, ਚੌੜਾਈ) == FAIL
	    || ga_append(&hdr, ';') == FAIL
	    || ga_concat_int(&hdr, ਉਚਾਈ) == FAIL)
	ਜਾਓ fail;

    // bitmask buffer: (crop) width bytes per palette index, +1 for the
    // unused index 0.  Sized off the view's width, not the parent image's
    // width, so encoding a small crop out of a large quantized image only
    // needs scratch space proportional to the crop.
    ਜੇ (sixel_ensure_band_ਸਮਰੱਥਾ(ਚੌੜਾਈ, npal) == FAIL)
	ਜਾਓ fail;

    ਲਈ (band = 0; band < (ਉਚਾਈ + 5) / 6; band++)
    {
	ਪੂਰਨ gen;
	ਪੂਰਨ ਆਖਰੀ_was_cr = 0;
	ਪੂਰਨ ਵਰਤਿਆ_ਗਿਣਤੀ = 0;

	ਜੇ (++band_state->ਵੇਖਿਆ_gen <= 0)
	{
	    vim_memset(band_state->ਵੇਖਿਆ, 0, (npal + 1) * ਆਕਾਰ(ਪੂਰਨ));
	    band_state->ਵੇਖਿਆ_gen = 1;
	}
	gen = band_state->ਵੇਖਿਆ_gen;

	ਜੇ (band > 0 && ga_append(&body, '-') == FAIL)
	    ਜਾਓ fail;

	// Fill the bitmask for this band.
	ਲਈ (p = 0; p < 6; p++)
	{
	    ਪੂਰਨ y = band * 6 + p;
	    char_u  rowmask = (char_u)(1 << p);
	    char_u  *ਪੰਕਤੀ;

	    ਜੇ (y >= ਉਚਾਈ)
		ਜਾਰੀ;
	    // Use the *parent* stride to find the row -- "idx" already
	    // points at the crop's top-left corner, so only the row-to-row
	    // step needs to know about the parent's real width.
	    ਪੰਕਤੀ = idx + (size_t)y * view->stride;
	    ਲਈ (x = 0; x < ਚੌੜਾਈ; x++)
	    {
		char_u  pix = ਪੰਕਤੀ[x];

		ਜੇ (pix == 0)
		    ਜਾਰੀ;
		ਜੇ (band_state->ਵੇਖਿਆ[pix] != gen)
		{
		    band_state->ਵੇਖਿਆ[pix] = gen;
		    band_state->ਵਰਤਿਆ[ਵਰਤਿਆ_ਗਿਣਤੀ++] = pix;
		    band_state->xmin[pix] = x;
		    band_state->xmax[pix] = x;
		    vim_memset(band_state->ਬਿੱਟ + (size_t)pix * ਚੌੜਾਈ, 0, ਚੌੜਾਈ);
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (x < band_state->xmin[pix])
			band_state->xmin[pix] = x;
		    ਜੇ (x > band_state->xmax[pix])
			band_state->xmax[pix] = x;
		}
		band_state->ਬਿੱਟ[(size_t)pix * ਚੌੜਾਈ + x] |= rowmask;
	    }
	}

	ਲਈ (n = 0; n < ਵਰਤਿਆ_ਗਿਣਤੀ; n++)
	{
	    ਪੂਰਨ	     pix = band_state->ਵਰਤਿਆ[n];
	    char_u  *ਪੰਕਤੀ;
	    char_u   ch0;
	    ਪੂਰਨ	     cnt;
	    ਪੂਰਨ	     ਸ਼ੁਰੂ = band_state->xmin[pix];
	    ਪੂਰਨ	     ਸਿਰਾ = band_state->xmax[pix];

	    // First time this colour is used in the subrect: add its
	    // definition "#N;2;R;G;B" (RGB scaled to 0..100) to the header.
	    // Colours the subrect never touches are never defined.  Round to
	    // nearest, not truncate, so the round-trip 8bit -> 0..100 -> 8bit
	    // stays within ~1 level instead of drifting up to 2-3 levels
	    // darker.  This matters when the popup blends RGBA alpha onto the
	    // terminal background: truncation made the flattened bg visibly
	    // darker than the surrounding terminal cells.
	    ਜੇ (!defined[pix])
	    {
		ਪੂਰਨ r = (pal[(pix - 1) * 3]     * 100 + 127) / 255;
		ਪੂਰਨ g = (pal[(pix - 1) * 3 + 1] * 100 + 127) / 255;
		ਪੂਰਨ b = (pal[(pix - 1) * 3 + 2] * 100 + 127) / 255;

		defined[pix] = true;
		ਜੇ (ga_append(&hdr, '#') == FAIL
			|| ga_concat_int(&hdr, pix) == FAIL
			|| ga_concat_ਬਾਈਟਾਂ(&hdr, ";2;", 3) == FAIL
			|| ga_concat_int(&hdr, r) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, g) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, b) == FAIL)
		    ਜਾਓ fail;
	    }

	    ਜੇ (ਆਖਰੀ_was_cr && ga_append(&body, '$') == FAIL)
		ਜਾਓ fail;
	    ਜੇ (ga_append(&body, '#') == FAIL
		    || ga_concat_int(&body, pix) == FAIL)
		ਜਾਓ fail;

	    ਪੰਕਤੀ = band_state->ਬਿੱਟ + (size_t)pix * ਚੌੜਾਈ;
	    ਜੇ (ਸ਼ੁਰੂ > 0 && emit_ਲੜੀਵਾਰ(&body, 0, ਸ਼ੁਰੂ) == FAIL)
		ਜਾਓ fail;
	    ch0 = ਪੰਕਤੀ[ਸ਼ੁਰੂ];
	    cnt = 1;
	    ਲਈ (x = ਸ਼ੁਰੂ + 1; x <= ਸਿਰਾ; x++)
	    {
		char_u  ch = ਪੰਕਤੀ[x];

		ਜੇ (ch == ch0)
		{
		    cnt++;
		    ਜਾਰੀ;
		}
		ਜੇ (emit_ਲੜੀਵਾਰ(&body, ch0, cnt) == FAIL)
		    ਜਾਓ fail;
		ch0 = ch;
		cnt = 1;
	    }
	    ਜੇ (emit_ਲੜੀਵਾਰ(&body, ch0, cnt) == FAIL)
		ਜਾਓ fail;
	    ਆਖਰੀ_was_cr = 1;
	}
    }

    ਜੇ (ga_append(&hdr, NUL) == FAIL
	    || ga_concat_ਬਾਈਟਾਂ(&body, "\033\\", 3) == FAIL)
	ਜਾਓ fail;

    seq[0] = (char_u *)hdr.ga_ਅੰਕੜੇ;
    seq[1] = (char_u *)body.ga_ਅੰਕੜੇ;

    ਵਾਪਸ OK;

fail:
    ga_clear(&hdr);
    ga_clear(&body);
    ਵਾਪਸ FAIL;
}

    ਪੂਰਨ
image_sixel_init(image_T *img)
{
    image_sixel_T *ctx;

    ctx = ALLOC_CLEAR_ONE(image_sixel_T);
    ਜੇ (ctx == ਨਿਰਾ || sixel_image_quantize(img, &ctx->image8) == FAIL)
    {
	vim_free(ctx);
	ਵਾਪਸ FAIL;
    }

#ਜੇ defined(FEAT_IMAGE_SIXEL) && defined(FEAT_TERMRESPONSE)
    ਜੇ (!ਭੇਜਿਆ_decsdm)
    {
	// Disable sixel scrolling using DECSDM
	ਬਾਹਰਲਾ_str((char_u *)"\033[?80l");
	ਭੇਜਿਆ_decsdm = true;
    }
#ਜੇ_ਅੰਤ

    img->backend_ਅੰਕੜੇ = ctx;
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
image_sixel_uninit(image_T *img)
{
    image_sixel_T *ctx = img->backend_ਅੰਕੜੇ;

    sixel_image8_clear(&ctx->image8);
    vim_free(img->backend_ਅੰਕੜੇ);
}

    ਪੂਰਨ
image_placement_sixel_init(image_placement_T *ਰੱਖੋ)
{
    image_placement_sixel_T *ctx = ALLOC_CLEAR_ONE(image_placement_sixel_T);

    ਜੇ (ctx == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਰੱਖੋ->backend_ਅੰਕੜੇ = ctx;
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਖਾਲੀ
clear_chunks(image_placement_sixel_T *ctx)
{
    ਜੇ (ctx->chunks == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (ਪੂਰਨ i = 0; i < ctx->n_chunks; i++)
    {
	vim_free(ctx->chunks[i].seq[0]);
	vim_free(ctx->chunks[i].seq[1]);
    }
    VIM_CLEAR(ctx->chunks);
    ctx->n_chunks = 0;
}

    ਖਾਲੀ
image_placement_sixel_uninit(image_placement_T *ਰੱਖੋ)
{
    image_placement_sixel_T *ctx = ਰੱਖੋ->backend_ਅੰਕੜੇ;

    ਜੇ (ctx->visible_init)
	pixman_region32_fini(&ctx->visible_region);
    clear_chunks(ctx);
    vim_free(ctx);
}

    ਖਾਲੀ
image_placement_sixel_draw(image_placement_T *ਰੱਖੋ)
{
    image_T		    *img = ਰੱਖੋ->img;
    image_sixel_T	    *ictx = img->backend_ਅੰਕੜੇ;
    image_placement_sixel_T *ctx = ਰੱਖੋ->backend_ਅੰਕੜੇ;
    sixel_view_T	    view;
    pixman_box32_t	    *rects;
    ਪੂਰਨ			    n_rects;

    // Check if visible region is still the same, if so then use the cached
    // sixel sequences.
    ਜੇ (!(ਰੱਖੋ->flags & IMAGEPF_FORCE) && ctx->visible_init
	    && pixman_region32_equal(&ctx->visible_region, &ਰੱਖੋ->visible))
    {
	ਲਈ (ਪੂਰਨ i = 0; i < pixman_region32_n_rects(&ctx->visible_region); i++)
	{
	    sixel_chunk_T *chunk = ctx->chunks + i;

	    windgoto(ਰੱਖੋ->ਪੰਕਤੀ + ਰੱਖੋ->ਪੰਕਤੀ_off
		    + chunk->ਪੰਕਤੀ_off, ਰੱਖੋ->ਖਾਨਾ + chunk->ਖਾਨਾ_off);
	    ਜੇ (chunk->seq[0] != ਨਿਰਾ)
		ਬਾਹਰਲਾ_str(chunk->seq[0]);
	    ਜੇ (chunk->seq[1] != ਨਿਰਾ)
		ਬਾਹਰਲਾ_str(chunk->seq[1]);
	    screen_ਸ਼ੁਰੂ();
	}
	ਵਾਪਸ;
    }

    rects = pixman_region32_rectangles(&ਰੱਖੋ->visible, &n_rects);
    ਜੇ (rects == ਨਿਰਾ || n_rects == 0)
	ਵਾਪਸ;

    clear_chunks(ctx);
    ctx->chunks = ALLOC_CLEAR_MULT(sixel_chunk_T, n_rects);

    ਜੇ (ctx->chunks == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (ਪੂਰਨ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ਪੂਰਨ		ਪੰਕਤੀ, ਖਾਨਾ;
	ਪੂਰਨ		x, y, w, h;
	sixel_chunk_T	*chunk = ctx->chunks + i;
	char_u		*seq[2];

	image_placement_subrect(ਰੱਖੋ, rect, &ਪੰਕਤੀ, &ਖਾਨਾ, &x, &y, &w, &h, false);

	ਜੇ (sixel_image8_crop(&ictx->image8, x, y, w, h, &view) == FAIL
		|| sixel_encode_view(&view, seq) == FAIL)
	    ਜਾਰੀ;

	windgoto(ਪੰਕਤੀ, ਖਾਨਾ);
	ਜੇ (seq[0] != ਨਿਰਾ)
	    ਬਾਹਰਲਾ_str(seq[0]);
	ਜੇ (seq[1] != ਨਿਰਾ)
	    ਬਾਹਰਲਾ_str(seq[1]);
	screen_ਸ਼ੁਰੂ();

	chunk->ਪੰਕਤੀ_off = rect.y1;
	chunk->ਖਾਨਾ_off = rect.x1;
	chunk->seq[0] = seq[0];
	chunk->seq[1] = seq[1];
	ctx->n_chunks++;
    }

    ਜੇ (ctx->visible_init)
	pixman_region32_clear(&ctx->visible_region);
    pixman_region32_copy(&ctx->visible_region, &ਰੱਖੋ->visible);
    ctx->visible_init = true;
}

    ਖਾਲੀ
image_placement_sixel_clear(image_placement_T *ਰੱਖੋ ਅਣਵਰਤਿਆ)
{
}

#ਜੇ_ਅੰਤ // FEAT_IMAGE_SIXEL
