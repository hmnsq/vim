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

#شامل "vim.h"

#اگر defined(FEAT_IMAGE_SIXEL)

// Palette size cap (sixel allows up to 256 color registers; index 0 is
// reserved as a transparent key, so usable colors are 1..MAX_COLORS).
#تعریف_کرو SIXEL_MAX_COLORS    255

/*
 * Working buffer for one band (6 pixel rows). Allocated as
 * width * (palette_size+1) bytes; bit p (0..5) marks pixel at row offset p.
 */
قسمتعریف ساخت
{
    char_u  *بٹ;	// bitmask buffer, width * paletteSize bytes
    size_t   بٹ_len;	// allocated bytes in "bits"
    صحیح	    *نظر_آیا;	// per-color "used in this band" flags
    صحیح	    *مستعمل;	// colors used in this band
    صحیح	    *xmin;	// leftmost x used by color in this band
    صحیح	    *xmax;	// rightmost x used by color in this band
    صحیح	     color_len;	// allocated length for per-color arrays
    صحیح	     نظر_آیا_gen;	// generation counter to avoid memset
} sixel_band_T;

قسمتعریف ساخت
{
    غیرعلامتی صحیح    *کلیدیں;	// hash keys for dynamic palette lookup
    char_u	    *vals;	// hash values for dynamic palette lookup
    صحیح		     hash_cap;	// allocated slots in keys/vals
    char_u	    *idx;	// width * height paletted image
    size_t	     idx_len;	// allocated bytes in idx
    char_u	    *pal;	// dynamic palette storage (RGB triples)
    صحیح		     pal_len;	// allocated bytes in pal
    sixel_band_T     band;	// reusable band scratch
} sixel_state_T;

/*
 * A fully quantized (8bpp) image: one palette index per pixel, tightly
 * packed (stride == width), plus the palette that goes with it.
 */
قسمتعریف ساخت
{
    char_u  *idx; // Is width*height bytes, indices 1..npal (0 = transparent)
    صحیح      چوڑائی;
    صحیح      اونچائی;
    char_u  *pal; // Is npal*3 bytes, RGB triples
    صحیح      npal;
} sixel_image8_T;

/*
 * A view into a sixel_image8_T: an offset + stride,
 */
قسمتعریف ساخت
{
    char_u  *idx; // Points at (x0,y0) inside the parent buffer
    صحیح      stride; // Row length of the *parent* image, not this view
    صحیح      چوڑائی;
    صحیح      اونچائی;
    char_u  *pal;
    صحیح      npal;
} sixel_view_T;

قسمتعریف ساخت
{
    // These are offsets relative to the top left of the image.
    صحیح	    صف_off;
    صحیح	    ستون_off;
    char_u  *seq[2]; // Header + body
} sixel_chunk_T;

قسمتعریف ساخت
{
    sixel_image8_T image8;
} image_sixel_T;

قسمتعریف ساخت
{
    pixman_region32_t	visible_region; // Region used for the previous redraw
    bool		visible_init;
    sixel_chunk_T	*chunks;    // Cached sixel sequence (stored as multiple
				    // chunks).
    صحیح			n_chunks;
} image_placement_sixel_T;

ساکن char_u		sixel_fixed_palette[240 * 3];
ساکن char_u		sixel_rgb_cube_idx[256];
ساکن bool		sixel_fixed_tables_ready = false;
ساکن sixel_state_T	sixel_state;
ساکن bool		بھیجا_decsdm = false;

/*
 * Release all module-level allocations cached by the sixel encoder.
 */
    خالی
sixel_uninit(خالی)
{
    sixel_band_T *band = &sixel_state.band;

    VIM_CLEAR(sixel_state.کلیدیں);
    VIM_CLEAR(sixel_state.vals);
    VIM_CLEAR(sixel_state.idx);
    VIM_CLEAR(sixel_state.pal);
    sixel_state.hash_cap = 0;
    sixel_state.idx_len = 0;
    sixel_state.pal_len = 0;

    VIM_CLEAR(band->بٹ);
    VIM_CLEAR(band->نظر_آیا);
    VIM_CLEAR(band->مستعمل);
    VIM_CLEAR(band->xmin);
    VIM_CLEAR(band->xmax);
    band->بٹ_len = 0;
    band->color_len = 0;
    band->نظر_آیا_gen = 0;
}

/*
 * Initialize the shared fixed sixel palette and RGB->cube lookup table.
 */
    ساکن خالی
sixel_init_fixed_tables(خالی)
{
    صحیح	    r, g, b, gr, n;

    اگر (sixel_fixed_tables_ready)
	واپس;

    n = 0;
    برائے (r = 0; r < 6; r++)
	برائے (g = 0; g < 6; g++)
	    برائے (b = 0; b < 6; b++)
	    {
		sixel_fixed_palette[n * 3]     = (char_u)(r * 51);
		sixel_fixed_palette[n * 3 + 1] = (char_u)(g * 51);
		sixel_fixed_palette[n * 3 + 2] = (char_u)(b * 51);
		n++;
	    }
    برائے (gr = 0; gr < 24; gr++)
    {
	صحیح v = 8 + gr * 10;

	sixel_fixed_palette[n * 3]     = (char_u)v;
	sixel_fixed_palette[n * 3 + 1] = (char_u)v;
	sixel_fixed_palette[n * 3 + 2] = (char_u)v;
	n++;
    }
    برائے (n = 0; n < 256; n++)
    {
	صحیح idx = n / 43;

	sixel_rgb_cube_idx[n] = (char_u)(idx > 5 ? 5 : idx);
    }
    sixel_fixed_tables_ready = true;
}

    ساکن صحیح
sixel_ensure_hash_استعداد(صحیح max_colors, صحیح *cap_برآمد)
{
    صحیح cap = 1024;

    جب (cap < max_colors * 4)
	cap <<= 1;
    اگر (cap > sixel_state.hash_cap)
    {
	غیرعلامتی صحیح	*کلیدیں;
	char_u		*vals;

	کلیدیں = ALLOC_MULT(غیرعلامتی صحیح, cap);
	vals = ALLOC_MULT(char_u, cap);
	اگر (کلیدیں == تہی || vals == تہی)
	{
	    vim_free(کلیدیں);
	    vim_free(vals);
	    واپس FAIL;
	}
	اگر (sixel_state.hash_cap > 0)
	{
	    mch_memmove(کلیدیں, sixel_state.کلیدیں,
		    (size_t)sixel_state.hash_cap * حجم(*کلیدیں));
	    mch_memmove(vals, sixel_state.vals,
		    (size_t)sixel_state.hash_cap * حجم(*vals));
	}
	vim_free(sixel_state.کلیدیں);
	vim_free(sixel_state.vals);
	sixel_state.کلیدیں = کلیدیں;
	sixel_state.vals = vals;
	sixel_state.hash_cap = cap;
    }
    vim_memset(sixel_state.کلیدیں, 0, (size_t)cap * حجم(*sixel_state.کلیدیں));
    *cap_برآمد = cap;
    واپس OK;
}

    ساکن صحیح
sixel_ensure_idx_استعداد(size_t pixels)
{
    اگر (pixels > sixel_state.idx_len)
    {
	char_u *idx = vim_realloc(sixel_state.idx, pixels);

	اگر (idx == تہی)
	    واپس FAIL;
	sixel_state.idx = idx;
	sixel_state.idx_len = pixels;
    }
    واپس OK;
}

    ساکن صحیح
sixel_ensure_pal_استعداد(صحیح max_colors)
{
    صحیح need = max_colors * 3;

    اگر (need > sixel_state.pal_len)
    {
	char_u *pal = vim_realloc(sixel_state.pal, (size_t)need);

	اگر (pal == تہی)
	    واپس FAIL;
	sixel_state.pal = pal;
	sixel_state.pal_len = need;
    }
    واپس OK;
}

    ساکن صحیح
sixel_ensure_band_استعداد(صحیح چوڑائی, صحیح npal)
{
    size_t	    بٹ_need = (size_t)چوڑائی * (npal + 1);
    sixel_band_T    *band = &sixel_state.band;

    اگر (بٹ_need > band->بٹ_len)
    {
	char_u *بٹ = vim_realloc(band->بٹ, بٹ_need);

	اگر (بٹ == تہی)
	    واپس FAIL;
	band->بٹ = بٹ;
	band->بٹ_len = بٹ_need;
    }
    اگر (npal + 1 > band->color_len)
    {
	صحیح *نظر_آیا = ALLOC_MULT(صحیح, npal + 1);
	صحیح *مستعمل = ALLOC_MULT(صحیح, npal + 1);
	صحیح *xmin = ALLOC_MULT(صحیح, npal + 1);
	صحیح *xmax = ALLOC_MULT(صحیح, npal + 1);

	اگر (نظر_آیا == تہی || مستعمل == تہی || xmin == تہی || xmax == تہی)
	{
	    vim_free(نظر_آیا);
	    vim_free(مستعمل);
	    vim_free(xmin);
	    vim_free(xmax);
	    واپس FAIL;
	}
	اگر (band->color_len > 0)
	{
	    mch_memmove(نظر_آیا, band->نظر_آیا,
		    (size_t)band->color_len * حجم(*نظر_آیا));
	    mch_memmove(مستعمل, band->مستعمل,
		    (size_t)band->color_len * حجم(*مستعمل));
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
	    size_t دم = (size_t)(npal + 1 - band->color_len);

	    vim_memset(نظر_آیا + band->color_len, 0, دم * حجم(*نظر_آیا));
	    vim_memset(مستعمل + band->color_len, 0, دم * حجم(*مستعمل));
	    vim_memset(xmin + band->color_len, 0, دم * حجم(*xmin));
	    vim_memset(xmax + band->color_len, 0, دم * حجم(*xmax));
	}
	vim_free(band->نظر_آیا);
	vim_free(band->مستعمل);
	vim_free(band->xmin);
	vim_free(band->xmax);
	band->نظر_آیا = نظر_آیا;
	band->مستعمل = مستعمل;
	band->xmin = xmin;
	band->xmax = xmax;
	band->color_len = npal + 1;
    }
    واپس OK;
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
    ساکن صحیح
rgb_مقصد_paletted_fast(
	char_u	*pixels,
	صحیح	 چوڑائی,
	صحیح	 اونچائی,
	bool	 has_alpha,
	صحیح	 max_colors,
	char_u **pal_برآمد,
	صحیح	*npal_برآمد,
	char_u **idx_برآمد)
{
    صحیح		     cap;
    صحیح		     نقاب;
    صحیح		     مستعمل = 0;
    غیرعلامتی صحیح    *کلیدیں;
    char_u	    *vals;
    char_u	    *idx;
    char_u	    *pal;
    صحیح		     i, n;
    صحیح		     bpp = has_alpha ? 4 : 3;

    اگر (sixel_ensure_hash_استعداد(max_colors, &cap) == FAIL
	    || sixel_ensure_idx_استعداد((size_t)چوڑائی * اونچائی) == FAIL
	    || sixel_ensure_pal_استعداد(max_colors) == FAIL)
	واپس FAIL;
    کلیدیں = sixel_state.کلیدیں;
    vals = sixel_state.vals;
    idx = sixel_state.idx;
    pal = sixel_state.pal;
    نقاب = cap - 1;

    n = چوڑائی * اونچائی;
    برائے (i = 0; i < n; i++)
    {
	char_u		*p = pixels + (size_t)i * bpp;
	غیرعلامتی صحیح	 کلید;
	غیرعلامتی صحیح	 h;
	صحیح		 خانہ;

	اگر (has_alpha && p[3] < 128)
	{
	    idx[i] = 0;	    // transparent -- terminal leaves cell as-is
	    جاری;
	}
	کلید = ((غیرعلامتی صحیح)p[0] << 16)
	    | ((غیرعلامتی صحیح)p[1] << 8)
	    | (غیرعلامتی صحیح)p[2];
	h = کلید * 2654435761u;
	خانہ = (صحیح)(h & نقاب);

	برائے (;;)
	{
	    اگر (کلیدیں[خانہ] == 0)
	    {
		// new color
		اگر (مستعمل >= max_colors)
		    جاؤ too_many;
		کلیدیں[خانہ] = کلید + 1;
		vals[خانہ] = (char_u)(مستعمل + 1);    // 1-based palette index
		pal[مستعمل * 3]	  = p[0];
		pal[مستعمل * 3 + 1] = p[1];
		pal[مستعمل * 3 + 2] = p[2];
		مستعمل++;
		idx[i] = vals[خانہ];
		توڑو;
	    }
	    اگر (کلیدیں[خانہ] == کلید + 1)
	    {
		idx[i] = vals[خانہ];
		توڑو;
	    }
	    خانہ = (خانہ + 1) & نقاب;
	}
    }
    *pal_برآمد = pal;
    *npal_برآمد = مستعمل;
    *idx_برآمد = idx;
    واپس OK;

too_many:
    واپس FAIL;
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
    ساکن صحیح
rgb_مقصد_paletted_fixed(
	char_u	*pixels,
	صحیح	چوڑائی,
	صحیح	اونچائی,
	bool	has_alpha,
	char_u	**pal_برآمد,
	صحیح	*npal_برآمد,
	char_u	**idx_برآمد)
{
    char_u  *idx;
    صحیح	     n;
    صحیح	     bpp = has_alpha ? 4 : 3;

    اگر (sixel_ensure_idx_استعداد((size_t)چوڑائی * اونچائی) == FAIL)
	واپس FAIL;
    idx = sixel_state.idx;

    sixel_init_fixed_tables();

    // map every pixel to the nearest cube cell
    برائے (n = 0; n < چوڑائی * اونچائی; n++)
    {
	char_u	*p = pixels + (size_t)n * bpp;
	صحیح	 ri, gi, bi;

	اگر (has_alpha && p[3] < 128)
	{
	    idx[n] = 0;
	    جاری;
	}
	ri = sixel_rgb_cube_idx[p[0]];
	gi = sixel_rgb_cube_idx[p[1]];
	bi = sixel_rgb_cube_idx[p[2]];
	idx[n] = (char_u)(ri * 36 + gi * 6 + bi + 1);	// 1-based
    }

    *pal_برآمد = sixel_fixed_palette;
    *npal_برآمد = 240;
    *idx_برآمد = idx;
    واپس OK;
}

    ساکن خالی
sixel_image8_clear(sixel_image8_T *simg)
{
    vim_free(simg->idx);
    vim_free(simg->pal);
}

/*
 * Quantize an RGB(A) image to an 8bpp paletted image. Returns OK on success,
 * FAIL on invalid input or OOM.
 */
    ساکن صحیح
sixel_image_quantize(image_T *img, sixel_image8_T *برآمد)
{
    char_u  *pal = تہی;
    char_u  *idx = تہی;
    صحیح	     npal = 0;

    اگر (rgb_مقصد_paletted_fast(img->ڈیٹا, img->چوڑائی, img->اونچائی,
		img->fmt == IMAGE_FORMAT_RGBA,
		SIXEL_MAX_COLORS, &pal, &npal, &idx) == FAIL)
    {
	اگر (rgb_مقصد_paletted_fixed(img->ڈیٹا, img->چوڑائی, img->اونچائی,
		    img->fmt == IMAGE_FORMAT_RGBA,
		    &pal, &npal, &idx) == FAIL)
	    واپس FAIL;
    }

    برآمد->idx = vim_memsave(idx, (size_t)img->چوڑائی * img->اونچائی);
    برآمد->چوڑائی = img->چوڑائی;
    برآمد->اونچائی = img->اونچائی;
    برآمد->pal = vim_memsave(pal, (size_t)npal * 3);
    برآمد->npal = npal;

    اگر (برآمد->pal == تہی || برآمد->idx == تہی)
    {
	sixel_image8_clear(برآمد);
	واپس FAIL;
    }
    واپس OK;
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
    ساکن صحیح
sixel_image8_crop(
	sixel_image8_T	*src,
	صحیح		 x,
	صحیح		 y,
	صحیح		 w,
	صحیح		 h,
	sixel_view_T	*برآمد)
{
    صحیح x0, y0, x1, y1;   // clamped rectangle, half-open [x0,x1) x [y0,y1)

    اگر (src == تہی || برآمد == تہی)
	واپس FAIL;

    x0 = x < 0 ? 0 : x;
    y0 = y < 0 ? 0 : y;
    x1 = x + w;
    y1 = y + h;
    اگر (x1 > src->چوڑائی)
	x1 = src->چوڑائی;
    اگر (y1 > src->اونچائی)
	y1 = src->اونچائی;
    اگر (x1 < x0)
	x1 = x0;
    اگر (y1 < y0)
	y1 = y0;

    اگر (x1 <= x0 || y1 <= y0)
	واپس FAIL;

    برآمد->stride = src->چوڑائی;
    برآمد->idx    = (x1 > x0 && y1 > y0)
	? src->idx + (size_t)y0 * src->چوڑائی + x0
	: تہی;
    برآمد->چوڑائی  = x1 - x0;
    برآمد->اونچائی = y1 - y0;
    برآمد->pal    = src->pal;
    برآمد->npal   = src->npal;

    واپس OK;
}

/*
 * Emit a run of `cnt` identical sixel data bytes (ch in 0..63) into gap.
 * Uses RLE form `!N{c}` when it shortens the output.
 */
    ساکن صحیح
emit_تسلسل(garray_T *gap, char_u ch, صحیح cnt)
{
    char_u  c = (char_u)(63 + ch);

    جب (cnt > 255)
    {
	اگر (ga_concat_بائٹیں(gap, "!255", 4) == FAIL
		|| ga_append(gap, c) == FAIL)
	    واپس FAIL;
	cnt -= 255;
    }
    اگر (cnt <= 0)
	واپس OK;
    اگر (cnt <= 3)
    {
	جب (cnt-- > 0)
	    اگر (ga_append(gap, c) == FAIL)
		واپس FAIL;
	واپس OK;
    }
    اگر (ga_append(gap, '!') == FAIL
	    || ga_concat_int(gap, cnt) == FAIL
	    || ga_append(gap, c) == FAIL)
	واپس FAIL;
    واپس OK;
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
    ساکن صحیح
sixel_encode_view(sixel_view_T *view, char_u *seq[2])
{
    garray_T	    hdr;    // DCS introducer, raster attrs, used-color defs
    garray_T	    body;   // band data (colour selects + sixel bytes)
    sixel_band_T    *band_state = &sixel_state.band;
    char_u	    *idx;
    char_u	    *pal;
    صحیح		    npal;
    صحیح		    چوڑائی, اونچائی;
    صحیح		    band, p, x, n;
    bool	    defined[256];   // register already defined in header

    idx = view->idx;
    pal = view->pal;
    npal = view->npal;
    چوڑائی = view->چوڑائی;
    اونچائی = view->اونچائی;

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
    اگر (ga_concat_بائٹیں(&hdr, "\033P0;1;8q\"1;1;", 13) == FAIL
	    || ga_concat_int(&hdr, چوڑائی) == FAIL
	    || ga_append(&hdr, ';') == FAIL
	    || ga_concat_int(&hdr, اونچائی) == FAIL)
	جاؤ fail;

    // bitmask buffer: (crop) width bytes per palette index, +1 for the
    // unused index 0.  Sized off the view's width, not the parent image's
    // width, so encoding a small crop out of a large quantized image only
    // needs scratch space proportional to the crop.
    اگر (sixel_ensure_band_استعداد(چوڑائی, npal) == FAIL)
	جاؤ fail;

    برائے (band = 0; band < (اونچائی + 5) / 6; band++)
    {
	صحیح gen;
	صحیح آخری_was_cr = 0;
	صحیح مستعمل_گنتی = 0;

	اگر (++band_state->نظر_آیا_gen <= 0)
	{
	    vim_memset(band_state->نظر_آیا, 0, (npal + 1) * حجم(صحیح));
	    band_state->نظر_آیا_gen = 1;
	}
	gen = band_state->نظر_آیا_gen;

	اگر (band > 0 && ga_append(&body, '-') == FAIL)
	    جاؤ fail;

	// Fill the bitmask for this band.
	برائے (p = 0; p < 6; p++)
	{
	    صحیح y = band * 6 + p;
	    char_u  rowmask = (char_u)(1 << p);
	    char_u  *صف;

	    اگر (y >= اونچائی)
		جاری;
	    // Use the *parent* stride to find the row -- "idx" already
	    // points at the crop's top-left corner, so only the row-to-row
	    // step needs to know about the parent's real width.
	    صف = idx + (size_t)y * view->stride;
	    برائے (x = 0; x < چوڑائی; x++)
	    {
		char_u  pix = صف[x];

		اگر (pix == 0)
		    جاری;
		اگر (band_state->نظر_آیا[pix] != gen)
		{
		    band_state->نظر_آیا[pix] = gen;
		    band_state->مستعمل[مستعمل_گنتی++] = pix;
		    band_state->xmin[pix] = x;
		    band_state->xmax[pix] = x;
		    vim_memset(band_state->بٹ + (size_t)pix * چوڑائی, 0, چوڑائی);
		}
		ورنہ
		{
		    اگر (x < band_state->xmin[pix])
			band_state->xmin[pix] = x;
		    اگر (x > band_state->xmax[pix])
			band_state->xmax[pix] = x;
		}
		band_state->بٹ[(size_t)pix * چوڑائی + x] |= rowmask;
	    }
	}

	برائے (n = 0; n < مستعمل_گنتی; n++)
	{
	    صحیح	     pix = band_state->مستعمل[n];
	    char_u  *صف;
	    char_u   ch0;
	    صحیح	     cnt;
	    صحیح	     آغاز = band_state->xmin[pix];
	    صحیح	     انتہا = band_state->xmax[pix];

	    // First time this colour is used in the subrect: add its
	    // definition "#N;2;R;G;B" (RGB scaled to 0..100) to the header.
	    // Colours the subrect never touches are never defined.  Round to
	    // nearest, not truncate, so the round-trip 8bit -> 0..100 -> 8bit
	    // stays within ~1 level instead of drifting up to 2-3 levels
	    // darker.  This matters when the popup blends RGBA alpha onto the
	    // terminal background: truncation made the flattened bg visibly
	    // darker than the surrounding terminal cells.
	    اگر (!defined[pix])
	    {
		صحیح r = (pal[(pix - 1) * 3]     * 100 + 127) / 255;
		صحیح g = (pal[(pix - 1) * 3 + 1] * 100 + 127) / 255;
		صحیح b = (pal[(pix - 1) * 3 + 2] * 100 + 127) / 255;

		defined[pix] = true;
		اگر (ga_append(&hdr, '#') == FAIL
			|| ga_concat_int(&hdr, pix) == FAIL
			|| ga_concat_بائٹیں(&hdr, ";2;", 3) == FAIL
			|| ga_concat_int(&hdr, r) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, g) == FAIL
			|| ga_append(&hdr, ';') == FAIL
			|| ga_concat_int(&hdr, b) == FAIL)
		    جاؤ fail;
	    }

	    اگر (آخری_was_cr && ga_append(&body, '$') == FAIL)
		جاؤ fail;
	    اگر (ga_append(&body, '#') == FAIL
		    || ga_concat_int(&body, pix) == FAIL)
		جاؤ fail;

	    صف = band_state->بٹ + (size_t)pix * چوڑائی;
	    اگر (آغاز > 0 && emit_تسلسل(&body, 0, آغاز) == FAIL)
		جاؤ fail;
	    ch0 = صف[آغاز];
	    cnt = 1;
	    برائے (x = آغاز + 1; x <= انتہا; x++)
	    {
		char_u  ch = صف[x];

		اگر (ch == ch0)
		{
		    cnt++;
		    جاری;
		}
		اگر (emit_تسلسل(&body, ch0, cnt) == FAIL)
		    جاؤ fail;
		ch0 = ch;
		cnt = 1;
	    }
	    اگر (emit_تسلسل(&body, ch0, cnt) == FAIL)
		جاؤ fail;
	    آخری_was_cr = 1;
	}
    }

    اگر (ga_append(&hdr, NUL) == FAIL
	    || ga_concat_بائٹیں(&body, "\033\\", 3) == FAIL)
	جاؤ fail;

    seq[0] = (char_u *)hdr.ga_ڈیٹا;
    seq[1] = (char_u *)body.ga_ڈیٹا;

    واپس OK;

fail:
    ga_clear(&hdr);
    ga_clear(&body);
    واپس FAIL;
}

    صحیح
image_sixel_init(image_T *img)
{
    image_sixel_T *ctx;

    ctx = ALLOC_CLEAR_ONE(image_sixel_T);
    اگر (ctx == تہی || sixel_image_quantize(img, &ctx->image8) == FAIL)
    {
	vim_free(ctx);
	واپس FAIL;
    }

#اگر defined(FEAT_IMAGE_SIXEL) && defined(FEAT_TERMRESPONSE)
    اگر (!بھیجا_decsdm)
    {
	// Disable sixel scrolling using DECSDM
	برآمد_str((char_u *)"\033[?80l");
	بھیجا_decsdm = true;
    }
#اگر_ختم

    img->backend_ڈیٹا = ctx;
    واپس OK;
}

    خالی
image_sixel_uninit(image_T *img)
{
    image_sixel_T *ctx = img->backend_ڈیٹا;

    sixel_image8_clear(&ctx->image8);
    vim_free(img->backend_ڈیٹا);
}

    صحیح
image_placement_sixel_init(image_placement_T *رکھو)
{
    image_placement_sixel_T *ctx = ALLOC_CLEAR_ONE(image_placement_sixel_T);

    اگر (ctx == تہی)
	واپس FAIL;
    رکھو->backend_ڈیٹا = ctx;
    واپس OK;
}

    ساکن خالی
clear_chunks(image_placement_sixel_T *ctx)
{
    اگر (ctx->chunks == تہی)
	واپس;

    برائے (صحیح i = 0; i < ctx->n_chunks; i++)
    {
	vim_free(ctx->chunks[i].seq[0]);
	vim_free(ctx->chunks[i].seq[1]);
    }
    VIM_CLEAR(ctx->chunks);
    ctx->n_chunks = 0;
}

    خالی
image_placement_sixel_uninit(image_placement_T *رکھو)
{
    image_placement_sixel_T *ctx = رکھو->backend_ڈیٹا;

    اگر (ctx->visible_init)
	pixman_region32_fini(&ctx->visible_region);
    clear_chunks(ctx);
    vim_free(ctx);
}

    خالی
image_placement_sixel_draw(image_placement_T *رکھو)
{
    image_T		    *img = رکھو->img;
    image_sixel_T	    *ictx = img->backend_ڈیٹا;
    image_placement_sixel_T *ctx = رکھو->backend_ڈیٹا;
    sixel_view_T	    view;
    pixman_box32_t	    *rects;
    صحیح			    n_rects;

    // Check if visible region is still the same, if so then use the cached
    // sixel sequences.
    اگر (!(رکھو->flags & IMAGEPF_FORCE) && ctx->visible_init
	    && pixman_region32_equal(&ctx->visible_region, &رکھو->visible))
    {
	برائے (صحیح i = 0; i < pixman_region32_n_rects(&ctx->visible_region); i++)
	{
	    sixel_chunk_T *chunk = ctx->chunks + i;

	    windgoto(رکھو->صف + رکھو->صف_off
		    + chunk->صف_off, رکھو->ستون + chunk->ستون_off);
	    اگر (chunk->seq[0] != تہی)
		برآمد_str(chunk->seq[0]);
	    اگر (chunk->seq[1] != تہی)
		برآمد_str(chunk->seq[1]);
	    screen_آغاز();
	}
	واپس;
    }

    rects = pixman_region32_rectangles(&رکھو->visible, &n_rects);
    اگر (rects == تہی || n_rects == 0)
	واپس;

    clear_chunks(ctx);
    ctx->chunks = ALLOC_CLEAR_MULT(sixel_chunk_T, n_rects);

    اگر (ctx->chunks == تہی)
	واپس;

    برائے (صحیح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحیح		صف, ستون;
	صحیح		x, y, w, h;
	sixel_chunk_T	*chunk = ctx->chunks + i;
	char_u		*seq[2];

	image_placement_subrect(رکھو, rect, &صف, &ستون, &x, &y, &w, &h, false);

	اگر (sixel_image8_crop(&ictx->image8, x, y, w, h, &view) == FAIL
		|| sixel_encode_view(&view, seq) == FAIL)
	    جاری;

	windgoto(صف, ستون);
	اگر (seq[0] != تہی)
	    برآمد_str(seq[0]);
	اگر (seq[1] != تہی)
	    برآمد_str(seq[1]);
	screen_آغاز();

	chunk->صف_off = rect.y1;
	chunk->ستون_off = rect.x1;
	chunk->seq[0] = seq[0];
	chunk->seq[1] = seq[1];
	ctx->n_chunks++;
    }

    اگر (ctx->visible_init)
	pixman_region32_clear(&ctx->visible_region);
    pixman_region32_copy(&ctx->visible_region, &رکھو->visible);
    ctx->visible_init = true;
}

    خالی
image_placement_sixel_clear(image_placement_T *رکھو غیر_مستعمل)
{
}

#اگر_ختم // FEAT_IMAGE_SIXEL
