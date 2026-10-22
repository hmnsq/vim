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
#अन्तर्भाव <windows.h>

#अन्तर्भाव "xpm_w32.h"

// Engage Windows support in libXpm
#define FOR_MSW

#अन्तर्भाव "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    पूर्णाङ्क
LoadXpmImage(
    वर्ण    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    XpmAttributes   attr;
    पूर्णाङ्क		    res;
    HDC		    hdc = CreateCompatibleDC(रिक्त);

    attr.valuemask = 0;
    res = XpmReadFileToImage(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    यदि (res < 0)
	प्रत्यागम -1;
    यदि (shp == रिक्त)
    {
	यदि (img)
	    XDestroyImage(img);
	प्रत्यागम -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    प्रत्यागम 0;
}
