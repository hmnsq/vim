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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32_LEAN_AND_ਔਸਤ
# define WIN32_LEAN_AND_ਔਸਤ
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <windows.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "xpm_w32.h"

// Engage Windows support in libXpm
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_MSW

#ਅਨ੍ਤਰ੍ਭਾਵ "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    ਪੂਰਨ
ਲੱਦੋ_Xpm_Image(
    ਅੱਖਰ    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    Xpm_ਗੁਣ   attr;
    ਪੂਰਨ		    res;
    HDC		    hdc = CreateCompatibleDC(ਨਿਰਾ);

    attr.valuemask = 0;
    res = Xpm_Read_File_ਮੰਜ਼ਿਲ_Image(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    ਜੇ (res < 0)
	ਵਾਪਸ -1;
    ਜੇ (shp == ਨਿਰਾ)
    {
	ਜੇ (img)
	    XDestroyImage(img);
	ਵਾਪਸ -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    ਵਾਪਸ 0;
}
