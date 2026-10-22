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

#جيڪڏهن_وصف_نه_ٿيل WIN32_LEAN_AND_سراسري
# define WIN32_LEAN_AND_سراسري
#جيڪڏهن_پورو
#ضمن <windows.h>

#ضمن "xpm_w32.h"

// Engage Windows support in libXpm
#وصف_ڪريو FOR_MSW

#ضمن "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    صحيح
ڀريو_Xpm_Image(
    حرف    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    Xpm_خاصيتون   attr;
    صحيح		    res;
    HDC		    hdc = CreateCompatibleDC(عدم);

    attr.valuemask = 0;
    res = Xpm_Read_File_منزل_Image(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    إذا (res < 0)
	أرجع -1;
    إذا (shp == عدم)
    {
	إذا (img)
	    XDestroyImage(img);
	أرجع -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    أرجع 0;
}
