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
#include <windows.h>

#include "xpm_w32.h"

// Engage Windows support in libXpm
#define FOR_MSW

#include "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    정수
LoadXpmImage(
    char    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    XpmAttributes   attr;
    정수		    res;
    HDC		    hdc = CreateCompatibleDC(NULL);

    attr.valuemask = 0;
    res = XpmReadFileToImage(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    만약 (res < 0)
	반환 -1;
    만약 (shp == NULL)
    {
	만약 (img)
	    XDestroyImage(img);
	반환 -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    반환 0;
}
