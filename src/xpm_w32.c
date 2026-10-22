/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Load XPM image.
 *
 * This function is placed in separate file because Xpm headers conflict with
 * Vim ones :(
 *
 * Written by Sergey Khorev.
 * (original link, now dead: iamphet.nm.ru/vim/index.html)
 */

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#અન્તર્ભાવ <windows.h>

#અન્તર્ભાવ "xpm_w32.h"

// Engage Windows support in libXpm
#define FOR_MSW

#અન્તર્ભાવ "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    પૂર્ણાંક
LoadXpmImage(
    અક્ષર    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    XpmAttributes   attr;
    પૂર્ણાંક		    res;
    HDC		    hdc = CreateCompatibleDC(ખાલી);

    attr.valuemask = 0;
    res = XpmReadFileToImage(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    જો (res < 0)
	પાછા -1;
    જો (shp == ખાલી)
    {
	જો (img)
	    XDestroyImage(img);
	પાછા -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    પાછા 0;
}
