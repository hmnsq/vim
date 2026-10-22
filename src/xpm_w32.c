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
#ಅನ್ತರ್ಭಾವ <windows.h>

#ಅನ್ತರ್ಭಾವ "xpm_w32.h"

// Engage Windows support in libXpm
#define FOR_MSW

#ಅನ್ತರ್ಭಾವ "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    ಪೂರ್ಣಾಙ್ಕ
LoadXpmImage(
    ವರ್ಣ    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    XpmAttributes   attr;
    ಪೂರ್ಣಾಙ್ಕ		    res;
    HDC		    hdc = CreateCompatibleDC(ರಿಕ್ತ);

    attr.valuemask = 0;
    res = XpmReadFileToImage(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    ಯದಿ (res < 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (shp == ರಿಕ್ತ)
    {
	ಯದಿ (img)
	    XDestroyImage(img);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    ಪ್ರತ್ಯಾಗಮ 0;
}
