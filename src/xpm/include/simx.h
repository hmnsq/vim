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


#ifndef _SIMX_H
#define _SIMX_H

#ifdef FOR_MSW

#شامل "windows.h"			/* MS windows GDI types */

/*
 * minimal portability layer between ansi and KR C
 */
/* this comes from xpm.h, and is here again, to avoid complicated
    includes, since this is included from xpm.h */
/* these defines get undefed at the end of this file */
#اگر __STDC__ || defined(__cplusplus) || defined(c_plusplus)
 /* ANSI || C++ */
#define FUNC(f, t, p) بیرونی t f p
#define LFUNC(f, t, p) ایستا t f p
#وگرنه /* k&R */
#define FUNC(f, t, p) بیرونی t f()
#define LFUNC(f, t, p) ایستا t f()
#endif


FUNC(boundCheckingMalloc, تهی *, (long s));
FUNC(boundCheckingCalloc, تهی *, (long رقم, long s));
FUNC(boundCheckingRealloc, تهی *, (تهی *p, long s));

/* define MSW types for X window types,
   I don't know much about MSW, but the following defines do the job */

تعریفنوع HDC Display;			/* this should be similar */
تعریفنوع تهی *Screen;			/* not used */
تعریفنوع تهی *Visual;			/* not used yet, is for GRAY, COLOR,
					 * MONO */

تعریفنوع تهی *Colormap;			/* should be COLORPALETTE, not done
					 * yet */

تعریفنوع COLORREF Pixel;

#define PIXEL_ALREADY_TYPEDEFED		/* to let xpm.h know about it */

تعریفنوع ساختار {
    Pixel pixel;
    BYTE red, green, blue;
}      XColor;

تعریفنوع ساختار {
    HBITMAP bitmap;
    unsigned صحیح width;
    unsigned صحیح height;
    unsigned صحیح depth;
}      XImage;

#اگر defined(__cplusplus) || defined(c_plusplus)
بیرونی "C" {
#endif
/* some replacements for X... functions */

/* XDefaultXXX */
    FUNC(XDefaultVisual, Visual *, (Display *display, Screen *screen));
    FUNC(XDefaultScreen, Screen *, (Display *d));
    FUNC(XDefaultColormap, Colormap *, (Display *display, Screen *screen));
    FUNC(XDefaultDepth, صحیح, (Display *d, Screen *s));

/* color related */
    FUNC(XParseColor, صحیح, (Display *, Colormap *, نویسه *, XColor *));
    FUNC(XAllocColor, صحیح, (Display *, Colormap *, XColor *));
    FUNC(XQueryColors, تهی, (Display *display, Colormap *colormap,
			      XColor *xcolors, صحیح ncolors));
    FUNC(XFreeColors, صحیح, (Display *d, Colormap cmap,
			    unsigned long pixels[],
			    صحیح npixels, unsigned long planes));
/* XImage */
    FUNC(XCreateImage, XImage *, (Display *, Visual *, صحیح depth, صحیح format,
				  صحیح x, صحیح y, صحیح width, صحیح height,
				  صحیح pad, صحیح foo));

/* free and destroy bitmap */
    FUNC(XDestroyImage, تهی /* ? */ , (XImage *));
/* free only, bitmap remains */
    FUNC(XImageFree, تهی, (XImage *));
#اگر defined(__cplusplus) || defined(c_plusplus)
} /* end of extern "C" */
#endif /* cplusplus */

#define ZPixmap 1			/* not really used */
#define XYBitmap 1			/* not really used */

#ifndef True
#define True 1
#define False 0
#endif
#ifndef Bool
تعریفنوع BOOL Bool;		/* take MSW bool */
#endif
/* make these local here, simx.c gets the same from xpm.h */
#undef LFUNC
#undef FUNC

#endif /* def FOR_MSW */

#endif /* _SIMX_H */
