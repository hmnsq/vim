/*
 * Copyright (C) 1989-95 GROUPE BULL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * GROUPE BULL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of GROUPE BULL shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from GROUPE BULL.
 */

/*****************************************************************************\
* simx.h: 0.1a                                                                *
*                                                                             *
* This emulates some Xlib functionality for MSW. It's not a general solution, *
* it is close related to XPM-lib. It is only intended to satisfy what is need *
* there. Thus allowing to read XPM files under MS windows.                    *
*                                                                             *
* Developed by HeDu 3/94 (hedu@cul-ipn.uni-kiel.de)                           *
\*****************************************************************************/


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _SIMX_H
#ವ್ಯಾಖ್ಯಾನಿಸು _SIMX_H

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FOR_MSW

#ಅನ್ತರ್ಭಾವ "windows.h"			/* MS windows GDI types */

/*
 * minimal portability layer between ansi and KR C
 */
/* this comes from xpm.h, and is here again, to avoid complicated
    includes, since this is included from xpm.h */
/* these defines get undefed at the end of this file */
#ಆದರೆ __STDC__ || defined(__cplusplus) || defined(c_plusplus)
 /* ANSI || C++ */
#ವ್ಯಾಖ್ಯಾನಿಸು FUNC(f, t, p) ಬಾಹ್ಯ t f p
#ವ್ಯಾಖ್ಯಾನಿಸು LFUNC(f, t, p) ಸ್ಥಿರ t f p
#ಇಲ್ಲದಿದ್ದರೆ /* k&R */
#ವ್ಯಾಖ್ಯಾನಿಸು FUNC(f, t, p) ಬಾಹ್ಯ t f()
#ವ್ಯಾಖ್ಯಾನಿಸು LFUNC(f, t, p) ಸ್ಥಿರ t f()
#ಆದರೆ_ಅಂತ್ಯ


FUNC(boundCheckingMalloc, ಶೂನ್ಯ *, (ದೀರ್ಘ s));
FUNC(boundCheckingCalloc, ಶೂನ್ಯ *, (ದೀರ್ಘ ಅಂಕೆ, ದೀರ್ಘ s));
FUNC(boundCheckingRealloc, ಶೂನ್ಯ *, (ಶೂನ್ಯ *p, ದೀರ್ಘ s));

/* define MSW types for X window types,
   I don't know much about MSW, but the following defines do the job */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ HDC Display;			/* this should be similar */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ *Screen;			/* not used */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ *Visual;			/* not used yet, is for GRAY, COLOR,
					 * MONO */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ *Colormap;			/* should be COLORPALETTE, not done
					 * yet */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ COLORREF Pixel;

#ವ್ಯಾಖ್ಯಾನಿಸು PIXEL_ALREADY_TYPEDEFED		/* to let xpm.h know about it */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    Pixel pixel;
    BYTE red, green, blue;
}      XColor;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    HBITMAP bitmap;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಅಗಲ;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಎತ್ತರ;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಆಳ;
}      XImage;

#ಆದರೆ defined(__cplusplus) || defined(c_plusplus)
ಬಾಹ್ಯ "C" {
#ಆದರೆ_ಅಂತ್ಯ
/* some replacements for X... functions */

/* XDefaultXXX */
    FUNC(XDefaultVisual, Visual *, (Display *display, Screen *screen));
    FUNC(XDefaultScreen, Screen *, (Display *d));
    FUNC(XDefaultColormap, Colormap *, (Display *display, Screen *screen));
    FUNC(XDefault_ಆಳ, ಪೂರ್ಣಾಂಕ, (Display *d, Screen *s));

/* color related */
    FUNC(XParseColor, ಪೂರ್ಣಾಂಕ, (Display *, Colormap *, ಅಕ್ಷರ *, XColor *));
    FUNC(XAllocColor, ಪೂರ್ಣಾಂಕ, (Display *, Colormap *, XColor *));
    FUNC(XQueryColors, ಶೂನ್ಯ, (Display *display, Colormap *colormap,
			      XColor *xcolors, ಪೂರ್ಣಾಂಕ ncolors));
    FUNC(XFreeColors, ಪೂರ್ಣಾಂಕ, (Display *d, Colormap cmap,
			    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pixels[],
			    ಪೂರ್ಣಾಂಕ npixels, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ planes));
/* XImage */
    FUNC(XCreateImage, XImage *, (Display *, Visual *, ಪೂರ್ಣಾಂಕ ಆಳ, ಪೂರ್ಣಾಂಕ format,
				  ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ,
				  ಪೂರ್ಣಾಂಕ pad, ಪೂರ್ಣಾಂಕ foo));

/* free and destroy bitmap */
    FUNC(XDestroyImage, ಶೂನ್ಯ /* ? */ , (XImage *));
/* free only, bitmap remains */
    FUNC(XImageFree, ಶೂನ್ಯ, (XImage *));
#ಆದರೆ defined(__cplusplus) || defined(c_plusplus)
} /* end of extern "C" */
#ಆದರೆ_ಅಂತ್ಯ /* cplusplus */

#ವ್ಯಾಖ್ಯಾನಿಸು ZPixmap 1			/* not really used */
#ವ್ಯಾಖ್ಯಾನಿಸು XYBitmap 1			/* not really used */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ True
#ವ್ಯಾಖ್ಯಾನಿಸು True 1
#ವ್ಯಾಖ್ಯಾನಿಸು False 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ Bool
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BOOL Bool;		/* take MSW bool */
#ಆದರೆ_ಅಂತ್ಯ
/* make these local here, simx.c gets the same from xpm.h */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು LFUNC
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು FUNC

#ಆದರೆ_ಅಂತ್ಯ /* def FOR_MSW */

#ಆದರೆ_ಅಂತ್ಯ /* _SIMX_H */
