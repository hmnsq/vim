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
#অন্তর্ভাৱ <windows.h>

#অন্তর্ভাৱ "xpm_w32.h"

// Engage Windows support in libXpm
#define FOR_MSW

#অন্তর্ভাৱ "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    পূর্ণাঙ্ক
LoadXpmImage(
    ৱর্ণ    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    XpmAttributes   attr;
    পূর্ণাঙ্ক		    res;
    HDC		    hdc = CreateCompatibleDC(রিক্ত);

    attr.valuemask = 0;
    res = XpmReadFileToImage(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    যদি (res < 0)
	ঘূৰাই -1;
    যদি (shp == রিক্ত)
    {
	যদি (img)
	    XDestroyImage(img);
	ঘূৰাই -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    ঘূৰাই 0;
}
