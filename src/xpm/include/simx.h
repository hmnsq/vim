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

#অন্তর্ভাৱ "windows.h"			/* MS windows GDI types */

/*
 * minimal portability layer between ansi and KR C
 */
/* this comes from xpm.h, and is here again, to avoid complicated
    includes, since this is included from xpm.h */
/* these defines get undefed at the end of this file */
#যদি __STDC__ || defined(__cplusplus) || defined(c_plusplus)
 /* ANSI || C++ */
#define FUNC(f, t, p) বাহ্য t f p
#define LFUNC(f, t, p) স্থির t f p
#অন্যথা /* k&R */
#define FUNC(f, t, p) বাহ্য t f()
#define LFUNC(f, t, p) স্থির t f()
#endif


FUNC(boundCheckingMalloc, শূন্য *, (long s));
FUNC(boundCheckingCalloc, শূন্য *, (long অঙ্ক, long s));
FUNC(boundCheckingRealloc, শূন্য *, (শূন্য *p, long s));

/* define MSW types for X window types,
   I don't know much about MSW, but the following defines do the job */

প্রকারনির্দেশ HDC Display;			/* this should be similar */
প্রকারনির্দেশ শূন্য *Screen;			/* not used */
প্রকারনির্দেশ শূন্য *Visual;			/* not used yet, is for GRAY, COLOR,
					 * MONO */

প্রকারনির্দেশ শূন্য *Colormap;			/* should be COLORPALETTE, not done
					 * yet */

প্রকারনির্দেশ COLORREF Pixel;

#define PIXEL_ALREADY_TYPEDEFED		/* to let xpm.h know about it */

প্রকারনির্দেশ সংরচনা {
    Pixel pixel;
    BYTE red, green, blue;
}      XColor;

প্রকারনির্দেশ সংরচনা {
    HBITMAP bitmap;
    unsigned পূর্ণাঙ্ক width;
    unsigned পূর্ণাঙ্ক height;
    unsigned পূর্ণাঙ্ক depth;
}      XImage;

#যদি defined(__cplusplus) || defined(c_plusplus)
বাহ্য "C" {
#endif
/* some replacements for X... functions */

/* XDefaultXXX */
    FUNC(XDefaultVisual, Visual *, (Display *display, Screen *screen));
    FUNC(XDefaultScreen, Screen *, (Display *d));
    FUNC(XDefaultColormap, Colormap *, (Display *display, Screen *screen));
    FUNC(XDefaultDepth, পূর্ণাঙ্ক, (Display *d, Screen *s));

/* color related */
    FUNC(XParseColor, পূর্ণাঙ্ক, (Display *, Colormap *, ৱর্ণ *, XColor *));
    FUNC(XAllocColor, পূর্ণাঙ্ক, (Display *, Colormap *, XColor *));
    FUNC(XQueryColors, শূন্য, (Display *display, Colormap *colormap,
			      XColor *xcolors, পূর্ণাঙ্ক ncolors));
    FUNC(XFreeColors, পূর্ণাঙ্ক, (Display *d, Colormap cmap,
			    unsigned long pixels[],
			    পূর্ণাঙ্ক npixels, unsigned long planes));
/* XImage */
    FUNC(XCreateImage, XImage *, (Display *, Visual *, পূর্ণাঙ্ক depth, পূর্ণাঙ্ক format,
				  পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, পূর্ণাঙ্ক width, পূর্ণাঙ্ক height,
				  পূর্ণাঙ্ক pad, পূর্ণাঙ্ক foo));

/* free and destroy bitmap */
    FUNC(XDestroyImage, শূন্য /* ? */ , (XImage *));
/* free only, bitmap remains */
    FUNC(XImageFree, শূন্য, (XImage *));
#যদি defined(__cplusplus) || defined(c_plusplus)
} /* end of extern "C" */
#endif /* cplusplus */

#define ZPixmap 1			/* not really used */
#define XYBitmap 1			/* not really used */

#ifndef True
#define True 1
#define False 0
#endif
#ifndef Bool
প্রকারনির্দেশ BOOL Bool;		/* take MSW bool */
#endif
/* make these local here, simx.c gets the same from xpm.h */
#undef LFUNC
#undef FUNC

#endif /* def FOR_MSW */

#endif /* _SIMX_H */
