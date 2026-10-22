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
#ଅନ୍ତର୍ଭାଵ <windows.h>

#ଅନ୍ତର୍ଭାଵ "xpm_w32.h"

// Engage Windows support in libXpm
#define FOR_MSW

#ଅନ୍ତର୍ଭାଵ "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
LoadXpmImage(
    ଅକ୍ଷର    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    XpmAttributes   attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    res;
    HDC		    hdc = CreateCompatibleDC(ଖାଲି);

    attr.valuemask = 0;
    res = XpmReadFileToImage(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    ଯଦି (res < 0)
	ଫେରନ୍ତୁ -1;
    ଯଦି (shp == ଖାଲି)
    {
	ଯଦି (img)
	    XDestroyImage(img);
	ଫେରନ୍ତୁ -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    ଫେରନ୍ତୁ 0;
}
