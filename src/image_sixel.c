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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_IMAGE_SIXEL)

// Palette size cap (sixel allows up to 256 color registers; index 0 is
// reserved as a transparent key, so usable colors are 1..MAX_COLORS).
#ವ್ಯಾಖ್ಯಾನಿಸು SIXEL_MAX_COLORS    255

/*
 * Working buffer for one band (6 pixel rows). Allocated as
 * width * (palette_size+1) bytes; bit p (0..5) marks pixel at row offset p.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u  *ಬಿಟ್ಟುಗಳು;	// bitmask buffer, width * paletteSize bytes
    size_t   ಬಿಟ್ಟುಗಳು_len;	// allocated bytes in "bits"
    ಪೂರ್ಣಾಂಕ	    *ನೋಡಿದ;	// per-color "used in this band" flags
    ಪೂರ್ಣಾಂಕ	    *ಬಳಸಿದ;	// colors used in this band
    ಪೂರ್ಣಾಂಕ	    *xmin;	// leftmost x used by color in this band
    ಪೂರ್ಣಾಂಕ	    *xmax;	// rightmost x used by color in this band
    ಪೂರ್ಣಾಂಕ	     color_len;	// allocated length for per-color arrays
    ಪೂರ್ಣಾಂಕ	     ನೋಡಿದ_gen;	// generation counter to avoid memset
} sixel_band_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    *ಕೀಲಿಗಳು;	// hash keys for dynamic palette lookup
    char_u	    *vals;	// hash values for dynamic palette lookup
    ಪೂರ್ಣಾಂಕ		     hash_cap;	// allocated slots in keys/vals
    char_u	    *idx;	// width * height paletted image
    size_t	     idx_len;	// allocated bytes in idx
    char_u	    *pal;	// dynamic palette storage (RGB triples)
    ಪೂರ್ಣಾಂಕ		     pal_len;	// allocated bytes in pal
    sixel_band_T     band;	// reusable band scratch
} sixel_state_T;

/*
 * A fully quantized (8bpp) image: one palette index per pixel, tightly
 * packed (stride == width), plus the palette that goes with it.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u  *idx; // Is width*height bytes, indices 1..npal (0 = transparent)
    ಪೂರ್ಣಾಂಕ      ಅಗಲ;
    ಪೂರ್ಣಾಂಕ      ಎತ್ತರ;
    char_u  *pal; // Is npal*3 bytes, RGB triples
    ಪೂರ್ಣಾಂಕ      npal;
} sixel_image8_T;

/*
 * A view into a sixel_image8_T: an offset + stride,
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u  *idx; // Points at (x0,y0) inside the parent buffer
    ಪೂರ್ಣಾಂಕ      stride; // Row length of the *parent* image, not this view
    ಪೂರ್ಣಾಂಕ      ಅಗಲ;
    ಪೂರ್ಣಾಂಕ      ಎತ್ತರ;
    char_u  *pal;
    ಪೂರ್ಣಾಂಕ      npal;
} sixel_view_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    // These are offsets relative to the top left of the image.
    ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು_off;
    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು_off;
    char_u  *seq[2]; // Header + body
} sixel_chunk_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    sixel_image8_T image8;
} image_sixel_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    pixman_region32_t	visible_region; // Region used for the previous redraw
    bool		visible_init;
    sixel_chunk_T	*chunks;    // Cached sixel sequence (stored as multiple
				    // chunks).
    ಪೂರ್ಣಾಂಕ			n_chunks;
} image_placement_sixel_T;

ಸ್ಥಿರ char_u		sixel_fixed_palette[240 * 3];
ಸ್ಥಿರ char_u		sixel_rgb_cube_idx[256];
ಸ್ಥಿರ bool		sixel_fixed_tables_ready = false;
ಸ್ಥಿರ sixel_state_T	sixel_state;
ಸ್ಥಿರ bool		ಕಳುಹಿಸಿದ_decsdm = false;

/*
 * Release all module-level allocations cached by the sixel encoder.
 */
    ಶೂನ್ಯ
sixel_uninit(ಶೂನ್ಯ)
{
    sixel_band_T *band = &sixel_state.band;

    VIM_CLEAR(sixel_state.ಕೀಲಿಗಳು);
    VIM_CLEAR(sixel_state.vals);
    VIM_CLEAR(sixel_state.idx);
    VIM_CLEAR(sixel_state.pal);
    sixel_state.hash_cap = 0;
    sixel_state.idx_len = 0;
    sixel_state.pal_len = 0;

    VIM_CLEAR(band->ಬಿಟ್ಟುಗಳು);
    VIM_CLEAR(band->ನೋಡಿದ);
    VIM_CLEAR(band->ಬಳಸಿದ);
    VIM_CLEAR(band->xmin);
    VIM_CLEAR(band->xmax);
    band->ಬಿಟ್ಟುಗಳು_len = 0;
    band->color_len = 0;
    band->ನೋಡಿದ_gen = 0;
}

/*
 * Initialize the shared fixed sixel palette and RGB->cube lookup table.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sixel_init_fixed_tables(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    r, g, b, gr, n;

    ಆದರೆ (sixel_fixed_tables_ready)
	ಹಿಂತಿರುಗಿ;

    n = 0;
    ಗಾಗಿ (r = 0; r < 6; r++)
	ಗಾಗಿ (g = 0; g < 6; g++)
	    ಗಾಗಿ (b = 0; b < 6; b++)
	    {
		sixel_fixed_palette[n * 3]     = (char_u)(r * 51);
		sixel_fixed_palette[n * 3 + 1] = (char_u)(g * 51);
		sixel_fixed_palette[n * 3 + 2] = (char_u)(b * 51);
		n++;
	    }
    ಗಾಗಿ (gr = 0; gr < 24; gr++)
    {
	ಪೂರ್ಣಾಂಕ v = 8 + gr * 10;

	sixel_fixed_palette[n * 3]     = (char_u)v;
	sixel_fixed_palette[n * 3 + 1] = (char_u)v;
	sixel_fixed_palette[n * 3 + 2] = (char_u)v;
	n++;
    }
    ಗಾಗಿ (n = 0; n < 256; n++)
    {
	ಪೂರ್ಣಾಂಕ idx = n / 43;

	sixel_rgb_cube_idx[n] = (char_u)(idx > 5 ? 5 : idx);
    }
    sixel_fixed_tables_ready = true;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sixel_ensure_hash_ಸಾಮರ್ಥ್ಯ(ಪೂರ್ಣಾಂಕ max_colors, ಪೂರ್ಣಾಂಕ *cap_ಹೊರಹೋಗು)
{
    ಪೂರ್ಣಾಂಕ cap = 1024;

    ಆಗ (cap < max_colors * 4)
	cap <<= 1;
    ಆದರೆ (cap > sixel_state.hash_cap)
    {
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	*ಕೀಲಿಗಳು;
	char_u		*vals;

	ಕೀಲಿಗಳು = ALLOC_MULT(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, cap);
	vals = ALLOC_MULT(char_u, cap);
	ಆದರೆ (ಕೀಲಿಗಳು == ಖಾಲಿ || vals == ಖಾಲಿ)
	{
	    vim_free(ಕೀಲಿಗಳು);
	    vim_free(vals);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (sixel_state.hash_cap > 0)
	{
	    mch_memmove(ಕೀಲಿಗಳು, sixel_state.ಕೀಲಿಗಳು,
		    (size_t)sixel_state.hash_cap * ಗಾತ್ರ(*ಕೀಲಿಗಳು));
	    mch_memmove(vals, sixel_state.vals,
		    (size_t)sixel_state.hash_cap * ಗಾತ್ರ(*vals));
	}
	vim_free(sixel_state.ಕೀಲಿಗಳು);
	vim_free(sixel_state.vals);
	sixel_state.ಕೀಲಿಗಳು = ಕೀಲಿಗಳು;
	sixel_state.vals = vals;
	sixel_state.hash_cap = cap;
    }
    vim_memset(sixel_state.ಕೀಲಿಗಳು, 0, (size_t)cap * ಗಾತ್ರ(*sixel_state.ಕೀಲಿಗಳು));
    *cap_ಹೊರಹೋಗು = cap;
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sixel_ensure_idx_ಸಾಮರ್ಥ್ಯ(size_t pixels)
{
    ಆದರೆ (pixels > sixel_state.idx_len)
    {
	char_u *idx = vim_realloc(sixel_state.idx, pixels);

	ಆದರೆ (idx == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	sixel_state.idx = idx;
	sixel_state.idx_len = pixels;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sixel_ensure_pal_ಸಾಮರ್ಥ್ಯ(ಪೂರ್ಣಾಂಕ max_colors)
{
    ಪೂರ್ಣಾಂಕ need = max_colors * 3;

    ಆದರೆ (need > sixel_state.pal_len)
    {
	char_u *pal = vim_realloc(sixel_state.pal, (size_t)need);

	ಆದರೆ (pal == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	sixel_state.pal = pal;
	sixel_state.pal_len = need;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sixel_ensure_band_ಸಾಮರ್ಥ್ಯ(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ npal)
{
    size_t	    ಬಿಟ್ಟುಗಳು_need = (size_t)ಅಗಲ * (npal + 1);
    sixel_band_T    *band = &sixel_state.band;

    ಆದರೆ (ಬಿಟ್ಟುಗಳು_need > band->ಬಿಟ್ಟುಗಳು_len)
    {
	char_u *ಬಿಟ್ಟುಗಳು = vim_realloc(band->ಬಿಟ್ಟುಗಳು, ಬಿಟ್ಟುಗಳು_need);

	ಆದರೆ (ಬಿಟ್ಟುಗಳು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	band->ಬಿಟ್ಟುಗಳು = ಬಿಟ್ಟುಗಳು;
	band->ಬಿಟ್ಟುಗಳು_len = ಬಿಟ್ಟುಗಳು_need;
    }
    ಆದರೆ (npal + 1 > band->color_len)
    {
	ಪೂರ್ಣಾಂಕ *ನೋಡಿದ = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, npal + 1);
	ಪೂರ್ಣಾಂಕ *ಬಳಸಿದ = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, npal + 1);
	ಪೂರ್ಣಾಂಕ *xmin = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, npal + 1);
	ಪೂರ್ಣಾಂಕ *xmax = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, npal + 1);

	ಆದರೆ (ನೋಡಿದ == ಖಾಲಿ || ಬಳಸಿದ == ಖಾಲಿ || xmin == ಖಾಲಿ || xmax == ಖಾಲಿ)
	{
	    vim_free(ನೋಡಿದ);
	    vim_free(ಬಳಸಿದ);
	    vim_free(xmin);
	    vim_free(xmax);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (band->color_len > 0)
	{
	    mch_memmove(ನೋಡಿದ, band->ನೋಡಿದ,
		    (size_t)band->color_len * ಗಾತ್ರ(*ನೋಡಿದ));
	    mch_memmove(ಬಳಸಿದ, band->ಬಳಸಿದ,
		    (size_t)band->color_len * ಗಾತ್ರ(*ಬಳಸಿದ));
	    mch_memmove(xmin, band->xmin,
		    (size_t)band->color_len * ಗಾತ್ರ(*xmin));
	    mch_memmove(xmax, band->xmax,
		    (size_t)band->color_len * ಗಾತ್ರ(*xmax));
	}

	// seen[] is checked against the band's generation counter; if the
	// newly grown tail held garbage equal to the current generation, the
	// band loop would read uninitialised xmin/xmax.  Zero the new tail
	// (and the matching slots in used/xmin/xmax for good measure) so the
	// "first time this colour is touched" branch is always taken.
	{
	    size_t ಬಾಲ = (size_t)(npal + 1 - band->color_len);

	    vim_memset(ನೋಡಿದ + band->color_len, 0, ಬಾಲ * ಗಾತ್ರ(*ನೋಡಿದ));
	    vim_memset(ಬಳಸಿದ + band->color_len, 0, ಬಾಲ * ಗಾತ್ರ(*ಬಳಸಿದ));
	    vim_memset(xmin + band->color_len, 0, ಬಾಲ * ಗಾತ್ರ(*xmin));
	    vim_memset(xmax + band->color_len, 0, ಬಾಲ * ಗಾತ್ರ(*xmax));
	}
	vim_free(band->ನೋಡಿದ);
	vim_free(band->ಬಳಸಿದ);
	vim_free(band->xmin);
	vim_free(band->xmax);
	band->ನೋಡಿದ = ನೋಡಿದ;
	band->ಬಳಸಿದ = ಬಳಸಿದ;
	band->xmin = xmin;
	band->xmax = xmax;
	band->color_len = npal + 1;
    }
    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
rgb_ಗಮ್ಯ_paletted_fast(
	char_u	*pixels,
	ಪೂರ್ಣಾಂಕ	 ಅಗಲ,
	ಪೂರ್ಣಾಂಕ	 ಎತ್ತರ,
	bool	 has_alpha,
	ಪೂರ್ಣಾಂಕ	 max_colors,
	char_u **pal_ಹೊರಹೋಗು,
	ಪೂರ್ಣಾಂಕ	*npal_ಹೊರಹೋಗು,
	char_u **idx_ಹೊರಹೋಗು)
{
    ಪೂರ್ಣಾಂಕ		     cap;
    ಪೂರ್ಣಾಂಕ		     ಮುಖವಾಡ;
    ಪೂರ್ಣಾಂಕ		     ಬಳಸಿದ = 0;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    *ಕೀಲಿಗಳು;
    char_u	    *vals;
    char_u	    *idx;
    char_u	    *pal;
    ಪೂರ್ಣಾಂಕ		     i, n;
    ಪೂರ್ಣಾಂಕ		     bpp = has_alpha ? 4 : 3;

    ಆದರೆ (sixel_ensure_hash_ಸಾಮರ್ಥ್ಯ(max_colors, &cap) == FAIL
	    || sixel_ensure_idx_ಸಾಮರ್ಥ್ಯ((size_t)ಅಗಲ * ಎತ್ತರ) == FAIL
	    || sixel_ensure_pal_ಸಾಮರ್ಥ್ಯ(max_colors) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಕೀಲಿಗಳು = sixel_state.ಕೀಲಿಗಳು;
    vals = sixel_state.vals;
    idx = sixel_state.idx;
    pal = sixel_state.pal;
    ಮುಖವಾಡ = cap - 1;

    n = ಅಗಲ * ಎತ್ತರ;
    ಗಾಗಿ (i = 0; i < n; i++)
    {
	char_u		*p = pixels + (size_t)i * bpp;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	 ಕೀಲಿ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	 h;
	ಪೂರ್ಣಾಂಕ		 ಸ್ಥಾನ;

	ಆದರೆ (has_alpha && p[3] < 128)
	{
	    idx[i] = 0;	    // transparent -- terminal leaves cell as-is
	    ಮುಂದುವರಿ;
	}
	ಕೀಲಿ = ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)p[0] << 16)
	    | ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)p[1] << 8)
	    | (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)p[2];
	h = ಕೀಲಿ * 2654435761u;
	ಸ್ಥಾನ = (ಪೂರ್ಣಾಂಕ)(h & ಮುಖವಾಡ);

	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (ಕೀಲಿಗಳು[ಸ್ಥಾನ] == 0)
	    {
		// new color
		ಆದರೆ (ಬಳಸಿದ >= max_colors)
		    ಹೋಗು too_many;
		ಕೀಲಿಗಳು[ಸ್ಥಾನ] = ಕೀಲಿ + 1;
		vals[ಸ್ಥಾನ] = (char_u)(ಬಳಸಿದ + 1);    // 1-based palette index
		pal[ಬಳಸಿದ * 3]	  = p[0];
		pal[ಬಳಸಿದ * 3 + 1] = p[1];
		pal[ಬಳಸಿದ * 3 + 2] = p[2];
		ಬಳಸಿದ++;
		idx[i] = vals[ಸ್ಥಾನ];
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (ಕೀಲಿಗಳು[ಸ್ಥಾನ] == ಕೀಲಿ + 1)
	    {
		idx[i] = vals[ಸ್ಥಾನ];
		ನಿಲ್ಲಿಸು;
	    }
	    ಸ್ಥಾನ = (ಸ್ಥಾನ + 1) & ಮುಖವಾಡ;
	}
    }
    *pal_ಹೊರಹೋಗು = pal;
    *npal_ಹೊರಹೋಗು = ಬಳಸಿದ;
    *idx_ಹೊರಹೋಗು = idx;
    ಹಿಂತಿರುಗಿ OK;

too_many:
    ಹಿಂತಿರುಗಿ FAIL;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
rgb_ಗಮ್ಯ_paletted_fixed(
	char_u	*pixels,
	ಪೂರ್ಣಾಂಕ	ಅಗಲ,
	ಪೂರ್ಣಾಂಕ	ಎತ್ತರ,
	bool	has_alpha,
	char_u	**pal_ಹೊರಹೋಗು,
	ಪೂರ್ಣಾಂಕ	*npal_ಹೊರಹೋಗು,
	char_u	**idx_ಹೊರಹೋಗು)
{
    char_u  *idx;
    ಪೂರ್ಣಾಂಕ	     n;
    ಪೂರ್ಣಾಂಕ	     bpp = has_alpha ? 4 : 3;

    ಆದರೆ (sixel_ensure_idx_ಸಾಮರ್ಥ್ಯ((size_t)ಅಗಲ * ಎತ್ತರ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    idx = sixel_state.idx;

    sixel_init_fixed_tables();

    // map every pixel to the nearest cube cell
    ಗಾಗಿ (n = 0; n < ಅಗಲ * ಎತ್ತರ; n++)
    {
	char_u	*p = pixels + (size_t)n * bpp;
	ಪೂರ್ಣಾಂಕ	 ri, gi, bi;

	ಆದರೆ (has_alpha && p[3] < 128)
	{
	    idx[n] = 0;
	    ಮುಂದುವರಿ;
	}
	ri = sixel_rgb_cube_idx[p[0]];
	gi = sixel_rgb_cube_idx[p[1]];
	bi = sixel_rgb_cube_idx[p[2]];
	idx[n] = (char_u)(ri * 36 + gi * 6 + bi + 1);	// 1-based
    }

    *pal_ಹೊರಹೋಗು = sixel_fixed_palette;
    *npal_ಹೊರಹೋಗು = 240;
    *idx_ಹೊರಹೋಗು = idx;
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
sixel_image8_clear(sixel_image8_T *simg)
{
    vim_free(simg->idx);
    vim_free(simg->pal);
}

/*
 * Quantize an RGB(A) image to an 8bpp paletted image. Returns OK on success,
 * FAIL on invalid input or OOM.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sixel_image_quantize(image_T *img, sixel_image8_T *ಹೊರಹೋಗು)
{
    char_u  *pal = ಖಾಲಿ;
    char_u  *idx = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ	     npal = 0;

    ಆದರೆ (rgb_ಗಮ್ಯ_paletted_fast(img->ದತ್ತಾಂಶ, img->ಅಗಲ, img->ಎತ್ತರ,
		img->fmt == IMAGE_FORMAT_RGBA,
		SIXEL_MAX_COLORS, &pal, &npal, &idx) == FAIL)
    {
	ಆದರೆ (rgb_ಗಮ್ಯ_paletted_fixed(img->ದತ್ತಾಂಶ, img->ಅಗಲ, img->ಎತ್ತರ,
		    img->fmt == IMAGE_FORMAT_RGBA,
		    &pal, &npal, &idx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹೊರಹೋಗು->idx = vim_memsave(idx, (size_t)img->ಅಗಲ * img->ಎತ್ತರ);
    ಹೊರಹೋಗು->ಅಗಲ = img->ಅಗಲ;
    ಹೊರಹೋಗು->ಎತ್ತರ = img->ಎತ್ತರ;
    ಹೊರಹೋಗು->pal = vim_memsave(pal, (size_t)npal * 3);
    ಹೊರಹೋಗು->npal = npal;

    ಆದರೆ (ಹೊರಹೋಗು->pal == ಖಾಲಿ || ಹೊರಹೋಗು->idx == ಖಾಲಿ)
    {
	sixel_image8_clear(ಹೊರಹೋಗು);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sixel_image8_crop(
	sixel_image8_T	*src,
	ಪೂರ್ಣಾಂಕ		 x,
	ಪೂರ್ಣಾಂಕ		 y,
	ಪೂರ್ಣಾಂಕ		 w,
	ಪೂರ್ಣಾಂಕ		 h,
	sixel_view_T	*ಹೊರಹೋಗು)
{
    ಪೂರ್ಣಾಂಕ x0, y0, x1, y1;   // clamped rectangle, half-open [x0,x1) x [y0,y1)

    ಆದರೆ (src == ಖಾಲಿ || ಹೊರಹೋಗು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    x0 = x < 0 ? 0 : x;
    y0 = y < 0 ? 0 : y;
    x1 = x + w;
    y1 = y + h;
    ಆದರೆ (x1 > src->ಅಗಲ)
	x1 = src->ಅಗಲ;
    ಆದರೆ (y1 > src->ಎತ್ತರ)
	y1 = src->ಎತ್ತರ;
    ಆದರೆ (x1 < x0)
	x1 = x0;
    ಆದರೆ (y1 < y0)
	y1 = y0;

    ಆದರೆ (x1 <= x0 || y1 <= y0)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹೊರಹೋಗು->stride = src->ಅಗಲ;
    ಹೊರಹೋಗು->idx    = (x1 > x0 && y1 > y0)
	? src->idx + (size_t)y0 * src->ಅಗಲ + x0
	: ಖಾಲಿ;
    ಹೊರಹೋಗು->ಅಗಲ  = x1 - x0;
    ಹೊರಹೋಗು->ಎತ್ತರ = y1 - y0;
    ಹೊರಹೋಗು->pal    = src->pal;
    ಹೊರಹೋಗು->npal   = src->npal;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Emit a run of `cnt` identical sixel data bytes (ch in 0..63) into gap.
 * Uses RLE form `!N{c}` when it shortens the output.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
emit_ಸತತ(garray_T *gap, char_u ch, ಪೂರ್ಣಾಂಕ cnt)
{
    char_u  c = (char_u)(63 + ch);

    ಆಗ (cnt > 255)
    {
	ಆದರೆ (ga_concat_ಬೈಟುಗಳು(gap, "!255", 4) == FAIL
		|| ga_append(gap, c) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	cnt -= 255;
    }
    ಆದರೆ (cnt <= 0)
	ಹಿಂತಿರುಗಿ OK;
    ಆದರೆ (cnt <= 3)
    {
	ಆಗ (cnt-- > 0)
	    ಆದರೆ (ga_append(gap, c) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಆದರೆ (ga_append(gap, '!') == FAIL
	    || ga_concat_int(gap, cnt) == FAIL
	    || ga_append(gap, c) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sixel_encode_view(sixel_view_T *view, char_u *seq[2])
{
    garray_T	    hdr;    // DCS introducer, raster attrs, used-color defs
    garray_T	    body;   // band data (colour selects + sixel bytes)
    sixel_band_T    *band_state = &sixel_state.band;
    char_u	    *idx;
    char_u	    *pal;
    ಪೂರ್ಣಾಂಕ		    npal;
    ಪೂರ್ಣಾಂಕ		    ಅಗಲ, ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		    band, p, x, n;
    bool	    defined[256];   // register already defined in header

    idx = view->idx;
    pal = view->pal;
    npal = view->npal;
    ಅಗಲ = view->ಅಗಲ;
    ಎತ್ತರ = view->ಎತ್ತರ;

    ga_init2(&hdr, ಗಾತ್ರ(char_u), 256);
    ga_init2(&body, ಗಾತ್ರ(char_u), 4096);
    vim_memset(defined, 0, ಗಾತ್ರ(defined));

    // DECSIXEL Introducer + Raster Attributes "1;1;W;H.
    // P2=1 means pixel positions left unspecified by any colour register
    // keep their previous on-screen contents instead of being painted with
    // colour register 0.  That gives true transparency for RGBA images:
    // alpha < 128 pixels are emitted as palette index 0 (which we never
    // write to the bitmask), so the terminal leaves the popup's underlying
    // cell colour visible there -- no flatten, no colour-match drift.
    ಆದರೆ (ga_concat_ಬೈಟುಗಳು(&hdr, "\033P0;1;8q\"1;1;", 13) == FAIL
	    || ga_concat_int(&hdr, ಅಗಲ) == FAIL
	    || ga_append(&hdr, ';') == FAIL
	    || ga_concat_int(&hdr, ಎತ್ತರ) == FAIL)
	ಹೋಗು fail;

    // bitmask buffer: (crop) width bytes per palette index, +1 for the
    // unused index 0.  Sized off the view's width, not the parent image's
    // width, so encoding a small crop out of a large quantized image only
    // needs scratch space proportional to the crop.
    ಆದರೆ (sixel_ensure_band_ಸಾಮರ್ಥ್ಯ(ಅಗಲ, npal) == FAIL)
	ಹೋಗು fail;

    ಗಾಗಿ (band = 0; band < (ಎತ್ತರ + 5) / 6; band++)
    {
	ಪೂರ್ಣಾಂಕ gen;
	ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_was_cr = 0;
	ಪೂರ್ಣಾಂಕ ಬಳಸಿದ_ಎಣಿಕೆ = 0;

	ಆದರೆ (++band_state->ನೋಡಿದ_gen <= 0)
	{
	    vim_memset(band_state->ನೋಡಿದ, 0, (npal + 1) * ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
	    band_state->ನೋಡಿದ_gen = 1;
	}
	gen = band_state->ನೋಡಿದ_gen;

	ಆದರೆ (band > 0 && ga_append(&body, '-') == FAIL)
	    ಹೋಗು fail;

	// Fill the bitmask for this band.
	ಗಾಗಿ (p = 0; p < 6; p++)
	{
	    ಪೂರ್ಣಾಂಕ y = band * 6 + p;
	    char_u  rowmask = (char_u)(1 << p);
	    char_u  *ಅಡ್ಡಸಾಲು;

	    ಆದರೆ (y >= ಎತ್ತರ)
		ಮುಂದುವರಿ;
	    // Use the *parent* stride to find the row -- "idx" already
	    // points at the crop's top-left corner, so only the row-to-row
	    // step needs to know about the parent's real width.
	    ಅಡ್ಡಸಾಲು = idx + (size_t)y * view->stride;
	    ಗಾಗಿ (x = 0; x < ಅಗಲ; x++)
	    {
		char_u  pix = ಅಡ್ಡಸಾಲು[x];

		ಆದರೆ (pix == 0)
		    ಮುಂದುವರಿ;
		ಆದರೆ (band_state->ನೋಡಿದ[pix] != gen)
		{
		    band_state->ನೋಡಿದ[pix] = gen;
		    band_state->ಬಳಸಿದ[ಬಳಸಿದ_ಎಣಿಕೆ++] = pix;
		    band_state->xmin[pix] = x;
		    band_state->xmax[pix] = x;
		    vim_memset(band_state->ಬಿಟ್ಟುಗಳು + (size_t)pix * ಅಗಲ, 0, ಅಗಲ);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (x < band_state->xmin[pix])
			band_state->xmin[pix] = x;
		    ಆದರೆ (x > band_state->xmax[pix])
			band_state->xmax[pix] = x;
		}
		band_state->ಬಿಟ್ಟುಗಳು[(size_t)pix * ಅಗಲ + x] |= rowmask;
	    }
	}

	ಗಾಗಿ (n = 0; n < ಬಳಸಿದ_ಎಣಿಕೆ; n++)
	{
	    ಪೂರ್ಣಾಂಕ	     pix = band_state->ಬಳಸಿದ[n];
	    char_u  *ಅಡ್ಡಸಾಲು;
	    char_u   ch0;
	    ಪೂರ್ಣಾಂಕ	     cnt;
	    ಪೂರ್ಣಾಂಕ	     ಆರಂಭ = band_state->xmin[pix];
	    ಪೂರ್ಣಾಂಕ	     ಕೊನೆ = band_state->xmax[pix];

	    // First time this colour is used in the subrect: add its
	    // definition "#N;2;R;G;B" (RGB scaled to 0..100) to the header.
	    // Colours the subrect never touches are never defined.  Round to
	    // nearest, not truncate, so the round-trip 8bit -> 0..100 -> 8bit
	    // stays within ~1 level instead of drifting up to 2-3 levels
	    // darker.  This matters when the popup blends RGBA alpha onto the
	    // terminal background: truncation made the flattened bg visibly
	    // darker than the surrounding terminal cells.
	    ಆದರೆ (!defined[pix])
	    {
		ಪೂರ್ಣಾಂಕ r = (pal[(pix - 1) * 3]     * 100 + 127) / 255;
		ಪೂರ್ಣಾಂಕ g = (pal[(pix - 1) * 3 + 1] * 100 + 127) / 255;
		ಪೂರ್ಣಾಂಕ b = (pal[(pix - 1) * 3 + 2] * 100 + 127) / 255;

		defined[pix] = true;
		ಆದರೆ (ga_append(&hdr, '#') == FAIL
			|| ga_concat_int(&hdr, pix) == FAIL
			|| ga_concat_ಬೈಟುಗಳು(&hdr, ";2;", 3) == FAIL
			|| ga_concat_int(&hdr, r) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, g) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, b) == FAIL)
		    ಹೋಗು fail;
	    }

	    ಆದರೆ (ಕೊನೆಯ_was_cr && ga_append(&body, '$') == FAIL)
		ಹೋಗು fail;
	    ಆದರೆ (ga_append(&body, '#') == FAIL
		    || ga_concat_int(&body, pix) == FAIL)
		ಹೋಗು fail;

	    ಅಡ್ಡಸಾಲು = band_state->ಬಿಟ್ಟುಗಳು + (size_t)pix * ಅಗಲ;
	    ಆದರೆ (ಆರಂಭ > 0 && emit_ಸತತ(&body, 0, ಆರಂಭ) == FAIL)
		ಹೋಗು fail;
	    ch0 = ಅಡ್ಡಸಾಲು[ಆರಂಭ];
	    cnt = 1;
	    ಗಾಗಿ (x = ಆರಂಭ + 1; x <= ಕೊನೆ; x++)
	    {
		char_u  ch = ಅಡ್ಡಸಾಲು[x];

		ಆದರೆ (ch == ch0)
		{
		    cnt++;
		    ಮುಂದುವರಿ;
		}
		ಆದರೆ (emit_ಸತತ(&body, ch0, cnt) == FAIL)
		    ಹೋಗು fail;
		ch0 = ch;
		cnt = 1;
	    }
	    ಆದರೆ (emit_ಸತತ(&body, ch0, cnt) == FAIL)
		ಹೋಗು fail;
	    ಕೊನೆಯ_was_cr = 1;
	}
    }

    ಆದರೆ (ga_append(&hdr, NUL) == FAIL
	    || ga_concat_ಬೈಟುಗಳು(&body, "\033\\", 3) == FAIL)
	ಹೋಗು fail;

    seq[0] = (char_u *)hdr.ga_ದತ್ತಾಂಶ;
    seq[1] = (char_u *)body.ga_ದತ್ತಾಂಶ;

    ಹಿಂತಿರುಗಿ OK;

fail:
    ga_clear(&hdr);
    ga_clear(&body);
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಪೂರ್ಣಾಂಕ
image_sixel_init(image_T *img)
{
    image_sixel_T *ctx;

    ctx = ALLOC_CLEAR_ONE(image_sixel_T);
    ಆದರೆ (ctx == ಖಾಲಿ || sixel_image_quantize(img, &ctx->image8) == FAIL)
    {
	vim_free(ctx);
	ಹಿಂತಿರುಗಿ FAIL;
    }

#ಆದರೆ defined(FEAT_IMAGE_SIXEL) && defined(FEAT_TERMRESPONSE)
    ಆದರೆ (!ಕಳುಹಿಸಿದ_decsdm)
    {
	// Disable sixel scrolling using DECSDM
	ಹೊರಹೋಗು_str((char_u *)"\033[?80l");
	ಕಳುಹಿಸಿದ_decsdm = true;
    }
#ಆದರೆ_ಅಂತ್ಯ

    img->backend_ದತ್ತಾಂಶ = ctx;
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
image_sixel_uninit(image_T *img)
{
    image_sixel_T *ctx = img->backend_ದತ್ತಾಂಶ;

    sixel_image8_clear(&ctx->image8);
    vim_free(img->backend_ದತ್ತಾಂಶ);
}

    ಪೂರ್ಣಾಂಕ
image_placement_sixel_init(image_placement_T *ಇರಿಸು)
{
    image_placement_sixel_T *ctx = ALLOC_CLEAR_ONE(image_placement_sixel_T);

    ಆದರೆ (ctx == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಇರಿಸು->backend_ದತ್ತಾಂಶ = ctx;
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_chunks(image_placement_sixel_T *ctx)
{
    ಆದರೆ (ctx->chunks == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ctx->n_chunks; i++)
    {
	vim_free(ctx->chunks[i].seq[0]);
	vim_free(ctx->chunks[i].seq[1]);
    }
    VIM_CLEAR(ctx->chunks);
    ctx->n_chunks = 0;
}

    ಶೂನ್ಯ
image_placement_sixel_uninit(image_placement_T *ಇರಿಸು)
{
    image_placement_sixel_T *ctx = ಇರಿಸು->backend_ದತ್ತಾಂಶ;

    ಆದರೆ (ctx->visible_init)
	pixman_region32_fini(&ctx->visible_region);
    clear_chunks(ctx);
    vim_free(ctx);
}

    ಶೂನ್ಯ
image_placement_sixel_draw(image_placement_T *ಇರಿಸು)
{
    image_T		    *img = ಇರಿಸು->img;
    image_sixel_T	    *ictx = img->backend_ದತ್ತಾಂಶ;
    image_placement_sixel_T *ctx = ಇರಿಸು->backend_ದತ್ತಾಂಶ;
    sixel_view_T	    view;
    pixman_box32_t	    *rects;
    ಪೂರ್ಣಾಂಕ			    n_rects;

    // Check if visible region is still the same, if so then use the cached
    // sixel sequences.
    ಆದರೆ (!(ಇರಿಸು->flags & IMAGEPF_FORCE) && ctx->visible_init
	    && pixman_region32_equal(&ctx->visible_region, &ಇರಿಸು->visible))
    {
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < pixman_region32_n_rects(&ctx->visible_region); i++)
	{
	    sixel_chunk_T *chunk = ctx->chunks + i;

	    windgoto(ಇರಿಸು->ಅಡ್ಡಸಾಲು + ಇರಿಸು->ಅಡ್ಡಸಾಲು_off
		    + chunk->ಅಡ್ಡಸಾಲು_off, ಇರಿಸು->ಲಂಬಸಾಲು + chunk->ಲಂಬಸಾಲು_off);
	    ಆದರೆ (chunk->seq[0] != ಖಾಲಿ)
		ಹೊರಹೋಗು_str(chunk->seq[0]);
	    ಆದರೆ (chunk->seq[1] != ಖಾಲಿ)
		ಹೊರಹೋಗು_str(chunk->seq[1]);
	    screen_ಆರಂಭ();
	}
	ಹಿಂತಿರುಗಿ;
    }

    rects = pixman_region32_rectangles(&ಇರಿಸು->visible, &n_rects);
    ಆದರೆ (rects == ಖಾಲಿ || n_rects == 0)
	ಹಿಂತಿರುಗಿ;

    clear_chunks(ctx);
    ctx->chunks = ALLOC_CLEAR_MULT(sixel_chunk_T, n_rects);

    ಆದರೆ (ctx->chunks == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ		x, y, w, h;
	sixel_chunk_T	*chunk = ctx->chunks + i;
	char_u		*seq[2];

	image_placement_subrect(ಇರಿಸು, rect, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, &x, &y, &w, &h, false);

	ಆದರೆ (sixel_image8_crop(&ictx->image8, x, y, w, h, &view) == FAIL
		|| sixel_encode_view(&view, seq) == FAIL)
	    ಮುಂದುವರಿ;

	windgoto(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	ಆದರೆ (seq[0] != ಖಾಲಿ)
	    ಹೊರಹೋಗು_str(seq[0]);
	ಆದರೆ (seq[1] != ಖಾಲಿ)
	    ಹೊರಹೋಗು_str(seq[1]);
	screen_ಆರಂಭ();

	chunk->ಅಡ್ಡಸಾಲು_off = rect.y1;
	chunk->ಲಂಬಸಾಲು_off = rect.x1;
	chunk->seq[0] = seq[0];
	chunk->seq[1] = seq[1];
	ctx->n_chunks++;
    }

    ಆದರೆ (ctx->visible_init)
	pixman_region32_clear(&ctx->visible_region);
    pixman_region32_copy(&ctx->visible_region, &ಇರಿಸು->visible);
    ctx->visible_init = true;
}

    ಶೂನ್ಯ
image_placement_sixel_clear(image_placement_T *ಇರಿಸು ಬಳಸದ)
{
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_IMAGE_SIXEL
