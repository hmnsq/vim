/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_IMAGE_KITTY)

પ્રકારવ્યાખ્યા સંરચના
{
    bool transmitted; // If image data has been transmitted to terminal
} image_kitty_T;

પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક n_ids; // Number of placements ids used for previous redraw
} image_placement_kitty_T;

    પૂર્ણાંક
image_kitty_init(image_T *img)
{
    image_kitty_T *ctx = ALLOC_CLEAR_ONE(image_kitty_T);

    જો (ctx == ખાલી)
	પાછા FAIL;

    img->backend_data = ctx;
    પાછા OK;
}

    શૂન્ય
image_kitty_uninit(image_T *img)
{
    જો (((image_kitty_T *)img->backend_data)->transmitted)
    {
	// Delete image data from terminal
	vim_snprintf((અક્ષર *)IObuff, IOSIZE,
		"\033_Ga=d,d=I,i=%d,q=2\033\\", img->id);

	out_str((char_u *)IObuff);
	out_flush();
    }
    vim_free(img->backend_data);
}

    પૂર્ણાંક
image_placement_kitty_init(image_placement_T *place)
{
    image_placement_kitty_T *ctx = ALLOC_CLEAR_ONE(image_placement_kitty_T);

    જો (ctx == ખાલી)
	પાછા FAIL;

    place->backend_data = ctx;
    પાછા OK;
}

    શૂન્ય
image_placement_kitty_uninit(image_placement_T *place)
{
    vim_free(place->backend_data);
}

/*
 * Transmit image data to terminal if it hasn't already been done.
 */
    સ્થિર શૂન્ય
transmit_image(image_T *img)
{
    સ્થિર અક્ષર buf[4096 + 3]; // Extra bytes for NUL and terminator

    int_u   off = 0;
    int_u   sz;
    bool    first = true;
    પૂર્ણાંક	    width, height;

    જો (likely(((image_kitty_T *)img->backend_data)->transmitted))
	પાછા;

    image_get_dimensions(img, &width, &height);
    sz = img->fmt * width * height;

    જ્યારે (off < sz)
    {
	int_u chunk_sz = MIN(4096 * 3 / 4, sz - off);
	bool more = off + chunk_sz < sz;
	long encoded;

	જો (unlikely(first))
	{
	    vim_snprintf(buf, કદ(buf),
		    "\033_Ga=t,i=%d,f=%d,s=%u,v=%u,m=%d,q=2;", img->id,
		    img->fmt * 8, width, height, more);
	    first = false;
	}
	નહીંતર
	    vim_snprintf(buf, કદ(buf), "\033_Gm=%d;", more);
	out_str((char_u *)buf);

	encoded = base64_encode_buf((char_u *)buf, img->data + off, chunk_sz);

	buf[encoded] = '\033';
	buf[encoded + 1] = '\\';
	buf[encoded + 2] = NUL;

	out_str((char_u *)buf);
	off += chunk_sz;
    }

    ((image_kitty_T *)img->backend_data)->transmitted = true;
    out_flush();
}

    સ્થિર શૂન્ય
clear_placement(પૂર્ણાંક id, પૂર્ણાંક place_id)
{
    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
	    "\033_Ga=d,d=i,i=%d,p=%d,q=2\033\\", id, place_id);

    out_str((char_u *)IObuff);
}

    શૂન્ય
image_placement_kitty_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_placement_kitty_T *ctx = place->backend_data;
    pixman_box32_t	    *rects;
    પૂર્ણાંક			    n_rects;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    જો (n_rects > PLACEMENT_ID_INC)
	પાછા;

    transmit_image(img);

    જો (rects == ખાલી)
	// Not sure if this can happen...
	પાછા;

    માટે (પૂર્ણાંક i = 0; i < n_rects; i++)
    {
	pixman_box32_t rect = rects[i];

	પૂર્ણાંક rect_id = place->id + i; // The placement id of each rect is
				     // "place->id + i".
	પૂર્ણાંક row, col;
	પૂર્ણાંક x, y, w, h;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);

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
		(અક્ષર *)IObuff, IOSIZE,
		"\033_Ga=p,i=%d,p=%d,x=%u,y=%u,w=%u,h=%u,z=0,q=2,C=1\033\\",
		img->id, rect_id,
		x, y, w, h);

	windgoto(row, col);
	out_str((char_u *)IObuff);
	screen_start(); // Probably not needed because of C=1, but do it
			// anyways...
    }

    // Clear all placements that have not been reused.
    માટે (પૂર્ણાંક i = n_rects; i < ctx->n_ids; i++)
	clear_placement(img->id, place->id + i);

    ctx->n_ids = n_rects;
}

    શૂન્ય
image_placement_kitty_clear(image_placement_T *place)
{
    image_placement_kitty_T *ctx = place->backend_data;

    જો (ctx->n_ids == 0)
	પાછા;

    માટે (પૂર્ણાંક i = 0; i < ctx->n_ids; i++)
	clear_placement(place->img->id, place->id + i);
    out_flush();
    ctx->n_ids = 0;
}

#endif // FEAT_IMAGE_KITTY
