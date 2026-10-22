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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32_LEAN_AND_ಸರಾಸರಿ
# define WIN32_LEAN_AND_ಸರಾಸರಿ
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <windows.h>

#ಅನ್ತರ್ಭಾವ "xpm_w32.h"

// Engage Windows support in libXpm
#ವ್ಯಾಖ್ಯಾನಿಸು FOR_MSW

#ಅನ್ತರ್ಭಾವ "xpm.h"

/*
 * Tries to load an Xpm image from the file "filename".
 * Returns -1 on failure.
 * Returns 0 on success and stores image and mask BITMAPS in "hImage" and
 * "hShape".
 */
    ಪೂರ್ಣಾಂಕ
ತುಂಬು_Xpm_Image(
    ಅಕ್ಷರ    *filename,
    HBITMAP *hImage,
    HBITMAP *hShape)
{
    XImage	    *img;  // loaded image
    XImage	    *shp;  // shapeimage
    Xpm_ಗುಣಲಕ್ಷಣಗಳು   attr;
    ಪೂರ್ಣಾಂಕ		    res;
    HDC		    hdc = CreateCompatibleDC(ಖಾಲಿ);

    attr.valuemask = 0;
    res = Xpm_Read_File_ಗಮ್ಯ_Image(&hdc, filename, &img, &shp, &attr);
    DeleteDC(hdc);
    ಆದರೆ (res < 0)
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (shp == ಖಾಲಿ)
    {
	ಆದರೆ (img)
	    XDestroyImage(img);
	ಹಿಂತಿರುಗಿ -1;
    }
    *hImage = img->bitmap;
    *hShape = shp->bitmap;
    ಹಿಂತಿರುಗಿ 0;
}
