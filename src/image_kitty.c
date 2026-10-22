/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

#اگر defined(FEAT_IMAGE_KITTY)

قسمتعریف ساخت
{
    bool transmitted; // If image data has been transmitted to terminal
} image_kitty_T;

قسمتعریف ساخت
{
    صحیح n_ids; // Number of placements ids used for previous redraw
} image_placement_kitty_T;

    صحیح
image_kitty_init(image_T *img)
{
    image_kitty_T *ctx = ALLOC_CLEAR_ONE(image_kitty_T);

    اگر (ctx == تہی)
	واپس FAIL;

    img->backend_ڈیٹا = ctx;
    واپس OK;
}

    خالی
image_kitty_uninit(image_T *img)
{
    اگر (((image_kitty_T *)img->backend_ڈیٹا)->transmitted)
    {
	// Delete image data from terminal
	vim_snprintf((حرف *)IObuff, IOSIZE,
		"\033_Ga=d,d=I,i=%d,q=2\033\\", img->id);

	برآمد_str((char_u *)IObuff);
	برآمد_flush();
    }
    vim_free(img->backend_ڈیٹا);
}

    صحیح
image_placement_kitty_init(image_placement_T *رکھو)
{
    image_placement_kitty_T *ctx = ALLOC_CLEAR_ONE(image_placement_kitty_T);

    اگر (ctx == تہی)
	واپس FAIL;

    رکھو->backend_ڈیٹا = ctx;
    واپس OK;
}

    خالی
image_placement_kitty_uninit(image_placement_T *رکھو)
{
    vim_free(رکھو->backend_ڈیٹا);
}

/*
 * Transmit image data to terminal if it hasn't already been done.
 */
    ساکن خالی
transmit_image(image_T *img)
{
    ساکن حرف buf[4096 + 3]; // Extra bytes for NUL and terminator

    int_u   off = 0;
    int_u   sz;
    bool    پہلا = true;
    صحیح	    چوڑائی, اونچائی;

    اگر (likely(((image_kitty_T *)img->backend_ڈیٹا)->transmitted))
	واپس;

    image_get_dimensions(img, &چوڑائی, &اونچائی);
    sz = img->fmt * چوڑائی * اونچائی;

    جب (off < sz)
    {
	int_u chunk_sz = MIN(4096 * 3 / 4, sz - off);
	bool مزید = off + chunk_sz < sz;
	طویل encoded;

	اگر (unlikely(پہلا))
	{
	    vim_snprintf(buf, حجم(buf),
		    "\033_Ga=t,i=%d,f=%d,s=%u,v=%u,m=%d,q=2;", img->id,
		    img->fmt * 8, چوڑائی, اونچائی, مزید);
	    پہلا = false;
	}
	ورنہ
	    vim_snprintf(buf, حجم(buf), "\033_Gm=%d;", مزید);
	برآمد_str((char_u *)buf);

	encoded = base64_encode_buf((char_u *)buf, img->ڈیٹا + off, chunk_sz);

	buf[encoded] = '\033';
	buf[encoded + 1] = '\\';
	buf[encoded + 2] = NUL;

	برآمد_str((char_u *)buf);
	off += chunk_sz;
    }

    ((image_kitty_T *)img->backend_ڈیٹا)->transmitted = true;
    برآمد_flush();
}

    ساکن خالی
clear_placement(صحیح id, صحیح رکھو_id)
{
    vim_snprintf((حرف *)IObuff, IOSIZE,
	    "\033_Ga=d,d=i,i=%d,p=%d,q=2\033\\", id, رکھو_id);

    برآمد_str((char_u *)IObuff);
}

    خالی
image_placement_kitty_draw(image_placement_T *رکھو)
{
    image_T		    *img = رکھو->img;
    image_placement_kitty_T *ctx = رکھو->backend_ڈیٹا;
    pixman_box32_t	    *rects;
    صحیح			    n_rects;

    rects = pixman_region32_rectangles(&رکھو->visible, &n_rects);
    اگر (n_rects > PLACEMENT_ID_INC)
	واپس;

    transmit_image(img);

    اگر (rects == تہی)
	// Not sure if this can happen...
	واپس;

    برائے (صحیح i = 0; i < n_rects; i++)
    {
	pixman_box32_t rect = rects[i];

	صحیح rect_id = رکھو->id + i; // The placement id of each rect is
				     // "place->id + i".
	صحیح صف, ستون;
	صحیح x, y, w, h;

	image_placement_subrect(رکھو, rect, &صف, &ستون, &x, &y, &w, &h, false);

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

	windgoto(صف, ستون);
	برآمد_str((char_u *)IObuff);
	screen_آغاز(); // Probably not needed because of C=1, but do it
			// anyways...
    }

    // Clear all placements that have not been reused.
    برائے (صحیح i = n_rects; i < ctx->n_ids; i++)
	clear_placement(img->id, رکھو->id + i);

    ctx->n_ids = n_rects;
}

    خالی
image_placement_kitty_clear(image_placement_T *رکھو)
{
    image_placement_kitty_T *ctx = رکھو->backend_ڈیٹا;

    اگر (ctx->n_ids == 0)
	واپس;

    برائے (صحیح i = 0; i < ctx->n_ids; i++)
	clear_placement(رکھو->img->id, رکھو->id + i);
    برآمد_flush();
    ctx->n_ids = 0;
}

#اگر_ختم // FEAT_IMAGE_KITTY
