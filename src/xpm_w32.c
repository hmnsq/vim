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

#నిర్వచితం_కాకపోతే WIN32_LEAN_AND_సగటు
# define WIN32_LEAN_AND_సగటు
#అయితే_ముగింపు
#అన్తర్భావ <windows.h>

#అన్తర్భావ "xpm_w32.h"

// Engage Windows support in libXpm
#నిర్వచించు FOR_MSW

#అన్తర్భావ "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    పూర్ణాంకం
ఎక్కించు_Xpm_Image(
    అక్షరం    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    Xpm_లక్షణాలు   attr;
    పూర్ణాంకం		    res;
    HDC		    hdc = CreateCompatibleDC(ఖాళీ);

    attr.valuemask = 0;
    res = Xpm_Read_File_గమ్యం_Image(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    ఒకవేళ (res < 0)
	తిరిగి -1;
    ఒకవేళ (shp == ఖాళీ)
    {
	ఒకవేళ (img)
	    XDestroyImage(img);
	తిరిగి -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    తిరిగి 0;
}
