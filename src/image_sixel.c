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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_IMAGE_SIXEL)

// Palette size cap (sixel allows up to 256 color registers; index 0 is
// reserved as a transparent key, so usable colors are 1..MAX_COLORS).
#నిర్వచించు SIXEL_MAX_COLORS    255

/*
 * Working buffer for one band (6 pixel rows). Allocated as
 * width * (palette_size+1) bytes; bit p (0..5) marks pixel at row offset p.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u  *బిట్లు;	// bitmask buffer, width * paletteSize bytes
    size_t   బిట్లు_len;	// allocated bytes in "bits"
    పూర్ణాంకం	    *చూసిన;	// per-color "used in this band" flags
    పూర్ణాంకం	    *వాడిన;	// colors used in this band
    పూర్ణాంకం	    *xmin;	// leftmost x used by color in this band
    పూర్ణాంకం	    *xmax;	// rightmost x used by color in this band
    పూర్ణాంకం	     color_len;	// allocated length for per-color arrays
    పూర్ణాంకం	     చూసిన_gen;	// generation counter to avoid memset
} sixel_band_T;

రకనిర్వచనం నిర్మాణం
{
    గుర్తులేని పూర్ణాంకం    *కీలకాలు;	// hash keys for dynamic palette lookup
    char_u	    *vals;	// hash values for dynamic palette lookup
    పూర్ణాంకం		     hash_cap;	// allocated slots in keys/vals
    char_u	    *idx;	// width * height paletted image
    size_t	     idx_len;	// allocated bytes in idx
    char_u	    *pal;	// dynamic palette storage (RGB triples)
    పూర్ణాంకం		     pal_len;	// allocated bytes in pal
    sixel_band_T     band;	// reusable band scratch
} sixel_state_T;

/*
 * A fully quantized (8bpp) image: one palette index per pixel, tightly
 * packed (stride == width), plus the palette that goes with it.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u  *idx; // Is width*height bytes, indices 1..npal (0 = transparent)
    పూర్ణాంకం      వెడల్పు;
    పూర్ణాంకం      ఎత్తు;
    char_u  *pal; // Is npal*3 bytes, RGB triples
    పూర్ణాంకం      npal;
} sixel_image8_T;

/*
 * A view into a sixel_image8_T: an offset + stride,
 */
రకనిర్వచనం నిర్మాణం
{
    char_u  *idx; // Points at (x0,y0) inside the parent buffer
    పూర్ణాంకం      stride; // Row length of the *parent* image, not this view
    పూర్ణాంకం      వెడల్పు;
    పూర్ణాంకం      ఎత్తు;
    char_u  *pal;
    పూర్ణాంకం      npal;
} sixel_view_T;

రకనిర్వచనం నిర్మాణం
{
    // These are offsets relative to the top left of the image.
    పూర్ణాంకం	    అడ్డువరుస_off;
    పూర్ణాంకం	    నిలువు_off;
    char_u  *seq[2]; // Header + body
} sixel_chunk_T;

రకనిర్వచనం నిర్మాణం
{
    sixel_image8_T image8;
} image_sixel_T;

రకనిర్వచనం నిర్మాణం
{
    pixman_region32_t	visible_region; // Region used for the previous redraw
    bool		visible_init;
    sixel_chunk_T	*chunks;    // Cached sixel sequence (stored as multiple
				    // chunks).
    పూర్ణాంకం			n_chunks;
} image_placement_sixel_T;

స్థిర char_u		sixel_fixed_palette[240 * 3];
స్థిర char_u		sixel_rgb_cube_idx[256];
స్థిర bool		sixel_fixed_tables_ready = false;
స్థిర sixel_state_T	sixel_state;
స్థిర bool		పంపిన_decsdm = false;

/*
 * Release all module-level allocations cached by the sixel encoder.
 */
    శూన్య
sixel_uninit(శూన్య)
{
    sixel_band_T *band = &sixel_state.band;

    VIM_CLEAR(sixel_state.కీలకాలు);
    VIM_CLEAR(sixel_state.vals);
    VIM_CLEAR(sixel_state.idx);
    VIM_CLEAR(sixel_state.pal);
    sixel_state.hash_cap = 0;
    sixel_state.idx_len = 0;
    sixel_state.pal_len = 0;

    VIM_CLEAR(band->బిట్లు);
    VIM_CLEAR(band->చూసిన);
    VIM_CLEAR(band->వాడిన);
    VIM_CLEAR(band->xmin);
    VIM_CLEAR(band->xmax);
    band->బిట్లు_len = 0;
    band->color_len = 0;
    band->చూసిన_gen = 0;
}

/*
 * Initialize the shared fixed sixel palette and RGB->cube lookup table.
 */
    స్థిర శూన్య
sixel_init_fixed_tables(శూన్య)
{
    పూర్ణాంకం	    r, g, b, gr, n;

    ఒకవేళ (sixel_fixed_tables_ready)
	తిరిగి;

    n = 0;
    కొరకు (r = 0; r < 6; r++)
	కొరకు (g = 0; g < 6; g++)
	    కొరకు (b = 0; b < 6; b++)
	    {
		sixel_fixed_palette[n * 3]     = (char_u)(r * 51);
		sixel_fixed_palette[n * 3 + 1] = (char_u)(g * 51);
		sixel_fixed_palette[n * 3 + 2] = (char_u)(b * 51);
		n++;
	    }
    కొరకు (gr = 0; gr < 24; gr++)
    {
	పూర్ణాంకం v = 8 + gr * 10;

	sixel_fixed_palette[n * 3]     = (char_u)v;
	sixel_fixed_palette[n * 3 + 1] = (char_u)v;
	sixel_fixed_palette[n * 3 + 2] = (char_u)v;
	n++;
    }
    కొరకు (n = 0; n < 256; n++)
    {
	పూర్ణాంకం idx = n / 43;

	sixel_rgb_cube_idx[n] = (char_u)(idx > 5 ? 5 : idx);
    }
    sixel_fixed_tables_ready = true;
}

    స్థిర పూర్ణాంకం
sixel_ensure_hash_సామర్థ్యం(పూర్ణాంకం max_colors, పూర్ణాంకం *cap_బహిర్గతం)
{
    పూర్ణాంకం cap = 1024;

    అయితే (cap < max_colors * 4)
	cap <<= 1;
    ఒకవేళ (cap > sixel_state.hash_cap)
    {
	గుర్తులేని పూర్ణాంకం	*కీలకాలు;
	char_u		*vals;

	కీలకాలు = ALLOC_MULT(గుర్తులేని పూర్ణాంకం, cap);
	vals = ALLOC_MULT(char_u, cap);
	ఒకవేళ (కీలకాలు == ఖాళీ || vals == ఖాళీ)
	{
	    vim_free(కీలకాలు);
	    vim_free(vals);
	    తిరిగి FAIL;
	}
	ఒకవేళ (sixel_state.hash_cap > 0)
	{
	    mch_memmove(కీలకాలు, sixel_state.కీలకాలు,
		    (size_t)sixel_state.hash_cap * పరిమాణం(*కీలకాలు));
	    mch_memmove(vals, sixel_state.vals,
		    (size_t)sixel_state.hash_cap * పరిమాణం(*vals));
	}
	vim_free(sixel_state.కీలకాలు);
	vim_free(sixel_state.vals);
	sixel_state.కీలకాలు = కీలకాలు;
	sixel_state.vals = vals;
	sixel_state.hash_cap = cap;
    }
    vim_memset(sixel_state.కీలకాలు, 0, (size_t)cap * పరిమాణం(*sixel_state.కీలకాలు));
    *cap_బహిర్గతం = cap;
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
sixel_ensure_idx_సామర్థ్యం(size_t pixels)
{
    ఒకవేళ (pixels > sixel_state.idx_len)
    {
	char_u *idx = vim_realloc(sixel_state.idx, pixels);

	ఒకవేళ (idx == ఖాళీ)
	    తిరిగి FAIL;
	sixel_state.idx = idx;
	sixel_state.idx_len = pixels;
    }
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
sixel_ensure_pal_సామర్థ్యం(పూర్ణాంకం max_colors)
{
    పూర్ణాంకం need = max_colors * 3;

    ఒకవేళ (need > sixel_state.pal_len)
    {
	char_u *pal = vim_realloc(sixel_state.pal, (size_t)need);

	ఒకవేళ (pal == ఖాళీ)
	    తిరిగి FAIL;
	sixel_state.pal = pal;
	sixel_state.pal_len = need;
    }
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
sixel_ensure_band_సామర్థ్యం(పూర్ణాంకం వెడల్పు, పూర్ణాంకం npal)
{
    size_t	    బిట్లు_need = (size_t)వెడల్పు * (npal + 1);
    sixel_band_T    *band = &sixel_state.band;

    ఒకవేళ (బిట్లు_need > band->బిట్లు_len)
    {
	char_u *బిట్లు = vim_realloc(band->బిట్లు, బిట్లు_need);

	ఒకవేళ (బిట్లు == ఖాళీ)
	    తిరిగి FAIL;
	band->బిట్లు = బిట్లు;
	band->బిట్లు_len = బిట్లు_need;
    }
    ఒకవేళ (npal + 1 > band->color_len)
    {
	పూర్ణాంకం *చూసిన = ALLOC_MULT(పూర్ణాంకం, npal + 1);
	పూర్ణాంకం *వాడిన = ALLOC_MULT(పూర్ణాంకం, npal + 1);
	పూర్ణాంకం *xmin = ALLOC_MULT(పూర్ణాంకం, npal + 1);
	పూర్ణాంకం *xmax = ALLOC_MULT(పూర్ణాంకం, npal + 1);

	ఒకవేళ (చూసిన == ఖాళీ || వాడిన == ఖాళీ || xmin == ఖాళీ || xmax == ఖాళీ)
	{
	    vim_free(చూసిన);
	    vim_free(వాడిన);
	    vim_free(xmin);
	    vim_free(xmax);
	    తిరిగి FAIL;
	}
	ఒకవేళ (band->color_len > 0)
	{
	    mch_memmove(చూసిన, band->చూసిన,
		    (size_t)band->color_len * పరిమాణం(*చూసిన));
	    mch_memmove(వాడిన, band->వాడిన,
		    (size_t)band->color_len * పరిమాణం(*వాడిన));
	    mch_memmove(xmin, band->xmin,
		    (size_t)band->color_len * పరిమాణం(*xmin));
	    mch_memmove(xmax, band->xmax,
		    (size_t)band->color_len * పరిమాణం(*xmax));
	}

	// seen[] is checked against the band's generation counter; if the
	// newly grown tail held garbage equal to the current generation, the
	// band loop would read uninitialised xmin/xmax.  Zero the new tail
	// (and the matching slots in used/xmin/xmax for good measure) so the
	// "first time this colour is touched" branch is always taken.
	{
	    size_t తోక = (size_t)(npal + 1 - band->color_len);

	    vim_memset(చూసిన + band->color_len, 0, తోక * పరిమాణం(*చూసిన));
	    vim_memset(వాడిన + band->color_len, 0, తోక * పరిమాణం(*వాడిన));
	    vim_memset(xmin + band->color_len, 0, తోక * పరిమాణం(*xmin));
	    vim_memset(xmax + band->color_len, 0, తోక * పరిమాణం(*xmax));
	}
	vim_free(band->చూసిన);
	vim_free(band->వాడిన);
	vim_free(band->xmin);
	vim_free(band->xmax);
	band->చూసిన = చూసిన;
	band->వాడిన = వాడిన;
	band->xmin = xmin;
	band->xmax = xmax;
	band->color_len = npal + 1;
    }
    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
rgb_గమ్యం_paletted_fast(
	char_u	*pixels,
	పూర్ణాంకం	 వెడల్పు,
	పూర్ణాంకం	 ఎత్తు,
	bool	 has_alpha,
	పూర్ణాంకం	 max_colors,
	char_u **pal_బహిర్గతం,
	పూర్ణాంకం	*npal_బహిర్గతం,
	char_u **idx_బహిర్గతం)
{
    పూర్ణాంకం		     cap;
    పూర్ణాంకం		     ముసుగు;
    పూర్ణాంకం		     వాడిన = 0;
    గుర్తులేని పూర్ణాంకం    *కీలకాలు;
    char_u	    *vals;
    char_u	    *idx;
    char_u	    *pal;
    పూర్ణాంకం		     i, n;
    పూర్ణాంకం		     bpp = has_alpha ? 4 : 3;

    ఒకవేళ (sixel_ensure_hash_సామర్థ్యం(max_colors, &cap) == FAIL
	    || sixel_ensure_idx_సామర్థ్యం((size_t)వెడల్పు * ఎత్తు) == FAIL
	    || sixel_ensure_pal_సామర్థ్యం(max_colors) == FAIL)
	తిరిగి FAIL;
    కీలకాలు = sixel_state.కీలకాలు;
    vals = sixel_state.vals;
    idx = sixel_state.idx;
    pal = sixel_state.pal;
    ముసుగు = cap - 1;

    n = వెడల్పు * ఎత్తు;
    కొరకు (i = 0; i < n; i++)
    {
	char_u		*p = pixels + (size_t)i * bpp;
	గుర్తులేని పూర్ణాంకం	 కీలకం;
	గుర్తులేని పూర్ణాంకం	 h;
	పూర్ణాంకం		 స్థానం;

	ఒకవేళ (has_alpha && p[3] < 128)
	{
	    idx[i] = 0;	    // transparent -- terminal leaves cell as-is
	    కొనసాగు;
	}
	కీలకం = ((గుర్తులేని పూర్ణాంకం)p[0] << 16)
	    | ((గుర్తులేని పూర్ణాంకం)p[1] << 8)
	    | (గుర్తులేని పూర్ణాంకం)p[2];
	h = కీలకం * 2654435761u;
	స్థానం = (పూర్ణాంకం)(h & ముసుగు);

	కొరకు (;;)
	{
	    ఒకవేళ (కీలకాలు[స్థానం] == 0)
	    {
		// new color
		ఒకవేళ (వాడిన >= max_colors)
		    వెళ్ళు too_many;
		కీలకాలు[స్థానం] = కీలకం + 1;
		vals[స్థానం] = (char_u)(వాడిన + 1);    // 1-based palette index
		pal[వాడిన * 3]	  = p[0];
		pal[వాడిన * 3 + 1] = p[1];
		pal[వాడిన * 3 + 2] = p[2];
		వాడిన++;
		idx[i] = vals[స్థానం];
		ఆపు;
	    }
	    ఒకవేళ (కీలకాలు[స్థానం] == కీలకం + 1)
	    {
		idx[i] = vals[స్థానం];
		ఆపు;
	    }
	    స్థానం = (స్థానం + 1) & ముసుగు;
	}
    }
    *pal_బహిర్గతం = pal;
    *npal_బహిర్గతం = వాడిన;
    *idx_బహిర్గతం = idx;
    తిరిగి OK;

too_many:
    తిరిగి FAIL;
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
    స్థిర పూర్ణాంకం
rgb_గమ్యం_paletted_fixed(
	char_u	*pixels,
	పూర్ణాంకం	వెడల్పు,
	పూర్ణాంకం	ఎత్తు,
	bool	has_alpha,
	char_u	**pal_బహిర్గతం,
	పూర్ణాంకం	*npal_బహిర్గతం,
	char_u	**idx_బహిర్గతం)
{
    char_u  *idx;
    పూర్ణాంకం	     n;
    పూర్ణాంకం	     bpp = has_alpha ? 4 : 3;

    ఒకవేళ (sixel_ensure_idx_సామర్థ్యం((size_t)వెడల్పు * ఎత్తు) == FAIL)
	తిరిగి FAIL;
    idx = sixel_state.idx;

    sixel_init_fixed_tables();

    // map every pixel to the nearest cube cell
    కొరకు (n = 0; n < వెడల్పు * ఎత్తు; n++)
    {
	char_u	*p = pixels + (size_t)n * bpp;
	పూర్ణాంకం	 ri, gi, bi;

	ఒకవేళ (has_alpha && p[3] < 128)
	{
	    idx[n] = 0;
	    కొనసాగు;
	}
	ri = sixel_rgb_cube_idx[p[0]];
	gi = sixel_rgb_cube_idx[p[1]];
	bi = sixel_rgb_cube_idx[p[2]];
	idx[n] = (char_u)(ri * 36 + gi * 6 + bi + 1);	// 1-based
    }

    *pal_బహిర్గతం = sixel_fixed_palette;
    *npal_బహిర్గతం = 240;
    *idx_బహిర్గతం = idx;
    తిరిగి OK;
}

    స్థిర శూన్య
sixel_image8_clear(sixel_image8_T *simg)
{
    vim_free(simg->idx);
    vim_free(simg->pal);
}

/*
 * Quantize an RGB(A) image to an 8bpp paletted image. Returns OK on success,
 * FAIL on invalid input or OOM.
 */
    స్థిర పూర్ణాంకం
sixel_image_quantize(image_T *img, sixel_image8_T *బహిర్గతం)
{
    char_u  *pal = ఖాళీ;
    char_u  *idx = ఖాళీ;
    పూర్ణాంకం	     npal = 0;

    ఒకవేళ (rgb_గమ్యం_paletted_fast(img->దత్తాంశం, img->వెడల్పు, img->ఎత్తు,
		img->fmt == IMAGE_FORMAT_RGBA,
		SIXEL_MAX_COLORS, &pal, &npal, &idx) == FAIL)
    {
	ఒకవేళ (rgb_గమ్యం_paletted_fixed(img->దత్తాంశం, img->వెడల్పు, img->ఎత్తు,
		    img->fmt == IMAGE_FORMAT_RGBA,
		    &pal, &npal, &idx) == FAIL)
	    తిరిగి FAIL;
    }

    బహిర్గతం->idx = vim_memsave(idx, (size_t)img->వెడల్పు * img->ఎత్తు);
    బహిర్గతం->వెడల్పు = img->వెడల్పు;
    బహిర్గతం->ఎత్తు = img->ఎత్తు;
    బహిర్గతం->pal = vim_memsave(pal, (size_t)npal * 3);
    బహిర్గతం->npal = npal;

    ఒకవేళ (బహిర్గతం->pal == ఖాళీ || బహిర్గతం->idx == ఖాళీ)
    {
	sixel_image8_clear(బహిర్గతం);
	తిరిగి FAIL;
    }
    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
sixel_image8_crop(
	sixel_image8_T	*src,
	పూర్ణాంకం		 x,
	పూర్ణాంకం		 y,
	పూర్ణాంకం		 w,
	పూర్ణాంకం		 h,
	sixel_view_T	*బహిర్గతం)
{
    పూర్ణాంకం x0, y0, x1, y1;   // clamped rectangle, half-open [x0,x1) x [y0,y1)

    ఒకవేళ (src == ఖాళీ || బహిర్గతం == ఖాళీ)
	తిరిగి FAIL;

    x0 = x < 0 ? 0 : x;
    y0 = y < 0 ? 0 : y;
    x1 = x + w;
    y1 = y + h;
    ఒకవేళ (x1 > src->వెడల్పు)
	x1 = src->వెడల్పు;
    ఒకవేళ (y1 > src->ఎత్తు)
	y1 = src->ఎత్తు;
    ఒకవేళ (x1 < x0)
	x1 = x0;
    ఒకవేళ (y1 < y0)
	y1 = y0;

    ఒకవేళ (x1 <= x0 || y1 <= y0)
	తిరిగి FAIL;

    బహిర్గతం->stride = src->వెడల్పు;
    బహిర్గతం->idx    = (x1 > x0 && y1 > y0)
	? src->idx + (size_t)y0 * src->వెడల్పు + x0
	: ఖాళీ;
    బహిర్గతం->వెడల్పు  = x1 - x0;
    బహిర్గతం->ఎత్తు = y1 - y0;
    బహిర్గతం->pal    = src->pal;
    బహిర్గతం->npal   = src->npal;

    తిరిగి OK;
}

/*
 * Emit a run of `cnt` identical sixel data bytes (ch in 0..63) into gap.
 * Uses RLE form `!N{c}` when it shortens the output.
 */
    స్థిర పూర్ణాంకం
emit_నిరంతరం(garray_T *gap, char_u ch, పూర్ణాంకం cnt)
{
    char_u  c = (char_u)(63 + ch);

    అయితే (cnt > 255)
    {
	ఒకవేళ (ga_concat_బైట్లు(gap, "!255", 4) == FAIL
		|| ga_append(gap, c) == FAIL)
	    తిరిగి FAIL;
	cnt -= 255;
    }
    ఒకవేళ (cnt <= 0)
	తిరిగి OK;
    ఒకవేళ (cnt <= 3)
    {
	అయితే (cnt-- > 0)
	    ఒకవేళ (ga_append(gap, c) == FAIL)
		తిరిగి FAIL;
	తిరిగి OK;
    }
    ఒకవేళ (ga_append(gap, '!') == FAIL
	    || ga_concat_int(gap, cnt) == FAIL
	    || ga_append(gap, c) == FAIL)
	తిరిగి FAIL;
    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
sixel_encode_view(sixel_view_T *view, char_u *seq[2])
{
    garray_T	    hdr;    // DCS introducer, raster attrs, used-color defs
    garray_T	    body;   // band data (colour selects + sixel bytes)
    sixel_band_T    *band_state = &sixel_state.band;
    char_u	    *idx;
    char_u	    *pal;
    పూర్ణాంకం		    npal;
    పూర్ణాంకం		    వెడల్పు, ఎత్తు;
    పూర్ణాంకం		    band, p, x, n;
    bool	    defined[256];   // register already defined in header

    idx = view->idx;
    pal = view->pal;
    npal = view->npal;
    వెడల్పు = view->వెడల్పు;
    ఎత్తు = view->ఎత్తు;

    ga_init2(&hdr, పరిమాణం(char_u), 256);
    ga_init2(&body, పరిమాణం(char_u), 4096);
    vim_memset(defined, 0, పరిమాణం(defined));

    // DECSIXEL Introducer + Raster Attributes "1;1;W;H.
    // P2=1 means pixel positions left unspecified by any colour register
    // keep their previous on-screen contents instead of being painted with
    // colour register 0.  That gives true transparency for RGBA images:
    // alpha < 128 pixels are emitted as palette index 0 (which we never
    // write to the bitmask), so the terminal leaves the popup's underlying
    // cell colour visible there -- no flatten, no colour-match drift.
    ఒకవేళ (ga_concat_బైట్లు(&hdr, "\033P0;1;8q\"1;1;", 13) == FAIL
	    || ga_concat_int(&hdr, వెడల్పు) == FAIL
	    || ga_append(&hdr, ';') == FAIL
	    || ga_concat_int(&hdr, ఎత్తు) == FAIL)
	వెళ్ళు fail;

    // bitmask buffer: (crop) width bytes per palette index, +1 for the
    // unused index 0.  Sized off the view's width, not the parent image's
    // width, so encoding a small crop out of a large quantized image only
    // needs scratch space proportional to the crop.
    ఒకవేళ (sixel_ensure_band_సామర్థ్యం(వెడల్పు, npal) == FAIL)
	వెళ్ళు fail;

    కొరకు (band = 0; band < (ఎత్తు + 5) / 6; band++)
    {
	పూర్ణాంకం gen;
	పూర్ణాంకం చివరి_was_cr = 0;
	పూర్ణాంకం వాడిన_లెక్క = 0;

	ఒకవేళ (++band_state->చూసిన_gen <= 0)
	{
	    vim_memset(band_state->చూసిన, 0, (npal + 1) * పరిమాణం(పూర్ణాంకం));
	    band_state->చూసిన_gen = 1;
	}
	gen = band_state->చూసిన_gen;

	ఒకవేళ (band > 0 && ga_append(&body, '-') == FAIL)
	    వెళ్ళు fail;

	// Fill the bitmask for this band.
	కొరకు (p = 0; p < 6; p++)
	{
	    పూర్ణాంకం y = band * 6 + p;
	    char_u  rowmask = (char_u)(1 << p);
	    char_u  *అడ్డువరుస;

	    ఒకవేళ (y >= ఎత్తు)
		కొనసాగు;
	    // Use the *parent* stride to find the row -- "idx" already
	    // points at the crop's top-left corner, so only the row-to-row
	    // step needs to know about the parent's real width.
	    అడ్డువరుస = idx + (size_t)y * view->stride;
	    కొరకు (x = 0; x < వెడల్పు; x++)
	    {
		char_u  pix = అడ్డువరుస[x];

		ఒకవేళ (pix == 0)
		    కొనసాగు;
		ఒకవేళ (band_state->చూసిన[pix] != gen)
		{
		    band_state->చూసిన[pix] = gen;
		    band_state->వాడిన[వాడిన_లెక్క++] = pix;
		    band_state->xmin[pix] = x;
		    band_state->xmax[pix] = x;
		    vim_memset(band_state->బిట్లు + (size_t)pix * వెడల్పు, 0, వెడల్పు);
		}
		లేకపోతే
		{
		    ఒకవేళ (x < band_state->xmin[pix])
			band_state->xmin[pix] = x;
		    ఒకవేళ (x > band_state->xmax[pix])
			band_state->xmax[pix] = x;
		}
		band_state->బిట్లు[(size_t)pix * వెడల్పు + x] |= rowmask;
	    }
	}

	కొరకు (n = 0; n < వాడిన_లెక్క; n++)
	{
	    పూర్ణాంకం	     pix = band_state->వాడిన[n];
	    char_u  *అడ్డువరుస;
	    char_u   ch0;
	    పూర్ణాంకం	     cnt;
	    పూర్ణాంకం	     ప్రారంభం = band_state->xmin[pix];
	    పూర్ణాంకం	     చివర = band_state->xmax[pix];

	    // First time this colour is used in the subrect: add its
	    // definition "#N;2;R;G;B" (RGB scaled to 0..100) to the header.
	    // Colours the subrect never touches are never defined.  Round to
	    // nearest, not truncate, so the round-trip 8bit -> 0..100 -> 8bit
	    // stays within ~1 level instead of drifting up to 2-3 levels
	    // darker.  This matters when the popup blends RGBA alpha onto the
	    // terminal background: truncation made the flattened bg visibly
	    // darker than the surrounding terminal cells.
	    ఒకవేళ (!defined[pix])
	    {
		పూర్ణాంకం r = (pal[(pix - 1) * 3]     * 100 + 127) / 255;
		పూర్ణాంకం g = (pal[(pix - 1) * 3 + 1] * 100 + 127) / 255;
		పూర్ణాంకం b = (pal[(pix - 1) * 3 + 2] * 100 + 127) / 255;

		defined[pix] = true;
		ఒకవేళ (ga_append(&hdr, '#') == FAIL
			|| ga_concat_int(&hdr, pix) == FAIL
			|| ga_concat_బైట్లు(&hdr, ";2;", 3) == FAIL
			|| ga_concat_int(&hdr, r) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, g) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, b) == FAIL)
		    వెళ్ళు fail;
	    }

	    ఒకవేళ (చివరి_was_cr && ga_append(&body, '$') == FAIL)
		వెళ్ళు fail;
	    ఒకవేళ (ga_append(&body, '#') == FAIL
		    || ga_concat_int(&body, pix) == FAIL)
		వెళ్ళు fail;

	    అడ్డువరుస = band_state->బిట్లు + (size_t)pix * వెడల్పు;
	    ఒకవేళ (ప్రారంభం > 0 && emit_నిరంతరం(&body, 0, ప్రారంభం) == FAIL)
		వెళ్ళు fail;
	    ch0 = అడ్డువరుస[ప్రారంభం];
	    cnt = 1;
	    కొరకు (x = ప్రారంభం + 1; x <= చివర; x++)
	    {
		char_u  ch = అడ్డువరుస[x];

		ఒకవేళ (ch == ch0)
		{
		    cnt++;
		    కొనసాగు;
		}
		ఒకవేళ (emit_నిరంతరం(&body, ch0, cnt) == FAIL)
		    వెళ్ళు fail;
		ch0 = ch;
		cnt = 1;
	    }
	    ఒకవేళ (emit_నిరంతరం(&body, ch0, cnt) == FAIL)
		వెళ్ళు fail;
	    చివరి_was_cr = 1;
	}
    }

    ఒకవేళ (ga_append(&hdr, NUL) == FAIL
	    || ga_concat_బైట్లు(&body, "\033\\", 3) == FAIL)
	వెళ్ళు fail;

    seq[0] = (char_u *)hdr.ga_దత్తాంశం;
    seq[1] = (char_u *)body.ga_దత్తాంశం;

    తిరిగి OK;

fail:
    ga_clear(&hdr);
    ga_clear(&body);
    తిరిగి FAIL;
}

    పూర్ణాంకం
image_sixel_init(image_T *img)
{
    image_sixel_T *ctx;

    ctx = ALLOC_CLEAR_ONE(image_sixel_T);
    ఒకవేళ (ctx == ఖాళీ || sixel_image_quantize(img, &ctx->image8) == FAIL)
    {
	vim_free(ctx);
	తిరిగి FAIL;
    }

#ఒకవేళ defined(FEAT_IMAGE_SIXEL) && defined(FEAT_TERMRESPONSE)
    ఒకవేళ (!పంపిన_decsdm)
    {
	// Disable sixel scrolling using DECSDM
	బహిర్గతం_str((char_u *)"\033[?80l");
	పంపిన_decsdm = true;
    }
#అయితే_ముగింపు

    img->backend_దత్తాంశం = ctx;
    తిరిగి OK;
}

    శూన్య
image_sixel_uninit(image_T *img)
{
    image_sixel_T *ctx = img->backend_దత్తాంశం;

    sixel_image8_clear(&ctx->image8);
    vim_free(img->backend_దత్తాంశం);
}

    పూర్ణాంకం
image_placement_sixel_init(image_placement_T *ఉంచు)
{
    image_placement_sixel_T *ctx = ALLOC_CLEAR_ONE(image_placement_sixel_T);

    ఒకవేళ (ctx == ఖాళీ)
	తిరిగి FAIL;
    ఉంచు->backend_దత్తాంశం = ctx;
    తిరిగి OK;
}

    స్థిర శూన్య
clear_chunks(image_placement_sixel_T *ctx)
{
    ఒకవేళ (ctx->chunks == ఖాళీ)
	తిరిగి;

    కొరకు (పూర్ణాంకం i = 0; i < ctx->n_chunks; i++)
    {
	vim_free(ctx->chunks[i].seq[0]);
	vim_free(ctx->chunks[i].seq[1]);
    }
    VIM_CLEAR(ctx->chunks);
    ctx->n_chunks = 0;
}

    శూన్య
image_placement_sixel_uninit(image_placement_T *ఉంచు)
{
    image_placement_sixel_T *ctx = ఉంచు->backend_దత్తాంశం;

    ఒకవేళ (ctx->visible_init)
	pixman_region32_fini(&ctx->visible_region);
    clear_chunks(ctx);
    vim_free(ctx);
}

    శూన్య
image_placement_sixel_draw(image_placement_T *ఉంచు)
{
    image_T		    *img = ఉంచు->img;
    image_sixel_T	    *ictx = img->backend_దత్తాంశం;
    image_placement_sixel_T *ctx = ఉంచు->backend_దత్తాంశం;
    sixel_view_T	    view;
    pixman_box32_t	    *rects;
    పూర్ణాంకం			    n_rects;

    // Check if visible region is still the same, if so then use the cached
    // sixel sequences.
    ఒకవేళ (!(ఉంచు->flags & IMAGEPF_FORCE) && ctx->visible_init
	    && pixman_region32_equal(&ctx->visible_region, &ఉంచు->visible))
    {
	కొరకు (పూర్ణాంకం i = 0; i < pixman_region32_n_rects(&ctx->visible_region); i++)
	{
	    sixel_chunk_T *chunk = ctx->chunks + i;

	    windgoto(ఉంచు->అడ్డువరుస + ఉంచు->అడ్డువరుస_off
		    + chunk->అడ్డువరుస_off, ఉంచు->నిలువు + chunk->నిలువు_off);
	    ఒకవేళ (chunk->seq[0] != ఖాళీ)
		బహిర్గతం_str(chunk->seq[0]);
	    ఒకవేళ (chunk->seq[1] != ఖాళీ)
		బహిర్గతం_str(chunk->seq[1]);
	    screen_ప్రారంభం();
	}
	తిరిగి;
    }

    rects = pixman_region32_rectangles(&ఉంచు->visible, &n_rects);
    ఒకవేళ (rects == ఖాళీ || n_rects == 0)
	తిరిగి;

    clear_chunks(ctx);
    ctx->chunks = ALLOC_CLEAR_MULT(sixel_chunk_T, n_rects);

    ఒకవేళ (ctx->chunks == ఖాళీ)
	తిరిగి;

    కొరకు (పూర్ణాంకం i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	పూర్ణాంకం		అడ్డువరుస, నిలువు;
	పూర్ణాంకం		x, y, w, h;
	sixel_chunk_T	*chunk = ctx->chunks + i;
	char_u		*seq[2];

	image_placement_subrect(ఉంచు, rect, &అడ్డువరుస, &నిలువు, &x, &y, &w, &h, false);

	ఒకవేళ (sixel_image8_crop(&ictx->image8, x, y, w, h, &view) == FAIL
		|| sixel_encode_view(&view, seq) == FAIL)
	    కొనసాగు;

	windgoto(అడ్డువరుస, నిలువు);
	ఒకవేళ (seq[0] != ఖాళీ)
	    బహిర్గతం_str(seq[0]);
	ఒకవేళ (seq[1] != ఖాళీ)
	    బహిర్గతం_str(seq[1]);
	screen_ప్రారంభం();

	chunk->అడ్డువరుస_off = rect.y1;
	chunk->నిలువు_off = rect.x1;
	chunk->seq[0] = seq[0];
	chunk->seq[1] = seq[1];
	ctx->n_chunks++;
    }

    ఒకవేళ (ctx->visible_init)
	pixman_region32_clear(&ctx->visible_region);
    pixman_region32_copy(&ctx->visible_region, &ఉంచు->visible);
    ctx->visible_init = true;
}

    శూన్య
image_placement_sixel_clear(image_placement_T *ఉంచు వాడని)
{
}

#అయితే_ముగింపు // FEAT_IMAGE_SIXEL
