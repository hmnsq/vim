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

#اگر_غیر_تعریف_شدہ WIN32_LEAN_AND_اوسط
# define WIN32_LEAN_AND_اوسط
#اگر_ختم
#شامل <windows.h>

#شامل "xpm_w32.h"

// Engage Windows support in libXpm
#تعریف_کرو FOR_MSW

#شامل "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    صحیح
بھرو_Xpm_Image(
    حرف    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    Xpm_اوصاف   attr;
    صحیح		    res;
    HDC		    hdc = CreateCompatibleDC(تہی);

    attr.valuemask = 0;
    res = Xpm_Read_File_مقصد_Image(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    اگر (res < 0)
	واپس -1;
    اگر (shp == تہی)
    {
	اگر (img)
	    XDestroyImage(img);
	واپس -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    واپس 0;
}
