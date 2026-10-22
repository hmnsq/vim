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


#جے_بنا_تعریف _SIMX_H
#تعریف_کرو _SIMX_H

#جے_تعریف_شدہ FOR_MSW

#ضمن "windows.h"			/* MS windows GDI types */

/*
 * minimal portability layer between ansi and KR C
 */
/* this comes from xpm.h, and is here again, to avoid complicated
    includes, since this is included from xpm.h */
/* these defines get undefed at the end of this file */
#إذا __STDC__ || defined(__cplusplus) || defined(c_plusplus)
 /* ANSI || C++ */
#تعریف_کرو FUNC(f, t, p) خارجي t f p
#تعریف_کرو LFUNC(f, t, p) ساكن t f p
#وإلا /* k&R */
#تعریف_کرو FUNC(f, t, p) خارجي t f()
#تعریف_کرو LFUNC(f, t, p) ساكن t f()
#جے_انت


FUNC(boundCheckingMalloc, فراغ *, (لما s));
FUNC(boundCheckingCalloc, فراغ *, (لما عدد, لما s));
FUNC(boundCheckingRealloc, فراغ *, (فراغ *p, لما s));

/* define MSW types for X window types,
   I don't know much about MSW, but the following defines do the job */

تعريفنوع HDC Display;			/* this should be similar */
تعريفنوع فراغ *Screen;			/* not used */
تعريفنوع فراغ *Visual;			/* not used yet, is for GRAY, COLOR,
					 * MONO */

تعريفنوع فراغ *Colormap;			/* should be COLORPALETTE, not done
					 * yet */

تعريفنوع COLORREF Pixel;

#تعریف_کرو PIXEL_ALREADY_TYPEDEFED		/* to let xpm.h know about it */

تعريفنوع بنية {
    Pixel pixel;
    BYTE red, green, blue;
}      XColor;

تعريفنوع بنية {
    HBITMAP bitmap;
    بنانشانی صحيح چوڑائی;
    بنانشانی صحيح اچائی;
    بنانشانی صحيح ڈونگھائی;
}      XImage;

#إذا defined(__cplusplus) || defined(c_plusplus)
خارجي "C" {
#جے_انت
/* some replacements for X... functions */

/* XDefaultXXX */
    FUNC(XDefaultVisual, Visual *, (Display *display, Screen *screen));
    FUNC(XDefaultScreen, Screen *, (Display *d));
    FUNC(XDefaultColormap, Colormap *, (Display *display, Screen *screen));
    FUNC(XDefault_ڈونگھائی, صحيح, (Display *d, Screen *s));

/* color related */
    FUNC(XParseColor, صحيح, (Display *, Colormap *, حرف *, XColor *));
    FUNC(XAllocColor, صحيح, (Display *, Colormap *, XColor *));
    FUNC(XQueryColors, فراغ, (Display *display, Colormap *colormap,
			      XColor *xcolors, صحيح ncolors));
    FUNC(XFreeColors, صحيح, (Display *d, Colormap cmap,
			    بنانشانی لما pixels[],
			    صحيح npixels, بنانشانی لما planes));
/* XImage */
    FUNC(XCreateImage, XImage *, (Display *, Visual *, صحيح ڈونگھائی, صحيح format,
				  صحيح x, صحيح y, صحيح چوڑائی, صحيح اچائی,
				  صحيح pad, صحيح foo));

/* free and destroy bitmap */
    FUNC(XDestroyImage, فراغ /* ? */ , (XImage *));
/* free only, bitmap remains */
    FUNC(XImageFree, فراغ, (XImage *));
#إذا defined(__cplusplus) || defined(c_plusplus)
} /* end of extern "C" */
#جے_انت /* cplusplus */

#تعریف_کرو ZPixmap 1			/* not really used */
#تعریف_کرو XYBitmap 1			/* not really used */

#جے_بنا_تعریف True
#تعریف_کرو True 1
#تعریف_کرو False 0
#جے_انت
#جے_بنا_تعریف Bool
تعريفنوع BOOL Bool;		/* take MSW bool */
#جے_انت
/* make these local here, simx.c gets the same from xpm.h */
#تعریف_رد LFUNC
#تعریف_رد FUNC

#جے_انت /* def FOR_MSW */

#جے_انت /* _SIMX_H */
