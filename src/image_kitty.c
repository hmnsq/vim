/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#إذا defined(FEAT_IMAGE_KITTY)

تعريفنوع بنية
{
    bool transmitted; // If image data has been transmitted to terminal
} image_kitty_T;

تعريفنوع بنية
{
    صحيح n_ids; // Number of placements ids used for previous redraw
} image_placement_kitty_T;

    صحيح
image_kitty_init(image_T *img)
{
    image_kitty_T *ctx = ALLOC_CLEAR_ONE(image_kitty_T);

    إذا (ctx == عدم)
	أرجع FAIL;

    img->backend_معلومات = ctx;
    أرجع OK;
}

    فراغ
image_kitty_uninit(image_T *img)
{
    إذا (((image_kitty_T *)img->backend_معلومات)->transmitted)
    {
	// Delete image data from terminal
	vim_snprintf((حرف *)IObuff, IOSIZE,
		"\033_Ga=d,d=I,i=%d,q=2\033\\", img->id);

	ٻاهريون_str((char_u *)IObuff);
	ٻاهريون_flush();
    }
    vim_free(img->backend_معلومات);
}

    صحيح
image_placement_kitty_init(image_placement_T *رکو)
{
    image_placement_kitty_T *ctx = ALLOC_CLEAR_ONE(image_placement_kitty_T);

    إذا (ctx == عدم)
	أرجع FAIL;

    رکو->backend_معلومات = ctx;
    أرجع OK;
}

    فراغ
image_placement_kitty_uninit(image_placement_T *رکو)
{
    vim_free(رکو->backend_معلومات);
}

/*
 * Transmit image data to terminal if it hasn't already been done.
 */
    ساكن فراغ
transmit_image(image_T *img)
{
    ساكن حرف buf[4096 + 3]; // Extra bytes for NUL and terminator

    int_u   off = 0;
    int_u   sz;
    bool    پهريون = true;
    صحيح	    ويڪر, اوچائي;

    إذا (likely(((image_kitty_T *)img->backend_معلومات)->transmitted))
	أرجع;

    image_get_dimensions(img, &ويڪر, &اوچائي);
    sz = img->fmt * ويڪر * اوچائي;

    طالما (off < sz)
    {
	int_u chunk_sz = MIN(4096 * 3 / 4, sz - off);
	bool وڌيڪ = off + chunk_sz < sz;
	ڊگهو encoded;

	إذا (unlikely(پهريون))
	{
	    vim_snprintf(buf, حجم(buf),
		    "\033_Ga=t,i=%d,f=%d,s=%u,v=%u,m=%d,q=2;", img->id,
		    img->fmt * 8, ويڪر, اوچائي, وڌيڪ);
	    پهريون = false;
	}
	وإلا
	    vim_snprintf(buf, حجم(buf), "\033_Gm=%d;", وڌيڪ);
	ٻاهريون_str((char_u *)buf);

	encoded = base64_encode_buf((char_u *)buf, img->معلومات + off, chunk_sz);

	buf[encoded] = '\033';
	buf[encoded + 1] = '\\';
	buf[encoded + 2] = NUL;

	ٻاهريون_str((char_u *)buf);
	off += chunk_sz;
    }

    ((image_kitty_T *)img->backend_معلومات)->transmitted = true;
    ٻاهريون_flush();
}

    ساكن فراغ
clear_placement(صحيح id, صحيح رکو_id)
{
    vim_snprintf((حرف *)IObuff, IOSIZE,
	    "\033_Ga=d,d=i,i=%d,p=%d,q=2\033\\", id, رکو_id);

    ٻاهريون_str((char_u *)IObuff);
}

    فراغ
image_placement_kitty_draw(image_placement_T *رکو)
{
    image_T		    *img = رکو->img;
    image_placement_kitty_T *ctx = رکو->backend_معلومات;
    pixman_box32_t	    *rects;
    صحيح			    n_rects;

    rects = pixman_region32_rectangles(&رکو->visible, &n_rects);
    إذا (n_rects > PLACEMENT_ID_INC)
	أرجع;

    transmit_image(img);

    إذا (rects == عدم)
	// Not sure if this can happen...
	أرجع;

    لكل (صحيح i = 0; i < n_rects; i++)
    {
	pixman_box32_t rect = rects[i];

	صحيح rect_id = رکو->id + i; // The placement id of each rect is
				     // "place->id + i".
	صحيح صف, کانو;
	صحيح x, y, w, h;

	image_placement_subrect(رکو, rect, &صف, &کانو, &x, &y, &w, &h, false);

	// Since we send the image id and the placement id, the existing
	// placement (if any) will be replaced by this one (essentially moving
	// it).
	//
	// Add C=1 (don't move the cursor), since as said in the specification:
	// ``` After placing an image on the screen the cursor must be moved to
	// the right by the number of cols in the image placement rectangle and
	// down by the number of rows in the image placement rectangle. If
	// either of these cause the cursor to leave either the screen or the
	// scroll area, the exact positioning of the cursor is undefined, and up
	// to implementations. The client can ask the terminal emulator to not
	// move the cursor at all by specifying C=1 in the command, which sets
	// the cursor movement policy to no movement for placing the current
	// image. ```
	//
	// On kitty terminal it seems to scroll the terminal, messing up the
	// screen.
	vim_snprintf(
		(حرف *)IObuff, IOSIZE,
		"\033_Ga=p,i=%d,p=%d,x=%u,y=%u,w=%u,h=%u,z=0,q=2,C=1\033\\",
		img->id, rect_id,
		x, y, w, h);

	windgoto(صف, کانو);
	ٻاهريون_str((char_u *)IObuff);
	screen_شروعات(); // Probably not needed because of C=1, but do it
			// anyways...
    }

    // Clear all placements that have not been reused.
    لكل (صحيح i = n_rects; i < ctx->n_ids; i++)
	clear_placement(img->id, رکو->id + i);

    ctx->n_ids = n_rects;
}

    فراغ
image_placement_kitty_clear(image_placement_T *رکو)
{
    image_placement_kitty_T *ctx = رکو->backend_معلومات;

    إذا (ctx->n_ids == 0)
	أرجع;

    لكل (صحيح i = 0; i < ctx->n_ids; i++)
	clear_placement(رکو->img->id, رکو->id + i);
    ٻاهريون_flush();
    ctx->n_ids = 0;
}

#جيڪڏهن_پورو // FEAT_IMAGE_KITTY
