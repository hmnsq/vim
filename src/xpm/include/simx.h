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

#અન્તર્ભાવ "windows.h"			/* MS windows GDI types */

/*
 * minimal portability layer between ansi and KR C
 */
/* this comes from xpm.h, and is here again, to avoid complicated
    includes, since this is included from xpm.h */
/* these defines get undefed at the end of this file */
#જો __STDC__ || defined(__cplusplus) || defined(c_plusplus)
 /* ANSI || C++ */
#define FUNC(f, t, p) બાહ્ય t f p
#define LFUNC(f, t, p) સ્થિર t f p
#નહીંતર /* k&R */
#define FUNC(f, t, p) બાહ્ય t f()
#define LFUNC(f, t, p) સ્થિર t f()
#endif


FUNC(boundCheckingMalloc, શૂન્ય *, (long s));
FUNC(boundCheckingCalloc, શૂન્ય *, (long અંક, long s));
FUNC(boundCheckingRealloc, શૂન્ય *, (શૂન્ય *p, long s));

/* define MSW types for X window types,
   I don't know much about MSW, but the following defines do the job */

પ્રકારવ્યાખ્યા HDC Display;			/* this should be similar */
પ્રકારવ્યાખ્યા શૂન્ય *Screen;			/* not used */
પ્રકારવ્યાખ્યા શૂન્ય *Visual;			/* not used yet, is for GRAY, COLOR,
					 * MONO */

પ્રકારવ્યાખ્યા શૂન્ય *Colormap;			/* should be COLORPALETTE, not done
					 * yet */

પ્રકારવ્યાખ્યા COLORREF Pixel;

#define PIXEL_ALREADY_TYPEDEFED		/* to let xpm.h know about it */

પ્રકારવ્યાખ્યા સંરચના {
    Pixel pixel;
    BYTE red, green, blue;
}      XColor;

પ્રકારવ્યાખ્યા સંરચના {
    HBITMAP bitmap;
    unsigned પૂર્ણાંક width;
    unsigned પૂર્ણાંક height;
    unsigned પૂર્ણાંક depth;
}      XImage;

#જો defined(__cplusplus) || defined(c_plusplus)
બાહ્ય "C" {
#endif
/* some replacements for X... functions */

/* XDefaultXXX */
    FUNC(XDefaultVisual, Visual *, (Display *display, Screen *screen));
    FUNC(XDefaultScreen, Screen *, (Display *d));
    FUNC(XDefaultColormap, Colormap *, (Display *display, Screen *screen));
    FUNC(XDefaultDepth, પૂર્ણાંક, (Display *d, Screen *s));

/* color related */
    FUNC(XParseColor, પૂર્ણાંક, (Display *, Colormap *, અક્ષર *, XColor *));
    FUNC(XAllocColor, પૂર્ણાંક, (Display *, Colormap *, XColor *));
    FUNC(XQueryColors, શૂન્ય, (Display *display, Colormap *colormap,
			      XColor *xcolors, પૂર્ણાંક ncolors));
    FUNC(XFreeColors, પૂર્ણાંક, (Display *d, Colormap cmap,
			    unsigned long pixels[],
			    પૂર્ણાંક npixels, unsigned long planes));
/* XImage */
    FUNC(XCreateImage, XImage *, (Display *, Visual *, પૂર્ણાંક depth, પૂર્ણાંક format,
				  પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક width, પૂર્ણાંક height,
				  પૂર્ણાંક pad, પૂર્ણાંક foo));

/* free and destroy bitmap */
    FUNC(XDestroyImage, શૂન્ય /* ? */ , (XImage *));
/* free only, bitmap remains */
    FUNC(XImageFree, શૂન્ય, (XImage *));
#જો defined(__cplusplus) || defined(c_plusplus)
} /* end of extern "C" */
#endif /* cplusplus */

#define ZPixmap 1			/* not really used */
#define XYBitmap 1			/* not really used */

#ifndef True
#define True 1
#define False 0
#endif
#ifndef Bool
પ્રકારવ્યાખ્યા BOOL Bool;		/* take MSW bool */
#endif
/* make these local here, simx.c gets the same from xpm.h */
#undef LFUNC
#undef FUNC

#endif /* def FOR_MSW */

#endif /* _SIMX_H */
