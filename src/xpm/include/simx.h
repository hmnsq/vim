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

#अन्तर्भाव "windows.h"			/* MS windows GDI types */

/*
 * minimal portability layer between ansi and KR C
 */
/* this comes from xpm.h, and is here again, to avoid complicated
    includes, since this is included from xpm.h */
/* these defines get undefed at the end of this file */
#यदि __STDC__ || defined(__cplusplus) || defined(c_plusplus)
 /* ANSI || C++ */
#define FUNC(f, t, p) बाह्य t f p
#define LFUNC(f, t, p) स्थिर t f p
#अन्यथा /* k&R */
#define FUNC(f, t, p) बाह्य t f()
#define LFUNC(f, t, p) स्थिर t f()
#endif


FUNC(boundCheckingMalloc, शून्य *, (long s));
FUNC(boundCheckingCalloc, शून्य *, (long अंक, long s));
FUNC(boundCheckingRealloc, शून्य *, (शून्य *p, long s));

/* define MSW types for X window types,
   I don't know much about MSW, but the following defines do the job */

प्रकारनिर्देश HDC Display;			/* this should be similar */
प्रकारनिर्देश शून्य *Screen;			/* not used */
प्रकारनिर्देश शून्य *Visual;			/* not used yet, is for GRAY, COLOR,
					 * MONO */

प्रकारनिर्देश शून्य *Colormap;			/* should be COLORPALETTE, not done
					 * yet */

प्रकारनिर्देश COLORREF Pixel;

#define PIXEL_ALREADY_TYPEDEFED		/* to let xpm.h know about it */

प्रकारनिर्देश संरचना {
    Pixel pixel;
    BYTE red, green, blue;
}      XColor;

प्रकारनिर्देश संरचना {
    HBITMAP bitmap;
    unsigned पूर्णाङ्क width;
    unsigned पूर्णाङ्क height;
    unsigned पूर्णाङ्क depth;
}      XImage;

#यदि defined(__cplusplus) || defined(c_plusplus)
बाह्य "C" {
#endif
/* some replacements for X... functions */

/* XDefaultXXX */
    FUNC(XDefaultVisual, Visual *, (Display *display, Screen *screen));
    FUNC(XDefaultScreen, Screen *, (Display *d));
    FUNC(XDefaultColormap, Colormap *, (Display *display, Screen *screen));
    FUNC(XDefaultDepth, पूर्णाङ्क, (Display *d, Screen *s));

/* color related */
    FUNC(XParseColor, पूर्णाङ्क, (Display *, Colormap *, वर्ण *, XColor *));
    FUNC(XAllocColor, पूर्णाङ्क, (Display *, Colormap *, XColor *));
    FUNC(XQueryColors, शून्य, (Display *display, Colormap *colormap,
			      XColor *xcolors, पूर्णाङ्क ncolors));
    FUNC(XFreeColors, पूर्णाङ्क, (Display *d, Colormap cmap,
			    unsigned long pixels[],
			    पूर्णाङ्क npixels, unsigned long planes));
/* XImage */
    FUNC(XCreateImage, XImage *, (Display *, Visual *, पूर्णाङ्क depth, पूर्णाङ्क format,
				  पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क width, पूर्णाङ्क height,
				  पूर्णाङ्क pad, पूर्णाङ्क foo));

/* free and destroy bitmap */
    FUNC(XDestroyImage, शून्य /* ? */ , (XImage *));
/* free only, bitmap remains */
    FUNC(XImageFree, शून्य, (XImage *));
#यदि defined(__cplusplus) || defined(c_plusplus)
} /* end of extern "C" */
#endif /* cplusplus */

#define ZPixmap 1			/* not really used */
#define XYBitmap 1			/* not really used */

#ifndef True
#define True 1
#define False 0
#endif
#ifndef Bool
प्रकारनिर्देश BOOL Bool;		/* take MSW bool */
#endif
/* make these local here, simx.c gets the same from xpm.h */
#undef LFUNC
#undef FUNC

#endif /* def FOR_MSW */

#endif /* _SIMX_H */
