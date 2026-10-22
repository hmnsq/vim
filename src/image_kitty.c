/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_IMAGE_KITTY)

రకనిర్వచనం నిర్మాణం
{
    bool transmitted; // If image data has been transmitted to terminal
} image_kitty_T;

రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం n_ids; // Number of placements ids used for previous redraw
} image_placement_kitty_T;

    పూర్ణాంకం
image_kitty_init(image_T *img)
{
    image_kitty_T *ctx = ALLOC_CLEAR_ONE(image_kitty_T);

    ఒకవేళ (ctx == ఖాళీ)
	తిరిగి FAIL;

    img->backend_దత్తాంశం = ctx;
    తిరిగి OK;
}

    శూన్య
image_kitty_uninit(image_T *img)
{
    ఒకవేళ (((image_kitty_T *)img->backend_దత్తాంశం)->transmitted)
    {
	// Delete image data from terminal
	vim_snprintf((అక్షరం *)IObuff, IOSIZE,
		"\033_Ga=d,d=I,i=%d,q=2\033\\", img->id);

	బహిర్గతం_str((char_u *)IObuff);
	బహిర్గతం_flush();
    }
    vim_free(img->backend_దత్తాంశం);
}

    పూర్ణాంకం
image_placement_kitty_init(image_placement_T *ఉంచు)
{
    image_placement_kitty_T *ctx = ALLOC_CLEAR_ONE(image_placement_kitty_T);

    ఒకవేళ (ctx == ఖాళీ)
	తిరిగి FAIL;

    ఉంచు->backend_దత్తాంశం = ctx;
    తిరిగి OK;
}

    శూన్య
image_placement_kitty_uninit(image_placement_T *ఉంచు)
{
    vim_free(ఉంచు->backend_దత్తాంశం);
}

/*
 * Transmit image data to terminal if it hasn't already been done.
 */
    స్థిర శూన్య
transmit_image(image_T *img)
{
    స్థిర అక్షరం buf[4096 + 3]; // Extra bytes for NUL and terminator

    int_u   off = 0;
    int_u   sz;
    bool    మొదటి = true;
    పూర్ణాంకం	    వెడల్పు, ఎత్తు;

    ఒకవేళ (likely(((image_kitty_T *)img->backend_దత్తాంశం)->transmitted))
	తిరిగి;

    image_get_dimensions(img, &వెడల్పు, &ఎత్తు);
    sz = img->fmt * వెడల్పు * ఎత్తు;

    అయితే (off < sz)
    {
	int_u chunk_sz = MIN(4096 * 3 / 4, sz - off);
	bool మరిన్ని = off + chunk_sz < sz;
	దీర్ఘ encoded;

	ఒకవేళ (unlikely(మొదటి))
	{
	    vim_snprintf(buf, పరిమాణం(buf),
		    "\033_Ga=t,i=%d,f=%d,s=%u,v=%u,m=%d,q=2;", img->id,
		    img->fmt * 8, వెడల్పు, ఎత్తు, మరిన్ని);
	    మొదటి = false;
	}
	లేకపోతే
	    vim_snprintf(buf, పరిమాణం(buf), "\033_Gm=%d;", మరిన్ని);
	బహిర్గతం_str((char_u *)buf);

	encoded = base64_encode_buf((char_u *)buf, img->దత్తాంశం + off, chunk_sz);

	buf[encoded] = '\033';
	buf[encoded + 1] = '\\';
	buf[encoded + 2] = NUL;

	బహిర్గతం_str((char_u *)buf);
	off += chunk_sz;
    }

    ((image_kitty_T *)img->backend_దత్తాంశం)->transmitted = true;
    బహిర్గతం_flush();
}

    స్థిర శూన్య
clear_placement(పూర్ణాంకం id, పూర్ణాంకం ఉంచు_id)
{
    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
	    "\033_Ga=d,d=i,i=%d,p=%d,q=2\033\\", id, ఉంచు_id);

    బహిర్గతం_str((char_u *)IObuff);
}

    శూన్య
image_placement_kitty_draw(image_placement_T *ఉంచు)
{
    image_T		    *img = ఉంచు->img;
    image_placement_kitty_T *ctx = ఉంచు->backend_దత్తాంశం;
    pixman_box32_t	    *rects;
    పూర్ణాంకం			    n_rects;

    rects = pixman_region32_rectangles(&ఉంచు->visible, &n_rects);
    ఒకవేళ (n_rects > PLACEMENT_ID_INC)
	తిరిగి;

    transmit_image(img);

    ఒకవేళ (rects == ఖాళీ)
	// Not sure if this can happen...
	తిరిగి;

    కొరకు (పూర్ణాంకం i = 0; i < n_rects; i++)
    {
	pixman_box32_t rect = rects[i];

	పూర్ణాంకం rect_id = ఉంచు->id + i; // The placement id of each rect is
				     // "place->id + i".
	పూర్ణాంకం అడ్డువరుస, నిలువు;
	పూర్ణాంకం x, y, w, h;

	image_placement_subrect(ఉంచు, rect, &అడ్డువరుస, &నిలువు, &x, &y, &w, &h, false);

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
		(అక్షరం *)IObuff, IOSIZE,
		"\033_Ga=p,i=%d,p=%d,x=%u,y=%u,w=%u,h=%u,z=0,q=2,C=1\033\\",
		img->id, rect_id,
		x, y, w, h);

	windgoto(అడ్డువరుస, నిలువు);
	బహిర్గతం_str((char_u *)IObuff);
	screen_ప్రారంభం(); // Probably not needed because of C=1, but do it
			// anyways...
    }

    // Clear all placements that have not been reused.
    కొరకు (పూర్ణాంకం i = n_rects; i < ctx->n_ids; i++)
	clear_placement(img->id, ఉంచు->id + i);

    ctx->n_ids = n_rects;
}

    శూన్య
image_placement_kitty_clear(image_placement_T *ఉంచు)
{
    image_placement_kitty_T *ctx = ఉంచు->backend_దత్తాంశం;

    ఒకవేళ (ctx->n_ids == 0)
	తిరిగి;

    కొరకు (పూర్ణాంకం i = 0; i < ctx->n_ids; i++)
	clear_placement(ఉంచు->img->id, ఉంచు->id + i);
    బహిర్గతం_flush();
    ctx->n_ids = 0;
}

#అయితే_ముగింపు // FEAT_IMAGE_KITTY
