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


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _SIMX_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _SIMX_H

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FOR_MSW

#ਅਨ੍ਤਰ੍ਭਾਵ "windows.h"			/* MS windows GDI types */

/*
 * minimal portability layer between ansi and KR C
 */
/* this comes from xpm.h, and is here again, to avoid complicated
    includes, since this is included from xpm.h */
/* these defines get undefed at the end of this file */
#ਜੇ __STDC__ || defined(__cplusplus) || defined(c_plusplus)
 /* ANSI || C++ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FUNC(f, t, p) ਬਾਹਰੀ t f p
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LFUNC(f, t, p) ਸਥਿਰ t f p
#ਨਹੀਂਤਾਂ /* k&R */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FUNC(f, t, p) ਬਾਹਰੀ t f()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LFUNC(f, t, p) ਸਥਿਰ t f()
#ਜੇ_ਅੰਤ


FUNC(boundCheckingMalloc, ਖਾਲੀ *, (ਲੰਮਾ s));
FUNC(boundCheckingCalloc, ਖਾਲੀ *, (ਲੰਮਾ ਅੰਕ, ਲੰਮਾ s));
FUNC(boundCheckingRealloc, ਖਾਲੀ *, (ਖਾਲੀ *p, ਲੰਮਾ s));

/* define MSW types for X window types,
   I don't know much about MSW, but the following defines do the job */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ HDC Display;			/* this should be similar */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ *Screen;			/* not used */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ *Visual;			/* not used yet, is for GRAY, COLOR,
					 * MONO */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ *Colormap;			/* should be COLORPALETTE, not done
					 * yet */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ COLORREF Pixel;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PIXEL_ALREADY_TYPEDEFED		/* to let xpm.h know about it */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    Pixel pixel;
    BYTE red, green, blue;
}      XColor;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    HBITMAP bitmap;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਚੌੜਾਈ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਉਚਾਈ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਡੂੰਘਾਈ;
}      XImage;

#ਜੇ defined(__cplusplus) || defined(c_plusplus)
ਬਾਹਰੀ "C" {
#ਜੇ_ਅੰਤ
/* some replacements for X... functions */

/* XDefaultXXX */
    FUNC(XDefaultVisual, Visual *, (Display *display, Screen *screen));
    FUNC(XDefaultScreen, Screen *, (Display *d));
    FUNC(XDefaultColormap, Colormap *, (Display *display, Screen *screen));
    FUNC(XDefault_ਡੂੰਘਾਈ, ਪੂਰਨ, (Display *d, Screen *s));

/* color related */
    FUNC(XParseColor, ਪੂਰਨ, (Display *, Colormap *, ਅੱਖਰ *, XColor *));
    FUNC(XAllocColor, ਪੂਰਨ, (Display *, Colormap *, XColor *));
    FUNC(XQueryColors, ਖਾਲੀ, (Display *display, Colormap *colormap,
			      XColor *xcolors, ਪੂਰਨ ncolors));
    FUNC(XFreeColors, ਪੂਰਨ, (Display *d, Colormap cmap,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pixels[],
			    ਪੂਰਨ npixels, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ planes));
/* XImage */
    FUNC(XCreateImage, XImage *, (Display *, Visual *, ਪੂਰਨ ਡੂੰਘਾਈ, ਪੂਰਨ format,
				  ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ,
				  ਪੂਰਨ pad, ਪੂਰਨ foo));

/* free and destroy bitmap */
    FUNC(XDestroyImage, ਖਾਲੀ /* ? */ , (XImage *));
/* free only, bitmap remains */
    FUNC(XImageFree, ਖਾਲੀ, (XImage *));
#ਜੇ defined(__cplusplus) || defined(c_plusplus)
} /* end of extern "C" */
#ਜੇ_ਅੰਤ /* cplusplus */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ZPixmap 1			/* not really used */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XYBitmap 1			/* not really used */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ True
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ True 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ False 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ Bool
ਕਿਸਮਪਰਿਭਾਸ਼ਾ BOOL Bool;		/* take MSW bool */
#ਜੇ_ਅੰਤ
/* make these local here, simx.c gets the same from xpm.h */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ LFUNC
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ FUNC

#ਜੇ_ਅੰਤ /* def FOR_MSW */

#ਜੇ_ਅੰਤ /* _SIMX_H */
