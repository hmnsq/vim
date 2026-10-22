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
    ཧྲིལ
LoadXpmImage(
    var_na    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    XpmAttributes   attr;
    ཧྲིལ		    res;
    HDC		    hdc = CreateCompatibleDC(NULL);

    attr.valuemask = 0;
    res = XpmReadFileToImage(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    གལ་ཏེ (res < 0)
	ཕྱིར -1;
    གལ་ཏེ (shp == NULL)
    {
	གལ་ཏེ (img)
	    XDestroyImage(img);
	ཕྱིར -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    ཕྱིར 0;
}
