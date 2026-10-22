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
#شامل <windows.h>

#شامل "xpm_w32.h"

// Engage Windows support in libXpm
#define FOR_MSW

#شامل "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    صحیح
LoadXpmImage(
    نویسه    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    XpmAttributes   attr;
    صحیح		    res;
    HDC		    hdc = CreateCompatibleDC(پوچ);

    attr.valuemask = 0;
    res = XpmReadFileToImage(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    اگر (res < 0)
	بازگشت -1;
    اگر (shp == پوچ)
    {
	اگر (img)
	    XDestroyImage(img);
	بازگشت -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    بازگشت 0;
}
