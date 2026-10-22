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

#ଅନ୍ତର୍ଭାଵ "windows.h"			/* MS windows GDI types */

/*
 * minimal portability layer between ansi and KR C
 */
/* this comes from xpm.h, and is here again, to avoid complicated
    includes, since this is included from xpm.h */
/* these defines get undefed at the end of this file */
#ଯଦି __STDC__ || defined(__cplusplus) || defined(c_plusplus)
 /* ANSI || C++ */
#define FUNC(f, t, p) ବାହ୍ୟ t f p
#define LFUNC(f, t, p) ସ୍ଥିର t f p
#ଅନ୍ୟଥା /* k&R */
#define FUNC(f, t, p) ବାହ୍ୟ t f()
#define LFUNC(f, t, p) ସ୍ଥିର t f()
#endif


FUNC(boundCheckingMalloc, ଶୂନ୍ୟ *, (long s));
FUNC(boundCheckingCalloc, ଶୂନ୍ୟ *, (long ଅଙ୍କ, long s));
FUNC(boundCheckingRealloc, ଶୂନ୍ୟ *, (ଶୂନ୍ୟ *p, long s));

/* define MSW types for X window types,
   I don't know much about MSW, but the following defines do the job */

ପ୍ରକାରବ୍ୟାଖ୍ୟା HDC Display;			/* this should be similar */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ *Screen;			/* not used */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ *Visual;			/* not used yet, is for GRAY, COLOR,
					 * MONO */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ *Colormap;			/* should be COLORPALETTE, not done
					 * yet */

ପ୍ରକାରବ୍ୟାଖ୍ୟା COLORREF Pixel;

#define PIXEL_ALREADY_TYPEDEFED		/* to let xpm.h know about it */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    Pixel pixel;
    BYTE red, green, blue;
}      XColor;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    HBITMAP bitmap;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା height;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth;
}      XImage;

#ଯଦି defined(__cplusplus) || defined(c_plusplus)
ବାହ୍ୟ "C" {
#endif
/* some replacements for X... functions */

/* XDefaultXXX */
    FUNC(XDefaultVisual, Visual *, (Display *display, Screen *screen));
    FUNC(XDefaultScreen, Screen *, (Display *d));
    FUNC(XDefaultColormap, Colormap *, (Display *display, Screen *screen));
    FUNC(XDefaultDepth, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, (Display *d, Screen *s));

/* color related */
    FUNC(XParseColor, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, (Display *, Colormap *, ଅକ୍ଷର *, XColor *));
    FUNC(XAllocColor, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, (Display *, Colormap *, XColor *));
    FUNC(XQueryColors, ଶୂନ୍ୟ, (Display *display, Colormap *colormap,
			      XColor *xcolors, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ncolors));
    FUNC(XFreeColors, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, (Display *d, Colormap cmap,
			    unsigned long pixels[],
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା npixels, unsigned long planes));
/* XImage */
    FUNC(XCreateImage, XImage *, (Display *, Visual *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth, ପୂର୍ଣ୍ଣସଂଖ୍ୟା format,
				  ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height,
				  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pad, ପୂର୍ଣ୍ଣସଂଖ୍ୟା foo));

/* free and destroy bitmap */
    FUNC(XDestroyImage, ଶୂନ୍ୟ /* ? */ , (XImage *));
/* free only, bitmap remains */
    FUNC(XImageFree, ଶୂନ୍ୟ, (XImage *));
#ଯଦି defined(__cplusplus) || defined(c_plusplus)
} /* end of extern "C" */
#endif /* cplusplus */

#define ZPixmap 1			/* not really used */
#define XYBitmap 1			/* not really used */

#ifndef True
#define True 1
#define False 0
#endif
#ifndef Bool
ପ୍ରକାରବ୍ୟାଖ୍ୟା BOOL Bool;		/* take MSW bool */
#endif
/* make these local here, simx.c gets the same from xpm.h */
#undef LFUNC
#undef FUNC

#endif /* def FOR_MSW */

#endif /* _SIMX_H */
