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
#അന്തര്ഭാവ <windows.h>

#അന്തര്ഭാവ "xpm_w32.h"

// Engage Windows support in libXpm
#define FOR_MSW

#അന്തര്ഭാവ "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    പൂർണ്ണസംഖ്യ
LoadXpmImage(
    അക്ഷരം    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    XpmAttributes   attr;
    പൂർണ്ണസംഖ്യ		    res;
    HDC		    hdc = CreateCompatibleDC(ശൂന്യ);

    attr.valuemask = 0;
    res = XpmReadFileToImage(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    എങ്കിൽ (res < 0)
	തിരികെ -1;
    എങ്കിൽ (shp == ശൂന്യ)
    {
	എങ്കിൽ (img)
	    XDestroyImage(img);
	തിരികെ -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    തിരികെ 0;
}
